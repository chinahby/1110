/* =========================================================================
                 Camera Interface

   DESCRIPTIONFileSourceDevice Driver
     This file contains the definitions needed for the camera interface
     device driver (LDD).

   Design Note:
     All function calls are executed from the caller's context. The graph
     task context is only used in lieu of interrupt context. All state
     transition (except change to sensor ready which does not have racing
     issue) happens in the caller's context.

     Since state and callbacks are used in graph task context, these two
     variables are protected by disabling the interrupt.

   Copyright (c) 2006-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/cameraservices/main/latest/src/camera_svcs.c#58 $ $DateTime: 2009/04/22 23:29:28 $ $Author: c_caluru $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/22/09   cg     Enabled ASF for Viewfinder for LCU.
 04/14/09   cg     Fixed the Featurization issue with ANY_FLASH
 04/07/09  cg      Re-Enabling Feature FEATURE_CAMERA_BURST_MODE 
 04/05/09  kamit   Moved camcorder_svcs_stop_record() under #ifdef  FEATURE_VIDEO_ENCODE 
 04/04/09   cg     Multiframe HJR should not be implemented using Multisot AEC i.e there is no need to skip the frames,AEC stats are not required,
                   VFE should be put to IDLE state after we get all the frames.
 03/30/09   chai   Vfe should not be configures with there is no sufficient memory in preview buffers for CR#170929 
                   from QDSP Services within the stipulated time
 03/25/09   cg     Preview  not displaying after taking snapshot with Multishot option  For only xga size 
 03/12/09   cg     Fixed the featurization issue with YCBCR_ONLY feature and Turning off the blocks not required for YUV sensor.
 03/12/09   hap    Reject camera_get_info call if camera is not started
 03/05/09   hap    Add error handling if set dimensions is not called before
                   start preview or start QVP encode
 03/03/09   chai   fix for the dark preview and snapshot isuue when FEATURE_CAMERA_AEC_DURING_MULTISHOT is disabled
 03/02/09   sm     Fix for CR165240 to ensure that right co-ordinates 
                   are given to mdp when ui given dimensions are less 
                   than thumbnail dimensions
 03/02/09   sm     Add NULL check before accessing sensor name as a fix 
                   for CR171324 
 02/25/09  chai    Fixed the manual white balance issue with LCU
 02/18/09  chai    Fixed the compilation problem when FEATURE_CAMERA_YCBCR_ONLY is defined
 02/17/09  kamit   Added camcorder_svcs_stop_record() in camera_svcs_stop() when camera state is
                            CAMERA_STATE_RECORDING
 02/12/09  cgera  Added support for MULTISHOT AEC 1.1,5MP SENSOR support
 02/09/09  girip   Fix for CR 169491: Added function call camcorder_process_set_default_parms() to clear 
                   the camcorder parameters which needs to be reset to default while camcorder starts.
 02/05/09  sm      Fix for CR 171203
 02/04/09  chai    Fixed RVCT warnings
 01/13/09  sm      Fix for CR 168645
 01/08/09  sm      Registering to CLKRGM_CPU_CLIENT_QCAMERA_HIGH client for snapshot in 6290
 12/24/08  cg      Changes for Ez Tune 1.0 and AEC Multishot
 12/18/08  sm      Added support for multi raw preview capture
 11/30/08  sm      Added support for preview rotation
 11/26/08  cg      Fix for CR # 164073
 11/25/08  hap     Adding support for shutter sound play and VFE capture 
                   concurrency and changes made to resolve AHB 
                   overflow issues
 11/20/08  sanal   Changes CAMERA_SIG to CAMERA_TIMER_SIG for timed waits
 11/03/08  cg      Updates For Ez Tune 1.0 and 3A 1.4
 10/22/08  hap     Removed vote for sleep before camera_terminate
 10/14/08  sm      Fix for CR 153677 also resolved compiler warning
 10/03/08  cg      Aligning thumbnail buffers to 64 bytes
 09/25/08  cg      Allowing camera client to place a restriction on SWFI
 09/12/08  ps      Featurized timestamp special effect support(FEATURE_CAMCORDER_OVERLAY_TIMESTAMP)
 09/10/08  hap     Fix for QVP concurrency issue (CR#152780)
 09/10/08  shiven  Camcorder related WB Cacche Policy Support
 09/05/08  shiven  Camcorder error recovery after camera_config_vfe() failure 
 09/04/08  hap     Limiting output1 dimensions to QVGA in preview mode
 08/20/08 cg       Lint Error Fix
 08/13/08  hap     Made setting dimensions as synchronous; rounding off the 
                   fps value sent to OEM layer; modified camera_svcs_get_parm 
                   for unconditionally sending multishot mode to OEM layer
 08/13/08  nshree  Fix for KW error
 07/30/08  sm      Added code for burst mode run time detection
 07/17/08  hap     Fixed in camera_svcs_get_parm for SetGetParmPreviewFPS 
                   failure in OAT tests
 07/09/08  cg      Added EV compensation support for YUV sensors.
 07/02/08  chai    resolved rvct compiler warnings
 06/24/08  cg      Register camera with Clock Regime for higher clocks in case of Snapshot for LCU. 
                   For preview it will be conditional depending upoon the pixel clock.
 06/17/08   hap    Fix critical lint errors
 06/17/08   hap    Fix CR#147403
 05/08/08   cg     Added Support for YCBCR 420 Snpashot for VFE05
 04/16/08  sm     Merged changes for Multishot Single Encode feature to work
 04/14/08  nshree Merged the changes for, Fixed the GPIO issues (RST and EN_N) for 6290 FFA 
MCNs 10-VF130-21/-22/-23 with new 3MP (Micron sensor) camera, from branch to mainline.
 04/09/08  sm     Fixed Klocwork errors
 04/04/08  kd     Added support for mobicat preview over Chromatix for PC
 04/02/08  hap    Fixed Klocwork errors
 03/17/08  anil   Added support for setting udta atom for recording a clip
 03/11/08  anil   Fixed handling of CAMERA_PARM_FRAME_TIMESTAMP in camera_svcs_set_parm
                  and support to handle CAMERA_PARM_FRAME_TIMESTAMP in camera_svcs_get_parm
 02/11/08  cg     Support for new firmware VFE firmware image for ULC.
 02/28/08  cg     Register the camera client for getting lower clocks after the VFE Module is enabled for Sc2x
 02/27/08  cg     Fixed  bug with the  DCVS implementation for SC2x.
 02/21/08  hap    Added the NULL check while displaying the frame
 02/18/08  hap    Optimize YCbCr encoding type
 02/15/08  sanal  Adding Idiag support for frames dropped in a recorded clip 
 02/12/08  hap    Update the current luma adaptation enable value in 
                  camera_svcs_get_parm
 02/11/08  anil   Fixed camera_svcs_set_parm to get correct values of variable camera_parm_fps when camera_svcs_get_parm
                  is called.
 02/11/08  yp     Handling ILLEGAL_COMMAND message from DSP during recording.
 02/05/08  cg     Igonre the output1 when we are in Raw snapshot mode.
 01/25/08  hap    Bypass SW ABF for LCU
 01/24/08  sm     added error conditions on red_eye and exposure_comp of set/get param
 01/22/08  cg     Modified clipping calculation for LCU.
 12/19/07  nshree Fixed the compiler warning.
 12/19/07  nshree Request only for lower clock frequency when camera is in ready state for SC2X.
 12/18/07  nshree Corrected the parameters of qcamrawSetHeader() in the function camera_encode_picture_common
                  for to support YCbCr type encoding.
 12/17/07  kdiv   Control of setting Date and Time tags in Exif encoder is given to svcs too
 12/10/07  hap    Return CAMERA_INVALID_PARM in case of invalid parameters
 12/13/07  Yogi   Removed the screen flicker when we move from Preview Menu 
                  to Record Menu. Changes in camera_svcs_stop_preview, camera_svcs_blt_ex, 
                  camera_process_stop_preview & camera_config_vfe functions. 
 12/13/07  Yogi   Introducing cleanup code for recording when CAMQDSP_SERVICE_UNAVAILABLE
                  event is sent by qdsp service module.
 12/13/07  cg     Prevent calling AEC functions when using FLASH in YUV sensors.
 12/07/07  hap    Modified to signal UI task after camera complete clean up 
                  is done   
 12/07/07  cg     Release the frames when the MDP update fails during TCXO shut down, to avoid 
                  viewfinder freeze in SC2X.
 11/30/07  zl     Redo the fixes for stretched thumbnail on SC2x.
 11/02/07  sanal  Removed compiler error when FEATURE_VIDEO_ENCODE is disabled
 11/21/07  cg     Reverted the camera_svcs.c#600 changes to avoid thumbnail issues on SC2x.
 11/17/07  Yogi   Added Post Card Recording Support
 11/15/07  hap    Fixing RVCT2.2 compiler warnings
 11/14/07  hap    Added CLK_REGIME_UP_SLEEP__MCLK_HCLK fetaure protection
                  in camera_svcs_init
 11/13/07 cg/hap  Added support for DCVS for SC2X and  Added support for Chromatix header 0201
 11/1/07   jdas   Merged from branch: VFE is moved to idle state before proceeding
                  with recording stop operation, inorder to prevent AHB overflow error. 
 10/1/07  hap     Added changes needed to support YCbCr as a type of encoding.
 09/20/07  jdas   Dynamic Overlay support in camcorder
 09/18/07  zl     Fixed stretched thumbnail by keeping the same aspect ratio
                  as main image when configuring VFE.
 09/18/07 nshree  change in code to achieve correct clk value, to avoid horizontal lines while taking jpeg due to AHB overflow.
 09/17/07  ven    moved calling camsensor_msm_auto_detect from camera_svcs_init
                  to vfe_init function
 08/17/07 chai    fixed the compiler error
 08/30/07  hh     Moved camsensor calls in camera_svcs_start to graph task because MDDI CAMIF supports calls from Graph only.
 08/24/07  cg     Added support for fixed fps for YUV sensors.
 08/15/07  KD     Added call back function for multishot so each frame that is
                  captured calls a callback with CAMERA_EVT_CB_PICTURE
 08/10/08  jch    Further power optimization by reducing 6280/6800 bus clock.
 07/27/07  smn    Added the call to the new smart registration for multi-frame handjitter
 07/26/07  smn    Added extra check for burstmode defined to avoid compiler error if burst mode is not available
 07/23/07  smn    Added return to insure NULL pointer is never used in error HJR case.
 07/20/07  smn    Needed to add FEATURE_CAMERA_HJR around variable and added a catch for
                  an error condition.
 07/18/07  smn    Added logic to correclty find the image data header after
                  multi-frame HJR.
 06/26/07  arv    Vote for HCLK restriction during camera preview and snapshot
                  Also applies to Burst mode and QVP Encode
 06/20/07  ven    Fixed a problem with stand alone downsized snapshot image having
                  shift in the image.
 06/16/07  dle    Removed workaround for not able to idle VFE when in taking
                  picture mode. The needed VFEs are in all builds and the
                  workaround can be removed.
 06/11/07  zl     Fixed the problem when taking raw snapshot:
                  - Changed AHB burst length to 8.
                  - Fixed snapshot_frame.buffer in _process_qdsp_output2_msg().
                  Ignored invalid frames in _blt_ex() to prevent crash.
 06/04/07  zl     Ignored spurious VFE messages when camera is not in right
                  state to prevent state transition or crash.
 05/24/07  zl     Added preview fps settings from UI for YUV sensor through
                  camsensor_set_frame_rate() if AFR is supported by the
                  sensor driver.
 05/21/07  ven    Configured malloc to not to clear the memory to reduce the
                  latency and save power.
 05/17/07  ven    Fixed a problem with IN-CALL downsizer. The PadSize in the
                  downsizer output buffer should be 0 since data follows the
                  header immediately.
 05/16/07  ven    Fixed CR 119039. For snapshot 600x800, the actual image
                  encoded is 608x800 in 6260 and SC2X. Fixed it.
 05/16/07  dhaval Flashing screen prevention changes are featurized
 05/11/07  zl     Cleared screen when stopping preview in case of
                  CAMERA_PREVIEW_MODE_SNAPSHOT.
 05/11/07  zl     Added support to retry once for single shot and ignore
                  the failed picture in multishot with AHB overflows.
 05/08/07  Kadhir Added support in camcorder for dynamic voting of
                  high arm clock for QVGA resolution
 05/02/07  ven    Added support for setting 32 byte aligned address as
                  snapshot buffer start address to use the bus efficently
                  Changed the AHB burst length to 16.
 05/01/07  ven    Changed the error message for snapshot failure due to
                  tiering violation to be meaningful
 04/30/07  dle    Removed window centering in _blt_ex(). UI have all the
                  information and should be able to do centering itself.
 04/20/07  zl     Back out delay _RSP_CB_SUCCESS for preview until first
                  frame is received.
 04/16/07  zl     Resized preview overlay when picture size < disp size.
 04/11/07  dlee   Makes custcamera.h easier to change software operations.
 04/09/07  dlee   In camera_process_terminate(), if configured or enabled,
                  always send IDLE to DSP.
                  Added logging to capture time of VFE interrupt.
                  Corrected LPM logging.
 04/06/07  Fixed the following problems:
           1) Should not use CAMERA_MDP_LCD_ORIGIN_*. Should use
              BREW_OFFSET_* from OEMFeatures.h.
                Replaced CAMERA_MDP_LCD_ORIGIN_* with BREW_OFFSET_*.
           2) Should not use magic numbers like 240, 239, 320, 319.
                Replaced magic number 240, 239, 320, 319 with
                CAMERA_LCD_WIDTH and CAMERA_LCD_HEIGHT.
           3) In _blt_ex(), the application may change display window. Provide
              a way to recalculate the window. And always shift window with
              BREW_OFFSET_* when MDP is used. Currently it only shift if whole
              window is used. That is indeterministic and cause application to
              be messy.
           4) When overlay is used with MDP, and the overlay is less than the
              window, part of the window has garbage momentarily and causes
              flashing when the application is started the very first time
              after power up.
                a) In _process_start_preview, if MDP is used and preview has
                   overlay, then call camera_svcs_display_frame_overlay(TRUE)
                   to clear the overlay layer of MDP.
                b) Modified camera_svcs_display_frame_overlay() so that it can
                   also be used to clear the overlay layer of MDP.
                c) Removed calling _display_frame_overlay() from
                   camera_process_set_overlay().
 04/05/07  ven    Disabled FPS settings.
 03/30/07  klee   Clean-up lint errors due to un-matched data type in 3a
 03/28/07  ven    Each MSM can select the preview rotation by defining
                  CAMERA_PREVIEW_ROTATION in their respective custcamera.h file
 03/23/07  ven    Replace the magic numbers with the macros for CLKREGIME
 03/22/07  jv     Fix CR 113677. Send abort callback for CAMERA_FUNC_TAKE_PICTURE
                  when camera_release_picture is called.
 03/15/07  klee   Updated preview re-configuration on re-starting
 03/14/07  klee   Removed unnecessary conditional statement which is always TRUE
 03/13/07  jdas   Fixed problem of recording_cleanup while AHB overflow error
                  occurred during the process of stop record
 03/13/07  klee   Moved the code on special effect into AWB layer
 03/12/07  klee   Modified by 3A-restructuring
 03/09/07  dle    When UIONE, CAMERA_MDP_LCD_ORIGIN_Y is 30 lines.
                  Delay _RSP_CB_SUCCESS for preview until first frame is
                  received.
 03/09/07  Ven    Fix to display camera preview.
 03/09/07  ven    Split the single allocation of snapshot buffer into 2
                  fragments so that memory allocation will be in small
                  sizes for snapshot.
 03/06/07  jch    Use MED power mode during snapshot and viewfinder 30fps.
 03/07/07  zl     Updated FEATURE_CAMERA_MULTI_SENSOR support.
 03/06/07  dle    Removed FEATURE_UIONE_HDK.
                  Removed T_FFA_CROSSBOW_OV2620
                  Make sure when MDP is not defined,
                  FEATURE_CAMERA_LCD_DIRECT_MODE is undefined.
 03/02/07  js     In QVP specific handling in camera_qdsp_cb(), only take
                  action if IGNORE_CAMIF_ERROR is set to FALSE in the events
                  frame corruption reported by VFE.
 02/28/07  ven    Added support for LED flash in SC2X.
 02/28/07  ven    Removed support for 10 bit bayer filter
 02/27/07  jch    Removed sleep.h and include inside camera_svcs.h, fix of CR 109954
 02/26/07  jch    Remove MDP Qcopy references, newer targets don't need this to achieve 30fps.
 02/22/07  Gopi   Changes to prevent Flashing screen in camcorder app
 02/20/07  dhaval Set rotation to zero if it is equal to 360 for recording.
 02/16/07  dle    Fixed a simple _LCD_DIRECT_MODE bug. No CR filed.
 02/08/07  dle    Fixed a few LINT errors. Replaced a few MDP hardcode values.
 02/14/07  dle    Enclose AEEstd.h in CAMERA_USES_LIBSTD.
 02/08/07  ven    Changed the thumbnail settings for SC2X to QCIF since
                  the display is QCIF in SC2X
 02/07/07  dle    Replaced banned strcpy, stcncpy with std_strlcpy, using
                  CAMERA_USES_LIBSTD.
 01/29/07  ven    Fixed the problem with LCD_DIRECT_MODE.
 01/25/07  ven    Fixed the compiler warning for sc2x
 01/23/07  dle    Added FEATURE_CAMERA_ZOOM_TABLE2.
 01/19/07  sigu   ARM side changes to integrate the incall downsizer for 6260 target.
 01/18/07  jv     Fixed 2 second delay after YCbCr downsizer.
 01/18/07  dle    Removed un-needed -1 in _blt_direct() calculation.
 01/15/07  jv     Fixed handling order of output1, output2, and camif_done msgs
                  Fixed crash in multishot.
 01/09/07  dle    Fixing of the new buffer management scheme. Need to reset
                  buffers when reconfiguring.
 01/09/07  dle    Need to take of application that does not use camera_blt_*
                  for display, fixing of the new buffer management scheme.
 01/04/07  dl     Fixed round off error in luma filter during snapshot.
 12/27/06  khk    Fixed the CR 108897 on RAW Capture on YCbCr sensors.
 12/22/06  jch    Disable HJR when multishot is called.
 12/21/06  klee   Updated camera_qdsp_cb() not to request AWB/AEC stats with YUV sensor
 12/20/06  dle    Fixed couple _LCD_DIRECT_MODE bugs.
 12/20/06  klee   Added fix on CR 107950 - NULL pointer problem when camera is running with YUV sensor
 12/08/06  jv     Wait for capture to finish before send VFE_Idle.
 12/05/06  dle    Backed out reading of QDSP_vfeCommandReg.
 12/05/06  ven    Added CAMIF_PAD_REG_RESET support for SC2X.
 12/01/06  ven    Made the clkregim changes for SC2X
 11/21/06  jv     Fix handling of camera_timed_wait_tcb.
 11/21/06  klee   Fixed compiling error in camera_process_qdsp_output2_msg()
 11/18/06  klee   Fixed CR106426 - Crash on AWB/AEC stats request in ISR
 11/15/06  jn     AF always on without DSP changes.
 11/14/06  arv    Featurized QVP related code for SC2x
 11/14/06  klee   Enhance AEC/AWB stats requesting logic - CR104167
 11/13/16  kadhir Moved camcorder specific functions
                  to seperate file "camcorder_svcs.c"
 11/10/06  ven    Removed redundant waiting for offline snapshot.
                  This saves atleast 80ms during snapshot
                  Fixed some issues in latency profile logging.
 11/09/06  ven    Added additional logging for multishot.
 11/08/06  dhaval Pass the space_limit value through camera_svcs_set_parm
 11/07/06  ven    Added latency logging for multishot.
 11/02/06  jdas   AAC configuration setting moved to camera_svcs_set_parm_2
 11/03/06  ven    Added latency logging for various modules.
 11/03/06  Gopi   Fixed distortion due to CameraMidlet recorded clip
 11/02/06  klee   Added AEC Digital gain implementation
 10/30/06  dle    Fixed FEATURE_CAMERA_LCD_DIRECT_MODE bug. When feature is
                  enabled but direct mode is disabled, camera and camcorder
                  will use _release_frame() to ack, and should not be ignored.
 10/30/06  dhaval Space limit threshold value is passed to video engine.
 10/26/06  jv     Added soft focus for best shot.
 10/26/06  jv     Call camera_terminate at camera_process_qdsp_illegal_cmd_msg
 10/25/06  dl     Do not process lpm for YUV sensor.
 10/12/06  mm     Fixed camera's default_rotation for T_FFA.
 10/24/06  jv     Fix crash observed on 6800 when taking raw snapshot.
 10/23/06  dle    Implemented FEATURE_CAMERA_LCD_DIRECT_MODE
 10/23/06  jdas   Put code under FEATURE_VIDEO_ENCODE in camera_process_qdsp_output2_msg
 10/20/06  jv     Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
 10/18/06  jch    Preview async mode and new buffer mechanism.
 10/16/06  dle    Corrected a few typos. Also output snapshot FPS.
 10/16/06  ven    Featurized HJR
 10/12/06  dle    When taking picture, also ask for higher Arm/AHB clock.
 10/12/06  gopi   Reset the camcorder related variables when we start a
                  fresh recording session.
 10/05/06  ven    Added support for using chromatix variables in HJR.
 10/04/06  jv     Set camera_dsp_state = DSP_DISABLED before call to
                  CAMQDSP_Terminate and process YCbCr downsize.
 09/28/06  jv     Fix CRS 102778 and 102748. Cleaned up JPEG aborting when
                  stopping and added handling for JPEGENC_FILE_SIZE_FAIL
 09/25/06  klee   Fixed the bug on skip_pattern applying for CR100902
 09/25/06  dle    Moved sensor specific idle wait time to camctrl_tbl.
 09/22/06  dle    Removed the use of camera_take_picture_enabled.
                  Use header->frameType to make distinction between preview
                  and picture frame in _blt_ext. Also use frame->rotation
                  for preview or video rotation. Make sure rotation is set
                  correctly in the header.
 09/22/06  klee   Fix the bug on float->Q7 converting
 09/21/06  dle    Added support for the following two features for 6280 only:
                   FEATURE_ARM_HALT_CONTROL_CAMERA_OKTH_SIG
                    Camera will now vote to halt Arm.
                   FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE
                    Camera will now run at higher speed when needed.
 09/21/06  jv     Clean up termination redesign
 09/18/06  jv     Fix problems from previous checkin.
 09/14/06  jv     Redesign camera termination
 09/14/06  dle    Do not calculate VFE clipping in wrong state.
                  In _calculate_vfe_clipping() prevernt divide by 0.
 09/14/06  jv     Added white balance adjustment to best shot table
 09/14/06  cg     fixed indentation issues
 09/13/06  dle    Fixed a memory leak camera_clr_overlays().
 09/13/06  jv     Supply chroma pointer to PNG encoder
 09/12/06  dl     Used tuning variables for la and cs from chromatix.
 09/12/06  cg     Added support for Antibanding, Special effects , Auto white balance , Exposure Mode
                  to be done on sensor side(In case of YUV sensors).
 09/11/06  sigu   Fixed the ECR 100849 for 6260 target
                  Renamed camera_svcs_is_ycbcr_downsizer_enable as
                  camera_svcs_is_ycbcr_downsizer_needed
                  Ensured that when using downsizer, we are not going to
                  support BURST_MODE.
 09/11/06  ven    Removed the old bayer filter tuning parameters from the
                  function signature
 09/08/06  dle    Fixed an overlay problem. preview_overlay was deleted when
                  _create_encode_overlay.
 09/08/06  dle    Fixed high lint. Removed dead code.
 09/08/06  klee   Moved camera_aec_awb_delay_cnt, camera_aec_awb_delay() into 3a layer
 09/08/06  klee   Updated camera_svcs_setparam_vfe_snapshot() by AWB modularization
 09/07/06  chai   removed compiler warnings
 09/06/06  kadhir Modified prototype for camera_svcs_setparam_vfe_video()
 09/01/06  kadhir Removed unwanted Cross Compilation Dependency
                  When qcamcorder Feature Disabled
 09/01/06  dle    Fixed a thumbnail display bug (for ones that need padding)
                  in _blt_ext().
                  Fixed two bug in _pad_encode_overlay_frame().
                  Need to _adjust_overlay_for_rotation() after
                  _create_thumbnail_overlay.
                  Added OV2630 to 3 second wait in _send_vfe_idle_and_wait().
                  Fixed a bug in _rotate_encode_overlay().
 09/01/06  jdas   Removed EVENT_CAMERA_VIDEO_FAILED event in case of normal
                  error free CAMERA_VIDEO_ENGINE_EXIT_CB
 08/31/06  klee   Bug fix on previous and current wb maintaining
 08/30/06  wyh    Video Encoder State Machine robust to VFE reset.
 08/30/06  dle    Added CAMQDSP_Terminate() to release the FW before process
                  YCbCr downsize.
 08/30/06  khk    Added line camera_update_vfe = TRUE in camera_svcs_set_parm
                  to solve the frame rate updation problem in preview.
 08/29/06  dle    With MDP, icons can be supported in layer2. Removed icon
                  related code via FEATURE_CAMERA_SUPPORT_ICON_ARRAY. Code is
                  stripped.
 08/28/06  dle    Use picture_preview_copy to make a copy of picture display.
                  In the case of multishot, when the first picture is encoded,
                  its buffer is deleted. Without a copy, MDP would show some
                  corrupt frame.
                  Make sure thumbnail is in landscape mode.
                  Make sure thumbnail is not larger than picture.
                  Simplify _create_preview_overlay().
                  Need to recalculate_window when taking multiple_pictures.
                  preview_overlay is now P4 of _set_overlay() callback.
                  Deregister of background and layer 1 is now in GRAPH.
                  preview_overlay is now P4 of _set_overlay() callback.
 08/28/06  jch    Fix camif skip pattern, skip up to 15 frames.
 08/28/06  jv     Fix bug in bestshot related to special effects.
 08/24/06  jv     Removed call to camera_terminate from camera_reject.
 08/24/06  dle    BLT window is re-calculated once in camera_svcs_blt_ext()
                  when getting into preview, recording or snapshot.
                  MDP rotation is also adjusted at the same time.
                  camera_svcs_burstmode_check_frame () now returns whether all
                  messages are received from VFE.
                  camera_wait_camif_done_clk is disabled when all VFE messages
                  are received.
                  Change _process_take_picture_standalone(boolean) to
                  _process_take_single_picture(), both name and type.
 08/24/06  ven    Added support for Offline snapshot for msm6260
 08/24/06  chai   cleared the critical lint errors
 08/23/06  klee   Fixed no stats problem for AF in case of YUV
 08/23/06  jv     Fix crash when rotating for PNG encoding.
 08/21/06  sigu   Added the required S/W changes to incorporate the new vfe
                  message vfeMsgInvalidConfigparamreg.
 08/19/06  jv     Fix crash with YCbCr sensors
 08/18/06  rcc    Implemented changes from dliu's design review for luma
                  adaptation and changed asf variable names to make
                  consistent.
 08/16/06  rcc    Added support for luma adaptation option in UI
 08/15/06  jv     Bestshot code clean up. All bestshot functions now start
                  with camera_svcs_bestshot_*. Improved Error handling for
                  Best Shot.
 08/15/06  dle    Updated some MSG_ thing. Fixed snapshot blt rotated issue.
 08/15/06  jv     Partial fix for CR 99225. Now use UI supplied display size
                  for preview size.
 07/19/06  dle    In camera_send_idle_and_wait(), default to wait for 600ms.
                  For SONY sensors, wait for 3 seconds.
 08/14/06  jv     Enhanced snapshot memory allocation to only allocate enough
                  memory for specified format (H2V2 or H2V1)
 08/11/06  klee   Cleaned up the code regarding tuning variables
                  - ASF, LVCM and BVCM
 08/11/06  klee   Cleaned up the code regarding tuning variables
                  - AEC
 08/10/06  dle    Corrected typo.
 08/10/06  klee   Cleaned up the code regarding tuning variables relocation
                  - AWB/AEC
                  - Color matrix
 08/08/06  dle    Removed some junk, convert_buffer, reflect_buffer,
                  filter_buffer.
                  Removed camera_preview_rotation, use only
                  camera_default_preview_rotation.
                  Re-do preview/encode/thumbnail overlay.
                  Moved processing of _set_overlay() to GRAPH task. Block UI
                  when processing _set_overlay() and _set_icon_array().
                  Sensor position is now used only to indicate whether we need
                  to rotate overlay or not.
                  Setting of MDP rotations are moved to
                  camera_svcs_adjust_mdp_for_video();
                  Make sure offset for preview video and overlay are the same.
 08/08/06  jv     Fixed chroma pointer arg in qcamrawSetHeader for burst mode
 08/08/06  jv     Cleaned up camera_svcs_set_dimensions
 08/07/06  sigu   Integrated ycbcr downsizer for M6260 target
 08/07/06  dhaval Sending ack2 when FEATURE_VIDEO_ENCODE_THROTTLE macro and
                  camera_video_skip_buffer_flag are enabled
 08/05/06  cg     Color Conversion Matrix Pointer should be checked for  NULL only in case of NON -YUV Sensors,
                  in camera_svcs_set_parm()
 08/04/06  dl     Added chromaBuffer in qcamrawHeader to allow independent
                  Luma and Chroma data allocation.
 08/04/06  dl     Used enable_5x5_asf to determine sensor support for 5x5 ASF.
 08/04/06  klee   Update camera_svcs_start() to sync service and 3a layers
 08/04/06  dl     Removed extern variable camctrl_tbl.
 08/02/06  nag    Added init parameter to reorder of moov mdat atoms according to KDDI specification
 07/31/06  js     Added QVP-specific testing code for encoder rotation
                  feature under feature FEATURE_QVP_TEST_ENC_ROTATION.
 07/30/06  dl     Moved post processing params to camctrl.
 07/28/06  jv     Fixed Best Shot ISO and HJR handling as they are now under
                  a single set of selectable parameters from UI. Implemented
                  ISO/HJR setting restrictions during best shot mode operation.
 07/28/06  ven    Changed the middle frame as the reference frame for 3 frame
                  registration for HJR.
 07/27/06  jch    Fixed discard first frame in preview.
 07/27/06  ven    Since HJR and ISO are mutually exclusive, they are merged
                  under a single set of selectable parameters from UI.
                  CAMERA_PARM_HAND_JITTER_REDUCTION will be removed once UI
                  stops referring it.
 07/26/06  wyh    Remove global variables that control special effects.
 07/26/06  ven    Changed the bayer filter name and added comments
                  to the bayer filter function
 07/26/06  jv     Set manual flash white balance in bvcm if needed by bestshot
 07/25/06  ven    Removed HJR structure from vfe_sw_feature_type and added
                  to cam_ctrl_tbl
 07/25/06  jv     Enhanced handling of flash during bestshot mode. Fixed handling
                  of color correction. Implemented anti-banding restriction during
                  best shot outdoor modes.
 07/25/06  jch    Fixed thumbnail display during multishot.
 07/24/06  ven    Added support for Antibanding for HJR.Changed the Normal CC matrix to
                  hybrid CC matrix for HJR.
 07/19/06  dl     Added thumbnail processing for LPM.
 07/19/06  jv     Fix handling of camera_svcs_start for bestshot.
 07/18/06  jv     Best shot mode feature implementation.
 07/18/06  ven    Added support for HJR on thumbnail in case of Multishot based HJR
 07/15/06  ven    Cleaned Lint errors related to HJR
 07/13/06  ven    Moved hjr setup function to camera_aec.c
 07/11/06  ven    Moved HJR AEC related code from camera svcs to aec
 07/11/06  jch    Embedded burstmode clear pictures inside camera_clr_take_picture
 07/07/06  jch    Added protection timer back in for Burst Mode Operation
 07/07/06  ven    Revamped the rotation/reflection functionality.
                  Sensor position is no longer supported
                  UI to use encode_rotation and relection to achieve rotation
                  for snapshot and preview respectively
 07/05/06  jch    Added clearing pictures in camera svcs layer.
 07/05/06  klee   Updated to refer color matrix in VFE
 06/26/06  jv     Changed parameters and return value for camera_get_bestshot_config
 06/26/06  klee   Update output1 msg handler to request AEC/AWB stats for every frame
                  Update camera_aec_awb_ready() not to make any delay with camera_update_vfe flag
 06/21/06  jn     Added more AF window restrictions.
 06/20/06  ven    Cleaned up the unused offline snapshot routines meant to do
                  offline snapshot from raw image in EFS.
 06/20/06  jv     Refactored bestshot hand jitter and luma adapatation control.
                  Added camera_get_bestshot_config.
 06/19/06  jv     Bestshot mode feature implementation. Added
                  camera_svcs_hjr_enabled and camera_svcs_la_enabled to test
                  if features are enabled. Replaced hjr_enable == TRUE with
                  camera_svcs_hjr_enabled() == TRUE.
 06/19/06  chai   ported fix for Memory leak in camera engine when encoding of
                  snapshot is not done.
 06/16/06  ven    Added HJR support with bayer filtering for MSMs with HW Roll OFF
 06/13/06  ven    Changed the output 2 configuration to be same as clipping
                  window for snapshot in 6260.
 06/12/06  jch    Added camera_take_multiple_pictures and related functions.
 06/11/06  ven    Added support for Full Size snapshot for 6260.
 06/09/06  dle    If service is not available, make sure camera_dsp_state is
                  in DSP_DISABLED statte.
 05/27/06  dle    When not HANDLE_CAMIF_ERROR, just keep corrupt count.
 06/06/06  dl     Added Luma Adaptation support.
 06/06/06  ven    Added support for 6260 QVGA snapshot. Need to test other modes.
 06/02/06  dle    Added support for _PARM_FPS_LIST.
 05/27/06  ven    Added UI support for HJR. Fixed a problem with LPM for HJR
 05/26/06  dle    Fixed high and critical lint errors.
                  Removed camera_preview_expansion, returnData, cameraFrameCnt,
                  not used.
                  Optimize now vfeVideoConfig, vfeSnapshotConfig, and
                  vfeRawConfig are in vfe.c.
 05/25/06  jv     Added 3x3 ASF/Luma support for Snapshot, and Preview
                  Moved call to vfe_start() in camera_svcs_start() to after
                  camsensor_start() and moved initial vfe configuration from
                  camera_svcs_start() to vfe_start().
 05/22/06  klee   Updated bvcm logic to use the table in chromatix header
 05/11/06  ven    Fixed the compilation issue for HJR and checked HJR for single
                  shot and multishot.
 05/10/06  dle    Fixed additional compilation problem with HJR multishot.
                  Fixed typo from _mulitframe_array to _multiframe_array.
 05/08/06  ven    Fixed the compilation problem with HJR mutishot.
 05/04/06  Shahed Changes for new A/V sync implementation for camcorder for AAC
                  with CMX MM API sync signal.
 05/04/06  nhk    Change in QVP VFE OUTPUT2 acknowledgement to improve VT fps
 05/03/06  jn     AF window is relative to the VFE clipping window.
 05/02/06  klee   Moved AWB and AEC code into 3a layer
 05/02/06  ven    Fixed the following issues with frame overlay
                    - Optimized the heap usage for frame overlay by effectively
                      using temporary buffers
                    - Seperated thumbnail overlay from preview overlay since preview window
                      size could be different from the thumbnail standard size.
 05/01/06  ven    Hand Jitter Reduction feature is incorporated.
                    - Tested on 130 nm.
                    - Thumb nail HJR and support for Anti banding are pending.
 04/28/06  dl     Wait until VFE offline processing is done reading
                  from external memory to download firmware.
 04/28/06  nshree To make AEC operation during digital zoom a configurable option
 04/21/06  jch    Implementation of Burst Mode Feature.
 04/20/06  sch    Display preview frame at user specified x,y coordinates,
                  CR91980.
 04/19/06  sigu   [Ported the fix for CR 91014 by mvd from 4.3 branch]
                  In  camera_config_vfe, set output1 size to thumbnail
                  size instead of the preview resolution size.  Here
                  we rely on the camera_svcs_start which initializes
                  these to 320x240.  Output1 is limited further if the
                  capture size is less than the display size.
 04/19/06  sigu   [Ported the fix by mvd from 4.3 branch]
                  Fix for the MDP "green line" issue where the last CbCr
                  line buffer needs to be duplicated before giving the
                  preview window to MDP to avoid a hardware problem
                  Note that the limitation: "MDP needs multiple of 16
                  dimensions" has been removed by this fix.
 04/13/06  dw     Reset VFE ACK2 globals for video to prevent ASSERT errors
 04/13/06  klee   Added camera_recall_vfe_config() to let preview start with
                  the latest AEC/AWB gain during camera service.
 04/07/06  Shahed Added a variable for to log timing for audio/video sync
                  in camcorder.
 04/03/06  dl     Turn off BVCM when resize_facotr is greater than 16.
 04/02/06  ven    Fixed the Frame Overlay issue for frames bigger than QVGA
                  Added support for overlay in thumbnail also.
 04/02/06  sch    VFE20 Support
 03/29/06  dle    Changed _set_exif_make_model() to use mob_sw_rev for make.
 03/27/06  wyh    Run output2enc_ack2command() from GRAPH task, not ISR.
 03/24/06  dlee   In camera_timed_wait() added comment to make sure clock
                  is stopped.
 03/24/06  nshree check/report mdp failures
 03/23/06  dl     Added support for chroma suppression.
 03/22/06  dle    Note that IGNORE_CAMIF_ERROR is really related to frame
                  corrupt reported by VFE. CAMIF corrupt is gated by sensor
                  flag ignore_camif_error.
                  Added code to handle CAMIF_ERROR, when HANDLE_CAMIF_ERROR
                  is defined. Default is not defined. It handles CAMIF_ERROR
                  reported by the VFE.
                  When CAMIF_ERROR is reported in camera_qdsp_cb, it is queued
                  for processing, but further errors are ignore, until when
                  the VFE is re-configure. This is to avoid handling multiple
                  CAMIF error in a frame.
                  When CAMIF_ERROR is processed, it does nothing but set a flag.
                  When output1 is available, if the flag is set, the VFE is
                  reconfigured.
                  When output2 is available, if the flag is set, ouput is
                  ignored.
 03/20/06  klee   cam3a.c and cam3a.h don't exist in 3a layer anymore.
                  camera_svcs needs to call the same 3a functions with different names
 03/20/06  klee   Fix SR 791445 by calling vfe_start() in camera_svcs_start()
                  6 vfe related variables have been transited to vfe layer
 03/17/06  dle    In camera_svcs_process_func, if not in _RECORDING or
                  _QVP_ENCODING state, ignore _VIDEO_HANDSHAKE.
 03/13/06  dle    Added CAMERA_MALLOC_LOGGING to enable or disaable
                  camera_malloc() and camera_free(). Default is disabled.
                  bAudioPostCard in _start_record() is _V770, not just _V7.
                  When handling pclk_invert, positively set or clear invert.
 03/10/06  ven    Fixed problem with images taken with special effects by not
                  adjusting the color conversion and color correction matrices
                  when special effects are selected. It also fixed CR 89492
 03/09/06  dle    Renamed thumbnail to thumbnail_buf, raw_snapshot to
                  raw_snapshot_buf, make it more unique and easy to search.
                  Init thumbnail_buf, raw_snapshot_buf, offline_input to NULL
                  and make sure they are deleted before malloc to prevent
                  memory leak.
                  Removed picture_buffer, no longer used.
                  When camera_release_picture() is called, camera_app_cb is
                  cleared to prevent callback to application. This function
                  needs not callback and may not take callback as the
                  application may be tearing down and the callback object
                  may no longer valid.
 03/03/06  klee   Removed camera_vfe_luma from service layer
                  - value on current luma should be stored in cam3a_aec_state.current_luma
                  - current luma will be managed in 3a layer
 03/06/05  dw     Auto stop camcorder recording during memory space out events
 02/28/06  sch    FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK for using internal
                  clock to configuring VFE.
 02/27/06  sch    Added camera_svcs_optimize_mdp_settings () to optimize
                  mdp setting in one place, ex., Use MDPOP_QUEUE_COPY only if
                  the sensor frame rate is > 15
 02/24/06  ven    Added support for profiling awb and aec
 02/21/06  jn     Added CAMERA_FUNC_MOVE_LENS_DONE.
 02/13/06  dle    Added CAMERA_PARM_FLASH_STATE.
                  Changed camera_aec_awb_delay_cnt from 3,2,2 to 3,1,1.
 02/01/06  dle    Changed AE/AW delay count.
                  Fixed a problem for camcorder where output 2 is larger than
                  output 1 and zoom step 8 failed because the clip window is
                  now less than output 2.
 01/31/06  dle    Linted.
 01/26/06  sch    Defer AEC/AWB stats for specified number of frames.
 01/30/06  ali    Added a new parameter to camera_start_record for audio
                  postcard mode
 12/25/06  sch    Adhering to camctrl table function invocation methods.
 01/24/06  dle    Allows _enable_qvp() and _disable_qvp() in STATE_INIT.
 01/24/06  dl     Minimized the effect of coefficient quantization error.
 01/23/06  dle    AF window is set to 1/2 of clipping window width and height.
 12/19/05  sch    Added back legacy implementation of night shotmode for
                  backward compatibility of sensor drivers.
 12/19/05  sch    Modifications for nightshot mode to go through cam3a
                  to adjust the frame rate, indoor / outdoor indices.
 01/17/05  babakf Added smooth zooming based on look up table.
 01/17/06  babakf Added CAMERA_FUNC_MOVE_LENS message processing to process_func
 01/16/06  dle    Fixed all lint problems.
                  Fixed camera_timed_wait. Need to be protected with critical
                  section and need to clk_dereg. Removed invalid use of
                  camera_wait_clk. All should call camera_timed_wait().
                  Removed FEATURE_CAMERA_ENCODE_PROPERTIES.
                  Removed _start_stand_alone(), _stop_stand_alone(), and
                  _encode_picture_stand_alone(), _color_convert(). Not
                  applicable for 6550.
                  Moved setting of camera_dsp_state = DSP_ENABLING to before
                  calling VFE_Initialize(). The state could be checked in
                  ISR, so must be set first.
                  In camera_send_vfe_idle_and_wait(), wait for 2000ms to
                  accommodate Sony 2M sensor. It may take 1.6 second to
                  transition from snapshot to preview mode.
 01/13/06  dw     Fixed divide-by-zero issue while generating skip pattern
 01/06/06  dl     Fixed pink layer in BVCM when 10bit sensor output is used.
 01/06/06  jn     Added snapshot profiling info in terms of QXDM messages.
 01/04/06  ali    Support for audio only record in camcorder
 12/22/05  nhk    Fixed video freeze in QVP caused due to race ccondition between
                  camqdsp_isr and graph_task to process ack2output
 12/23/05  rve    Removed auto exposure adjustment from
                  camera_process_qdsp_output1_msg.  AEC needs to be done as
                  soon as stats are available.  Move function call to cam3a.c
 12/22/05  anb    - Made timeout dependent on Frame rate.
                  - Added more debug messages.
 12/22/05  gm     Added new events for camcorder
 12/16/05  dle    Replaced cam_timtest_out_addr with GP1_BASE so can be compiled
                  in L4 build.
 12/09/05  dw     Set sensor frame rate to 15 for video applications if
                  sensor outputs 30 fps by default
 12/08/05  mvd    Enabled MDP Queuing by default in Camera_start.  Disabled
                  MDP Queuing in param CAMERA_PARM_FPS when the requested FPS
                  is less than or equal to 15.
 12/08/05  jj     Removed "static" from camera_output1_frame_rate for MobiCaT.
 12/05/05  dw     Added support for dynamic vfe skip frame pattern calculation
 11/30/05  anb    When changing contrast made sure that camera services layer
                  reconfigures the VFE after putting it into idle state.
 11/29/05  dw     Default video encoding frame rate to 15fps if rate not set
 11/28/05  anb    Made sure that camera_svcs.c file compiles with FEATURE_MDP
                  disabled.
 11/23/05  rve    Update preview_fps variable for frame rate when UI
                  successfully enables and disables nightshot mode.
 11/22/05  Gopi   Added support for aspect ratio checks(output1 and output2)
                  in Camcorder.
 11/17/05  ss     In Qvideophone, change vfe configuration to
                  VFE_O2YCbCr420_LinePack to allign with overlay frame format
 11/14/05  Shahed Changes due to new implementation of camcorder space status
                  error/warning msgs.
 11/14/05  anb    - Made Debug messages more meaningful.
                  - Change code to validate pointers before de-referencing it.
 11/11/05  Shahed Undo SPACE error/warning changes.
 11/11/05  ss     Removed build dependency on qvp_encvfe.h
 10/08/05  Shahed Added support for more detailed messages of SPACE
                  error/warning to report to app.
 10/08/05  dw     Added a logic in camera_video_engine_error_recovery to check
                  if recovery argument is set before sending callback to app.
 11/07/05  dl     Corrected feature flags for post processing.
 11/03/05  anb    - Made stopping operation blocking.
                  - Split stop functionality such that take picture operation
                    can be aborted.
                  - Added initial support to measure Frame Rate as a function
                    of frames received from the VFE.
                  - State transition from take picture to ready state is now
                    dependent on CAMIF_DONE ISR from the DSP.
                  - Recovery mechanism if take picture operation fails.
 10/27/05  dl     Added BVCM and LPM abort message.
 10/21/05  anb    Enable camera debug.
 10/21/05  dl     Resolved lint issues.
 10/21/05  ss     Added code to handle acking of output2 frame when caputred
                  frame is corrupted for QVideophone
 10/16/05  anb    PNG and JPEG images in landscape mode are 180 degrees
                  rotated wrt each other.
 10/14/05  dle    Renamed VFE1X to VFE1P.
 10/13/05  anb    Change camera state to READY after stopping Preview.
 10/12/05  dl     Disable Luma filter when no smoothing is needed.
 10/11/05  sch    Removed FEATURE_CAMSENSOR_PANASONIC_MN101E19A,
                  FEATURE_PANASONIC_MN101E19A_ZOOM dependency.
 10/11/05  Gopi   Changes in camera_process_video_engine_cb to notify the
                  client in case of any error in pause/resume scenario.
 10/11/05  ss     Fix msg_output2 ack for offline encoding in qvp
 09/30/05  dl     Resolved lint issues.
 09/30/05  nhk    - Added support for offline encoding for Qvideophone
 09/30/05  mvd    - Fixed bug where AEC/AWB stats were not happening after AF
                    operation.
                  - Added Arshad's fix for QVP_ENCODING state to decrement
                    frame drop counter because QVP is using the contrast
                    feature.
                  - Fixed slow update of AE/AWB by issuing stats request
                    during stats ISR as well.
 09/29/05  anb    - Modified how encode abort is called to prevent timing
                    issues.
                  - Fixed memory leak during BVCM operation.
                  - Update camera_bvcm_capture_mode when switching mode from
                    snapshot to raw mode.
                  - Added logic to proactively check for memory leaks.
 09/27/05  dw/anb Modified video engine callback routine so camera uses video
                  engine APIs legally during error recovery
                  Fixes for camera_do_stopping if it is called when VFE idle
 09/23/05  dw     Added new error recovery routine/scenario in video engine cb
 09/20/05  drl    Fixed error recovery in camcorder when video_engine reports
                  a failure.
 09/14/05  dl     Replace LOCAL macros with static keyword.
 09/13/05  dl     Resolved lint issues.
 09/13/05  ven    added support for storing sensor specific exif paramters
 09/12/05  mvd    -Fixed bug in the function camera_process_qdsp_output1_msg
                   in the checking of AF state flags and reworked the
                   AF and AEC AWB stats issuing commands
                  -Fixed output2enc_ack2_criteria() by adding TASKFREE to
                   the error return path.
                   output2enc_ack2_criteria()
                  -In start_preview, added check for camera_qvp_enabled
 09/09/05  dw     Added support for adaptive VFE luma filtering
                  Changes for video driver to access N-frame video buffers
 09/06/05  anb    In function camera_process_start_preview(), changed the way
                  UI call back was handled in case of in call handoff.  This
                  change fixed a problem where the screen went blank after
                  taking a picture.
 09/02/05  anb    Added support for in call voice hand off.
 09/01/05  anb    Call pnge_encode_abort only if FEATURE_PNG_ENCODER is
                  defined.
 08/30/05  kln    Used a JPEGE function to get MCU padded dimensions
 08/30/05  dl     Use D65 BVCM table when lighting condition is SUNLIGHT1.
                  Use hybrid color correction matrix in BVCM.
 08/29/05  sch    Changed the clipping window to be multiple of 8 for
                  JPEG requirement.
 08/29/05  mvd    Move #def for FEATURE_PNG_ENCODER to fix compile error
                  if this flag is undefined.
 08/29/05  dl     Define and replace offsetof macro.
 08/24/05  anb    - Fixed memory leak when stopping application in READY state.
                  - Reset runningOutput2SkipPattern back to zero after being
                    used.
                  - Reset ackOutput2 in camera_svcs_start.
                  - Call pnge_encode_abort if we are asked to stop the
                    application during PNG encoding.
                  - Removed extra CAMERA_SIG generated in function
                    camera_process_qdsp_idle_state_msg.
                  - Rewrite how camera service layer stops or terminates the
                    application.
 08/22/05  dle    if vfe_init_rolloff_tables() failed, fail camera_start().
 08/18/05  dl     Disable 3x3 vfe_filter if hw feature is not defined.
 08/18/05  dle    Renamed vfe_hw_features_auto_detect() to
                  vfe_features_auto_detect().
                  At camera_svcs_start(), init gamma and rolloff tables.
                  camera_modify_contrast() is moved to vfe_modify_contrast().
                  Modifed to call vfe_put_gamma() which then make decision on
                  which table to use and adjust contrast accordingly.
                  VFE configuration functions now expects configuration pointer.
 08/17/05  dw     Enable N-frames buffer skipping in video pause/stop record
 08/17/05  dl     Use 8 bit value of black level for BVCM. Removed
                  MSMHW_VFE_LUMA_FILTER
 08/11/05  ng     Don't release MDP of VT app if VT call in progress
 08/16/05  cl     Added support for setting multiple sets of uuid.
 08/16/05  dw     Added throttling mechanism for video encode stop/pause.
 08/09/05  sch    - Updates for independent RGB gain configurations.
 08/09/05  anb    Fixed some compiler warnings.
 08/08/05  anb    Fixed pink/black strips at left border of QQVGA images with
                  frame added.
 08/05/05  anb    - Fixed frame overlay icons not being displayed during
                    viewfinder preview.
                  - Fixed memory leaks in function
                    a. camera_clr_icons.
                    b. camera_process_set_exif_tag.
                  - Added initial support to use camera_set_thumbnail_properties()
                    and camera_get_thumbnail_properties().
 08/04/05  alin   Resize overlay frame for preview if preview size != encode size
 08/01/05  dl     Checked vfe_hw_features before luma filter, asf and
                  vignette correction.
 07/29/05  kln    Added UI controled EXIF attributes to EXIF header
 07/28/05  sch    Runtime detection of VFE hardware features.
 07/21/05  dl     Enabled BVCM. Also added black level substraction and WB
                  on BVCM.
 07/25/05  mvd    Completed the camera_svcs_get_info() function
 07/25/05  dw     Added static luma filter update for video recording
 07/07/05  dl     Added the following feature flags
                  FEATURE_CAMERA_BVCM
                  FEATURE_CAMERA_5x5_ASF
                  MSMHW_VFE_LUMA_FILTER
 07/07/05  rve    Added black rows offset for black level module
                  start_line_gap = 5+camsensor_static_params[camera_asi].blackrows;
                  in function camera_calculate_vfe_clipping.
 07/05/05  kln    Added a function to fill EXIF info from sensor,UI, and VFE
 07/05/05  anb    Some cosmetic changes.
 06/27/05  sch    MDP needs mulitple of 16 input image dimensions.
                  VFE ouput programmed to multiple of 16 dimensions.
                  Clipping is done on MDP to the requested image dimensions.
 06/25/05  anb    Stopping the camera app while encoding the picture causes the
                  camera to be in incorrect state. Fixed this problem.
 06/22/05  dle    Enabled CAMERA_LOGGING.
                  Cleaned up camera_enable_solarize().
 16/21/05  dw     Changed camera_svcs_set_parm to receive AAC advanced config
 06/20/05  anb    Fixed UI freeze on changing contrast.
 06/17/05  alin   Use curtain effect global var instead of macro
 06/15/05  jk     Changed Camcorder AAC extern variables to globals.
 06/15/05  jk     Added support for the Camcorder AAC UI.
 06/13/05  anb    Fixed the problem where we cannot save jpeg images on MMC
                  after saving few images on Flash.
 06/09/05  lrw    Change alpha branding to 100% - only allow transparency for the desired color
 06/08/05  mvd    Fixed bug in CAMERA_PARM_PREVIEW_FPS
 06/07/05  dle    Removed _PARM_THUMBNAIL_*. Please use:
                    camera_set_thumbnail_properties() and
                    camera_get_thumbnail_properties().
 06/07/05  dle    If resolution is not even, round it up. Report modified
                  resolution with parm4 in callback.
 06/06/05  dl     Added ASF 5x5 support
 06/01/05  jk     Passed CAMQDSP_ImageType to VideoEncDSP_Initialize for
                  concurrent DSP.
 06/01/05  mvd    Added error fatals in camera svcs start to catch critical
                  error conditions.
 06/01/05  anb    Made sure that are check to see that the state machine is
                  in correct state does not cause a dog timeout.
 06/01/05  mvd    Check in for Arshad.  Fix CR 64907.  Added wait loop in
                  camera svcs start so that previous sessions of camera
                  have time to exit.
                  Fix CR 60916.  Added EVENT_CAMERA_EXIT to the function
                  camera_process_qdsp_idle_state_msg(), where it seems
                  to belong.
 05/31/05  anb    Changed logic to allocate and deallocate memory while
                  handling flip and rotate operation.
 05/24/05  drl    Fix CR 62635.  Problem with re-entering camcorder record
                  session after entering QVP.
 05/17/05  dw     Added special fade effects menu support for camcorder
 05/17/05  alin   Fixed timestamp so that (1) first frame has timestamp
                                          (2) timestamp starts at 0.
 05/17/05  mvd    Changed camera_vc_mode = CAMERA_LUMA_VIGNETTE_CORRECTION
                  Added changes for BREW 3.1.4
 05/13/05  drl    Fixed Video Rotation Problem (Rotation of clip is 90 off of
                  preview rotation).
 05/13/05  wyh    Video Transition Effects.
 05/12/05  wyh    Fixed freeing camera_overlay and video_encode overlay 2x
 05/12/05  drl    Fixed quitting and re-entering app state problem.
 05/11/05  anb    Added code to support rotation for PNG images.
 05/09/05  dl     Added BVCM supports.
 05/09/05  js     In camera_process_release frame, if QVP is enabled,
                  issue CAMQDSP_CMD_END_OF_FRAME_ACK1 to DSP only if VFE
                  is not in IDLE state.
 05/09/05  mvd    Removed FEATURE_CAMERA_BRIDLE
 05/06/05  anb    Fixed camera flickering on changing contrast.
 05/04/05  dle    camsensor_config() is freed of VFE. After calling
                  camsensor_config(), call vfe_copy_sensor_config().
 04/28/05  lrw    Check sizes in camera_svcs_set_overlay, comment out un-used vars
 04/26/05  drl    More adjustments to skip frame pattern.
 04/26/05  mvd    Added error checking of fs_write call (if the full
                  amt requested was not written)
 04/25/05  sch    - Sanity check for divide by zero.
 04/25/05  sch    - Programmed VFE to output thumbnail, main image with
                    multiple of 16 dimensions as JPEG requires it.
                  - Zooming modification to have the same effect when
                    recording and preview are of different sizes in Qcamcorder.
                  - Abstracted GPIO and CLK regime configurations under
                    macros.
 04/23/05  lrw    Update camera_complete_terminate in QVP to change camera_state
                  to CMAERA_STATE_INIT only when camera_qvp_enabled is TRUE
 04/22/05  drl    Re-fixed the skip frame pattern to set skip pattern based on
                  FPS settings.
 04/22/05  jk     Fixed a problem of camcorder END key while recording.
                  Moved video frame time stamping to ISR context.
 04/21/05  kwa    Replaced direct references to GPIO_OUT_XX to fix
                  portability issues.
 04/19/05  drl    Fixed bug where stopping record after pause failed to
                  reset skip frame pattern.
 04/15/05  jk     Fixed camcorder quick pause/resume problem.
 04/12/05  sch    Fixed portability issues with clk configurations.
 04/11/05  babak  Added camera_stopping code around VFE_Idle() call to make
                  sure we do not attempt to put VFE into idle too soon after
                  we just tried to put VFE into idle. (fix bug when user
                  changes contrast too quickly).
 04/09/05  anb    Stopping the camera while taking the picture causes the
                  camera to be in incorrect state. Fixed this problem.
 04/09/05  mvd    Fixed bug in Contrast parm where viewfinder/image would
                  go completely black if Contrast was other than the default
                  setting after taking a picture.
 04/09/05  mvd    Fixed potential error in camera_svcs_set_dimensions.
 04/08/05  dw     Added support to power down/off camsensors properly after
                  QDSP has been disabled.
 04/07/05  sch    More power saving by halting MDDI_CLIENT_IO_CLK,
                  MDDI_CLIENT_DIV4_CLK, MDDI_CLIENT_VFE_CLK.
 04/06/05  dl     Process LPM only in VFE snapshot/ offline mode.
 04/05/05  drl    Revise camera_set_dimensions args to provided a consistent
                  interface for camera and camcorder.
 04/04/05  anb    Added code to support mirror/water reflections except for
                  H1V1 format.
 04/01/05  sch    For saving power consumption, halt the related VFE and
                  and MDDI client clocks.
                  HWIO_MSM_CLK_HALTB_ADDR (0x80000004)
                  0x0F039692 (Upon reset)
                  0x08039012 (After starting Qcam App)
                  0x0F039692 (After exiting Qcam App)
 03/30/05  drl    Added Short Header/Rotation checks.
 03/24/05  kln    Added a check to perform thumbnail upsampling only if
                  image has thumbnail
 03/24/05  mvd    Added #include assert.h at the request of the 6275 build
                  team
 03/24/05  mvd    Added support for new CAMERA_PARM_PREVIEW_FPS used in
                  camera Auto Frate Rate mode.  Added set_parm_2
                  interface to support this effort.
 03/24/05  drl    Fixed AHB computation for output2/linepack.
 03/23/05  sai    Fixed support for offline snapshot.
 03/23/05  lrw    Landscape mode Overlay for QQVGA and QVGA
 03/22/05  sai    Added support for offline snapshot.
 03/22/05  drl    Added support for variable dimension output1 vs output2
 03/22/05  jk     Added support for 'fixed_fps_mode' and 'movie_size_limit'.
 03/22/05  babak  Added contrast
 03/21/05  wyh    Fixed overlay for camcorder, fade-out, and pause/resume
 03/21/05  mvd    Added support for nightshot PARM.  A sensor-dependent
                  feature.
 03/17/05  mvd    Added special AWB state var. to fix special effects mode.
 03/16/05  jz     Fixed QVP Setup Failure and QVP/QCamcorder Concurrency Issue
 03/16/05  lrw    MDP frame overlay for QVGA, QQVCA, Preview and taking picture.
 03/15/05  drl    Added rapid Start/Stop record support and
                  Added V7 parameters to camera_set_dimensions for encode
                  width and height
 03/10/05  anb    Change code to support EV range from -2 to 2.
 03/14/05  sch    Merged Troy's fixes for.
                  - fix the EXIF sensor name tag hardcoded to Casio NX5300
                  - Misfeaturization around strobe/LED/any-flash code
                    that caused AEC to be turned OFF after a picture was
                    taken when using an LED flash.
 03/14/05  wyh    Output2 video encoding using new VFE with out2 linepack.
 03/14/05  babakf Added solarize functionality
 03/13/05  sch    Zoom updates for more linearity with certain censors.
                  camera_set_encode_properties returns camera_ret_code_type.
 03/11/05  wyh    Video Overlay: pass transperant color to vse_overlay()
 03/10/05  anb    Added code to support exposure compensation.
 03/09/05  ss     Added frame skip bit pattern to support range 1-15 fps
                  in camera_set_parm()
 03/04/05  dl     Added LPM process after take picture.
 03/04/05  sch    Fix the wrong call back in camera_process_stop ()
 03/03/05  dw     Initialize VFE for camera take picture
                  Terminate DSP in all end preview contexts
 03/02/05  dw     Fully disabled the DSP whenever exiting preview context
 03/02/05  drl    Added stability to pause/resume record
 03/02/05  wyh    Added the TimeStamp special effects
 03/02/05  mvd    Fixed stop/clear key/abort state machine issues during
                  JPEG encode.  Now calling jpeg_encoder_abort().  State
                  machine issues still exist when hitting a clear key
                  during snapshot mode (before encoding).
 03/02/05  sch    De-registering background layer in the camera_jpeg_encoding
                  callback.
                  Updated output1 preivew AHB computations.
 02/25/05  sch    Added *_focus_window () functions.
 02/24/05  wyh    Added Fade-in/Fade-out, fixed pause/resume bug.
 02/24/05  drl    Added support for user_bitrate.
 02/23/05  sch    - Ceiling operation for AHB burst factor in raw snapshot
                  mode.
 02/23/05  mvd    Added cloudy manual white-balance support.  Added fix to
                  special effects in OFF mode where the default Color
                  Conversion array written now takes into account manual
                  white balance settings (CH Mobile request)
 02/23/05  sch    - Merged Sai's 10-Bit Bayer raw snapshot Raw support.
                  - Flexible burst factor computation.
 02/23/05  drl    Moved camera_state change in camera_complete_terminate
                  to always transition to CAMERA_INIT_STATE.
 02/23/05  dw     Removed target bitrate parameter for video in camera layer
 02/22/05  dw     Added globals for fadein/out video encoding effects
 02/22/05  sch    - Zooming support for YCbCr sensors.
 02/22/05  wyh    -Raw Encoder API changes.
                  -Support for video encoding from output1
 02/18/05  mvd    Added antibanding support.
 02/15/05  sch    Power down sequence updates.
 02/15/05  sch    Fix for AWB when switching between snapshot and preview.
 02/15/05  dw     Disabled VFE output2 when configuring VFE for preview
                  Added check to ignore output2 frames received during preview
 02/14/05  mvd    Moved cam3a_mem_deallocate to process_stop, it's
                  rightful position.
 02/11/05  lrw    Add gain and full frame integration setting for Flash
 02/10/05  jk     Initialized text_type to VIDEO_ENG_TEXT_INPUT_NONE in
                  video_eng_init().
 02/09/05  sch    Fixed recording to preview freezing problem when camcorder
                  is accessed from camera.
 02/09/05  lrw    Fixed CR57224 Camera camera_set_encode_overlay()
                  dereferences NULL pointer
 02/09/05  drl/ap Added include of bio.h for 6275 build support.
 02/03/05  dw     Modified VideoEncFrameDoneBuf to include HW ME diagnostics
 02/03/05  sai    Added support for Panasonic 1.3 CMOS MN101E19A YCbCr
                  sensor capture and zoom under features :
                  FEATURE_CAMSENSOR_PANASONIC_MN101E19A and
                  FEATURE_PANASONIC_MN101E19A_ZOOM
 02/02/05  sch    Moving the burst parameters from Drivers to Service layers.
 02/02/05  sch    Power down sequence updates.
 01/27/05  jz     Fixed Preview Freeze problem for QVP, and QVP crash
                  problem when calling camera_enable_qvp
 01/26/05  mvd    Added preview rotation format and encode rotation
                  support.  Now the API set_dimensions does not rely on the
                  caller (UI) to properly set x,y dimensions, it
                  automatically rearranges x,y to suit the internal captured
                  buffer dimensions (which are always landscape).  Also,
                  preview rotation (how it is rotated by MDP) and encode
                  rotation (performed by JPEG encoder) are treated
                  by separate variables & code.
 01/25/05  sch    If VFE_CAMIF sub-sampling is used, adjust clipping
                  parameters accordingly for zooming.
 01/25/05  sch    Turning off PCLK turning down sequence as it is causing
                  VFE hangs. Zooming while video recording.
 01/25/05  sch    Don't send any more VFE commands if the last command sent
                  is IDLE. Process the stats messgaes if the last command is
                  not IDLE. Doing the power down/off sequence when IDLE
                  COMPLETE message is recieved to avoid VFE hanging.
                  Added camera_dsp_response for debugging purposes.
 01/25/05  kln    Removed encodeInfoFileSize structure and its definition,
                  Removed camera_jpeg_file_size_encode function and
                  camera_svcs_encode_picture_size_limit
                  Merged regular jpeg and file size jpeg paths till
                  camera_process_encode_picture() where different jpeg encode
                  functions are called.
 01/25/05  sch    Moved the camsensor_power_down (), camsensor_power_off ()
                  sequence to camera_process_stop ().
 01/25/05  sch    Shuts down any clock from the MSM going to the camera when
                  camera services is stopped.
 01/21/05  sch    Passing back the length of encoded image to the caller for
                  mem mode encoding.
 01/21/05  mvd    Renamed camsensor_config_params to camsensor_start - which
                  is more consistent with the naming scheme.
 01/20/05  sch    Zooming feature.
                  Single window Auto focus API
                  AHB burst factor fix.
 01/20/05  drl    Fixed VideoBuffersDeallocate call which was not being
                  called due to incorrect state variable test/modification.
 01/20/05  wh     Integrated IPL MB-packed conversion for video overlay
 01/19/05  drl    Fixed Feature flag problems, changed clock function, fixed
                  re-entry to app when recording QCIF problem.
 01/19/05  wh/dw  Video recording with overlay using the video_eng_frame API.
01/17/05  drl    Moved camera.c code to camera_svcs.c to facilitate
                  separation of application support of the general camera
                  services interface
 ------ Below is camera.c modification histor
 01/12/05  jz     Added QVideophone Support for 6550
 01/12/05  mvd    Added File Size Control support.  Changed args to
                  cam3a_set_aec function for Cam3a parameter moving.
 01/07/05  sch    Fix for wrong color when scaling factor is more than 16
                  during take picture.
 01/06/05  sch    Ported FEATURE_CAMERA_ENCODE_PROPERTIES and removed
                  CAMERA_PARM_SNAPSHOT_ENCODING_FORMAT.
 01/05/05  sch    Added CAMERA_PARM_SNAPSHOT_ENCODING_FORMAT to let the
                  upperlayer convey the encoding format information to
                  service layer using camera_set_parm().
 12/21/04  drl    In stop_record and pause operations, make sure the video_buffer
                  is not already empty.
 12/20/04  sch    Use of forth buffer in order to be absolutely true that the
                  VFEHW and the MDP are not using the same buffer at the same
                  time.
 12/14/04  sch    Changing VFE_YCbCr_OutputCositingType to VFE_JPEG_COSITING
                  during snapshot.
 12/10/04  drl    Changed N-Frame Delayed Encoding to N=1
 12/09/04  drl    Changed test for holding off of output2 frames from STOP_RECORD
                  to !NOT_STOPPING.  This allows any stopping scenario to
                  holdoff on adding frames to the frame buffer when we're trying
                  to shut down.
 12/09/04  drl/wh In camera_config_vfe, reset video_buffers in case there is
                  an error condition causing the code to reconfigure.
 12/08/04  drl/wh In CAMERA_FUN_VIDEO_HANDSHAKE, remove un-used YCbCrAddress
                  variable.
 12/08/04  drl    When stopping preview, make sure that there is not a pending
                  stop preview command.
 12/07/04  kln    Removed H1V2 nomenclature, added capability to preserve
                  original image size
 12/06/04  drl    camera_process_stop() and camera_process_record() cannot
                  start emptying the queue if BE is already encoding.
 12/05/04  wyh    For offline encoding CAMQDSP_MSG_CONFIG_COMPLETE must not
                  respond by issuing the VideoEnc_CONFIG command if the BE
                  is in IDLE. Instead it should issue the SLEEP command.
 12/03/04  mvd    Removing malloc and free of vfeStatDspOutput
 12/03/04  mvd    Removing rex_task_lock() & rex_task_free() calls for
                  3007 build
 12/03/04  mvd    Slamming over #72 changes for release.
 12/03/04  mvd    Fixes heap deallocate bug where vfeStatDspOutput was
                  getting freed too soon (before the VFE went to IDLE)
 12/02/04  ali    Added Transcoder API for enabling DSP
 12/01/04  drl    Added more camcorder offline support: severed VFE->encoding
                  connection.  Now, encoding one frame at a time - driven by ARM.
 11/30/04  mvd    Changes for new VFE/Camcorder image 0x1008.  Adds error
                  msgs and splits stat reporting.
 11/29/04  mvd    Fix to inital brightness setting.  Added code to ignore
                  CAMIF errors comming back from the Casio sensor during
                  take picture mode.
 11/29/04  mvd    Cleaned up several camera set/get parms.  Calling cam3a_
                  init() which clears memory for power-up state only.
                  These changes should fix: parm state & initial focus
                  position issues.
 11/23/04  mvd    Fixed AWB enable after selecting manual WB.  Now applying
                  vfeUpdate Color Cor and Convert. arrays all the time
                  to the first frame in preview/movie mode.
 11/18/04  mvd    Removed "stand-alone" functions (see JPEG services now)
                  Move cam3a_config() to camera_start so that 3A config
                  is only done once & hopefully makes 3A more stable &
                  reliable.  This also fixes AEC selection and AWB
                  init problems when going in/out of preview mode.
                  Added manual WB support.
 11/20/04  lrw    Add LED PMIC driver for LED flash on Kestrel
 11/19/04  wyh    Make sure not to send backend select command if videoEncState
                  is not INIT.
 11/08/04  drl    Added check in camera_process_stop to avoid going to idle
                  if we're already in idle.  This fixes a problem leaving album
                  mode within camcorder.
 10/28/04  sch    Don't encode thumbnail for images less than VGA size.
                  Decoder will take care of filling the album with main
                  image when the image size is less than VGA.
                  Output dimensions are rounded to the next upper multiple
                  of 16 value.
 10/28/04  sch    More checks for fragments support.
                  MDP implementation under FEATURE_MDP flag.
                  T_FFA_CROSSBOW_OV2620 for crossbow FFA with 180 degrees
                  rotation by default.
                  Added support for different image sizes during snapshot.
 10/27/04  drl    Added camcorder offline encoding support.
 10/26/04  mvd    Added "response" callback to new start_focus function so
                  that caller has callbacks both when function starts
                  executing and completes (two callbacks).  Also found
                  some bugs in calling back in manual focus mode.
 10/25/04  sch    Falling back to H2V1 format for snapshot and no rotation
                  as the JPEG encoder is very instable at this point of time.
                  It will be turned on once all the JPEG encoder issues are
                  resolved.
 10/23/04  lrw    Fixed the pre-flash for first redeye reduction
 10/23/04  sch    Fixed snapshot state machine bug.
                  Using H2V2 format for snapshot and 90 clock wise rotation
                  during encoding. Now the preview and encoded image will have
                  same orientation.
 10/22/04  mvd    Added fix for special effects settings within state
                  machine.
 10/22/04  drl    Added N-Frame buffering support for Camcorder.  Also added
                  fix for Mono and Sepia settings in camcorder.
 10/20/04  sch    Thumbnail display support during snapshot.
                  Moved AHB computation, VFE output format determination from
                  sensor driver to service layer.
                  added camera_snapshot_format, camera_raw_snapshot_format
 10/20/04  mvd    Added manual focus support via set/get parm and
                  call to camera_start_focus with manual mode setting.
                  CAMERA_PARM_FOCUS_STEP now supported
                  Added exposure mode support via CAMERA_PARM_
                  EXPOSURE_MODE.
                  In snapshot mode, now applying color convert &
                  correct values from preview mode.
 10/18/04  lrw    Add strobe_flash_redeye_reduction function call
 10/17/04  sch    Limiting the preview size to max. display dimensions.
                  Raw Snapshot to Preview mode fix.
 10/17/04  sch    Code cleanup.
 10/16/04  lrw    check strobe_flash_is_ready when takes picture
 10/15/04  sch    All fragments for main image should contain valid buffer
                  pointers even though less than maximum number of fragments
                  are used to avoid VFE crash incase there is any overflow.
 10/07/04  mvd    Added state information to color conversion matrix changes
                  so that special effects are applied properly.
 10/14/04  sch    TIMETEST updates for snapshot.
 10/12/04  sch    Fix related to Raw snapshot.
 10/12/04  sch    Added FEATURE_CAMERA Flag.
 10/11/04  sch    TIMETEST updates for snapshot.
 10/11/04  dle    Modified to use same camera.h file.
 10/09/04  lrw    Ignore Epoch interrupt after the first one
 10/06/04  sj     Renamed #include video_encaux.h to video_enc_aux.h
                  for the camcorder folks.
 10/06/04  sch    Fix for out of the sequence of messages for thumbnail
                  and main image.
 10/06/04  lrw    Add cam3a_flash_needed() and discardFirstFrame
 10/05/04  sch    Fix for false colors in preview 48 x 80 mode.
 10/05/04  mvd    Renamed #include video_enc_aux.h to video_encaux.h
                  for the camcorder folks.
 10/05/04  mvd    Renamed #include vfe_if.h to vfe.h
 09/30/04  mvd    Accomodated chromatix name changes.
 09/24/04  lrw    Added camera_fire_flash setting, leave out
                  discard_first_frame and camif_flash_needed for enhancement
 09/23/04  mvd    Added support for get/set parms: BRIGHTNESS and
                  SPECIAL EFFECTS.
 09/21/04  pdo    Added EXIF function calls to set EXIF parameters
 09/21/04  sch    Added Raw Snapshot support.
 09/16/04  sch    Picture buffers are dynamically allocated as static
                  allocation is making NOR builds fail to load.
 09/16/04  lrw    LED and Strobe Flash related addition/removal. Added sections
                  are protected in FEATURE_WHITE_LED_FLASH or
                  FEATURE_STROBE_FLASH.
 09/16/04  sch    MDP related additions, for screen clearing.
 09/16/04  sch    IDLE the VFE image when switching modes between
                  Snapshot and Preview before reconfiguring it.
 09/09/04  mvd    Fixes to AF / AWB state machine.  Dynamically allocating
                  mem instead of declaring.
 09/08/04  wh     Video Config, Start and Selection command pass argument by
                  reference.
 09/08/04  sch    Added Snapshot, RawSnapshot related changes.
 09/06/04  mvd    Made cleaner brightness code. Accomodated changes for
                  cam3a.c
 09/01/04  mvd    Fixes for AF API & state machine.  Added brightness
                  support.  Changed "autoctrl" to "cam3a"
 08/31/04  mvd    Change over - from "camif" to "camsensor" driver.
                  Removed MVD_CAMSENSOR params - initally working.
 08/30/04  sch    Support for new end of frame ack format.
                  the ack message contains buffer pointer that VFE
                  is going to use.
 08/24/03  dle    Removed clk_unexpected_enable(). Can only be called in ISR.
                  Changed camera_qdsp_cb() response param to pointer.
 08/20/04  mvd    Advanced MVD_CAMSENSOR progress, no functional change.
                  (this features is undefined now)
 08/19/04  mvd    Added TLMM config code to reset and enable camera for FFA
                  mode.  Also added temporary #defed code under
                  MVD_CAMSENSOR to smooth transition to new "camsensor"
                  driver (as a replacement for "camif")
 08/13/04  sch    Setting the Video frame as Background.
 08/09/04  sch    Using MDP interface for QVGA display.
 07/27/04  mvd    For Ncai.  Merged in Parms to control antibanding,
                  thumbnail size, & thumbnail quality.
 07/26/04  mvd    Added preliminary auto-focus stat handling code.  Added
                  time test code (using a scope).  Moved 3A init to
                  camera_start() so that is happens upon entering the
                  app every time.
 07/07/04  drl    Pickup Video Support.  Used #235 as baseline.
 07/01/04  mvd    Removed awbRGainCtrl - need for this code has ended now
                  that the color problem has been corrected.
                  Removed MVD_AEC_DEBUG.
 06/24/04  dle    Updated call to ipl_crop_ycrcb420_to_rgb565.
 06/23/04  mvd    Moved gamma tablet to proper location in Casio driver
                  file.  Added variable called "awbRGainCtrl" to control
                  the redishness in the AWB results.  Set to 0.85
                  initially.
 06/23/04  mvd    Fixed silly #def bug.
 06/23/04  mvd    Debugged & tuned auto white-balance implementation.
                  Implemented Gamma correction.  Enabled VFE Update for
                  color correction (AWB).  AEC output now placed in
                  "camera_vfe_luma" global variable, regardless of
                  algorithm used to produce it.
 06/16/04  drl    Changed stop_preview to shutdown dsp and release display
                  buffers.
 06/09/04  mvd    Added support for AEC and AWB statistic gathering
 04/18/04  drl    Adaption for 6550
============================================================================*/



/*============================================================================
                        INCLUDE FILES
============================================================================*/
#include "assert.h"
#include "camera.h"
#include "camerai.h"
#include "camera_svcs.h"
#include "mobile.h"
#include "AEEStdLib.h"
#ifdef FEATURE_PNG_ENCODER
#include "pnge.h"
#endif /* FEATURE_PNG_ENCODER */
#ifdef FEATURE_CAMERA
#include CLKRGM_H
#include "bio.h"
#include "msm.h"
#include "msmhwio.h"
#include "camsensor.h"
#include "camctrl.h"
#include "camera_3a_shared.h"
#include "disp.h"
#include "event.h"
#include "exif.h"
#include "graphi.h"
#include "graph.h"
#include "ipl.h"
#include "msg.h"
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
#include <stdlib.h>
#ifdef CAMERA_USES_LIBSTD
#include "AEEstd.h"
#endif /* CAMERA_USES_LIBSTD */
#include "task.h"
#include "qcamraw.h"
#include "OEMFeatures.h"
#ifdef FEATURE_VIDEO_ENCODE
#include "clk.h"
#include "video_buffers.h"
#include "video_enc_dsp_if.h"
#include "videoeng.h"
#include "video_enc_aux.h"
#include "video_enc.h"
#endif /* FEATURE_VIDEO_ENCODE */
#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
#include "camcorder_svcs.h"
#endif //FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
#include "vfe.h"
#include "jpege.h"
#ifdef FEATURE_CAMERA_HJR
#include "camera_hjr.h"
#endif /* FEATURE_CAMERA_HJR */
#include "camera_drv.h"

/* Luma Processing Module */
#ifdef FEATURE_CAMERA_LPM
#error code not present
#endif /* FEATURE_CAMERA_LPM */

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
#include "ycbcrdownsizer.h"
#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/

#ifdef FEATURE_VIDEO_ENCODE
#define MSG_CAMQDSPIO(x,a,b,c)
#endif
//MSG_ERROR(x,a,b,c)

/* To obtain hardware version (chip id) or
 * partnum version (for Jaguar/Saber)
 */
#include "hw.h"

#ifndef FEATURE_WHITE_LED_FLASH
define FEATURE_WHITE_LED_FLASH
#endif

#ifdef FEATURE_QVPHONE
#include "qvp_msg_venc.h"

#ifdef FEATURE_QVP_OFFLINE_VIDEO_ENCODING
#error code not present
#endif  //#ifdef FEATURE_QVP_OFFLINE_VIDEO_ENCODING
extern void CAMQDSP_qvp_isr( void);
#endif /* FEATURE_QVPHONE */

#ifdef FEATURE_STROBE_FLASH
#include "strobe_flash.h"
#ifdef FEATURE_WHITE_LED_FLASH
#undef FEATURE_WHITE_LED_FLASH
#endif /* FEATURE_WHITE_LED_FLASH */
#ifdef FEATURE_PMIC_FLASH_LED_DRIVER
#undef FEATURE_PMIC_FLASH_LED_DRIVER
#endif /* FEATURE_PMIC_FLASH_LED_DRIVER */
#endif /* FEATURE_STROBE_FLASH */

/* Make sure we don't have LCD_DIRECT_MODE if MDP is not enabled */
#ifndef FEATURE_MDP
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
#undef FEATURE_CAMERA_LCD_DIRECT_MODE
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
#endif /* nFEATURE_MDP */

#ifdef FEATURE_PMIC_FLASH_LED_DRIVER
#include "pm.h"  /* Using PMIC3 */
#endif /* FEATURE_PMIC_FLASH_LED_DRIVER */

#ifdef FEATURE_MDP
#include "mdp_drv_api.h"
#endif /* FEATURE_MDP */

#ifdef FEATURE_CAMERA_MOBICAT_PREVIEW
#include "mobicat_preview.h"
#endif //FEATURE_CAMERA_MOBICAT_PREVIEW

#ifdef FEATURE_PMIC_FLASH_LED_DRIVER
#define PMIC_FLASH_CURRENT_HIGH 280
#define PMIC_FLASH_CURRENT_DOWN 0
#endif /* FEATURE_PMIC_FLASH_LED_DRIVER */

#define MSG_CAMERA_LATENCY_PROFILING(x,a,b,c) MSG_HIGH(x,a,b,c)

/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/
#define MSG_CAMERAPBUF(x,a,b,c) MSG_LOW(x,a,b,c)
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
#define CAMERA_ICON_ARRAY_SIZE 100
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */

#ifndef TIMETEST
/* #define CAMERA_TIMETEST
   boolean logic_timetest = FALSE; */
#endif /* TIMETEST */

#define CAMERA_MIN_CONTRAST    0
#define CAMERA_DEF_CONTRAST    5
#define CAMERA_MAX_CONTRAST    10


#define CAMERA_MIN_BRIGHTNESS 0
#define CAMERA_DEF_BRIGHTNESS 5
#define CAMERA_MAX_BRIGHTNESS 10

#define MP4E_FRAME_BUF_SIZE         8

/* AHB burst parameters */
#define CAMERA_AHB_BURST_LEN   16
#define CAMERA_AHB_BURST_NUM   1

#define CAMERA_AHB_BURST_LEN_8  8
#define CAMERA_AHB_BURST_LEN_16 16

/* AHB burst length for raw snapshot only */
#define CAMERA_RAW_SNAPSHOT_AHB_BURST_LEN  8

#define CEILING16(X) (((X) + 0x000F) & 0xFFF0)
/*
#define CEILING8(X)  (((X) + 0x0007) & 0xFFF8)
*/
#define CEILING4(X)  (((X) + 0x0003) & 0xFFFC)
#define CEILING2(X)  (((X) + 0x0001) & 0xFFFE)
#define FLOOR16(X) ((X) & 0xFFF0)
/*
#define FLOOR8(X)  ((X) & 0xFFF8)
*/
#define FLOOR4(X)  ((X) & 0xFFFC)
#define FLOOR2(X)  ((X) & 0xFFFE)

/* Return the smallest integral value not less than x/y */
#define CAMCEIL(x,y)  ((x+(y-1))/y)
#define OFFSETOF(s,m) ((uint32)&(((s *)0)->m))

/* LCD specific constants */

/* For QVGA LCD in portrait mode */
/* Can be overwritten by custcamera.h file if defined there */
#ifndef CAMERA_LCD_WIDTH
#define CAMERA_LCD_WIDTH          240
#endif /* nCAMERA_LCD_WIDTH */
#ifndef CAMERA_LCD_HEIGHT
#define CAMERA_LCD_HEIGHT         320
#endif /* nCAMERA_LCD_HEIGHT */

#define CAMERA_QVGA_WIDTH          320
#define CAMERA_QVGA_HEIGHT         240

#ifdef CAMERA_USES_OEM_HEAP
#define CAM_ALLOC_NO_ZMEM (0x0)
#else
#define CAM_ALLOC_NO_ZMEM (0x80000001)
#endif

/* For generating 30bit VFE skip patterns / sensor frame rates for video */
#define CAMERA_VFE_SKIP_PATTERN_WIDTH      30
#define CAMERA_VIDEO_MAX_SENSOR_FRAME_RATE 15

#define CAMERA_BUF_ALIGN 64

// set to the maximum desired zoom steps you want UI to handle.
// When this number is high, smooth zoom can be implemented in UI.
// Setting this number to be greater than CAMERA_ZOOM_TABLE_MAX has no benefit.
// Make sure it is not set to 0 or 1.
// FYI: If using 3MP sensor and want to take stamp size snapshots, you
// will be able to have 94 zoom steps. This means, if you set
// CAMERA_MAX_ZOOM_STEPS to be 182, you actually get 94 zoom steps. In
// otherwords, if you set CAMERA_MAX_ZOOM_STEPS to higher than 94, you get the
// same effect.
#define CAMERA_MAX_ZOOM_STEPS   182

// This zoom table was generated by system team.
#define CAMERA_ZOOM_TABLE_MAX   182
static const uint32 camera_zoom_table[CAMERA_ZOOM_TABLE_MAX] =
{
  4096,  4191,  4289,  4389,  4492,
  4597,  4705,  4815,  4927,  5042,
  5160,  5281,  5404,  5531,  5660,
  5792,  5928,  6066,  6208,  6353,
  6501,  6653,  6809,  6968,  7131,
  7298,  7468,  7643,  7822,  8004,
  8192,  8383,  8579,  8779,  8985,
  9195,  9410,  9630,  9855,  10085,
  10321,  10562,  10809,  11062,  11320,
  11585,  11856,  12133,  12416,  12706,
  13003,  13307,  13619,  13937,  14263,
  14596,  14937,  15286,  15644,  16009,
  16384,  16766,  17158,  17559,  17970,
  18390,  18820,  19260,  19710,  20171,
  20642,  21125,  21618,  22124,  22641,
  23170,  23712,  24266,  24833,  25413,
  26007,  26615,  27238,  27874,  28526,
  29192,  29875,  30573,  31288,  32019,
  32768,  33533,  34317,  35119,  35940,
  36780,  37640,  38520,  39420,  40342,
  41285,  42250,  43237,  44248,  45282,
  46340,  47424,  48532,  49666,  50827,
  52015,  53231,  54476,  55749,  57052,
  58385,  59750,  61147,  62576,  64039,
  65536,  67067,  68635,  70239,  71881,
  73561,  75281,  77040,  78841,  80684,
  82570,  84500,  86475,  88496,  90565,
  92681,  94848,  97065,  99334,  101655,
  104031,  106463,  108952,  111498,  114104,
  116771,  119501,  122294,  125152,  128078,
  131072,  134135,  137270,  140479,  143763,
  147123,  150562,  154081,  157682,  161368,
  165140,  169000,  172950,  176993,  181130,
  185363,  189696,  194130,  198668,  203311,
  208063,  212927,  217904,  222997,  228209,
  233543,  239002,  244589,  250305,  256156,
  262144,  999999
};

#ifdef FEATURE_CAMERA_ZOOM_TABLE2
#define CAMERA_MIN_ZOOM_STEPS 14
static const uint32 *camera_zoom_table_ptr = camera_zoom_table;
static const uint32 camera_zoom_table2[CAMERA_ZOOM_TABLE_MAX] =
{
  4096, 4144, 4192, 4240,
  4290, 4340, 4390, 4441,
  4493, 4545, 4598, 4651,
  4705, 4760, 4815, 4871,
  4928, 4985, 5043, 5101,
  5161, 5221, 5281, 5343,
  5405, 5468, 5531, 5595,
  5660, 5726, 5793, 5860,
  5928, 5997, 6067, 6137,
  6208, 6281, 6353, 6427,
  6502, 6578, 6654, 6731,
  6810, 6889, 6969, 7050,
  7132, 7214, 7298, 7383,
  7469, 7556, 7643, 7732,
  7822, 7913, 8005, 8098,
  8192, 8287, 8383, 8481,
  8579, 8679, 8780, 8882,
  8985, 9090, 9195, 9302,
  9410, 9519, 9630, 9742,
  9855, 9970, 10086, 10203,
  10321, 10441, 10563, 10685,
  10809, 10935, 11062, 11191,
  11321, 11452, 11585, 11720,
  11856, 11994, 12133, 12274,
  12417, 12561, 12707, 12855,
  13004, 13155, 13308, 13463,
  13619, 13777, 13937, 14099,
  14263, 14429, 14596, 14766,
  14938, 15111, 15287, 15464,
  15644, 15826, 16010, 16196,
  16384, 16574, 16767, 16962,
  17159, 17358, 17560, 17764,
  17970, 18179, 18390, 18604,
  18820, 19039, 19260, 19484,
  19710, 19939, 20171, 20405,
  20643, 20882, 21125, 21371,
  21619, 21870, 22124, 22381,
  22641, 22904, 23170, 23440,
  23712, 23988, 24266, 24548,
  24834, 25122, 25414, 25709,
  26008, 26310, 26616, 26925,
  27238, 27554, 27875, 28199,
  28526, 28858, 29193, 29532,
  29875, 30222, 30574, 30929,
  31288, 31652, 32020, 32392,
  32768, 999999
};
#endif /* FEATURE_CAMERA_ZOOM_TABLE2 */

// this table holds values used to bump the zoom table lookup entry a given
// UI zoom step. For example, if we have computed that we need 61 zoom steps to
// go from 2MP down to 320x240, but we want the UI to show 10 steps, then
// each UI zoom step is really 6 hops in the zoom table above. But
// by 9*6 = 54, not 61. Given 61-54 = 7, we generate a table below to handle
// exact 61 step zoom:
//
// 0 1 2 2 3 3 4 5 6 7
static uint8 camera_zoom_table_bump[CAMERA_ZOOM_TABLE_MAX];


#ifdef FEATURE_NEW_SLEEP_API
sleep_okts_handle camera_sleep_okts_handle;
boolean camera_sleep_handle_register=FALSE;
#endif

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

/* No support for BURST MODE with downsizer */
/* This is applicable for the images greater than QVGA size*/
#if defined FEATURE_CAMERA_BURST_MODE && (defined QDSP_MODULE_DS05_DEFINED || defined QDSP_MODULE_SA_DS05_DEFINED)
#error "Multishot cannot be supported with downsizer."
#endif /* (QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED) && FEATURE_CAMERA_BURST_MODE */

/*============================================================================
                        LOCAL DATA TYPES
============================================================================*/
/* Delay AEC/AWB stats */
#define ZOOM_STATS_DELAY         3
#define AEC_STATS_PROC_DELAY     0
#define UPDATE_CMPLT_STATS_DELAY 0
#define CAMERA_PIXELS_FOR_DEMOSAIC 12
#define CAMERA_LINES_FOR_DEMOSAIC 8

/*============================================================================
                     GLOBAL DATA DEFINITIONS
============================================================================*/

camera_frame_type          snapshot_frame;
#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
uint32 *downsizer_out_luma_buffer = NULL;
uint32 *downsizer_out_chroma_buffer = NULL;
#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/
camera_preview_mode_type   preview_mode;

/* When set to true, the VFE is reconfigured */
//boolean                    camera_reconfigure;
/* When set to TRUE, the VFE configuration is updated */
boolean                    camera_update_vfe;

boolean                    camera_ok_to_sleep;

#ifdef FEATURE_CAMERA_HJR
extern HJR_struct HJR_structure;
#endif /* FEATURE_CAMERA_HJR */

/* To deal with MDP issue where padding is needed when sending a
   none multiple of 16 image to it                             */
static uint32 camera_preview_pad_for_mdp = 0;

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

/*lint -save */
/*lint -e46 uint32 as bit field */
/*lint -e64 Type mismatch */
/*lint -e413 offset of macro measure offset from start of struct */
/*lint -e611 cast to generic callback funtion pointers */
/*lint -e641 conversion xxx to int */
/*lint -e701 value are defined such as upper bit 16 | lower 16 bit */
/*lint -e702 10 bit black level to 8 bit */
/*lint -e704 shift right ok */
/*lint -e818 Could be pointer to const */
/*lint -e825 control flows into case/default without -fallthrough comment */
/*lint -e826 Suspicious pointer-to-pointer conversion (area too small) */
/*lint -e834 Operator '?' followed by operator '?' is confusing */

/*============================================================================
                     LOCAL DATA DEFINITIONS
============================================================================*/
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
static int camera_lcd_direct_mode;
static ipl_rect_type camera_lcd_direct_rect;
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
/* The BLT window need to be calculated only when preview,
 * start record or take picture started */
boolean camera_blt_ext_recalculate_window = TRUE;
static uint32 ui_resolution[2];
static CAMQDSP_IllegalCommandMessageType illegalCmdMessage;
static CAMQDSP_AFStatsMessageType        afStatsMessage;
static CAMQDSP_AE_AWBStatsMessageType    aeAwbStatsMessage;
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */
typedef enum
{
  CAMERA_TERMINATE_CLOCK_STOPPED,
  CAMERA_TERMINATE_CLOCK_RUNNING,
  CAMERA_TERMINATE_CLOCK_EXPIRED
} camera_terminate_clk_state_type;

static camera_terminate_clk_state_type camera_terminate_clk_state = CAMERA_TERMINATE_CLOCK_STOPPED;
static clk_cb_type camera_terminate_clk;
static clk_cb_type camera_service_unavailable_clk;
static boolean camera_service_unavailable = FALSE;

static uint16                            frameDoneMessage [MP4E_FRAME_BUF_SIZE];
VFE_EOF_Ack2Type                        ackOutput2;

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
static ycbcr_downsizer_cmd_info_type yds_cmd;
/*Pixel count is defined for Y data size for the output image*/
static uint32 out_pixel_cnt = 0;
#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/

static camera_handle_type                camera_handle;

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/

#ifdef FEATURE_MDP
static MDPIMG mdpImg;
static MDPIMG mdpImg_frame;
static uint32 mdpImgId = NULL;
static uint32 mdpImgId_frame = NULL;
#endif /* FEATURE_MDP */

static boolean camera_initialized = FALSE;
static boolean camera_timer_up    = FALSE;

/* A clock for timed wait */
static clk_cb_type camera_wait_clk;
static rex_crit_sect_type camera_wait_clk_cs;
static rex_tcb_type *camera_timed_wait_tcb;
//#define HANDLE_CAMIF_ERROR
/* VFE generates CAMIF_ERROR when
 * 1. the active HSYNC cycles is different between lines in a frame
 * 2. the number of HSYNCs in a frame is different than programmed in the
 *    CAMIF_FRAME_CONFIG register
 * 3. the VSYNC and HSYNC lines falling on the same cycle. This is fixed in
 *    later MSM. */
#ifdef HANDLE_CAMIF_ERROR
/* When VFE reports CAMIF_ERROR, just set an indication. Do not retry
 * until end of frame. Immediat retry may not help cause. If no frame
 * is received after this, a timer can be used to detect. */
static boolean                      camera_camif_error_clk_running = FALSE;
static clk_cb_type                  camera_camif_error_clk;
#endif /* HANDLE_CAMIF_ERROR */

static disp_info_type        camera_disp_info;
camera_state_type            camera_state  = CAMERA_STATE_INIT;
camera_cb_f_type      camera_app_cb = NULL;
static rex_tcb_type          *camera_tcb = NULL;

// this struct holds the min and max points in the zoom table
// we want to travel between (inclusive) to get our zoom factor
static camera_parm_info_type camera_parm_zoom_table;

static camera_parm_info_type camera_parm_zoom;
static camera_parm_info_type camera_parm_preview_rotation;
static camera_parm_info_type camera_parm_encode_rotation;
static camera_parm_info_type camera_parm_fps;
/* Flash is on/off or auto, control, both get and set. */
static camera_parm_info_type camera_parm_flash;
/* Flash is ready or not, query, get only */
static camera_parm_info_type camera_parm_flash_state;
static camera_parm_info_type camera_parm_reflect;
static camera_parm_info_type camera_parm_preview_mode;
static camera_parm_info_type camera_parm_antibanding;
#ifdef FEATURE_STROBE_FLASH
static camera_parm_info_type camera_parm_red_eye_reduction;
#endif /*FEATURE_STROBE_FLASH*/
static camera_parm_info_type camera_parm_brightness;
static camera_parm_info_type camera_parm_contrast;
static camera_parm_info_type camera_parm_special_effects;
static camera_parm_info_type camera_parm_white_balance;
camera_parm_info_type camera_parm_manual_focus;
static camera_parm_info_type camera_parm_auto_exposure_mode;
static camera_parm_info_type camera_parm_exposure_compensation;
static camera_parm_info_type camera_parm_preview_fps;
static camera_parm_info_type camera_parm_iso;
static camera_parm_info_type camera_parm_af_mode;
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /*FEATURE_CAMERA_BURST_MODE */

/* camera_dx and camera_dy are in sensor orientation */
uint16               camera_dx = 0;
uint16               camera_dy = 0;

static uint32               resize_factor = 0;

/* Can be overwritten by custcamera.h if defined there */
#ifdef T_QSC1110 // Gemsea Add
	#ifndef CAMERA_PREVIEW_ROTATION
		#define CAMERA_PREVIEW_ROTATION 0
	#endif /* nCAMERA_PREVIEW_ROTATION */
#else
	#ifndef CAMERA_PREVIEW_ROTATION
		#define CAMERA_PREVIEW_ROTATION 90
	#endif /* nCAMERA_PREVIEW_ROTATION */
#endif
static uint16  camera_default_preview_rotation;
#if defined(FEATURE_CAMERA_LCD_DIRECT_MODE) || !defined(FEATURE_MDP)
static ipl_rotate90_type camera_default_preview_rotation_ipl;
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE || nFEATURE_MDP */

/* Indicating that default JPEG images are encoded in portrait
   orientation (rotated  counter-clockwise by 90 degrees) */
static uint16  camera_default_jpeg_encode_rotation = 90;
static uint16               video_encode_rotation;
static uint16               orig_picture_dx = 0;
static uint16               orig_picture_dy = 0;

static camera_position_type camera_position;

static camera_orientation_type camera_default_orientation = CAMERA_ORIENTATION_PORTRAIT;
static boolean                 camera_odd_rotation;

static ipl_reflect_type        camera_reflect_ipl;
static qcamrawReflectType      camera_reflect;

static uint32 offline_read_wait = 0;

/* Preview decimation factor */
static uint32  camera_preview_decimation;
static uint32  camera_min_decimation;
static uint32  camera_decimation_factor;


static uint32  zoom_step_size;
static uint32  camera_preview_buffer_size = 0;
static uint32  camera_encode_buffer_size  = 0;
boolean camera_take_images = FALSE;

/* Variables needed for EXIF */
static void *camera_description = 0;
static uint16 camera_description_count = 0;
static void *camera_user_comment = 0;
static uint16 camera_user_comment_count = 0;
static void *camera_gps_area_information = 0;
static uint16 camera_gps_area_information_count = 0;
static void* camera_make_information = 0;
static uint16 camera_make_information_count = 0;
static void* camera_model_information = 0;
static uint16 camera_model_information_count = 0;
static VFE_SkipBitMaskType runningOutput2SkipPattern = 0;

/* New preview buffer scheme */
#ifdef FEATURE_VERSION_K212_HUALU//yangdecai modify 
#define CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_VFE    2
#define CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_DISPLAY    1
#else
#define CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_VFE     2
#define CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_DISPLAY     1
#endif
#define CAMERA_NUM_OF_PREVIEW_BUFFERS \
   (CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_VFE + \
    CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_DISPLAY)
typedef enum
{
  AVAILABLE = 0,
  /* Means buffer is delivered to application. */
  BUSY_WITH_APP,
  /* Means buffer in the queue to be displayed */
  BUSY_WITH_DISPLAY,
  /* Means buffer sent to VFE */
  BUSY_WITH_VFE
} camera_preview_buffer_status_type;

typedef struct
{
  byte                               *buf_ptr;
  camera_preview_buffer_status_type  status;
} camera_preview_buffer_type;

typedef struct
{
  camera_preview_buffer_type      buffers[CAMERA_NUM_OF_PREVIEW_BUFFERS];
  /* Incremented when buffer is delivered to APP, decremented when
   * become available, either in BUSY_WITH_APP or BUSY_WITH_DISPLAY */
  uint32                          num_buffers_with_display;
  /* Incremented when buffer is delivered to VFE. Decremented when
   * it is received from VFE */
  uint32                          num_buffers_with_VFE;
} camera_preview_buffers_info_type;

static camera_preview_buffers_info_type      camera_preview_buffers;

#define CAMERA_INVALID_BUF_INDEX 10
static uint32 camera_app_buf_index[2];

#ifdef FEATURE_MDP
/* When displaying picture, once picture buffer is deleted, the display on
 * LCD would become clobbered. This solution is to keep a copy. The copy
 * is deleted when background layer is de-registered. */
static byte   *picture_preview_copy = NULL;
#endif /* FEATURE_MDP */

LOCAL uint8 *ycbcr_image_info;

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /*FEATURE_CAMERA_BURST_MODE*/

static uint32 camera_capture_delay = 0; /* in ms */

#ifdef FEATURE_QVPHONE
static byte   *qvp_enc_buffer0 = NULL;
static byte   *qvp_enc_buffer1 = NULL;
static byte   *qvp_enc_buffer2 = NULL;
static byte   *qvp_enc_buffer3 = NULL;
static int output2_ack = 0;
#endif /* FEATURE_QVPHONE */

static uint8 *raw_snapshot_buf = NULL;
static uint8 *snapshot_luma_buf = NULL;
static uint8 *snapshot_chroma_buf = NULL;
static uint8 *thumbnail_luma_buf =NULL;
static uint8 *thumbnail_chroma_buf=NULL;

static qcamrawHeaderType my_main_image_header;
static qcamrawHeaderType my_thumbnail_image_header;
static qcamrawHeaderType my_raw_image_header;

#ifdef FEATURE_PNG_ENCODER
static qcamrawHeaderType my_png_image_header;
#endif /* FEATURE_PNG_ENCODER */

LOCAL qcamrawHeaderType my_ycbcr_image_header;

static camera_parm_type  camera_parm_id;
boolean           camera_odd_encode_rotation;
static camera_encode_properties_type camera_encode_properties;

camera_func_type   camera_func = CAMERA_FUNC_MAX;
#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE

static int16              encoder_acked;
#endif /* FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE */
/* camera_preview_dx and camera_preview_dy specifies the preview
 * display dimensions. They are always in landscape */
uint32             camera_preview_dx   = 0;
uint32             camera_preview_dy   = 0;

/* Input dimension to VFE */
static uint32             camera_vfe_input_dx     = 0;
static uint32             camera_vfe_input_dy     = 0;

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
/* Used to store the icon array */
static ipl_icon_type   **camera_icons   = NULL;
static ipl_icon_type   **blt_icons      = NULL;
static uint32            camera_icon_dx = 0;
static uint32            camera_icon_dy = 0;
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
static ipl_image_type   *preview_overlay = NULL;
static ipl_image_type    ui_overlay;
typedef struct
{
  /* We need to pad pad_lines at the top and bottom, pad_pixels on the left
   * for every lines, and then pad_pixels to the end of last line.
   * This is because we need to be able to index each of the four corners
   * when we merge with the main image, due to 4 different rotations.
   * After rotation, the top left corner must be a real pixel, not padded.
   * For example, top right corner becomes top left corner after CCL90 degree
   * rotation, and we need padding at the bottom and right, hence the extra
   * pad_pixels at the end of last line.
   * Another example, bottom right corner becomes top left corner after 180
   * degree rotation, and we need padding on the top and left. */
  /* buffer size */
  uint32          size;
  /* Number of pixels to pad to be mod 16 */
  uint16          pad_pixels;
  /* Number of lines to pad to be mod 16 */
  uint16          pad_lines;
  /* Number of pixels after padding */
  uint16          padded_width;
  /* Number of lines after padding */
  uint16          padded_height;
  /* The overlay buffer, with paddings */
  uint8          *imgPtr;
  /* The imgPtr will be adjusted before giving to IPL, based on rotation */
  ipl_image_type  ipl_image;
} camera_encode_overlay_type;
static camera_encode_overlay_type *encode_overlay;
static camera_encode_overlay_type *thumbnail_overlay;
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
static ipl_image_type   *icon_only_overlay = NULL;
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
static uint16            camera_transparent_color;
static exif_info_type    camera_exif_info;
void             *camera_app_data = 0;
static boolean           camera_frame_callback_enabled = TRUE;
static boolean           camera_abort_picture = FALSE;
camsensor_op_mode_type      camera_camsensor_op_mode;
CAMQDSP_VFE_CommandType camera_dsp_command = CAMQDSP_CMD_MAX;
static CAMQDSP_ResponseMsgType camera_dsp_response = CAMQDSP_SERVICE_UNAVAILABLE;
static boolean           scale_preview_overlay = FALSE;
static uint32  corrupt_count = 0;
static uint32  illegal_count = 0;
static uint32  efs_fail_count = 0;
uint8   preview_red_average = 128;
uint8   preview_green_average = 128;
uint8   preview_blue_average = 128;
uint32        preview_gain;

/* Luma Processing Module global variables */
boolean camera_asf_enable = TRUE;  /*  enable or disable adaptive spatial filter */
boolean camera_cs_enable  = TRUE;  /* enable or disable chroma suppression */
boolean camera_la_enable  = TRUE;  /* enable or disable luma adaptation */
#if defined QDSP_MODULE_DS05_DEFINED || \
    defined QDSP_MODULE_SA_DS05_DEFINED || \
    defined FEATURE_CAMERA_LPM
static int32  session_id = 0;
#endif

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
/* select vignette correction type */
camera_vc_mode_type camera_vc_mode = CAMERA_BAYER_VIGNETTE_CORRECTION;

static exif_attributes_type camera_exif_attributes;
static uint8 * camera_svcs_raw_snapshot_copy = NULL;

/********************************************************/

static uint32 qcamraw_header_size = 0;

camera_dsp_state_type camera_dsp_state = DSP_DISABLED;
boolean camera_stopping = FALSE;
boolean camera_stopping_record = FALSE;
/* Store Downscaler factor for Preview and Snasphot */
#if defined QDSP_MODULE_VFE25_DEFINED && !defined FEATURE_CAMERA_YCBCR_ONLY
float preview_downscale_factor = 1.0;
float snapshot_downscale_factor = 1.0;
#endif /* QDSP_MODULE_VFE25_DEFINED && ! FEATURE_CAMERA_YCBCR_ONLY */
static camera_cb_type        camera_stopping_cb;

static fs_handle_type  camera_fs_handle = NULL;
static fs_rsp_msg_type fs_rsp;

#ifdef ANY_FLASH
static boolean camera_flash_on;
static boolean camera_flash_auto;
static boolean camera_fire_flash;
static boolean camera_flash_fired;
#endif /* ANY_FLASH */

JPEGENC_encodeSpecType  encodeInfo;   /* encode specification   */

static CAMQDSP_VideoEncStateType videoEncoderState;

#ifdef CAMERA_LOGGING
uint32 log_index = 0;
typedef struct
{
  camera_log_type         type;
  camera_func_type        func;
  CAMQDSP_ResponseMsgType rsp;
  camera_log_info_type    info;
  uint32                  line;
} log_entry_type;

#define LOG_SIZE 128
log_entry_type camera_logs [LOG_SIZE];
#endif /* CAMERA_LOGGING */

#define NUM_OF_SENSORS_ON_PHONE  2

#ifdef FEATURE_CAMERA_MULTI_SENSOR
uint16 camera_number_of_sensors = 0;
/* camera_sensors[] is to allow the application to get sensor info
 * before camera_start() gets called. */
camera_info_type camera_sensors[NUM_OF_SENSORS_ON_PHONE] = {0};
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

uint8 camera_asi = 0;
// May need an array of these
camsensor_static_params_type camsensor_static_params[NUM_OF_SENSORS_ON_PHONE];

/* Thumbnail width and height */
#define THUMBNAIL_MIN_DIMENSION 48
#define THUMBNAIL_MAX_DIMENSION 320
#define THUMBNAIL_MIN_QUALITY   1
#define THUMBNAIL_MAX_QUALITY   100
static uint32 thumbnail_width, ui_thumbnail_width;
static uint32 thumbnail_height, ui_thumbnail_height;
static uint32 thumbnail_quality;

/* VFE output1 format for preview */
static camera_format_type camera_preview_format = CAMERA_YCBCR_4_2_0;

/* VFE output1 and output2 format for snapshot. VFE doesn't
 * allow to specify different formats for output1 and output2.
 */
static camera_format_type camera_snapshot_format = CAMERA_H2V2;
#ifdef T_QSC1110 // Gemsea Add
static JPEGENC_OrientType camera_jpeg_encode_rotation_format = JPEGENC_NO_ROT;
#else
static JPEGENC_OrientType camera_jpeg_encode_rotation_format = JPEGENC_CCLK270;
#endif

static int dsp_camif_error_count = 0;



static camera_ret_code_type camera_svcs_blt_ex
(
  camera_frame_type *frame,
  /* The destination RGB buffer dimension and pointer. Pointer indeces
   * beginning of data buffer. */
  uint16             dst_dx,
  uint16             dst_dy,
  void               *dst_ptr,
  /* Rectangle of the destination used for update */
  uint16             x,
  uint16             y,
  uint16             dx,
  uint16             dy,
  camera_format_type format
);
#ifdef FEATURE_QVPHONE
boolean camera_qvp_enabled;
static boolean camera_qvp_video_handshake = TRUE;
#endif /* FEATURE_QVPHONE */

#ifdef FEATURE_PNG_ENCODER
/* code for the PNG encoder */
static PNGE_spec_type   png_encode_spec;
static pnge_handle_type png_handle;
#endif /* FEATURE_PNG_ENCODER */

/* Set to true when a particular stats command is requested,
   Specific bit is cleared to false when that stats command is received */
int32 camera_pending_stats_cmds = NO_PENDING_AF_CMDS_TO_ISSUE;

/*  Following structure definition is used to copy/move data to
 *  camera_parm_info_type structure. Since camera_parm_info_type
 *  does not allow to transfer floats. This internal structure will
 *  store the meaning of the data that is being transfered.
 */
typedef struct
{
  int16  minimum_numerator_value;
  uint16 minimum_denominator_value;
  int16  maximum_numerator_value;
  uint16 maximum_denominator_value;
  int16  step_numerator_value;
  uint16 step_denominator_value;
  int16  default_numerator_value;
  uint16 default_denominator_value;
  uint32 value_to_get_index;
  float * ev_ptr;      /* EV Compensation table */
} camera_exposure_compensation_type;

/* ----------------------------------------------------------------------------
   Formula to calucalte the new EV target using adjustment step (EV_Comp) and
   the default value.
           EV_Comp
   Enew = 2        * EVdefault.

   We are using the adjustment step of 1/6, the following table contains the
    EV_comp
   2       value at different EV_Comp step.

   Enum camera_ev_compensation_type is used as an index to get the correct
   multiplying factor from this table.
---------------------------------------------------------------------------- */
float camera_ev_compensation_one_over_six_table[] =
{
    0.2500,    /* 2^EV_Comp = 2^-12/6 */
    0.2806,    /* 2^EV_Comp = 2^-11/6 */
    0.3150,    /* 2^EV_Comp = 2^-10/6 */
    0.3536,    /* 2^EV_Comp = 2^-9/6  */
    0.3969,    /* 2^EV_Comp = 2^-8/6  */
    0.4454,    /* 2^EV_Comp = 2^-7/6  */
    0.5000,    /* 2^EV_Comp = 2^-6/6  */
    0.5612,    /* 2^EV_Comp = 2^-5/6  */
    0.6299,    /* 2^EV_Comp = 2^-4/6  */
    0.7071,    /* 2^EV_Comp = 2^-3/6  */
    0.7937,    /* 2^EV_Comp = 2^-2/6  */
    0.8909,    /* 2^EV_Comp = 2^-1/6  */
    1.0000,    /* 2^EV_Comp = 2^0     */
    1.1225,    /* 2^EV_Comp = 2^1/6   */
    1.2599,    /* 2^EV_Comp = 2^2/6   */
    1.4142,    /* 2^EV_Comp = 2^3/6   */
    1.5874,    /* 2^EV_Comp = 2^4/6   */
    1.7818,    /* 2^EV_Comp = 2^5/6   */
    2.0000,    /* 2^EV_Comp = 2^6/6   */
    2.2449,    /* 2^EV_Comp = 2^7/6   */
    2.5198,    /* 2^EV_Comp = 2^8/6   */
    2.8284,    /* 2^EV_Comp = 2^9/6   */
    3.1748,    /* 2^EV_Comp = 2^10/6  */
    3.5636,    /* 2^EV_Comp = 2^11/6  */
    4.0000     /* 2^EV_Comp = 2^12/6  */
};

static camera_exposure_compensation_type camera_ev_compensation_table=
{
  -12, 6,      /* Minimun value  Numerator,Denominator  */
   12, 6,      /* Maximum value  Numerator,Denominator  */
   1, 6,       /* Step Value     Numerator,Denominator  */
   0, 6,       /* Default value  Numerator,Denominator  */
   12,         /* Value to convert fraction to index    */
   camera_ev_compensation_one_over_six_table,
};

static Camera_EndOfFrameMessageType      endOfFrameMessage[2];

//  The global variable below is no longer static
//  because systems wants to be able to see it with
//  the Mobile Camera Tuning tool.

camera_vfe_configuration_control_and_status_type camera_vfe_control_and_status;

static int32 camera_parm4_value = 0;

typedef struct
{
  boolean  received_output1;         /* Variable used to tell whether we received output1 isr   */
  boolean  received_output2;         /* Variable used to tell whether we received output2 isr   */
  boolean  received_camif_done;      /* Variable used to tell whether we received camif done    */
  boolean  bvcm_in_progress;         /* This will tell whether the BVCM process is in progress  */
  boolean  lpm_encoding_start;       /* This will tell whether the LPM process is in progress   */
  boolean  lpm_encoding_in_progress; /* This will tell whether the LPM process is in progress   */
  boolean  abort;                    /* This will tell whether we need to abort the operation   */
  boolean  abort_lpm_session;        /* This will tell whether we need to abort the operation   */
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
  uint16   camif_done_counter;       /* Counter to count number of times CAMIF_DONE is received */
  uint32   *Y_Address;               /* Luma Address   */
  uint32   *CbCr_Address;            /* Chroma Address */
#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
  boolean ycbcr_downsizer_in_progress; /* ycbcr_downsizer status */
#endif /* QDSP_IMAGE_YCBCR_DOWNSIZE_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/
  boolean encode_in_progress;
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */
} camera_take_picture_status_type;

typedef struct
{
  boolean  received_camif_error;        /* Variable used to tell whether we received camif error        */
  boolean  received_ahb_overflow_error; /* Variable used to tell whether we received AHB overflow error */
} camera_dsp_error_status_type;

#define FPS_LOG_SIZE 5
static  uint16   fps_log_index = 0;
typedef struct
{
  uint32   start_ms;
  uint32   end_ms;
  uint16   count;
  uint16   last_fps;
  uint16   fps[FPS_LOG_SIZE];
  boolean  waiting_for_first_output1_frame;
} camera_output1_frame_rate_type;

static camera_take_picture_status_type camera_take_picture_status;
static clk_cb_type camera_wait_camif_done_clk;

#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */

static camera_dsp_error_status_type camera_dsp_error_status;
static camera_output1_frame_rate_type camera_output1_frame_rate;

typedef enum
{
  CAMERA_TAKE_PICTURE_RETRY_NONE,
  CAMERA_TAKE_PICTURE_RETRY_NEEDED, /* AHB overflow detected */
  CAMERA_TAKE_PICTURE_RETRY_IDLING, /* sending VFE to IDLE   */
  CAMERA_TAKE_PICTURE_RETRY_INP,    /* retry capturing       */
  CAMERA_TAKE_PICTURE_RETRY_DONE    /* picture received      */
} camera_take_picture_retry_state_type;
static camera_take_picture_retry_state_type camera_take_picture_retry = CAMERA_TAKE_PICTURE_RETRY_NONE;

camera_snapshot_timing_type camera_snapshot_timing;
camera_snapshot_information_type camera_snapshot_information;


#ifdef FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK
typedef enum
{
  VFE_MODE_CONFIG = 0,
  VFE_MODE_CAPTURE,
  VFE_MODE_INVALID
} camera_svcs_vfe_mode_type;
#endif /* FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK */

#ifdef FEATURE_QVP_TEST_ENC_ROTATION
#error code not present
#endif

#ifdef FEATURE_CAMERA_5x5_ASF
uint32 luma_filter[9];
#endif

#ifdef FEATURE_VIDEO_ENCODE
static int output1BuffersQueueSize;    /* output1 Buffers Queue size */
extern uint32 allocBufSize;
extern uint32 rawHeaderSize;
#endif

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*************************************************************************************
This flag is to check AEC/AWB stats is requested by CAMQDSP_MSG_AE_AWB_STATS_COMPLETE.
If we are requesting stats with CAMQDSP_MSG_AE_AWB_STATS_COMPLETE, this flag is TRUE.
However, with any reason, if we don't get CAMQDSP_MSG_AE_AWB_STATS_COMPLETE, this flag
will be set to be FALSE again and stats will be requested by CAMQDSP_MSG_OUTPUT1_END_OF_FRAME
*************************************************************************************/
static boolean aec_awb_stats_received = FALSE;
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */

#if ( defined (FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III)||defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE || \
     defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_II)||defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_IV && \
    (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING || \
     defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)

typedef enum
{
  CAMERA_CLK_RESOURCE_VIEWFINDER = 0,
  CAMERA_CLK_RESOURCE_SNAPSHOT,
  CAMERA_CLK_RESOURCE_MAX
} camera_clk_resource_type;

#ifdef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE /* for 6280, the default clock is low at 122/61MHz*/
#define CAMERA_CLK_RESOURCE_DEFAULT CAMERA_CLK_RESOURCE_VIEWFINDER
#define CAMERA_SET_CLK_VIEWFINDER()      {clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_MED);}
/* Need to provide _HIGH() because if in case the pclk is higher than 61MHz, we need higher Hclk for 6280*/
#define CAMERA_SET_CLK_VIEWFINDER_HIGH() {clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_MED);}
#ifdef QDSP_MODULE_VFE20_DEFINED /*6290 target */
#define CAMERA_SET_CLK_SNAPSHOT()        {clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_HIGH);}
#define CAMERA_SET_CLK_HIGH_DEFAULT()     {clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_HIGH);} 
#else
#define CAMERA_SET_CLK_SNAPSHOT()        {clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_MED);}
#endif /*QDSP_MODULE_VFE20_DEFINED*/
#define CAMERA_SET_CLK_DEFAULT()         {clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_MED);}

#elif defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III
#define CAMERA_CLK_RESOURCE_DEFAULT CAMERA_CLK_RESOURCE_SNAPSHOT
#define CAMERA_SET_CLK_VIEWFINDER()       {clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER);}

#define CAMERA_SET_CLK_VIEWFINDER_HIGH()  {clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_MAX);}
#define CAMERA_SET_CLK_SNAPSHOT()         {clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_MAX);}
#define CAMERA_SET_CLK_DEFAULT()          {clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER);}
#define CAMERA_SET_CLK_HIGH_DEFAULT()     {clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_MAX);} 

#elif defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_IV /* for LCU(6270/6240 default clock is LOW at 122/61 Mhz) */
#error code not present
#else /* for 6800, the default clock is high, at 268/89MHz*/
#define CAMERA_CLK_RESOURCE_DEFAULT CAMERA_CLK_RESOURCE_SNAPSHOT
#define CAMERA_SET_CLK_VIEWFINDER()      {clk_regime_register_for_cpu_resource(CLKRGM_QCAM_VIEWFINDER);}
/* in 6800, low hclk is 65MHz, the max pclk supported is 64MHz, so no need for higher hclk, just for compatibility*/
#define CAMERA_SET_CLK_VIEWFINDER_HIGH() {clk_regime_register_for_cpu_resource(CLKRGM_QCAM_VIEWFINDER);}
#define CAMERA_SET_CLK_SNAPSHOT()        {clk_regime_deregister_for_cpu_resource(CLKRGM_QCAM_VIEWFINDER);}
#define CAMERA_SET_CLK_DEFAULT()         {clk_regime_deregister_for_cpu_resource(CLKRGM_QCAM_VIEWFINDER);}
#endif /* FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE*/
static camera_clk_resource_type camera_clk_resource = CAMERA_CLK_RESOURCE_DEFAULT;
#endif /* defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE &&    \
          (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING || \
           defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING) */
/*============================================================================
                     LOCAL FUNCTIONS DECLARATIONS
============================================================================*/

static void camera_svcs_queue_call_to_terminate(void);
static void camera_process_terminate(void);
static void camera_terminate_clk_cb (int4 ms);
static void camera_service_unavailable_clk_cb(int4 ms);

#ifdef FEATURE_MDP
void  camera_deregister_video (boolean clear_ovimg);
void  camera_deregister_layer1 (void);
static void camera_display_async_cb(void *, MDP_STATUS);
#endif /* FEATURE_MDP */

static void camera_preview_set_buffer_status(uint8 buffer_index, camera_preview_buffer_status_type new_status);

#ifdef    FEATURE_CAMERA_MOBICAT
extern void mobicat_deliver_config_to_jpege ( void );
#endif /* FEATURE_CAMERA_MOBICAT */
extern void exif_set_date_time (uint8 *ptr);
static void camera_jpeg_encode_callback(JPEGENC_CBrtnType  *clientData );
static void camera_jpeg_encode (qcamrawHeaderType *main_image_header, uint8 *main_image_buffer, qcamrawHeaderType *thumbnail_image_header, uint8 *thumbnail_image_buffer);
void camera_reject (camera_cb_f_type callback, void * client_data,
                           camera_func_type func);
static void camera_clr_overlays      (void);
static void camera_calculate_zoom    (void);
static camera_ret_code_type camera_svcs_set_encode_overlay(void);
static camera_ret_code_type camera_svcs_create_encode_overlay(ipl_image_type *overlay);
static camera_ret_code_type camera_svcs_create_thumbnail_overlay(ipl_image_type *overlay);
static camera_ret_code_type camera_svcs_create_preview_overlay(ipl_image_type   *overlay);
static void camera_svcs_clr_encode_overlay(void);
static void camera_svcs_clr_preview_overlay(void);
static void camera_svcs_clr_thumbnail_overlay(void);
#ifdef FEATURE_MDP
void camera_svcs_display_frame_overlay(boolean clear_overlay_layer);
static void camera_svcs_fill_buffer_16(byte *buffer, uint16 color, uint32 width, uint32 height);
#endif /* FEATURE_MDP */
static void camera_set_exif_make_model(void);
static void camera_process_start ( camera_cb_f_type callback, void *client_data, rex_tcb_type *tcb, camera_ret_code_type *status);
static void camera_process_stop ( camera_cb_f_type callback, void *client_data, rex_tcb_type *tcb);
static void camera_process_cleanup
(
  rex_tcb_type    *tcb
);
static void camera_process_start_preview ( camera_cb_f_type callback, void *client_data);
static void camera_process_stop_preview ( rex_tcb_type *tcb );
static void camera_process_take_picture (
  camera_cb_f_type callback,
  void *client_data
  );
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/

static void camera_process_release_picture ( rex_tcb_type *tcb );
static void camera_process_release_frame (uint8 buffer_index);
static void camera_process_blt (uint8 buffer_index);
static void camera_process_encode_picture ( camera_cb_f_type callback, void *client_data, qcamrawHeaderType *main_image_header, byte *main_image_buffer, qcamrawHeaderType *thumbnail_image_header, byte *thumbnail_image_buffer);
static camera_ret_code_type camera_encode_picture_common ( camera_frame_type *frame, camera_handle_type *handle, camera_cb_f_type callback, void * client_data);
#ifdef FEATURE_QVPHONE
// static void camera_process_qdsp_service_unavailable_msg(void);
static void camera_process_start_qvp_encode ( camera_cb_f_type callback, void *client_data, rex_tcb_type *tcb, camera_ret_code_type *status);
static void camera_process_stop_qvp_encode( rex_tcb_type *tcb );
#endif /* FEATURE_QVPHONE */
static void camera_process_enable_frame_callback ( void );
static void camera_process_disable_frame_callback ( void );
static void camera_process_set_overlay ( camera_cb_f_type callback, void *client_data, rex_tcb_type *tcb);
static void camera_process_clr_overlay ( camera_cb_f_type callback, void *client_data);
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
static void camera_process_set_icon_array ( rex_tcb_type *tcb, ipl_icon_type **icons, uint32 dx, uint32 dy);
static void camera_process_clr_icon_array (void);
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
static void camera_process_set_parm_shutter_sound(camera_cb_f_type callback, void *client_data, camera_shutter_snd_play_type info1);
#endif
static void camera_process_set_reflect (void);
#ifdef FEATURE_MDP
static void camera_svcs_adjust_mdp_for_video (void);
static void camera_svcs_adjust_mdp_for_snapshot(uint32 rotation);
#endif /* FEATURE_MDP */
static void camera_process_set_dimensions ( camera_cb_f_type callback, void *client_data,rex_tcb_type *info1);
static void camera_process_set_position ( camera_cb_f_type callback, void *client_data);
static void camera_process_set_exif_tag ( camera_cb_f_type callback, void *client_data);

void camera_terminate (camera_cb_type cb, int32 parm4);
static void camera_process_qdsp_idle_state_msg (void);
static void camera_process_qdsp_output1_msg (Camera_EndOfFrameMessageType *msg);
static void camera_process_qdsp_output2_msg (Camera_EndOfFrameMessageType *msg);
static void camera_process_qdsp_detect_error_msg (CAMQDSP_ResponseMsgType dsp_response);
static void camera_process_qdsp_config_done_msg (CAMQDSP_VideoEncStateType *x);
static void camera_process_qdsp_illegal_cmd_msg (void);
static void camera_process_qdsp_update_complete_msg (void);
static void camera_process_qdsp_hist_stats_complete_msg (void);
static void camera_wait_clk_cb (int4 ms);
#ifdef HANDLE_CAMIF_ERROR
static void camera_camif_error_clk_cb (int4 ms);
#endif /* HANDLE_CAMIF_ERROR */
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
static void camera_clr_icons (void);
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
void camera_initiate_capture (void);


boolean camera_set_preview_buffers (void);
void camera_set_preview_headers (void);

static void camera_set_preview_headers_rotation (void);

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
static camera_ret_code_type camera_set_icon_and_preview_overlay(void);
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
boolean camera_config_vfe(void);
void    camera_qdsp_cb (CAMQDSP_ResponseType *response);

static void camera_calculate_vfe_clipping (void);
static void camera_calculate_ahb_burst_factors (uint32 *burst_factor1, uint32 *burst_factor2, uint32 image_size);
#ifdef FEATURE_CAMERA_LPM
#error code not present
#endif /* FEATURE_CAMERA_LPM */

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
/* ycbcr downsizer process */
static void camera_svcs_process_ycbcr_downsize (void);
static void camera_svcs_ycbcr_downsizer_cb (ycbcr_downsizer_cbmsg_type  msg);
static boolean camera_svcs_is_ycbcr_downsizer_needed ( void );
#endif /*QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED */

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED 
static void camera_svcs_chroma_downsample_h2v1_to_h2v2(uint8 *image, uint16 height, uint16 width);
#endif
static void camera_svcs_swap_dimensions (uint16 *x, uint16 *y);
#ifdef FEATURE_CAMERA_5x5_ASF
static void camera_svcs_set_luma_filter (void);
#endif /* FEATURE_CAMERA_5x5_ASF */

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/

#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
static camera_shutter_snd_play_type  shutter_snd_status = NO_SHUTTER_SND;
#endif /* FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND */

#if 0

Causing compile warnings, removing

/* Mapping of VFE Focus Window to Display Focus Window.
 * Input : camera_vfe_focus_window
 * Output: camera_display_focus_window
 */
static boolean camera_vfe_to_display_focus_window_mapping
(
  camera_focus_window_type *camera_vfe_focus_window,
  camera_focus_window_type *camera_display_focus_window
);

/* Display Focus Window mapping to VFE Focus Window
 * Input : camera_display_focus_window
 * Output: camera_vfe_focus_window
 */
static boolean camera_display_to_vfe_focus_window_mapping
(
  camera_focus_window_type *camera_display_focus_window,
  camera_focus_window_type *camera_vfe_focus_window
);

/* Sanity check for Focus window */
static boolean camera_verify_vfe_focus_window
(
  camera_focus_window_type *camera_vfe_focus_window
);
static boolean camera_verify_display_focus_window
(
  camera_focus_window_type *camera_display_focus_window
);
#endif /* 0 */

#ifdef FEATURE_PNG_ENCODER
/* for PNG encoder */
static void camera_PNG_CB (PNGE_status_enum_type command, void *handle, void *pnge_data);
#endif /* FEATURE_PNG_ENCODER */

static void camera_svcs_fill_exif_attributes
(
 exif_attributes_type *attributes
 );

#ifdef CAMERA_LOGGING
void camera_log
(
  camera_log_type type,
  uint32          func,
  uint32          rsp,
  uint32          info,
  uint32          line
)
{
  camera_logs [log_index].type   = type;
  camera_logs [log_index].func   = (camera_func_type)func;
  camera_logs [log_index].rsp    = (CAMQDSP_ResponseMsgType)rsp;
  camera_logs [log_index].info = (camera_log_info_type)info;
  camera_logs [log_index++].line = line;
  log_index %= LOG_SIZE;
  camera_logs [log_index].type   = LOG_NEXT;
}
#endif /* CAMERA_LOGGING */

static void camera_clr_take_picture (void);
#ifdef CAMERA_MALLOC_LOGGING
static boolean camera_check_memory_leaks(void);
#endif /* CAMERA_MALLOC_LOGGING */
static void camera_process_qdsp_camif_done_msg(void);
static void camera_wait_camif_done_clk_cb (int4 ms);

#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */

static void camera_process_handle_error (void);
static void camera_handle_frames_for_takepicture(void);
static uint32 camera_calculate_skip_pattern(uint32 input_sensor_fps, float input_desired_fps, uint32 bitwidth);

static void camera_process_take_single_picture(void);
static boolean camera_abort_takepicture_encoding_operation(void);
static void camera_show_snapshot_timing( void );

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
#if (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED)
static void camera_handle_single_frame_for_handjitter(void);
static void camera_svcs_setup_vfe_for_hjr(void);
#endif /* (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED) */
static void camera_svcs_optimize_resources (void);

#ifdef FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK
static void camera_svcs_set_vfe_clk (camera_svcs_vfe_mode_type);
#endif /* FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK */

#ifdef FEATURE_CAMERA_LPM
#error code not present
#endif /* FEATURE_CAMERA_LPM */

#ifdef FEATURE_CAMERA_HJR
static boolean camera_svcs_hjr_enabled(void);
#endif /* FEATURE_CAMERA_HJR */
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
static void camera_rotate_encode_overlay(void);

#ifdef FEATURE_CAMERA_YCBCR_ONLY
#define camera_aec_awb_ready FALSE
#define camera_svcs_set_ev_compensation(parm) CAMERA_NOT_SUPPORTED
#define camera_restore_config_for_preview()
#define camera_process_qdsp_ae_awb_stats_complete_msg(msg)
#else
static boolean camera_aec_awb_ready (void);
static camera_ret_code_type camera_svcs_set_ev_compensation(int32 parm);
static void camera_restore_config_for_preview(void);
static void camera_process_qdsp_ae_awb_stats_complete_msg (CAMQDSP_AE_AWBStatsMessageType * msg);
static void camera_request_aec_awb_stats(void);
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

/* Applies the overlay on the main image and thumbnail */
static void camera_apply_overlay_on_snapshot(void);
static camera_cb_type camera_svcs_translate_ret_val_to_cb
(
 camera_ret_code_type ret_val
);

#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT
#ifdef FEATURE_CAMERA_MULTI_SENSOR
extern camsensor_sensor_model_pair_type current_camsensor_pair;
#endif
extern camsensor_sensor_model_type current_camsensor_id;
static boolean camsensor_sensor_detected = FALSE;
#endif
/*===========================================================================

FUNCTION      CAMERA_VFE_CONTROL_AND_STATUS_SET_DEFAULT

DESCRIPTION
              Init the structure.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_vfe_control_and_status_set_default()
{
  camera_vfe_control_and_status.number_of_frames_to_ignore = 0;
  camera_vfe_control_and_status.reconfigure_vfe            = FALSE;
  camera_vfe_control_and_status.vfe_idle_msg_failed        = FALSE;
  camera_vfe_control_and_status.wait_for_dsp_idle          = FALSE;
}

/*===========================================================================

FUNCTION      CAMERA_TAKE_PICTURE_STATUS_SET_DEFAULT

DESCRIPTION
              Init the structure.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_take_picture_status_set_default()
{
  camera_take_picture_status.received_output1         = FALSE;
  camera_take_picture_status.received_output2         = FALSE;
  camera_take_picture_status.received_camif_done      = FALSE;
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
  camera_take_picture_status.camif_done_counter       = 0;
  camera_take_picture_status.lpm_encoding_in_progress = FALSE;
  camera_take_picture_status.lpm_encoding_start       = FALSE;
  camera_take_picture_status.bvcm_in_progress         = FALSE;
  camera_take_picture_status.abort_lpm_session        = FALSE;
  camera_take_picture_status.Y_Address                = NULL;
  camera_take_picture_status.CbCr_Address             = NULL;
  camera_take_picture_status.abort                    = FALSE;
#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
  camera_take_picture_status.ycbcr_downsizer_in_progress  = FALSE;
#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/
  camera_take_picture_status.encode_in_progress       = FALSE;
}

/*===========================================================================

FUNCTION      CAMERA_DSP_ERROR_STATUS_SET_DEFAULT

DESCRIPTION
              Init the structure.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_dsp_error_status_set_default()
{
  camera_dsp_error_status.received_camif_error        = FALSE;
  camera_dsp_error_status.received_ahb_overflow_error = FALSE;
}

/*===========================================================================

FUNCTION      CAMERA_FREE_PREVIEW_BUFFERS

DESCRIPTION
              Free preview buffers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_free_preview_buffers()
{
  uint32 current_buffer;
  MSG_CAMERADEBUG("CAMERA_SVCS: Freeing Preview Buffers", 0, 0, 0);
  /* Free all buffers */
  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    CAM_Free (camera_preview_buffers.buffers[current_buffer].buf_ptr);
    camera_preview_buffers.buffers[current_buffer].buf_ptr = NULL;
    camera_preview_buffers.buffers[current_buffer].status  = AVAILABLE;
  }
  camera_preview_buffers.num_buffers_with_display = 0;
  camera_preview_buffers.num_buffers_with_VFE = 0;
  camera_app_buf_index[0] = CAMERA_INVALID_BUF_INDEX;
  camera_app_buf_index[1] = CAMERA_INVALID_BUF_INDEX;
}

/*===========================================================================

FUNCTION      CAMERA_RESET_PREVIEW_BUFFERS

DESCRIPTION
              Reset the preview buffer status.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_reset_preview_buffers(void)
{
  uint32 current_buffer;

  MSG_CAMERAPBUF("PBUF Reset buffer status.", 0, 0, 0);
  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    if (camera_preview_buffers.buffers[current_buffer].buf_ptr)
    {
      camera_preview_buffers.buffers[current_buffer].status = AVAILABLE;
    }
  }
  camera_preview_buffers.num_buffers_with_display = 0;
  camera_preview_buffers.num_buffers_with_VFE = 0;
  camera_app_buf_index[0] = CAMERA_INVALID_BUF_INDEX;
  camera_app_buf_index[1] = CAMERA_INVALID_BUF_INDEX;
}

/*===========================================================================

FUNCTION      CAMERA_MALLOC_PREVIEW_BUFFERS

DESCRIPTION
              Malloc preview buffers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_malloc_preview_buffers()
{
  uint32 current_buffer;
  MSG_CAMERAPBUF("PBUF ALLOC Preview Buffers", 0, 0, 0);
  /* Allocate all buffers */
  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    camera_preview_buffers.buffers[current_buffer].buf_ptr =
      CAM_Malloc (camera_preview_buffer_size | CAM_ALLOC_NO_ZMEM);
  }
  camera_reset_preview_buffers();
}

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
void camera_svcs_init(void)
{
#ifdef FEATURE_PMIC_FLASH_LED_DRIVER
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
#endif /* FEATURE_PMIC_FLASH_LED_DRIVER */

#ifndef T_IO_CARD
#ifdef CAMERA_LOGGING
  MSG_HIGH("Appease lint, camera_log.type = ", camera_logs[0].type, 0, 0);
#endif /* CAMERA_LOGGING */
#ifdef FEATURE_VIDEO_ENCODE
  MSG_HIGH("Appease lint, output1BuffersQueueSize = ", output1BuffersQueueSize, 0, 0);
#endif /* FEATURE_VIDEO_ENCODE */
  if (camera_initialized)
  {
    return;
  }

  camera_initialized = TRUE;

#ifdef FEATURE_NEW_SLEEP_API
  if(!camera_sleep_handle_register)
  {
    camera_sleep_okts_handle = sleep_register("CAMERA", TRUE);
    camera_sleep_handle_register = TRUE;

#ifdef CLK_REGIME_UP_SLEEP__MCLK_HCLK
    sleep_set_hclk_restriction (camera_sleep_okts_handle, CLKRGM_HCLK_NO_SWITCH);
    sleep_set_mclk_restriction (camera_sleep_okts_handle, CLKRGM_MCLK_ON);
#endif /* CLK_REGIME_UP_SLEEP__MCLK_HCLK */

#ifndef FEATURE_ARM_HALT_CONTROL_CAMERA_OKTH_SIG
    sleep_set_swfi_restriction(camera_sleep_okts_handle, SLEEP_SWFI_RESTRICT_ON);
#endif /* FEATURE_ARM_HALT_CONTROL_CAMERA_OKTH_SIG */


  }
#endif

  /* Don't need gpios and clk regimes yet */
  camsensor_unconfig_sensor_gpios_clks ();

#ifdef FEATURE_MDP
  /* De-register the video */
  camera_deregister_video (TRUE);

  mdpImg.dest    = PRIMARY_LCD_TYPE;
  mdpImg.imgType = MDP_YCBCR420;
  mdpImg.width   = CAMERA_LCD_WIDTH;
  mdpImg.cwin.x1 = 0;
  mdpImg.cwin.x2 = CAMERA_LCD_WIDTH - 1;
  mdpImg.cwin.y1 = 0;
  mdpImg.cwin.y2 = CAMERA_LCD_HEIGHT - 1;
  mdpImg.lcd_x   = -BREW_OFFSET_X;
  mdpImg.lcd_y   = -BREW_OFFSET_Y;

  /* De-register the overlay layer */
  camera_deregister_layer1 ();

  mdpImg_frame.dest    = PRIMARY_LCD_TYPE;

  mdpImg_frame.imgType = MDP_RGB565;
  mdpImg_frame.width   = CAMERA_LCD_WIDTH;
  mdpImg_frame.cwin.x1 = 0;
  mdpImg_frame.cwin.x2 = CAMERA_LCD_WIDTH - 1;
  mdpImg_frame.cwin.y1 = 0;
  mdpImg_frame.cwin.y2 = CAMERA_LCD_HEIGHT - 1;
  mdpImg_frame.lcd_x   = -BREW_OFFSET_X;
  mdpImg_frame.lcd_y   = -BREW_OFFSET_Y;

  mdpImg.layer = MDP_LAYER_0;
  mdpImg_frame.layer = MDP_LAYER_1;

  /* For overlay frame, rotation is performed in s/w. MDP does rotation
   * evertime it displays the frame. We avoid that by rotating it once
   * in s/w and giving it to MDP */

  mdpImg_frame.mdpOp   = MDPOP_ALPHAB | MDPOP_TRANSP;
  mdpImg_frame.tpVal   = 0xF81F;
  mdpImg_frame.alpha   = 0x100;
#endif /* FEATURE_MDP */

  camera_default_preview_rotation = CAMERA_PREVIEW_ROTATION;

#if defined(FEATURE_CAMERA_LCD_DIRECT_MODE) || !defined(FEATURE_MDP)
  if (camera_default_preview_rotation == 0)
  {
    camera_default_preview_rotation_ipl = IPL_NOROT;
  }
  else if (camera_default_preview_rotation == 90)
  {
    camera_default_preview_rotation_ipl = IPL_ROT90;
  }
  else if (camera_default_preview_rotation == 180)
  {
    camera_default_preview_rotation_ipl = IPL_ROT180;
  }
  else
  {
    camera_default_preview_rotation_ipl = IPL_ROT270;
  }
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE || nFEATURE_MDP */

  clk_def (&camera_wait_clk);
  rex_init_crit_sect(&camera_wait_clk_cs);
  clk_def (&camera_wait_camif_done_clk);
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */
#ifdef HANDLE_CAMIF_ERROR
  clk_def (&camera_camif_error_clk);
#endif /* HANDLE_CAMIF_ERROR */

  clk_def (&camera_terminate_clk);
  clk_def (&camera_service_unavailable_clk);

#ifdef CUST_EDITION

#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT

#ifdef FEATURE_CAMERA_MULTI_SENSOR
	for (current_camsensor_pair=CAMSENSOR_ID_PAIR_START; current_camsensor_pair < CAMSENSOR_ID_PAIR_END; current_camsensor_pair++)
  	{
  		camera_number_of_sensors = 0;
  		for(camera_asi=0; camera_asi < NUM_OF_SENSORS_ON_PHONE; camera_asi++)
  		{
			camsensor_power_on();

			camsensor_power_up();

			/* Assign this sensor the available ID */
    		camera_sensors[camera_asi].sensor_id = (uint16)camera_asi;
			if ( camsensor_initialized == TRUE )
			{
				/* Sensor responded as expected during initization. */
				camera_sensors[camera_asi].sensor_available = TRUE;

				/* Don't set camera_sensors[camera_asi].sensor_width/height
				* here because camsensor_start() (called in camera_start())
				* change it based on the specific sensor selected. */

				/* Set the fps to the maximum supported by this sensor */
				camera_sensors[camera_asi].fps = 15 * Q8;

				 /* Whether the device driver can sense when sensor is rotated */
				if(camera_asi == 0)
					camera_sensors[camera_asi].sensor_rotation_sensing = FALSE;
				else if(camera_asi == 1)
					camera_sensors[camera_asi].sensor_rotation_sensing = TRUE;

				/* How the sensor are installed */
				if(camera_asi == 0)
				{
					camera_sensors[camera_asi].default_rotation = 0;
					camera_sensors[camera_asi].default_orientation = CAMERA_ORIENTATION_PORTRAIT;
				}
				else
				{
					camera_sensors[camera_asi].default_rotation = 90;
					camera_sensors[camera_asi].default_orientation = CAMERA_ORIENTATION_PORTRAIT;
				}

				camera_number_of_sensors++;
			}
			else
    		{
      			camera_sensors[camera_asi].sensor_available = FALSE;
    		}

    		camsensor_power_down();
  			camsensor_power_off();
  		}

  		if ( camera_number_of_sensors == NUM_OF_SENSORS_ON_PHONE )
  		{
			break;
  		}
  	}
#else
	for (current_camsensor_id = (camsensor_sensor_model_type) 0; current_camsensor_id < CAMSENSOR_ID_MAX; ++current_camsensor_id)
	{
		camsensor_power_on();
		camsensor_power_up();

		if ( camsensor_initialized == TRUE )
		{
			camsensor_sensor_detected = TRUE;
		}

		camsensor_power_down();
		camsensor_power_off();

		if ( camsensor_sensor_detected == TRUE )
		{
			break;
		}
	}
#endif

#else
  /* The following sequence gets the camera initialized and then
   * put in power down mode. We cannot leave camera in power up state
   * until used.
   * Also, camsensor_gp_clk_hz is set during power_up, but is used at power_on.
   * This seemingly dummy sequence actually get the right clock rate set
   * when the application is started. */

#ifdef FEATURE_CAMERA_MULTI_SENSOR
  for (camera_asi = 0; camera_asi < NUM_OF_SENSORS_ON_PHONE; camera_asi++)
#endif /* FEATURE_CAMERA_MULTI_SENSOR */
  {
  	MSG_FATAL("camera_svcs_init is ok................",0,0,0);
  camsensor_power_on  ();
  camsensor_power_up  ();

#ifdef FEATURE_CAMERA_MULTI_SENSOR
    /* Assign this sensor the available ID */
    camera_sensors[camera_asi].sensor_id = (uint16)camera_asi;

    if (camsensor_initialized == TRUE)
    {
      /* Sensor responded as expected during initization. */
      camera_sensors[camera_asi].sensor_available = TRUE;

      /* Don't set camera_sensors[camera_asi].sensor_width/height
       * here because camsensor_start() (called in camera_start())
       * change it based on the specific sensor selected. */

      /* Set the fps to the maximum supported by this sensor */
      camera_sensors[camera_asi].fps = 15 * Q8;

      /* Whether the device driver can sense when sensor is rotated */
      camera_sensors[camera_asi].sensor_rotation_sensing = FALSE;

      /* How the sensor are installed */
      camera_sensors[camera_asi].default_rotation = camera_default_preview_rotation;
      camera_sensors[camera_asi].default_orientation = camera_default_orientation;

      camera_number_of_sensors++;
    }
    else
    {
      camera_sensors[camera_asi].sensor_available = FALSE;
    }
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

  camsensor_power_down();
  camsensor_power_off ();
  }
#endif
#endif

#ifdef FEATURE_CAMERA_MULTI_SENSOR
  camera_asi = 0; /* select the primary sensor */
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

#ifdef FEATURE_STROBE_FLASH
  /* Initialize the strobe flash and put the device in powered down and
   * off state.
   */
  strobe_flash_init ();
#endif  /* FEATURE_STROBE_FLASH */

#if defined FEATURE_WHITE_LED_FLASH && !defined FEATURE_PMIC_FLASH_LED_DRIVER
  /* turn off white LED */
#ifndef CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH
  BIO_TRISTATE(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_M);
  BIO_OUT(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_OFF_V);
#endif /* CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH */
#endif /* FEATURE_WHITE_LED_FLASH */

#ifdef FEATURE_PMIC_FLASH_LED_DRIVER
  errFlag |= pm_flash_led_set_mode(PM_FLASH_LED_MODE__MANUAL);
  errFlag |= pm_flash_led_set_current(PMIC_FLASH_CURRENT_DOWN);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    // DM message sent
    MSG_ERROR("PMIC error", 0, 0, 0);
  }
#endif /* FEATURE_PMIC_FLASH_LED_DRIVER */

  CAMERA_SLEEP_ALLOW ();
  CAMERA_HALT_ALLOW();
  CAMERA_BUS_OFF();

  camera_disp_info = disp_get_info ();
  if (camera_disp_info.disp_width == 0 || camera_disp_info.disp_height == 0)
  {
    camera_disp_info.disp_width = CAMERA_LCD_WIDTH;
    camera_disp_info.disp_height = CAMERA_LCD_HEIGHT;
  }

  MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_INIT", 0, 0, 0);
  camera_state = CAMERA_STATE_INIT;
  camera_app_cb    = NULL;

  camera_parm_zoom.minimum_value = 0;
  camera_parm_zoom.maximum_value = 0;
  camera_parm_zoom.default_value = 0;
  camera_parm_zoom.current_value = 0;
  camera_parm_zoom.step_value    = 1;

  // this struct holds the min and max points in the zoom table
  // we want to travel between (inclusive) to get our zoom factor
  camera_parm_zoom_table.minimum_value = 0;
  camera_parm_zoom_table.maximum_value = 0;
  camera_parm_zoom_table.default_value = 0;
  camera_parm_zoom_table.current_value = 0;
  camera_parm_zoom_table.step_value    = 1;

  camera_parm_preview_rotation.minimum_value = CAMERA_SP_NORMAL;
  camera_parm_preview_rotation.maximum_value = CAMERA_SP_ACLK_TANGENTIAL;
  camera_parm_preview_rotation.default_value = CAMERA_SP_CLK_TANGENTIAL;
  camera_parm_preview_rotation.current_value = CAMERA_SP_CLK_TANGENTIAL;
  camera_parm_preview_rotation.step_value    = 1;

  camera_parm_encode_rotation.minimum_value = 0;
  camera_parm_encode_rotation.maximum_value = 270;
  camera_parm_encode_rotation.default_value = camera_default_jpeg_encode_rotation;
  camera_parm_encode_rotation.current_value = camera_default_jpeg_encode_rotation;
  camera_parm_encode_rotation.step_value    = 90;

  camera_parm_fps.minimum_value = 1;
  camera_parm_fps.maximum_value = 1;
  camera_parm_fps.default_value = 1;
  camera_parm_fps.current_value = 1;
  camera_parm_fps.step_value    = 1;

  camera_parm_flash.minimum_value = (int)CAMERA_FLASH_AUTO;
  camera_parm_flash.maximum_value = (int)CAMERA_FLASH_MAX - 1;
  camera_parm_flash.default_value = (int)CAMERA_FLASH_OFF;
  camera_parm_flash.current_value = (int)CAMERA_FLASH_OFF;
  camera_parm_flash.step_value    = 1;

  camera_parm_flash_state.minimum_value = (int)CAMERA_FLASH_READY;
  camera_parm_flash_state.maximum_value = (int)CAMERA_MAX_FLASH_STATE - 1;
  camera_parm_flash_state.default_value = (int)CAMERA_FLASH_NOT_READY;
#ifdef FEATURE_STROBE_FLASH
  camera_parm_flash_state.current_value = (int)CAMERA_FLASH_NOT_READY;
#elif defined FEATURE_WHITE_LED_FLASH
  camera_parm_flash_state.current_value = (int)CAMERA_FLASH_READY;
#elif defined FEATURE_PMIC_FLASH_LED_DRIVER
  camera_parm_flash_state.current_value = (int)CAMERA_FLASH_READY;
#else
  camera_parm_flash_state.current_value = (int)CAMERA_NO_FLASH;
#endif /* FEATURE_STROBE_FLASH */
  camera_parm_flash_state.step_value    = 1;

  camera_parm_reflect.minimum_value = (int)CAMERA_NO_REFLECT;
  camera_parm_reflect.maximum_value = (int)CAMERA_MAX_REFLECT - 1;
  camera_parm_reflect.default_value = (int)CAMERA_NO_REFLECT;
  camera_parm_reflect.current_value = (int)CAMERA_NO_REFLECT;
  camera_parm_reflect.step_value    = 1;
#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
  preview_mode         = CAMERA_PREVIEW_MODE_MOVIE;
#else /* defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE */
  preview_mode         = CAMERA_PREVIEW_MODE_SNAPSHOT;
#endif /* defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE */

  camera_parm_preview_mode.minimum_value = 0;
  camera_parm_preview_mode.maximum_value = (int)CAMERA_MAX_PREVIEW_MODE - 1;
  camera_parm_preview_mode.default_value = (int)preview_mode;
  camera_parm_preview_mode.current_value = (int)preview_mode;
  camera_parm_preview_mode.step_value    = 1;

  camera_parm_antibanding.minimum_value = 0;
  camera_parm_antibanding.maximum_value = (int)CAMERA_MAX_ANTIBANDING - 1;
  camera_parm_antibanding.default_value = (int)CAMERA_ANTIBANDING_OFF;
  camera_parm_antibanding.current_value = (int)CAMERA_ANTIBANDING_OFF;
  camera_parm_antibanding.step_value    = 1;
#ifdef FEATURE_STROBE_FLASH
  camera_parm_red_eye_reduction.minimum_value = FALSE;
  camera_parm_red_eye_reduction.maximum_value = TRUE;
  camera_parm_red_eye_reduction.default_value = FALSE;
  camera_parm_red_eye_reduction.current_value = FALSE;
  camera_parm_red_eye_reduction.step_value    = 1;
#endif/*FEATURE_STROBE_FLASH*/
  camera_parm_brightness.minimum_value = CAMERA_MIN_BRIGHTNESS;
  camera_parm_brightness.maximum_value = CAMERA_MAX_BRIGHTNESS;
  camera_parm_brightness.default_value = CAMERA_DEF_BRIGHTNESS;
  camera_parm_brightness.current_value = CAMERA_DEF_BRIGHTNESS;
  camera_parm_brightness.step_value    = 1;

  camera_parm_contrast.minimum_value = CAMERA_MIN_CONTRAST;
  camera_parm_contrast.maximum_value = CAMERA_MAX_CONTRAST;
  camera_parm_contrast.default_value = CAMERA_DEF_CONTRAST;
  camera_parm_contrast.current_value = CAMERA_DEF_CONTRAST;
  camera_parm_contrast.step_value    = 1;

  camera_parm_special_effects.minimum_value = (int)CAMERA_EFFECT_MIN_MINUS_1 +1;
  camera_parm_special_effects.maximum_value = (int)CAMERA_EFFECT_MAX_PLUS_1  -1;
  camera_parm_special_effects.default_value = (int)CAMERA_EFFECT_OFF;
  camera_parm_special_effects.current_value = (int)CAMERA_EFFECT_OFF;
  camera_parm_special_effects.step_value    = 1;

  camera_parm_white_balance.minimum_value = (int)CAMERA_WB_MIN_MINUS_1    +1;
  camera_parm_white_balance.maximum_value = (int)CAMERA_EFFECT_MAX_PLUS_1 -1;
  camera_parm_white_balance.default_value = (int)CAMERA_WB_AUTO;
  camera_parm_white_balance.current_value = (int)CAMERA_WB_AUTO;
  camera_parm_white_balance.step_value    = 1;

  /* There are not known yet */
  camera_parm_manual_focus.minimum_value = 0;
  camera_parm_manual_focus.maximum_value = 0;
  camera_parm_manual_focus.default_value = 0;
  camera_parm_manual_focus.current_value = 0;
  camera_parm_manual_focus.step_value    = 1;

  camera_parm_auto_exposure_mode.minimum_value = (int)CAMERA_AEC_FRAME_AVERAGE;
  camera_parm_auto_exposure_mode.maximum_value = (int)CAMERA_AEC_MAX_MODES-1;
  camera_parm_auto_exposure_mode.default_value = (int)CAMERA_AEC_CENTER_WEIGHTED;
  camera_parm_auto_exposure_mode.current_value = (int)CAMERA_AEC_CENTER_WEIGHTED;
  camera_parm_auto_exposure_mode.step_value    = 1;

  camera_parm_exposure_compensation.minimum_value = ( camera_ev_compensation_table.minimum_numerator_value << 16 |
                                                      camera_ev_compensation_table.minimum_denominator_value
                                                    );
  camera_parm_exposure_compensation.maximum_value = ( camera_ev_compensation_table.maximum_numerator_value << 16 |
                                                      camera_ev_compensation_table.maximum_denominator_value
                                                    );
  camera_parm_exposure_compensation.default_value = ( camera_ev_compensation_table.default_numerator_value << 16 |
                                                      camera_ev_compensation_table.default_denominator_value
                                                    );
  camera_parm_exposure_compensation.current_value = ( camera_ev_compensation_table.default_numerator_value << 16 |
                                                      camera_ev_compensation_table.default_denominator_value
                                                    );
  camera_parm_exposure_compensation.step_value    = ( camera_ev_compensation_table.step_numerator_value << 16 |
                                                      camera_ev_compensation_table.step_denominator_value
                                                    );

  {
    /* Min == max FPS here initially. */
    /* Denominator in the MS 16 bits; numerator time denominator in the LS 16 */
    /* So, 15 FPS looks like this: 1000 | 15000 */
    camera_parm_preview_fps.minimum_value = ((CAMERA_FPS_DENOMINATOR << 16) |
       (0xFFFF & (CAMERA_FPS_DENOMINATOR*(camsensor_static_params[camera_asi].max_preview_fps>>8))));

    /* Max FPS supported from this sensor */
    camera_parm_preview_fps.maximum_value = camera_parm_preview_fps.minimum_value;
    camera_parm_preview_fps.default_value = camera_parm_preview_fps.minimum_value;
    camera_parm_preview_fps.current_value = camera_parm_preview_fps.default_value;
    /* Not applicable here */
    camera_parm_preview_fps.step_value    = 0;

  }


  camera_parm_iso.minimum_value = CAMERA_ISO_AUTO;
  camera_parm_iso.maximum_value = CAMERA_ISO_MAX;
  camera_parm_iso.default_value = CAMERA_ISO_AUTO;
  camera_parm_iso.current_value = CAMERA_ISO_AUTO;
  camera_parm_iso.step_value = 1;

  camera_parm_af_mode.minimum_value = CAMERA_AF_MODE_NORMAL;
  camera_parm_af_mode.maximum_value = CAMERA_AF_MODE_MAX;
  camera_parm_af_mode.default_value = CAMERA_AF_MODE_NORMAL;
  camera_parm_af_mode.current_value = CAMERA_AF_MODE_NORMAL;
  camera_parm_af_mode.step_value = 1;

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /*FEATURE_CAMERA_BURST_MODE*/

  qcamraw_header_size = qcamrawInitHeaderSize();

  /* Memclear global and locally used structures for 3A */
  camera_3a_init();

  /* Initialize the trigerring flags */

 #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
 #endif /* CAMERA_USES_CHROMATIX_0x203 */

  /* Runtime detection of VFE features */
  /* renamed. This used to be vfe_features_auto_detect() */
  vfe_init ();
#endif /* nT_IO_CARD */
}

#if defined FEATURE_QVP_OFFLINE_VIDEO_ENCODING || defined FEATURE_VIDEO_ENCODE
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
/*lint -save -e715 code not reference */
void camera_svcs_video_handshake(uint32 code)
{
  camera_log(LOG_FUNC, (uint32)CAMERA_FUNC_VIDEO_HANDSHAKE, 0, 0, __LINE__);
  graph_queue_camera_func((uint32)CAMERA_FUNC_VIDEO_HANDSHAKE, (void *)0, (void *)0, 0, 0, 0, 0);
}
/*lint -restore */
#endif /* FEATURE_QVP_OFFLINE_VIDEO_ENCODING || FEATURE_VIDEO_ENCODE*/

#ifdef FEATURE_CAMERA_MULTI_SENSOR
/*===========================================================================

FUNCTION      CAMERA_SELECT_SENSOR

DESCRIPTION
              Set camera_asi to sensor_id.

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_select_sensor(uint16 sensor_id)
{
  int i;

#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  camera_log(LOG_FUNC, (uint32)CAMERA_FUNC_SELECT_SENSOR, 0, 0, __LINE__);
  MSG_MED("camera_select_sensor", 0, 0, 0);

  if (camera_state != CAMERA_STATE_INIT)
  {
    event_report(EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  /* Check to make sure we have information for the sensor being selected.
   * Otherwise, its an invalid sensor selection and leave camera_asi
   * as it was. */
  for (i = 0; i < NUM_OF_SENSORS_ON_PHONE; i++)
  {
    if ((camera_sensors[i].sensor_available == TRUE) &&
        (camera_sensors[i].sensor_id == sensor_id))
    {
      camera_asi = sensor_id;
      return CAMERA_SUCCESS;
    }
  }

  return CAMERA_NO_SENSOR;
#endif /* T_IO_CARD */
} /* camera_select_sensor */
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

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
camera_ret_code_type camera_svcs_start
(
  camera_cb_f_type callback,
  void *client_data
)
{

#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  rex_tcb_type *tcb;

  camera_ret_code_type camera_start_status = CAMERA_SUCCESS;

  camera_timer_up = TRUE;
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_START, 0, 0, __LINE__);
  MSG_HIGH("camera_start, camera_state %d", camera_state, 0, 0);
  dsp_camif_error_count = 0;

  if (camera_state != CAMERA_STATE_INIT)
  {
    if (callback)
    {
      (*callback) (CAMERA_EXIT_CB_FAILED, client_data, CAMERA_FUNC_START, 0);
    }
    event_report (EVENT_CAMERA_INVALID_STATE);
    MSG_ERROR("CAMERA_SVCS: Invalid state for camera_state %d", camera_state, 0, 0);
    return CAMERA_INVALID_STATE;
  }

  if (camera_stopping)
  {
    if (callback)
    {
      (*callback) (CAMERA_EXIT_CB_FAILED, client_data, CAMERA_FUNC_START, 0);
    }
    event_report (EVENT_CAMERA_INVALID_STATE);
    MSG_ERROR("CAMERA_SVCS: Invalid state for camera_stopping", 0, 0, 0);
    return CAMERA_INVALID_STATE;
  }

  tcb = rex_self();
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  graph_queue_camera_func ((uint32)CAMERA_FUNC_START, (void *)callback,
                           client_data, (uint32)tcb, (uint32)&camera_start_status, 0, 0);

  (void) rex_wait (CAMERA_SIG);
  (void) rex_clr_sigs (tcb, CAMERA_SIG);


  return camera_start_status;

#endif /* T_IO_CARD */
} /* camera_svcs_start */



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
camera_ret_code_type camera_svcs_stop
(
  camera_cb_f_type callback,
  void *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  rex_tcb_type *tcb;

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_STOP, 0, 0, __LINE__);
  MSG_HIGH ("camera_stop, camera_state:%d", camera_state, 0, 0);

  #ifdef FEATURE_CAMERA_MOBICAT_PREVIEW
  mpp_end();
  #endif //FEATURE_CAMERA_MOBICAT_PREVIEW
  
  if (camsensor_initialized == FALSE)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_STOP);
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_STOP);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
#ifdef  FEATURE_VIDEO_ENCODE 
  if(camera_state == CAMERA_STATE_RECORDING)
  {
    (void)camcorder_svcs_stop_record();
  }
#endif
  tcb = rex_self ();
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  graph_queue_camera_func ((uint32)CAMERA_FUNC_STOP, (void *)callback,
                           client_data, (uint32)tcb, 0, 0, 0);
  MSG_CAMERADEBUG("Waiting on CAMERA_SIG in camera_svcs_stop", 0, 0, 0 );
  (void) rex_wait (CAMERA_SIG);
  MSG_CAMERADEBUG("Got CAMERA_SIG in camera_svcs_stop", 0, 0, 0 );
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  graph_queue_camera_func ((uint32)CAMERA_FUNC_CLEANUP, (void *)callback,
                           client_data, (uint32)tcb, 0, 0, 0);
  MSG_CAMERADEBUG("Waiting on CAMERA_SIG in camera_svcs_stop for camera cleanup", 0, 0, 0 );
  (void) rex_wait (CAMERA_SIG);
  MSG_CAMERADEBUG("Got CAMERA_SIG in camera_svcs_stop after camera cleanup", 0, 0, 0 );
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  return CAMERA_SUCCESS;
#endif /* T_IO_CARD */
} /* camera_stop */



/*===========================================================================

FUNCTION      CAMERA_SVCS_SET_DIMENSIONS

DESCRIPTION
              Set picture and display sizes.
              This function is executed in the caller's context.

DEPENDENCIES
  Must be in READY state.
  ui_picture_width, ui_picture_height are used for recording.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_set_dimensions
(
  uint16           ui_picture_width,  /* width of the picture  */
  uint16           ui_picture_height, /* height of the picture */
  uint16           display_width,     /* width of display      */
  uint16           display_height,    /* height of display */
  camera_cb_f_type callback,
  void             *client_data
)
{
  uint32 i;
  boolean ui_is_portrait = FALSE;

  /* maximum cropping (smallest window) for zoom calculation */
  uint16 max_crop_width;
  uint16 max_crop_height;

  uint16 pad_lines = 0;
  uint16 pad_pixels = 0;

  camera_ret_code_type ret_code = CAMERA_SUCCESS;

  rex_tcb_type *tcb;

#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  MSG_HIGH("camera_set_dimension, picture width %d, picture height %d, display width %d", ui_picture_width, ui_picture_height, display_width);
  if (camsensor_initialized == FALSE)
  {
    MSG_ERROR ("No sensor", 0, 0, 0);
    ret_code = CAMERA_NO_SENSOR;
    event_report (EVENT_CAMERA_NO_SENSOR);
  }
  else if ((camera_state != CAMERA_STATE_READY) &&
           (camera_state != CAMERA_STATE_INIT))
  {
    MSG_ERROR ("Invalid state", 0, 0, 0);
    event_report (EVENT_CAMERA_INVALID_STATE);
    ret_code = CAMERA_INVALID_STATE;
  }

  if ((display_width == 0) || (display_height == 0))
  {
    MSG_ERROR ("Format not supported", 0, 0, 0);
    event_report (EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
    ret_code = CAMERA_INVALID_FORMAT;
  }

  if ((ui_picture_width  == 0) || (ui_picture_height == 0))
  {
    MSG_ERROR ("Format not supported", 0, 0, 0);
    event_report (EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
    ret_code = CAMERA_INVALID_FORMAT;
  }

  if (ret_code != CAMERA_SUCCESS)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_DIMENSIONS);
    return ret_code;
  }

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_SET_DIMENSIONS, 0, 0, __LINE__);
#ifndef CUST_EDITION
  /* Be smart about setting dimensions.  Note that internal buffer dimensions
     during preview/capture are ALWAYS landscape: so the larger UI dimension
     is the "width" and the smaller the height */
  if (ui_picture_width < ui_picture_height) /* Landscape from UI */
  {
    camera_svcs_swap_dimensions (&ui_picture_width, &ui_picture_height);
    ui_is_portrait  = TRUE;
  }
  if (display_width < display_height)
  {
    camera_svcs_swap_dimensions (&display_width, &display_height);
  }

  /* Limiting the display width & display height to QVGA resolution */
  if (display_width > CAMERA_QVGA_WIDTH)
  {
    display_width = CAMERA_QVGA_WIDTH;
  }

  if (display_height > CAMERA_QVGA_HEIGHT)
  {
    display_height = CAMERA_QVGA_HEIGHT;
  }
#endif
  /* Delimit the image size to maximum sensor output
   * The full_size_* includes necessary paddings for VFE, which is 12 pixels
   * per line and 8 lines (5 on the top and 3 at the bottom) for bayer sensors.
   * We need to subtract that first. For YCBCR sensors only have 2 pad lines.
   * One on the top and one on the bottom.
   */
  if( camsensor_static_params[camera_asi].output_format == CAMSENSOR_BAYER )
  {
    pad_pixels = 12;
    pad_lines = 8;
  }
  else
  {
    pad_lines = 2;
  }
  if ((ui_picture_width  > FLOOR16(camsensor_static_params[camera_asi].full_size_width  - pad_pixels))  ||
      (ui_picture_height > FLOOR16(camsensor_static_params[camera_asi].full_size_height - pad_lines)))
  {
    /* make the image dimensions multiple of 16 - JPEG requirement. Flooring
     * operation to multiple of 16 as upsampling is not supported yet.
     * Need to make sure we don't get a 0 as these values are used in division later.
     */
    /*lint -save -e820 */

    ui_picture_width  = FLOOR16(camsensor_static_params[camera_asi].full_size_width - pad_pixels);  
    ui_picture_height = FLOOR16(camsensor_static_params[camera_asi].full_size_height - pad_lines);
  
    if( !(ui_picture_width) ||
        !(ui_picture_height )
      )
    {
      MSG_ERROR ("Format not supported", 0, 0, 0);
      event_report (EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
      return CAMERA_INVALID_FORMAT;
    }
    /*lint -restore */
  }

  /* Force dimension to even */
  ui_picture_width  = CEILING2(ui_picture_width);
  ui_picture_height = CEILING2(ui_picture_height);

  if( ui_is_portrait )
  {
    ui_resolution[0] = ui_picture_height;
    ui_resolution[1] = ui_picture_width;
  }
  else
  {
    ui_resolution[0] = ui_picture_width;
    ui_resolution[1] = ui_picture_height;
  }

  /* Preserve original dimensions.
   * Always ensure:
   * orig_picture_dx <= ui_picture_width
   * orig_picture_dy <= ui_picture_height
   */
  orig_picture_dx = ui_picture_width;
  orig_picture_dy = ui_picture_height;

  /* Make sure thumbnail is not larger then picture */
  if (ui_thumbnail_width > orig_picture_dx || ui_thumbnail_height > orig_picture_dy)
  {
    thumbnail_width  = orig_picture_dx;
    thumbnail_height = orig_picture_dy;
  }
  else
  {
    thumbnail_width  = ui_thumbnail_width;
    thumbnail_height = ui_thumbnail_height;
  }

  /* Set the image size for snapshot */
  camera_dx = ui_picture_width;
  camera_dy = ui_picture_height;

  /* set preview size */
  camera_preview_dx = display_width;
  camera_preview_dy = display_height;

  /* Adjust zoom cropping */
  if ((camera_dx < display_width) || (camera_dy < display_height))
  {
    /* Camera fits preview size to provided display dimensions. There is now
     * more pixels needed to show the preview so update max_crop_width and
     * max_crop_height.
     */
    max_crop_width  = display_width;
    max_crop_height = display_height;
  }
  else
  {
    max_crop_width  = CEILING16(orig_picture_dx);
    max_crop_height = CEILING16(orig_picture_dy);
  }

  /* if preview size and image size are not the same, resize overlay
   * frame to match preview size
   */
  if( (display_width != orig_picture_dx) && (display_height != orig_picture_dy) )
  {
    scale_preview_overlay = TRUE;
  }
  else
  {
    scale_preview_overlay = FALSE;
  }

  if ((max_crop_width != 0) && (max_crop_height != 0))
  {
    /* Calculating resize factor as a percentage. Use the smaller one. */
    if ((camsensor_static_params[camera_asi].full_size_width * Q12 / max_crop_width) >
        (camsensor_static_params[camera_asi].full_size_height * Q12 / max_crop_height))
    {
      resize_factor = (camsensor_static_params[camera_asi].full_size_height * Q12 / max_crop_height);
    }
    else
    {
      resize_factor = (camsensor_static_params[camera_asi].full_size_width * Q12 / max_crop_width);
    }
  }

  if ((display_width != 0) && (display_height != 0))
  {
    /* calculate max zoom based off sensor preview decimation */
    if ((max_crop_width * Q12 / display_width) > (max_crop_height * Q12 / display_height))
    {
      camera_preview_decimation = max_crop_height  * Q12 / display_height;
    }
    else
    {
      camera_preview_decimation = max_crop_width * Q12 / display_width;
    }
  }

  /* Sensor decides the minimum decimation it can handle */
  if (camera_preview_decimation < camsensor_static_params[camera_asi].preview_dy_decimation)
  {
    camera_min_decimation = (camsensor_static_params[camera_asi].preview_dy_decimation * Q12) / camera_preview_decimation;
    if (camera_min_decimation < Q12)
    {
      camera_min_decimation = Q12;
    }
  }
  else
  {
    camera_min_decimation = Q12;
  }

  /* Compute min and max zoom values
   *
   * Now that we know the resize factor, figure out which entry in our zoom
   * table equals this zoom factor. This entry actually will correspond to
   * NO zoom, since it will tell the VFE to crop the entire image. Entry
   * min_decimation (often 0) in the table, is the smallest amount we
   * can crop, which is max zoom.
   */
  {
    boolean found_min = FALSE;
    for (i = 0; i < CAMERA_ZOOM_TABLE_MAX-1; i++)
    {
      if (camera_min_decimation < camera_zoom_table[i+1] && !found_min)
      {
        camera_parm_zoom_table.minimum_value = (int32)i;
        found_min = TRUE;
      }
      if (resize_factor < camera_zoom_table[i+1])
      {
        camera_parm_zoom_table.maximum_value = (int32)i;
        /* Since Min must be less than Max we are done */
        break;
      }
    }

#ifdef FEATURE_CAMERA_ZOOM_TABLE2
    /* If number of zoom steps too small, use camera_zoom_table2 */
    if (i < CAMERA_MIN_ZOOM_STEPS)
    {
      found_min = FALSE;
      for (i = 0; i < CAMERA_ZOOM_TABLE_MAX-1; i++)
      {
        if (camera_min_decimation < camera_zoom_table2[i+1] && !found_min)
        {
          camera_parm_zoom_table.minimum_value = (int32)i;
          found_min = TRUE;
        }
        if (resize_factor < camera_zoom_table2[i+1])
        {
          camera_parm_zoom_table.maximum_value = (int32)i;
          /* Since Min must be less than Max we are done */
          break;
        }
      }
      camera_zoom_table_ptr = camera_zoom_table2;
    }
    else
    {
      camera_zoom_table_ptr = camera_zoom_table;
    }
#endif /* FEATURE_CAMERA_ZOOM_TABLE2 */
    /* if value not found, handle gracefully */
    if (i == CAMERA_ZOOM_TABLE_MAX)
    {
      MSG_HIGH("Could not find entry in zoom table denoting min zoom", 0, 0, 0);
      camera_parm_zoom_table.maximum_value = 0;
      camera_parm_zoom_table.minimum_value = 0;
    }
    else if(!found_min)
    {
      MSG_HIGH("Could not find entry in zoom table denoting max zoom", 0, 0, 0);
      camera_parm_zoom_table.minimum_value = 0;
    }
    else if(camera_parm_zoom_table.maximum_value < camera_parm_zoom_table.minimum_value)
    {
      MSG_HIGH("Error calculating min and max zoom", 0, 0, 0);
      camera_parm_zoom_table.maximum_value = 0;
      camera_parm_zoom_table.minimum_value = 0;
    }
  }

  /* setup ui values */
  camera_parm_zoom.minimum_value = 0;
  camera_parm_zoom.maximum_value = (camera_parm_zoom_table.maximum_value -
                                    camera_parm_zoom_table.minimum_value);

  /* if we always want to have 10 zoom steps (a good feature until the UI can
   * handle press and hold zooming where say, 60 steps (4x), zooming does
   * not require 60 key presses, then lets do the following
   */
  if (camera_parm_zoom.maximum_value > CAMERA_MAX_ZOOM_STEPS-1)
  {
    zoom_step_size = (uint32)(camera_parm_zoom.maximum_value/(CAMERA_MAX_ZOOM_STEPS-1));
    camera_parm_zoom.maximum_value = CAMERA_MAX_ZOOM_STEPS-1;
  }
  else
  {
    zoom_step_size = 1;
  }

  camera_parm_zoom.current_value = camera_parm_zoom.minimum_value;
  camera_parm_zoom.default_value = camera_parm_zoom.current_value;
  camera_parm_zoom.step_value    = 1;

  /* If we have say computed 34 zoom steps, but we want to have
   * 10 steps in the UI, then we move 34/(10-1), or 3 steps in the zoom
   * table per click. However, that would take us to 3*9, 27, not 34.
   *
   * 0 3 6 9 12 15 18 21 24 27
   *
   * So we need to bump each value by bump number, specifically,
   *
   * 0 1 2 3  4  5  6  6  7  7
   *
   * which will give us
   *
   * 0 4 8 12 16 20 24 27 31 34
   *
   * This bit of code sets up that bump table
   */
  if ((camera_parm_zoom_table.maximum_value -
       camera_parm_zoom_table.minimum_value) > (CAMERA_MAX_ZOOM_STEPS-1))
  {
    int32 num;

    num = ((camera_parm_zoom_table.maximum_value -
            camera_parm_zoom_table.minimum_value)%(CAMERA_MAX_ZOOM_STEPS-1));
    if (num < 0)
    {
      num = 0;
      MSG_ERROR("Zoom calculation failed.", 0, 0, 0);
      event_report (EVENT_CAMERA_INVALID_STATE);
    }
    for (i = 0; i < CAMERA_MAX_ZOOM_STEPS; i++)
    {
      /*lint -save -e734 -e737 */
      camera_zoom_table_bump[i] = ((i<<4)/(CAMERA_MAX_ZOOM_STEPS-1)) * num;
      camera_zoom_table_bump[i] >>= 4;
      /*lint -restore */
    }
  }
  else
  {
    memset (camera_zoom_table_bump, 0, sizeof(camera_zoom_table_bump));
  }

  /* 4:2:0 = X * Y * 1.5
   * 4:2:2 = X * Y * 2
   * Set to max size. These are used for camcorder.
   */
  camera_preview_buffer_size = camera_preview_dx * camera_preview_dy * 2 + qcamraw_header_size;
  camera_encode_buffer_size  = camera_dx * camera_dy * 2 + qcamraw_header_size;

#ifdef FEATURE_VIDEO_ENCODE  
  /* Update the sizes used for the flusing data */
  allocBufSize = camera_encode_buffer_size;
  rawHeaderSize = qcamraw_header_size;
#endif

  /* If height of preview (dy) is not a multiple of 16 then
    add an extra dx line to pad it out one line when sending to the MDP */
  if (camera_preview_dy & 0xF)
  {
    /* Store value of padding to global variable for use later */
    camera_preview_pad_for_mdp = camera_preview_dx;
    /* Add padding of output preview buffer */
    camera_preview_buffer_size += camera_preview_pad_for_mdp;
  }
  else
  {
    /* Else clear to zero to prevent unneeded work around for MDP */
    camera_preview_pad_for_mdp = 0;
  }

  if (camsensor_static_params[camera_asi].sensor_features.zoom == TRUE)
  {
    if ((camsensor_static_params[camera_asi].full_size_width > display_width) &&
      (camsensor_static_params[camera_asi].full_size_height > display_height))
    {
      camera_parm_zoom.maximum_value = (int32) (CAMERA_MAX_ZOOM_STEPS-1);
    }
    if (camsensor_set_zoom ((void *) (&camera_parm_zoom.current_value)) != CAMERA_SUCCESS)
    {
      MSG_HIGH ("Cannot set the sensor's zoom", 0, 0, 0);
    }
  }
  else
  {
    /* Now calculate current zoom */
    camera_calculate_zoom ();

    tcb = rex_self();
    (void) rex_clr_sigs (tcb, CAMERA_SIG);

    graph_queue_camera_func ((uint32)CAMERA_FUNC_SET_DIMENSIONS,
                           (void *)callback, client_data, (uint32)tcb, 0, 0, 0);

    (void) rex_wait (CAMERA_SIG);
    (void) rex_clr_sigs (tcb, CAMERA_SIG);
  }
  return ret_code;

#endif /* T_IO_CARD */
} /* camera_svcs_set_dimensions */


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
camera_ret_code_type camera_svcs_start_preview
(
  camera_cb_f_type callback,
  void *client_data
)
{
  camera_ret_code_type ret_code = CAMERA_SUCCESS;

  camera_snapshot_timing.viewfinder_start = (uint32)timetick_get_ms();
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_START_PREVIEW, 0, 0, __LINE__);
  MSG_HIGH("camera_start_preview, camera_state %d", camera_state, 0, 0);
#ifdef FEATURE_QVPHONE
  if (camera_qvp_enabled)
  {
    MSG_ERROR("Invalid state", 0, 0, 0);
    event_report(EVENT_CAMERA_INVALID_STATE);
    ret_code = CAMERA_INVALID_STATE;
  }
#endif /* FEATURE_QVPHONE */

  if (camsensor_initialized == FALSE)
  {
    /* Need to power on sensor, initialize it, power it up, before continue */
    MSG_HIGH("CAMERA_SVCS: Call POWER ON sequence in camera_svcs_start_preview", 0, 0, 0);
    camsensor_power_on();
    camsensor_power_up();
    MSG_CAMERADEBUG("CAMERA_SVCS: Configure GPIO settings", 0, 0, 0);
    camsensor_config_sensor_gpios_clks ();
  }
#ifdef FEATURE_VIDEO_ENCODE
  if (camera_video_engine_stop_requested)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    ret_code = CAMERA_INVALID_STATE;
  }
#endif
#ifdef  FEATURE_STROBE_FLASH
  if (camera_flash_on || camera_flash_auto)
    strobe_flash_start_charging();
#endif /* FEATURE_STROBE_FLASH */

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if (ret_code != CAMERA_SUCCESS)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_START_PREVIEW);
    return ret_code;
  }

  camera_take_images = FALSE;

  graph_queue_camera_func ((uint32)CAMERA_FUNC_START_PREVIEW,
                           (void *)callback, client_data, 0, 0, 0, 0);

  return ret_code;

#endif /* T_IO_CARD */
} /* camera_start_preview */


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

camera_ret_code_type camera_svcs_stop_preview(void)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  rex_tcb_type *tcb;

  camera_snapshot_timing.stop_viewfinder_start = (uint32)timetick_get_ms();

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_STOP_PREVIEW, 0, 0, __LINE__);
  MSG_HIGH ("camera_stop_preview",0,0,0);
#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state != CAMERA_STATE_PREVIEW)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
  camera_take_images = FALSE;

#ifdef FEATURE_MDP
#if defined FEATURE_VIDEO_ENCODE
  /* De-register the video */
  if (preview_mode == CAMERA_PREVIEW_MODE_MOVIE)
  {
#ifdef FEATURE_MMOVERLAY
    camera_deregister_video (FALSE);
#else /* FEATURE_MMOVERLAY */
    camera_deregister_video (TRUE);
#endif /* !FEATURE_MMOVERLAY */
  }
  else
#endif /* defined FEATURE_VIDEO_ENCODE */
  {
    camera_deregister_video (TRUE);
  }
  /* De-register the overlay layer */
  camera_deregister_layer1();
  CAM_Free (picture_preview_copy);
  picture_preview_copy = NULL;
#endif /* FEATURE_MDP */

  /* Block caller until function is executed */
  tcb = rex_self();
  (void)rex_clr_sigs(tcb, CAMERA_SIG);
  graph_queue_camera_func ((uint32)CAMERA_FUNC_STOP_PREVIEW, (void *)0,
                           (void *)0, (uint32)tcb, 0, 0, 0);
  MSG_CAMERADEBUG("Waiting on CAMERA_SIG in camera_svcs_stop_preview", 0, 0, 0 );
  (void)rex_wait(CAMERA_SIG);
  MSG_CAMERADEBUG("Got CAMERA_SIG in camera_svcs_stop_preview", 0, 0, 0 );
  (void)rex_clr_sigs(tcb, CAMERA_SIG);
  return CAMERA_SUCCESS;

#endif /* T_IO_CARD */
} /* camera_stop_preview */


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

camera_ret_code_type camera_svcs_take_picture
(
  camera_cb_f_type callback,
  void * client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_ret_code_type ret_code = CAMERA_SUCCESS;

  camera_snapshot_timing.take_snapshot_start = (uint32)timetick_get_ms();

  camera_log(LOG_FUNC, (uint32)CAMERA_FUNC_TAKE_PICTURE, 0, 0, __LINE__);
  MSG_HIGH("camera_take_picture, camera_state %d", camera_state, 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if (camsensor_initialized == FALSE)
  {
    ret_code = CAMERA_NO_SENSOR;
    event_report(EVENT_CAMERA_NO_SENSOR);
  }
  else if ((camera_dx == 0) || (camera_dy == 0))
  {
    MSG_ERROR("Format not supported", 0, 0, 0);
    event_report(EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
    ret_code = CAMERA_INVALID_FORMAT;
  }

#ifdef FEATURE_STROBE_FLASH
  if ((camera_fire_flash) && (strobe_flash_is_ready() == FALSE)) {
    /* Need to add a different state and event type for strobe */
    ret_code = CAMERA_EXIT_CB_ERROR;
    camera_fire_flash = FALSE;
  }
#endif /* FEATURE_STROBE_FLASH */

  if (ret_code != CAMERA_SUCCESS)
  {
    camera_reject(callback, client_data, CAMERA_FUNC_TAKE_PICTURE);
    return ret_code;
  }
#ifdef ANY_FLASH

  if (camera_flash_on){
    camera_fire_flash = TRUE;
  }
  else
  {
    if (camera_flash_auto &&( camsensor_static_params[camera_asi].output_format != CAMSENSOR_YCBCR)\
                                                                     &&(camera_aec_flash_needed()))
    {
      camera_fire_flash = TRUE;
    }
    else
    {
      camera_fire_flash = FALSE;
    }
  }
#endif /* ANY_FLASH */

#ifdef FEATURE_STROBE_FLASH
  if ((camera_fire_flash) && (camera_parm_red_eye_reduction.current_value))
     strobe_flash_redeye_reduction();
#endif /* FEATURE_STROBE_FLASH */

  /* If AF is active, then stop it */
  if (camera_af_is_active())
  {
    camera_af_stop_focus();
  }

  camera_take_images = FALSE;
  camera_take_picture_retry = CAMERA_TAKE_PICTURE_RETRY_NONE;

  graph_queue_camera_func((uint32)CAMERA_FUNC_TAKE_PICTURE, (void *)callback, client_data, 0, 0, 0, 0);

  return ret_code;

#endif /* nT_IO_CARD */

} /* camera_take_picture */


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

camera_ret_code_type camera_svcs_encode_picture
(
  camera_frame_type *frame,
  camera_handle_type *handle,
  camera_cb_f_type callback,
  void * client_data
)
{
  camera_ret_code_type ret_val = CAMERA_SUCCESS;

  camera_snapshot_information.encode_picture_width = (uint32) frame->captured_dx;
  camera_snapshot_information.encode_picture_height = (uint32) frame->captured_dy;

  camera_log(LOG_FUNC, (uint32)CAMERA_FUNC_ENCODE_PICTURE, 0, 0, __LINE__);
  MSG_HIGH("camera_encode_picture", 0, 0, 0);

#ifdef FEATURE_QVPHONE
  if (camera_qvp_enabled)
  {
    MSG_ERROR("Invalid state", 0, 0, 0);
    event_report(EVENT_CAMERA_INVALID_STATE);
    ret_val = CAMERA_INVALID_STATE;
  }
#endif /* FEATURE_QVPHONE */
  if (camsensor_initialized == FALSE)
  {
    ret_val = CAMERA_NO_SENSOR;
    event_report(EVENT_CAMERA_NO_SENSOR);
  }
  else if (camera_state == CAMERA_STATE_INIT)
  {
    MSG_ERROR("Invalid state", 0, 0, 0);
    event_report(EVENT_CAMERA_INVALID_STATE);
    ret_val = CAMERA_INVALID_STATE;
  }

  if (ret_val == CAMERA_SUCCESS) {
    ret_val = camera_encode_picture_common( frame, handle, callback, client_data );
  }

  return ret_val;

} /* camera_encode_picture */


/*===========================================================================

FUNCTION      CAMERA_ENCODE_PICTURE_COMMON

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

static camera_ret_code_type camera_encode_picture_common
(
  camera_frame_type *frame,
  camera_handle_type *handle,
  camera_cb_f_type callback,
  void * client_data
)
{
  camera_ret_code_type ret_code = CAMERA_SUCCESS;

  qcamrawHeaderType *main_image_header;
  uint8 *main_image_buffer = NULL;
  uint32 main_image_y_size;
  uint32 main_image_cbcr_size;

  qcamrawHeaderType *thumbnail_image_header;
  uint8 *thumbnail_image_buffer;
  uint32 thumbnail_image_y_size;
  uint32 thumbnail_image_cbcr_size;

  qcamrawHeaderType *raw_image_header;
  uint8  *raw_image_buffer;
  uint32 raw_image_size;

  qcamrawDataType data_type = QCAMRAW_FORMAT_INVALID;

#ifdef FEATURE_PNG_ENCODER
  qcamrawHeaderType *png_image_header;
  uint8  *png_image_buffer;
#endif /* FEATURE_PNG_ENCODER */

  uint8  pad_size = 0;

  camera_handle = *handle;

  /* Both thumbnail and main image are in the same format */
  if (((frame->format != CAMERA_H2V1) &&
       (frame->format != CAMERA_H2V2) &&
       (frame->format != CAMERA_BAYER_8BIT) &&
       (frame->format != CAMERA_BAYER_10BIT) &&
       (frame->format != CAMERA_YCBCR_4_2_2)))

  {
    MSG_ERROR("Invalid format", 0, 0, 0);
    event_report(EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
    ret_code = CAMERA_INVALID_FORMAT;
  }
  else if((camera_encode_properties.format != CAMERA_JPEG) &&
#ifdef FEATURE_PNG_ENCODER
           (camera_encode_properties.format != CAMERA_PNG)  &&
#endif /* FEATURE_PNG_ENCODER */
           (camera_encode_properties.format != CAMERA_RAW)  &&
           (camera_encode_properties.format != CAMERA_YCBCR_ENCODE))
  {
    MSG_ERROR("Invalid format", 0, 0, 0);
    event_report(EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
    ret_code = CAMERA_INVALID_FORMAT;
  }

  if (ret_code != CAMERA_SUCCESS)
  {
    camera_reject(callback, client_data, CAMERA_FUNC_ENCODE_PICTURE);
    return ret_code;
  }

  camera_take_images = FALSE;

  /* JPEG Encoding */
  if (camera_encode_properties.format == CAMERA_JPEG)
  {
    /* Assign header and buffer pointers. If the main and thumbnail image
     * does not have  qcamraw header, then use
     * my_main_image_header, my_thumbnail_image_header. */
    if (qcamrawIsHeader(frame->buffer) &&
        qcamrawIsHeader(frame->thumbnail_image))
    {
      main_image_header = (qcamrawHeaderType *) frame->buffer;
      main_image_buffer = frame->buffer + main_image_header->headerSize + main_image_header->padSize;

      thumbnail_image_header = (qcamrawHeaderType *) frame->thumbnail_image;
      thumbnail_image_buffer = frame->thumbnail_image + thumbnail_image_header->headerSize + thumbnail_image_header->padSize;
    }
    else
    {
      /* No header information, suppy one */
      main_image_header      = &my_main_image_header;
      thumbnail_image_header = &my_thumbnail_image_header;

      main_image_buffer      = frame->buffer;
      thumbnail_image_buffer = frame->thumbnail_image;

      if ((frame->format == CAMERA_H2V1) ||
          (frame->format == CAMERA_H2V2))
      {
        data_type = QCAMRAW_H2V1;

        main_image_y_size    = frame->captured_dx * frame->captured_dy;
        main_image_cbcr_size = frame->captured_dx * frame->captured_dy;

        thumbnail_image_y_size    = frame->dx * frame->dy;
        thumbnail_image_cbcr_size = frame->dx * frame->dy;

        if (frame->format == CAMERA_H2V2)
        {
          main_image_cbcr_size = frame->captured_dx * frame->captured_dy / 2;
          thumbnail_image_cbcr_size = frame->dx * frame->dy / 2;
          data_type = QCAMRAW_H2V2;
        }

        /* Ensure that Y, CbCr blocks are 32-bit aligned for main image */
        pad_size = (main_image_y_size % 4) ? (4 - (main_image_y_size % 4)) : 0;

        /* Set the header for main image */
        qcamrawSetHeader((qcamrawHeaderType *) &my_main_image_header, 0,
                          frame->captured_dx * frame->captured_dy,
                          data_type, QCAMRAW_PICTURE_FRAME, QCAMRAW_NO_REFLECT,
                          frame->rotation, orig_picture_dx, orig_picture_dy,
                          frame->captured_dx, frame->captured_dy,
                          0, 0, 0, 0, main_image_y_size, pad_size,
                          main_image_cbcr_size,
                          main_image_buffer + main_image_y_size + pad_size);

        /* Ensure that Y, CbCr blocks are 32-bit aligned for thumb nail */
        pad_size = (thumbnail_image_y_size % 4) ? (4 - (thumbnail_image_y_size % 4)) : 0;

        /* Set the header for thumbnail image */
        qcamrawSetHeader((qcamrawHeaderType *) &my_thumbnail_image_header, 0,
                         frame->dx * frame->dy, data_type, QCAMRAW_PICTURE_FRAME,
                         QCAMRAW_NO_REFLECT, frame->rotation, frame->dx, frame->dy,
                         frame->dx, frame->dy, 0, 0, 0, 0,
                         thumbnail_image_y_size, pad_size,
                         thumbnail_image_cbcr_size,
                         thumbnail_image_buffer + thumbnail_image_y_size + pad_size);
      }
    }

    graph_queue_camera_func((uint32)CAMERA_FUNC_ENCODE_PICTURE, (void *)callback, client_data, (uint32) main_image_header, (uint32) main_image_buffer, (uint32) thumbnail_image_header, (uint32) thumbnail_image_buffer);
  }
#ifdef FEATURE_PNG_ENCODER
  else if (camera_encode_properties.format == CAMERA_PNG)
  {
      if (qcamrawIsHeader(frame->buffer))
      {
          png_image_header = (qcamrawHeaderType *) frame->buffer;
          png_image_buffer = frame->buffer + png_image_header->headerSize + png_image_header->padSize;

      }
      else
      {
          png_image_header      = &my_png_image_header;
          png_image_buffer      = frame->buffer;
      }

      graph_queue_camera_func((uint32)CAMERA_FUNC_ENCODE_PICTURE, (void *)callback, client_data, (uint32) png_image_header, (uint32) png_image_buffer, 0, 0);
  }
#endif /* FEATURE_PNG_ENCODER */
  else if (camera_encode_properties.format == CAMERA_YCBCR_ENCODE)
  {
     /* Assign header and buffer pointers. If the main and thumbnail image
      * does not have  qcamraw header, then use
      * my_main_image_header, my_thumbnail_image_header. */
    if (qcamrawIsHeader(frame->buffer) &&
        qcamrawIsHeader(frame->thumbnail_image))
    {
      main_image_header = (qcamrawHeaderType *) frame->buffer;
      main_image_buffer = frame->buffer + main_image_header->headerSize + main_image_header->padSize;

      thumbnail_image_header = (qcamrawHeaderType *) frame->thumbnail_image;
      thumbnail_image_buffer = frame->thumbnail_image + thumbnail_image_header->headerSize + thumbnail_image_header->padSize;
    }
    else
    {
      /* No header information, suppy one */
      main_image_header      = &my_main_image_header;
      thumbnail_image_header = &my_thumbnail_image_header;

      main_image_buffer      = frame->buffer;
      thumbnail_image_buffer = frame->thumbnail_image;

      if ((frame->format == CAMERA_H2V1) ||
          (frame->format == CAMERA_H2V2))
      {
        data_type = QCAMRAW_H2V1;

        main_image_y_size    = frame->captured_dx * frame->captured_dy;
        main_image_cbcr_size = frame->captured_dx * frame->captured_dy;

        thumbnail_image_y_size    = frame->dx * frame->dy;
        thumbnail_image_cbcr_size = frame->dx * frame->dy;

        if (frame->format == CAMERA_H2V2)
        {
          main_image_cbcr_size = frame->captured_dx * frame->captured_dy / 2;
          thumbnail_image_cbcr_size = frame->dx * frame->dy / 2;
          data_type = QCAMRAW_H2V2;
        }

        /* Ensure that Y, CbCr blocks are 32-bit aligned for main image */
        pad_size = (main_image_y_size % 4) ? (4 - (main_image_y_size % 4)) : 0;

        /* Set the header for main image */
        qcamrawSetHeader((qcamrawHeaderType *) &my_main_image_header, 0,
                          frame->captured_dx * frame->captured_dy,
                          data_type, QCAMRAW_PICTURE_FRAME, QCAMRAW_NO_REFLECT,
                          frame->rotation, orig_picture_dx, orig_picture_dy,
                          frame->captured_dx, frame->captured_dy,
                          0, 0, 0, 0, main_image_y_size, pad_size,
                          main_image_cbcr_size,
                          main_image_buffer + main_image_y_size + pad_size);

        /* Ensure that Y, CbCr blocks are 32-bit aligned for thumb nail */
        pad_size = (thumbnail_image_y_size % 4) ? (4 - (thumbnail_image_y_size % 4)) : 0;

        /* Set the header for thumbnail image */
        qcamrawSetHeader((qcamrawHeaderType *) &my_thumbnail_image_header, 0,
                         frame->dx * frame->dy, data_type, QCAMRAW_PICTURE_FRAME,
                         QCAMRAW_NO_REFLECT, frame->rotation, frame->dx, frame->dy,
                         frame->dx, frame->dy, 0, 0, 0, 0,
                         thumbnail_image_y_size, pad_size,
                         thumbnail_image_cbcr_size,
                         thumbnail_image_buffer + thumbnail_image_y_size + pad_size);
      }
    }

    graph_queue_camera_func((uint32)CAMERA_FUNC_ENCODE_PICTURE, (void *)callback, client_data, (uint32) main_image_header, (uint32) main_image_buffer, (uint32) thumbnail_image_header, (uint32) thumbnail_image_buffer);    
  }
  else
  {
    /* Raw capture */

    /* Assign header and buffer pointers. If the raw image
     * does not have  qcamraw header, then use  my_raw_image_header.
     */
    if (qcamrawIsHeader(frame->buffer))
    {
      raw_image_header = (qcamrawHeaderType *) frame->buffer;
      raw_image_buffer = frame->buffer + raw_image_header->headerSize + raw_image_header->padSize;

    }
    else
    {
      raw_image_header      = &my_raw_image_header;
      raw_image_buffer      = frame->buffer;
      raw_image_size    = vfeRawConfig.burstFactor2ForBuffers * vfeRawConfig.burstFactor1ForBuffers * sizeof(uint32) * vfeRawConfig.burstLenForBuffers;

      switch (frame->format)
      {
        case CAMERA_YCBCR_4_2_2:
          data_type = QCAMRAW_YCBYCR;
          break;
        case CAMERA_BAYER_8BIT:
          data_type = QCAMRAW_BAYER_8BIT;
          break;
        case CAMERA_BAYER_10BIT:
          data_type = QCAMRAW_BAYER_10BIT;
          break;
        default:
          break;
      }

      /* Set the header for raw image */
      qcamrawSetHeader((qcamrawHeaderType *) &my_raw_image_header, 0,
                       frame->captured_dx * frame->captured_dy,
                       data_type, QCAMRAW_PICTURE_FRAME, QCAMRAW_NO_REFLECT,
                       frame->rotation, frame->captured_dx, frame->captured_dy,
                       frame->captured_dx, frame->captured_dy, 0, 0, 0, 0,
                       raw_image_size, 0, 0, NULL);
    }
    graph_queue_camera_func((uint32)CAMERA_FUNC_ENCODE_PICTURE, (void *)callback, client_data, (uint32) raw_image_header, (uint32) raw_image_buffer, 0, 0);
  }
  return CAMERA_SUCCESS;
} /* camera_encode_picture_common */

/*===========================================================================

FUNCTION      CAMERA_SVCS_GET_THUMBNAIL_PROPERTIES

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
camera_ret_code_type camera_svcs_get_thumbnail_properties
(
  uint32 *width,   /* width of the thumbnail */
  uint32 *height,  /* height of the thumbnail */
  uint32 *quality  /* encode quality of the thumbnail */
  /* When encoding, picture of this size and up must have thumbnail */
  //uint32 *picture_size
)
{
  *width   = thumbnail_width;
  *height  = thumbnail_height;
  *quality = thumbnail_quality;
  return CAMERA_SUCCESS;
}


/*===========================================================================

FUNCTION      CAMERA_SVCS_SET_THUMBNAIL_PROPERTIES

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
camera_ret_code_type camera_svcs_set_thumbnail_properties
(
  uint32 width,   /* width of the thumbnail */
  uint32 height,  /* height of the thumbnail */
  uint32 quality  /* encode quality of the thumbnail */
  /* When encoding, picture of this size and up must have thumbnail */
  //uint32 picture_size
)
{
  MSG_HIGH("camera_set_thumbnail_properties, width %d, height %d, quality %d", width, height, quality);
  if ((width >= THUMBNAIL_MIN_DIMENSION &&
       width <= THUMBNAIL_MAX_DIMENSION) &&
      (height >= THUMBNAIL_MIN_DIMENSION &&
       height <= THUMBNAIL_MAX_DIMENSION) &&
      (quality >= THUMBNAIL_MIN_QUALITY &&
       quality <= THUMBNAIL_MAX_QUALITY))
  {
    /* Make sure thumbnail is in landscape mode */
    if (width < height)
    {
      ui_thumbnail_width = height;
      ui_thumbnail_height = width;
    }
    else
    {
      ui_thumbnail_width = width;
      ui_thumbnail_height = height;
    }
    /* Make sure thumbnail is not larger than picture */
    if (ui_thumbnail_width > orig_picture_dx || ui_thumbnail_height > orig_picture_dy)
    {
      thumbnail_width  = orig_picture_dx;
      thumbnail_height = orig_picture_dy;
    }
    else
    {
      thumbnail_width  = ui_thumbnail_width;
      thumbnail_height = ui_thumbnail_height;
    }
    if (quality != thumbnail_quality)
    {
      thumbnail_quality = quality;
    }
    return CAMERA_SUCCESS;
  }
  else
  {
    return CAMERA_INVALID_PARM;
  }
}

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
camera_ret_code_type camera_svcs_release_picture (void)
{
  rex_tcb_type *tcb;

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_RELEASE_PICTURE, 0, 0, __LINE__);
  MSG_HIGH ("camera_release_picture, camera_state:%d", camera_state, 0, 0);

  if (camsensor_initialized == FALSE)
  {
    MSG_ERROR("No sensor, camera_state %d", camera_state, 0, 0);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    MSG_ERROR("Invalid state for camera_state %d", camera_state, 0, 0);
    return CAMERA_INVALID_STATE;
  }

  tcb = rex_self ();
  (void) rex_clr_sigs (tcb, CAMERA_SIG);
  graph_queue_camera_func ((uint32)CAMERA_FUNC_RELEASE_PICTURE, (void *)0,
                           (void *)0, (uint32)tcb, 0, 0, 0);
  MSG_CAMERADEBUG("Waiting on CAMERA_SIG in camera_svcs_release_picture", 0, 0, 0 );
  (void) rex_wait (CAMERA_SIG);
  MSG_CAMERADEBUG("Got CAMERA_SIG in camera_svcs_release_picture", 0, 0, 0 );
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  return CAMERA_SUCCESS;
} /* camera_release_picture */

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
camera_ret_code_type camera_svcs_enable_frame_callback (void)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_ENABLE_FRAME_CALLBACK, 0, 0, __LINE__);
  MSG_HIGH("camera_enable_frame_callback, camera_state %d", camera_state, 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }
  if ((camera_state == CAMERA_STATE_PREVIEW)      ||
      (camera_state == CAMERA_STATE_QVP_ENCODING) ||
      (camera_state == CAMERA_STATE_RECORDING))
  {
    graph_queue_camera_func ((uint32) CAMERA_FUNC_ENABLE_FRAME_CALLBACK,
                             (void *) 0, (void *)0, 0, 0, 0, 0);
    return CAMERA_SUCCESS;
  }
  else
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
#endif /* T_IO_CARD */
} /* camera_enable_frame_callback */

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
camera_ret_code_type camera_svcs_disable_frame_callback (void)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_DISABLE_FRAME_CALLBACK, 0, 0, __LINE__);
  MSG_HIGH("camera_disable_frame_callback, camera_state %d", camera_state, 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }
  if ((camera_state == CAMERA_STATE_PREVIEW)      ||
      (camera_state == CAMERA_STATE_QVP_ENCODING) ||
      (camera_state == CAMERA_STATE_RECORDING))
  {
    graph_queue_camera_func ((uint32) CAMERA_FUNC_DISABLE_FRAME_CALLBACK,
                             (void *)0, (void *)0, 0, 0, 0, 0);
    return CAMERA_SUCCESS;
  }
  else
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
#endif /* T_IO_CARD */
} /* camera_disable_frame_callback */

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
camera_ret_code_type camera_svcs_release_frame (void)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_RELEASE_FRAME, 0, 0, __LINE__);
  MSG_CAMERADEBUG("camera_release_frame, camera_state %d", camera_state, 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }
  graph_queue_camera_func ((uint32)CAMERA_FUNC_RELEASE_FRAME, (void *)0,
      (void *)0, CAMERA_INVALID_BUF_INDEX, 0, 0, 0);
  return CAMERA_SUCCESS;
#endif /* T_IO_CARD */
} /* camera_release_frame */


/*===========================================================================

FUNCTION      CAMERA_SVCS_SET_OVERLAY

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

camera_ret_code_type camera_svcs_set_overlay
(
  uint16            dx,
  uint16            dy,
  uint32            transparent_color,
  void             *overlay_bmp,
  camera_cb_f_type  callback,
  void             *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  rex_tcb_type *tcb;
  ipl_status_type status;
  ipl_image_type work;
  uint32          size = dx * dy * 2;

  work.imgPtr = NULL;
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_SET_OVERLAY, 0, 0, __LINE__);
  MSG_HIGH("camera_set_overlay, camera_state %d", camera_state, 0, 0);

  if (camera_state == CAMERA_STATE_INIT)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_OVERLAY);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  if (camsensor_initialized == FALSE)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_OVERLAY);
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  /* if this is a clear overlay instruction from application */
  if (overlay_bmp == NULL && preview_mode == CAMERA_PREVIEW_MODE_SNAPSHOT) {
    camera_clr_overlays();
    return CAMERA_SUCCESS;
  }

  /* reject overlay frame if picture size != overlay size. */
#if defined FEATURE_VIDEO_ENCODE
  /* For video, it is camera_dx and camera_dy. */
  if (preview_mode == CAMERA_PREVIEW_MODE_MOVIE)
  {
    if (!(((camera_dx == dy) &&
          (camera_dy == dx))  ||
        ((camera_dx == dx) &&
          (camera_dy == dy))))
    {
      camera_reject(callback, client_data, CAMERA_FUNC_SET_OVERLAY);
      event_report(EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
      return CAMERA_INVALID_FORMAT;
    }
  }
  else
#endif /* defined FEATURE_VIDEO_ENCODE */
  {
    if (!(((orig_picture_dx == dy) &&
          (orig_picture_dy == dx))  ||
        ((orig_picture_dx == dx) &&
          (orig_picture_dy == dy))))
    {
      camera_reject(callback, client_data, CAMERA_FUNC_SET_OVERLAY);
      event_report(EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
      return CAMERA_INVALID_FORMAT;
    }
  }

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  /* transparent color must be the same as icon's */
  if (camera_icons)
  {
    if (transparent_color != camera_transparent_color)
    {
      camera_reject(callback, client_data, CAMERA_FUNC_SET_OVERLAY);
      event_report(EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
      return CAMERA_INVALID_FORMAT;
    }
  }
  else
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
  {
    camera_transparent_color = (uint16) transparent_color;
  }
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  if (preview_mode == CAMERA_PREVIEW_MODE_MOVIE)
  {
    camera_ret_code_type retval;
    retval = camcorder_svcs_set_overlay(&preview_overlay,&work,size,overlay_bmp);
    if ( retval != CAMERA_SUCCESS)
    {
      camera_reject (callback, client_data, CAMERA_FUNC_SET_OVERLAY);
      if (retval == CAMERA_NO_MEMORY)
        event_report(EVENT_CAMERA_NO_MEMORY);
      else
        event_report (EVENT_CAMERA_INVALID_STATE);
      /* in failure condition, call this to free all the buffers */
      camera_clr_overlays();
      return retval;
    }
    else if (!overlay_bmp || overlay_bmp == (void *)1)
    {
      return CAMERA_SUCCESS;
    }
  }
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */

  if (preview_mode == CAMERA_PREVIEW_MODE_SNAPSHOT)
  {
    work.imgPtr = (uint8*)CAM_Malloc(size);
  }
  if (work.imgPtr == NULL)
  {
    camera_reject(callback, client_data, CAMERA_FUNC_SET_OVERLAY);
    event_report(EVENT_CAMERA_NO_MEMORY);
    return CAMERA_NO_MEMORY;
  }

  ui_overlay.imgPtr = overlay_bmp;
  ui_overlay.dx = dx;
  ui_overlay.dy = dy;
  ui_overlay.cFormat = IPL_RGB565;

  work.dx = dx;
  work.dy = dy;
  work.cFormat = IPL_RGB565;

  /* Fix overlay frame boundary to prevent color smear */
  status = ipl_fix_boundary(&ui_overlay, &work, transparent_color);
  if (status == IPL_SUCCESS)
  {
    /* Align overlay frame to make sure transparent color at even boundary */
    status = ipl_align_frame(&work, &ui_overlay, transparent_color);
  }
  if (preview_mode == CAMERA_PREVIEW_MODE_SNAPSHOT)
  {
    CAM_Free(work.imgPtr);
  }

  if (status == IPL_SUCCESS)
  {
    tcb = rex_self ();
    (void) rex_clr_sigs (tcb, CAMERA_SIG);
    graph_queue_camera_func ((uint32) CAMERA_FUNC_SET_OVERLAY,
        (void *) callback, client_data, (uint32) tcb, 0, 0, 0);
    MSG_CAMERADEBUG("Waiting on CAMERA_SIG in camera_svcs_set_overlay", 0, 0, 0 );
    (void) rex_wait (CAMERA_SIG);
    MSG_CAMERADEBUG("Got CAMERA_SIG in camera_svcs_set_overlay", 0, 0, 0 );
    (void) rex_clr_sigs (tcb, CAMERA_SIG);
    return CAMERA_SUCCESS;
  }
  else
  {
    camera_reject(callback, client_data, CAMERA_FUNC_SET_OVERLAY);
    event_report(EVENT_CAMERA_SET_FAILED);
    return CAMERA_FAILED;
  }

#endif /* T_IO_CARD */
} /* camera_svcs_set_overlay */

/*===========================================================================

FUNCTION      CAMERA_CLR_OVERLAYS

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

static void camera_clr_overlays(void)
{
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  if(preview_mode == CAMERA_PREVIEW_MODE_MOVIE)
  {
    (void)camcorder_svcs_clr_overlay(&preview_overlay);
  }
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */
#ifdef FEATURE_MDP
  /* De-register the overlay layer */
  camera_deregister_layer1();
#endif /* FEATURE_MDP */
  camera_svcs_clr_encode_overlay();
  camera_svcs_clr_preview_overlay();
  camera_svcs_clr_thumbnail_overlay();
  ui_overlay.dx = 0;
  ui_overlay.dy = 0;
  ui_overlay.cFormat = IPL_YCbCr;
  ui_overlay.imgPtr  = NULL;
  ui_overlay.clrPtr  = NULL;
} /* camera_clr_overlays */

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
camera_ret_code_type camera_svcs_set_icon_array
(
  /* Display size */
  uint32          dx,
  uint32          dy,
  /* Transparent color, must be the same for all icons and overlay */
  uint32          transparent_color,
  ipl_icon_type **icon_array
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  rex_tcb_type *tcb;

  camera_ret_code_type ret_code = CAMERA_SUCCESS;
  uint32               icon_count,
                       i,
                       j,
                       size;
  ipl_icon_type      **icons,
                     **cur_icon;

   rex_tcb_type *tcb;

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_SET_ICON_ARRAY, 0, 0, __LINE__);
  MSG_HIGH("camera_set_icon_array, camera_state %d", camera_state, 0, 0);

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    ret_code   = CAMERA_NO_SENSOR;
    icon_count = 0;
  }
  else if (camera_state == CAMERA_STATE_INIT)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    ret_code   = CAMERA_INVALID_STATE;
    icon_count = 0;
  }
  else
  {
    /* Reject if too many icons or format is not supported.
     * Also, find the largest icon size, to be used for all icons.
     * This makes all icon buffer the same size. The buffers then be
     * reused when reflection is needed. */
    for (icon_count = 0; icon_count < CAMERA_ICON_ARRAY_SIZE; icon_count++)
    {
      if (icon_array [icon_count] == NULL)
      {
        break;
      }
      else if (icon_array [icon_count]->cFormat != IPL_RGB565)
      {
        ret_code = CAMERA_INVALID_PARM;
        break;
      }
    }

    if ((icon_count >= CAMERA_ICON_ARRAY_SIZE) || (ret_code != CAMERA_SUCCESS))
    {
      event_report (EVENT_CAMERA_INVALID_CONFIG_PARM);
      ret_code = CAMERA_INVALID_PARM;
    }
  }

  /* Transparent color must be the same as overlay's. */
  if (preview_overlay)
  {
    if (transparent_color != camera_transparent_color)
    {
      event_report (EVENT_CAMERA_INVALID_CONFIG_PARM);
      ret_code = CAMERA_INVALID_PARM;
    }
  }
  else
  {
    camera_transparent_color = (uint16) transparent_color;
  }

  if (ret_code != CAMERA_SUCCESS)
  {
    return ret_code;
  }

  camera_clr_icons();

  /* Make a copy of new icons */
  icons = CAM_Malloc (icon_count * 4 + 4);

  if (icons == NULL)
  {
    event_report (EVENT_CAMERA_NO_MEMORY);
    return CAMERA_NO_MEMORY;
  }

  cur_icon = icons;

  for (i = 0; i < icon_count; i++, cur_icon++)
  {
    uint8 *icon;

    *cur_icon = (ipl_icon_type *) CAM_Malloc (sizeof (ipl_icon_type));
    size      = (uint32) icon_array [i]->pitch * icon_array [i]->dy;
    icon      = CAM_Malloc (size);

    if ((*cur_icon == NULL) || icon == NULL)
    {
      if (icon != NULL)
        CAM_Free (icon);
      if (*cur_icon != NULL)
        CAM_Free (*cur_icon);

      cur_icon = icons;
      for (j = 0; j < i; j++)
      {
        CAM_Free ((*cur_icon)->imgPtr);
        CAM_Free (*cur_icon);
      }

      CAM_Free (icons);
      event_report (EVENT_CAMERA_NO_MEMORY);
      return CAMERA_NO_MEMORY;
    }

/*lint -save -e661 */
    **cur_icon = *icon_array [i];
    (*cur_icon)->imgPtr = icon;
    memcpy (icon, icon_array [i]->imgPtr, size);
  }

  *cur_icon = NULL;
/*lint -restore */

  camera_transparent_color = transparent_color;
  tcb = rex_self ();
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  graph_queue_camera_func ((uint32) CAMERA_FUNC_SET_ICON_ARRAY,
                           0, 0, (uint32)tcb, (uint32)icons, dx, dy);
  MSG_CAMERADEBUG("Waiting on CAMERA_SIG in camera_svcs_set_icon_array", 0, 0, 0 );
  (void) rex_wait (CAMERA_SIG);
  MSG_CAMERADEBUG("Got CAMERA_SIG in camera_svcs_set_icon_array", 0, 0, 0 );
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  return ret_code;
#else /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
  return CAMERA_NOT_SUPPORTED;
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */

#endif /* T_IO_CARD */
} /* camera_svcs_set_icon_array */


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
camera_ret_code_type camera_svcs_clr_icon_array(void)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_CLR_ICON_ARRAY, 0, 0, __LINE__);
  MSG_HIGH("camera_clr_icon_array, camera_state %d", camera_state, 0, 0);

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  graph_queue_camera_func ((uint32) CAMERA_FUNC_CLR_ICON_ARRAY,
                           0, 0, 0, 0, 0, 0);
  return CAMERA_SUCCESS;
#else /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
  return CAMERA_NOT_SUPPORTED;
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */

#endif /* T_IO_CARD */
}


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
camera_ret_code_type camera_svcs_set_position
(
  camera_position_type *position,
  camera_cb_f_type      callback,
  void                 *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_SET_POSITION, 0, 0, __LINE__);
  MSG_HIGH("camera_set_position, camera_state %d", camera_state, 0, 0);

  if (camsensor_initialized == FALSE)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_POSITION);
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_POSITION);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  camera_position = *position;

  graph_queue_camera_func ((uint32) CAMERA_FUNC_SET_POSITION,
                           (void *)callback, client_data, 0, 0, 0, 0);

  return CAMERA_SUCCESS;
#endif /* T_IO_CARD */
}


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
camera_ret_code_type camera_svcs_set_exif_tag
(
  camera_exif_tag_type *tag,
  camera_cb_f_type      callback,
  void                 *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  camera_ret_code_type ret_val = CAMERA_SUCCESS;

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_SET_EXIF_TAG, 0, 0, __LINE__);
  MSG_CAMERADEBUG ("Camera set string", 0, 0, 0);

  if (camsensor_initialized == FALSE)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_EXIF_TAG);
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_EXIF_TAG);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  /* Check for tag ID */
  switch ((exif_tag_id_type) tag->tag_id)
  {
    case IMAGE_DESCRIPTION:
      if (tag->type != TIFF_DATA_ASCII)
      {
        ret_val = CAMERA_INVALID_FORMAT;
        event_report (EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
      }
      else if (camera_description)
      {
        /* Cannot not set twice in a roll */
        ret_val = CAMERA_NO_MEMORY;
        event_report (EVENT_CAMERA_NO_MEMORY);
      }
      else
      {
        camera_description = CAM_Malloc (tag->count);
        if (camera_description)
        {
          camera_description_count = tag->count;
          memcpy (camera_description, tag->value, tag->count);
        }
        else
        {
          ret_val = CAMERA_NO_MEMORY;
          event_report (EVENT_CAMERA_NO_MEMORY);
        }
      }
      break;

    case GPS_AREA_INFORMATION:
      if (tag->type != TIFF_DATA_UNDEFINED)
      {
        ret_val = CAMERA_INVALID_FORMAT;
        event_report (EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
      }
      else if (camera_gps_area_information)
      {
        /* Cannot not set twice in a roll */
        ret_val = CAMERA_NO_MEMORY;
        event_report (EVENT_CAMERA_NO_MEMORY);
      }
      else
      {
        camera_gps_area_information = CAM_Malloc (tag->count);
        if (camera_gps_area_information)
        {
          camera_gps_area_information_count = tag->count;
          memcpy (camera_gps_area_information, tag->value, tag->count);
        }
        else
        {
          ret_val = CAMERA_NO_MEMORY;
          event_report (EVENT_CAMERA_NO_MEMORY);
        }
      }
      break;

    case EXIF_USER_COMMENT:
      if (tag->type != TIFF_DATA_UNDEFINED)
      {
        ret_val = CAMERA_INVALID_FORMAT;
        event_report(EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
      }
      else if (camera_user_comment)
      {
        /* Cannot not set twice in a roll */
        ret_val = CAMERA_NO_MEMORY;
        event_report (EVENT_CAMERA_NO_MEMORY);
      }
      else
      {
        camera_user_comment = CAM_Malloc (tag->count);
        if (camera_user_comment)
        {
          camera_user_comment_count = tag->count;
          memcpy (camera_user_comment, tag->value, tag->count);
        }
        else
        {
          ret_val = CAMERA_NO_MEMORY;
          event_report (EVENT_CAMERA_NO_MEMORY);
        }
      }
      break;

    case MAKE:
     if (tag->type != TIFF_DATA_ASCII)
     {
       ret_val = CAMERA_INVALID_FORMAT;
       event_report(EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
     }
     else if (camera_make_information)
     {
       /* Cannot not set twice in a roll */
       ret_val = CAMERA_NO_MEMORY;
       event_report(EVENT_CAMERA_NO_MEMORY);
     }
     else
     {
       camera_make_information = CAM_Malloc(tag->count);
       if (camera_make_information)
       {
         camera_make_information_count = tag->count;
         memcpy(camera_make_information, tag->value, tag->count);
       }
       else
       {
         ret_val = CAMERA_NO_MEMORY;
         event_report(EVENT_CAMERA_NO_MEMORY);
       }
     }
     break;

    case MODEL:
     if (tag->type != TIFF_DATA_ASCII)
     {
       ret_val = CAMERA_INVALID_FORMAT;
       event_report(EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
     }
     else if (camera_model_information)
     {
       /* Cannot not set twice in a roll */
       ret_val = CAMERA_NO_MEMORY;
       event_report(EVENT_CAMERA_NO_MEMORY);
     }
     else
     {
       camera_model_information = CAM_Malloc(tag->count);
       if (camera_model_information)
       {
         camera_model_information_count = tag->count;
         memcpy(camera_model_information, tag->value, tag->count);
       }
       else
       {
         ret_val = CAMERA_NO_MEMORY;
         event_report(EVENT_CAMERA_NO_MEMORY);
       }
     }
     break;


    default:
      camera_reject (callback, client_data, CAMERA_FUNC_SET_EXIF_TAG);
      event_report (EVENT_CAMERA_INVALID_SET_ID);
      return CAMERA_NOT_SUPPORTED;
  }

  if (ret_val != CAMERA_SUCCESS)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_EXIF_TAG);
    event_report (EVENT_CAMERA_SET_FAILED);
    return CAMERA_NOT_SUPPORTED;
  }

  graph_queue_camera_func ((uint32) CAMERA_FUNC_SET_EXIF_TAG,
                           (void *) callback, client_data, 0, 0, 0, 0);

  return CAMERA_SUCCESS;

#endif /* T_IO_CARD */
}

/*===========================================================================

FUNCTION      CAMERA_BLT

DESCRIPTION
              Color conversion, rotation, or adding frame and block transfer
              the result into destination buffer. Output color is RGB565.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_blt
(
  camera_frame_type *frame,
  /* The destination RGB buffer dimension and pointer. Pointer indeces
   * beginning of data buffer. */
  uint16             dst_dx,
  uint16             dst_dy,
  void              *dst_ptr,
  /* Rectangle of the destination used for update */
  uint16             x,
  uint16             y,
  uint16             dx,
  uint16             dy
)
{
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  if (camera_lcd_direct_mode)
  {
    return CAMERA_INVALID_STATE;
  }
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
  return camera_svcs_blt_ex(frame, dst_dx, dst_dy, dst_ptr, x, y, dx, dy, CAMERA_YCBCR_4_2_0);
}

#ifdef FEATURE_CAMERA_NOFULLSCREEN
#include "disp.h"
camera_ret_code_type camera_svcs_blt_to_lcd
(
  camera_frame_type *frame,
  uint16             x,
  uint16             y,
  uint16             dx,
  uint16             dy
)
{
/*lint -save -e715 function param not referenced */
  qcamrawHeaderType *header;
  byte *bmy_addr;
  
  /* Do a Null Check for frame pointer */
  if(frame->buffer!=NULL)
  {
     header= (qcamrawHeaderType *) frame->buffer;
  }
  else
  {  
     return CAMERA_INVALID_PARM;
  }
  
  if (header->frameType == QCAMRAW_PICTURE_FRAME)
  {

    if ((*(uint32*)(frame->thumbnail_image)   != 0x4D414351) ||
        (*(uint32*)(frame->thumbnail_image+4) != 0x20574152))
    {
      event_report (EVENT_CAMERA_INVALID_STATE);
      return CAMERA_INVALID_FORMAT;
    }
    bmy_addr = qcamrawGetDataYPtr (frame->thumbnail_image);
  }
  else if ((header->frameType == QCAMRAW_PREVIEW_FRAME) &&
           ((camera_state == CAMERA_STATE_PREVIEW) ||
            (camera_state == CAMERA_STATE_RECORDING) ||
            (camera_state == CAMERA_STATE_QVP_ENCODING)))
  {
    if ((header->buffer_index >= CAMERA_NUM_OF_PREVIEW_BUFFERS) ||
        ((uint32)(frame->buffer) % 4 != 0) ||
        (*(uint32*)(frame->buffer) != 0x4D414351) ||
        (*(uint32*)(frame->buffer+4) != 0x20574152))
    {
      event_report (EVENT_CAMERA_INVALID_STATE);
      return CAMERA_INVALID_FORMAT;
    }
    bmy_addr = qcamrawGetDataYPtr (frame->buffer);
  }
  else
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_FORMAT;
  }
  
  if(frame->format == CAMERA_RGB565)
  {
    //disp_update(bmy_addr, dx, 0, 0, dy, dx, y, x);
    disp_update_lock(bmy_addr, dy, dx);
  }
  else
  {
    if (header->frameType == QCAMRAW_PREVIEW_FRAME)
    {
       graph_queue_camera_func ((uint32)CAMERA_FUNC_RELEASE_FRAME,(void *)0, (void *)0, (uint32) header->buffer_index, 0, 0, 0);
    }
    return CAMERA_INVALID_FORMAT;
  }
  
  if (header->frameType == QCAMRAW_PREVIEW_FRAME)
  {
    graph_queue_camera_func ((uint32)CAMERA_FUNC_RELEASE_FRAME,(void *)0, (void *)0, (uint32) header->buffer_index, 0, 0, 0);
  }
  return CAMERA_SUCCESS;

/*lint -restore */
} /* camera_svcs_blt_ex */

#endif

/*===========================================================================

FUNCTION      CAMERA_SVCS_BLT_EXT

DESCRIPTION
              Color conversion, rotation, or adding frame and block transfer
              the result into destination buffer. Output color is RGB565.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_blt_ext
(
  camera_frame_type *frame,
  /* The destination RGB buffer dimension and pointer. Pointer indeces
   * beginning of data buffer. */
  uint16             dst_dx,
  uint16             dst_dy,
  void               *dst_ptr,
  /* Rectangle of the destination used for update */
  uint16             x,
  uint16             y,
  uint16             dx,
  uint16             dy,
  camera_format_type format
)
{
#ifdef FEATURE_CAMERA_NOFULLSCREEN
  if(format == CAMERA_TOLCD)
  {
    return camera_svcs_blt_to_lcd(frame, x, y, dx, dy);
  }
#endif
  return camera_svcs_blt_ex(frame, dst_dx, dst_dy, dst_ptr, x, y, dx, dy, format);
}
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
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
)
{
  MSG_FATAL("camera_set_lcd_direct_mode %x %d %d",mode,dst_x,dst_y);
  MSG_FATAL("camera_set_lcd_direct_mode %x %d %d",camera_state,dst_dx,dst_dy);
  if (camera_state != CAMERA_STATE_READY)
  {
    return CAMERA_INVALID_STATE;
  }
  if (mode)
  {
    int layer = mode & 0xf;
    int dst = (mode & 0xf0) >> 4;
    LCD_TYPE mdp_dest;
    MDP_LAYER mdp_layer;
    if (dst == 1)
    {
      mdp_dest = PRIMARY_LCD_TYPE;
    }
    else if (dst == 2)
    {
      mdp_dest = SECONDARY_LCD_TYPE;
    }
    else
    {
      return CAMERA_INVALID_PARM;
    }
    if (layer == 0)
    {
      mdp_layer   = MDP_LAYER_0;
    }
    else if (layer == 1)
    {
      mdp_layer   = MDP_LAYER_1;
    }
    else if (layer == 2)
    {
      mdp_layer   = MDP_LAYER_2;
    }
    else
    {
      return CAMERA_INVALID_PARM;
    }
    mdpImg.dest = mdp_dest;
    mdpImg.layer = mdp_layer;
    mdpImg_frame.dest = mdp_dest;
    camera_lcd_direct_rect.x = dst_x;
    camera_lcd_direct_rect.y = dst_y;
    camera_lcd_direct_rect.dx = dst_dx;
    camera_lcd_direct_rect.dy = dst_dy;
  }
  camera_lcd_direct_mode = mode;
  camera_deregister_video(TRUE);
  camera_deregister_layer1 ();
  return CAMERA_SUCCESS;
}
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */

#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
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
camera_ret_code_type camera_blt_direct(camera_frame_type *frame)
{
  if (camera_lcd_direct_mode)
  {
    return camera_svcs_blt_ex(
        frame,
        camera_lcd_direct_rect.x + camera_lcd_direct_rect.dx,
        camera_lcd_direct_rect.y + camera_lcd_direct_rect.dy,
        0,
        camera_lcd_direct_rect.x, camera_lcd_direct_rect.y,
        camera_lcd_direct_rect.dx, camera_lcd_direct_rect.dy,
        CAMERA_YCBCR_4_2_0);
  }
  else
  {
    return CAMERA_INVALID_STATE;
  }
}
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */

/*===========================================================================

FUNCTION      CAMERA_SVCS_BLT_EX

DESCRIPTION
  Display to either LCD directly or via bitmap.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_blt_ex
(
  camera_frame_type *frame,
  /* The destination RGB buffer dimension and pointer. Pointer indeces
   * beginning of data buffer. */
  uint16             dst_dx,
  uint16             dst_dy,
  void               *dst_ptr,
  /* Rectangle of the destination used for update */
  uint16             x,
  uint16             y,
  uint16             dx,
  uint16             dy,
  camera_format_type format
)
{
/*lint -save -e715 function param not referenced */

  static ipl_rect_type src_window, dst_window;
  static uint16 my_dst_dx = 0, my_dst_dy = 0, my_x=0, my_y=0, my_dx=0, my_dy=0;
  boolean picture_frame;
  uint32 *bmy_addr, *cbcr_addr;
  uint32 rotation;
  qcamrawHeaderType *header;
  /* For snapshot, thumbnail is displayed. Thumbnail dimensions may not
   * be multiple of 16, but the thumbnail frame dimensions are always multiple
   * of 16. Overlay has the same size as thumbnail, and may not be multiple of
   * 16. Overlay is placed over the thumbnail at certain offset according to
   * encode rotation. The offset must be added in order to display only the
   * area with overlay. */
  boolean adjust_win_for_thumbnail = FALSE;
  uint32 size;

  /* Do a Null Check for frame pointer */
  if(frame->buffer!=NULL)
  {
     header= (qcamrawHeaderType *) frame->buffer;
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_BLT, 0, 0, __LINE__);
  }
  else 

  {  
     return CAMERA_INVALID_PARM;
  }

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  /* When not in direct mode, each function call may have different params */
  if (camera_lcd_direct_mode == 0)
  {
    camera_blt_ext_recalculate_window = TRUE;
  }
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
  if ((x >= dst_dx) || (y >= dst_dy))
  {
    return CAMERA_INVALID_PARM;
  }
  if(camera_state == CAMERA_STATE_RECORDING && header->rotation != video_encode_rotation)
  	camera_blt_ext_recalculate_window = TRUE;
  else if(camera_state != CAMERA_STATE_RECORDING && header->rotation != camera_default_preview_rotation)
    camera_blt_ext_recalculate_window = TRUE;

  if (header->frameType == QCAMRAW_PICTURE_FRAME)
  {
    boolean valid = FALSE;

    /* Check if input thumbnail_image looks OK */
    if ((frame->thumbnail_image != NULL) && ((uint32)(frame->thumbnail_image) % 4 == 0))
    {
      /* Check if this is a valid thumbnail frame for single shot */
      if ((uint8 *)frame->thumbnail_image == thumbnail_luma_buf)
      {
        valid = TRUE;
      }
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
    }

    if ((valid == FALSE) ||
        (*(uint32*)(frame->thumbnail_image)   != 0x4D414351) ||
        (*(uint32*)(frame->thumbnail_image+4) != 0x20574152))
    {
      event_report (EVENT_CAMERA_INVALID_STATE);
      return CAMERA_INVALID_FORMAT;
    }
    picture_frame = TRUE;
    bmy_addr = (uint32 *) qcamrawGetDataYPtr (frame->thumbnail_image);
    cbcr_addr = (uint32 *) qcamrawGetDataCbCrPtr (frame->thumbnail_image);
    /* 2 time is too much, but covers the pad area. */
    size = frame->dx * frame->dy * 2;
#ifdef FEATURE_MDP
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
    if (camera_lcd_direct_mode)
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
    {
      CAM_Free(picture_preview_copy);
      picture_preview_copy = CAM_Malloc(size | CAM_ALLOC_NO_ZMEM);
      if (picture_preview_copy)
      {
        memcpy(picture_preview_copy, bmy_addr, (size/2));
        memcpy((picture_preview_copy+(size/2)),cbcr_addr,(size/2));
        cbcr_addr = (uint32*)((uint32)picture_preview_copy + (size/2));
        bmy_addr = (uint32*)picture_preview_copy;
      }
    }
#endif /* FEATURE_MDP */
  }
  else if ((header->frameType == QCAMRAW_PREVIEW_FRAME) &&
           ((camera_state == CAMERA_STATE_PREVIEW) ||
            (camera_state == CAMERA_STATE_RECORDING) ||
            (camera_state == CAMERA_STATE_QVP_ENCODING)))
  {
    if ((header->buffer_index >= CAMERA_NUM_OF_PREVIEW_BUFFERS) ||
        ((uint32)(frame->buffer) % 4 != 0) ||
        (*(uint32*)(frame->buffer) != 0x4D414351) ||
        (*(uint32*)(frame->buffer+4) != 0x20574152))
    {
      MSG_CAMERADEBUG("CAMERA_SVCS: frame->buffer received from UI is corrupted camera_state:%d", camera_state, 0, 0);
      event_report (EVENT_CAMERA_INVALID_STATE);
      return CAMERA_INVALID_FORMAT;
    }
    picture_frame = FALSE;
    bmy_addr  = (uint32 *) qcamrawGetDataYPtr (frame->buffer);
    cbcr_addr = (uint32 *) qcamrawGetDataCbCrPtr (frame->buffer);
  }
  else
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_FORMAT;
  }
  /* If input parameter changed, recalculate window */
  if ((x != my_x) || (y != my_y) || (dx != my_dx) || (dy != my_dy) ||
      (my_dst_dx != dst_dx) || (my_dst_dy != dst_dy) || camera_blt_ext_recalculate_window)
  {
    camera_blt_ext_recalculate_window = TRUE;
    my_x = x;
    my_y = y;
    my_dx = dx;
    my_dy = dy;
    my_dst_dx = dst_dx;
    my_dst_dy = dst_dy;
  }

  /* Re-calculate window when needed */
  if (camera_blt_ext_recalculate_window)
  {
    boolean odd_rotation;
    qcamrawHeaderType *thumbnail_header = (qcamrawHeaderType *)frame->thumbnail_image;

    if(header->rotation != camera_default_preview_rotation)
	camera_set_preview_headers_rotation();
    camera_blt_ext_recalculate_window = FALSE;
    /* 1) Init based on frame type */
    if (picture_frame)
    {
      src_window.x = 0;
      src_window.y = 0;

      if((camera_dx <= thumbnail_header->picWidth)
        && (camera_dy <= thumbnail_header->picHeight) )
      {
        /* display the buffer with piture dimensions set by UI */
        src_window.dx = camera_dx;
        src_window.dy = camera_dy;	 
      }
      else
      {
      src_window.dx = thumbnail_header->picWidth;
      src_window.dy = thumbnail_header->picHeight;
      }

      if (thumbnail_overlay || (thumbnail_header->picHeight < thumbnail_header->dataHeight))
      {
        adjust_win_for_thumbnail = TRUE;
      }
      rotation = frame->rotation;
      /* Try to match picture orientation same as preview */
      if ((camera_default_preview_rotation == 0) ||
          (camera_default_preview_rotation == 180))
      {
        if (rotation == 90 || rotation == 270)
        {
          rotation -= 90;
        }
      }
      else
      {
        if (rotation == 0 || rotation == 180)
        {
          rotation += 90;
        }
      }
#ifdef FEATURE_MDP
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
      if (camera_lcd_direct_mode)
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
      {
        camera_svcs_adjust_mdp_for_snapshot(rotation);
      }
#endif /* FEATURE_MDP */
    }
    else
    {
      src_window.x = 0;
      src_window.y = 0;
      src_window.dx = frame->dx;
      src_window.dy = frame->dy;
      rotation = header->rotation;
    }
#ifdef FEATURE_MDP
    /* If MDP is used, update width. Also, reclaim the offsets. */
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
    if (camera_lcd_direct_mode)
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
    {
      /* width here is based on the source orientation */
      mdpImg.width = frame->dx;
#ifndef CUST_EDITION
      dst_dx += BREW_OFFSET_X;
      dst_dy += BREW_OFFSET_Y;
      dx += BREW_OFFSET_X;
      dy += BREW_OFFSET_Y;
#endif
    }
#endif /* FEATURE_MDP */
    /* 2) Adjust destination rectangle so it does not exceed destination */
    if (x + dx > dst_dx)
    {
      dx = dst_dx - x;
    }
    if (y + dy > dst_dy)
    {
      dy = dst_dy - y;
    }
    /* 3) if odd rotation, adjust rectangle based on source view,
     *    used to calculate source rectangle. */
    if (rotation == 90 || rotation == 270)
    {
      odd_rotation = TRUE;
    }
    else
    {
      odd_rotation = FALSE;
    }
    /* 5) destination view */
    if (odd_rotation)
    {
      dst_window.x = x;
      dst_window.y = y;
      dst_window.dx = src_window.dy;
      dst_window.dy = src_window.dx;
    }
    else
    {
      dst_window.x = x;
      dst_window.y = y;
      dst_window.dx = src_window.dx;
      dst_window.dy = src_window.dy;
    }
    if (adjust_win_for_thumbnail)
    {
      switch (frame->rotation)
      {
        case 0:
          break;
        case 90:
          if (thumbnail_overlay)
          {
            src_window.y += thumbnail_overlay->pad_lines;
          }
          else
          {
            src_window.y += (thumbnail_header->dataHeight - thumbnail_header->picHeight);
          }
          break;
        case 180:
          if (thumbnail_overlay)
          {
            src_window.x += thumbnail_overlay->pad_pixels;
            src_window.y += thumbnail_overlay->pad_lines;
          }
          else
          {
            src_window.y += (thumbnail_header->dataHeight - thumbnail_header->picHeight);
          }
          break;
        case 270:
        default:
          if (thumbnail_overlay)
          {
            src_window.x += thumbnail_overlay->pad_pixels;
          }
          break;
      }
    }

#ifdef FEATURE_MDP
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
    if (camera_lcd_direct_mode)
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
    {
      mdpImg.cwin.x1 = src_window.x;
      mdpImg.cwin.x2 = src_window.x + src_window.dx - 1;
      mdpImg.cwin.y1 = src_window.y;
      mdpImg.cwin.y2 = src_window.y + src_window.dy - 1;
      mdpImg.lcd_x = dst_window.x - BREW_OFFSET_X;
      mdpImg.lcd_y = dst_window.y - BREW_OFFSET_Y;
      if (picture_frame == FALSE)
      {
        CAM_Free (picture_preview_copy);
        picture_preview_copy = NULL;
        camera_svcs_adjust_mdp_for_video();
        if (preview_overlay && (mdpImg.layer == MDP_LAYER_0))
        {
          /* Overlay available. Make sure it is displayed */
          camera_svcs_display_frame_overlay(FALSE);
        }
      }
    }
#endif /* FEATURE_MDP */
  }

  /* MDP supports RGBxxx, H2V2 input formats only*/
  if (header->dataType == QCAMRAW_H2V2)
  {
#ifdef FEATURE_MDP
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
    if (camera_lcd_direct_mode)
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
    {
      if (picture_frame == FALSE)
      {
        /* For preview framesw who are not a multiple of 16 in dy we
           must pad the last line to avoid an MDP bug ("green line" issue) */
        if (camera_preview_pad_for_mdp > 0)
        {
          uint32 * end_cbcr_ptr = (uint32 *) qcamrawGetDataCbCrEndPtr (frame->buffer);
          /* Duplicate last CbCr line to next line for MDP hardware workaround */
          memcpy(end_cbcr_ptr, (end_cbcr_ptr-camera_preview_pad_for_mdp), camera_preview_pad_for_mdp);
        }
      }
      mdpImg.bmy_addr = bmy_addr;
      mdpImg.cbcr_addr = cbcr_addr;

      if (camera_dsp_command != CAMQDSP_CMD_IDLE
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
          )
      {
        static uint16 my_mdp_x = 0xFFFF, my_mdp_y = 0xFFFF;
        if ((mdpImgId == NULL) || (mdp_is_ovimg_valid(mdpImgId) != TRUE) ||
            (my_mdp_x != x) || (my_mdp_y != y))
        {
          /* De-register the video */
#ifdef FEATURE_MMOVERLAY
          camera_deregister_video (FALSE);
#else  /* FEATURE_MMOVERLAY */
          camera_deregister_video (TRUE);
#endif /* !FEATURE_MMOVERLAY */
          mdpImgId = mdp_register_ovimg (&mdpImg);
          my_mdp_x = x;
          my_mdp_y = y;
          if(mdpImgId == NULL)
          {
            MSG_ERROR("primary or secondary display overlay list fails", 0, 0, 0);
          }
        }
        else
        {
          if(mdp_replace_ovimg(&mdpImg, mdpImgId) == NULL)
          {
            MSG_ERROR("Fails to replace the specified overlay image", 0, 0, 0);
          }
        }
        {
          /* Calling MDP update async */
          MDP_CB cb;
          /* pass back this buffer index when mdp is done update */
          cb.arg = (void*)header -> buffer_index;
          /* We don't want mdp to give us callback if we are displaying a picture frame */
          if (picture_frame == TRUE)
          {
            cb.cb_func = NULL;
          }
          else
          {
            cb.cb_func = camera_display_async_cb;
          }
          if(mdp_update_ovimg_async(mdpImgId, &cb) == MDP_FAIL)
          {
            MSG_ERROR("Fails to update the pre-registered ovimg from the list", 0, 0, 0);
            
            /* Issue callback if MDP rejects update so preview buffer is released */
            camera_display_async_cb(cb.arg,MDP_FAIL);           
          }
          else
          {
            graph_queue_camera_func ((uint32)CAMERA_FUNC_BLT, (void *)0,
                (void *)0, header->buffer_index, 0, 0, 0);
          }
        }
      }
    }
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
    else
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
#endif /* FEATURE_MDP */
#if defined(FEATURE_CAMERA_LCD_DIRECT_MODE) || !defined(FEATURE_MDP)
    {
      ipl_image_type i_img, o_img;
      ipl_status_type status;

      i_img.dx = frame->dx;
      i_img.dy = frame->dy;
      i_img.cFormat = IPL_YCrCb420_LINE_PK;
      i_img.imgPtr = (byte *)bmy_addr;
      i_img.clrPtr = (byte *)cbcr_addr;
      o_img.dx = dst_dx;
      o_img.dy = dst_dy;
      o_img.cFormat = IPL_RGB565;
      o_img.imgPtr  = (byte *)dst_ptr;
      o_img.clrPtr = NULL;
      dst_window.dx= dx;
      dst_window.dy= dy;      
      /* For preview frame, we need to apply both water and mirror reflection.
       * For picture frame, we need to apply only water reflection (XREF)
       * since sensor is rotated. */
      if ((camera_reflect_ipl == IPL_YREF) ||
          (camera_reflect_ipl == IPL_XREF && picture_frame == FALSE))
      {
        status = ipl_reflect(&i_img, NULL,IPL_YREF);
        if (status != IPL_SUCCESS )
        {
          MSG_ERROR("ipl_reflect failed", 0, 0, 0);
          return CAMERA_FAILED;
        }
      }
      if (picture_frame)
      {
        status = ipl_rot_add_crop(&i_img, NULL, &o_img,
            &dst_window, camera_default_preview_rotation_ipl, NULL,
            camera_transparent_color);
      }
      else
      {
        status = ipl_rot_add_crop(&i_img, preview_overlay, &o_img,
            &dst_window, camera_default_preview_rotation_ipl, NULL,
            camera_transparent_color);
      }
      if (status != IPL_SUCCESS)
      {
        MSG_ERROR("ipl_rot_add_crop has failed", 0, 0, 0);
      }
    }
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE || nFEATURE_MDP */
    if (camera_snapshot_timing.snapshot_completion_callback_start !=0)
    {
      /* Either First frame after starting the camera or after snapshot */
      camera_snapshot_timing.first_frame_to_display = (uint32)timetick_get_ms();
      /* Display the snapshot latency information */
      camera_show_snapshot_timing();
    }
  }
  else
  {
    return CAMERA_INVALID_FORMAT;
  }

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */
#ifdef FEATURE_VIDEO_ENCODE
  MSG_CAMQDSPIO("end blit %d",mp4e_get_time_ms(),0,0);
#endif

  return CAMERA_SUCCESS;

/*lint -restore */
} /* camera_svcs_blt_ex */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      CAMERA_ENABLE_SOLARIZE

DESCRIPTION
              Puts VFE in idle, downloads solarize table

DEPENDENCIES
  None

RETURN VALUE
  CAMERA_SUCCESS if all goes well
  camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, 0); if failure

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_enable_solarize()
{
  if (camsensor_initialized == TRUE)
  {
    camera_3x3_ycbcr_matrix_type * matrix_ptr =
      vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT);

    if(matrix_ptr == NULL)
    {
      MSG_ERROR("Color conversion matrix is not ready in VFE",0,0,0);
      return CAMERA_FAILED;
    }

    camera_parm_special_effects.current_value =  (int)CAMERA_EFFECT_SOLARIZE;
    /* Tell AWB algo that special effects are enabled */
    cam3a_awb_state.color_conversion_state = SPECIAL_EFFECTS_COLOR_CONVERSION;
    matrix_ptr = camera_awb_effects_off(camera_parm_white_balance.current_value);

    vfe_set_color_conversion(matrix_ptr, (float)CAMERA_SATURATION_DEFAULT);

    camera_update_vfe = TRUE;
    // flag that will re-configure the VFE hence taking the default gammma value
    camera_vfe_control_and_status.reconfigure_vfe = TRUE;
    camera_vfe_control_and_status.number_of_frames_to_ignore = 1;
    return CAMERA_SUCCESS;
  }
  else
    return CAMERA_NO_SENSOR;
}
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      CAMERA_DEFAULT_GAMMA
ISABLE_SOLARIZE

DESCRIPTION
              Puts VFE in idle, downloads default gamma table

DEPENDENCIES
  None

RETURN VALUE
  CAMERA_SUCCESS if all goes well
  camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, 0); if failure

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_default_gamma()
{
  if (camera_parm_special_effects.current_value != (int)CAMERA_EFFECT_SOLARIZE)
  {
    // set it to default gamma
    camera_parm_contrast.current_value = CAMERA_DEF_CONTRAST;
    // flag that will re-configure the VFE hence taking the default gammma value
    camera_vfe_control_and_status.reconfigure_vfe = TRUE;
    camera_vfe_control_and_status.number_of_frames_to_ignore = 1;
  }

  return CAMERA_SUCCESS;
}
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

/*===========================================================================

FUNCTION      CAMERA_SVCS_ADJUST_OVERLAY_FOR_ROTATION

DESCRIPTION
  Depending on encode rotation, the overlay imgPtr need to be adjusted.
  Please see for camera_encode_overlay_type define for details.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_svcs_adjust_overlay_for_rotation()
{
  switch (camera_jpeg_encode_rotation_format)
  {
    case JPEGENC_NO_ROT:
      /* Top left corner is a real pixel */
      if (encode_overlay)
      {
        encode_overlay->ipl_image.imgPtr = encode_overlay->imgPtr +
          (encode_overlay->pad_lines * encode_overlay->padded_width +
          encode_overlay->pad_pixels) * 2;
      }
      if (thumbnail_overlay)
      {
        thumbnail_overlay->ipl_image.imgPtr = thumbnail_overlay->imgPtr +
          (thumbnail_overlay->pad_lines * thumbnail_overlay->padded_width +
          thumbnail_overlay->pad_pixels) * 2;
      }
      break;
    case JPEGENC_CCLK270:
      /* Bottom left corner is a real pixel */
      if (encode_overlay)
      {
        encode_overlay->ipl_image.imgPtr = encode_overlay->imgPtr +
          encode_overlay->pad_pixels * 2;
      }
      if (thumbnail_overlay)
      {
        thumbnail_overlay->ipl_image.imgPtr = thumbnail_overlay->imgPtr +
          thumbnail_overlay->pad_pixels * 2;
      }
      break;
    case JPEGENC_CCLK180:
      /* Bottom right corner is a real pixel */
      if (encode_overlay)
      {
        encode_overlay->ipl_image.imgPtr = encode_overlay->imgPtr;
      }
      if (thumbnail_overlay)
      {
        thumbnail_overlay->ipl_image.imgPtr = thumbnail_overlay->imgPtr;
      }
      break;
    case JPEGENC_CCLK90:
      /* Top right corner is a real pixel */
      if (encode_overlay)
      {
        encode_overlay->ipl_image.imgPtr = encode_overlay->imgPtr +
          (encode_overlay->pad_lines * encode_overlay->padded_width) * 2;
      }
      if (thumbnail_overlay)
      {
        thumbnail_overlay->ipl_image.imgPtr = thumbnail_overlay->imgPtr +
          (thumbnail_overlay->pad_lines * thumbnail_overlay->padded_width) * 2;
      }
      break;
  }
}

/*===========================================================================

FUNCTION      CAMERA_SVCS_SET_PARM

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
camera_ret_code_type camera_svcs_set_parm
(
  camera_parm_type  id,
  int32             parm,
  camera_cb_f_type  callback,
  void             *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  camera_ret_code_type ret_val = CAMERA_SUCCESS;

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_SET_PARM, (uint32) id, (uint32) parm, __LINE__);
  MSG_HIGH("camera_set_parm, id %d, parm %d", id, parm, 0);

  if (camsensor_initialized == FALSE)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_PARM);
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_PARM);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  camera_parm_id = id;

  switch (id)
  {
    case CAMERA_PARM_ZOOM:
      if (parm == camera_parm_zoom.current_value)
      {
        break;
      }
      if (parm < camera_parm_zoom.minimum_value)
      {
        ret_val = CAMERA_INVALID_PARM;
        break;
      }
      if (parm > camera_parm_zoom.maximum_value)
      {
        ret_val = CAMERA_INVALID_PARM;
        break;
      }
      camera_parm_zoom.current_value = parm;

      if (camsensor_static_params[camera_asi].sensor_features.zoom == TRUE)
      {
        if (camsensor_set_zoom ((void *) (&camera_parm_zoom.current_value)) != CAMERA_SUCCESS)
        {
          MSG_HIGH ("Cannot set the sensor's zoom", 0, 0, 0);
        }
      }
      else
      {
        camera_calculate_zoom ();
        if (camera_state == CAMERA_STATE_PREVIEW ||
            camera_state == CAMERA_STATE_RECORDING ||
            camera_state == CAMERA_STATE_QVP_ENCODING)
        {
          camera_calculate_vfe_clipping ();
          vfeUpdate.clip = vfeVideoConfig.clip;
          camera_update_vfe = TRUE;
        }

        /* Defer stats processing for specified frames */
        MSG_CAMERADEBUG ("Delay AEC/AWB for zoom, %d frames",
            ZOOM_STATS_DELAY, 0, 0);
        if(camctrl_tbl.defer_aec_when_zoom)
        {
          camera_3a_shared_set_aec_awb_delay_count (ZOOM_STATS_DELAY, AEC_AWB_DELAY_UPDATE);
        }
      }

      break;

    case CAMERA_PARM_PREVIEW_ROTATION:
      if (parm == camera_parm_preview_rotation.current_value)
      {
        break;
      }
      if ((parm == (int)CAMERA_SP_NORMAL) || (parm == (int)CAMERA_SP_REVERSE))
      {
       camera_parm_preview_rotation.current_value = parm; 
       camera_rotate_encode_overlay();
       if(parm == (int)CAMERA_SP_NORMAL)
       {
        camera_default_preview_rotation = 0;
#if defined(FEATURE_CAMERA_LCD_DIRECT_MODE) || !defined(FEATURE_MDP)
        camera_default_preview_rotation_ipl = IPL_NOROT;
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE || nFEATURE_MDP */
       }
       else
       {
         camera_default_preview_rotation = 180;
#if defined(FEATURE_CAMERA_LCD_DIRECT_MODE) || !defined(FEATURE_MDP)
         camera_default_preview_rotation_ipl = IPL_ROT180;
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE || nFEATURE_MDP */
       }
      }
      else if(parm == (int)CAMERA_SP_CLK_TANGENTIAL)
      {
        camera_parm_preview_rotation.current_value = parm; 
        camera_default_preview_rotation = 90;
#if defined(FEATURE_CAMERA_LCD_DIRECT_MODE) || !defined(FEATURE_MDP)
        camera_default_preview_rotation_ipl = IPL_ROT90;
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE || nFEATURE_MDP */	
      }
      else if (parm == (int)CAMERA_SP_ACLK_TANGENTIAL)
      {
        camera_parm_preview_rotation.current_value = parm; 
        camera_default_preview_rotation = 270;
#if defined(FEATURE_CAMERA_LCD_DIRECT_MODE) || !defined(FEATURE_MDP)
        camera_default_preview_rotation_ipl = IPL_ROT270;
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE || nFEATURE_MDP */
      }
      else
        ret_val = CAMERA_INVALID_PARM;
      break;

    case CAMERA_PARM_ENCODE_ROTATION:
      if ((camera_state == CAMERA_STATE_RECORDING) ||
          (camera_state == CAMERA_STATE_QVP_ENCODING))
      {
        MSG_ERROR ("Invalid state", 0, 0, 0);
        event_report (EVENT_CAMERA_INVALID_STATE);
        ret_val = CAMERA_INVALID_PARM;
        break;
      }

      /* This mapping for setting camera_jpeg_encode_rotation_format
         is based on the fact that the raw data sits in the buffer
         where the top is aiming left like this:

       ----------------
      |                 |
      |TOP              |
      |                 |
       ----------------

      --->>

      |-------|
      | TOP   |
      |       |
      |       |
      |       |
      |       |
      |-------|

      270 counter-clock wise (for JPEG params) == 90 clockwise!!
      */

      if ( (parm != 0) && (parm != 90) && (parm != 180) && (parm != 270))
      {
        ret_val = CAMERA_INVALID_PARM;
        break;
      }

      /* For Camera */

      if (parm == 0)
      {
        camera_jpeg_encode_rotation_format  = JPEGENC_NO_ROT;
      }
      else if (parm == 90)
      {
        camera_jpeg_encode_rotation_format  = JPEGENC_CCLK270;
      }
      else if (parm == 180)
      {
        camera_jpeg_encode_rotation_format  = JPEGENC_CCLK180;
      }
      else if (parm == 270)
      {
        camera_jpeg_encode_rotation_format  = JPEGENC_CCLK90;
      }
      camera_parm_encode_rotation.current_value = parm;

      camera_svcs_adjust_overlay_for_rotation();

      /* For Cam corder */
      /* Apply parameter and default preview rotations.  Rotations must be
         inverted due to issues with the camcorder backend getting its rotation
         direction backwards (true 90 == encoded 270, true 270 == encoded 90) */
      video_encode_rotation = (uint16) (360 - (parm + camera_default_preview_rotation));

      /* If we have water reflection, we need to add 180 degree for encode
       * rotation. */
      if (camera_reflect_ipl == IPL_XREF)
      {
        video_encode_rotation += 180;
      }
      if (video_encode_rotation >= 360)
      {
         video_encode_rotation -= 360;
      }

      if (video_encode_rotation == 0)
      {
        camera_odd_encode_rotation = FALSE;
      }
      else if (video_encode_rotation == 90)
      {
        camera_odd_encode_rotation = TRUE;
      }
      else if (video_encode_rotation == 180)
      {
        camera_odd_encode_rotation = FALSE;
      }
      else
      {
        camera_odd_encode_rotation = TRUE;
      }

      break;

    case CAMERA_PARM_CONTRAST:
      /* Update the brightness index */
      if (parm > CAMERA_MAX_CONTRAST)
      {
        ret_val =  CAMERA_INVALID_PARM;
      }
      else if (parm < CAMERA_MIN_CONTRAST)
      {
        ret_val =  CAMERA_INVALID_PARM;
      }
      else
      {
        ret_val = CAMERA_SUCCESS;

        if (camera_parm_contrast.current_value != parm)
        {
          camera_parm_contrast.current_value = parm;

          if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR)
          {
            (void) camsensor_set_contrast ((int8) parm);
          }
          else if ( camera_vfe_control_and_status.reconfigure_vfe == FALSE )
          {
            MSG_CAMERADEBUG("CAMERA_SVCS: Reconfigure VFE camera_stopping:%d", camera_stopping, 0, 0);
            camera_vfe_control_and_status.reconfigure_vfe = TRUE;
            camera_vfe_control_and_status.number_of_frames_to_ignore = 1;
          }
        }
      }
      break;

    case CAMERA_PARM_BRIGHTNESS:
      if (parm > camera_parm_brightness.maximum_value)
      {
        ret_val =  CAMERA_INVALID_PARM;
      }
      else if (parm < camera_parm_brightness.minimum_value)
      {
        ret_val =  CAMERA_INVALID_PARM;
      }
      else
      {
        if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR)
        {
          (void) camsensor_set_brightness ((int8) parm);
          camera_parm_brightness.current_value = parm;
          ret_val = CAMERA_SUCCESS;
        }
#ifndef FEATURE_CAMERA_YCBCR_ONLY
        else
        {
          /* Subtract indices to get the change (positive or negative) */
          int32 new_luma_target = parm - camera_parm_brightness.current_value;

          /* step 2* AEC tolerance here */
          int32 brightness_step = cam3a_aec_state.luma_tolerance*2;

          /* Update the brightness index */
          camera_parm_brightness.current_value = parm;

          /* must change by step size */
          new_luma_target *= brightness_step;

          /* Add current luma */
          new_luma_target += cam3a_aec_state.luma_target;

          /* Set luma variables */
          camera_aec_set_brightness((uint8) new_luma_target);

          ret_val = CAMERA_SUCCESS;
        }
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
      }
      break;

    case CAMERA_PARM_EXPOSURE_COMPENSATION:
     if ((parm > camera_parm_exposure_compensation.maximum_value) ||(parm < camera_parm_exposure_compensation.minimum_value) )
      {
        ret_val =  CAMERA_INVALID_PARM;
      }
      else
      {
        if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR)
        {
          (void) camsensor_set_ev_compensation ((int32)parm);
          camera_parm_exposure_compensation.current_value =  parm;
          ret_val = CAMERA_SUCCESS;
        }
#ifndef FEATURE_CAMERA_YCBCR_ONLY
        else
        {
          ret_val = camera_svcs_set_ev_compensation(parm);
          if(ret_val == CAMERA_SUCCESS)
          {
           /* Update the brightness index */
           camera_parm_exposure_compensation.current_value =  parm;
          }
        }
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
      }
      break;

    case CAMERA_PARM_SHARPNESS:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_EXPOSURE:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_WB:

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

      if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR)
      {
         ret_val=camsensor_set_wb ((int8) parm);
         camera_parm_white_balance.current_value=parm;
      }

      else
      {
         ret_val = camera_awb_set_wb(&(camsensor_static_params[camera_asi]),
                                   parm, camera_parm_special_effects.current_value);
      }
      if(ret_val == CAMERA_SUCCESS)
      {
        /* Update global wb setting */
        camera_parm_white_balance.current_value =  parm;
      }
      break;

    case CAMERA_PARM_EFFECT:

      camera_parm_special_effects.current_value =  parm;
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
     if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR)
      {
         ret_val=camsensor_set_effect ((int8) parm);

      }
     else
     {
        ret_val =  camera_awb_set_special_effect(parm, camera_parm_white_balance.current_value,
                                                 &camera_parm_special_effects.current_value);
     }
     break;

    case CAMERA_PARM_FPS:
      {
        uint16 numerator = 0;
        uint16 denominator = 0;
        uint32 skip_pattern = 0;
        uint32 sensor_fps = 0;
        float  desired_fps = 0.0;

        numerator = (uint16)parm;
        denominator = (uint16)((uint32)parm >> 16);

        if (denominator == 0)
        {
          denominator = 1;
        }

        desired_fps = (float)numerator / (float)denominator;

        if (desired_fps < (float) camera_parm_fps.minimum_value 
			||desired_fps > (float) camera_parm_fps.maximum_value)
        {
          ret_val =  CAMERA_INVALID_PARM;
          break;
        }

        sensor_fps = camsensor_static_params[camera_asi].preview_fps/Q8;

        /* Assumption: Sensor can always support 15fps          */
        /* Force sensor frame rate to 15 for video applications */
        if (sensor_fps > CAMERA_VIDEO_MAX_SENSOR_FRAME_RATE)
        {
#ifndef FEATURE_CAMERA_YCBCR_ONLY
         if(camera_aec_set_parm(CAMERA_PARM_FPS,CAMERA_VIDEO_MAX_SENSOR_FRAME_RATE*Q8)
             == CAMERA_SUCCESS)
           {
#else
          if(camsensor_set_frame_rate(CAMERA_VIDEO_MAX_SENSOR_FRAME_RATE*Q8)
             == CAMERA_SUCCESS)
          {
#endif /* CAMERA_USES_CHROMATIX_0x203 */
  
            sensor_fps = CAMERA_VIDEO_MAX_SENSOR_FRAME_RATE;
            camsensor_static_params[camera_asi].preview_fps =
              CAMERA_VIDEO_MAX_SENSOR_FRAME_RATE*Q8;
          }
          else
          {
            MSG_ERROR("Unable to reduce sensor frame rate from %d to 15",sensor_fps,0,0);
          }
        }

#ifdef FEATURE_VIDEO_ENCODE
        camcorder_svcs_set_parm_fps(numerator, denominator);
#endif /* FEATURE_VIDEO_ENCODE */

        camera_parm_fps.current_value = parm;

        skip_pattern = camera_calculate_skip_pattern(sensor_fps, desired_fps,
                                                     CAMERA_VFE_SKIP_PATTERN_WIDTH);

        if(skip_pattern)
        {
          vfeUpdate.output2SkipPattern.bitPatternWidth = CAMERA_VFE_SKIP_PATTERN_WIDTH;
          vfeUpdate.output2SkipPattern.skipBitPattern  = skip_pattern;
          vfeUpdate.output1SkipPattern.bitPatternWidth = CAMERA_VFE_SKIP_PATTERN_WIDTH;
          vfeUpdate.output1SkipPattern.skipBitPattern  = skip_pattern;

          /* To solve the frame rate updation problem in preview */
          camera_update_vfe = TRUE;

          MSG_HIGH ("Camera set fps %d max sensor rate %d pattern %x",
                     desired_fps, sensor_fps, skip_pattern);
        }
        else
          MSG_ERROR ("Camera cannot set fps, skip pattern returned %x", skip_pattern,0,0);

      }
      break;

#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
    case CAMERA_PARM_USER_ATOM:
		ret_val = camcorder_svcs_set_parm(CAMERA_PARM_USER_ATOM, parm);
		break;
#endif

    case CAMERA_PARM_FLASH:
#ifdef ANY_FLASH
      if (parm == camera_parm_flash.current_value)
      {
        break;
      }
      switch (parm)
      {
        case CAMERA_FLASH_OFF:
          camera_flash_on   = FALSE;
          camera_flash_auto = FALSE;
          break; /* switch parm */

        case CAMERA_FLASH_AUTO:
          if (camsensor_static_params[camera_asi].support_auto_flash)
          {
            camera_flash_on   = FALSE;
            camera_flash_auto = TRUE;
          }
          else
          {
            /* Device driver does not support auto flash, then treated as on */
            camera_flash_on   = TRUE;
            camera_flash_auto = FALSE;
          }
          break; /* switch parm */

        case CAMERA_FLASH_LOW:
        case CAMERA_FLASH_MEDIUM:
        case CAMERA_FLASH_HIGH:
          camera_flash_on   = TRUE;
          camera_flash_auto = FALSE;
          break; /* switch parm */

        case CAMERA_FLASH_CUSTOM:
          camera_flash_on   = FALSE;
          camera_flash_auto = FALSE;
          ret_val = CAMERA_NOT_SUPPORTED;
        break;

        default:
          camera_flash_on   = FALSE;
          camera_flash_auto = FALSE;
          ret_val = CAMERA_INVALID_PARM;
        break;
      }

      if ((ret_val == CAMERA_NOT_SUPPORTED) || 
          (ret_val == CAMERA_INVALID_PARM)) 
      {
        break;
      }

      camera_parm_flash.current_value = parm;

#ifdef  FEATURE_STROBE_FLASH
     if (camera_flash_on || camera_flash_auto)
        strobe_flash_start_charging();
     else
        strobe_flash_stop_charging();
#endif /* FEATURE_STROBE_FLASH */

#else /* No flash is supported */

      ret_val = CAMERA_NOT_SUPPORTED;
#endif /* ANY_FLASH */
      break;

    case CAMERA_PARM_FLASH_STATE:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_NIGHTSHOT_MODE:
      ret_val = camera_aec_set_parm(CAMERA_PARM_NIGHTSHOT_MODE, parm);
      break;

    case CAMERA_PARM_REFLECT:

      if((parm < CAMERA_NO_REFLECT) || (parm >= CAMERA_MAX_REFLECT))
      {
        ret_val = CAMERA_INVALID_PARM;
        break;
      }
      if (parm == camera_parm_reflect.current_value)
      {
        break;
      }
      camera_parm_reflect.current_value = parm;
      graph_queue_camera_func ((uint32)CAMERA_FUNC_SET_REFLECT,
                               (void *) callback, client_data, 0, 0, 0, 0);
      break;

    case CAMERA_PARM_PREVIEW_MODE:
      if ((parm == (int)CAMERA_PREVIEW_MODE_MOVIE) ||
          (parm == (int)CAMERA_PREVIEW_MODE_SNAPSHOT))
      {
        camera_parm_preview_mode.current_value = parm;
        preview_mode                           = (camera_preview_mode_type)parm;
      }
      else
      {
        ret_val = CAMERA_INVALID_PARM;
      }
      break;

    case CAMERA_PARM_ANTIBANDING:

      if ((int)camera_parm_antibanding.current_value != parm)
      {
        if (parm < (int)CAMERA_MAX_ANTIBANDING)
        {
          camera_parm_antibanding.current_value = parm;
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
          if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR)
          {
            ret_val=camsensor_set_antibanding ((int8) parm);

          }

          else
          {

            ret_val = camera_aec_set_antibanding (
           (camera_antibanding_type)camera_parm_antibanding.current_value,
                      &(camsensor_static_params[camera_asi]));
          }

        }
        else
        {
          ret_val = CAMERA_INVALID_PARM;
        }
      }
      break;
    case CAMERA_PARM_RED_EYE_REDUCTION:
#ifdef FEATURE_STROBE_FLASH
      if (parm == camera_parm_red_eye_reduction.current_value)
      {
        break;
      }
      camera_parm_red_eye_reduction.current_value = parm;
#else
      ret_val = CAMERA_NOT_SUPPORTED;
#endif /* FEATURE_STROBE_FLASH */
      break;

    case CAMERA_PARM_FOCUS_STEP:
      /* AF must be both supported by current sensor and enabled */
      if (!camctrl_tbl.af_is_supported)
      {
        ret_val = CAMERA_NOT_SUPPORTED;
      }
      else
      {
        /* Invalid state if the current value is out of range */
        if ((parm < camera_parm_manual_focus.minimum_value) ||
            (parm > camera_parm_manual_focus.maximum_value))
        {

          ret_val = CAMERA_INVALID_PARM;
        }
        else
        {
          camera_parm_manual_focus.current_value = parm;
        }
      }
      break;


    case CAMERA_PARM_EXPOSURE_METERING:
    case CAMERA_PARM_AUTO_EXPOSURE_MODE:  /* Legacy parm, EXPOSURE_METERING has replaced it */

      /* Invalid state if the current value is out of range */
      if ((parm < camera_parm_auto_exposure_mode.minimum_value) ||
          (parm > camera_parm_auto_exposure_mode.maximum_value))
      {

        ret_val = CAMERA_INVALID_PARM;
      }
      else
      {
        /* Even though exposure metering is controlled by bestshot we will allow
         * user to change value after bestshot mode is applied
         */
        camera_parm_auto_exposure_mode.current_value = parm;
        if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR)
        {
            ret_val= camsensor_set_exposure_mode ((int8) parm);
        }

        else
        {

          (void) camera_aec_set_aec_mode( (camera_auto_exposure_mode_type) parm, \
            &(camsensor_static_params[camera_asi]) );
        }

      }

      break;

#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
    case CAMERA_PARM_SHUTTER_SOUND:
      graph_queue_camera_func ((uint32)CAMERA_FUNC_SHUTTER_SND,
                               (void *) callback, client_data,(camera_shutter_snd_play_type)parm, 0, 0, 0);

      break;
#endif /* FEATURE_VIDEO_ENCODE_FADING */

#ifdef FEATURE_VIDEO_ENCODE_FADING
    case CAMERA_PARM_FADING:
      camcorder_svcs_set_parm_fading(parm);
      break;
#endif /* FEATURE_VIDEO_ENCODE_FADING */

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
    case CAMERA_PARM_TRANSITION:
      camcorder_svcs_set_parm_transit(parm);
      break;
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */

    case CAMERA_PARM_ISO:
      camera_parm_iso.current_value = parm;
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
      ret_val = camera_aec_set_parm(CAMERA_PARM_ISO, parm);
      break;

    case CAMERA_PARM_APERTURE:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_SHUTTER_SPEED:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
  case CAMERA_PARM_FRAME_TIMESTAMP:
      ret_val = camcorder_svcs_set_parm_timestamp(parm);
      break;
#endif /* FEATURE_CAMCORDER_OVERLAY_TIMESTAMP */

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

    case CAMERA_PARM_LUMA_ADAPTATION:
      
      if(parm < 0 || parm > 1)
      {
        ret_val = CAMERA_INVALID_PARM;
      }
      if (parm == 0)
      {
        camera_la_enable = FALSE;
      }
      else
      {
        camera_la_enable = TRUE;
      }
      break;

#ifdef FEATURE_VIDEO_ENCODE
    case CAMERA_PARM_SPACE_LIMIT:
      camcorder_svcs_set_parm_space_limit(parm);
      break;
#endif /* FEATURE_VIDEO_ENCODE */

    default:
      MSG_ERROR("Invalid control", 0, 0, 0);
      event_report (EVENT_CAMERA_INVALID_SET_ID);
      ret_val = CAMERA_INVALID_PARM;
      break;
  }
  if (ret_val == CAMERA_SUCCESS)
  {
    if (callback)
    {
      (callback) (CAMERA_EXIT_CB_DONE,
                  client_data,
                  CAMERA_FUNC_SET_PARM,
                  (int)camera_parm_id);
    }
    MSG_HIGH ("Set done.", 0, 0, 0);
    event_report (EVENT_CAMERA_SET_SUCCEEDED);
  }
  return ret_val;

#endif /* T_IO_CARD */
} /* camera_svcs_set_parm */




/*===========================================================================

FUNCTION      CAMERA_SVCS_SET_PARM_2

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
camera_ret_code_type camera_svcs_set_parm_2
(
  camera_parm_type  id,
  int32             parm1,
  int32             parm2,
  camera_cb_f_type  callback,
  void             *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  camera_ret_code_type ret_val = CAMERA_SUCCESS;

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_SET_PARM, (uint32) id, (uint32) parm1, __LINE__);
  MSG_LOW ("Camera set parm2", id, parm1, parm2);

  if (camsensor_initialized == FALSE)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_PARM);
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_PARM);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  camera_parm_id = id;

  switch (id)
  {
    /* For now, this parm only has application for Auto Frame Rate */
    case CAMERA_PARM_PREVIEW_FPS:
      {
        int32 num_possible_frame_rates = camsensor_static_params[camera_asi].num_possible_frame_rates;
        uint16 max_fps, min_fps, numerator, denominator;
        uint16 max_capable_fps, min_capable_fps;

        /* First, AFR must be enabled, and we must support at least 1 frame rate */
        if ((num_possible_frame_rates == 0)
#ifndef FEATURE_CAMERA_YCBCR_ONLY
            || ((camsensor_static_params[camera_asi].output_format == CAMSENSOR_BAYER) &&
                (cam3a_aec_state.afr_enable == FALSE))
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
           )
        {
          ret_val = CAMERA_NOT_SUPPORTED;
          break;
        }

        denominator = (((uint32)parm1 >> 16) & 0xFFFF);
        numerator = (uint16)(parm1 & 0xFFFF);
        if (denominator == 0)
        {
          denominator = CAMERA_FPS_DENOMINATOR;
        }
        /* Translate to Q8 format */
        max_fps = (uint16) ((numerator << 8) / denominator);

        denominator = (((uint32)parm2 >> 16) & 0xFFFF);
        numerator = (uint16)(parm2 & 0xFFFF);
        if (denominator == 0)
        {
          denominator = CAMERA_FPS_DENOMINATOR;
        }
        /* Translate to Q8 format */
        min_fps = (uint16) ((numerator << 8) / denominator);

        /* Max FPS supported from this sensor */
        max_capable_fps = camsensor_static_params[camera_asi].frame_rate_array[0].fps;
        /* All calcs in Q8 format */
        min_capable_fps = camsensor_static_params[camera_asi].frame_rate_array[num_possible_frame_rates-1].fps;

        if ((max_fps > max_capable_fps) ||
            (min_fps < min_capable_fps) ||
            (min_fps == 0) || (max_fps == 0) ||
            (max_fps < min_fps))
        {
          /* Out of range */
          ret_val = CAMERA_INVALID_PARM;
          break;
        }
        else /* in range */
        {
          /* Return current FPS in Q8 format since we cannot return P1 and P2 */
          //camera_parm_preview_fps.current_value = max_fps << 16 | min_fps;
          camera_parm_preview_fps.maximum_value = parm1;
          camera_parm_preview_fps.minimum_value = parm2;
          camera_parm_preview_fps.current_value = camera_parm_preview_fps.minimum_value;
          if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR)
          {
            if ((ret_val = camsensor_set_possible_frame_rate(min_fps, max_fps)) == CAMERA_SUCCESS)
            {
              camsensor_static_params[camera_asi].preview_fps = max_fps;
            }
            else
            {
              MSG_ERROR("Unable to reduce sensor frame rate from %d to %d",
                        camsensor_static_params[camera_asi].preview_fps, max_fps, 0);
            }
          }
#ifndef FEATURE_CAMERA_YCBCR_ONLY
          /* This IF is exclusively for Auto Frame Rate: when
             both Parm1 (max rate) and Parm2 (min rate) are specified
          */
          else
          {
            /* Set the recommended max and min FPS for
               Auto Frame Rate only; Q8 format */
            cam3a_aec_state.afr_max_fps = max_fps;
            cam3a_aec_state.afr_min_fps = min_fps;
          }
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
        } /* max and min are in range */
      }
      break;

    case CAMERA_PARM_AF_MODE:
      {
        camera_parm_af_mode.current_value = parm1;
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
        ret_val = camera_af_config(&(camsensor_static_params[camera_asi]), 
                                    (camera_af_mode_type)parm1, 
                                    AF_PROCESS_UNCHANGED);
        break;
      }
#ifdef FEATURE_VIDEO_ENCODE
    case CAMERA_PARM_AUDIO_FMT:
      ret_val = camcorder_svcs_set_parm_audio_fmt(parm1, parm2);
      break;
#endif /* FEATURE_VIDEO_ENCODE */

    default:
      MSG_ERROR("Invalid control", 0, 0, 0);
      event_report (EVENT_CAMERA_INVALID_SET_ID);
      ret_val = CAMERA_INVALID_PARM;
      break;
  }

  if (ret_val == CAMERA_SUCCESS)
  {
    if (callback)
    {
      (callback) (CAMERA_EXIT_CB_DONE,
                  client_data,
                  CAMERA_FUNC_SET_PARM,
                  (int)camera_parm_id);
    }
    MSG_HIGH ("Set done.", 0, 0, 0);
    event_report (EVENT_CAMERA_SET_SUCCEEDED);
  }
  return ret_val;

#endif /* T_IO_CARD */
} /* camera_svcs_set_parm_2 */




/*===========================================================================

FUNCTION      CAMERA_GET_INFO

DESCRIPTION
              Retrieve sensor info

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/

void camera_svcs_get_info(camera_info_type *info)
{
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_GET_INFO, 0, 0, __LINE__);

  if ((info != NULL) && 
      (camera_state != CAMERA_STATE_INIT))
  {
    /* Sensor ID - not sure exactly what this is, but we'll assume it
       is the sensor enum in a multiple-sensor camera */
    info->sensor_id = (uint16) camera_asi;

    /* char *strncpy( char *strDest, const char *strSource, size_t count ); */
#ifdef CAMERA_USES_LIBSTD
    (void) std_strlcpy (info->sensor_model,
                    camsensor_static_params[camera_asi].sensor_name,
                    31 ); /* truncate to 30 characters */
#else /* CAMERA_USES_LIBSTD */
    (void) strncpy (info->sensor_model,
                    camsensor_static_params[camera_asi].sensor_name,
                    31 ); /* truncate to 30 characters */
    info->sensor_model[31] = 0; /* Null terminate string if not already */
#endif /* CAMERA_USES_LIBSTD */

    /* Assuming we need full size width and height */
    info->sensor_width            = camsensor_static_params[camera_asi].full_size_width;
    info->sensor_height           = camsensor_static_params[camera_asi].full_size_height;
    /* This puts it in Q8 format by default */
    info->fps                     = (uint16) camera_parm_fps.current_value;
    info->sensor_rotation_sensing = FALSE;
    info->default_rotation        = camera_default_preview_rotation;
    info->default_orientation     = camera_default_orientation;
  }

} /* camera_svcs_get_info */



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

camera_ret_code_type camera_svcs_get_parm
(
  camera_parm_type       id,
  camera_parm_info_type *parm
)
{
  camera_parm_info_type camera_parm;
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_GET_PARM, (uint32) id, 0, __LINE__);
  MSG_LOW ("Camera get parm", id, 0, 0);

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  switch (id)
  {
    case CAMERA_PARM_STATE:
      parm->current_value = (int)camera_state;
      break;

    case CAMERA_PARM_ACTIVE_CMD:
      parm->current_value = (int)camera_func;
      break;

    case CAMERA_PARM_ZOOM:
      *parm = camera_parm_zoom;
      break;

    case CAMERA_PARM_PREVIEW_ROTATION:
      *parm = camera_parm_preview_rotation;
      break;

    case CAMERA_PARM_ENCODE_ROTATION:
      *parm = camera_parm_encode_rotation;
      break;

    case CAMERA_PARM_CONTRAST:
      *parm = camera_parm_contrast;
      break;

    case CAMERA_PARM_BRIGHTNESS:
      *parm = camera_parm_brightness;
      break;

    case CAMERA_PARM_SHARPNESS:
      return CAMERA_NOT_SUPPORTED;

    case CAMERA_PARM_EXPOSURE:
      return CAMERA_NOT_SUPPORTED;

    case CAMERA_PARM_WB:
      *parm = camera_parm_white_balance;
      break;

    case CAMERA_PARM_EFFECT:
      *parm = camera_parm_special_effects;
      break;

    case CAMERA_PARM_FPS:
      *parm = camera_parm_fps;
      break;

    case CAMERA_PARM_FLASH:
      *parm = camera_parm_flash;
      break;

    case CAMERA_PARM_FLASH_STATE:
#ifdef FEATURE_STROBE_FLASH
      if (strobe_flash_is_ready() == TRUE) {
        camera_parm_flash_state.current_value = (int)CAMERA_FLASH_READY;
      }
      else
      {
        camera_parm_flash_state.current_value = (int)CAMERA_FLASH_NOT_READY;
      }
#endif /* FEATURE_STROBE_FLASH */
      *parm = camera_parm_flash_state;
      break;

    case CAMERA_PARM_RED_EYE_REDUCTION:
#ifdef FEATURE_STROBE_FLASH
    *parm = camera_parm_red_eye_reduction;
    break;
#else
        return CAMERA_NOT_SUPPORTED;
#endif
      

    case CAMERA_PARM_NIGHTSHOT_MODE:
      return camera_aec_get_parm(CAMERA_PARM_NIGHTSHOT_MODE, parm);

    case CAMERA_PARM_REFLECT:
      *parm = camera_parm_reflect;
      break;

    case CAMERA_PARM_PREVIEW_MODE:
      *parm = camera_parm_preview_mode;
      break;

    case CAMERA_PARM_ANTIBANDING:
      *parm = camera_parm_antibanding;
      break;
      
#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT
#ifdef FEATURE_CAMERA_MULTI_SENSOR
	case CAMERA_PARM_CURRENT_SENSOR:
	  parm->current_value = current_camsensor_pair;
      break;
#endif
#endif
    case CAMERA_PARM_FOCUS_STEP:
      /* AF must be both supported by current sensor and enabled */
      if (!camctrl_tbl.af_is_supported)
      {
        return CAMERA_NOT_SUPPORTED;
      }
      else
      {
        camera_parm = camera_parm_manual_focus;

        /*  The "current_value" here is defined as the current sensor
            position, NOT the requested or default position reflected in:
            "camera_parm_manual_focus.current_value" */
        camera_parm.current_value = camera_af_get_focus_position();

        /* Invalid state if the current value is out of range */
        if ((camera_parm.current_value < camera_parm.minimum_value) ||
            (camera_parm.current_value > camera_parm.maximum_value))
        {

          return CAMERA_INVALID_STATE;
        }
        *parm = camera_parm;
      }
      break;

    case CAMERA_PARM_EXPOSURE_METERING:
    case CAMERA_PARM_AUTO_EXPOSURE_MODE:  /* Legacy parm, EXPOSURE_METERING has replaced it */
      *parm = camera_parm_auto_exposure_mode;
      break;

    case CAMERA_PARM_EXPOSURE_COMPENSATION:
      *parm = camera_parm_exposure_compensation;
      break;

    case CAMERA_PARM_PREVIEW_FPS:
      {
        int32 num_possible_frame_rates = camsensor_static_params[camera_asi].num_possible_frame_rates;
        uint16 max_capable_fps, min_capable_fps;

        /* First, AFR must be enabled, and we must support at least 1 frame rate */
        if ((num_possible_frame_rates == 0)
#ifndef FEATURE_CAMERA_YCBCR_ONLY
            || ((camsensor_static_params[camera_asi].output_format == CAMSENSOR_BAYER) &&
                (cam3a_aec_state.afr_enable == FALSE))
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
           )
        {
          return CAMERA_NOT_SUPPORTED;
        }

        /* Max FPS supported from this sensor */
        max_capable_fps = camsensor_static_params[camera_asi].frame_rate_array[0].fps;
        max_capable_fps = (uint16)((((float)(max_capable_fps))/Q8) +0.5);
        /* All calcs in Q8 format */
        min_capable_fps = camsensor_static_params[camera_asi].frame_rate_array[num_possible_frame_rates - 1].fps;
        min_capable_fps = (uint16)((((float)(min_capable_fps))/Q8) +0.5);
        camera_parm_preview_fps.maximum_value = ((CAMERA_FPS_DENOMINATOR << 16) |
                                                 (0xFFFF & (CAMERA_FPS_DENOMINATOR * max_capable_fps)));
        camera_parm_preview_fps.minimum_value = ((CAMERA_FPS_DENOMINATOR << 16) |
                                                 (0xFFFF & (CAMERA_FPS_DENOMINATOR * min_capable_fps)));
        *parm = camera_parm_preview_fps;
        break;
      }

    case CAMERA_PARM_ISO:
      if( camera_aec_get_parm(CAMERA_PARM_ISO, parm) == CAMERA_SUCCESS )
      {
        parm->current_value = camera_parm_iso.current_value;
      }
      else
      {
        return CAMERA_NOT_SUPPORTED;
      }
      break;

    case CAMERA_PARM_APERTURE:
      return CAMERA_NOT_SUPPORTED;

    case CAMERA_PARM_SHUTTER_SPEED:
      return CAMERA_NOT_SUPPORTED;

    case CAMERA_PARM_LUMA_ADAPTATION:
        parm->current_value = camera_la_enable;
      return CAMERA_SUCCESS;
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
  case CAMERA_PARM_FRAME_TIMESTAMP:
      camcorder_svcs_get_parm(CAMERA_PARM_FRAME_TIMESTAMP, parm);
      break;
#endif /* FEATURE_CAMCORDER_OVERLAY_TIMESTAMP */

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
#ifdef FEATURE_VIDEO_ENCODE
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
#endif //FEATURE_VIDEO_ENCODE
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /*FEATURE_CAMERA_BURST_MODE*/

    default:
      MSG_ERROR ("Invalid control", 0, 0, 0);
      event_report (EVENT_CAMERA_INVALID_SET_ID);
      return CAMERA_INVALID_PARM;
  }
  return CAMERA_SUCCESS;
} /* camera_get_parm */


/*===========================================================================

FUNCTION      CAMERA_GET_PARM_2

DESCRIPTION
              Retrieve control values.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/

camera_ret_code_type camera_svcs_get_parm_2
(
  camera_parm_type      id,
  int32                *parm1_ptr,
  int32                *parm2_ptr)
{
  camera_ret_code_type ret_val = CAMERA_NOT_SUPPORTED;

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  switch (id)
  {
  case CAMERA_PARM_AF_MODE:
    if (camctrl_tbl.af_is_supported)
    {
      *parm1_ptr = camera_parm_af_mode.current_value;
      *parm2_ptr = 0;
      ret_val = CAMERA_SUCCESS;
    }
    break;

  case CAMERA_PARM_FPS_LIST:
    ret_val = camsensor_get_fps_list(parm1_ptr, parm2_ptr);
    break;

  default:
    ret_val = CAMERA_NOT_SUPPORTED;
    break;
  }

  return ret_val;
} /* camera_get_parm_2 */

#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND

static void camera_process_set_parm_shutter_sound
(
  camera_cb_f_type callback,
  void            *client_data,
  camera_shutter_snd_play_type parm
)
{
#ifdef QDSP_MODULE_SA_DS05_DEFINED
  qcamrawHeaderType *thumbnail_header;
#endif /* QDSP_MODULE_SA_DS05_DEFINED */

  camera_app_cb = callback;
  shutter_snd_status = parm;

  MSG_CAMERADEBUG("CAMERA_SVCS: Shutter Sound Status %d",shutter_snd_status ,0, 0);

  switch(parm)
  {
    case NO_SHUTTER_SND:
    case SHUTTER_SND_PLAY_START:
      shutter_snd_status = parm;
    break;

    case SHUTTER_SND_PLAY_DONE:
    case SHUTTER_SND_PLAY_ABORT:

      shutter_snd_status = NO_SHUTTER_SND;

#ifdef QDSP_MODULE_SA_DS05_DEFINED

    if((camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT) &&
        camera_take_picture_status.received_output1 && 
        camera_take_picture_status.received_output2 && 
        camera_take_picture_status.received_camif_done)
    {
        
      if (camera_svcs_is_ycbcr_downsizer_needed())
      {
        thumbnail_header = (qcamrawHeaderType *)snapshot_frame.thumbnail_image;

        camera_snapshot_timing.downsizer_start = (uint32)timetick_get_ms();

        camera_dsp_state = DSP_DISABLED;
        CAMQDSP_Terminate();
        camera_svcs_process_ycbcr_downsize();
        /* Subsample thumbnail image when VFE outputs in H2V1 format */
        if(vfeSnapshotConfig.output1Configuration.imageFormat == VFE_YCbCrH2V1)
        {
          camera_svcs_chroma_downsample_h2v1_to_h2v2(
          qcamrawGetDataCbCrPtr((byte *)thumbnail_header),
          thumbnail_header->dataHeight, thumbnail_header->dataWidth);
        }
      }
    }
#endif /* QDSP_MODULE_SA_DS05_DEFINED */
    break;

    default:
      MSG_CAMERADEBUG("CAMERA_SVCS: Invalid Shutter Sound Status %d",shutter_snd_status ,0, 0);
      shutter_snd_status = NO_SHUTTER_SND;
    break;

  }

}
#endif /* FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND */

#ifdef FEATURE_CAMERA_LPM
#error code not present
#endif /* FEATURE_CAMERA_LPM */

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
/*=============================================================================

FUNCTION      CAMERA_SVCS_PROCESS_YCBCR_DOWNSIZE

DESCRIPTION   This function process downsizing of the image.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void camera_svcs_process_ycbcr_downsize ()
{
  session_id++;
  yds_cmd.client_id = session_id;
  yds_cmd.cb_fun = camera_svcs_ycbcr_downsizer_cb;

  /* Calculating the output  pixel_cnt */
  out_pixel_cnt = yds_cmd.data.out_image.width * yds_cmd.data.out_image.height;

  /* For H2V1 format */
  yds_cmd.data.row_in_increment.Y_increment    = yds_cmd.data.in_image.width;
  yds_cmd.data.row_in_increment.CbCr_increment = yds_cmd.data.in_image.width;

  /*Standalone version DSP does not support fragment but incall version does.*/
  /*Arm does not support either for the time being.*/
  yds_cmd.data.in_frag_n_format.num_fragments = 0;

  yds_cmd.data.in_lines_frag0_1.frag0 = 0;
  yds_cmd.data.in_lines_frag0_1.frag1 = 0;

  yds_cmd.data.in_lines_frag2_3.frag0 = 0;
  yds_cmd.data.in_lines_frag2_3.frag1 = 0;

  yds_cmd.data.in_Yaddress_frag1.address    = NULL;
  yds_cmd.data.in_CbCraddress_frag1.address = NULL;
  yds_cmd.data.in_Yaddress_frag2.address    = NULL;
  yds_cmd.data.in_CbCraddress_frag2.address = NULL;

  yds_cmd.data.in_Yaddress_frag3.address    = NULL;
  yds_cmd.data.in_CbCraddress_frag3.address = NULL;

  yds_cmd.data.row_out_increment.Y_increment    = yds_cmd.data.out_image.width;
  yds_cmd.data.row_out_increment.CbCr_increment = yds_cmd.data.out_image.width;

  yds_cmd.data.out_frag_n_format.num_fragments = 0;
  yds_cmd.data.out_frag_n_format.format = YDS_H2V2;

  yds_cmd.data.out_lines_frag0_1.frag0 = 0;
  yds_cmd.data.out_lines_frag0_1.frag1 = 0;
  yds_cmd.data.out_lines_frag2_3.frag0 = 0;
  yds_cmd.data.out_lines_frag2_3.frag1 = 0;

  /* Check whether it is using incall downsizer */
  if(camctrl_tbl.use_incall_downsizer == TRUE)
  {
    /*Updating Input/Output Lines fragmenet 0 to height*/
    yds_cmd.data.in_lines_frag0_1.frag0 = yds_cmd.data.in_image.height;
    yds_cmd.data.out_lines_frag0_1.frag0 = yds_cmd.data.out_image.height;

    /* When using the Incall downsizer, the process in not inplace */
    /* Allocate a buffer, which is ( 1.5 times of the output image size + */
    /* qcamera_header_size) so that when we return from downsizer sucessfully,*/
    /* We will be able to copy the snapshot_frame.buffer header into it*/
    /* The expected output format is line packed H2V2 */
    CAM_Free (downsizer_out_luma_buffer);
    CAM_Free (downsizer_out_chroma_buffer);
    downsizer_out_chroma_buffer = NULL;
    downsizer_out_luma_buffer  = CAM_Malloc((qcamraw_header_size + out_pixel_cnt)  | CAM_ALLOC_NO_ZMEM );
    if(downsizer_out_luma_buffer == NULL)
    {
      /* Insufficent memory */
      /* Printing the Insufficent memory status */
      MSG_HIGH("CAMERA_SVCS: Insufficent memory for the new buffer required by ycbcrdownsizer",0 ,0, 0);
      camera_no_memory();
      return;
    }
    downsizer_out_chroma_buffer  = CAM_Malloc( (out_pixel_cnt/2) | CAM_ALLOC_NO_ZMEM );
    if(downsizer_out_chroma_buffer == NULL)
    {
      CAM_Free (downsizer_out_luma_buffer);
      downsizer_out_luma_buffer = NULL;
      /* Insufficent memory */
      /* Printing the Insufficent memory status */
      MSG_HIGH("CAMERA_SVCS: Insufficent memory for the new buffer required by ycbcrdownsizer",0 ,0, 0);
      camera_no_memory();
      return;
    }
    /*Copy the Snapshot_frame.buffer header to the newly created buffer*/
    /*When downsizer is suceeded, this will be the new ptr which carries the new address*/
    memcpy (downsizer_out_luma_buffer, snapshot_frame.buffer, qcamraw_header_size);
    yds_cmd.data.out_Yaddress_frag0.address =
      (uint32 *)((uint32)downsizer_out_luma_buffer + qcamraw_header_size);
    yds_cmd.data.out_CbCraddress_frag0.address =
      (uint32 *)((uint32)downsizer_out_chroma_buffer);
  }
  /* Check whether it is using standalone downsizer */
  else
  {
    /* Same as input location since inplace */
    yds_cmd.data.out_Yaddress_frag0.address    = yds_cmd.data.in_Yaddress_frag0.address;
    yds_cmd.data.out_CbCraddress_frag0.address = yds_cmd.data.in_CbCraddress_frag0.address;
  }

  yds_cmd.data.out_Yaddress_frag1.address    = NULL;
  yds_cmd.data.out_CbCraddress_frag1.address = NULL;

  yds_cmd.data.out_Yaddress_frag2.address    = NULL;
  yds_cmd.data.out_CbCraddress_frag2.address = NULL;

  yds_cmd.data.out_Yaddress_frag3.address    = NULL;
  yds_cmd.data.out_CbCraddress_frag3.address = NULL;

  camera_take_picture_status.ycbcr_downsizer_in_progress = TRUE;

  /* if command is accepted, caller should expect a callback from downsizer */
  if (ycbcr_downsizer_process_data (&yds_cmd) != YCBCR_DOWNSIZER_CMD_ACCEPT_RET)
  {
    ycbcr_downsizer_cbmsg_type ret_msg;
    ret_msg.client_id = session_id;
    ret_msg.status = YCBCR_DOWNSIZER_INVALID_CMD_MSG;
    camera_svcs_ycbcr_downsizer_cb (ret_msg);
  }
}
#endif /*QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
/*===========================================================================

FUNCTION      CAMERA_SVCS_YCBCR_DOWNSIZER_CB

DESCRIPTION   Process callback from function camera_srvs_ycbcr_downsizer_cb.
              Regardless of ycbcr_downsizer return status we need to notify OEM layer
              with callback function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_svcs_ycbcr_downsizer_cb (ycbcr_downsizer_cbmsg_type  msg)
{
  /* Default status to success. Change if failure encountered */
  camera_cb_type ycbcr_downsizer_status = CAMERA_EXIT_CB_DONE;
  if (msg.client_id == session_id)
  {
    if (msg.status == YCBCR_DOWNSIZER_SUCCESS_MSG)
    {
       qcamrawHeaderType * main_image_header = NULL;
      /* Checking whether it is using incall downsizer */
      if(camctrl_tbl.use_incall_downsizer == TRUE)
      {
        snapshot_frame.buffer = (byte *) ((uint32) downsizer_out_luma_buffer) ;
        /*Free the raw snapshot_frame buffer */
        CAM_Free(snapshot_luma_buf);
        snapshot_luma_buf = NULL;
        CAM_Free(snapshot_chroma_buf);
        snapshot_chroma_buf = NULL;
      }

      main_image_header = (qcamrawHeaderType *) snapshot_frame.buffer;
      if(camctrl_tbl.use_incall_downsizer == TRUE)
      {
        main_image_header->chromaBuffer = (byte *)(downsizer_out_chroma_buffer);
        main_image_header->padSize  = 0;
      }

      camera_snapshot_timing.downsizer_end = (uint32)timetick_get_ms();
      main_image_header->dataWidth  = yds_cmd.data.out_image.width;
      main_image_header->dataHeight = yds_cmd.data.out_image.height;

      main_image_header->YSize    = out_pixel_cnt;
      main_image_header->CbCrSize = main_image_header->YSize / 2;

      snapshot_frame.captured_dx = yds_cmd.data.out_image.width;
      snapshot_frame.captured_dy = yds_cmd.data.out_image.height;

      /* Do the overlay on the main frame and thumbnail */
      /* Currently, IPL can only support H2V2 for overlay */
      if ((encode_overlay != NULL) && (snapshot_frame.format == CAMERA_H2V2))
      {
        camera_apply_overlay_on_snapshot();
      }

      MSG_HIGH("CAMERA_SVCS: ycbcr_downsizer succeed",0 ,0, 0);
      ycbcr_downsizer_status = CAMERA_EXIT_CB_DONE;
    }
    else
    {
      if(camctrl_tbl.use_incall_downsizer == TRUE)
      {
        if (downsizer_out_luma_buffer != NULL)
        {
          /* Free the new buffer that has been created */
          /* while using incall downsizer */
          CAM_Free (downsizer_out_luma_buffer);
          downsizer_out_luma_buffer = NULL;
          CAM_Free (downsizer_out_chroma_buffer);
          downsizer_out_chroma_buffer = NULL;
        }
      }

      if (msg.status == YCBCR_DOWNSIZER_INVALID_CMD_MSG)
      {
        MSG_HIGH("CAMERA_SVCS: Invalid command for ycbcrdownsizer",0 ,0, 0);
        ycbcr_downsizer_status = CAMERA_EXIT_CB_FAILED;
        /* Check whether it is using incall downsizer */
      }
      else if (msg.status == YCBCR_DOWNSIZER_ABORT_MSG)
      {
        MSG_HIGH("CAMERA_SVCS: ycbcr_downsizer is aborted",0 ,0, 0);
      }
      else
      {
        MSG_ERROR("CAMERA_SVCS: ycbcr_downsizer failed",0 ,0, 0);
        ycbcr_downsizer_status = CAMERA_EXIT_CB_FAILED;
      }
    }
    camera_take_picture_status.ycbcr_downsizer_in_progress = FALSE;
    if (camera_take_picture_status.abort)
    {
      camera_svcs_queue_call_to_terminate();
    }
    else
    {
      event_report(EVENT_CAMERA_PICTURE_TAKEN);
      camera_terminate(ycbcr_downsizer_status, (int32) &snapshot_frame);
    }
  }
  else
  {
    MSG_ERROR("CAMERA_SVCS: wrong ycbcr_downsizer session id",0 ,0, 0);
  }
} /*camera_svcs_ycbcr_downsizer_cb */
#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED */

/*===========================================================================

FUNCTION      CAMERA_JPEG_ENCODE_CALLBACK

DESCRIPTION
  Callback function for JPEG encoding module. This function
  gets called once the encoding is finished and the image is stored
  onto the storage medium.

DEPENDENCIES
  camera_state should be in READY once returned.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_jpeg_encode_callback(JPEGENC_CBrtnType  *clientData )
{
  /* Default status to success. Change if failure encountered */
  camera_cb_type jpeg_callback_status   = CAMERA_EXIT_CB_DONE;
  uint32         jpeg_encoded_data_size = 0;


  MSG_FATAL ("camera_jpeg_encode_callback", 0, 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if (clientData)
  {
    switch (clientData->status)
    {
      case JPEGENC_IMG_DONE:

        /* Set the encoded data size */
        jpeg_encoded_data_size = clientData->size;
        camera_snapshot_information.encoded_data_size = jpeg_encoded_data_size;

        if (encodeInfo.Dest.device == JPEGENC_EFS)
        {
          event_report_payload (EVENT_CAMERA_PICTURE_SAVED, (uint8) strlen(camera_handle.efs.filename), encodeInfo.Dest.handle.efs.filename);
        }
        else
        {
          event_report_payload (EVENT_CAMERA_PICTURE_SAVED, 0, 0);
        }

        /* Set the camera_state to ready */
        MSG_FATAL("Encoding done. State transition to ready", 0, 0, 0);
        break;

      case JPEGENC_FILE_SIZE_FAIL:
        MSG_FATAL("JPEG encoder aborted due to file size error.", 0, 0, 0);
        jpeg_callback_status = CAMERA_EXIT_CB_FILE_SIZE_EXCEEDED;
        break;

      case JPEGENC_IMG_ABORT:
      default:
        MSG_FATAL("Encoding Aborted. State transition to ready", 0, 0, 0);
        jpeg_callback_status = CAMERA_EXIT_CB_FAILED;
        break;
     }
  }
  else
  {
    MSG_FATAL("No data from Encoder. State transition to ready", 0, 0, 0);
    jpeg_callback_status = CAMERA_EXIT_CB_FAILED;
  }

  /* clean up before exit */
  camera_clr_take_picture ();

  camera_take_picture_status.encode_in_progress = FALSE;

  if (camera_take_picture_status.abort)
  {
    camera_svcs_queue_call_to_terminate();
  }
  else
  {
    camera_snapshot_timing.encode_end = (uint32)timetick_get_ms();
    if (camera_snapshot_timing.encode_end > camera_snapshot_timing.encode_start)
    {
      MSG_CAMERA_LATENCY_PROFILING("Jpeg encoding took      %d ms", camera_snapshot_timing.encode_end -
                    camera_snapshot_timing.encode_start, 0, 0);
    }
    camera_snapshot_timing.snapshot_completion_callback_start = (uint32)timetick_get_ms();
    camera_terminate(jpeg_callback_status, (int32) jpeg_encoded_data_size );
  }

#ifdef FEATURE_CAMERA_3A_PROFILING
 camera_3a_shared_show_timing();
#endif /* FEATURE_CAMERA_3A_PROFILING */

} /* camera_jpeg_encode_callback */


/*===========================================================================

FUNCTION      CAMERA_JPEG_ENCODE

DESCRIPTION
  Entry point for JPEG encoding. It is a synchronous function.

DEPENDENCIES
  JPEG requires VFE to produce an Image of dimensions multiple of 16.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_jpeg_encode (qcamrawHeaderType *main_image_header, uint8 *main_image_buffer, qcamrawHeaderType *thumbnail_image_header, uint8 *thumbnail_image_buffer)
{
  uint8  jpeg_encode_fragments;
  JPEGENC_formatType jpeg_format;
  JPEGENCReturnCodeType jpeg_encoder_status;

  camera_snapshot_timing.encode_start = (uint32)timetick_get_ms();

  MSG_HIGH ("camera_jpeg_encode", 0, 0, 0);
  encodeInfo.ClientId = 12;
  encodeInfo.CallBack = camera_jpeg_encode_callback;

  if (camera_handle.device == CAMERA_DEVICE_MEM) {
    encodeInfo.Dest.device            = JPEGENC_MEM;
    encodeInfo.Dest.handle.mem.buffer = camera_handle.mem.buffer;
    encodeInfo.Dest.handle.mem.length = camera_handle.mem.length;
  }
  else if (camera_handle.device == CAMERA_DEVICE_EFS){
    encodeInfo.Dest.device = JPEGENC_EFS;
    /* ensure null termination in case dst string is longer than src */
    encodeInfo.Dest.handle.efs.filename[FS_FILENAME_MAX_LENGTH - 1] = '\0';
#ifdef CAMERA_USES_LIBSTD
    (void)std_strlcpy(encodeInfo.Dest.handle.efs.filename, camera_handle.efs.filename,
                  FS_FILENAME_MAX_LENGTH_P - 1 );
#else /* CAMERA_USES_LIBSTD */
    (void)strncpy(encodeInfo.Dest.handle.efs.filename, camera_handle.efs.filename,
                  FS_FILENAME_MAX_LENGTH_P - 1 );
#endif /* CAMERA_USES_LIBSTD */
  }
  else {
    return;
  }

  /*
   * Input format - Rotation supported
   * H2V2 - 0, 90, 180, 270
   * H2V1 - 0, 180
   * H1V2 - 90, 270
   */

  switch (camera_snapshot_format)
  {
    case CAMERA_H2V1:
      jpeg_format = JPEGENC_H2V1;
      MSG_CAMERADEBUG("Encode snapshot: JPEG H2V1 quality %d, filesize %d",
                      camera_encode_properties.quality,
                      camera_encode_properties.file_size, 0);
      break;

    case CAMERA_H2V2:
    default:
      jpeg_format = JPEGENC_H2V2;
      MSG_CAMERADEBUG("Encode snapshot: JPEG H2V2 quality %d, filesize %d",
                      camera_encode_properties.quality,
                      camera_encode_properties.file_size, 0);
      break;
  }

  encodeInfo.Rotation        = camera_jpeg_encode_rotation_format;
  encodeInfo.Main.Quality    = (uint16) camera_encode_properties.quality;

  /* Original image dimensions requested */
  encodeInfo.Main.OrigHeight = main_image_header->picHeight;
  encodeInfo.Main.OrigWidth  = main_image_header->picWidth;

  encodeInfo.Main.Width   = main_image_header->dataWidth;
  /* If sensor cannot provide requested width
   * trim requested width. VFE captured image width is always
   * multiple of 16.
   */
  if (encodeInfo.Main.OrigWidth > encodeInfo.Main.Width)
  {
    encodeInfo.Main.OrigWidth = encodeInfo.Main.Width;
  }

  encodeInfo.Main.Height = main_image_header->dataHeight;

  /* If sensor cannot provide requested height
   * trim requested height. VFE captured image height is always
   * multiple of 16.
   */
  if (encodeInfo.Main.OrigHeight > encodeInfo.Main.Height)
  {
    encodeInfo.Main.OrigHeight = encodeInfo.Main.Height;
  }

  encodeInfo.Main.Subsample = jpeg_format;
  encodeInfo.Main.FragCnt   = 1;

  /* JPEG H/W supports upto 8 fragments. Initialize all the fragments to
   * valid buffers. Valid fragments avoid fatal errors should the H/W
   * overflow happen.
   */
  for (jpeg_encode_fragments = 0; jpeg_encode_fragments < 8; ++jpeg_encode_fragments)
  {
    encodeInfo.Main.Fragmnt[jpeg_encode_fragments].Luma_ptr   = main_image_buffer;
    encodeInfo.Main.Fragmnt[jpeg_encode_fragments].Chroma_ptr = main_image_header->chromaBuffer;
    encodeInfo.Main.Fragmnt[jpeg_encode_fragments].Height     = main_image_header->dataHeight;
    encodeInfo.Main.Fragmnt[jpeg_encode_fragments].Reserved   = 0;
  }

  /* Encode thumbnail only if the main image is more than QVGA size */
  if ( (main_image_header->picWidth > 320) &&
       (main_image_header->picHeight > 240) )
  {
    encodeInfo.HasThumbnail               = TRUE;
    encodeInfo.Thumbnail.Quality          = (uint16) thumbnail_quality;
    encodeInfo.Thumbnail.Width            = thumbnail_image_header->dataWidth;
    encodeInfo.Thumbnail.Height           = thumbnail_image_header->dataHeight;
    encodeInfo.Thumbnail.OrigWidth        = thumbnail_image_header->picWidth;
    encodeInfo.Thumbnail.OrigHeight       = thumbnail_image_header->picHeight;
    encodeInfo.Thumbnail.Subsample      = jpeg_format;
    encodeInfo.Thumbnail.Data.Luma_ptr    = thumbnail_image_buffer;
    encodeInfo.Thumbnail.Data.Chroma_ptr  = thumbnail_image_header->chromaBuffer;
    encodeInfo.Thumbnail.Data.Height      = thumbnail_image_header->dataHeight;
    encodeInfo.Thumbnail.Data.Reserved    = 0;
  }
  else
  {
    encodeInfo.HasThumbnail = FALSE;

  }

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */
  if (encodeInfo.HasThumbnail)
  {
    if (encodeInfo.Thumbnail.Subsample == JPEGENC_H2V2)
    {
      if (thumbnail_image_header->dataHeight > thumbnail_image_header->picHeight)
      {
        /* There is padding in thumbnail buffer, need to fill the padding with
         * either the first line or the last line data from VFE. Otherwise,
         * jpege_vertical_upsample() would use the garbage data in the buffer,
         * causing wrong cbcr information passed to JPEG encoder.
         *
         * Initialize the pointers assuming the padding is at the beginning
         * in thumbnail buffer.
         */
        uint8 *y_padding = encodeInfo.Thumbnail.Data.Luma_ptr;
        uint8 *cbcr_padding = encodeInfo.Thumbnail.Data.Chroma_ptr;
        uint8 *y_line, *cbcr_line;
        uint8 i;

        if (((encodeInfo.Rotation == JPEGENC_CLK90) ||
             (encodeInfo.Rotation == JPEGENC_CCLK270) ||
             (encodeInfo.Rotation == JPEGENC_CLK180) ||
             (encodeInfo.Rotation == JPEGENC_CCLK180)))
        {
          /* The padding is at the beginning in thumbnail buffer, y_line
           * and cbcr_line are pointing to the first line of VFE data.
           */
          y_line    = y_padding + (thumbnail_image_header->picWidth) *
                      (thumbnail_image_header->dataHeight - thumbnail_image_header->picHeight);
          cbcr_line = cbcr_padding + (thumbnail_image_header->picWidth) *
                      (thumbnail_image_header->dataHeight - thumbnail_image_header->picHeight) / 2;
        }
        else
        {
          /* The padding is at the end in thumbnail buffer, y_line and
           * cbcr_line are pointing to the last line of VFE data.
           */
          y_padding    += (thumbnail_image_header->picWidth * thumbnail_image_header->picHeight);
          y_line        = y_padding - thumbnail_image_header->picWidth;
          cbcr_padding += (thumbnail_image_header->picWidth * thumbnail_image_header->picHeight) / 2;
          cbcr_line     = cbcr_padding - thumbnail_image_header->picWidth;
        }

        /* Fill the padding with either the first line or the last line data from VFE */
        for (i = 0;  i < (thumbnail_image_header->dataHeight - thumbnail_image_header->picHeight) / 2; i++)
        {
          memcpy(y_padding, y_line, thumbnail_image_header->picWidth);
          y_padding += thumbnail_image_header->picWidth;
          memcpy(y_padding, y_line, thumbnail_image_header->picWidth);
          y_padding += thumbnail_image_header->picWidth;

          memcpy(cbcr_padding, cbcr_line, thumbnail_image_header->picWidth);
          cbcr_padding += thumbnail_image_header->picWidth;
        }
      }

      if ( (encodeInfo.Rotation == JPEGENC_CCLK90) ||
           (encodeInfo.Rotation == JPEGENC_CCLK270) )
      {
        jpege_horizontal_upsample(encodeInfo.Thumbnail.Data.Chroma_ptr, encodeInfo.Thumbnail.Height / 2, encodeInfo.Thumbnail.Width / 2);
      }
      else
      {
        jpege_vertical_upsample(encodeInfo.Thumbnail.Data.Chroma_ptr, encodeInfo.Thumbnail.Height / 2, encodeInfo.Thumbnail.Width / 2);
      }
      encodeInfo.Thumbnail.Subsample = JPEGENC_H2V1;
    }
  }

  /* Tell JPEG about size limit (in K bytes units) */
  encodeInfo.TargetEncodedSize = (uint16) camera_encode_properties.file_size;

  #ifdef    FEATURE_CAMERA_MOBICAT
  if ((camsensor_static_params[camera_asi].output_format == CAMSENSOR_BAYER) ||
      (camctrl_tbl.af_is_supported))
  {
    mobicat_deliver_config_to_jpege();
  }
  #endif /* FEATURE_CAMERA_MOBICAT */

  if (encodeInfo.TargetEncodedSize)
  {
    jpeg_encoder_status = jpege_encode_file_size_exif ( &encodeInfo, &camera_exif_info);
  }
  else
  {
    jpeg_encoder_status = jpege_encode_exif ( &encodeInfo, &camera_exif_info);
  }

  if (jpeg_encoder_status == JPEGENC_SUCCESS)
  {
    /* set flag to let us know encoding is in progress */
    camera_take_picture_status.encode_in_progress = TRUE;
  }
  else
  {
    MSG_ERROR("JPEG encoder failed. Status=%d", jpeg_encoder_status, 0, 0);
    camera_terminate(CAMERA_EXIT_CB_FAILED, (uint32)jpeg_encoder_status);
  }

} /* camera_jpeg_encode */

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

void camera_svcs_process_func
(
  camera_func_type function,
  void  *callback,
  void  *client_data,
  uint32 info1,
  uint32 info2,
  uint32 info3,
  uint32 info4
)
{
  camera_cb_f_type                  cb       = (camera_cb_f_type) callback;
  void                             *data     = client_data;
  camera_log (LOG_PFUNC, (int)function, 0, 0, __LINE__);
  switch (function)
  {
    case CAMERA_FUNC_START:
      camera_process_start (cb, data, (rex_tcb_type *) info1, (camera_ret_code_type *) info2);
      break;

    case CAMERA_FUNC_STOP:
      camera_process_stop (cb, data, (rex_tcb_type *) info1);
      break;

    case CAMERA_FUNC_CLEANUP:
      camera_process_cleanup ((rex_tcb_type *) info1);
      break;

    case CAMERA_FUNC_START_PREVIEW:
      camera_process_start_preview (cb, data);
      break;

    case CAMERA_FUNC_STOP_PREVIEW:
      camera_process_stop_preview ((rex_tcb_type *) info1);
      break;

    case CAMERA_FUNC_TAKE_PICTURE:
      camera_process_take_picture (cb, data
                                  );
      break;

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

    case CAMERA_FUNC_RELEASE_PICTURE:
      camera_process_release_picture ((rex_tcb_type *) info1);
      break;

    case CAMERA_FUNC_ENCODE_PICTURE:
      camera_process_encode_picture (cb, data, (qcamrawHeaderType *) info1,
                                     (uint8 *) info2,
                                     (qcamrawHeaderType *) info3, (uint8 *) info4);
      break;

#ifdef FEATURE_VIDEO_ENCODE
    case CAMERA_FUNC_START_RECORD:
      camcorder_process_start_record (cb, data);
      break;

    case CAMERA_FUNC_PAUSE_RECORD:
      camcorder_process_pause_record ();
      break;

    case CAMERA_FUNC_RESUME_RECORD:
      camcorder_process_resume_record ();
      break;

    case CAMERA_FUNC_STOP_RECORD:
      camcorder_process_stop_record ();
      break;
#endif /* FEATURE_VIDEO_ENCODE */

#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
    case CAMERA_FUNC_VIDEO_HANDSHAKE:
      {
        if (camera_state != CAMERA_STATE_RECORDING &&
            camera_state != CAMERA_STATE_QVP_ENCODING)
        {
          return;
        }
#ifdef FEATURE_QVPHONE

         // Assume camera_qvp_enabled can only be disabled by QVP after this
         // handshake process is already completed (this is assuming GRAPH
         // task is a higher priority task than QVP IO task
        if(camera_qvp_enabled == TRUE )
        {
          if ((camera_state == CAMERA_STATE_QVP_ENCODING) &&
              (camera_qvp_video_handshake == TRUE))
          {

             QVP_MSG_VENC_LOW("QVP Video Encode Frame Handshake", 0, 0, 0);
#ifndef FEATURE_QVP_OFFLINE_VIDEO_ENCODING
             if(output2_ack == CAMERA_DELAYED_ENCODE_FRAME_SIZE)
#endif //FEATURE_QVP_OFFLINE_VIDEO_ENCODING
             {
               CAMQDSP_ReturnCodeType Ret1;
               camera_dsp_command = CAMQDSP_CMD_END_OF_FRAME_ACK2;
               Ret1 = VFE_EOF_Ack2 (ackOutput2);
               if((Ret1 == CAMQDSP_INVALID_STATE) ||
                 (Ret1 == CAMQDSP_INVALID_PARM))
               {
                 QVP_MSG_VENC_FATAL(" VFE_EOF_Ack2 Failed return type = %d", Ret1, 0, 0);
               }
             }

          }

          //Skip the following QCamcorder Processing if QVP is active
          //Need to clean it up
          break;
        }
#endif
#ifdef FEATURE_VIDEO_ENCODE
      camcorder_svcs_process_video_hanshake();
#endif /* FEATURE_VIDEO_ENCODE */
    }
    break;
#endif /* FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE*/

#ifdef FEATURE_QVPHONE
    case CAMERA_FUNC_START_QVP_ENCODE:
      camera_process_start_qvp_encode (cb, data, (rex_tcb_type *) info1, (camera_ret_code_type *) info2);
      break;
#endif /* FEATURE_QVPHONE */

#ifdef FEATURE_QVPHONE
    case CAMERA_FUNC_STOP_QVP_ENCODE:
      camera_process_stop_qvp_encode ((rex_tcb_type *) info1);
      break;
#endif /* FEATURE_QVPHONE */

    case CAMERA_FUNC_ENABLE_FRAME_CALLBACK:
      camera_process_enable_frame_callback ();
      break;

    case CAMERA_FUNC_DISABLE_FRAME_CALLBACK:
      camera_process_disable_frame_callback ();
      break;

    case CAMERA_FUNC_RELEASE_FRAME:
      camera_process_release_frame ((uint8) info1);
      break;

    case CAMERA_FUNC_BLT:
      camera_process_blt ((uint8) info1);
      break;

    case CAMERA_FUNC_SET_OVERLAY:
      camera_process_set_overlay (cb, data, (rex_tcb_type *) info1);
      break;

    case CAMERA_FUNC_CLR_OVERLAY:
      camera_process_clr_overlay (cb, data);
      break;

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
    case CAMERA_FUNC_SET_ICON_ARRAY:
      camera_process_set_icon_array ((rex_tcb_type*)info1, (ipl_icon_type **)info2, info3, info4);
      break;
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
    case CAMERA_FUNC_CLR_ICON_ARRAY:
      camera_process_clr_icon_array ();
      break;
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */

    case CAMERA_FUNC_SET_REFLECT:
      camera_process_set_reflect ();
      break;

#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
    case CAMERA_FUNC_SHUTTER_SND:
      camera_process_set_parm_shutter_sound(cb, data,(camera_shutter_snd_play_type)info1);
      break;
#endif /* FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND */

    case CAMERA_FUNC_SET_DIMENSIONS:
      camera_process_set_dimensions (cb, data,(rex_tcb_type*)info1);
      break;

    case CAMERA_FUNC_SET_POSITION:
      camera_process_set_position (cb, data);
      break;

    case CAMERA_FUNC_SET_EXIF_TAG:
      camera_process_set_exif_tag (cb, data);
      break;

#ifdef FEATURE_VIDEO_ENCODE
    case CAMERA_FUNC_VIDEO_ENGINE_CB:
      camcorder_process_video_engine_cb (cb, data, &runningOutput2SkipPattern, &encoder_acked);
      break;
    case CAMERA_FUNC_AUDIO_READY_CB:
      camcorder_process_audio_ready_cb ();
      break;
#endif /* FEATURE_VIDEO_ENCODE */

    case CAMERA_FUNC_START_FOCUS:
      camera_af_process_start_focus(cb, data, (camera_focus_e_type)info1);
      break;

    case CAMERA_FUNC_MOVE_LENS_DONE:
      camera_af_process_lens_move_done(info1);
      break;

    case CAMERA_FUNC_HANDLE_ERROR:
      camera_process_handle_error ();
      break;

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE	  
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/	  
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */
    case CAMERA_FUNC_TERMINATE:
      camera_process_terminate();
      break;

    default:
      break;
  }
} /* camera_process_func */



/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_MSG

DESCRIPTION
              Processes QDSP events in graph task context.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_svcs_process_qdsp_msg (uint32 msg, void *buffer)
{
  CAMQDSP_ResponseMsgType qdsp_msg = (CAMQDSP_ResponseMsgType) msg;

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  camera_dsp_response = qdsp_msg;
  camera_log (LOG_RSP, 0, (int)qdsp_msg, 0, __LINE__);  
  switch (qdsp_msg)
  {
    case CAMQDSP_DOWNLOAD_DONE:

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

      MSG_CAMERADEBUG ("CAMERA_SVCS: DSP Image downloaded", 0, 0, 0);
      camera_dsp_state = DSP_ENABLED;

      camera_service_unavailable = FALSE;
      clk_dereg(&camera_service_unavailable_clk);
      if (camera_stopping)
      {
        camera_process_terminate();
        return;
      }

      if ((camera_state == CAMERA_STATE_TAKE_PICTURE)
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
         )

      {
        (void) camera_config_vfe();
      }

#ifdef FEATURE_VIDEO_ENCODE
      /* If in video recording state, initialize the engine,
       * otherwise fall through as if DSP is idle. */
      else if (camera_state == CAMERA_STATE_RECORDING)
      {
        camcorder_process_qdsp_start_record_msg(video_encode_rotation);
      }
#endif /* FEATURE_VIDEO_ENCODE */
      else
      {
        MSG_CAMERADEBUG("CAMERA_SVCS: Image Downloaded now decide whether to CONFIG VFE", 0, 0, 0);
        camera_process_qdsp_idle_state_msg ();
      }
      break;
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */

#ifdef FEATURE_VIDEO_ENCODE
    case CAMQDSP_OUTPUT2ACK2_CRITERIA:
      output2enc_ack2_criteria(HANDSHAKE_RECEIVED2);
      break;
#endif

#ifdef CAMCORDER_VFE_RECONFIG_TEST
  case CAMQDSP_VFE_RECONFIGURE_UNIT_TEST:
    (void) vfe_process_reconfigure_unit_test();
    break;
#endif

    case CAMQDSP_MSG_IDLE_COMPLETE:
      MSG_CAMERADEBUG ("CAMERA_SVCS: Received IDLE_COMPLETE message from DSP", 0, 0, 0);
      camera_process_qdsp_idle_state_msg ();
      break;

    case CAMQDSP_MSG_OUTPUT1_END_OF_FRAME:
#ifdef HANDLE_CAMIF_ERROR
      if (camera_camif_error_clk_running)
      {
        clk_dereg(&camera_camif_error_clk);
        camera_camif_error_clk_running = FALSE;
      }
#endif /* HANDLE_CAMIF_ERROR */
      camera_process_qdsp_output1_msg ((Camera_EndOfFrameMessageType *) buffer);
      break;

    case CAMQDSP_MSG_OUTPUT2_END_OF_FRAME:
      camera_process_qdsp_output2_msg ((Camera_EndOfFrameMessageType *) buffer);
      break;

    case CAMQDSP_MSG_AHB_OVERFLOW_ERROR:
      camera_dsp_error_status.received_ahb_overflow_error = TRUE;
      camera_process_qdsp_detect_error_msg (CAMQDSP_MSG_AHB_OVERFLOW_ERROR);
      break;

    case CAMQDSP_MSG_CAMIF_ERROR:
      /* Simply increment count and set a flag */
      dsp_camif_error_count++;
      MSG_HIGH("dsp_camif_error_count - %d", dsp_camif_error_count, 0, 0);
      camera_dsp_error_status.received_camif_error = TRUE;
#ifdef HANDLE_CAMIF_ERROR
      /* Bail out if no output1 is not received in half a second. */
      clk_reg (&camera_camif_error_clk, &camera_camif_error_clk_cb, (int32) 500, 0, FALSE);
      camera_camif_error_clk_running = TRUE;
#endif /* HANDLE_CAMIF_ERROR */
      break;

    case CAMQDSP_MSG_MICRO_DMA_OVERFLOW_ERROR:
      camera_process_qdsp_detect_error_msg (CAMQDSP_MSG_MICRO_DMA_OVERFLOW_ERROR);
      break;

    case CAMQDSP_MSG_CONFIG_COMPLETE:
      MSG_CAMERADEBUG ("CAMERA_SVCS: Received CONFIG_COMPLETE message from DSP", 0, 0, 0);
      event_report(EVENT_VFE_MSG_CONFIG_COMPLETE);
      camera_process_qdsp_config_done_msg((CAMQDSP_VideoEncStateType *) buffer);
      break;

    case CAMQDSP_MSG_ILLEGAL_COMMAND:
      MSG_CAMERADEBUG ("CAMERA_SVCS: Received ILLEGAL COMMAND dsp_command:0x%x response:%d", camera_dsp_command, camera_dsp_response, 0);
      if (camera_dsp_command == CAMQDSP_CMD_IDLE)
      {
        /* Last command was IDLE, the DSP may reject it when it is not yet
         * configured. This is OK. Treat it as if DSP in IDLE state */
        camera_process_qdsp_idle_state_msg ();
      }
#ifdef FEATURE_VIDEO_ENCODE
      else if(camera_state == CAMERA_STATE_RECORDING)
      {
        camcorder_video_engine_error_recovery(VIDEO_FAILURE);
      }
#endif /* FEATURE_VIDEO_ENCODE */
      else if( (qdsp_read(QDSP_vfeState) == 1) && (camera_dsp_command == CAMQDSP_CMD_END_OF_FRAME_ACK2) )
	  {
	    MSG_CAMERADEBUG ("CAMERA_SVCS: ACK2 Command in idle state is invalid ignoring.", 0, 0, 0);
	  }

      else
      {
        camera_process_qdsp_illegal_cmd_msg ();
      }
      break;

    case CAMQDSP_MSG_UPDATE_COMPLETE:
      if (camera_dsp_command != CAMQDSP_CMD_IDLE) {
        camera_process_qdsp_update_complete_msg ();
      }
      break;

    case CAMQDSP_MSG_AF_STATS_COMPLETE:
      if (camera_dsp_command != CAMQDSP_CMD_IDLE) {
        camera_af_process_stats_complete_msg ((CAMQDSP_AFStatsMessageType *) buffer);
      }
      break;

    case CAMQDSP_MSG_AE_AWB_STATS_COMPLETE:
      if (camera_dsp_command != CAMQDSP_CMD_IDLE) {
        camera_process_qdsp_ae_awb_stats_complete_msg ((CAMQDSP_AE_AWBStatsMessageType *) buffer);
      }
      break;

    case CAMQDSP_MSG_HIST_STATS_COMPLETE:
      if (camera_dsp_command != CAMQDSP_CMD_IDLE)
      {
        camera_process_qdsp_hist_stats_complete_msg ();
      }
      break;
#ifdef FEATURE_VIDEO_ENCODE
    case CAMQDSP_VIDEO_FRAME_DONE:
      camera_process_qdsp_video_frame_done_msg ((uint16 *) buffer);
      break;

    case CAMQDSP_VIDEO_ENCODE_SELECTION_DONE:
      camera_process_qdsp_video_encode_selection_done_msg ();
      break;

    case CAMQDSP_VIDEO_ENCODE_CONFIG_DONE:
      camera_process_qdsp_video_encode_config_done_msg ();
      break;

    case CAMQDSP_VIDEO_ENCODE_IDLE_DONE:
      camera_process_qdsp_video_encode_idle_done_msg ();
      break;

    case CAMQDSP_VIDEO_ENCODE_FEATURE_UPDATE_DONE:
      camera_process_qdsp_video_encode_feature_update_done_msg ();
      break;
#endif /* FEATURE_VIDEO_ENCODE */
    case CAMQDSP_SERVICE_UNAVAILABLE:
      /* This can occur if there is a hand off. The vocoder image will be
       * swapped out and the VFE will be temporarily disabled. If we are in
       * preview we acknowledge the reset request and wait for download done to
       * restart preview. A timer is set to make sure that we eventually
       * re-download the image to the DSP. All other cases we abort the current
       * operation.
       */
      MSG_CAMERADEBUG ("CAMERA_SVCS: Received CAMQDSP_SERVICE_UNAVAILABLE message from DSP", 0, 0, 0);
      camera_service_unavailable = TRUE;
      if (CAMERA_STATE_PREVIEW == camera_state)
      {
        /* Issue Idle command only if the last command issued was not IDLE command */
        if ( camera_dsp_command != CAMQDSP_CMD_IDLE )
        {
          MSG_CAMERADEBUG("CAMERA_SVCS: Send IDLEMessage to DSP", 0, 0, 0);
          (void) camera_send_vfe_idle_and_wait();
        }
        clk_reg (&camera_service_unavailable_clk, camera_service_unavailable_clk_cb , (int32) 500, 0, FALSE);
        MSG_CAMERADEBUG("CAMERA_SVCS: Send DSP reset ack", 0, 0, 0);
        CAMQDSP_Reset_Ack();
        MSG_CAMERADEBUG ("CAMERA_SVCS: Set camera_dsp_state=DSP_ENABLING", 0, 0, 0);
        camera_dsp_state = DSP_ENABLING;
      }
#ifdef FEATURE_VIDEO_ENCODE
      else if(camera_state == CAMERA_STATE_RECORDING)
      {
        camcorder_video_engine_error_recovery(VIDEO_FAILURE);
      }
#endif /* FEATURE_VIDEO_ENCODE */
      else
      {
        camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int32)CAMERA_ERROR_CONFIG);
      }

      break;

    case CAMQDSP_MSG_CAMIF_DONE:
      MSG_CAMERADEBUG ("CAMERA_SVCS: Received CAMIF_DONE message from DSP", 0, 0, 0);
      camera_take_picture_status.camif_done_counter++;
      camera_process_qdsp_camif_done_msg();
      MSG_CAMERADEBUG("CAMERA_SVCS: CAMIF_DONE received %d times", camera_take_picture_status.camif_done_counter, 0, 0);
      break;
#if defined  QDSP_MODULE_VFE05_DEFINED || defined QDSP_MODULE_VFE25_DEFINED
    case CAMQDSP_MSG_INVALID_CONFIG_PARAMETER:
      {
        hw_rev_type hardware_revision_number;
        hw_partnum_version(&hardware_revision_number);
        ERR_FATAL("The specified VFE configuration cannot be supported by this VFE HW. MSM ID is %4x.", hardware_revision_number.partnum, 0, 0);
      }
      break;
#endif /*QDSP_MODULE_VFE05_DEFINED*/

    default:
      break;
  }

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

} /* camera_process_qdsp_msg */

/*===========================================================================

FUNCTION      CAMERA_WAIT_CLK_CB

DESCRIPTION
              Handle Callback after a delay has completed.

RETURN VALUE
  if in graph context
    TRUE
  else
    FALSE

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 function param not referenced */
static void camera_wait_clk_cb(int4 ms)
{
  if (camera_timed_wait_tcb)
  {
    (void) rex_set_sigs (camera_timed_wait_tcb, CAMERA_TIMER_SIG);
  }
}
/*lint -restore */

#ifdef HANDLE_CAMIF_ERROR
/*===========================================================================

FUNCTION      CAMERA_CAMIF_ERROR_CLK_CB

DESCRIPTION
              Handle Callback after a delay has completed.

RETURN VALUE
  if in graph context
    TRUE
  else
    FALSE

SIDE EFFECTS
  None

===========================================================================*/
void camera_camif_error_clk_cb(int4 ms)
{
  /* Pretend that MICRO DMA OVERFLOW error had occurred */
  graph_queue_camqdsp_msg (CAMQDSP_MSG_MICRO_DMA_OVERFLOW_ERROR, (void *) 0, 0);
}
#endif /* HANDLE_CAMIF_ERROR */

/*===========================================================================

FUNCTION      CAMERA_TIMED_WAIT

DESCRIPTION
              Wait for some milli-seconds.
              Resolution is 5ms.

RETURN VALUE
  if in graph context
    TRUE
  else
    FALSE

SIDE EFFECTS
  None

===========================================================================*/
void camera_timed_wait (uint32 ms)
{
  rex_enter_crit_sect(&camera_wait_clk_cs);
  if (camera_timer_up)
  {
    uint32        save;

    /* Lock to make sure timer callback does
     * not happen until rex_wait is called */
    INTLOCK_SAV (save);

    camera_timed_wait_tcb = rex_self ();

    clk_reg (&camera_wait_clk, camera_wait_clk_cb, ms, 0, FALSE);
    (void) rex_wait (CAMERA_TIMER_SIG);
    /* We are here due to either timeout or other conditions.
     * Make sure timer is stopped. */
    clk_dereg (&camera_wait_clk);
    (void) rex_clr_sigs (camera_timed_wait_tcb, CAMERA_TIMER_SIG);

    camera_timed_wait_tcb = NULL;

    INTFREE_SAV (save);
  }
  else
  {
    clk_busy_wait (ms * 1000);
  }
  rex_leave_crit_sect(&camera_wait_clk_cs);
} /* camera_timed_wait */

#ifdef FEATURE_QVPHONE

/*===========================================================================

FUNCTION      CAMERA_ENABLE_QVP

DESCRIPTION
              Enable QVP mode. In this mode, camera service does not do DSP
              code download. Camera service also does not support video
              recording and cannot take or encode pictures.


DEPENDENCIES
  Must be in READY state.

RETURN VALUE
  camera_ret_code_type
  cb = DSP ISR callback function pointer
  sensor_type = camera sensor type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_enable_qvp
(
 void **cb, /* Output parameter */
 camera_sensor_type *sensor_type,/* Output parameter */
 boolean            use_ycbcr1, /* YCbCr type preference */
 boolean arm_kick_backend   /* Flag indicating if ARM kick backend */
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_log(LOG_FUNC, (int)CAMERA_FUNC_ENABLE_QVP, 0, 0, __LINE__);
  MSG_LOW("Camera enable QVP", 0, 0, 0);
  *cb = 0;
  *sensor_type = CAMERA_SENSOR_TYPE_NONE;
  if (camsensor_initialized == FALSE)
  {
    event_report(EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  /* In order to switch camera, we should allow this function be called
   * before ready state */
  if (camera_state != CAMERA_STATE_READY &&
      camera_state != CAMERA_STATE_INIT)
  {
    event_report(EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  /* Only Bayer Sensor Supported for 6550 */
  *cb = (void*)CAMQDSP_qvp_isr;

  *sensor_type = CAMERA_SENSOR_TYPE_YCBCR;

#ifdef FEATURE_QVPHONE
  camera_qvp_enabled = TRUE;
#endif
  /* In non-VT operations, camera_dsp_state transitions to DSP_ENABLED when
   * camqdsp download is done.
   * For VT, where camqdsp does not do the download, we force the state to
   * follow camera_qvp_enabled. */
  camera_dsp_state = DSP_ENABLED;
  /* force the preview_mode to the only mode that is valid for VT, that is,
     the MOVIE mode. This prevents other camera applications to set the
     preview_mode to an invalid oprational mode while VT is suspended.  */
  preview_mode = CAMERA_PREVIEW_MODE_MOVIE;

  (void) CAMQDSP_Register(camera_qdsp_cb);

#ifdef FEATURE_QVP_TEST_ENC_ROTATION
#error code not present
#endif


  QVP_MSG_VENC_HIGH("Enable QVP Camera @state %d, YCbCr Sensor, ARM BE Kick",
                    camera_state, use_ycbcr1, arm_kick_backend);

  return CAMERA_SUCCESS;

#endif /* T_IO_CARD */
} /* camera_svcs_enable_qvp */


/*===========================================================================

FUNCTION      CAMERA_DISABLE_QVP

DESCRIPTION
              Disable QVP mode.


DEPENDENCIES
  Must be in READY state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_disable_qvp(void)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_log(LOG_FUNC, (int)CAMERA_FUNC_DISABLE_QVP, 0, 0, __LINE__);
  MSG_LOW("Camera enable QVP", 0, 0, 0);
  if (camsensor_initialized == FALSE)
  {
    event_report(EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state != CAMERA_STATE_INIT &&
      camera_state != CAMERA_STATE_READY)
  {
    event_report(EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  CAMQDSP_Deregister();
#ifdef FEATURE_QVP_TEST_ENC_ROTATION
#error code not present
#endif
#ifdef FEATURE_QVPHONE
  camera_qvp_enabled = FALSE;
#endif
  /* In non-VT operations, camera_dsp_state transitions to DSP_ENABLED when
   * camqdsp download is done.
   * For VT, where camqdsp does not do the download, we force the state to
   * follow camera_qvp_enabled. */
  camera_dsp_state = DSP_DISABLED;

  QVP_MSG_VENC_HIGH("Dis-able QVP Camera @ state %d", camera_state, 0, 0);
  return CAMERA_SUCCESS;
#endif /* T_IO_CARD */
} /* camera_svcs_disable_qvp */


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
camera_ret_code_type camera_svcs_start_qvp_encode
(
  camera_cb_f_type             callback,
  void                        *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else
  rex_tcb_type    *tcb;
  camera_ret_code_type ret_code = CAMERA_SUCCESS;

  camera_log(LOG_FUNC, (int)CAMERA_FUNC_START_QVP_ENCODE, 0, 0, __LINE__);
  MSG_LOW("Camera start record", 0, 0, 0);
  if (camsensor_initialized == FALSE)
  {
    ret_code = CAMERA_NO_SENSOR;
    event_report(EVENT_CAMERA_NO_SENSOR);
  }
  else if (camera_state == CAMERA_STATE_INIT)
  {
    QVP_MSG_VENC_FATAL("Invalid state", 0, 0, 0);
    event_report(EVENT_CAMERA_INVALID_STATE);
    ret_code = CAMERA_INVALID_STATE;
  }
  else if (camera_preview_decimation != Q12)
  {
    QVP_MSG_VENC_FATAL("Format not supported", 0, 0, 0);
    event_report(EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
    ret_code = CAMERA_INVALID_PARM;
  }

  if (ret_code != CAMERA_SUCCESS)
  {
    camera_reject(callback, client_data, CAMERA_FUNC_START_RECORD);
    return ret_code;
  }
  camera_clr_overlays();
  camera_take_images = FALSE;

  camera_qvp_video_handshake = TRUE;

  QVP_MSG_VENC_HIGH("Start QVP Camera @ state %d", camera_state, 0, 0);

  tcb = rex_self();
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  graph_queue_camera_func((uint32)CAMERA_FUNC_START_QVP_ENCODE,
      (void *)callback, client_data, (uint32)tcb, (uint32)&ret_code, 0, 0);

  (void) rex_wait (CAMERA_SIG);
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  return ret_code;
#endif /* T_IO_CARD */
}


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
camera_ret_code_type camera_svcs_stop_qvp_encode (void)
{
  rex_tcb_type *tcb;
#ifdef T_IO_CARD
#error code not present
#else

  camera_log(LOG_FUNC, (int)CAMERA_FUNC_STOP_QVP_ENCODE, 0, 0, __LINE__);
  MSG_LOW("Camera stop record", 0, 0, 0);
  if (camsensor_initialized == FALSE)
  {
    event_report(EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state != CAMERA_STATE_QVP_ENCODING)
  {
    event_report(EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  QVP_MSG_VENC_HIGH("Stop QVP Camera @ state %d", camera_state, 0, 0);

  camera_qvp_video_handshake = FALSE;
  /* Block caller until function is executed */
  tcb = rex_self();
  (void)rex_clr_sigs(tcb, CAMERA_SIG);
  /* Block caller until function is executed */
  graph_queue_camera_func((uint32)CAMERA_FUNC_STOP_QVP_ENCODE,
                          (void *)0, 0, (uint32)tcb, 0, 0, 0);

  MSG_CAMERADEBUG("Waiting on CAMERA_SIG in camera_svcs_stop_qvp_encode", 0, 0, 0 );
  (void)rex_wait(CAMERA_SIG);
  MSG_CAMERADEBUG("Got CAMERA_SIG in camera_svcs_stop_qvp_encode", 0, 0, 0 );
  (void)rex_clr_sigs(tcb, CAMERA_SIG);

  return CAMERA_SUCCESS;
#endif /* T_IO_CARD */
}

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
void camera_svcs_qvp_reset(void)
{
  camera_log(LOG_FUNC, (int)CAMERA_FUNC_QVP_RESET, 0, 0, __LINE__);

  QVP_MSG_VENC_HIGH("Reset QVP Camera @ state %d", camera_state, 0, 0);
  //Clean up Camera state machine, so we could restart
  //camera_process_qdsp_service_unavailable_msg();
}

#endif /* FEATURE_QVPHONE */


/* ------------------------------------------------------------------ */
/* Local Function Implementations                                     */
/* ------------------------------------------------------------------ */

/*===========================================================================

FUNCTION      CAMERA_REJECT

DESCRIPTION
              Reject a function call. It is used only in pre-processing
              of the function calls.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_reject
(
 camera_cb_f_type callback,
 void            *client_data,
 camera_func_type func
)
{
  MSG_CAMERADEBUG("Camera Reject called. camera_state:%d function:%d", camera_state, func, 0);
  if (callback)
  {
    (*callback) (CAMERA_EXIT_CB_FAILED, client_data, func, 0);
  }
  event_report (EVENT_CAMERA_FUNCTION_REJECTED);
}

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
camera_ret_code_type camera_svcs_clr_overlay
(
  camera_cb_f_type callback,
  void            *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  camera_log (LOG_FUNC, (int)CAMERA_FUNC_CLR_OVERLAY, 0, 0, __LINE__);
  MSG_LOW ("Camera clear overlay", 0, 0, 0);
  if (camsensor_initialized == FALSE)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_CLR_OVERLAY);
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_CLR_OVERLAY);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
  camera_clr_overlays ();
  graph_queue_camera_func ((uint32) CAMERA_FUNC_CLR_OVERLAY,
                           (void *) callback, client_data, 0, 0, 0, 0);
  return CAMERA_SUCCESS;

#endif /* T_IO_CARD */
} /* camera_clr_overlay */


/*===========================================================================

FUNCTION      CAMERA_CALCULATE_ZOOM

DESCRIPTION
              Calculate zooming. Computes the input dimensions to the VFE
              clipping module for different zoom levels.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_calculate_zoom (void)
{
  int32 index;

  /* maximum cropping (smallest window) for zoom calculation */
  uint32 max_crop_width;
  uint32 max_crop_height;

  if ((orig_picture_dx == 0) || (orig_picture_dy == 0) ||
      (camera_preview_dx == 0) || (camera_preview_dy == 0))
  {
    return;
  }

  /* use the largest size of preview or snapshot as the maximum cropping window */
  if ( (orig_picture_dx * orig_picture_dy) >= (camera_preview_dx * camera_preview_dy) )
  {
    max_crop_width  = orig_picture_dx;
    max_crop_height = orig_picture_dy;
  }
  else
  {
    max_crop_width  = camera_preview_dx;
    max_crop_height = camera_preview_dy;
  }

  // This is the meat of the zoom algorithm. This is where we use a table
  // to determine how many times greater than the smallest crop window
  // (which provides maximum zoom) should we crop. If our crop is the
  // smallest crop permissible (low table entry), we get maximum zoom, if
  // we are high in the LUT, we are then a big multiple of min crop window,
  // i.e. big crop window, i.e. little zooming.
  /*lint -save -e713 -e737 */
  index = camera_parm_zoom_table.maximum_value -
    zoom_step_size*camera_parm_zoom.current_value -
    camera_zoom_table_bump[camera_parm_zoom.current_value];
  /*lint -restore */
  if (index < 0)
  {
    MSG_ERROR("Zoom calculation failed.", 0, 0, 0);
    event_report (EVENT_CAMERA_INVALID_STATE);
    index = 0;
  }
#ifdef FEATURE_CAMERA_ZOOM_TABLE2
  camera_decimation_factor = camera_zoom_table_ptr[index];
#else /* FEATURE_CAMERA_ZOOM_TABLE2 */
  camera_decimation_factor = camera_zoom_table[index];
#endif /* FEATURE_CAMERA_ZOOM_TABLE2 */

  /* VFE input dimensions */
  camera_vfe_input_dx = CEILING16(max_crop_width) * camera_decimation_factor / Q12;

  if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_BAYER)
  {
    /* dx must be multiple of 4. */
    camera_vfe_input_dx &= FLOOR4(camera_vfe_input_dx);
  }

  /* Clipping module input width cannot exceed sensor output width */
#ifndef QDSP_MODULE_VFE25_DEFINED
  if (camera_vfe_input_dx > camsensor_static_params[camera_asi].full_size_width)
  {
    camera_vfe_input_dx = camsensor_static_params[camera_asi].full_size_width;
  }
#else
#error code not present
#endif /* ! QDSP_MODULE_VFE25_DEFINED */

  /* Recompute camera_decimation factor */
  camera_decimation_factor = camera_vfe_input_dx * Q12 / CEILING16(max_crop_width);

  camera_vfe_input_dy = CEILING16(max_crop_height) * camera_decimation_factor / Q12;

  /* Make sure dy is multiple of 16 */
  camera_vfe_input_dy = FLOOR16(camera_vfe_input_dy);

  /* Clipping module input height cannot exceed sensor output height */
 #ifndef QDSP_MODULE_VFE25_DEFINED
  if (camera_vfe_input_dy > camsensor_static_params[camera_asi].full_size_height)
  {
    camera_vfe_input_dy = camsensor_static_params[camera_asi].full_size_height;
    camera_decimation_factor = camera_vfe_input_dy * Q12 / CEILING16(max_crop_height);

    camera_vfe_input_dx = CEILING16(max_crop_width) * camera_decimation_factor / Q12;

    if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_BAYER)
    {
      /* Make sure dx is multiple of 16 */
      camera_vfe_input_dx = FLOOR16(camera_vfe_input_dx);
    }
  }
#else
#error code not present
#endif /* ! QDSP_MODULE_VFE25_DEFINED */
} /* camera_calculate_zoom */


/*===========================================================================

FUNCTION      CAMERA_CALCULATE_VFE_CLIPPING

DESCRIPTION
              Computes VFE clipping parameters corresponding to
              the current zooming level.

              First Pixel: For Bayer data, ensure that there are atleast
              6 pixels to the left of this position. The first pixel must
              be green for Bayer data. The width of the line must be a
              multiple of 4. For YCbCr data, please set an even value.

              Last Pixel: For Bayer data, ensure that there are atleast
              6 pixels to the right of this position. The width of the line
              must be a multiple of 4. For YCbCr data, please set an odd
              value.

              First Line: For Bayer data, ensure that there are at least
              3 lines above this line; the number of lines must be a multiple
              of 4

              Last Line: For Bayer data, ensure that there are at least 3
              lines below this line; the number of lines must be a multiple
              of 4


DEPENDENCIES
  Must only be called after the camsensor_config() or during zoom set.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_calculate_vfe_clipping (void)
{
  uint32 vfe_clip_width, vfe_clip_height;
  uint32 window_width_gap, window_height_gap;
  uint8 start_pixel_gap = 0, start_line_gap = 0;
  uint8 end_pixel_gap = 0, end_line_gap = 0;
  uint8 window_multiple_factor = 1;
  uint32 vfe_output_width;
  uint32 vfe_output_height;


  switch (camsensor_static_params[camera_asi].output_format)
  {
    case CAMSENSOR_YCBCR:
      start_pixel_gap = 0;
      start_line_gap = 2;
      end_pixel_gap = 0;
      end_line_gap = 0;
      window_multiple_factor = 8;
      break;
    case CAMSENSOR_BAYER:
  #ifndef QDSP_MODULE_VFE25_DEFINED
      start_pixel_gap = 6;
      start_line_gap = (uint8) (2+ 3 + camsensor_static_params[camera_asi].blackrows);
      end_pixel_gap = 6;
      end_line_gap = 3;
      window_multiple_factor = 8;
  #else
#error code not present
  #endif /*QDSP_MODULE_VFE25_DEFINED */
      break;
  }
  switch (camera_camsensor_op_mode)
  {
    uint32 subsample;
    case CAMSENSOR_OP_MODE_PREVIEW:
    case CAMSENSOR_OP_MODE_VIDEO:
      /* Sanity check for possible divide-by-0 */
      if ( (camsensor_static_params[camera_asi].preview_dx_decimation == 0) ||
           (vfeVideoConfig.inputConfiguration.width == 0) ||
           (camsensor_static_params[camera_asi].preview_dy_decimation == 0) ||
           (vfeVideoConfig.inputConfiguration.height == 0)
         )
      {
        MSG_HIGH ("Return on Divide by Zero, camera_camsensor_op_mode = %d", camera_camsensor_op_mode, 0, 0);
        return;
      }
      window_width_gap  = ((camsensor_static_params[camera_asi].full_size_width - camera_vfe_input_dx) * Q12 / camsensor_static_params[camera_asi].preview_dx_decimation);

      /* Check if there is any sub-sampling done in the VFE */
      subsample = (camsensor_static_params[camera_asi].camsensor_width / vfeVideoConfig.inputConfiguration.width);
      if (subsample)
      {
        window_width_gap /= subsample;
      }

      /* window width gap must be atleast 12 pixels for Bayer.
       * VFE requires (6 + 6) pixel boundary horizontally
       */
      if (window_width_gap < (start_pixel_gap + end_pixel_gap))
      {
        window_width_gap = (start_pixel_gap + end_pixel_gap);
      }

      window_height_gap  = ((camsensor_static_params[camera_asi].full_size_height - camera_vfe_input_dy) * Q12 / camsensor_static_params[camera_asi].preview_dy_decimation);

      /* Check if there is any sub-sampling done in the VFE */
      window_height_gap /= (camsensor_static_params[camera_asi].camsensor_height / vfeVideoConfig.inputConfiguration.height);

      /* window height gap must be atleast 8 pixels for Bayer.
       * VFE requires (3 + 3) pixel boundary vertically. It is
       * observed that Update command is stable with (5 + 3) pixel boundary
       */
      if (window_height_gap < (start_line_gap + end_line_gap))
      {
        window_height_gap = (start_line_gap + end_line_gap);
      }

      /* Check the boundary conditions for both output conditions and adjust
       * window_width_gap accordingly.
       */
      if (vfeVideoConfig.output1Configuration.width <
          vfeVideoConfig.output2Configuration.width)
      {
        vfe_output_width = vfeVideoConfig.output2Configuration.width;
      }
      else
      {
        vfe_output_width = vfeVideoConfig.output1Configuration.width;
      }

      if (((vfeVideoConfig.inputConfiguration.width - window_width_gap) -
            ((vfeVideoConfig.inputConfiguration.width - window_width_gap) %
             window_multiple_factor)) < vfe_output_width)
      {
        /* Set the window width to max */
        window_width_gap = (vfeVideoConfig.inputConfiguration.width -
            vfe_output_width) - ((vfeVideoConfig.inputConfiguration.width -
                vfe_output_width) % window_multiple_factor);

        if (window_width_gap < (start_pixel_gap + end_pixel_gap))
        {
          /* It is not possible to have this window gap */
          MSG_HIGH ("window_width_gap = %d is not possible, camera_camsensor_op_mode = %d", window_width_gap, camera_camsensor_op_mode, 0);
          return;
        }
      }

      /* Check the boundary conditions for both output conditions and adjust
       * window_height_gap accordingly.
       */
      if (vfeVideoConfig.output1Configuration.height <
          vfeVideoConfig.output2Configuration.height)
      {
        vfe_output_height = vfeVideoConfig.output2Configuration.height;
      }
      else
      {
        vfe_output_height = vfeVideoConfig.output1Configuration.height;
      }

      if (((vfeVideoConfig.inputConfiguration.height - window_height_gap) -
            ((vfeVideoConfig.inputConfiguration.height - window_height_gap) %
             window_multiple_factor)) < vfe_output_height)
      {
        /* Set the window_height gap to max */
        window_height_gap = (vfeVideoConfig.inputConfiguration.height -
            vfe_output_height) - ((vfeVideoConfig.inputConfiguration.height -
                vfe_output_height) % window_multiple_factor);

        if (window_height_gap < (start_line_gap + end_line_gap))
        {
          /* It is not possible to have this window gap */
          MSG_HIGH ("window_height_gap = %d is not possible, camera_camsensor_op_mode = %d", window_height_gap, camera_camsensor_op_mode, 0);
          return;
        }
      }

      /* The above logic must ensure that window_width_gap
       * is atleast 12 pixels
       */
      window_width_gap -= (start_pixel_gap + end_pixel_gap);
      window_width_gap /= 2;
      window_width_gap = FLOOR2(window_width_gap);

      /* The above logic must ensure that window_height_gap
       * is atleast 8 pixels
       */
      window_height_gap -= (start_line_gap + end_line_gap);
      window_height_gap /= 2;
      window_height_gap = FLOOR2(window_height_gap);

      vfeVideoConfig.clip.firstPixel = start_pixel_gap + window_width_gap;
      vfeVideoConfig.clip.lastPixel = vfeVideoConfig.inputConfiguration.width - 1 - end_pixel_gap - window_width_gap;

      /* For Bayer data, number of pixels in a line must be multiple of 4 */
      vfeVideoConfig.clip.lastPixel -= (vfeVideoConfig.clip.lastPixel - vfeVideoConfig.clip.firstPixel + 1) %  window_multiple_factor;

      vfeVideoConfig.clip.firstLine = start_line_gap + window_height_gap;
      vfeVideoConfig.clip.lastLine = vfeVideoConfig.inputConfiguration.height - 1 - end_line_gap - window_height_gap;

      /* For Bayer data, number of lines must be multiple of 4 */
      vfeVideoConfig.clip.lastLine -= (vfeVideoConfig.clip.lastLine - vfeVideoConfig.clip.firstLine + 1) % window_multiple_factor;

      /*Width & height of the vfe clipping window*/
      vfe_clip_width = (vfeVideoConfig.clip.lastPixel -
        vfeVideoConfig.clip.firstPixel);
      vfe_clip_height = (vfeVideoConfig.clip.lastLine -
        vfeVideoConfig.clip.firstLine);

       #if defined  QDSP_MODULE_VFE25_DEFINED && !defined FEATURE_CAMERA_YCBCR_ONLY
       if (camsensor_static_params[camera_asi].output_format!= CAMSENSOR_YCBCR) /* No ASF for YUV sensors */
       {
         preview_downscale_factor = (float)((float)(vfe_clip_width+1)/(float)(vfeVideoConfig.output1Configuration.width));
         /* Update the ASF for the downscale factor */
         vfe_update_5x5_asf(&(camsensor_static_params[camera_asi]),VFE_PREVIEW);
       }
      #endif /* QDSP_MODULE_VFE25_DEFINED */

      /* Recalculating the AF window, for the new clipping window parameters */
      vfe_calculate_af_window( vfe_clip_width, vfe_clip_height);

      break;

    case CAMSENSOR_OP_MODE_SNAPSHOT:

      /* Sanity check for possible divide-by-0 */
      if ( (vfeSnapshotConfig.inputConfiguration.width == 0) ||
           (vfeSnapshotConfig.inputConfiguration.height == 0)
         )
      {
        MSG_HIGH ("Return on Divide by Zero, camera_camsensor_op_mode = %d", camera_camsensor_op_mode, 0, 0);
        return;
      }
      #ifndef QDSP_MODULE_VFE25_DEFINED
      window_width_gap  = (camsensor_static_params[camera_asi].full_size_width - camera_vfe_input_dx);
      #else
#error code not present
      #endif /*QDSP_MODULE_VFE25_DEFINED */
      
      

      /* Check if there is any sub-sampling done in the VFE */
      window_width_gap /= (camsensor_static_params[camera_asi].full_size_width / vfeSnapshotConfig.inputConfiguration.width);

      /* window width gap must be atleast 12 pixels for Bayer.
       * VFE requires (6 + 6) pixel boundary horizontally
       */
      if (window_width_gap < (start_pixel_gap + end_pixel_gap))
      {
        window_width_gap = (start_pixel_gap + end_pixel_gap);
      }

      #ifndef QDSP_MODULE_VFE25_DEFINED
      window_height_gap  = (camsensor_static_params[camera_asi].full_size_height - camera_vfe_input_dy);
      #else
#error code not present
      #endif /* QDSP_MODULE_VFE25_DEFINED */

      /* Check if there is any sub-sampling done in the VFE */
      window_height_gap /= (camsensor_static_params[camera_asi].full_size_height / vfeSnapshotConfig.inputConfiguration.height);

      /* window height gap must be atleast 8 pixels for Bayer.
       * VFE requires (3 + 3) pixel boundary vertically. It is
       * observed that Update command is stable with (5 + 3) pixel boundary
       */
      if (window_height_gap < (start_line_gap + end_line_gap))
      {
        window_height_gap = (start_line_gap + end_line_gap);
      }

      if (((vfeSnapshotConfig.inputConfiguration.width - window_width_gap) - ((vfeSnapshotConfig.inputConfiguration.width - window_width_gap) % window_multiple_factor)) < vfeSnapshotConfig.output2Configuration.width)
      {
        /* Set the window width to max */
        window_width_gap = (vfeSnapshotConfig.inputConfiguration.width - vfeSnapshotConfig.output2Configuration.width) - ((vfeSnapshotConfig.inputConfiguration.width - vfeSnapshotConfig.output2Configuration.width) % window_multiple_factor);

        if (window_width_gap < (start_pixel_gap + end_pixel_gap))
        {
          /* It is not possible to have this window gap */
          MSG_HIGH ("window_width_gap = %d not possible, camera_camsensor_op_mode = %d", window_width_gap, camera_camsensor_op_mode, 0);
          return;
        }
      }

      if (((vfeSnapshotConfig.inputConfiguration.height - window_height_gap) - ((vfeSnapshotConfig.inputConfiguration.height - window_height_gap)) % window_multiple_factor) < vfeSnapshotConfig.output2Configuration.height)
      {
        /* Set the window_height gap to max */
        window_height_gap = (vfeSnapshotConfig.inputConfiguration.height - vfeSnapshotConfig.output2Configuration.height) - ((vfeSnapshotConfig.inputConfiguration.height - vfeSnapshotConfig.output2Configuration.height) % window_multiple_factor);

        if (window_height_gap < (start_line_gap + end_line_gap))
        {
          /* It is not possible to have this window gap */
          MSG_HIGH ("window_height_gap = %d not possible, camera_camsensor_op_mode = %d", window_height_gap, camera_camsensor_op_mode, 0);
          return;
        }
      }

      /* The above logic must ensure that window_width_gap
       * is atleast 12 pixels for bayer.
       */
      window_width_gap -= (start_pixel_gap + end_pixel_gap);
      window_width_gap /= 2;
      window_width_gap = FLOOR2(window_width_gap);

      /* The above logic must ensure that window_height_gap
       * is atleast 8 pixels for bayer
       */
      window_height_gap -= (start_line_gap + end_line_gap);
      window_height_gap /= 2;
      window_height_gap = FLOOR2(window_height_gap);

      vfeSnapshotConfig.clip.firstPixel = start_pixel_gap + window_width_gap;

      #ifndef QDSP_MODULE_VFE25_DEFINED
      vfeSnapshotConfig.clip.lastPixel = vfeSnapshotConfig.inputConfiguration.width - 1 - end_pixel_gap - window_width_gap;
      #else
#error code not present
      #endif
      
      /* For Bayer data, number of pixels in a line must be multiple of 4 */
      vfeSnapshotConfig.clip.lastPixel -= (vfeSnapshotConfig.clip.lastPixel - vfeSnapshotConfig.clip.firstPixel + 1) % window_multiple_factor;

      vfeSnapshotConfig.clip.firstLine = start_line_gap + window_height_gap;

      #ifndef QDSP_MODULE_VFE25_DEFINED
      vfeSnapshotConfig.clip.lastLine =  vfeSnapshotConfig.inputConfiguration.height - 1 - end_line_gap - window_height_gap;
      #else
#error code not present
      #endif
      /* For Bayer data, number of lines must be multiple of 4 */
       vfeSnapshotConfig.clip.lastLine -= (vfeSnapshotConfig.clip.lastLine - vfeSnapshotConfig.clip.firstLine + 1) % window_multiple_factor;
       #if defined  QDSP_MODULE_VFE25_DEFINED && !defined FEATURE_CAMERA_YCBCR_ONLY
       if (camsensor_static_params[camera_asi].output_format!= CAMSENSOR_YCBCR) /* No ASF for YUV sensors */
       {
         snapshot_downscale_factor = (float)((float)(vfeSnapshotConfig.clip.lastPixel - vfeSnapshotConfig.clip.firstPixel + 1)/\
         (float)(vfeSnapshotConfig.output2Configuration.width));
         /* Update the ASF for the downscale factor */
         vfe_update_5x5_asf(&(camsensor_static_params[camera_asi]),VFE_SNAPSHOT);
       }
       #endif /* QDSP_MODULE_VFE25_DEFINED */

      
      break;

    default:
      break;
  }
  return;
} /* camera_calculate_vfe_clipping */

/*===========================================================================

FUNCTION      CAMERA_CALCULATE_AND_SET_CAPTURE_DELAY

DESCRIPTION
              This is for calculating the time need to wait before capture,
              the user will specify delay N number of frames and this delay
              will be translated to delay in milliseconds. This delay is
              applied before VFE_Capture()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_calculate_and_set_capture_delay (uint16 delay_n_frames)
{
  uint16 preview_fps;
  uint16 snapshot_fps;
  float delay_tolerance = 0.2;
  camera_capture_delay = 0;

  preview_fps = camctrl_tbl.camsensor_params_ptr->preview_fps;
  if (camctrl_tbl.get_snapshot_frame_per_sec)
  {
    snapshot_fps = camctrl_tbl.get_snapshot_frame_per_sec(preview_fps);
  }
  else
  {
    /* Assume snapshot fps as half of the preview fps when snapshot fps is not available */
    snapshot_fps = preview_fps / 2;
    delay_tolerance = 0.6; /* this value is derived from testing with Sharp 2M sensor, assumed the worst */
  }
/*lint -save -e790 integral to float */
  camera_capture_delay = (uint32) CAMCEIL((delay_n_frames * 1000 * Q8 * (1.0 + delay_tolerance)) , snapshot_fps);
/*lint -restore */
}

/*===========================================================================

FUNCTION      CAMERA_CALCULATE_AHB_BURST_FACTORS

DESCRIPTION
              Computes the burst factors.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_calculate_ahb_burst_factors (uint32 *burst_factor1, uint32 *burst_factor2, uint32 data_len)
{
  *burst_factor2 = 256;

  while ((*burst_factor2) > 1)
  {
    if ((data_len % (*burst_factor2)) == 0)
    {
      break;
    }
    *burst_factor2 -= 1;
  }

  *burst_factor1 = data_len / (*burst_factor2);
} /* camera_calculate_ahb_burst_factors */


/*===========================================================================

FUNCTION      CAMERA_CALCULATE_CEILING_NUMBER

DESCRIPTION
              Computes the ceiling number.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static uint32 camera_calculate_ceiling_number (uint32 numerator, uint32 denominator)
{
  if (numerator < denominator)
  {
    return denominator;
  }
  if (numerator % denominator)
  {
    return (numerator + (denominator - (numerator % denominator)));
  }
  else
  {
    return numerator;
  }
} /* camera_calculate_ceiling_number */


/*===========================================================================

FUNCTION      CAMERA_SVCS_SET_ENCODE_OVERLAY

DESCRIPTION
              Set the encode overlay.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static camera_ret_code_type camera_svcs_set_encode_overlay(void)
{

  ipl_status_type   status;
  camera_ret_code_type ret_val;

  if (ui_overlay.imgPtr == NULL)
  {
    /* No camera overlay, clear all overlays */
    camera_clr_overlays ();
    return CAMERA_SUCCESS;
  }

  camera_svcs_clr_encode_overlay();
  camera_svcs_clr_thumbnail_overlay();

  if ( ui_overlay.dx < ui_overlay.dy )
  {
    /* overlay frame is in portrait mode, we need to rotate first. */
    ipl_image_type work_buffer;
    uint32 size = ui_overlay.dx* ui_overlay.dy * 2;

    work_buffer.dx = ui_overlay.dy;
    work_buffer.dy = ui_overlay.dx;
    work_buffer.cFormat = ui_overlay.cFormat;
    work_buffer.clrPtr = NULL;
    work_buffer.imgPtr = CAM_Malloc (size | CAM_ALLOC_NO_ZMEM);
    if ( work_buffer.imgPtr == NULL)
    {
      camera_clr_overlays ();
      return CAMERA_NO_MEMORY;
    }
    status = ipl_rotate90_frame (&ui_overlay, &work_buffer, IPL_ROT270, camera_transparent_color);
    if (status != IPL_SUCCESS)
    {
      CAM_Free(work_buffer.imgPtr);
      camera_clr_overlays ();
      return CAMERA_FAILED;
    }
    ret_val = camera_svcs_create_encode_overlay(&work_buffer);
    if (ret_val == CAMERA_SUCCESS)
    {
      ret_val = camera_svcs_create_thumbnail_overlay(&work_buffer);
    }
    CAM_Free(work_buffer.imgPtr);
  }
  else
  {
    ret_val = camera_svcs_create_encode_overlay(&ui_overlay);
    if (ret_val == CAMERA_SUCCESS)
    {
      ret_val = camera_svcs_create_thumbnail_overlay(&ui_overlay);
    }
  }
  if (ret_val != CAMERA_SUCCESS)
  {
    return ret_val;
  }

  if (camera_parm_preview_rotation.current_value == (int)CAMERA_SP_REVERSE)
  {
    /* This will happen when the sensor orientation changes *
     * Need to rotate the encode_overlay frame by 180 degrees */
    camera_rotate_encode_overlay();
  }
  return ret_val;
} /* camera_svcs_set_encode_overlay */

/* ---------------------------------------------------------------- */
/* Local functions which are executed in the graph task's context.  */
/* ---------------------------------------------------------------- */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_START

DESCRIPTION
              Set the LCD display window where application can render on.
              Other parts of LCD may be owned by others.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_start
(
  camera_cb_f_type callback,
  void            *client_data,
  rex_tcb_type    *tcb,
  camera_ret_code_type *status
)
{
#ifndef T_IO_CARD

  uint32 number_of_fragments;

  camsensor_static_params_type *camsensor_params = &(camsensor_static_params[camera_asi]);
  
#ifdef T_FFA
#error code not present
#endif /* T_FFA */

  /* Need to power on sensor, initialize it, power it up, before continue */
  camsensor_power_on();
  camsensor_power_up();

#ifdef FEATURE_VIDEO_ENCODE
  camcorder_process_set_default_global_parms();
#endif //FEATURE_VIDEO_ENCODE

  if (camsensor_initialized == FALSE)
  {
    if (callback)
    {
      (*callback) (CAMERA_EXIT_CB_FAILED, client_data, CAMERA_FUNC_START, 0);
    }
    event_report (EVENT_CAMERA_NO_SENSOR);
    MSG_ERROR("CAMERA_SVCS: Invalid state for camsensor_initialized %d", camsensor_initialized, 0, 0);
    *status = CAMERA_NO_SENSOR;
    if (tcb)
    {
      (void) rex_set_sigs (tcb, CAMERA_SIG);
    }
    return;
  }

#ifdef CAMERA_MALLOC_LOGGING
  if (camera_check_memory_leaks() == TRUE)
  {
    camera_3a_mem_deallocate();
    camera_clr_take_picture ();
    camera_clr_overlays ();
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
    camera_clr_icons();
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
  }
#endif /* CAMERA_MALLOC_LOGGING */

  /* Make sure that the clock is deregistered before we start */
  clk_dereg(&camera_wait_camif_done_clk);

  /* Set config params for VFE for this specific sensor.  Place
     these in global structure */
  (void) camsensor_start( camsensor_params );

  camera_parm_fps.maximum_value = camsensor_static_params[camera_asi].max_video_fps >> 8;
  camera_parm_fps.default_value = camsensor_static_params[camera_asi].video_fps >> 8;

  /* Forcefully clear or invert PCLK */
  {
    boolean invert = camsensor_static_params[camera_asi].pclk_invert;

    CAMERA_CLK_REGIME_SEL_CLK_INV (CLK_RGM_INV_SEL_VIDEO_VFE, invert);
    CAMERA_CLK_REGIME_SEL_CLK_INV (CLK_RGM_INV_SEL_MDDI_CLIENT_VFE, invert);
#ifdef CAMERA_USES_CAMIF_PAD
    CAMERA_CLK_REGIME_SEL_CLK_INV (CLK_RGM_INV_SEL_CAMIF_PCLK, invert);
#endif /* CAMERA_USES_CAMIF_PAD */

  }

  /* Neet to get vfe service started */
  /* color matrix converting should be happened before 3a is configured*/
  if (vfe_start(&camsensor_static_params[camera_asi]) != CAMERA_SUCCESS)
  {
    MSG_ERROR("CAMERA_SVCS: vfe_start failed, %d", camera_state, 0, 0);
    *status = CAMERA_FAILED;
    if (tcb)
    {
      (void) rex_set_sigs (tcb, CAMERA_SIG);
    }
    return;

  }

  camera_update_vfe = TRUE;
  camera_vfe_control_and_status_set_default();
  /* Initializes global and locally used structures for AEC/AWB */
  camera_3a_start( camsensor_params );
  /* AF is independent of 3a_config */
  camera_af_start( camsensor_params, (camera_af_mode_type)(camera_parm_af_mode.current_value) );

  if (camctrl_tbl.af_is_supported)
  {
    /* Fill out focus values for manual focus - that we did not have
       at INIT time */
    camera_parm_manual_focus.minimum_value = 0;

    /* Num steps to go across whole range */
    camera_parm_manual_focus.maximum_value = camsensor_params->num_steps_near_to_far;

    /* Default position at optimum general focus: nearest infinity
     point.  This is in terms of number of steps from near side.
     This, with the number above, defines the search space. */
    camera_parm_manual_focus.default_value = \
        camsensor_params->num_steps_near_to_closest_infinity;
    camera_parm_manual_focus.current_value = camera_parm_manual_focus.default_value;

    /* Num steps for fine second phase sweep search -also apply to manual step value */
    camera_parm_manual_focus.step_value = camsensor_params->num_fine_steps_between_stat_points;
  }

  for (number_of_fragments = 0; number_of_fragments < VFE_MAX_BUFFER_FRAGMENTS; number_of_fragments++)
  {
    ackOutput2.eofAck2[number_of_fragments].Y_Address    = NULL;
    ackOutput2.eofAck2[number_of_fragments].CbCr_Address = NULL;
  }

#ifdef CAMERA_THUMBNAIL_SIZE_QCIF
  thumbnail_width = 176;
  ui_thumbnail_width = 176;
  thumbnail_height = 144;
  ui_thumbnail_height = 144;
#else
  thumbnail_width = 320;
  ui_thumbnail_width = 320;
  thumbnail_height = 240;
  ui_thumbnail_height = 240;
#endif
  thumbnail_quality = 90;

  camera_dx            = 0;
  camera_dy            = 0;

  if ((camera_default_preview_rotation == 0) || (camera_default_preview_rotation == 180))
  {
    camera_default_orientation = CAMERA_ORIENTATION_LANDSCAPE;
    camera_odd_rotation        = FALSE;
  }
  else
  {
    camera_default_orientation = CAMERA_ORIENTATION_PORTRAIT;
    camera_odd_rotation        = TRUE;
  }


  camera_parm_fps.current_value             = camsensor_static_params[camera_asi].video_fps >> 8;

#ifdef ANY_FLASH
  camera_parm_flash.current_value = (int)CAMERA_FLASH_OFF;
  camera_flash_on = FALSE;
  camera_flash_auto = FALSE;
  camera_fire_flash = FALSE;
  camera_flash_fired = FALSE;
#endif /* ANY_FLASH */

  camera_parm_reflect.current_value        = (int)CAMERA_NO_REFLECT;
  camera_reflect_ipl                       = IPL_NOREF;
  camera_reflect                           = QCAMRAW_NO_REFLECT;
#ifdef FEATURE_MDP
  mdpImg.bmy_addr = NULL;
  mdpImg.cbcr_addr = NULL;
#endif

#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
  preview_mode         = CAMERA_PREVIEW_MODE_MOVIE;
#else /* defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE */
  preview_mode         = CAMERA_PREVIEW_MODE_SNAPSHOT;
#endif /* defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE */

  camera_parm_preview_mode.current_value      = (int)preview_mode;
  camera_parm_antibanding.current_value       = (int)CAMERA_ANTIBANDING_OFF;
  /* Turn it off in CAM3a */
  if( camsensor_static_params[camera_asi].output_format == CAMSENSOR_BAYER )
  {
    (void) camera_aec_set_antibanding ((camera_antibanding_type)camera_parm_antibanding.current_value, camsensor_params);
  }
#ifdef FEATURE_STROBE_FLASH
  camera_parm_red_eye_reduction.current_value = FALSE;
#endif /*FEATURE_STROBE_FLASH*/
  scale_preview_overlay = FALSE; /* do not resize overlay frame by default */

#ifdef FEATURE_QVPHONE
  camera_qvp_enabled = FALSE;
#endif /* FEATURE_QVPHONE */

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
  camera_parm_iso.current_value = camera_parm_iso.default_value;
  if( camsensor_static_params[camera_asi].output_format == CAMSENSOR_BAYER )
  {
    /* we need to reset ISO to the not best shot values in
     * case we are out of sync with 3a
     */
    (void) camera_aec_set_parm(CAMERA_PARM_ISO, camera_parm_iso.current_value);
  }

#ifndef FEATURE_CAMERA_YCBCR_ONLY
  /* Follows the set to default of the special effects array */
  cam3a_awb_state.color_conversion_state = DEFAULT_COLOR_CONVERSION;
  if( camsensor_static_params[camera_asi].output_format == CAMSENSOR_BAYER )
  {
    /* Restore back to the default Luma target */
    camera_aec_set_exposure_compensation((uint8) cam3a_aec_state.default_luma_target);
  }
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

  /* Set these back to default */
  camera_parm_exposure_compensation.current_value = camera_parm_exposure_compensation.default_value;
  camera_parm_brightness.current_value = camera_parm_brightness.default_value;

  camera_parm_contrast.current_value = camera_parm_contrast.default_value;

  /* Set these back to default */
  camera_parm_special_effects.current_value = camera_parm_special_effects.default_value;
  camera_parm_white_balance.current_value = camera_parm_white_balance.default_value;
  camera_parm_auto_exposure_mode.current_value = camera_parm_auto_exposure_mode.default_value;

  {
    /* Min == max FPS here initially. */
    /* Denominator in the MS 16 bits; numerator time denominator in the LS 16 */
    camera_parm_preview_fps.minimum_value = ((CAMERA_FPS_DENOMINATOR << 16) |
       (0xFFFF & (CAMERA_FPS_DENOMINATOR*(camsensor_static_params[camera_asi].max_preview_fps>>8))));

    /* Max FPS supported from this sensor */
    camera_parm_preview_fps.maximum_value = camera_parm_preview_fps.minimum_value;
    camera_parm_preview_fps.default_value = camera_parm_preview_fps.minimum_value;
    camera_parm_preview_fps.current_value = camera_parm_preview_fps.default_value;

  }
  camera_dsp_error_status_set_default();
  camera_take_picture_status_set_default();
  camera_capture_delay = 0;

  camera_service_unavailable = FALSE;
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  camera_lcd_direct_mode = 0;
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */

  /* One exception here. We need to set READY state in client context.
   * Client may call other functions right away, before the function is
   * executed in the service context. */
  MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_READY", 0, 0, 0);
  camera_state = CAMERA_STATE_READY;

  camera_func     = CAMERA_FUNC_START;
  camera_dsp_error_status.received_camif_error = FALSE;
  MSG_CAMERADEBUG("CAMERA_SVCS: Request camera_stopping=NO_STOPPING", 0, 0, 0);
  camera_stopping = FALSE;
  /* Configure the gpios and clk regimes */
  MSG_CAMERADEBUG("CAMERA_SVCS: Configure GPIO settings", 0, 0, 0);
  camsensor_config_sensor_gpios_clks ();

  exif_init_info(&camera_exif_info);

  /* Set Exif Tags Make and Model */
  camera_set_exif_make_model();

  *status = CAMERA_SUCCESS;

  if (callback)
    (callback) (CAMERA_EXIT_CB_DONE, client_data, camera_func, 0);

  MSG_HIGH("CAMERA_SVCS: Start. State transition to ready", 0, 0, 0);

  event_report (EVENT_CAMERA_START);
  if (tcb)
  {
    (void) rex_set_sigs (tcb, CAMERA_SIG);
  }
#endif /* nT_IO_CARD */
} /* camera_process_start */

/*===========================================================================

FUNCTION      CAMERA_ABORT_TAKEPICTURE_OPERATION

DESCRIPTION
              This function will decide whether camera service layer need to
              abort take picture or encode operation.

DEPENDENCIES
              Reason needs to send to this function as to why the abort
              TRUE  if STOPPING the application.
              FALSE if SERVICE UNAVAILABLE.

RETURN VALUE
              TRUE if aborting.
              FALSE if not.

SIDE EFFECTS
  None

===========================================================================*/
static boolean camera_abort_takepicture_encoding_operation(void)
{
  boolean ret_val = FALSE;

  if ( ( camera_state == CAMERA_STATE_ENCODE_PICTURE ) ||
       ( camera_state == CAMERA_STATE_TAKE_PICTURE )   ||
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
       ( (camera_take_picture_status.lpm_encoding_in_progress) ||
         (camera_take_picture_status.lpm_encoding_start)
       )
     )
  {
    if (camera_state == CAMERA_STATE_ENCODE_PICTURE && camera_take_picture_status.encode_in_progress)
    {
      camera_take_picture_status.abort = TRUE;
      switch (camera_encode_properties.format)
      {
#ifdef FEATURE_PNG_ENCODER
        case CAMERA_PNG:
          MSG_CAMERADEBUG("CAMERA_SVCS: PNG ENCODING IN PROGRESS SO ABORT", 0, 0, 0);
          pnge_encode_abort();
          ret_val = TRUE;
          break;
#endif /* FEATURE_PNG_ENCODER */
        case CAMERA_JPEG:
          MSG_CAMERADEBUG("CAMERA_SVCS: JPEG ENCODING IN PROGRESS SO ABORT", 0, 0, 0);
          /* Note that the JPEG process may have already completed at this point */
          jpeg_encoder_abort( (uint16) encodeInfo.ClientId );
          ret_val = TRUE;
          break;
        case CAMERA_RAW:
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/
        default:
          /* RAW case nothing to abort. */
          MSG_CAMERADEBUG("CAMERA_SVCS: RAW ENCODING IN PROGRESS NOTHING TO ABORT", 0, 0, 0);
          camera_take_picture_status.abort = FALSE;
      }
    }
    else if (camera_state == CAMERA_STATE_TAKE_PICTURE)
    {
#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
      if (camera_take_picture_status.ycbcr_downsizer_in_progress)
      {
        if (ycbcr_downsizer_abort() ==   YCBCR_DOWNSIZER_CMD_ACCEPT_RET)
        {
          camera_take_picture_status.abort = TRUE;
          MSG_CAMERADEBUG("CAMERA_SVCS: YCBCR_DOWNSIZER IN PROGRESS SO ABORT", 0, 0, 0);
          ret_val = TRUE;
        }
      }
#endif /*QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED */
    }
#ifdef FEATURE_CAMERA_LPM
#error code not present
#endif
  }

  return ret_val;

}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_STOP

DESCRIPTION
              The camera operation is stopped and the sensor enters power
              saving mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_stop
(
  camera_cb_f_type callback,
  void            *client_data,
  rex_tcb_type    *tcb
)
{
#ifndef T_IO_CARD
  camera_func     = CAMERA_FUNC_STOP;
  camera_app_cb   = callback;
  camera_app_data = client_data;
  camera_tcb      = tcb;
  MSG_HIGH ("Stop. camera_process_stop camera_state:%d camera_stopping:%d", camera_state, camera_stopping, 0);
  event_report (EVENT_CAMERA_STOP);
  camera_terminate (CAMERA_EXIT_CB_DONE, 0);
#endif /* nT_IO_CARD */
} /* camera_process_stop */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_CLEANUP

DESCRIPTION
              The camera operation is stopped, the snapshot buffers are cleared
              and sensor is powered down.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_cleanup
(
  rex_tcb_type    *tcb
)
{
#ifndef T_IO_CARD
  camera_svcs_optimize_resources();

#ifdef FEATURE_MDP
  /* De-register the video */
  camera_deregister_video (TRUE);
  /* De-register the overlay layer */
  camera_deregister_layer1();
  CAM_Free (picture_preview_copy);
  picture_preview_copy = NULL;
#endif /* FEATURE_MDP */

#ifdef FEATURE_STROBE_FLASH
  strobe_flash_stop_charging();
#endif /* FEATURE_STROBE_FLASH */

  /* If AF is active, then stop it */
  if (camera_af_is_active())
  {
    camera_af_stop_focus();
  }

  if (camsensor_initialized == TRUE)
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: Call power down sequence", 0, 0, 0);
    camsensor_power_down ();
    camsensor_power_off ();

    /* default the gpios and disable clk
     * regimes */
    MSG_CAMERADEBUG("CAMERA_SVCS: Default GPIO settings", 0, 0, 0);
    camsensor_unconfig_sensor_gpios_clks ();
  }

  /* Deallocate 3A memory  */
  camera_3a_mem_deallocate();

  exif_terminate(&camera_exif_info);

  camera_clr_take_picture ();

  /* Disable the modules in Update Command */

#ifdef QDSP_MODULE_VFE25_DEFINED 
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
  camera_clr_overlays ();
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  camera_clr_icons();
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

  MSG_CAMERADEBUG("CAMERA_SVCS: Clean Up: Set camera_state=CAMERA_STATE_INIT", 0, 0, 0);
  camera_state = CAMERA_STATE_INIT;

  if (tcb)
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: Clean Up: Set CAMERA_SIG", 0, 0, 0 );
    (void) rex_set_sigs (tcb, CAMERA_SIG);
  }

#endif /* nT_IO_CARD */
} /* camera_process_cleanup */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_START_PREVIEW

DESCRIPTION
              Enters previow mode.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_start_preview
(
  camera_cb_f_type callback,
  void            *client_data
)
{
#ifndef T_IO_CARD
  camera_blt_ext_recalculate_window = TRUE;
  camera_dsp_error_status.received_camif_error = FALSE;
  /* Terminate an existing function first */
  /* Be certain this is the VFE/MP4E image by checking the state */
  if ((camera_state == CAMERA_STATE_PREVIEW) ||
      (camera_state == CAMERA_STATE_QVP_ENCODING))
  {
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate (CAMERA_EXIT_CB_DONE, 0);
    MSG_HIGH ("Stop previous command.", 0, 0, 0);
  }
#ifdef FEATURE_MDP
  /* De-register the video */
  camera_deregister_video (TRUE);
  /* De-register the overlay layer */
  camera_deregister_layer1();
  if (preview_overlay && (mdpImg.layer == MDP_LAYER_0))
  {
    /* The following call is needed to clear layer 0, before the window is
     * adjusted, in case preview window is smaller than the display. The same
     * function is called in camera_svcs_blt_ex() after window is adjusted. */
    camera_svcs_display_frame_overlay(TRUE);
  }
#endif /* FEATURE_MDP */

  CAMERA_SLEEP_FORBID ();
  CAMERA_HALT_ALLOW();
  CAMERA_BUS_ON();

#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */

  camera_clr_take_picture();
  camera_func          = CAMERA_FUNC_START_PREVIEW;
  MSG_CAMERADEBUG("CAMERA_SVCS: Request camera_stopping=NO_STOPPING", 0, 0, 0);
  camera_stopping = FALSE;
  camera_app_cb        = callback;
  camera_app_data      = client_data;
  MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_PREVIEW", 0, 0, 0);
  camera_state = CAMERA_STATE_PREVIEW;
  camera_camsensor_op_mode = CAMSENSOR_OP_MODE_PREVIEW;
  camera_output1_frame_rate.waiting_for_first_output1_frame = FALSE;
  camera_dsp_command = CAMQDSP_CMD_MAX;
#ifndef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III
  camera_svcs_optimize_resources ();
#endif /* FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III */

  MSG_HIGH ("State transition to preview", 0, 0, 0);
  event_report (EVENT_CAMERA_PREVIEW);

  if((camera_preview_dx == 0 || camera_preview_dy == 0))
  {
    MSG_ERROR("Invalid Preview Dimensions, Failing in camera_process_start_preview",0,0,0);

    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate (CAMERA_EXIT_CB_ERROR, (int)CAMERA_ERROR_CONFIG);
    return;
  }
  
  if(camera_set_preview_buffers())
  {
      MSG_HIGH("No memory!, Failing camera_process_start_preview",0,0,0);
      return;
  }
  camera_set_preview_headers ();

#ifdef FEATURE_QVPHONE
  if (camera_qvp_enabled)
  {
    camera_dsp_command = CAMQDSP_CMD_IDLE;
    MSG_CAMERADEBUG("CAMERA_SVCS: Send IDLEMessage to DSP", 0, 0, 0);
    camera_log (LOG_VFEC, 0, 0, (uint32)VFE_IDLE, __LINE__);
    if (VFE_Idle () != CAMQDSP_SUCCESS)
    {
      camera_log (LOG_INFO, 0, 0, (uint32)IDLE_REJ, __LINE__);
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
      MSG_ERROR ("CAMERA_SVCS: Cannot idle DSP. State transition to ready", 0, 0, 0);
      event_report (EVENT_CAMERA_CANNOT_IDLE_DSP);
    }
    else if (camera_app_cb)
    {
      MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
      (camera_app_cb)( CAMERA_RSP_CB_SUCCESS, camera_app_data, camera_func, 0);
    }
  }
  else
#endif /* FEATURE_QVPHONE */
  {
    boolean ret_val;

    camera_log (LOG_VFEC, 0, 0, (uint32)VFE_INIT, __LINE__);

    MSG_CAMERADEBUG("CAMERA_SVCS: Initialize Image in Preview", 0, 0, 0);
    camera_dsp_state = DSP_ENABLING;
    ret_val = (VFE_Initialize (camera_qdsp_cb) != CAMQDSP_SUCCESS);

    if (ret_val)
    {
      camera_dsp_state = DSP_DISABLED;
      camera_log (LOG_INFO, 0, 0, (uint32)INIT_REJ, __LINE__);
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
      MSG_ERROR ("CAMERA_SVCS: Cannot load DSP. State transition to ready", 0, 0, 0);
      event_report (EVENT_CAMERA_CANNOT_LOAD_DSP);
    }
    else
    {
      if (camera_app_cb)
      {
        MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
        (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, camera_func, 0);
      }
    }
  }
#endif /* nT_IO_CARD */
} /* camera_process_start_preview */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_STOP_PREVIEW

DESCRIPTION
              Stop the preview mode.
              CAMERA_PREVIEW_STOPPED callback is make before function
              returns.

DEPENDENCIES
  The camera sensor must be make ready first.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_stop_preview ( rex_tcb_type *tcb )
{
#ifndef T_IO_CARD

  camera_tcb = tcb;

#ifdef FEATURE_MDP
#if defined FEATURE_VIDEO_ENCODE
  /* De-register the video */
  if (preview_mode == CAMERA_PREVIEW_MODE_MOVIE)
  {
#ifdef FEATURE_MMOVERLAY
    camera_deregister_video (FALSE);
#else /* FEATURE_MMOVERLAY */
    camera_deregister_video (TRUE);
#endif /* !FEATURE_MMOVERLAY */
  }
  else
#endif /* defined FEATURE_VIDEO_ENCODE */
  {
    camera_deregister_video (TRUE);
  }
  /* De-register the overlay layer */
  camera_deregister_layer1();
#endif /* FEATURE_MDP */

  if ( !camera_stopping)
  {
    /* If AF is active, then stop it */
    if (camera_af_is_active())
    {
      camera_af_stop_focus();
    }
    camera_terminate (CAMERA_EXIT_CB_DONE, 0);
  }
  else
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: PROCESS_STOP_PREVIEW camera_stopping:%d camera_state:%d", camera_stopping, camera_state, 0);
  }


#endif /* nT_IO_CARD */
} /* camera_process_stop_preview */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_TAKE_SINGLE_PICTURE

DESCRIPTION
              The next frame is taken for encoding at the specified format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_take_single_picture(void)
{
#ifndef T_IO_CARD
  boolean ret_val = FALSE;
  snapshot_frame.thumbnail_image = 0;
  snapshot_frame.buffer = 0;

  camera_abort_picture = FALSE;

  MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_TAKE_PICTURE", 0, 0, 0);
  camera_state = CAMERA_STATE_TAKE_PICTURE;

  camera_svcs_optimize_resources ();
  clk_dereg(&camera_wait_camif_done_clk);
  camera_take_picture_status_set_default();
  camera_capture_delay = 0;
  camera_dsp_error_status_set_default();
  camera_camsensor_op_mode = CAMSENSOR_OP_MODE_SNAPSHOT;
  camera_dsp_command = CAMQDSP_CMD_MAX;

#if (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED)
  if ( (camera_svcs_hjr_enabled() == TRUE) && (camctrl_tbl.hjr_bayer_filtering_enable == TRUE) )
  {
    /* Need to take a raw image and run bayer filter then do
     * offline snapshot if ABF is not done in H/W (e.g., LCU)
     */
    camera_camsensor_op_mode = CAMSENSOR_OP_MODE_RAW_SNAPSHOT;
  }
#endif /* (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED) */

  if (camera_encode_properties.format == CAMERA_RAW)
  {
    camera_camsensor_op_mode = CAMSENSOR_OP_MODE_RAW_SNAPSHOT;
  }
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*  FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/

  MSG_HIGH ("State transition to take picture", 0, 0, 0);
  event_report (EVENT_CAMERA_TAKE_PICTURE);

  if (camera_take_picture_retry == CAMERA_TAKE_PICTURE_RETRY_NONE)
  {
    camera_log (LOG_VFEC, 0, 0, (uint32)VFE_INIT, __LINE__);
    MSG_CAMERADEBUG("CAMERA_SVCS: Initialize Image in Take Picture", 0, 0, 0);
    camera_dsp_state = DSP_ENABLING;
#ifdef QDSP_MODULE_VFE05_CAPTURE_DEFINED
    camctrl_tbl.use_vfe_image_swap = TRUE;
#endif /*QDSP_MODULE_VFE05_CAPTURE_DEFINED*/
#ifdef QDSP_IMAGE_VFE_SA_DEFINED   
#error code not present
#endif	/* QDSP_IMAGE_VFE_SA_DEFINED */   
    ret_val = (VFE_Initialize (camera_qdsp_cb) != CAMQDSP_SUCCESS);
#if defined QDSP_MODULE_VFE05_CAPTURE_DEFINED || defined QDSP_IMAGE_VFE_SA_DEFINED
    camctrl_tbl.use_vfe_image_swap = FALSE;
#endif /* defined QDSP_MODULE_VFE05_CAPTURE_DEFINED || defined QDSP_IMAGE_VFE_SA_DEFINED*/

    if (ret_val)
    {
      camera_dsp_state = DSP_DISABLED;
      camera_log (LOG_INFO, 0, 0, (uint32)INIT_REJ, __LINE__);
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
      MSG_ERROR ("Cannot load DSP. State transition to ready", 0, 0, 0);
      event_report (EVENT_CAMERA_CANNOT_LOAD_DSP);
    }
    else
    {
      if (camera_app_cb)
      {
        MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
        (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, camera_func, 0);
      }
    }
  }
#endif /* nT_IO_CARD */
} /* camera_process_take_single_picture */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_TAKE_PICTURE

DESCRIPTION
              The next frame is taken for encoding at the specified format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_take_picture
(
  camera_cb_f_type callback,
  void * client_data
)
{
#ifndef T_IO_CARD

  /* Terminate an existing function first */
  if ((camera_state != CAMERA_STATE_INIT) &&
      (camera_state != CAMERA_STATE_READY))
  {
    camera_log(LOG_TRM, 0, 0, (uint32)INV_STATE, __LINE__);
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate(CAMERA_EXIT_CB_DONE, 0);
    MSG_HIGH("Stop previous command.", 0, 0, 0);
  }

  camera_app_cb = callback;
  camera_app_data = client_data;
  camera_func = CAMERA_FUNC_TAKE_PICTURE;
  camera_blt_ext_recalculate_window = TRUE;
  CAMERA_SLEEP_FORBID();
  CAMERA_HALT_FORBID();
  CAMERA_BUS_ON();
#ifdef FEATURE_MDP
  CAM_Free (picture_preview_copy);
  picture_preview_copy = NULL;
#endif /* FEATURE_MDP */

  /* Call the Hand Jitter Reduction Controller to decide on the
   * Gain, Exposure Time and No of frames to capture
   */

#ifdef FEATURE_CAMERA_HJR

  if (camera_svcs_hjr_enabled() == TRUE)
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: Hand Jitter Enabled", 0, 0, 0);
    camera_aec_prepare_for_hjr(&camsensor_static_params[camera_asi]);

    if ( HJR_structure.new_numberFrames == 1 )
    {
      camera_process_take_single_picture();
    }
    else
    {
      /* Set up the information required for multishot */
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#else
      HJR_structure.new_numberFrames = 1;
      camera_process_take_single_picture();
#endif /* FEATURE_CAMERA_BURST_MODE */
    }
  }
  else
#endif /* FEATURE_CAMERA_HJR */
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: Hand Jitter Disabled", 0, 0, 0);
    camera_process_take_single_picture();
  }

#endif /* nT_IO_CARD */
} /* camera_process_take_picture */

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/


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

/*===========================================================================

FUNCTION: CAMERA_PREVIEW_SET_BUFFER_STATUS

DESCRIPTION:
  To keep track of which preview buffers are busy, meaning buffers that are
  used either by the VFE or display.
  Or, to clr the busy flag when the preview buffers are not used either by
  the display, or the VFE.
DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  None
============================================================================*/
static void camera_preview_set_buffer_status(uint8 buffer_index, camera_preview_buffer_status_type new_status)
{
  camera_preview_buffer_status_type old_status;
  old_status = camera_preview_buffers.buffers[buffer_index].status;
  if (buffer_index < CAMERA_NUM_OF_PREVIEW_BUFFERS)
  {
    if (new_status == BUSY_WITH_APP)
    {
      if (camera_preview_buffers.num_buffers_with_display < CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_DISPLAY)
      {
        camera_preview_buffers.num_buffers_with_display++;
      }
      else
      {
        MSG_ERROR("CAMERA_SVCS: Number of buffers with display is more than 2", 0, 0, 0);
        return;
      }
    }
    /* We do not modify num_buffers_with_display when changing to
     * BUSY_WITH_DISPLAY */
    else if (new_status == BUSY_WITH_VFE) /* meaning new_status = BUSY_WITH_VFE */
    {
      if (camera_preview_buffers.num_buffers_with_VFE < CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_VFE)
      {
        camera_preview_buffers.num_buffers_with_VFE++;
      }
      else
      {
        MSG_ERROR("CAMERA_SVCS: Number of buffers with VFE is more than 3", 0, 0, 0);
        return;
      }
    }
    else if (new_status == AVAILABLE)
    {
      /* if buffer is previously with display */
      if (old_status == BUSY_WITH_APP || old_status == BUSY_WITH_DISPLAY)
      {
        if (camera_preview_buffers.num_buffers_with_display)
        {
          camera_preview_buffers.num_buffers_with_display--;
        }
        else
        {
          MSG_ERROR("CAMERA_SVCS: Number of buffers with display is negative", 0, 0, 0);
          return;
        }
      }
      else if (camera_preview_buffers.buffers[buffer_index].status == BUSY_WITH_VFE)
      {
        if (camera_preview_buffers.num_buffers_with_VFE)
        {
          camera_preview_buffers.num_buffers_with_VFE--;
        }
        else
        {
          MSG_ERROR("CAMERA_SVCS: Number of buffers with display is negative", 0, 0, 0);
          return;
        }
      }
    }
    camera_preview_buffers.buffers[buffer_index].status = new_status;
    MSG_CAMERAPBUF("PBUF SET status preview buffer %d new status=%d old status=%d",
        buffer_index, new_status, old_status);
    if (new_status == BUSY_WITH_APP)
    {
      if (camera_app_buf_index[0] == CAMERA_INVALID_BUF_INDEX)
      {
        camera_app_buf_index[0] = buffer_index;
        MSG_CAMERAPBUF("PBUF SET status, preview buffer %d is app0", buffer_index, 0, 0);
      }
      else
      {
        camera_app_buf_index[1] = buffer_index;
        MSG_CAMERAPBUF("PBUF SET status, preview buffer %d is app1", buffer_index, 0, 0);
      }
    }
  }
}
/*===========================================================================

FUNCTION      CAMERA_SVCS_ACK_VFE_OUTPUT1

DESCRIPTION
              Send VFE acknowledgement during preview

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void camera_svcs_ack_vfe_output1 (void)
{
  uint32 current_buffer;
  boolean buffer_found = FALSE;
  VFE_EOF_Ack1Type ackOutput1;
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif
  ackOutput1.eofAck1.Y_Address = NULL;
  ackOutput1.eofAck1.CbCr_Address = NULL;
  /* Check if VFE has less than 3 buffers, if it does, send acknowledgement */
  if (camera_preview_buffers.num_buffers_with_VFE < CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_VFE)
  {
    /* select the available buffer if the display has not released frame yet */
    for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
    {
      if (camera_preview_buffers.buffers[current_buffer].status == AVAILABLE)
      {
        buffer_found = TRUE;
        ackOutput1.eofAck1.Y_Address =
          (VFE_AddressType) qcamrawGetDataYPtr(camera_preview_buffers.
                                               buffers[current_buffer].buf_ptr);
        ackOutput1.eofAck1.CbCr_Address =
          (VFE_AddressType) qcamrawGetDataCbCrPtr (camera_preview_buffers.
                                                   buffers[current_buffer].buf_ptr);
#if 0 /* check later, need chromatix */
        ackOutput1.asfNormalizeFactor =
#endif
        /* Ack only if the last command is not IDLE */
        if (camera_dsp_command != CAMQDSP_CMD_IDLE)
        {
          camera_dsp_command = CAMQDSP_CMD_END_OF_FRAME_ACK1;
          (void) VFE_EOF_Ack1 (ackOutput1);
          camera_preview_set_buffer_status(current_buffer, BUSY_WITH_VFE);
          MSG_CAMERAPBUF("PBUF ack_vfe_output1, SET buffer %d BUSY_WITH_VFE",
              current_buffer, 0, 0);
        }
        break;
      }
    }
    if (!buffer_found)
    {
      MSG_ERROR("Cannot find AVAILABLE buffer to send to VFE in _ack_vfe_output1", 0, 0, 0);
    }
  }
}

#ifdef FEATURE_MDP
/*===========================================================================

FUNCTION: CAMERA_DISPLAY_ASYNC_CB

DESCRIPTION:
  Callback _release_frame function when display update is finished. Queue to graph task

DEPENDENCIES:

SIDE EFFECTS:
  None

RETURN VALUE:
  None
============================================================================*/
static void camera_display_async_cb(void * cb_arg, MDP_STATUS status)
{
  /* pass the returned buffer by MDP to graph context */
  graph_queue_camera_func ((uint32)CAMERA_FUNC_RELEASE_FRAME,
                             (void *)0, (void *)0, (uint32) cb_arg, 0, 0, 0);
}
#endif /* FEATURE_MDP */


/*===========================================================================

FUNCTION      CAMERA_CALCULATE_OUTPUT1_FRAMERATE

DESCRIPTION
              Calculate output1 fps.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

static void camera_calculate_output1_framerate (void)
{
  uint32 preview_fps = Q8;
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
 {
    preview_fps = camsensor_static_params[camera_asi].preview_fps/Q8;
 }


  camera_output1_frame_rate.end_ms = (uint32) timetick_get_ms();
  /* Calculate frame rate based on an average of 5 seconds */
  if ((camera_output1_frame_rate.end_ms - camera_output1_frame_rate.start_ms) >= 4000)
  {
    /* output MSG only if changed */
    if (camera_output1_frame_rate.last_fps != camera_output1_frame_rate.count)
    {
      MSG_HIGH("FPS in Viewfinder Mode: from_VFE:%d driver_configured:%d",
               camera_output1_frame_rate.count/4,
               (preview_fps), 0);
    }
    camera_output1_frame_rate.last_fps = camera_output1_frame_rate.count;
    camera_output1_frame_rate.fps[fps_log_index++] = camera_output1_frame_rate.count;
    fps_log_index %= FPS_LOG_SIZE;
    camera_output1_frame_rate.start_ms = camera_output1_frame_rate.end_ms;
    camera_output1_frame_rate.count = 0;
  }
 } /* camera_calculate_output1_framerate */
/*===========================================================================

FUNCTION      CAMERA_PROCESS_RELEASE_FRAME

DESCRIPTION
              Release frame for VFE reuse.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_release_frame (uint8 buffer_index)
{
  camera_preview_buffer_status_type status;
  /* If invalid, it is a release call from application. */
  if (buffer_index == CAMERA_INVALID_BUF_INDEX)
  {
    /* if the first application buffer is invalid, nothing to do. */
    if (camera_app_buf_index[0] == CAMERA_INVALID_BUF_INDEX)
    {
      MSG_CAMERAPBUF("PBUF process_release_frame, app0 is not valid", 0, 0, 0);
      return;
    }

    buffer_index = camera_app_buf_index[0];
    /* Move up the buffer index */
    camera_app_buf_index[0] = camera_app_buf_index[1];
    camera_app_buf_index[1] = CAMERA_INVALID_BUF_INDEX;
    status = camera_preview_buffers.buffers[buffer_index].status;
    /* if the first application buffer is not with application or display,
     * nothing to do */
    if ((status != BUSY_WITH_APP) && (status != BUSY_WITH_DISPLAY))
    {
      MSG_CAMERAPBUF("PBUF process_release_frame, preview buffer %d is not app0", buffer_index, 0, 0);
      return;
    }

    if(status == BUSY_WITH_DISPLAY)
    {
      /* If buffer with display, wait for display callback */
      MSG_CAMERAPBUF("PBUF process_release_frame, preview buffer %d is with display", buffer_index, 0, 0);
      return;
    }
  }
  camera_calculate_output1_framerate();
  /* Set the already displayed buffer to AVAILABLE and also to be the acknowledgement buffer */
  camera_preview_set_buffer_status(buffer_index, AVAILABLE);
  MSG_CAMERAPBUF("PBUF process_release_frame, SET buffer %d AVAILABLE",
      buffer_index, 0, 0);
} /* camera_process_release_frame */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_BLT

DESCRIPTION
              Post processing of BLT function. Change buffer state to
              busy_with_display.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_blt (uint8 buffer_index)
{
  if (camera_preview_buffers.buffers[buffer_index].status == BUSY_WITH_APP)
  {
    camera_preview_set_buffer_status(buffer_index, BUSY_WITH_DISPLAY);
    MSG_CAMERAPBUF("PBUF process_blt, SET buffer %d BUSY_WITH_DISPLAY", buffer_index, 0, 0);
  }
} /* camera_process_blt */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_RELEASE_PICTURE

DESCRIPTION
              Release picture (if picture is available), stop taking
              picture (if taking picture) or stop encoding (if doing
              encoding).

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_release_picture ( rex_tcb_type *tcb )
{
#ifndef T_IO_CARD
  camera_tcb = tcb;
  camera_terminate(CAMERA_EXIT_CB_ABORT, CAMERA_ERROR_MAX);
#endif /* nT_IO_CARD */
} /* camera_process_release_picture */

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/

/*===========================================================================

FUNCTION      CAMERA_COPY_TO_FILE

DESCRIPTION
              The raw picture is copied to the specified file.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/

void camera_copy_to_file(uint32 size, uint8 *buffer, uint32 size2, uint8 *buffer2)
{
#ifndef T_IO_CARD
  fs_open_xparms_type open_parms;
  char *copyfilename = NULL;
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /* FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/
   copyfilename = camera_handle.efs.filename;

 if(copyfilename != NULL)
 {
    fs_remove(copyfilename, NULL, &fs_rsp);
    open_parms.create.cleanup_option = FS_OC_CLOSE;
    open_parms.create.buffering_option = FS_OB_PROHIBIT;
    fs_open(copyfilename, FS_OA_CREATE, &open_parms, NULL, &fs_rsp);
    if (fs_rsp.open.status != FS_OKAY_S)
    {
      efs_fail_count++;
      camera_log(LOG_INFO, 0, 0, (uint32)EFS_FAIL, __LINE__);
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      camera_terminate(CAMERA_EXIT_CB_ERROR, 0);
      MSG_ERROR("EFS failed. %d State transition to ready", efs_fail_count, 0, 0);
      event_report(EVENT_CAMERA_EFS_FAILED);
      return;
    }
    camera_fs_handle = fs_rsp.open.handle;
  
    fs_write(camera_fs_handle, buffer, size, NULL, &fs_rsp);
    if (fs_rsp.write.count != size || fs_rsp.write.status != FS_OKAY_S)
    {
      efs_fail_count++;
      camera_log(LOG_INFO, 0, 0, (uint32)EFS_FAIL, __LINE__);
      camera_terminate(CAMERA_EXIT_CB_ERROR, 0);
      MSG_ERROR("EFS failed to wr data. %d %d State transition to ready", fs_rsp.write.count, size, 0);
      event_report(EVENT_CAMERA_EFS_FAILED);
      fs_close(camera_fs_handle, NULL, &fs_rsp);
      camera_fs_handle = 0;
      return;
    }
  
    if(buffer2!=NULL)
    {
      fs_seek_write(camera_fs_handle, FS_SEEK_SET, size, buffer2, size2, NULL, &fs_rsp);
  
      if (fs_rsp.seek_write.count != size2 || fs_rsp.seek_write.status != FS_OKAY_S)
      {
        efs_fail_count++;
        camera_log(LOG_INFO, 0, 0, (uint32)EFS_FAIL, __LINE__);
        camera_terminate(CAMERA_EXIT_CB_ERROR, 0);
        MSG_ERROR("EFS failed to append data. %d %d State transition to ready", fs_rsp.seek_write.count, size2, 0);
        event_report(EVENT_CAMERA_EFS_FAILED);
        fs_close(camera_fs_handle, NULL, &fs_rsp);
        camera_fs_handle = 0;
        return;
      }
    }
  
    fs_close(camera_fs_handle, NULL, &fs_rsp);
    camera_fs_handle = 0;
    event_report_payload (EVENT_CAMERA_PICTURE_SAVED, (uint8) strlen (copyfilename), copyfilename);
   }
#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

#endif /* nT_IO_CARD */
} /* camera_copy_to_file */

/*===========================================================================
FUNCTION      CAMERA_SVCS_IMAGE_ROTATE_FOR_PNG

DESCRIPTION
              This function will rotate the image for PNG format since JPEG
              does its own rotation of image.

DEPENDENCIES
  None

RETURN VALUE
              True for success.
              False for failure.

SIDE EFFECTS
  None

===========================================================================*/
boolean camera_svcs_image_rotate_for_png
(
  qcamrawHeaderType *main_image_header,
  uint8             *main_image_buffer
)
{
  uint8             *rawsnapshotrotate_ptr = NULL;
  ipl_image_type    rawsnapshotimg_in;
  ipl_image_type    rawsnapshotimg_out;
  ipl_col_for_type  inputformat;
  ipl_status_type   status=IPL_SUCCESS;

  if ((main_image_header == NULL) || (main_image_buffer == NULL))
  {
    return FALSE;
  }

  switch (camera_snapshot_format)
  {
    case CAMERA_H2V1:
      inputformat = IPL_YCrCb422_LINE_PK;
      break;

    case CAMERA_H2V2:
    default:
      inputformat = IPL_YCrCb420_LINE_PK;
      break;
  }

  rawsnapshotrotate_ptr = CAM_Malloc ( (main_image_header->YSize + main_image_header->padSize2 + main_image_header->CbCrSize)
                                       | CAM_ALLOC_NO_ZMEM );

  if ( rawsnapshotrotate_ptr == NULL )
  {
    return FALSE;
  }

  /* Handle the main image
   * Things to remember
   * 1. We are using IPL to achieve rotation.
   */

  rawsnapshotimg_in.dx      = main_image_header->picWidth;
  rawsnapshotimg_in.dy      = main_image_header->picHeight;
  rawsnapshotimg_in.imgPtr  = main_image_buffer;
  rawsnapshotimg_in.clrPtr  = main_image_header->chromaBuffer;
  rawsnapshotimg_out.dx     = main_image_header->picWidth;
  rawsnapshotimg_out.dy     = main_image_header->picHeight;
  rawsnapshotimg_out.imgPtr = rawsnapshotrotate_ptr;
  rawsnapshotimg_out.clrPtr = ( rawsnapshotrotate_ptr + main_image_header->YSize );

  rawsnapshotimg_in.cFormat  = inputformat;
  rawsnapshotimg_out.cFormat = inputformat;

  if ( camera_encode_properties.format == CAMERA_PNG )
  {
    if (camera_jpeg_encode_rotation_format != JPEGENC_NO_ROT)
    {
      ipl_rotate90_type rotate_val = IPL_ROT90;

      if (camera_jpeg_encode_rotation_format == JPEGENC_CCLK180)
      {
        rotate_val = IPL_ROT180;
        rawsnapshotimg_out.dx = rawsnapshotimg_in.dx;
        rawsnapshotimg_out.dy = rawsnapshotimg_in.dy;
      }
      else
      {
        rawsnapshotimg_out.dx = rawsnapshotimg_in.dy;
        rawsnapshotimg_out.dy = rawsnapshotimg_in.dx;
      }

      status = ipl_crop_resize_rot(&rawsnapshotimg_in, &rawsnapshotimg_out, NULL, NULL, rotate_val, IPL_QUALITY_MEDIUM);
      if (status == IPL_SUCCESS)
      {
        rawsnapshotimg_in.dx = rawsnapshotimg_out.dx;
        rawsnapshotimg_in.dy = rawsnapshotimg_out.dy;
        main_image_header->picWidth   = (uint16) rawsnapshotimg_out.dx;
        main_image_header->picHeight  = (uint16) rawsnapshotimg_out.dy;
        main_image_header->dataWidth  = (uint16) rawsnapshotimg_out.dx;
        main_image_header->dataHeight = (uint16) rawsnapshotimg_out.dy;
        status = ipl_copy_and_paste(&rawsnapshotimg_out,&rawsnapshotimg_in, NULL, NULL);
      }
    }
  }

  CAM_Free ( rawsnapshotrotate_ptr );

  if (status == IPL_SUCCESS)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION      CAMERA_ENCODE_PICTURE_HANDLER

DESCRIPTION
              The picture is encoded to the specified file name or copied
              into client memory.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
void camera_encode_picture_handler
(
  qcamrawHeaderType *main_image_header,
  uint8             *main_image_buffer,
  qcamrawHeaderType *thumbnail_image_header,
  uint8             *thumbnail_image_buffer
)
{

#ifdef FEATURE_PNG_ENCODER
  /* If it is PNG encoding then prepare for PNG encoding */
  /* PNG encoding is only ARM base */
  /* PNG encoder is using GRAPH thread. If Camera using GRAPH thread to start    */
  /* the PNG encoder then it should release the thread for the encoder to process */
  if (camera_encode_properties.format == CAMERA_PNG)
  {

    /* Later UI should make sure that png pictures are always H2V1 */
    if (camera_snapshot_format == CAMERA_H2V2)
    {
      png_encode_spec.input_data   = YCrCb420LP;
      MSG_CAMERADEBUG("Encode snapshot: PNG H2V2", 0, 0, 0);
    }
    else
    {
      if ((camera_jpeg_encode_rotation_format == JPEGENC_CCLK90) || (camera_jpeg_encode_rotation_format == JPEGENC_CCLK270))
      {
        png_encode_spec.input_data   = YCrCb420LP;
        MSG_CAMERADEBUG("Encode snapshot: PNG H2V2", 0, 0, 0);
      }
      else
      {
        png_encode_spec.input_data   = YCrCb422LP;
        MSG_CAMERADEBUG("Encode snapshot: PNG H2V1", 0, 0, 0);
      }
    }

    /* Set up essential parameters */
    png_encode_spec.input_ptr    = (uint8 *) main_image_buffer ;
    png_encode_spec.chroma_ptr   = main_image_header->chromaBuffer;
    png_encode_spec.image_width  = main_image_header->picWidth;
    png_encode_spec.image_height = main_image_header->picHeight;
    png_encode_spec.bayer_image_width  = main_image_header->picWidth;
    png_encode_spec.bayer_image_height = main_image_header->picHeight;
#ifdef CAMERA_USES_LIBSTD
    (void) std_strlcpy(png_encode_spec.filename, camera_handle.efs.filename,
                  FS_FILENAME_MAX_LENGTH_P - 1 );
#else /* CAMERA_USES_LIBSTD */
    (void) strcpy(png_encode_spec.filename, camera_handle.efs.filename);
#endif /* CAMERA_USES_LIBSTD */
    if (camera_handle.device == CAMERA_DEVICE_EFS)
    {
       png_handle.device = PNGE_DEVICE_EFS;
       png_handle.fs_rsp = fs_rsp;
    }
    else if (camera_handle.device == CAMERA_DEVICE_MEM)
    {
       png_handle.device = PNGE_DEVICE_MEM;
       png_handle.buffer = camera_handle.mem.buffer;
       png_handle.length = camera_handle.mem.length;
    }

    if ( pnge_encode (png_encode_spec, &png_handle, camera_PNG_CB ) != PNGE_SUCCESS)
    {
        MSG_ERROR(" Can't encode the image", 0, 0, 0);
        camera_clr_take_picture ();
        camera_terminate(CAMERA_EXIT_CB_FAILED, 0);
    }
    else
    {
      camera_take_picture_status.encode_in_progress = TRUE;
      /* if queueing command successfully then notify the apps */
      if (camera_app_cb)
      {
        MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
        (camera_app_cb)(CAMERA_RSP_CB_SUCCESS, camera_app_data, camera_func, 0);
        MSG_HIGH("Picture saved. State transition to ready", 0, 0, 0);
        event_report_payload (EVENT_CAMERA_PICTURE_SAVED, 0, 0);
      }
    }

  }
  // END else if (encode_info.dst_format == CAMERA_PNG)
  else
#endif /* FEATURE_PNG_ENCODER */
  {
      camera_svcs_fill_exif_attributes(&camera_exif_attributes);
      exif_set_attribute_info(&camera_exif_info, &camera_exif_attributes);
#ifdef FEATURE_CAMERA_LPM
#error code not present
#else
      camera_jpeg_encode (main_image_header, main_image_buffer, thumbnail_image_header, thumbnail_image_buffer);
#endif
  }
}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_ENCODE_PICTURE_STANDALONE

DESCRIPTION
              The picture is encoded to the specified file name or copied
              into client memory.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_encode_picture_standalone
(
  qcamrawHeaderType *main_image_header,
  uint8             *main_image_buffer,
  qcamrawHeaderType *thumbnail_image_header,
  uint8             *thumbnail_image_buffer
)
{
#ifndef T_IO_CARD
  camera_func               = CAMERA_FUNC_ENCODE_PICTURE;
  camera_abort_picture      = FALSE;
  camera_take_picture_status.encode_in_progress = FALSE;

  CAMERA_SLEEP_FORBID();
  CAMERA_HALT_ALLOW();
  CAMERA_BUS_ON();
  MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_ENCODE_PICTURE", 0, 0, 0);
  camera_state = CAMERA_STATE_ENCODE_PICTURE;
  camera_svcs_optimize_resources();

  MSG_CAMERADEBUG("camera_process_encode_picture_standalone", 0, 0, 0);
  event_report(EVENT_CAMERA_SAVE_PICTURE);


  if (camera_app_cb)
  {
    MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
    (camera_app_cb)(CAMERA_RSP_CB_SUCCESS, camera_app_data, camera_func, 0);
  }

  /* If Raw image store to file */
  if (camera_encode_properties.format == CAMERA_RAW)
  {
    /* Store to EFS */
    camera_copy_to_file (main_image_header->YSize, main_image_buffer, 0, NULL);
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate(CAMERA_EXIT_CB_DONE, 0);
    MSG_HIGH("Picture saved. State transition to ready", 0, 0, 0);
    CAM_Free (raw_snapshot_buf);
    raw_snapshot_buf = NULL;
    return;
  }

  /* If YCbCr image store it to the file */
  if (camera_encode_properties.format == CAMERA_YCBCR_ENCODE) 
  {
    /* Store to EFS */
    camera_copy_to_file (main_image_header->YSize, 
                         (uint8*)main_image_buffer, main_image_header->CbCrSize, 
                         (uint8*)main_image_header->chromaBuffer);

    if (camera_app_cb)
    {
      (camera_app_cb)( CAMERA_EXIT_CB_DONE, camera_app_data, camera_func, 0);
    }
    CAM_Free (ycbcr_image_info);
    ycbcr_image_info = NULL;
    CAM_Free (snapshot_chroma_buf);
    snapshot_chroma_buf = NULL;
    CAM_Free (snapshot_luma_buf);
    snapshot_luma_buf = NULL;
    return;
  }
  
#ifdef FEATURE_PNG_ENCODER
  if(camera_encode_properties.format == CAMERA_PNG)
  {
    if (camera_svcs_image_rotate_for_png( main_image_header, main_image_buffer))
    {
       camera_encode_picture_handler(main_image_header, main_image_buffer, thumbnail_image_header, thumbnail_image_buffer);
    }
    else
    {
      if (camera_app_cb)
      {
        MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
        (camera_app_cb)( CAMERA_EXIT_CB_FAILED, camera_app_data, camera_func, 0);
      }
      camera_clr_take_picture();
    }
  }
  else
#endif
  {
     camera_encode_picture_handler(main_image_header, main_image_buffer, thumbnail_image_header, thumbnail_image_buffer);
  }

#endif /* nT_IO_CARD */
} /* camera_process_encode_picture_standalone */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_ENCODE_PICTURE

DESCRIPTION
              The picture is encoded to the specified file name or copied
              into client memory.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_encode_picture
(
  camera_cb_f_type   callback,
  void              *client_data,
  qcamrawHeaderType *main_image_header,
  uint8             *main_image_buffer,
  qcamrawHeaderType *thumbnail_image_header,
  uint8             *thumbnail_image_buffer
)
{
#ifndef T_IO_CARD

  /* Terminate an existing function first */
  if ((camera_state != CAMERA_STATE_INIT) &&
      (camera_state != CAMERA_STATE_READY)
     )
  {
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate(CAMERA_EXIT_CB_DONE, 0);
    MSG_HIGH("Stop previous command.", 0, 0, 0);
  }

  camera_app_cb    = callback;
  camera_app_data  = client_data;

  camera_process_encode_picture_standalone ( main_image_header, main_image_buffer,
                                             thumbnail_image_header, thumbnail_image_buffer
                                           );
#endif /* nT_IO_CARD */
} /* camera_process_encode_picture */

#ifdef FEATURE_QVPHONE
/*===========================================================================

FUNCTION      CAMERA_SET_QVP_ENCODE_BUFFERS

DESCRIPTION
              Set QVP Encode (and video mode) buffers.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_set_qvp_encode_buffers (void)
{
  //Set up output2 buffers
  CAM_Free (qvp_enc_buffer0);
  CAM_Free (qvp_enc_buffer1);
  CAM_Free (qvp_enc_buffer2);
  CAM_Free (qvp_enc_buffer3);

  //Assumption here: Output1 and Output2 have the same image size for QVP
  qvp_enc_buffer0 = CAM_Malloc (camera_preview_buffer_size);
  qvp_enc_buffer1 = CAM_Malloc (camera_preview_buffer_size);
  qvp_enc_buffer2 = CAM_Malloc (camera_preview_buffer_size);
  qvp_enc_buffer3 = CAM_Malloc (camera_preview_buffer_size);

  if (!qvp_enc_buffer0 || !qvp_enc_buffer1 || !qvp_enc_buffer2 ||
      !qvp_enc_buffer3)
  {
    //No memory, terminate the Camera

    QVP_MSG_VENC_ERROR("No Memory for Encoder Buffer", 0, 0, 0);
    camera_no_memory ();
    return;
  }
}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_START_QVP_ENCODE

DESCRIPTION
              QVP Encoding is started.

DEPENDENCIES

  QDSP VFE should be in IDLE State

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_start_qvp_encode
(
  camera_cb_f_type callback,
  void *client_data,
  rex_tcb_type    *tcb,
  camera_ret_code_type *status
)
{
  *status = CAMERA_SUCCESS;

  /* Terminate an existing function first */
  if ((camera_state != CAMERA_STATE_INIT) &&
      (camera_state != CAMERA_STATE_READY))
  {
    camera_log(LOG_TRM, 0, 0, (uint32)INV_STATE, __LINE__);
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate(CAMERA_EXIT_CB_DONE, 0);
  }
  CAMERA_SLEEP_FORBID();
  CAMERA_HALT_ALLOW();
  CAMERA_BUS_ON();

  camera_func = CAMERA_FUNC_START_QVP_ENCODE;
  MSG_CAMERADEBUG("CAMERA_SVCS: Request camera_stopping=NO_STOPPING", 0, 0, 0);
  camera_stopping = FALSE;
  camera_app_cb = callback;
  camera_app_data = client_data;
  MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_QVP_ENCODING", 0, 0, 0);
  camera_state = CAMERA_STATE_QVP_ENCODING;

  output2_ack                = 0;
  camera_camsensor_op_mode   = CAMSENSOR_OP_MODE_VIDEO;

  if((camera_preview_dx == 0 || camera_preview_dy == 0) ||
     (camera_dx == 0 || camera_dy == 0))
  {
    MSG_ERROR("Invalid Preview/Encoder Dimensions, Failing in camera_process_start_qvp_encode",0,0,0);

    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate (CAMERA_EXIT_CB_ERROR, (int)CAMERA_ERROR_CONFIG); 
	*status = CAMERA_FAILED;
	   
    if (tcb)
    {
      (void) rex_set_sigs (tcb, CAMERA_SIG);
    }
    return;
  }
  
  if(camera_set_preview_buffers())
  {
    MSG_HIGH("No memory!, Failing camera_process_start_preview",0,0,0);  
   *status = CAMERA_FAILED;
	   
    if (tcb)
    {
      (void) rex_set_sigs (tcb, CAMERA_SIG);
    }
    return;
  }

  camera_set_qvp_encode_buffers();

  event_report(EVENT_CAMERA_RECORD);

  camera_set_preview_headers();
  camera_dsp_command = CAMQDSP_CMD_MAX;

  if (camera_config_vfe())
  {
    encoder_acked = 2;
    if (camera_app_cb)
    {
       MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
      (camera_app_cb)(CAMERA_RSP_CB_SUCCESS, camera_app_data, camera_func, 0);
    }
  }
  else
  {
    *status = CAMERA_FAILED;
  }

  QVP_MSG_VENC_HIGH("Camera State transition to QVP Encoding", 0, 0, 0);

  if (tcb)
  {
    (void) rex_set_sigs (tcb, CAMERA_SIG);
  }

} /* camera_process_start_qvp_encode */
#endif /* FEATURE_QVPHONE */


#ifdef FEATURE_QVPHONE
/*===========================================================================

FUNCTION      CAMERA_PROCESS_STOP_QVP_ENCODE

DESCRIPTION
              Recording is stopped.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_stop_qvp_encode ( rex_tcb_type *tcb )
{
  camera_tcb = tcb;

  QVP_MSG_VENC_HIGH("Stop QVP encode. State transition to stop encoding", 0, 0, 0);

  MSG_CAMERADEBUG("CAMERA_SVCS: Request camera_stopping=STOP_QVP_ENCODING_AT_IDLE_STATE", 0, 0, 0);
#ifdef FEATURE_MDP
    /* De-register the video */
    camera_deregister_video (TRUE);
    /* De-register the overlay layer */
    camera_deregister_layer1();
#endif /* FEATURE_MDP */
  camera_terminate(CAMERA_EXIT_CB_DONE, 0);

} /* camera_process_stop_qvp_encode */
#endif /* FEATURE_QVPHONE */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_ENABLE_FRAME_CALLBACK

DESCRIPTION
              Process enable frame callback.

DEPENDENCIES
  Must be in PERVIEW or RECORDING state
  Use start_preview or start_record callback.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_enable_frame_callback (void)
{
  camera_frame_callback_enabled = TRUE;

  if (camera_app_cb)
  {
    MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
    (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, CAMERA_FUNC_ENABLE_FRAME_CALLBACK, 0);
  }
} /* camera_process_enable_frame_callback */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_DISABLE_FRAME_CALLBACK

DESCRIPTION
              process disable frame callback.

DEPENDENCIES
  Must be in PERVIEW or RECORDING state
  Use start_preview or start_record callback.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_disable_frame_callback (void)
{
  camera_frame_callback_enabled = FALSE;

  if (camera_app_cb)
  {
    MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
    (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, CAMERA_FUNC_DISABLE_FRAME_CALLBACK, 0);
  }
} /* camera_process_disable_frame_callback */

#ifdef FEATURE_MDP
/*===========================================================================

FUNCTION      CAMERA_SVCS_DISPLAY_FRAME_OVERLAY

DESCRIPTION
              Display the created preview frame overlay or fill transparency
              color for the whole layer if clear_overlay_layer is true.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_svcs_display_frame_overlay(boolean clear_overlay_layer)
{
  uint32 * temp = NULL;

  if (clear_overlay_layer)
  {
    /* Prepare to clear the overlay layer */
    mdpImg_frame.width   = 240;
    mdpImg_frame.cwin.x1 = 0;
    mdpImg_frame.cwin.x2 = 239;
    mdpImg_frame.cwin.y1 = 0;
    mdpImg_frame.cwin.y2 = 319;
    mdpImg_frame.lcd_x   = -BREW_OFFSET_X;
    mdpImg_frame.lcd_y   = -BREW_OFFSET_Y;
    mdpImg_frame.tpVal   = camera_transparent_color;
    temp = CAM_Malloc((CAMERA_LCD_WIDTH*CAMERA_LCD_HEIGHT*2) | CAM_ALLOC_NO_ZMEM);
    if (temp)
    {
      camera_svcs_fill_buffer_16((byte *)temp, camera_transparent_color, CAMERA_LCD_WIDTH, CAMERA_LCD_HEIGHT);
    }
    mdpImg_frame.bmy_addr = temp;
  }
  else
  {
    /* turn the overlay to MDP for display */
    mdpImg_frame.bmy_addr = (uint32 *) preview_overlay->imgPtr;

    mdpImg_frame.tpVal   = camera_transparent_color;
    mdpImg_frame.width   = (uint16) preview_overlay->dx;
    mdpImg_frame.cwin.x1 = 0;
    mdpImg_frame.cwin.x2 = (int32) (preview_overlay->dx - 1);

    mdpImg_frame.cwin.y1 = 0;
    mdpImg_frame.cwin.y2 = (int32) (preview_overlay->dy - 1);

    mdpImg_frame.tpVal = camera_transparent_color;

    /* Make sure preview video offset is the same as overlay */
    mdpImg_frame.lcd_x = mdpImg.lcd_x;
    mdpImg_frame.lcd_y = mdpImg.lcd_y;
  }

  if ((mdpImgId_frame == NULL) || (mdp_is_ovimg_valid(mdpImgId_frame) != TRUE))
  {
    /* De-register the overlay layer */
    camera_deregister_layer1 ();
    mdpImgId_frame = mdp_register_ovimg (&mdpImg_frame);
    if(mdpImgId_frame == NULL)
    {
      MSG_ERROR("primary or secondary display overlay list fails", 0, 0, 0);
    }
  }
  else
  {
    if(mdp_replace_ovimg(&mdpImg_frame, mdpImgId_frame) == NULL)
    {
      MSG_ERROR("Fails to replace the specified overlay image", 0, 0, 0);
    }
  }
  if(mdp_update_ovimg (mdpImgId_frame) == NULL)
  {
    MSG_ERROR("Fails to update the pre-registered ovimg from the list", 0, 0, 0);
  }
  if (temp)
  {
    CAM_Free(temp);
  }
} /* camera_svcs_display_frame_overlay */
#endif /* FEATURE_MDP */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_SET_OVERLAY

DESCRIPTION
              Process set overlay

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_set_overlay
(
  camera_cb_f_type callback,
  void            *client_data,
  rex_tcb_type    *tcb
)
{
  camera_cb_type cb;
  camera_ret_code_type ret_val;

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  ret_val = camera_set_icon_and_preview_overlay();
#else /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
  ret_val = camera_svcs_create_preview_overlay(&ui_overlay);
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
  if (ret_val == CAMERA_SUCCESS)
  {
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
    /* For MOVIE mode, we don't want to set camera encode overlay */
    if(preview_mode == CAMERA_PREVIEW_MODE_MOVIE)
    {
      ret_val = camcorder_set_video_eng_overlay(camera_transparent_color, &ui_overlay);
    }else
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */
    {
      ret_val = camera_svcs_set_encode_overlay();
    }
  }

  if (tcb)
  {
    (void) rex_set_sigs (tcb, CAMERA_SIG);
  }

  if (callback)
  {
    cb = camera_svcs_translate_ret_val_to_cb(ret_val);
    (callback)(cb, client_data, CAMERA_FUNC_SET_OVERLAY, (int)preview_overlay);
  }
} /* camera_process_set_overlay */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_CLR_OVERLAY

DESCRIPTION
              Process clear overlay

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_clr_overlay
(
  camera_cb_f_type callback,
  void            *client_data
)
{
  if (callback)
  {
    (callback) (CAMERA_EXIT_CB_DONE, client_data, CAMERA_FUNC_CLR_OVERLAY, 0);
  }
} /* camera_process_clr_overlay */

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
/*===========================================================================

FUNCTION      CAMERA_PROCESS_SET_ICON_ARRAY

DESCRIPTION
              Process set icon array

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_set_icon_array
(
  rex_tcb_type  *tcb,
  ipl_icon_type **icons,
  uint32          dx,
  uint32          dy
)
{
  ipl_image_type *pFrame = NULL;
  uint32 total;
  uint32 counter = 0;
  ipl_status_type status;

  /* Clear current icons */
  camera_clr_icons();

  camera_icons = blt_icons = icons;
  MSG_MED("blt_icons - %d", blt_icons, 0, 0);
  camera_icon_dx = dx;
  camera_icon_dy = dy;

  if (icon_only_overlay)
  {
    CAM_Free (icon_only_overlay->imgPtr);
    CAM_Free (icon_only_overlay);
    icon_only_overlay = NULL;
  }

  total = camera_icon_dx * camera_icon_dy;
  pFrame = CAM_Malloc(sizeof(ipl_image_type));
  if (pFrame == NULL)
    return;

  pFrame->dx = camera_icon_dx;
  pFrame->dy = camera_icon_dy;
  pFrame->cFormat = IPL_RGB565;
  pFrame->imgPtr = CAM_Malloc(total * 2);
  if (pFrame->imgPtr == NULL)
  {
    CAM_Free(pFrame);
    return;
  }

  //Fill in all default transtarent color
  do
  {
    *(uint16*)pFrame->imgPtr = camera_transparent_color;
    pFrame->imgPtr += 2;
    counter++;
  }while (counter < total);
  pFrame->imgPtr -= 2 * total;

  icon_only_overlay = (ipl_image_type *) CAM_Malloc (sizeof (ipl_image_type));

  if (icon_only_overlay == NULL)
  {
    CAM_Free(pFrame->imgPtr);
    CAM_Free(pFrame);
    return;
  }

  icon_only_overlay->imgPtr = CAM_Malloc(total * 2);
  if (icon_only_overlay->imgPtr == NULL)
  {
    CAM_Free(pFrame->imgPtr);
    CAM_Free(pFrame);
    CAM_Free(icon_only_overlay);
    icon_only_overlay = NULL;
    return;
  }

  icon_only_overlay->dx = camera_icon_dx;
  icon_only_overlay->dy = camera_icon_dy;
  icon_only_overlay->cFormat = IPL_RGB565;

  if (ipl_frame_compose(pFrame, camera_icons, camera_transparent_color, icon_only_overlay) != IPL_SUCCESS)
  {
    CAM_Free(pFrame->imgPtr);
    CAM_Free(pFrame);
    CAM_Free (icon_only_overlay->imgPtr);
    CAM_Free(icon_only_overlay);
    icon_only_overlay = NULL;
    return;
  }

  /* Fix overlay frame boundary to prevent color smear */
  status = ipl_fix_boundary(icon_only_overlay, pFrame, camera_transparent_color);
  if (status == IPL_SUCCESS)
  {
    /* Align overlay frame to make sure transparent color at even boundary */
    status = ipl_align_frame(pFrame, icon_only_overlay, camera_transparent_color);
  }

  CAM_Free(pFrame->imgPtr);
  CAM_Free(pFrame);

  if (status == IPL_SUCCESS)
  {
    /* Update preview overlay */
    (void)camera_set_icon_and_preview_overlay();
  }
  else
  {
    CAM_Free (icon_only_overlay->imgPtr);
    CAM_Free(icon_only_overlay);
    icon_only_overlay = NULL;
    return;
  }

  if (tcb)
  {
    (void) rex_set_sigs (tcb, CAMERA_SIG);
  }
} /* camera_process_set_icon_array */
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
/*===========================================================================

FUNCTION      CAMERA_PROCESS_CLR_ICON_ARRAY

DESCRIPTION
              Process clear icon array

DEPENDENCIES

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_EXIT_CB_DONE callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_clr_icon_array(void)
{

  MSG_CAMERADEBUG("CAMERA_SVCS: PROCESS_CLR_ICON_ARRAY", 0, 0, 0);
  /* Clear current icons */
  camera_clr_icons();

  /* Update the preview overlay */
  (void)camera_set_icon_and_preview_overlay();
} /* camera_process_clr_icon_array */
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_SET_REFLECT

DESCRIPTION
              Process set_reflect

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_set_reflect (void)
{
  if (camera_parm_reflect.current_value == (int)CAMERA_MIRROR_REFLECT)
  {
    camera_reflect = QCAMRAW_MIRROR_REFLECT;
    if (camera_reflect_ipl != IPL_YREF)
    {
      /* Need to apply the reflection to the overlay frame only for preview*/
      camera_reflect_ipl = IPL_YREF;
    }
  }
  else if (camera_parm_reflect.current_value == (int)CAMERA_WATER_REFLECT)
  {
    camera_reflect = QCAMRAW_WATER_REFLECT;
    if (camera_reflect_ipl != IPL_XREF)
    {
      camera_reflect_ipl = IPL_XREF;
    }
  }
  else
  {
    camera_reflect = QCAMRAW_NO_REFLECT;
    camera_reflect_ipl = IPL_NOREF;
  }

#ifdef FEATURE_MDP
  camera_svcs_adjust_mdp_for_video();
#endif /* FEATURE_MDP */

  camera_set_preview_headers();
  return;
} /* camera_process_set_reflect */


#ifdef FEATURE_MDP
/*===========================================================================

FUNCTION      CAMERA_SVCS_ADJUST_MDP_FOR_VIDEO

DESCRIPTION
              Adjust MDP for video based on rotation and reflection.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_svcs_adjust_mdp_for_video (void)
{
  /* First, set mdpOp based on preview_rotation. */
  mdpImg.mdpOp   = MDPOP_NOP;
  switch(camera_default_preview_rotation)
  {
    case 90:
      mdpImg.mdpOp   = MDPOP_ROT90;
      break;

    case 180:
      mdpImg.mdpOp   = MDPOP_ROT180;
      break;

    case 270:
      mdpImg.mdpOp   = MDPOP_ROT270;
      break;

    case 0:
    default:
      break;
  }

  if (camera_parm_reflect.current_value == (int)CAMERA_MIRROR_REFLECT)
  {
    if (mdpImg.mdpOp & MDPOP_LR)
    {
      mdpImg.mdpOp = mdpImg.mdpOp & (~MDPOP_LR);
    }
    else
    {
      mdpImg.mdpOp = mdpImg.mdpOp | MDPOP_LR;
    }
  }
  else if (camera_parm_reflect.current_value == (int)CAMERA_WATER_REFLECT)
  {
    if (mdpImg.mdpOp & MDPOP_UD)
    {
      mdpImg.mdpOp = mdpImg.mdpOp & (~MDPOP_UD);
    }
    else
    {
      mdpImg.mdpOp = mdpImg.mdpOp | MDPOP_UD;
    }
  }
#ifdef CUST_EDITION
  mdpImg.mdpOp = mdpImg.mdpOp | MDPOP_QUEUE_COPY;
#endif
} /* camera_svcs_adjust_mdp_for_video */
#endif /* FEATURE_MDP */

#ifdef FEATURE_MDP
/*===========================================================================

FUNCTION      CAMERA_SVCS_ADJUST_MDP_FOR_SNAPSHOT

DESCRIPTION
              Adjust MDP for snapshot based on rotation.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_svcs_adjust_mdp_for_snapshot(uint32 rotation)
{
  /* Thumbnail and main image should not be displayed with reflection.
   * If sensor is in reverse position, the application should apply
   * correct encode rotation. Display will only be based on encode
   * rotation. */
  mdpImg.mdpOp = MDPOP_NOP;

  switch(rotation)
  {
    case 90:
        mdpImg.mdpOp |= MDPOP_ROT90;
        break;
    case 180:
        mdpImg.mdpOp |= MDPOP_ROT180;
        break;
    case 270:
        mdpImg.mdpOp |= MDPOP_ROT270;
        break;
    case 0:
    default:
        break;
  }
}
#endif
/*===========================================================================

FUNCTION      CAMERA_PROCESS_SET_DIMENSIONS

DESCRIPTION
              Process set_dimensions

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_set_dimensions ( camera_cb_f_type callback, void *client_data, rex_tcb_type *tcb)
{
  /* This function will check the overlay frame dimension and clear them if they
   * do not match with the new dimensions
   */

  if (preview_overlay != NULL)
  {
    /* Clear overlay frame if picture size != ui overlay size,
     * preview overlay could be resized, but ui overlay still
     * keeps the original size.
     */
    if (!(((orig_picture_dx == ui_overlay.dy) &&
           (orig_picture_dy == ui_overlay.dx))  ||
         ((orig_picture_dx == ui_overlay.dx) &&
          (orig_picture_dy == ui_overlay.dy)))&&
        !scale_preview_overlay)
    {
      camera_clr_overlays();
    }
    else
    {
      /* Resize the preview overlay frame if necessary */
      if (!(((camera_preview_dx == preview_overlay->dx) &&
             (camera_preview_dy == preview_overlay->dy)) ||
            ((camera_preview_dx == preview_overlay->dy) &&
             (camera_preview_dy == preview_overlay->dx))))
      {
        /* Save a temp copy of preview overlay before resize it */
        ipl_image_type overlay = *preview_overlay;

        preview_overlay->imgPtr = NULL;
        scale_preview_overlay = TRUE;
        (void)camera_svcs_create_preview_overlay(&overlay);
        CAM_Free(overlay.imgPtr);
      }
    }
  }

  if (callback)
  {
    (callback) (CAMERA_EXIT_CB_DONE, client_data, CAMERA_FUNC_SET_DIMENSIONS, (int32)&ui_resolution[0]);
  }

  MSG_HIGH("Camera Set Dimensions calling UI",0,0,0);

  if(tcb)
  {
    (void) rex_set_sigs(tcb, CAMERA_SIG);
  }

} /* camera_process_set_dimensions */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_SET_POSITION

DESCRIPTION
              Process set position

DEPENDENCIES
  Must be in PERVIEW or RECORDING state

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_set_position
(
  camera_cb_f_type callback,
  void            *client_data
)
{
  exif_position_type position;

  position.timestamp = camera_position.timestamp;
  position.latitude  = camera_position.latitude;
  position.longitude = camera_position.longitude;
  position.altitude  = camera_position.altitude;

  exif_set_position (&camera_exif_info, &position);
  if (callback)
  {
    (callback) (CAMERA_EXIT_CB_DONE, client_data, CAMERA_FUNC_SET_POSITION, 0);
  }
}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_SET_EXIF_TAG

DESCRIPTION
              Process set string

DEPENDENCIES
  Must be in PERVIEW or RECORDING state

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_set_exif_tag
(
  camera_cb_f_type callback,
  void            *client_data
)
{
  if (camera_description)
  {
    exif_set_string(&camera_exif_info,
                    EXIF_DESCRIPTION_STRING,
                    camera_description,
                    camera_description_count);

    CAM_Free (camera_description);
    camera_description       = NULL;
    camera_description_count = 0;
  }

  if (camera_user_comment)
  {
    exif_set_string(&camera_exif_info,
                    EXIF_USER_COMMENT_STRING,
                    camera_user_comment,
                    camera_user_comment_count);

    CAM_Free (camera_user_comment);
    camera_user_comment       = NULL;
    camera_user_comment_count = 0;
  }

  if (camera_gps_area_information)
  {
    exif_set_string(&camera_exif_info,
                    EXIF_GPS_AREA_INFORMATION_STRING,
                    camera_gps_area_information,
                    camera_gps_area_information_count);

    CAM_Free (camera_gps_area_information);
    camera_gps_area_information       = NULL;
    camera_gps_area_information_count = 0;
  }

  if (camera_make_information)
  {
    exif_set_string(&camera_exif_info,
                    EXIF_MAKE_STRING,
                    camera_make_information,
                    camera_make_information_count);

    CAM_Free (camera_make_information);
    camera_make_information = NULL;
    camera_make_information_count = 0;
  }

  if(camera_model_information)
  {
    exif_set_string(&camera_exif_info,
                    EXIF_MODEL_STRING,
                    camera_model_information,
                    camera_model_information_count);
    CAM_Free (camera_model_information);
    camera_model_information = NULL;
    camera_model_information_count = 0;
  }

  if (callback)
  {
    (callback) (CAMERA_EXIT_CB_DONE, client_data, CAMERA_FUNC_SET_EXIF_TAG, 0);
  }
} /* camera_process_set_exif_tag */

/*===========================================================================

FUNCTION      CAMERA_TERMINATE

DESCRIPTION
  Bring the VFE to Idle state.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

void camera_terminate (camera_cb_type cb, int32 parm4)
{
#ifndef T_IO_CARD
  if(!camera_stopping)
  {
    camera_stopping = TRUE;
    camera_stopping_cb = cb;
    camera_parm4_value = parm4;
  }
  else
  {
    MSG_ERROR("CAMERA_SVCS: camera terminate called while stopping", 0, 0, 0);
  }
  camera_log(LOG_INFO, (uint32)camera_func, 0, (uint32)TERM_START, __LINE__);
  camera_process_terminate();
#endif /* nT_IO_CARD */
} /* camera_terminate */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_IDLE_STATE_MSG

DESCRIPTION
              Handles QDSP events in graph task context.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_qdsp_idle_state_msg()
{
  MSG_CAMERADEBUG ("CAMERA_SVCS: PROCESS_IDLE_STATE_MSG camera_stopping:%d", camera_stopping, 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if ((camera_dsp_state == DSP_ENABLING) ||
      (camera_dsp_state == DSP_CONFIGURING))
  {
    MSG_ERROR("CAMERA_SVCS: Idle Msg received in camera_dsp_state %d. Ignoring",
              camera_dsp_state, 0, 0);
    return;
  }

  if (camera_stopping)
  {
    camera_process_terminate();
  }
  else if (camera_abort_picture)
  {
    camera_abort_picture = FALSE;
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate (CAMERA_EXIT_CB_ABORT, 0);

    MSG_HIGH ("Abort. State transition to ready", 0, 0, 0);
    event_report (EVENT_CAMERA_ABORT);
  }
  else if ((camera_state == CAMERA_STATE_TAKE_PICTURE) &&
           (camera_take_picture_retry == CAMERA_TAKE_PICTURE_RETRY_IDLING))
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: Take picture retry", 0, 0, 0);
    camera_process_take_single_picture();

    /* Config VFE with the same snapshot params */
    if (VFE_ConfigureSnapshot(&vfeSnapshotConfig) != CAMQDSP_SUCCESS)
    {
      camera_log (LOG_INFO, 0, 0, (uint32)CFG_REJ, __LINE__);
      camera_take_picture_retry = CAMERA_TAKE_PICTURE_RETRY_DONE;
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      camera_terminate (CAMERA_EXIT_CB_ABORT, 0);
      MSG_HIGH ("Abort. State transition to ready", 0, 0, 0);
      event_report (EVENT_CAMERA_ABORT);
    }
    else
    {
      MSG_CAMERADEBUG("CAMERA_SVCS: Take picture retry in progress", 0, 0, 0);
      camera_take_picture_retry = CAMERA_TAKE_PICTURE_RETRY_INP;
    }
  }
  else if ((camera_state == CAMERA_STATE_PREVIEW)      ||
           (camera_state == CAMERA_STATE_TAKE_PICTURE) ||
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
           (camera_state == CAMERA_STATE_QVP_ENCODING) ||
           (camera_state == CAMERA_STATE_RECORDING))
  {
    boolean allow_configuring_vfe = FALSE;
    MSG_CAMERADEBUG("CAMERA_SVCS: Last DSP command issued camera_dsp_command:0x%x", camera_dsp_command, 0, 0);

    if  ( ( camera_dsp_response == CAMQDSP_DOWNLOAD_DONE ) ||
          ( ( camera_dsp_response == CAMQDSP_MSG_IDLE_COMPLETE ) &&
            (camera_dsp_command == CAMQDSP_CMD_IDLE)
          ) ||
          ( camera_vfe_control_and_status.reconfigure_vfe == TRUE )
        )
    {
      allow_configuring_vfe = TRUE;
    }

    if (allow_configuring_vfe)
    {
      if (camera_config_vfe ())
      {
#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
        encoder_acked = 2;
#endif /* defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE */
      }
      else
      {
        MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
        camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
        MSG_HIGH ("Abort. State transition to ready", 0, 0, 0);
        event_report (EVENT_CAMERA_ABORT);
      }
    }
    else
    {
      /* The only way we can land here is that the user is pressing
       * button to reconfigure the VFE very fast as a result of
       * which our task queue is full with reponses from the previous
       * executed command. By adding the above mentioned check we make
       * sure that we our in sync.
       */
      MSG_CAMERADEBUG("CAMERA_SVCS: PROCESS_IDLE_STATE_MSG last_dsp_command %d", camera_dsp_command, 0, 0);
    }
  }


} /* camera_process_qdsp_idle_state_msg */


#ifdef  FEATURE_CAMERA_MOBICAT_PREVIEW
/*===========================================================================

FUNCTION      camera_svcs_start_mobicat_preview

DESCRIPTION
  Asks MPP to start communication with the PC over USB

RETURN VALUE
  TRUE if USB connection is established successfully 
===========================================================================*/
boolean camera_svcs_start_mobicat_preview(uint32 mode, char* ret)
{
  if (camera_state != CAMERA_STATE_PREVIEW)
    return FALSE;

  return mpp_start(
                mode,
                camera_preview_dx, 
                camera_preview_dy, 
                camera_preview_format, 
                camera_parm_zoom.minimum_value,
                camera_parm_zoom.maximum_value,
                NULL,
                NULL
              );
}
#endif //FEATURE_CAMERA_MOBICAT_PREVIEW

/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_OUTPUT1_MSG

DESCRIPTION
  Data along output1 path. Preview uses output1 path for display.
  Handles QDSP events in graph task context. In the snapshot mode,
  thumbnail is configured on output1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void  camera_process_qdsp_output1_msg (Camera_EndOfFrameMessageType *msg)
{
#ifdef FEATURE_PMIC_FLASH_LED_DRIVER
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
#endif  /* FEATURE_PMIC_FLASH_LED_DRIVER */
  CAMQDSP_EndOfFrameMessageType *eof_msg = &msg->eof_msg;
  boolean corrupt;
  camera_frame_type frame;
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */

  qcamrawHeaderType *header = (qcamrawHeaderType *)
    ((uint32)eof_msg->output_Y_Address - qcamraw_header_size);

  if(camera_preview_buffers.buffers[header->buffer_index].status!= BUSY_WITH_VFE)
  {
       if(camera_preview_buffers.buffers[0].status == BUSY_WITH_VFE)
       {
          header->buffer_index = 0;          
       }
       if(camera_preview_buffers.buffers[1].status == BUSY_WITH_VFE)
       {
          header->buffer_index = 1;          
       }
       if(camera_preview_buffers.buffers[2].status == BUSY_WITH_VFE)
       {
          header->buffer_index = 2;          
       }
  }

#ifdef FEATURE_CAMERA_MOBICAT_PREVIEW
  if (mpp_is_ready())
  {
    //Send the current frame to PC if preview is enabled and USB isn't busy
    mpp_send_frame(
      (byte*)eof_msg->output_Y_Address,
      /* header->CbCrSize +  header->YSize, */
      camera_parm_zoom.current_value
      );
  }
#endif //FEATURE_CAMERA_MOBICAT_PREVIEW
  
  if ((camera_state == CAMERA_STATE_READY) ||
      (camera_state == CAMERA_STATE_INIT) ||
      (camera_dsp_state == DSP_ENABLING) ||
      (camera_dsp_state == DSP_CONFIGURING) ||
      (camera_stopping)) 
  {
    MSG_ERROR("CAMERA_SVCS: Output1 Msg received in camera_state %d, camera_dsp_state %d. Ignoring",
              camera_state, camera_dsp_state,0);
    return;
  }

  if(camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT)
  {
    MSG_ERROR("CAMERA_SVCS: Output1 Msg received in camera_camsensor_op_mode %d.Ignoring", camera_camsensor_op_mode ,0,0);
	return;
  } 
  
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/

  camera_preview_set_buffer_status(header->buffer_index, AVAILABLE);
  MSG_CAMERAPBUF("PBUF process_qdsp_output1_msg, SET buffer %d AVAILABLE",
      header->buffer_index, 0, 0);

  MSG_CAMERADEBUG ("output1_ready, AEC/AWB delay count = %d",
      camera_3a_shared_get_aec_awb_delay_count(), 0, 0);
#ifdef FEATURE_VIDEO_ENCODE
  MSG_CAMQDSPIO("start AAA %d",mp4e_get_time_ms(),0,0);
#endif
  camera_output1_frame_rate.count++;

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

  if (vfeStatStruct != NULL) /* Must have an allocated Stat Struct to continue */
  {
#if !defined QDSP_MODULE_VFE25_DEFINED
    /* Average black value that is subtracted from the active image region */
    vfeStatStruct->AvgBlackValue = eof_msg->averageBlackFromActive;
#endif /* nQDSP_MODULE_VFE25_DEFINED */

    /* Num red, blue defective pixels corrected in frame, 8 bits */
    vfeStatStruct->RBDefectPixelCount = eof_msg->redBlueDefectPixelCount;
    /* Num green defective pixels corrected in frame, 8 bits */
    vfeStatStruct->GDefectPixelCount = eof_msg->greenDefectPixelCount;
  }

#ifdef ANY_FLASH
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#else
  if ((camera_state == CAMERA_STATE_TAKE_PICTURE) && camera_fire_flash)
#endif /* FEATURE_CAMERA_BURST_MODE */
  {
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
#ifdef FEATURE_WHITE_LED_FLASH
#ifdef FEATURE_PMIC_FLASH_LED_DRIVER
      errFlag |= pm_flash_led_set_current(PMIC_FLASH_CURRENT_DOWN);
      if (errFlag != PM_ERR_FLAG__SUCCESS)
      {
        // DM message sent
        MSG_ERROR("PMIC error", 0, 0, 0);
      }
#else
     /* turoff white LED */
#ifdef CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH
     gpio_out(CAMIF_FLASH, GPIO_LOW_VALUE);
#else
     BIO_TRISTATE(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_M);
     BIO_OUT(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_OFF_V);
#endif  /* CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH */
#endif /* FEATURE_PMIC_FLASH_LED_DRIVER */
#endif /* FEATURE_WHITE_LED_FLASH */

#ifdef FEATURE_STROBE_FLASH
      strobe_flash_stop_trigger();
      strobe_flash_start_charging();
      camsensor_static_params[camera_asi].aec_enable = TRUE;
#endif /* FEATURE_STROBE_FLASH */
      camera_flash_fired = FALSE;
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
  }
#endif /* ANY_FLASH */

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

#ifdef HANDLE_CAMIF_ERROR
  /* When CAMIF error is detected, we need to abort picture or reconfigure
   * for video. */
  if (camera_dsp_error_status.received_camif_error)
  {
    /* Don't clear received_camif_error yet. It is needed to ignore output 2.
     * Will clear when config is done. */
#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

    camera_log (LOG_INFO, 0, 0, (uint32)QDSP_CRPT, __LINE__);
    if (camera_state == CAMERA_STATE_TAKE_PICTURE)
    {
      MSG_HIGH("CAMIF_ERROR, abort picture.", 0, 0, 0);
      camera_abort_picture = TRUE;
    }
    else
    {
      MSG_HIGH("CAMIF_ERROR, reconfigure VFE.", 0, 0, 0);
      camera_vfe_control_and_status.reconfigure_vfe = TRUE;
    }
  }
#endif /* HANDLE_CAMIF_ERROR */

  if (camsensor_static_params[camera_asi].ignore_camif_error == TRUE)
  {
     corrupt = 0;
  }
  else
  {
    corrupt = (eof_msg->frameCorruption != CAMQDSP_NOT_CORRUPTED);
  }

#ifndef HANDLE_CAMIF_ERROR
  if (corrupt)
  {

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */
    /* Corrupt is associated with CAMIF error.
     * Since we do not handle CAMIF error, only keep count */
    corrupt_count++;
    MSG_ERROR ("Corrupt count = %d", corrupt_count, 0, 0);
  }
#endif /* nHANDLE_CAMIF_ERROR */

  if (camera_vfe_control_and_status.reconfigure_vfe || camera_abort_picture)
  {
    /* To reconfigure or abort, go back to idle first */
    camera_take_images = FALSE;
    camera_dsp_command = CAMQDSP_CMD_IDLE;
    camera_log (LOG_VFEC, 0, 0, (uint32)VFE_IDLE, __LINE__);

#ifdef FEATURE_QVPHONE
    output2_ack  = 0;
#endif /* FEATURE_QVPHONE */
    MSG_CAMERADEBUG("CAMERA_SVCS: Send IDLEMessage to DSP", 0, 0, 0);
    if (VFE_Idle () != CAMQDSP_SUCCESS)
    {
      camera_log (LOG_INFO, 0, 0, (uint32)IDLE_REJ, __LINE__);
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
      MSG_ERROR ("Cannot idle DSP. State transition to ready", 0, 0, 0);
      event_report (EVENT_CAMERA_CANNOT_IDLE_DSP);
    }

    return;
  }

  if (!camera_take_images) return;

  if ((camera_state == CAMERA_STATE_PREVIEW)      ||
      (camera_state == CAMERA_STATE_QVP_ENCODING) ||
      (camera_state == CAMERA_STATE_RECORDING))
  {
    /* except for the 1st time, all subsequent AF requests are sent here */
    if (camera_pending_stats_cmds & PENDING_AF_STATS_CMD_TO_ISSUE)
    {
      camera_af_send_vfe_af_start_command();
      MSG_AFDEBUG("AF Start sent", 0, 0, 0);
    }
#ifdef FEATURE_VIDEO_ENCODE
#ifndef FEATURE_CAMERA_YCBCR_ONLY
    if(camera_state == CAMERA_STATE_RECORDING)
    {
      camcorder_svcs_update_vfe_filter();
    }
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
#endif /* FEATURE_VIDEO_ENCODE */
    if ( (camera_update_vfe) &&
         (camera_vfe_control_and_status.number_of_frames_to_ignore == 0) &&
         (camera_vfe_control_and_status.vfe_idle_msg_failed == FALSE)
       )
    {
      camera_update_vfe  = FALSE;

      if (camera_dsp_command != CAMQDSP_CMD_IDLE)
      {
        camera_dsp_command = CAMQDSP_CMD_UPDATE;
        camera_log (LOG_VFEC, 0, 0, (uint32)VFE_UPDATE, __LINE__);
        MSG_CAMERADEBUG ("Send Update command", 0, 0, 0);
#ifndef FEATURE_CAMERA_YCBCR_ONLY
      if(camsensor_static_params[camera_asi].aec_enable && camsensor_static_params[camera_asi].awb_enable)
      {
       camera_aec_apply_digital_gain_for_preview(&vfeUpdate.colorCorrection);
      }
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
        if (VFE_Update(&vfeUpdate, &camsensor_static_params[camera_asi]) != CAMQDSP_SUCCESS)
        {
          camera_log (LOG_INFO, 0, 0, (uint32)IDLE_REJ, __LINE__);
          MSG_CAMERADEBUG ("CAMERA_SVCS: Cannot issue vfe_update to DSP.", 0, 0, 0);
        }
      }
    }
  }
#ifdef FEATURE_VIDEO_ENCODE
  MSG_CAMQDSPIO("end AAA %d",mp4e_get_time_ms(),0,0);
#endif
  if (camera_state == CAMERA_STATE_PREVIEW)
  {
    if (corrupt)
    {
      /* During preview, ignore corrupt frame */
      camera_log (LOG_VFEC, 0, 0, (uint32)VFE_RLS, __LINE__);
    }
    else if(camera_vfe_control_and_status.number_of_frames_to_ignore != 0)
    {
      camera_vfe_control_and_status.number_of_frames_to_ignore--;
    }
    else if (camera_frame_callback_enabled && camera_app_cb &&
             (camera_preview_buffers.num_buffers_with_display <
              CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_DISPLAY)
            )
    {
      /* Callback is enabled and callback present, do callback if
       * display has less than 2 buffers at the moment.
       */
      frame.format   = camera_preview_format;
      frame.dx       = (uint16) vfeVideoConfig.output1Configuration.width;
      frame.dy       = (uint16) vfeVideoConfig.output1Configuration.height;
      frame.rotation = camera_default_preview_rotation;
      frame.buffer   = (byte *) ((uint32) eof_msg->output_Y_Address - qcamraw_header_size);

      camera_preview_set_buffer_status (header->buffer_index, BUSY_WITH_APP);
      MSG_CAMERAPBUF("PBUF process_qdsp_output1_msg, SET buffer %d BUSY_WITH_APP",
          header->buffer_index, 0, 0);
      (camera_app_cb) (CAMERA_EVT_CB_FRAME, camera_app_data, camera_func,
                       (int32) (&frame));
    }
    /* Send acknowledgement if VFE has less than 3 buffers */
    camera_svcs_ack_vfe_output1();
  }
  else if ((camera_state == CAMERA_STATE_RECORDING) ||
           (camera_state == CAMERA_STATE_QVP_ENCODING))
  {
    if(camera_vfe_control_and_status.number_of_frames_to_ignore != 0)
    {
      camera_vfe_control_and_status.number_of_frames_to_ignore--;
    }
    /* Currently num_bufers_with_display for camcorder can be max of 2,
     * for QVP only one with display at a time.
     */
    else if (camera_frame_callback_enabled && camera_app_cb && (
             ((camera_state == CAMERA_STATE_RECORDING) && (camera_preview_buffers.num_buffers_with_display
                                                           < CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_DISPLAY)) ||
             ((camera_state == CAMERA_STATE_QVP_ENCODING) && (!camera_preview_buffers.num_buffers_with_display)))
            )
    {
      /* Since resize factor for video mode must be 1, preview
       * size is the same as recording size */
      frame.format   = CAMERA_YCBCR_4_2_0;
      frame.dx       = (uint16) camera_preview_dx;
      frame.dy       = (uint16) camera_preview_dy;
      frame.rotation = camera_default_preview_rotation;
      frame.buffer   = (byte *) ((uint32) eof_msg->output_Y_Address - qcamraw_header_size);

      /* Do a callback and wait for application to ack before
       * releasing the frame */
#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
      MSG_MED("application_acked - %d", FALSE, 0, 0);
#endif /* FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE */
      camera_preview_set_buffer_status (header->buffer_index, BUSY_WITH_APP);
      MSG_CAMERAPBUF("PBUF process_qdsp_output1_msg, SET buffer %d BUSY_WITH_APP",
          header->buffer_index, 0, 0);
      (camera_app_cb) (CAMERA_EVT_CB_FRAME, camera_app_data, camera_func,
                       (int32)(&frame));
    }
    /* Send acknowledgement if VFE has less than 3 buffers */
    camera_svcs_ack_vfe_output1();
  }    /* if encoding or QVP_encoding */
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#else
  else if (camera_state == CAMERA_STATE_TAKE_PICTURE)
#endif /* FEATURE_CAMERA_BURST_MODE */
  {
    /* Thumbnail */
    snapshot_frame.format            = camera_snapshot_format;
    snapshot_frame.dx                = (uint16) vfeSnapshotConfig.output1Configuration.width;
    snapshot_frame.dy                = (uint16) vfeSnapshotConfig.output1Configuration.height;
    snapshot_frame.rotation          = camera_parm_encode_rotation.current_value;
    snapshot_frame.thumbnail_image   = thumbnail_luma_buf;
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
    camera_take_picture_status.received_output1 = TRUE;

	MSG_FATAL("camera_take_picture_status.received_output1 == TRUE!!!",0,0,0);
#ifdef FEATURE_CAMERA_HJR
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
#endif /* FEATURE_CAMERA_HJR */

#if (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED)
    if ( (camera_svcs_hjr_enabled() == TRUE) && (HJR_structure.new_numberFrames == 1)
       && (camctrl_tbl.hjr_bayer_filtering_enable == TRUE) )
    {
      if (camera_take_picture_status.received_output2 == TRUE)
      {
        if (camera_take_picture_status.abort)
        {
          camera_svcs_queue_call_to_terminate();
        }
        else
        {
          /* Free the RAW memory since it is not required any more */
          CAM_Free (camera_svcs_raw_snapshot_copy);
          camera_svcs_raw_snapshot_copy = NULL;
          clk_dereg(&camera_wait_camif_done_clk);
          camera_handle_frames_for_takepicture();
        }
      }
    }
    else
#endif /* (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED) */
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
    if ((camera_state == CAMERA_STATE_TAKE_PICTURE) &&
        (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT) &&
        (camera_take_picture_status.received_camif_done == TRUE) &&
        (camera_take_picture_status.received_output2 == TRUE)
       )
    {
      if (camera_take_picture_status.abort)
      {
        camera_svcs_queue_call_to_terminate();
      }
      else
      {
        clk_dereg(&camera_wait_camif_done_clk);
        camera_handle_frames_for_takepicture();
      }
    }
  }
} /*  camera_process_qdsp_output1_msg */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_OUTPUT2_MSG

DESCRIPTION
              Handles QDSP events in graph task context.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_qdsp_output2_msg (Camera_EndOfFrameMessageType *msg)
{
  CAMQDSP_EndOfFrameMessageType *eof_msg = &msg->eof_msg;
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */
  boolean corrupt = (eof_msg->frameCorruption != CAMQDSP_NOT_CORRUPTED);

  if ((camera_state == CAMERA_STATE_READY) ||
      (camera_state == CAMERA_STATE_INIT) ||
      (camera_dsp_state == DSP_ENABLING) ||
      (camera_dsp_state == DSP_CONFIGURING) ||
      (camera_stopping))
  {
    MSG_ERROR("CAMERA_SVCS: Output2 Msg received in camera_state %d, camera_dsp_state %d. Ignoring",
              camera_state, camera_dsp_state, 0);
    return;
  }

  if (camera_vfe_control_and_status.reconfigure_vfe)
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: VFE will be reconfigured: Ignore output2 frame", 0, 0, 0);
#ifdef FEATURE_VIDEO_ENCODE
    if(camera_state == CAMERA_STATE_RECORDING)
    {
      /* video encoder needs this to properly handle vfe-resets */
      output2enc_ack2_criteria(SPECIAL_EFFECTS_DONE2);
    }
#endif /* FEATURE_VIDEO_ENCODE */
    return;
  }

#ifdef HANDLE_CAMIF_ERROR
  /* If CAMIF_ERROR was reported, the output1 side will reconfigure VFE */
  if (camera_dsp_error_status.received_camif_error)
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: VFE reported CAMIF_ERROR: Ignore output2 frame", 0, 0, 0);
    return;
  }
#endif /* HANDLE_CAMIF_ERROR */

#ifdef FEATURE_QVPHONE
  if(camera_qvp_enabled)
  {
    ackOutput2.eofAck2[0].Y_Address    = eof_msg->output_Y_Address;
    ackOutput2.eofAck2[0].CbCr_Address = eof_msg->output_CbCr_Address;
  }
#endif /* FEATURE_QVPHONE */

  MSG_CAMERADEBUG("output2_ready", 0, 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if (camsensor_static_params[camera_asi].ignore_camif_error == TRUE)
  {
     corrupt = 0;
  }

  if (corrupt)
  {
#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */
  }

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#else
  if (camera_state == CAMERA_STATE_TAKE_PICTURE)
#endif /* FEATURE_CAMERA_BURST_MODE */
  {
    camera_take_picture_status.received_output2 = TRUE;
    camera_take_picture_status.Y_Address    = eof_msg->output_Y_Address;
    camera_take_picture_status.CbCr_Address = eof_msg->output_CbCr_Address;;
    if (camera_state == CAMERA_STATE_TAKE_PICTURE)
    {
      if (
           ( (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT) &&
             (camera_take_picture_status.received_output1 == TRUE) &&
             (camera_take_picture_status.received_camif_done == TRUE)
           ) ||
           ( ((camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT) 
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE     
#error code not present
#endif /* FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/                          
              )&&
             (camera_take_picture_status.received_camif_done == TRUE))
         )
      {
        /* Stop the wait_camif_done clock now we have all */
        clk_dereg(&camera_wait_camif_done_clk);
      }
    }

    if((camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT)
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/    		
      )
    {
      /* Raw Image */
      switch (vfeRawConfig.rawOutputOption)
      {
        case VFE_10_BIT_DIRECT:
          snapshot_frame.format       = CAMERA_BAYER_10BIT;
          break;
        case VFE_8_BIT_DIRECT:
          if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_BAYER)
          {
            snapshot_frame.format       = CAMERA_BAYER_8BIT;
          }
          else
          {
            snapshot_frame.format       = CAMERA_YCBCR_4_2_2;
          }
          break;
        default:
          break;
      }
      snapshot_frame.captured_dx  = (uint16) vfeRawConfig.camif_window_width_config.lastPixel - vfeRawConfig.camif_window_width_config.firstPixel + 1;
      snapshot_frame.captured_dy  = (uint16) vfeRawConfig.camif_window_height_config.lastLine - vfeRawConfig.camif_window_height_config.firstLine + 1 ;
      /* Padding is not needed here because raw_snapshot_buf does not use 32-byte aligned Y_Address */
      snapshot_frame.buffer       = (byte *) ((uint32) eof_msg->output_Y_Address - qcamraw_header_size);
    }
    else
    {
      /* main image */
      snapshot_frame.format       = camera_snapshot_format;
      snapshot_frame.captured_dx  = (uint16) vfeSnapshotConfig.output2Configuration.width;
      snapshot_frame.captured_dy  = (uint16) vfeSnapshotConfig.output2Configuration.height;

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif
  {
        snapshot_frame.buffer   = (byte *) (snapshot_luma_buf);
  }
 }

#if (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED)
    if ( (camera_svcs_hjr_enabled() == TRUE) && (HJR_structure.new_numberFrames == 1)
       && (camctrl_tbl.hjr_bayer_filtering_enable == TRUE) )
    {
      if (camera_take_picture_status.received_output1 == TRUE)
      {
        if (camera_take_picture_status.abort)
        {
          camera_svcs_queue_call_to_terminate();
        }
        else
        {
          /* Free the RAW memory since it is not required any more */
          CAM_Free (camera_svcs_raw_snapshot_copy);
          camera_svcs_raw_snapshot_copy = NULL;
          camera_handle_frames_for_takepicture();
        }
      }
    }
    else
#endif /* (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED) */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
    {
      if (
          ( (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT) &&
            (camera_take_picture_status.received_output1 == TRUE) &&
            (camera_take_picture_status.received_camif_done == TRUE)
          ) ||
          (( (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT) 
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE     
#error code not present
#endif /* FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/                                    
           )&&
           (camera_take_picture_status.received_camif_done == TRUE))
         )
      {
        if (camera_take_picture_status.abort)
        {
          camera_svcs_queue_call_to_terminate();
        }
        else
        {
          camera_handle_frames_for_takepicture();
        }
      }

    }
  }
#ifdef FEATURE_QVPHONE
  else if(camera_state == CAMERA_STATE_QVP_ENCODING)
  {
    //Need to change here if offline encoding is enabled
    if (output2_ack != CAMERA_DELAYED_ENCODE_FRAME_SIZE)
    {
      camera_dsp_command = CAMQDSP_CMD_END_OF_FRAME_ACK2;
      (void) VFE_EOF_Ack2 (ackOutput2);
      ackOutput2.eofAck2[0].Y_Address    = NULL;
      output2_ack++;
    }
  }
#endif /* FEATURE_QVPHONE */
  else if (camera_state == CAMERA_STATE_PREVIEW)
  {
    /* Fatal error when output2 frame received during preview.
    ** Ignore frame, do not ack to prevent VFE from using the invalid address
    ** in ACK to dump the subsequent frames and potentially corrupt memory
    */
    MSG_CAMERADEBUG("CAMERA_SVCS: ERROR Received output2 frames during preview.",0,0,0);
  }
#ifdef FEATURE_VIDEO_ENCODE
  else if ( camera_state == CAMERA_STATE_RECORDING )
  {
    camcorder_process_qdsp_output2_msg(msg);
  }   /* if camera_state = CAMERA_STATE_RECORDING */
#endif /* FEATURE_VIDEO_ENCODE */
  else
  {
    MSG_ERROR("cannot be here",0,0,0);
  }
} /* camera_process_qdsp_output2_msg */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_DETECT_ERROR_MSG

DESCRIPTION
              Handles QDSP events in graph task context.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_qdsp_detect_error_msg (CAMQDSP_ResponseMsgType dsp_response)
{
  /* This is an indication that the firmware is not able to keep up with
   * the camera data rate, either because the DSP cannot handle the
   * amount of data or the DME is too slow
   */

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  MSG_CAMERADEBUG ("CAMERA_SVCS: QDSP Error Message Handler %d", dsp_response, 0, 0);

  switch (dsp_response)
  {
    case CAMQDSP_MSG_CAMIF_ERROR:
      break;

    case CAMQDSP_MSG_AHB_OVERFLOW_ERROR:
      if ((camera_state == CAMERA_STATE_TAKE_PICTURE)
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
         )
      {
        if (camera_take_picture_retry == CAMERA_TAKE_PICTURE_RETRY_NONE)
        {
          camera_take_picture_retry = CAMERA_TAKE_PICTURE_RETRY_NEEDED;
        }
      }
    case CAMQDSP_MSG_MICRO_DMA_OVERFLOW_ERROR:
#ifdef FEATURE_VIDEO_ENCODE
      /* If AHB/DMA OVERFLOW errors occur during camcorder stop record, then
       * do not initiate another stop sequence. 
       */
      if (camera_state == CAMERA_STATE_RECORDING && !camera_stopping_record)
      {
        camera_log (LOG_INFO, 0, 0, (uint32)DET_ERR, __LINE__);
        MSG_ERROR ("VFE fatal error. State transition to ready", 0, 0, 0);
        /* During recording, setting stop_requested flag would ensure clean up
         * of video engine and camera_process_terminate
         */
        camera_video_engine_stop_requested = TRUE;
        event_report (EVENT_CAMERA_DSP_FATAL);
      }
#endif /* FEATURE_VIDEO_ENCODE */
      break;

    default:
      break;
  }

} /* camera_process_qdsp_detect_error_msg */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_CONFIG_DONE_MSG

DESCRIPTION
              Handles QDSP events in graph task context.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_qdsp_config_done_msg (CAMQDSP_VideoEncStateType * vidEncState)
{
#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  /* Reset camif_error flag after config is done. */
  camera_dsp_error_status.received_camif_error = FALSE;
  MSG_LOW ("Config done.", 0, 0, 0);

  camera_dsp_state = DSP_CONFIGURED;

  if ( camera_stopping )
  {
    /* Return to shutdown */
    camera_process_terminate();
    return;
  }

  /* Register the camera client with clock regime for requesting the 
    resources from clock regime as per the DCVS Design for SC2x*/

  #ifdef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III 
  if(camera_state == CAMERA_STATE_PREVIEW)
  {
    camera_svcs_optimize_resources ();
  }
  #endif /* FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III */


  if ((camera_state == CAMERA_STATE_PREVIEW)      ||
      (camera_state == CAMERA_STATE_TAKE_PICTURE) ||
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
      (camera_state == CAMERA_STATE_QVP_ENCODING))
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: Send Initiate Capture command to DSP", 0, 0, 0);

    if (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT)
    {
      camera_snapshot_timing.raw_config_end = (uint32)timetick_get_ms();
      camera_snapshot_timing.raw_capture_start = (uint32)timetick_get_ms() + camera_capture_delay;
    }
    else if (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT)
    {
#if (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED)
      if ( (camera_svcs_hjr_enabled() == TRUE) && (HJR_structure.new_numberFrames == 1)
         && (camctrl_tbl.hjr_bayer_filtering_enable == TRUE) )
      {
        camera_snapshot_timing.offline_snapshot_config_end = (uint32)timetick_get_ms();
        camera_snapshot_timing.offline_snapshot_capture_start = camera_snapshot_timing.offline_snapshot_config_end;
        camera_capture_delay = 0;
      }
      else
#endif /* (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED) */
      {
        camera_snapshot_timing.snapshot_config_end = (uint32)timetick_get_ms();
        camera_snapshot_timing.snapshot_capture_start = (uint32)timetick_get_ms() + camera_capture_delay;
      }
    }
    /* do a timed wait before capture, if needed */
    /* Sometimes this time wait is useful for the sensor configuration to be valid */
    /* This is also used in strobe flash where discard first frame is needed */
    if (camera_capture_delay)
    {
      camera_timed_wait(camera_capture_delay);
    }
    camera_initiate_capture ();
  }
#ifdef FEATURE_VIDEO_ENCODE
  else if (camera_state == CAMERA_STATE_RECORDING)
  {
    camcorder_process_qdsp_config_done_msg(vidEncState);
  }
#endif /* FEATURE_VIDEO_ENCODE */
} /* camera_process_qdsp_config_done_msg */




/*===========================================================================

FUNCTION      CAMERA_INITIATE_CAPTURE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

void camera_initiate_capture (void)
{

#ifdef FEATURE_PMIC_FLASH_LED_DRIVER
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
#endif  /* FEATURE_PMIC_FLASH_LED_DRIVER */

  camera_log (LOG_VFEC, 0, 0, (uint32)VFE_CAP, __LINE__);
  camera_output1_frame_rate.count = 0;
  camera_output1_frame_rate.start_ms = (uint32) timetick_get_ms();
  camera_dsp_command = CAMQDSP_CMD_CAPTURE;

#ifdef FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK
  camera_svcs_set_vfe_clk (VFE_MODE_CAPTURE);
#endif /* FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK */

  if (VFE_Capture () != CAMQDSP_SUCCESS)
  {
    camera_log (LOG_INFO, 0, 0, (uint32)CAP_REJ, __LINE__);
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
    MSG_ERROR ("Cannot capture. State transition to ready", 0, 0, 0);
    event_report (EVENT_CAMERA_CANNOT_CAPTURE);
  }
  else
  {
    camera_take_images = TRUE;
#ifdef FEATURE_VIDEO_ENCODE
    camera_video_engine_start_requested = FALSE;
#endif
    camera_output1_frame_rate.waiting_for_first_output1_frame = TRUE;
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#else
    if (camera_state == CAMERA_STATE_TAKE_PICTURE)
#endif /* FEATURE_CAMERA_BURST_MODE */
    {
#ifdef FEATURE_WHITE_LED_FLASH
      if ((camera_fire_flash) && (camera_flash_fired == FALSE))
      {
        camera_flash_fired = TRUE;
        /* turnon_white_led */
#ifdef FEATURE_PMIC_FLASH_LED_DRIVER
        errFlag |= pm_flash_led_set_current(PMIC_FLASH_CURRENT_HIGH);
        if (errFlag != PM_ERR_FLAG__SUCCESS)
        {
             // DM message sent
             MSG_ERROR("PMIC error", 0, 0, 0);
        }
#else
#ifdef CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH
        gpio_out(CAMIF_FLASH, GPIO_HIGH_VALUE);
#else
        BIO_TRISTATE(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_M);
        BIO_OUT(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_ON_V);
#endif /* CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH */
#endif /* FEATURE_PMIC_FLASH_LED_DRIVER */
      }
#endif /* FEATURE_WHITE_LED_FLASH */
    }
  }

  /* Turn on AF stats as soon as vfe is in video mode */
  if (camera_pending_stats_cmds & PENDING_AF_STATS_CMD_TO_ISSUE)
  {
    camera_af_send_vfe_af_start_command();
    MSG_AFDEBUG("AF Start sent", 0, 0, 0);
  }
} /* camera_initiate_capture */



/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_ILLEGAL_COMMAND_MSG

DESCRIPTION
              Handles QDSP events in graph task context.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_qdsp_illegal_cmd_msg (void)
{
  MSG_LOW ("Illegal command.", 0, 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  illegal_count++;
  MSG_ERROR ("Illegal DSP req. - %d", illegal_count, 0, 0);
  event_report (EVENT_CAMERA_DSP_REQ_ILLEGAL);

  /* If we were trying to configure the VFE when we sent illegal command the
   * configuration is now stopped. Need to update camera_dsp_state */
  if (camera_dsp_state == DSP_CONFIGURING)
  {
    camera_dsp_state = DSP_ENABLED;
  }

  camera_terminate(CAMERA_EXIT_CB_FAILED, 0);
} /* camera_process_qdsp_illegal_cmd_msg */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_UPDATE_COMMAND_MSG

DESCRIPTION
              Handles QDSP events in graph task context.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_qdsp_update_complete_msg (void)
{

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if ((camera_state == CAMERA_STATE_READY) ||
      (camera_state == CAMERA_STATE_INIT) ||
      (camera_dsp_state == DSP_ENABLING) ||
      (camera_dsp_state == DSP_CONFIGURING) ||
      (camera_stopping))
  {
    MSG_ERROR("CAMERA_SVCS: Update Msg received in camera_state %d, camera_dsp_state. Ignoring",
              camera_state, camera_dsp_state, 0);
    return;
  }

  event_report(EVENT_VFE_MSG_UPDATE_COMPLETE);

  /* Wait for the vfe update parameters to settle before sending another
   * stats request.
   */
  MSG_CAMERADEBUG ("Delay AEC/AWB for update complete, %d frames",
      UPDATE_CMPLT_STATS_DELAY, 0, 0);
  camera_3a_shared_set_aec_awb_delay_count (UPDATE_CMPLT_STATS_DELAY, AEC_AWB_DELAY_UPDATE);


} /* camera_process_qdsp_update_complete_msg */


#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_AE_AWB_STATS_COMPLETE_MSG

DESCRIPTION
              Processes Auto Exposure and Auto White Balance stats
              comming from VFE.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_qdsp_ae_awb_stats_complete_msg (CAMQDSP_AE_AWBStatsMessageType * msg)
{
  /* 3A */
  MSG_CAMERADEBUG ("Received AEC/AWB Stats Msg, %d", camera_3a_shared_get_aec_awb_delay_count(), 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if ((camera_state == CAMERA_STATE_READY) ||
      (camera_state == CAMERA_STATE_INIT) ||
      (camera_dsp_state == DSP_ENABLING) ||
      (camera_dsp_state == DSP_CONFIGURING) ||
      (camera_stopping))
  {
    MSG_ERROR("CAMERA_SVCS: AEC/AWB Stats Msg received in camera_state %d, camera_dsp_state %d. Ignoring",
              camera_state, camera_dsp_state, 0);
    return;
  }

 #if defined FEATURE_CAMERA_BURST_MODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
 #endif /* FEATURE_CAMERA_BURST_MODE && FEATURE_CAMERA_AEC_DURING_MULTISHOT */
  {

  /* Skip the stats processing, if the stats are not valid */
  if (camera_aec_awb_ready () == TRUE)
  {
    camera_3a_parse_and_implement_aec_awb( msg->pixelsUsedForExposure,
                                       &(camsensor_static_params[camera_asi]) );

    /* Wait for the new exposure setting on sensor to settle before
     * sending new stats request.
     */
    MSG_CAMERADEBUG ("Delay AEC/AWB for after processing, %d frames",
        AEC_STATS_PROC_DELAY, 0, 0);
    camera_3a_shared_set_aec_awb_delay_count (AEC_STATS_PROC_DELAY, AEC_AWB_DELAY_UPDATE);

    /* For AF's sake, see if AE has settled */
    camera_af_focus_if_wait_for_aec();
  }
  }


} /* camera_process_qdsp_ae_awb_stats_complete_msg */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_HIST_STATS_COMPLETE_MSG

DESCRIPTION
              Processes histogram stats comming from the VFE.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_qdsp_hist_stats_complete_msg (void)
{
   /* 3A */
   MSG_LOW ("Hist Stats Complete.", 0, 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */


}

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
/*===========================================================================

FUNCTION      CAMERA_CLR_ICONS

DESCRIPTION
              Clear all overlays.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_clr_icons(void)
{
  if (camera_icons)
  {
    ipl_icon_type **ptr = camera_icons;

    /* Clean up current icons */
    while (*ptr)
    {
      CAM_Free((*ptr)->imgPtr);
      CAM_Free (*ptr);
      ptr++;
    }
    CAM_Free(camera_icons);
    camera_icons = NULL;
  }
  camera_icon_dx = 0;
  camera_icon_dy = 0;

  if ( icon_only_overlay )
  {
    CAM_Free (icon_only_overlay->imgPtr);
    CAM_Free(icon_only_overlay);
    icon_only_overlay = NULL;
  }
} /* camera_clr_icons */
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */


/*===========================================================================

FUNCTION      CAMERA_SET_PREVIEW_BUFFERS

DESCRIPTION
              Set preview (and video mode) buffers.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

boolean camera_set_preview_buffers(void)
{
  boolean malloc_fail = FALSE;
  uint32 current_buffer;
#ifdef FEATURE_VIDEO_ENCODE
  if (camera_state == CAMERA_STATE_RECORDING)
  {
    camcorder_process_alloc_memory(camera_encode_buffer_size);
  }
#endif /* FEATURE_VIDEO_ENCODE */

  camera_free_preview_buffers();
  camera_malloc_preview_buffers();

  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    malloc_fail = (malloc_fail || !camera_preview_buffers.buffers[current_buffer].buf_ptr);
  }
  if (malloc_fail)
  {
    camera_free_preview_buffers();
#ifdef FEATURE_VIDEO_ENCODE
    if (camera_state == CAMERA_STATE_RECORDING)
    {
      (void) VideoBuffersDeallocate(&encodeBufferHandle);
      memset(&encodeBufferHandle,0,sizeof(encodeBufferHandle));
    }
#endif /* FEATURE_VIDEO_ENCODE */
    camera_no_memory ();
  }
  return malloc_fail;
} /* camera_set_preview_buffers */

/*===========================================================================

FUNCTION      CAMERA_SVCS_SETPARAM_VFE_VIDEO

DESCRIPTION
              Configures VFE.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean camera_svcs_setparam_vfe_video (void)
{
  uint8           chroma_factor = 1;
  uint32          burst_factor1, burst_factor2;
  /* Compute the VFE clipping parameters corresponding to
   * current zooming level
   */
  if (camsensor_static_params[camera_asi].sensor_features.zoom == FALSE)
  {
    camera_calculate_vfe_clipping ();
  }
  vfeUpdate.clip = vfeVideoConfig.clip;


  /* Copy over WB settings for first frame              */
  if (((int32) vfeVideoConfig.colorConversion.enable == (int)VFE_Enable) &&
      ((int32) vfeUpdate.colorConversion.enable == (int)VFE_Enable)
     )
  {
    vfeVideoConfig.colorConversion = vfeUpdate.colorConversion;
  }
  else
  {
    vfeUpdate.colorConversion  = vfeVideoConfig.colorConversion;
  }

  if (((int32) vfeVideoConfig.colorCorrection.enable == (int)VFE_Enable) &&
      ((int32) vfeUpdate.colorCorrection.enable == (int)VFE_Enable)
     )
  {
    vfeVideoConfig.colorCorrection = vfeUpdate.colorCorrection;
  }
  else
  {
    vfeUpdate.colorCorrection = vfeVideoConfig.colorCorrection;
  }

  /* Copy over the parameters to vfeVideoConfig */

#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */

  /* Determine the VFE output1 format */
  vfeVideoConfig.output1Configuration.format = VFE_O1YCbCr420_LinePack;

  /* AHB burst length and burst number parameters */
  vfeVideoConfig.output1_AHB_Configuration.numBurstForYBuffers = CAMERA_AHB_BURST_NUM;
  vfeVideoConfig.output1_AHB_Configuration.burstLenForYBuffers = CAMERA_AHB_BURST_LEN;
  vfeVideoConfig.output1_AHB_Configuration.numBurstForCbCrBuffers = CAMERA_AHB_BURST_NUM;
  vfeVideoConfig.output1_AHB_Configuration.burstLenForCbCrBuffers = CAMERA_AHB_BURST_LEN;

  vfeVideoConfig.output2_AHB_Configuration.numBurstForYBuffers = CAMERA_AHB_BURST_NUM;
  vfeVideoConfig.output2_AHB_Configuration.burstLenForYBuffers = CAMERA_AHB_BURST_LEN;
  vfeVideoConfig.output2_AHB_Configuration.numBurstForCbCrBuffers = CAMERA_AHB_BURST_NUM;
  vfeVideoConfig.output2_AHB_Configuration.burstLenForCbCrBuffers = CAMERA_AHB_BURST_LEN;

  if ((int32) vfeVideoConfig.output2Configuration.output2Enable == (int)VFE_Enable)
  {
    vfeVideoConfig.output2Configuration.format = VFE_O2YCbCr420_LinePack;
  }

  /* Compute the AHB parameters output1. */

  /* Y - Component */
  camera_calculate_ahb_burst_factors (&burst_factor1, &burst_factor2, (camera_calculate_ceiling_number((vfeVideoConfig.output1Configuration.width * vfeVideoConfig.output1Configuration.height), (sizeof (uint32) * vfeVideoConfig.output1_AHB_Configuration.burstLenForYBuffers))) / (sizeof (uint32) * vfeVideoConfig.output1_AHB_Configuration.burstLenForYBuffers));

  vfeVideoConfig.output1_AHB_Configuration.burstFactor1ForYBuffers = burst_factor1;
  vfeVideoConfig.output1_AHB_Configuration.burstFactor2ForYBuffers = burst_factor2;

  /* In 4:2:0 mode, the size of chroma component is half the luma component.
   * Factor by 2
   */
  if (vfeVideoConfig.output1Configuration.format == VFE_O1YCbCr420_LinePack)
  {
    chroma_factor = 2;
  }
  else
  {
    chroma_factor = 1;
  }

  /* CbCr - Component */
  camera_calculate_ahb_burst_factors (&burst_factor1, &burst_factor2, (camera_calculate_ceiling_number((vfeVideoConfig.output1Configuration.width * vfeVideoConfig.output1Configuration.height), (sizeof (uint32) * vfeVideoConfig.output1_AHB_Configuration.burstLenForCbCrBuffers * chroma_factor))) / (sizeof (uint32) * vfeVideoConfig.output1_AHB_Configuration.burstLenForCbCrBuffers * chroma_factor));

  vfeVideoConfig.output1_AHB_Configuration.burstFactor1ForCbCrBuffers = burst_factor1;
  vfeVideoConfig.output1_AHB_Configuration.burstFactor2ForCbCrBuffers = burst_factor2;

  /* Compute the AHB parameters output2. */

  /* Y - Component */
  camera_calculate_ahb_burst_factors (&burst_factor1, &burst_factor2, (camera_calculate_ceiling_number((vfeVideoConfig.output2Configuration.width * vfeVideoConfig.output2Configuration.height), (sizeof (uint32) * vfeVideoConfig.output2_AHB_Configuration.burstLenForYBuffers))) / (sizeof (uint32) * vfeVideoConfig.output2_AHB_Configuration.burstLenForYBuffers));

  vfeVideoConfig.output2_AHB_Configuration.burstFactor1ForYBuffers = burst_factor1;
  vfeVideoConfig.output2_AHB_Configuration.burstFactor2ForYBuffers = burst_factor2;

  /* In 4:2:0 mode, the size of chroma component is half the luma component.
   * Factor by 2
   */
  if (vfeVideoConfig.output2Configuration.format == VFE_O2YCbCr420_LinePack)
  {
    chroma_factor = 2;
  }
  else
  {
    chroma_factor = 1;
  }

  /* CbCr - Component */
  camera_calculate_ahb_burst_factors (&burst_factor1, &burst_factor2, (camera_calculate_ceiling_number((vfeVideoConfig.output2Configuration.width * vfeVideoConfig.output2Configuration.height), (sizeof (uint32) * vfeVideoConfig.output2_AHB_Configuration.burstLenForCbCrBuffers * chroma_factor))) / (sizeof (uint32) * vfeVideoConfig.output2_AHB_Configuration.burstLenForCbCrBuffers * chroma_factor));

  vfeVideoConfig.output2_AHB_Configuration.burstFactor1ForCbCrBuffers = burst_factor1;
  vfeVideoConfig.output2_AHB_Configuration.burstFactor2ForCbCrBuffers = burst_factor2;


#ifndef FEATURE_CAMERA_YCBCR_ONLY
  /* Update the gamma table if set by the sensor
   */
  if ((int32) vfeVideoConfig.gammaCorrection.enable == (int)VFE_Enable)
  {
    int32 current_special_effect = camera_parm_special_effects.current_value;
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
    vfe_put_gamma_table(current_special_effect,
        camera_parm_contrast.current_value, camera_camsensor_op_mode);
  }
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

  /* Reset the preview buffers */
  camera_reset_preview_buffers();

  /* Assign buffers to VFE */
  vfeVideoConfig.output1Buffer.Y_OutputTripleBuffer0   =
    (VFE_AddressType) qcamrawGetDataYPtr    (camera_preview_buffers.buffers[0].buf_ptr);
  vfeVideoConfig.output1Buffer.CbCrOutputTripleBuffer0 =
    (VFE_AddressType) qcamrawGetDataCbCrPtr (camera_preview_buffers.buffers[0].buf_ptr);
  camera_preview_set_buffer_status(0, BUSY_WITH_VFE);
  MSG_CAMERAPBUF("PBUF setparam_vfe_video, SET buffer 0 BUSY_WITH_VFE", 0, 0, 0);
  vfeVideoConfig.output1Buffer.Y_OutputTripleBuffer1   =
    (VFE_AddressType) qcamrawGetDataYPtr    (camera_preview_buffers.buffers[1].buf_ptr);
  vfeVideoConfig.output1Buffer.CbCrOutputTripleBuffer1 =
    (VFE_AddressType) qcamrawGetDataCbCrPtr (camera_preview_buffers.buffers[1].buf_ptr);
  camera_preview_set_buffer_status(1, BUSY_WITH_VFE);
  MSG_CAMERAPBUF("PBUF setparam_vfe_video, SET buffer 1 BUSY_WITH_VFE", 0, 0, 0);
  vfeVideoConfig.output1Buffer.Y_OutputTripleBuffer2   =
    (VFE_AddressType) qcamrawGetDataYPtr    (camera_preview_buffers.buffers[0].buf_ptr);
  vfeVideoConfig.output1Buffer.CbCrOutputTripleBuffer2 =
    (VFE_AddressType) qcamrawGetDataCbCrPtr (camera_preview_buffers.buffers[0].buf_ptr);
  //camera_preview_set_buffer_status(0, BUSY_WITH_VFE);
  MSG_CAMERAPBUF("PBUF setparam_vfe_video, SET buffer 2 BUSY_WITH_VFE", 0, 0, 0);

  vfeUpdate.defectPixelCorrection = vfeVideoConfig.defectPixelCorrection;
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
  vfeUpdate.f3x3LumaFilter        = vfeVideoConfig.f3x3LumaFilter;
#endif /* QDSP_MODULE_VFE25_DEFINED */
  vfeUpdate.asf_cfg               = vfeVideoConfig.asf_cfg;

  /* For YCbCr sensors */
  if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR)
  {
    vfeVideoConfig.inputConfiguration.YCbCrCositing = VFE_CHROMA_COSITED;
  }

  /* Congif command to VFE */
  camera_dsp_command = CAMQDSP_CMD_CONFIG_VIDEO;

  return TRUE;
} /* camera_svcs_setparam_vfe_video */


/*===========================================================================

FUNCTION      CAMERA_SVCS_SETPARAM_VFE_RAW_SNAPSHOT

DESCRIPTION
              Configures VFE.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

boolean camera_svcs_setparam_vfe_raw_snapshot ()
{
  uint32          raw_image_size = 0;
  uint8           number_of_fragments = 0;
  qcamrawDataType qcamraw_format = QCAMRAW_H2V2;
  uint32          burst_factor1, burst_factor2;
  uint32          hjr_extra_buffer_for_10_bit_unpacking = 0;
  uint32          hjr_extra_buffer_for_bayer_filtering = 0;

  /* Compute the AHB parameters */

  /* Burst length and Burst number parameters */
  vfeRawConfig.burstLenForBuffers = CAMERA_RAW_SNAPSHOT_AHB_BURST_LEN;
  vfeRawConfig.numBurstForBuffers = CAMERA_AHB_BURST_NUM;

  raw_image_size = ((vfeRawConfig.camif_window_width_config.lastPixel - vfeRawConfig.camif_window_width_config.firstPixel + 1) * (vfeRawConfig.camif_window_height_config.lastLine - vfeRawConfig.camif_window_height_config.firstLine + 1))  ;

  /* Output2 */
  switch (vfeRawConfig.rawOutputOption)
  {
    case VFE_8_BIT_DIRECT:
      qcamraw_format                 = QCAMRAW_BAYER_8BIT;
#ifdef FEATURE_CAMERA_HJR
      if (camera_svcs_hjr_enabled() == TRUE)
      {
        /* bayer filter needs to operate on the raw data
         * The buffer requirement is based on the picture
         * size. The buffer is 3 times the size of the one
         * picture line */
        /* This extra buffer is currently not considered in the head pointer
           calculation during snapshot. It may result in dummy lines */
        hjr_extra_buffer_for_bayer_filtering = (vfeRawConfig.camif_window_width_config.lastPixel
                          - vfeRawConfig.camif_window_width_config.firstPixel
                          + 1) * 3;
      }
#endif /* FEATURE_CAMERA_HJR */

      if (raw_image_size % (sizeof (uint32) * vfeRawConfig.burstLenForBuffers))
      {
        raw_image_size = (raw_image_size + ((sizeof (uint32) * vfeRawConfig.burstLenForBuffers) - (raw_image_size % (sizeof (uint32) * vfeRawConfig.burstLenForBuffers)))) / (sizeof (uint32) * vfeRawConfig.burstLenForBuffers);
      }
      else
      {
        raw_image_size /= (sizeof (uint32) * vfeRawConfig.burstLenForBuffers);
      }
      break;
    case VFE_10_BIT_DIRECT:
      qcamraw_format                 = QCAMRAW_BAYER_10BIT;
#ifdef FEATURE_CAMERA_HJR
      if (camera_svcs_hjr_enabled() == TRUE)
      {
         /* while unpacking the bayer data, it is calculated that
          * for every 4 bytes in packed format, 6 bytes are required
          * to unpack them */
         hjr_extra_buffer_for_10_bit_unpacking = ( (raw_image_size * 6) / 4 ) - raw_image_size;
        /* bayer filter needs to operate on the raw data
         * The buffer requirement is based on the picture
         * size. The buffer is 3 times the size of the one
         * picture line */
        hjr_extra_buffer_for_bayer_filtering = ((vfeRawConfig.camif_window_width_config.lastPixel
                          - vfeRawConfig.camif_window_width_config.firstPixel
                          + 1) * 3) * 2;
      }
#endif /* FEATURE_CAMERA_HJR */

      if (raw_image_size % (3 * vfeRawConfig.burstLenForBuffers))
      {
        /* Every 3 pixels fits into one 32-bit memory */
        raw_image_size = (raw_image_size + ((3 * vfeRawConfig.burstLenForBuffers) - (raw_image_size % (3 * vfeRawConfig.burstLenForBuffers)))) / (3 * vfeRawConfig.burstLenForBuffers);
      }
      else
      {
        /* Every 3 pixels fits into one 32-bit memory */
        raw_image_size /= (3 * vfeRawConfig.burstLenForBuffers);
      }
      break;
    default:
      break;
  }

  camera_calculate_ahb_burst_factors (&burst_factor1, &burst_factor2, raw_image_size);

  /* program from 1 to 256 */
  vfeRawConfig.burstFactor2ForBuffers = burst_factor2;

  /* program from 1 to 4096 */
  vfeRawConfig.burstFactor1ForBuffers = burst_factor1;

  /* Allocate memory */
  CAM_Free (raw_snapshot_buf);
  raw_snapshot_buf = CAM_Malloc ( ((vfeRawConfig.burstFactor2ForBuffers *
                     vfeRawConfig.burstFactor1ForBuffers *
                     sizeof(uint32) * vfeRawConfig.burstLenForBuffers) +
                     sizeof (qcamrawHeaderType) + hjr_extra_buffer_for_10_bit_unpacking
                     + hjr_extra_buffer_for_bayer_filtering)
                      | CAM_ALLOC_NO_ZMEM);

  if (raw_snapshot_buf == NULL)
  {
   return FALSE;
  }

  /* Check if fragments can be supported */
  if ((vfeRawConfig.burstFactor1ForBuffers % (vfeRawConfig.numOutputFragsMinus1 + 1)) == 0)
  {
    vfeRawConfig.burstFactor1ForBuffers /= (vfeRawConfig.numOutputFragsMinus1 + 1);
  }
  else
  {
    vfeRawConfig.numOutputFragsMinus1 = 0;
  }


  qcamrawSetHeader((qcamrawHeaderType *) raw_snapshot_buf, 0,
                   (vfeRawConfig.camif_window_width_config.lastPixel - vfeRawConfig.camif_window_width_config.firstPixel + 1) *
                   (vfeRawConfig.camif_window_height_config.lastLine - vfeRawConfig.camif_window_height_config.firstLine + 1),
                   qcamraw_format, QCAMRAW_PICTURE_FRAME, QCAMRAW_NO_REFLECT, camera_default_preview_rotation,
                   (vfeRawConfig.camif_window_width_config.lastPixel - vfeRawConfig.camif_window_width_config.firstPixel + 1),
                   (vfeRawConfig.camif_window_height_config.lastLine - vfeRawConfig.camif_window_height_config.firstLine + 1),
                   (vfeRawConfig.camif_window_width_config.lastPixel - vfeRawConfig.camif_window_width_config.firstPixel + 1),
                   (vfeRawConfig.camif_window_height_config.lastLine - vfeRawConfig.camif_window_height_config.firstLine + 1),
                   0, 0, 0, 0,
                   (vfeRawConfig.burstFactor2ForBuffers * vfeRawConfig.burstFactor1ForBuffers * sizeof(uint32) *
                    vfeRawConfig.burstLenForBuffers), 0, 0, NULL);

  /* Initialize all the fragments to the begining of the buffer, this is
   * to eliminate the possibility crashing when VFE overflows into NULL
   * addressed  fragment
   */
  for (number_of_fragments = 0; number_of_fragments < VFE_MAX_BUFFER_FRAGMENTS; ++number_of_fragments)
  {
    vfeRawConfig.outputBufferFragment[number_of_fragments] =
               (VFE_AddressType) ( qcamrawGetDataYPtr ((uint8 *) raw_snapshot_buf)
                                   + hjr_extra_buffer_for_bayer_filtering);
  }

  /* Fill the addresses of specified number of fragments */
  for (number_of_fragments = 0; number_of_fragments <= vfeRawConfig.numOutputFragsMinus1; ++number_of_fragments)
  {
    vfeRawConfig.outputBufferFragment[number_of_fragments] = (VFE_AddressType) (qcamrawGetDataYPtr ((uint8 *) raw_snapshot_buf) + hjr_extra_buffer_for_bayer_filtering + number_of_fragments * (vfeRawConfig.camif_window_height_config.lastLine - vfeRawConfig.camif_window_height_config.firstLine + 1) * (vfeRawConfig.camif_window_width_config.lastPixel - vfeRawConfig.camif_window_width_config.firstPixel + 1) / (vfeRawConfig.numOutputFragsMinus1 + 1));
  }

  camera_dsp_command = CAMQDSP_CMD_CONFIG_RAW_SNAPSHOT;

  return TRUE;
}

/*===========================================================================

FUNCTION      CAMERA_SVCS_SETPARAM_VFE_SNAPSHOT

DESCRIPTION
              Configures VFE.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Success
  FALSE: Failure

SIDE EFFECTS
  None

===========================================================================*/

boolean camera_svcs_setparam_vfe_snapshot ()
{
  uint8           pad_size = 0;
  uint8           chroma_factorQ1 = 2;
  uint8           chroma_factor   = 2;
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
  uint8           number_of_fragments = 0;
  uint16          width, height, jpege_thumbnail_height;
  qcamrawDataType qcamraw_format = QCAMRAW_H2V2;
  boolean         is_h2v2 = (camera_snapshot_format == CAMERA_H2V2);
  int16           normal = 0;
  uint16          number_of_frames;
  uint8           total_fragments = vfeSnapshotConfig.output2Configuration.fragmentsMinus1 + 1;

  /* Amount of AHB data for each Y-Fragment */
  uint32          main_image_y_fragment_ahb_bytes;

  /* Amount of AHB data for each CbCr-Fragment */
  uint32          main_image_cbcr_fragment_ahb_bytes;

  /* Amount of AHB data for entire Y component*/
  uint32          main_image_y_ahb_bytes;

  /* Amount of AHB data for entire CbCr component */
  uint32          main_image_cbcr_ahb_bytes;

  uint8* luma_start_address;
  uint8* luma_address_32_byte_aligned;
  uint16 luma_pad_size = 0;
  uint8* chroma_address_32_byte_aligned;
  uint32 thumbnail_y_offset = 0;
  uint32 thumbnail_cbcr_offset = 0;
  uint8* tb_luma_start_address;
  uint16 tb_luma_pad_size =0;
  uint8* tb_chroma_address_32_byte_aligned;
  uint8* tb_luma_address_32_byte_aligned;

  /* Limit the numfragments that can be supported */
  if (total_fragments > VFE_MAX_BUFFER_FRAGMENTS)
  {
    /* De-Limit the max. number of fragments, f/w interface
     * allows upto 12 fragments. Maximum of 4 fragments is used so far
     */
    total_fragments = VFE_MAX_BUFFER_FRAGMENTS;
  }

  /* Compute the VFE clipping parameters corresponding to
   * current zooming level
   */
  camera_calculate_vfe_clipping ();

  /* Trigger BlackLevel Correction , Bas Pixel correction and adaptive layer bayer filter */

#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */

  /* Chroma cositing for YCbCr sensors */
  if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR)
  {
#ifndef QDSP_MODULE_VFE05_DEFINED
    vfeSnapshotConfig.inputConfiguration.YCbCrCositing = VFE_CHROMA_COSITED;
#endif
  }


  /* Round up thumbnail dimensions to MCU boundary */
  jpege_get_dimensions(&width, &jpege_thumbnail_height,
                     vfeSnapshotConfig.output1Configuration.width, vfeSnapshotConfig.output1Configuration.height,
                     is_h2v2, camera_jpeg_encode_rotation_format,
                     vfeSnapshotConfig.clip.lastPixel - vfeSnapshotConfig.clip.firstPixel +1,
                     vfeSnapshotConfig.clip.lastLine - vfeSnapshotConfig.clip.firstLine +1);

  vfeSnapshotConfig.output1Configuration.width = width;
  vfeSnapshotConfig.output1Configuration.height = jpege_thumbnail_height;

  /* Round up image dimensions to MCU boundary subject to max dimensions */
  jpege_get_dimensions(&width, &height,
                      vfeSnapshotConfig.output2Configuration.width, vfeSnapshotConfig.output2Configuration.height,
                      is_h2v2, camera_jpeg_encode_rotation_format,
                      vfeSnapshotConfig.clip.lastPixel - vfeSnapshotConfig.clip.firstPixel +1,
                      vfeSnapshotConfig.clip.lastLine - vfeSnapshotConfig.clip.firstLine +1);

  vfeSnapshotConfig.output2Configuration.width = width;
  vfeSnapshotConfig.output2Configuration.height = height;

  /* Make sure thumbnail has the same aspect ratio as main image */
  if (((thumbnail_width != orig_picture_dx) && (thumbnail_height != orig_picture_dy)) &&
      (((unsigned long)orig_picture_dx *
        (unsigned long)vfeSnapshotConfig.output1Configuration.height) !=
       ((unsigned long)vfeSnapshotConfig.output1Configuration.width *
        (unsigned long)orig_picture_dy)))
  {
    /* Adjust thumbnail height according to the aspect ratio of main image */
    vfeSnapshotConfig.output1Configuration.height =
      (vfeSnapshotConfig.output1Configuration.width * orig_picture_dy) / (orig_picture_dx);

    /* Make sure VFE configuration height is divisible by 4 */
    vfeSnapshotConfig.output1Configuration.height = CEILING4(vfeSnapshotConfig.output1Configuration.height);

    /* Make sure VFE configuration height <= jpege_thumbnail_height */
    vfeSnapshotConfig.output1Configuration.height = MIN(vfeSnapshotConfig.output1Configuration.height,
                                                        jpege_thumbnail_height);
  }

  switch (camera_snapshot_format)
  {
    case CAMERA_H2V1:
      switch (camera_jpeg_encode_rotation_format)
      {

        /* If H2V1 with odd rotation capture in H2V2 */
        case JPEGENC_CLK90:
        case JPEGENC_CLK270:
        case JPEGENC_CCLK90:
        case JPEGENC_CCLK270:
          if( camera_encode_properties.format == CAMERA_JPEG )
          {
            #ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
            #endif /* FEATURE_CAMERA_BURST_MODE */
            qcamraw_format = QCAMRAW_H1V1;
            #ifndef QDSP_MODULE_VFE05_DEFINED
            vfeSnapshotConfig.output1Configuration.imageFormat = VFE_YCbCrH1V1;
            #endif
            chroma_factorQ1 = 1;
            chroma_factor=1;
            break;
          }
          /* If we are not doing JPEG fall through and use H2V1 encoding */
        case JPEGENC_NO_ROT:
        case JPEGENC_CLK180:
        case JPEGENC_CCLK180:
         #ifdef FEATURE_CAMERA_BURST_MODE  
#error code not present
         #endif  /* FEATURE_CAMERA_BURST_MODE */
          qcamraw_format = QCAMRAW_H2V1;
          vfeSnapshotConfig.output1Configuration.imageFormat = VFE_YCbCrH2V1;
          chroma_factorQ1 = 2;
          chroma_factor=2;
          break;

      }
      break;

    case CAMERA_H2V2:
    default:
     #ifdef FEATURE_CAMERA_BURST_MODE 
#error code not present
     #endif /* FEATURE_CAMERA_BURST_MODE */
      qcamraw_format = QCAMRAW_H2V2;
      vfeSnapshotConfig.output1Configuration.imageFormat = VFE_YCbCrH2V2;
    #if defined QDSP_MODULE_VFE05_DEFINED
      vfeSnapshotConfig.output2Configuration.imageFormat = VFE_YCbCrH2V2;
    #endif /*  QDSP_MODULE_VFE05_DEFINED  &&  FEATURE_CAMERA_VFE_SNAPSHOT_YCBCR420 */
      chroma_factorQ1 = 4;
      chroma_factor=4;
      break;
  }

#ifdef QDSP_MODULE_VFE05_DEFINED
  /* For VFE051, H2V2 is supported only till QVGA size  and for VFE052 H2V2is supported  in case of YUV sensors. For all sizes
   * higher than QVGA, H2V1 only is supported.
   * Snapshot size is always the clip window size since scaling is not
   * supported for output size higher than QVGA */
  if ( vfeSnapshotConfig.output2Configuration.width > 320 ||
       vfeSnapshotConfig.output2Configuration.height > 240)
  {
     if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR)
     {
     /* Configure the output2 format to 0x2 for ULC in case of YUV sensor only */
     vfeSnapshotConfig.output2Configuration.imageFormat = VFE_YCbCrH2V2_YUV;
     vfeSnapshotConfig.output1Configuration.imageFormat = VFE_YCbCrH2V2;
     chroma_factor   = 4;
     chroma_factorQ1 = 4; 
     }
    else
     {
     vfeSnapshotConfig.output2Configuration.imageFormat = VFE_YCbCrH2V1;
     vfeSnapshotConfig.output1Configuration.imageFormat = VFE_YCbCrH2V1;
     chroma_factorQ1 = 2;
     chroma_factor   = 2;
     }

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
    if(vfeSnapshotConfig.output2Configuration.imageFormat == VFE_YCbCrH2V1)
     { 
       yds_cmd.data.in_frag_n_format.format = YDS_H2V1;
     }
    else
     {
    yds_cmd.data.in_frag_n_format.format = YDS_H2V2;
     }   
    /* Preserve the user/output width & height and passing it to the QDSP command structure.*/
    yds_cmd.data.out_image.width = vfeSnapshotConfig.output2Configuration.width;
    yds_cmd.data.out_image.height = vfeSnapshotConfig.output2Configuration.height;
#endif /*QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/
    vfeSnapshotConfig.output2Configuration.width  = vfeSnapshotConfig.clip.lastPixel - vfeSnapshotConfig.clip.firstPixel + 1;
    vfeSnapshotConfig.output2Configuration.height = vfeSnapshotConfig.clip.lastLine - vfeSnapshotConfig.clip.firstLine + 1;
#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
    /* Passing the input width and height to QDSP command structure.*/
    yds_cmd.data.in_image.width = vfeSnapshotConfig.output2Configuration.width;
    yds_cmd.data.in_image.height = vfeSnapshotConfig.output2Configuration.height;
#endif /*QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/
  }
#endif /*QDSP_MODULE_VFE05_DEFINED */

  /* Adjust VFE output1 buffer offset depending on rotation */
  if ((jpege_thumbnail_height > vfeSnapshotConfig.output1Configuration.height) &&
      ((camera_jpeg_encode_rotation_format == JPEGENC_CLK90) ||
       (camera_jpeg_encode_rotation_format == JPEGENC_CCLK270) ||
       (camera_jpeg_encode_rotation_format == JPEGENC_CLK180) ||
       (camera_jpeg_encode_rotation_format == JPEGENC_CCLK180)))
  {
    thumbnail_y_offset    = (jpege_thumbnail_height - vfeSnapshotConfig.output1Configuration.height) *
                            vfeSnapshotConfig.output1Configuration.width;
    thumbnail_cbcr_offset = 2 * (jpege_thumbnail_height - vfeSnapshotConfig.output1Configuration.height) *
                            vfeSnapshotConfig.output1Configuration.width / chroma_factor;
  }
  else
  {
    thumbnail_y_offset    = 0;
    thumbnail_cbcr_offset = 0;
  }

  /* specify the chroma siting */
  vfeSnapshotConfig.output1Configuration.cositing = JPEG;

  /* Compute output1 AHB parameters */

  /* Configure the burst length and burst number */

  /* AHB burst length and burst number parameters */
  vfeSnapshotConfig.output1_AHB_Configuration.numBurstForYBuffers = CAMERA_AHB_BURST_NUM;
  vfeSnapshotConfig.output1_AHB_Configuration.numBurstForCbCrBuffers = CAMERA_AHB_BURST_NUM;
  if (jpege_thumbnail_height > vfeSnapshotConfig.output1Configuration.height)
  {
    vfeSnapshotConfig.output1_AHB_Configuration.burstLenForYBuffers = CAMERA_AHB_BURST_LEN_8;
    vfeSnapshotConfig.output1_AHB_Configuration.burstLenForCbCrBuffers = CAMERA_AHB_BURST_LEN_8;
  }
  else
  {
    vfeSnapshotConfig.output1_AHB_Configuration.burstLenForYBuffers = CAMERA_AHB_BURST_LEN_16;
    vfeSnapshotConfig.output1_AHB_Configuration.burstLenForCbCrBuffers = CAMERA_AHB_BURST_LEN_16;
  }

  vfeSnapshotConfig.output2_AHB_Configuration.numBurstForYBuffers = CAMERA_AHB_BURST_NUM;
  vfeSnapshotConfig.output2_AHB_Configuration.burstLenForYBuffers = CAMERA_AHB_BURST_LEN;
  vfeSnapshotConfig.output2_AHB_Configuration.numBurstForCbCrBuffers = CAMERA_AHB_BURST_NUM;
  vfeSnapshotConfig.output2_AHB_Configuration.burstLenForCbCrBuffers = CAMERA_AHB_BURST_LEN;

  /* Output1 - Y component */
  /* program from 1 to 4096 */
  vfeSnapshotConfig.output1_AHB_Configuration.burstFactor1ForYBuffers = vfeSnapshotConfig.output1Configuration.width / vfeSnapshotConfig.output1_AHB_Configuration.burstLenForYBuffers;

  /* program from 1 to 256 */
  vfeSnapshotConfig.output1_AHB_Configuration.burstFactor2ForYBuffers = vfeSnapshotConfig.output1Configuration.height / sizeof (uint32);

  /* Output1 - CbCr Component */
  /* program from 1 to 4096 */
  vfeSnapshotConfig.output1_AHB_Configuration.burstFactor1ForCbCrBuffers = vfeSnapshotConfig.output1Configuration.width / vfeSnapshotConfig.output1_AHB_Configuration.burstLenForCbCrBuffers;

  /* program from 1 to 256 */
  vfeSnapshotConfig.output1_AHB_Configuration.burstFactor2ForCbCrBuffers = vfeSnapshotConfig.output1Configuration.height / sizeof (uint32);

  if (((2*vfeSnapshotConfig.output1_AHB_Configuration.burstFactor2ForCbCrBuffers) % chroma_factor) == 0)
  {
    /* Ensure that burst factor is evenly divisible */
    vfeSnapshotConfig.output1_AHB_Configuration.burstFactor2ForCbCrBuffers = (2*vfeSnapshotConfig.output1_AHB_Configuration.burstFactor2ForCbCrBuffers)/chroma_factor;
  }
  else if (((2*vfeSnapshotConfig.output1_AHB_Configuration.burstFactor1ForCbCrBuffers) % chroma_factor) == 0)
  {
    /* Ensure that burst factor is evenly divisible */
    vfeSnapshotConfig.output1_AHB_Configuration.burstFactor1ForCbCrBuffers = (2*vfeSnapshotConfig.output1_AHB_Configuration.burstFactor1ForCbCrBuffers) / chroma_factor;
  }
  else
  {
    /* For now no need to do anything, the above two cases should handle
     * all the image resolutions that are supported
     */
  }

  /* Compute output2 AHB parameters */

  /* Output2 - Y Component */
  /* program from 1 to 4096 */
  vfeSnapshotConfig.output2_AHB_Configuration.burstFactor1ForYBuffers = vfeSnapshotConfig.output2Configuration.width / sizeof (uint32);

  /* program from 1 to 256 */
  vfeSnapshotConfig.output2_AHB_Configuration.burstFactor2ForYBuffers = vfeSnapshotConfig.output2Configuration.height / vfeSnapshotConfig.output2_AHB_Configuration.burstLenForYBuffers;

  /* Output2 - CbCr Component */
  /* program from 1 to 4096 */
  vfeSnapshotConfig.output2_AHB_Configuration.burstFactor1ForCbCrBuffers = vfeSnapshotConfig.output2Configuration.width / sizeof (uint32);

  /* program from 1 to 256 */
  vfeSnapshotConfig.output2_AHB_Configuration.burstFactor2ForCbCrBuffers =  vfeSnapshotConfig.output2Configuration.height / vfeSnapshotConfig.output2_AHB_Configuration.burstLenForCbCrBuffers;

  if (((2*vfeSnapshotConfig.output2_AHB_Configuration.burstFactor2ForCbCrBuffers) % chroma_factorQ1) == 0)
  {
    /* Ensure that burst factor is evenly divisible */
    vfeSnapshotConfig.output2_AHB_Configuration.burstFactor2ForCbCrBuffers = (2*vfeSnapshotConfig.output2_AHB_Configuration.burstFactor2ForCbCrBuffers) /chroma_factorQ1;
  }
  else if (((2*vfeSnapshotConfig.output2_AHB_Configuration.burstFactor1ForCbCrBuffers) % chroma_factorQ1) == 0)
  {
    /* Ensure that burst factor is evenly divisible */
    vfeSnapshotConfig.output2_AHB_Configuration.burstFactor1ForCbCrBuffers = (2* vfeSnapshotConfig.output2_AHB_Configuration.burstFactor1ForCbCrBuffers)/chroma_factorQ1;
  }
  else
  {
    /* For now no need to do anything, the above two cases should handle
     * all the image resolutions that are supported */
  }

  /* For 2M image burstFactor2 is 300, so we divide it by 2 and multiply burstFactor1 by 2 */
  if (vfeSnapshotConfig.output2_AHB_Configuration.burstFactor2ForCbCrBuffers > 256)
  {
    vfeSnapshotConfig.output2_AHB_Configuration.burstFactor2ForCbCrBuffers /= 2;
    vfeSnapshotConfig.output2_AHB_Configuration.burstFactor1ForCbCrBuffers *= 2;
  }

  /* Check if fragments can be supported */
  if ( ((vfeSnapshotConfig.output2_AHB_Configuration.burstFactor1ForYBuffers % total_fragments) == 0) &&
       ((vfeSnapshotConfig.output2_AHB_Configuration.burstFactor1ForCbCrBuffers % total_fragments) == 0) )
  {
    vfeSnapshotConfig.output2_AHB_Configuration.burstFactor1ForYBuffers /= total_fragments;
    vfeSnapshotConfig.output2_AHB_Configuration.burstFactor1ForCbCrBuffers /= total_fragments;
  }
  else
  {
    vfeSnapshotConfig.output2Configuration.fragmentsMinus1 = 0;
    total_fragments = 1;
  }

  /* setup color conversion */

  /* Copy Color Correction and Color Convert over for Snapshot */
  if ( ((int32) vfeSnapshotConfig.colorConversion.enable == (int)VFE_Enable) &&
       ((int32) vfeUpdate.colorConversion.enable == (int)VFE_Enable)
     )
  {
    vfeSnapshotConfig.colorConversion = vfeUpdate.colorConversion;
  }
  else
  {
    vfeSnapshotConfig.colorConversion = vfeVideoConfig.colorConversion;
  }

  if (( camera_parm_special_effects.current_value == CAMERA_EFFECT_OFF )
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif
      )
  {
    /* Adjust color conversion matrix */
    /* We will compute the diagonal of the matrix based on other coefficients */
    /* 1st row sum to unity, 2nd and 3rd row sum to zero */
    normal = (int16) (1 << ((int16)vfeSnapshotConfig.colorConversion.coefficient_Q_Factor + 7));
    vfeSnapshotConfig.colorConversion.A11 = normal -
                                         (vfeSnapshotConfig.colorConversion.A12 +
                                          vfeSnapshotConfig.colorConversion.A13);

    vfeSnapshotConfig.colorConversion.A22 = -(vfeSnapshotConfig.colorConversion.A21 +
                                            vfeSnapshotConfig.colorConversion.A23);

    vfeSnapshotConfig.colorConversion.A33 = -(vfeSnapshotConfig.colorConversion.A31 +
                                            vfeSnapshotConfig.colorConversion.A32);
  }

  /* setup color correction */

  if ( ((int32) vfeSnapshotConfig.colorCorrection.enable == (int)VFE_Enable) &&
       ((int32) vfeUpdate.colorCorrection.enable == (int)VFE_Enable)
     )
  {
    vfeSnapshotConfig.colorCorrection = vfeUpdate.colorCorrection;
  }
  else
  {
    vfeSnapshotConfig.colorCorrection = vfeVideoConfig.colorCorrection;
  }
#ifndef FEATURE_CAMERA_YCBCR_ONLY
  if (camsensor_static_params[camera_asi].output_format != CAMSENSOR_YCBCR)
  {
#ifdef ANY_FLASH
    boolean fire_flash = camera_fire_flash;
#else /* ANY_FLASH */
    boolean fire_flash = FALSE;
#endif /* ANY_FLASH */
#ifdef FEATURE_CAMERA_HJR
    camera_awb_prepare_wb_for_snapshot(fire_flash, camera_svcs_hjr_enabled());
#else
    camera_awb_prepare_wb_for_snapshot(fire_flash, FALSE);
#endif /* FEATURE_CAMERA_HJR */
  }
  /* Update the gamma table if set by the sensor
   */
  if ((int32) vfeSnapshotConfig.gammaCorrection.enable == (int)VFE_Enable)
  {
    int32 current_special_effect = camera_parm_special_effects.current_value;
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
    vfe_put_gamma_table(current_special_effect,
        camera_parm_contrast.current_value, camera_camsensor_op_mode);
  }
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
  main_image_y_fragment_ahb_bytes =
    (vfeSnapshotConfig.output2_AHB_Configuration.burstFactor1ForYBuffers *
     vfeSnapshotConfig.output2_AHB_Configuration.burstFactor2ForYBuffers *
     sizeof(uint32) *
     vfeSnapshotConfig.output2_AHB_Configuration.burstLenForYBuffers);

  main_image_cbcr_fragment_ahb_bytes =
    (vfeSnapshotConfig.output2_AHB_Configuration.burstFactor1ForCbCrBuffers *
     vfeSnapshotConfig.output2_AHB_Configuration.burstFactor2ForCbCrBuffers *
     sizeof(uint32) *
     vfeSnapshotConfig.output2_AHB_Configuration.burstLenForCbCrBuffers);

  main_image_y_ahb_bytes = main_image_y_fragment_ahb_bytes * total_fragments;

  main_image_cbcr_ahb_bytes = main_image_cbcr_fragment_ahb_bytes *
                                                           total_fragments;

  /* Pad data between Y and CbCr buffers */
  pad_size = main_image_y_ahb_bytes -
             (vfeSnapshotConfig.output2Configuration.height *
              vfeSnapshotConfig.output2Configuration.width);

  /* allocate and prepare output buffer */
  if (camera_state == CAMERA_STATE_TAKE_PICTURE)
  {
    /* Main image: Allocate exact memory required for the output format */
    CAM_Free (snapshot_luma_buf);
    /* CAM_Malloc gives 32-bit aligned memory buffer. */
    snapshot_luma_buf = CAM_Malloc ((main_image_y_ahb_bytes +
                                   sizeof (qcamrawHeaderType)+ CAMERA_BUF_ALIGN)
                                   | CAM_ALLOC_NO_ZMEM);
    if (snapshot_luma_buf == NULL)
    {
      return FALSE;
    }

    /* Make the Luma buffer start address to be 32 byte aligned to use the
     *  BUS efficently */
    luma_start_address = (uint8*) (snapshot_luma_buf+ sizeof(qcamrawHeaderType));
    luma_address_32_byte_aligned = (uint8*)( ((uint32)(luma_start_address + (CAMERA_BUF_ALIGN - 1)) / CAMERA_BUF_ALIGN )  * CAMERA_BUF_ALIGN ) ;
    luma_pad_size = (uint32)luma_address_32_byte_aligned - (uint32)luma_start_address;

    CAM_Free(snapshot_chroma_buf);
    snapshot_chroma_buf = CAM_Malloc ( (main_image_cbcr_ahb_bytes + CAMERA_BUF_ALIGN)  | CAM_ALLOC_NO_ZMEM);
    if (snapshot_chroma_buf == NULL)
    {
      CAM_Free (snapshot_luma_buf);
      snapshot_luma_buf = NULL;
      return FALSE;
    }
    /* Make the chroma buffer start address to be 32 byte aligned to use the
     *  BUS efficently */

    chroma_address_32_byte_aligned = (uint8*)( ((uint32)( snapshot_chroma_buf + (CAMERA_BUF_ALIGN - 1) ) / CAMERA_BUF_ALIGN ) * CAMERA_BUF_ALIGN ) ;

    /* Set the header for main image */
    qcamrawSetHeader((qcamrawHeaderType *) snapshot_luma_buf, 0, vfeSnapshotConfig.output2Configuration.height *
                     vfeSnapshotConfig.output2Configuration.width, qcamraw_format, QCAMRAW_PICTURE_FRAME,
                     QCAMRAW_NO_REFLECT, camera_default_preview_rotation, orig_picture_dx,
                     orig_picture_dy, vfeSnapshotConfig.output2Configuration.width,
                     vfeSnapshotConfig.output2Configuration.height, 0, 0, 0, luma_pad_size,
                     vfeSnapshotConfig.output2Configuration.height * vfeSnapshotConfig.output2Configuration.width,
                     pad_size, main_image_cbcr_ahb_bytes,
                     (uint8 *)chroma_address_32_byte_aligned);
    /* padding required for 32-bit alignment of Y and CbCr components
     * for thumbnail
     */
    pad_size = (jpege_thumbnail_height * vfeSnapshotConfig.output1Configuration.width % 4) ? (4 - (jpege_thumbnail_height * vfeSnapshotConfig.output1Configuration.width % 4)) : 0;

    /* Thumbnail: Allocate the memory for H2V1 format which is the max size
     * possible since if H1V2 we capture the image in H2V2 and later upsample
     * in the same CrCb buffer assuming that there is space (it is twice its size)
     */


    CAM_Free (thumbnail_luma_buf);
    thumbnail_luma_buf = CAM_Malloc (((jpege_thumbnail_height * vfeSnapshotConfig.output1Configuration.width) + CAMERA_BUF_ALIGN+ sizeof (qcamrawHeaderType))
                    | CAM_ALLOC_NO_ZMEM);

    if (thumbnail_luma_buf == NULL)
    {
      CAM_Free (snapshot_luma_buf);
      snapshot_luma_buf = NULL;
      CAM_Free (snapshot_chroma_buf);
      snapshot_chroma_buf = NULL;
      return FALSE;
    }

    tb_luma_start_address = (uint8*)(thumbnail_luma_buf+sizeof(qcamrawHeaderType));
    tb_luma_address_32_byte_aligned= (uint8*)( ((uint32)(tb_luma_start_address + (CAMERA_BUF_ALIGN - 1)) / CAMERA_BUF_ALIGN )  * CAMERA_BUF_ALIGN ) ;
    tb_luma_pad_size = (uint32)tb_luma_address_32_byte_aligned - (uint32)tb_luma_start_address;

    CAM_Free (thumbnail_chroma_buf);
    thumbnail_chroma_buf=CAM_Malloc(((jpege_thumbnail_height * vfeSnapshotConfig.output1Configuration.width) + CAMERA_BUF_ALIGN)
                    | CAM_ALLOC_NO_ZMEM);

    tb_chroma_address_32_byte_aligned = (uint8*)( ((uint32)( thumbnail_chroma_buf + (CAMERA_BUF_ALIGN - 1) ) / CAMERA_BUF_ALIGN ) * CAMERA_BUF_ALIGN ) ;

    if(thumbnail_chroma_buf ==NULL)
    {
      CAM_Free(thumbnail_luma_buf);
      thumbnail_luma_buf=NULL;
      CAM_Free (snapshot_luma_buf);
      snapshot_luma_buf = NULL;
      CAM_Free (snapshot_chroma_buf);
      snapshot_chroma_buf = NULL;
      return FALSE;

    }

    /* Set the header for thumbnail image */
    qcamrawSetHeader((qcamrawHeaderType *) thumbnail_luma_buf, 0, jpege_thumbnail_height *
                     vfeSnapshotConfig.output1Configuration.width, qcamraw_format, QCAMRAW_PICTURE_FRAME,
                     QCAMRAW_NO_REFLECT, camera_default_preview_rotation, vfeSnapshotConfig.output1Configuration.width,
                     vfeSnapshotConfig.output1Configuration.height, vfeSnapshotConfig.output1Configuration.width,
                     jpege_thumbnail_height, 0, 0, 0, tb_luma_pad_size,
                     jpege_thumbnail_height * vfeSnapshotConfig.output1Configuration.width,
                     pad_size, jpege_thumbnail_height * vfeSnapshotConfig.output1Configuration.width,
                     (uint8 *)tb_chroma_address_32_byte_aligned);

    for (number_of_frames = 0; number_of_frames < VFE_MIN_NUM_OF_FRAMES; number_of_frames++)
    {
  /* Y componet for thumbnail */
      vfeSnapshotConfig.outputBufferAddress[number_of_frames].Y_Address    =
        (VFE_AddressType) (qcamrawGetDataYPtr ((byte *) thumbnail_luma_buf) + thumbnail_y_offset);
  /* CbCr component for thumbnail */
      vfeSnapshotConfig.outputBufferAddress[number_of_frames].CbCr_Address =
        (VFE_AddressType) (qcamrawGetDataCbCrPtr ((byte *) thumbnail_luma_buf) + thumbnail_cbcr_offset);
    }

  /* Initialize all the fragments to the begining of the buffer, this is
   * to eliminate the possibility of crashing when VFE overflows into NULL
   * addressed  fragment
   */
    for (number_of_fragments = 0; number_of_fragments < VFE_MAX_BUFFER_NUMBER; ++number_of_fragments)
    {
      vfeSnapshotConfig.outputBufferFragment[number_of_fragments].Y_Address =
        (VFE_AddressType) (qcamrawGetDataYPtr ((uint8 *) snapshot_luma_buf));
      vfeSnapshotConfig.outputBufferFragment[number_of_fragments].CbCr_Address =
        (VFE_AddressType) (qcamrawGetDataCbCrPtr ((uint8 *) snapshot_luma_buf));
    }

  /* Fill the addresses of specified number of fragments */
    for (number_of_fragments = 0; number_of_fragments < total_fragments; ++number_of_fragments)
    {
      vfeSnapshotConfig.outputBufferFragment[number_of_fragments].
      Y_Address =
        (VFE_AddressType) (qcamrawGetDataYPtr ((uint8 *) snapshot_luma_buf) +
                                         number_of_fragments *
                                         main_image_y_fragment_ahb_bytes);

      vfeSnapshotConfig.outputBufferFragment[number_of_fragments].
      CbCr_Address =
        (VFE_AddressType) (qcamrawGetDataCbCrPtr ((uint8 *) snapshot_luma_buf) +
                                         number_of_fragments *
                                         main_image_cbcr_fragment_ahb_bytes);
    }
#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
    /* Passing the input Y & CbCr address to QDSP command structure.*/
    yds_cmd.data.in_Yaddress_frag0.address = vfeSnapshotConfig.outputBufferFragment[0].Y_Address;
    yds_cmd.data.in_CbCraddress_frag0.address = vfeSnapshotConfig.outputBufferFragment[0].CbCr_Address;
#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED */
  }

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_5x5_ASF
  /* if asf is enable but not in hardware, setup luma filter as smoothing filter */
  if (camctrl_tbl.asf_5x5_is_supported == TRUE &&
      (vfe_hw_features.filter_3x3_asf == FALSE ||
       vfe_sw_features.enable_hw_filter_3x3_asf == FALSE))
  {
    camera_svcs_set_luma_filter ();
  }
#endif /* FEATURE_CAMERA_5x5_ASF */

  /* Config command to VFE */
  camera_dsp_command = CAMQDSP_CMD_CONFIG_SNAPSHOT;
  return TRUE;

}

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION     CAMERA_SVCS_SETPARAM_VFE_OFFLINE_SNAPSHOT

DESCRIPTION
             Configures VFE for offline mode using the output of raw snapshot.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

boolean camera_svcs_setparam_vfe_offline_snapshot ()
{
  uint8 num_frags = 0;

  camera_3x3_rgb_matrix_type *color_correction_normal_ptr;
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
   color_correction_normal_ptr = vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL);

#endif

  if(color_correction_normal_ptr == NULL)
  {
    MSG_ERROR ("color correction matrix is not ready in VFE", 0, 0, 0);
    return FALSE;
  }

  /* setup snapshot vfe parameters */
  if ((camera_svcs_setparam_vfe_snapshot () == FALSE) ||
      (camera_svcs_raw_snapshot_copy == NULL))
  {
    return FALSE;
  }

  /* width is the same as raw output width */
  vfeOfflineConfig.input.width = camsensor_static_params[camera_asi].full_size_width;

  /* height is the same as raw output height */
  vfeOfflineConfig.input.height = camsensor_static_params[camera_asi].full_size_height;

  /* input data width is the same as raw output */
  vfeOfflineConfig.input.dataWidth = (VFE_PixelDataWidthType)(vfeRawConfig.rawOutputOption);

  /* input format is the same as input for snapshot */
  vfeOfflineConfig.input.format = (CAMIF_InputFormatType)vfeSnapshotConfig.inputConfiguration.format;
#ifndef QDSP_MODULE_VFE05_DEFINED
  vfeOfflineConfig.input.YCbCrCositing = vfeSnapshotConfig.inputConfiguration.YCbCrCositing;
#endif

  /* configure input AHB */
  vfeOfflineConfig.input_AHB.numBurstForYBuffers = CAMERA_AHB_BURST_NUM;
  vfeOfflineConfig.input_AHB.burstLenForYBuffers = CAMERA_AHB_BURST_LEN;

  vfeOfflineConfig.input_AHB.burstFactor1ForYBuffers = vfeRawConfig.burstFactor1ForBuffers;
  vfeOfflineConfig.input_AHB.burstFactor2ForYBuffers = vfeRawConfig.burstFactor2ForBuffers;

  /* bvcm snapshot only call offline mode with RGB data */
  vfeOfflineConfig.input_AHB.numBurstForCbCrBuffers = CAMERA_AHB_BURST_NUM;
  vfeOfflineConfig.input_AHB.burstLenForCbCrBuffers = CAMERA_AHB_BURST_LEN;
  vfeOfflineConfig.input_AHB.burstFactor1ForCbCrBuffers = 1;
  vfeOfflineConfig.input_AHB.burstFactor2ForCbCrBuffers = 1;

  /* copy snapshot common parameters */
  memcpy ((void *)vfeOfflineConfig.output2BufferFragment,
          (void *)vfeSnapshotConfig.outputBufferFragment,
          (uint32)(&vfeSnapshotConfig.camif_config) -
          (uint32)(&vfeSnapshotConfig.outputBufferFragment));

  memcpy ((void *)&vfeOfflineConfig.output1Configuration,
          (void *)&vfeSnapshotConfig.output1Configuration,
          (uint32)(&vfeSnapshotConfig.outputBufferAddress) -
          (uint32)(&vfeSnapshotConfig.output1Configuration));

  vfeOfflineConfig.output1Buffer = vfeSnapshotConfig.outputBufferAddress[0];

  /* estimate the time needed to wait for AHB read */
  offline_read_wait = ((vfeOfflineConfig.input.height - vfeOfflineConfig.clip.lastLine) *
                        vfeOfflineConfig.input.width) >> 13;
  /* Need to over-ride all the settings done for CC if BVCM is not used */
  vfeOfflineConfig.colorCorrection = vfeSnapshotConfig.colorCorrection;
  vfeOfflineConfig.whiteBalance = vfeSnapshotConfig.whiteBalance;

#ifdef QDSP_MODULE_VFE25_DEFINED  
#error code not present
#else
  if ( vfe_hw_features.filter_3x3_asf == TRUE )
  {
    /* copy snapshot ASF configuration */
    vfeOfflineConfig.asf_cfg = vfeSnapshotConfig.asf_cfg;
  }
  else
  {
    vfeOfflineConfig.asf_cfg.enable = VFE_Disable;
  }
#endif /* QDSP_MODULE_VFE25_DEFINED */

  /* setup input data */
  for (num_frags = 0; num_frags < VFE_MAX_BUFFER_FRAGMENTS; num_frags++)
  {
    vfeOfflineConfig.inputBufferFragment[num_frags].Y_Address = (VFE_AddressType) qcamrawGetDataYPtr (camera_svcs_raw_snapshot_copy);
  }

  /* Config command to VFE */
  camera_dsp_command = CAMQDSP_CMD_CONFIG_OFFLINE;
  return TRUE;

}
#endif /* FEATURE_CAMERA_YCBCR_ONLY */


/*===========================================================================

FUNCTION      CAMERA_SET_PREVIEW_HEADERS

DESCRIPTION
              Set the raw data format header for the preview frame.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

void camera_set_preview_headers(void)
{
  uint16 pic_dx, pic_dy;
  qcamrawDataType data_type;
  uint32 y_size = 0, y_pad = 0, cbcrsize = 0;
  uint32 y_size_encode = 0, y_pad_encode = 0, cbcrsize_encode =0;
  uint16  rotation;
  uint32 current_buffer;

  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    if (camera_preview_buffers.buffers[current_buffer].buf_ptr == NULL)
    {
      return;
    }
  }

  if (camera_odd_rotation)
  {
    pic_dx = (uint16) camera_preview_dy;
    pic_dy = (uint16) camera_preview_dx;
  }
  else
  {
    pic_dx = (uint16) camera_preview_dx;
    pic_dy = (uint16) camera_preview_dy;
  }

  /* In preview state, set data format based on camera_preview_format */
  switch (camera_preview_format)
  {
  case CAMERA_YCBCR_4_2_0:
    data_type = QCAMRAW_H2V2;
    y_size    = camera_preview_dx * camera_preview_dy;
    y_pad     = y_size % 4;
    cbcrsize  = camera_preview_dx * camera_preview_dy / 2;

    y_size_encode   = camera_dx * camera_dy;
    cbcrsize_encode = camera_dx * camera_dy / 2;
    y_pad_encode    = y_size_encode % 4;
    break;
  case CAMERA_YCBCR_4_2_2:
    data_type = QCAMRAW_H2V1;
    y_size    = camera_preview_dx * camera_preview_dy;
    y_pad     = y_size % 4;
    cbcrsize  = camera_preview_dx * camera_preview_dy;

    y_size_encode   = camera_dx * camera_dy;
    cbcrsize_encode = camera_dx * camera_dy;
    y_pad_encode    = y_size_encode % 4;
    break;
  default:
     MSG_ERROR ("Unsupported preview format %d.", camera_preview_format, 0, 0);
     return;
  }

  if ((camera_state == CAMERA_STATE_PREVIEW) ||
      (camera_state == CAMERA_STATE_QVP_ENCODING))
  {
    rotation = camera_default_preview_rotation;
  }
  else
  {
    rotation = video_encode_rotation;
  }
  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    qcamrawSetHeader((qcamrawHeaderType *)camera_preview_buffers.buffers[current_buffer].buf_ptr,
                      current_buffer, camera_preview_buffer_size, data_type, QCAMRAW_PREVIEW_FRAME,
                      camera_reflect, rotation, pic_dx, pic_dy, (uint16)camera_preview_dx,
                     (uint16)camera_preview_dy, 0, 0, 0, 0, y_size, y_pad, cbcrsize,
                     (uint8 *)((uint32)camera_preview_buffers.buffers[current_buffer].buf_ptr +
                               qcamrawHeaderSize + 0 + y_size + y_pad));
  }
#ifdef FEATURE_QVPHONE
  if(camera_state == CAMERA_STATE_QVP_ENCODING && qvp_enc_buffer0 &&
     qvp_enc_buffer1 && qvp_enc_buffer2 && qvp_enc_buffer3)
  {
    qcamrawSetHeader((qcamrawHeaderType *)qvp_enc_buffer0, 0,
                     camera_preview_buffer_size, data_type, QCAMRAW_PREVIEW_FRAME,
                     camera_reflect, rotation, pic_dx, pic_dy,
                     (uint16)camera_preview_dx, (uint16)camera_preview_dy, 0, 0, 0, 0,
                     y_size, y_pad, cbcrsize,
                     (uint8 *)((uint32)qvp_enc_buffer0 + qcamrawHeaderSize + 0 + y_size + y_pad));

    qcamrawSetHeader((qcamrawHeaderType *)qvp_enc_buffer1, 0,
                     camera_preview_buffer_size, data_type, QCAMRAW_PREVIEW_FRAME,
                     camera_reflect, rotation, pic_dx, pic_dy,
                     (uint16)camera_preview_dx, (uint16)camera_preview_dy, 0, 0, 0, 0,
                     y_size, y_pad, cbcrsize,
                     (uint8 *)((uint32)qvp_enc_buffer1 + qcamrawHeaderSize + 0 + y_size + y_pad));

    qcamrawSetHeader((qcamrawHeaderType *)qvp_enc_buffer2, 0,
                     camera_preview_buffer_size, data_type, QCAMRAW_PREVIEW_FRAME,
                     camera_reflect, rotation, pic_dx, pic_dy,
                     (uint16)camera_preview_dx, (uint16)camera_preview_dy, 0, 0, 0, 0,
                     y_size, y_pad, cbcrsize,
                     (uint8 *)((uint32)qvp_enc_buffer2 + qcamrawHeaderSize + 0 + y_size + y_pad));

    qcamrawSetHeader((qcamrawHeaderType *)qvp_enc_buffer3, 0,
                     camera_preview_buffer_size, data_type, QCAMRAW_PREVIEW_FRAME,
                     camera_reflect, rotation, pic_dx, pic_dy,
                     (uint16)camera_preview_dx, (uint16)camera_preview_dy, 0, 0, 0, 0,
                     y_size, y_pad, cbcrsize,
                     (uint8 *)((uint32)qvp_enc_buffer3 + qcamrawHeaderSize + 0 + y_size + y_pad));
  }
#endif /* FEATURE_QVPHONE */
#ifdef FEATURE_VIDEO_ENCODE
  if (camera_state == CAMERA_STATE_RECORDING)
  {
    VideoBufferHandle *videoBufferPtr;
    int i;

    videoBufferPtr = &encodeBufferHandle;

    for (i = 0; i < videoBufferPtr->listSize; i++)
    {
      uint16 dx, dy;

      dx = camera_dx;
      dy = camera_dy;

      qcamrawSetHeader((qcamrawHeaderType *)videoBufferPtr->pool.bufferList[i], 0,
                         camera_encode_buffer_size, data_type, QCAMRAW_PICTURE_FRAME,
                         camera_reflect, rotation, dx, dy,
                         dx, dy, 0, 0, 0, 0, y_size_encode, y_pad_encode, cbcrsize_encode,
                         (uint8 *)((uint32)(videoBufferPtr->pool.bufferList[i]) +
                         qcamrawHeaderSize + 0 + y_size_encode + y_pad_encode));
      videoBufferPtr->pool.freePool [i].Y_Address    = (uint32 *)
          qcamrawGetDataYPtr (videoBufferPtr->pool.bufferList[i]);
      videoBufferPtr->pool.freePool [i].CbCr_Address = (uint32 *)
          qcamrawGetDataCbCrPtr (videoBufferPtr->pool.bufferList[i]);
    }
  }
#endif /* FEATURE_VIDEO_ENCODE */
} /* camera_set_preview_header */

/*===========================================================================

FUNCTION      CAMERA_SET_PREVIEW_HEADERS

DESCRIPTION
              Set the raw data format header for the preview frame.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

static void camera_set_preview_headers_rotation(void)
{
  uint16 pic_dx, pic_dy;
  uint16  rotation;
  uint32 current_buffer;
  
  if ((camera_default_preview_rotation == 0) || (camera_default_preview_rotation == 180))
  {
    camera_default_orientation = CAMERA_ORIENTATION_LANDSCAPE;
    camera_odd_rotation        = FALSE;
  }
  else
  {
    camera_default_orientation = CAMERA_ORIENTATION_PORTRAIT;
    camera_odd_rotation        = TRUE;
  }

  if (camera_odd_rotation)
  {
    pic_dx = (uint16) camera_preview_dy;
    pic_dy = (uint16) camera_preview_dx;
  }
  else
  {
    pic_dx = (uint16) camera_preview_dx;
    pic_dy = (uint16) camera_preview_dy;
  }

  if ((camera_state == CAMERA_STATE_PREVIEW) ||
      (camera_state == CAMERA_STATE_QVP_ENCODING))
  {
    rotation = camera_default_preview_rotation;
  }
  else
  {
    rotation = video_encode_rotation;
  }
  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    if(((qcamrawHeaderType *)camera_preview_buffers.buffers[current_buffer].buf_ptr) != NULL)
    {
        ((qcamrawHeaderType *)camera_preview_buffers.buffers[current_buffer].buf_ptr)->rotation = rotation;
        ((qcamrawHeaderType *)camera_preview_buffers.buffers[current_buffer].buf_ptr)->picWidth = pic_dx;
        ((qcamrawHeaderType *)camera_preview_buffers.buffers[current_buffer].buf_ptr)->picHeight = pic_dy;	
    }
  }
} /* camera_set_preview_header */

/*===========================================================================

FUNCTION      CAMERA_PAD_ENCODE_OVERLAY_FRAME

DESCRIPTION
              Pad the encode overlay frame boundary with transparent color.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_svcs_pad_encode_overlay_frame
(
 camera_encode_overlay_type *frame
)
{
  uint32 color = (camera_transparent_color << 16) + camera_transparent_color;
  uint32 i, count;

  /* Frame is always RGB565 */
  if (frame->pad_lines)
  {
    /* Pad the top and bottom pad_lines with
     * transparent color two pixels at a time. */
    uint32 *dst1 = (uint32*)&frame->imgPtr[0];
    uint32 *dst2 = (uint32*)&frame->imgPtr[frame->padded_height*frame->padded_width*2];

    count = frame->pad_lines*frame->padded_width/2;
    for (i=0; i<count; i++)
    {
      dst1[i] = color;
    }
    dst1 = (uint32*)&frame->imgPtr[0];
    memcpy(dst2, dst1, count*4);
  }
  if (frame->pad_pixels != 0)
  {
    uint32 line;
    count = frame->pad_pixels/2;
    for (line = 0; line < frame->padded_height; line++)
    {
      uint32 *buf = (uint32*)&(frame->imgPtr[line * frame->padded_width * 2]);
      for (i = 0; i < count; i++)
      {
        buf[i] = color;
      }
    }
  }
}

#ifdef FEATURE_MDP
/*===========================================================================

FUNCTION      CAMERA_SVCS_FILL_FRAME

DESCRIPTION
              Fill the whole frame with transparent color.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_svcs_fill_buffer_16
(
 byte *buffer,
 uint16 color,
 uint32 width,
 uint32 height
)
{
  uint32 dcolor = (color << 16) + color;
  uint32 *dst1 = (uint32*)buffer;
  uint32 *dst2 = (uint32*)buffer;
  uint32 i;

  for (i=0; i < (uint32)(width/2); i++)
  {
    dst2[i] = dcolor;
  }
  for (i=1; i<height; i++)
  {
    memcpy(dst2, dst1, width*2);
    dst2 += (uint32)(width/2);
  }
}
#endif /* FEATURE_MDP */

/*===========================================================================

FUNCTION      CAMERA_SVCS_CREATE_ENCODE_OVERLAY

DESCRIPTION
              Create the encode overlay

RETURN VALUE
  True or False

SIDE EFFECTS
  None

===========================================================================*/
static camera_ret_code_type camera_svcs_create_encode_overlay
(
 ipl_image_type *overlay
)
{
  ipl_rect_type src_area, dst_area;

  encode_overlay = (camera_encode_overlay_type *) CAM_Malloc (sizeof (camera_encode_overlay_type));
  if (encode_overlay == NULL)
  {
    MSG_HIGH("camera_create_encode_overlay: encode_overlay allocation failed", 0, 0, 0);
    return CAMERA_FAILED;
  }
  encode_overlay->padded_width = CEILING16(overlay->dx);
  encode_overlay->padded_height = CEILING16(overlay->dy);
  encode_overlay->pad_pixels = encode_overlay->padded_width - overlay->dx;
  encode_overlay->pad_lines = encode_overlay->padded_height - overlay->dy;
  /* Please see camera_encode_overlay_type for more detail of why
   * we need the paddings. */
  encode_overlay->size = (encode_overlay->padded_width * (encode_overlay->pad_lines + encode_overlay->padded_height) + encode_overlay->pad_pixels) * 2;
  encode_overlay->imgPtr = CAM_Malloc (encode_overlay->size  | CAM_ALLOC_NO_ZMEM);
  if (encode_overlay->imgPtr == NULL)
  {
    CAM_Free(encode_overlay);
    encode_overlay = NULL;
    MSG_HIGH("camera_create_encode_overlay: encode_overlay->imgPtr allocation failed", 0, 0, 0);
    return CAMERA_NO_MEMORY;
  }
  encode_overlay->ipl_image.imgPtr = encode_overlay->imgPtr;
  encode_overlay->ipl_image.cFormat = overlay->cFormat;
  encode_overlay->ipl_image.clrPtr = NULL;
  encode_overlay->ipl_image.dx = encode_overlay->padded_width;
  encode_overlay->ipl_image.dy = encode_overlay->padded_height;

  /* Pad the buffer with transparent color, copy the overlay to the middle. */
  camera_svcs_pad_encode_overlay_frame(encode_overlay);
  src_area.x = 0;
  src_area.y = 0;
  src_area.dx = overlay->dx;
  src_area.dy = overlay->dy;
  dst_area.x = encode_overlay->pad_pixels;
  dst_area.y = encode_overlay->pad_lines;
  dst_area.dx = overlay->dx;
  dst_area.dy = overlay->dy;
  if (ipl_copy_and_paste(overlay, &encode_overlay->ipl_image, &src_area, &dst_area) != IPL_SUCCESS)
  {
    camera_clr_overlays ();
    return CAMERA_FAILED;
  }
  camera_svcs_adjust_overlay_for_rotation();
  return CAMERA_SUCCESS;
} /* camera_svcs_create_encode_overlay */

/*===========================================================================

FUNCTION      CAMERA_SVCS_CREATE_THUMBNAIL_OVERLAY

DESCRIPTION
              Set the thumbnail overlay

RETURN VALUE
  True or False

SIDE EFFECTS
  None

===========================================================================*/
static camera_ret_code_type camera_svcs_create_thumbnail_overlay
(
 ipl_image_type   *overlay
)
{
  ipl_rect_type src_area, dst_area;
  ipl_image_type work_buffer;
  ipl_status_type status;

  camera_svcs_clr_thumbnail_overlay();

  thumbnail_overlay = (camera_encode_overlay_type *) CAM_Malloc (sizeof (camera_encode_overlay_type));
  if (thumbnail_overlay == NULL)
  {
    MSG_HIGH("camera_create_thumbnail_overlay: thumbnail_overlay allocation failed", 0, 0, 0);
    return CAMERA_FAILED;
  }
  thumbnail_overlay->padded_width = CEILING16(thumbnail_width);
  thumbnail_overlay->padded_height = CEILING16(thumbnail_height);
  thumbnail_overlay->pad_pixels = thumbnail_overlay->padded_width - thumbnail_width;
  thumbnail_overlay->pad_lines = thumbnail_overlay->padded_height - thumbnail_height;
  /* Please see camera_encode_overlay_type for more detail of why
   * we need the paddings. */
  thumbnail_overlay->size = (thumbnail_overlay->padded_width * (thumbnail_overlay->pad_lines + thumbnail_overlay->padded_height) + thumbnail_overlay->pad_pixels) * 2;
  src_area.x = 0;
  src_area.y = 0;
  src_area.dx = thumbnail_width;
  src_area.dy = thumbnail_height;
  dst_area.x = thumbnail_overlay->pad_pixels;
  dst_area.y = thumbnail_overlay->pad_lines;
  dst_area.dx = thumbnail_width;
  dst_area.dy = thumbnail_height;
  thumbnail_overlay->imgPtr = CAM_Malloc (thumbnail_overlay->size | CAM_ALLOC_NO_ZMEM);
  if (thumbnail_overlay->imgPtr == NULL)
  {
    CAM_Free(thumbnail_overlay);
    thumbnail_overlay = NULL;
    MSG_HIGH("camera_create_thumbnail_overlay: thumbnail_overlay->imgPtr allocation failed", 0, 0, 0);
    return CAMERA_NO_MEMORY;
  }
  thumbnail_overlay->ipl_image.imgPtr = thumbnail_overlay->imgPtr;
  thumbnail_overlay->ipl_image.cFormat = overlay->cFormat;
  thumbnail_overlay->ipl_image.clrPtr = NULL;
  thumbnail_overlay->ipl_image.dx = thumbnail_overlay->padded_width;
  thumbnail_overlay->ipl_image.dy = thumbnail_overlay->padded_height;

  /* Pad the buffer with transparent color, copy the overlay to the middle. */
  camera_svcs_pad_encode_overlay_frame(thumbnail_overlay);

  if ((overlay->dx != thumbnail_width) || (overlay->dy != thumbnail_height))
  {
    /* Need to resize the frame to thumbnail window size */
    work_buffer.dx = thumbnail_width;
    work_buffer.dy = thumbnail_height;
    work_buffer.cFormat = IPL_RGB565;
    work_buffer.imgPtr = CAM_Malloc(thumbnail_overlay->size | CAM_ALLOC_NO_ZMEM);
    work_buffer.clrPtr = NULL;
    if (work_buffer.imgPtr == NULL)
    {
      camera_svcs_clr_thumbnail_overlay();
      MSG_HIGH("camera_create_thumbnail_overlay: work_buffer.imgPtr allocation failed", 0, 0, 0);
      return CAMERA_NO_MEMORY;
    }
    status = ipl_resizeFrame(overlay, &work_buffer, NULL, camera_transparent_color);
    if (status != IPL_SUCCESS)
    {
      CAM_Free(work_buffer.imgPtr);
      camera_svcs_clr_thumbnail_overlay();
      MSG_HIGH("camera_create_thumbnail_overlay: ipl resize failed",0,0,0);
      return CAMERA_FAILED;
    }
    status = ipl_copy_and_paste(&work_buffer, &thumbnail_overlay->ipl_image, &src_area, &dst_area);
    CAM_Free(work_buffer.imgPtr);
  }
  else
  {
    status = ipl_copy_and_paste(overlay, &thumbnail_overlay->ipl_image, &src_area, &dst_area);
  }
  if (status == IPL_SUCCESS)
  {
    camera_svcs_adjust_overlay_for_rotation();
    return CAMERA_SUCCESS;
  }
  else
  {
    camera_svcs_clr_thumbnail_overlay ();
    return CAMERA_FAILED;
  }
} /* camera_svcs_create_thumbnail_overlay */


/*===========================================================================

FUNCTION      CAMERA_SVCS_CREATE_PREVIEW_OVERLAY

DESCRIPTION
              Set the icons and preview overlay. If overlay and icon display
              dimensions are the same, then merge overlay and icons
              for preview overlay.

RETURN VALUE
  True or False

SIDE EFFECTS
  None

===========================================================================*/
static camera_ret_code_type camera_svcs_create_preview_overlay
(
ipl_image_type   *overlay
)
{
  uint32          size;
  ipl_image_type  img;
  uint16          overlay_rotation = 0;

  if (overlay == NULL)
  {
    return CAMERA_FAILED;
  }
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  MSG_HIGH("camera_create_preview_overlay: enters",0,0,0);
  if (preview_mode == CAMERA_PREVIEW_MODE_MOVIE)
  {
    return camcorder_create_preview_overlay(overlay,
                                            camera_default_preview_rotation,
                                            camera_transparent_color);
  }
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */

  size = camera_preview_dx * camera_preview_dy * 2;

  camera_svcs_clr_preview_overlay();
  preview_overlay = (ipl_image_type *) CAM_Malloc (sizeof (ipl_image_type));
  if (preview_overlay == NULL)
  {
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
    blt_icons = camera_icons;
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
    return CAMERA_NO_MEMORY;
  }
  preview_overlay->imgPtr = CAM_Malloc(size | CAM_ALLOC_NO_ZMEM);
  if (preview_overlay->imgPtr == NULL)
  {
    /* No overlay buffer */
    CAM_Free (preview_overlay);
    preview_overlay = NULL;
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
    blt_icons       = camera_icons;
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
    MSG_HIGH("camera_create_preview_overlay: memory allocation for preview_overlay failed",0,0,0);
    return CAMERA_NO_MEMORY;
  }

  /* First, create preview overlay in the same mode as input overlay */
  if (overlay->dx < overlay->dy)
  {
    /* Portrait mode */
    preview_overlay->dx=camera_preview_dy;
    preview_overlay->dy=camera_preview_dx;
  }
  else
  {
    /* Landscape mode */
    preview_overlay->dx      = camera_preview_dx;
    preview_overlay->dy      = camera_preview_dy;
  }
  preview_overlay->cFormat = overlay->cFormat;

  /* Resize the frame to preview window size */
  if (scale_preview_overlay)
  {
    if (ipl_resizeFrame(overlay,preview_overlay,NULL,camera_transparent_color) != IPL_SUCCESS)
    {
      camera_svcs_clr_preview_overlay();
      MSG_HIGH("camera_create_preview_overlay: IPL Resize failed",0,0,0);
      return CAMERA_FAILED;
    }
  }
  else
  {
    memcpy(preview_overlay->imgPtr, overlay->imgPtr, size);
  }

  /* Second, rotate overlay frame to match the preview mode. */
  if (camera_default_preview_rotation == 0 || camera_default_preview_rotation == 180)
  {
    if (preview_overlay->dx < preview_overlay->dy)
    {
      /* Preview in landscape mode, overlay image is in portrait.
       * So need to rotate 270 */
      overlay_rotation = 270;
    }
  }
  else
  {
    if (preview_overlay->dx > preview_overlay->dy)
    {
      /* Preview in portrait mode, overlay image is in landscape.
       * So need to rotate 90 */
      overlay_rotation = 90;
    }
  }

  if (overlay_rotation > 0 )
  {
    ipl_rotate90_type ipl_rotation = IPL_NOROT;

    switch (overlay_rotation)
    {
      case 90:
        ipl_rotation = IPL_ROT90;
        break;
      case 270:
      default:
        ipl_rotation = IPL_ROT270;
        break;
    }
    img.dx      = preview_overlay->dy;
    img.dy      = preview_overlay->dx;
    img.imgPtr  = CAM_Malloc(size | CAM_ALLOC_NO_ZMEM);
    img.cFormat = preview_overlay->cFormat;
    if (img.imgPtr == NULL)
    {
      /* No overlay buffer */
      camera_svcs_clr_preview_overlay();
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
      blt_icons       = camera_icons;
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
      return CAMERA_NO_MEMORY;
    }

    if (ipl_rotate90_frame (preview_overlay, &img, ipl_rotation, camera_transparent_color) != IPL_SUCCESS)
    {
      camera_svcs_clr_preview_overlay();
      CAM_Free (img.imgPtr);
      MSG_HIGH("camera_create_preview_overlay: IPL rotation failed",0,0,0);
      return CAMERA_FAILED;
    }
    CAM_Free (preview_overlay->imgPtr);
    preview_overlay->imgPtr  = img.imgPtr;
    preview_overlay->dx      = img.dx;
    preview_overlay->dy      = img.dy;
  }
  if (camera_reflect_ipl != IPL_NOREF)
  {
    /* For mirror/water reflect, need to reflect the overlay frame for preview */
    (void) ipl_reflect (preview_overlay, NULL, camera_reflect_ipl);
  }
  return CAMERA_SUCCESS;
}

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
/*===========================================================================

FUNCTION      CAMERA_SET_ICON_AND_PREVIEW_OVERLAY

DESCRIPTION
              Set the icons and preview overlay. If overlay and icon display
              dimensions are the same, then merge overlay and icons
              for preview overlay.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

static camera_ret_code_type camera_set_icon_and_preview_overlay(void)
{
  camera_ret_code_type ret_val;

  if (icon_only_overlay != NULL)
  {
    ret_val = camera_svcs_create_preview_overlay(icon_only_overlay);
  }
  else
  {
    if (ui_overlay.imgPtr == NULL)
    {
      /* No camera overlay, clear all overlays */
      camera_clr_overlays ();
      blt_icons = camera_icons;
      return CAMERA_SUCCESS;
    }

    ret_val = camera_svcs_create_preview_overlay(&ui_overlay);
    if (ret_val == CAMERA_SUCCESS)
    {
      if ((preview_overlay->dx == camera_icon_dx) &&
          (preview_overlay->dy  == camera_icon_dy))
      {
        /* Since preview dimension and icon display dimension
         * are the same, merge preview and icons. */
        (void) ipl_frame_compose (preview_overlay,
                                  camera_icons,
                                  camera_transparent_color,
                                  preview_overlay);
        blt_icons = 0;
      }
      else
      {
        blt_icons = camera_icons;
      }
    }
  }
  return ret_val;
} /* camera_set_icon_and_preview_overlay */
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */

/*===========================================================================

FUNCTION      CAMERA_CONFIG_VFE

DESCRIPTION
              Configures VFE.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean camera_config_vfe(void)
{
  uint32          output1_width = 0, output1_height = 0;
  uint32          output2_width = 0, output2_height = 0;
  boolean         ret_val = FALSE;
  boolean         sensor_config_stat = FALSE;
#ifdef FEATURE_VIDEO_ENCODE
  boolean         config_not_due_to_init;
#endif
  boolean _flag;

  MSG_CAMERADEBUG("CAMERA_SVCS: Construct CONFIG_VFE msg and send it to DSP camera_state:%d OPMODE:%d", camera_state, camera_camsensor_op_mode, 0);

#ifdef FEATURE_MDP
  if ( camera_vfe_control_and_status.reconfigure_vfe == FALSE )
  {
#ifdef FEATURE_MMOVERLAY
    camera_deregister_video (FALSE);
#else /* FEATURE_MMOVERLAY */
    camera_deregister_video (TRUE);
#endif /* !FEATURE_MMOVERLAY */
  }
#endif /* FEATURE_MDP */


#ifdef FEATURE_VIDEO_ENCODE
  config_not_due_to_init = camera_vfe_control_and_status.reconfigure_vfe;
#endif

  camera_vfe_control_and_status.reconfigure_vfe = FALSE;

  if (runningOutput2SkipPattern != 0)
  {
    vfeUpdate.output2SkipPattern.skipBitPattern  = runningOutput2SkipPattern;
    runningOutput2SkipPattern = 0;
  }


#ifdef FEATURE_VIDEO_ENCODE
  if (camera_state == CAMERA_STATE_RECORDING)
  {
    /* VFE cannot be re-setted while we are waiting for a the current VFE
    ** frame to complet processing.  After receiving both the handshake done
    ** and special effects done in output2enc_ack2_criteria() we can
    ** proceed to reset the VFE.
    **
    ** camera_recording_vfe_reconfig_permitted() is the API to this state
    ** machine.
    */
    if (!camera_recording_vfe_reconfig_permitted(config_not_due_to_init))
    {
      return TRUE;
    }

    MSG_CAMQDSPIO("reconfig permitted",0,0,0);
    (void) VideoBuffersReset (&output1BufferHandle);
    (void) VideoBuffersReset (&encodeBufferHandle);
  }
#endif /* FEATURE_VIDEO_ENCODE */
  if (!((camera_state == CAMERA_STATE_PREVIEW)      ||
        (camera_state == CAMERA_STATE_QVP_ENCODING) ||
        (camera_state == CAMERA_STATE_RECORDING)    ||
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
        (camera_state == CAMERA_STATE_TAKE_PICTURE)))
  {
    camera_log (LOG_INFO, 0, 0, (uint32)CFG_REJ, __LINE__);
    camera_terminate (CAMERA_EXIT_CB_ERROR, (int)CAMERA_ERROR_INVALID_FORMAT);
    MSG_ERROR ("Invalid config parm. State transition to ready", 0, 0, 0);
    event_report (EVENT_CAMERA_INVALID_CONFIG_PARM);
    return FALSE;
  }


#ifdef FEATURE_VIDEO_ENCODE
  if ((camera_state == CAMERA_STATE_RECORDING) &&
      camera_video_engine_paused != CAMERA_PAUSING_STATE_NOT_PAUSED )
  {
    camera_camsensor_op_mode = CAMSENSOR_OP_MODE_PREVIEW;
  }
#endif /* FEATURE_VIDEO_ENCODE */

  /* Initialize AEC/AWB stats processing defer count */
  camera_3a_shared_set_aec_awb_delay_count (0, AEC_AWB_DELAY_INIT);

  output1_width  = 0;
  output1_height = 0;
  output2_width  = 0;
  output2_height = 0;
  switch (camera_camsensor_op_mode)
  {
    case CAMSENSOR_OP_MODE_RAW_SNAPSHOT:
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/
      break;

    case CAMSENSOR_OP_MODE_SNAPSHOT:

      output1_width  = thumbnail_width;
      output1_height = thumbnail_height;
      output2_width  = orig_picture_dx;
      output2_height = orig_picture_dy;
      break;

    case CAMSENSOR_OP_MODE_PREVIEW:
    case CAMSENSOR_OP_MODE_VIDEO:
      output1_width  = camera_preview_dx;
      output1_height = camera_preview_dy;
      break;
  }

  sensor_config_stat = camsensor_config(camera_camsensor_op_mode, &camsensor_static_params[camera_asi]);

  if (sensor_config_stat)
  {
    ret_val = vfe_copy_sensor_config(
               camera_camsensor_op_mode,
               (uint16) output1_width,
               (uint16) output1_height,
               (uint16) output2_width,
               (uint16) output2_height,
               &camsensor_static_params[camera_asi]
               );

    if (camsensor_static_params[camera_asi].discardFirstFrame == TRUE)
    {
      camera_calculate_and_set_capture_delay(1);
    }
  }
  if (ret_val == FALSE)
  {
    camera_log (LOG_INFO, 0, 0, (uint32)CFG_REJ, __LINE__);
#ifdef FEATURE_VIDEO_ENCODE
    if (camera_state == CAMERA_STATE_RECORDING)
    {
        MSG_ERROR ("Cannot config.", 0, 0, 0);
    }
    else
#endif
    {
    camera_terminate (CAMERA_EXIT_CB_ERROR, (int)CAMERA_ERROR_INVALID_FORMAT);
    MSG_ERROR ("Invalid config parm. State transition to ready", 0, 0, 0);
    event_report (EVENT_CAMERA_INVALID_CONFIG_PARM);
    }
    return FALSE;
  }

  /* In the event of preview, ensure that output2 frames are
  ** disabled because camera layer only expects output1 frames.
  ** This removes reliance on camsensor drivers to disable output2
  ** and centralizes enabling/disabling output2 in camera services.
  */
  if (camera_state == CAMERA_STATE_PREVIEW)
  {
    vfeVideoConfig.output2Configuration.output2Enable    = VFE_Disable;
    vfeVideoConfig.output2Configuration.startBackend     = VFE_Disable;
    vfeVideoConfig.output2Configuration.width            = 0;
    vfeVideoConfig.output2Configuration.height           = 0;
    vfeVideoConfig.output2Buffer.Y_OutputTripleBuffer0   = NULL;
    vfeVideoConfig.output2Buffer.CbCrOutputTripleBuffer0 = NULL;
    vfeVideoConfig.output2Buffer.Y_OutputTripleBuffer1   = NULL;
    vfeVideoConfig.output2Buffer.CbCrOutputTripleBuffer1 = NULL;
    vfeVideoConfig.output2Buffer.Y_OutputTripleBuffer2   = NULL;
    vfeVideoConfig.output2Buffer.CbCrOutputTripleBuffer2 = NULL;
#if !defined QDSP_MODULE_VFE25_DEFINED
    vfeVideoConfig.f3x3LumaFilter.enable                 = VFE_Disable;
#endif /* nQDSP_MODULE_VFE25_DEFINED */
  }
#ifdef FEATURE_VIDEO_ENCODE
  /* If we're recording, setup output2 parameters */
  else if (camera_state == CAMERA_STATE_RECORDING)
  {
   camcorder_config_vfe();
  }
#endif /* FEATURE_VIDEO_ENCODE */
#ifdef FEATURE_QVPHONE
  else if(camera_state == CAMERA_STATE_QVP_ENCODING)
  {
    vfeVideoConfig.output2Configuration.output2Enable  = VFE_Enable;
#ifdef FEATURE_QVP_OFFLINE_VIDEO_ENCODING
#error code not present
#else
    vfeVideoConfig.output2Configuration.startBackend   = VFE_Enable;
#endif // #ifdef FEATURE_QVP_OFFLINE_VIDEO_ENCODING
    vfeVideoConfig.output2Configuration.width          = camera_dx;
    vfeVideoConfig.output2Configuration.height         = camera_dy;
    vfeVideoConfig.output2Configuration.format         = VFE_O2YCbCr420_LinePack;
    vfeVideoConfig.output2Buffer.Y_OutputTripleBuffer0   = (VFE_AddressType) qcamrawGetDataYPtr    (qvp_enc_buffer0);
    vfeVideoConfig.output2Buffer.CbCrOutputTripleBuffer0 = (VFE_AddressType) qcamrawGetDataCbCrPtr (qvp_enc_buffer0);
    vfeVideoConfig.output2Buffer.Y_OutputTripleBuffer1   = (VFE_AddressType) qcamrawGetDataYPtr    (qvp_enc_buffer1);
    vfeVideoConfig.output2Buffer.CbCrOutputTripleBuffer1 = (VFE_AddressType) qcamrawGetDataCbCrPtr (qvp_enc_buffer1);
    vfeVideoConfig.output2Buffer.Y_OutputTripleBuffer2   = (VFE_AddressType) qcamrawGetDataYPtr    (qvp_enc_buffer2);
    vfeVideoConfig.output2Buffer.CbCrOutputTripleBuffer2 = (VFE_AddressType) qcamrawGetDataCbCrPtr (qvp_enc_buffer2);
    vfeVideoConfig.videoOutput2FrameSkip                 = vfeVideoConfig.videoOutput1FrameSkip;
    vfeUpdate.colorConversion = vfeVideoConfig.colorConversion;
  }
#endif /* FEATURE_QVPHONE */

#ifdef FEATURE_STROBE_FLASH
  /* Discard first frame if flash needs to fire */
  if ((camera_fire_flash) && (camera_state == CAMERA_STATE_TAKE_PICTURE)) 
  {
    uint16  gain;
    uint16  gain_factor;
    uint32  line_count;

    /* For strobe flash, needs to discard the first frame */
    camera_calculate_and_set_capture_delay(1); /* 1 means discard 1 (first) frame */

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
    line_count = camsensor_static_params[camera_asi].chromatix_parms->exposure_table[cam3a_aec_state.exp_index].line_count;
#endif

    /* for Casio sesnor line_count 0 means full frame integration */
    if ((line_count !=0) &&
        (line_count != camsensor_static_params[camera_asi].full_size_height))
    {
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
      gain = camsensor_static_params[camera_asi].chromatix_parms->exposure_table[cam3a_aec_state.exp_index].gain;
#endif /* CAMERA_USES_CHROMATIX_0x203 */
      gain_factor = camsensor_static_params[camera_asi].full_size_height / line_count;
      if (gain_factor == 0) gain_factor = 1;
      camsensor_static_params[camera_asi].aec_enable = FALSE;

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
      camsensor_write_exposure_gain((
      camsensor_static_params[camera_asi].chromatix_parms->exposure_table[cam3a_aec_state.exp_index].gain / gain_factor),
      camsensor_static_params[camera_asi].full_size_height);
#endif /* CAMERA_USES_CHROMATIX_0x203 */

    }

  }
#endif /* FEATURE_STROBE_FLASH */

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

  if ((camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT) ||
       (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT))
  {

#ifdef FEATURE_STROBE_FLASH
    camera_parm_info_type current_iso_parm;
    boolean iso_changed = FALSE;
    /* Set ISO mode to auto if using flash. We need to restore this setting
     * after snapshot is finished
     */
    if( camera_fire_flash )
    {
      /* ISO may not be supported, if that is the case iso_changed should remain
       * false so we will not try to restore any ISO settings
       */
      if( camera_aec_get_parm(CAMERA_PARM_ISO, &current_iso_parm) == CAMERA_SUCCESS )
      {
        if( camera_aec_set_parm(CAMERA_PARM_ISO, CAMERA_ISO_AUTO) == CAMERA_SUCCESS )
          iso_changed = TRUE;
      }
    }
#endif /* FEATURE_STROBE_FLASH */
#ifdef FEATURE_CAMERA_HJR
    if (camera_svcs_hjr_enabled () == TRUE )
    {
      camera_aec_adjust_exposure_as_per_hjr ();
    }
    else
#endif /* FEATURE_CAMERA_HJR */
    if (camera_aec_adjust_exposure_for_snapshot() != CAMERA_SUCCESS)
      MSG_CAMERADEBUG("Failed to adjust Snapshot Exposure", 0, 0, 0);

#ifdef FEATURE_STROBE_FLASH
    /* Restore ISO setting if we changed due to flash now that sensor has been
     * configured properly for the snapshot */
    if( iso_changed )
    {
      if( camera_aec_set_parm(CAMERA_PARM_ISO, current_iso_parm.current_value) != CAMERA_SUCCESS )
        MSG_ERROR("Failed to restore ISO settings after flash", 0, 0, 0);
    }
#endif /* FEATURE_STROBE_FLASH */
  }

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

  camera_log (LOG_VFEC, 0, 0, (uint32)VFE_CFG, __LINE__);

#ifdef FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK
  if ((camera_camsensor_op_mode == CAMSENSOR_OP_MODE_PREVIEW) ||
      (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_VIDEO) ||
      (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT) ||
      (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT)
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/
      )
  {
    camera_svcs_set_vfe_clk (VFE_MODE_CONFIG);
  }
#endif /* FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK */

  /* setup the remaining vfe parameters and configure vfe */
  switch (camera_camsensor_op_mode)
  {
    case CAMSENSOR_OP_MODE_PREVIEW:
    case CAMSENSOR_OP_MODE_VIDEO:
      MSG_CAMERADEBUG("Preview has been started ", 0, 0, 0);
      if (camsensor_static_params[camera_asi].output_format != CAMSENSOR_YCBCR)
      {
        /* recall the latest AEC/AWB gain and apply it into VFE */
        camera_restore_config_for_preview();
      }
      _flag = camera_svcs_setparam_vfe_video();
#ifndef FEATURE_CAMERA_YCBCR_ONLY
      camera_aec_apply_digital_gain_for_preview(&vfeVideoConfig.colorCorrection);
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
      ret_val = ( _flag && (VFE_ConfigureVideo (&vfeVideoConfig) == CAMQDSP_SUCCESS));
      break;


    case CAMSENSOR_OP_MODE_SNAPSHOT:
#if (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED)
      if ( (camera_svcs_hjr_enabled() == TRUE) && (HJR_structure.new_numberFrames == 1)
           && (camctrl_tbl.hjr_bayer_filtering_enable == TRUE) )
      {
        _flag = camera_svcs_setparam_vfe_offline_snapshot();
#ifndef FEATURE_CAMERA_YCBCR_ONLY
        camera_aec_apply_digital_gain_for_iso_snapshot(&vfeOfflineConfig.colorCorrection);
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
        ret_val = ( _flag && (VFE_ConfigureOffline (&vfeOfflineConfig) == CAMQDSP_SUCCESS));
      }
      else
#endif /* (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED ) */
      {
        _flag = camera_svcs_setparam_vfe_snapshot();
#ifndef FEATURE_CAMERA_YCBCR_ONLY
        camera_aec_apply_digital_gain_for_iso_snapshot(&vfeSnapshotConfig.colorCorrection);
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
        ret_val = ( _flag && (VFE_ConfigureSnapshot (&vfeSnapshotConfig) == CAMQDSP_SUCCESS)
                  );
      }
      break;

    case CAMSENSOR_OP_MODE_RAW_SNAPSHOT:
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/		
      ret_val = ( (camera_svcs_setparam_vfe_raw_snapshot()) &&
                  (VFE_ConfigureRawSnapshot (&vfeRawConfig) == CAMQDSP_SUCCESS)
                );
      break;
  }

  if ((camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT) ||
      (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT)
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/      
      )
  {
    uint16 snapshot_fps, preview_fps;
    preview_fps = camctrl_tbl.camsensor_params_ptr->preview_fps;
    if (camctrl_tbl.get_snapshot_frame_per_sec)
    {
      snapshot_fps = camctrl_tbl.get_snapshot_frame_per_sec(preview_fps);
    }
    else
    {
      /* Assume snapshot fps as half of the preview fps when snapshot fps is not available */
      snapshot_fps = preview_fps / 2;
    }
    MSG_HIGH("Snapshot FPS = %d", snapshot_fps >> 8, 0, 0);
  }

  if (ret_val == FALSE)
  {
    camera_log (LOG_INFO, 0, 0, (uint32)CFG_REJ, __LINE__);
#ifdef FEATURE_VIDEO_ENCODE
    if (camera_state == CAMERA_STATE_RECORDING)
    {
      MSG_ERROR ("Cannot config vfe.", 0, 0, 0);
    }
    else
#endif
    {
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
    {
      camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
    }
    MSG_ERROR ("Cannot config. State transition to ready", 0, 0, 0);
    event_report (EVENT_CAMERA_CANNOT_CONFIG_VFE);
    }
    return FALSE;
  }
  camera_dsp_state = DSP_CONFIGURING;

#ifdef FEATURE_CAMSENSOR_PANASONIC_MN101E19A
#error code not present
#endif

  return ret_val;

} /* camera_config_vfe */

/*===========================================================================

FUNCTION      CAMERA_NO_MEMORY

DESCRIPTION
              Terminate due to no memory.
              Should only be called in post processing after callback
              and client data are saved.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

void camera_no_memory(void)
{
  MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
  camera_terminate (CAMERA_EXIT_CB_ERROR, (int)CAMERA_ERROR_NO_MEMORY);
  MSG_ERROR ("No memory. State transition to ready", 0, 0, 0);
  event_report (EVENT_CAMERA_NO_MEMORY);
} /* camera_no_memory */


/*===========================================================================

FUNCTION      CAMERA_QDSP_CB

DESCRIPTION
              Callback from QDSP.
              Callback is queued to graph context and then processed
              at camera_process_qdsp_msg().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camera_qdsp_cb (CAMQDSP_ResponseType *response)
{
  static boolean camif_error_sent_flag = FALSE;
  uint32 time_ms[2]={0,0};

  clk_uptime_ms(time_ms);
  camera_log (LOG_RSP, 0, (int)response->responseMsg, 1, time_ms[0]);
  switch (response->responseMsg)
  {
  case CAMQDSP_DOWNLOAD_DONE:
     graph_queue_camqdsp_msg (response->responseMsg, (void *) 0, 0);
     break;

  case CAMQDSP_MSG_OUTPUT1_END_OF_FRAME:
#ifndef FEATURE_CAMERA_YCBCR_ONLY
    if( camsensor_static_params[camera_asi].output_format == CAMSENSOR_BAYER )
    {
      if(aec_awb_stats_received)
      {
        if ((camera_state == CAMERA_STATE_PREVIEW)      ||
            (camera_state == CAMERA_STATE_QVP_ENCODING) ||
            (camera_state == CAMERA_STATE_RECORDING))
        {
            aec_awb_stats_received = FALSE;
        }
      }
      else
      {
        /*****************************************************************
        Stats not yet received, request it. After video capture
        starts, we need to request the first time to get stats going,
        then stats will be requested again when stats is received.
        Also, when zooming, stats may be lost due to stats window larger
        then VFE clipping window.
        *****************************************************************/
        if ((camera_state == CAMERA_STATE_PREVIEW)      ||
            (camera_state == CAMERA_STATE_QVP_ENCODING) ||
            (camera_state == CAMERA_STATE_RECORDING))
        {
          camera_request_aec_awb_stats();
        }
      }
    }
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
    /* Reset so error can be reported for the next frame */
    camif_error_sent_flag = FALSE;
    endOfFrameMessage[0].eof_msg = response->responsePayload.endOfFrame;
    endOfFrameMessage[0].time = timetick_get_ms();
#ifdef FEATURE_VIDEO_ENCODE
    MSG_CAMQDSPIO("Out1 Address: %x",endOfFrameMessage[0].eof_msg.output_Y_Address,0,0);
#endif
    if (camera_output1_frame_rate.waiting_for_first_output1_frame == TRUE)
    {
      camera_output1_frame_rate.waiting_for_first_output1_frame = FALSE;
      MSG_CAMERADEBUG("CAMERA_SVCS: Received first output1 frame after initate capture cmd", 0, 0, 0);
    }
    graph_queue_camqdsp_msg (response->responseMsg, (void *) &endOfFrameMessage[0], 0);
#ifdef FEATURE_VIDEO_ENCODE
    output1BuffersQueueSize = VideoBufferUseQueueSize(&output1BufferHandle);
    MSG_CAMQDSPIO("Out1 Address: %x QSize %d",endOfFrameMessage[0].eof_msg.output_Y_Address,
                  output1BuffersQueueSize,0);
    MSG_CAMQDSPIO("isr output1 done %d",time_ms[0],0,0);
#endif /* FEATURE_VIDEO_ENCODE */
     break;

  case CAMQDSP_MSG_OUTPUT2_END_OF_FRAME:
    endOfFrameMessage[1].eof_msg = response->responsePayload.endOfFrame;
    endOfFrameMessage[1].time = timetick_get_ms();
#ifdef FEATURE_VIDEO_ENCODE
    /* Log the first video frame time for A/V Sync purpose */
    if(!glb_avs_info.timelog_video_start)
    {
       /* Make sure this is the same reference clk as CMX sync cb timestamp clk */
       glb_avs_info.timelog_video_start = time_ms[0];
    }
    MSG_CAMQDSPIO("Out2 Address: %x (ack2=%x)",endOfFrameMessage[1].eof_msg.output_Y_Address,
                  ackOutput2.eofAck2[0].Y_Address,0);
#endif /* FEATURE_VIDEO_ENCODE */

#ifdef FEATURE_VIDEO_ENCODE
    if ( camera_state == CAMERA_STATE_RECORDING )
    {
      if(camcorder_qdsp_cb(&endOfFrameMessage[1], response->responseMsg) == FALSE)
        return;
    } /* 'CAMERA_STATE_RECORDING' state */
    else
#endif /* FEATURE_VIDEO_ENCODE */
    {
      // In offline mode signal the DSP to start encoding the frame and then do remaining
      // II Post it to VFE task
#ifdef FEATURE_QVP_OFFLINE_VIDEO_ENCODING
#error code not present
#endif  //#ifdef FEATURE_QVP_OFFLINE_VIDEO_ENCODING
      {
        graph_queue_camqdsp_msg (response->responseMsg, (void *) &endOfFrameMessage[1], 0);
      }
    }
    break;

  case CAMQDSP_MSG_AF_STATS_COMPLETE:
     afStatsMessage = response->responsePayload.AFStats;
     graph_queue_camqdsp_msg (response->responseMsg, (void *) &afStatsMessage, 0);
     break;
  case CAMQDSP_MSG_AE_AWB_STATS_COMPLETE:
#ifndef FEATURE_CAMERA_YCBCR_ONLY
    if( camsensor_static_params[camera_asi].output_format == CAMSENSOR_BAYER )
    {
      if ((camera_state == CAMERA_STATE_PREVIEW)      ||
          (camera_state == CAMERA_STATE_QVP_ENCODING) ||
          (camera_state == CAMERA_STATE_RECORDING)
   #if defined FEATURE_CAMERA_BURST_MODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
   #endif /* FEATURE_CAMERA_BURST_MODE*/  
        )
      {
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /*       FEATURE_CAMERA_AEC_DURING_MULTISHOT */
        camera_request_aec_awb_stats();
      }
      aec_awb_stats_received = TRUE;
    }
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
     aeAwbStatsMessage = response->responsePayload.AE_AWBStats;

     if ((camera_state == CAMERA_STATE_PREVIEW)      ||
          (camera_state == CAMERA_STATE_QVP_ENCODING) ||
          (camera_state == CAMERA_STATE_RECORDING))
     {
     graph_queue_camqdsp_msg (response->responseMsg, (void *) &aeAwbStatsMessage, 0);
     }
     break;
  case CAMQDSP_MSG_ILLEGAL_COMMAND:
     illegalCmdMessage = response->responsePayload.illegalCmd;
     graph_queue_camqdsp_msg (response->responseMsg, (void *) &illegalCmdMessage, 0);
     break;

  case CAMQDSP_VIDEO_FRAME_DONE:
     memcpy (frameDoneMessage,(void *) response->responsePayload.mybuf,
             MP4E_FRAME_BUF_SIZE*2);
     graph_queue_camqdsp_msg (response->responseMsg, (void *) frameDoneMessage, 0);
     break;
#ifdef FEATURE_STROBE_FLASH
     case CAMQDSP_EPOCH_INTR:
     {
       camera_log (LOG_RSP, 0, (int)response->responseMsg, 0, __LINE__);
       if ((camera_fire_flash) && (camera_state == CAMERA_STATE_TAKE_PICTURE))
       {  if (camera_flash_fired == FALSE)
         {
             strobe_flash_start_trigger();
             camera_flash_fired = TRUE;
          }
       }
     break;
     }
#endif /* FEATURE_STROBE_FLASH */

#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif 

  case CAMQDSP_VIDEO_ENCODE_SLEEP_DONE:
    /* Sleep is handled the same way as CONFIG_COMPLETE
    */
  case CAMQDSP_MSG_CONFIG_COMPLETE:
    /* Reset so error can be reported for the next frame */
    camif_error_sent_flag = FALSE;
    videoEncoderState = (CAMQDSP_VideoEncStateType)(response->responsePayload.DSPStateReg);
    graph_queue_camqdsp_msg(CAMQDSP_MSG_CONFIG_COMPLETE,&videoEncoderState,
                            sizeof(CAMQDSP_VideoEncStateType));
    break;

  case CAMQDSP_MSG_IDLE_COMPLETE:
    if ( camera_vfe_control_and_status.wait_for_dsp_idle )
    {
      camera_log (LOG_RSP, 0, (int)response->responseMsg, 0, __LINE__);
      camera_vfe_control_and_status.wait_for_dsp_idle = FALSE;
      if (camera_timed_wait_tcb)
      {
        (void) rex_set_sigs (camera_timed_wait_tcb, CAMERA_TIMER_SIG);
      }
    }
    else
    {
      graph_queue_camqdsp_msg (response->responseMsg, 0, 0);
    }
    break;

  case CAMQDSP_MSG_CAMIF_ERROR:
    /* Filter so only one message is sent */
    if (camif_error_sent_flag == FALSE)
    {
      camif_error_sent_flag = TRUE;
      graph_queue_camqdsp_msg (response->responseMsg, 0, 0);
    }
    break;

  default:
     graph_queue_camqdsp_msg (response->responseMsg, 0, 0);
  }
} /* camera_qdsp_cb */

/*===========================================================================
FUNCTION      CAMERA_SET_EXIF_MAKE_MODEL

DESCRIPTION
              Set the exif tags make (Surf's make) and model (sensor type)

DEPENDENCIES
              None

INPUT
              None

RETURN VALUE
              None

SIDE EFFECTS
              None

============================================================================*/

static void camera_set_exif_make_model(void)
{
  /*setting exif tags make and model */
  camera_exif_tag_type exif_make_tag, exif_model_tag;

  exif_model_tag.tag_id = MODEL;
  exif_model_tag.ifd    = ZERO_IFD;
  exif_model_tag.type   = (tiff_data_type)EXIF_ASCII;
  exif_model_tag.value  = (void *) camsensor_static_params[camera_asi].sensor_name;
  exif_model_tag.count  = (uint16) (strlen((char*) (exif_model_tag.value)) + 1);

  (void) camera_set_exif_tag(&exif_model_tag,NULL,NULL);

  exif_make_tag.tag_id = MAKE;
  exif_make_tag.ifd    = ZERO_IFD;
  exif_make_tag.type   = (tiff_data_type)EXIF_ASCII;

/*lint -save -e545 */
  exif_make_tag.value = (void*)(&mob_sw_rev);
/*lint -restore */
  exif_make_tag.count = (uint16) (strlen((char *)exif_make_tag.value) + 1);
  (void) camera_set_exif_tag(&exif_make_tag,NULL,NULL);

} /* camera_set_exif_make_model */

#ifdef CAMERA_MALLOC_LOGGING

#define CAMERA_MEMORY_LOG_COUNT      50

typedef struct
{
  uint32 mySizes;
  void  *myPtrs;
  uint32 mallocline;
  uint32 freeline;
} camera_memory_usage_info_struct;

typedef struct
{
  uint32 myBlocks;
  uint32 totalSize;
  uint32 maxSize;
  camera_memory_usage_info_struct mydata[CAMERA_MEMORY_LOG_COUNT];
} memory_usage_logs;

memory_usage_logs camera_memory_logs;

/*===========================================================================

FUNCTION      CAMERA_MALLOC

DESCRIPTION
              When used in debug mode, do malloc and keep track
              of memory allocations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void *camera_malloc (uint32 size, uint32 linenumber)
{
  uint32 i;
  void *ptr = malloc(size);
  if (ptr)
  {
    camera_memory_logs.myBlocks++;
    for (i=0; i< CAMERA_MEMORY_LOG_COUNT; i++)
    {
      if (camera_memory_logs.mydata[i].myPtrs == 0)
      {
        camera_memory_logs.mydata[i].myPtrs     = ptr;
        camera_memory_logs.mydata[i].mySizes    = size;
        camera_memory_logs.totalSize            += size;
        camera_memory_logs.mydata[i].mallocline = linenumber;
        camera_memory_logs.mydata[i].freeline   = 0;

        if (camera_memory_logs.totalSize > camera_memory_logs.maxSize)
          camera_memory_logs.maxSize = camera_memory_logs.totalSize;
        break;
      }
    }
  }
  return ptr;
}


/*===========================================================================

FUNCTION      CAMERA_MALLOC & CAMERA_FREE

DESCRIPTION
              When used in debug mode, do free and keep track
              of memory de-allocations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camera_free(void *ptr, uint32 linenumber)
{
  uint32 i;
  boolean found_ptr = FALSE;

  if (ptr)
  {
    for (i=0; i< CAMERA_MEMORY_LOG_COUNT; i++)
    {
      if (camera_memory_logs.mydata[i].myPtrs == ptr)
      {
        found_ptr = TRUE;
        break;
      }
    }

    if (found_ptr == FALSE)
      MSG_FATAL("Freeing Memory we did not allocated line:%d", linenumber, 0, 0);

    camera_memory_logs.myBlocks--;
    for (i=0; i< CAMERA_MEMORY_LOG_COUNT; i++)
    {
      if (camera_memory_logs.mydata[i].myPtrs == ptr)
      {
        camera_memory_logs.mydata[i].myPtrs  = NULL;
        camera_memory_logs.totalSize         -= camera_memory_logs.mydata[i].mySizes;
        camera_memory_logs.mydata[i].freeline = linenumber;
        break;
      }
    }
    free(ptr);
    ptr = NULL;
  }
}

/*===========================================================================

FUNCTION      CAMERA_CHECK_MEMORY_LEAKS

DESCRIPTION
              Check for memory leaks in camera services.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  if leak found
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean camera_check_memory_leaks()
{
  boolean ret_val = FALSE;
  uint32 i;

  for (i=0; i< CAMERA_MEMORY_LOG_COUNT; i++)
  {
    if (camera_memory_logs.mydata[i].myPtrs)
    {
      if (ret_val == FALSE)
      {
        MSG_CAMERADEBUG("CAMERA_SVCS: Camera Service Layer detected Memory Leak:", 0, 0, 0);
      }
      ret_val = TRUE;
      MSG_CAMERADEBUG ("Memory leak: mallocline:%d size:%d",
                         camera_memory_logs.mydata[i].mallocline,
                         camera_memory_logs.mydata[i].mySizes,
                         0
                      );
    }
  }

  return ret_val;
}
#endif /* CAMERA_MALLOC_LOGGING */

#ifdef FEATURE_MDP
/*===========================================================================

FUNCTION      CAMERA_DEREGISTER_VIDEO

DESCRIPTION
  Deregisters the video from MDP

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  camera_deregister_video (boolean clear_ovimg)
{
  if (mdpImgId)
  {
    if (clear_ovimg)
    {
       mdp_clear_ovimg (mdpImgId);
    }
    mdp_deregister_all_ovimg_in_layer(mdpImg.dest, mdpImg.layer);
    mdpImgId = 0;
  }
}
#endif /* FEATURE_MDP */

#ifdef FEATURE_MDP
/*===========================================================================

FUNCTION      CAMERA_DEREGISTER_LAYER1

DESCRIPTION
  Deregisters the layer 1 (frame layer) from MDP

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  camera_deregister_layer1 (void)
{
  if (mdpImgId_frame)
  {
    mdp_clear_ovimg (mdpImgId_frame);
    mdp_deregister_all_ovimg_in_layer(mdpImg_frame.dest, mdpImg_frame.layer);
    mdpImgId_frame = 0;
  }
}
#endif /* FEATURE_MDP */



#ifdef FEATURE_PNG_ENCODER
/*===========================================================================

FUNCTION      CAMERA_PNG_CB

DESCRIPTION
              A designated callback function for the PNG encoder

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 function param not referenced */
static void camera_PNG_CB (PNGE_status_enum_type command, void *handle, void *pnge_data)
{
  camera_snapshot_timing.encode_end = (uint32)timetick_get_ms();
  camera_snapshot_timing.snapshot_completion_callback_start = (uint32)timetick_get_ms();

  camera_clr_take_picture ();

  camera_take_picture_status.encode_in_progress = FALSE;

  if (camera_take_picture_status.abort)
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: PNG_CB Queue STOP_STANDALONE", 0, 0, 0);
    camera_svcs_queue_call_to_terminate();
  }
  else
  {
    camera_terminate(CAMERA_EXIT_CB_DONE, 0);
  }
}
/*lint -restore */
/*===========================================================================

FUNCTION      CAMERA_PROCESS_PNGE_MSG

DESCRIPTION
              Process all messages fr PNG encoder

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_svcs_process_pnge_msg(uint32 msg)
{
  PNGE_status_enum_type command = (PNGE_status_enum_type) msg;

#ifdef TEST_PNGE_MEM_DEVICE
  fs_rsp_msg_type     temp_fs_resp;
#endif /* TEST_PNGE_MEM_DEVICE */

  camera_take_picture_status.encode_in_progress = FALSE;

  switch (command)
  {
    case PNGE_CB_DONE:

      /* Pass back the size of the new encoded PNG image to the app */
      // CAN'T DO THIS
      //camera_app_data = (void *) png_handle.length;

      // Some operation in this function is essential, else the
      // phone will error fatal when trying enter another menu (like
      // config menu) after doing PNG encoding
      // Will also transition to CAMERA_STATE_READY
      camera_log(LOG_TRM, 0, 0, (uint32)PIC_SAVED, __LINE__);
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      camera_terminate(CAMERA_EXIT_CB_DONE, 0);

      MSG_LOW("PNG encode done", 0, 0, 0);
      break;

    case PNGE_CB_ABORT:

      // Some operation in this function is essential, else the
      // phone will error fatal when trying enter another menu (like
      // config menu) after doing PNG encoding
      // Will also transition to CAMERA_STATE_READY
      camera_log(LOG_TRM, 0, 0, (uint32)ABORT, __LINE__);
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      camera_terminate(CAMERA_EXIT_CB_ABORT, 0);

      MSG_LOW("PNG encoder abort", 0, 0, 0);
      break;

    default:
      MSG_HIGH("PNG encoder - unknown message", 0, 0, 0);
  }
}

#endif /* FEATURE_PNG_ENCODER */


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
  camera_encode_properties_type  *encode_properties
)
{
  camera_encode_properties = *encode_properties;

  return CAMERA_SUCCESS;
} /* camera_set_encode_properties */

#ifdef FEATURE_CAMERA_V7
/*===========================================================================

FUNCTION      CAMERA_SVCS_GET_FOCUS_WINDOW

DESCRIPTION
              Gets the focus window. Executes under caller's context

DEPENDENCIES

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 function param not referenced */
camera_ret_code_type camera_svcs_get_focus_window (camera_focus_window_type *camera_display_focus_window)
{
  /* returning CAMERA_NOT_SUPPORTED so that it doesn't break the build as
   * OEMCamera.c will call these functions. The actual implementation of
   * this function is due.
   */
  return CAMERA_NOT_SUPPORTED;
} /* camera_get_focus_window */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMERA_SVCS_SET_FOCUS_WINDOW

DESCRIPTION
              Sets the focus window. Executes under caller's context

DEPENDENCIES

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 function param not referenced */
camera_ret_code_type camera_svcs_set_focus_window (camera_focus_window_type *camera_display_focus_window)
{
  /* returning CAMERA_NOT_SUPPORTED so that it doesn't break the build as
   * OEMCamera.c will call these functions. The actual implementation of
   * this function is due.
   */
  return CAMERA_NOT_SUPPORTED;
} /* camera_set_focus_window */
/*lint -restore */


#endif /* FEATURE_CAMERA_V7 */

void camera_svcs_init_self (camerai_func_type *funcTable)
{
  funcTable->camera_init                       = camera_svcs_init;
  funcTable->camera_start                      = camera_svcs_start;
  funcTable->camera_stop                       = camera_svcs_stop;
  funcTable->camera_set_dimensions             = camera_svcs_set_dimensions;
  funcTable->camera_start_preview              = camera_svcs_start_preview;
  funcTable->camera_stop_preview               = camera_svcs_stop_preview;
  funcTable->camera_take_picture               = camera_svcs_take_picture;
  funcTable->camera_encode_picture             = camera_svcs_encode_picture;
  funcTable->camera_get_thumbnail_properties   = camera_svcs_get_thumbnail_properties;
  funcTable->camera_set_thumbnail_properties   = camera_svcs_set_thumbnail_properties;
  funcTable->camera_start_focus                = camera_af_start_focus;
  funcTable->camera_stop_focus                 = NULL;
  funcTable->camera_enable_frame_callback      = camera_svcs_enable_frame_callback;
  funcTable->camera_disable_frame_callback     = camera_svcs_disable_frame_callback;
  funcTable->camera_release_frame              = camera_svcs_release_frame;
  funcTable->camera_set_overlay                = camera_svcs_set_overlay;
  funcTable->camera_clr_overlay                = camera_svcs_clr_overlay;
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  funcTable->camera_set_icon_array             = camera_svcs_set_icon_array;
  funcTable->camera_clr_icon_array             = camera_svcs_clr_icon_array;
#endif /* FEATURE_CAMERA_SUPPORT_ICON_ARRAY */
  funcTable->camera_set_position               = camera_svcs_set_position;
  funcTable->camera_set_exif_tag               = camera_svcs_set_exif_tag;
  funcTable->camera_blt                        = camera_svcs_blt;
  funcTable->camera_blt_ext                    = camera_svcs_blt_ext;
  funcTable->camera_set_parm                   = camera_svcs_set_parm;
  funcTable->camera_set_parm_2                 = camera_svcs_set_parm_2;
  funcTable->camera_get_info                   = camera_svcs_get_info;
  funcTable->camera_get_parm                   = camera_svcs_get_parm;
  funcTable->camera_get_parm_2                 = camera_svcs_get_parm_2;
  funcTable->camera_process_func               = camera_svcs_process_func;
  funcTable->camera_process_qdsp_msg           = camera_svcs_process_qdsp_msg;
  funcTable->camera_release_picture            = camera_svcs_release_picture;
#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVP_OFFLINE_VIDEO_ENCODING
  funcTable->camera_video_handshake            = camera_svcs_video_handshake;
#endif /* FEATURE_VIDEO_ENCODE || FEATURE_QVP_OFFLINE_VIDEO_ENCODING*/
#ifdef FEATURE_VIDEO_ENCODE
  funcTable->camera_start_record               = camcorder_svcs_start_record;
  funcTable->camera_pause_record               = camcorder_svcs_pause_record;
  funcTable->camera_resume_record              = camcorder_svcs_resume_record;
  funcTable->camera_stop_record                = camcorder_svcs_stop_record;
#endif /* FEATURE_VIDEO_ENCODE */
#ifdef FEATURE_QVPHONE
  funcTable->camera_enable_qvp                 = camera_svcs_enable_qvp;
  funcTable->camera_disable_qvp                = camera_svcs_disable_qvp;
  funcTable->camera_start_qvp_encode           = camera_svcs_start_qvp_encode;
  funcTable->camera_stop_qvp_encode            = camera_svcs_stop_qvp_encode;
  funcTable->camera_qvp_reset                  = camera_svcs_qvp_reset;
#endif /* FEATURE_QVPHONE */
#ifdef FEATURE_MP4_TRANSCODER
  funcTable->camera_enable_offline             = camcorder_svcs_enable_offline;
#endif /* FEATURE_MP4_TRANSCODER */
#ifdef FEATURE_PNG_ENCODER
  funcTable->camera_process_pnge_msg           = camera_svcs_process_pnge_msg;
#endif /* FEATURE_PNG_ENCODER */

#ifdef FEATURE_CAMERA_V7
  funcTable->camera_get_focus_window           = camera_svcs_get_focus_window;
  funcTable->camera_set_focus_window           = camera_svcs_set_focus_window;
#endif /* FEATURE_CAMERA_V7 */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/

} /* camera_svcs_init_self */


/*===========================================================================

FUNCTION      CAMERA_SVCS_SWAP_DIMENSIONS

DESCRIPTION
              swap the values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_svcs_swap_dimensions (uint16 *x, uint16 *y)
{
  *x ^= *y;
  *y ^= *x;
  *x ^= *y;
} /* camera_svcs_swap_dimensions */

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
/*=============================================================================

Function: CAMERA_SVCS_IS_YCBCR_DOWNSIZER_NEEDED

Description: This function return True if ycbcr_downsizer is required, False otherwise.
             There are three conditions required for ycbcr_downsizer (DSP) to be enabled
             (1) vfeSnapshotConfig.output2Configuration.width > 320 or
             (2) vfeSnapshotConfig.output2Configuration.height > 240
             (3) It should be in snapshot mode.

Return Value: TRUE or FALSE

Side Effects: None.

=============================================================================*/

static boolean camera_svcs_is_ycbcr_downsizer_needed ( void )
{
  return (((camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT) &&
          ((vfeSnapshotConfig.output2Configuration.width > 320) ||
          (vfeSnapshotConfig.output2Configuration.height > 240))));
}

#endif /*QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/

#ifdef FEATURE_CAMERA_5x5_ASF
/*===========================================================================

FUNCTION      CAMERA_SVCS_SET_LUMA_FILTER

DESCRIPTION
              Set luma filter for ASF according to current exposure index.

DEPENDENCIES
              Sensor must configure to support ASF.

RETURN VALUE
              None

SIDE EFFECTS
              Luma filter is enable for snapshot capture is enable.

===========================================================================*/


static void camera_svcs_set_luma_filter (void)
{
    chromatix_asf_5_5_type * p;
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else /* CAMERA_USES_CHROMATIX_0x203 */

  uint8 i = 0;
  uint32 sum = 0;
  float smooth_p = 0;
  float index_ratio = 0;

  p = &camsensor_static_params[camera_asi].chromatix_parms->asf_5_5;
  /* if current exposure index is less than index1 */
  /* use BRIGHT LIGHT settings */
#ifdef CAMERA_USES_CHROMATIX_0x201
#error code not present
#else
  if ((uint16)cam3a_aec_state.exp_index <= p->exposure_index1)
#endif
  {
    smooth_p =  (float) p->setting[BRIGHT_LIGHT].smoothing_percent;
  }
  /* if current exposure index is greater than index2 */
  /* interpolate between NORMAL and LOW LIGHT settings */
#ifdef CAMERA_USES_CHROMATIX_0x201
#error code not present
#else
  else if ((uint16)cam3a_aec_state.exp_index >= p->exposure_index2)
  {
    index_ratio = (((uint16)cam3a_aec_state.exp_index - p->exposure_index2) /
                   (float)(p->max_exposure_index - p->exposure_index2));
#endif

    smooth_p = (float) (p->setting[NORMAL_LIGHT].smoothing_percent + index_ratio *
                       (p->setting[LOW_LIGHT].smoothing_percent -
                        p->setting[NORMAL_LIGHT].smoothing_percent));
  }
  /* if current exposure index is between index1 and index2 */
  /* interpolate between BRIGHT and NORMAL LIGHT settings */
  else
  {
#ifdef CAMERA_USES_CHROMATIX_0x201
#error code not present
#else
    index_ratio = (((uint16)cam3a_aec_state.exp_index - p->exposure_index1) /
                   (float)(p->exposure_index2 - p->exposure_index1));
#endif

    smooth_p = (float) (p->setting[BRIGHT_LIGHT].smoothing_percent + index_ratio *
                       (p->setting[NORMAL_LIGHT].smoothing_percent -
                        p->setting[BRIGHT_LIGHT].smoothing_percent));
  }

#endif /* CAMERA_USES_CHROMATIX_0x203 */

  for (i = 0; i < 9; i++)
  {
    luma_filter[i] = (uint32) ((((1.0/9.0) * p->smoothing_filter[i]) + ((8.0/9.0) *
                        (1- smooth_p/100)) * (1.0/8.0) * (p->laplacian_filter[i])) * Q7);
    sum += luma_filter[i];
  }

  /* all nine coefficients must add to unity */
  /* adjust the middle coefficient for luma filter */
  luma_filter[4] += (Q7 - sum);

  /* use 3x3 luma filter as smoothing filter if the feature is supported in hw */
  if ((vfe_hw_features.filter_3x3_luma == TRUE) && (luma_filter[4] != Q7))
  {
    vfeSnapshotConfig.asf_cfg.enable = VFE_Disable;
    vfeSnapshotConfig.f3x3LumaFilter.enable = VFE_Enable;
    vfeSnapshotConfig.f3x3LumaFilter.coefficient_Q_Factor = VFE_SIGNED_3BIT_Q7;

    vfeSnapshotConfig.f3x3LumaFilter.A11 = (int32) luma_filter[0];
    vfeSnapshotConfig.f3x3LumaFilter.A12 = (int32) luma_filter[1];
    vfeSnapshotConfig.f3x3LumaFilter.A13 = (int32) luma_filter[2];
    vfeSnapshotConfig.f3x3LumaFilter.A21 = (int32) luma_filter[3];
    vfeSnapshotConfig.f3x3LumaFilter.A22 = (int32) luma_filter[4];
    vfeSnapshotConfig.f3x3LumaFilter.A23 = (int32) luma_filter[5];
    vfeSnapshotConfig.f3x3LumaFilter.A31 = (int32) luma_filter[6];
    vfeSnapshotConfig.f3x3LumaFilter.A32 = (int32) luma_filter[7];
    vfeSnapshotConfig.f3x3LumaFilter.A33 = (int32) luma_filter[8];
  }
  else if (vfe_hw_features.filter_3x3_asf == TRUE  && (luma_filter[4] != Q7))
  {
    vfeSnapshotConfig.f3x3LumaFilter.enable = VFE_Disable;
    vfeSnapshotConfig.asf_cfg.enable = VFE_Enable;
    vfeSnapshotConfig.asf_cfg.lpf_q_factor = VFE_SIGNED_3BIT_Q7;
    /* set filter thresholds so that lowpass always used, highpass never used*/
    vfeSnapshotConfig.asf_cfg.lpf_threshold = 0x3FF;
    vfeSnapshotConfig.asf_cfg.hpf_threshold = 0x3FF;
    vfeSnapshotConfig.asf_cfg.l11 = (int32) luma_filter[0];
    vfeSnapshotConfig.asf_cfg.l12 = (int32) luma_filter[1];
    vfeSnapshotConfig.asf_cfg.l13 = (int32) luma_filter[2];
    vfeSnapshotConfig.asf_cfg.l21 = (int32) luma_filter[3];
    vfeSnapshotConfig.asf_cfg.l22 = (int32) luma_filter[4];
    vfeSnapshotConfig.asf_cfg.l23 = (int32) luma_filter[5];
    vfeSnapshotConfig.asf_cfg.l31 = (int32) luma_filter[6];
    vfeSnapshotConfig.asf_cfg.l32 = (int32) luma_filter[7];
    vfeSnapshotConfig.asf_cfg.l33 = (int32) luma_filter[8];
  }
  else
  {
    vfeSnapshotConfig.f3x3LumaFilter.enable = VFE_Disable;
    vfeSnapshotConfig.asf_cfg.enable = VFE_Disable;
  }
}
#endif /* FEATURE_CAMERA_5x5_ASF */
/*===========================================================================

FUNCTION      camera_svcs_fill_exif_attributes

DESCRIPTION
              Populate EXIF structure with attributes comming from sensor
              driver, UI, and VFE
DEPENDENCIES


RETURN VALUE
              None

SIDE EFFECTS
              None

===========================================================================*/

static void camera_svcs_fill_exif_attributes(exif_attributes_type *attributes)
{
    /* Invalidate previous attributes */
    attributes->sensor_info.exposureTime.active = FALSE;
    attributes->sensor_info.fNumber.active = FALSE;
    attributes->sensor_info.oecf.active = FALSE;
    attributes->sensor_info.shutterSpeed.active = FALSE;
    attributes->sensor_info.aperture.active = FALSE;
    attributes->sensor_info.max_aperture.active = FALSE;
    attributes->sensor_info.focalLength.active = FALSE;
    attributes->sensor_info.exposureIndex.active = FALSE;
    attributes->sensor_info.cfaPattern.active = FALSE;
    attributes->sensor_info.focalLength35mm.active = FALSE;
    attributes->sensor_info.sensingMethod.active = FALSE;
    attributes->vfe_info.subjectDistance.active = FALSE;
    attributes->vfe_info.lightSource.active = FALSE;
    attributes->vfe_info.subjectLocation.active = FALSE;
    attributes->vfe_info.gainControl.active = FALSE;
    attributes->vfe_info.subjectDistanceRange.active = FALSE;
    attributes->UI_info.exposureProgram.active = FALSE;
    attributes->UI_info.meteringMode.active = FALSE;
    attributes->UI_info.customIP.active = FALSE;
    attributes->UI_info.exposureMode.active = FALSE;
    attributes->UI_info.whiteBalance.active = FALSE;
    attributes->UI_info.digitalZoomRatio.active = FALSE;
    attributes->UI_info.sceneCaptureType.active = FALSE;
    attributes->UI_info.contrast.active = FALSE;
    attributes->UI_info.saturation.active = FALSE;
    attributes->UI_info.sharpness.active = FALSE;
    attributes->UI_info.brightness.active = FALSE;
    attributes->camera_info.flash.active = FALSE;

    /* Set sensor dependent attributes */

    /* Set UI dependent attributes */
     if (camera_parm_contrast.current_value < 4)
         attributes->UI_info.contrast.value = EXIF_SOFT_CONTRAST;
     else if (camera_parm_contrast.current_value < 7)
         attributes->UI_info.contrast.value = EXIF_NORMAL_CONTRAST;
     else
         attributes->UI_info.contrast.value = EXIF_HARD_CONTRAST;
     attributes->UI_info.contrast.active = TRUE;

     if (camera_parm_auto_exposure_mode.current_value == (int)CAMERA_AEC_CENTER_WEIGHTED)
     {
         attributes->UI_info.meteringMode.value = EXIF_CENTER_WEIGHTED_AVERAGE_MM;
     }
     else if (camera_parm_auto_exposure_mode.current_value == (int)CAMERA_AEC_SPOT_METERING)
     {
         attributes->UI_info.meteringMode.value = EXIF_SPOT_MM;
     }
     else
     {
         attributes->UI_info.meteringMode.value = EXIF_UNKNOWN_MM;
     }
     attributes->UI_info.meteringMode.active = TRUE;

     if (camera_parm_white_balance.current_value == (int)CAMERA_WB_AUTO)
     {
         attributes->UI_info.whiteBalance.value = EXIF_AUTO_WB;
     }
     else
     {
         attributes->UI_info.whiteBalance.value = EXIF_MANUAL_WB;
     }
     attributes->UI_info.whiteBalance.active = TRUE;

     /* Exposure compenation is given in -2 2 range and has to be
        converted to -99.99 99.99 - use numerator only since
        denominator is always fixed (6)                           */
     attributes->UI_info.brightness.value = (camera_parm_exposure_compensation.current_value>>16)*8532;
     attributes->UI_info.brightness.active = TRUE;

     attributes->UI_info.exposureProgram.value = EXIF_NORMAL_PROGRAM_EP;
     attributes->UI_info.exposureProgram.active = TRUE;

     attributes->UI_info.exposureMode.value = EXIF_AUTO_EXPOSURE;
     attributes->UI_info.exposureMode.active = TRUE;

     attributes->UI_info.digitalZoomRatio.value = 1;
     attributes->UI_info.digitalZoomRatio.active = TRUE;

     attributes->UI_info.saturation.value = EXIF_NORMAL_SATURATION;
     attributes->UI_info.saturation.active = TRUE;

     attributes->UI_info.sharpness.value = EXIF_NORMAL_SHARPNESS;
     attributes->UI_info.sharpness.active = TRUE;

     attributes->UI_info.customIP.value = EXIF_CUSTOM_PROCESS;
     attributes->UI_info.customIP.active = TRUE;

    /* Get VFE dependent attributes */

    /* Get Sensor dependent attributes */
    attributes->sensor_info = camsensor_static_params[camera_asi].exif_info_from_sensor;

    /* Set original date & time */
    /* Get Date and time string flag by exif_get_date_time_string() and write date and time on the pointer
     using exif_set_date_time. User can write any date and time in the pointer if needed*/
    exif_set_date_time(exif_get_date_time_string());
    /*set set_date_time flag to FALSE as it is already done by camera services */
    exif_set_date_time_from_svcs(FALSE);
    /* Get general attributes */
}

/*===========================================================================

FUNCTION      CAMERA_CLR_TAKE_PICTURE

DESCRIPTION
              Clear take picture variable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_clr_take_picture()
{
  CAM_Free (raw_snapshot_buf);
  raw_snapshot_buf = NULL;
  CAM_Free (snapshot_luma_buf);
  snapshot_luma_buf = NULL;
  CAM_Free (snapshot_chroma_buf);
  snapshot_chroma_buf = NULL;
#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
  /*If incall downsizer is used, then we should free the new Memory*/
  /*which was created for the downsizer output*/
  if(camctrl_tbl.use_incall_downsizer == TRUE)
  {
    CAM_Free (downsizer_out_luma_buffer);
    downsizer_out_luma_buffer = NULL;
    CAM_Free (downsizer_out_chroma_buffer);
    downsizer_out_chroma_buffer = NULL;
  }
#endif /*QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/
  CAM_Free (thumbnail_luma_buf);
  thumbnail_luma_buf = NULL;
  CAM_Free(thumbnail_chroma_buf);
  thumbnail_chroma_buf=NULL;


  CAM_Free (camera_svcs_raw_snapshot_copy);
  camera_svcs_raw_snapshot_copy = NULL;

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

}

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
/*===========================================================================

FUNCTION      CAMERA_WAIT_CAMIF_DONE_CLK_CB

DESCRIPTION
              Handle Callback after a delay has completed.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e715 function param not referenced */
static void camera_wait_camif_done_clk_cb(int4 ms)
{
  MSG_CAMERADEBUG("CAMERA_SVCS: CAMIF_DONE_CALLBACK called", 0, 0, 0);
  graph_queue_camera_func ((uint32)CAMERA_FUNC_HANDLE_ERROR,
                           (void *)0, (void *)0, 0, 0, 0, 0);
}
/*lint -restore */
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */
/*===========================================================================

FUNCTION      CAMERA_PROCESS_HANDLE_ERROR

DESCRIPTION
              Error Handler.

RETURN VALUE
  if in graph context
    TRUE
  else
    FALSE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_handle_error ()
{
  MSG_CAMERADEBUG("Error Handler: camera_state:%d camera_stopping:%d", camera_state, camera_stopping, 0);

  if (camera_state == CAMERA_STATE_TAKE_PICTURE)
  {
    if (camera_take_picture_status.abort)
    {
      camera_svcs_queue_call_to_terminate();
    }
    else if (!camera_stopping)
    {
      if (
           ( ((camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT ) 
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE     
#error code not present
#endif /* FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/                                    
           )&&
             (camera_take_picture_status.received_output2 == TRUE)
           ) ||
           ( (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT) &&
             (camera_take_picture_status.received_output1 == TRUE) &&
             (camera_take_picture_status.received_output2 == TRUE)
           )
         )
      {
        camera_handle_frames_for_takepicture();
      }
      else
      {
        MSG_CAMERADEBUG("CAMERA_SVCS: Take Picture Operation Failed", 0, 0, 0);
        MSG_CAMERADEBUG("==================================", 0, 0, 0);
        if ( (camera_take_picture_status.received_output1 == FALSE) ||
             (camera_take_picture_status.received_output2 == FALSE) ||
             (camera_dsp_error_status.received_camif_error == TRUE) ||
             (camera_dsp_error_status.received_ahb_overflow_error == TRUE)
           )
        {
          MSG_CAMERADEBUG("Interrupts Status: Output1:%d Output2:%d", camera_take_picture_status.received_output1,
                                                                     camera_take_picture_status.received_output2,
                                                                     0
                         );
          MSG_CAMERADEBUG("Error Status: CAMIFERR:%d AHBOVERFLOW:%d", camera_dsp_error_status.received_camif_error,
                                                                      camera_dsp_error_status.received_ahb_overflow_error,
                                                                      0
                         );
        }
        MSG_CAMERADEBUG("==================================", 0, 0, 0);
        camera_clr_take_picture();
        camera_terminate(CAMERA_EXIT_CB_FAILED, 0);
      }
    }
  }
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_CAMIF_DONE_MSG

DESCRIPTION
              Handle Camif Done Message.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_qdsp_camif_done_msg()
{
  boolean send_idle_command  = FALSE;
  boolean start_timer = FALSE;
  MSG_CAMERADEBUG("CAMERA_SVCS: CAMIF DONE Handler camera_state:%d OPMODE:%d", camera_state, camera_camsensor_op_mode, 0);
  camera_take_picture_status.received_camif_done = TRUE;

  if (camera_state == CAMERA_STATE_TAKE_PICTURE)
  {
    if ( camera_take_picture_status.abort)
    {
      if (
           ( (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT) &&
             (camera_take_picture_status.received_output1 == TRUE) &&
             (camera_take_picture_status.received_output2 == TRUE)
           ) ||           
           (( (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT) 
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE     
#error code not present
#endif /* FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/                                    
           )&&
             (camera_take_picture_status.received_output2 == TRUE)
           )
         )
      {
        send_idle_command = TRUE;
      }
      else
      {
        start_timer = TRUE;
      }
    }
    else if (!camera_stopping)
    {
      if (
           (( (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT )
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE     
#error code not present
#endif /* FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/                                    
           ) &&
             (camera_take_picture_status.received_output2 == FALSE)
           ) ||
           ( (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT) &&
             (
               (camera_take_picture_status.received_output1 == FALSE) ||
               (camera_take_picture_status.received_output2 == FALSE)
             )
           )
         )
      {
        start_timer = TRUE;
      }
      else
      {
        clk_dereg(&camera_wait_camif_done_clk);
        camera_handle_frames_for_takepicture();
      }
    }

    if (send_idle_command == TRUE)
    {
      camera_process_terminate();
      return;
    }
    else if (start_timer == TRUE)
    {
      MSG_CAMERADEBUG("Register TimerCallback: CAMIF_DONE received", 0, 0, 0);
      MSG_CAMERADEBUG("Output1:%d Output2:%d", camera_take_picture_status.received_output1,
                                               camera_take_picture_status.received_output2,
                                               0
                     );
      MSG_CAMERADEBUG("CAMIFERR:%d AHBOVERFLOW:%d", camera_dsp_error_status.received_camif_error,
                                                    camera_dsp_error_status.received_ahb_overflow_error,
                                                    0
                     );
      clk_reg (&camera_wait_camif_done_clk, camera_wait_camif_done_clk_cb, 50, 0, FALSE);
    }
  }
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
}

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED 
/*===========================================================================

Function: CAMERA_SVCS_CHROMA_DOWNSAMPLE_H2V1_TO_H2V2

Description:  This function downsamples 2D CrCb array in vertical dimension
              by averaging

Return Value:
   NOne

Comments: downsampling is done in-place
Side Effects:

============================================================================*/
static void camera_svcs_chroma_downsample_h2v1_to_h2v2(uint8 *image, uint16 height, uint16 width)
{
  uint8 *prevLinePtr;
  uint8 *nextLinePtr;
  uint8 *downsampledPtr;
  uint16 i,j;

  prevLinePtr = image;
  nextLinePtr = image + 2*width;
  downsampledPtr = image;

  for (i=0; i<height/2; i++)
  {
    for (j=0;j<2*width; j++)
    {
      /* Average prevLine and nextline */
      *downsampledPtr++ = (uint8)(((uint16)(*prevLinePtr++) + (uint16)(*nextLinePtr++))>>1);
    }
    prevLinePtr += 2*width;
    nextLinePtr += 2*width;
  }
}
#endif /*QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED && ! FEATURE_CAMERA_VFE_SNAPSHOT_YCBCR420*/

/*===========================================================================

FUNCTION      CAMERA_HANDLE_FRAMES_FOR_TAKEPICTURE

DESCRIPTION
              Handle frames for take picture.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_handle_frames_for_takepicture()
{
#if defined FEATURE_CAMERA_BURST_MODE && defined FEATURE_CAMERA_HJR
  uint32 fcount;
#endif

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/

  if (camera_dsp_error_status.received_ahb_overflow_error)
  {
    MSG_ERROR("Detect AHB bus overflow", 0, 0, 0);
  }

  if (camera_state == CAMERA_STATE_TAKE_PICTURE)
  {
    if (camera_take_picture_retry == CAMERA_TAKE_PICTURE_RETRY_NEEDED)
    {
      /* Send Idle command */
      MSG_CAMERADEBUG("CAMERA_SVCS: Send IDLE Message to DSP to retry", 0, 0, 0);
      camera_dsp_command = CAMQDSP_CMD_IDLE;
      camera_log (LOG_VFEC, 0, 0, (uint32)VFE_IDLE, __LINE__);

      if (VFE_Idle() != CAMQDSP_SUCCESS)
      {
        camera_log (LOG_INFO, 0, 0, (uint32)IDLE_REJ, __LINE__);
        camera_take_picture_retry = CAMERA_TAKE_PICTURE_RETRY_DONE;
        MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
        camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
        MSG_ERROR ("Cannot idle DSP. State transition to ready", 0, 0, 0); //?
        event_report (EVENT_CAMERA_CANNOT_IDLE_DSP);
      }
      else
      {
        camera_take_picture_retry = CAMERA_TAKE_PICTURE_RETRY_IDLING;
      }
      return;
    }
    else if (camera_take_picture_retry == CAMERA_TAKE_PICTURE_RETRY_INP)
    {
      MSG_CAMERADEBUG("CAMERA_SVCS: Take picture retry done", 0, 0, 0);
      camera_take_picture_retry = CAMERA_TAKE_PICTURE_RETRY_DONE;
    }
  }

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
  if (camera_svcs_is_ycbcr_downsizer_needed())
  {
    if ( camera_snapshot_timing.offline_snapshot_capture_start != 0)
    {
      /* It is offline snapshot */
      camera_snapshot_timing.offline_snapshot_capture_end = (uint32)timetick_get_ms();
    }
    else
    {
      /* It is regular snapshot */
      camera_snapshot_timing.snapshot_capture_end = (uint32)timetick_get_ms();
    }

#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
    if (shutter_snd_status == NO_SHUTTER_SND)
#endif /* FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND */
    {
  
      qcamrawHeaderType *thumbnail_header = (qcamrawHeaderType *)snapshot_frame.thumbnail_image;
    
    camera_snapshot_timing.downsizer_start = (uint32)timetick_get_ms();

    camera_dsp_state = DSP_DISABLED;
    CAMQDSP_Terminate();
    camera_svcs_process_ycbcr_downsize();
    /* Subsample thumbnail image when VFE outputs in H2V1 format */
    if(vfeSnapshotConfig.output1Configuration.imageFormat == VFE_YCbCrH2V1)
    {
    camera_svcs_chroma_downsample_h2v1_to_h2v2(
      qcamrawGetDataCbCrPtr((byte *)thumbnail_header),
      thumbnail_header->dataHeight, thumbnail_header->dataWidth);
    }
    }

    return;
  }
#endif /*QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
  {
     MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_READY", 0, 0, 0);
     camera_state = CAMERA_STATE_READY;
     MSG_HIGH("Picture taken. State transition to ready", 0, 0, 0);
     event_report(EVENT_CAMERA_PICTURE_TAKEN);
  }
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

  if (camera_app_cb)
  {
    /* If no thumbnail yet, wait for it */
    if ( (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT) &&
         (snapshot_frame.thumbnail_image != NULL)
       )
    {
      /* wait for memory transfer to complete */
      if (offline_read_wait)
      {
        camera_timed_wait(offline_read_wait);
        offline_read_wait = 0;
      }
/* LPM is bypassed during Burst Mode operation */
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
      {
        if ( camera_snapshot_timing.offline_snapshot_capture_start != 0)
        {
          /* It is offline snapshot */
          camera_snapshot_timing.offline_snapshot_capture_end = (uint32)timetick_get_ms();
        }
        else
        {
          /* It is regular snapshot */
          camera_snapshot_timing.snapshot_capture_end = (uint32)timetick_get_ms();
        }

        MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
        /* Apply overlay to the main image and thumbnail */
        /* Currently, IPL can only support H2V2 for overlay */
        if ((encode_overlay != NULL) && (snapshot_frame.format == CAMERA_H2V2))
        {
          camera_apply_overlay_on_snapshot();
        }
        camera_terminate(CAMERA_EXIT_CB_DONE, (int32) &snapshot_frame);
        /* reset lens */
        if (camctrl_tbl.reset_lens_after_snap)
        {
          camera_drv_queue_reset_lens(NULL, 0, 0, 0, 0, 0);
        }
      }
    }
    else if (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT)
    {
      camera_snapshot_timing.raw_capture_end = (uint32)timetick_get_ms();
#if (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED)
      if ((camera_encode_properties.format == CAMERA_JPEG) &&
          (camera_svcs_hjr_enabled() == TRUE))
      {
        if (camctrl_tbl.hjr_bayer_filtering_enable == TRUE)
        {
          camera_handle_single_frame_for_handjitter();
        }
        camera_snapshot_timing.offline_snapshot_config_start = (uint32)timetick_get_ms();        camera_svcs_setup_vfe_for_hjr();
      }
      else
#endif /* (defined FEATURE_CAMERA_HJR) && (!defined QDSP_MODULE_VFE25_DEFINED) */
      {
        MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
        camera_terminate(CAMERA_EXIT_CB_DONE, (int32) &snapshot_frame);
        /* reset lens */
        if (camctrl_tbl.reset_lens_after_snap)
        {
          camera_drv_queue_reset_lens(NULL, 0, 0, 0, 0, 0);
        }
      }
    }
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE	
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/	
  }
}

/*===========================================================================

FUNCTION      CAMERA_SEND_VFE_IDLE_AND_WAIT

DESCRIPTION
              This function will send a VFE_IDLE command to DSP and wait
              for a response.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean camera_send_vfe_idle_and_wait(void)
{
  CAMQDSP_ReturnCodeType dummy;

  /* If Camera Service did not receive any output1 frames then APP is trying to
   * end or abort very quickly need to give some additional time to VFE HW and DSP
   * to be stable before it responds to our IDLE command
   */
  if ( (camera_state == CAMERA_STATE_PREVIEW) &&
       (camera_output1_frame_rate.waiting_for_first_output1_frame == TRUE)
     )
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: Frame Count is Zero give some Time to VFE HW and DSP", 0, 0, 0 );
    camera_timed_wait(50);
  }

  camera_log (LOG_VFEC, 0, 0, (uint32)VFE_IDLE, __LINE__);
  camera_dsp_command = CAMQDSP_CMD_IDLE;
  camera_vfe_control_and_status.wait_for_dsp_idle = TRUE;
  dummy = VFE_Idle ();

  if ( dummy != CAMQDSP_SUCCESS )
  {
    camera_vfe_control_and_status.wait_for_dsp_idle = FALSE;
    camera_vfe_control_and_status.vfe_idle_msg_failed = TRUE;
    MSG_ERROR("CAMERA_SVCS: FAILED to send IDLEMessage:%d", dummy, 0, 0 );
    return FALSE;
  }
  else
  {
    uint32 wait_time = camctrl_tbl.mode_transition_time_ms;
    camera_vfe_control_and_status.vfe_idle_msg_failed = FALSE;

    /* check if CAMQDSP_MSG_IDLE_COMPLETE already received */
    if (camera_vfe_control_and_status.wait_for_dsp_idle == TRUE)
    {
      MSG_CAMERADEBUG("CAMERA_SVCS: Wait for up to %dms", wait_time, 0, 0 );
      camera_timed_wait(wait_time);
      if (camera_vfe_control_and_status.wait_for_dsp_idle == TRUE)
      {
        camera_vfe_control_and_status.wait_for_dsp_idle = FALSE;
        MSG_ERROR("CAMERA_SVCS: We WAITED for %d NO IDLE RESPONSE FROM DSP vfestate:%d vfestopflag:%d", wait_time, qdsp_read(QDSP_vfeState), qdsp_read(QDSP_vfeStopFlag));
        MSG_ERROR("CAMERA_SVCS: TIEMOUT Triggered Call to IdleCompleteResponse Handler", 0, 0, 0);
      }
      else
      {
        MSG_HIGH("CAMERA_SVCS: Got IDLE response", 0, 0, 0);
      }
    }
    else
    {
      MSG_HIGH("CAMERA_SVCS: IDLE response received before wait", 0, 0, 0);
    }

    return TRUE;
  }
}

/*===========================================================================

FUNCTION      CAMERA_CALCULATE_SKIP_PATTERN

DESCRIPTION
              This function will take the input arguments of current max
              sensor rate and the desired frame rate and generate a skip
              pattern based on 30bit-wide pattern mask.

             This code assumes a maximum vfe skip pattern width of 30 bits.

RETURN VALUE
  Nuint32

SIDE EFFECTS
  None

===========================================================================*/
static uint32 camera_calculate_skip_pattern(uint32 input_sensor_rate,
                                             float  input_desired_rate,
                                             uint32 input_bit_width)
{
  uint32  sensor_frame_rate = 0;
  uint32  desired_frame_rate = 0;
  uint32  subpattern_width = 0;
  uint32  numzeros = 0;
  uint32  current_pos = 0;
  double  next_pad_pos = 0;
  double  factor = 0;
  uint32  result = 0;
  uint32  i = 0;


  /* Note: Support for 15 or 30 fps sensors only */
  if(input_sensor_rate &&
     input_desired_rate &&
     input_sensor_rate <= input_bit_width)
  {
    if(input_sensor_rate * 2 == input_bit_width)
    {
      sensor_frame_rate = (uint32)(input_sensor_rate * 2);
      desired_frame_rate = (uint32)(input_desired_rate * 2);
    }
    else
    {
      sensor_frame_rate = (uint32)input_sensor_rate;
      desired_frame_rate = (uint32)input_desired_rate;
    }
  }
  else
  {
    MSG_ERROR("Invalid inputs: sensor %d  desired %d  bitwidth %d",
              input_sensor_rate, input_desired_rate, input_bit_width);
    return 0;
  }

  /* since we already make sure input_sensor_rate and input_desired_rate
   * are non-zero, therefore sensor_frame_rate and desired_frame_rate are
   * non zero. */
  if(sensor_frame_rate < desired_frame_rate)
  {
    MSG_ERROR("Generated invalid frame rates: sensor %d  desired %d",
              sensor_frame_rate, desired_frame_rate, 0);
    return 0;
  }

  /* Set 1 bit every 'subpattern_width' bits, evenly distribute zeros */
  subpattern_width = sensor_frame_rate / desired_frame_rate;
  numzeros = sensor_frame_rate % desired_frame_rate;
  next_pad_pos = factor = (!numzeros ? (double)input_bit_width :
                                       (double)input_bit_width / numzeros);

  /* Total no. of 1's in final skip pattern */
  i = (input_bit_width - numzeros) / subpattern_width;

  while(i--)
  {
    result |= 1;
    result <<= (!i ? subpattern_width - 1 : subpattern_width);
    current_pos += subpattern_width;

    /* Check if sequence will reach/exceed next zero stuffing position */
    if((current_pos+1 >= (uint32)next_pad_pos) && numzeros)
    {
      result <<= 1;
      next_pad_pos += factor;
      current_pos++;
    }
  }

  return result;

}

uint16  camera_recent_fps( void )
{
  return camera_output1_frame_rate.last_fps;
}

/*===========================================================================

FUNCTION      CAMERA_SHOW_SNAPSHOT_TIMING

DESCRIPTION
              This function outs snapshot related timing measurements to
              the debug port. It looks something like this:
              ******** Snapshot timing ********
              Config VFE took       33
              Raw capture took      342 ms
              BVCM took             810 ms
              LPM took              1218 ms
              Encoding took         769 ms

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_show_snapshot_timing( void )
{
  MSG_CAMERA_LATENCY_PROFILING("************************** Snapshot timing ****************************", 0, 0, 0);
  if (camera_snapshot_timing.autofocus_end > camera_snapshot_timing.autofocus_start)
  {
    if ( camera_snapshot_information.autofocus_mode == 0)
    {
      MSG_CAMERA_LATENCY_PROFILING("Auto Focus Mode is NORMAL",0, 0, 0);
    }
    else
    {
      MSG_CAMERA_LATENCY_PROFILING("Auto Focus Mode is MACRO",0, 0, 0);
    }

    MSG_CAMERA_LATENCY_PROFILING("Auto Focus took                                                      %d ms", (camera_snapshot_timing.autofocus_end -
                    camera_snapshot_timing.autofocus_start), 0, 0);

  }


  if (camera_snapshot_timing.take_snapshot_start > camera_snapshot_timing.stop_viewfinder_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("Mode transition from stop view finder to take snapshot               %d ms", (camera_snapshot_timing.take_snapshot_start -
                    camera_snapshot_timing.stop_viewfinder_start), 0, 0);
  }


  if (camera_snapshot_timing.raw_config_end > camera_snapshot_timing.take_snapshot_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("raw config took                                                      %d ms", (camera_snapshot_timing.raw_config_end -
                  camera_snapshot_timing.take_snapshot_start), 0, 0);
    if (camera_capture_delay != 0)
    {
      MSG_CAMERA_LATENCY_PROFILING("Waiting time before the catpure begins                               %d ms",camera_capture_delay, 0, 0);
    }
  }

  if (camera_snapshot_timing.raw_capture_end > camera_snapshot_timing.raw_capture_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("raw capture took                                                     %d ms", (camera_snapshot_timing.raw_capture_end -
                    camera_snapshot_timing.raw_capture_start), 0, 0);
  }

  if (camera_snapshot_timing.bvcm_end > camera_snapshot_timing.bvcm_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("BVCM took                                                            %d ms", (camera_snapshot_timing.bvcm_end -
                    camera_snapshot_timing.bvcm_start), 0, 0);
  }

  if (camera_snapshot_timing.hjr_single_frame_end > camera_snapshot_timing.hjr_single_frame_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("HJR Single frame processing took                                     %d ms", (camera_snapshot_timing.hjr_single_frame_end -
                    camera_snapshot_timing.hjr_single_frame_start), 0, 0);
  }

  if (camera_snapshot_timing.offline_snapshot_config_end > camera_snapshot_timing.offline_snapshot_config_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("Offline snapshot config took                                         %d ms", (camera_snapshot_timing.offline_snapshot_config_end -
                    camera_snapshot_timing.offline_snapshot_config_start), 0, 0);
  }

  if (camera_snapshot_timing.offline_snapshot_capture_end > camera_snapshot_timing.offline_snapshot_capture_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("Offline snapshot capture took                                        %d ms", (camera_snapshot_timing.offline_snapshot_capture_end -
                    camera_snapshot_timing.offline_snapshot_capture_start), 0, 0);
  }

  if (camera_snapshot_timing.snapshot_config_end > camera_snapshot_timing.take_snapshot_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("vfe regular snapshot config took                                     %d ms", (camera_snapshot_timing.snapshot_config_end -
                   camera_snapshot_timing.take_snapshot_start), 0, 0);
    if (camera_capture_delay != 0)
    {
      MSG_CAMERA_LATENCY_PROFILING("Waiting time before the catpure begins                               %d ms",camera_capture_delay, 0, 0);
    }

  }


  if (camera_snapshot_timing.snapshot_capture_end > camera_snapshot_timing.snapshot_capture_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("vfe regular snapshot capture(all frames in case of multi shot) took  %d ms", (camera_snapshot_timing.snapshot_capture_end -
                    camera_snapshot_timing.snapshot_capture_start), 0, 0);
  }

  if (camera_snapshot_timing.downsizer_end > camera_snapshot_timing.downsizer_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("Downsizer took                                                       %d ms", (camera_snapshot_timing.downsizer_end -
                    camera_snapshot_timing.downsizer_start), 0, 0);
  }

  if (camera_snapshot_timing.overlay_end > camera_snapshot_timing.overlay_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("snapshot and thumbnail overlay took                                  %d ms", (camera_snapshot_timing.overlay_end -
                    camera_snapshot_timing.overlay_start), 0, 0);
  }


  if (camera_snapshot_timing.hjr_two_frame_end > camera_snapshot_timing.hjr_two_frame_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("HJR 2 frame processing took                                          %d ms", (camera_snapshot_timing.hjr_two_frame_end -
                    camera_snapshot_timing.hjr_two_frame_start), 0, 0);
  }


  if (camera_snapshot_timing.hjr_three_frame_end > camera_snapshot_timing.hjr_three_frame_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("HJR 3 frame processing took                                          %d ms", (camera_snapshot_timing.hjr_three_frame_end -
                    camera_snapshot_timing.hjr_three_frame_start), 0, 0);
  }

  if (camera_snapshot_timing.lpm_end > camera_snapshot_timing.lpm_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("LPM (last frame in case of multi shot) took                          %d ms", (camera_snapshot_timing.lpm_end -
                    camera_snapshot_timing.lpm_start), 0, 0);
  }


  if (camera_snapshot_timing.encode_end > camera_snapshot_timing.encode_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("Jpeg encoding (last frame in case of multi shot) took                %d ms", (camera_snapshot_timing.encode_end -
                    camera_snapshot_timing.encode_start), 0, 0);
  }


  if (camera_snapshot_timing.viewfinder_start > camera_snapshot_timing.snapshot_completion_callback_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("Snapshot completion to view finder start command took                %d ms", (camera_snapshot_timing.viewfinder_start -
                    camera_snapshot_timing.snapshot_completion_callback_start), 0, 0);
  }

  if (camera_snapshot_timing.first_frame_to_display > camera_snapshot_timing.viewfinder_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("View finder start command to first frame to MDP took                 %d ms", (camera_snapshot_timing.first_frame_to_display -
                    camera_snapshot_timing.viewfinder_start), 0, 0);
  }

  if (camera_snapshot_timing.snapshot_completion_callback_start > camera_snapshot_timing.take_snapshot_start)
  {
    MSG_CAMERA_LATENCY_PROFILING("Snapshot start to encoding end (all frames in case of multishot)     %d ms", (camera_snapshot_timing.snapshot_completion_callback_start -
                    camera_snapshot_timing.take_snapshot_start), 0, 0);
  }

  MSG_CAMERA_LATENCY_PROFILING("*************************Additional Information******************************", 0, 0, 0);
  MSG_CAMERA_LATENCY_PROFILING("Encoded picture width                                                %d", camera_snapshot_information.encode_picture_width, 0, 0);

  MSG_CAMERA_LATENCY_PROFILING("Encoded picture height                                               %d", camera_snapshot_information.encode_picture_height, 0, 0);

  if (camera_snapshot_timing.luma_adaptation_time > 0 )
  {
    MSG_CAMERA_LATENCY_PROFILING("luma adaptation (last frame in case of multi shot) took              %d ms", camera_snapshot_timing.luma_adaptation_time, 0, 0);
  }

  if (camera_snapshot_timing.chroma_suppression_time > 0 )
  {
    MSG_CAMERA_LATENCY_PROFILING("chroma supression (last frame in case of multi shot) took            %d ms", camera_snapshot_timing.chroma_suppression_time, 0, 0);
  }

  if (camera_snapshot_timing.luma_filter_three_by_three_time > 0 )
  {
    MSG_CAMERA_LATENCY_PROFILING("luma filter (last frame in case of multi shot) 3x3 took              %d ms", camera_snapshot_timing.luma_filter_three_by_three_time, 0, 0);
  }

  if (camera_snapshot_timing.asf_five_by_five_single_fragment_time > 0)
  {
    MSG_CAMERA_LATENCY_PROFILING("ASF filter (last frame in case of multi shot) 5x5 took               %d ms", camera_snapshot_timing.asf_five_by_five_single_fragment_time, 0, 0);
  }


  if (camera_snapshot_information.encoded_data_size > 0 )
  {
    MSG_CAMERA_LATENCY_PROFILING("Encoded Data size                                                    %d bytes", camera_snapshot_information.encoded_data_size, 0, 0);
  }

  MSG_CAMERA_LATENCY_PROFILING("*************************************************************************", 0, 0, 0);
  memset(&camera_snapshot_timing, 0, sizeof(camera_snapshot_timing));
  memset(&camera_snapshot_information, 0, sizeof(camera_snapshot_information));
}

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION: CAMERA_AEC_AWB_READY

DESCRIPTION:
  Decides whether to send AEC/AWB request or process AEC/AWB stats.

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  TRUE:  If AEC/AWB stats request can be sent or AEC/AWB stats can be
         processed.
  FALSE: Otherwise.

============================================================================*/
static boolean camera_aec_awb_ready (void)
{
  if ((camsensor_static_params[camera_asi].aec_enable !=
                                           (boolean) VFE_Enable) &&
      (camsensor_static_params[camera_asi].awb_enable !=
                                           (boolean) VFE_Enable))
  {
    return FALSE;
  }

  if (!camera_af_is_active() &&
      (camera_3a_shared_get_aec_awb_delay_count() == 0) &&
      (camera_dsp_command != CAMQDSP_CMD_IDLE))
  {
    return TRUE;
  }

  return FALSE;
} /* camera_aec_awb_ready */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifdef FEATURE_CAMERA_HJR
#ifndef QDSP_MODULE_VFE25_DEFINED
/*===========================================================================

FUNCTION: CAMERA_HANDLE_SINGLE_FRAME_FOR_HANDJITTER

DESCRIPTION:
  RUNS BAYER FILTER ON THE INPUT FRAME IN BAYER FORMAT

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  NONE
============================================================================*/
static void camera_handle_single_frame_for_handjitter(void)
{
  uint16 width, height;
  uint32 start_time, end_time;

  camera_snapshot_timing.hjr_single_frame_start = (uint32)timetick_get_ms();

  width = (vfeRawConfig.camif_window_width_config.lastPixel -
          vfeRawConfig.camif_window_width_config.firstPixel + 1);

  height = (vfeRawConfig.camif_window_height_config.lastLine -
           vfeRawConfig.camif_window_height_config.firstLine + 1) ;

#ifdef CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED

/* 10 bit bayer noise reduction is not supported.
 * Please do not use 10-bit raw data as
 * input for the bayer noise reduction  */

/*lint -save -e650 */
  if (vfeRawConfig.rawOutputOption == VFE_10_BIT_DIRECT)
/*lint -restore */
  {
    start_time = (uint32)timetick_get_ms();
    ipl_unpack_10_bit_bayer_data((uint8*)camera_take_picture_status.Y_Address,
                                 width*height);
    end_time = (uint32)timetick_get_ms();
    if (end_time > start_time)
    {
      MSG_CAMERADEBUG("HJR: 10_BIT_UNPACKING %d ms", (end_time-start_time), 0, 0);
    }
    start_time = (uint32)timetick_get_ms();
    /* Input address and output address are same.
     * Need to allocate memory here for output */
    hjr_bayer_noise_reduction(camera_take_picture_status.Y_Address,
                ( (uint8*)camera_take_picture_status.Y_Address - (width * 3 * 2)),
                width,
                height,
                camsensor_static_params[camera_asi].raw_output_option,
                camsensor_static_params[camera_asi].format,
                &(camsensor_static_params[camera_asi])
                );
    end_time = (uint32)timetick_get_ms();
    /* The output data is put from 3 lines above the input buffer */
    camera_take_picture_status.Y_Address = (uint32*) ( (uint8*)camera_take_picture_status.Y_Address - (width * 3));
    if (end_time > start_time)
    {
      MSG_CAMERADEBUG("HJR: 10_BIT_BAYER_FILTERING %d ms", (end_time-start_time), 0, 0);
    }
    start_time = (uint32)timetick_get_ms();
    ipl_pack_10_bit_bayer_data((uint8*)camera_take_picture_status.Y_Address, width*height);
    end_time = (uint32)timetick_get_ms();
    if (end_time > start_time)
    {
      MSG_CAMERADEBUG("HJR: 10_BIT_PACKING %d ms", (end_time-start_time), 0, 0);
    }
  }
  else
#endif /* CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED */
  {
    start_time = (uint32)timetick_get_ms();
    hjr_bayer_noise_reduction((uint8*)camera_take_picture_status.Y_Address,
                  ( (uint8*)camera_take_picture_status.Y_Address - (width * 3)),
                  width,
                  height,
                  camsensor_static_params[camera_asi].raw_output_option,
                  camsensor_static_params[camera_asi].format,
                  &(camsensor_static_params[camera_asi])
                  );
    /* The output data is put from 3 lines above the input buffer */
    camera_take_picture_status.Y_Address =(uint32*) ( (uint8*)camera_take_picture_status.Y_Address - (width * 3));
    end_time = (uint32)timetick_get_ms();
    if (end_time > start_time)
    {
      MSG_CAMERADEBUG("HJR: 8_BIT_BAYER_FILTERING %d ms", (end_time-start_time), 0, 0);
    }

  }
  camera_snapshot_timing.hjr_single_frame_end = (uint32)timetick_get_ms();
}
#endif /* QDSP_MODULE_VFE25_DEFINED */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifndef QDSP_MODULE_VFE25_DEFINED
/*===========================================================================

FUNCTION: CAMERA_SVCS_SETUP_VFE_FOR_HJR

DESCRIPTION:
  Sets up the environment and VFE for offline snapshot.

DEPENDENCIES:

SIDE EFFECTS:
  None

RETURN VALUE:
  Void

============================================================================*/

static void camera_svcs_setup_vfe_for_hjr(void)
{
  /* configure vfe for offline snapshot */
  camera_camsensor_op_mode = CAMSENSOR_OP_MODE_SNAPSHOT;

  /* reset frame buffer */
  snapshot_frame.buffer = NULL;
  /* make a local copy of rawsnapshot */
  camera_svcs_raw_snapshot_copy = raw_snapshot_buf;
  /* reset raw_snapshot pointer */
  raw_snapshot_buf = NULL;
  camera_take_picture_status_set_default();
  camera_dsp_error_status_set_default();
  camera_state = CAMERA_STATE_TAKE_PICTURE;
  if (camera_config_vfe() == FALSE)
  {
    MSG_ERROR("CAMERA_SVCS: Cannot configure VFE for OFFline snapshot for HJR",0 ,0, 0);
  }
}
#endif /* QDSP_MODULE_VFE25_DEFINED */
#endif /* FEATURE_CAMERA_HJR */

/*===========================================================================

FUNCTION: CAMERA_SVCS_OPTIMIZE_RESOURCES

DESCRIPTION:
  Adjust the resources available

DEPENDENCIES:

SIDE EFFECTS:
  None

RETURN VALUE:
  Void

============================================================================*/
static void camera_svcs_optimize_resources (void)
{
#if (defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III||defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE|| \
     defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_II)|| defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_IV && \
    (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING   || \
     defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)
    switch (camera_state) {
    case CAMERA_STATE_PREVIEW:
        if (camera_clk_resource != CAMERA_CLK_RESOURCE_VIEWFINDER) {
          #ifdef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III /* SC2X */
            if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR) {
                if ((camsensor_static_params[camera_asi].preview_fps <= (15*Q8)) \
                   || (camsensor_static_params[camera_asi].max_video_fps <=(15*Q8))) {
                    CAMERA_SET_CLK_VIEWFINDER();
                } else {
                    CAMERA_SET_CLK_VIEWFINDER_HIGH();
                }
            }

            else {
                #ifndef FEATURE_CAMERA_YCBCR_ONLY
                if ((cam3a_aec_state.afr_max_fps <= (15*Q8)) \
                   || (camsensor_static_params[camera_asi].max_video_fps <=(15*Q8))) {
                    CAMERA_SET_CLK_VIEWFINDER();
                } else {
                    CAMERA_SET_CLK_VIEWFINDER_HIGH();
                }
               #endif /* FEATURE_CAMERA_YCBCR_ONLY */

             }
            camera_clk_resource = CAMERA_CLK_RESOURCE_VIEWFINDER;
           #else
              CAMERA_SET_CLK_VIEWFINDER();
              camera_clk_resource = CAMERA_CLK_RESOURCE_VIEWFINDER;
           #endif
      }
           
      /* if the pixel clock for sensor is too high, when the real time dynamic pclk value is available
       * should switch to using that one instead of this static variable */
    #ifndef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III /* SC2x */
        if ((clk_regime_msm_get_clk_freq_khz(CLK_RGM_HCLK_M) < camsensor_static_params[camera_asi].pixel_clock / 1000) ) {
        CAMERA_SET_CLK_VIEWFINDER_HIGH();
      }
    #endif
      break;

    case CAMERA_STATE_TAKE_PICTURE:
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
    case CAMERA_STATE_ENCODE_PICTURE:
    #ifdef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III
        CAMERA_SET_CLK_SNAPSHOT();
    #else
        if(camera_clk_resource != CAMERA_CLK_RESOURCE_SNAPSHOT)
         {
            CAMERA_SET_CLK_SNAPSHOT();
            camera_clk_resource = CAMERA_CLK_RESOURCE_SNAPSHOT;
         }
    #endif
      break;
    case CAMERA_STATE_READY:
        if (camera_clk_resource != CAMERA_CLK_RESOURCE_DEFAULT) {
    #ifdef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III
        CAMERA_SET_CLK_DEFAULT();
        CAMERA_SET_CLK_HIGH_DEFAULT();
        camera_clk_resource = CAMERA_CLK_RESOURCE_DEFAULT;
    #else
        CAMERA_SET_CLK_DEFAULT();
    #if defined (FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE) && defined (QDSP_MODULE_VFE20_DEFINED)
        CAMERA_SET_CLK_HIGH_DEFAULT();
    #endif /*FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE && QDSP_MODULE_VFE20_DEFINED*/
        camera_clk_resource = CAMERA_CLK_RESOURCE_DEFAULT;
    #endif
      }
      break;
    default:
      break;
  }
#endif /* (defined(FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III)||defined (FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE  ||
                   FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE) || FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_IV &&
         (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING ||
          defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING) */

} /* camera_svcs_optimize_resources */

#ifdef FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK
/*===========================================================================

FUNCTION: CAMERA_SVCS_SET_VFE_CLK

DESCRIPTION:
  If PCLK is slower, i.e., aDSP clk / PCLK > 8, uses faster internal clock
  to configure vfe. camclk_po is identified as suitable internal clock
  as it is intended only for camera application.
  Switches back vfe clk source to pclk during VFE capture.
  Enable FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK if it is
  determined aDSP clk / PCLK > 8. This function doesn't determine the
  exact ratio, aDSP clk / PCLK, to keep implementation simple.
  Current Max. aDSP clock freq = 100.8 MHz.

DEPENDENCIES:
  VFE h/w must be in idle state.

SIDE EFFECTS:
  None

RETURN VALUE:
  Void

============================================================================*/
static void camera_svcs_set_vfe_clk (camera_svcs_vfe_mode_type mode)
{
  static uint32 orig_camclk_po_hz;

  /* Ensure (Max. aDSP clk / vfe_config_clk_freq < 8)
   * Current Max. aDSP clock freq = 100.8 MHz.
   * If the Max. aDSP clock freq is increased, tune vfe_config_clk_freq
   * accordingly.
   */
  static uint32 vfe_config_clk_freq = 24000000;

  switch (mode)
  {
    case VFE_MODE_CONFIG:
      /* Disable the vfe clock */
      CAMERA_CLK_REGIME_DISABLE     (CLK_RGM_MDDI_CLIENT_VFE_M);

      /* Save the latest camclk_po requested by camsensor module.
       * Need to restore original camclk_po during capture
       */
      orig_camclk_po_hz = camctrl_tbl.current_camclk_po_hz;

      if (orig_camclk_po_hz < vfe_config_clk_freq)
      {
        /* program internal clock, camclk_po, for desired freqency */
        (void) camsensor_config_camclk_po (vfe_config_clk_freq);
      }

      /* Select the vfe input clock as camclk_po */
      CAMERA_CLK_REGIME_SEL_CLK_SRC ( \
                          CLK_RGM_SRC_SEL_MDDI_CLIENT_VFE, VFE_CAMCLK_PO\
                                    );

      /* Enable the vfe input clock */
      CAMERA_CLK_REGIME_ENABLE      (CLK_RGM_MDDI_CLIENT_VFE_M);
      break;

    case VFE_MODE_CAPTURE:
      /* Disable the vfe input clock */
      CAMERA_CLK_REGIME_DISABLE     (CLK_RGM_MDDI_CLIENT_VFE_M);

      if (orig_camclk_po_hz != camctrl_tbl.current_camclk_po_hz)
      {
        /* camclk_po freq was changed during vfe config
         * from the camsensor driver settings, so restore
         * original camclk_po.
         */
        (void) camsensor_config_camclk_po (orig_camclk_po_hz);
      }

      /* Select the vfe input clock as pclk */
      CAMERA_CLK_REGIME_SEL_CLK_SRC ( \
                    CLK_RGM_SRC_SEL_MDDI_CLIENT_VFE, VFE_CAMIF_PCLK
                                    );

      /* Enable the vfe input clock */
      CAMERA_CLK_REGIME_ENABLE      (CLK_RGM_MDDI_CLIENT_VFE_M);
      break;

    default:
      return;
  }
} /* camera_svcs_set_vfe_clk */
#endif /* FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK */

/*===========================================================================

FUNCTION: CAMERA_ROTATE_ENCODE_OVERLAY

DESCRIPTION:
  Rotates the overlay frame that is to be encoded to align with the VFE output frame
  This is normally used when there is a rotation in sensor orientation.
DEPENDENCIES:

SIDE EFFECTS:
  None

RETURN VALUE:
  Void

============================================================================*/
static void camera_rotate_encode_overlay_ex(camera_encode_overlay_type *overlay)
{
  ipl_status_type status;
  ipl_image_type  src_image, dst_image;

  /* Rotate the encode overlay frame 180 degrees */
  if (overlay != NULL)
  {
    /* For main image */
    src_image.dx = overlay->padded_width;
    src_image.dy = overlay->padded_height + overlay->pad_lines;
    src_image.cFormat = overlay->ipl_image.cFormat;
    src_image.imgPtr = overlay->imgPtr + overlay->pad_pixels * 2;
    src_image.clrPtr = NULL;
    dst_image.dx = overlay->padded_width;
    dst_image.dy = overlay->padded_height + overlay->pad_lines;
    dst_image.cFormat = overlay->ipl_image.cFormat;
    dst_image.imgPtr = CAM_Malloc (overlay->size | CAM_ALLOC_NO_ZMEM);
    dst_image.clrPtr = NULL;
    if (dst_image.imgPtr == NULL)
    {
      camera_clr_overlays();
      MSG_ERROR ("Encode Overlay Rotation: Memory Allocation failed", 0, 0, 0);
      return;
    }
    status = ipl_rotate90_frame (&src_image, &dst_image, IPL_ROT180, camera_transparent_color);
    CAM_Free(overlay->imgPtr);
    overlay->imgPtr = dst_image.imgPtr;
    if (status != IPL_SUCCESS)
    {
      camera_clr_overlays();
      MSG_ERROR ("Encode Overlay Rotation: Rotation failed", 0, 0, 0);
      return;
    }
  }
}

static void camera_rotate_encode_overlay(void)
{
  camera_rotate_encode_overlay_ex(encode_overlay);
  camera_rotate_encode_overlay_ex(thumbnail_overlay);
  camera_svcs_adjust_overlay_for_rotation();
}

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION: CAMERA_RECALL_VFE_CONFIG

DESCRIPTION:
  Recall the latest AEC/AWB gain and apply it into VFE

DEPENDENCIES:

SIDE EFFECTS:
  None

RETURN VALUE:
  Void

============================================================================*/
static void camera_restore_config_for_preview(void)
{
  if(camsensor_static_params[camera_asi].aec_enable)
  {
    camera_aec_restore_config_for_preview();
  }
  if(camsensor_static_params[camera_asi].awb_enable)
  {
    vfe_restore_config_for_preview();
  }

#ifdef FEATURE_CAMERA_HJR
  /* Frame rate */
  if (camera_svcs_hjr_enabled () == TRUE )
  {
    /*Check if the frame rate is changed for HJR */
    if (HJR_structure.current_frame_rate != HJR_structure.new_frame_rate)
    {
      /*Change the frame rate to the preview frame rate */
      (void)camsensor_set_frame_rate((uint16)cam3a_aec_state.afr_frame_rate);
    }
  }
#endif /* FEATURE_CAMERA_HJR */
}
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION: CAMERA_SVCS_SET_EV_COMPENSATION

DESCRIPTION:
  Apply EV compensation

DEPENDENCIES:

SIDE EFFECTS:
  None

RETURN VALUE:
  camera_ret_code_type

============================================================================*/
static camera_ret_code_type camera_svcs_set_ev_compensation(int32 parm)
{
  camera_ret_code_type ret_val = CAMERA_SUCCESS;
  int16  parmCurNum = (int16)(parm>>16);
  uint16 parmCurDen = (uint16)(parm & 0x0000FFFF);
  int32 max_parmVal =
    (camera_ev_compensation_table.maximum_numerator_value * camera_ev_compensation_table.step_denominator_value)/
    (camera_ev_compensation_table.maximum_denominator_value * camera_ev_compensation_table.step_numerator_value);
  int32 min_parmVal =
    (camera_ev_compensation_table.minimum_numerator_value * camera_ev_compensation_table.step_denominator_value)/
    (camera_ev_compensation_table.minimum_denominator_value * camera_ev_compensation_table.step_numerator_value);
  int32  parmVal;
  uint32 new_luma_target;

  if( parmCurNum == 0 || parmCurDen == 0)
  {
    parmVal = 0;
  }
  else
  {
    parmVal = (parmCurNum * camera_ev_compensation_table.step_denominator_value)/
      (parmCurDen * camera_ev_compensation_table.step_numerator_value);
  }

  if ( parmVal > max_parmVal )
  {
    ret_val =  CAMERA_INVALID_PARM;
  }
  else if ( parmVal < min_parmVal )
  {
    ret_val =  CAMERA_INVALID_PARM;
  }
  else
  {

    if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR)
    {
      (void) camsensor_set_contrast ((int8) parm);

      /* Update the brightness index */
      camera_parm_exposure_compensation.current_value = parm;

      ret_val = CAMERA_SUCCESS;
    }
    else
    {
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

      /* parm contains Numerator,Denominator. So first extract the
       * numerator and covert into index.
       */
      new_luma_target = (uint32) parmVal + camera_ev_compensation_table.value_to_get_index;
      new_luma_target = (uint32) (( cam3a_aec_state.default_luma_target *
            camera_ev_compensation_table.ev_ptr[new_luma_target] ) + 0.5);

      /* Set luma variables */
      camera_aec_set_exposure_compensation((uint8) new_luma_target);
      ret_val = CAMERA_SUCCESS;
    }
  }
  return ret_val;
} /* camera_svcs_set_ev_compensation */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

#ifdef FEATURE_CAMERA_LPM
#error code not present
#endif /* FEATURE_CAMERA_LPM */

#ifdef FEATURE_CAMERA_HJR
/*===========================================================================

FUNCTION: CAMERA_SVCS_HJR_ENABLED

DESCRIPTION:
  Determine if Hand Jitter Reduction is enabled considering best shot.

DEPENDENCIES:

SIDE EFFECTS:
  None

RETURN VALUE:
boolean indicating if feature should be On or Off

============================================================================*/
static boolean camera_svcs_hjr_enabled(void)
{
  boolean camera_hjr_enable = FALSE;
  if( camera_parm_iso.current_value == CAMERA_ISO_DEBLUR )
  {
    camera_hjr_enable = TRUE;
  }
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
#ifdef FEATURE_STROBE_FLASH
  /* Strobe flash needs full frame exposure for CMOS sensors */
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#else
      if(camera_state == CAMERA_STATE_TAKE_PICTURE)
#endif /* FEATURE_CAMERA_BURST_MODE */
      {

        if (camera_fire_flash == TRUE)
        {
          return FALSE;
        }
      }
#endif /* FEATURE_STROBE_FLASH */
#ifndef FEATURE_CAMERA_BESTSHOT_MODE
  return camera_hjr_enable;
#else
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
} /* camera_svcs_hjr_enabled */

#endif /* FEATURE_CAMERA_HJR */

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

static void camera_apply_overlay_on_snapshot(void)
{
  ipl_status_type status;
  ipl_image_type main_image;
  ipl_rect_type frame_location;
  ipl_image_type img_out;

  if (encode_overlay == NULL)
  {
    return;
  }

  camera_snapshot_timing.overlay_start = (uint32)timetick_get_ms();

  /* Apply the overlay on the main image */
  img_out.dx      = encode_overlay->ipl_image.dx;
  img_out.dy      = encode_overlay->ipl_image.dy;
  img_out.imgPtr  = encode_overlay->ipl_image.imgPtr;
  img_out.cFormat = encode_overlay->ipl_image.cFormat;

  main_image.dx      = encode_overlay->ipl_image.dx;
  main_image.dy      = encode_overlay->ipl_image.dy;
  main_image.imgPtr  = qcamrawGetDataYPtr(snapshot_frame.buffer);
  main_image.clrPtr  = qcamrawGetDataCbCrPtr(snapshot_frame.buffer);
  main_image.cFormat = IPL_YCrCb420_LINE_PK;

  frame_location.x = 0;
  frame_location.y = 0;
  frame_location.dx = snapshot_frame.captured_dx;
  frame_location.dy = snapshot_frame.captured_dy;

  status = ipl_image_add_inplace(&main_image, &img_out, &frame_location, camera_transparent_color);
  if (status != IPL_SUCCESS)
  {
    MSG_ERROR ("Cannot composite overlay to main image", 0, 0, 0);
  }

  if (thumbnail_overlay == NULL)
  {
    return;
  }

  /* Apply the overlay on the thumbnail */
  img_out.dx      = thumbnail_overlay->ipl_image.dx;
  img_out.dy      = thumbnail_overlay->ipl_image.dy;
  img_out.imgPtr  = thumbnail_overlay->ipl_image.imgPtr;
  img_out.cFormat = thumbnail_overlay->ipl_image.cFormat;

  main_image.dx      = thumbnail_overlay->ipl_image.dx;
  main_image.dy      = thumbnail_overlay->ipl_image.dy;
  main_image.imgPtr  = qcamrawGetDataYPtr(snapshot_frame.thumbnail_image);
  main_image.clrPtr  = qcamrawGetDataCbCrPtr(snapshot_frame.thumbnail_image);
  main_image.cFormat = IPL_YCrCb420_LINE_PK;

  frame_location.x = 0;
  frame_location.y = 0;
  frame_location.dx = thumbnail_overlay->ipl_image.dx;
  frame_location.dy = thumbnail_overlay->ipl_image.dy;

  status = ipl_image_add_inplace(&main_image, &img_out, &frame_location, camera_transparent_color);
  if (status != IPL_SUCCESS)
  {
    MSG_ERROR ("Cannot composite overlay to thumbnail image", 0, 0, 0);
  }
  camera_snapshot_timing.overlay_end = (uint32)timetick_get_ms();
}

static camera_cb_type camera_svcs_translate_ret_val_to_cb
(
 camera_ret_code_type ret_val
)
{
  camera_cb_type cb;
  switch (ret_val)
  {
    case CAMERA_SUCCESS:
      cb = CAMERA_EXIT_CB_DONE;
      break;
    case CAMERA_NO_MEMORY:
    case CAMERA_FAILED:
    case CAMERA_INVALID_STAND_ALONE_FORMAT:
    case CAMERA_MALLOC_FAILED_STAND_ALONE:
      cb = CAMERA_EXIT_CB_FAILED;
      break;
    case CAMERA_NOT_SUPPORTED:
    case CAMERA_INVALID_STATE:
    case CAMERA_INVALID_PARM:
    case CAMERA_INVALID_FORMAT:
    case CAMERA_NO_SENSOR:
    default:
      cb = CAMERA_EXIT_CB_ERROR;
      break;
  }
  return cb;
}

static void camera_svcs_clr_preview_overlay(void)
{
  if (preview_overlay)
  {
    CAM_Free(preview_overlay->imgPtr);
    CAM_Free(preview_overlay);
    preview_overlay = NULL;
  }
}

static void camera_svcs_clr_encode_overlay(void)
{
  if (encode_overlay)
  {
    CAM_Free(encode_overlay->imgPtr);
    CAM_Free(encode_overlay);
    encode_overlay = NULL;
  }
}

static void camera_svcs_clr_thumbnail_overlay(void)
{
  if (thumbnail_overlay)
  {
    CAM_Free(thumbnail_overlay->imgPtr);
    CAM_Free(thumbnail_overlay);
    thumbnail_overlay = NULL;
  }
}
/*lint -restore */

/*===========================================================================

FUNCTION      CAMERA_SVCS_QUEUE_CALL_TO_TERMINATE

DESCRIPTION
              The camera operation is stopped in graph context.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_svcs_queue_call_to_terminate(void)
{
  camera_take_picture_status.abort                     = FALSE;
  camera_take_picture_status.abort_lpm_session         = FALSE;
  camera_take_picture_status.lpm_encoding_start        = FALSE;
  camera_take_picture_status.lpm_encoding_in_progress  = FALSE;
  camera_take_picture_status.bvcm_in_progress          = FALSE;

  MSG_CAMERADEBUG("CAMERA_SVCS: Queuing camera_process_terminate to graph task", 0, 0, 0);
  graph_queue_camera_func ((uint32)CAMERA_FUNC_TERMINATE,
                                 (void *)0, (void *)0, 0, 0, 0, 0);
}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_TERMINATE

DESCRIPTION
              The camera operation is stopped, VFE is brought to idle and dsp
              services are released. All memory except snapshot buffers are
              freed. Final state is CAMERA_STATE_READY.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_terminate(void)
{
  MSG_CAMERADEBUG("CAMERA_SVCS: Enter camera_process_terminate", 0, 0, 0);

  /* check if this is not first time through function */
  if (camera_terminate_clk_state == CAMERA_TERMINATE_CLOCK_RUNNING)
  {
    camera_log(LOG_INFO, (uint32)camera_func, 0, (uint32)TERM_RESUME, __LINE__);
    MSG_HIGH("CAMERA_SVCS: Termination Resumed", 0, 0, 0);
    clk_dereg(&camera_terminate_clk);
    camera_terminate_clk_state = CAMERA_TERMINATE_CLOCK_STOPPED;
  }
  else if ( camera_terminate_clk_state == CAMERA_TERMINATE_CLOCK_EXPIRED)
  {
    /* we are here because a timer expired, go to ready state and clean up */
    camera_log(LOG_INFO, (uint32)camera_func, 0, (uint32)TERM_CLK_EXP, __LINE__);
    MSG_HIGH("CAMERA_SVCS: Timed out waiting for termination", 0, 0, 0);
    MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_READY", 0, 0, 0);
    camera_state = CAMERA_STATE_READY;
    camera_terminate_clk_state = CAMERA_TERMINATE_CLOCK_STOPPED;
  }

  /* camera_abort_takepicture_encoding_operation will set
   * camera_take_picture_status.abort to TRUE, call appropriate abort command
   * if capture, YCBCR downsizer, BVCM, LPM, or encoding is in progress, and
   * return TRUE. If camera_abort_takepicture_encoding_operation returns true
   * a timer is set for 2000ms and camera_process_terminate returns. Upon a
   * callback from the abort request or receiving expected frame for capture
   * camera_process_terminate will be called again and we will continue
   * termination. If the timer expires camera_process_terminate is called.
   * In the case of multishot we will continue to return at this
   * point until capture of all frames is complete. With maximum of 16 frames
   * with 1 second delay this means there is a potential delay of 16 seconds.
   * Eventually the DSP firmware will support sending VFE_Idle during capture.
   * When this is true camera_abort_takepicture_encoding_operation should be
   * modified to return false if capture is in progress.
   */
  if ( camera_abort_takepicture_encoding_operation() == TRUE )
  {
    /* set a timer to make sure termination will continue */
    camera_log(LOG_INFO, (uint32)camera_func, 0, (uint32)TERM_WAIT_FOR_ENCODE, __LINE__);
    MSG_HIGH("CAMERA_SVCS: Abort take picture encoding requested. Wait upto 2000ms", 0, 0, 0);
    clk_reg (&camera_terminate_clk, camera_terminate_clk_cb , (int32) 2000, 0, FALSE);
    camera_terminate_clk_state = CAMERA_TERMINATE_CLOCK_RUNNING;
    return;
  }

  if(camera_service_unavailable)
  {
    camera_log(LOG_INFO, (uint32)camera_func, 0, (uint32)TERM_SRVC_UNAVAIL, __LINE__);
    MSG_ERROR("CAMERA_SVCS: QDSP Service Unavailable. Call QDSP Terminate", 0, 0, 0);
    camera_dsp_state = DSP_DISABLED;
    CAMQDSP_Terminate();
  }


  MSG_HIGH("CAMERA_SVCS: camera_process_terminate: camera_state=%d camera_dsp_state=%d",
      camera_state, camera_dsp_state, 0);
  if( (CAMERA_STATE_READY != camera_state) &&
      (CAMERA_STATE_INIT != camera_state) )
  {
    switch ( camera_dsp_state )
    {
      case DSP_DISABLED:
        /* transition to READY state */
        MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_READY", 0, 0, 0);
        camera_state = CAMERA_STATE_READY;
        break;
      case DSP_CONFIGURING:
      case DSP_ENABLING:
        /* wait up to 500ms for Configure to finish */
        camera_log(LOG_INFO, (uint32)camera_func, 0, (uint32)TERM_WAIT_FOR_VFE, __LINE__);
        MSG_HIGH("CAMERA_SVCS: DSP not ready. Terminate will wait upto 500ms for DSP", 0, 0, 0);
        clk_reg (&camera_terminate_clk, camera_terminate_clk_cb , (int32) 500, 0, FALSE);
        camera_terminate_clk_state = CAMERA_TERMINATE_CLOCK_RUNNING;
        return;
      case DSP_CONFIGURED:
      case DSP_ENABLED:
        /* bring vfe idle */
        /* Always issue Idle command, even if was already issued; except when 
         * the below mentioned conditions are satisfied: - 
         * a) FEATURE_VIDEO_ENCODE_THROTTLE is disabled
         * b) camera_state is CAMERA_STATE_RECORDING and 
         * c)VFE is alread in Idle state 
         */
        #ifndef FEATURE_VIDEO_ENCODE_THROTTLE
        if(camera_state != CAMERA_STATE_RECORDING || 
           (camera_state == CAMERA_STATE_RECORDING && camera_dsp_command != CAMQDSP_CMD_IDLE))
        #endif /* ! FEATURE_VIDEO_ENCODE_THROTTLE */
        {
          MSG_HIGH("CAMERA_SVCS: Call to send IDLE Message to DSP", 0, 0, 0);
          (void) camera_send_vfe_idle_and_wait();
        }
        MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_READY", 0, 0, 0);
        camera_state = CAMERA_STATE_READY;
        break;
      default:
        MSG_ERROR("Terminate, wrong DSP state %d", camera_dsp_state, 0, 0);
        break;
    }
  }

  if( (camera_state == CAMERA_STATE_READY) ||
      (camera_state == CAMERA_STATE_INIT) )
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: Termination Cleanup", 0, 0, 0);

    camera_take_picture_status_set_default();
    camera_dsp_error_status_set_default();
    camera_stopping = FALSE;
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE	
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/
    camera_stopping_record = FALSE;
    camera_service_unavailable = FALSE;
#ifdef FEATURE_VIDEO_ENCODE
    camera_video_engine_stop_requested = FALSE;
    camera_video_engine_start_requested = FALSE;
#if defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE && \
  (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING || \
   defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)

  if(camera_dx * camera_dy >= CAMCORDER_QVGA_SIZE)
  {
    clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMCORDER);
  }
#endif
#endif /* FEATURE_VIDEO_ENCODE */

    camera_svcs_optimize_resources();

    /* free preview buffers and terminate dsp */
#ifdef FEATURE_QVPHONE
    if (camera_dsp_state && !camera_qvp_enabled)
#else /* FEATURE_QVPHONE */
    if (camera_dsp_state)
#endif /* FEATURE_QVPHONE */
    {
      /* Camera DSP image is downloaded, disable it */
      camera_dsp_state = DSP_DISABLED;
      camera_log (LOG_VFEC, 0, 0, (uint32)VFE_TERM, __LINE__);
      MSG_CAMERADEBUG("CAMERA_SVCS: Call QDSP Terminate", 0, 0, 0);
      CAMQDSP_Terminate ();
    }

    /* Return all buffers */
    camera_free_preview_buffers();

#ifdef FEATURE_QVPHONE
    if (camera_qvp_enabled)
    {
      MSG_CAMERADEBUG("CAMERA_SVCS: Free QVP Buffers", 0, 0, 0);
      CAM_Free (qvp_enc_buffer0);
      CAM_Free (qvp_enc_buffer1);
      CAM_Free (qvp_enc_buffer2);
      CAM_Free (qvp_enc_buffer3);
      qvp_enc_buffer0 = qvp_enc_buffer1 = qvp_enc_buffer2 = qvp_enc_buffer3
        = NULL;
    }
#endif /* FEATURE_QVPHONE */

#ifdef ANY_FLASH
    if (camera_flash_fired)
    {
#ifdef FEATURE_STROBE_FLASH
      /* disable strobe flash triggering */
      strobe_flash_stop_trigger();
#endif /*  FEATURE_STROBE_FLASH */

#ifdef FEATURE_WHITE_LED_FLASH
      /* turnoff_white_led   */
#ifdef CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH
      gpio_out(CAMIF_FLASH, GPIO_LOW_VALUE);
#else
      BIO_TRISTATE(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_M);
      BIO_OUT(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_OFF_V);
#endif /* CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH */
#endif  /* FEATURE_WHITE_LED_FLASH */
      camera_flash_fired = FALSE;
    }
#endif /* ANY_FLASH */

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
    /* Its possible to get here by pushing a stop button
     * in the middle of a pause ...without freeing the
     * fade image frame
     */
    camera_video_transition_effects_store(NULL);
#endif

    CAMERA_SLEEP_ALLOW ();
    CAMERA_HALT_ALLOW();
    CAMERA_BUS_OFF();

    camera_log(LOG_INFO, (uint32)camera_func, 0, (uint32)TERM_END, __LINE__);
    event_report(EVENT_CAMERA_EXIT);
    MSG_FATAL("CAMERA_SVCS: Termination complete", 0, 0, 0);

    if (camera_app_cb)
    {
      MSG_CAMERADEBUG("Camera_Svcs-->UI stopping_cb:%d camera_func:%d",
          camera_stopping_cb, camera_func, camera_parm4_value);
      (camera_app_cb) (camera_stopping_cb, camera_app_data, camera_func, camera_parm4_value);
    }

    if (camera_tcb)
    {
      MSG_CAMERADEBUG("CAMERA_SVCS: Set CAMERA_SIG", 0, 0, 0 );
      (void) rex_set_sigs (camera_tcb, CAMERA_SIG);
      camera_tcb = NULL;
    }
  }

}

/*===========================================================================

FUNCTION      CAMERA_TERMINATE_CLK_CB

DESCRIPTION
              Callback to queue camera_process_terminate to graph context when
              terminate timer set expires.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 function param not referenced */
static void camera_terminate_clk_cb(int4 ms)
{
    camera_terminate_clk_state = CAMERA_TERMINATE_CLOCK_EXPIRED;
    MSG_HIGH("CAMERA_SVCS: Timed out waiting for abort", 0, 0, 0);
    camera_svcs_queue_call_to_terminate();
}
/*lint -restore */

/*===========================================================================

FUNCTION      CAMERA_SERVICE_UNAVAILABLE_CLK_CB

DESCRIPTION
              Callback to queue camera_process_terminate to graph context when
              service unavailable timer set expires.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 function param not referenced */
static void camera_service_unavailable_clk_cb(int4 ms)
{
  clk_dereg(&camera_service_unavailable_clk);
  camera_service_unavailable = FALSE;
  MSG_HIGH("CAMERA_SVCS: Timed out waiting for DSP service.", 0, 0, 0);
  camera_svcs_queue_call_to_terminate();
}
/*lint -restore */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      CAMERA_REQUEST_AEC_AWB_STATS

DESCRIPTION
              Request stats for AEC and AWB

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_request_aec_awb_stats(void)
{
  CAMQDSP_ReturnCodeType qdspRetCode;

  if (camera_dsp_command != CAMQDSP_CMD_IDLE)
  {
    /* Reduce AEC/AWB stats processing defer count */
    camera_3a_shared_set_aec_awb_delay_count (1, AEC_AWB_DELAY_DECR);
    MSG_CAMERADEBUG ("Decremented, AEC/AWB delay count = %d",
        camera_3a_shared_get_aec_awb_delay_count(), 0, 0);
    /* Send AEC/AWB stats request */
    camera_dsp_command = CAMQDSP_CMD_AE_AWB_STATS_REQ;
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

    qdspRetCode = VFE_AE_AWB_Stats( vfeExpWbStatDspInput );

    if (qdspRetCode != CAMQDSP_SUCCESS)
    {
      /* Must give the AEC AWB VFE Statistics command again */
      MSG_HIGH ("VFE AE AWB Stats command failed, code: %d",
                                           qdspRetCode, 0, 0);
    }
  }
}
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
#endif /* FEATURE_CAMERA */
