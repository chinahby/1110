/*===========================================================================
FILE: qcamcorder.c

GENERAL DESCRIPTION
    This file contains a BREW application qcamcorder. It provides functions to
    view, record, edit, and transcode video.

EXTERNALIZED FUNCTIONS
    QCam_Load

    Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camcorder/apps/6k/main/latest/src/qcamcorder.c#66 $ $DateTime: 2009/05/07 22:46:49 $ $Author: kamit $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
05/07/09   kamit Limiting the max value for bitrate in case of AMC file format to 64kbps.
04/28/09   ps     Featurized frequent pause resume calculation for lcd direct mode.
04/01/09   girip  Correction in featurisation of TELOP text
03/13/09   girip  Fixed problem in progressbar updation while the clip is played
03/04/09   ps     synchronize pause and resume. 
                  Changed timer calculation to avoid exceeding the movie duration limit(if set) in case of frequent pause resume 
03/10/09   girip  Possibility of "Divide by zero" avoided in Qcamcorder_UpdateProgCtl()
03/09/09   girip  One of the argument to ICAMERA_SetAudioEncode() corrected for AAC recording
02/20/09   girip  Code change to accomodate the change of unit of measurement of "m_dwTotalTime" in structure "Qcamcorder" 
                  from seconds to milliseconds.
02/20/09   kamit  Dynamic frame overlay support is added.Change made in Qcamcorder_LoadFrame()
01/30/09   ps     Fixed issue when frequent playback key pressed as it wont create player instance
                  (after once created) untill stop key is pressed or playback aborted.
01/23/09   shiven Fixed GPS menu related issues
01/20/09   girip  Moved filename diplay to next line of recordtime. Both moved to the right end.
                  Playback window is now centered to the space leftout by progressbar and menubar
10/01/08   shiven Crash fix for file merge on MMC for AMC format
01/19/08   shiven Changing Qos timer to 89 sec for standalone gps fixes
01/16/08   sanal  Removed registering of top menu region for full screen playback display.
12/24/08   shiven Featurized remaining postediting features (save, postedit etc) in UI
12/24/08   shiven Fixed playing of shutter sound & recording when BT-SCO is connected 
12/23/08   girip  Featurised audio recording for FTM camcorder
11/26/08   girip  Enabled playing shutter sound for telop text enabled recording &
                  fixed faulty filename increment of 2 instead of 1
11/26/08   shiven Removing 3g2frag recording for MAX & Dynamic bitrate and vice versa
11/24/08   shiven Blocking 3g2frag recording greater than 384kbps
11/25/08   shiven Fixed critical lint warning 
11/25/08   anil   Removed audio dubbing featurization for displaying status bar while playback
11/23/08   shiven Added missing framerate info in post edit read open callback
11/20/08   ps     Migrated from ICM to ITelephone APIs to check voice call during camcorder
11/18/08   shiven Added QVGA Support for H263
11/15/08   ps     Featurized Save option in post editing menu
11/04/08   sanal  Fix compilation warning.
11/03/08   giri   Fixed the preview missing issue for the case when LCD screen size is less than current display mode
11/03/08   sanal  Workaround for handling slow arrival AEE Callbacks
10/30/08   ps     Changed FLASH_DIR_PATH when FEATURE_IMOVIEEDIT_V2 is defined.
10/23/08   ps     Error returning in File merge for different audio formats 
10/10/08   anil     RVCT compiler warning fixes
10/10/08   shiven Fixed Merged Clip duration higher than sum of two merged clip duration
10/06/08   ps     Crash fix for file listing
10/01/08   shiven Fix for File merge on MMC
09/23/08   ps     Done Changes for more than 4GB card support
09/22/08   sanal  Use  QCAMCORDER_IMOVIEEDIT_RELEASE in transcode portion selection
09/12/08   ps     Featurization of Timestamp special effects support.
09/10/08   anil   Featurized audio dubbing related code
08/29/08   shiven Fixed crash during File merge, modified error handling
08/27/08   ps     Replaced GETTIMEMS and GETTIMESECONDS API calls with GETUPTIMEMS as it doesnt need network to be latched for correct time.
08/27/08   shiven Fixed crash when telop text edited file is saved
08/27/08   ps     Chnaged display file list functionality for large number of files
08/19/08   ps     Done changes for Runtime TE detection 
08/09/08   ps     Changed delete all clips functionality by deleting 1000 files at a time 
07/28/08   ps     Sent Delayed Response to the automation client for Transcoding. 
07/25/08   ps     Updated the File to rectify formatting error occured in previous checkin
07/22/08   ps     Error reporting when ".info" file is not present for raw mode encoding.
07/21/08   sanal  Changes to not allow a new playback until audiodub is complete
07/14/08   ps     Changed UI to prevent display of dyanimic overlay option in unsupported mode.
07/10/08   sanal  Added Transcoding error amd unsupported states.
07/09/08   shiven Fix for Kernel panic when aborting file merge operation
07/08/08   shiven Adding support for loci atom
07/04/08   ps     Removed RVCT compiler warnings
07/04/08   yogi   When Audio Dub operation is aborted, modified the code to not generate the output audio dubbed file.
06/30/08   yogi   Removed usage of QCAM_PICTURE_WIDTH/HEIGHT
06/20/08   sanal  Set frame position in partial transcoding using IMovieEdit
06/20/08   ps     Error reporting for unsupported format in Audio Dubbing 
05/22/08   sanal  Exit file merge gracefully when interrupted by call
05/15/08   anil   featurised code with FEATURE_MP4_TRANSCODER and FEATURE_BREW_IMOVIEFILE
04/08/08   sanal  Added NULL checking while IMOVIEEDIT_release
04/03/08   sanal  Synchronous stopping in transcoder
03/17/08   anil   Added support set udta atom
03/26/08   sanal  Fix for MMC audio dubing not working
03/18/08   anil   Fixed Data abort problem in Qcamcorder_DisplayFrame and handled case sensitive problem
                  for linux builds
03/12/08   sanal  Transcoder with Imovieedit, unsupported case was not 
                  handled properly
03/12/08   sanal  Target filebrand menu was partial with IMoviefile transcode
03/10/08   sanal  Fix for end frames not removed when second cut position not
                  selected in cut and merge.
02/18/08   anil   Fixed code to take correct input bitrate from user in 
                  Qcamcorder_GetBitrateHandleEvent
02/11/08   sanal  Adding Idiag support for frames dropped in a recorded clip
02/07/08   dhaval Featurize the code
01/30/08   sanal  Terminate transcoder on receiving error callback.
01/30/08   dhaval  Removed compilation warning
01/29/08   Gopi    Added support for transcoder with movieedit interface
01/23/07   jdas   Added error checking
01/07/07   Yogi   Fixed the special effects menu and Recording Menu when
                  Postcard recording is enabled.

01/02/08   jdas   added png file parsing to check allowed overlay frames
12/14/07   Yogi   Modified code to use MMOverlay.
12/07/07   jdas   Added Dynamic frame overlay UI support
11/26/07   Yogi   Fixed the overlapping menu names problem, minor code cleanup
                  in Qcamcorder_DisplayPreviewMenuName Function.
11/21/07   Yogi   Handled IDB_STOPPED case properly, merged this change from
                  7K Qcamcorder code. 
11/21/07   Yogi   Added Post card recording support 
11/20/07   gopi    Changed the File Merge Api as per brew team comments.
11/19/07   dhaval Removed compilation warnings
10/16/07   dhaval Do Post processing in play callback instead of pause callback
10/10/07   Yogi   Fix: Displayed error messages during postediting of audio
                  only clip weren't appropriate. This has been fixed
10/16/07   dhaval  featurize code for overlay support in transcoder
09/26/07   jdas   fixed playback issue when lcd is smaller than clip resolution
09/25/07   Yogi   Removed Banned API
09/24/07   Yogi   Added Fix: Error thrown during recording, when save_loc is MMC and MMC is removed
09/20/07   jdas   Dynamic Overlay support in camcorder
09/18/07   gopi   Added support for file merge.
09/17/07   dhaval Do not accept save request if it is already in progres
09/14/07   sanal  Added retaining of zoom and QVGA Preview feature
09/11/07   dhaval Removed critical lint errors
09/03/07   Yogi   Replacing FEATURE_AAC_REC with FEATURE_CAMCORDER_AAC_REC in camcorder module 
09/04/07   dhaval Added frame overlay support in transcoder
08/31/07   Kadhir Removed OEMBitmap_priv.h
08/14/07   jdas   Max clip length 15 sec for AMC file brand
07/24/07   Yogi   Added UI for audio only recording.   
07/24/07   jdas   Added error checking
07/17/07   Kadhir Enabled QVGA LOW FPS recording for 6260 target
06/29/07   kadhir Added max and dynamic bitrate support for camcorder recording
06/22/07   dhaval Remove critical lint errors.
06/19/07   kadhir Added FTL and USB as storage media
06/15/07   gopi   Featurize QVGA (FEATURE_QVGANOTSUPPORTED) resolution option in camcorder UI.
                  if FEATURE_QVGANOTSUPPORTED is defined then QVGA option is
                  disabled in UI.
06/11/07   gopi   Fixed issues with recording with MMC/Flash and raw mode encoding
05/24/07   dhaval Corrected error due to flashing screen prevention featurization.
05/16/07   dhaval Flashing screen prevention changes are featurized
05/10/07   jdas   Checked non-NULL before calling IMEDIA functions
05/09/07   dhaval Set user bitrate and quality to transcoder
05/09/07   dhaval Display post edit unsupport message for 3g2 fragement transcoding 
05/08/07   gopi   Convert Elapsed recording time units as milliseconds from seconds.
04/25/07   gopi   Release Imedia object for post editing.
04/17/07   dhaval Return if file open failed
04/03/07   jdas   Checked not-NULL before calling IMEDIA_Stop and IMEDIA_Release
03/28/07   gopi   Fixed SKM brand recording problems with AMR audio format.
03/21/07   Kadhir Added UI support for video stabilization
                  feature (post processing mode).
03/23/07   jdas   Added code for FEATURE_CAMERA_LCD_DIRECT_MODE
03/21/07   Kadhir Fixed bug in Qcamcorder_GetTranscodeFileName()
03/14/07   dw     Prevent release of NULL IMEDIA pointer during audio dubbing abort.
03/08/07   dhaval Removed critical lint errors.
03/08/07   dhaval Allocate memory properly to pfileformat pointer
03/06/07   jdas   Removed Banned API
03/08/07   dhaval Handle case when H.264 transcoder is disable for target.
03/01/07   dhaval Show Post-edit error message if file_open operation fail.
02/26/07   dhaval Allocate a max_extension_size buffer to pFileFormat pointer.
02/22/07   Gopi   Changes to prevent Flashing screen in camcorder app
01/25/06   dhaval Removed the invalid Qtv API usage.
12/21/06   dhaval Removed lint error
12/08/06   dhaval Clear screen before doing partial transcoding for QVGA format.
11/30/06   jdas   Added error handling of ICAMERA_Stop while moving from
                  preview to recording
11/29/06   jdas   Addded error recovery if ICAMERA_Stop() fails when rapid
                  key press of in empty album menu
11/28/06   Pavan  Support added for Moviedit interface
11/27/06   dhaval Display post edit unsupport message while transcoding is
                  not supported
11/23/06   Pavan  Reset Pointers to NULL for Brew 4.0 Release
11/20/06   Pavan  Fixed problem with extra temporary file generated during cut-merge
                  and video-editing for H264 clips
11/20/06   Kadhir Replaced OEMOS_Sleep() with MSLEEP().
11/14/06   dhaval Removed temporary file generated during video edit operation
11/09/06   dhaval Added partial transcoding support
10/30/06   dhaval Added UI support to select the space limit threshold value
10/23/06   jdas   Disabled Qcamcorder app while in VT call
10/11/06   Nagesh Fixed the error due to wrong audio format being sent while file format selection
10/10/06   kadhir Released Media instance immediately after the completion of playback.
10/03/06   Nag    Error message added in case target bit rate is out of range
10/03/06   dhaval Handled the case if User selects no for transcoding in
                  playback mode.
09/27/06   Gopi   Changed usage of space check parameters for post editing.
09/22/06   Gopi   Handle MM_STATUS_ABORT event in media notify function.
09/21/06   Gopi   Included ipl related header files to remove lint warning.
09/04/06   dhaval Added UI support to select the target video codec brand
                  from transcoder
09/01/06   kadhir Removed unwanted Cross Compilation Dependency
                  When qcamcorder Feature Disabled
08/25/06   Kadhir Corrected preview shift in viewfinder
08/23/06   dw     Remove calls to ICAMERA_SetParm if CAM object is released
08/18/06   jdas   Disabling H.264 option from video format menu in case of AMC file brand
08/08/06   Nag    Fixed the error due to cancelling the recording process
                  with Telop Text and Timer enabled
08/04/06   dhaval Removed incorrect messages in edit menu after removing
                  MMC card.
08/03/06   jdas   Removed the hardcoded build id length
07/25/06   jdas   Changes to copy text stream when post editing
07/25/06   Wyh    Remove globals that control special effects
07/24/06   gopi    Fixes file properties crash when MMC is yanked out.
06/30/06   Nag    Modified code to update preview window and progress bar
05/09/06   Nag    Added support for start and end time for Transcoder
05/31/06   kusuma Changed the prototype of Qcamcorder_Transcode to support IDIAG
                  start transcode to send response when it is completed
05/23/06   kusuma Changed the prototype of Qcamcorder_DisplayPreviewMenuName to
                  support the IDIAG smooth zoom feature
05/04/06   wyh    m4v_2_rgb cleanup ... m4v2rgb_init() parameters changed.
05/03/06   kadhir Added support to keep build ID as a part of camcorder config file
03/27/06   dw     Changes for raw mode advanced configurations
03/28/06   claw   Removed unnecessary floating point math in qcamcorder.c
03/21/06   jdas   Added deregister layer which was used to display playback frames
                  in the function Qcamcorder_FreeAppData
03/08/06   kusuma Added support for smooth zoom.
03/06/05   dw     Display recording space out icons during memory shortage
03/06/06   claw   Resolved corrupted UI after rapid start stop.
01/31/06   Gopi   Added idiag suppport for raw mode encoding.
01/27/06   claw   Resolved problem that arises when timer, telop text and
                  shutter sound are all enabled.
01/17/06   Nagesh Added support for giving options to user for deleting
                  files from either Flash,MMC or both.
01/13/06   ali    Enabled all audio formats for all UMTS targets
01/05/05   cla    Added stop softkey option for audio dubbing
01/03/06   Gopi   Added support for launching camcorder App with out sensor
                  into album mode.
                  Code is featurized under feature FEATURE_VIDEONOSENSOR.
                  As a dependency need support from camera team to featurize
                  Sony sensor which doesn't support Auto dectect.
12/22/05   Shahed Fixed image position to better center it during transcoding
                  for all desired transcoding sizes.
12/22/05   gm     Added Events for Camcorder (Record/Transcode/Audio-dub)
12/1/05    Gopi   Added support to start preview in case of space
                  error/warning.
11/30/05   dw     Added display of clip's telop text stream and GPS properties
                  Corrected content of default GPS location gpsi atom
11/21/05   Gopi   Added support for Auto and Manual focus for camcorder.
11/18/05   gm     Modified Qcamcorder_SendPicture to remove compile warning
11/14/05   Shahed New implementation of camcorder space status error/warning msgs.
11/14/05   Gopi   Added support for AAC recording in the function
                  Qcamcorder_CreateCamcorder.
                  Cleaned up function Qcamcorder_ExecuteAudioFormatMenu.
11/11/05   Shahed Undo SPACE error/warning changes.
11/09/05   dw     Removed some critical lint warnings for video editing
11/08/05   Shahed Added support for reporting of detailed SPACE error/warning
                  messages.
11/07/05   wyh    Make Qcamcorder_PostEdit compatible with 3g2 frag
10/28/05   pavan  Changed the prototype of Qcamcorder_DeleteFileResponse to
                  support the IDIAG Delete Recorded Clip feature
10/27/05   dhvl   Prototype of QCamcorder_PlayAllGetNextFile and
                  QCamcorder_PlayMovie is changed for IDIAG.
10/20/05   dw     Disable mdp transparency / alpha blend when using FGL only
10/17/05   gm     Automated script to modify the MSG macro's
10/11/05   wyh    Fixed Cut-n-Merge blockiness.
09/26/05   dw     Added support for displaying file name during recording
                  Fixes to restart preview if start recording failed
09/23/05   dw     Telop text fixes to allow preview restart after MT calls
09/23/05   dw     Modify preview state change for error recovery scenarios
09/13/05   dw     Code enhancements for Raw Mode feature
08/26/05   alin   Add timed text stream support to cutmerge
08/24/05   cl     Fix for corrupted camcorder menu after rapid start-stop
                  record.
                  Fix for  background not updated when user goto album.
08/24/05   wyh    Workaround for OEMMediaMPEG42PV_GetVideoTrackHeader failure
                  in Qcamcorder_CutMerge.
08/22/05   gopi   fixed coverity errors
                  Functions modified: QCamcorder_CopyToShared,
                                      Qcamcorder_LoadFrame.
08/18/05   wyh    Use media time instead of EStime in VideoEditing and CutMerge
08/18/05   alin   Add timed text stream support for video editing
08/17/05   dw     Enable countdown time after telop text is entered
                  Removed compiler warnings, cleaned up Qcamcorder_Record
08/17/05   cl     changes made by Scott based on feedback from Coverity tool
08/17/05   wyh    Qcamcorder_CutMerge do not do audioHeader separately.
08/16/05   cl     Added support for setting multiple sets of uuid.
08/16/05   gopi   Replaced MM_QCP* with local camcorder enum members
05/12/05   dw     Featurized album mode to handle YCbCr/RGB in 3.x & 4.x builds
08/11/05   alin   Audiodubbing fixes: Enable video-only playback;  Map camcorder
                  to BREW audio format;  Fix memory leaks,  IMovieFile bug.
08/09/05   dw     Modified album playback to handle YCbCr/RGB frames from IMEDIA
08/09/05   cl     Made Qcamcorder app available while in PS data call.
08/04/05   alin   Resize overlay frame for preview if preview size != encode size
08/02/05   dw     Modified IDIAG support for Cut and Merge feature
08/02/05   cl     Added an option in album mode (play-all) where all recorded
                  clips can be played
08/02/05   drl    Fix for eqmpty album not displaying preview.
07/29/05   wyh    VideoEditing/CutMerge changes to address CRs
07/13/05   cl     Lint-free qcamcorder.c
07/12/05   dw     Added user notification system for post editing errors
                  Removed frame overlay during recording, reduced lint errors
07/12/05   cl     Return to Preview if album is empty.
                  When user presses the clr key to go back to album,
                  UI highlights the last clip that was played.
07/08/05   alin   Restart + Pause QTV playback prior to video editing
06/30/05   cl     Fixed white balance menu focus.  Revised Lint changes.
06/27/05   cl     Fix for unable to save GPS
06/24/05   wyh    New API for m4v2rgb_test_codedVOP()
06/23/05   cl     Linted qcamcorder.c
06/21/05   dw     Renabled AAC advanced menus.
06/17/05   alin   Make frame overlay visible during recording
06/15/05   dw     Added support for advanced AAC settings, but function
                  of new menus currently disabled.
06/14/05   alin   Set preview to rec flag to true when using rec timer
06/07/05   dw     Added support for camsensor antibanding
                  Improved processing of camera cb to reflect error status
                  Improved target bitrate menu to accept user typed input
06/07/05   alin   Modify frame overlay to use BREW PNG decoder - supports QVGA
                  frame overlays
06/07/05   dw     Added feature for QTV vdec support during playback
06/02/05   dw     Updated album mode playback using new OEMMedia APIs.
05/31/05   dw     Added full screen playback display capability.
05/25/05   dw     Fixes to handle frame overlay size larger than encode size.
                  Fixes for disabling qcamcorder while in call.
                  Added hooks to prepare for full screen playback display.
05/17/05   dw     Added menu icons for AAC audio and special fade effects
05/10/05   dw     Enabled all postediting to work between FLASH and MMC
                  Added return boolean for Video Editing
05/09/05   drl    Added CAM_STATUS_ABORT handling of m_bPreview_to_Record
05/05/05   ali    Do not save files when VideoEditing Encounter an error.
05/05/05   wyh    VideoEditing return rebustly when encountering an error.
05/04/05   dw     Enable Qcamcorder_DeleteAllClips to work with IDIAG
                  Changed bitrate menu, effects & merge icons addressing CR63037
05/02/05   jk     Fixed MMS send picture.
04/27/05   dw     Added H264 option within new Video Format menu.
                  Added support for displaying MMC usage.
04/26/05   wyh    VideoEditing: Generate sync frames after 1st fragment.
                                Generate footer at end of frame.
04/22/05   dw     Added raw mode support
04/20/05   wyh    Qcamcorder_PostEdit compatible with new IMoviefile_Read
04/15/05   wyh    Use OEMOS_Sleep() in VideoEditing so UI don't start others
04/13/05   jk     Merged in a fix to a MMS problem from msmshared #106.
04/12/05   dw     Enable continuous recording mode to work with CLR key
04/06/05   wyh    Video editing for H.263 and MPEG4 short header
04/05/05   dw     Fixed invalid menu settings for short header
04/04/05   dw     Enabled continuous recording with fading special effects
                  Fixed language header bits for audio dubbing.
                  Removed featurization for MDP.
03/30/05   dw     Added IDIAG code support for 6550 Qcamcorder
                  Fixed invalid menu settings for short header
03/29/05   dw     Fixed audio dubbing to use fixed rate QCELP header only.
03/29/05   wyh    Fix VideoEditing to make it work for files over 47sec.
03/23/05   dw     Removed Memory Leak during MDP Registration.
                  Added seperate UI for QVGA Preview and encode size.
03/16/05   dw     Added check on/off box for time stamp menu option.
                  Fixed menu file list display bug due to MDP registration.
                  Fixed bug to enable continuous recording mode.
                  Fixed bug to prevent erasement of time display in
                  recording and in the file properties screen.
03/16/05   drl    Added Rapid Start/Stop Support
03/16/05   wyh    Video Editing to work with fragmented files.
03/01/05   dw     Added menu option for time stamp special effects
                  Re-merged transcoded file extension to .3gp default
                  Prevent QVGA and QCIF frame size encoding for amc files
                  Removed multiple MDP registrations to sync with QTV playback
                  Added bug fix for enabling Short Header mode
03/01/05   wyh    AudioDub work with fragmented 3g2 input files.
03/01/05   ali    Changes for 3gp brand in transcoder
02/22/05   dw     Added globals for fade in/out video encoding effects
02/22/05   wh/dw  Fading/bitrate control.
02/17/05   jk     Added support for fragmented 3g2 brand.
01/26/05   dw     Fixes for getting zoom parameters only after camera callback
                  when transitioning from Record to Preview modes.
                  Reversed QCam_Sizes dimensions for correct rotation in preview.
                  Changed feature name of bitrate menu to FEATURE_VIDEO_ENCODE_BITRATE.
                  Fixes for MMC premature extraction.
01/20/05   dw     Added and featurized new user specified bit rate menu
                  Added bug fix to remove viewfinder image from settings/GPS menu
                  Added bug fix to stop timer countdown if GPS menu is activated
01/19/05   wh     Changed name of overlay feature to avoid confusion with pre-
                  6550 overlay feature name.
01/18/05   dw     Added routines for featurized frame overlay UI support
01/08/05   dw     Fixes for full display of telop text during text entry
12/13/04   ssc    #include file name case corrections for builds in
                  case-sensitive Linux build environment.
12/09/04   dw     Enabled post editing features
12/07/04   dw     Fixes for displaying edit menu icons during video playback
                  Registered edit/playback menu for full screen MDP display
12/01/04   dw     Added Negative Effects option into menu and preview
12/01/04   dw     Removed CIF option for video encode size
11/22/04   dw     Centralized position of video display during audio dubbing
11/18/04   dw     Repositioned progress bar during playback for all video sizes
                  Associate progress bar display with star key menu toggling
11/17/04   dw     Added star key option to toggle preview and edit menu display
                  Increased display size for QVGA video playback
11/16/04   dw     Added menu options check for dynamic MMC detection
09/16/04   drl    Add MDP Support.  Removed SQCIF outline boxes.
09/15/04   wyh    Free old menu before allocating a new menu.

08/18/04   sch    uncommenting the IDISPLAY_Update().
06/22/04   ali    Fixes for Cut/Merge
06/14/04   ali    Fix for shutter sound
06/11/04   ali    Check space before initiating any post-recording features
                  Fixed the space warning message
                  Fixed the deletion of file after viewing properties
                  Added zoom item to pause screen
                  Added continuos mode recording support
06/03/04   ali    Changes to zupport zoom while recording paused
06/03/04   ali    Changes for Cut/Merge and Post Editing
05/17/04   ali    Changes for common directory structure.
                  Added a new error message
05/17/04   ali    Changes for Brew 3.0
04/30/04   ali    Changes for various UI bug fixes:
                  Unexpected menus in file list view
                  File properties show file size
                  Audio dubbing/Video Editing fixes
04/21/04   ali    Changes for audio dubbing, where input file can have video
                  and audio streams flipped
04/13/04   ali    Fixed some lint errors
04/08/04   ali    Fix for no preview with timer/telop enabled
04/07/04   ali    Fix for audio dubbing, when target file has no audio
04/07/04   ali    Fixed some lint errors/warnings. Code cleanup
03/26/04   ali    Always set the variable to NULL after freeing it
03/23/04   ali    Changes to end the transcoder by stopping it, or terminating
                  when there is an incoming call
03/08/04   ali    Changes for timer, effects, rotation
03/02/04   ali    Change the pause button to play when in pause state
03/02/04   ali    Changes for offline encoder
2/23/04    ali    Changes to correspond to ICamera changes
01/28/04   aaa    Merged changes from Jaguar branch onto mainline
12/15/03   aaa    Set short header before recording
12/9/03    aaa    Fixed some issues with Telop
12/8/03    aaa    Changes for Video Editing, adding bar to playback,
                  Telop text
11/6/03    aaa    Changes for BREW 3.0
08/13/03   aaa    Added Revision history. Made changes to stop recording when
                  recieving space warning.  Memory status display
============================================================================*/
//lint -save -e611
//lint -save -e740
//lint -save -e620
//lint -save -e641
//lint -save -e613
//lint -save -esym(754,version)
//lint -save -esym(754,flags)
//lint -save -esym(754,copy_guard_attribute)
//lint -save -esym(754,limit_date)
//lint -save -esym(754,limit_period)
//lint -save -esym(754,limit_count)
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "comdef.h"
#include "qcamcorder_msg.h"
#include "BREWVersion.h"
#include "event.h"
#include "AEEstd.h"

#ifdef FEATURE_MP4_TRANSCODER
#include "transcoder_task.h"
#endif

#include "ipl_types.h"
#include "ipl_convert.h"

#if defined(FEATURE_APP_QCAMCORDER)
#include "qcamcorder.h"
#include "Qcamcorder.bid"
#include "AEEText.h"
#include "OEMFeatures.h"
#include "AEEMediaUtil.h"

#ifdef FEATURE_MMOVERLAY
#include "AEECLSID_MMOVERLAYMGR.BID"
#endif /* FEATURE_MMOVERLAY */

#include "AEEStdLib.h"
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
#include "AEEMovieFile.h"
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */
#ifdef FEATURE_BREW_IMOVIEEDIT
#include "AEEMovieEdit.h"
#endif /* FEATURE_BREW_IMOVIEEDIT */
#include "AEEMediaMPEG4.h"
#include "OEMMediaMPEG4.h"
#ifdef FEATURE_APP_MMS
#error code not present
#endif /* FEATURE_APP_MMS */

#include "ICameraExt.h"

#include "camcorder_util.h"

#include "assert.h"
#include "QcamDisp.h"
#include "IYCbCr.h"
#ifdef FEATURE_VIDEO_ENCODE_BITRATE
#include "AEEShell.h"
#endif

#include "mobile.h"
#include "QcamDef.h"
#define MAXSIZE_OF_BUILD_ID 100

/* AEE_SHARED_DIR is defined as "Shared" in AEE.h. */
#undef  AEE_SHARED_DIR
#define AEE_SHARED_DIR     "shared"

#define MS_IN_SEC 1000

//shahed - for new SPACE MSGS SUPPOPT until new BREW Support
//These definition will move to AEECamera.h later.
#ifndef CAM_STATUS_SPACE_WARNING_SAMPLE_TABLE
#define CAM_STATUS_SPACE_WARNING_SAMPLE_TABLE   (CAM_STATUS_BASE + 51)
#endif
#ifndef CAM_STATUS_SPACE_WARNING_CHUNK_TABLE
#define CAM_STATUS_SPACE_WARNING_CHUNK_TABLE    (CAM_STATUS_BASE + 52)
#endif
#ifndef CAM_STATUS_SPACE_WARNING_STORAGE
#define CAM_STATUS_SPACE_WARNING_STORAGE        (CAM_STATUS_BASE + 53)
#endif
#ifndef CAM_STATUS_SPACE_WARNING_MOVIE_DURATION
#define CAM_STATUS_SPACE_WARNING_MOVIE_DURATION (CAM_STATUS_BASE + 54)
#endif
#ifndef CAM_STATUS_SPACE_ERROR_CHUNK_TABLE
#define CAM_STATUS_SPACE_ERROR_CHUNK_TABLE      (CAM_STATUS_BASE + 55)
#endif
#ifndef CAM_STATUS_SPACE_ERROR_SAMPLE_TABLE
#define CAM_STATUS_SPACE_ERROR_SAMPLE_TABLE     (CAM_STATUS_BASE + 56)
#endif
#ifndef CAM_STATUS_SPACE_ERROR_MOVIE_DURATION
#define CAM_STATUS_SPACE_ERROR_MOVIE_DURATION   (CAM_STATUS_BASE + 57)
#endif
#ifndef CAM_STATUS_SPACE_ERROR_STORAGE
#define CAM_STATUS_SPACE_ERROR_STORAGE          (CAM_STATUS_BASE + 58)
#endif
#ifndef CAM_STATUS_SPACE_ERROR_VIDEO
#define CAM_STATUS_SPACE_ERROR_VIDEO            (CAM_STATUS_BASE + 59)
#endif

#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif //#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE

static boolean Qcamcorder_IsValidFileName(char* filename);
static char * QCamcorder_GetFileNameExtension(char * pszFileName);
static void Qcamcorder_BuildSpecialEffectsMenu(Qcamcorder *pMe, uint16 selectMenuItem);
static void Qcamcorder_BuildEditMenu(Qcamcorder *pMe);

#ifdef FEATURE_MP4_TRANSCODER
static void Qcamcorder_StartPartialTranscoding(Qcamcorder *pMe);
static boolean Qcamcorder_TranscodePortionSelection(Qcamcorder *pMe);
void Qcamcorder_Transcode(Qcamcorder *pMe);
static void Qcamcorder_DisplayFrame(Qcamcorder *pMe, xcoder_video_cont_status_type * pData);
static void Qcamcorder_ExecuteTranscoderVideoMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteTranscoderAudioMenu(Qcamcorder* pMe);
#endif
#define QCAMCORDER_IMEDIA_PAUSE(media) if(media){         \
                  (void) IMEDIA_Pause(media);} else {     \
                  QCAMCORDER_MSG_ERROR("media is NULL while pausing", 0, 0, 0);} 
#define QCAMCORDER_IMEDIA_RESUME(media) if(media){        \
                  (void) IMEDIA_Resume(media);} else {    \
                  QCAMCORDER_MSG_ERROR("media is NULL while resume", 0, 0, 0);}
#define QCAMCORDER_IMEDIA_STOP(media) if(media){          \
                  (void) IMEDIA_Stop(media);} else {      \
                  QCAMCORDER_MSG_ERROR("media is NULL while stop", 0, 0, 0);}
#define QCAMCORDER_IMEDIA_RELEASE(media) if(media){       \
                  (void) IMEDIA_Release(media);           \
                  media = NULL;} else {                   \
                  QCAMCORDER_MSG_ERROR("media is NULL while release", 0, 0, 0);}

#define QCAMCORDER_IMOVIEEDIT_RELEASE(movieedit) if(movieedit){ \
                  (void)IMOVIEEDIT_Release(movieedit);          \
                  movieedit = NULL;                             \
}
#ifdef FEATURE_VIDEO_ENCODE_RAW
#define CARD_0_STRING       "card0"
#ifdef FEATURE_IMOVIEEDIT_V2
  #define FLASH_DIR_PATH      "mod/qcamcorder/media/"
#else
  #define FLASH_DIR_PATH      "brew/mod/qcamcorder/media/"
#endif
#define MMC_DIR_PATH        "mmc1/media/"
#define INFO_FILE_EXT       ".info"
#define PART_FILENAME_BYTES 6
#define STRLCPY_NULLLEN_BYTES 1
#define FULL_FILENAME_BYTES (10 + STRLCPY_NULLLEN_BYTES)
#define GETLINE_MAX_CHARS   100
#endif /* FEATURE_VIDEO_ENCODE_RAW */

static void Qcamcorder_ShowconfirmMenu(Qcamcorder *pMe);
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
int OEMMediaMPEG42PV_SetAudDub(uint16 audioFormat);
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */
static boolean Qcamcorder_CheckVoiceCallState( Qcamcorder* pMe);
static void Qcamcorder_TelephoneListener (Qcamcorder* pMe, ModelEvent* pEvent);
static void Qcamcorder_HandleVoiceCallRejected( Qcamcorder *pMe);

static int Qcamcorder_GetFilecount(Qcamcorder *pMe);
static void Qcamcorder_DrawIcon(Qcamcorder *pMe, uint16 nResID);
static void Qcamcorder_StartRecording(Qcamcorder* pMe);
static void Qcamcorder_DelayedRecording(Qcamcorder* pMe);
static void Qcamcorder_DisplayRecordTime(Qcamcorder* pMe);
static void Qcamcorder_DisplayRecordFileName(Qcamcorder* pMe);
static boolean Qcamcorder_CheckSpace(Qcamcorder *pMe);
static void Qcamcorder_DisplayPostRecordingMessage(Qcamcorder *pMe);

#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
#ifndef FEATURE_BREW_IMOVIEEDIT
static uint32 Qcamcorder_GetFileBrand(Qcamcorder* pMe);
#endif//FEATURE_BREW_IMOVIEEDIT
boolean Qcamcorder_VideoEditing(Qcamcorder * pMe);
static void Qcamcorder_EditFileResponse(Qcamcorder * pMe, uint16 id);
#ifdef FEATURE_MP4_DECODER
uint32 OEMMediaMPEG42PV_GetMaxAudioFrameSize(void);
uint32 OEMMediaMPEG42PV_GetMaxVideoFrameSize(void);
boolean OEMMediaMPEG42PV_GetVideoTrackHeader(byte * pBuf, uint32 *pBufSize);
boolean OEMMediaMPEG42PV_GetAudioTrackHeader(byte * pBuf, uint32 *pBufSize);
boolean OEMMediaMPEG42PV_GetVideoFrame(uint32 FrameNum,
                                       AEEDataFrameInfo * pFrameInfo, byte *pBuf,
                                       uint32 bufSize);
boolean OEMMediaMPEG42PV_GetAudioFrame(uint32 FrameNum,
                                       AEEDataFrameInfo * pFrameInfo, byte *pBuf,
                                       uint32 bufSize);
boolean OEMMediaMPEG42PV_GetAudioFrameInfo(uint32 timeStamp,  AEEDataFrameInfo * pFrameInfo);
boolean OEMMediaMPEG42PV_GetVideoFrameInfo(uint32 timeStamp,  AEEDataFrameInfo * pFrameInfo);
boolean OEMMediaMPEG42PV_GetSyncFrameInfo(uint32 timeStamp, boolean reverse, AEEDataFrameInfo * pFrameInfo);
#endif//FEATURE_MP4_DECODER
#endif//FEATURE_QCAMCORDER_VIDEO_EDITING

#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
static boolean Qcamcorder_validPNG(Qcamcorder* pMe,const char* pFile);
static int32 Qcamcorder_LoadFrame(Qcamcorder *pMe);
static void Qcamcorder_ClearOverlayFileName(Qcamcorder *pMe, AEERect *rc);
static void Qcamcorder_Callback_LoadFrame(Qcamcorder *pMe);
#endif

#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
boolean Qcamcorder_CutMerge(Qcamcorder * pMe);
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
int Qcamcorder_SetDirectMode(Qcamcorder *pMe, boolean bDirectMode);
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static void   QCamcorder_InitMdp               (IDisplay   * pIDisp);
static void   QCamcorder_MdpDeregister         (void);

static void   QCamcorder_MdpRegisterPreview    (Qcamcorder * pMe,
                                                boolean    transparency);
static void   QCamcorder_MdpRegisterFullScreen (Qcamcorder * pMe,
                                                boolean    transparency);

static IMMOverlay * QCamcorder_MdpRegister (IDisplay *pIDisp,
#ifdef FEATURE_MMOVERLAY
                                           IMMOverlayMgr *pIMMOverlayMgr,
#endif /* FEATURE_MMOVERLAY */
                                           AEERect rc, 
                                           boolean transparency);

static void   QCamcorder_ToggleMenuDisplay     (Qcamcorder *pMe,
                                                QcamcorderSoftMenuType type);

static boolean Qcamcorder_HandleEvent(IApplet * pi, AEEEvent eCode,
                                      uint16 wParam, uint32 dwParam);
static boolean Qcamcorder_MapEventHandlers(Qcamcorder *pMe, IDialog *pIDialog, uint16 wDlgResID);
static boolean Qcamcorder_CreateCamcorder(Qcamcorder* pMe);
static void Qcamcorder_ReleaseCamcorder(Qcamcorder* pMe);
static boolean Qcamcorder_InitData (IApplet * pi);
static boolean QcamcorderInit( Qcamcorder * pMe );
static void Qcamcorder_FreeAppData (Qcamcorder * pMe);
static void Qcamcorder_DisplayWarningMessage(Qcamcorder *pMe, uint32 msg);
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
static void Qcamcorder_ExecuteConfirmMenu(Qcamcorder *pMe);
#endif
static void Qcamcorder_CamEventNotify(Qcamcorder * pMe, AEECameraNotify * pcn);
#ifdef FEATURE_BREW_IMOVIEEDIT
static void Qcamcorder_CutReadOpenCB(void * pUser);
static void Qcamcorder_CutWriteOpenCB(void * pUser);
static void Qcamcorder_MovieEditCutCB(void * pUser);
static void Qcamcorder_CutWriteCloseCB(void * pUser);
static void Qcamcorder_CutReadCloseCB(void * pUser);
static void Qcamcorder_AudioDubReadOpenCB(void * pUser);
static void Qcamcorder_AudioDubWriteOpenCB(void * pUser);
static void Qcamcorder_MovieEditAudioDubCB(void * pUser);
static void Qcamcorder_AudioDubWriteCloseCB(void * pUser);
static void Qcamcorder_AudioDubReadCloseCB(void * pUser);
static void Qcamcorder_PostEditReadOpenCB(void * pUser);
static void Qcamcorder_PostEditWriteOpenCB(void * pUser);
static void Qcamcorder_PostEditReadExtensionCB(void * pUser);
static void Qcamcorder_MovieEditPostEditCB(void * pUser);
static void Qcamcorder_PostEditWriteCloseCB(void * pUser);
static void Qcamcorder_PostEditReadCloseCB(void * pUser);
#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif //#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#endif /* FEATURE_BREW_IMOVIEEDIT */
static void Qcamcorder_ErrorEventUserNotify(Qcamcorder* pMe);
static void Qcamcorder_BuildPreviewMenu(Qcamcorder *pMe);
static void Qcamcorder_SmoothZoom(Qcamcorder * pMe);
static void Qcamcorder_OnFocus(Qcamcorder * pMe, uint16 key);
static void Qcamcorder_RemoveFocusIcon(Qcamcorder * pMe);
static void Qcamcorder_ShowAFIndicator(Qcamcorder *pMe);
/*------------------------------------------------------
Helper function
----------------------------------------------------------*/
static void Qcamcorder_BuildMenu(Qcamcorder *pMe, QcamcorderMenuType type);
static void Qcamcorder_SetMenuStyle(IMenuCtl* pIMenu, int so_x, int so_y);
static void Qcamcorder_BuildSubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected);
void Qcamcorder_BuildSoftMenu(Qcamcorder *pMe, QcamcorderSoftMenuType type);

static void Qcamcorder_PlayStartSound(Qcamcorder* pMe);
static void Qcamcorder_MediaNotify(void* pUser, AEEMediaCmdNotify* pCmdNotify);
static void Qcamcorder_PlayMediaNotify(void* pUser, AEEMediaCmdNotify* pCmdNotify);
void Qcamcorder_Record(Qcamcorder *pMe, QcamcorderRecordType type);

void Qcamcorder_GetMemUsage(Qcamcorder* pMe, Qcamcorder_MemUsageData *retVal,
                            QcamcorderSaveType saveLoc);
static void Qcamcorder_MemUsage(Qcamcorder* pMe);
static uint16 Qcamcorder_GetTotalClips(Qcamcorder * pMe);
boolean Qcamcorder_DeleteAllClips(Qcamcorder * pMe);
static boolean Qcamcorder_DeleteOldestFile(Qcamcorder * pMe);
void Qcamcorder_DeleteFileResponse(Qcamcorder * pMe, uint16 id);
static void Qcamcorder_GetGPSInfo(Qcamcorder *pMe);
static void Qcamcorder_UpdateGPSIcon(Qcamcorder *pMe);
static void Qcamcorder_BuildGPSMenu(Qcamcorder *pMe, boolean PostEdit);
static void Qcamcorder_BuildSizeSubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected);
static void Qcamcorder_BuildAudioFormatSubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected);
static void Qcamcorder_ExecuteTimerMenu(Qcamcorder* pMe);
static void Qcamcorder_BuildTimerMenu(Qcamcorder *pMe);
static void Qcamcorder_DisplaySelfTimerBar(Qcamcorder* pMe);
static void Qcamcorder_BuildPostEditMenu(Qcamcorder *pMe);
static void Qcamcorder_ExecutePostEditMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteGPSMenu(Qcamcorder *pMe, boolean PostEdit);
static void Qcamcorder_BuildSpaceThresholdMenu(Qcamcorder *pMe);
#ifdef FEATURE_QCAMCORDER_TELOP
static void Qcamcorder_BuildTelopMenu(Qcamcorder *pMe);
#endif
static void Qcamcorder_GetConfigRecord(Qcamcorder* pMe);
void Qcamcorder_SetConfigRecord(Qcamcorder* pMe);

static void Qcamcorder_DisplayEditMenuPic(Qcamcorder* pMe);
static void Qcamcorder_DisplayEditMenuName(Qcamcorder* pMe);
static void Qcamcorder_DisplayFileList(Qcamcorder *pMe);
#ifdef FEATURE_VIDEO_ENCODE_RAW
static void Qcamcorder_RawEncoderDisplayFileList(Qcamcorder *pMe);
static char* Qcamcorder_GetRawEncoderFileName(Qcamcorder* pMe,
                                              RawEncoderConfigType* cfg);
boolean Qcamcorder_RawEncoderInit(Qcamcorder *pMe);
static void Qcamcorder_RawEncoderStop(Qcamcorder *pMe);
#ifdef FEATURE_MP4_DECODER
static void Qcamcorder_DisplayRawFrame(Qcamcorder *pMe,
                                       raw_encoder_video_display_type * pData);
#endif
static uint16 Qcamcorder_RawEncoderMenuParser(Qcamcorder *pMe,
                                              RawEncoderConfigType* cfg);
static uint16 Qcamcorder_RawEncoderInfoParser(Qcamcorder *pMe,
                                              RawEncoderConfigType* cfg);
#endif /* FEATURE_VIDEO_ENCODE_RAW */
static uint8 Qcamcorder_ShowProperties(Qcamcorder* pMe);
static void Qcamcorder_RecordTimer(Qcamcorder *pMe);
static void Qcamcorder_RecordTelopTimer(Qcamcorder *pMe);

static char* Qcamcorder_GetFileName(Qcamcorder* pMe);
static void Qcamcorder_selfTimerCB(Qcamcorder* pMe);

static void Qcamcorder_DisplayMesssage(Qcamcorder *pMe, char *title, char *text);

static uint16 Qcamcorder_ExecuteSubMenu(Qcamcorder* pMe, uint16 * idList);
static uint16 Qcamcorder_ExecuteSizeSubMenu(Qcamcorder* pMe, uint16* idList);
static void Qcamcorder_ExecuteSoundMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteSpecialEffectsMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteSizeMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteQualityMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteAudioFormatMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteNumClipsMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteClipLengthMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteSaveLocMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteFileFormatMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteVideoFormatMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteWhiteBalanceMenu(Qcamcorder* pMe);
#ifdef FEATURE_QCAMCORDER_TELOP
static void Qcamcorder_ExecuteTelopMenu(Qcamcorder* pMe);
#endif
static void Qcamcorder_BuildSoundMenu(Qcamcorder *pMe);
static void Qcamcorder_BuildContMenu(Qcamcorder *pMe);
static void Qcamcorder_ExecuteContMenu(Qcamcorder* pMe);

void Qcamcorder_Preview(Qcamcorder* pMe);
void Qcamcorder_PlayMovie(Qcamcorder* pMe);
boolean Qcamcorder_PlayAllGetNextFile(Qcamcorder* pMe);
static uint8 Qcamcorder_PreviewKeyHandler(Qcamcorder* pMe, AEEEvent eCode, uint16 key);
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
static void Qcamcorder_PlayAudioDubMovie(Qcamcorder* pMe);
static void Qcamcorder_RecordAudio(Qcamcorder* pMe);
static void Qcamcorder_StartRecordAudio(Qcamcorder* pMe);
static void Qcamcorder_AudioDub(Qcamcorder * pMe);
#ifdef FEATURE_BREW_IMOVIEEDIT
static void Qcamcorder_AudioDubCleanUp(Qcamcorder *pMe);
#endif /* FEATURE_BREW_IMOVIEEDIT */
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */

#ifdef FEATURE_CAMCORDER_AAC_REC
static void Qcamcorder_BuildAACConfigMenu(Qcamcorder *pMe);
static void Qcamcorder_BuildAACModeMenu(Qcamcorder *pMe);
static void Qcamcorder_BuildAACFreqMenu(Qcamcorder *pMe);
static void Qcamcorder_BuildAACBitrateMenu(Qcamcorder *pMe);
static void Qcamcorder_ExecuteAACConfigMenu(Qcamcorder *pMe);
static void Qcamcorder_ExecuteAACModeMenu(Qcamcorder *pMe);
static void Qcamcorder_ExecuteAACFreqMenu(Qcamcorder *pMe);
static void Qcamcorder_ExecuteAACBitrateMenu(Qcamcorder *pMe);

#endif /* FEATURE_CAMCORDER_AAC_REC */
static void Qcamcorder_ExecuteSpaceMenu(Qcamcorder *pMe);


static void Qcamcorder_ExecuteFrameRateMenu(Qcamcorder* pMe);
static const char * Qcamcorder_GetDirectory(Qcamcorder * pMe);

#ifdef FEATURE_QCAMCORDER_TELOP
boolean Qcamcorder_GetInputHandleEvent(void * pUser, AEEEvent evt, uint16 wParam, uint32 dw);
#endif
#ifdef FEATURE_VIDEO_ENCODE_BITRATE
#define MAX_3G2_FRAG_BITRATE_SUPPORTED 393216 /* 384 kbps */
#define MAX_AMC_BITRATE_ALLOWED 64000
boolean Qcamcorder_GetBitrateHandleEvent(void * pUser, AEEEvent evt, uint16 wParam, uint32 dw);
#endif

static boolean QProgCtl_Init(QProgCtl * pme, AEERect * pRectMain);
static void QProgCtl_DrawHist(QProgCtl * pme, Qcamcorder *pCdr,  int16 wPct);
static void QProgCtl_SetPos(QProgCtl * pme, Qcamcorder *pCdr, int16 wPct);
static void Qcamcorder_UpdateProgCtl(Qcamcorder * pme);

#define QC_CLR_PROG_FILL      MAKE_RGB(0, 128, 192)
#define QC1_CLR_PROG_FILL      MAKE_RGB(255, 0, 0)
//#define QC2_CLR_PROG_FILL      MAKE_RGB(255, 0, 255)
AECHAR sVersion[] = {'1','.','0', 0};
uint32  efx_special_fade = 0;

char *DeleteFileName;

#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif //#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE

#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
static void Qcamcorder_TranscodeCb (void *puser);
static void Qcamcorder_transcoderAbort (Qcamcorder *pMe);
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE

#ifdef FEATURE_MP4_TRANSCODER
#ifndef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
static void Qcamcorder_Transcoder_End(Qcamcorder *pMe);
#endif
static void Qcamcorder_DisplayAlbum(Qcamcorder* pMe);
static void Qcamcorder_ExecuteTranscoderBrandMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteTranscoderVideoCodecMenu(Qcamcorder* pMe);
#endif
static void Qcamcorder_ExecuteEffectsMenu(Qcamcorder* pMe);
static void Qcamcorder_ExecuteAntibandingMenu(Qcamcorder* pMe);
static void Qcamcorder_BuildEffectsMenu(Qcamcorder *pMe);
static void Qcamcorder_BuildAntibandingMenu(Qcamcorder *pMe);
static void Qcamcorder_ExecuteRotateMenu(Qcamcorder* pMe);
static void Qcamcorder_BuildRotationMenu(Qcamcorder *pMe);

#ifdef FEATURE_DVS
static void Qcamcorder_BuildDVSMarginMenu(Qcamcorder *pMe);
static void Qcamcorder_ExecuteDVSMarginMenu(Qcamcorder *pMe);
#endif

#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
static void Qcamcorder_AddUdatAtom(Qcamcorder * pMe);
#endif
//Max no of calls in system
#define QCAMCORDER_MAX_NUM_CALLS 13
#define ITELEPHONE_IS_VOICECALL_PRESENT(po) \
 Qcamcorder_CheckVoiceCallState(po)

#ifdef FEATURE_BREW_3_0
const char QCAMCORDER_MOVIES_DIR_MMC[] = "fs:/card0/media";
#else
const char QCAMCORDER_MOVIES_DIR_MMC[] = "mmc1/media";
#endif /* FEATURE_BREW_3_0 */

#ifdef FEATURE_BREW_3_0

#ifdef FEATURE_COMMON_MM_DIR
#define QCAMCORDER_MOVIES_DIR_FLASH   "fs:/shared/media"
#else
#define QCAMCORDER_MOVIES_DIR_FLASH   "fs:/~/media"
#endif

#else
#define QCAMCORDER_MOVIES_DIR_FLASH  "media"
#endif

#define QCAMCORDER_MOVIES_DIR_MMC  AEEFS_MMC_DIR "media"

#ifdef FEATURE_EFS_FTL
#define QCAMCORDER_MOVIES_DIR_FTL  AEEFS_FTL_DIR "media"
#endif 

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
#define QCAMCORDER_MOVIES_DIR_USB  AEEFS_USB_DIR "media"
#endif


#ifndef FEATURE_BREW_3_0
#define QCAMCORDER_MOVIES_FLASH_MOUNT  "/"
#define QCAMCORDER_MOVIES_MMC_MOUNT    AEEFS_MMC_DIR
#ifdef FEATURE_EFS_FTL
#define QCAMCORDER_MOVIES_FTL_MOUNT    AEEFS_FTL_DIR
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
#define QCAMCORDER_MOVIES_USB_MOUNT    AEEFS_USB_DIR
#endif
#endif


const char *AEEDirList[] = 
{
  QCAMCORDER_MOVIES_DIR_FLASH,
  QCAMCORDER_MOVIES_DIR_MMC,
#ifdef FEATURE_EFS_FTL
  QCAMCORDER_MOVIES_DIR_FTL,
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  QCAMCORDER_MOVIES_DIR_USB,
#endif
};

const char *OEMDirList[] = 
{
  "",
  "mmc1/",
#ifdef FEATURE_EFS_FTL
  "ftl1/",
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  "usb1/"
#endif
};



AEESize QCam_Sizes[QCAMCORDER_SIZE_DISPLAY + 1] =
{  { 288, 352 },
   { 144, 176 },
   {  96, 128 },
   { 240, 320 },
   { 80,   96 }
};

// Array of uint16s used to create menus with multiple
// string options.
// First field is how many settings
// Second field is title
// rest are the settings
uint16 DeleteallIDList[] =
{
#if defined FEATURE_EFS_FTL && defined FEATURE_EFS_HOTPLUG_USB_HOST_MS
   6,
#elif defined FEATURE_EFS_FTL || defined FEATURE_EFS_HOTPLUG_USB_HOST_MS
   5,
#else
   4,
#endif 
   IDS_DELETEALL,
   IDS_QCAMCORDER_YES,// delete all
   IDS_FLASH_DELETEALL, // delete only FLASH
   IDS_MMC_DELETEALL, // delete only MMC
#ifdef FEATURE_EFS_FTL
   IDS_FTL_DELETEALL,   // delete only FTL
#endif

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
   IDS_USB_DELETEALL,   // delete only USB
#endif 
   IDS_QCAMCORDER_NO,
};

uint16 VideoFormatIDList[] =
{
#ifdef FEATURE_H264_ENCODER
   5,
#else
   4,
#endif /* FEATURE_H264_ENCODER */
   IDS_VIDEO_FORMAT,
   IDS_FORMAT_MPEG4,
   IDS_FORMAT_H263,
   IDS_FORMAT_H263_SAMPLE,
#ifdef FEATURE_H264_ENCODER
   IDS_FORMAT_H264,
#endif /* FEATURE_H264_ENCODER */
   IDS_NONE
};

uint16 ShortHeaderIDList[] =
{
   3,
   IDS_SHORT_HEADER,
   IDS_DISABLE,
   IDS_ENABLE,
   IDS_ENABLE_H263_SAMPLE
};

uint16 FileFormatIDList[] =
{
   7,
   IDS_FILE_FORMAT,
   IDS_FILE_MP4,
   IDS_FILE_AMC,
   IDS_FILE_3GP,
   IDS_FILE_3G2,
   IDS_FILE_K3G,
   IDS_FILE_3G2_FRAG,
   IDS_FILE_SKM
};

uint16 PostEditIDList[] =
{
   2,
   IDS_POST_EDIT,
   IDS_GPS_EDIT,
   IDS_TELOP_EDIT
};


uint16 SizeIDList[] =
{
   5,
   IDS_SIZE,
   IDS_SIZE_QVGA,
   IDS_SIZE_QCIF,
   IDS_SIZE_SQCIF,
   IDS_SIZE_MM,
   IDS_SIZE_DISPLAY
};

uint16 TranscoderSizeIDList[] =
{
   4,
   IDS_TRANSCODER_VIDEO_TYPE,
   IDS_SIZE_QVGA,
   IDS_SIZE_QCIF,
   IDS_SIZE_SQCIF,
   IDS_SIZE_MM,
};

uint16 NumClipsIDList[] =
{
   4,
   IDS_NUMCLIPS,
   IDS_NUMCLIPS_LOW,
   IDS_NUMCLIPS_MED,
   IDS_NUMCLIPS_HIGH,
   IDS_NO_LIMIT,
};

uint16 ClipLengthIDList[] =
{
   4,
   IDS_CLIPLENGTH,
   IDS_CLIPLENGTH_LOW,
   IDS_CLIPLENGTH_MED,
   IDS_CLIPLENGTH_HIGH,
   IDS_NO_LIMIT,
};

uint16 TranscoderBrandIDList[] =
{
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
   6,
#else
   5,
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
   IDS_TRANSCODE_TARGET_BRAND,
   IDS_FILE_AMC,
   IDS_FILE_3GP,
   IDS_FILE_MP4,
   IDS_FILE_3G2,
   IDS_FILE_K3G
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
   ,IDS_FILE_3G2_FRAG
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
};

uint16 TranscoderVideoCodecIDList[] =
{
#if defined(FEATURE_H264_ENCODER) && defined(FEATURE_H264_TRANSCODER)
   3,
#else
   2,
#endif /* FEATURE_H264_ENCODER */
   IDS_TRANSCODER_VIDEO_FORMAT,
   IDS_FORMAT_MPEG4,
   IDS_FORMAT_H263
#if defined(FEATURE_H264_ENCODER) && defined(FEATURE_H264_TRANSCODER)
   ,IDS_FORMAT_H264
#endif /* FEATURE_H264_ENCODER */
};

uint16 QualityIDList[] =
{
#ifdef FEATURE_VIDEO_ENCODE_BITRATE
   6,
#else
   5,
#endif
   IDS_QUALITY,
   IDS_QUALITY_LOW,
   IDS_QUALITY_MED,
   IDS_QUALITY_HIGH
#ifdef FEATURE_VIDEO_ENCODE_BITRATE
   ,IDS_QUALITY_USER
#endif
   ,IDS_QUALITY_MAX
   ,IDS_QUALITY_DYNAMIC
};

uint16 AudioFormatIDList[] =
{
#ifdef FEATURE_CAMCORDER_AAC_REC
   6,
#else /* FEATURE_CAMCORDER_AAC_REC */
   5,
#endif /* ! FEATURE_CAMCORDER_AAC_REC */
   IDS_AUDIO_FORMAT,
   IDS_AUDIO_FORMAT_13K_FULL,
   IDS_AUDIO_FORMAT_13K_HALF,
   IDS_AUDIO_FORMAT_EVRC,
#ifdef FEATURE_CAMCORDER_AAC_REC
   IDS_AUDIO_FORMAT_AAC,
#endif /* FEATURE_CAMCORDER_AAC_REC */
   IDS_AUDIO_FORMAT_AMR,
   IDS_NONE,
};

uint16 TranscodeAudioFormatIDList[] =
{

   5,
   IDS_TRANSCODER_TARGET_AUDIO,
   IDS_AUDIO_FORMAT_13K_FULL,
   IDS_AUDIO_FORMAT_13K_HALF,
   IDS_AUDIO_FORMAT_EVRC,
   IDS_AUDIO_FORMAT_AMR,
   IDS_NONE,
};

uint16 SaveLocIDList[] =
{
#if defined FEATURE_EFS_FTL && defined FEATURE_EFS_HOTPLUG_USB_HOST_MS
   4,
#elif defined FEATURE_EFS_FTL || defined FEATURE_EFS_HOTPLUG_USB_HOST_MS
   3,
#else
   2,
#endif  
   IDS_SAVE_LOC,
   IDS_FLASH,
   IDS_MMC,
#ifdef FEATURE_EFS_FTL
   IDS_FTL,
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
   IDS_USB
#endif

};

uint16 FrameRateIDList[] =
{
  3,
  IDS_FRAME_RATE,
  IDS_FRAME_RATE_1,
  IDS_FRAME_RATE_2,
  IDS_FRAME_RATE_3
};

uint16 WhiteBalanceIDList[] =
{
  4,
  IDS_WHITEBALANCE,
  IDS_AUTO,
  IDS_FLUORESCENT,
  IDS_INCANDESCENT,
  IDS_DAYLIGHT
};

uint16 TelopIDList[] =
{
   2,
   IDS_TELOP_TEXT,
   IDS_TELOP_ENABLED,
   IDS_TELOP_DISABLED
};

uint16 ContinuousIDList[] =
{
   2,
   IDS_CONTINUOUS,
   IDS_CONTINUOUS_ON,
   IDS_CONTINUOUS_OFF
};
#ifdef FEATURE_DVS
uint16 DVSMarginIDList[] =
{
  3,
  IDS_DVSMARGIN,
  IDS_DVSMARGIN_LOW,
  IDS_DVSMARGIN_MED,
  IDS_DVSMARGIN_HIGH
};
#endif
uint16 TimerIDList[] =
{
   4,
   IDS_TIMER,
   IDS_TIMER_OFF,
   IDS_TIMER_LOW,
   IDS_TIMER_MED,
   IDS_TIMER_HIGH

};

uint16 SpaceIDList[] =
{
   4,
   IDS_SPACE,
   IDS_SPACE_LOW,
   IDS_SPACE_MED,
   IDS_SPACE_HIGH,
   IDS_SPACE_DEFAULT

};

uint16 EffectsIDList[] =
{
   4,
   IDS_EFFECT,
   IDS_EFFECT_OFF,
   IDS_EFFECT_MONO,
   IDS_EFFECT_SEPIA,
   IDS_EFFECT_NEGATIVE

};

uint16 AntibandingIDList[] =
{
   3,
   IDS_ANTIBANDING,
   IDS_ANTIBANDING_OFF,
   IDS_ANTIBANDING_50HZ,
   IDS_ANTIBANDING_60HZ

};

uint16 RotateIDList[] =
{
   4,
   IDS_ROTATION,
   IDS_ROTATE_OFF,
   IDS_ROTATE_90,
   IDS_ROTATE_180,
   IDS_ROTATE_270
};

#ifdef FEATURE_CAMCORDER_AAC_REC
uint16 AACConfigIDList[] =
{
   3,
   IDS_AAC_CONFIG,
   IDS_AAC_CONFIG_MODE,
   IDS_AAC_CONFIG_FREQ,
   IDS_AAC_CONFIG_RATE
};

uint16 AACModeIDList[] =
{
  3,
  IDS_AAC_MODE,
  IDS_AAC_MONO2MONO,
  IDS_AAC_STEREO2MONO,
  IDS_AAC_STEREO2STEREO
};

uint16 AACFreqIDList[] =
{
  5,
  IDS_AAC_FREQ,
  IDS_AAC_FREQ_8,
  IDS_AAC_FREQ_11,
  IDS_AAC_FREQ_16,
  IDS_AAC_FREQ_22,
  IDS_AAC_FREQ_24
};

uint16 AACBitrateIDList[] =
{
  3,
  IDS_AAC_BITRATE,
  IDS_AAC_BITRATE_LOW,
  IDS_AAC_BITRATE_MED,
  IDS_AAC_BITRATE_HIGH,
};

#endif /* FEATURE_CAMCORDER_AAC_REC */

uint16 SpecialEffectsMenuIDList[] =
{
  ( 1  
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
    + 1 
#endif
#ifdef FEATURE_VIDEO_ENCODE_FADING
    + 2
#endif
#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
    + 3
#endif 
   ),
  IDS_SPECIAL_EFFECTS,
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
  IDS_ENABLE_FRAMEOVERLAY,
#endif /* FEATURE_VIDEO_ENCODE_OVERLAY_UI */
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
  IDS_TIME_STAMP,
#endif /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/  
#ifdef FEATURE_VIDEO_ENCODE_FADING
  IDS_FADE_IN,
  IDS_FADE_OUT,
#endif /* FEATURE_VIDEO_ENCODE_FADING */
#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
  IDS_ALPHA_FADE,
  IDS_CURTAIN_FADE,
  IDS_NONE,
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */
};

static const uint8 UUID_CPGD [] = {
        0x63, 0x70, 0x67, 0x64,
        0xa8, 0x8c, 0x11, 0xd4,
        0x81, 0x97, 0x00, 0x90,
        0x27, 0x08, 0x77, 0x03
 };
#ifdef FEATURE_QCAMCORDER_TELOP
static const uint8 UUID_TSML [] = {
        0x74, 0x73, 0x6d, 0x6c,
        0xa8, 0x8c, 0x11, 0xd4,
        0x81, 0x97, 0x00, 0x90,
        0x27, 0x08, 0x77, 0x03
 };
#endif
static const uint8 UUID_GPSI [] = {
        0x67, 0x70, 0x73, 0x69, /* 'gpsi' */
        0xa8, 0x8c, 0x11, 0xd4,
        0x81, 0x97, 0x00, 0x90,
        0x27, 0x08, 0x77, 0x03
 };

 struct {
        unsigned char version;
        unsigned char flags [3];
        unsigned long copy_guard_attribute;
        unsigned long limit_date;
        unsigned long limit_period;
        unsigned long limit_count;
    } cpgd = {
        0, {0, 0, 0}, 0, 0, 0, 0
    };

#ifdef FEATURE_MMOVERLAY
typedef struct
{
  AEERect rc;             /* overlay area rectangle */
  IBitmap *pBmp;          /* bitmap to be overlayed */
  RGBVAL  transparent;    /* transparency color */
  float   alpha;          /* alpha blend value */
  uint32 layer; /* top/base overlay layer */
} OverlayLayerData;
#endif /* FEATURE_MMOVERLAY */


#ifdef FEATURE_QCAMCORDER_TELOP
typedef struct {
        uint8 version;
        unsigned char flags ;
        char data[227];
        //        char data[177];
} tsml_struct;

enum { TELOP_OFF,
       TELOP_ON
}telop_state;
#endif

typedef  struct {

        uint32 uuid_versionflags;
        uint32 uuid_size;

        uint32 type;
        uint32 versionflags;
        uint8  byte_order[2];
        uint16 gps_version_num_if_id;

        uint16 gps_version_tag_id;
        uint8  gps_version_type[2];
        uint8  gps_version_count[4];
        uint8  gps_version_value[4];

        uint8 gps_latitude_ref_tagID[2];
        uint8 gps_latitude_ref_type[2];
        uint8 gps_latitude_ref_count[4];
        uint8 gps_latitude_ref_value[4];

        uint8 gps_latitude_tagID[2];
        uint8 gps_latitude_type[2];
        uint8 gps_latitude_count[4];
        uint8 gps_latitude_value[4];

        uint8 gps_longitude_ref_tagID[2];
        uint8 gps_longitude_ref_type[2];
        uint8 gps_longitude_ref_count[4];
        uint8 gps_longitude_ref_value[4];

        uint8 gps_longitude_tagID[2];
        uint8 gps_longitude_type[2];
        uint8 gps_longitude_count[4];
        uint8 gps_longitude_value[4];

        uint8 gps_altitude_ref_tagID[2];
        uint8 gps_altitude_ref_type[2];
        uint8 gps_altitude_ref_count[4];
        uint8 gps_altitude_ref_value[4];

        uint8 gps_altitude_tagID[2];
        uint8 gps_altitude_type[2];
        uint8 gps_altitude_count[4];
        uint8 gps_altitude_value[4];

        uint8 gps_timestamp_tagID[2];
        uint8 gps_timestamp_type[2];
        uint8 gps_timestamp_count[4];
        uint8 gps_timestamp_value[4];

        uint8 gps_mapDatum_tagID[2];
        uint8 gps_mapDatum_type[2];
        uint8 gps_mapDatum_count[4];
        uint8 gps_mapDatum_value[4];

        uint8 gps_processing_tagID[2];
        uint8 gps_processing_type[2];
        uint8 gps_processing_count[4];
        uint8 gps_processing_value[4];

        uint8 gps_area_tagID[2];
        uint8 gps_area_type[2];
        uint8 gps_area_count[4];
        uint8 gps_area_value[4];

        uint8 gps_dateStamp_tagID[2];
        uint8 gps_dateStamp_type[2];
        uint8 gps_dateStamp_count[4];
        uint8 gps_dateStamp_value[4];

        uint32 nextIFDOffset;

        uint8 latitude[24];
        uint8 longitude[24];
        uint8 altitude[8];
        uint8 geodetic_survey_data_method[22];

        //uint8 method[15];

} gpsi_struct;

/* These definition will move to AEECamera.h later. */
#ifndef CAM_FILE_FORMAT_3G2_FRAG
#define CAM_FILE_FORMAT_3G2_FRAG  "3g2f"
#endif

#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
#define ENABLE_FRAMEOVERLAY_STRING "Enable Overlay"
#define DISABLE_FRAMEOVERLAY_STRING "No Frame"
#endif
/* String for special effects menu */
#define EFX_MENU_NAME_LEN   15
#define TIME_STAMP_STRING   "Time Stamp  "

/* Including null char, EFX_MENU_NAME_LEN = maxlength(TIME_STAMP_STRING,
                                                      ENABLE_FRAMEOVERLAY_STRING,
                                                      ALPHA_FADE_STRING,
                                                      CURTAIN_FADE_STRING)+1 */

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
#define ALPHA_FADE_STRING   "Alpha Fade  "
#define CURTAIN_FADE_STRING "Curtain Fade"
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */

/* String for display size menu */
#define QVGA_PREVIEW_STRING "QVGA Preview"
#define QVGA_PREVIEW_LEN 12

#define QVGA_15FPS_LOW_PROFILE_RATE  96
#define QVGA_15FPS_MED_PROFILE_RATE  112
#define QVGA_15FPS_HI_PROFILE_RATE   128
#define QCIF_15FPS_LOW_PROFILE_RATE 48
#define QCIF_15FPS_MED_PROFILE_RATE 64
#define QCIF_15FPS_HI_PROFILE_RATE 80
#define SQCIF_15FPS_LOW_PROFILE_RATE 32
#define SQCIF_15FPS_MED_PROFILE_RATE 48
#define SQCIF_15FPS_HI_PROFILE_RATE 64
#define MM_15FPS_LOW_PROFILE_RATE 24
#define MM_15FPS_MED_PROFILE_RATE 32
#define MM_15FPS_HI_PROFILE_RATE 48

#define QVGA_10FPS_LOW_PROFILE_RATE  80
#define QVGA_10FPS_MED_PROFILE_RATE  96
#define QVGA_10FPS_HI_PROFILE_RATE   112
#define QCIF_10FPS_LOW_PROFILE_RATE 36
#define QCIF_10FPS_MED_PROFILE_RATE 48
#define QCIF_10FPS_HI_PROFILE_RATE 64
#define SQCIF_10FPS_LOW_PROFILE_RATE 24
#define SQCIF_10FPS_MED_PROFILE_RATE 32
#define SQCIF_10FPS_HI_PROFILE_RATE 48
#define MM_10FPS_LOW_PROFILE_RATE 24
#define MM_10FPS_MED_PROFILE_RATE 32
#define MM_10FPS_HI_PROFILE_RATE 48

#define QVGA_7FPS_LOW_PROFILE_RATE  64
#define QVGA_7FPS_MED_PROFILE_RATE  80
#define QVGA_7FPS_HI_PROFILE_RATE   96
#define QCIF_7FPS_LOW_PROFILE_RATE 24
#define QCIF_7FPS_MED_PROFILE_RATE 32
#define QCIF_7FPS_HI_PROFILE_RATE 48
#define SQCIF_7FPS_LOW_PROFILE_RATE 24
#define SQCIF_7FPS_MED_PROFILE_RATE 28
#define SQCIF_7FPS_HI_PROFILE_RATE 32
#define MM_7FPS_LOW_PROFILE_RATE 16
#define MM_7FPS_MED_PROFILE_RATE 24
#define MM_7FPS_HI_PROFILE_RATE 32

#define AUDIO_13K 13
#define AUDIO_13K_HALF 6.5
#define AUDIO_AMR 12.2
#define AUDIO_EVRC 8
#define AUDIO_NONE 0

#define QCAMCORDER_PROGBAR_DX_DIV     3     // dx is dx/3 of main rectangle
#define QCAMCORDER_PROGBAR_DY_DIV     2     // dy is dy/2 of main rectangle
#define QCAMCORDER_PROGBAR_DX_OFFSET  4     // dx is reduced by offset

//if <100k space is left, user can't record
#define QCAMCORDER_SPACE_THRESHOLD 100
#define MOVIEMAIL_DISPLAY_X 32
#define MOVIEMAIL_DISPLAY_Y 42
#define DISPLAY_COLOR_DEPTH 16

#define DEFAULT_LATITUDE 6897300
#define DEFAULT_LONGITUDE -21810381
#define DEFAULT_ALTITUDE 645

#define QCP_FIXED_HEADER_SIZE 162
#define AUTO_FOCUS_SELECTED 0
#define MANUAL_FOCUS_SELECTED 1

#define HEIGHT_QVGA 320
/*---------------------------------------------------------------------------
** QCP File Headers - these arrays contain the data to store in the decoder
** specific information descriptor for an audio track containing QCELP-13K
** vocoded packets. We provide this because IMedia_Record for QCELP audio
** only produces header with variable rate even though we only support
** fixed rate QCELP audio.
**---------------------------------------------------------------------------
*/
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
#ifndef FEATURE_BREW_IMOVIEEDIT
static const uint8 QCP_FILE_HEADER_13K_FIXED_FULL [QCP_FIXED_HEADER_SIZE] = {
    0x51, 0x4C, 0x43, 0x4D, /* 'QLCM' */
    0x66, 0x6D, 0x74, 0x20, /* 'fmt ' */
    0x96, 0x00, 0x00, 0x00, /* size of 'fmt ' subchunk excluding id,  size */
    0x01,                   /* major version (1) */
    0x00,                   /* minor version (0) */
    0x41, 0x6D, 0x7F, 0x5E, 0x15, 0xB1, 0xD0, 0x11,
    0xBA, 0x91, 0x00, 0x80, 0x5F, 0xB4, 0xB9, 0x7E, /* codec ID (QCELP-13K) */
    0x02, 0x00,             /* version number of the codec (2) */
    0x51, 0x63, 0x65, 0x6C, 0x70, 0x20, 0x31, 0x33,
    0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* codec name ('Qcelp  */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*   13K')             */
    0xB0, 0x36,             /* avg bps of the codec (14000 bps) */
    0x23, 0x00,             /* bytes per packet (35) */
    0xA0, 0x00,             /* samples per block (160) */
    0x40, 0x1F,             /* samples per second (8000) */
    0x10, 0x00,             /* bits per sample (16) */
    0x00, 0x00, 0x00, 0x00, /* number of rates (0 = fixed rate) */
    0x22,                   /* size of packet (full rate: 34) */
    0x04,                   /* rate index of packet (full rate: 4) */
    0x10,                   /* size of packet (half rate: 16) */
    0x03,                   /* rate index of packet (half rate: 3) */
    0x07,                   /* size of packet (quarter rate: 7) */
    0x02,                   /* rate index of packet (quarter rate: 2) */
    0x03,                   /* size of packet (eighth rate: 3) */
    0x01,                   /* rate index of packet (eighth rate: 1) */
    0x00,                   /* size of packet (unused: 0) */
    0x00,                   /* rate index of packet (unused: 0) */
    0x00,                   /* size of packet (unused: 0) */
    0x00,                   /* rate index of packet (unused: 0) */
    0x00,                   /* size of packet (unused: 0) */
    0x00,                   /* rate index of packet (unused: 0) */
    0x00,                   /* size of packet (unused: 0) */
    0x00,                   /* rate index of packet (unused: 0) */
    0x00, 0x00, 0x00, 0x00, /* reserved */
    0x00, 0x00, 0x00, 0x00, /* reserved */
    0x00, 0x00, 0x00, 0x00, /* reserved */
    0x00, 0x00, 0x00, 0x00, /* reserved */
    0x00, 0x00, 0x00, 0x00  /* reserved */
};

static const uint8 QCP_FILE_HEADER_13K_FIXED_HALF [QCP_FIXED_HEADER_SIZE] = {
    0x51, 0x4C, 0x43, 0x4D, /* 'QLCM' */
    0x66, 0x6D, 0x74, 0x20, /* 'fmt ' */
    0x96, 0x00, 0x00, 0x00, /* size of 'fmt ' subchunk excluding id,  size */
    0x01,                   /* major version (1) */
    0x00,                   /* minor version (0) */
    0x41, 0x6D, 0x7F, 0x5E, 0x15, 0xB1, 0xD0, 0x11,
    0xBA, 0x91, 0x00, 0x80, 0x5F, 0xB4, 0xB9, 0x7E, /* codec ID (QCELP-13K) */
    0x02, 0x00,             /* version number of the codec (2) */
    0x51, 0x63, 0x65, 0x6C, 0x70, 0x20, 0x31, 0x33,
    0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* codec name ('Qcelp  */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*   13K')             */
    0x90, 0x1A,             /* avg bps of the codec (6800 bps) */
    0x11, 0x00,             /* bytes per packet (17) */
    0xA0, 0x00,             /* samples per block (160) */
    0x40, 0x1F,             /* samples per second (8000) */
    0x10, 0x00,             /* bits per sample (16) */
    0x00, 0x00, 0x00, 0x00, /* number of rates (0 = fixed rate) */
    0x22,                   /* size of packet (full rate: 34) */
    0x04,                   /* rate index of packet (full rate: 4) */
    0x10,                   /* size of packet (half rate: 16) */
    0x03,                   /* rate index of packet (half rate: 3) */
    0x07,                   /* size of packet (quarter rate: 7) */
    0x02,                   /* rate index of packet (quarter rate: 2) */
    0x03,                   /* size of packet (eighth rate: 3) */
    0x01,                   /* rate index of packet (eighth rate: 1) */
    0x00,                   /* size of packet (unused: 0) */
    0x00,                   /* rate index of packet (unused: 0) */
    0x00,                   /* size of packet (unused: 0) */
    0x00,                   /* rate index of packet (unused: 0) */
    0x00,                   /* size of packet (unused: 0) */
    0x00,                   /* rate index of packet (unused: 0) */
    0x00,                   /* size of packet (unused: 0) */
    0x00,                   /* rate index of packet (unused: 0) */
    0x00, 0x00, 0x00, 0x00, /* reserved */
    0x00, 0x00, 0x00, 0x00, /* reserved */
    0x00, 0x00, 0x00, 0x00, /* reserved */
    0x00, 0x00, 0x00, 0x00, /* reserved */
    0x00, 0x00, 0x00, 0x00  /* reserved */
};
#endif /* FEATURE_BREW_IMOVIEEDIT */
#endif /* FEATURE_VIDEO_AUDIO_DUBBING*/
/* End of stream footer to append to last frame in MPEG4 elementary stream */
/* This is the visual_object_sequence_end_code in table 6-3, p31 of the    */
/* the standard ISO/IEC 14496-2                                            */
#if defined FEATURE_QCAMCORDER_CUT_N_MERGE || defined FEATURE_QCAMCORDER_VIDEO_EDITING
#ifndef FEATURE_BREW_IMOVIEEDIT
static uint8 QCAMCORDER_M4VFOOTER[] = {0x00, 0x00, 0x01, 0xB1};
#endif /*FEATURE_BREW_MOVIEEDIT*/
#endif /* FEATURE_QCAMCORDER_CUT_N_MERGE || FEATURE_QCAMCORDER_VIDEO_EDITING*/

/* For frame display during video playback */
static struct
{
  MDPIMG  image;                      /* MDP structures for display regions */
  uint32  id;                         /* MDP Image IDs */
  AEEMediaMPEG4RotationType rotation; /* rotation setting */
//  AEEMediaMPEG4ScalingType scaling;   /* scaling setting */

} VideoPlayback;

/*=========================================
static application
=====================================*/
#if defined(BREW_STATIC_APP)
int   Qcamcorder_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
int  Qcamcorder_Load(IShell *ps, void * pHelpers, IModule ** pMod);

// Constant Data...
static const AEEAppInfo    gaiQcamcorder = {AEECLSID_QCAMCORDER,QCAMCORDER_RES_FILE, IDS_TITLE, 0,0,0,0,AFLAG_TOOL};


PFNMODENTRY Qcamcorder_GetModInfo(IShell * ps, AEECLSID ** ppClasses, AEEAppInfo ** pApps, uint16 * pnApps,uint16 * pwMinPriv)
{
   *pApps = (AEEAppInfo *)&gaiQcamcorder;
   *pnApps = 1;
   return((PFNMODENTRY)Qcamcorder_Load);
}

int Qcamcorder_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,Qcamcorder_CreateInstance,NULL));
}

#define PACK_RGB565(r, g, b) (unsigned short)(((r&0xF8)<<8)+((g&0xFC)<<3)+((b&0xF8)>>3))
#endif  //BREW_STATIC_APP

/* ==========================================================================
FUNCTION Qcamcorder_TelephoneListener
DESCRIPTION
   Handles the Itelephone event used to check whether the call is present or not
============================================================================= */
static void Qcamcorder_TelephoneListener (Qcamcorder* pMe, ModelEvent* pEvent)
{
   boolean bCallEnded = FALSE;
   AEETCallEvent * pCallEvent = (AEETCallEvent*) pEvent;
   AEETCallEventData * pTCallEventData = (AEETCallEventData *)&(pCallEvent->call);
     
   if(pEvent->evCode == AEET_EVENT_CALL_END)
   {
     if(pTCallEventData->call_info.call_type == AEET_CALL_TYPE_NONE)
     {
         bCallEnded = TRUE;
     }
     else if(pTCallEventData->call_info.call_type == AEET_CALL_TYPE_VOICE)
     {
        if( pTCallEventData->call_info.call_state == AEET_CALL_STATE_NONE ||
            pTCallEventData->call_info.call_state == AEET_CALL_STATE_IDLE  ||
            pTCallEventData->call_info.call_state == AEET_CALL_STATE_ENDED||
            pTCallEventData->end_status == AEET_CALL_END_INCOM_REJ)
        {
          bCallEnded = TRUE;
        } 
     }
     if(bCallEnded)
     {
       IMENUCTL_SetActive(pMe->m_pIPreview, TRUE);
       (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
     } 
     else
     {
       QCamcorder_MdpRegisterFullScreen(pMe, TRUE);
       Qcamcorder_DrawIcon(pMe, IDB_NOT_AVAILABLE);
     }
   }
}

/* ==========================================================================
FUNCTION Qcamcorder_CheckVoiceRejected
DESCRIPTION
   Query Itelephone interface and add a listener to wait for CALL end event
============================================================================= */ 
static void Qcamcorder_HandleVoiceCallRejected( Qcamcorder *pMe)
{ 
   int32 nError; 
   IModel* pModel = NULL; 
   if (SUCCESS == ITELEPHONE_QueryInterface(pMe->pIPhone, AEEIID_MODEL, (void**) &pModel)) 
   { 
      nError = IMODEL_AddListenerEx(pModel, &pMe->phoneListener, 
                                      (PFNLISTENER)Qcamcorder_TelephoneListener, pMe); 
      IMODEL_Release(pModel); 
      if (SUCCESS != nError) 
      { 
        QCamcorder_MdpRegisterFullScreen(pMe, TRUE);
        Qcamcorder_DrawIcon(pMe, IDB_NOT_AVAILABLE);
      } 
   } 
   else
   {
     QCamcorder_MdpRegisterFullScreen(pMe, TRUE);
     Qcamcorder_DrawIcon(pMe, IDB_NOT_AVAILABLE);
   }
}

/* ==========================================================================
FUNCTION Qcamcorder_CheckVoiceRejected
DESCRIPTION
   True when voice call present, else false
============================================================================= */ 
static boolean Qcamcorder_CheckVoiceCallState( Qcamcorder *pMe)
{
  AEETCalls*           pCallsDesc = NULL;
  uint8                uSizeOfCallsDesc = 0;
  AEETCallInfo         callInfo;
  uint8                index = 0;
  boolean              bCallState = FALSE;

  uSizeOfCallsDesc = sizeof(AEETCalls) + 
                     (sizeof(AEECallDesc)*(QCAMCORDER_MAX_NUM_CALLS - 1));
  pCallsDesc = (AEETCalls*)MALLOC( uSizeOfCallsDesc );
  if ( pCallsDesc == NULL )
  {
    MSG_ERROR( "IS_VOICECALL_INCOMING - Malloc failed", 0, 0, 0 );
    return FALSE;  
  }

  if ( ITELEPHONE_GetCalls( pMe->pIPhone, pCallsDesc, 
                            uSizeOfCallsDesc ) == SUCCESS )
  {
    /* process information for each call */
    for ( index = 0; index < pCallsDesc->dwCount; index++ )
    {
      /* get call info */
      if ( ITELEPHONE_GetCallInfo( pMe->pIPhone, pCallsDesc->calls[index],
                                       &callInfo, sizeof(callInfo) ) != SUCCESS )
      {
        MSG_ERROR( "IS_VOICECALL_INCOMING - Can't obtain callinfo callID:%d",
                   pCallsDesc->calls[index], 0, 0 );
        bCallState = FALSE;
        break;
      }
  
      if ( callInfo.call_type == AEET_CALL_TYPE_VOICE )
      {
        pMe->bVoiceCallRejected = TRUE;
        bCallState = TRUE;
        break;
      }      
    }    
  }
  FREEIF( pCallsDesc );
  return bCallState;
}


/* ==========================================================================
FUNCTION QCAMCORDER_GETPICTURESIZE
DESCRIPTION
   returns the approximate size in kilo bytes of an incoded picture
============================================================================= */
int Qcamcorder_GetMovieSize(Qcamcorder *pMe)
{
  uint32  pictureSize =0;
  uint32  audioSize=0;
  switch(pMe->eSize)
  {
    case QCAMCORDER_SIZE_QVGA:
      switch(pMe->wQuality)
      {
        case QCAMCORDER_QUALITY_HIGH:
          switch (pMe->m_FrameRate)
          {
            case QCAMCORDER_FRAME_RATE_1:
              pictureSize =  QVGA_7FPS_HI_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_2:
              pictureSize = QVGA_15FPS_HI_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_3:
               pictureSize = QVGA_10FPS_HI_PROFILE_RATE;
               break;
            default:
               pictureSize = QVGA_7FPS_HI_PROFILE_RATE;
               break;
          }
          break;

        case QCAMCORDER_QUALITY_MED:
          switch (pMe->m_FrameRate)
          {
            case QCAMCORDER_FRAME_RATE_1:
              pictureSize = QVGA_7FPS_MED_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_2:
              pictureSize = QVGA_15FPS_MED_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_3:
              pictureSize = QVGA_10FPS_MED_PROFILE_RATE;
              break;
            default:
              pictureSize = QVGA_7FPS_MED_PROFILE_RATE;
              break;
          }
          break;

        case QCAMCORDER_QUALITY_LOW:
           switch (pMe->m_FrameRate)
           {
             case QCAMCORDER_FRAME_RATE_1:
               pictureSize = QVGA_7FPS_LOW_PROFILE_RATE;
               break;
             case QCAMCORDER_FRAME_RATE_2:
               pictureSize = QVGA_15FPS_LOW_PROFILE_RATE;
               break;
             case QCAMCORDER_FRAME_RATE_3:
               pictureSize = QVGA_10FPS_LOW_PROFILE_RATE;
               break;
             default:
               pictureSize = QVGA_7FPS_LOW_PROFILE_RATE;
               break;
           }
           break;
         default:
           pictureSize = QVGA_7FPS_LOW_PROFILE_RATE;
           break;
      }
      break;

    case QCAMCORDER_SIZE_QCIF:
      switch(pMe->wQuality)
      {
        case QCAMCORDER_QUALITY_HIGH:
          switch (pMe->m_FrameRate)
          {
            case QCAMCORDER_FRAME_RATE_1:
              pictureSize =  QCIF_7FPS_HI_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_2:
              pictureSize = QCIF_15FPS_HI_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_3:
              pictureSize = QCIF_10FPS_HI_PROFILE_RATE;
              break;
            default:
              pictureSize = QCIF_7FPS_HI_PROFILE_RATE;
              break;
          }
          break;

        case QCAMCORDER_QUALITY_MED:
          switch (pMe->m_FrameRate)
          {
            case QCAMCORDER_FRAME_RATE_1:
              pictureSize = QCIF_7FPS_MED_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_2:
              pictureSize = QCIF_15FPS_MED_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_3:
              pictureSize = QCIF_10FPS_MED_PROFILE_RATE;
              break;
            default:
              pictureSize = QCIF_7FPS_MED_PROFILE_RATE;
              break;
           }
           break;

        case QCAMCORDER_QUALITY_LOW:
          switch (pMe->m_FrameRate)
          {
            case QCAMCORDER_FRAME_RATE_1:
              pictureSize = QCIF_7FPS_LOW_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_2:
              pictureSize = QCIF_15FPS_LOW_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_3:
              pictureSize = QCIF_10FPS_LOW_PROFILE_RATE;
              break;
            default:
              pictureSize = QCIF_7FPS_LOW_PROFILE_RATE;
              break;
           }
           break;

        default:
          pictureSize = QCIF_7FPS_LOW_PROFILE_RATE;
          break;
      }
      break;

    case QCAMCORDER_SIZE_SQCIF:
      switch(pMe->wQuality)
      {
        case QCAMCORDER_QUALITY_HIGH:
          switch (pMe->m_FrameRate)
          {
            case QCAMCORDER_FRAME_RATE_1:
              pictureSize = SQCIF_7FPS_HI_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_2:
              pictureSize = SQCIF_15FPS_HI_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_3:
              pictureSize = SQCIF_10FPS_HI_PROFILE_RATE;
              break;
            default:
              pictureSize = SQCIF_7FPS_HI_PROFILE_RATE;
              break;
          }
          break;

        case QCAMCORDER_QUALITY_MED:
          switch (pMe->m_FrameRate)
          {
            case QCAMCORDER_FRAME_RATE_1:
              pictureSize = SQCIF_7FPS_MED_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_2:
              pictureSize =  SQCIF_15FPS_MED_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_3:
              pictureSize =  SQCIF_10FPS_MED_PROFILE_RATE;
              break;
            default:
              pictureSize = SQCIF_7FPS_MED_PROFILE_RATE;
              break;
           }
           break;

        case QCAMCORDER_QUALITY_LOW:
          switch (pMe->m_FrameRate)
          {
            case QCAMCORDER_FRAME_RATE_1:
              pictureSize =  SQCIF_7FPS_LOW_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_2:
              pictureSize = SQCIF_15FPS_LOW_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_3:
              pictureSize = SQCIF_10FPS_LOW_PROFILE_RATE;
              break;
            default:
              pictureSize =  SQCIF_7FPS_LOW_PROFILE_RATE;
              break;
           }
           break;

         default:
           pictureSize =  SQCIF_7FPS_LOW_PROFILE_RATE;
           break;
      }
      break;

    case QCAMCORDER_SIZE_MM:
      switch(pMe->wQuality)
      {
        case QCAMCORDER_QUALITY_HIGH:
          switch (pMe->m_FrameRate)
          {
            case QCAMCORDER_FRAME_RATE_1:
              pictureSize = MM_7FPS_HI_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_2:
              pictureSize = MM_15FPS_HI_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_3:
              pictureSize = MM_10FPS_HI_PROFILE_RATE;
              break;
            default:
              pictureSize = MM_7FPS_HI_PROFILE_RATE;
              break;
          }
          break;

        case QCAMCORDER_QUALITY_MED:
          switch (pMe->m_FrameRate)
          {
            case QCAMCORDER_FRAME_RATE_1:
              pictureSize = MM_7FPS_MED_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_2:
              pictureSize = MM_15FPS_MED_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_3:
              pictureSize = MM_10FPS_MED_PROFILE_RATE;
              break;
            default:
              pictureSize = MM_7FPS_MED_PROFILE_RATE;
              break;
          }
          break;

        case QCAMCORDER_QUALITY_LOW:
          switch (pMe->m_FrameRate)
          {
            case QCAMCORDER_FRAME_RATE_1:
              pictureSize = MM_7FPS_LOW_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_2:
              pictureSize = MM_15FPS_LOW_PROFILE_RATE;
              break;
            case QCAMCORDER_FRAME_RATE_3:
              pictureSize = MM_10FPS_LOW_PROFILE_RATE;
              break;
            default:
              pictureSize = MM_7FPS_LOW_PROFILE_RATE;
              break;
          }
          break;

        default:
          pictureSize = MM_7FPS_LOW_PROFILE_RATE;
          break;
      }
      break;

    default:
      QCAMCORDER_MSG_ERROR("Qcamcorder_GetMovieSize() invalid picture size", 0, 0, 0);
      break;
   }

   switch (pMe->wAudioFormat)
   {
     case QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K:
       audioSize = AUDIO_13K;
       break;
     case QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K:
       audioSize = (uint32)AUDIO_13K_HALF;
       break;
     case QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_EVRC:
       audioSize = AUDIO_EVRC;
       break;
     case QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR:
       audioSize = (uint32)AUDIO_AMR;
       break;
     case QCAMCORDER_AUDIO_FORMAT_NONE:
       audioSize = AUDIO_NONE;
       break;
     default:
       audioSize = AUDIO_NONE;
       break;
   }
   //return in bytes
   return ( (int) ((audioSize +  pictureSize)/8) );

} /* END Qcamcorder_GetPictureSize */


/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
   This function is invoked while the app is being loaded. All Modules must provide this
   function. Ensure to retain the same name and parameters for this function.
   In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
   that has been provided in AEEAppGen.c.

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to IDisplay_InitAppData().
   This is done as follows: InitAppData() is called to initialize AppletData
   instance. It is app developers responsibility to fill-in app data initialization
   code of InitAppData(). App developer is also responsible to release memory
   allocated for data contained in AppletData -- this can be done in
   IDisplay_FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
   clsID: [in]: Specifies the ClassID of the applet which is being loaded

   pIShell: [in]: Contains pointer to the IShell object.

   pIModule: pin]: Contains pointer to the IModule object to the current module to which
   this app belongs

   ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
   of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(BREW_STATIC_APP)
int   Qcamcorder_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#endif
{
   *ppObj = NULL;

   if(ClsId == AEECLSID_QCAMCORDER){
      if(AEEApplet_New(sizeof(Qcamcorder), ClsId, pIShell,po,(IApplet**)ppObj,
         (AEEHANDLER)Qcamcorder_HandleEvent,(PFNFREEAPPDATA)Qcamcorder_FreeAppData))

      {


        if(Qcamcorder_InitData((IApplet*) *ppObj) == TRUE)
           return (AEE_SUCCESS);
      }
   }
   return (EFAILED);
}

/*===========================================================================
    FUNCTION: QCamcorderTimer

    DESCRIPTION:
     This function keeps firing off a timer every second to keep BREW busy
     and make the phone not go to sleep
===========================================================================*/
void QCamcorderTimer (Qcamcorder* pMe)
{
   (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY)QCamcorderTimer,(void *)pMe);
}

/*===========================================================================

  FUNCTION: Qcamcorder_InitData

   DESCRIPTION:
    This function initializes app specific data, allocates memory for app
    data. App developer needs to fill-in the initialization code.

  PROTOTYPE:
    boolean Qcamcorder_InitAppData(IApplet* pi);

  PARAMETERS:
      pi [in]: Pointer to the IApplet structure. This structure contains
      information specific to this applet. It was initialized during the
      AEEClsCreateInstance() function.

  DEPENDENCIES:
     Assumes pMe is not NULL

  RETURN VALUE
   TRUE: If the app has app data is allocated and initialized successfully
   FALSE: Either app data could not be allocated or initialzied

  SIDE EFFECTS:
  none
===========================================================================*/
static boolean Qcamcorder_InitData (IApplet * pi)
{
  Qcamcorder *pMe = (Qcamcorder *) pi;
  boolean status = TRUE;
  AEERect rc;
  AEERect rectProg;
  uint32 ProgBar_x, ProgBar_y;

  (void)ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pMe->pIFileMgr);
  if (pMe->pIFileMgr)
  {
    if (IFILEMGR_Test(pMe->pIFileMgr, Qcamcorder_GetDirectory(pMe)) != SUCCESS)
    {
      if(IFILEMGR_MkDir(pMe->pIFileMgr, Qcamcorder_GetDirectory(pMe)) != SUCCESS)
        {
        status = FALSE;
        }
    }
  }
  else
  {
    status = FALSE;
  } 
  pMe->bVoiceCallRejected = FALSE;
  pMe->m_maxNumClips   = QCAMCORDER_NUMCLIPS_NONE;
  pMe->m_maxClipLength = QCAMCORDER_CLIPLENGTH_NONE;
  pMe->wQuality        = QCAMCORDER_QUALITY_HIGH;
  pMe->saveLoc         = QCAMCORDER_SAVE_FLASH;
  pMe->m_FrameRate     = QCAMCORDER_FRAME_RATE_1;
  pMe->m_nwhiteBalance = CAM_WB_AUTO;
  pMe->wFileFormat     = QCAMCORDER_FILE_3G2;
  pMe->wVideoFormat    = QCAMCORDER_VIDEO_FORMAT_MPEG4;
  pMe->wShortHeader    = QCAMCORDER_SH_DISABLE;
  pMe->m_nTimer        = QCAMCORDER_TIMER_OFF;
  pMe->m_nEffect       = QCAMCORDER_EFFECTS_OFF;
  pMe->m_nAntibanding  = QCAMCORDER_ANTIBANDING_OFF;
  pMe->m_bRotate       = QCAMCORDER_ROTATE_OFF;
  pMe->wSpaceLimit     = CAM_SPACELIMIT_DEFAULT;
  ISHELL_GetDeviceInfo (pMe->a.m_pIShell, &pMe->di);

#ifdef FEATURE_VIDEO_ENCODE_BITRATE
  pMe->wBitRate        = 128000; /* Assume user bitrate of 128kbps */
#endif

  pMe->wAudioFormat    = QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K;
  /*Set the class Id to QCP as Default*/
  pMe->wAudioFormatClsId = AEECLSID_MEDIAQCP;

#ifdef FEATURE_MP4_TRANSCODER

  pMe->wTranscodeAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K;
  pMe->eTranscoderBrand      = QCAMCORDER_FILE_XCODER_AMC;
  pMe->eTranscoderSize       = QCAMCORDER_SIZE_MM;
  pMe->bTranscode_portion    = FALSE;
  pMe->eTranscoderVideoCodec = QCAMCORDER_VIDEO_FORMAT_MPEG4;
  pMe->wTranscoderShortHeader = QCAMCORDER_SH_DISABLE;
  pMe->nStartFrameNo         = 0;
  pMe->nEndFrameNo           = 0;
#endif
#ifdef FEATURE_DVS
  pMe->bDVS                  = FALSE;
  pMe->eDVSMargin            = QCAMCORDER_DVSMARGIN_MED;
#endif
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
  pMe->bUpdatePreviewFrame   = TRUE;
  pMe->bEnableFrameOverlay = FALSE;
#endif

#ifdef FEATURE_CAMCORDER_AAC_REC
  pMe->m_AACConfig           = QCAMCORDER_AAC_CONFIG_MODE;
  pMe->m_AACInfo             = 0;
#endif /* FEATURE_CAMCORDER_AAC_REC */
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  MEMSET(&(pMe->ExtendedInfo),0,sizeof(AEEMediaMPEG4SpecEx));
  MEMSET(pMe->TelopTextInfo,0,sizeof(char)*USER_TELOP_MAX_LENGTH);

  Qcamcorder_GetConfigRecord(pMe);

  (void)STRLCPY(pMe->sStartSoundFile, "No Sound", sizeof(pMe->sStartSoundFile));

#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
   if (STRCMP(pMe->sFrameOverlay,DISABLE_FRAMEOVERLAY_STRING))
   {
      pMe->bEnableFrameOverlay = TRUE;
   }
#endif

#ifdef FEATURE_VIDEO_ENCODE_FADING
  pMe->mFadeInOut = QCAMCORDER_FADE_OFF;
#endif

  pMe->pFileFormat = (char *) MALLOC(QCAMCORDER_MAX_EXTENSION_SIZE);

  switch(pMe->wFileFormat)
  {
   case QCAMCORDER_FILE_MP4:
     (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_MPG4, QCAMCORDER_MAX_EXTENSION_SIZE);
     break;
   case QCAMCORDER_FILE_AMC:
     (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_AMC, QCAMCORDER_MAX_EXTENSION_SIZE);
     break;
   case QCAMCORDER_FILE_3GP:
     (void)STRLCPY(pMe->pFileFormat,CAM_FILE_FORMAT_3GP, QCAMCORDER_MAX_EXTENSION_SIZE);
     break;
   case QCAMCORDER_FILE_3G2:
     (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_3G2, QCAMCORDER_MAX_EXTENSION_SIZE);
     break;
   case QCAMCORDER_FILE_K3G:
     (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_K3G, QCAMCORDER_MAX_EXTENSION_SIZE);
     break;
   case QCAMCORDER_FILE_3G2_FRAG:
     (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_3G2_FRAG, QCAMCORDER_MAX_EXTENSION_SIZE);
     break;
   case QCAMCORDER_FILE_SKM:
     (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_SKM, QCAMCORDER_MAX_EXTENSION_SIZE);
     break;
   default:
     (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_MPG4, QCAMCORDER_MAX_EXTENSION_SIZE);
     break;
  }

  pMe->sCurrentFileName[0] = '\0';
  pMe->sRecordFileName[0]  = '\0';
  pMe->nBright             = 0;
  pMe->eState              = QCAMCORDER_NULL;
  pMe->bStopRecording      = FALSE;
  #ifndef  FEATURE_CAMERA_LCD_DIRECT_MODE 
  pMe->bPausePressed      = FALSE;
  pMe->nPreviousFrameTimeMs  = 0;
  #endif
  pMe->nRecordStartTime    = 0;
  pMe->nRecordStartTimeMs  = 0;

  pMe->nRecordTotalTime    = 0;
  pMe->nRecordTotalTimeMs  = 0;
  pMe->bPostEditDone       = FALSE;
  pMe->pMedia              = NULL;
  pMe->pRecordMedia        = NULL;
  pMe->uuid_data           = NULL;
  pMe->uuid                = NULL;
#ifdef FEATURE_VIDEONOSENSOR
  pMe->m_bNoSensor         = TRUE;
#endif /*FEATURE_VIDEONOSENSOR*/

  pMe->m_bKeySelect = FALSE; //Initialization for smooth zoom

  MEMSET(&pMe->GPSInfo, 0, sizeof(AEEGPSInfo));
  pMe->eGPSState = QCAMCORDER_GPS_DISABLED;
  MEMSET(&pMe->GPSInfoCB, 0, sizeof(AEECallback));
  pMe->nGPSAccuracy = AEEGPS_ACCURACY_HIGHEST;

  SETAEERECT (&rc, (pMe->di.cxScreen - QCAMCORDER_TIME_WIDTH), QCAMCORDER_TIME_Y,
                   QCAMCORDER_TIME_WIDTH, QCAMCORDER_TIME_HEIGHT);
  (void)IDISPLAY_Clone(pMe->a.m_pIDisplay, &(pMe->m_timeDisplay));
  IDISPLAY_SetClipRect(pMe->m_timeDisplay, &rc);
  (void)IDISPLAY_SetColor(pMe->m_timeDisplay, CLR_USER_TEXT, QCAMCORDER_TIME_COLOR);
  (void)IDISPLAY_SetColor(pMe->m_timeDisplay, CLR_USER_BACKGROUND, QCAMCORDER_BORDER_COLOR);

  SETAEERECT (&rc, (pMe->di.cxScreen - QCAMCORDER_NAME_WIDTH), QCAMCORDER_NAME_Y,
                   QCAMCORDER_NAME_WIDTH, QCAMCORDER_NAME_HEIGHT);
  (void)IDISPLAY_Clone(pMe->a.m_pIDisplay, &(pMe->m_nameDisplay));
  IDISPLAY_SetClipRect(pMe->m_nameDisplay, &rc);
  (void)IDISPLAY_SetColor(pMe->m_nameDisplay, CLR_USER_TEXT, QCAMCORDER_NAME_COLOR);
  (void)IDISPLAY_SetColor(pMe->m_nameDisplay, CLR_USER_BACKGROUND, QCAMCORDER_BORDER_COLOR);

#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
  SETAEERECT (&rc, QCAMCORDER_OVERLAY_NAME_X, pMe->di.cyScreen - 
              QCAMCORDER_SOFT_MENU_HEIGHT - QCAMCORDER_OVERLAY_NAME_HEIGHT,
              pMe->di.cxScreen - QCAMCORDER_OVERLAY_NAME_X, QCAMCORDER_OVERLAY_NAME_HEIGHT);
  (void)IDISPLAY_Clone(pMe->a.m_pIDisplay, &(pMe->m_overlayDisplay));
  QCAMCORDER_MSG_HIGH("Display overlay text from:2,%d,%d,%d" ,rc.y,rc.dx,rc.dy);
  IDISPLAY_SetClipRect(pMe->m_overlayDisplay, &rc);
  (void)IDISPLAY_SetColor(pMe->m_overlayDisplay, CLR_USER_TEXT, QCAMCORDER_NAME_COLOR);
  (void)IDISPLAY_SetColor(pMe->m_overlayDisplay, CLR_USER_BACKGROUND, QCAMCORDER_BORDER_COLOR);
#endif

  (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY)QCamcorderTimer,(void *)pMe);
#ifdef FEATURE_QCAMCORDER_TELOP
  pMe->b_telopEntered        = FALSE;
#endif /* FEATURE_QCAMCORDER_TELOP */
  pMe->m_bShowMenu           = FALSE;
  pMe->m_bRecord_to_Preview  = FALSE;
  pMe->m_bPreview_to_Record  = FALSE;
  pMe->m_bWait_to_Finish     = FALSE;
  pMe->bSaveInPrg            = FALSE;
  if((pMe->m_displaySize != QCAMCORDER_DISPLAY_FULL_SCREEN) &&
    (pMe->m_displaySize != QCAMCORDER_DISPLAY_MENU_SELECT))
  {
    pMe->m_displaySize = QCAMCORDER_DISPLAY_MENU_SELECT; //if parm read from config file invalid
  }
  pMe->m_playbackSize        = QCAMCORDER_PLAYBACK_REGULAR;

  // Initialize ProgCtl
  pMe->m_ProgCtl.m_pIShell   = pMe->a.m_pIShell;
  pMe->m_ProgCtl.m_pIDisplay = pMe->a.m_pIDisplay;

  // Positioning the progress bar right above soft menu during playback
  ProgBar_x = 0;
  ProgBar_y = pMe->di.cyScreen - QCAMCORDER_SOFT_MENU_HEIGHT - QCAMCORDER_PROGRESS_BAR_HEIGHT;
  SETAEERECT(&rectProg, ProgBar_x, ProgBar_y, pMe->di.cxScreen-1, QCAMCORDER_PROGRESS_BAR_HEIGHT-1);

  if(!QProgCtl_Init(&pMe->m_ProgCtl, &rectProg))
  {
    status = FALSE;
  }

#ifdef FEATURE_MMOVERLAY
  pMe->m_pIMMOverlayMgr = NULL;
#endif /* FEATURE_MMOVERLAY */

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  return status;
}

/*===========================================================================
FUNCTION: QcamcorderInit

  DESCRIPTION:
  This function allocates the classes for all the services required by
  Qcamcorder.

  PROTOTYPE:
    void QcamcorderInit(Qcamcorder * pMe);

  PARAMETERS:
   Qcamcorder * pMe - Pointer to the applet instance

  RETURN VALUE:
      None
===========================================================================*/
static boolean QcamcorderInit( Qcamcorder * pMe )
{
   boolean result;

   result = ((ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL,
                                   (void **) &pMe->m_pIPreview)  == SUCCESS) &&
             (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL,
                                   (void **) &pMe->m_pISettings) == SUCCESS) &&
             (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL,
                                   (void **) &pMe->m_pISoftMenu) == SUCCESS) &&
             (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MENUCTL,
                                   (void **) &pMe->m_pIEditMenu) == SUCCESS) &&
             (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MENUCTL,
                                   (void **) &pMe->m_pIMenu)     == SUCCESS)

                                                                             &&
            (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_TELEPHONE,
                                   (void **) &pMe->pIPhone)       == SUCCESS)


#ifdef FEATURE_MMOVERLAY
                                                                             &&
            (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MMOVERLAYMGR,
                                   (void **) &pMe->m_pIMMOverlayMgr) == SUCCESS)
#endif /* FEATURE_MMOVERLAY */

#ifdef FEATURE_QVPHONE
                                                                             &&
            (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_VIDEOPHONE,
                                   (void **) &pMe->m_pIVP)       == SUCCESS)
#endif /* FEATURE_QVPHONE */
            );

   if (result)
   {
      Qcamcorder_SetMenuStyle(pMe->m_pIPreview, 0, 0);
      Qcamcorder_SetMenuStyle(pMe->m_pISettings, 0, 0);
      Qcamcorder_SetMenuStyle(pMe->m_pISoftMenu, 0, 0);
      Qcamcorder_SetMenuStyle(pMe->m_pIEditMenu, 0, 0);
      Qcamcorder_SetMenuStyle(pMe->m_pIMenu, -2, 0);

      IMENUCTL_SetProperties(pMe->m_pIPreview, MP_MAXSOFTKEYITEMS);
      IMENUCTL_SetProperties(pMe->m_pISettings,MP_MAXSOFTKEYITEMS);
      IMENUCTL_SetProperties(pMe->m_pISoftMenu,MP_MAXSOFTKEYITEMS);
      IMENUCTL_SetProperties(pMe->m_pIEditMenu,MP_MAXSOFTKEYITEMS);

      result = Qcamcorder_CreateCamcorder(pMe);

#ifdef FEATURE_VIDEONOSENSOR
      /*
      If Camcorder Initialization Fails and there are files in the
      Flash, USB, FTL or MMC then return here
      */
      if(!result && (Qcamcorder_GetFilecount(pMe)))
         return(result);
#endif /*FEATURE_VIDEONOSENSOR*/

      //Default menu to display
      pMe->m_bShowMenu = TRUE;
      //lint -e570  Suppress complaint on loss of sign
      IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
      //lint +e570
      IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
      QCamcorder_InitMdp(pMe->a.m_pIDisplay);
   }

   if (!result)
   {
      if (pMe->m_pIPreview)
      {
         (void)IMENUCTL_Release(pMe->m_pIPreview);
         pMe->m_pIPreview = NULL;
      }

      if(pMe->m_pIEditMenu)
      {
         (void)IMENUCTL_Release(pMe->m_pIEditMenu);
         pMe->m_pIEditMenu = NULL;
      }

      if (pMe->m_pISettings)
      {
         (void)IMENUCTL_Release(pMe->m_pISettings);
         pMe->m_pISettings = NULL;
      }

      if (pMe->m_pISoftMenu)
      {
         (void)IMENUCTL_Release(pMe->m_pISoftMenu);
         pMe->m_pISoftMenu = NULL;
      }

      if (pMe->m_pIMenu)
      {
         (void)IMENUCTL_Release(pMe->m_pIMenu);
         pMe->m_pIMenu = NULL;
      }

    LISTENER_Cancel(&pMe->phoneListener);
    if (pMe->pIPhone)
    {
      (void)ITELEPHONE_Release(pMe->pIPhone);
      pMe->pIPhone = NULL;
    }


#ifdef FEATURE_MMOVERLAY
    if(pMe->m_pIMMOverlayMgr)
    {
      (void) IMMOVERLAYMGR_Release(pMe->m_pIMMOverlayMgr);
      pMe->m_pIMMOverlayMgr = NULL;
    }
#endif /* FEATURE_MMOVERLAY */

#ifdef FEATURE_QVPHONE
    if (pMe->m_pIVP)
    {
      (void)IVIDEOPHONE_Release( pMe->m_pIVP );
      pMe->m_pIVP = NULL;
    }
#endif /* FEATURE_QVPHONE */
   }

   return result;
}
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
/*=====================================================================
FUNCTION Qcamcorder_SetDirectMode

  DESCRIPTION: This function sets the LCD display mode. If it is to turn on
               the mode, the display info is passed from the second parm.

  PARAMETERS:
    pMe         - pointer to Qcamcorder data struct
    bDirectMode - flag to turn mode on/off

  RETURN VALUE:
    int Error code defined in AEEError.h
======================================================================*/
int Qcamcorder_SetDirectMode(Qcamcorder *pMe, boolean bDirectMode)
{
  int nRet = SUCCESS;
  if (!pMe)
    return EBADPARM;

  QCAMCORDER_MSG_MED("Qcamcorder SetDirectMode to %d", bDirectMode, 0, 0);
  if (!bDirectMode)
  {
    //No need to provide display info to turn off the mode
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_LCD_DIRECT_ACCESS_EX, 0, 0);
  }
  else
  {
    //Pass display info from p2 to turn on the mode.
    CameraDirectMode camera_direct;

    camera_direct.clsDisplay = 0; //for primary LCD display
    camera_direct.nLayer = -1;    //display preview in background
    if (pMe->m_displaySize == QCAMCORDER_DISPLAY_FULL_SCREEN)
    {
        camera_direct.cxDst = QCam_Sizes[QCAMCORDER_SIZE_QVGA].cx;
        camera_direct.cyDst = QCam_Sizes[QCAMCORDER_SIZE_QVGA].cy;
    }
    else
    {
        camera_direct.cxDst = QCam_Sizes[pMe->eSize].cx;
        camera_direct.cyDst = QCam_Sizes[pMe->eSize].cy;
    }

    camera_direct.xDst = (uint32)((pMe->di.cxScreen + BREW_OFFSET_X - camera_direct.cxDst)/2);
    camera_direct.yDst = (uint32)((pMe->di.cyScreen + BREW_OFFSET_Y - camera_direct.cyDst)/2);

    /*For LCDs with screen size less than the current display mode selected,
      xDst & yDst will be calculated negative in the above calculation. 
      Negative values are invalid for these variables.
    */
    if(camera_direct.xDst < 0)
    {
        camera_direct.xDst = 0;
    }
    if(camera_direct.yDst < 0)
    {
        camera_direct.yDst = 0;
    }

    QCAMCORDER_MSG_MED("Qcamcorder SetDirectMode widths cxDst=%d,cyDst=%d",
                       camera_direct.cxDst,camera_direct.cyDst, 0);
    QCAMCORDER_MSG_MED("Qcamcorder SetDirectMode offsets xDst=%d,yDst=%d",
                       camera_direct.xDst,camera_direct.yDst, 0);
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_LCD_DIRECT_ACCESS_EX ,
                           1, (int32)&camera_direct);
  }
  return nRet;
}
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE
/* ==========================================================================
FUNCTION QCAMCORDER_CREATECAMERA
DESCRIPTION
============================================================================= */
static boolean Qcamcorder_CreateCamcorder(Qcamcorder* pMe)
{
//#ifdef FEATURE_TMD_20_QVGA
//   AEESize displaySize = {240, 320};
//#else
//   AEESize displaySize = {176, 144};
//#endif

   AEEParmInfo pInfo;
   int32 nZoom = 0;
   if ((ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CAMERA, (void **)&pMe->m_pICam)) != SUCCESS)
      return FALSE;

#ifdef FEATURE_VIDEONOSENSOR
   /*Detected Sensor*/
   pMe->m_bNoSensor = FALSE;
#endif /*FEATURE_VIDEONOSENSOR*/
   if (pMe->m_displaySize == QCAMCORDER_DISPLAY_FULL_SCREEN)
     (void)ICAMERA_SetDisplaySize(pMe->m_pICam, &QCam_Sizes[QCAMCORDER_SIZE_QVGA]);
   else
     (void)ICAMERA_SetDisplaySize(pMe->m_pICam, &QCam_Sizes[pMe->eSize]);
   (void)ICAMERA_SetSize(pMe->m_pICam, &QCam_Sizes[pMe->eSize]);
   (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_WHITE_BALANCE, pMe->m_nwhiteBalance, 0);
   (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PREVIEW_TYPE , CAM_PREVIEW_MOVIE, 0);

   /*Check if Audio Format belongs to QCP Class*/
   if(pMe->wAudioFormatClsId == AEECLSID_MEDIAQCP &&
      pMe->wAudioFormat != QCAMCORDER_AUDIO_FORMAT_NONE)
   {
     (void)ICAMERA_SetAudioEncode(pMe->m_pICam, AEECLSID_MEDIAQCP, pMe->wAudioFormat);
   }
#ifdef FEATURE_CAMCORDER_AAC_REC
   /*Check if Audio Format belongs to AAC Class*/
   else if (pMe->wAudioFormatClsId == AEECLSID_MEDIAAAC)
   {
     (void)ICAMERA_SetAudioEncode(pMe->m_pICam, AEECLSID_MEDIAAAC, pMe->m_AACInfo);
   }
#endif /* FEATURE_CAMCORDER_AAC_REC */
   /*No Audio has been selected.*/
   else
   {
     (void)ICAMERA_SetAudioEncode(pMe->m_pICam, 0, 0); /* No Audio */
   }

   (void)ICAMERA_RegisterNotify(pMe->m_pICam, (PFNCAMERANOTIFY)Qcamcorder_CamEventNotify, pMe);

   (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_BRIGHTNESS, &pMe->nBright, (int32*)&pInfo);
   pMe->nBrightnessMin = pInfo.nMin; //0
   if(pMe->nBrightnessMin <0)
   { pMe->nBrightnessMin = 0;
   }
   if(pMe->nBright <0)
   { pMe->nBright=0;
   }
   pMe->nBrightnessMax = pInfo.nMax; //255
   pMe->nBrightnessStep = pInfo.nStep;  //1
   // Set brightness step to (max - min) / QCAMCORDER_BRIGHTNESS_STEPS if less
   if (pInfo.nStep < (pInfo.nMax - pInfo.nMin) / QCAMCORDER_BRIGHTNESS_STEPS)
   {
      pMe->nBrightnessStep = (pInfo.nMax - pInfo.nMin) / QCAMCORDER_BRIGHTNESS_STEPS;
   }


   (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_ZOOM, &nZoom, (int32*)&pInfo);
   if((pInfo.nMin != pMe->nZoomMin) ||
      (pInfo.nMax != pMe->nZoomMax) ||
      (pInfo.nStep != pMe->nZoomStep)||
      (pMe->nZoom > pInfo.nMax) ||
      (pMe->nZoom < pInfo.nMin))
   {
     pMe->nZoomMin = pInfo.nMin;//0
     pMe->nZoomMax = pInfo.nMax;//79
     pMe->nZoomStep = pInfo.nStep;//1
     pMe->nZoom = nZoom;
   }
   if(pMe->nZoom != nZoom)
   {
     (void)ICAMERA_SetZoom(pMe->m_pICam, pMe->nZoom);
   }

    // Reset the pInfo structure.
    ZEROAT(&pInfo);
    /*Get the values of Max focus,min focus and focus steps increment supported by
      sensor */
    (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_FOCUS, &pMe->nFocus, (int32*)&pInfo);
    pMe->nFocusMin = pInfo.nMin;
    pMe->nFocusMax = pInfo.nMax;
    pMe->nFocusStep = pInfo.nStep;
    pMe->nFocus = pInfo.nCurrent;

    /*Auto Focus By Default*/
    pMe->nAutoOrManFocus = AUTO_FOCUS_SELECTED;
    /*Camera is not focussing*/
    pMe->FocusInProgress = 0;

    if (pMe->nFocusStep <= 0)
        pMe->nFocusStep = 1;

   (void)ICAMERA_SetQuality(pMe->m_pICam, pMe->wQuality);
   (void)ICAMERA_SetSpaceLimit(pMe->m_pICam, pMe->wSpaceLimit);
#ifdef FEATURE_VIDEO_ENCODE_BITRATE
   (void)ICAMERA_SetBitRate(pMe->m_pICam, pMe->wBitRate);
#endif

   /* Inform OEM layer that Short Header is selected */
   if (pMe->wShortHeader != QCAMCORDER_SH_DISABLE)
   {
     CameraData *sh_data= NULL;

     sh_data = MALLOC(sizeof(CameraData));
     sh_data->pData = MALLOC(sizeof(pMe->wShortHeader));
     sh_data->dwBytes = sizeof(pMe->wShortHeader );
     (void)MEMCPY(sh_data->pData, &(pMe->wShortHeader), sizeof(pMe->wShortHeader));
     (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_SHORT_HEADER, sh_data);
     FREEIF(sh_data->pData);
     FREEIF(sh_data);
   }

   /* Inform OEM layer of video format: MPEG4 / H264 */
   (void)ICAMERA_SetVideoEncode(pMe->m_pICam, AEECLSID_MEDIAMPEG4, pMe->wVideoFormat);

#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  // Enable the Direct Mode by default if feature is on.
  (void)Qcamcorder_SetDirectMode(pMe, TRUE);
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE

   return TRUE;
}

/* ==========================================================================
FUNCTION QCAMCORDER_RELEASECAMERA
DESCRIPTION
============================================================================= */
static void Qcamcorder_ReleaseCamcorder(Qcamcorder* pMe)
{
   if (pMe->m_pICam)
   {
      int16   m_nMode;

      (void)ICAMERA_RegisterNotify(pMe->m_pICam, NULL, NULL);

      (void)ICAMERA_GetMode(pMe->m_pICam, &m_nMode, NULL);
      if (m_nMode != CAM_MODE_READY)
         (void)ICAMERA_Stop(pMe->m_pICam);

      (void)ICAMERA_Release(pMe->m_pICam);
      pMe->m_pICam = NULL;
   }
}

/*===========================================================================
FUNCTION Qcamcorder_FreeAppData

  DESCRIPTION
  This function frees any data initialized in the Init function.

  PROTOTYPE:
    void Qcamcorder_FreeAppData(IApplet* pi);

  PARAMETERS:
    Qcamcorder * pMe - Pointer to the applet instance

  RETURN VALUE:
   None
===========================================================================*/
static void Qcamcorder_FreeAppData (Qcamcorder * pMe)
{
   (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, NULL);

   if(pMe->pFileFormat)
   {
     FREE(pMe->pFileFormat);
     pMe->pFileFormat = NULL;
   }

#ifdef FEATURE_MP4_TRANSCODER
  if( pMe->pOutFile )
  {
    (void)IMOVIEFILE_Release(pMe->pOutFile);
    pMe->pOutFile = NULL;
  }
  if(pMe->pInFile)
  {
   (void)IMOVIEFILE_Release(pMe->pInFile);
   pMe->pInFile = NULL;
  }

#ifdef FEATURE_BREW_IMOVIEEDIT
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
  if(pMe->eState == QCAMCORDER_TRANSCODE)
  {
     if(pMe->m_pOutEdit)
     {
        (void)IMOVIEEDIT_TranscodeAbort(pMe->m_pOutEdit);
     }
     pMe->eState = QCAMCORDER_NULL;
  }
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
#endif //FEATURE_BREW_IMOVIEEDIT
  #endif /* FEATURE_MP4_TRANSCODER */

#ifdef FEATURE_BREW_IMOVIEEDIT
 QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);

 QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit); 

#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif //FEATURE_VIDEO_ENCODE_FILE_MERGE

  /*When the object is deleted, it must cancel all of its AEECallbacks 
  using CALLBACK_Cancel(). Canceling an un-scheduled callback is 
  harmless. If a callback were to be called after the 
  object is deleted, it would de-reference a dangling pointer
  Sice here we are releasing all handles this is a must.*/
  CALLBACK_Cancel(&pMe->m_movieeditCB); 

      
#endif //FEATURE_BREW_IMOVIEEDIT
#ifdef FEATURE_VIDEO_ENCODE_RAW
  if( pMe->pRawStartFile )
  {
    (void)IMOVIEFILE_Release(pMe->pRawStartFile);
    pMe->pRawStartFile = NULL;
  }
  if(pMe->pRawStopFile)
  {
    (void)IMOVIEFILE_Release(pMe->pRawStopFile);
    pMe->pRawStopFile = NULL;
  }
#endif /* FEATURE_VIDEO_ENCODE_RAW */

  if (pMe->m_pIPreview)
  {
     (void)IMENUCTL_Release(pMe->m_pIPreview);
     pMe->m_pIPreview = NULL;
  }

  if(pMe->m_pIEditMenu)
  {
     (void)IMENUCTL_Release(pMe->m_pIEditMenu);
     pMe->m_pIEditMenu = NULL;
  }

#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
  if (pMe->pFrame)
  {
      (void)IDIB_Release(pMe->pFrame);
      pMe->pFrame = NULL;
  }
  if (pMe->m_overlayDisplay)
  {
    (void)IDISPLAY_Release(pMe->m_overlayDisplay);
     pMe->m_overlayDisplay = NULL;
  }
#endif /* FEATURE_VIDEO_ENCODE_OVERLAY_UI */

  if (pMe->m_pISettings)
  {
     (void)IMENUCTL_Release(pMe->m_pISettings);
     pMe->m_pISettings = NULL;
  }
  if (pMe->pIFileMgr)
  {
     (void)IFILEMGR_Release(pMe->pIFileMgr);
     pMe->pIFileMgr = NULL;
  }
  if (pMe->pMedia)
  {
      /* MDP handle was registered in Qcamcorder_PlayMediaNotify
      which was used to display playback frames; Deregistering that layer */
      if(VideoPlayback.id != 0)
      {
        QCAMCORDER_MSG_HIGH("Deregistering MDP %d", VideoPlayback.id, 0, 0);
        mdp_clear_ovimg(VideoPlayback.id);
        mdp_deregister_ovimg(VideoPlayback.id);
        VideoPlayback.id = 0;
      }

      QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
      QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);
  }

  QCAMCORDER_IMEDIA_STOP(pMe->pRecordMedia);
  QCAMCORDER_IMEDIA_RELEASE(pMe->pRecordMedia);

  if (pMe->pIPosDet)
  {
     pMe->eGPSState = QCAMCORDER_GPS_DISABLED;
     CALLBACK_Cancel(&pMe->GPSInfoCB);
     (void)IPOSDET_Release(pMe->pIPosDet);
     pMe->pIPosDet = NULL;
  }
  if (pMe->m_pISoftMenu)
  {
     (void)IMENUCTL_Release(pMe->m_pISoftMenu);
     pMe->m_pISoftMenu = NULL;
  }
  if (pMe->m_pIMenu)
  {
     (void)IMENUCTL_Release(pMe->m_pIMenu);
     pMe->m_pIMenu = NULL;
  }
  if(pMe->a.m_pIDisplay)
  {
   (void)IDISPLAY_Release(pMe->a.m_pIDisplay);
   pMe->a.m_pIDisplay = NULL;
  }
  if(pMe->m_timeDisplay)
  {
    (void)IDISPLAY_Release(pMe->m_timeDisplay);
    pMe->m_timeDisplay = NULL;
  }
  if(pMe->m_nameDisplay)
  {
    (void)IDISPLAY_Release(pMe->m_nameDisplay);
    pMe->m_nameDisplay = NULL;
  }
  if(pMe->m_ProgCtl.m_pTitle)
  {
    (void)ISTATIC_Release(pMe->m_ProgCtl.m_pTitle);
    pMe->m_ProgCtl.m_pTitle = NULL;
  }

  LISTENER_Cancel(&pMe->phoneListener);
  if(pMe->pIPhone)
  {
    (void)ITELEPHONE_Release(pMe->pIPhone);
    pMe->pIPhone = NULL;
  }

#ifdef FEATURE_QVPHONE
  if (pMe->m_pIVP)
  {
    (void)IVIDEOPHONE_Release( pMe->m_pIVP );
    pMe->m_pIVP = NULL;
  }
#endif /* FEATURE_QVPHONE */

#ifdef FEATURE_VIDEO_AUDIO_DUBBING
  /* Free data used in creating IMedia object for audio dubbing */
  if(pMe->m_pmdExList)
  {
      if (pMe->m_pmdExList->pData)
      {
          FREE(pMe->m_pmdExList->pData);
          pMe->m_pmdExList->pData = NULL;
      }
      FREE(pMe->m_pmdExList);
      pMe->m_pmdExList = NULL;
  }

  pMe->AudioDubbing=FALSE;
#endif

#ifdef FEATURE_BREW_IMOVIEEDIT
  FREEIF(pMe->m_pPostEditTelopData);
#endif /* FEATURE_BREW_IMOVIEEDIT */

  //Clear the MDP register pointer before exit.
  //It ensures the other application can update the display
  QCamcorder_MdpDeregister ();

#ifdef FEATURE_MMOVERLAY
  if (pMe->m_pIMMOverlayMgr)
  {
    IMMOVERLAYMGR_Release(pMe->m_pIMMOverlayMgr);
    pMe->m_pIMMOverlayMgr = NULL;
  }
#else
  pMe->m_mdpMenu_ovId = NULL;
  pMe->m_mdpTitle_ovId = NULL;
#endif /* FEATURE_MMOVERLAY */

  Qcamcorder_ReleaseCamcorder(pMe);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  QCAMCORDER_MSG_HIGH("Qcamcorder App ended",0,0,0);
}

/* ==========================================================================
FUNCTION QCAMCORDER_SETMENUSTYLE
DESCRIPTION
============================================================================= */
static void Qcamcorder_SetMenuStyle(IMenuCtl* pIMenu, int so_x, int so_y)
{
   AEEItemStyle sstyle;
   AEEItemStyle ustyle;
   AEEMenuColors col;

   // selection style
   sstyle.ft = AEE_FT_RAISED;
   sstyle.xOffset = (uint16)so_x;
   sstyle.yOffset = (uint16)so_y;
   sstyle.roImage = AEE_RO_TRANSPARENT;
   ustyle.ft = AEE_FT_NONE;
   ustyle.xOffset = 0;
   ustyle.yOffset = 0;
   ustyle.roImage = AEE_RO_TRANSPARENT;
   IMENUCTL_SetStyle(pIMenu, &ustyle, &sstyle);

   // colors
   col.cSelText= RGB_BLACK;
   col.wMask = MC_BACK | MC_SEL_BACK| MC_SEL_TEXT;
   col.cBack = QCAMCORDER_SOFT_MENU_COLOR;

   col.cSelBack = QCAMCORDER_SOFT_MENU_SEL_COLOR;

   IMENUCTL_SetColors(pIMenu, &col);
   IMENUCTL_SetProperties(pIMenu, MP_UNDERLINE_TITLE);
   IMENUCTL_SetProperties(pIMenu, MP_WRAPSCROLL);
}

/* ==========================================================================
FUNCTION QCAMCORDER_MAPEVENTHANDLERS
DESCRIPTION
============================================================================= */
static boolean Qcamcorder_MapEventHandlers(Qcamcorder *pMe, IDialog *pIDialog, uint16 wDlgResID)
{
  switch(wDlgResID)
  {
#ifdef FEATURE_QCAMCORDER_TELOP
    case IDD_GET_INPUT:
      IDIALOG_SetEventHandler(pIDialog, Qcamcorder_GetInputHandleEvent, pMe);
      break;
#endif
#ifdef FEATURE_VIDEO_ENCODE_BITRATE
    case IDD_GET_BITRATE:
      IDIALOG_SetEventHandler(pIDialog, Qcamcorder_GetBitrateHandleEvent, pMe);
      break;
#endif /* FEATURE_VIDEO_ENCODE_BITRATE */
   default:
      return FALSE;
   }
   return TRUE;
}


#ifdef FEATURE_VIDEO_ENCODE_BITRATE
//lint -esym(715,dw)  Suppress complaint about dw not referenced
/* ==========================================================================
FUNCTION QCAMCORDER_GETBITRATEHANDLEEVENT
DESCRIPTION
============================================================================= */
boolean Qcamcorder_GetBitrateHandleEvent(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  Qcamcorder *pMe                = (Qcamcorder*) pUser;
  IDialog    *pCurrentDialog     = NULL;
  ITextCtl   *ctrl               = NULL;
  uint32     maxCharNum          = 6;
  AECHAR     bitrate[7]          = {0};   /* Allow maximum of 7 chars   */
  char       bitrate_in_chars[7] = {0};   /* 6 - bitrate, 1 - null char */
  boolean    result              = FALSE;

  pCurrentDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pCurrentDialog)
  {
    /* Get the pointer to the text control */
    ctrl = (ITextCtl*)IDIALOG_GetControl(pCurrentDialog, (int16)IDI_GET_BITRATE);
  }

  switch (evt)
  {
    case EVT_DIALOG_START:
    {
      /* Exit if we don't have valid a pointer */
      if (!ctrl)
      {
        QCAMCORDER_MSG_ERROR("Invalid context for starting user bitrate dialog",0,0,0);
          return FALSE;
      }

      /* Turn off Multiline property since we only need one line of dialog */
      ITEXTCTL_SetProperties(ctrl,
                             (ITEXTCTL_GetProperties(ctrl) & ~TP_MULTILINE) |
                             TP_FRAME);

      /* Limit the maximum number of chars allowed == 6 chars */
      ITEXTCTL_SetMaxSize(ctrl, (uint16)maxCharNum);

      if (pMe->wQuality == QCAMCORDER_QUALITY_USER)
      {
        /* Show the existing user specified bitrate */
        (void)SNPRINTF(bitrate_in_chars, sizeof(bitrate_in_chars),"%d", pMe->wBitRate);
        (void)ITEXTCTL_SetTitle(ctrl, QCAMCORDER_RES_FILE, IDS_ENTER_BITRATE, NULL);
      }
      else
      {
        QCAMCORDER_MSG_ERROR("Invalid bitrate dialog if Quality != User Specifed",0,0,0);
        return FALSE;
      }

      /* print the old value in the entry box */
      (void)STRTOWSTR(bitrate_in_chars, bitrate, (int)(sizeof(AECHAR) * (maxCharNum+1)));
      (void)ITEXTCTL_SetText(ctrl, bitrate, WSTRLEN(bitrate));

      /* Allow only numbers, no alphabets or symbols */
      (void)ITEXTCTL_SetInputMode(ctrl, AEE_TM_NUMBERS);
      ITEXTCTL_SetActive(ctrl, TRUE);

      if (pCurrentDialog)
      {
        (void)IDIALOG_SetFocus(pCurrentDialog, IDI_GET_BITRATE);
        IDIALOG_Redraw(pCurrentDialog);
      }

      result = TRUE;
      break;
    }

  case EVT_KEY:
    switch (w)
    {
      case AVK_UP:
      case AVK_LEFT:
      case AVK_DOWN:
      case AVK_RIGHT:
        result = TRUE;
        break;

     case AVK_SELECT:
        /* Exit if we don't have valid a pointer */
        if (!ctrl)
        {
          QCAMCORDER_MSG_ERROR("Invalid context for entering user bitrate dialog",0,0,0);
          return FALSE;
        }

        /* Get the user input */
        (void)ITEXTCTL_GetText(ctrl, bitrate, (int)(maxCharNum+1));

        /* Assign the user input into the final bitrate variable */
        if (WSTRLEN(bitrate) > 0)
        {
          (void)WSTRTOSTR(bitrate, bitrate_in_chars, sizeof(bitrate_in_chars));
          pMe->wBitRate = ATOI(bitrate_in_chars);
          if ((pMe->wFileFormat == QCAMCORDER_FILE_3G2_FRAG)
               && (pMe->wBitRate > MAX_3G2_FRAG_BITRATE_SUPPORTED))
          {
            (void)ISHELL_EndDialog(pMe->a.m_pIShell);
            pMe->msg = IDB_BITRATE;
            Qcamcorder_ErrorEventUserNotify(pMe);
            QCAMCORDER_MSG_ERROR("Invalid bitrate for 3g2frag format",0,0,0);
            pMe->wBitRate = 128000;
            return TRUE;
          }
		  if ((pMe->wFileFormat == QCAMCORDER_FILE_AMC)
               && (pMe->wBitRate > MAX_AMC_BITRATE_ALLOWED))
          {
            (void)ISHELL_EndDialog(pMe->a.m_pIShell);
            pMe->msg = IDB_BITRATE;
            Qcamcorder_ErrorEventUserNotify(pMe);
            QCAMCORDER_MSG_ERROR("Invalid bitrate for AMC format, Allowed 64kbps",0,0,0);
            pMe->wBitRate = MAX_AMC_BITRATE_ALLOWED;
            return TRUE;
          }
        }
        else
        {
          if (pMe->wFileFormat == QCAMCORDER_FILE_AMC)
	      pMe->wBitRate = 64000;
	  else
              pMe->wBitRate = 128000;
          QCAMCORDER_MSG_ERROR("Error in parsing user input. Assume %d",pMe->wBitRate,0,0);
        }

        /* if user entered bitRate is valid set it in OEM layer*/ 
        if(ICAMERA_SetBitRate(pMe->m_pICam, pMe->wBitRate) == EBADPARM)
        {
            pMe->msg = IDB_BITRATE;
            Qcamcorder_ErrorEventUserNotify(pMe);
            QCAMCORDER_MSG_ERROR("Invalid bitrate ",0,0,0);
            return TRUE;
        }

        /* End this dialog */
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        result = TRUE;
        break;

      case AVK_CLR:
        /* If user hits clear without entering user bitrate, we have to change
        ** the quality menu setting to HIGH quality and reset the user bitrate */
        pMe->wQuality = QCAMCORDER_QUALITY_HIGH;
        (void)ICAMERA_SetQuality(pMe->m_pICam, pMe->wQuality);

		if (pMe->wFileFormat == QCAMCORDER_FILE_AMC)
		      pMe->wBitRate = 64000;
		else
                      pMe->wBitRate = 128000;
        (void)ICAMERA_SetBitRate(pMe->m_pICam, pMe->wBitRate);

        /* End this dialog */
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        result = TRUE;
        break;
      default:
        break;
    }
    break;
  case EVT_DIALOG_END:
    /* Return to quality menu after the user bitrate dialog ends */
    (void)IMENUCTL_Redraw(pMe->m_pIMenu);
    result = TRUE;
    break;
  default: // Other events that have not been handled by the handler
    break;
  }

  return result;
}
//lint +esym(715,dw)
#endif /* FEATURE_VIDEO_ENCODE_BITRATE */


#ifdef FEATURE_QCAMCORDER_TELOP
/* ==========================================================================
FUNCTION QCAMCORDER_GETINPUTHANDLEEVENT
DESCRIPTION
============================================================================= */
boolean Qcamcorder_GetInputHandleEvent(void * pUser, AEEEvent evt, uint16 wParam, uint32 dw)
{
  Qcamcorder* pMe = (Qcamcorder*)pUser;
  ITextCtl * pInputTextCtl;
  IDialog * pCurrentDialog;
  tsml_struct tsml;
  AEERect rect;
  AEEMediaData md;
  uint32 stringsize=0;
  AECHAR* psInput = (AECHAR *) MALLOC(sizeof(AECHAR) * USER_TELOP_MAX_LENGTH);

  char prependText[200] = "<tsml><head><layout><region background-color=\"#000000\"></region><font color=\"#ffffff\"></font></layout></head><body><telop begin=\"0\" end=\"2000\">";
  char *pInput = (char *) MALLOC(sizeof(char) * USER_TELOP_MAX_LENGTH);
  char appendText[50] = "</telop></body></tsml>";

  boolean result = FALSE;

  if(((pCurrentDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell)) == NULL) &&
     evt != EVT_DIALOG_END)
  {
    if(!pMe->b_telopEntered)
    {
      Qcamcorder_Preview(pMe);
      pMe->b_telopEntered = FALSE;
    }

    if(pInput)
      FREEIF(pInput);

    if(psInput)
      FREEIF(psInput);

    return FALSE;
  }

  switch (evt)
  {
    case EVT_DIALOG_START:
      rect.x  = 0;
      rect.y  = 0;
      rect.dx = 150;
      rect.dy = 50;

      pInputTextCtl =  (ITextCtl*)IDIALOG_GetControl(pCurrentDialog,
                                                     IDI_GETINPUT_TEXT);
      ITEXTCTL_SetRect(pInputTextCtl, &rect);
      ITEXTCTL_SetMaxSize (pInputTextCtl, USER_TELOP_MAX_LENGTH);
      result = TRUE;
      break;

    case EVT_KEY:
      switch (wParam)
      {
        case AVK_UP:
        case AVK_LEFT:
        case AVK_DOWN:
        case AVK_RIGHT:
          result = TRUE;
          break;

        case AVK_SELECT:
          pInputTextCtl =  (ITextCtl*)IDIALOG_GetControl(pCurrentDialog,
                                                         IDI_GETINPUT_TEXT);

          (void)ITEXTCTL_GetText(pInputTextCtl, psInput, (USER_TELOP_MAX_LENGTH +1));
          (void)WSTRTOSTR(psInput, pInput, (USER_TELOP_MAX_LENGTH+1) * sizeof(char));
          QCAMCORDER_MSG_HIGH("SELECT PRESSED",0,0,0);
          tsml.version = 0;
          tsml.flags = 0;
          tsml.data[0] = '\0';
          tsml.data[1] = '\0';
          (void)STRLCPY(&tsml.data[2], prependText, sizeof(tsml.data)- 2);
          (void)STRLCAT(&tsml.data[2], pInput, sizeof(tsml.data) - 2);
          (void)STRLCAT(&tsml.data[2], appendText, sizeof(tsml.data) - 2);
          stringsize = STRLEN((&tsml.data[2]));
          pMe->b_telopEntered = TRUE;

#ifdef FEATURE_BREW_IMOVIEEDIT
          if(pMe->eState == QCAMCORDER_TELOP_POSTEDIT)
          {
            pMe->m_nPostEditTelopSize
              = stringsize+ 2 + sizeof(char) + sizeof(uint8);
            FREEIF(pMe->m_pPostEditTelopData);
            pMe->m_pPostEditTelopData = MALLOC( sizeof(tsml));
            (void)MEMCPY(pMe->m_pPostEditTelopData, &tsml, sizeof(tsml));
            pMe->eState = QCAMCORDER_POSTEDIT;
            Qcamcorder_BuildPostEditMenu(pMe);
          }
          else
          {
#ifdef FEATURE_VIDEO_MULTI_UUID
            CameraEncodeUUIDAtomInfo *uuid_info;
            uuid_info = MALLOC(sizeof(CameraEncodeUUIDAtomInfo));
            uuid_info->pAtom = MALLOC(sizeof(UUID_TSML));
            (void)MEMCPY(uuid_info->pAtom, UUID_TSML, sizeof(UUID_TSML));
            uuid_info->pData = MALLOC( sizeof(tsml));//
            uuid_info->dwDataSize = stringsize+ 2 + sizeof(char) + sizeof(uint8);
            (void)MEMCPY(uuid_info->pData, &tsml, sizeof(tsml));
            (void)ICAMERA_SetVideoEncode(pMe->m_pICam,
                                         CAM_ENCODE_UUID_LIST_ATOM_INFO,
                                         uuid_info);
            FREEIF(uuid_info->pAtom);
            FREEIF(uuid_info->pData);
            FREEIF(uuid_info);
#else /* FEATURE_VIDEO_MULTI_UUID */
            pMe->uuid = MALLOC(sizeof(CameraData));
            pMe->uuid->pData = MALLOC(sizeof(UUID_TSML));
            pMe->uuid->dwBytes = sizeof(UUID_TSML);
            (void)MEMCPY(pMe->uuid->pData, UUID_TSML, sizeof(UUID_TSML));
            pMe->uuid_data = MALLOC(sizeof(CameraData));
            pMe->uuid_data->pData = MALLOC( sizeof(tsml));//
            pMe->uuid_data->dwBytes = stringsize+ 2 + sizeof(char) + sizeof(uint8);
            (void)MEMCPY(pMe->uuid_data->pData, &tsml, sizeof(tsml));
            (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_UUID, pMe->uuid);
            (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_UUID_DATA, pMe->uuid_data);
            FREEIF(pMe->uuid->pData);
            FREEIF(pMe->uuid);
            FREEIF(pMe->uuid_data->pData);
            FREEIF(pMe->uuid_data);
#endif /* FEATURE_VIDEO_MULTI_UUID */

            md.clsData = MMD_FILE_NAME;
            md.pData = (void *)Qcamcorder_GetFileName(pMe);
            md.dwSize = 0;
            (void)ICAMERA_SetMediaData(pMe->m_pICam, &md, pMe->pFileFormat);

#ifndef FEATURE_VIDEO_MULTI_UUID
            if(pMe->wFileFormat == QCAMCORDER_FILE_AMC)
            {
              pMe->uuid = MALLOC(sizeof(CameraData));
              pMe->uuid->pData = MALLOC(sizeof(UUID_CPGD));
              pMe->uuid->dwBytes = sizeof(UUID_CPGD);
              (void)MEMCPY(pMe->uuid->pData, UUID_CPGD, sizeof(UUID_CPGD));
              pMe->uuid_data = MALLOC(sizeof(CameraData));
              pMe->uuid_data->pData = MALLOC(sizeof(cpgd));
              pMe->uuid_data->dwBytes = sizeof(cpgd);
              (void)MEMCPY(pMe->uuid_data->pData, &cpgd, sizeof(cpgd));
              (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_UUID, pMe->uuid);
              (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_UUID_DATA, pMe->uuid_data);
              FREEIF(pMe->uuid->pData);
              FREEIF(pMe->uuid);
              FREEIF(pMe->uuid_data->pData);
              FREEIF(pMe->uuid_data);
            }
#endif /* FEATURE_VIDEO_MULTI_UUID */
#else /* FEATURE_BREW_IMOVIEEDIT */

#ifdef FEATURE_VIDEO_MULTI_UUID
          {
            CameraEncodeUUIDAtomInfo *uuid_info;
            uuid_info = MALLOC(sizeof(CameraEncodeUUIDAtomInfo));
            uuid_info->pAtom = MALLOC(sizeof(UUID_TSML));
            (void)MEMCPY(uuid_info->pAtom, UUID_TSML, sizeof(UUID_TSML));
            uuid_info->pData = MALLOC( sizeof(tsml));//
            uuid_info->dwDataSize = stringsize+ 2 + sizeof(char) + sizeof(uint8);
            (void)MEMCPY(uuid_info->pData, &tsml, sizeof(tsml));
            (void)ICAMERA_SetVideoEncode(pMe->m_pICam,
                                         CAM_ENCODE_UUID_LIST_ATOM_INFO,
                                         uuid_info);
            FREEIF(uuid_info->pAtom);
            FREEIF(uuid_info->pData);
            FREEIF(uuid_info);
          }
#else /* FEATURE_VIDEO_MULTI_UUID */
          pMe->uuid = MALLOC(sizeof(CameraData));
          pMe->uuid->pData = MALLOC(sizeof(UUID_TSML));
          pMe->uuid->dwBytes = sizeof(UUID_TSML);
          (void)MEMCPY(pMe->uuid->pData, UUID_TSML, sizeof(UUID_TSML));
          pMe->uuid_data = MALLOC(sizeof(CameraData));
          pMe->uuid_data->pData = MALLOC( sizeof(tsml));//
          pMe->uuid_data->dwBytes = stringsize+ 2 + sizeof(char) + sizeof(uint8);
          (void)MEMCPY(pMe->uuid_data->pData, &tsml, sizeof(tsml));
          pMe->b_telopEntered = TRUE;
          (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_UUID, pMe->uuid);
          (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_UUID_DATA, pMe->uuid_data);
#endif /* FEATURE_VIDEO_MULTI_UUID */

          md.clsData = MMD_FILE_NAME;
          md.pData = (void *)Qcamcorder_GetFileName(pMe);
          md.dwSize = 0;
          (void)ICAMERA_SetMediaData(pMe->m_pICam, &md, pMe->pFileFormat);

#ifndef FEATURE_VIDEO_MULTI_UUID
          if(pMe->wFileFormat == QCAMCORDER_FILE_AMC)
          {
            pMe->uuid = MALLOC(sizeof(CameraData));
            pMe->uuid->pData = MALLOC(sizeof(UUID_CPGD));
            pMe->uuid->dwBytes = sizeof(UUID_CPGD);
            (void)MEMCPY(pMe->uuid->pData, UUID_CPGD, sizeof(UUID_CPGD));
            pMe->uuid_data = MALLOC(sizeof(CameraData));
            pMe->uuid_data->pData = MALLOC(sizeof(cpgd));
            pMe->uuid_data->dwBytes = sizeof(cpgd);
            (void)MEMCPY(pMe->uuid_data->pData, &cpgd, sizeof(cpgd));
            (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_UUID, pMe->uuid);
            (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_UUID_DATA, pMe->uuid_data);
            FREEIF(pMe->uuid->pData);
            FREEIF(pMe->uuid);
            FREEIF(pMe->uuid_data->pData);
            FREEIF(pMe->uuid_data);
          }
#endif /* FEATURE_VIDEO_MULTI_UUID */

          if(pMe->eState != QCAMCORDER_TELOP_POSTEDIT)
          {
#endif /* FEATURE_BREW_IMOVIEEDIT */
            switch(pMe->m_nTimer)
            {
              case QCAMCORDER_TIMER_LOW:
                pMe->nLeftSec = 3;
                break;
              case QCAMCORDER_TIMER_MED:
                pMe->nLeftSec = 5;
                break;
              case QCAMCORDER_TIMER_HI:
                pMe->nLeftSec = 10;
                break;
              case QCAMCORDER_TIMER_OFF:
              default:
                pMe->nLeftSec = 0;
                break;
            }

            if(pMe->nLeftSec > 0)
            {
              //lint -e570  Suppress complaint on loss of sign
              IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
              Qcamcorder_DisplaySelfTimerBar(pMe);
            }
            else
            {
              if((STRCMP(pMe->sStartSoundFile, "No Sound") != 0))
              {                                    /* If shutter sound file is valid, */
                Qcamcorder_PlayStartSound(pMe);    /* play sound and start recording */
              }
              else
              {                                    /* Start recording if no */
                Qcamcorder_RecordTelopTimer(pMe);  /* shutter sound file is selected */
              }
            }
          }
#ifndef FEATURE_BREW_IMOVIEEDIT
          else
          {
            pMe->eState = QCAMCORDER_POSTEDIT;
            Qcamcorder_BuildPostEditMenu(pMe);
          }
#endif /* FEATURE_BREW_IMOVIEEDIT */

          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
          break;

        case AVK_CLR:
          pMe->m_bPreview_to_Record = FALSE;
          if(pMe->eState == QCAMCORDER_TELOP_POSTEDIT)
          {
            pMe->eState = QCAMCORDER_POSTEDIT;
            Qcamcorder_BuildPostEditMenu(pMe);
          }
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
          break;

        default:
          break;
      }
      result = TRUE;
      break;

    case EVT_DIALOG_END:
      result = TRUE;
      break;

   default: // Other events that have not been handled by the handler
       result = TRUE;
      break;
 }

 if(pInput)
   FREEIF(pInput);

 if(psInput)
   FREEIF(psInput);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  return result;
}
#endif
/* ==========================================================================
FUNCTION QCAMCORDER_DISPLAYEDITMENUPIC
DESCRIPTION
============================================================================= */
static void Qcamcorder_DisplayEditMenuPic(Qcamcorder* pMe)
{
   AEERect rc = {0, 0, 176, 144};
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
   //lint +e570
}

/* ==========================================================================
FUNCTION QCAMCORDER_DISPLAYEDITMENUNAME
DESCRIPTION
============================================================================= */
static void Qcamcorder_DisplayEditMenuName(Qcamcorder* pMe)
{
   AEERect rc = {0, 0, QCAMCORDER_TIME_X, QCAMCORDER_MENU_HEIGHT};
   IImage *pImage = NULL;
   AECHAR szBuf[TEXT_STRING_SIZE] = {0};
   uint16 image0ID      = IDB_SELECT;
   uint16 image0Pos     = 0;
   uint16 image0StrID   = 0;
   int16 image0StrPos  = 16;

   image0StrID = IMENUCTL_GetSel(pMe->m_pISoftMenu);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
   //lint +e570

   // Draw the icon
   //lint -e774  Suppress complaint on if always evaluates to true
   if (image0ID)
   //lint +e774
   {
      pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, image0ID);
      if (pImage)
      {
         IIMAGE_Draw(pImage, image0Pos, 0);
         (void)IIMAGE_Release(pImage);
         pImage = NULL;
      }
      }

   // Draw the string
   if (image0StrID > 0)
   {
      rc.x = image0StrPos;
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, (uint16)image0StrID, szBuf, sizeof(szBuf));
      (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szBuf, -1, 0, 1, &rc, IDF_ALIGN_LEFT);
   }

   IDISPLAY_Update(pMe->a.m_pIDisplay);

}

/* ==========================================================================
FUNCTION QCAMCORDER_DISPLAYTAKEPICMENUNAME
DESCRIPTION
============================================================================= */
void Qcamcorder_DisplayPreviewMenuName(Qcamcorder* pMe, int item)
{
  AEERect rc = {0, 0, QCAMCORDER_TIME_X, QCAMCORDER_MENU_HEIGHT};
  IImage  *pImage = NULL;
  AECHAR  displayString[TEXT_STRING_SIZE] = {0};
  int32   index;
  int     count;
  uint16  selection = 0;
  uint16  imageID    = IDB_SELECT;

  selection = IMENUCTL_GetSel(pMe->m_pIPreview);
  (void)IMENUCTL_GetItemData(pMe->m_pIPreview, selection, (uint32 *)&index);
  count = IMENUCTL_GetItemCount(pMe->m_pIPreview);
  index += item;
  if (index < 0)
  {
     index = count - 1;
  }
  else if (index >= count)
  {
     index = 0;
  }

  selection = IMENUCTL_GetItemID(pMe->m_pIPreview, index);
  pMe->wPreviewMenuSel = selection;

  /*Remove the Focus Icon indicator once we navigate from Icon button*/
  if ((selection != IDS_FOCUS) && (pMe->m_bFocusStarted == TRUE))
  {
    Qcamcorder_RemoveFocusIcon(pMe);
  }

  if (!(pMe->eState == QCAMCORDER_RECORDING || 
        pMe->eState == QCAMCORDER_RESUME_RECORDING ||
        pMe->eState == QCAMCORDER_PAUSED))
  {
     /* If not recording set the rc.dx = cxScreen - 25. 
      * This is because the time needn't be displayed and only the focus icon 
      * might be displayed*/

    SETAEERECT(&rc, 0, 0, pMe->di.cxScreen - 25, QCAMCORDER_MENU_HEIGHT);
  }

  (void)ISHELL_LoadResString(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, 
                             (uint16)selection, displayString, 
                             sizeof(displayString));

  switch (selection)
  {
    case IDS_SETTINGS:
    case IDS_START:
    case IDS_STOP:
    case IDS_RESUME:
    case IDS_ALBUM:
    case IDS_PAUSE: 
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI 
    case IDS_OVERLAY_MENU:
#endif
      break;

    case IDS_BRIGHTNESS: 
      {
        char temp[20];

        int32 bright = pMe->nBright;
        if (pMe->nBrightnessMin < 0 || pMe->nBrightnessMin > 0)
        {
          bright -= pMe->nBrightnessMin;
        }
        if(pMe->nBrightnessStep)
        {
          (void)SNPRINTF(temp, sizeof(temp), "(%d/%d)", bright/pMe->nBrightnessStep,
                         (pMe->nBrightnessMax - pMe->nBrightnessMin)/pMe->nBrightnessStep);
        }
        else
        {
          (void)SNPRINTF(temp, sizeof(temp), " Error");
        }
        (void)STRTOWSTR(temp, &displayString[WSTRLEN(displayString)], 
                        (int)(sizeof(displayString) - (uint32)WSTRLEN(displayString)));
      }
      imageID    = IDB_UP_DOWN;
      break;

    case IDS_ZOOM:
      {
        char temp[20]; 
        (void)SNPRINTF(temp,sizeof(temp), 
                       " (%d/%d)", pMe->nZoom - pMe->nZoomMin + 1, 
                       pMe->nZoomMax - pMe->nZoomMin + 1);

        (void)STR_TO_WSTR(temp, &displayString[WSTRLEN(displayString)], 
                          (int)(sizeof(displayString) - (uint32)WSTRLEN(displayString))); 
      }
      imageID    = IDB_UP_DOWN;
      break;

    case IDS_FOCUS: 
      /*Manual focus*/
      if (pMe->nAutoOrManFocus) 
      { 
        char temp[20]; 

        (void)SNPRINTF(temp,sizeof(temp), " (%d/%d)", pMe->nFocus - pMe->nFocusMin +1,
                       pMe->nFocusMax - pMe->nFocusMin + 1);
        (void)STR_TO_WSTR(temp, &displayString[WSTRLEN(displayString)], 
                          (int)(sizeof(displayString) - (uint32)WSTRLEN(displayString)));
        imageID    = IDB_UP_DOWN;
      }   
      break;

    case IDS_GPS_SETUP:
      {
        int id = 0;

        (void)STR_TO_WSTR(" [", &displayString[WSTRLEN(displayString)], 
                          (int)(sizeof(displayString) - (uint32)WSTRLEN(displayString)));
        switch (pMe->eGPSState)
        {
          case QCAMCORDER_GPS_DISABLED: 
            id = IDS_GPS_DISABLED; 
            break; 
          case QCAMCORDER_GPS_ENABLED: 
            id = IDS_GPS_ENABLED; 
            break; 
          case QCAMCORDER_GPS_DATA_VALID: 
            id = IDS_GPS_DATA_VALID; 
            break; 
          case QCAMCORDER_GPS_ACQUIRING: 
            id = IDS_GPS_ACQUIRING; 
            break; 
          case QCAMCORDER_GPS_FAILURE: 
            id = IDS_GPS_FAILURE; 
            break; 
          case QCAMCORDER_GPS_TIMEOUT: 
            id = IDS_GPS_TIMEOUT; 
            break; 
          case QCAMCORDER_GPS_INFO_UNAVAILABLE: 
            id = IDS_GPS_INFO_UNAVAILABLE; 
            break; 
          case QCAMCORDER_GPS_STATUS_COUNT: 
            break;
        }

        (void)ISHELL_LoadResString(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, (uint16)id, 
                                   &displayString[WSTRLEN(displayString)], 
                                   (int)(sizeof(displayString) - (uint32)WSTRLEN(displayString))); 

        (void)STR_TO_WSTR("]", &displayString[WSTRLEN(displayString)], 
                          (int)(sizeof(displayString) - (uint32)WSTRLEN(displayString)));
      }
      break;

    default:
      QCAMCORDER_MSG_HIGH("Case not handled",0,0,0);
      break;
   }
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
   //lint +e570


   if(imageID)
   {
      pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, imageID);
      if (pImage)
      {
         IIMAGE_Draw(pImage, QCAMCORDER_PREVIEW_MENU_DISPLAY_IMAGE_X, 0);
         (void)IIMAGE_Release(pImage);
         pImage = NULL;
      }
   }

   if (displayString[0] != '\0')
   {
      rc.x = QCAMCORDER_PREVIEW_MENU_DISPLAY_STRING_X;
      rc.dx -= QCAMCORDER_PREVIEW_MENU_DISPLAY_STRING_X;
      (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, displayString, -1, 0, 1, &rc, IDF_ALIGN_LEFT);
   }

   (void)IMENUCTL_Redraw(pMe->m_pIPreview);

#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
  if(pMe->bEnableFrameOverlay)
  {
    Qcamcorder_ClearOverlayFileName(pMe,&rc);
  }
#endif
   IDISPLAY_Update(pMe->a.m_pIDisplay);
}

/* This macro takes in counter uint16 x & increments it & 
   performs a check for calling MSLEEP
*/
#define QCAMCORDER_FILE_LIST_LIMIT 200
#define QCAMCORDER_FILELIST_SLEEP_TIME 10
#define QCAMCORDER_CHECK_TIMETOSLEEP(x) {                        \
                    x++;                                         \
                    if(x >= QCAMCORDER_FILE_LIST_LIMIT){         \
                      MSLEEP(QCAMCORDER_FILELIST_SLEEP_TIME);    \
                      x = 0;}}

/* ==========================================================================
FUNCTION QCAMCORDER_STARTRECORDING
DESCRIPTION
============================================================================= */
static void Qcamcorder_StartRecording(Qcamcorder* pMe)
{
  boolean fDeleted = FALSE;

  while (pMe->m_maxNumClips &&
         (Qcamcorder_GetTotalClips(pMe) >= pMe->m_maxNumClips))
  {
    if (!Qcamcorder_DeleteOldestFile(pMe))
      break;

    /* Set flag if any file has been deleted */
    fDeleted = TRUE;
  }

  if(fDeleted)
  {
#ifdef FEATURE_MMOVERLAY  
    pMe->b_displayingSpaceErrorMsg = TRUE;
#endif /* FEATURE_MMOVERLAY */
    QCamcorder_MdpRegisterFullScreen(pMe, TRUE);

    Qcamcorder_DrawIcon(pMe, IDB_FILE_DELETE_DONE);
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, 500,
                    (PFNNOTIFY) Qcamcorder_DelayedRecording,
                    (void*)pMe);
  }
  else
  {
    Qcamcorder_Record(pMe, QCAMCORDER_RECORD_START);
  }
}

/* ==========================================================================
FUNCTION QCAMCORDER_DELAYEDRECORD
DESCRIPTION
============================================================================= */
static void Qcamcorder_DelayedRecording(Qcamcorder* pMe)
{

#ifdef FEATURE_MMOVERLAY 
    pMe->b_displayingSpaceErrorMsg = FALSE;
#endif /* FEATURE_MMOVERLAY  */

  Qcamcorder_Record(pMe, QCAMCORDER_RECORD_START);
}

/* ==========================================================================
FUNCTION QCAMCORDER_DISPLAYRECORDTIME
DESCRIPTION
============================================================================= */
static void Qcamcorder_DisplayRecordTime(Qcamcorder* pMe)
{
   static uint32 lastTime = 0;
   AECHAR szBuf[TEXT_STRING_SIZE] = {0};
   char   timeStr[9] = "";
   JulianType date = {0};

   uint32 timems = GETUPTIMEMS();
   uint32 time = (timems/1000);
   uint32 elapsedTime = 0,elapsedTimems = 0;

   if (pMe->nRecordStartTime > 0)
   {
      elapsedTime = time - pMe->nRecordStartTime;
   }

   if (pMe->nRecordStartTimeMs > 0)
   {
      elapsedTimems = timems - pMe->nRecordStartTimeMs;
   }

   elapsedTime += pMe->nRecordTotalTime;
   elapsedTimems += pMe->nRecordTotalTimeMs;

   if (pMe->eState == QCAMCORDER_RECORDING || pMe->eState == QCAMCORDER_PAUSED ||
       pMe->eState == QCAMCORDER_RESUME_RECORDING)
   {
      if (lastTime != time || pMe->eState == QCAMCORDER_PAUSED ||
          pMe->eState == QCAMCORDER_RESUME_RECORDING)
      {
         // GETJULIANDATE returns the current date/time when seconds == 0
         if (elapsedTime > 0)
         {
            GETJULIANDATE(elapsedTime, &date);
         }
         (void)SNPRINTF(timeStr, sizeof(timeStr), "%02d:%02d:%02d",
                         date.wHour, date.wMinute, date.wSecond);
         (void)STR_TO_WSTR(timeStr, szBuf, sizeof(szBuf));
         (void)IDISPLAY_DrawText(pMe->m_timeDisplay, AEE_FONT_BOLD, szBuf, -1,
                                 0, QCAMCORDER_TIME_Y, NULL, IDF_ALIGN_RIGHT);

         IDISPLAY_Update(pMe->m_timeDisplay);
         lastTime = time;
      }
      // Check for max clip length
      if (pMe->m_maxClipLength && (elapsedTimems >= (pMe->m_maxClipLength * 1000)))
      {
         Qcamcorder_Record(pMe, QCAMCORDER_RECORD_STOP);
      }

      /* Display the name of the current recording session */
      Qcamcorder_DisplayRecordFileName(pMe);

      if (pMe->eState == QCAMCORDER_RECORDING)
      {

        (void)ISHELL_SetTimer(pMe->a.m_pIShell, 250, (PFNNOTIFY)Qcamcorder_DisplayRecordTime,(void *)pMe);
      }

   }
}


/* ==========================================================================
FUNCTION QCAMCORDER_DISPLAYRECORDFILENAME
DESCRIPTION
============================================================================= */
static void Qcamcorder_DisplayRecordFileName(Qcamcorder* pMe)
{
  AECHAR szBuf[FILE_NAME_SIZE] = {0};

  (void)STR_TO_WSTR(pMe->sRecordFileName, szBuf, sizeof(szBuf));
  (void)IDISPLAY_DrawText(pMe->m_nameDisplay, AEE_FONT_NORMAL, szBuf, -1,
                          0, QCAMCORDER_NAME_Y, NULL, IDF_ALIGN_RIGHT);
  IDISPLAY_Update(pMe->m_nameDisplay);
}


/* ==========================================================================
FUNCTION QCAMCORDER_DOSETTINGSMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_DoSettingsMenu(Qcamcorder* pMe, int item)
{
   uint16 selection;
   int32 index;
   int count;

   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
   selection = IMENUCTL_GetSel(pMe->m_pISettings);
   (void)IMENUCTL_GetItemData(pMe->m_pISettings, selection, (uint32 *)&index);
   count = IMENUCTL_GetItemCount(pMe->m_pISettings);
   index += item;

   if (index < 0)
   {
      index = count - 1;
   }
   else if (index >= count)
   {
      index = 0;
   }

   selection = IMENUCTL_GetItemID(pMe->m_pISettings, index);

   switch (selection)
   {
   case IDS_START_SOUND:
      Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_SOUND);
      break;
   case IDS_QUALITY:
      Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_QUALITY);
      break;
   case IDS_SIZE:
      Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_SIZE);
      break;
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
   case IDS_AUDIO_FORMAT:
      Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_AUDIO_FORMAT);
      break;
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
   case IDS_SAVE_LOC:
      Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_SAVE_LOC);
      break;
   case IDS_DELETEALL:
      Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_DELETEALL);
      break;
   case IDS_NUMCLIPS:
      Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_NUMCLIPS);
      break;
   case IDS_CLIPLENGTH:
      Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_CLIPLENGTH);
      break;
   case IDS_FRAME_RATE:
       Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_FRAMERATE);
       break;
   case IDS_WHITEBALANCE:
       Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_WHITEBALANCE);
       break;
   case IDS_MEM_USAGE:
      Qcamcorder_MemUsage(pMe);
      break;
   case IDS_FILE_FORMAT:
       Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_FILE_FORMAT);
       break;
   case IDS_VIDEO_FORMAT:
       Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_VIDEO_FORMAT);
       break;
     case IDS_TIMER:
         Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_TIMER);
         break;
     case IDS_EFFECT:
         Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_EFFECTS);
         break;
     case IDS_ANTIBANDING:
         Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_ANTIBANDING);
         break;
     case IDS_ROTATION:
          Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_ROTATE);
          break;
#ifdef FEATURE_MP4_TRANSCODER
   case IDS_TRANSCODER_TARGET_AUDIO:
    Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_TRANSCODER_AUDIO_FORMAT);
    break;
   case IDS_TRANSCODE_TARGET_BRAND:
     Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_TRANSCODER_BRAND);
    break;
   case IDS_TRANSCODER_VIDEO_TYPE:
    Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_TRANSCODER_VIDEO_SIZE);
    break;
   case IDS_TRANSCODER_VIDEO_FORMAT:
    Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_TRANSCODER_VIDEO_FORMAT);
    break;
#endif
#ifdef FEATURE_DVS
   case IDS_DVSMARGIN:
    Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_DVSMARGIN);
    break;
#endif
#ifdef FEATURE_QCAMCORDER_TELOP
   case IDS_TELOP_TEXT:
       Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_TELOP);
       break;
#endif
   case IDS_CONTINUOUS:
        Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_CONT);
        break;
   case IDS_SPECIAL_EFFECTS:
        Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_SPECIAL_EFFECTS);
        break;
#ifdef FEATURE_CAMCORDER_AAC_REC
    case IDS_AAC_CONFIG:
      Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_AAC_CONFIG);
      break;
    case IDS_AAC_MODE:
      Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_AAC_MODE);
      break;
    case IDS_AAC_FREQ:
      Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_AAC_FREQ);
      break;
    case IDS_AAC_BITRATE:
      Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_AAC_BITRATE);
      break;
#endif /* FEATURE_CAMCORDER_AAC_REC */
     case IDS_SPACE:
       Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_SPACE_THRESHOLD);
       break;
     default:
       QCAMCORDER_MSG_HIGH("WRONG MENU ITEM PRESSED",0,0,0);
       break;
   }

   (void)IMENUCTL_Redraw(pMe->m_pISettings);
}


/* ==========================================================================
FUNCTION QCAMCORDER_RECORDTIMER
DESCRIPTION
============================================================================= */
static void Qcamcorder_RecordTimer(Qcamcorder *pMe)
{
  pMe->eState = QCAMCORDER_START_RECORDING;
  pMe->m_bPreview_to_Record = TRUE;
  (void)ICAMERA_Stop(pMe->m_pICam);
}


/* ==========================================================================
FUNCTION QCAMCORDER_RECORDTELOPTIMER
DESCRIPTION
============================================================================= */
static void Qcamcorder_RecordTelopTimer(Qcamcorder *pMe)
{
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
  {
    if(ICAMERA_RecordMovie(pMe->m_pICam) == EUNSUPPORTED)
    {
      pMe->msg = IDB_ERROR_QCAMCORDER;
      Qcamcorder_ErrorEventUserNotify(pMe);
    }
  }

  pMe->nRecordStartTimeMs = GETUPTIMEMS();
  pMe->nRecordStartTime = (pMe->nRecordStartTimeMs/1000);
  #ifndef  FEATURE_CAMERA_LCD_DIRECT_MODE 
  pMe->nPreviousFrameTimeMs  = pMe->nRecordStartTimeMs;
  #endif
  Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_PREVIEW);
  Qcamcorder_DisplayRecordTime(pMe);
}


/* ==========================================================================
FUNCTION QCAMCORDER_RECORDING
DESCRIPTION
============================================================================= */
void Qcamcorder_Record(Qcamcorder *pMe, QcamcorderRecordType type)
{
  (void)ISHELL_CancelTimer( pMe->a.m_pIShell, NULL, pMe );

  switch (type)
  {
    case QCAMCORDER_RECORD_START:

      // Check for max # clips
#ifdef FEATURE_VIDEO_ENCODE_FADING
      if(pMe->eState== QCAMCORDER_FADING_OUT)
      { //we are fading out, ignore start
        QCAMCORDER_MSG_HIGH("Fading out: Start record keypress ignored",0,0,0);
        return;
      }
#endif
      while (pMe->m_maxNumClips &&
             (Qcamcorder_GetTotalClips(pMe) >= pMe->m_maxNumClips))
      {
        if (!Qcamcorder_DeleteOldestFile(pMe))
        {
         break;
        }
      }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif//FEATURE_APP_TEST_AUTOMATION
      if ((pMe->m_bRecord_to_Preview) || (pMe->m_bPreview_to_Record))
      {
         QCAMCORDER_MSG_HIGH("Key Press too fast, ignoring Keypress." ,0,0,0);
         return;
      }
      switch(pMe->m_nTimer)
      {
        case QCAMCORDER_TIMER_LOW:
          pMe->nLeftSec = 3;
          break;
        case QCAMCORDER_TIMER_MED:
          pMe->nLeftSec = 5;
          break;
        case QCAMCORDER_TIMER_HI:
          pMe->nLeftSec = 10;
          break;
        case QCAMCORDER_TIMER_OFF:
        default:
          pMe->nLeftSec = 0;
          break;
      }

      if((STRCMP(pMe->sStartSoundFile, "No Sound") != 0)
#ifdef FEATURE_QCAMCORDER_TELOP
         && (pMe->m_bTelop == TELOP_OFF)
#endif /* FEATURE_QCAMCORDER_TELOP */
        )
      {
        if(pMe->nLeftSec > 0)
        {
          Qcamcorder_DisplaySelfTimerBar(pMe);
        }
        else
        {
          Qcamcorder_PlayStartSound(pMe);
        }
      }
      else
      {
        if(pMe->eState == QCAMCORDER_PREVIEW)
        {
          /* If telop enabled, do not countdown
          ** until user has entered telop text */
          if(pMe->nLeftSec > 0 && !pMe->m_bTelop)
          {
            Qcamcorder_DisplaySelfTimerBar(pMe);
          }
          else
          {
            pMe->eState = QCAMCORDER_START_RECORDING;
            pMe->m_bPreview_to_Record = TRUE;
            if (ICAMERA_Stop(pMe->m_pICam) != SUCCESS)
            {
              QCAMCORDER_MSG_HIGH("Qcamcorder_Record: stop fails while starting." ,0,0,0);
              pMe->m_bPreview_to_Record = FALSE;
              Qcamcorder_Preview(pMe);
            }
          }
        }
      }
      break;

    case QCAMCORDER_RECORD_PAUSE:
      if(ICAMERA_Pause(pMe->m_pICam) != SUCCESS)
      {
        QCAMCORDER_MSG_ERROR("Camera Pause Request failed", 0, 0, 0);
      }
#ifndef  FEATURE_CAMERA_LCD_DIRECT_MODE
      else
      {
        pMe->bPausePressed   = TRUE;
        pMe->nRecordStartTime = 0;
        pMe->nRecordStartTimeMs = 0;
      }
#endif
      break;

    case QCAMCORDER_RECORD_RESUME:
      if(ICAMERA_Resume(pMe->m_pICam) != SUCCESS)
      {
        QCAMCORDER_MSG_ERROR("Camera Resume Request failed", 0, 0, 0);
      }
#ifndef  FEATURE_CAMERA_LCD_DIRECT_MODE
      else
      {
        pMe->bPausePressed   = FALSE;
        pMe->nRecordStartTimeMs = GETUPTIMEMS(); 
        pMe->nRecordStartTime = (pMe->nRecordStartTimeMs/1000);
        pMe->nPreviousFrameTimeMs = pMe->nRecordStartTimeMs;
      }
#endif
      break;

  case QCAMCORDER_RECORD_STOP:
#ifndef  FEATURE_CAMERA_LCD_DIRECT_MODE
      pMe->bPausePressed   = FALSE;
#endif
      if ((pMe->m_bRecord_to_Preview) || (pMe->m_bPreview_to_Record))
      {
         QCAMCORDER_MSG_HIGH("Key Press too fast, ignoring Keypress." ,0,0,0);
         return;
      }
      /*
        Icamera_stop succeeds if camera is in correct state for
        closure.Also Icamera_stop issues a call back to
        Qcamcorder function to start the preview.
      */
      if (ICAMERA_Stop(pMe->m_pICam) == SUCCESS)
      {
#ifdef FEATURE_VIDEO_ENCODE_FADING
        if(pMe->mFadeInOut == QCAMCORDER_FADE_OUT ||
           pMe->mFadeInOut == QCAMCORDER_FADE_IN_FADE_OUT)
        {
           pMe->eState = QCAMCORDER_FADING_OUT;
        }
        else
        {
#endif
          pMe->eState = QCAMCORDER_START_PREVIEW;
#ifdef FEATURE_VIDEO_ENCODE_FADING
        }
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif//FEATURE_APP_TEST_AUTOMATION
        pMe->m_bRecord_to_Preview = TRUE;
        pMe->nRecordStartTime = 0;
        pMe->nRecordStartTimeMs = GETUPTIMEMS();  
        pMe->nRecordTotalTime = 0;
        pMe->nRecordTotalTimeMs = 0;
#ifndef  FEATURE_CAMERA_LCD_DIRECT_MODE
        pMe->nPreviousFrameTimeMs  = 0;
#endif
      }
      /*
        If Icamera_Stop fails then user initiated stop has failed.
        This is because camera may have already been closed
        due to error condition. In error conditions we start a
        timer of 1.5 s. Once timer expires preview is started.
        In between if user presses stop button, then timer will
        be cancelled and Icamera_stop also fails. Now we will not
        get a callback from either timer or Icamera_stop and preview
        will not be started.Hence we can start preview here.
      */
      else
      {
          Qcamcorder_Preview(pMe);
      }
      break;

  case QCAMCORDER_RECORD_ERROR:
      pMe->nRecordStartTime = 0;
      pMe->nRecordStartTimeMs = 0;
      pMe->nRecordTotalTime = 0;
      pMe->nRecordTotalTimeMs = 0;
#ifndef  FEATURE_CAMERA_LCD_DIRECT_MODE
      pMe->nPreviousFrameTimeMs = 0;
#endif
      /* Intentional fall through */

  case QCAMCORDER_RECORD_SPACE_WARNING:
      Qcamcorder_ErrorEventUserNotify(pMe);
      break;

   default:
      QCAMCORDER_MSG_HIGH("Qcamcorder_Record() case not handled", 0, 0, 0);
      break;
   }
   return;
} /* END Qcamcorder_Record */

static void Qcamcorder_FillGPSData(Qcamcorder *pMe, gpsi_struct *gpsi)
{
    int32 latitude = 0, longitude = 0;
    int32 degrees = 0;

    gpsi->uuid_size         = 0xf2000000;
    gpsi->uuid_versionflags = 0x00000000;
    gpsi->type              = 0x69737067; /* 'gpsi' */
    gpsi->versionflags      = 0x00000000;

    gpsi->byte_order[0]         = 0x4d;
    gpsi->byte_order[1]         = 0x4d;
    gpsi->gps_version_num_if_id = 0x0700;

    gpsi->gps_version_tag_id  = 0x00;
    gpsi->gps_version_type[0] = 0x00;
    gpsi->gps_version_type[1] = 0x01;

    gpsi->gps_version_count[0] = 0x00;
    gpsi->gps_version_count[1] = 0x00;
    gpsi->gps_version_count[2] = 0x00;
    gpsi->gps_version_count[3] = 0x04;

    gpsi->gps_version_value[0] = 0x02;
    gpsi->gps_version_value[1] = 0x02;
    gpsi->gps_version_value[2] = 0x00;
    gpsi->gps_version_value[3] = 0x00;

    gpsi->gps_latitude_ref_tagID[0] =0x00;
    gpsi->gps_latitude_ref_tagID[1] =0x01;

    gpsi->gps_latitude_ref_type[0] = 0x00;
    gpsi->gps_latitude_ref_type[1] = 0x02;

    gpsi->gps_latitude_ref_count[0] = 0x00;
    gpsi->gps_latitude_ref_count[1] = 0x00;
    gpsi->gps_latitude_ref_count[2] = 0x00;
    gpsi->gps_latitude_ref_count[3] = 0x02;

    if(pMe->GPSInfo.dwLat > 0)
      gpsi->gps_latitude_ref_value[0] = 0x4E;
    else
      gpsi->gps_latitude_ref_value[0] = 0x53;

    gpsi->gps_latitude_ref_value[1] = 0x00;
    gpsi->gps_latitude_ref_value[2] = 0x00;
    gpsi->gps_latitude_ref_value[3] = 0x00;

    gpsi->gps_latitude_tagID[0] = 0x00;
    gpsi->gps_latitude_tagID[1] = 0x02;

    gpsi->gps_latitude_type[0] = 0x00;
    gpsi->gps_latitude_type[1] = 0x05;

    gpsi->gps_latitude_count[0] = 0x00;
    gpsi->gps_latitude_count[1] = 0x00;
    gpsi->gps_latitude_count[2] = 0x00;
    gpsi->gps_latitude_count[3] = 0x03;

    gpsi->gps_latitude_value[0] = 0x00;
    gpsi->gps_latitude_value[1] = 0x00;
    gpsi->gps_latitude_value[2] = 0x00;
    gpsi->gps_latitude_value[3] = 0x98;

    gpsi->gps_longitude_ref_tagID[0] = 0x00;
    gpsi->gps_longitude_ref_tagID[1] = 0x03;
    gpsi->gps_longitude_ref_type[0]  = 0x00;
    gpsi->gps_longitude_ref_type[1]  = 0x02;

    gpsi->gps_longitude_ref_count[0] = 0x00;
    gpsi->gps_longitude_ref_count[1] = 0x00;
    gpsi->gps_longitude_ref_count[2] = 0x00;
    gpsi->gps_longitude_ref_count[3] = 0x02;

    if(pMe->GPSInfo.dwLon > 0)
      gpsi->gps_longitude_ref_value[0] = 0x45; //East
    else
      gpsi->gps_longitude_ref_value[0] = 0x57; //West

    gpsi->gps_longitude_ref_value[1] = 0x00;
    gpsi->gps_longitude_ref_value[2] = 0x00;
    gpsi->gps_longitude_ref_value[3] = 0x00;

    gpsi->gps_longitude_tagID[0] = 0x00;
    gpsi->gps_longitude_tagID[1] = 0x04;
    gpsi->gps_longitude_type[0] = 0x00;
    gpsi->gps_longitude_type[1] = 0x05;

    gpsi->gps_longitude_count[0] = 0x00;
    gpsi->gps_longitude_count[1] = 0x00;
    gpsi->gps_longitude_count[2] = 0x00;
    gpsi->gps_longitude_count[3] = 0x03;

    gpsi->gps_longitude_value[0] = 0x00;
    gpsi->gps_longitude_value[1] = 0x00;
    gpsi->gps_longitude_value[2] = 0x00;
    gpsi->gps_longitude_value[3] = 0xb0;

    gpsi->gps_altitude_ref_tagID[0] = 0x00;
    gpsi->gps_altitude_ref_tagID[1] = 0x05;

    gpsi->gps_altitude_ref_type[0] = 0x00;
    gpsi->gps_altitude_ref_type[1] = 0x01;

    gpsi->gps_altitude_ref_count[0] = 0x00;
    gpsi->gps_altitude_ref_count[1] = 0x00;
    gpsi->gps_altitude_ref_count[2] = 0x00;
    gpsi->gps_altitude_ref_count[3] = 0x01;

    if(pMe->GPSInfo.wAltitude > 0)
    {
      gpsi->gps_altitude_ref_value[0] = 0x00;
      gpsi->gps_altitude_ref_value[1] = 0x00;
      gpsi->gps_altitude_ref_value[2] = 0x00;
      gpsi->gps_altitude_ref_value[3] = 0x00;
    }
    else
    {
      gpsi->gps_altitude_ref_value[0] = 0x00;
      gpsi->gps_altitude_ref_value[1] = 0x00;
      gpsi->gps_altitude_ref_value[2] = 0x00;
      gpsi->gps_altitude_ref_value[3] = 0x01;
    }

    gpsi->gps_altitude_tagID[0] = 0x00;
    gpsi->gps_altitude_tagID[1] = 0x06;

    gpsi->gps_altitude_type[0] = 0x00;
    gpsi->gps_altitude_type[1] = 0x05;

    gpsi->gps_altitude_count[0] = 0x00;
    gpsi->gps_altitude_count[1] = 0x00;
    gpsi->gps_altitude_count[2] = 0x00;
    gpsi->gps_altitude_count[3] = 0x01;

    gpsi->gps_altitude_value[0] = 0x00;
    gpsi->gps_altitude_value[1] = 0x00;
    gpsi->gps_altitude_value[2] = 0x00;
    gpsi->gps_altitude_value[3] = 0xc8;

    gpsi->gps_timestamp_tagID[0] = 0x00;
    gpsi->gps_timestamp_tagID[1] = 0x07;

    gpsi->gps_timestamp_type[0] = 0x00;
    gpsi->gps_timestamp_type[1] = 0x05;

    gpsi->gps_timestamp_count[0] = 0x00;
    gpsi->gps_timestamp_count[1] = 0x00;
    gpsi->gps_timestamp_count[2] = 0x00;
    gpsi->gps_timestamp_count[3] = 0x03;

    gpsi->gps_timestamp_value[0] = (uint8)(((pMe->GPSInfo.dwTimeStamp)>>24) & 0xFF);

    gpsi->gps_timestamp_value[1] = (uint8)(((pMe->GPSInfo.dwTimeStamp)>>16) & 0xFF);
    gpsi->gps_timestamp_value[2] = (uint8)(((pMe->GPSInfo.dwTimeStamp)>>8) & 0xFF);
    gpsi->gps_timestamp_value[3] = (uint8)(pMe->GPSInfo.dwTimeStamp & 0xFF);

    gpsi->gps_mapDatum_tagID[0] = 0x00;
    gpsi->gps_mapDatum_tagID[1] = 0x12;

    gpsi->gps_mapDatum_type[0] = 0x00;
    gpsi->gps_mapDatum_type[1] = 0x02;

    gpsi->gps_mapDatum_count[0] = 0x00;
    gpsi->gps_mapDatum_count[1] = 0x00;
    gpsi->gps_mapDatum_count[2] = 0x00;
    gpsi->gps_mapDatum_count[3] = 0x06;

    gpsi->gps_mapDatum_value[0] = 0x00;
    gpsi->gps_mapDatum_value[1] = 0x00;
    gpsi->gps_mapDatum_value[2] = 0x00;
    gpsi->gps_mapDatum_value[3] = 0x8c;

    gpsi->gps_processing_tagID[0] = 0x00;
    gpsi->gps_processing_tagID[1] = 0x1b;
    gpsi->gps_processing_type[0]  = 0x00;
    gpsi->gps_processing_type[1]  = 0x07;
    gpsi->gps_processing_count[0] = 0x00;
    gpsi->gps_processing_count[1] = 0x00;
    gpsi->gps_processing_count[2] = 0x00;
    gpsi->gps_processing_count[3] = 0x00;
    gpsi->gps_processing_value[0] = 0x00;
    gpsi->gps_processing_value[1] = 0x00;
    gpsi->gps_processing_value[2] = 0x00;
    gpsi->gps_processing_value[3] = 0x00;

    gpsi->gps_area_tagID[0] = 0x00;
    gpsi->gps_area_tagID[1] = 0x1c;
    gpsi->gps_area_type[0]  = 0x00;
    gpsi->gps_area_type[1]  = 0x07;
    gpsi->gps_area_count[0] = 0x00;
    gpsi->gps_area_count[1] = 0x00;
    gpsi->gps_area_count[2] = 0x00;
    gpsi->gps_area_count[3] = 0x00;
    gpsi->gps_area_value[0] = 0x00;
    gpsi->gps_area_value[1] = 0x00;
    gpsi->gps_area_value[2] = 0x00;
    gpsi->gps_area_value[3] = 0x00;

    gpsi->gps_dateStamp_tagID[0] = 0x00;
    gpsi->gps_dateStamp_tagID[1] = 0x1d;
    gpsi->gps_dateStamp_type[0]  = 0x00;
    gpsi->gps_dateStamp_type[1]  = 0x02;
    gpsi->gps_dateStamp_count[0] = 0x00;
    gpsi->gps_dateStamp_count[1] = 0x00;
    gpsi->gps_dateStamp_count[2] = 0x00;
    gpsi->gps_dateStamp_count[3] = 0x0b;
    gpsi->gps_dateStamp_value[0] = 0x00;
    gpsi->gps_dateStamp_value[1] = 0x00;
    gpsi->gps_dateStamp_value[2] = 0x00;
    gpsi->gps_dateStamp_value[3] = 0x00;

    gpsi->nextIFDOffset = 0x00;

    if(pMe->GPSInfo.dwLat < 0)
      latitude = pMe->GPSInfo.dwLat * (-1);
    else
      latitude = pMe->GPSInfo.dwLat;

    /* equivalent to (latitude*180) / (2 << (25-1)); */
    degrees = (latitude*45) / (2 << (23-1));

    gpsi->latitude[0] = (uint8)(((uint32)degrees >> 24) & 0xFF);
    gpsi->latitude[1] = (uint8)(((uint32)degrees >> 16) & 0xFF);
    gpsi->latitude[2] = (uint8)(((uint32)degrees >> 8) & 0xFF);
    gpsi->latitude[3] = (uint8)((uint32)degrees & 0xFF);
    gpsi->latitude[4] = 0x00;
    gpsi->latitude[5] = 0x00;
    gpsi->latitude[6] = 0x00;
    gpsi->latitude[7] = 0x01;
    gpsi->latitude[8] = 0x00;
    gpsi->latitude[9] = 0x00;
    gpsi->latitude[10] = 0x00;
    gpsi->latitude[11] = 0x00;
    gpsi->latitude[12] = 0x00;
    gpsi->latitude[13] = 0x00;
    gpsi->latitude[14] = 0x00;
    gpsi->latitude[15] = 0x01;
    gpsi->latitude[16] = 0x00;
    gpsi->latitude[17] = 0x00;
    gpsi->latitude[18] = 0x00;
    gpsi->latitude[19] = 0x00;
    gpsi->latitude[20] = 0x00;
    gpsi->latitude[21] = 0x00;
    gpsi->latitude[22] = 0x00;
    gpsi->latitude[23] = 0x64;


    if(pMe->GPSInfo.dwLon < 0)
     longitude = pMe->GPSInfo.dwLon * (-1);
    else
     longitude = pMe->GPSInfo.dwLon;

    /* equivalent to (longitude * 360) / (2 << (26-1)) */
    degrees = (longitude * 45) / (2 << (23-1));

    gpsi->longitude[0] = (uint8)(((uint32)degrees>> 24) & 0xFF);
    gpsi->longitude[1] = (uint8)(((uint32)degrees>> 16) & 0xFF);
    gpsi->longitude[2] = (uint8)(((uint32)degrees>> 8) & 0xFF);
    gpsi->longitude[3] = (uint8)((uint32)degrees & 0xFF);
    gpsi->longitude[4] = 0x00;
    gpsi->longitude[5] = 0x00;
    gpsi->longitude[6] = 0x00;
    gpsi->longitude[7] = 0x01;
    gpsi->longitude[8] = 0x00;
    gpsi->longitude[9] = 0x00;
    gpsi->longitude[10] = 0x00;
    gpsi->longitude[11] = 0x00;
    gpsi->longitude[12] = 0x00;
    gpsi->longitude[13] = 0x00;
    gpsi->longitude[14] = 0x00;
    gpsi->longitude[15] = 0x01;
    gpsi->longitude[16] = 0x00;
    gpsi->longitude[17] = 0x00;
    gpsi->longitude[18] = 0x00;
    gpsi->longitude[19] = 0x00;
    gpsi->longitude[20] = 0x00;
    gpsi->longitude[21] = 0x00;
    gpsi->longitude[22] = 0x00;
    gpsi->longitude[23] = 0x64;

    gpsi->altitude[0] = 0x00;
    gpsi->altitude[1] = 0x00;
    gpsi->altitude[2] = (uint8)((((uint16)(pMe->GPSInfo.wAltitude)) >> 8) & 0xFF);
    gpsi->altitude[3] = (uint8)(((uint16)(pMe->GPSInfo.wAltitude)) & 0xFF);
    gpsi->altitude[4] = 0x00;
    gpsi->altitude[5] = 0x00;
    gpsi->altitude[6] = 0x00;
    gpsi->altitude[7] = 0x01;

    gpsi->geodetic_survey_data_method[0] = 0x57;
    gpsi->geodetic_survey_data_method[1] = 0x47;
    gpsi->geodetic_survey_data_method[2] = 0x53;
    gpsi->geodetic_survey_data_method[3] = 0x2D;
    gpsi->geodetic_survey_data_method[4] = 0x38;
    gpsi->geodetic_survey_data_method[5] = 0x34;
    gpsi->geodetic_survey_data_method[6] = 0x00;
    gpsi->geodetic_survey_data_method[7] = 0x41;
    gpsi->geodetic_survey_data_method[8] = 0x53;
    gpsi->geodetic_survey_data_method[9] = 0x43;
    gpsi->geodetic_survey_data_method[10] = 0x49;
    gpsi->geodetic_survey_data_method[11] = 0x49;
    gpsi->geodetic_survey_data_method[12] = 0x00;
    gpsi->geodetic_survey_data_method[13] = 0x00;
    gpsi->geodetic_survey_data_method[14] = 0x00;
    gpsi->geodetic_survey_data_method[15] = 0x47;
    gpsi->geodetic_survey_data_method[16] = 0x50;
    gpsi->geodetic_survey_data_method[17] = 0x53;
    gpsi->geodetic_survey_data_method[18] = 0x2D;
    gpsi->geodetic_survey_data_method[19] = 0x46;
    gpsi->geodetic_survey_data_method[20] = 0x49;
    gpsi->geodetic_survey_data_method[21] = 0x58;

}

#ifdef FEATURE_BREW_IMOVIEEDIT
static void Qcamcorder_PostEdit(Qcamcorder *pMe)
{
  char *pData;
  AEEMediaData src;

  pData = (void *)Qcamcorder_GetVideoEditFileName(pMe);
  (void)STRLCPY(pMe->sVideoEditFileName,  pData, sizeof(pMe->sVideoEditFileName));

  if (!pMe->pIFileMgr) {
    (void)ISHELL_CreateInstance(
      pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pMe->pIFileMgr);
  }
  if (IFILEMGR_Test(pMe->pIFileMgr, pMe->sVideoEditFileName) == SUCCESS) {
    if (IFILEMGR_Remove(pMe->pIFileMgr, pMe->sVideoEditFileName) != SUCCESS) {
      QCAMCORDER_MSG_HIGH("COULD NOT DELETE THE VIDEO EDITING FILE",0,0,0);
      return;
    }
  }

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IMOVIEEDIT,
                            (void **)&(pMe->m_pInEdit)) !=  SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("PostEditing cannot create infile",0,0,0);
    return;
  }

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IMOVIEEDIT,
                            (void **)&(pMe->m_pOutEdit)) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VideoEditing cannot create outfile",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
    return;
  }

  src.clsData = MMD_FILE_NAME;
  src.pData = pMe->sCurrentFileName;
  CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_PostEditReadOpenCB, pMe);
  if (IMOVIEEDIT_ReadOpen(pMe->m_pInEdit, &src,
                           &(pMe->m_mei), &(pMe->m_movieeditStatus),
                           &(pMe->m_movieeditCB)) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VIdeoEditing cannot open input file",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
    return ;
  }
  pMe->bSaveInPrg = TRUE;
}

#elif defined(FEATURE_BREW_IMOVIEFILE)
static void Qcamcorder_PostEdit(Qcamcorder *pMe)
{
   IMovieFile * pInFile = NULL, * pOutFile = NULL;
   IFile * pSoundFile = NULL;
   char * uuid = NULL;
   gpsi_struct *gpsi = NULL;
   AEEMovieFileInfo mfi;
   AEEMovieSampleInfo  audioInfo, videoInfo;
   AEEMovieStreamInfo streams[3];
   uint16 i;
   uint16 videoStreamID=0, audioStreamID=1;
   int16 textStreamID=-1;
   boolean noAudio = TRUE;
   uint8 * pAudioBuf = NULL, *pVideoBuf = NULL;
   uint32 offset;
   double dbl_audio_time_conversion_factor = 0;
   uint32 NVidFrames=0,NAudFrames=0;   /* nr. of frames in a fragment */
   boolean bFirstAudFrameInFragment = TRUE;
   uint64 target_video_time =0;

   char *newfile= Qcamcorder_GetVideoEditFileName(pMe);
   char* tmpFile = NULL;
   uint32 numbytesread=0, textTimeScale=0, videoTimeScale=0;
   uint32 curoffset, lastoffset;
   mfi.cbSize = sizeof(AEEMovieFileInfo);
   // Create the two moviefile instances
   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE, (void **)&pInFile) !=
       SUCCESS)
      goto AUDIODUB_END;

   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE, (void **)&pOutFile) !=
       SUCCESS)
      goto AUDIODUB_END;


   // Open the input file for reading
   if (IMOVIEFILE_OpenMp4Read(pInFile, pMe->sCurrentFileName, &mfi) != SUCCESS)
      goto AUDIODUB_END;

   streams[0].cbSize = 0;
   streams[1].cbSize = 0;
   streams[2].cbSize = 0;

   // Copy the video track over
   for (i = 0; i < mfi.nStreams; ++i)
   {
      if (mfi.pStreams[i].format == AEECLSID_MPEG4_VIDEO)
      {  videoStreamID = i;
         (void)MEMCPY(&streams[0], &mfi.pStreams[i], sizeof(AEEMovieStreamInfo));
         videoTimeScale = mfi.pStreams[i].nTimescale;
      }
      else if ( AEECLSID_MEDIAQCP == mfi.pStreams[i].format )
      { audioStreamID = i;
        (void)MEMCPY(&streams[1], &mfi.pStreams[i], sizeof(AEEMovieStreamInfo));
      }
      else if ( AEECLSID_3GPP_TEXT == mfi.pStreams[i].format )
      { textStreamID = i;
        (void)MEMCPY(&streams[2], &mfi.pStreams[i], sizeof(AEEMovieStreamInfo));
        textTimeScale = mfi.pStreams[i].nTimescale;
      }
   }


   if(!videoTimeScale){
      QCAMCORDER_MSG_ERROR("Error in PostEditing. VideoTimeScale == 0",0,0,0);
      (void)IMOVIEFILE_Release(pInFile);
      (void)IMOVIEFILE_Release(pOutFile);
      goto AUDIODUB_END;
   }
   if(streams[1].cbSize != 0 )  /* if audio stream exists */
   {
     dbl_audio_time_conversion_factor =
       ((double) mfi.pStreams[audioStreamID].nTimescale)/
         ((double) mfi.pStreams[videoStreamID].nTimescale);
     noAudio = FALSE;
   }

   if(STRSTR(newfile, AEEFS_MMC_DIR) || STRSTR(newfile, "mmc"))
   {
     tmpFile =  QCAMCORDER_TMP_VIDEO_MMC_FILE;
   }
#ifdef FEATURE_EFS_FTL
   else if(STRSTR(newfile, AEEFS_FTL_DIR) || STRSTR(newfile, "ftl"))
   {
     tmpFile =  QCAMCORDER_TMP_VIDEO_FTL_FILE;
   }
#endif /* FEATURE_EFS_FTL */

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
   else if(STRSTR(newfile, AEEFS_USB_DIR) || STRSTR(newfile, "usb"))
   {
     tmpFile =  QCAMCORDER_TMP_VIDEO_USB_FILE;
   }
#endif /* FEATURE_EFS_HOTPLUG_USB_HOST_MS */
   else
   {
     tmpFile =  QCAMCORDER_TMP_VIDEO_FILE;
   }

   if (IMOVIEFILE_OpenMp4Write(pOutFile, tmpFile, &mfi) != SUCCESS)
      goto AUDIODUB_END;

   //status = IMOVIEFILE_WriteUserAtom(pOutFile, atom, size);

   if(pMe->uuid_data != NULL)
   {uuid = (char *) MALLOC((pMe->uuid_data->dwBytes +  pMe->uuid->dwBytes)* sizeof(char));
    (void)MEMCPY(uuid, pMe->uuid->pData, pMe->uuid->dwBytes);
    (void)MEMCPY(uuid + pMe->uuid->dwBytes, pMe->uuid_data->pData, pMe->uuid_data->dwBytes);
    (void)IMOVIEFILE_WriteUserAtom(pOutFile, uuid, pMe->uuid_data->dwBytes + pMe->uuid->dwBytes);
   }
   if (pMe->eGPSState == QCAMCORDER_GPS_DATA_VALID)
   //ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_GPSINFO, (int)&pMe->GPSInfo, 0);

   {uuid = (char*) MALLOC(sizeof(UUID_GPSI) + sizeof(gpsi_struct));
    gpsi = MALLOC(sizeof(gpsi_struct)  + sizeof(uint8));
    Qcamcorder_FillGPSData(pMe, gpsi);
    (void)MEMCPY(uuid, UUID_GPSI, sizeof(UUID_GPSI));
    (void)MEMCPY(uuid + sizeof(UUID_GPSI), gpsi, sizeof(gpsi_struct));
    (void)IMOVIEFILE_WriteUserAtom(pOutFile, uuid, sizeof(UUID_GPSI) + sizeof(gpsi_struct)-2);
   }

   // Copy the video stream of the input file to the output file while copying

  // audio data from the audio file at a similar rate
  videoInfo.cbSize = sizeof(AEEMovieSampleInfo);
  audioInfo.cbSize = sizeof(AEEMovieSampleInfo);
  audioInfo.dwTime = 0;  /* initial setting before first audio read */
  lastoffset       = 0;

  for (offset=0;
      NULL !=  (pVideoBuf = (uint8 *)
                IMOVIEFILE_GetSampleStreamN(pInFile,videoStreamID,1,offset,
                                           &numbytesread,&videoInfo,&NVidFrames)
               );
      offset++) {
    (void)IMOVIEFILE_WriteStream(pOutFile, 0, 1, &videoInfo, pVideoBuf);
    FREE(pVideoBuf);
    pVideoBuf = NULL;
    target_video_time = (videoInfo.dwTime +(videoTimeScale /2))  / videoTimeScale ;
    if (!noAudio ) {
      curoffset = (uint32)(videoInfo.dwTime * dbl_audio_time_conversion_factor);

      while (
             /* we only want frames that are in the same temporal context as the
             ** vudeio frame we just wrote
             */
              (audioInfo.dwTime < curoffset  ||
             /* But if we're at the last video frame of the fragment then we want to
             ** flush all remain audio frame in the same framgment
             */
               (NVidFrames == offset))
              /* We cannot pass the last audio frame in the current fragment. However
              ** the lastoffset<NAudFrames test will fail if it is the first frame
              ** because in that case NAudFrames is not even set
              */
               && (bFirstAudFrameInFragment || lastoffset < NAudFrames ) &&
             NULL != (pAudioBuf = (uint8 *)
                      IMOVIEFILE_GetSampleStreamN(pInFile,audioStreamID,1,
                                                 lastoffset,
                                                 &numbytesread,&audioInfo,
                                                 &NAudFrames)
                     )
            ) {
        (void)IMOVIEFILE_WriteStream(pOutFile, 1, 1, &audioInfo, pAudioBuf);

        FREE(pAudioBuf);
        pAudioBuf = NULL;
        bFirstAudFrameInFragment = FALSE;

        lastoffset++;
      }
    }
    if(offset == NVidFrames )
    { /* the next read will start a new segment */
      bFirstAudFrameInFragment = TRUE;
    }
  }

  /* If timed text stream is present, copy text samples and
  ** description atoms for cut region*/
  if(textStreamID >= 0)
  {
    uint32 nTextSample = 0, nTextSize = 0, nDescSize = 0;
    uint8 *pTextBuffer = NULL, *pDescBuffer = NULL;
    AEEMovieSampleInfo textInfo;
    uint32 textend=0;
    uint32 runningtime = 0, delta = 0, current_desc = 0;
    uint32 cycle=1;  /*
                     ** 1 = writing out frames
                     */


    /* Rewind it again before we process timed text */
    IMOVIEFILE_Close(pInFile);
    (void)IMOVIEFILE_OpenMp4Read(pInFile, pMe->sCurrentFileName, &mfi);

    textInfo.cbSize = sizeof(textInfo);
    textend =  target_video_time  * textTimeScale;

    /* Find first text sample whose end time > starting marker */
    for (nTextSample=0;
         NULL!=(pTextBuffer = (uint8 *)
                 IMOVIEFILE_GetSampleStream(pInFile,textStreamID,1,
                                   nTextSample,&nTextSize,&textInfo)
               );
         nTextSample++
    )
    {
        delta=textInfo.dwDelta;

        /* Searching for first frame to include and found it */

        if (cycle==1) /* Write frames */
        {
            /* dwSegment is 0-based; current_desc is 1-based */
            if ((textInfo.dwSegment+1)>current_desc)
            {
                /* read and write description tx3g atoms */
                if (IMOVIEFILE_ReadSegmentHeader(pInFile, textStreamID,
                          textInfo.dwSegment, NULL, &nDescSize)!=SUCCESS)
                {
                    QCAMCORDER_MSG_ERROR("Fail:reading desc atom size",
                                          textInfo.dwSegment,0,0);
                    break;
                }

                if (pDescBuffer)
                    pDescBuffer = REALLOC(pDescBuffer, nDescSize);
                else
                    pDescBuffer = MALLOC(nDescSize);

                if (pDescBuffer == NULL)
                {
                    QCAMCORDER_MSG_ERROR ("descbuffer malloc fail",0,0,0);
                    break;
                }

                if (IMOVIEFILE_ReadSegmentHeader(pInFile, textStreamID,
                                        textInfo.dwSegment, pDescBuffer,
                                        &nDescSize)!=SUCCESS)
                {
                    QCAMCORDER_MSG_ERROR("Fail:reading desc atom",
                                         textInfo.dwSegment,0,0);
                    break;
                }

                if (IMOVIEFILE_WriteSegmentHeader(pOutFile,textStreamID,
                              pDescBuffer,nDescSize)
                                  !=SUCCESS)
                {
                    QCAMCORDER_MSG_ERROR("Fail: writing desc atom",
                                         0,0,0);
                    break;
                }
                /* keep track of current description atom written */
                current_desc=textInfo.dwSegment+1;

            }
            /* If we have reached the end marker,
            ** no need to write more frames after this one*/
            if (textInfo.dwTime+textInfo.dwDelta >= textend)
            {
                delta=textend-textInfo.dwTime;
                cycle=2;
            }
            /* Write text sample */
            textInfo.dwTime = runningtime;
            textInfo.dwDelta = delta;
            runningtime += delta;
            (void)IMOVIEFILE_WriteStream(pOutFile,textStreamID,1,&textInfo,
                                   pTextBuffer);
        }

        if (cycle==2) /* exit */
        {
            break;
        }

        if(pTextBuffer) /* free text sample */
        {
            FREE(pTextBuffer);
            pTextBuffer = NULL;
        }
    }

    /* free text and description buffers */
    if(pTextBuffer)
    {
        FREE(pTextBuffer);
        pTextBuffer = NULL;
    }
    if (pDescBuffer)
    {
       FREE(pDescBuffer);
       pDescBuffer=NULL;
    }
  }

AUDIODUB_END:

   // Release the two moviefiles
   if (pInFile){
      (void)IMOVIEFILE_Release(pInFile);
      pInFile = NULL;
   }

   if (pOutFile)
      {(void)IMOVIEFILE_Release(pOutFile);
      pOutFile = NULL;
   }

   if (pSoundFile){
      (void)IFILE_Release(pSoundFile);
     pSoundFile = NULL;
   }

   if(IFILEMGR_Test(pMe->pIFileMgr,newfile) == SUCCESS)
   {
     (void)IFILEMGR_Remove(pMe->pIFileMgr, newfile);
   }

   (void)IFILEMGR_Rename(pMe->pIFileMgr, tmpFile, newfile);
   (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_AUDIO_FILE);

   Qcamcorder_BuildEditMenu(pMe);
}
#endif /* FEATURE_BREW_IMOVIEEDIT */

#ifdef FEATURE_APP_MMS
#error code not present
#endif /* FEATURE_APP_MMS    */


#ifdef FEATURE_APP_MMS
#error code not present
#endif /* FEATURE_APP_MMS */

/* ==========================================================================
FUNCTION /
DESCRIPTION Handles any messages while the edit screen is active
============================================================================= */
static uint8 Qcamcorder_EditKeyHandler(Qcamcorder* pMe, AEEEvent eCode, uint16 key)
{ uint16 selection;
  int32 index;
  int count;

   switch(key)
   {

   case AVK_STAR:
      if(eCode == EVT_KEY)
      {
        //Toggle feature only apply when the menu is overlay on image.
        QCamcorder_ToggleMenuDisplay(pMe,QCAMCORDER_MENU_EDIT);
      }
    break;

   case AVK_LEFT:
   case AVK_RIGHT:
      if (eCode == EVT_KEY)
      {
         selection = IMENUCTL_GetSel(pMe->m_pISoftMenu);
         (void)IMENUCTL_GetItemData(pMe->m_pISoftMenu, selection, (uint32 *)&index);
         count = IMENUCTL_GetItemCount(pMe->m_pISoftMenu);

         if (index < 0)
         {
           index = count - 1;
         }
         else if (index >= count)
          {
            index = 0;
          }

         selection = IMENUCTL_GetItemID(pMe->m_pISoftMenu, index);

         if (pMe->eState == QCAMCORDER_EDIT)
         {
            // Change display
            Qcamcorder_DisplayEditMenuPic(pMe);
         }
         // Display titlebar
         if(pMe->eState != QCAMCORDER_PLAYING &&
            pMe->eState != QCAMCORDER_PAUSED &&
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
            pMe->eState != QCAMCORDER_CUT_MERGE &&
#endif
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
            pMe->eState != QCAMCORDER_VIDEO_EDIT_SELECTION &&
#endif
            pMe->eState != QCAMCORDER_PROPERTIES
#ifdef FEATURE_MP4_TRANSCODER
            && pMe->eState != QCAMCORDER_TRANSPORTION
#endif
            )
         {
          Qcamcorder_DisplayEditMenuName(pMe);
         }

         if(selection == IDS_POST_EDIT)
         {
           pMe->eState = QCAMCORDER_POSTEDIT;
           Qcamcorder_BuildPostEditMenu(pMe);
         }
         else
         {
          if(pMe->eState == QCAMCORDER_EDIT){
            //lint -e570  Suppress complaint on loss of sign
            IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
            //lint +e570
            Qcamcorder_DisplayEditMenuName(pMe);
            pMe->m_activeMenu = QCAMCORDER_MENU_FILELIST;
            (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
           }

         }
         return TRUE;
      }
      break;
   case AVK_UP:
   case AVK_DOWN:
      return TRUE;
   case AVK_CLR:
      if (eCode == EVT_KEY)
   	  {
        if ((pMe->eState == QCAMCORDER_EDIT ||
           pMe->eState == QCAMCORDER_POSTEDIT) &&
           pMe->m_activeMenu != QCAMCORDER_MENU_GPS)
        {
          pMe->b_focusFirstItem = FALSE;
          Qcamcorder_DisplayFileList(pMe);
          return TRUE;
        }
        else if (pMe->eState == QCAMCORDER_GPS_POSTEDIT)
        {
          pMe->eState = QCAMCORDER_POSTEDIT;
          if (pMe->eGPSState == QCAMCORDER_GPS_ACQUIRING)
          {
            pMe->eGPSState = QCAMCORDER_GPS_DISABLED;
            if (pMe->pIPosDet)
            {
              (void)IPOSDET_Release(pMe->pIPosDet);
              pMe->pIPosDet = NULL;
            }
          }
          Qcamcorder_BuildPostEditMenu(pMe);
          return TRUE;
        }
        else if(pMe->eState == QCAMCORDER_PROPERTIES)
        {
          pMe->eState = QCAMCORDER_EDIT;
          //lint -e570  Suppress complaint on loss of sign
          IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
          //lint +e570
          Qcamcorder_BuildEditMenu(pMe);
          return TRUE;
        }
#ifdef FEATURE_VIDEO_ENCODE_RAW
        else if (pMe->eState == QCAMCORDER_START_RAW_ENCODE)
        {
          Qcamcorder_RawEncoderDisplayFileList(pMe);
          return TRUE;
        }
#endif /* FEATURE_VIDEO_ENCODE_RAW */

        return TRUE;      
      }
      break;
   case AVK_SELECT:
      if (eCode == EVT_KEY)
      { 
         if (pMe->m_activeMenu == QCAMCORDER_MENU_GPS)
         {
           Qcamcorder_ExecuteGPSMenu(pMe, TRUE);
          return TRUE;
         }
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
         if(pMe->eState == QCAMCORDER_TRANSCODE && 
            pMe->bTranscoderStoping)
         {
             //ignore the key event
             return TRUE;
         }
#endif
         switch (IMENUCTL_GetSel(pMe->m_pISoftMenu))
         {
         case IDS_PLAY:
#ifdef FEATURE_VIDEO_AUDIO_DUBBING		 
            if (pMe->AudioDubbing)
            {
              QCAMCORDER_MSG_HIGH("Key Press too fast, ignoring in EditKeyHandler audio dubbing" ,0,0,0);
              return TRUE;
            }
#endif
            pMe->m_dwPlayPos = 0;
            pMe->m_bPlayAll = FALSE;
            pMe->m_bWait_to_Finish = TRUE;
            if(pMe->eState != QCAMCORDER_PLAYING)
            {
              Qcamcorder_PlayMovie(pMe);
            }
        /* girishm(09/01/2005): Relocated this Build Menu to inside Qcamcorder_PlayMediaNotify
         * So that this menu is built only after Qtv returns MM_MP4_OPEN_COMPLETE
         */
            //Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);
            return TRUE;
         case IDS_PLAY_ALL:
#ifdef FEATURE_VIDEO_AUDIO_DUBBING		 
            if (pMe->AudioDubbing)
            {
              QCAMCORDER_MSG_HIGH("Key Press too fast, ignoring in EditKeyHandler audio dubbing" ,0,0,0);
              return TRUE;
            }
#endif			
            (void)STRLCPY(pMe->sCurrentFileName,"",sizeof(pMe->sCurrentFileName));
            (void)Qcamcorder_PlayAllGetNextFile(pMe);
            pMe->m_dwPlayPos = 0;
            pMe->m_bPlayAll = TRUE;
            pMe->m_bWait_to_Finish = TRUE;
            Qcamcorder_PlayMovie(pMe);
            Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);
            return TRUE;
#ifdef FEATURE_VIDEO_ENCODE_RAW
         case IDS_RAW_RECORD:
         {
            boolean bRetVal;
            bRetVal = Qcamcorder_RawEncoderInit(pMe);
            if(bRetVal != FALSE)
            {
              Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);
            }
            return TRUE;
         }
         case IDS_RAW_STOP:
            Qcamcorder_RawEncoderStop(pMe);
            return TRUE;
#endif /* FEATURE_VIDEO_ENCODE_RAW */
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
         case IDS_RECORD:
            if (pMe->eState != QCAMCORDER_EDIT)
            {
               QCAMCORDER_MSG_HIGH("Cannot start audio dubbing.." ,0,0,0);
               return FALSE;
            }
            if (pMe->AudioDubbing)
            {
              QCAMCORDER_MSG_HIGH("Key Press too fast, ignoring in EditKeyHandler audio dubbing" ,0,0,0);
              return TRUE;
            }
            pMe->m_dwPlayPos = 0;
            if(Qcamcorder_CheckSpace(pMe))
            { 
              pMe->eState = QCAMCORDER_AUDIODUB;
              Qcamcorder_RecordAudio(pMe);
              Qcamcorder_BuildEditMenu(pMe);
            }
            else
            {Qcamcorder_DisplayPostRecordingMessage(pMe);
            }
            return TRUE;
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
         case IDS_START_EDITING:

             pMe->eState = QCAMCORDER_VIDEO_EDIT_SELECTION;
             if(pMe->m_bEditHit)
             {
                 /* stop video editing selection */
                 pMe->m_bEditHit = FALSE;
                 QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
             }
             else
             {
                 if((pMe->m_dwPlayPos*1000) > pMe->m_dwTotalTime)
                 {
                     pMe->eState = QCAMCORDER_PLAYING;
                     QCAMCORDER_MSG_HIGH("Video Editing Selection Started Too Late. ",0,0,0);
                 }
                 else
                 {
                    /* start video editing selection */
                    pMe->m_bEditHit = TRUE;
                    pMe->m_dStartPos = pMe->m_dwPlayPos;
                    Qcamcorder_BuildEditMenu(pMe);
                    QCAMCORDER_MSG_HIGH("Start Video Editing Selection",0,0,0);
                 }
             }
          break;
#endif
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
          case IDS_CUT_MERGE:
            if (pMe->eState != QCAMCORDER_PLAYING && !pMe->m_bEditHit)
            {
                QCAMCORDER_MSG_HIGH("Cut & Merge can only start while playing clip!!",0,0,0); 
                return TRUE;
            }
            pMe->eState = QCAMCORDER_CUT_MERGE;
            if(pMe->m_bEditHit)
            {
              pMe->m_bEditHit = FALSE;
              pMe->m_dSecondMarker = pMe->m_dwPlayPos;
              QCAMCORDER_MSG_HIGH("Stop Cut HIT",0,0,0);
              Qcamcorder_BuildEditMenu(pMe);
            }
            else{
              pMe->m_bEditHit = TRUE;
              pMe->m_dFirstMarker = pMe->m_dwPlayPos;
              pMe->m_dSecondMarker = pMe->m_dwPlayPos;
              pMe->m_dStartPos = pMe->m_dwPlayPos;
              QCAMCORDER_MSG_HIGH("START Cut HIT",0,0,0);
              Qcamcorder_BuildEditMenu(pMe);
            }

            break;
#endif
#ifdef FEATURE_MP4_TRANSCODER
          case IDS_TRANSPORTION://Transcode portion of clip
            if (pMe->eState != QCAMCORDER_PLAYING && !pMe->m_bEditHit)
            {
                QCAMCORDER_MSG_HIGH("Cut & Merge can only start while playing clip!!",0,0,0); 
                return TRUE;
            }
             pMe->eState = QCAMCORDER_TRANSPORTION;
            //set this boolean which can be used to delete Intermediate file
            pMe->bTranscode_portion = TRUE;

            if(pMe->m_bEditHit)
             {
                 /* stop video editing selection */
                 pMe->m_bEditHit = FALSE;
                 QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
             }
             else
             {
                 if((pMe->m_dwPlayPos*1000) > pMe->m_dwTotalTime)
                 {
                     pMe->eState = QCAMCORDER_PLAYING;
                     QCAMCORDER_MSG_HIGH("Video Editing Selection for Transcoding Started Too Late. ",0,0,0);
                 }
                 else
                 {
                    /* start video editing selection */
                    pMe->m_bEditHit = TRUE;
                    pMe->m_dStartPos = pMe->m_dwPlayPos;
                    Qcamcorder_BuildEditMenu(pMe);
                    QCAMCORDER_MSG_HIGH("Start Video Editing Selection for transcoding ",0,0,0);
                 }
             }

            break;
#endif

         case IDS_STOP_EDITING:
          if (pMe->eState == QCAMCORDER_PLAYING)
          { QCAMCORDER_MSG_HIGH("Stop Editing HIT",0,0,0);
          }
          break;
         case IDS_PAUSE:
            if (pMe->eState == QCAMCORDER_PLAYING)
            {
               pMe->eState = QCAMCORDER_PAUSED;
               // IDISPLAY_ClearScreen() in Qcamcorder_BuildSoftMenu() erases the progress bar
               //instead call Qcamcorder_BuildEditMenu()
               Qcamcorder_BuildEditMenu(pMe);
               QCAMCORDER_IMEDIA_PAUSE(pMe->pMedia);
            }
            else if (pMe->eState == QCAMCORDER_PAUSED)
            {
               pMe->eState = QCAMCORDER_PLAYING;
               Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);
               QCAMCORDER_IMEDIA_RESUME(pMe->pMedia);
            }
            return TRUE;
         case IDS_STOP:
            if (pMe->eState == QCAMCORDER_PLAYING ||
                pMe->eState == QCAMCORDER_PAUSED
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
                || pMe->eState == QCAMCORDER_CUT_MERGE
#endif
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
                || pMe->eState == QCAMCORDER_VIDEO_EDIT_SELECTION
#endif
#ifdef FEATURE_MP4_TRANSCODER
                || pMe->eState == QCAMCORDER_TRANSPORTION
#endif
                )
            {
              if (pMe->m_bWait_to_Finish)
              {
                QCAMCORDER_MSG_HIGH("Too fast key press for stopping..",0,0,0); 
                return TRUE;
              }
               // Stop playing
              pMe->m_bPlayAll = FALSE;
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
              if(pMe->eState == QCAMCORDER_CUT_MERGE)
                {pMe->eState = QCAMCORDER_PLAYING;
                }
#endif
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
              if(pMe->eState == QCAMCORDER_VIDEO_EDIT_SELECTION)
                {pMe->eState = QCAMCORDER_PLAYING;
                }
#endif
#ifdef FEATURE_MP4_TRANSCODER
              if(pMe->eState == QCAMCORDER_TRANSPORTION)
                {
                  pMe->eState = QCAMCORDER_PLAYING;
                }
#endif
              QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
            }
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
            else if (pMe->eState == QCAMCORDER_AUDIODUB)
            {
               // Stop playing/recording and do audio dubbing
               pMe->eState = QCAMCORDER_AUDIODUB_ABORT;
               pMe->AudioDubbing = FALSE;
               QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
               QCAMCORDER_IMEDIA_STOP(pMe->pRecordMedia);
            }
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */
#ifdef FEATURE_MP4_TRANSCODER
            else if(pMe->eState == QCAMCORDER_TRANSCODE)
            {
              //lint -e570  Suppress complaint on loss of sign
              IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
              //lint +e570
              IDISPLAY_Update(pMe->a.m_pIDisplay);
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
              Qcamcorder_transcoderAbort(pMe); 
#else
              pMe->eState = QCAMCORDER_EDIT;
              Qcamcorder_Transcoder_End(pMe);
#endif//FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
              
            }
#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif //#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#endif //FEATURE_MP4_TRANSCODER
            Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);
            return TRUE;
         case IDS_REVERT:
            // Delete the temporary file
            (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_MMC_FILE);
#ifdef FEATURE_EFS_FTL
            (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_FTL_FILE);
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
            (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_USB_FILE);
#endif
            (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_FILE);
            return TRUE;
#if defined(FEATURE_BREW_IMOVIEEDIT) || defined(FEATURE_BREW_IMOVIEFILE)
         case IDS_SAVE:
            {
              QCAMCORDER_MSG_HIGH("Save File Executed",0,0,0);
              if(Qcamcorder_CheckSpace(pMe))
              {
                if(!pMe->bSaveInPrg)
                  Qcamcorder_PostEdit(pMe);
              }
              else
              {
                Qcamcorder_DisplayPostRecordingMessage(pMe);
                pMe->bSaveInPrg = FALSE;
              }
            }
            return TRUE;
#endif //defined(FEATURE_BREW_IMOVIEEDIT) || defined(FEATURE_BREW_IMOVIEFILE)
         case IDS_PROPERTIES:
             pMe->eState = QCAMCORDER_PROPERTIES;

            if(Qcamcorder_ShowProperties(pMe) == FALSE)
            {
               pMe->msg = IDB_ERROR_QCAMCORDER;
               Qcamcorder_ErrorEventUserNotify(pMe);
            }
            return TRUE;

         case IDS_QCAMCORDER_DELETE:
            {
            }
            return TRUE;
#ifdef FEATURE_DVS
         case IDS_DVS:
           pMe->bDVS = TRUE;
           pMe->m_dwPlayPos = 0;
           if(Qcamcorder_CheckSpace(pMe))
           { pMe->eState = QCAMCORDER_TRANSCODE;
             Qcamcorder_Transcode(pMe);
           }
           else
           {Qcamcorder_DisplayPostRecordingMessage(pMe);
           }
           return TRUE;
#endif
         case IDS_TRANSCODE:
#ifdef FEATURE_MP4_TRANSCODER
          if (pMe->eState != QCAMCORDER_EDIT)
          {
            QCAMCORDER_MSG_HIGH("Transcode can start from EDIT menu only",0,0,0);
            return TRUE;
          }
#ifdef FEATURE_DVS
          pMe->bDVS = FALSE;
#endif
          pMe->m_dwPlayPos = 0;
          if(Qcamcorder_CheckSpace(pMe))
          { pMe->eState = QCAMCORDER_TRANSCODE;
            Qcamcorder_Transcode(pMe);
          }
          else
          {Qcamcorder_DisplayPostRecordingMessage(pMe);
          }

#endif
          return TRUE;
         case IDS_SEND:
#ifdef FEATURE_APP_MMS
#error code not present
#endif /* FEATURE_APP_MMS */
          return TRUE;

#if defined(FEATURE_BREW_IMOVIEEDIT) || defined(FEATURE_BREW_IMOVIEFILE)
           case IDS_POST_EDIT:
             //pMe->eState = QCAMCORDER_POSTEDIT;
             Qcamcorder_ExecutePostEditMenu(pMe);
             //Qcamcorder_BuildPostEditMenu(pMe);
            break;
#endif //defined(FEATURE_BREW_IMOVIEEDIT) || defined(FEATURE_BREW_IMOVIEFILE)

#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif

         default:
           QCAMCORDER_MSG_HIGH("Wrong item selected, case not handled",0,0,0);
          return FALSE;
         }
      }
      break;
     default:
       QCAMCORDER_MSG_HIGH("Wrong key pressed, case not handled",0,0,0);
     break;

   }

   return 2;
}

/* Draw Busy image */
void Qcamcorder_DrawBusyImage(Qcamcorder *pMe)
{
  IImage *pIImage;

  if (!pMe) 
    return;

  /* Load hourglass */
  pIImage = ISHELL_LoadBusyImage(pMe->a.m_pIShell);

  /* Draw hourglass */
  if (pIImage)
  {
    IIMAGE_Draw(pIImage, pMe->di.cxScreen/2, pMe->di.cyScreen/2);
	(void)IIMAGE_Release(pIImage);
    pIImage = NULL;
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
  }
}

static void Qcamcorder_ExecuteGPSMenu(Qcamcorder *pMe, boolean PostEdit)
{
  IMenuCtl *curMenu;
  uint16 selection;

  if(PostEdit)
  {
    curMenu = pMe->m_pIEditMenu;
  }
  else
  {
    curMenu = pMe->m_pIMenu;
  }
  selection = IMENUCTL_GetSel(curMenu);

  if (pMe->pIPosDet)
  {
    CALLBACK_Cancel(&pMe->GPSInfoCB);
    (void)IPOSDET_Release(pMe->pIPosDet);
    pMe->pIPosDet = NULL;
  }

  /* Close the GPS menu first
  ** IPOSDET_GetGPSInfo could create privacy dialog
  ** depending on the system setting. Therefore,
  ** any draw operation after calling the function
  ** could cause undesired result. The best way is
  ** to put up a dialog that could redraw itself
  ** before calling the function.
  ** Since the GPS operation takes time, it is
  ** recommended to put up a tracking bar like
  ** "GPS in progress" during the GPS operations
  ** [which is not done here] */

  pMe->m_activeMenu = QCAMCORDER_MENU_NOMENU;

  IMENUCTL_Reset(curMenu);
  (void)IMENUCTL_DeleteAll(curMenu);

  switch (selection)
  {
    case IDS_GPS_DISABLE:
      Qcamcorder_UpdateGPSIcon(pMe);	 
      pMe->eGPSState = QCAMCORDER_GPS_DISABLED;
      if (PostEdit)
      {
        Qcamcorder_BuildPostEditMenu(pMe);
      }
      else
      {
        (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
      }
      break;

    case IDS_GPS_ENABLE_NEW:
      pMe->eGPSState = QCAMCORDER_GPS_ENABLED;
      (void)MEMSET(&pMe->GPSInfo, 0, sizeof(AEEGPSInfo));
      Qcamcorder_GetGPSInfo(pMe);
      break;

    case IDS_GPS_ENABLE_PREV:
      pMe->eGPSState = QCAMCORDER_GPS_DATA_VALID;
      pMe->GPSInfo.dwTimeStamp = (GETUPTIMEMS()/1000);
      pMe->GPSInfo.dwLat = DEFAULT_LATITUDE;
      pMe->GPSInfo.dwLon = DEFAULT_LONGITUDE;
      pMe->GPSInfo.wAltitude = DEFAULT_ALTITUDE;
      pMe->GPSInfo.fValid = TRUE;
      if (PostEdit)
      {
        Qcamcorder_BuildPostEditMenu(pMe);
      }
      else
      {
        (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
      }
      break;

    default:
	  pMe->eGPSState = QCAMCORDER_GPS_DISABLED;
      Qcamcorder_UpdateGPSIcon(pMe);
      QCAMCORDER_MSG_HIGH("Case %d selection not handled", selection,0,0);
      break;
  }
}
/* ==========================================================================
FUNCTION QCAMCORDER_PREVIEWKEYHANDLER
DESCRIPTION
============================================================================= */
static uint8 Qcamcorder_PreviewKeyHandler(Qcamcorder* pMe, AEEEvent eCode, uint16 key)
{
   switch(key)
   {
   case AVK_STAR:
      if(eCode == EVT_KEY)
      {
        //Toggle feature only apply when the menu is overlay on image.
        QCamcorder_ToggleMenuDisplay(pMe,QCAMCORDER_MENU_PREVIEW);
      }
    break;

   case AVK_LEFT:
   case AVK_RIGHT:
      if (eCode == EVT_KEY)
      {
         if (pMe->m_activeMenu == QCAMCORDER_MENU_GPS)
         {
            return TRUE;
         }
         Qcamcorder_DisplayPreviewMenuName(pMe, (key == AVK_LEFT ? -1 : 1));
      }
      break;
   case AVK_SELECT:
      if (eCode == EVT_KEY)
      {
         if (pMe->m_activeMenu == QCAMCORDER_MENU_GPS)
         {
           Qcamcorder_ExecuteGPSMenu(pMe, FALSE);
           return TRUE;
         }
         else
         {  uint32 dwFreeSpace = 0;
            uint16 selection;

            int mSize =0;
            selection = IMENUCTL_GetSel(pMe->m_pIPreview);
            switch (selection)
            {
            case IDS_START:
               mSize = Qcamcorder_GetMovieSize(pMe);
               QCAMCORDER_MSG_MED("MOVIE SIZE IS %u", mSize,0,0);

#if MIN_BREW_VERSIONEx(3,1,3)
               {
                 const char *path;
                 switch(pMe->saveLoc)
                 {
                     case QCAMCORDER_SAVE_FLASH:
                       path = AEEFS_ROOT_DIR;
                       break;
                     case QCAMCORDER_SAVE_MMC :
                       path = AEEFS_CARD0_DIR;
                       break;
#ifdef FEATURE_EFS_FTL
                     case QCAMCORDER_SAVE_FTL :
                       path = AEEFS_MOD_FTL_DIR;
                       break;
#endif  /* FEATURE_EFS_FTL */
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
                     case QCAMCORDER_SAVE_USB :
                       path = AEEFS_MOD_USB_DIR;
                       break;
#endif  /* FEATURE_EFS_HOTPLUG_USB_HOST_MS */
                     default:
                       path = AEEFS_ROOT_DIR;
                 }
                 // With BREW 3.1.3+ we can now determine the remaining space on
                 // the MMC as well as the EFS
                 
                 if (IFILEMGR_Test(pMe->pIFileMgr, path) != SUCCESS)
                 {
                   path = AEEFS_ROOT_DIR;
                   pMe->saveLoc = QCAMCORDER_SAVE_FLASH;
                 }

                 if(pMe->saveLoc == QCAMCORDER_SAVE_MMC)
                 {
                   AEEFSFreeSpace FSSpace;
                   int pnSize;
                   pnSize = sizeof(AEEFSFreeSpace);
                   if(SUCCESS != ISHELL_GetDeviceInfoEx (pMe->a.m_pIShell,
                                                          AEE_DEVICEITEM_FSSPACE_CARD0 ,
                                                          (void *)&FSSpace,
                                                          &pnSize))
                   {
                     FSSpace.qwFreeSpace = 0;
                   }
                   else
                   {
                     dwFreeSpace = (uint32) (FSSpace.qwFreeSpace>>10);
                   }
                 }
                 else /* For ftm,efsand usb.*/
                 { 
                   if(SUCCESS != IFILEMGR_GetFreeSpaceEx(pMe->pIFileMgr,
                                             path,
                                             NULL,
                                             &dwFreeSpace))
                   {
                     dwFreeSpace = 0;
                   }
                   else
                   {
                     dwFreeSpace /= 1024;
                   }
                 }
               }
#else
               dwFreeSpace = IFILEMGR_GetFreeSpace(pMe->pIFileMgr, NULL) / 1024;
#endif /* MIN_BREW_VERSIONEx(3,1,3) */

               QCAMCORDER_MSG_MED("DWFREESPACE IS %u" ,dwFreeSpace,0,0);

               /* Check to see if our transition to preview is permitted.  If
                  not, then we're not done stopping record so we should ignore
                  the keypress */
               if ((pMe->m_bRecord_to_Preview) || (pMe->m_bPreview_to_Record))
               {
                  QCAMCORDER_MSG_HIGH("Key Press too fast, ignoring Keypress." ,0,0,0);
               }
               else if(dwFreeSpace > QCAMCORDER_SPACE_THRESHOLD)
               {
                 Qcamcorder_StartRecording(pMe);
               }
               else
               {
                 if(pMe->m_bCont)
                 {
                   pMe->bStopRecording = TRUE; /* Stop continuous mode */
                 }
                 Qcamcorder_DisplayWarningMessage(pMe, IDB_STOPPED);
               }
               break;
            case IDS_STOP:
              if ((pMe->m_bPreview_to_Record) || (pMe->m_bRecord_to_Preview))
               {
                  QCAMCORDER_MSG_HIGH("Key Press too fast, ignorning Keypress." ,0,0,0);
               }
               else
               {
                 pMe->bStopRecording = TRUE;
                 Qcamcorder_Record(pMe, QCAMCORDER_RECORD_STOP);
               }
               break;
            case IDS_PAUSE:
               if (pMe->eState == QCAMCORDER_RECORDING)
               {
                  Qcamcorder_Record(pMe, QCAMCORDER_RECORD_PAUSE);
               }
               else
               {
                 QCAMCORDER_MSG_HIGH("Recording is not started yet." ,0,0,0);
               }
               break;
            case IDS_RESUME:
               if (pMe->eState == QCAMCORDER_PAUSED)
               {
               Qcamcorder_Record(pMe, QCAMCORDER_RECORD_RESUME);
               }
               else
               {
                 QCAMCORDER_MSG_HIGH("Recording is not in paused mode." ,0,0,0);
               }
               break;
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
            case IDS_OVERLAY_MENU:
               {
                  AEERect rc;
                  QCAMCORDER_MSG_MED("Yes Overlay menu select received!!" ,0,0,0);
                  Qcamcorder_ClearOverlayFileName(pMe,&rc);
                  if (STRCMP(pMe->sOverlayMenuFile,pMe->sFrameOverlay))
                  {
                    (void)STRLCPY(pMe->sFrameOverlay,pMe->sOverlayMenuFile,sizeof(pMe->sFrameOverlay));
                    Qcamcorder_LoadFrame(pMe);
                  }
                  break;
               }
#endif
            case IDS_ALBUM:
               if (pMe->eState == QCAMCORDER_PREVIEW)
               {
                  IMENUCTL_Reset(pMe->m_pIPreview);
                  if (ICAMERA_Stop(pMe->m_pICam) == SUCCESS)
                  {
                    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe );
                    pMe->eState = QCAMCORDER_PREVIEW_TO_ALBUM;
                  }
                  else
                  {
                    Qcamcorder_Preview(pMe);
                  }
               }
               break;
#ifdef FEATURE_VIDEO_ENCODE_RAW
            case IDS_RAW_ALBUM:
               if (pMe->eState == QCAMCORDER_PREVIEW)
               {
                  IMENUCTL_Reset(pMe->m_pIPreview);
                  if(ICAMERA_Stop(pMe->m_pICam) == SUCCESS)
                  {
                    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe );
                    pMe->eState = QCAMCORDER_PREVIEW_TO_RAW_ALBUM;
                  }
                  else
                  {
                    Qcamcorder_Preview(pMe);
                  }
               }
               break;
#endif /* FEATURE_VIDEO_ENCODE_RAW */

            /*Selected Auto Focus*/
            case IDS_FOCUS:
                /*Check if sensor is already trying to Focus*/
                if(!pMe->FocusInProgress)
                {
                  Qcamcorder_OnFocus(pMe, key);
                  Qcamcorder_DisplayPreviewMenuName(pMe, 0);
                }
            break;

            case IDS_GPS_SETUP:
               {
                 /*
                 ** Set qcamcorder state here and build GPS menu in
                 ** Qcamcorder_CamEventNotify after ICamera_Stop is done
                 */

                 (void)ICAMERA_Stop(pMe->m_pICam);

                 pMe->eState = QCAMCORDER_PREVIEW_TO_GPS;
                 return TRUE;
               }

            case IDS_SETTINGS:
               {
                 /*
                 ** Set qcamcorder state here and build actual settings menu
                 ** menu in Qcamcorder_CamEventNotify after ICamera_Stop has
                 ** completed. This prevents the first settings menu from
                 ** being overlayed with viewfinder image
                 */

                 IMENUCTL_Reset(pMe->m_pIPreview);
                 (void)ICAMERA_Stop(pMe->m_pICam);
                 (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe );

                 pMe->eState = QCAMCORDER_PREVIEW_TO_SETTINGS;

               }
               break;
            default:
               break;
            }
         }
      }
      return TRUE;

   case AVK_UP:
   case AVK_DOWN:
      if ((eCode == EVT_KEY) && (pMe->m_activeMenu != QCAMCORDER_MENU_GPS) &&
          ((pMe->eState == QCAMCORDER_PREVIEW) ||(pMe->eState == QCAMCORDER_RECORDING) ||
           (pMe->eState == QCAMCORDER_PAUSED)))
      {
         uint16 selection;
         selection = IMENUCTL_GetSel(pMe->m_pIPreview);
         switch (selection)
         {

         case IDS_BRIGHTNESS:
            if (key == AVK_UP)
            {
               pMe->nBright += pMe->nBrightnessStep;
            }
            else
            {
               pMe->nBright -= pMe->nBrightnessStep;
            }
            if(pMe->nBright > pMe->nBrightnessMax)
            {
               pMe->nBright = pMe->nBrightnessMax;
            }
            if(pMe->nBright < pMe->nBrightnessMin)
            {
               pMe->nBright = pMe->nBrightnessMin;
            }
            (void)ICAMERA_SetBrightness(pMe->m_pICam, pMe->nBright);
            Qcamcorder_DisplayPreviewMenuName(pMe, 0);
            break;

            /*Manual Focus Key has been Selected*/
         case IDS_FOCUS:
             if(!pMe->FocusInProgress)
             {
               Qcamcorder_OnFocus(pMe, key);
               Qcamcorder_DisplayPreviewMenuName(pMe, 0);
             }
            break;

           case IDS_ZOOM:
            if (key == AVK_UP)
            {
               pMe->nZoom += pMe->nZoomStep;
            }
            else
            {
               pMe->nZoom -= pMe->nZoomStep;
            }
            if(pMe->nZoom > pMe->nZoomMax)
            {
               pMe->nZoom = pMe->nZoomMax;
            }
            if(pMe->nZoom < pMe->nZoomMin)
            {
               pMe->nZoom = pMe->nZoomMin;
            }

            /* The boolean below indicates key is not in hold and update which key it is. */
            pMe->m_bKeySelect = TRUE;
            pMe->m_bLastKey = key;

            (void)ICAMERA_SetZoom(pMe->m_pICam, pMe->nZoom);
            (void)ISHELL_SetTimer(pMe->a.m_pIShell,300,
            (PFNNOTIFY)Qcamcorder_SmoothZoom,(void *)pMe);
            Qcamcorder_DisplayPreviewMenuName(pMe, 0);
            break;
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
         case IDS_OVERLAY_MENU:
         {
           AECHAR szBuf[TEXT_STRING_SIZE] = {0};
           IFileMgr *pIFileMgr = NULL;
           FileInfo *filelist = NULL,fi;
           int32 enableoverlay = 0, file_count = 0,tempc = -1,found_index = -1,show_index = -1;
           AEERect rc;
           char* pfileExt;
           uint16 max_file_count = 0;
 
           if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr) != SUCCESS)
             return FALSE;
           (void)IFILEMGR_EnumInit(pIFileMgr, QCAMCORDER_FRAMES_DIR, FALSE);
           while (IFILEMGR_EnumNext(pIFileMgr, &fi))
           {
             QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);
             /* camcorder only supports .png file for frame overlay */
             pfileExt=QCamcorder_GetFileNameExtension(fi.szName);
             if (pfileExt && STRCMP(pfileExt, "png") == 0)
               file_count++; //counting the no. of .png files
           }
           if(file_count)
           {
             filelist = MALLOC((file_count) * sizeof(FileInfo));
             if(filelist == NULL)
             {
               (void)IFILEMGR_Release(pIFileMgr);
               return FALSE;
             }
             if (STRCMP(pMe->sOverlayMenuFile,ENABLE_FRAMEOVERLAY_STRING) == 0)
               enableoverlay = 1;
             (void)IFILEMGR_EnumInit(pIFileMgr, QCAMCORDER_FRAMES_DIR, FALSE);
             
             max_file_count = 0;
  
             while (tempc < file_count && IFILEMGR_EnumNext(pIFileMgr, &fi))
             {
               QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);
               /* camcorder only supports .png file for frame overlay */
               pfileExt=QCamcorder_GetFileNameExtension(fi.szName);
               if (pfileExt && STRCMP(pfileExt, "png") == 0)
               {
                 ++tempc;
                 filelist[tempc] = fi; //.png files
                 if (!enableoverlay && STRCMP(fi.szName, pMe->sOverlayMenuFile) == 0)
                   found_index = tempc;
               }
             }
           }
           (void)IFILEMGR_Release(pIFileMgr);
           if(file_count)
           {
             if(key == AVK_DOWN)
               show_index = found_index+1;
             else
               show_index = ((found_index==-1)?file_count - 1: found_index-1);
           }
           while (show_index >=0 && show_index <file_count)
           {
             if(Qcamcorder_validPNG(pMe,filelist[show_index].szName))
             {
               (void)STRLCPY(pMe->sOverlayMenuFile,filelist[show_index].szName,sizeof(pMe->sOverlayMenuFile));
               (void)STR_TO_WSTR(&pMe->sOverlayMenuFile[QCAMCORDER_FRAMES_DIR_LEN], szBuf, sizeof(szBuf));
               break;
             }
             (key == AVK_DOWN) ? ++show_index : --show_index;
           }
           FREEIF(filelist);
           if(show_index == -1 || show_index == file_count)
           {
             // need to display "No Frame"
             (void)STRLCPY(pMe->sOverlayMenuFile,ENABLE_FRAMEOVERLAY_STRING,sizeof(pMe->sOverlayMenuFile));
             (void)STR_TO_WSTR(DISABLE_FRAMEOVERLAY_STRING, szBuf, sizeof(szBuf));
           }
           Qcamcorder_ClearOverlayFileName(pMe,&rc);
           (void)IDISPLAY_DrawText(pMe->m_overlayDisplay, AEE_FONT_NORMAL, szBuf, -1, rc.x, rc.y, &rc, 0);
           IDISPLAY_Update(pMe->m_overlayDisplay);
           QCAMCORDER_MSG_MED("Yes Overlay up / down menu received!!" ,0,0,0);
         }
         break;
#endif
         default:
            //ISHELL_Beep(pMe->a.m_pIShell, BEEP_ERROR, FALSE);
            break;
         }
         return TRUE;
      }
      break;
   case AVK_CLR:
      if (eCode == EVT_KEY)
      {
         if (pMe->m_activeMenu == QCAMCORDER_MENU_GPS)
         {
            pMe->m_activeMenu = QCAMCORDER_MENU_NOMENU;
            IMENUCTL_Reset(pMe->m_pIMenu);
            (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
            (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
            return TRUE;
         }
         else if (pMe->eGPSState == QCAMCORDER_GPS_ACQUIRING)
         {
            pMe->eGPSState = QCAMCORDER_GPS_DISABLED;
            if (pMe->pIPosDet)
            {
              (void)IPOSDET_Release(pMe->pIPosDet);
               pMe->pIPosDet = NULL;
            }
            IMENUCTL_Reset(pMe->m_pIMenu);
            (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
            (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);

            return TRUE;
         } 
         else if (pMe->eState == QCAMCORDER_PREVIEW)
         {
            // Stop the camera driver before exiting
            pMe->eState = QCAMCORDER_NULL;
            (void)ICAMERA_Stop(pMe->m_pICam);
         }
         else if (pMe->eState == QCAMCORDER_RECORDING)
         {
           if(pMe->m_bCont)
           {
             pMe->bStopRecording = TRUE; /* Stop continuous mode */
           }
           pMe->m_bPreview_to_Record = FALSE;
           Qcamcorder_Record(pMe, QCAMCORDER_RECORD_STOP);
           return TRUE;
         }
      }
      break;
     default:
       QCAMCORDER_MSG_HIGH("Key %d not handled",key,0,0);
       break;
   }
   return 2;
} /* END Qcamcorder_PreviewKeyHandler */


/*===========================================================================
FUNCTION Qcamcorder_SmoothZoom

  DESCRIPTION
  This function performs continuous zoom at 1 step per increment.

  PARAMETERS:
    pMe - Pointer to  Qcamcorder struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamcorder_SmoothZoom(Qcamcorder * pMe)
{
   /* Continue to smooth zoom if key is still down*/
    if (pMe->m_bKeySelect)
    {
        pMe->nZoom = ( pMe->m_bLastKey == (uint16)AVK_UP) ? ( pMe->nZoom + pMe->nZoomStep) : (pMe->nZoom - pMe->nZoomStep);
        if (pMe->nZoom > pMe->nZoomMax)
        {
            pMe->nZoom = pMe->nZoomMax;
        }
        else if(pMe->nZoom < pMe->nZoomMin)
        {
            pMe->nZoom = pMe->nZoomMin;
        }
        (void)ICAMERA_SetZoom(pMe->m_pICam, pMe->nZoom);
        (void)ISHELL_SetTimer(pMe->a.m_pIShell,300,
        (PFNNOTIFY)Qcamcorder_SmoothZoom,(void *)pMe);
        Qcamcorder_DisplayPreviewMenuName(pMe, 0);
    }
}
/*===========================================================================
FUNCTION Qcamcorder_OnFocus

  DESCRIPTION
    Handler for key event on Focus menu item

  PARAMETERS:
    pMe - Pointer to  Qcamcorder struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamcorder_OnFocus(Qcamcorder * pMe, uint16 key)
{
    AEEParmInfo pInfo;
    //for Auto focus
    if (key == AVK_SELECT)
    {
        //Set the flag. Auto focus can be started without setting to the menu item
        pMe->m_bFocusStarted = TRUE;
        /*In Auto Focus Mode*/
        pMe->nAutoOrManFocus = AUTO_FOCUS_SELECTED;
        pMe->FocusInProgress = 1;

        //Auto Focus mode
        if (ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_FOCUS, CAM_FOCUS_AUTO, 0) != SUCCESS)
        {
            Qcamcorder_DrawIcon(pMe, IDB_AF_FAIL);
            pMe->FocusInProgress = 0;
        }
        else
        {
            //Update the status indicator
            Qcamcorder_DrawIcon(pMe, IDB_AF_DONE);
            /*Since Auto Focus takes about 2 secs, we set a timer for 2 secs
              Once timer expires we will display AF done button
            */
            (void)ISHELL_SetTimer(pMe->a.m_pIShell,2000, (PFNNOTIFY)Qcamcorder_ShowAFIndicator,(void *)pMe);
        }
    }
    //for manual focus
    else if ((key == AVK_UP) || (key == AVK_DOWN))
    {

        //Update the nFocus with current step got from camera before update the step.
        //It could be changed by auto focus or other factor.
        (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_FOCUS, &pMe->nFocus, (int32*)&pInfo);

        /*In Maual Focus Mode*/
        pMe->nAutoOrManFocus = MANUAL_FOCUS_SELECTED;

        if (key == AVK_UP)
        {
            pMe->nFocus += pMe->nFocusStep;
        }
        else
        {
            pMe->nFocus -= pMe->nFocusStep;
        }
        /*Check for Boundary Conditions*/
        if (pMe->nFocus > pMe->nFocusMax)
        {
            pMe->nFocus = pMe->nFocusMax;
        }
        else if (pMe->nFocus < pMe->nFocusMin)
        {
            pMe->nFocus = pMe->nFocusMin;
        }

        //Set the flag.
        pMe->m_bFocusStarted = TRUE;

        //Update the status indicator
        Qcamcorder_DrawIcon(pMe, IDB_AF_NONE);

        if (ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_FOCUS, CAM_FOCUS_MANUAL, pMe->nFocus) != SUCCESS)
        {
            Qcamcorder_DrawIcon(pMe, IDB_AF_FAIL);
        }
    }
}

/*===========================================================================
FUNCTION Qcamcorder_RemoveFocusIcon

  DESCRIPTION
    Function cleans up Focus Icon.

  PARAMETERS:
    None
  RETURN VALUE:
    None
===========================================================================*/
static void Qcamcorder_RemoveFocusIcon(Qcamcorder *pMe)
{
    AEERect rcTemp;
    AEEImageInfo info = {0};
    IImage *pImage = NULL;
    /*Set the Focus Started Flag to False*/
    pMe->m_bFocusStarted = FALSE;
    /*Set the Dimensions of rectangle to be cleaned up*/
    SETAEERECT(&rcTemp, pMe->di.cxScreen - 25, 0, 30, 30);
    /*Load the Image*/
    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, IDB_AF_NONE);
    /*Get the Image Dimensions*/
    IIMAGE_GetInfo(pImage, &info);
    /*Calculate x co-ordinate for Erasing*/
    rcTemp.x = rcTemp.x + (rcTemp.dx - info.cx)/2;
    /*Asign height and width of the rectangle area to be erased*/
    rcTemp.dx = info.cx;
    rcTemp.dy = info.cy;
    //lint -e570  Suppress complaint on loss of sign
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rcTemp);
    //lint +e570
}

/*===========================================================================
FUNCTION Qcamcorder_ShowAFIndicator

  DESCRIPTION
    Function Shows Auto Focus Icon after expiry of Timer.

  PARAMETERS:
    None
  RETURN VALUE:
    None
===========================================================================*/

void Qcamcorder_ShowAFIndicator(Qcamcorder *pMe)
{
   uint16 selection = 0;
   /*Auto Focus should be complete now*/
   pMe->FocusInProgress = 0;
   selection = IMENUCTL_GetSel(pMe->m_pIPreview);
   /*Check if we are in Focus menu*/
   if(selection == IDS_FOCUS)
      Qcamcorder_DrawIcon(pMe, IDB_AF_START);
}
/* ==========================================================================
FUNCTION QCAMCORDER_DRAWICON
DESCRIPTION
============================================================================= */
static void Qcamcorder_DrawIcon(Qcamcorder *pMe, uint16 nResID)
{
   AEERect rc = {0}; //area for image icon
   IImage *pImage = NULL;
   AEEImageInfo info = {0};

   SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, 80);

    switch (nResID)
    {
       /*Display Co-oridnates for Focus Icons*/
       case IDB_AF_NONE :
       case IDB_AF_DONE :
       case IDB_AF_FAIL :
       case IDB_AF_START :
           SETAEERECT(&rc, pMe->di.cxScreen - 25, 0, 30, 30);
       break;
       case IDB_TRANSCODER_DONE:
       case IDB_TRANSAUDIO_START:
       case IDB_TRANSAUDIO_DONE:
       case IDB_TRANSVIDEO_START:
#ifdef FEATURE_DVS
       case IDB_DVS_DONE :
#endif
           SETAEERECT(&rc, 0, 20, pMe->di.cxScreen, 80);
           break;

       case IDB_FILE_DELETE_DONE:
           SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen);
           break;

       default:
       break;
    }

   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
   //lint +e570
   pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, nResID);
   if (pImage)
   {
      IIMAGE_GetInfo(pImage, &info);
        /*Check for Image Id's and fix the co-ordinates accordingly*/
        switch (nResID)
        {
           /*Draw focus icons on Top right corner*/
           case IDB_AF_NONE :
           case IDB_AF_DONE :
           case IDB_AF_FAIL :
           case IDB_AF_START :
             IIMAGE_Draw(pImage, rc.x + (rc.dx - info.cx)/2, 0);
           break;
           /*Draw all other Icons with following co-ordinates*/
           default:
             IIMAGE_Draw(pImage, (pMe->di.cxScreen - info.cx)/2, 20);
           break;
        }
      (void)IIMAGE_Release(pImage);
      pImage = NULL;
      IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
   }
}
/* ==========================================================================
FUNCTION QCAMCORDER_SETTINGSKEYHANDLER
DESCRIPTION
============================================================================= */
static uint8 Qcamcorder_SettingsKeyHandler(Qcamcorder* pMe, AEEEvent eCode, uint16 key)
{
   uint8 result = 2;
   if (eCode == EVT_KEY)
   {
      // If the start sound menu is active, make sure it is not still playing a sound.
      if(pMe->m_activeMenu == QCAMCORDER_MENU_SOUND && pMe->pMedia)
      {
          QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
          QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);
      }

      switch(key)
      {
      case AVK_SELECT:
         break;

      case AVK_LEFT:
      case AVK_RIGHT:
#ifdef FEATURE_CAMCORDER_AAC_REC
        /* Disable left right key press when in AAC submenus */
        if(!(pMe->m_activeMenu == QCAMCORDER_MENU_AAC_CONFIG ||
             pMe->m_activeMenu == QCAMCORDER_MENU_AAC_MODE   ||
             pMe->m_activeMenu == QCAMCORDER_MENU_AAC_FREQ   ||
             pMe->m_activeMenu == QCAMCORDER_MENU_AAC_BITRATE))
#endif /* FEATURE_CAMCORDER_AAC_REC */
        {
          Qcamcorder_DoSettingsMenu(pMe, (key == AVK_LEFT ? -1 : 1) );
        }
        break;

      case AVK_UP:
      case AVK_DOWN:
         break;

      case AVK_CLR:
         result = TRUE;
#ifdef FEATURE_CAMCORDER_AAC_REC
        /* Go back to previous menu for AAC if we are
        ** currently displaying one of the AAC submenus*/
        if(pMe->m_activeMenu == QCAMCORDER_MENU_AAC_MODE   ||
           pMe->m_activeMenu == QCAMCORDER_MENU_AAC_FREQ   ||
           pMe->m_activeMenu == QCAMCORDER_MENU_AAC_BITRATE)
        {
          Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_AAC_CONFIG);
        }
        else if(pMe->m_activeMenu == QCAMCORDER_MENU_AAC_CONFIG)
        {
          Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_AUDIO_FORMAT);
          IMENUCTL_SetActive(pMe->m_pISettings, TRUE);
          (void)IMENUCTL_Redraw(pMe->m_pISettings);
        }
        else
#endif /* FEATURE_CAMCORDER_AAC_REC */
        {
         // Save the settings
         Qcamcorder_SetConfigRecord(pMe);

         IMENUCTL_SetActive(pMe->m_pISettings, FALSE);
         IMENUCTL_SetActive(pMe->m_pIPreview, TRUE);
         (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
        }
        break;

#ifdef FEATURE_CAMCORDER_AAC_REC
      case AVK_POUND:
        {
          uint16 selection = IMENUCTL_GetSel(pMe->m_pIMenu);
          result = TRUE;

          //Open the advanced settings for AAC audio only
          if(pMe->m_activeMenu == QCAMCORDER_MENU_AUDIO_FORMAT &&
             selection == IDS_AUDIO_FORMAT_AAC)
          {
            IMENUCTL_SetActive(pMe->m_pISettings, FALSE);
            Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_AAC_CONFIG);
          }
        }
         break;
#endif /* FEATURE_CAMCORDER_AAC_REC */

        default:
          QCAMCORDER_MSG_HIGH("Key %d not handled",key,0,0);
          break;
      }
   }
   return result;
} /* END Qcamcorder_EditPicKeyHandler */
/* ==========================================================================
FUNCTION Qcamcorder_GetFilecount
DESCRIPTION
============================================================================= */

static int Qcamcorder_GetFilecount(Qcamcorder *pMe)
{  FileInfo fi;
   int i, count=0;
   uint16 max_file_count = 0;
 
   for(i = 0; i < (int)ARR_SIZE(AEEDirList); i++)
   {
     if(IFILEMGR_EnumInit(pMe->pIFileMgr, AEEDirList[i], FALSE) == SUCCESS)
     {
       while (IFILEMGR_EnumNext(pMe->pIFileMgr, &fi))
       {
         QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);  
         if(Qcamcorder_IsValidFileName(fi.szName))
          count++;
       }
     }
   }
   return count;
}
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
/* ==========================================================================
FUNCTION QCAM_EXECUTECONFIRMMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteConfirmMenu(Qcamcorder* pMe)
{
   CtlAddItem pai;
  AECHAR acBuf[PROMPT_SIZE];
  AECHAR szBuf[PROMPT_SIZE] = {0};
 AEEPromptInfo pi = {0};
  uint16 buttonIDs[3] = { IDS_QCAMCORDER_NO,IDS_QCAMCORDER_YES, 0};
   (void)MEMSET(&pai, 0, sizeof(CtlAddItem));


  pi.pszRes   = QCAMCORDER_RES_FILE;
  pi.wTitleID = IDS_EDIT_FILE_TITLE;
  pi.fntTitle = AEE_FONT_BOLD;
  pi.fntText  = AEE_FONT_NORMAL;
  pi.pBtnIDs  = buttonIDs;
  (void)ISHELL_LoadResString(pMe->a.m_pIShell, QCAMCORDER_RES_FILE,
                       IDS_EDIT_FILE_PROMPT, szBuf, sizeof(szBuf));
  (void)STR_TO_WSTR("\n", acBuf, sizeof(acBuf));
  (void)WSTRLCAT(szBuf, acBuf,PROMPT_SIZE);

  pi.pText = szBuf;
  //lint -e570  Suppress complaint on loss of sign
  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
  //lint +e570

  IDISPLAY_Update(pMe->a.m_pIDisplay);
  if (!ISHELL_Prompt(pMe->a.m_pIShell, &pi))
      {
          Qcamcorder_EditFileResponse(pMe, IDS_QCAMCORDER_YES);
      }

  return;
} /* END Qcamcorder_ExecuteConfirmMenu */
#endif
/* ==========================================================================
FUNCTION QCAM_EXECUTEDELETEALLMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteDeleteMenu(Qcamcorder* pMe)
{
  CtlAddItem pai;
  AECHAR acBuf[PROMPT_SIZE];
  AECHAR szBuf[PROMPT_SIZE] = {0};
  AEEPromptInfo pi = {0};
  uint16 buttonIDs[3] = { IDS_QCAMCORDER_NO,IDS_QCAMCORDER_YES, 0};
  (void)MEMSET(&pai, 0, sizeof(CtlAddItem));


            pi.pszRes   = QCAMCORDER_RES_FILE;
            pi.wTitleID = IDS_DELETE_FILE_TITLE;
            pi.fntTitle = AEE_FONT_BOLD;
            pi.fntText  = AEE_FONT_NORMAL;
            //pi.wDefBtn  = IDB_YES;
            pi.pBtnIDs  = buttonIDs;

            (void)ISHELL_LoadResString(pMe->a.m_pIShell, QCAMCORDER_RES_FILE,
               IDS_DELETE_FILE_PROMPT, szBuf, sizeof(szBuf));

            (void)STR_TO_WSTR("\n", acBuf, sizeof(acBuf));
            (void)WSTRLCAT(szBuf, acBuf, PROMPT_SIZE);

            pi.pText = szBuf;

            //lint -e570  Suppress complaint on loss of sign
            IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
            //lint +e570

            IDISPLAY_Update(pMe->a.m_pIDisplay);
            if (!ISHELL_Prompt(pMe->a.m_pIShell, &pi))
            {
              Qcamcorder_DeleteFileResponse(pMe, IDS_QCAMCORDER_YES);
            }

   return;
} /* END QCam_ExecuteDeleteAllMenu */


/*===========================================================================

FUNCTION Qcamcorder_HandleEvent

DESCRIPTION
   This is the EventHandler for this app. All events to this app are handled in this
   function. All APPs must supply an Event Handler.

PROTOTYPE:
   boolean Qcamcorder_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
   pi: Pointer to the AEEApplet structure. This structure contains information specific
   to this applet. It was initialized during the AEEClsCreateInstance() function.

   ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
static boolean Qcamcorder_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   Qcamcorder* pMe = (Qcamcorder*)pi;
#if defined(FEATURE_MP4_TRANSCODER) && !defined(FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE)
  xcoder_status_type *stat;
#endif
#ifdef FEATURE_VIDEO_ENCODE_RAW
   raw_encoder_info_type *info;
#endif /* FEATURE_VIDEO_ENCODE_RAW */
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

   switch (eCode)
   {
   case EVT_APP_START:
     {
       if (QcamcorderInit(pMe) == FALSE)

#ifdef FEATURE_VIDEONOSENSOR
       {
          if( !(Qcamcorder_GetFilecount(pMe)) )
#endif /*FEATURE_VIDEONOSENSOR*/
         return FALSE;
#ifdef FEATURE_VIDEONOSENSOR
           /*No Sensor Found Hence goto Album Menu*/
           pMe->b_focusFirstItem = TRUE;
           pMe->m_activeMenu = QCAMCORDER_MENU_FILELIST;
           pMe->m_activeSoftMenu = QCAMCORDER_MENU_NONE;
           pMe->eState = QCAMCORDER_NULL;
           pMe->m_bNoSensor = TRUE;
           /*Display file list when sensor checking fails. */
           Qcamcorder_DisplayFileList(pMe);
           return TRUE;
       }
#endif /*FEATURE_VIDEONOSENSOR*/


       /* Disallow Qcamcorder start when in a voice call
       ** Draw icon notifying user of the non-operation */
       if (ITELEPHONE_IS_VOICECALL_PRESENT(pMe))
       {
         if(pMe->bVoiceCallRejected)
         {
           Qcamcorder_HandleVoiceCallRejected(pMe);
         }
         else
         {
           QCamcorder_MdpRegisterFullScreen(pMe, TRUE);
           Qcamcorder_DrawIcon(pMe, IDB_NOT_AVAILABLE);
         }
       }
       else

#ifdef FEATURE_QVPHONE
      /* Disallow Qcamcorder start when in a VT call
      ** Draw icon notifying user of the non-operation */
       if (AEEVP_IS_VIDEOCALL_PRESENT( pMe->m_pIVP ))
       {
           QCamcorder_MdpRegisterFullScreen(pMe, TRUE);
           Qcamcorder_DrawIcon(pMe, IDB_NOT_AVAILABLE);
       }
       else
#endif
       {
         IMENUCTL_SetActive(pMe->m_pIPreview, TRUE);
         (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
       }
       return(TRUE);
     }

   case EVT_APP_STOP:
      Qcamcorder_SetConfigRecord(pMe);
      return TRUE;

   case EVT_APP_SUSPEND://one case: if there is incoming call
      Qcamcorder_SetConfigRecord(pMe);
      return FALSE;

   case EVT_APP_RESUME://after calling
      if(QcamcorderInit(pMe) == FALSE)
      { return FALSE;
      }
      IMENUCTL_SetActive(pMe->m_pIPreview, TRUE);
      (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
      return TRUE;

   case EVT_DIALOG_INIT:
      if(Qcamcorder_MapEventHandlers(pMe, (IDialog *) dwParam, wParam)) {
         IDIALOG_SetProperties((IDialog *) dwParam, DLG_HANDLE_ALL_EVENTS );
         return TRUE;
      }
      else
         return FALSE;

   case EVT_DIALOG_START:
      // Add dialog startup code here
      return TRUE;
   case EVT_DIALOG_END:
      // Add dialog ending code here
      return TRUE;
   case EVT_KEY_RELEASE:
      // The boolean below is used to help identify the operation that needs to be performed
      pMe->m_bKeySelect = FALSE;
      //lint -fallthrough
   case EVT_KEY_PRESS:
   case EVT_KEY:
      if (pMe->m_activeSoftMenu == QCAMCORDER_MENU_PREVIEW)
      {
         uint8 result = Qcamcorder_PreviewKeyHandler(pMe, eCode, wParam);
         if (result <= TRUE)
         {
            return result;
         }
      }
      else if (pMe->m_activeSoftMenu == QCAMCORDER_MENU_SETTINGS)
      {
#ifdef FEATURE_VIDEONOSENSOR
          /*
            If there is no Sensor and user presses 0 then we go back to Album Mode
          */
         if(wParam == AVK_0 && pMe->m_bNoSensor)
         {
            /*No Sensor Found Hence goto Album Menu*/
            pMe->b_focusFirstItem = TRUE;
            pMe->m_activeMenu = QCAMCORDER_MENU_FILELIST;
            pMe->m_activeSoftMenu = QCAMCORDER_MENU_NONE;
            pMe->eState = QCAMCORDER_NULL;
            /*Display file list when sensor checking fails. */
            Qcamcorder_DisplayFileList(pMe);

         }
         /*
           If we don't have sensor then Clear Key is ignored since it will take
           us to preview mode
         */
         else if(
                 (!pMe->m_bNoSensor) ||
                 (pMe->m_bNoSensor && wParam != AVK_CLR)

                  )
#endif /*FEATURE_VIDEONOSENSOR*/
         {
            uint8 result = Qcamcorder_SettingsKeyHandler(pMe, eCode, wParam);
            if (result <= TRUE)
            {
               return result;
            }
         }
      }
      else if (pMe->m_activeSoftMenu == QCAMCORDER_MENU_EDIT)
      {
         uint8 result;
         // Let the menu have first shot unless the delete screen is up
         if(wParam != AVK_LEFT && wParam != AVK_RIGHT)
         {
           if(pMe->m_activeMenu == QCAMCORDER_MENU_POST_EDIT ||
              pMe->m_activeMenu == QCAMCORDER_MENU_GPS)
           {
             (void)IMENUCTL_HandleEvent(pMe->m_pIEditMenu, eCode, wParam, 0);
           }
           else
           {
             (void)IMENUCTL_HandleEvent(pMe->m_pISoftMenu, eCode, wParam, 0);
           }
         }
         else if (pMe->m_activeMenu != QCAMCORDER_MENU_DELETE)
         {
           if(pMe->eState == QCAMCORDER_POSTEDIT)
           {//clear the screen if we are exiting out of post edit screen

             (void)IMENUCTL_HandleEvent(pMe->m_pISoftMenu, eCode, wParam, 0);
             pMe->eState = QCAMCORDER_EDIT;
             IDISPLAY_Update(pMe->a.m_pIDisplay);
           }
           else
           {
             (void)IMENUCTL_HandleEvent(pMe->m_pISoftMenu, eCode, wParam, 0);
           }
         }
         result = Qcamcorder_EditKeyHandler(pMe, eCode, wParam);
         if (result <= TRUE)
         {
            return result;
         }
      }
#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif //#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
      else if (pMe->m_activeMenu == QCAMCORDER_MENU_FILELIST)
      {
         // If the user hit select on the filelist show the edit screen
         if (wParam == AVK_SELECT && eCode == EVT_KEY)
         {
           int count, i;
           FileInfo fi;
           uint16 max_file_count = 0;

           count = IMENUCTL_GetSel(pMe->m_pIMenu);

           for(i = 0; i < (int) ARR_SIZE(AEEDirList); i++)
           {
             (void)IFILEMGR_EnumInit(pMe->pIFileMgr, AEEDirList[i], FALSE);
             while (IFILEMGR_EnumNext(pMe->pIFileMgr, &fi))
             {
                QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);
               // Check if the extension of the file matches our movie extension
               if(Qcamcorder_IsValidFileName(fi.szName))
               {
                  if (!--count)
                     break;
               }
             }
             if(!count)
             { char * p_newfile;
               p_newfile = MALLOC(FILE_NAME_SIZE);
               if(!p_newfile)
               { QCAMCORDER_MSG_HIGH("MALLOC FAILED will not play file",0,0,0);
                 (void)STRLCPY(pMe->sCurrentFileName, fi.szName, sizeof(pMe->sCurrentFileName));
               }else{
#ifdef FEATURE_BREW_3_0
                 (void)STRLCAT(p_newfile, "",FILE_NAME_SIZE);
#else
                 STRLCAT(p_newfile, OEMDirList[i],FILE_NAME_SIZE);
#endif
                 (void)STRLCAT(p_newfile, fi.szName,FILE_NAME_SIZE);
                 (void)STRLCPY(pMe->sCurrentFileName, p_newfile, sizeof(pMe->sCurrentFileName));
               }
               FREEIF(p_newfile);
               break;
             }
           }

           IMENUCTL_SetActive(pMe->m_pIPreview, FALSE);
           pMe->eState = QCAMCORDER_EDIT;
           Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);

           // Remove any temporary video file
           (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_MMC_FILE);
#ifdef FEATURE_EFS_FTL
           (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_FTL_FILE);
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
           (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_USB_FILE);
#endif
           (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_FILE);
           return TRUE;
         }
         // If the user hit clr go back to preview if there is a Sensor
         if (wParam == AVK_CLR && eCode == EVT_KEY
#ifdef FEATURE_VIDEONOSENSOR
             && !pMe->m_bNoSensor
#endif /*FEATURE_VIDEONOSENSOR*/
            )
         {
            IMENUCTL_SetActive(pMe->m_pIPreview, TRUE);
            (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
            return TRUE;
         }
#ifdef FEATURE_VIDEONOSENSOR
         /*
         if zero key is pressed and sensor is missing, then we navigate to settings
         menu.
         */
         if(wParam == AVK_0 && pMe->m_bNoSensor)
         {
             pMe->eState = QCAMCORDER_NULL;
             Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_SETTINGS);
         }
#endif /*FEATURE_VIDEONOSENSOR*/
      }
#ifdef FEATURE_VIDEO_ENCODE_RAW
      else if (pMe->m_activeMenu == QCAMCORDER_MENU_RAWFILELIST)
      {
        // If the user hit select on the raw filelist show the edit screen
        if (wParam == AVK_SELECT && eCode == EVT_KEY)
        {
          int count, i;
          FileInfo fi;
          char * pdot, * pseperator;
          uint16 max_file_count = 0;
          count = IMENUCTL_GetSel(pMe->m_pIMenu);

          for(i = 0; i < ARR_SIZE(AEEDirList); i++)
          {
            (void)IFILEMGR_EnumInit(pMe->pIFileMgr, AEEDirList[i], FALSE);
            while (IFILEMGR_EnumNext(pMe->pIFileMgr, &fi))
            {
              QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);
              // Only add raw files - distinguished by name format only: rawXXX.yuv
              pdot = STRRCHR(fi.szName, '.');
              pseperator = STRRCHR(fi.szName, '/');
              if(pdot && pseperator && 
                 !STRCMP(pdot, QCAMCORDER_YUV_MOVIE_EXTENSION) &&
                 !STRNCMP(pseperator+1, "raw", 3))
              {
                if (!--count)
                  break;
              }
            }
            if(!count)
            {
              char * p_newfile;
              p_newfile = MALLOC(FILE_NAME_SIZE);
              if(!p_newfile)
              {
                QCAMCORDER_MSG_HIGH("MALLOC FAILED will not play file",0,0,0);
                (void)STRLCPY(pMe->sCurrentFileName, fi.szName, sizeof(pMe->sCurrentFileName));
              }
              else
              {
#ifdef FEATURE_BREW_3_0
                (void)STRLCAT(p_newfile, "",FILE_NAME_SIZE);
#else
                STRLCAT(p_newfile, OEMDirList[i],FILE_NAME_SIZE);
#endif
                (void)STRLCAT(p_newfile, fi.szName,FILE_NAME_SIZE);
                (void)STRLCPY(pMe->sCurrentFileName, p_newfile, sizeof(pMe->sCurrentFileName));
              }
              FREEIF(p_newfile);
              break;
            }
          }

          IMENUCTL_SetActive(pMe->m_pIPreview, FALSE);
          pMe->eState = QCAMCORDER_START_RAW_ENCODE;
          Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);
          // Remove any temporary video file
          (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_FILE);
          return TRUE;
        }
        // If the user hit clr go back to preview
        if (wParam == AVK_CLR && eCode == EVT_KEY)
        {
          IMENUCTL_SetActive(pMe->m_pIPreview, TRUE);
          (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
          return TRUE;
        }
      }
#endif /* FEATURE_VIDEO_ENCODE_RAW */
      /* -dlb
      else if (eCode == EVT_KEY)
      {
         IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
         IMENUCTL_SetActive(pMe->m_pIPreview, TRUE);
         ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
         return TRUE;

      }
      */
      if(IMENUCTL_HandleEvent(pMe->m_pIPreview, eCode, wParam, 0))
         return TRUE;
      else if(IMENUCTL_HandleEvent(pMe->m_pISettings, eCode, wParam, 0))
         return TRUE;
      else if(IMENUCTL_HandleEvent(pMe->m_pIMenu, eCode, wParam, 0))
         return TRUE;
      break;
   case EVT_COMMAND:
      if(IMENUCTL_IsActive(pMe->m_pIPreview)
#ifdef FEATURE_VIDEONOSENSOR
         && !pMe->m_bNoSensor
#endif /*FEATURE_VIDEONOSENSOR*/
         )
      {
         switch(wParam)
         {
         case IDS_PREVIEW: // Builds Preview menu and starts preview mode
            IMENUCTL_SetActive(pMe->m_pIPreview, FALSE);
            //lint -e570  Suppress complaint on loss of sign
            IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
            //lint +e570

            // TODO fix this, find a way to wait until the graph task
            // processes the camere start request, then call Qcamcorder_Preview.

            // Let the graph task process the camere start request
            // before calling Qcamcorder_Preview
            (void)ISHELL_SetTimer(pMe->a.m_pIShell, 500, (PFNNOTIFY)Qcamcorder_Preview,(void *)pMe);

            return TRUE;
         default:
            return FALSE;

         }
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_POST_EDIT)
      {
        if(pMe->eState == QCAMCORDER_POSTEDIT)
        {
          Qcamcorder_ExecutePostEditMenu(pMe);
        }
        return TRUE;
      }
      else if(pMe->eState == QCAMCORDER_POSTEDIT &&
          pMe->m_activeMenu != QCAMCORDER_MENU_GPS)
      {
        Qcamcorder_BuildPostEditMenu(pMe);
      }

      else if(pMe->m_activeSoftMenu == QCAMCORDER_MENU_EDIT)
      {
         switch(wParam)
         {
           case IDS_QCAMCORDER_YES:
           case IDS_QCAMCORDER_NO:
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
             if(pMe->eState == QCAMCORDER_VIDEO_EDIT_DONE
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
                || pMe->eState == QCAMCORDER_CUT_MERGE_DONE
#endif
#ifdef FEATURE_MP4_TRANSCODER
                || pMe->eState == QCAMCORDER_TRANSPORTION
#endif
               )
             {
               Qcamcorder_EditFileResponse(pMe, wParam);
             }
             else if(pMe->eState == QCAMCORDER_VIDEO_EDIT_DONE_ERROR ||
                     pMe->eState == QCAMCORDER_VIDEO_EDIT_UNSUPPORTED )
             {
               if(IFILEMGR_Remove(pMe->pIFileMgr, pMe->sVideoEditFileName)!= SUCCESS)
               {
                 QCAMCORDER_MSG_ERROR("Error in removing file",0,0,0);
               }

               /* Display bitmap to user regarding cut feature error */
               pMe->msg = (pMe->eState == QCAMCORDER_VIDEO_EDIT_DONE_ERROR) ?
                          IDB_POSTEDIT_ERROR : IDB_POSTEDIT_UNSUPPORTED;

               Qcamcorder_ErrorEventUserNotify(pMe);
             }
             else
             {
#endif
               Qcamcorder_DeleteFileResponse(pMe, wParam);
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
             }
#endif
             break;

         case IDS_QCAMCORDER_DELETE:
             Qcamcorder_ExecuteDeleteMenu(pMe);
            break;
           default:
            break;
         }

         return TRUE;
      }

      else if(pMe->m_activeMenu == QCAMCORDER_MENU_SOUND)
      {
         Qcamcorder_ExecuteSoundMenu(pMe);
         return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_SPECIAL_EFFECTS)
      {
        Qcamcorder_ExecuteSpecialEffectsMenu(pMe);
        return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_SIZE)
      {
         Qcamcorder_ExecuteSizeMenu(pMe);
         return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_QUALITY)
      {
         Qcamcorder_ExecuteQualityMenu(pMe);
         return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_AUDIO_FORMAT)
      {
         Qcamcorder_ExecuteAudioFormatMenu(pMe);
         return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_SAVE_LOC)
      {
         Qcamcorder_ExecuteSaveLocMenu(pMe);
         return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_NUMCLIPS)
      {
         Qcamcorder_ExecuteNumClipsMenu(pMe);
         return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_CLIPLENGTH)
      {
         Qcamcorder_ExecuteClipLengthMenu(pMe);
         return TRUE;
      }
      else if (pMe->m_activeMenu == QCAMCORDER_MENU_FRAMERATE)
      {
          Qcamcorder_ExecuteFrameRateMenu(pMe);
          return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_WHITEBALANCE)
      { Qcamcorder_ExecuteWhiteBalanceMenu(pMe);
        return TRUE;
      }
      else if (pMe->m_activeMenu ==QCAMCORDER_MENU_FILE_FORMAT)
      { Qcamcorder_ExecuteFileFormatMenu(pMe);
        return TRUE;
      }
      else if (pMe->m_activeMenu ==QCAMCORDER_MENU_VIDEO_FORMAT)
      { Qcamcorder_ExecuteVideoFormatMenu(pMe);
        return TRUE;
      }
#ifdef FEATURE_MP4_TRANSCODER
      else if( pMe->m_activeMenu ==QCAMCORDER_MENU_TRANSCODER_AUDIO_FORMAT )
      {
        Qcamcorder_ExecuteTranscoderAudioMenu(pMe);
        return TRUE;
      }
      else if( pMe->m_activeMenu ==QCAMCORDER_MENU_TRANSCODER_BRAND )
      {
        Qcamcorder_ExecuteTranscoderBrandMenu(pMe);
        return TRUE;
      }
      else if( pMe->m_activeMenu ==QCAMCORDER_MENU_TRANSCODER_VIDEO_SIZE )
      {
        Qcamcorder_ExecuteTranscoderVideoMenu(pMe);
        return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_TRANSCODER_VIDEO_FORMAT )
      {
        Qcamcorder_ExecuteTranscoderVideoCodecMenu(pMe);
        return TRUE;
      }
#endif
#ifdef FEATURE_DVS
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_DVSMARGIN )
      {
        Qcamcorder_ExecuteDVSMarginMenu(pMe);
        return TRUE;
      }
#endif
#ifdef FEATURE_QCAMCORDER_TELOP
      else if (pMe->m_activeMenu ==QCAMCORDER_MENU_TELOP)
      { Qcamcorder_ExecuteTelopMenu(pMe);
        return TRUE;
      }
#endif
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_CONT)
      {Qcamcorder_ExecuteContMenu(pMe);
       return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_TIMER)
      {Qcamcorder_ExecuteTimerMenu(pMe);
       return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_EFFECTS)
      {Qcamcorder_ExecuteEffectsMenu(pMe);
       return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_ANTIBANDING)
      {Qcamcorder_ExecuteAntibandingMenu(pMe);
       return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_ROTATE)
      {Qcamcorder_ExecuteRotateMenu(pMe);
       return TRUE;
      }
#ifdef FEATURE_CAMCORDER_AAC_REC
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_AAC_CONFIG)
      {Qcamcorder_ExecuteAACConfigMenu(pMe);
       return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_AAC_MODE)
      {Qcamcorder_ExecuteAACModeMenu(pMe);
       return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_AAC_FREQ)
      {Qcamcorder_ExecuteAACFreqMenu(pMe);
       return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_AAC_BITRATE)
      {Qcamcorder_ExecuteAACBitrateMenu(pMe);
       return TRUE;
      }
#endif /* FEATURE_CAMCORDER_AAC_REC */
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_SPACE_THRESHOLD)
      {Qcamcorder_ExecuteSpaceMenu(pMe);
       return TRUE;
      }
      else if(pMe->m_activeMenu == QCAMCORDER_MENU_DELETEALL)
      {
       switch(Qcamcorder_ExecuteSubMenu(pMe, DeleteallIDList))
       {
        case IDS_QCAMCORDER_YES:
        case IDS_FLASH_DELETEALL:
        case IDS_MMC_DELETEALL:
#ifdef FEATURE_EFS_FTL
        case IDS_FTL_DELETEALL: 
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
        case IDS_USB_DELETEALL: 
#endif
           switch(wParam)
            { case IDS_DELETEALL:
                if(Qcamcorder_GetFilecount(pMe) >0){
                  Qcamcorder_ExecuteDeleteMenu(pMe);
                  }
               break;
              case IDS_QCAMCORDER_YES:
                    if(Qcamcorder_DeleteAllClips(pMe))
                    {Qcamcorder_DrawIcon(pMe, IDB_QCAMCORDER_ERASED);
                    }
                    else
                    {Qcamcorder_DrawIcon(pMe, IDB_QCAMCORDER_NO);
                    }
                    (void)IMENUCTL_Redraw(pMe->m_pISettings);
                    (void)IMENUCTL_Redraw(pMe->m_pIMenu);
                    IDISPLAY_Update(pMe->a.m_pIDisplay);
                break;
              case IDS_QCAMCORDER_NO:
                    Qcamcorder_DrawIcon(pMe, IDB_QCAMCORDER_NO);
                    (void)IMENUCTL_Redraw(pMe->m_pISettings);
                    (void)IMENUCTL_Redraw(pMe->m_pIMenu);
                    IDISPLAY_Update(pMe->a.m_pIDisplay);
               break;
              default:
                QCAMCORDER_MSG_HIGH("Case %d not handled",wParam,0,0);
               break;
            }
        default:
         break;
           }
      }
      break;

    case EVT_USER:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

     switch( wParam )
     {

#ifdef FEATURE_MP4_TRANSCODER
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
     //Transcoder specific events
     case IMOVIEEDIT_TRANSCODER_STATE_ERROR:
     //Recived error, end the transcoder
      //lint -e570  Suppress complaint on loss of sign
      Qcamcorder_DrawIcon(pMe, IDB_ERROR_QCAMCORDER);
      //lint +e570
      IDISPLAY_Update(pMe->a.m_pIDisplay);
      break;

     case IMOVIEEDIT_TRANSCODER_STATE_UNSUPPORTED:
      //lint -e570  Suppress complaint on loss of sign
      Qcamcorder_DrawIcon(pMe, IDB_POSTEDIT_UNSUPPORTED);
      //lint +e570
      IDISPLAY_Update(pMe->a.m_pIDisplay);
      break;

     case IMOVIEEDIT_TRANSCODER_STATE_DONE:
      //Transcoder is done with no errors,
      //clear the screen
      //lint -e570  Suppress complaint on loss of sign
      IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
      //lint +e570
      IDISPLAY_Update(pMe->a.m_pIDisplay);

#ifdef FEATURE_DVS
      if(pMe->bDVS)
        Qcamcorder_DrawIcon(pMe, IDB_DVS_DONE);
      else
#endif
        Qcamcorder_DrawIcon(pMe, IDB_TRANSCODER_DONE);
       break;

     case IMOVIEEDIT_TRANSCODER_STATE_TERMINATED:
      if((pMe->bTranscode_portion) && 
          (pMe->eState == QCAMCORDER_TRANSCODE_DONE))
      {
          pMe->eState             = QCAMCORDER_TRANSPORTION;
          pMe->bTranscode_portion = FALSE;
          pMe->nStartFrameNo      = 0;
          pMe->nEndFrameNo        = 0;
          Qcamcorder_ExecuteConfirmMenu(pMe);
      }
      else
      {
          pMe->eState = QCAMCORDER_EDIT;
          Qcamcorder_DisplayAlbum(pMe);
      }
      break;

     case IMOVIEEDIT_TRANSCODER_STATE_AUDIO_DONE:
       pMe->eState = QCAMCORDER_TRANSCODE;
       Qcamcorder_DrawIcon(pMe, IDB_TRANSAUDIO_DONE);
       break;
     case IMOVIEEDIT_TRANSCODER_STATE_AUDIO_START:
       pMe->eState = QCAMCORDER_TRANSCODE;
       Qcamcorder_DrawIcon(pMe, IDB_TRANSAUDIO_START);
       break;
     case IMOVIEEDIT_TRANSCODER_STATE_VIDEO_START:
       pMe->eState = QCAMCORDER_TRANSCODE;
       Qcamcorder_DrawIcon(pMe, IDB_TRANSVIDEO_START);
       break;
#else
         //Transcoder specific events
      case IMOVIEFILE_TRANSCODER_STATE_ERROR:
      //Recived error, end the transcoder
       //lint -e570  Suppress complaint on loss of sign
       //IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
       if(pMe->eState != QCAMCORDER_EDIT)
       {
         Qcamcorder_DrawIcon(pMe, IDB_ERROR_QCAMCORDER);
         //lint +e570
         IDISPLAY_Update(pMe->a.m_pIDisplay);
         Qcamcorder_Transcoder_End(pMe);
         pMe->eState = QCAMCORDER_EDIT;
       }
       //Qcamcorder_BuildEditMenu(pMe);
       (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1000,
               (PFNNOTIFY) Qcamcorder_DisplayAlbum, (void*)pMe);
      break;

     case IMOVIEFILE_TRANSCODER_STATE_UNSUPPORTED:
       //lint -e570  Suppress complaint on loss of sign
       Qcamcorder_DrawIcon(pMe, IDB_POSTEDIT_UNSUPPORTED);
       //lint +e570
       IDISPLAY_Update(pMe->a.m_pIDisplay);
       Qcamcorder_Transcoder_End(pMe);
       pMe->eState = QCAMCORDER_EDIT;
       (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1000,
           (PFNNOTIFY) Qcamcorder_DisplayAlbum, (void*)pMe);
       break;

      case IMOVIEFILE_TRANSCODER_STATE_VIDEO_CONT:
       if(pMe->eState != QCAMCORDER_EDIT)
       {
         //Display the frame on the screen
         stat = (xcoder_status_type *) dwParam;
         Qcamcorder_DisplayFrame(pMe, &(stat->video));
       }
       break;

      case IMOVIEFILE_TRANSCODER_STATE_DONE:
       //Transcoder is done with no errors,
       //clear the screen
       //lint -e570  Suppress complaint on loss of sign
       IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
       //lint +e570
       IDISPLAY_Update(pMe->a.m_pIDisplay);
       if( pMe->pOutFile )
       {
         QCAMCORDER_MSG_HIGH("Closing output file",0,0,0);
         (void)IMOVIEFILE_Release(pMe->pOutFile);
         pMe->pOutFile = NULL;
       }

       if( pMe->pInFile )
       {
         QCAMCORDER_MSG_HIGH("Closing output file",0,0,0);
         (void)IMOVIEFILE_Release(pMe->pInFile);
         pMe->pInFile = NULL;
       }

       pMe->eState = QCAMCORDER_EDIT;
#ifdef FEATURE_DVS
       if(pMe->bDVS)
         Qcamcorder_DrawIcon(pMe, IDB_DVS_DONE);
       else
#endif
         Qcamcorder_DrawIcon(pMe, IDB_TRANSCODER_DONE);
       if(pMe->bTranscode_portion)
       {
           pMe->eState             = QCAMCORDER_TRANSPORTION;
           pMe->bTranscode_portion = FALSE;
           pMe->nStartFrameNo      = 0;
           pMe->nEndFrameNo        = 0;
           (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1000,
             (PFNNOTIFY) Qcamcorder_ExecuteConfirmMenu, (void*)pMe);
       }
       else
       {
       (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1000,
                       (PFNNOTIFY) Qcamcorder_DisplayAlbum, (void*)pMe);
       }
       break;

      case IMOVIEFILE_TRANSCODER_STATE_AUDIO_DONE:
        pMe->eState = QCAMCORDER_TRANSCODE;
        Qcamcorder_DisplayAlbum(pMe);
        Qcamcorder_DrawIcon(pMe, IDB_TRANSAUDIO_DONE);
        break;
      case IMOVIEFILE_TRANSCODER_STATE_AUDIO_START:
        pMe->eState = QCAMCORDER_TRANSCODE;
        Qcamcorder_DisplayAlbum(pMe);
        Qcamcorder_DrawIcon(pMe, IDB_TRANSAUDIO_START);
        break;
      case IMOVIEFILE_TRANSCODER_STATE_VIDEO_START:
        pMe->eState = QCAMCORDER_TRANSCODE;
        Qcamcorder_DisplayAlbum(pMe);
        Qcamcorder_DrawIcon(pMe, IDB_TRANSVIDEO_START);
        break;
      //no processing required at this level
      case IMOVIEFILE_TRANSCODER_STATE_AUDIO_FRAME_DONE:
      case IMOVIEFILE_TRANSCODER_STATE_VIDEO_FRAME_DONE:
      case IMOVIEFILE_TRANSCODER_STATE_INIT_DONE:
      case IMOVIEFILE_TRANSCODER_STATE_AUDIO_FRAME_RESET:
      case IMOVIEFILE_TRANSCODER_STATE_VIDEO_FRAME_REQUEST:
      case IMOVIEFILE_TRANSCODER_STATE_AUDIO_FRAME_REQUEST:
        break;

#endif
#endif /* FEATURE_MP4_TRANSCODER */

        /* Events for raw encoding */
#ifdef FEATURE_VIDEO_ENCODE_RAW

        case IMOVIEFILE_RAW_ENCODER_STATE_ERROR:
          QCAMCORDER_MSG_ERROR("Raw Encoder exit with errors",0,0,0);
          /* Fall through to deallocate memory */

        case IMOVIEFILE_RAW_ENCODER_STATE_COMPLETE:
          // Raw Encoder completed with no errors, clear screen */
          //lint -e570  Suppress complaint on loss of sign
          IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
          //lint +e570
          IDISPLAY_Update(pMe->a.m_pIDisplay);
          if( pMe->pRawStopFile )
          {
            QCAMCORDER_MSG_HIGH("Releasing RawStopFile",0,0,0);
            (void)IMOVIEFILE_Release(pMe->pRawStopFile);
            pMe->pRawStopFile = NULL;
          }

          if( pMe->pRawStartFile )
          {
            QCAMCORDER_MSG_HIGH("Releasing RawStartFile",0,0,0);
            (void)IMOVIEFILE_Release(pMe->pRawStartFile);
            pMe->pRawStartFile = NULL;
          }

          /* Raw Encoder complete, return to UI */
          pMe->eState = QCAMCORDER_START_RAW_ENCODE;
          Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);
          break;

        case IMOVIEFILE_RAW_ENCODER_STATE_FRAME_DISPLAY:
          /* Display raw frame here */
          info = (raw_encoder_info_type *) dwParam;
          Qcamcorder_DisplayRawFrame(pMe, &(info->video));
          QCAMCORDER_MSG_HIGH("UI Raw Frame #%d displayed",info->video.currentFrame,0,0);
          break;

 #endif /* FEATURE_VIDEO_ENCODE_RAW */
      case QCC_EXECUTE_CONFIRM_MENU_EVENT:
          Qcamcorder_ShowconfirmMenu(pMe);
          break;
      default:
       break;
     }

    return TRUE;

   default:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else //FEATURE_APP_TEST_AUTOMATION
     break; /* For EVT cases */
#endif //FEATURE_APP_TEST_AUTOMATION
   }

   return FALSE;
}



#ifdef FEATURE_MP4_TRANSCODER
static void Qcamcorder_DisplayAlbum(Qcamcorder* pMe)
{
  //lint -e570  Suppress complaint on loss of sign
  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
  //lint +e570
  IDISPLAY_Update(pMe->a.m_pIDisplay);
  Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);
}
#endif
/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTESUBMENU
DESCRIPTION Updates the bullets in a menu to reflect the new focus.
============================================================================= */
static uint16 Qcamcorder_ExecuteSubMenu(Qcamcorder* pMe, uint16 * idList)
{
   CtlAddItem pai;
   uint16 i;
   uint16 selection = IMENUCTL_GetSel(pMe->m_pIMenu);

   // Update the bullets
   for (i = 0; i < idList[0]; ++i)
   {
      if(IMENUCTL_GetItem(pMe->m_pIMenu, idList[i+2], &pai))
      {
         pai.pszResImage = pai.pszResText = QCAMCORDER_RES_FILE;
         pai.pImage = NULL;
         pai.wImage = (uint16)(idList[i+2] == selection ? IDB_RADIO_ON : IDB_RADIO_OFF);
         (void)IMENUCTL_SetItem(pMe->m_pIMenu, idList[i+2], MSIF_IMAGE, &pai);
      }
   }

   (void)IMENUCTL_Redraw(pMe->m_pIMenu);
   return selection;
}


/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTESIZESUBMENU
DESCRIPTION Updates the bullets in the size menu to reflect the new focus.
============================================================================= */
static uint16 Qcamcorder_ExecuteSizeSubMenu(Qcamcorder* pMe, uint16 * idList)
{
  CtlAddItem pai;
  uint16 i = 0;
  uint16 selection = IMENUCTL_GetSel(pMe->m_pIMenu);
  CtlAddItem preview_pai;
  boolean QVGA_preview_selected = FALSE;
  char         preview_string[QVGA_PREVIEW_LEN] = {0};
  char         preview_teststring[QVGA_PREVIEW_LEN] = {0};

  // Determine if display preview size menu option is chosen
  (void)IMENUCTL_GetItem(pMe->m_pIMenu, selection, &preview_pai);
  (void)WSTRTOSTR(preview_pai.pText,preview_string,sizeof(QVGA_PREVIEW_STRING));
  QVGA_preview_selected = !STRCMP(preview_string,QVGA_PREVIEW_STRING);

  for (; i < idList[0]; ++i)
  {
    if(IMENUCTL_GetItem(pMe->m_pIMenu, idList[i+2], &pai))
    {
      (void)WSTRTOSTR(pai.pText,preview_teststring,sizeof(QVGA_PREVIEW_STRING));

      if(QVGA_preview_selected && selection == idList[i+2])
      {
        pai.pImage = NULL;
        /* Time stamp selected. Current loop focus: modify time stamp item */
        if(pai.wImage != IDB_CHECK_ON &&
           pMe->m_displaySize == QCAMCORDER_DISPLAY_MENU_SELECT)
          pai.wImage = (uint16)(IDB_CHECK_ON);
        else
          pai.wImage = (uint16)(IDB_CHECK_OFF);

        (void)IMENUCTL_SetItem(pMe->m_pIMenu, idList[i+2], MSIF_IMAGE, &pai);

      }
      else if (STRCMP(preview_teststring,QVGA_PREVIEW_STRING) && !QVGA_preview_selected)
      {
        /* Do not update MM bullet unless SH disabled */
        if(!(pMe->wShortHeader != QCAMCORDER_SH_DISABLE && i == 3))
        {
          pai.pszResImage = pai.pszResText = QCAMCORDER_RES_FILE;
          pai.pImage = NULL;
          pai.wImage = (uint16)(idList[i+2] == selection ? IDB_RADIO_ON : IDB_RADIO_OFF);
          (void)IMENUCTL_SetItem(pMe->m_pIMenu, idList[i+2], MSIF_IMAGE, &pai);
        }
      }
    }
  }

  (void)IMENUCTL_Redraw(pMe->m_pIMenu);

  return selection;
}

/* ==========================================================================
FUNCTION Qcamcorder_ExecutePostEditSubMenu
DESCRIPTION Updates the bullets in a menu to reflect the new focus.
============================================================================= */
static uint16 Qcamcorder_ExecutePostEditSubMenu(Qcamcorder* pMe, uint16 * idList)
{
   CtlAddItem pai;
   uint16 i;
   uint16 selection = IMENUCTL_GetSel(pMe->m_pIEditMenu);

   // Update the bullets
   for (i = 0; i < idList[0]; ++i)
   {
      if(IMENUCTL_GetItem(pMe->m_pIEditMenu, idList[i+2], &pai))
      {
         pai.pszResImage = pai.pszResText = QCAMCORDER_RES_FILE;
         pai.pImage = NULL;
         pai.wImage = (uint16)(idList[i+2] == selection ? IDB_RADIO_ON : IDB_RADIO_OFF);
         (void)IMENUCTL_SetItem(pMe->m_pIEditMenu, idList[i+2], MSIF_IMAGE, &pai);
      }
   }

   (void)IMENUCTL_Redraw(pMe->m_pIEditMenu);
   return selection;
}

/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTESOUNDMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteSoundMenu(Qcamcorder* pMe)
{
   CtlAddItem pai;
   FileInfo fi;
   int count = 0;
   uint16 sel = 1;
   IFileMgr *pIFileMgr;
   uint16 selection = IMENUCTL_GetSel(pMe->m_pIMenu);
   uint16 max_file_count = 0;

   if (selection == IDS_NO_SOUND)
   {
      (void)STRLCPY(pMe->sStartSoundFile, "No Sound", sizeof(pMe->sStartSoundFile));
   }
   else
   {
      if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr) != SUCCESS)
       return;

      (void)IFILEMGR_EnumInit(pIFileMgr, QCAMCORDER_SOUNDS_DIR, FALSE);
      while (IFILEMGR_EnumNext(pIFileMgr, &fi))
      {
         QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);
         if(++count == selection)
         {
            (void)STRLCPY(pMe->sStartSoundFile, fi.szName, sizeof(pMe->sStartSoundFile));
            break;
         }
      }
      (void)IFILEMGR_Release(pIFileMgr);
      pIFileMgr = NULL;
   }

   do
   {
      if(IMENUCTL_GetItem(pMe->m_pIMenu, sel, &pai))
      {
         pai.pszResImage = pai.pszResText = QCAMCORDER_RES_FILE;
         pai.pImage = NULL;
         pai.wImage = (uint16)(sel == selection ? IDB_RADIO_ON : IDB_RADIO_OFF);
         (void)IMENUCTL_SetItem(pMe->m_pIMenu, sel, MSIF_IMAGE, &pai);
         sel++;
      }
      else
      {
         sel = 0;
      }
   } while (sel != 0);

   if (selection == IDS_NO_SOUND)
   {
      (void)IMENUCTL_GetItem(pMe->m_pIMenu, IDS_NO_SOUND, &pai);
      pai.pszResImage = pai.pszResText = QCAMCORDER_RES_FILE;
      pai.pImage = NULL;
      pai.wImage = IDB_RADIO_ON;
      (void)IMENUCTL_SetItem(pMe->m_pIMenu, (uint16)selection, MSIF_IMAGE, &pai);
   }

   if (selection != IDS_NO_SOUND)
   {
      (void)IMENUCTL_GetItem(pMe->m_pIMenu, IDS_NO_SOUND, &pai);
      pai.pszResImage = pai.pszResText = QCAMCORDER_RES_FILE;
      pai.pImage = NULL;
      if (pai.wImage == IDB_RADIO_ON)
        pai.wImage = IDB_RADIO_OFF;
      (void)IMENUCTL_SetItem(pMe->m_pIMenu, (uint16)IDS_NO_SOUND, MSIF_IMAGE, &pai);
   }

   if (selection != IDS_NO_SOUND)
     Qcamcorder_PlayStartSound(pMe);

   (void)IMENUCTL_Redraw(pMe->m_pIMenu);
   return;
} /* END Qcamcorder_ExecuteSoundMenu */



/* ==========================================================================
FUNCTION Qcamcorder_ExecuteSpecialEffectsMenu
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteSpecialEffectsMenu(Qcamcorder* pMe)
{
  uint16 selection = IMENUCTL_GetSel(pMe->m_pIMenu);

  switch(selection)
  {
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */

#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
    case IDS_ENABLE_FRAMEOVERLAY:
    { 
      pMe->bEnableFrameOverlay = !pMe->bEnableFrameOverlay; 
      if(pMe->bEnableFrameOverlay) 
      { 
        (void)STRLCPY(pMe->sFrameOverlay, ENABLE_FRAMEOVERLAY_STRING, sizeof(pMe->sFrameOverlay)); 
      } 
      else 
      { 
        (void)STRLCPY(pMe->sFrameOverlay, DISABLE_FRAMEOVERLAY_STRING, sizeof(pMe->sFrameOverlay)); 
      } 
      pMe->bUpdatePreviewFrame = TRUE; 
    }
    break;
#endif /* FEATURE_VIDEO_ENCODE_OVERLAY_UI */

#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
    case IDS_TIME_STAMP: 
    {                  
      pMe->bEnableTimeStamp = !pMe->bEnableTimeStamp; 
      ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_FRAME_TIMESTAMP, (int32)pMe->bEnableTimeStamp,0);
    }
    break;
#endif /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/  

#ifdef FEATURE_VIDEO_ENCODE_FADING
    case IDS_FADE_IN:
    { 
      int16 cameraFadeEffect = 0; 
      if(pMe->mFadeInOut == QCAMCORDER_FADE_OUT) 
      { 
        pMe->mFadeInOut = QCAMCORDER_FADE_IN_FADE_OUT; 
        cameraFadeEffect = CAM_FADE_IN_OUT; 
      } 
      else if(pMe->mFadeInOut == QCAMCORDER_FADE_IN) 
      { 
        pMe->mFadeInOut = QCAMCORDER_FADE_OFF; 
        cameraFadeEffect = CAM_FADE_OFF; 
      } 
      else if(pMe->mFadeInOut == QCAMCORDER_FADE_OFF) 
      { 
        pMe->mFadeInOut = QCAMCORDER_FADE_IN; 
        cameraFadeEffect = CAM_FADE_IN; 
      } 
      else if (pMe->mFadeInOut == QCAMCORDER_FADE_IN_FADE_OUT)
      {
        pMe->mFadeInOut = QCAMCORDER_FADE_OUT;
        cameraFadeEffect = CAM_FADE_OUT;
      }
      ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_FADE, cameraFadeEffect, 0); 
    }
    break;
      
    case IDS_FADE_OUT: 
    { 
      int16 cameraFadeEffect = 0; 
      if(pMe->mFadeInOut == QCAMCORDER_FADE_OUT) 
      { 
        pMe->mFadeInOut = QCAMCORDER_FADE_OFF; 
        cameraFadeEffect = CAM_FADE_OFF; 
      } 
      else if(pMe->mFadeInOut == QCAMCORDER_FADE_IN) 
      { 
        pMe->mFadeInOut = QCAMCORDER_FADE_IN_FADE_OUT; 
        cameraFadeEffect = CAM_FADE_IN_OUT; 
      } 
      else if(pMe->mFadeInOut == QCAMCORDER_FADE_OFF) 
      { 
        pMe->mFadeInOut = QCAMCORDER_FADE_OUT; 
        cameraFadeEffect = CAM_FADE_OUT; 
      } 
      else if(pMe->mFadeInOut == QCAMCORDER_FADE_IN_FADE_OUT)
      {
        pMe->mFadeInOut = QCAMCORDER_FADE_IN;
        cameraFadeEffect = CAM_FADE_IN;
      }
      ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_FADE, cameraFadeEffect, 0); 
    }
    break;
#endif /* FEATURE_VIDEO_ENCODE_FADING */

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
    case IDS_ALPHA_FADE: 
    {
      pMe->mSpecialFade = QCAMCORDER_SPECIAL_FADE_ALPHA; 
      ICAMERA_SetParm(pMe->m_pICam,CAM_PARM_TRANSITION,CAM_TRANSITION_ALPHA,0); 
    }
    break;

    case IDS_CURTAIN_FADE:
    {
      pMe->mSpecialFade = QCAMCORDER_SPECIAL_FADE_CURTAIN;
      ICAMERA_SetParm(pMe->m_pICam,CAM_PARM_TRANSITION, CAM_TRANSITION_CURTAIN,0);
    }
    break;

    case IDS_NONE:
    {
      pMe->mSpecialFade = QCAMCORDER_SPECIAL_FADE_NONE;
      ICAMERA_SetParm(pMe->m_pICam,CAM_PARM_TRANSITION, CAM_TRANSITION_OFF,0);
    }
    break;

    default:
      QCAMCORDER_MSG_ERROR("Not able to detect the changes at frameoverlay menu!!!",0,0,0);
      return;
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */
  }
  Qcamcorder_BuildSpecialEffectsMenu(pMe, selection);
  return;
} /* END Qcamcorder_ExecuteSpecialEffectsMenu */



/* ==========================================================================
FUNCTION QCAM_EXECUTESIZEMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteSizeMenu(Qcamcorder* pMe)
{
   uint32 disable_overlay = 0;
   switch (Qcamcorder_ExecuteSizeSubMenu(pMe, SizeIDList))
   {
     case IDS_SIZE_QCIF:
       pMe->eSize = QCAMCORDER_SIZE_QCIF;
       break;
     case IDS_SIZE_SQCIF:
       pMe->eSize = QCAMCORDER_SIZE_SQCIF;
       break;
     case IDS_SIZE_MM:
       pMe->eSize = QCAMCORDER_SIZE_MM;
       break;
#ifndef FEATURE_QVGANOTSUPPORTED
     case IDS_SIZE_QVGA:
       pMe->eSize = QCAMCORDER_SIZE_QVGA;
#ifdef FEATURE_QVGA_LOW_FPS_ONLY
       pMe->m_FrameRate = QCAMCORDER_FRAME_RATE_1;
#endif /* FEATURE_QVGA_LOW_FPS_ONLY */
       break;
#endif //#ifndef FEATURE_QVGANOTSUPPORTED
     case IDS_SIZE_DISPLAY:
       if(pMe->m_displaySize == QCAMCORDER_DISPLAY_MENU_SELECT)
         pMe->m_displaySize = QCAMCORDER_DISPLAY_FULL_SCREEN;
       else
         pMe->m_displaySize = QCAMCORDER_DISPLAY_MENU_SELECT;
       break;
     default:
       pMe->eSize = QCAMCORDER_SIZE_MM;
       break;
   }

   if (pMe->eSize <= QCAMCORDER_SIZE_MM)
   {
     (void)ICAMERA_SetSize(pMe->m_pICam, &QCam_Sizes[pMe->eSize]);

     if (pMe->m_displaySize == QCAMCORDER_DISPLAY_FULL_SCREEN)
     {
       (void)ICAMERA_SetDisplaySize(pMe->m_pICam, &QCam_Sizes[QCAMCORDER_SIZE_QVGA]);
       if( pMe->eSize != QCAMCORDER_SIZE_QVGA)
       {
         disable_overlay = 1;
       }		 
     } 
     else
       (void)ICAMERA_SetDisplaySize(pMe->m_pICam, &QCam_Sizes[pMe->eSize]);
   }
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  (void)Qcamcorder_SetDirectMode(pMe, TRUE);
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE

#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
   if(pMe->bEnableFrameOverlay && disable_overlay)
   {
	//so disabling the overlay
    (void)STRLCPY(pMe->sFrameOverlay, DISABLE_FRAMEOVERLAY_STRING, sizeof(pMe->sFrameOverlay));
    pMe->bEnableFrameOverlay = FALSE;
   }
#endif

   return;
} /* END QCam_ExecuteSizeMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTEQUALITYMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteQualityMenu(Qcamcorder* pMe)
{
  switch (Qcamcorder_ExecuteSubMenu(pMe, QualityIDList))
  {
    case IDS_QUALITY_LOW:
      pMe->wQuality = QCAMCORDER_QUALITY_LOW;
      break;
    case IDS_QUALITY_MED:
      pMe->wQuality = QCAMCORDER_QUALITY_MED;
      break;
    case IDS_QUALITY_HIGH:
      pMe->wQuality = QCAMCORDER_QUALITY_HIGH;
      break;
#ifdef FEATURE_VIDEO_ENCODE_BITRATE
    case IDS_QUALITY_USER:
      pMe->wQuality = QCAMCORDER_QUALITY_USER;

      /* Get the user specifed bitrate using IDIALOG window */
      if (ISHELL_CreateDialog(pMe->a.m_pIShell, QCAMCORDER_RES_FILE,
                           IDD_GET_BITRATE, NULL) == AEE_SUCCESS)
      {
        IDialog * dlg = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        ITextCtl * ctl = (ITextCtl*)IDIALOG_GetControl(dlg, (int16)IDI_GET_BITRATE);
        uint32 sel = TC_MAKE_SEL(TC_SEL_START, TC_SEL_END);
        ITEXTCTL_SetSelection(ctl, sel);
      }
      else
      {
        QCAMCORDER_MSG_ERROR("Failed to create dialog window for bitrate",0,0,0);
      }
      break;
#endif
    case IDS_QUALITY_MAX:
      pMe->wQuality = QCAMCORDER_QUALITY_MAX;
      break;
    case IDS_QUALITY_DYNAMIC:
      pMe->wQuality = QCAMCORDER_QUALITY_DYNAMIC;
      break;
    default:
      pMe->wQuality = QCAMCORDER_QUALITY_LOW;
      break;
  }

  (void)ICAMERA_SetQuality(pMe->m_pICam, pMe->wQuality);

  return;
} /* END QCam_ExecuteQualityMenu */


/* ==========================================================================
FUNCTION QCAM_EXECUTEAUDIOFORMATMENU
DESCRIPTION
============================================================================= */

static void Qcamcorder_ExecuteAudioFormatMenu(Qcamcorder* pMe)
{

   switch (Qcamcorder_ExecuteSubMenu(pMe, AudioFormatIDList))
   {
   case IDS_AUDIO_FORMAT_13K_FULL:
      pMe->wAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K;
      break;
   case IDS_AUDIO_FORMAT_13K_HALF:
      pMe->wAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K;
      break;
   case IDS_AUDIO_FORMAT_EVRC:
      pMe->wAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_EVRC;
      break;
   case IDS_AUDIO_FORMAT_AMR:
      pMe->wAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR;
      break;
#ifdef FEATURE_CAMCORDER_AAC_REC
     case IDS_AUDIO_FORMAT_AAC:
      pMe->wAudioFormat = QCAMCORDER_AUDIO_FORMAT_VAR_AAC;
       break;
#endif /* FEATURE_CAMCORDER_AAC_REC */
   case IDS_NONE:
     default:
      pMe->wAudioFormat = QCAMCORDER_AUDIO_FORMAT_NONE;
      break;
   }
  /*Set the class ID of audio format*/
  if (pMe->wAudioFormat == QCAMCORDER_AUDIO_FORMAT_NONE)
   {
    pMe->wAudioFormatClsId = 0;                 /* No Audio */
    (void)ICAMERA_SetAudioEncode(pMe->m_pICam, 0, 0);
   }
#ifdef FEATURE_CAMCORDER_AAC_REC
  else if (pMe->wAudioFormat == QCAMCORDER_AUDIO_FORMAT_VAR_AAC)
  {
    pMe->wAudioFormatClsId = AEECLSID_MEDIAAAC; /* AAC audio */
    (void)ICAMERA_SetAudioEncode(pMe->m_pICam, pMe->wAudioFormatClsId,
                                 pMe->m_AACInfo);
  }
#endif /* FEATURE_CAMCORDER_AAC_REC */
  else
  {
    pMe->wAudioFormatClsId = AEECLSID_MEDIAQCP; /* QCELP audio */
    (void)ICAMERA_SetAudioEncode(pMe->m_pICam, pMe->wAudioFormatClsId,
                                 pMe->wAudioFormat);
  }

  return;
} /* END QCam_ExecuteAudioFormatMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTESAVELOCMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteSaveLocMenu(Qcamcorder* pMe)
{
   switch (Qcamcorder_ExecuteSubMenu(pMe, SaveLocIDList))
   {
   case IDS_FLASH:
      pMe->saveLoc = QCAMCORDER_SAVE_FLASH;
      break;
   case IDS_MMC:
      pMe->saveLoc = QCAMCORDER_SAVE_MMC;
      break;
#ifdef FEATURE_EFS_FTL
   case IDS_FTL:
      pMe->saveLoc = QCAMCORDER_SAVE_FTL;
      break;
#endif /* FEATURE_EFS_FTL */

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
   case IDS_USB:
      pMe->saveLoc = QCAMCORDER_SAVE_USB;
      break;
#endif /* FEATURE_EFS_HOTPLUG_USB_HOST_MS */
   default:
      pMe->saveLoc = QCAMCORDER_SAVE_FLASH;
      break;

   }

   return;
} /* END QCam_ExecuteSaveLocMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTENUMCLIPSMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteNumClipsMenu(Qcamcorder* pMe)
{
   switch (Qcamcorder_ExecuteSubMenu(pMe, NumClipsIDList))
   {
   case IDS_NUMCLIPS_LOW:
      pMe->m_maxNumClips = QCAMCORDER_NUMCLIPS_LOW;
      break;
   case IDS_NUMCLIPS_MED:
      pMe->m_maxNumClips = QCAMCORDER_NUMCLIPS_MED;
      break;
   case IDS_NUMCLIPS_HIGH:
      pMe->m_maxNumClips = QCAMCORDER_NUMCLIPS_HIGH;
      break;
   case IDS_NO_LIMIT:
   default:
      pMe->m_maxNumClips = QCAMCORDER_NUMCLIPS_NONE;
      break;

   }

   return;
} /* END QCam_ExecuteNumClipsMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTECLIPLENGTHMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteClipLengthMenu(Qcamcorder* pMe)
{
   switch (Qcamcorder_ExecuteSubMenu(pMe, ClipLengthIDList))
   {
   case IDS_CLIPLENGTH_LOW:
      pMe->m_maxClipLength = QCAMCORDER_CLIPLENGTH_LOW;
      break;
   case IDS_CLIPLENGTH_MED:
      pMe->m_maxClipLength = QCAMCORDER_CLIPLENGTH_MED;
      break;
   case IDS_CLIPLENGTH_HIGH:
      pMe->m_maxClipLength = QCAMCORDER_CLIPLENGTH_HIGH;
      break;
   case IDS_NO_LIMIT:
   default:
      pMe->m_maxClipLength = QCAMCORDER_CLIPLENGTH_NONE;
      break;
   }

   return;
} /* END QCam_ExecuteSizeMenu */

static void Qcamcorder_ExecuteFrameRateMenu(Qcamcorder* pMe)
{

   if (pMe->wFileFormat == QCAMCORDER_FILE_AMC && pMe->eSize == QCAMCORDER_SIZE_MM
#ifdef FEATURE_QVGA_LOW_FPS_ONLY
       || pMe->eSize == QCAMCORDER_SIZE_QVGA
#endif /* FEATURE_QVGA_LOW_FPS_ONLY */
       )
     FrameRateIDList[0] = 1;
   else
     FrameRateIDList[0] = 3;

   switch (Qcamcorder_ExecuteSubMenu(pMe, FrameRateIDList))
   {
   case IDS_FRAME_RATE_1:
      pMe->m_FrameRate = QCAMCORDER_FRAME_RATE_1;
      break;
   case IDS_FRAME_RATE_2:
      pMe->m_FrameRate = QCAMCORDER_FRAME_RATE_2;
      break;
   case IDS_FRAME_RATE_3:
       pMe->m_FrameRate = QCAMCORDER_FRAME_RATE_3;
     break;
   default:
      pMe->m_FrameRate = QCAMCORDER_FRAME_RATE_1;
     break;
   }

   return;
} /* END QCam_ExecuteSizeMenu */
/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTEWHITEBALANCEMENU
DESCRIPTION
============================================================================= */

static void Qcamcorder_ExecuteWhiteBalanceMenu(Qcamcorder* pMe)
{
   switch (Qcamcorder_ExecuteSubMenu(pMe, WhiteBalanceIDList))
   {
   case IDS_AUTO:
       pMe->m_nwhiteBalance = CAM_WB_AUTO;
      break;
   case IDS_FLUORESCENT:
     pMe->m_nwhiteBalance = CAM_WB_FLUORESCENT;
      break;
   case IDS_INCANDESCENT:
       pMe->m_nwhiteBalance = CAM_WB_INCANDESCENT;
       break;
   case IDS_DAYLIGHT:
       pMe->m_nwhiteBalance = CAM_WB_DAYLIGHT;
       break;
   default:
       pMe->m_nwhiteBalance = CAM_WB_AUTO;
       break;
   }
   (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_WHITE_BALANCE, pMe->m_nwhiteBalance, 0);
   return;
} /* END QCam_ExecuteWhiteBalanceMenu */

static void Qcamcorder_ExecutePostEditMenu(Qcamcorder* pMe)
{

   pMe->bPostEditDone = TRUE;

   switch (Qcamcorder_ExecutePostEditSubMenu(pMe, PostEditIDList))
   {
     case IDS_GPS_EDIT:
       pMe->eState = QCAMCORDER_GPS_POSTEDIT;
       Qcamcorder_BuildGPSMenu(pMe, TRUE);
       QCAMCORDER_MSG_HIGH("GPS EDIT SELECTED",0,0,0);

      break;
     case IDS_TELOP_EDIT:
       pMe->eState = QCAMCORDER_TELOP_POSTEDIT;
       QCAMCORDER_MSG_HIGH("TELOP EDIT SELECTED",0,0,0);
       if(ISHELL_CreateDialog(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, IDD_GET_INPUT, NULL)==AEE_SUCCESS)
        { QCAMCORDER_MSG_HIGH("CAME HERE",0,0,0);
        }

      break;

     default:
       QCAMCORDER_MSG_HIGH("DEFAULT",0,0,0);

     break;
   }

   return;
} /* END QCam_ExecuteSizeMenu */

/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTEFILEFORMATMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteFileFormatMenu(Qcamcorder* pMe)
{
   switch (Qcamcorder_ExecuteSubMenu(pMe, FileFormatIDList))
   {

   case IDS_FILE_AMC:
      pMe->wFileFormat = QCAMCORDER_FILE_AMC;
      pMe->wAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K;
      pMe->m_maxClipLength = QCAMCORDER_CLIPLENGTH_LOW;
      if(pMe->eSize == QCAMCORDER_SIZE_QCIF ||
         pMe->eSize == QCAMCORDER_SIZE_QVGA
        )
      {
        pMe->eSize= QCAMCORDER_SIZE_SQCIF;
      }
      if(pMe->wShortHeader == QCAMCORDER_SH_TEST)
      {
        pMe->wShortHeader = QCAMCORDER_SH_DISABLE;
      }
      #ifdef FEATURE_H264_ENCODER
      if(pMe->wVideoFormat == QCAMCORDER_VIDEO_FORMAT_H264)
      {
        pMe->wVideoFormat = QCAMCORDER_VIDEO_FORMAT_MPEG4;
      }
      #endif /* FEATURE_H264_ENCODER */
      (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_AMC, QCAMCORDER_MAX_EXTENSION_SIZE);
	  if (pMe->wBitRate > MAX_AMC_BITRATE_ALLOWED)
      {
        /* Reset the quality to high */
        QCAMCORDER_MSG_HIGH("AMC does not allow %d so resetting to high quality",pMe->wBitRate,0,0);
        pMe->wQuality = QCAMCORDER_QUALITY_HIGH;
        (void)ICAMERA_SetQuality(pMe->m_pICam, pMe->wQuality);
      }
      break;

   case IDS_FILE_3GP:
      pMe->wFileFormat = QCAMCORDER_FILE_3GP;
      if(pMe->wAudioFormat != QCAMCORDER_AUDIO_FORMAT_NONE)
      {
        pMe->wAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR;
      }

      if(pMe->wShortHeader ==QCAMCORDER_SH_TEST)
      {
        pMe->wShortHeader = QCAMCORDER_SH_DISABLE;
      }
      (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_3GP, QCAMCORDER_MAX_EXTENSION_SIZE);

      break;

   case IDS_FILE_3G2:
      pMe->wFileFormat = QCAMCORDER_FILE_3G2;
      (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_3G2, QCAMCORDER_MAX_EXTENSION_SIZE);

      break;

   case IDS_FILE_3G2_FRAG:
      pMe->wFileFormat = QCAMCORDER_FILE_3G2_FRAG;
      (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_3G2_FRAG, QCAMCORDER_MAX_EXTENSION_SIZE);
      /* Check for the Bitrate */
      if (pMe->wBitRate > MAX_3G2_FRAG_BITRATE_SUPPORTED)
      {
        /* Reset the quality to high */
        QCAMCORDER_MSG_HIGH("3g2 Frag does not support %d so resetting to high quality",pMe->wBitRate,0,0);
        pMe->wQuality = QCAMCORDER_QUALITY_HIGH;
        (void)ICAMERA_SetQuality(pMe->m_pICam, pMe->wQuality);
      }
	  break;

   case IDS_FILE_K3G:
      pMe->wFileFormat = QCAMCORDER_FILE_K3G;
      (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_K3G, QCAMCORDER_MAX_EXTENSION_SIZE);
     break;
   case IDS_FILE_SKM:
      pMe->wFileFormat = QCAMCORDER_FILE_SKM;
      (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_SKM, QCAMCORDER_MAX_EXTENSION_SIZE);
     break;

   case IDS_FILE_MP4:
   default:
      pMe->wFileFormat = QCAMCORDER_FILE_MP4;
      pMe->wAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K;
      if(pMe->wShortHeader ==QCAMCORDER_SH_TEST)
      {
        pMe->wShortHeader = QCAMCORDER_SH_DISABLE;
      }
      (void)STRLCPY(pMe->pFileFormat, CAM_FILE_FORMAT_MPG4, QCAMCORDER_MAX_EXTENSION_SIZE);
      break;
   }


   if (pMe->wAudioFormat == QCAMCORDER_AUDIO_FORMAT_NONE)
   {
      // No Audio
      (void)ICAMERA_SetAudioEncode(pMe->m_pICam, 0, 0);
   }
#ifdef FEATURE_CAMCORDER_AAC_REC
   /*Check if Audio Format belongs to AAC Class*/
   else if (pMe->wAudioFormat == QCAMCORDER_AUDIO_FORMAT_VAR_AAC)
   {
     (void)ICAMERA_SetAudioEncode(pMe->m_pICam, AEECLSID_MEDIAAAC, pMe->m_AACInfo);
   }
#endif /* FEATURE_CAMCORDER_AAC_REC */
   else
   {
      (void)ICAMERA_SetAudioEncode(pMe->m_pICam, AEECLSID_MEDIAQCP, pMe->wAudioFormat);
   }

   /* Prevent video format from being reset incorrectly if Short Header
   ** has been disabled but the current video format is still valid */
   if((pMe->wShortHeader == QCAMCORDER_SH_DISABLE &&
       (pMe->wVideoFormat == QCAMCORDER_VIDEO_FORMAT_H263 ||
        pMe->wVideoFormat == QCAMCORDER_VIDEO_FORMAT_H263_SAMPLE)) ||

      /* Cancel short header and reset video format if previous setting of
      ** H263 Sample does not apply to the newly selected file format */
      (pMe->wShortHeader == QCAMCORDER_SH_TEST &&
       (pMe->wFileFormat != QCAMCORDER_FILE_3G2 &&
        pMe->wFileFormat != QCAMCORDER_FILE_3G2_FRAG)))
   {
     pMe->wShortHeader = QCAMCORDER_SH_DISABLE;
     pMe->wVideoFormat = QCAMCORDER_VIDEO_FORMAT_MPEG4;
   }

   (void)ICAMERA_SetSize(pMe->m_pICam, &QCam_Sizes[pMe->eSize]);

   if(pMe->m_displaySize == QCAMCORDER_DISPLAY_MENU_SELECT)
   {
     (void)ICAMERA_SetDisplaySize(pMe->m_pICam, &QCam_Sizes[pMe->eSize]);
   }
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  (void)Qcamcorder_SetDirectMode(pMe, TRUE);
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE
   return;
} /* END Qcamcorder_ExecuteFileFormatMenu */

/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTEVIDEOFORMATMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteVideoFormatMenu(Qcamcorder* pMe)
{
  CameraData *sh_data= NULL;

  switch (Qcamcorder_ExecuteSubMenu(pMe, VideoFormatIDList))
  {
#ifdef FEATURE_H264_ENCODER
    case IDS_FORMAT_H264:
      pMe->wVideoFormat = QCAMCORDER_VIDEO_FORMAT_H264;
      pMe->wShortHeader = QCAMCORDER_SH_DISABLE;
      break;
#endif /* FEATURE_H264_ENCODER */

    case IDS_FORMAT_H263:
      pMe->wVideoFormat = QCAMCORDER_VIDEO_FORMAT_H263;
      pMe->wShortHeader = QCAMCORDER_SH_ENABLE;
      break;

    case IDS_FORMAT_H263_SAMPLE:
      pMe->wVideoFormat = QCAMCORDER_VIDEO_FORMAT_H263_SAMPLE;
      pMe->wShortHeader = QCAMCORDER_SH_TEST;
      break;

    case IDS_NONE:
      pMe->wVideoFormat = QCAMCORDER_VIDEO_FORMAT_NONE;
      pMe->wShortHeader = QCAMCORDER_SH_DISABLE;
      break;

    case IDS_FORMAT_MPEG4:
    default:
      pMe->wVideoFormat = QCAMCORDER_VIDEO_FORMAT_MPEG4;
      pMe->wShortHeader = QCAMCORDER_SH_DISABLE;
      break;
  }

  /* Inform OEM layer of selected video format */
  (void)ICAMERA_SetVideoEncode(pMe->m_pICam, AEECLSID_MEDIAMPEG4, pMe->wVideoFormat);

  /* Inform OEM layer of short header selection */
  sh_data = MALLOC(sizeof(CameraData));
  sh_data->pData = MALLOC(sizeof(pMe->wShortHeader));
  sh_data->dwBytes = sizeof(pMe->wShortHeader );
  (void)MEMCPY(sh_data->pData, &(pMe->wShortHeader), sizeof(pMe->wShortHeader));
  (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_SHORT_HEADER, sh_data);
  FREEIF(sh_data->pData);
  FREEIF(sh_data);

  /* Ensure rotation/frame size parameters are supported by short header */
  if(pMe->wShortHeader != QCAMCORDER_SH_DISABLE)
  {
#ifdef FEATURE_VIDEO_ENCODE_LANDSCAPE_MOUNT
    if(pMe->m_bRotate == QCAMCORDER_ROTATE_90 ||
       pMe->m_bRotate == QCAMCORDER_ROTATE_270)
    {
       pMe->m_bRotate = QCAMCORDER_ROTATE_OFF;
       ICAMERA_RotateEncode(pMe->m_pICam, 0);
    }
#else
    if(pMe->m_bRotate == QCAMCORDER_ROTATE_OFF ||
       pMe->m_bRotate == QCAMCORDER_ROTATE_180)
    {
      pMe->m_bRotate = QCAMCORDER_ROTATE_90;
      (void)ICAMERA_RotateEncode(pMe->m_pICam, 90);
    }
#endif /* FEATURE_VIDEO_ENCODE_LANDSCAPE_MOUNT */

    if(pMe->eSize == QCAMCORDER_SIZE_QVGA)
    {
      pMe->eSize = QCAMCORDER_SIZE_QCIF;
    }
    else if(pMe->eSize == QCAMCORDER_SIZE_MM)
    {
      pMe->eSize = QCAMCORDER_SIZE_SQCIF;
    }

    (void)ICAMERA_SetSize(pMe->m_pICam, &QCam_Sizes[pMe->eSize]);

    if(pMe->m_displaySize == QCAMCORDER_DISPLAY_MENU_SELECT)
      (void)ICAMERA_SetDisplaySize(pMe->m_pICam, &QCam_Sizes[pMe->eSize]);
  }
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  (void)Qcamcorder_SetDirectMode(pMe, TRUE);
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE

  return;
} /* END Qcamcorder_ExecuteVideoFormatMenu */

#ifdef FEATURE_CAMCORDER_AAC_REC
/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTEAACCONFIGMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteAACConfigMenu(Qcamcorder* pMe)
{
  uint16 focus = 0;

  switch (Qcamcorder_ExecuteSubMenu(pMe, AACConfigIDList))
  {
    case IDS_AAC_CONFIG_FREQ:
      pMe->m_AACConfig = QCAMCORDER_AAC_CONFIG_FREQ;
      focus = QCAMCORDER_MENU_AAC_FREQ;
      break;
    case IDS_AAC_CONFIG_RATE:
      pMe->m_AACConfig = QCAMCORDER_AAC_CONFIG_BITRATE;
      focus = QCAMCORDER_MENU_AAC_BITRATE;
      break;
    case IDS_AAC_CONFIG_MODE:
    default:
      pMe->m_AACConfig = QCAMCORDER_AAC_CONFIG_MODE;
      focus = QCAMCORDER_MENU_AAC_MODE;
      break;
  }

  /* Build hierarchical menus for AAC submenus */
  Qcamcorder_BuildMenu(pMe, focus);
}


/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTEAACMODEMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteAACModeMenu(Qcamcorder* pMe)
{
  uint32 selection = 0;

  switch (Qcamcorder_ExecuteSubMenu(pMe, AACModeIDList))
  {
    case IDS_AAC_STEREO2MONO:
      selection = QCAMCORDER_AAC_STEREO2MONO;
      break;
    case IDS_AAC_STEREO2STEREO:
      selection = QCAMCORDER_AAC_STEREO2STEREO;
      break;
    case IDS_AAC_MONO2MONO:
    default:
      selection = QCAMCORDER_AAC_MONO2MONO;
      break;
  }

  pMe->m_AACInfo = ((uint32)0x00FFFFFF & pMe->m_AACInfo) |
                   ((selection << 24) & (uint32)0xFF000000);

  (void)ICAMERA_SetAudioEncode(pMe->m_pICam, AEECLSID_MEDIAAAC,
                               pMe->m_AACInfo);
}


/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTEAACFREQMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteAACFreqMenu(Qcamcorder* pMe)
{
  uint32 selection = 0;

  switch (Qcamcorder_ExecuteSubMenu(pMe, AACFreqIDList))
  {
    case IDS_AAC_FREQ_11:
      selection = QCAMCORDER_AAC_FREQ_11;
      break;
    case IDS_AAC_FREQ_16:
      selection = QCAMCORDER_AAC_FREQ_16;
      break;
    case IDS_AAC_FREQ_22:
      selection = QCAMCORDER_AAC_FREQ_22;
      break;
    case IDS_AAC_FREQ_24:
      selection = QCAMCORDER_AAC_FREQ_24;
      break;
    case IDS_AAC_FREQ_8:
    default:
      selection = QCAMCORDER_AAC_FREQ_8;
      break;
  }

  pMe->m_AACInfo = ((uint32)0xFF00FFFF & pMe->m_AACInfo) |
                   ((selection << 16) & (uint32)0xFF0000);

  (void)ICAMERA_SetAudioEncode(pMe->m_pICam, AEECLSID_MEDIAAAC,
                               pMe->m_AACInfo);
}



/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTEAACBITRATEMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteAACBitrateMenu(Qcamcorder* pMe)
{
  uint32 selection = 0;

  switch (Qcamcorder_ExecuteSubMenu(pMe, AACBitrateIDList))
  {
    case IDS_AAC_BITRATE_MED:
      selection = QCAMCORDER_AAC_BITRATE_MED;
      break;
    case IDS_AAC_BITRATE_HIGH:
      selection = QCAMCORDER_AAC_BITRATE_HIGH;
      break;
    case IDS_AAC_BITRATE_LOW:
    default:
      selection = QCAMCORDER_AAC_BITRATE_LOW;
      break;
  }

  pMe->m_AACInfo = ((uint32)0xFFFF00FF & pMe->m_AACInfo) |
                   ((selection << 8) & (uint32)0xFF00);

  (void)ICAMERA_SetAudioEncode(pMe->m_pICam, AEECLSID_MEDIAAAC,
                               pMe->m_AACInfo);
}
#endif /* FEATURE_CAMCORDER_AAC_REC */


/* ==========================================================================
FUNCTION QCAMCORDER_PLAYSTARTSOUND
DESCRIPTION
============================================================================= */
static void Qcamcorder_PlayStartSound(Qcamcorder* pMe)
{
   AEEMediaData      md;
   char* pfileExt;
   md.clsData = MMD_FILE_NAME;
   md.pData = (void *)pMe->sStartSoundFile;
   md.dwSize = 0;

   QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
   QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);

   if (AEEMediaUtil_CreateMedia(pMe->a.m_pIShell, &md, &pMe->pMedia) != SUCCESS)
   {
     QCAMCORDER_MSG_HIGH("Could not create media object",0,0,0);
     return;
   }

   if (pMe->pMedia)
   {
      pfileExt=QCamcorder_GetFileNameExtension(pMe->sStartSoundFile);
      if (pfileExt && STRCMP(pfileExt, "wav") == 0)
         (void)IMEDIA_SetMediaParm(pMe->pMedia, MM_PARM_CHANNEL_SHARE, FALSE, 0);
	  
      (void)IMEDIA_SetMediaParm(pMe->pMedia, MM_PARM_AUDIO_DEVICE,AEE_SOUND_DEVICE_SPEAKER, 0);
      (void)IMEDIA_RegisterNotify(pMe->pMedia, Qcamcorder_MediaNotify, pMe);
      (void)IMEDIA_Play(pMe->pMedia);
   }
}

/* ==========================================================================
FUNCTION QCamcorder_GetFileNameExtension
DESCRIPTION
============================================================================= */
static char * QCamcorder_GetFileNameExtension(char * pszFileName)
{
    char * pszExtName = STRRCHR(pszFileName, (int)DOT_CHAR);

    if(pszExtName)
        pszExtName++;
    else
          return NULL;

    return pszExtName;
}

/* ==========================================================================
FUNCTION QCAMCORDER_MEDIANOTIFY
DESCRIPTION
============================================================================= */
static void Qcamcorder_MediaNotify(void* pUser, AEEMediaCmdNotify* pCmdNotify)
{
   Qcamcorder *   pMe = (Qcamcorder *)pUser;

   if (pCmdNotify->nCmd == MM_CMD_PLAY)  // IMEDIA_Play events
   {
      if (pCmdNotify->nStatus == MM_STATUS_DONE ||
          (pCmdNotify->nStatus == MM_STATUS_ABORT &&
           STRCMP(pMe->sStartSoundFile, QCAMCORDER_SOUND_FILE) == 0))
      {
         if (!IMENUCTL_IsActive(pMe->m_pISettings))
         {
           if(pMe->m_bTelop)                   /* If telop is enabled, preview has */
             Qcamcorder_RecordTelopTimer(pMe); /* already stopped, start recording */
           else                                /* If telop disabled, preview has   */
             Qcamcorder_RecordTimer(pMe);      /* not stopped, stop preview first  */
         }
      }
      else if (pCmdNotify->nStatus == MM_STATUS_ABORT)
      {
         if (STRCMP(pMe->sStartSoundFile, QCAMCORDER_SOUND_FILE) != 0)
         {
            // Bad sound file, reset to default
            // And try to play again
            (void)STRLCPY(pMe->sStartSoundFile, QCAMCORDER_SOUND_FILE,
                          sizeof(pMe->sStartSoundFile));
            Qcamcorder_PlayStartSound(pMe);
            if (IMENUCTL_IsActive(pMe->m_pISettings))
            {
               // reset the sound menu to reflect the bad sound file
               Qcamcorder_BuildMenu(pMe, QCAMCORDER_MENU_SOUND);
            }
         }
      }
   } // MM_CMD_PLAY
}
static void Qcamcorder_ErasePostEdit(Qcamcorder *pMe)
{
   pMe->eGPSState = QCAMCORDER_GPS_DISABLED;
   FREE(pMe->uuid_data );
   pMe->uuid_data = NULL;
   FREE(pMe->uuid);
   pMe->uuid = NULL;
}

static boolean Qcamcorder_IsValidFileName(char* filename)
{
  char * pdot, *pseperator;
  pdot = STRRCHR(filename, '.'); 
  pseperator = STRRCHR(filename, '/');
  // Only add our movie files
  if(pdot && pseperator &&
     (!STRCMP(pdot, QCAMCORDER_3G_MOVIE_EXTENSION)  ||
      !STRCMP(pdot, QCAMCORDER_3GP_MOVIE_EXTENSION) ||
      !STRCMP(pdot, QCAMCORDER_AMC_MOVIE_EXTENSION) ||
      !STRCMP(pdot, QCAMCORDER_MP4_MOVIE_EXTENSION) ||
      !STRCMP(pdot, QCAMCORDER_SKM_MOVIE_EXTENSION) ||
      !STRCMP(pdot, QCAMCORDER_K3G_MOVIE_EXTENSION)
     ) &&
     (!STRNCMP(pseperator+1, "mov", 3) ||
       !STRNCMP(pseperator+1, "ve", 2) ||  /* Video Edit */
       !STRNCMP(pseperator+1, "ad", 2) ||  /* Audio Dub */
       !STRNCMP(pseperator+1, "vs", 2) ||  /* Video Stabilize */
       !STRNCMP(pseperator+1, "rm", 2)     /* Raw Mode */
     )
    )
  {
     return TRUE;
  }
  return FALSE;
}
/* ==========================================================================
FUNCTION QCAMCORDER_DISPLAYFILELIST
DESCRIPTION Displays the list of current Qcamcorder movie files to the user
to select for editing
============================================================================= */
static void Qcamcorder_DisplayFileList(Qcamcorder *pMe)
{
   AEERect qrc;
   CtlAddItem ai;
   AECHAR szBuf[FILE_NAME_SIZE];
   FileInfo fi;
   char * p_file = NULL;
   char * p_newfile = NULL;
   uint16 menuFocus = 1;
   int i;
   uint16 max_file_count = 0;

   QCamcorder_MdpRegisterFullScreen(pMe, FALSE);

   IMENUCTL_Reset(pMe->m_pIMenu);
   (void)IMENUCTL_SetTitle(pMe->m_pIMenu, QCAMCORDER_RES_FILE, IDS_FILELISTMENU_TITLE, NULL);
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen);
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
   Qcamcorder_ErasePostEdit(pMe);

   ai.wText = 0;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 1;
   ai.wImage = IDB_VIDEO;
   ai.wItemID = 1;
   for(i = 0; i < (int)ARR_SIZE(AEEDirList); i++)
   {
     (void)IFILEMGR_EnumInit(pMe->pIFileMgr, AEEDirList[i], FALSE);
     while (IFILEMGR_EnumNext(pMe->pIFileMgr, &fi))
     {
       QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);

       // Only add our movie files
       if(Qcamcorder_IsValidFileName(fi.szName))
       {  
          p_newfile = MALLOC(FILE_NAME_SIZE);
          if(!p_newfile)
          { QCAMCORDER_MSG_HIGH("MALLOC FAILED",0,0,0);
            (void)STR_TO_WSTR(fi.szName, szBuf, sizeof(szBuf));
            return;
          }
          if(STRCMP(AEEDirList[i], QCAMCORDER_MOVIES_DIR_FLASH) == 0)
          {
            (void)STRLCPY(p_newfile, "media/",FILE_NAME_SIZE);
          }
          else
          {
            (void)STRLCPY(p_newfile, OEMDirList[i],FILE_NAME_SIZE);
          }
#ifdef FEATURE_BREW_3_0
          p_file = STRCHR(fi.szName+1, '/');
          p_file = STRCHR(p_file+1, '/');
          p_file = STRCHR(p_file+1, '/')+1;
#else
          p_file = STRCHR(fi.szName, '/')+1;
#endif

          if(!p_file)
          { QCAMCORDER_MSG_HIGH("STRCHR FAILED",0,0,0);
            (void)STR_TO_WSTR(fi.szName, szBuf, sizeof(szBuf));
            return;
          }

         (void)STRLCAT(p_newfile, p_file, FILE_NAME_SIZE);
         (void)STR_TO_WSTR(p_newfile, szBuf, sizeof(szBuf));
  
         ai.pText = szBuf;
         (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
         if (STRCMP(pMe->sCurrentFileName, fi.szName) == 0)
         {
           menuFocus = ai.wItemID;
         }
         ++ai.wItemID;
         ++ai.dwData;

         if(p_newfile)
         {
           FREE(p_newfile);
           p_newfile = NULL;
         }
       }
     }
   }

   /* Reset file specific GPS/Telop information */
   MEMSET(&(pMe->ExtendedInfo),0,sizeof(AEEMediaMPEG4SpecEx));
   MEMSET(pMe->TelopTextInfo,0,sizeof(char)*USER_TELOP_MAX_LENGTH);

   if (!IMENUCTL_GetItemCount(pMe->m_pIMenu)
#ifdef FEATURE_VIDEONOSENSOR
       && !pMe->m_bNoSensor
#endif
       )
   {
      Qcamcorder_Preview (pMe);
   }
   else
   {
     //lint -e570  Suppress complaint on loss of sign
     IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
     //lint +e570
     //lint -e570  Suppress complaint on loss of sign
     IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
     //lint +e570
     IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
     if (!pMe->b_focusFirstItem)
     {
       IMENUCTL_SetSel(pMe->m_pIMenu, menuFocus);
     }
     (void)IMENUCTL_Redraw(pMe->m_pIMenu);
     pMe->m_activeMenu = QCAMCORDER_MENU_FILELIST;
     pMe->m_activeSoftMenu = QCAMCORDER_MENU_NONE;
   }
}


#ifdef FEATURE_VIDEO_ENCODE_RAW
/* ==========================================================================
FUNCTION QCAMCORDER_RAWENCODERDISPLAYFILELIST
DESCRIPTION Displays the list of current raw movie files to the user
to select for offline encoding
============================================================================= */
static void Qcamcorder_RawEncoderDisplayFileList(Qcamcorder *pMe)
{
   AEERect qrc;
   CtlAddItem ai;
   AECHAR szBuf[FILE_NAME_SIZE];
   FileInfo fi;
   char * p_file;
   char * p_newfile, *pdot, *pseperator;
   int i;
   uint16 max_file_count = 0;

   if(pMe->m_activeMenu != QCAMCORDER_MENU_RAWFILELIST &&
      pMe->m_activeSoftMenu != QCAMCORDER_MENU_NONE)
   {
     QCamcorder_MdpRegisterFullScreen(pMe, FALSE);
   }

   IMENUCTL_Reset(pMe->m_pIMenu);
   (void)IMENUCTL_SetTitle(pMe->m_pIMenu, QCAMCORDER_RES_FILE, IDS_FILELISTMENU_TITLE, NULL);
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen);
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
   Qcamcorder_ErasePostEdit(pMe);


   ai.wText = 0;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 1;
   ai.wImage = IDB_VIDEO;
   ai.wItemID = 1;
   for(i = 0; i < (int)ARR_SIZE(AEEDirList); i++)
   {
     (void)IFILEMGR_EnumInit(pMe->pIFileMgr, AEEDirList[i], FALSE);
     while (IFILEMGR_EnumNext(pMe->pIFileMgr, &fi))
     {
       QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);
       // Only add raw files - distinguised by name format only: rawXXX.yuv
       pdot = STRRCHR(fi.szName, '.');
       pseperator = STRRCHR(fi.szName, '/');
       if(pdot && pseperator && 
          !STRCMP(pdot, QCAMCORDER_YUV_MOVIE_EXTENSION) &&
          !STRNCMP(pseperator+1, "raw", 3))
       {
         p_newfile = MALLOC(FILE_NAME_SIZE);
         if(!p_newfile)
         {
           QCAMCORDER_MSG_HIGH("MALLOC FAILED",0,0,0);
           (void)STR_TO_WSTR(fi.szName, szBuf, sizeof(szBuf));
           return;
         }
         if(STRCMP(AEEDirList[i], QCAMCORDER_MOVIES_DIR_FLASH) == 0)
         {
           (void)STRLCPY(p_newfile, "media/",FILE_NAME_SIZE);
         }
         else
         {
           (void)STRLCPY(p_newfile, OEMDirList[i],FILE_NAME_SIZE);  
         }
#ifdef FEATURE_BREW_3_0
         p_file = STRCHR(fi.szName+1, '/');
         p_file = STRCHR(p_file+1, '/');
         p_file = STRCHR(p_file+1, '/')+1;
#else
         p_file = STRCHR(fi.szName, '/')+1;
#endif

         if(!p_file)
         {
           QCAMCORDER_MSG_HIGH("STRCHR FAILED",0,0,0);
           (void)STR_TO_WSTR(fi.szName, szBuf, sizeof(szBuf));
           return;
         }
         (void)STRLCAT(p_newfile, p_file,FILE_NAME_SIZE);
         (void)STR_TO_WSTR(p_newfile, szBuf, sizeof(szBuf));
         ai.pText = szBuf;
         (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
         ++ai.wItemID;
         ++ai.dwData;

         FREE(p_newfile);
         p_newfile = NULL;
       }
     }
   }

   if (!IMENUCTL_GetItemCount(pMe->m_pIMenu))
   {
      Qcamcorder_Preview (pMe);
      return;
   }

   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
   //lint +e570
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570
   IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
   (void)IMENUCTL_Redraw(pMe->m_pIMenu);
   pMe->m_activeMenu = QCAMCORDER_MENU_RAWFILELIST;
   pMe->m_activeSoftMenu = QCAMCORDER_MENU_NONE;
}
#endif /* FEATURE_VIDEO_ENCODE_RAW */

/* ==========================================================================
FUNCTION QCAMCORDER_PLAYALLGETNEXTFILE
DESCRIPTION Get the next movie file to be played for play all option
============================================================================= */
boolean Qcamcorder_PlayAllGetNextFile(Qcamcorder *pMe)
{
   FileInfo fi;
   boolean found=FALSE;
   int i;
   uint16 max_file_count = 0;

   if (STRCMP(pMe->sCurrentFileName, "") == 0)
       found = TRUE;

   for(i = 0; i < (int)ARR_SIZE(AEEDirList); i++)
   {
     (void)IFILEMGR_EnumInit(pMe->pIFileMgr, AEEDirList[i], FALSE);
     while (IFILEMGR_EnumNext(pMe->pIFileMgr, &fi))
     {
       QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);
       // Only get our movie files
       if(Qcamcorder_IsValidFileName(fi.szName))
       {
         if (found == TRUE)
         {
           (void)STRLCPY(pMe->sCurrentFileName, fi.szName,
                             sizeof(pMe->sCurrentFileName));
           return TRUE;
         }
         if (STRCMP(pMe->sCurrentFileName, fi.szName) == 0)
         {
           found = TRUE;
         }
       }
     }
   }
   return FALSE;
}

/* ==========================================================================
FUNCTION QCAMCORDER_PLAYMOVIE
DESCRIPTION Starts playing a recorded movie.
============================================================================= */
void Qcamcorder_PlayMovie(Qcamcorder * pMe)
{
  AEEMediaData md;

  md.clsData = MMD_FILE_NAME;
  md.dwSize = 0;
  md.pData = (void *)pMe->sCurrentFileName;

  QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
  QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);

  if(AEEMediaUtil_CreateMedia(pMe->a.m_pIShell, &md, &pMe->pMedia) != SUCCESS)
  {
    QCAMCORDER_MSG_HIGH("Could not create media object",0,0,0);
    return;
  }

  if (pMe->pMedia)
  {
    (void)IMEDIA_RegisterNotify(pMe->pMedia, Qcamcorder_PlayMediaNotify, pMe);

#ifdef FEATURE_MP4_TRANSCODER
    if( pMe->eState != QCAMCORDER_TRANSCODE )
    {
#endif
      pMe->eState = QCAMCORDER_PLAYING;
#ifdef FEATURE_MP4_TRANSCODER
    }
#endif
  }
}

static boolean Qcamcorder_CheckSpace(Qcamcorder *pMe)
{
  uint32 freespace = 0;
  FileInfo fi;
  pMe->pIFile = IFILEMGR_OpenFile(pMe->pIFileMgr, pMe->sCurrentFileName, _OFM_READ);
  if (!pMe->pIFile)
  {
    return FALSE;
  }
  (void)IFILE_GetInfo(pMe->pIFile, &fi);
  (void)IFILE_Release(pMe->pIFile);
  pMe->pIFile = NULL;

#if MIN_BREW_VERSIONEx(3,1,3)
  // With BREW 3.1.3+ we can now determine the remaining space on the MMC
  // as well as the EFS
  {
    const char *path;
    switch(pMe->saveLoc)
    {
      case QCAMCORDER_SAVE_FLASH:
        path = AEEFS_ROOT_DIR;
        break;
      case QCAMCORDER_SAVE_MMC :
        path = AEEFS_CARD0_DIR;
        break;
#ifdef FEATURE_EFS_FTL
      case QCAMCORDER_SAVE_FTL :
        path = AEEFS_MOD_FTL_DIR;
        break;
#endif  /* FEATURE_EFS_FTL */

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
      case QCAMCORDER_SAVE_USB :
        path = AEEFS_MOD_USB_DIR;
        break;
#endif /* FEATURE_EFS_HOTPLUG_USB_HOST_MS */
      default:
        path = AEEFS_ROOT_DIR;
    }

   if(pMe->saveLoc == QCAMCORDER_SAVE_MMC)
   {
     AEEFSFreeSpace FSSpace;
     int pnSize;
     pnSize = sizeof(AEEFSFreeSpace);
     if(SUCCESS != ISHELL_GetDeviceInfoEx (pMe->a.m_pIShell,
                                            AEE_DEVICEITEM_FSSPACE_CARD0 ,
                                            (void *)&FSSpace,
                                            &pnSize))
     {
       FSSpace.qwFreeSpace = 0;
     }
     else
     {
       if(FSSpace.qwFreeSpace > (uint64)fi.dwSize)
       {
         return TRUE;
       }
     }
   }
   else if(SUCCESS !=
       IFILEMGR_GetFreeSpaceEx(pMe->pIFileMgr,
                               path,
                                        NULL,
                               &freespace))
    {
      freespace = 0;
    }
  }
#else
  freespace = IFILEMGR_GetFreeSpace(pMe->pIFileMgr, NULL);
#endif /* MIN_BREW_VERSIONEx(3,1,3) */

  if(freespace > fi.dwSize)
  {
    return TRUE;
  }
  return FALSE;
}
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
/* ==========================================================================
FUNCTION QCAMCORDER_PLAYMOVIE
DESCRIPTION Starts playing a recorded movie.
============================================================================= */
static void Qcamcorder_PlayAudioDubMovie(Qcamcorder * pMe)
{
   if (pMe->pMedia)
   {
      (void)IMEDIA_SetTickTime(pMe->pMedia, 1000);
      if (SUCCESS != IMEDIA_GetTotalTime(pMe->pMedia))
      {QCAMCORDER_MSG_HIGH("COULD NOT GET MEDIA TIME",0,0,0);
       pMe->m_dwTotalTime = 100;
      }

      (void)IMEDIA_Play(pMe->pMedia);
      pMe->eState = QCAMCORDER_PLAYING;
   }
}

#endif
/* ==========================================================================
FUNCTION QCAMCORDER_PLAYMEDIANOTIFY
DESCRIPTION Used when playing content in the edit screen.
============================================================================= */
static void Qcamcorder_PlayMediaNotify(void* pUser, AEEMediaCmdNotify* pCmdNotify)
{
  Qcamcorder *   pMe = (Qcamcorder *)pUser;

  if (pMe->m_activeSoftMenu != QCAMCORDER_MENU_EDIT)
  {
    return;
  }

  if (pCmdNotify->nCmd == MM_CMD_PLAY)  // IMEDIA_Play events
  {
    if (pCmdNotify->nStatus == MM_STATUS_START)
    {
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
    if (pMe->eState == QCAMCORDER_AUDIODUB)
      {
        /* Start recording for audio dubbing */
        if(pMe->wAudioFormatUsed != QCAMCORDER_AUDIO_FORMAT_NONE && pMe->pRecordMedia)
        {
          (void)IMEDIA_Record(pMe->pRecordMedia);
        } else {
          QCAMCORDER_MSG_ERROR("RecordMedia object is NULL",0,0,0);
        }
      }
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */
    }
    else if(pCmdNotify->nStatus == MM_STATUS_TICK_UPDATE ||
            pCmdNotify->nStatus == MM_STATUS_DATA_IO_DELAY ||
            pCmdNotify->nStatus == MM_STATUS_SEEK)
    {
       Qcamcorder_UpdateProgCtl(pMe);
       QCAMCORDER_MSG_HIGH("Updating bar ",0,0,0);
       pMe->m_dwPlayPos = pMe->m_dwPlayPos +1;
    }
    else if (pCmdNotify->nStatus == MM_STATUS_ABORT)
    {
       /*Set the error Msg*/
       pMe->msg = IDB_ERROR_QCAMCORDER;

      /*Check if we were Any clip or were in Paused state*/
      if (pMe->eState == QCAMCORDER_PLAYING ||
          pMe->eState == QCAMCORDER_PAUSED)
      {
         pMe->m_bPlayAll = FALSE;
         pMe->eState = QCAMCORDER_EDIT;
        /* Disable frame callbacks at end of clip */
         if (pMe->pMedia)
         {
           (void)IMEDIA_EnableFrameCallback(pMe->pMedia, FALSE);
         } else {
           QCAMCORDER_MSG_ERROR("Media object is NULL",0,0,0);
         }
         pMe->m_playbackSize = QCAMCORDER_PLAYBACK_REGULAR;
      }
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
      /*Check if we were in middle of Audio dubbing session*/
      else if(pMe->eState == QCAMCORDER_AUDIODUB)
      {
    /*Move audio dubbing state*/
        pMe->eState = QCAMCORDER_AUDIODUB_ABORT;
        Qcamcorder_AudioDubCleanUp(pMe);

        /* If video format is none then display Feature not
         * supported error msg */
        if(!pMe->m_mei.videoFormat)
        {
          pMe->msg = IDB_POSTEDIT_UNSUPPORTED;
        }

      }
#endif /*FEATURE_VIDEO_AUDIO_DUBBING*/
      else 
      {
        pMe->eState = QCAMCORDER_EDIT;
        pMe->m_bEditHit = FALSE;
        pMe->m_bPlayAll = FALSE;
        if(IFILEMGR_Remove(pMe->pIFileMgr, pMe->sVideoEditFileName)!= SUCCESS)
        {
          QCAMCORDER_MSG_ERROR("Error in removing file",0,0,0);
        }
        pMe->msg = IDB_POSTEDIT_ERROR;
      }
      QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);
      if(VideoPlayback.id != 0)
      {
        mdp_clear_ovimg(VideoPlayback.id);
        mdp_deregister_ovimg(VideoPlayback.id);
        VideoPlayback.id = 0;
      }
      Qcamcorder_ErrorEventUserNotify(pMe);
    }
    else if (pCmdNotify->nStatus == MM_STATUS_DONE)
    {
      /* Deregister layer which was used to display playback frames */
      if(VideoPlayback.id != 0)
      {
        QCAMCORDER_MSG_HIGH("Deregistering MDP %d", VideoPlayback.id, 0, 0);
        mdp_clear_ovimg(VideoPlayback.id);
        mdp_deregister_ovimg(VideoPlayback.id);
        VideoPlayback.id = 0;
      }

      pMe->m_playbackSize = QCAMCORDER_PLAYBACK_REGULAR;
      QCamcorder_MdpRegisterFullScreen(pMe, FALSE);

      if (pMe->eState == QCAMCORDER_PLAYING ||
          pMe->eState == QCAMCORDER_PAUSED)
      {
        /* Disable frame callbacks at end of clip */
        if (pMe->pMedia)
        {
          (void)IMEDIA_EnableFrameCallback(pMe->pMedia, FALSE);
        } else {
           QCAMCORDER_MSG_ERROR("Media object is NULL",0,0,0);
        }

        if (pMe->m_bPlayAll && Qcamcorder_PlayAllGetNextFile(pMe))
        {
          pMe->m_dwPlayPos = 0;
          (void)ISHELL_SetTimer(pMe->a.m_pIShell, 250, (PFNNOTIFY)Qcamcorder_PlayMovie,(void *)pMe);
        }
        else
        {
          pMe->m_bEditHit = FALSE;
          pMe->eState = QCAMCORDER_EDIT;
          QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);
          Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);
        }
      }
      else
      {     /* should arrive here after IMedia_Stop or when clip playback ends */
            pMe->m_bEditHit = FALSE;
        /* Disable frame callbacks at end of clip */
        if (pMe->pMedia)
        {
          (void)IMEDIA_EnableFrameCallback(pMe->pMedia, FALSE);
        } else {
           QCAMCORDER_MSG_ERROR("Media object is NULL",0,0,0);
        }

        if(Qcamcorder_CheckSpace(pMe))
        {
          switch(pMe->eState)
          {
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
          case QCAMCORDER_VIDEO_EDIT_SELECTION:
             pMe->eState = QCAMCORDER_VIDEO_EDIT_STARTED;
#if defined(FEATURE_BREW_IMOVIEEDIT) || defined(FEATURE_BREW_IMOVIEFILE)
             (void)Qcamcorder_VideoEditing(pMe);
#endif
             break;
#endif /* FEATURE_QCAMCORDER_VIDEO_EDITING */
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
          case QCAMCORDER_CUT_MERGE:
             if(pMe->m_dFirstMarker == pMe->m_dSecondMarker)
               pMe->m_dSecondMarker = pMe->m_dwPlayPos;
#if defined(FEATURE_BREW_IMOVIEEDIT) || defined(FEATURE_BREW_IMOVIEFILE)
               (void)Qcamcorder_CutMerge(pMe);
#endif
             break;
#endif //FEATURE_QCAMCORDER_CUT_N_MERGE
#ifdef FEATURE_MP4_TRANSCODER
          case QCAMCORDER_TRANSPORTION:
             Qcamcorder_StartPartialTranscoding(pMe);
             break;
#endif
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
          case QCAMCORDER_AUDIODUB:
            /* Need to stop recording and create new mp4 file */
            if(pMe->wAudioFormatUsed != QCAMCORDER_AUDIO_FORMAT_NONE)
            {
              QCAMCORDER_IMEDIA_STOP(pMe->pRecordMedia); 
            }
            Qcamcorder_AudioDub(pMe);

            if(pMe->wAudioFormatUsed == QCAMCORDER_AUDIO_FORMAT_NONE)
            {
              pMe->eState = QCAMCORDER_EDIT;
              Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);
            }
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
            break;

          case QCAMCORDER_AUDIODUB_ABORT:
            Qcamcorder_AudioDubCleanUp(pMe);
	    pMe->eState = QCAMCORDER_EDIT;
            break;
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */

          default:
            break;
          }
        }
        else
        {
          Qcamcorder_DisplayPostRecordingMessage(pMe);
        }
        QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);
      }
    }

    else if(pCmdNotify->nStatus == MM_MP4_STATUS_OPEN_COMPLETE && pMe->pMedia)
    {
      pMe->m_bWait_to_Finish = FALSE;
      Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);
      (void)IMEDIA_GetMediaParm(pMe->pMedia, MM_MP4_PARM_TRACK_TIME_SCALE,
                          (int32 *)VIDEO_TRACK,
                          (int32*)&(pMe->nVideoTimeScale));
      (void)IMEDIA_GetMediaParm(pMe->pMedia, MM_MP4_PARM_TRACK_TIME_SCALE,
                          (int32 *)AUDIO_TRACK,
                          (int32*)&(pMe->nAudioTimeScale));

#ifdef FEATURE_VIDEO_AUDIO_DUBBING
      if(pMe->AudioDubbing)
      {
        Qcamcorder_StartRecordAudio(pMe);
      }
      else
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */
      {
        AEEMediaMPEG4Spec *pSpec;
        int32             dwSize;

        /* Get the media specification (i.e. clip meta data). */
        if(IMEDIA_GetMediaSpec(pMe->pMedia, &pSpec, &dwSize) != SUCCESS)
        {
          QCAMCORDER_MSG_ERROR("Media Spec not available",0,0,0);
        }

        /* Extract GPS and Telop related information */
        MEMCPY(&(pMe->ExtendedInfo), &(pSpec->ExtendedInfo), sizeof(AEEMediaMPEG4SpecEx));

        /* Find out frame dimensions so we know whether to use full screen
        ** 240 was calculated based on the maximum rect we can set that will
        ** utilize most of the screen without any form of overlay i.e. frames
        ** will not overlap areas for annunciator/progess bars and menus */
        if((pSpec->dwWidth >= 240 || pSpec->dwHeight >= 240))
        {
          /* Set state for full screen video playback */
          pMe->m_playbackSize = QCAMCORDER_PLAYBACK_FULL_SCREEN;

          /* To utilize entire screen, we might have to
          ** convert landscape frames to portrait mode */
          if(pSpec->dwWidth > pSpec->dwHeight)
            VideoPlayback.rotation = MM_MPEG4_90_CW_ROTATION;
          else
            VideoPlayback.rotation = MM_MPEG4_NO_ROTATION;

          /* During full screen playback, ensure menus on top MDP FGL */
          QCamcorder_MdpRegisterPreview(pMe, TRUE);

        }
        else
        {
          pMe->m_playbackSize = QCAMCORDER_PLAYBACK_REGULAR;
          VideoPlayback.rotation = MM_MPEG4_NO_ROTATION;
        }

        /* Set time interval between tick updates */
        (void)IMEDIA_SetTickTime(pMe->pMedia, 1000);
        if(IMEDIA_GetTotalTime(pMe->pMedia) != SUCCESS)
        {
          QCAMCORDER_MSG_ERROR("Could not get Media Time",0,0,0);
          pMe->m_dwTotalTime = 100; /* 100 ms */
        }

        /* Enable frame callbacks so we can display them */
        (void)IMEDIA_EnableFrameCallback(pMe->pMedia, TRUE);

        /* Set the volume to the maximum */
        (void)IMEDIA_SetVolume(pMe->pMedia, AEE_MAX_VOLUME);

        /* Start playing the clip */
        (void)IMEDIA_Play(pMe->pMedia);
      }
    }

    /* MM_STATUS_FRAME nStatus is available only if frame callbacks are
    ** initialized via IMEDIA_EnableFrameCallback. Much of the code is
    ** adapted from OEMMediaMPEG4_CallbackNotify. The purpose is to support
    ** full screen display within Qcamcorder context (QTV does not plan
    ** to add such support) */
    else if(pCmdNotify->nStatus == MM_STATUS_FRAME)
    {
      IBitmap *pFrame = NULL;

      /* Retrieve frame for display when we receive this status */
      if(pMe->pMedia && IMEDIA_GetFrame(pMe->pMedia, &pFrame) != SUCCESS)
      {
        QCAMCORDER_MSG_ERROR("Cannot get frame",0,0,0);
      }
      else
      {
        if(pFrame != NULL)
        {
          AEEBitmapInfo frameInfo;

          MEMSET(&frameInfo,0,sizeof(AEEBitmapInfo));

          /* Obtain the size of the bitmap */
          (void)IBITMAP_GetInfo(pFrame, &frameInfo, sizeof(frameInfo));

          /* Display frame only with valid dimensions without scaling */
          if(frameInfo.cx != 0 && frameInfo.cy != 0)
          {
            IDIB   *pDIB = NULL;
            IYCbCr *pYCbCr = NULL;

            /* Let's see what the IBitmap really is.  First check IDIB */
            if( SUCCESS == IBITMAP_QueryInterface(pFrame, AEECLSID_DIB, (void**)&pDIB ) &&
                pDIB != NULL )
            {

#ifdef FEATURE_VIDEO_ENCODE_VDEC_SUPPORT
              VideoPlayback.image.imgType   = MDP_RGB_565;
#else
              VideoPlayback.image.imgType   = MDP_YCBCR420;
              VideoPlayback.image.cbcr_addr = (uint32*)&pDIB->pBmp[frameInfo.cx*frameInfo.cy];
#endif /* FEATURE_VIDEO_ENCODE_VDEC_SUPPORT */

              VideoPlayback.image.bmy_addr  = (uint32*)pDIB->pBmp;
            }
            else if( SUCCESS == IBITMAP_QueryInterface(pFrame, AEEIID_YCBCR, (void**)&pYCbCr) &&
                     pYCbCr != NULL )
            {

#ifdef FEATURE_VIDEO_ENCODE_VDEC_SUPPORT
              VideoPlayback.image.imgType   = MDP_Y_CBCR_H2V2;
#else
              VideoPlayback.image.imgType   = MDP_YCBCR420;
#endif /* FEATURE_VIDEO_ENCODE_VDEC_SUPPORT */

              VideoPlayback.image.bmy_addr  = (uint32*)pYCbCr->pLuma;
              VideoPlayback.image.cbcr_addr = (uint32*)pYCbCr->pChroma;
            }
            else
            {
              QCAMCORDER_MSG_ERROR("Couldn't get IDIB/IYCbCr(%d,%d)", pDIB, pYCbCr, 0);

              (void)IBITMAP_Release(pFrame);
              pFrame = NULL;
              return;
            }

            /* Select which MDP layer to display frames and
            ** define top left corner of display region */
            if(pMe->m_playbackSize == QCAMCORDER_PLAYBACK_REGULAR)
            {
              VideoPlayback.image.layer = MDP_FGL;
              /* cxScreen,cyScreen is the width and height of screen excluding brew offsets.
                 if() conditions check it the size of video frame fits into the total screen size.
              */
              if ((pMe->di.cxScreen + BREW_OFFSET_X) >= (frameInfo.cx))
              {
                VideoPlayback.image.lcd_x = (int32)(pMe->di.cxScreen -
                                             frameInfo.cx -
                                             BREW_OFFSET_X) >> 1;
              }
              else
              {
                VideoPlayback.image.lcd_x = 0 - BREW_OFFSET_X;
              }
              if ((pMe->di.cyScreen + BREW_OFFSET_Y) >= (frameInfo.cy + 
                                                         QCAMCORDER_SOFT_MENU_HEIGHT + 
                                                         QCAMCORDER_PROGRESS_BAR_HEIGHT))
              {
                /* The total screen size(cyScreen + BREW_OFFSET_Y) should be greater than the 
                   total size of the objects to be displayed in the screen(video frame+progressbar+softmenu)
                */
                VideoPlayback.image.lcd_y = (int32)(pMe->di.cyScreen -
                                             frameInfo.cy -
                                             BREW_OFFSET_Y -
                                             QCAMCORDER_SOFT_MENU_HEIGHT - 
                                             QCAMCORDER_PROGRESS_BAR_HEIGHT) >> 1;
              }
              else
              {
                VideoPlayback.image.lcd_y = 0 - BREW_OFFSET_Y;
              }
            }
            else if(pMe->m_playbackSize == QCAMCORDER_PLAYBACK_FULL_SCREEN)
            {
              VideoPlayback.image.layer = MDP_BGL;
              VideoPlayback.image.lcd_x = 0 - BREW_OFFSET_X;
              VideoPlayback.image.lcd_y = 0 - BREW_OFFSET_Y;
            }
            else
            {
              QCAMCORDER_MSG_ERROR("Invalid playback mode %d", pMe->m_playbackSize, 0, 0);

              (void)IBITMAP_Release(pFrame);
              pFrame = NULL;
              return;
            }

            /* Common MDP config regardless of frame display size */
            VideoPlayback.image.dest      = PRIMARY_LCD_TYPE;
            VideoPlayback.image.width     = (uint16)frameInfo.cx;
            VideoPlayback.image.cwin.x1   = 0;
            VideoPlayback.image.cwin.y1   = 0;
            VideoPlayback.image.cwin.x2   = (int32)(frameInfo.cx - 1);
            VideoPlayback.image.cwin.y2   = (int32)(frameInfo.cy - 1);

            /* Get MDP to rotate display to fill screen if necessary
            ** We do not support any form of scaling */
            if(VideoPlayback.rotation == MM_MPEG4_90_CW_ROTATION)
              VideoPlayback.image.mdpOp = MDPOP_ROT90;
            else
              VideoPlayback.image.mdpOp = MDPOP_NOP;

            /* Check if the current MDP handle is valid */
            if(mdp_is_ovimg_valid(VideoPlayback.id))
            {
              (void)mdp_replace_ovimg(&VideoPlayback.image, VideoPlayback.id);
            }
            else
            {
              /* MDP handle wasn't registered, so register a new one */
              VideoPlayback.id = mdp_register_ovimg(&VideoPlayback.image);
              QCAMCORDER_MSG_HIGH("Registered new MDP ID %d", VideoPlayback.id, 0, 0);
            }

            /* Check MDP handle just to make sure */
            if(VideoPlayback.id != 0)
            {
              /* Update the display */
              mdp_update_ovimg(VideoPlayback.id);
            }
            else
            {
              QCAMCORDER_MSG_ERROR("MDP registration/replacement failed!!!", 0, 0, 0);
            }

            /* Release IDIB pointer on a per frame basis */
            if( pDIB != NULL )
            {
              (void)IDIB_Release(pDIB);
              pDIB = NULL;
            }
            if( pYCbCr != NULL )
            {
              (void)IYCBCR_Release( pYCbCr );
              pYCbCr = NULL;
            }
          }
        }
      }

      /* Release pFrame pointer on per frame basis  - similar to IDIB */
      if(pFrame)
      {
        (void)IBITMAP_Release(pFrame);
        pFrame = NULL;
      }
    }

    /* This event will only be invoked if there is telop stream */
    else if(pCmdNotify->nStatus == MM_MP4_STATUS_TELOP_TEXT)
    {
      uint32                i;
      char                  *pTxtStr  = NULL;
      AEETelopTextSubString *pTxtSubStr;
      AEETelopTextElement   *pTxtElement =
                            (AEETelopTextElement *) (pCmdNotify->pCmdData);

      pTxtStr = (char *)MALLOC(pTxtElement->dwTelopSize);

      if (pTxtStr)
      {
        MEMSET(pTxtStr, 0, pTxtElement->dwTelopSize);

        for (i = 0; i < pTxtElement->dwNumberOfSubStrings; i++)
        {
          IMEDIA_GetMediaParm(pMe->pMedia, MM_MP4_PARM_TELOP_SUB_STR,
                              (int32 *) & pTxtSubStr, (int32 *) & i);
          if (pTxtSubStr && pTxtSubStr->pTextSubString)
          {
            (void) STRLCPY(pMe->TelopTextInfo, pTxtSubStr->pTextSubString,
                            USER_TELOP_MAX_LENGTH);
          }
        }

        FREEIF(pTxtStr);
      }
    }

  } // MM_CMD_PLAY
  else if (pCmdNotify->nCmd == MM_CMD_GETTOTALTIME)
  {
    if (pCmdNotify->nStatus == MM_STATUS_DONE)
    {
      pMe->m_dwTotalTime = ((uint32)pCmdNotify->pCmdData);
    }
  }
  else if (pCmdNotify->nCmd == MM_CMD_RECORD)
  {
    if (pCmdNotify->nStatus == MM_STATUS_DONE ||
        pCmdNotify->nStatus == MM_STATUS_ABORT)
    {
      QCAMCORDER_IMEDIA_RELEASE(pMe->pRecordMedia);
      QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);
      pMe->eState = QCAMCORDER_EDIT;
      Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_EDIT);
    }
  }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}


#ifdef FEATURE_VIDEO_AUDIO_DUBBING
/* ==========================================================================
FUNCTION QCAMCORDER_RECORDAUDIO
DESCRIPTION Plays the current movie while recording audio for audio dubbing.
============================================================================= */
static void Qcamcorder_RecordAudio(Qcamcorder * pMe)
{  
   AEERect rectImage;
   char *pFileExtension = NULL;
   boolean AudioUnsupported = FALSE;
   uint32   err_msg;

   if (pMe->AudioDubbing)
   {
     QCAMCORDER_MSG_HIGH("Key Press too fast, ignoring Keypress in Qcamcorder_RecordAudio" ,0,0,0);
     return;
   }
   QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
   QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);

   pMe->AudioDubbing = TRUE;

   /* Free data used in creating IMedia object for audio dubbing*/
   if (pMe->m_pmdExList)
   {
       if (pMe->m_pmdExList->pData)
       {
           FREE(pMe->m_pmdExList->pData);
           pMe->m_pmdExList->pData = NULL;
       }
       FREE(pMe->m_pmdExList);
       pMe->m_pmdExList = NULL;
   }

   pMe->m_pmdExList = (AEEMediaDataEx *) MALLOC(sizeof(AEEMediaDataEx));
   if (pMe->m_pmdExList == NULL)
   {
      err_msg = IDB_POSTEDIT_ERROR;
      goto RECORD_AUDIO_DUB_ERROR;
   }

   /* Set media data properties*/
   pMe->m_pmdExList->clsData = MMD_FILE_NAME;
   pMe->m_pmdExList->pData = MALLOC(MAX_FILE_NAME);
   if (pMe->m_pmdExList->pData == NULL)
   {
     err_msg = IDB_POSTEDIT_ERROR;
     goto RECORD_AUDIO_DUB_ERROR;
   }

   // If audio codec is not supported by existing file format then exit the audio dub operation
   
   pFileExtension = STRRCHR(pMe->sCurrentFileName, '.') ;
   if(pFileExtension == NULL)
   {
      QCAMCORDER_MSG_ERROR("No file extension present",0,0,0);
      err_msg = IDB_POSTEDIT_ERROR;
      goto RECORD_AUDIO_DUB_ERROR;
   }
   if( !(STRNCMP(pFileExtension, QCAMCORDER_MP4_MOVIE_EXTENSION,4)) ||
       !(STRNCMP(pFileExtension, QCAMCORDER_AMC_MOVIE_EXTENSION,4)) )
   {
     if(pMe->wAudioFormat != QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K &&
        pMe->wAudioFormat != QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K)
     {
       AudioUnsupported = TRUE;
     }
   }
   else if(!STRNCMP(pFileExtension, QCAMCORDER_3GP_MOVIE_EXTENSION,4) &&
          (pMe->wAudioFormat != QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR))
   {
     AudioUnsupported = TRUE;
   }
   if(AudioUnsupported)
   {
     QCAMCORDER_MSG_ERROR("Audio Codec is not supported by File format",0,0,0);
     err_msg = IDB_POSTEDIT_UNSUPPORTED;
     goto RECORD_AUDIO_DUB_ERROR;
   }

   //as AEEMediaUtil_CreateMediaEx is an OEMlayer call, path has to be translated to mmc1/... or ft1/... from fs:/shared/...
#ifdef FEATURE_BREW_3_0

   if(STRSTR(pMe->sCurrentFileName, "card0") || STRSTR(pMe->sCurrentFileName, "mmc"))
   {
      STRLCPY(pMe->m_pmdExList->pData, "mmc1/",MAX_FILE_NAME);
      STRLCAT(pMe->m_pmdExList->pData, 
             STRCHR(STRCHR(pMe->sCurrentFileName, '/')+1, '/')+1, MAX_FILE_NAME);
   }
#ifdef FEATURE_EFS_FTL
   else if(STRSTR(pMe->sCurrentFileName, AEEFS_FTL_DIR) || STRSTR(pMe->sCurrentFileName, "ftl"))
   {
      STRLCPY(pMe->m_pmdExList->pData, "ftl1/", MAX_FILE_NAME);
      STRLCAT(pMe->m_pmdExList->pData, 
      STRCHR(STRCHR(STRCHR(pMe->sCurrentFileName, '/')+1, '/')+1, '/')+1, MAX_FILE_NAME);
   }
#endif //FEATURE_EFS_FTL
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
   else if(STRSTR(pMe->sCurrentFileName, AEEFS_USB_DIR) || STRSTR(pMe->sCurrentFileName, "usb"))
   {
      STRLCPY(pMe->m_pmdExList->pData, "usb1/", MAX_FILE_NAME);
      STRLCAT(pMe->m_pmdExList->pData, 
                STRCHR(STRCHR(pMe->sCurrentFileName, '/')+1, '/')+1,MAX_FILE_NAME);
   }
#endif //FEATURE_EFS_HOTPLUG_USB_HOST_MS
   else
#endif//FEATURE_BREW_3_0
   STRLCPY(pMe->m_pmdExList->pData, pMe->sCurrentFileName, MAX_FILE_NAME);

   pMe->m_pmdExList->dwSize = STRLEN(pMe->m_pmdExList->pData);
   pMe->m_pmdExList->dwStructSize = sizeof(pMe->m_pmdExList);
   pMe->m_pmdExList->dwCaps = MM_CAPS_VIDEO; /* video capability only */

   if(AEEMediaUtil_CreateMediaEx(pMe->a.m_pIShell, pMe->m_pmdExList, 1, &pMe->pMedia) != SUCCESS)
   {
     QCAMCORDER_MSG_HIGH("Could not create media object",0,0,0);
     err_msg = IDB_POSTEDIT_ERROR;
     goto RECORD_AUDIO_DUB_ERROR;
   }


   if(pMe->pMedia)
   {
     // Rect dimensions dependent on QTV full screen display size
     SETAEERECT(&rectImage, 0, 0,
                pMe->di.cxScreen, 
                pMe->di.cyScreen - QCAMCORDER_SOFT_MENU_HEIGHT -
                                 QCAMCORDER_PROGRESS_BAR_HEIGHT);
     (void)IMEDIA_SetRect(pMe->pMedia, &rectImage, NULL);
     if(IMEDIA_RegisterNotify(pMe->pMedia, Qcamcorder_PlayMediaNotify, pMe) != SUCCESS)
     {
       QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);
       err_msg = IDB_POSTEDIT_ERROR;
       goto RECORD_AUDIO_DUB_ERROR;
     }
   }

   return;

   RECORD_AUDIO_DUB_ERROR:
   //to display error message in case of any failure or non supported request in audio dubbing

   if (pMe->m_pmdExList)
   {
     if (pMe->m_pmdExList->pData)
     {
        FREE(pMe->m_pmdExList->pData);
        pMe->m_pmdExList->pData = NULL;
     }
     FREE(pMe->m_pmdExList);
     pMe->m_pmdExList = NULL;
   }

   pMe->eState = QCAMCORDER_AUDIODUB_ABORT;
   pMe->msg = err_msg;
   pMe->AudioDubbing = FALSE;
   Qcamcorder_ErrorEventUserNotify(pMe);

}


static void Qcamcorder_StartRecordAudio(Qcamcorder * pMe)
{
   int nReturnStatus= SUCCESS;
   AEEMediaData md;

   md.clsData = MMD_FILE_NAME;
   md.pData = QCAMCORDER_TMP_AUDIO_FILE;
   md.dwSize = 0;
   pMe->AudioDubbing = TRUE;

   pMe->wAudioFormatUsed=pMe->wAudioFormat;
#ifdef FEATURE_MP4_DECODER
   if(pMe->wAudioFormatUsed != QCAMCORDER_AUDIO_FORMAT_NONE)
   {
    nReturnStatus = OEMMediaMPEG42PV_SetAudDub(pMe->wAudioFormatUsed);
   }
   if(nReturnStatus != SUCCESS)
   {
     QCAMCORDER_MSG_HIGH("OEMMediaMPEG42PV_SetAudDub returned error",0,0,0);
   }
   QCAMCORDER_IMEDIA_RELEASE(pMe->pRecordMedia);
   if(AEEMediaUtil_CreateMedia(pMe->a.m_pIShell, &md, &pMe->pRecordMedia) != SUCCESS)
   {
     QCAMCORDER_MSG_HIGH("Could not create media object",0,0,0);
     return;
   }
#endif//FEATURE_MP4_DECODER
   if (pMe->pRecordMedia)
   {
     if(pMe->wAudioFormatUsed == QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR)
     {
       (void)IMEDIAQCP_SetFormat(pMe->pRecordMedia, MM_AMR_FORMAT_FIXED_12_2_AMR);
     }
     else
     {
      (void)IMEDIAQCP_SetFormat(pMe->pRecordMedia, pMe->wAudioFormatUsed);
     }
     (void)IMEDIA_RegisterNotify(pMe->pRecordMedia, Qcamcorder_PlayMediaNotify, pMe);
     // Start playing
     Qcamcorder_PlayAudioDubMovie(pMe);
     pMe->eState = QCAMCORDER_AUDIODUB;
   }
   else
   {
     QCAMCORDER_MSG_ERROR("NULL pRecordMedia",0,0,0);
     return;
   }
   IDISPLAY_Update(pMe->a.m_pIDisplay);
}


char* QCamcorder_GetAuddubfileName(Qcamcorder* pMe)
{
  char         dirName[FILE_NAME_SIZE] = {0};
  static char  fileName[FILE_NAME_SIZE] = {0};
  char         num[FILE_NAME_SIZE] = {0};
  char         s_version[FILE_NAME_SIZE] = {0};
  static uint8 version = 0;
  char         *filename = pMe->sCurrentFileName;
  char         *pExt = NULL;
  char         *newfile = NULL;

  pExt = STRSTR(filename, ".");
  newfile = STRCHR(filename, '.');
  newfile -=3;
  (void)STRLCPY(num, newfile, STD_MIN(4,sizeof(num)));

  (void)STRLCPY(dirName, Qcamcorder_GetDirectory(pMe), sizeof(dirName));
  (void)STRLCAT(dirName, DIRECTORY_STR,sizeof(dirName));

  newfile -=3;
  if(!(STRNCMP(newfile, "mov", 3)))
    version = 0;
  else
  {
    newfile+=2;
    (void)STRLCPY(s_version,newfile,STD_MIN(2,sizeof(s_version)));
    version = (uint8)((atoi(s_version)+1)%10);
  }

  (void)SNPRINTF(fileName,sizeof(fileName),"%sad%d%s%s",dirName,version,num,pExt);

  return fileName;
}

#ifdef FEATURE_BREW_IMOVIEEDIT
static void Qcamcorder_AudioDubCleanUp(Qcamcorder *pMe)
{
  char* audioDubbedFile = NULL;

  if(pMe->m_pmdExList)
  {
    if(pMe->m_pmdExList->pData)
    {
      FREE(pMe->m_pmdExList->pData);
      pMe->m_pmdExList->pData = NULL;
    }
    FREE(pMe->m_pmdExList);
    pMe->m_pmdExList = NULL;
  }

  QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
  QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);
  QCAMCORDER_IMEDIA_STOP(pMe->pRecordMedia);
  QCAMCORDER_IMEDIA_RELEASE(pMe->pRecordMedia);

  pMe->AudioDubbing = FALSE;

  if(IFILEMGR_Test(pMe->pIFileMgr, QCAMCORDER_TMP_AUDIO_FILE) == SUCCESS)
  {
    (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_AUDIO_FILE);
  }

  audioDubbedFile = (void*)QCamcorder_GetAuddubfileName(pMe);
  if(IFILEMGR_Test(pMe->pIFileMgr, audioDubbedFile) == SUCCESS)
  {
    (void)IFILEMGR_Remove(pMe->pIFileMgr, audioDubbedFile);
  }
}


static void Qcamcorder_AudioDub(Qcamcorder *pMe)
{
  char *pData = (void *)QCamcorder_GetAuddubfileName(pMe);
  AEEMediaData src;

  if (!pMe->pIFileMgr) {
    (void)ISHELL_CreateInstance(
      pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pMe->pIFileMgr);
  }
  if (IFILEMGR_Test(pMe->pIFileMgr, pData) == SUCCESS) {
    if (IFILEMGR_Remove(pMe->pIFileMgr, pData) != SUCCESS) {
      QCAMCORDER_MSG_HIGH("COULD NOT DELETE THE AUDIO DUBBING FILE",0,0,0);
      return;
    }
  }

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IMOVIEEDIT,
                            (void **)&(pMe->m_pInEdit)) !=  SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("Audio Dubbing cannot create infile",0,0,0);
    return;
  }

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IMOVIEEDIT,
                            (void **)&(pMe->m_pOutEdit)) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("Audio Dubbing cannot create outfile",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
    return;
  }

  src.clsData = MMD_FILE_NAME;
  src.pData = pMe->sCurrentFileName;
  CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_AudioDubReadOpenCB, pMe);
  if (IMOVIEEDIT_ReadOpen(pMe->m_pInEdit, &src,
                           &(pMe->m_mei), &(pMe->m_movieeditStatus),
                           &(pMe->m_movieeditCB)) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("Audio dubbing cannot open input file",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
    return ;
  }

}

#else /* FEATURE_BREW_IMOVIEEDIT */

/* ==========================================================================
FUNCTION QCAMCORDER_AUDIODUB
DESCRIPTION Takes the current video and changes the audio to the audio
that was just recorded and saves to another mp4 file.
============================================================================= */
static void Qcamcorder_AudioDub(Qcamcorder * pMe)
{
   IMovieFile * pInFile = NULL, * pOutFile = NULL;
   IFile * pSoundFile = NULL;
   AEEMovieFileInfo mfi;
   AEEMovieSampleInfo * pSampleInfo=NULL, videoInfo;
   AEEMovieStreamInfo streams[2];
   uint16 headerSize, packetSize;
   uint16 atomNum = 0, i;
   uint16 stream=0;
   boolean noAudio = FALSE;
   uint8 * pBuf = NULL, *pVideoBuf = NULL;
   uint32 amt, offset;
   int32 extra_bytes;
   char *pExt = NULL;
   char *newfile= NULL;
   char *tempFile = NULL;
   uint32 numbytesread=0;
   uint32 curoffset=0 , lastoffset = 0, rpt=0, additional=0;
   int status = SUCCESS;
   mfi.cbSize = sizeof(AEEMovieFileInfo);

   if (pMe->eState == QCAMCORDER_AUDIODUB_ABORT)
      goto AUDIODUB_END;

   // Create the two moviefile instances
   if ((status = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE, (void **)&pInFile)) !=
       SUCCESS)
      goto AUDIODUB_END;

   if ((status = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE, (void **)&pOutFile)) !=
       SUCCESS)
      goto AUDIODUB_END;

   // Open the input file for reading
   if ((status = IMOVIEFILE_OpenMp4Read(pInFile, pMe->sCurrentFileName, &mfi)) != SUCCESS)
      goto AUDIODUB_END;

   // Copy the video track over
   for (i = 0; i < mfi.nStreams; ++i)
   {
      if (mfi.pStreams[i].format == AEECLSID_MPEG4_VIDEO)
      {  stream = i;
         (void)MEMCPY(&streams[0], &mfi.pStreams[i], sizeof(AEEMovieStreamInfo));
      }
   }

   // Open the audio for for reading
   if ((pSoundFile = IFILEMGR_OpenFile(pMe->pIFileMgr, QCAMCORDER_TMP_AUDIO_FILE, _OFM_READ)) == NULL)
   {   noAudio = TRUE;
   }

   // Write the audio track data based on our audio input format
   mfi.pStreams = streams;
   streams[1].cbSize = sizeof(AEEMovieStreamInfo);
   streams[1].format = AEECLSID_MEDIAQCP;
   streams[1].nSubFormat = pMe->wAudioFormatUsed;
   streams[1].nTimescale = 8000;
   streams[1].dwLength = 160;
   streams[1].dwFrames = 1;

   /* Copy the language bit settings from video */
   streams[1].rgchLanguage[0] = streams[stream].rgchLanguage[0];
   streams[1].rgchLanguage[1] = streams[stream].rgchLanguage[1];
   streams[1].rgchLanguage[2] = streams[stream].rgchLanguage[2];

   if(noAudio)
   {mfi.nStreams = 1;
   }
   else
   {mfi.nStreams = 2;
    pExt = STRSTR(pMe->sCurrentFileName, ".") ;

    if(!STRCMP(pExt, QCAMCORDER_MP4_MOVIE_EXTENSION))
    {if(pMe->wAudioFormatUsed != QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K &&
        pMe->wAudioFormatUsed != QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K)
     {pMe->wAudioFormatUsed = QCAMCORDER_AUDIO_FORMAT_NONE;
      mfi.nStreams = 1;
      noAudio = TRUE;
     }
    }
    else if(!STRCMP(pExt, QCAMCORDER_AMC_MOVIE_EXTENSION))
    {if(pMe->wAudioFormatUsed != QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K &&
        pMe->wAudioFormatUsed != QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K)
     {pMe->wAudioFormatUsed = QCAMCORDER_AUDIO_FORMAT_NONE;
      mfi.nStreams = 1;
      noAudio = TRUE;
     }
    }
    else if(!STRCMP(pExt, QCAMCORDER_3GP_MOVIE_EXTENSION))
    {if(pMe->wAudioFormatUsed != QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR)
     {pMe->wAudioFormatUsed = QCAMCORDER_AUDIO_FORMAT_NONE;
      mfi.nStreams = 1;
      noAudio = TRUE;
     }
    }


   }
   // Figure out the size of the header and packets size of the audio
   switch (pMe->wAudioFormatUsed)
   {
   case QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K:
      headerSize = 162;
      packetSize = VOCODER_PACKET_SIZE_13K_FULL;
      break;
   case QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K:
      headerSize = 162;
      packetSize = VOCODER_PACKET_SIZE_13K_HALF;
      break;
   case QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_EVRC:
      headerSize = 0;
      packetSize = VOCODER_PACKET_SIZE_EVRC;
      break;
   case QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR:
      headerSize = 0;
      packetSize = VOCODER_PACKET_SIZE_AMR;
      break;
     case QCAMCORDER_AUDIO_FORMAT_NONE:
       mfi.nStreams = 1;
       headerSize = 0;
       packetSize =0;
       break;
   default:
      status = EFAILED;
      goto AUDIODUB_END;
   };

   streams[1].dwHeaderSize = headerSize;
   newfile = QCamcorder_GetAuddubfileName(pMe);

   if(STRSTR(newfile, AEEFS_MMC_DIR) || STRSTR(newfile, "mmc"))
   {
     tempFile = QCAMCORDER_TMP_VIDEO_MMC_FILE;
   }
#ifdef FEATURE_EFS_FTL
   else if(STRSTR(newfile, AEEFS_FTL_DIR ) || STRSTR(newfile, "ftl"))
   {
     tempFile = QCAMCORDER_TMP_VIDEO_FTL_FILE;
   }
#endif /* FEATURE_EFS_FTL */

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
   else if(STRSTR(newfile, AEEFS_USB_DIR ) || STRSTR(newfile, "usb"))
   {
     tempFile = QCAMCORDER_TMP_VIDEO_USB_FILE;
   }
#endif /* FEATURE_EFS_HOTPLUG_USB_HOST_MS */
   else
   {
     tempFile = QCAMCORDER_TMP_VIDEO_FILE;
   }

   // Open the output file for writing
   if ((status = IMOVIEFILE_OpenMp4Write(pOutFile, tempFile, &mfi)) != SUCCESS)
      goto AUDIODUB_END;

   // Copy the user atoms over
   status = IMOVIEFILE_ReadUserAtom(pInFile, atomNum, NULL, &amt);
   if(status != SUCCESS)
   {goto AUDIODUB_END;
   }
   while (amt)
   {
      pBuf = (uint8 *)REALLOC(pBuf, amt);
      if (!pBuf)
      {
         status = EFAILED;
         goto AUDIODUB_END;
      }
      (void)IMOVIEFILE_ReadUserAtom(pInFile, atomNum, pBuf, &amt);
      (void)IMOVIEFILE_WriteUserAtom(pOutFile, pBuf, amt);
      ++atomNum;
      (void)IMOVIEFILE_ReadUserAtom(pInFile, atomNum, NULL, &amt);
   };
   FREE(pBuf);
   pBuf = NULL;



   pBuf = (uint8 *)MALLOC(AUDIODUB_BUF_SIZE);
   if (!pBuf)
   {
      status = EFAILED;
      goto AUDIODUB_END;
   }

   pSampleInfo = (AEEMovieSampleInfo *)MALLOC(sizeof(AEEMovieSampleInfo)*AUDIODUB_SAMPLES);
   if (!pSampleInfo)
   {
      status = EFAILED;
      goto AUDIODUB_END;
   }
   if(pSoundFile){

    if(headerSize >0)
    {
     (void)IFILE_Seek(pSoundFile, _SEEK_START, 8);
     // Copy the audio header
     (void)IFILE_Read(pSoundFile, pBuf, headerSize);

     /* Only fixed rate QCELP supported. Replace CMX QCELP
     ** audio header containing variable rate with fixed rate
     ** header */
     if(pMe->wAudioFormatUsed == QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K)
     {
       (void)MEMCPY(pBuf, QCP_FILE_HEADER_13K_FIXED_FULL, headerSize);
     }
     else if(pMe->wAudioFormatUsed == QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K)
     {
       (void)MEMCPY(pBuf, QCP_FILE_HEADER_13K_FIXED_HALF, headerSize);
     }
    }
    if(pMe->wAudioFormatUsed== QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR)
    { (void)IFILE_Seek(pSoundFile, _SEEK_START, 6);
    }
    else if (pMe->wAudioFormatUsed != QCAMCORDER_AUDIO_FORMAT_NONE){
      (void)IFILE_Seek(pSoundFile, _SEEK_START, 194);
    }
   }
   else
   { QCAMCORDER_MSG_HIGH("NO Audio in Audio dubbing",0,0,0);
   }
   pSampleInfo[0].cbSize = sizeof(AEEMovieSampleInfo);
   pSampleInfo[0].dwBytes = packetSize + headerSize;
   for (i=1; i<AUDIODUB_SAMPLES; i++)
   { pSampleInfo[i].cbSize = sizeof(AEEMovieSampleInfo);
     pSampleInfo[i].dwBytes = packetSize;
   }
   videoInfo.cbSize = sizeof(AEEMovieSampleInfo);
   extra_bytes = headerSize;

//   IMOVIEFILE_WriteStream(pOutFile, 1, 1, pSampleInfo, pBuf);

   // Copy the video stream of the input file to the output file while copying
   // audio data from the audio file at a similar rate
   offset = 0;
   while ( NULL !=
            (pVideoBuf = (uint8 *)
              IMOVIEFILE_GetSampleStream(pInFile,stream,1,offset++,
                                       &numbytesread,&videoInfo)
            )
         )
   {
     ASSERT(videoInfo.dwBytes == numbytesread);
      (void)IMOVIEFILE_WriteStream(pOutFile, 0, 1, &videoInfo, pVideoBuf);
      if(streams[stream].nTimescale)
      { curoffset = (videoInfo.dwTime * 50 )/ streams[stream].nTimescale;
      }
      else
      {QCAMCORDER_MSG_ERROR("Error in Audio Dubbing Timescale == 0",0,0,0);
      }
      while (lastoffset < curoffset)
      {
        additional  = curoffset - lastoffset;
          if (additional > AUDIODUB_SAMPLES)
              additional = AUDIODUB_SAMPLES;
          if(!noAudio){
              if(packetSize){
                rpt = (uint32)(IFILE_Read(pSoundFile, pBuf+extra_bytes, packetSize * additional) / packetSize);
              }
              else
              {QCAMCORDER_MSG_ERROR("Error in Audio Dubbing. PacketSize == 0",0,0,0);
               lastoffset +=  additional;
               break;
              }
              if(rpt){
               (void)IMOVIEFILE_WriteStream(pOutFile, 1, rpt, pSampleInfo, pBuf);
              }
          }
          lastoffset +=  additional;
          extra_bytes = 0;
          pSampleInfo[0].dwBytes = packetSize;
       }
     FREE(pVideoBuf);  pVideoBuf = NULL;
   }


  event_report_payload(EVENT_CAMCORDER_AUDIODUB,sizeof(pMe->wAudioFormatUsed),&pMe->wAudioFormatUsed);

AUDIODUB_END:

   // Release the two moviefiles
   if (pInFile){
      (void)IMOVIEFILE_Release(pInFile);
      pInFile = NULL;
   }

   if (pOutFile)
   {  (void)IMOVIEFILE_Release(pOutFile);
      pOutFile = NULL;
   }

   if (pSoundFile){
      (void)IFILE_Release(pSoundFile);
     pSoundFile = NULL;
   }

   if(IFILEMGR_Test(pMe->pIFileMgr,newfile) == SUCCESS)
   {
     (void)IFILEMGR_Remove(pMe->pIFileMgr, newfile);
   }
   (void)IFILEMGR_Rename(pMe->pIFileMgr, tempFile, newfile);
   (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_AUDIO_FILE);
   pMe->AudioDubbing = FALSE;
   FREE(pSampleInfo);
   pSampleInfo = NULL;
   FREE(pBuf);
   pBuf = NULL;
   FREE(pVideoBuf);
   pVideoBuf = NULL;
}
#endif /* FEATURE_BREW_IMOVIEEDIT */

#endif




#ifdef FEATURE_MP4_TRANSCODER
/* ==========================================================================
FUNCTION QCAMCORDER_GETVIDEOEDITFILENAME
DESCRIPTION
============================================================================= */
char* Qcamcorder_GetTranscodeFileName(Qcamcorder* pMe)
{
  char         dirName[FILE_NAME_SIZE] = {0};
  static char  fileName[FILE_NAME_SIZE] = {0};
  char         num[FILE_NAME_SIZE] = {0};
  char         s_version[FILE_NAME_SIZE] = {0};
  static uint8 version = 0;
  char         *filename = pMe->sCurrentFileName;
  char         *pExt = NULL;
  char         *newfile = NULL;

  /* Get transcoder file brand */
  switch(pMe->eTranscoderBrand)
  {
  case QCAMCORDER_FILE_XCODER_AMC:
    pExt = ".amc";
    break;
  case QCAMCORDER_FILE_XCODER_MP4:
    pExt = ".mp4";
    break;
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
  case QCAMCORDER_FILE_XCODER_3G2_FRAG:
#endif//QCAMCORDER_FILE_XCODER_3G2_FRAG:
  case QCAMCORDER_FILE_XCODER_3G2:
    pExt = ".3g2";
    break;
  case QCAMCORDER_FILE_XCODER_K3G:
    pExt = ".k3g";
    break;
  case QCAMCORDER_FILE_XCODER_3GP:
  default:
    pExt = ".3gp";
    break;
  }

  /* Extract the name and index associated with input file */
  newfile = STRCHR(filename, '.');
  newfile -=3;
  (void)STRLCPY(num, newfile, STD_MIN(4,sizeof(num)));

  (void)STRLCPY(dirName, Qcamcorder_GetDirectory(pMe), sizeof(dirName));
  (void)STRLCAT(dirName, DIRECTORY_STR,sizeof(dirName));

  newfile -=3;
  if(!(STRNCMP(newfile, "mov", 3)))
    version = 0;
  else
  {
    newfile+=2;
    (void)STRLCPY(s_version,newfile,STD_MIN(2,sizeof(s_version)));
    version = (uint8)((atoi(s_version)+1)%10);
  }

  (void)SNPRINTF(fileName,sizeof(fileName),"%sve%d%s%s",dirName,version,num,pExt);

  return fileName;
}

/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTETRANSCODERVIDEOMENU
DESCRIPTION
============================================================================= */

static void Qcamcorder_ExecuteTranscoderVideoMenu(Qcamcorder* pMe)
{
  switch( Qcamcorder_ExecuteSubMenu(pMe, SizeIDList) )
  {
#ifndef FEATURE_QVGANOTSUPPORTED
  case IDS_SIZE_QVGA:
    pMe->eTranscoderSize = QCAMCORDER_SIZE_QVGA;
    break;
#endif //#ifndef FEATURE_QVGANOTSUPPORTED
  case IDS_SIZE_QCIF:
    pMe->eTranscoderSize = QCAMCORDER_SIZE_QCIF;
    break;
  case IDS_SIZE_SQCIF:
    pMe->eTranscoderSize = QCAMCORDER_SIZE_SQCIF;
    break;
  case IDS_SIZE_MM:
  default:
    pMe->eTranscoderSize = QCAMCORDER_SIZE_MM;
    break;

  }


  return;
} /* END Qcamcorder_ExecuteShortHeaderMenu */

/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTETRANSCODERAUDIOMENU
DESCRIPTION
============================================================================= */

static void Qcamcorder_ExecuteTranscoderAudioMenu(Qcamcorder* pMe)
{
  switch( Qcamcorder_ExecuteSubMenu(pMe, AudioFormatIDList) )
  {
  case IDS_AUDIO_FORMAT_13K_FULL:
    pMe->wTranscodeAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K;
    break;
  case IDS_AUDIO_FORMAT_13K_HALF:
    pMe->wTranscodeAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K;
    break;
  case IDS_AUDIO_FORMAT_EVRC:
    pMe->wTranscodeAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_EVRC;
    break;
  case IDS_AUDIO_FORMAT_AMR:
    pMe->wTranscodeAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR;
    break;
  case IDS_NONE:
  default:
    pMe->wTranscodeAudioFormat = QCAMCORDER_AUDIO_FORMAT_NONE;
    break;
  }
  return;
} /* END Qcamcorder_ExecuteTranscoderAudioMenu */

/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTETRANSCODERVIDEOCODECMENU
DESCRIPTION
============================================================================= */

static void Qcamcorder_ExecuteTranscoderVideoCodecMenu(Qcamcorder* pMe)
{
  switch( Qcamcorder_ExecuteSubMenu(pMe, TranscoderVideoCodecIDList) )
  {
#ifdef FEATURE_H264_ENCODER
  case IDS_FORMAT_H264:
    pMe->eTranscoderVideoCodec  = QCAMCORDER_VIDEO_FORMAT_H264;
    pMe->wTranscoderShortHeader = QCAMCORDER_SH_DISABLE;
    break;
#endif
  case IDS_FORMAT_H263:
    pMe->eTranscoderVideoCodec  = QCAMCORDER_VIDEO_FORMAT_H263;
    pMe->wTranscoderShortHeader = QCAMCORDER_SH_ENABLE;
    break;
  case IDS_FORMAT_MPEG4:
  default:
    pMe->eTranscoderVideoCodec  = QCAMCORDER_VIDEO_FORMAT_MPEG4;
    pMe->wTranscoderShortHeader = QCAMCORDER_SH_DISABLE;
    break;
  }

  /* Ensure frame size parameters are supported by short header */
  if(pMe->wTranscoderShortHeader == QCAMCORDER_SH_ENABLE)
  {
    if(pMe->eTranscoderSize == QCAMCORDER_SIZE_QVGA)
    {
      pMe->eTranscoderSize = QCAMCORDER_SIZE_QCIF;
    }
    else if(pMe->eTranscoderSize == QCAMCORDER_SIZE_MM)
    {
      pMe->eTranscoderSize = QCAMCORDER_SIZE_SQCIF;
    }
  }
  return;
} /* END Qcamcorder_ExecuteTranscoderVideoFormatMenu */

/* ==========================================================================
FUNCTION QCAMCORDER_EXECUTETRANSCODERBRANDMENU
DESCRIPTION
============================================================================= */

static void Qcamcorder_ExecuteTranscoderBrandMenu(Qcamcorder* pMe)
{
  switch( Qcamcorder_ExecuteSubMenu(pMe, TranscoderBrandIDList) )
  {
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
  case IDS_FILE_3G2_FRAG:
      pMe->eTranscoderBrand = QCAMCORDER_FILE_XCODER_3G2_FRAG;
      if(pMe->wTranscodeAudioFormat != QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR &&
         pMe->wTranscodeAudioFormat != QCAMCORDER_AUDIO_FORMAT_NONE)

          {pMe->wTranscodeAudioFormat = QCAMCORDER_AUDIO_FORMAT_NONE;

          }
      break; 
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
  case IDS_FILE_3GP:
    pMe->eTranscoderBrand = QCAMCORDER_FILE_XCODER_3GP;
    if(pMe->wTranscodeAudioFormat != QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR &&
       pMe->wTranscodeAudioFormat != QCAMCORDER_AUDIO_FORMAT_NONE)

        {pMe->wTranscodeAudioFormat = QCAMCORDER_AUDIO_FORMAT_NONE;

        }
    break;

  case IDS_FILE_MP4:
    pMe->eTranscoderBrand = QCAMCORDER_FILE_XCODER_MP4;
    if(pMe->wTranscodeAudioFormat != QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K &&
       pMe->wTranscodeAudioFormat != QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K)
    {pMe->wTranscodeAudioFormat = QCAMCORDER_AUDIO_FORMAT_NONE;
    }

    break;

  case IDS_FILE_3G2:
    pMe->eTranscoderBrand = QCAMCORDER_FILE_XCODER_3G2;
    break;

  case IDS_FILE_K3G:
    pMe->eTranscoderBrand = QCAMCORDER_FILE_XCODER_K3G;
    break;

  case IDS_FILE_AMC:
  default:

    pMe->eTranscoderBrand = QCAMCORDER_FILE_XCODER_AMC;
    if(pMe->wTranscodeAudioFormat != QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K &&
       pMe->wTranscodeAudioFormat != QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K)
    {pMe->wTranscodeAudioFormat = QCAMCORDER_AUDIO_FORMAT_NONE;
    }

    //checking for size support
    if(pMe->eTranscoderSize == QCAMCORDER_SIZE_QCIF ||
      pMe->eTranscoderSize == QCAMCORDER_SIZE_QVGA
      )
    {
      pMe->eTranscoderSize = QCAMCORDER_SIZE_SQCIF;
    }

#ifdef FEATURE_H264_ENCODER
    //checking for brand support
    if( pMe->eTranscoderVideoCodec == QCAMCORDER_VIDEO_FORMAT_H264)
      pMe->eTranscoderVideoCodec = QCAMCORDER_VIDEO_FORMAT_MPEG4;
#endif
    break;

  }
  return;
} /* END Qcamcorder_ExecuteTranscoderBrandMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDFILESIZEUBMENU
DESCRIPTION
   Builds the Size submenu.
============================================================================= */
static void Qcamcorder_BuildTranscoderSizeSubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected)
{
   AEERect qrc;
   CtlAddItem ai;
   uint16 i=0;

   // Delete any previous menu
   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 160 - qrc.y);
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, idList[1], NULL);

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 0;

   if(pMe->eTranscoderBrand != QCAMCORDER_FILE_XCODER_AMC)
   {

#ifndef FEATURE_QVGANOTSUPPORTED
     if(pMe->wTranscoderShortHeader != QCAMCORDER_SH_ENABLE)
     {
       //QVGA
       ai.wText    = idList[i+2];
       ai.wItemID  = idList[i+2];
       ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
       (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
       ai.dwData++;
     }
#endif //#ifndef FEATURE_QVGANOTSUPPORTED

     //QCIF
    ai.wText    = idList[i+3];
    ai.wItemID  = idList[i+3];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
   }

   //SQCIF
     ai.wText    = idList[i+4];
     ai.wItemID  = idList[i+4];
     ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
     (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
     ai.dwData++;

   if(pMe->wTranscoderShortHeader != QCAMCORDER_SH_ENABLE)
   {
     //MOVIEMAIL
     ai.wText    = idList[i+5];
     ai.wItemID  = idList[i+5];
     ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
     (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
     ai.dwData++;
   }

   IMENUCTL_SetSel(pMe->m_pIMenu, selected);
   return;
} /* END QCam_BuildSizeSubMenu */
/* ==========================================================================
FUNCTION QCAM_BUILDSIZEMENU
DESCRIPTION
   Builds the Size menu.
============================================================================= */
static void Qcamcorder_BuildTranscoderSizeMenu(Qcamcorder *pMe)
{
  uint16 focus  = 0;

  switch( pMe->eTranscoderSize )
  {
#ifndef FEATURE_QVGANOTSUPPORTED
  case QCAMCORDER_SIZE_QVGA:
    focus = IDS_SIZE_QVGA;
    break;
#endif //#ifndef FEATURE_QVGANOTSUPPORTED
  case QCAMCORDER_SIZE_QCIF:
    focus = IDS_SIZE_QCIF;
    break;
  case QCAMCORDER_SIZE_SQCIF:
    focus = IDS_SIZE_SQCIF;
    break;
  case QCAMCORDER_SIZE_MM:
  default:
    focus = IDS_SIZE_MM;
    break;

  }

  Qcamcorder_BuildTranscoderSizeSubMenu(pMe, TranscoderSizeIDList, focus);

  return;
} /* END QCam_BuildSizeMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDTRANSCODERVIDEOCODECSUBMENU
DESCRIPTION
   Builds the Size submenu.
============================================================================= */
static void Qcamcorder_BuildTranscoderVideoCodecSubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected)
{
   AEERect qrc;
   CtlAddItem ai;
   uint16 i=0;

   // Delete any previous menu
   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 160 - qrc.y);
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, idList[1], NULL);

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 0;

   //mpeg4
   ai.wText    = idList[i+2];
   ai.wItemID  = idList[i+2];
   ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
   (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
   ai.dwData++;

   //h.263
   ai.wText    = idList[i+3];
   ai.wItemID  = idList[i+3];
   ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
   (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
   ai.dwData++;

#if defined(FEATURE_H264_ENCODER) && defined(FEATURE_H264_TRANSCODER)
   if(pMe->eTranscoderBrand != QCAMCORDER_FILE_XCODER_AMC)
   {
     //h.264
     ai.wText    = idList[i+4];
     ai.wItemID  = idList[i+4];
     ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
     (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
     ai.dwData++;
   }
#endif //FEATURE_H264_ENCODER

   IMENUCTL_SetSel(pMe->m_pIMenu, selected);
   return;
} /* END Qcamcorder_BuildTranscoderVideoCodecSubMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDTRANSCODERVIDEOCODECMENU
DESCRIPTION
   Builds the Size menu.
============================================================================= */
static void Qcamcorder_BuildTranscoderVideoCodecMenu(Qcamcorder *pMe)
{
  uint16 focus  = 0;

  switch( pMe->eTranscoderVideoCodec)
  {
#if defined(FEATURE_H264_ENCODER) && defined(FEATURE_H264_TRANSCODER)
  case QCAMCORDER_VIDEO_FORMAT_H264:
    focus = IDS_FORMAT_H264;
    break;
#endif
  case QCAMCORDER_VIDEO_FORMAT_H263:
    focus = IDS_FORMAT_H263;
    break;
  case QCAMCORDER_VIDEO_FORMAT_MPEG4:
  default:
    focus = IDS_FORMAT_MPEG4;
    break;
  }

  Qcamcorder_BuildTranscoderVideoCodecSubMenu(pMe, TranscoderVideoCodecIDList, focus);

  return;
} /* END QCam_BuildSizeMenu */
#ifndef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
/* ==========================================================================
FUNCTION QCAM_BUILDTRANSCOVDERBRANDSUBMENU
DESCRIPTION
   Builds the Size submenu.
============================================================================= */
static void Qcamcorder_BuildTranscoderBrandSubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected)
{
   AEERect qrc;
   CtlAddItem ai;
   uint16 j=2;

   // Delete any previous menu
   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 160 - qrc.y);
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, idList[1], NULL);

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 0;

   for( ; j < 7 ; j++)
   {
     ai.wText    = idList[j];
     ai.wItemID  = idList[j];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
   }

   IMENUCTL_SetSel(pMe->m_pIMenu, selected);
   return;
} /* END QCam_BuildSizeSubMenu */
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
/* ==========================================================================
FUNCTION QCAM_BUILDTRANSCODERBRANDMENU
DESCRIPTION
   Builds the Transcoder Brand menu.
============================================================================= */
static void Qcamcorder_BuildTranscoderBrandMenu(Qcamcorder *pMe)
{
  uint16 focus  = 0;

  switch( pMe->eTranscoderBrand )
  {
  case QCAMCORDER_FILE_XCODER_AMC:
    focus = IDS_FILE_AMC;
    break;
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
  case QCAMCORDER_FILE_XCODER_3G2_FRAG:
      focus = IDS_FILE_3G2_FRAG;
      break;
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
  case QCAMCORDER_FILE_XCODER_3GP:
    focus = IDS_FILE_3GP;
    break;
  case QCAMCORDER_FILE_XCODER_MP4:
    focus = IDS_FILE_MP4;
    break;
  case QCAMCORDER_FILE_XCODER_3G2:
    focus = IDS_FILE_3G2;
    break;
  case QCAMCORDER_FILE_XCODER_K3G:
    focus = IDS_FILE_K3G;
    break;
  default:
    focus = IDS_FILE_AMC;
    break;

  }
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
  Qcamcorder_BuildSubMenu(pMe, TranscoderBrandIDList,focus);
#else
  Qcamcorder_BuildTranscoderBrandSubMenu(pMe, TranscoderBrandIDList, focus);
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE

  return;
} /* END QCam_BuildSizeMenu */

static void Qcamcorder_BuildTranscoderAudioFormatSubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected)
{
   AEERect qrc;
   CtlAddItem ai;
   uint16 i=0;
   uint16 j=2;

   // Delete any previous menu
   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 160 - qrc.y);
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, idList[1], NULL);

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 0;

   //13K Full
   if(pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_MP4 ||
      pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_3G2 ||
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
      pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_3G2_FRAG ||
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
      pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_AMC ||
      pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_K3G)
   {
    ai.wText    = idList[i+j];
    ai.wItemID  = idList[i+j];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
   }
   j++;
   //13K half
   if(pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_MP4 ||
      pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_3G2 ||
      pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_AMC ||
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
      pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_3G2_FRAG ||
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
      pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_K3G)
   {
    ai.wText    = idList[i+j];
    ai.wItemID  = idList[i+j];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
   }
   j++;

  //EVRC Full
   if(pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_3G2 ||
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
      pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_3G2_FRAG ||
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
     pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_K3G)
   {
    ai.wText    = idList[i+j];
    ai.wItemID  = idList[i+j];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
   }
  j++;

   //AMR 12.2
   if(pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_3G2 ||
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
      pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_3G2_FRAG ||
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
      pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_3GP ||
      pMe->eTranscoderBrand == QCAMCORDER_FILE_XCODER_K3G)
   {
       ai.wText    = idList[i+j];
    ai.wItemID  = idList[i+j];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
   }
  j++;
  //None
      ai.wText    = idList[i+j];
    ai.wItemID  = idList[i+j];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;

   IMENUCTL_SetSel(pMe->m_pIMenu, selected);
   return;
} /* END QCam_BuildSubMenu */
/* ==========================================================================
FUNCTION QCAM_BUILDAUDIOFORMATMENU
DESCRIPTION
   Builds the Audio Format menu.
============================================================================= */
static void Qcamcorder_BuildTranscoderAudioFormatMenu(Qcamcorder *pMe)
{
  uint16 focus  = 0;

  switch( pMe->wTranscodeAudioFormat )
  {
  case QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K:
    focus = IDS_AUDIO_FORMAT_13K_FULL;
    break;
  case QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K:
    focus = IDS_AUDIO_FORMAT_13K_HALF;
    break;
  case QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_EVRC:
    focus = IDS_AUDIO_FORMAT_EVRC;
    break;
  case QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR:
    focus = IDS_AUDIO_FORMAT_AMR;
    break;
  case QCAMCORDER_AUDIO_FORMAT_NONE:
    focus = IDS_NONE;
    break;
  default:
    focus = IDS_NONE;
    break;
  }

  Qcamcorder_BuildTranscoderAudioFormatSubMenu(pMe, TranscodeAudioFormatIDList, focus);

  return;
} /* END QCam_BuildAudioFormatMenu */

#ifndef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
/**************************************************************
 * Qcamcorder_Transcoder_End
 * Cleans up the memory allocated by Transcoder
 **************************************************************/
static void Qcamcorder_Transcoder_End(Qcamcorder *pMe)
{
  if( pMe->pOutFile )
  {
    (void)IMOVIEFILE_Release(pMe->pOutFile);
    pMe->pOutFile = NULL;
  }
  if(pMe->pInFile){
   (void)IMOVIEFILE_Release(pMe->pInFile);
   pMe->pInFile = NULL;

  }

  pMe->bTranscode_portion = FALSE;
  pMe->nStartFrameNo         = 0;
  pMe->nEndFrameNo           = 0;
#ifdef FEATURE_DVS
  pMe->bDVS = FALSE;
#endif
  //Qcamcorder_ExecuteConfirmMenu(pMe);
}
#endif

/**************************************************************************************************
* Qcamcorder_DisplayFrames
* This function is used to display the frames to the user
* ARGUMENTS:
* pMe: Qcamcorder structure
* pData: Data containing image data to be displayed
***************************************************************************************************/

//static void Qcamcorder_DisplayFrame(Qcamcorder *pMe, mp4_transcoder_video_done_status_type * pData)
static void Qcamcorder_DisplayFrame(Qcamcorder *pMe, xcoder_video_cont_status_type * pData)
{ //declare, initialize locals
  IDIB *pDIB=NULL;
  IBitmap *pIBitmap = NULL;
  AEERect imageRC;

  //Set the display rectangle
  SETAEERECT (&imageRC, QCAMCORDER_PICTURE_X_POS, QCAMCORDER_PICTURE_Y_POS,
              (pMe->di.cxScreen + BREW_OFFSET_X), (pMe->di.cyScreen + BREW_OFFSET_Y));
  //lint -e570  Suppress complaint on loss of sign
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &imageRC);
  //lint +e570

  //Create the Device Independent Bitmap
  (void)IDISPLAY_CreateDIBitmap(pMe->a.m_pIDisplay, &pDIB, DISPLAY_COLOR_DEPTH, pData->width, pData->height);
  (void)MEMCPY(pDIB->pBmp, pData->rgb, pData->width * pData->height * 2);

  pDIB->nColorScheme = DISPLAY_COLOR_DEPTH;

  //get the IBitmap
  pIBitmap = IDIB_TO_IBITMAP(pDIB);

  // calculate the image position from supported LCD Device size spec.
  imageRC.x = (pMe->di.cxScreen - pData->height) >> 1;
  imageRC.y = (pMe->di.cyScreen - pData->width) >> 1;

  if( pDIB )
  { //blit the image on the screen
    IDISPLAY_BitBlt(pMe->a.m_pIDisplay, imageRC.x, imageRC.y, pDIB->cx, pDIB->cy,
                    (const void *)pIBitmap, 0, 0, AEE_RO_COPY);
  }

  //update the display
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, TRUE);
  if( pDIB )
  {
    (void)IDIB_Release(pDIB);
    pDIB = NULL;
  }
}
#endif //FEATURE_MP4_TRANSCODER
#ifdef FEATURE_DVS
/* ==========================================================================
FUNCTION QCAMCORDER_GETVIDEOEDITFILENAME
DESCRIPTION
============================================================================= */
char* Qcamcorder_GetDVSFileName(Qcamcorder* pMe, int32 *pTargetBrand )
{
  char         dirName[FILE_NAME_SIZE] = {0};
  static char  fileName[FILE_NAME_SIZE] = {0};
  char         num[FILE_NAME_SIZE] = {0};
  char         s_version[FILE_NAME_SIZE] = {0};
  static uint8 version = 0;
  char         *filename = pMe->sCurrentFileName;
  char         *pExt = NULL;
  char         *newfile = NULL;


  /* Get the file brand/extension */
  pExt = STRCHR(filename, '.');

  if(!STRCMP(pExt, ".amc"))
    *pTargetBrand = MOVIEFILE_BRAND_AMC;
  else if(!STRCMP(pExt, ".mp4"))
    *pTargetBrand = MOVIEFILE_BRAND_MP4;
  else if(!STRCMP(pExt, ".3g2"))
    *pTargetBrand = MOVIEFILE_BRAND_3G2;
  else if(!STRCMP(pExt, ".k3g"))
    *pTargetBrand = MOVIEFILE_BRAND_K3G;
  else if(!STRCMP(pExt, ".3gp"))
    *pTargetBrand = MOVIEFILE_BRAND_3GP;

  /* Get file number */
  newfile = pExt - 3;
  (void)STRLCPY(num, newfile, STD_MIN(4, sizeof(num)));

  (void)STRLCPY(dirName, Qcamcorder_GetDirectory(pMe), sizeof(dirName));
  (void)STRLCAT(dirName, DIRECTORY_STR, sizeof(dirName));

  /* If the file is already video edited, upddate the version */
  newfile -=3;
  if(!(STRNCMP(newfile, "mov", 3)))
    version = 0;
  else
  {
    newfile+=2;
    (void)STRLCPY(s_version,newfile, STD_MIN(2, sizeof(s_version)));
    version = (uint8)((atoi(s_version)+1)%10);
  }

  /* Form the target file name */
  (void)SNPRINTF(fileName, sizeof(fileName), "%svs%d%s%s",dirName,version,num,pExt);

  return fileName;
}
#endif

#ifdef FEATURE_TRANSCODER_OVERLAY_SUPPORT
#error code not present
#endif //FEATURE_TRANSCODER_OVERLAY_SUPPORT

#ifdef FEATURE_MP4_TRANSCODER
/******************************************************************************
 * This is the transcoder function call, which calls the transcoder API and
 * starts the transcoding process
 ******************************************************************************/
#ifndef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
void Qcamcorder_Transcode(Qcamcorder * pMe)
{
  int retval;
//  mp4_transcoder_return_type retval;
  AEEMovieFileInfo mfi;
  int32 xcoder_brand = MOVIEFILE_BRAND_AMC;
  xcoder_video_type transcodeVideo=XCODER_QCIF_7FPS;
  xcoder_audio_type transcodeAudio=XCODER_NOAUDIO;
  xcoder_video_codec transcoderVideoCodec = XCODER_VIDEO_MPEG4;
  _xcoder_event_payload xcoder_event_payload;
  char *pFileName;
  xcoder_params transparams;
  int16 i = 0;
  boolean isValidStream = FALSE;

#ifdef FEATURE_DVS
  if(pMe->bDVS)
    pFileName = Qcamcorder_GetDVSFileName(pMe, &xcoder_brand);
  else
    pFileName = Qcamcorder_GetTranscodeFileName(pMe);
#else
  pFileName = Qcamcorder_GetTranscodeFileName(pMe);
#endif
  (void)STRLCPY(pMe->sTranscodeOutFile,pFileName, sizeof(pMe->sTranscodeOutFile));
  (void)MEMSET(&mfi,0,sizeof(AEEMovieFileInfo));

  //if the IMovieFile object is not initialized, initialize it
  if( pMe->pOutFile == NULL )
  {
    if( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE, (void **)&(pMe->pOutFile)) !=
        SUCCESS )
      return;
  }

  if(pMe->pInFile == NULL)
  {
    if( (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE, (void **)&(pMe->pInFile))) !=
        SUCCESS )
      return;

  }
  mfi.cbSize =  sizeof(AEEMovieFileInfo);
  (void)IMOVIEFILE_OpenMp4Read(pMe->pInFile, pMe->sCurrentFileName, &mfi);

  for (i = 0; (mfi.pStreams != NULL) && (i < mfi.nStreams); ++i)
  {
    if ((mfi.pStreams[i].format == AEECLSID_MPEG4_VIDEO||
        mfi.pStreams[i].format == AEECLSID_H264_VIDEO)
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING*/
        )
    {
      isValidStream = TRUE;
      break;
    }
  }

  if(!isValidStream)
  {
    (void)IMOVIEFILE_Release(pMe->pInFile);
    pMe->pInFile = NULL;
    (void)IMOVIEFILE_Release(pMe->pOutFile);
    pMe->pOutFile = NULL;
    QCAMCORDER_MSG_HIGH("COULD NOT Transcoder file",0,0,0);

    /* If a valid video stream is not present, display error 
     * message and return to Display Album*/
    Qcamcorder_DrawIcon(pMe, IDB_POSTEDIT_UNSUPPORTED);
    IDISPLAY_Update(pMe->a.m_pIDisplay);
    pMe->eState = QCAMCORDER_EDIT;
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1000,
         (PFNNOTIFY) Qcamcorder_DisplayAlbum, (void*)pMe);
    return;
  }


  if( IFILEMGR_Test(pMe->pIFileMgr, pFileName) == SUCCESS )
  {
    if( IFILEMGR_Remove(pMe->pIFileMgr, pFileName) != SUCCESS )
    { (void)IMOVIEFILE_Release(pMe->pInFile);
      pMe->pInFile = NULL;
      (void)IMOVIEFILE_Release(pMe->pOutFile);
      pMe->pOutFile = NULL;
      QCAMCORDER_MSG_HIGH("COULD NOT Transcoder file",0,0,0);
      return;
    }
  }

#ifdef FEATURE_DVS
  {
    dvs_margin eDVSMargin;
    switch(pMe->eDVSMargin)
    {
    case QCAMCORDER_DVSMARGIN_LOW :
      eDVSMargin = DVS_MARGIN_LOW;
      break;
    case QCAMCORDER_DVSMARGIN_MED :
      eDVSMargin = DVS_MARGIN_MED;
      break;
    case QCAMCORDER_DVSMARGIN_HIGH :
      eDVSMargin = DVS_MARGIN_HIGH;
      break;
    default:
      eDVSMargin = DVS_MARGIN_LOW;
    }
  retval = IMOVIEFILE_DVSSetParams(pMe->pOutFile, pMe->bDVS, eDVSMargin);
  /*pss: check must be done on retval */
  }
  if(!pMe->bDVS)
#endif
  {
  switch( pMe->eTranscoderSize )
  {
  case QCAMCORDER_SIZE_QVGA:
    switch (pMe->m_FrameRate)
    {
      case QCAMCORDER_FRAME_RATE_1:
        transcodeVideo =  XCODER_QVGA_7FPS;
        break;
      case QCAMCORDER_FRAME_RATE_2:
        transcodeVideo = XCODER_QVGA_10FPS;
        break;
      case QCAMCORDER_FRAME_RATE_3:
      default:
        transcodeVideo = XCODER_QVGA_15FPS;
        break;
    }
    break;
  case QCAMCORDER_SIZE_QCIF:
    switch (pMe->m_FrameRate)
    {
      case QCAMCORDER_FRAME_RATE_1:
        transcodeVideo =  XCODER_QCIF_7FPS;
        break;
      case QCAMCORDER_FRAME_RATE_2:
        transcodeVideo = XCODER_QCIF_10FPS;
        break;
      case QCAMCORDER_FRAME_RATE_3:
      default:
        transcodeVideo = XCODER_QCIF_15FPS;
        break;
    }
    break;
  case QCAMCORDER_SIZE_SQCIF:
    switch (pMe->m_FrameRate)
    {
      case QCAMCORDER_FRAME_RATE_1:
        transcodeVideo =  XCODER_SUBQCIF_7FPS;
        break;
      case QCAMCORDER_FRAME_RATE_2:
        transcodeVideo = XCODER_SUBQCIF_10FPS;
        break;
      case QCAMCORDER_FRAME_RATE_3:
      default:
        transcodeVideo = XCODER_SUBQCIF_15FPS;
        break;
    }
    break;
  case QCAMCORDER_SIZE_MM:
    switch (pMe->m_FrameRate)
    {
      case QCAMCORDER_FRAME_RATE_1:
        transcodeVideo = XCODER_MOVIEMAIL_7_FPS;
        break;
      case QCAMCORDER_FRAME_RATE_2:
        transcodeVideo = XCODER_MOVIEMAIL_10_FPS;
        break;
      case QCAMCORDER_FRAME_RATE_3:
      default:
        transcodeVideo = XCODER_MOVIEMAIL_15_FPS;
        break;
    }
    break;
  default:
    transcodeVideo = XCODER_MOVIEMAIL_7_FPS;         /* 96x80, 7.5fps  */
   break;
  }
  switch(pMe->wTranscodeAudioFormat)
  {
  case QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K:
    transcodeAudio = XCODER_AUDIO_QCELP_F;
    break;
  case QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K:
    transcodeAudio = XCODER_AUDIO_QCELP_H;
    break;
  case QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_EVRC:
    transcodeAudio = XCODER_AUDIO_EVRC;
    break;
  case QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR:
    transcodeAudio = XCODER_AUDIO_AMR_12_2;
    break;
  case QCAMCORDER_AUDIO_FORMAT_NONE:
  default:
    transcodeAudio =   XCODER_NOAUDIO;
    break;


  }
  switch(pMe->eTranscoderBrand)
  {
     case QCAMCORDER_FILE_XCODER_AMC:
      xcoder_brand = MOVIEFILE_BRAND_AMC;
      break;
     case QCAMCORDER_FILE_XCODER_3GP:
       xcoder_brand = MOVIEFILE_BRAND_3GP;
      break;
     case QCAMCORDER_FILE_XCODER_MP4:
       xcoder_brand = MOVIEFILE_BRAND_MP4;
       break;
     case QCAMCORDER_FILE_XCODER_K3G:
       xcoder_brand = MOVIEFILE_BRAND_K3G;
       break;
     case QCAMCORDER_FILE_XCODER_3G2:
     default:
       xcoder_brand = MOVIEFILE_BRAND_3G2;
       break;
  }

  switch(pMe->eTranscoderVideoCodec)
  {
#ifdef FEATURE_H264_ENCODER
  case QCAMCORDER_VIDEO_FORMAT_H264:
    transcoderVideoCodec = XCODER_VIDEO_H264;
    break;
#endif
  case QCAMCORDER_VIDEO_FORMAT_H263:
    transcoderVideoCodec = XCODER_VIDEO_H263;
    break;
  case QCAMCORDER_VIDEO_FORMAT_MPEG4:
  default:
    transcoderVideoCodec = XCODER_VIDEO_MPEG4;
    break;
  }
  }
  //Stop the player if it is playing
  QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
  QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);

  //Initialize the transcoder
  xcoder_event_payload.record_video_format=pMe->wVideoFormat;
  xcoder_event_payload.record_file_format=pMe->wFileFormat;
  xcoder_event_payload.record_audio_format=(AudioFormatType)pMe->wAudioFormat;
  xcoder_event_payload.xcode_audio_format=(uint16)transcodeAudio;
  xcoder_event_payload.xcode_file_format=(TranscodeFileFormatType)xcoder_brand;
  event_report_payload(EVENT_CAMCORDER_START_TRANSCODE,sizeof(xcoder_event_payload),&xcoder_event_payload);

  transparams.nStartFrameNo = pMe->nStartFrameNo;
  transparams.nEndFrameNo = pMe->nEndFrameNo;
  transparams.nProfile = pMe->wQuality;
  transparams.nBitrate = pMe->wBitRate;
#ifdef FEATURE_TRANSCODER_OVERLAY_SUPPORT
#error code not present
#endif //FEATURE_TRANSCODER_OVERLAY_SUPPORT

  retval = IMOVIEFILE_TranscoderSetParams(
                                          pMe->pOutFile, //output IMovieFile object
                                          transparams
                                         );
  if( retval != SUCCESS )
  { //if error occured in initialization, end the transcoder
    Qcamcorder_Transcoder_End(pMe);
    pMe->eState = QCAMCORDER_EDIT;
    Qcamcorder_BuildEditMenu(pMe);
  }

  retval = IMOVIEFILE_TranscoderStart( pMe->pOutFile, //output IMovieFile object
                                       pMe->pInFile,
                                       pMe->sCurrentFileName,  //input file name
                                       pFileName, //output file name
                                       transcodeAudio,  //target audio
                                       transcodeVideo, //target video
                                       xcoder_brand,
                                       transcoderVideoCodec,
                                       &mfi
                                     );

  if( retval == EUNSUPPORTED )
  { 
     //lint -e570  Suppress complaint on loss of sign
     Qcamcorder_DrawIcon(pMe, IDB_POSTEDIT_UNSUPPORTED);
     //lint +e570
     IDISPLAY_Update(pMe->a.m_pIDisplay);
     Qcamcorder_Transcoder_End(pMe);
     pMe->eState = QCAMCORDER_EDIT;
     (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1000,
         (PFNNOTIFY) Qcamcorder_DisplayAlbum, (void*)pMe);
  }
  else if( retval != SUCCESS )
  { //if error occured in initialization, end the transcoder
    Qcamcorder_Transcoder_End(pMe);
    pMe->eState = QCAMCORDER_EDIT;
    Qcamcorder_BuildEditMenu(pMe);
  }
  else
  {//retval == SUCCESS
    //IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
     // IDISPLAY_Update(pMe->a.m_pIDisplay);
    IMENUCTL_Reset(pMe->m_pISoftMenu);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    Qcamcorder_BuildEditMenu(pMe);

  }
}
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE

/**********************************************************************************
* Qcamcorder_StartPartialTranscoding
* This function is for starting partial transcoding
* ARGUMENTS:
* pMe: Qcamcorder structure
**********************************************************************************/
static void Qcamcorder_StartPartialTranscoding(Qcamcorder *pMe)
{
  pMe->m_bEditHit = FALSE;
  if((pMe->m_dwPlayPos*1000) > pMe->m_dwTotalTime)
  {
    pMe->m_dwPlayPos = (pMe->m_dwTotalTime/1000);
  }
  if(Qcamcorder_CheckSpace(pMe))
  {
    IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
    IDISPLAY_Update(pMe->a.m_pIDisplay);
    pMe->eState=QCAMCORDER_TRANSCODE;
    //Stop the player if it is playing
    QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
    QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);

    if(Qcamcorder_TranscodePortionSelection(pMe))
    {
      (void)ISHELL_SetTimer(pMe->a.m_pIShell, 200,
                           (PFNNOTIFY)Qcamcorder_Transcode,(void *)pMe);
    }
    else
    {
      //Stop the player if it is playing
      QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
      QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);
      /* Display bitmap to user regarding cut feature error */
      pMe->msg = IDB_POSTEDIT_UNSUPPORTED;
      Qcamcorder_ErrorEventUserNotify(pMe);
    }
  }
  else
  {
    Qcamcorder_DisplayPostRecordingMessage(pMe);
  }
}
#endif
/*********************************************************************************
* Qcamcorder_ShowconfirmMenu
* This function will show the confirmation menu for video editing, cut merge
* ARGUMENTS:
* pMe: Qcamcorder structure
**********************************************************************************/
static void Qcamcorder_ShowconfirmMenu(Qcamcorder *pMe)
{
  pMe->m_bEditHit = FALSE;
  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
  IDISPLAY_Update(pMe->a.m_pIDisplay);

  if(pMe->m_playbackSize == QCAMCORDER_PLAYBACK_FULL_SCREEN)
  {
    if(VideoPlayback.id != 0)
    {
      QCAMCORDER_MSG_HIGH("Deregistering MDP %d", VideoPlayback.id, 0, 0);
      mdp_clear_ovimg(VideoPlayback.id);
      mdp_deregister_ovimg(VideoPlayback.id);
      VideoPlayback.id = 0;
    }
    pMe->m_playbackSize = QCAMCORDER_PLAYBACK_REGULAR;
    QCamcorder_MdpRegisterFullScreen(pMe, FALSE);
  }
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
  if(pMe->eState != QCAMCORDER_VIDEO_EDIT_DONE_ERROR &&
     pMe->eState != QCAMCORDER_VIDEO_EDIT_UNSUPPORTED
    )
  {
     Qcamcorder_ExecuteConfirmMenu(pMe);  
  }
  else
  {
    (void)ISHELL_Beep(pMe->a.m_pIShell, BEEP_ERROR, TRUE);
    /* Display bitmap to user regarding cut feature error */
    pMe->msg = (pMe->eState == QCAMCORDER_VIDEO_EDIT_DONE_ERROR) ?
               IDB_POSTEDIT_ERROR : IDB_POSTEDIT_UNSUPPORTED;
    Qcamcorder_ErrorEventUserNotify(pMe);
  }
#endif
}
#ifdef FEATURE_MP4_TRANSCODER
/*********************************************************************************
* Qcamcorder_TranscodePortionSelection
* This function is for selecting first and last video frame in partial transcoding
* ARGUMENTS:
* pMe: Qcamcorder structure
**********************************************************************************/
static boolean Qcamcorder_TranscodePortionSelection(Qcamcorder *pMe)
{
  uint32             videoTimeScale        = 0;

#ifndef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
  uint32             dCurrentVideoFrameNum = 0;
  uint64             target_video_time     = 0;
  AEEMovieSampleInfo vidInfo;
  AEEMovieFileInfo   mfi;
  IMovieFile         *pFile                = NULL;
  int16              videoStreamID         = -1;
  int16              i                     = 0;
  int32              res                   = 0;

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE,
                            (void **)&pFile) !=  SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VideoEditing cannot create infile",0,0,0);
    return FALSE;
  }
  mfi.cbSize =  sizeof(AEEMovieFileInfo);

  if (IMOVIEFILE_OpenMp4Read(pFile, pMe->sCurrentFileName, &mfi) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VIdeoEditing cannot open input file",0,0,0);
    (void)IMOVIEFILE_Release(pFile);
    pFile = NULL;
    return FALSE;
  }


  if(mfi.pStreams == NULL)
  {
    (void)IMOVIEFILE_Release(pFile);
    pFile = NULL;
    QCAMCORDER_MSG_HIGH("COULD NOT Transcoder file",0,0,0);
    return FALSE;
  }

  for (i = 0; i < mfi.nStreams; ++i)
  {
    if (mfi.pStreams[i].format == AEECLSID_MPEG4_VIDEO||
        mfi.pStreams[i].format == AEECLSID_H264_VIDEO
       )
    {
      videoStreamID = i;
      videoTimeScale = mfi.pStreams[i].nTimescale;
    }
  }

  /* Video edit only if the formats are supported */
  if(videoStreamID == -1 || videoTimeScale == 0)
  {
    QCAMCORDER_MSG_ERROR("Video Editing: MP4 file contains no recognized video/audio",0,0,0);
    (void)IMOVIEFILE_Release(pFile);
    pFile = NULL;
    return FALSE;
  }

  target_video_time = pMe->m_dStartPos * videoTimeScale - videoTimeScale/2;
  vidInfo.dwTime = 0;
  vidInfo.cbSize = mfi.cbSize;

  while(vidInfo.dwTime <= target_video_time &&
        res == SUCCESS  /* the previous read was successful */
       )
  {
    res = IMOVIEFILE_GetSampleInfo(pFile,videoStreamID,1,
                                    dCurrentVideoFrameNum++,&vidInfo);

  }

  pMe->nStartFrameNo = dCurrentVideoFrameNum - 1;

  target_video_time = pMe->m_dwPlayPos * videoTimeScale - videoTimeScale/2;

  while(vidInfo.dwTime <= target_video_time &&
        res == SUCCESS  /* the previous read was successful */
       )
  {
    res = IMOVIEFILE_GetSampleInfo(pFile,videoStreamID,1,
                                    dCurrentVideoFrameNum++,&vidInfo);

  }

  pMe->nEndFrameNo = dCurrentVideoFrameNum - 1;

  IMOVIEFILE_Close(pFile);
  if (pFile)
  {
    (void)IMOVIEFILE_Release(pFile);
     pFile = NULL;
  }

  return TRUE;
#else
  AEEMediaData src;
  AEEMovieEditSampleInfo pInfo;
  uint32 step =0;
  uint32 approx_frame_position = 0;
  uint32 frame_position[2] = {0,0};
  uint32 select_points[2];
  uint32 position_error = 0;
  uint32 prev_position_error = 0;
  uint32 target_video_time[2];
  uint32 i;

  /*Init source file info*/
  src.clsData = MMD_FILE_NAME;
  src.pData = pMe->sCurrentFileName;
  
  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IMOVIEEDIT,
                            (void **)&(pMe->m_pInEdit)) !=  SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("Failed to create IMovieEdit Instance",0,0,0);
    return FALSE;
  }

  //OPen the source File 
  if (IMOVIEEDIT_ReadOpen(pMe->m_pInEdit, &src,
                           &(pMe->m_mei), NULL,
                           NULL) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("Transcoding cannot open in file",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
    return FALSE;
  }

  select_points[0] = pMe->m_dStartPos;
  select_points[1] = pMe->m_dwPlayPos;
  videoTimeScale = pMe->m_mei.videoTimescale;

  /*WARNING: IMovieEdit returns time in 32 bit precision and when 
   video_time_resolution is MAX(65536) we can handle stream upto 
   18 hours duration only*/

  target_video_time[0] = select_points[0] * videoTimeScale - 
                          (videoTimeScale >> 1); /*Give a small - margin*/
  target_video_time[1] = select_points[1] * videoTimeScale + 
                          (videoTimeScale >> 1); /*Give a small + margin*/

  for(i = 0; i < 2; i++)/*For start and end frame..*/
  {
      /*Find an approximate frame number matching the time*/
      approx_frame_position = select_points[i] * pMe->m_mei.frame_rate;
      if(!approx_frame_position)
      {
         continue;
      }
      /*Boundary Check*/
      approx_frame_position = (approx_frame_position > pMe->m_mei.videoTotalFrame)?
                              pMe->m_mei.videoTotalFrame : approx_frame_position;

      /*Find our approximate is how far from target time. Using approximate frame to
      find frame saves calling this function for each frame starting from beginning*/
      if(IMOVIEEDIT_ReadSampleInfo(pMe->m_pInEdit,
                                   AEEMOVIEEDIT_STREAM_VIDEO, 
                                   approx_frame_position - 1,
                                   1, 
                                   &pInfo,
                                   NULL,
                                   NULL)!= SUCCESS)
      {
         IMOVIEEDIT_ReadClose(pMe->m_pInEdit,NULL,NULL);      
         QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit); 
         return FALSE; 
      }

      /*Step - increment or decrement to reach closer to target frame.
       Step will take 1 or 0xFFFFFFFF. This in unsigned arithmetic will 
       help in incrementing and decrementing approx_frame_position until
       approx_frame_position is less than 2^31 which is 100s of hours with 30 fps */
      step = (pInfo.dwTime > target_video_time[i]) ? -1 : 1;

      /*Find error in position. We will try to find the frame which will 
      give minimum position_error w.r.t start and end times. We use prev_position_error
       and position error to find the minima*/

      position_error = (int32)(pInfo.dwTime - target_video_time[i]) > 0 ?
                       (pInfo.dwTime - target_video_time[i]) :
                       (target_video_time[i] - pInfo.dwTime);
      prev_position_error = pInfo.dwTime;


      while(position_error &&  approx_frame_position &&
            (position_error < prev_position_error) &&
            (approx_frame_position < pMe->m_mei.videoTotalFrame))
      {
         approx_frame_position += step;
         if(IMOVIEEDIT_ReadSampleInfo(pMe->m_pInEdit,
                                   AEEMOVIEEDIT_STREAM_VIDEO, 
                                   approx_frame_position - 1,
                                   1, 
                                   &pInfo,
                                   NULL,
                                   NULL)!= SUCCESS)
         {
            IMOVIEEDIT_ReadClose(pMe->m_pInEdit,NULL,NULL);      
            QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);                
            return FALSE; 
         }

         prev_position_error = position_error;
         position_error = (int32)(pInfo.dwTime - target_video_time[i]) > 0 ?
                       (pInfo.dwTime - target_video_time[i]) :
                       (target_video_time[i] - pInfo.dwTime);

      }

      if(position_error && (approx_frame_position > 0) && 
         (approx_frame_position < pMe->m_mei.videoTotalFrame))
      {
         frame_position[i] = approx_frame_position - step;
         /*if position error is non zero, we have to take previous frame as 
         position error has already deviated from the minima*/
      }
      else
      {
         frame_position[i] = approx_frame_position;
      }

  }

  pMe->nStartFrameNo = frame_position[0];
  pMe->nEndFrameNo = frame_position[1];

  IMOVIEEDIT_ReadClose(pMe->m_pInEdit,NULL,NULL);
  QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);

  return TRUE;
#endif /*FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE*/
}
#endif   /*  FEATURE_MP4_TRANSCODER */

void *qcmd_malloc(uint32 uSize)
{
  return MALLOC(uSize);
}

void qcmd_free(void *ptr)
{
  FREE(ptr);
}

#ifdef FEATURE_BREW_IMOVIEFILE

#if (defined(FEATURE_QCAMCORDER_CUT_N_MERGE) || \
      defined(FEATURE_QCAMCORDER_VIDEO_EDITING) )

/* capture audio in memory for CutMerge and VideoEdit
*/

typedef enum {
  CUTNMERGE_START_SEG2_AUDIO_WITH_FRAMENUM,
  CUTNMERGE_START_SEG2_AUDIO_WITH_TIME
}  QCamcorder_seg2audio_data_type;

camcorderutil_llmo_type *Qcamcorder_Vedit_AudioCapture(
  IMovieFile *pInFile,
  QCamcorder_seg2audio_data_type start_type,
  int32 start_data,   /* meaning according to start_type*/
  int32 target_time,
  int16 audioStreamID,
  int32 *startFrameNum,
  int32 *endFrameNum,
  boolean *bError,
  CCD_MALLOC_FN mlc,
  CCD_FREE_FN fr,
  camcorderutil_llmo_type **paudio_data_read
  )
{
  camcorderutil_llmo_data_header_type aud_header;
  camcorderutil_llmo_type *audio_data_write;
  AEEMovieSampleInfo   audioInfo;
  boolean fStarted = FALSE;
  int32 dCurrentAudioFrameNum;
  uint32 nBytes;
  uint32 *tmp = NULL;
  uint32 sizeoftmp = 0;

  /* initialize the container */
  audio_data_write   = camcorderutil_llmo_constructor(3000,mlc,fr);
  *paudio_data_read  = audio_data_write;

  for(dCurrentAudioFrameNum = 0;
       !(*bError) &&
       SUCCESS == IMOVIEFILE_GetSampleInfo(pInFile,(uint16)audioStreamID,1,
                                           dCurrentAudioFrameNum,&audioInfo
                                         );
       dCurrentAudioFrameNum++
     )
  {
    if(target_time >= 0 &&
       audioInfo.dwTime > target_time)
    {
      break;
    }

    if( !fStarted )
    {
      if(start_type == CUTNMERGE_START_SEG2_AUDIO_WITH_TIME &&
         audioInfo.dwTime < start_data )
        continue;

      if(start_type == CUTNMERGE_START_SEG2_AUDIO_WITH_FRAMENUM &&
         dCurrentAudioFrameNum < start_data )
        continue;

      /* if we got here that mean the start criteria has been met */
      fStarted = TRUE;
      *startFrameNum = dCurrentAudioFrameNum;
    }

    if( audioInfo.dwBytes != sizeoftmp )
    {
      if( tmp )
      {
        fr(tmp);
        tmp = NULL;
      }
      sizeoftmp = audioInfo.dwBytes;
      tmp = (uint32 *) mlc(audioInfo.dwBytes);
      if(tmp == NULL )
      {
        *bError = TRUE;
      }
    }

    if( ! *bError )
    {
      if ( SUCCESS !=
         IMOVIEFILE_ReadSampleStream(pInFile,audioStreamID,
                               &nBytes,tmp,&audioInfo)
         )
        *bError = TRUE;
    }

    if(! *bError )
    {
      aud_header.param1 = (int32) audioInfo.dwTime;
      aud_header.param2 = audioInfo.bSync;
      aud_header.param3 = (int32) audioInfo.dwDelta;
      aud_header.param4 = (int32) audioInfo.dwSegment;
      if ( camcorderutil_llmo_write(&audio_data_write,(int32) nBytes,
                                    (uint8 *) tmp, &aud_header)
           )
      {
        QCAMCORDER_MSG_ERROR("error saving audio packets into memory",0,0,0);
        camcorderutil_llmo_destructor(audio_data_write);
        *bError = TRUE;
      }
    }

    /* we pause every 128 calls to allow slower processes to
    ** catch up
    */
    if(0 == (dCurrentAudioFrameNum & 0xFF))
      MSLEEP(10);
  }

  if(tmp)
    fr(tmp);

  *endFrameNum = dCurrentAudioFrameNum;

  return audio_data_write;
}

typedef struct
{
  int32   allocated_size;
  int32   remaining_size;
  uint8  *buffer;   /* video buffer allocated*/
  uint8  *ptr;      /* pointer to start of vidbuffer */
} QCAMCORDER_VIDEDIT_VIDEDIT;

#endif // if defined CUT_N_MERGE or VIDEO_EDITING
#endif // FEATURE_BREW_IMOVIEFILE
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE

#ifdef FEATURE_CAMCORDER_MALLOC
  camcorderutil_malloc_context_type *cutnmerge_malloc_context = NULL;
  #define cutnmerge_malloc(x)  \
     camcorderutil_malloc(cutnmerge_malloc_context, x, \
                            __func__,__LINE__)
  #define cutnmerge_free(x) \
    { static msg_const_type m; \
       camcorderutil_free(cutnmerge_malloc_context, \
                           __LINE__,__func__,&m,x); \
    }
  #define cutnmerge_realloc(x,y) \
      camcorderutil_realloc(cutnmerge_malloc_context,\
                           __LINE__,__func__,x,y)
#else
  #define cutnmerge_malloc(x)    MALLOC(x)
  #define cutnmerge_free(x)      FREE(x)
#endif

#ifdef FEATURE_CAMCORDER_MALLOC
  #define CUTMERGE_ERREXIT \
  { \
    static msg_const_type m;  \
    pMe->eState = QCAMCORDER_CUT_MERGE_DONE; \
    QCAMCORDER_IMEDIA_STOP(pMe->pMedia); \
    camcorderutil_malloc_terminate(cutnmerge_malloc_context, \
                                   __LINE__,__func__,&m);  \
    cutnmerge_malloc_context = NULL; \
  }
#else
  #define CUTMERGE_ERREXIT { \
     pMe->eState = QCAMCORDER_CUT_MERGE_DONE; \
     QCAMCORDER_IMEDIA_STOP(pMe->pMedia);     \
  }
#endif

/* malloc & free callback functions functions */

void *cutandmerge_malloc_cb(uint32 uSize)
{
  return cutnmerge_malloc(uSize);
}

void cutandmerge_free_cb(void *ptr)
{
  cutnmerge_free(ptr);
}

#ifdef FEATURE_BREW_IMOVIEEDIT
boolean Qcamcorder_CutMerge(Qcamcorder * pMe)
{
  char *pData;
  AEEMediaData src;

  pData = (void *)Qcamcorder_GetVideoEditFileName(pMe);
  (void)STRLCPY(pMe->sVideoEditFileName,  pData, sizeof(pMe->sVideoEditFileName));

  pMe->m_nEditPortion = 2;
  pMe->m_cutInfo = MALLOC(sizeof(AEEMovieEditCutInfo)
    *pMe->m_nEditPortion);
  pMe->m_cutInfo[0].dwStartTime = 0;
  pMe->m_cutInfo[0].dwEndTime = (pMe->m_dFirstMarker*1000);
  pMe->m_cutInfo[1].dwStartTime = (pMe->m_dSecondMarker*1000);
  pMe->m_cutInfo[1].dwEndTime = pMe->m_dwTotalTime;

  if(pMe->m_cutInfo[1].dwStartTime > pMe->m_dwTotalTime)
  {
     pMe->m_cutInfo[1].dwStartTime = pMe->m_dwTotalTime;
  }

  if (!pMe->pIFileMgr) {
    (void)ISHELL_CreateInstance(
      pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pMe->pIFileMgr);
  }
  if (IFILEMGR_Test(pMe->pIFileMgr, pMe->sVideoEditFileName) == SUCCESS) {
    if (IFILEMGR_Remove(pMe->pIFileMgr, pMe->sVideoEditFileName) != SUCCESS) {
      QCAMCORDER_MSG_HIGH("COULD NOT DELETE THE VIDEO EDITING FILE",0,0,0);
      return FALSE;
    }
  }
  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IMOVIEEDIT,
                            (void **)&(pMe->m_pInEdit)) !=  SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VideoEditing cannot create infile",0,0,0);
    return FALSE;
  }

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IMOVIEEDIT,
                            (void **)&(pMe->m_pOutEdit)) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VideoEditing cannot create outfile",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
    return FALSE;
  }

  src.clsData = MMD_FILE_NAME;
  src.pData = pMe->sCurrentFileName;
  CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_CutReadOpenCB, pMe);
  if (IMOVIEEDIT_ReadOpen(pMe->m_pInEdit, &src,
                           &(pMe->m_mei), &(pMe->m_movieeditStatus),
                           &(pMe->m_movieeditCB)) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VIdeoEditing cannot open input file",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
    return FALSE;
  }
  return TRUE;
}

#elif defined(FEATURE_BREW_IMOVIEFILE)

/* ==========================================================================
FUNCTION QCAMCORDER_CutMerge
DESCRIPTION Takes the current video and changes the audio to the audio
that was just recorded and saves to another mp4 file.
LIMITATIONS: audio is stored in memory. Therefore this will no work on video
             clips that are humongous.
============================================================================= */
//lint -esym(644,videoInfo)  Suppress complaint about not initialized videoInfo
boolean Qcamcorder_CutMerge(Qcamcorder * pMe)
{
   uint32 dCurrentAudioFrameNum = 0, dCurrentVideoFrameNum = 0;
   uint32 dCurrentAudioFrameTime = 0, dCurrentVideoFrameTime = 0;
   uint32 segment1_lastFrameNum = 0;
   uint32 segment1_lastFrameTime = 0;
   uint32 segment1_lastFrameTimeAudio = 0;
   uint32 audioHeaderSize=0, videoHeaderSize=0;
   AEEMovieFileInfo mfi;
   byte *pVideoHeaderContents = NULL;
   byte *pAudioHeaderContents = NULL;
   boolean videoHeaderWritten = FALSE, audioHeaderWritten = FALSE;
   AEEMovieSampleInfo videoInfo, audioInfo;
   uint32 curoffset=0;
   IMovieFile *pInFile = NULL,  * pOutFile = NULL;
   AEEMediaData md;
   uint32 const INITIAL_HEADER_SIZE = 30;
   int16 videoStreamID = -1, audioStreamID = -1, textStreamID = -1;
   M4V2RGB_handle *m4v2rgb_handle = NULL;
   camcorderutil_llmo_data_header_type aud_header;
   boolean  bFooterWritten=FALSE;  /* TRUE when End-of-Stream written*/
   /* ping pong buffer for video format write */
   QCAMCORDER_VIDEDIT_VIDEDIT pFrameContents[2] = {0};
   AEEMovieSampleInfo vidInfoPP[2] = {0}; /* this also needs pingponged */
   uint16 ppIdx = 0;                      /* ping pong index for above */
   uint16 prevIdx = 0;                    /* previous ping-pong index */

   /* for converting video media time to audio media time */
   double audio_time_conversion_factor = 0.0;

   /* for converting video ES time to video media time */
   double video_time_scale_conversion_factor = 0.0;

   /* for convertnig video media time to video ES time */
   double video_mediatime_conversion_factor = 0.0;

   int32  target_video_time = 0;
   int32 latestSyncFrameNum = 0, latestAudioFrameNUM = 0;
   uint32 latestSyncTime = 0;
   int32 lastAudioFrameNUM, startAudioFrameNUM;
   uint8 *pAudioFrameContents = 0;
   uint32 videoTimeScale = 0, audioTimeScale = 0, textTimeScale=0;
   int i = 0;
   byte *tmp = NULL;
   uint32 nBytes = 0;
   boolean bShortHeader = FALSE;
   int cycle = 0;
   boolean bError = FALSE;
   int32 nBytes_Audio = 0;
   /* output of m4v2rgb_test_CodedVOP */
   uint32 delta_time_video = 0, delta_time_audio = 0;

   int32 override_delta;   /* override delta computation for m4v2rgb subrs */
   uint32 seg2StartSyncTime = 0; /* starting time for segment 2 */

   /* in the inner look this will contain the new frame being read */
   m4v2rgb_test_codedVOP_output_type testCodedVopRes = {0};
   /* this is the previous frame, which will be written*/
   m4v2rgb_test_codedVOP_output_type testCodedVopResPrev = {0};
   /* we need to store one frame earlier because this where we will
   ** test_codedVOP() from a sync.
   */
   m4v2rgb_test_codedVOP_output_type testCodedVopResPrevPrev;

   /* sync point will be cached here */
   m4v2rgb_test_codedVOP_output_type testCodedVopSync;  /* result at sync frame */

   /* this output pairs correspond to the testCodedVopRes-pair and is used
   ** in cycle 2 to enforce a delta time
   */
   m4v2rgb_test_codedVOP_output_type outputVopRes = {0},outputVopResPrev={0};
   /* this is to cache the end of first segment to begin second segment */
   m4v2rgb_test_codedVOP_output_type secondSegmentStartTime;
   uint8 **newStream = NULL;
   uint16 newNumBytes = 0;
   int32  res = 0;

   /* if visual_object_sequence_start_code exists at the header. If TRUE
   ** then we need to write the footer (visual_object_sequence_end_code)
   */
   boolean bStreamHeaderExists = FALSE;


   M4V2RGB_return_type testCodedVOP_res;

   /* audio packets stored temporarily in memory
   */

   camcorderutil_llmo_type *audio_data_write = NULL;
   camcorderutil_llmo_type *audio_data_read  = NULL;


#ifdef FEATURE_CAMCORDER_MALLOC
  if (!cutnmerge_malloc_context)
  {
    static msg_const_type m;
    cutnmerge_malloc_context = camcorderutil_malloc_init(__func__,__LINE__,&m);
  }
#endif


   pMe->eState = QCAMCORDER_RECORDING;
   md.clsData = MMD_FILE_NAME;
   md.pData = (void *)Qcamcorder_GetVideoEditFileName(pMe);
   (void)STRLCPY(pMe->sVideoEditFileName,  md.pData,
                  sizeof(pMe->sVideoEditFileName));
   md.dwSize = 0;

   (void)ICAMERA_SetVideoEncode(pMe->m_pICam, AEECLSID_MEDIAMPEG4, 0);
   (void)ICAMERA_SetMediaData(pMe->m_pICam, &md, pMe->pFileFormat);

   if(!pMe->pIFileMgr)
   {
     (void)ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pMe->pIFileMgr);
   }
   if(IFILEMGR_Test(pMe->pIFileMgr, pMe->sVideoEditFileName) == SUCCESS)
   {
     if(IFILEMGR_Remove(pMe->pIFileMgr, pMe->sVideoEditFileName) != SUCCESS)
     {
       QCAMCORDER_MSG_HIGH("COULD NOT DELETE THE VIDEO EDITING FILE",0,0,0);
#ifdef FEATURE_CAMCORDER_MALLOC
       {
         static msg_const_type m;
         camcorderutil_malloc_terminate(cutnmerge_malloc_context,
                                        __LINE__,__func__,&m);
         cutnmerge_malloc_context = NULL;
       }
#endif
       return FALSE;
     }
   }

   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE,
                             (void **)&pInFile) != SUCCESS)
   {
     QCAMCORDER_MSG_ERROR("CutAndMerge cannot create infile",0,0,0);
     CUTMERGE_ERREXIT;
     return FALSE;
   }
   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE,
                             (void **)&pOutFile) != SUCCESS)
   {
     QCAMCORDER_MSG_ERROR("CuatAndMerge cannot creat outfile",0,0,0);
     CUTMERGE_ERREXIT
     return FALSE;
   }

   mfi.cbSize =  sizeof(AEEMovieFileInfo);
   if (IMOVIEFILE_OpenMp4Read(pInFile, pMe->sCurrentFileName, &mfi) != SUCCESS)
   {
     QCAMCORDER_MSG_ERROR("CutAndMerge cannot open input file",0,0,0);
     (void)IMOVIEFILE_Release(pInFile);
     (void)IMOVIEFILE_Release(pOutFile);
     QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
     pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
     pInFile = NULL;
     pOutFile = NULL;
     return FALSE;
   }
   mfi.nBrand = Qcamcorder_GetFileBrand(pMe);
   if (IMOVIEFILE_OpenMp4Write(pOutFile, md.pData, &mfi) != SUCCESS)
   {
     QCAMCORDER_MSG_ERROR("CutAndMerge cannot open output file",0,0,0);
     (void)IMOVIEFILE_Release(pInFile);
     (void)IMOVIEFILE_Release(pOutFile);
     QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
     pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
     pInFile = NULL;
     pOutFile = NULL;
     return FALSE;
   }

   /* find the video and audio stream ID in the MP4 file*/
   videoStreamID = -1;
   audioStreamID = -1;
   for (i = 0; i < mfi.nStreams; ++i)
   {
     if (mfi.pStreams[i].format == AEECLSID_MPEG4_VIDEO)
     {
       videoStreamID = (int16)i;
       videoTimeScale = mfi.pStreams[i].nTimescale;
     }
     else if (mfi.pStreams[i].format == AEECLSID_MEDIAQCP )
     {
       audioStreamID = (int16)i;
       audioTimeScale = mfi.pStreams[i].nTimescale;
     }
     else if (mfi.pStreams[i].format == AEECLSID_3GPP_TEXT )
     {
         textStreamID = (int16)i;
         textTimeScale = mfi.pStreams[i].nTimescale;
     }
   }
   if(videoStreamID == -1 )
   {
     QCAMCORDER_MSG_ERROR("CutAndMerge: MP4 file contains no recognized video",0,0,0);
     (void)IMOVIEFILE_Release(pInFile);
     (void)IMOVIEFILE_Release(pOutFile);
     pInFile = NULL;
     pOutFile = NULL;
     if(IFILEMGR_Test(pMe->pIFileMgr,pMe->sVideoEditFileName) == SUCCESS)
     {
       (void)IFILEMGR_Remove(pMe->pIFileMgr, pMe->sVideoEditFileName);
     }
     CUTMERGE_ERREXIT
     pMe->eState =  QCAMCORDER_VIDEO_EDIT_UNSUPPORTED; /* To Display unsupported format message*/
     return FALSE;
   }

   if (!videoTimeScale )
   {
     QCAMCORDER_MSG_ERROR("Error in CutMerge. MovieTimeScale == 0",0,0,0);
     (void)IMOVIEFILE_Release(pInFile);
     (void)IMOVIEFILE_Release(pOutFile);
     pInFile = NULL;
     pOutFile = NULL;
     CUTMERGE_ERREXIT
     return FALSE;
   }

   videoInfo.cbSize = sizeof(videoInfo);
   audioInfo.cbSize = sizeof(audioInfo);

   /* TODO: copy over user atoms */

   if(audioStreamID >= 0 )
   {
     audio_time_conversion_factor =
       ((double) audioTimeScale)/((double)videoTimeScale);
   }


   /* Get the video header */
   /* 08/24/05  williamh
   ** This code, to get the video header, is broken. Even though it is
   ** identical to the video header section in Qcamcorder_VideoEditing.
   ** Possible causes: changes in Qcamcorder UI and or recent changes
   ** in OEMMediaMPEG42PV.cpp?
   **
   ** Eventually we need to find the root cause. But for now we do a
   ** work around. Because IMovieFile returns the header with the first
   ** frame we don't care about the header in encoding. However we do
   ** need the first frame to initialize M4V2RGB.
   */
#ifdef QCAMCORDER_CUTMERGE_VIDEOTRACKHEADER_WORKAROUND_DEBUG
#error code not present
#else  // #if QCAMCORDER_CUTMERGE_VIDEOTRACKHEADER_WORKAROUND_DEBUG
   pVideoHeaderContents = (byte *)
        IMOVIEFILE_GetSampleStream(pInFile,(uint16)videoStreamID,
                                         1,0,&nBytes,&videoInfo);

   if(pVideoHeaderContents == NULL )
   {
     QCAMCORDER_MSG_ERROR("header frame read error",0,0,0);
     bError = TRUE;
   }
   else
   {
     /* We used to detect short head by OEMMediaPMEG42PV_GetVideoTrackHeader()
     ** not returning any video header. Now we have to really detect it.
     */

     if(  pVideoHeaderContents[0]          == 0x00 &&
          pVideoHeaderContents[1]          == 0x00 &&
        ((pVideoHeaderContents[2] & 0xF0)  == 0x80)
       )
     {
       /* the actual setting of the bShortHeader flag happens later */
       FREE(pVideoHeaderContents);
       pVideoHeaderContents = 0;
     }
     else
     {
       /* IMovieFile allocates pVideoHeaderContents using MALLOC(),
       ** here we use cutnmerge_malloc().  So make the switch before
       ** somebody gets confused
       */
       videoHeaderSize = videoInfo.dwBytes;
       tmp = cutnmerge_malloc(videoHeaderSize);
       if(tmp == NULL )
       {
         QCAMCORDER_MSG_ERROR("cut_n_merge malloc error",0,0,0);
         bError = TRUE;
         videoHeaderSize = 0;
       }
       else
       {
         memcpy(tmp,pVideoHeaderContents,videoHeaderSize);
         FREE(pVideoHeaderContents);
         pVideoHeaderContents = tmp;
         tmp = NULL;
       }
     }
   }

#endif  // #ifndef QCAMCORDER_CUTMERGE_VIDEOTRACKHEADER_WORKAROUND_DEBUG

   /* get the audio header */
   if (!bError && audioStreamID >= 0 )
   {
     pAudioHeaderContents = (byte *) IMOVIEFILE_GetSampleStream(pInFile,(uint16)audioStreamID,
                                                                1,0,&nBytes,&audioInfo);

     audioHeaderSize = INITIAL_HEADER_SIZE;
     tmp = (byte*) cutnmerge_malloc(audioHeaderSize * sizeof(byte));
     if (OEMMediaMPEG42PV_GetAudioTrackHeader(tmp, &audioHeaderSize) == FALSE)
     {
       cutnmerge_free(tmp);
       tmp = NULL;
       if (audioHeaderSize)
       {
         tmp = (byte*) cutnmerge_malloc(audioHeaderSize);
         (void)OEMMediaMPEG42PV_GetAudioTrackHeader(tmp, &audioHeaderSize);
       }
     }
     ASSERT(audioHeaderSize <= audioInfo.dwBytes);
     if(tmp)
       cutnmerge_free(tmp);
   }

   pMe->original_timestamp1 = -1;

   if(!bError)
   {

    /* initialize the m4v2rgb engine because it has a parser which can be
    ** used to determine if a VOP is I- or P-
    */

    int16 errorSubCode;
    uint16 width,height;
    M4V2RGB_return_type errorCode;

    if(videoHeaderSize == 0 )
    {
      /* this will be the case if its a short header or h.263
      */
      bShortHeader    = TRUE;

      /* for short header, the first frame will act as the header */
      if(SUCCESS !=
         IMOVIEFILE_GetSampleInfo(pInFile,videoStreamID,1,0,&videoInfo)
         )
      {
        bError = TRUE;
      }
      else
      {
        pVideoHeaderContents = (byte *) cutnmerge_malloc(videoInfo.dwBytes);
        if(pVideoHeaderContents == NULL )
          bError = TRUE;
        else if(SUCCESS != IMOVIEFILE_ReadSampleStream(pInFile,videoStreamID,
                                   &videoHeaderSize,(uint32 *) pVideoHeaderContents,
                                   &videoInfo)
                )
                 bError = TRUE;
      }
    }
    else
      bShortHeader = FALSE;

    if(!bError)
    {
      m4v2rgb_handle = m4v2rgb_init(pVideoHeaderContents,videoHeaderSize,&width, &height,
                                    NULL,&errorCode,&errorSubCode,cutandmerge_malloc_cb,
                                    cutandmerge_free_cb,NULL);

      m4v2rgb_test_codedVOP(pVideoHeaderContents,videoHeaderSize,&testCodedVopResPrev,
                            &testCodedVopRes,-1,m4v2rgb_handle);

      video_time_scale_conversion_factor =
        ((double) videoTimeScale) /
        ((double) testCodedVopRes.time_increment_resolution);

      video_mediatime_conversion_factor =
        ((double) testCodedVopRes.time_increment_resolution) /
        ((double) videoTimeScale);

      bStreamHeaderExists = testCodedVopRes.bHasSequenceStartCode;

      MEMSET(&testCodedVopRes,0,sizeof(m4v2rgb_test_codedVOP_output_type));

      if(bShortHeader)
      {
        videoHeaderSize = 0;  /* prevent this from being copied as the header */
        cutnmerge_free(pVideoHeaderContents);
        pVideoHeaderContents = NULL;
      }
    }
  }

  /* start off with delta_time = 0. During cycle 2 this will be set to the actual delta
  ** of the gap that we cut off during cycle 2.
  */
  delta_time_video = delta_time_audio = 0;

  /* Read all the video packets up to m_dFirstMarker and write it out
  ** along with the header and the audio
  */

  latestSyncFrameNum         = -1;
  latestAudioFrameNUM        =  -1;


  /* The current characteristic of IMovieFile is that the header is always
  ** included in the first frame.  If this changes in the future then we do
  ** not want to start audioHeaderWritten in TRUE state.
  */
  audioHeaderWritten = TRUE;


  /* cycle 0:  copy first segment from input to output
  ** cycle 1:  skip to the next Sync Frame
  ** cycle 2:  copy second segment
  */

  newStream = (uint8 **) cutnmerge_malloc(sizeof(uint8 *));
  *newStream = NULL;

  for(cycle=0; !bError && cycle<3; cycle++)
  {
    struct {
      enum whichone{
        CUTNMERGE_START_SEG2_AUDIO_WITH_FRAMENUM,
        CUTMERGE_START_SEG2_AUDIO_WITH_TIME
      } whichone;
      uint32       data;
    } seg2audio;

    /* Read to memory all audio packtes up to the m_dFirstMarker Marker */
    if(cycle != 1 && audioStreamID >= 0 )
    {
      if (cycle == 0 )
      {
        /* target audio time is added audioTimeScale/2. Since m_dFristMarker
        ** is rounded up to the nearest second, we subtract 1/2 second
        ** because the user might have pressed it a bit earlier.
        */
        int32 targetAudioOffset = audioTimeScale * pMe->m_dFirstMarker
                                  - audioTimeScale/2;

        audio_data_write =
          Qcamcorder_Vedit_AudioCapture(
            pInFile,
            CUTNMERGE_START_SEG2_AUDIO_WITH_FRAMENUM,
            0,     /* startime = 0 */
            targetAudioOffset,
            audioStreamID,&startAudioFrameNUM, &lastAudioFrameNUM,
            &bError,cutandmerge_malloc_cb, cutandmerge_free_cb,
            &audio_data_read);
      }
      else if ( cycle == 2 )
      {
        /* rewind the file since IMovieFile cannot seek backwords */
        IMOVIEFILE_Close(pInFile);
        IMOVIEFILE_OpenMp4Read(pInFile, pMe->sCurrentFileName, &mfi);

        audio_data_write =
        Qcamcorder_Vedit_AudioCapture(pInFile,
          seg2audio.whichone,
          seg2audio.data,
          0x7FFFFFFF,         /* targettime ... to the end */
          audioStreamID,&startAudioFrameNUM, &lastAudioFrameNUM,
          &bError,cutandmerge_malloc_cb,cutandmerge_free_cb,&audio_data_read);
      }

      dCurrentAudioFrameNum = startAudioFrameNUM;

      /* rewind the file since IMovieFile cannot seek backwords */
      IMOVIEFILE_Close(pInFile);
      IMOVIEFILE_OpenMp4Read(pInFile, pMe->sCurrentFileName, &mfi);

      /* prepare pAudioFrameContents for the audio processing part
      ** of the inner loop
      */
      pAudioFrameContents =  camcorderutil_llmo_read(
                                  &audio_data_read,
                                  &nBytes_Audio,
                                  &aud_header
                                );
    }

    if(!bError && cycle == 2 )
    { /* fast forward video to latestSyncFrameNum */

      for(dCurrentVideoFrameNum = 0;
          dCurrentVideoFrameNum < latestSyncFrameNum;
          dCurrentVideoFrameNum++ )
      {

        res = IMOVIEFILE_GetSampleInfo(pInFile,videoStreamID,1,
                                 dCurrentVideoFrameNum,&videoInfo
                                 );
        if (res != SUCCESS)
        {
          bError = TRUE;
          break;
        }

      }
      outputVopRes = secondSegmentStartTime;
    }

    if (!bError && cycle != 1 )
    {
      /* Initialize the ping-pong buffer for storing video frames.
      */
      for(ppIdx=0; ppIdx<2; ppIdx++)
      {
        if(pFrameContents[ppIdx].allocated_size )
        {
          cutnmerge_free(pFrameContents[ppIdx].buffer);
          pFrameContents[ppIdx].ptr =
          pFrameContents[ppIdx].buffer = NULL;
          pFrameContents[ppIdx].remaining_size =
          pFrameContents[ppIdx].allocated_size = 0;
        }
      }
      vidInfoPP[0].cbSize = vidInfoPP[1].cbSize = sizeof(AEEMovieSampleInfo);
      ppIdx = 0;

      /* get the first frame */
      if (SUCCESS != IMOVIEFILE_GetSampleInfo(pInFile,videoStreamID,1,
                           dCurrentVideoFrameNum++,&vidInfoPP[ppIdx])
         )
      {
        QCAMCORDER_MSG_ERROR("video frame read error",0,0,0);
        bError = TRUE;
        break;
      }

      pFrameContents[ppIdx].allocated_size = vidInfoPP[ppIdx].dwBytes
                                             + videoHeaderSize;
      pFrameContents[ppIdx].buffer =
        cutnmerge_malloc(pFrameContents[ppIdx].allocated_size);

      if(pFrameContents[ppIdx].buffer == NULL )
      {
        QCAMCORDER_MSG_ERROR("empty read",0,0,0);
        bError = TRUE;
        break;
      }

      pFrameContents[ppIdx].ptr =
        &pFrameContents[ppIdx].buffer[videoHeaderSize];
      pFrameContents[ppIdx].remaining_size = vidInfoPP[ppIdx].dwBytes;

      res = IMOVIEFILE_ReadSampleStream(pInFile,videoStreamID,&nBytes,
                                        (uint32 *)pFrameContents[ppIdx].ptr,
                                        &vidInfoPP[ppIdx]);

      if(res != SUCCESS )
      {
        QCAMCORDER_MSG_ERROR("video stream read error",0,0,0);
        bError = TRUE;
        break;
      }

      testCodedVopResPrevPrev = testCodedVopResPrev;
      testCodedVopResPrev     = testCodedVopRes;
      outputVopResPrev        = outputVopRes;

      override_delta =
        video_mediatime_conversion_factor *
            (vidInfoPP[ppIdx].dwTime -
               (video_time_scale_conversion_factor *testCodedVopResPrev.time)
            );

      testCodedVOP_res =
        m4v2rgb_VOPtime_offset(pFrameContents[ppIdx].ptr,
              (uint16)vidInfoPP[ppIdx].dwBytes,
              &testCodedVopResPrev,&testCodedVopRes,
              &outputVopResPrev,&outputVopRes,override_delta,
              newStream,&newNumBytes,m4v2rgb_handle);

      if(testCodedVOP_res != M4V2RGB_NoError )
      {
        QCAMCORDER_MSG_ERROR("video stream read error",0,0,0);
        bError = TRUE;
        break;
      }

      if(newNumBytes )
      {
        cutnmerge_free(pFrameContents[ppIdx].buffer);
        pFrameContents[ppIdx].buffer = pFrameContents[ppIdx].ptr =
            *newStream;
        *newStream = NULL;
        pFrameContents[ppIdx].allocated_size = newNumBytes;
        pFrameContents[ppIdx].remaining_size = newNumBytes;

      }

      /* fill in the pong buffer */
      ppIdx     = 1;
      prevIdx   = 0;

      /* initiate the target_video_time
      */
      if(cycle == 0 )
      {
        /* shave 500 msec off the video time because FirstMarker is
        ** rounded up to the nearest second.
        */
        target_video_time = pMe->m_dFirstMarker * videoTimeScale -
                         videoTimeScale/2;
      }
      else
      {
        /* go to the end fo the file ... specifi max target time
        */
        target_video_time = 0x7FFFFFFF;
      }
    }
    else if ( !bError && cycle == 1)
    {
      /* now we need to find a sync frame in front of m_dSecondMarker
      ** that's closest to it
      */
      target_video_time = pMe->m_dSecondMarker * videoTimeScale
               - videoTimeScale/2;
    }

    while(!bError &&
          vidInfoPP[prevIdx].dwTime <= target_video_time &&
          res == SUCCESS  /* the previous read was successful */
         )
    {

      res =
        IMOVIEFILE_GetSampleInfo(pInFile,videoStreamID,1,
                  dCurrentVideoFrameNum,&vidInfoPP[ppIdx]);

      testCodedVopResPrevPrev = testCodedVopResPrev;
      testCodedVopResPrev     = testCodedVopRes;
      outputVopResPrev        = outputVopRes;

      /* If this fails, we will assume its because the prev-frame was
      ** the last frame of the video stream. (The exact type of failure
      ** is hidden by IMOVIEFILE_GetSampleInfo().)  In this case allow
      ** the code to finish processing the prev-frame as the last frame
      ** and allow it to exit out during the next frame.
      */

      if(SUCCESS == res)
      {
        /* make sure we have enough memeory for the new frame */
        if(pFrameContents[ppIdx].remaining_size < vidInfoPP[ppIdx].dwBytes )
        {
          if(pFrameContents[ppIdx].allocated_size )
            cutnmerge_free(pFrameContents[ppIdx].buffer);
          pFrameContents[ppIdx].remaining_size =
          pFrameContents[ppIdx].allocated_size = vidInfoPP[ppIdx].dwBytes;
          pFrameContents[ppIdx].ptr =
          pFrameContents[ppIdx].buffer = cutnmerge_malloc(vidInfoPP[ppIdx].dwBytes);

          if(pFrameContents[ppIdx].buffer == NULL )
          {
            QCAMCORDER_MSG_ERROR("malloc error",0,0,0);
            bError = TRUE;
            break;
          }
        }

        /* read in the new frame */
        res = IMOVIEFILE_ReadSampleStream(pInFile,videoStreamID,&nBytes,
                (uint32 *) pFrameContents[ppIdx].ptr,&vidInfoPP[ppIdx]);
        if(SUCCESS != res )
        {
          bError = TRUE;
          QCAMCORDER_MSG_ERROR("Read the info but failed to read contents",0,0,0);
          break;
        }

        override_delta =
          video_mediatime_conversion_factor *
              (vidInfoPP[prevIdx].dwTime -
                 (video_time_scale_conversion_factor *testCodedVopResPrev.time)
              );

        /* because of fragments, we have to compute time, coded */
        testCodedVOP_res =
          m4v2rgb_VOPtime_offset(pFrameContents[ppIdx].ptr,
                (uint16)vidInfoPP[ppIdx].dwBytes,
                &testCodedVopResPrev,&testCodedVopRes,
                &outputVopResPrev,&outputVopRes,override_delta,
                newStream,&newNumBytes,m4v2rgb_handle);

        if(testCodedVOP_res != M4V2RGB_NoError )
        {
          bError = TRUE;
          continue;
        }

        if(newNumBytes > 0 )
        {
          cutnmerge_free(pFrameContents[ppIdx].buffer);
          pFrameContents[ppIdx].buffer = *newStream;
          pFrameContents[ppIdx].ptr    = *newStream;
          pFrameContents[ppIdx].allocated_size = newNumBytes;
          pFrameContents[ppIdx].remaining_size = newNumBytes;
        }

      }  /* if SUCCESS == res */

      if(1 == dCurrentVideoFrameNum )
      {
        /* since we're using the IMovieFile API, the first frame
        ** contains the header
        */
        videoHeaderWritten = TRUE;
      }

      if (!videoHeaderWritten && pVideoHeaderContents) {

        /* this should never be called because of the lines above
        ** that set videoHeaderWritten to TRUE.
        ** As long as IMovieFile pulls in the header along with the first
        ** frame we will never call this.
        */

        //pMe->nVopTimeIncRes =
        //  (uint32)IMOVIEFILE_GetTimeIncRes(pOutFile,
        //                 pVideoHeaderContents, (int)videoHeaderSize);

        if(vidInfoPP[prevIdx].bSync )
        {
          latestSyncFrameNum = dCurrentVideoFrameNum - 1;
          latestSyncTime     = vidInfoPP[prevIdx].dwTime;
          testCodedVopSync   = testCodedVopResPrev;
          ASSERT(dCurrentVideoFrameNum == 1 );
        }
        else
          QCAMCORDER_MSG_ERROR("Warning: first frame not sync?",0,0,0);

        /* We're required to attach the header info to the first
        ** video frame. The only way to do that is via memcpy.
        */
        if ( (pFrameContents[prevIdx].allocated_size -
              pFrameContents[prevIdx].remaining_size) == videoHeaderSize
           )
        {
            /* williamh:  The first frame seem to always contain the
            ** header.
            */
          memcpy(pFrameContents[prevIdx].buffer,pVideoHeaderContents,
                 videoHeaderSize);
          pFrameContents[prevIdx].ptr = pFrameContents[prevIdx].buffer;
          pFrameContents[prevIdx].remaining_size += videoHeaderSize;
        }
        else
        {
          /* we really should never be here */
          ASSERT(0);
        }

        vidInfoPP[prevIdx].dwBytes += videoHeaderSize;

        videoHeaderWritten = TRUE;
        cutnmerge_free(pVideoHeaderContents);
        pVideoHeaderContents = NULL;

      }
      else
      {
        /* don't trust IMovieFile to detect bSync
        **
        ** if (vidInfoPP[prevIdx].bSync == FALSE ) {
        */

        if (testCodedVopResPrev.bCoded &&       /* it is coded ... and */
            testCodedVopResPrev.codingType == 0 /* its a sync frame    */ )
        {
          vidInfoPP[prevIdx].bSync = TRUE;
          latestSyncFrameNum       = dCurrentVideoFrameNum - 1;
          latestSyncTime           = vidInfoPP[prevIdx].dwTime;
          testCodedVopSync         = testCodedVopResPrevPrev;
        }
      }

      if(cycle != 1 )
      {
        uint32 dwTime_cache;  /* cache the video time in MPeg4 video time scale */

        if(testCodedVopResPrev.bHasSequenceEndCode)
        { bFooterWritten = TRUE;
          if ( !bStreamHeaderExists )
          {
            QCAMCORDER_MSG_ERROR("Waring: footer written but there is no header",0,0,0);
          }
        }

        /* TODO:  Can we merge this w/ Qtv's definition of M4VFOOTER? */
        if(SUCCESS != res      /* last frame to write */
           && !bFooterWritten  /* footer ntoe written yet */
           && bStreamHeaderExists /* we ahve a header */
          )
        { /* this conditions indicate we're processing the last frame */
          if(pFrameContents[prevIdx].remaining_size <
              vidInfoPP[prevIdx].dwBytes + sizeof(QCAMCORDER_M4VFOOTER))
          {
            uint32 newsize;
            uint8 *newbuf;
            newsize = vidInfoPP[prevIdx].dwBytes + sizeof(QCAMCORDER_M4VFOOTER);
            newbuf = cutnmerge_malloc(newsize);
            if(newbuf == NULL)
            {
              QCAMCORDER_MSG_ERROR("malloc error",0,0,0);
              bError = TRUE;
              break;
            }
            memcpy(newbuf,pFrameContents[prevIdx].ptr,
                   vidInfoPP[prevIdx].dwBytes);
            cutnmerge_free(pFrameContents[prevIdx].buffer);
            pFrameContents[prevIdx].buffer =
              pFrameContents[prevIdx].ptr = newbuf;
            pFrameContents[prevIdx].allocated_size =
              pFrameContents[prevIdx].remaining_size = newsize;

          }
          memcpy(&pFrameContents[prevIdx].ptr[vidInfoPP[prevIdx].dwBytes],
                 QCAMCORDER_M4VFOOTER,sizeof(QCAMCORDER_M4VFOOTER));
          vidInfoPP[prevIdx].dwBytes += sizeof(QCAMCORDER_M4VFOOTER);

        }

        dwTime_cache               = vidInfoPP[prevIdx].dwTime;

        vidInfoPP[prevIdx].dwDelta = 0.5 +
          (outputVopRes.timeDelta * video_time_scale_conversion_factor);
        vidInfoPP[prevIdx].dwTime  = 0.5 +
          ( outputVopResPrev.time * video_time_scale_conversion_factor);
        (void)IMOVIEFILE_WriteStream(pOutFile, (uint16)videoStreamID, 1,
                             &vidInfoPP[prevIdx], pFrameContents[prevIdx].ptr);

        /* we need to restore this for the audio to work right */
        vidInfoPP[prevIdx].dwTime = dwTime_cache;
      }

      /* give lower priority processes a chance
      **
      ** ToDo:  This will go away with cleanup effort. All computation codes
      **        move to OEMMovieFile and execute in GRAPHICS task
      */
      if(0 == (dCurrentVideoFrameNum & 0xFF))
        MSLEEP(10);

      dCurrentVideoFrameNum++;
      dCurrentVideoFrameTime = vidInfoPP[prevIdx].dwTime;


      /* output the audio frames corresponding to the video frame that was
      ** just written
      */
      if (cycle != 1 && !bError && audioStreamID >= 0 )
      {
        /* convert the video time offset to audio time offset
        */
        curoffset = (uint32)(vidInfoPP[prevIdx].dwTime *
                             audio_time_conversion_factor);

        while(NULL != pAudioFrameContents &&
                (uint32)aud_header.param1 < curoffset
             )
        {
          if (!audioHeaderWritten && audioHeaderSize > 0 )
          {
            byte *firstPacket;

            audioInfo.cbSize  = sizeof (AEEMovieSampleInfo);
            audioInfo.dwBytes = (uint32)nBytes_Audio + audioHeaderSize;
            audioInfo.dwTime  = (uint32)(aud_header.param1) - delta_time_audio;
            audioInfo.dwDelta = (uint32)(aud_header.param3);
            audioInfo.bSync   =  ((aud_header.param2==0)?FALSE:TRUE);
            firstPacket       = (byte*) cutnmerge_malloc(audioInfo.dwBytes +audioHeaderSize);
            (void)MEMCPY(firstPacket, pAudioHeaderContents, audioHeaderSize);
            (void)MEMCPY(firstPacket+audioHeaderSize, pAudioFrameContents, audioInfo.dwBytes);
            (void)IMOVIEFILE_WriteStream(pOutFile, (uint16)audioStreamID, 1,
                                   &audioInfo, firstPacket);
            cutnmerge_free(firstPacket);
            firstPacket = NULL;
            audioHeaderWritten = TRUE;
          }
          else
          {
            audioInfo.dwTime    = (uint32)aud_header.param1 - delta_time_audio;
            audioInfo.bSync     = ((aud_header.param2==0)?FALSE:TRUE);
            audioInfo.dwDelta   = (uint32)aud_header.param3;
            audioInfo.dwSegment = (uint32)aud_header.param4;
            audioInfo.dwBytes   = (uint32)aud_header.size;

            (void)IMOVIEFILE_WriteStream(pOutFile, (uint16)audioStreamID, 1,
                                   &audioInfo, pAudioFrameContents);
          }

          /* read the next audio frame */
          pAudioFrameContents = camcorderutil_llmo_read(
                                    &audio_data_read,
                                    &nBytes_Audio,
                                    &aud_header
                                    );
          dCurrentAudioFrameNum++;
          dCurrentAudioFrameTime = audioInfo.dwTime;

        } /* write audio loop */
      } /* if audio */

      /* advance the ping-pong indices */
      prevIdx = ppIdx;
      ppIdx   = 1-prevIdx;

    }
    if(cycle == 0)
    {
      segment1_lastFrameNum = dCurrentVideoFrameNum-1;
      latestAudioFrameNUM   = dCurrentAudioFrameNum;

      segment1_lastFrameTime      = dCurrentVideoFrameTime;
      segment1_lastFrameTimeAudio = dCurrentAudioFrameTime;

      /* the second segment time will continue from the end of the
      ** first segment
      */
      secondSegmentStartTime      = testCodedVopResPrev;


      /* free up all audio memory */
      if(audioStreamID >= 0 )
      {
        camcorderutil_llmo_destructor(audio_data_read);
      }
    }
    else if (cycle == 1 )
    {
      if(latestSyncFrameNum < segment1_lastFrameNum )
      {
        /* the first sync frame is before the first marker so
        ** basically no video was cut
        */
        QCAMCORDER_MSG_ERROR("Warning: no frames were cut because no sync frames found",
                   0,0,0);
        latestSyncFrameNum = segment1_lastFrameNum - 1;
        seg2audio.data     = latestAudioFrameNUM;
        seg2audio.whichone = CUTNMERGE_START_SEG2_AUDIO_WITH_FRAMENUM;
        seg2StartSyncTime  = segment1_lastFrameTime;
        /* since no gap was cut out there is no delta_time
        */
      }
      else
      {
        seg2audio.data     = latestSyncTime * audio_time_conversion_factor;
        seg2audio.whichone = CUTMERGE_START_SEG2_AUDIO_WITH_TIME;

        /* compute the delta time (the time of the gap that was cut out */
        delta_time_video   =  latestSyncTime - segment1_lastFrameTime -
                              vidInfoPP[prevIdx].dwDelta;
        if(audioStreamID >= 0)
          delta_time_audio =  seg2audio.data - audioInfo.dwDelta;


        /* input of 2nd segment will start from sync */
        testCodedVopRes    = testCodedVopSync;
        seg2StartSyncTime  = latestSyncTime;
      }
    }
  }  /* for ( cycle */

  if (!bError && textStreamID >= 0)
  {
      double dblTextTimeScaleFactor;   /* convert video time scale to text */
      uint32 nTextSample = 0, nTextSize = 0, nDescSize = 0;
      uint8 *pTextBuffer = NULL, *pDescBuffer = NULL;
      AEEMovieSampleInfo textInfo;
      enum cutmerge_timedtext_state {
          TT_SEG1, /* writing frames till first marker */
          TT_SKIP,  /* skipping frames till second marker */
          TT_SEG2   /* writing frames from second marker to end */
      } cycle = TT_SEG1;
      uint32 runningtime=0, delta=0, current_desc=0;
      uint32 start_marker, end_marker;
      boolean write_frame = TRUE; /* start writing frames by default */


      /* Rewind it again before we process timed text */
      IMOVIEFILE_Close(pInFile);
      (void)IMOVIEFILE_OpenMp4Read(pInFile, pMe->sCurrentFileName, &mfi);

      textInfo.cbSize = sizeof(textInfo);

      dblTextTimeScaleFactor = (double) textTimeScale / (double) videoTimeScale;
      start_marker = pMe->m_dFirstMarker * textTimeScale -
                      textTimeScale/2;
      end_marker = seg2StartSyncTime * dblTextTimeScaleFactor;

      /* go through all samples in text stream */
      for (nTextSample=0;
             NULL!=(pTextBuffer = (uint8 *)
                      IMOVIEFILE_GetSampleStream(pInFile,textStreamID,1,
                                        nTextSample,&nTextSize,&textInfo)
                   );
             nTextSample++
            )
        {
            delta=textInfo.dwDelta;


            if (cycle == TT_SEG1)
            {
                /* if we have found a frame that passes the start marker*/
                if ((textInfo.dwTime + textInfo.dwDelta >= start_marker) &&
                    (textInfo.dwTime + textInfo.dwDelta >= end_marker))
                {
                    /* current text frame spans over cut region, so
                    we can bypass skipping cycle*/
                    cycle = TT_SEG2;
                    delta = textInfo.dwDelta - (end_marker - start_marker);

                }
                else if (textInfo.dwTime + textInfo.dwDelta >= start_marker)
                {
                    /* now start skipping frames because we passed first marker */
                    cycle = TT_SKIP;
                    delta = start_marker - textInfo.dwTime;
                }
            }

            else if (cycle == TT_SKIP)
            {


                if (textInfo.dwTime + textInfo.dwDelta > end_marker)
                {
                    /* we are in the skipping cycle and have reached
                       end marker */
                    /* need to write current frame, but only the portion
                    of it that is after the end marker */
                    cycle = TT_SEG2;
                    delta = textInfo.dwDelta - (end_marker - textInfo.dwTime);
                    write_frame=TRUE; /* write frame in this special case */
                }
                else
                {
                    /* do NOT write current frame */
                    write_frame=FALSE;
                }
            }

            if (write_frame) /* do we need to write the current frame? */
            {
                /* ??? the following code is common to video editing, cutmerge, so
                can we merge it into one function ??? */

                /* dwSegment is 0-based; current_desc is 1-based */
                /* is the description atom number associated with the
                ** sample we just read > the last description atom number
                ** we wrote ? */
                if ((textInfo.dwSegment+1)>current_desc)
                {
                    /* read and write description tx3g atoms */
                    if (IMOVIEFILE_ReadSegmentHeader(pInFile, textStreamID,
                              textInfo.dwSegment, NULL, &nDescSize)!=SUCCESS)
                    {
                        QCAMCORDER_MSG_ERROR("reading description atom size failed ",textInfo.dwSegment,0,0);
                        bError = TRUE;
                        break;
                    }

                    if (pDescBuffer)
                        pDescBuffer = REALLOC(pDescBuffer, nDescSize);
                    else
                        pDescBuffer = MALLOC(nDescSize);

                    if (pDescBuffer == NULL)
                    {
                        QCAMCORDER_MSG_ERROR ("description buffer malloc failed",0,0,0);
                        bError = TRUE;
                        break;
                    }

                    if (IMOVIEFILE_ReadSegmentHeader(pInFile, textStreamID, textInfo.dwSegment, pDescBuffer, &nDescSize)!=SUCCESS)
                    {
                        QCAMCORDER_MSG_ERROR("reading description atom failed ",textInfo.dwSegment,0,0);
                        bError = TRUE;
                        break;
                    }

                    if (IMOVIEFILE_WriteSegmentHeader(pOutFile,textStreamID,pDescBuffer,nDescSize)!=SUCCESS)
                    {
                        QCAMCORDER_MSG_ERROR("writing description atom failed ",0,0,0);
                        bError = TRUE;
                        break;
                    }

                    current_desc=textInfo.dwSegment+1; /* keep track of current description atom written */
                }

                /* Write text sample */
                textInfo.dwTime = runningtime;
                textInfo.dwDelta = delta;
                runningtime += delta;
                IMOVIEFILE_WriteStream(pOutFile,textStreamID,1,&textInfo,pTextBuffer);

            }

            if(pTextBuffer) /* free text sample */
            {
                FREE(pTextBuffer);
                pTextBuffer = NULL;
            }
        }

        /* free text and description buffers */
        if(pTextBuffer)
        {
            FREE(pTextBuffer);
            pTextBuffer = NULL;
        }
        if (pDescBuffer)
        {
            FREE(pDescBuffer);
            pDescBuffer=NULL;
        }

      }


  if(bError )
    QCAMCORDER_MSG_ERROR("VideoCutMerg exited wtih an error",0,0,0);

  m4v2rgb_terminate(m4v2rgb_handle);

  if(audioStreamID >= 0 )
    camcorderutil_llmo_destructor(audio_data_read);

  if (newStream )
    cutnmerge_free(newStream);

  if(pVideoHeaderContents)
  {
    cutnmerge_free(pVideoHeaderContents);
    pVideoHeaderContents = NULL;
  }

  if(pAudioHeaderContents)
  {
    /* pAudioHeaderContents is allocated by IMoviefile which uses MALLOC()
    */
    FREE(pAudioHeaderContents);
  }

  if(pFrameContents[0].buffer )
  {
    cutnmerge_free(pFrameContents[0].buffer);
    pFrameContents[0].buffer = NULL;
  }
  if(pFrameContents[1].buffer )
  {
    cutnmerge_free(pFrameContents[1].buffer);
    pFrameContents[1].buffer = NULL;
  }

  if(pInFile)
  {
    (void)IMOVIEFILE_Release(pInFile);
    pInFile = NULL;
  }
  if(pOutFile)
  {
    (void)IMOVIEFILE_Release(pOutFile);
    pOutFile = NULL;
  }
  QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
  pMe->eState = QCAMCORDER_CUT_MERGE_DONE;

#ifdef FEATURE_CAMCORDER_MALLOC
  {
    static msg_const_type m;
    camcorderutil_malloc_terminate(cutnmerge_malloc_context,
                                   __LINE__,__func__,&m);
    cutnmerge_malloc_context = NULL;
  }
#endif
  return TRUE;
}
//lint +esym(644,videoInfo)
#undef CUTMERGE_ERREXIT
#endif /* FEATURE_BREW_IMOVIEEDIT */

#endif

#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
#ifdef FEATURE_BREW_IMOVIEEDIT
boolean Qcamcorder_VideoEditing(Qcamcorder * pMe)
{
  char *pData;
  AEEMediaData src;

  pData = (void *)Qcamcorder_GetVideoEditFileName(pMe);
  (void)STRLCPY(pMe->sVideoEditFileName, pData,
                 sizeof(pMe->sVideoEditFileName));

  pMe->m_nEditPortion = 1;
  pMe->m_cutInfo = MALLOC(sizeof(AEEMovieEditCutInfo)
    *pMe->m_nEditPortion);
  pMe->m_cutInfo[0].dwStartTime = (pMe->m_dStartPos*1000);
  pMe->m_cutInfo[0].dwEndTime = (pMe->m_dwPlayPos*1000);

  if (!pMe->pIFileMgr) {
    (void)ISHELL_CreateInstance(
      pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pMe->pIFileMgr);
  }
  if (IFILEMGR_Test(pMe->pIFileMgr, pMe->sVideoEditFileName) == SUCCESS) {
    if (IFILEMGR_Remove(pMe->pIFileMgr, pMe->sVideoEditFileName) != SUCCESS) {
      QCAMCORDER_MSG_HIGH("COULD NOT DELETE THE VIDEO EDITING FILE",0,0,0);
      return FALSE;
    }
  }
  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IMOVIEEDIT,
                            (void **)&(pMe->m_pInEdit)) !=  SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VideoEditing cannot create infile",0,0,0);
    return FALSE;
  }

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IMOVIEEDIT,
                            (void **)&(pMe->m_pOutEdit)) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VideoEditing cannot create outfile",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
    return FALSE;
  }

  src.clsData = MMD_FILE_NAME;
  src.pData = pMe->sCurrentFileName;
  CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_CutReadOpenCB, pMe);
  if (IMOVIEEDIT_ReadOpen(pMe->m_pInEdit, &src,
                           &(pMe->m_mei), &(pMe->m_movieeditStatus),
                           &(pMe->m_movieeditCB)) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VIdeoEditing cannot open input file",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
    return FALSE;
  }
  return TRUE;
}

#elif defined(FEATURE_BREW_IMOVIEFILE)

//lint -esym(644,aud_header)  Suppress complaint about not initialized aud_header
/* ==========================================================================
FUNCTION QCAMCORDER_VIDEOEDITING
DESCRIPTION Takes the current video and changes the audio to the audio
that was just recorded and saves to another mp4 file.
============================================================================= */
boolean Qcamcorder_VideoEditing(Qcamcorder * pMe)
{
  uint32 dCurrentAudioFrameNum = 0, dCurrentVideoFrameNum = 0;
  uint32 audioHeaderSize = 0, videoHeaderSize = 0;
  AEEMovieFileInfo mfi;
  byte *pVideoHeaderContents = NULL, *pAudioHeaderContents = NULL;
  byte *tmp = NULL,*pAudioFrameContents = NULL;
  boolean status = FALSE;
  boolean videoHeaderWritten = FALSE, audioHeaderWritten = FALSE;
  AEEMovieSampleInfo videoInfo, outputVideoInfo;
  AEEMovieSampleInfo audioInfo, outputAudioInfo;
  uint32 curoffset=0;
  IMovieFile *pInFile = NULL, *pOutFile = NULL;
  AEEMediaData md;
  uint32 const INITIAL_HEADER_SIZE  = 30;
  int16 videoStreamID= -1,audioStreamID= -1,textStreamID = -1;
  uint32 videoTimeScale=0, audioTimeScale=0, textTimeScale=0;
  uint32 videoStreamTimeScale = 0;             /* TS of the elementary video stream */
  double videoTimeScaleConversionFactor = 0.0; /* videoTimeScale/videoStreamTS */
  double videoStreamConversionFactor = 0.0;    /* videoStreamTS/videoTimeScale */
  uint16 i = 0;
  int32 latestSyncFrameNum = 0;
  uint32 nBytes,latestSyncFrameTime = 0;
  boolean bError = FALSE;
  camcorderutil_llmo_data_header_type aud_header;
  uint32 target_video_time = 0;
  double dbl_audio_time_conversion_factor = 0.0;
  M4V2RGB_handle *m4v2rgb_handle = NULL;
  boolean  bFooterWritten=FALSE;  /* TRUE when End-of-Stream written*/
  /* ping-pong buffer for video frame write */
  byte *pFrameContents[2] = {NULL,NULL};
  AEEMovieSampleInfo vidInfoPP[2]; /* this also needs pingponged */
  uint16 ppIdx;                    /* ping pong index for above */
  uint16 prevIdx;                  /* previous ping-pong index */

  uint32 override_delta;   /* override delta for m4v2rgb subroutines */

  /* for user atoms e.g. telop text */
  uint32 *pUserAtom = NULL;                /* user atom buffer */
  uint32 nUserAtomSize=0, nUserAtom=0;


  m4v2rgb_test_codedVOP_output_type testCodedVopRes;
  m4v2rgb_test_codedVOP_output_type testCodedVopResPrev;
  m4v2rgb_test_codedVOP_output_type testCodedVopSync;  /* result at sync frame */
  m4v2rgb_test_codedVOP_output_type outputVopRes={0},outputVopResPrev;
  uint8 **newStream = NULL;
  /* if visual_object_sequence_start_code exists at the header. If TRUE
  ** then we need to write the footer (visual_object_sequence_end_code)
  */
  boolean bStreamHeaderExists = FALSE;

  /* audio packets must be stored in memory temporarily */
  camcorderutil_llmo_type *audio_data_write = NULL,*audio_data_read= NULL;

  /* this is a place to store the results of m4v2rgb_test_codedVOP() */
  M4V2RGB_return_type testRes;

  md.clsData = MMD_FILE_NAME;
  md.pData = (void *)Qcamcorder_GetVideoEditFileName(pMe);
  (void)STRLCPY(pMe->sVideoEditFileName, md.pData,
                 sizeof(pMe->sVideoEditFileName));
  md.dwSize = 0;

  (void)ICAMERA_SetVideoEncode(pMe->m_pICam, AEECLSID_MEDIAMPEG4, 0);

  (void)ICAMERA_SetMediaData(pMe->m_pICam, &md, pMe->pFileFormat);

  if (!pMe->pIFileMgr) {
    (void)ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pMe->pIFileMgr);
  }
  if (IFILEMGR_Test(pMe->pIFileMgr, pMe->sVideoEditFileName) == SUCCESS) {
    if (IFILEMGR_Remove(pMe->pIFileMgr, pMe->sVideoEditFileName) != SUCCESS) {
      QCAMCORDER_MSG_HIGH("COULD NOT DELETE THE VIDEO EDITING FILE",0,0,0);
      QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
      pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
      return FALSE;
    }
  }
  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE,
                            (void **)&pInFile) !=  SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VideoEditing cannot create infile",0,0,0);
    pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
    QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
    return FALSE;
  }

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE,
                            (void **)&pOutFile) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VideoEditing cannot create outfile",0,0,0);
    (void)IMOVIEFILE_Release(pInFile);
    pInFile = NULL;
    pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
    QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
    return FALSE;
  }

  mfi.cbSize =  sizeof(AEEMovieFileInfo);
  if (IMOVIEFILE_OpenMp4Read(pInFile, pMe->sCurrentFileName, &mfi) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VIdeoEditing cannot open input file",0,0,0);
    (void)IMOVIEFILE_Release(pInFile);
    (void)IMOVIEFILE_Release(pOutFile);
    QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
    pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
    pInFile = NULL;
    pOutFile = NULL;
    return FALSE;
  }
  mfi.nBrand = Qcamcorder_GetFileBrand(pMe);
  if (IMOVIEFILE_OpenMp4Write(pOutFile, md.pData, &mfi) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("VIdeoEditing cannot open output file",0,0,0);
    (void)IMOVIEFILE_Release(pInFile);
    (void)IMOVIEFILE_Release(pOutFile);
    pInFile = NULL;
    pOutFile = NULL;
    pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
    QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
    return FALSE;
  }

  /* find out the StreamID neede for IMOVIEFILE_GetSampleStream */
  videoStreamID = -1;
  audioStreamID = -1;
  textStreamID  = -1;

  for (i = 0; i < mfi.nStreams; ++i) {
    if (mfi.pStreams[i].format == AEECLSID_MPEG4_VIDEO) {
      videoStreamID = (int16)i;
      videoTimeScale = mfi.pStreams[i].nTimescale;
    } else if (mfi.pStreams[i].format == AEECLSID_MEDIAQCP ) {
      audioStreamID = (int16)i;
      audioTimeScale = mfi.pStreams[i].nTimescale;
    } else if (mfi.pStreams[i].format == AEECLSID_3GPP_TEXT ) {
      textStreamID = (int16) i;
      textTimeScale = mfi.pStreams[i].nTimescale;
    }
  }

  /* Video edit only if the formats are supported */
  if(videoStreamID == -1 || videoTimeScale == 0)
  {
    QCAMCORDER_MSG_ERROR("Video Editing: MP4 file contains no recognized video/audio",0,0,0);
    (void)IMOVIEFILE_Release(pInFile);
    (void)IMOVIEFILE_Release(pOutFile);
    pInFile = NULL;
    pOutFile = NULL;
    if(IFILEMGR_Test(pMe->pIFileMgr,pMe->sVideoEditFileName) == SUCCESS)
    {
      (void)IFILEMGR_Remove(pMe->pIFileMgr, pMe->sVideoEditFileName);
    }
    pMe->eState = QCAMCORDER_VIDEO_EDIT_UNSUPPORTED;
    QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
    return FALSE;
  }

  videoInfo.cbSize = sizeof(videoInfo);
  audioInfo.cbSize = sizeof(audioInfo);

  /* copy over user atoms */
  if (IMOVIEFILE_ReadUserAtom(pInFile, nUserAtom, NULL, &nUserAtomSize) != SUCCESS)
  {
      /* ??? Is this fatal ??? or is this a non-fata UUID error?
      ** If warning we should just QCAMCORDER_MSG_ERROR the warning and do nothing.
      ** If error we set bError to TRUE
      */
      QCAMCORDER_MSG_ERROR("Error reading size of user atom # %d",nUserAtom,0,0);
      bError = TRUE;
  }
  while (!bError && nUserAtomSize)
  {
      if (pUserAtom)
      {
        pUserAtom = REALLOC(pUserAtom, nUserAtomSize);
      }
      else
      {
        pUserAtom = MALLOC(nUserAtomSize);
      }

      if (pUserAtom == NULL)
      {
          QCAMCORDER_MSG_ERROR("user atom malloc failed",0,0,0);
          bError = TRUE;
          break;
      }

      if (IMOVIEFILE_ReadUserAtom(pInFile, nUserAtom, pUserAtom, &nUserAtomSize) != SUCCESS)
      {
          QCAMCORDER_MSG_ERROR("cannot read user atom",0,0,0);
          bError = TRUE;
          break;
      }

      if (IMOVIEFILE_WriteUserAtom(pOutFile, pUserAtom, nUserAtomSize) != SUCCESS)
      {
          QCAMCORDER_MSG_ERROR("Error writing user atom",0,0,0);
          bError = TRUE;
          break;
      }

      nUserAtom++;

      if (IMOVIEFILE_ReadUserAtom(pInFile, nUserAtom, NULL, &nUserAtomSize) != SUCCESS)
      {
          QCAMCORDER_MSG_ERROR("Error reading size of user atom # %d",nUserAtom,0,0);
          bError = TRUE;
          break;
      }
  }
  if (pUserAtom)
  {
      FREE(pUserAtom);
      pUserAtom = NULL;
  }

  /* We have to rewind because if the previous I-Frame belongs to a different
  ** sync frame we cannot get there by IMOVIEFILE_GetSampleInfo().
  */
  if( !bError)
  { IMOVIEFILE_Close(pInFile);
    (void)IMOVIEFILE_OpenMp4Read(pInFile, pMe->sCurrentFileName, &mfi);
  }

  videoHeaderSize = INITIAL_HEADER_SIZE;
  tmp = (byte*) MALLOC(videoHeaderSize * sizeof(byte));
  if(tmp == NULL )
  { bError = TRUE;
  }
#ifdef FEATURE_MP4_DECODER
  else
  {
    if (OEMMediaMPEG42PV_GetVideoTrackHeader(tmp, &videoHeaderSize) == FALSE) {
      FREE(tmp);
      tmp = NULL;
      if (videoHeaderSize) {
        tmp = (byte*) MALLOC(videoHeaderSize);
        status = OEMMediaMPEG42PV_GetVideoTrackHeader(tmp, &videoHeaderSize);
      }
    }
    pVideoHeaderContents = tmp;
    tmp = NULL;
  }
#endif//FEATURE_MP4_DECODER
  if(!bError)
  {
    boolean bShortHeader = FALSE;

    /* initialize the m4v2rgb engine because it has a parser which can be
    ** used to determine if a VOP is I- or P-
    */

    int16 errorSubCode;
    uint16 width,height;
    M4V2RGB_return_type errorCode;

    if(videoHeaderSize == 0 )
    {
      /* this will be the case if its a short header or h.263
      ** in this case let the first frame be the header for m4v2rgb_init
      */
      pVideoHeaderContents =  (byte *)
                IMOVIEFILE_GetSampleStream(pInFile,videoStreamID,1,0,
                                           &nBytes,&videoInfo);
      videoHeaderSize = nBytes;
      bShortHeader    = TRUE;

      if(videoHeaderSize == 0 || pVideoHeaderContents == NULL )
      {
        QCAMCORDER_MSG_ERROR("Cant get header",0,0,0);
        bError = TRUE;
      }

    }

    if(!bError )
    {
      m4v2rgb_handle = m4v2rgb_init(pVideoHeaderContents,videoHeaderSize,&width, &height,
                  NULL,&errorCode,&errorSubCode,qcmd_malloc,qcmd_free,NULL);

      /* find out if we have the header */
      m4v2rgb_test_codedVOP(pVideoHeaderContents,videoHeaderSize,&testCodedVopResPrev,
        &testCodedVopRes, -1, m4v2rgb_handle);

      bStreamHeaderExists = testCodedVopRes.bHasSequenceStartCode;
      videoStreamTimeScale = testCodedVopRes.time_increment_resolution;
      if(videoStreamTimeScale == 0 || videoTimeScale == 0)
      { QCAMCORDER_MSG_ERROR("Invalid stream timescale or timescale",0,0,0);
        pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
        bError = TRUE;
      }
      else
      {
        videoTimeScaleConversionFactor = ( (double) videoTimeScale) /
                                            ((double) videoStreamTimeScale);
        videoStreamConversionFactor    = ( (double) videoStreamTimeScale) /
                                            ((double) videoTimeScale);
      }
      MEMSET(&testCodedVopRes,0,sizeof(m4v2rgb_test_codedVOP_output_type));

      if(bShortHeader)
      {
         videoHeaderSize = 0;  /* prevent this from being copied as the header */
         FREE(pVideoHeaderContents);
         pVideoHeaderContents = NULL;
      }
    }
  }


  if ( !bError && audioStreamID >= 0 ) {
    pAudioHeaderContents = (byte *) IMOVIEFILE_GetSampleStream(pInFile,audioStreamID,
                                                               1,0,&nBytes,&audioInfo);

    audioHeaderSize = INITIAL_HEADER_SIZE;
    tmp = (byte*) MALLOC(audioHeaderSize * sizeof(byte));
    if(tmp == NULL )
    {
      bError = TRUE;
    }
    else
    {
      if (OEMMediaMPEG42PV_GetAudioTrackHeader(tmp, &audioHeaderSize) == FALSE) {
        FREE(tmp);
        tmp = NULL;
        if (audioHeaderSize) {
          tmp = (byte*) MALLOC(audioHeaderSize);
          status = OEMMediaMPEG42PV_GetAudioTrackHeader(tmp, &audioHeaderSize);
        }
      }
      ASSERT(audioHeaderSize <= audioInfo.dwBytes);
      FREE(tmp);
      tmp = NULL;
    }

  }

  /* in this loop, we're just looking for the nearest I-Frame in
  ** front of pMe->m_dStartPos.
  ** If the previous sync frame belongs to a previous segment then
  ** we're toast. So we need to rewind first.
  */


  /* this being -1 tells us that we didn't find a single sync frame
  */
  latestSyncFrameNum = -1;

  testCodedVopRes.time = 0;

  if(!bError )
  for (dCurrentVideoFrameNum=0;
      NULL !=  (tmp = (byte *)
                IMOVIEFILE_GetSampleStream(pInFile,(uint16)videoStreamID,1,dCurrentVideoFrameNum,
                                           &nBytes,&videoInfo)
               );
      ) {
    /* override any EStime computation with the delta read in here */
    override_delta = videoStreamConversionFactor * videoInfo.dwDelta;

    /* copy the current state into the prev parameters */
    testCodedVopResPrev = testCodedVopRes;
    testRes =
      m4v2rgb_test_codedVOP(tmp,(uint16)nBytes,&testCodedVopResPrev,
                            &testCodedVopRes,override_delta,
                            m4v2rgb_handle);
    if (videoInfo.bSync ) {
      latestSyncFrameNum = (int32)dCurrentVideoFrameNum;
      latestSyncFrameTime = videoInfo.dwTime;
      testCodedVopSync = testCodedVopResPrev;
    } else {
      /*  videoInfo.bSync is not reliable after the first fragment
      **  so we better decode the VOP header to see if its an I-Frame.
      **  So this will not work for H264 files.
      */
      if (M4V2RGB_NoError != testRes)
      {
        pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
        bError = TRUE;
      }
      if(testCodedVopRes.time == videoInfo.dwTime)
      {
        QCAMCORDER_MSG_LOW("timediff=%d",testCodedVopRes.time - videoInfo.dwTime,0,0);
      }
      if (testCodedVopRes.bCoded && testCodedVopRes.codingType == 0 ) {
        latestSyncFrameTime = videoInfo.dwTime;
        latestSyncFrameNum = (int32)dCurrentVideoFrameNum;
        testCodedVopSync   = testCodedVopResPrev;
      }
    }

    FREE(tmp);
    tmp = NULL;

    if ( videoInfo.dwTime >= pMe->m_dStartPos * videoTimeScale)
      break;


    /* every 128 calls we pause to allow other processes to go. This way
    ** the UI task does not starge out other tasks
    */
    dCurrentVideoFrameNum++;
    if(0 == (dCurrentVideoFrameNum&0xFF))
      MSLEEP(10);
  }

  if(latestSyncFrameNum == -1 )
  {
    QCAMCORDER_MSG_ERROR("Warning: VideoEditing could not find a single sync frame",0,0,0);
  }


  if (videoInfo.dwTime < pMe->m_dStartPos * videoTimeScale) {
    QCAMCORDER_MSG_ERROR("ERROR start is beyond the end of file",0,0,0);
    bError = TRUE;
  }


  /* Since there's no way in IMovieFile to tell if we crossed
  ** a fragment boundary we must rewind again to be sure.
  */
  IMOVIEFILE_Close(pInFile);
  if (!bError )
    (void)IMOVIEFILE_OpenMp4Read(pInFile, pMe->sCurrentFileName, &mfi);


  if(latestSyncFrameNum == 0 )
  {
    /* We're starting from the first frame.
    ** Since IMovieFile gives us the header along with the first
    ** frame we shouldn't repeat.
    */
    videoHeaderWritten = TRUE;
    audioHeaderWritten = TRUE;
  }


  if (audioStreamID >= 0 && videoTimeScale != 0)
  {
    /* Find the audio timescale conversion factor
    */
    dbl_audio_time_conversion_factor =
      ((double) audioTimeScale)/((double) videoTimeScale);


    /*******************************************
    ** Fast firward Audio data to starting point
    ********************************************
    */

    audioInfo.cbSize = sizeof(AEEMovieSampleInfo);

    curoffset = (uint32)(dbl_audio_time_conversion_factor * latestSyncFrameTime);

    //curoffsetmsec = latestSyncFrameTime * 1000 / videoTimeScale;
    //curoffset   = curoffsetmsec * audioTimeScale/1000;

    if ( !bError)
      for (dCurrentAudioFrameNum = 0;
          SUCCESS == IMOVIEFILE_GetSampleInfo(pInFile,(uint16)audioStreamID,1,
                                             dCurrentAudioFrameNum,&audioInfo);
          ) {
        if (audioInfo.dwTime >= curoffset )
          break;

        /* we pause every 128 calls to make sure we don't starge out
        ** other tasks
        */
        dCurrentAudioFrameNum++;
        if(0 == (dCurrentAudioFrameNum & 0xFF))
          MSLEEP(10);
      }

    if (!bError)
      audio_data_write =
      camcorderutil_llmo_constructor(3000,qcmd_malloc,qcmd_free);
    audio_data_read = audio_data_write;

    /*******************************************
    ** Read Audio Samples into memory
    ********************************************
    */
    if (!bError)
      for (;
          NULL != (tmp = (byte *)
                   IMOVIEFILE_GetSampleStream(pInFile,
                                              (uint16)audioStreamID,1,
                                              dCurrentAudioFrameNum,
                                              &nBytes,&audioInfo)
                  );

          ) {

        if (pMe->m_dwPlayPos * audioTimeScale <= audioInfo.dwTime)
          break;

        aud_header.param1 = (int32)audioInfo.dwTime;
        aud_header.param2 = audioInfo.bSync;
        aud_header.param3 = (int32)audioInfo.dwDelta;
        aud_header.param4 = (int32)audioInfo.dwSegment;
        if (camcorderutil_llmo_write(&audio_data_write,(int32)nBytes,tmp,
                                     &aud_header)
           ) {
          QCAMCORDER_MSG_ERROR("error saving audio to memory",0,0,0);
          FREE(tmp);
          tmp = NULL;
          camcorderutil_llmo_destructor(audio_data_write);
          bError = TRUE;

        }
        FREE(tmp);
        tmp = NULL;

        dCurrentAudioFrameNum++;
        if (0 == (dCurrentAudioFrameNum & 0xFF))
          MSLEEP(10);

      }

    /* Rewind it again before we start video processing
    */
    IMOVIEFILE_Close(pInFile);
    if (!bError)
      (void)IMOVIEFILE_OpenMp4Read(pInFile, pMe->sCurrentFileName, &mfi);

    aud_header.param1 = 0;
  }


  /* single step to the target sync frame */
  if (!bError)
    for (i=0; i<latestSyncFrameNum;)
    {
      (void)IMOVIEFILE_GetSampleInfo(pInFile,(uint16)videoStreamID,1,i,&videoInfo);

      i++;
      if(0 == (i&0xFF))
      {
        MSLEEP(10);
      }
    }

  testCodedVopRes   = testCodedVopSync;
  outputVopRes.time           = 0;
  outputVopRes.time_increment = 0;

  /* compute the target video time in video time scale
  ** PlayPos is rounded up so try to go down about 1/2 sec
  */
  target_video_time = pMe->m_dwPlayPos * videoTimeScale - videoTimeScale/2;

  if(!bError )
  {

    newStream = (uint8 **) MALLOC(sizeof(uint8 *));
    *newStream = NULL;

    /* As we read video frames, store it into a ping-pong buffer.
    ** This way at the last frame we can append the footer if needed
    */
    pFrameContents[0] = pFrameContents[1] = NULL;
    ppIdx = 0;
    vidInfoPP[0].cbSize = vidInfoPP[1].cbSize = sizeof(AEEMovieSampleInfo);
    audioInfo.cbSize = sizeof(AEEMovieSampleInfo);

    dCurrentVideoFrameNum = (uint32)latestSyncFrameNum;

    /* get the first frame to fill in the first part of the ping-pong buffer*/
    pFrameContents[ppIdx] = (byte *)
      IMOVIEFILE_GetSampleStream(pInFile,(uint16)videoStreamID,1,dCurrentVideoFrameNum++,
                                &nBytes,&vidInfoPP[ppIdx]);

    if(pFrameContents[ppIdx] == NULL )
    {
      QCAMCORDER_MSG_ERROR("Empty Cut",0,0,0);
      bError = TRUE;
    }

    ppIdx   = 1;
    prevIdx = 0;


    while( !bError &&
           target_video_time >= vidInfoPP[prevIdx].dwTime &&
           NULL != pFrameContents[prevIdx]
         )
    {
      int32 num_bytes;
      uint16 newNumBytes;

      pFrameContents[ppIdx] = (byte *)
       IMOVIEFILE_GetSampleStream(pInFile,videoStreamID,1,
                                  dCurrentVideoFrameNum,&nBytes,
                                  &vidInfoPP[ppIdx]);

      testCodedVopResPrev = testCodedVopRes;
      outputVopResPrev    = outputVopRes;


      /* to prevent drift we need to compute delta time based on
      ** media time
      */
      override_delta =
        videoStreamConversionFactor *
            (vidInfoPP[prevIdx].dwTime -
               (videoTimeScaleConversionFactor *testCodedVopResPrev.time)
            );

      testRes = m4v2rgb_VOPtime_offset(
        pFrameContents[prevIdx],(uint16)vidInfoPP[prevIdx].dwBytes,
        &testCodedVopResPrev,&testCodedVopRes,
        &outputVopResPrev,&outputVopRes,override_delta,
        newStream,&newNumBytes,m4v2rgb_handle);

      if(testRes != M4V2RGB_NoError )
      {
        bError = TRUE;
        continue;
      }

      if(newNumBytes != 0 )
      {
        FREE(pFrameContents[prevIdx]);
        pFrameContents[prevIdx]    = *newStream;
        vidInfoPP[prevIdx].dwBytes = newNumBytes;
        *newStream = NULL;
      }


      if (!videoHeaderWritten) {
        byte *firstPacket;

        outputVideoInfo.cbSize = sizeof (AEEMovieSampleInfo);
        outputVideoInfo.dwBytes = vidInfoPP[prevIdx].dwBytes + videoHeaderSize;
        outputVideoInfo.dwTime  = 0.5 +
          (outputVopRes.time * videoTimeScaleConversionFactor);
        outputVideoInfo.dwDelta = 0.5 +
          (outputVopRes.timeDelta * videoTimeScaleConversionFactor);
        outputVideoInfo.bSync =  TRUE;

        /* We're required to attach the header info to the first
        ** video frame. The only way to do that is via memcpy.
        */
        firstPacket = (byte*) MALLOC(vidInfoPP[prevIdx].dwBytes +videoHeaderSize);
        if(firstPacket == NULL )
        {
          QCAMCORDER_MSG_ERROR("malloc error in video editing",0,0,0);
          goto QCAMCORDER_VIDEOEDITING_CLEANEXIT;
        }
        (void)MEMCPY(firstPacket, pVideoHeaderContents, videoHeaderSize);
        (void)MEMCPY(firstPacket+videoHeaderSize, pFrameContents[prevIdx],
               vidInfoPP[prevIdx].dwBytes);
        FREE(pFrameContents[prevIdx]);
        pFrameContents[prevIdx] = firstPacket;
        firstPacket = NULL;
        videoHeaderWritten = TRUE;
        FREE(pVideoHeaderContents);
        pVideoHeaderContents = NULL;

      } else {

        memcpy(&outputVideoInfo,&vidInfoPP[prevIdx],sizeof(AEEMovieSampleInfo));
        outputVideoInfo.dwTime  = 0.5 +
          ( outputVopRes.time * videoTimeScaleConversionFactor );
        outputVideoInfo.dwDelta = 0.5 +
          ( outputVopRes.timeDelta * videoTimeScaleConversionFactor );

        if (vidInfoPP[prevIdx].bSync == FALSE ) {
            /* because of fragments, we have to recompute this */
            if (M4V2RGB_NoError != testRes )
              bError = TRUE;
            if (testCodedVopRes.bCoded && testCodedVopRes.codingType == 0 )
                  outputVideoInfo.bSync = TRUE;

          }
      }

      /* see if this frame has the footer
      */
      if (testCodedVopRes.bHasSequenceEndCode )
      { bFooterWritten = TRUE;
        if(!bStreamHeaderExists )
          QCAMCORDER_MSG_ERROR("Wnaring: footer written but there is no header",0,0,0);
      }

      /* if we're the last frame and footer is not written then we
      ** need to write it
      */
      if ( !bFooterWritten &&
            ( target_video_time < vidInfoPP[ppIdx].dwTime ||
              pFrameContents[ppIdx] == NULL
            )
            && bStreamHeaderExists
         )
      {
        uint32 newsize;
        newsize = sizeof(QCAMCORDER_M4VFOOTER) + outputVideoInfo.dwBytes;
        pFrameContents[prevIdx] = (byte *)
              REALLOC(pFrameContents[prevIdx],newsize);
        if(pFrameContents[prevIdx] == NULL )
        {
          QCAMCORDER_MSG_ERROR("VideoEditing realloc error",0,0,0);
          goto QCAMCORDER_VIDEOEDITING_CLEANEXIT;
        }
        memcpy(&pFrameContents[prevIdx][outputVideoInfo.dwBytes],
               QCAMCORDER_M4VFOOTER,sizeof(QCAMCORDER_M4VFOOTER));
        outputVideoInfo.dwBytes = newsize;
        bFooterWritten = TRUE;
      }

      (void)IMOVIEFILE_WriteStream(pOutFile, (uint16)videoStreamID, 1,
                             &outputVideoInfo, pFrameContents[prevIdx]);


      if (!videoTimeScale ) {
        QCAMCORDER_MSG_ERROR("Error in Video Editing. MovieTimeScale == 0",0,0,0);
        bError = TRUE;
        break;
      }

      FREE(pFrameContents[prevIdx]);
      pFrameContents[prevIdx] = NULL;


      dCurrentVideoFrameNum++;

      /* give lower priority processes a chance
      */
      if(0 == (dCurrentVideoFrameNum & 0xFF))
        MSLEEP(10);

      if (audioStreamID >= 0 ) {

        curoffset = (uint32)(vidInfoPP[prevIdx].dwTime * dbl_audio_time_conversion_factor);

        while((uint32)aud_header.param1 < curoffset &&
              NULL != (pAudioFrameContents =
                        camcorderutil_llmo_read(
                          &audio_data_read,
                          &num_bytes,
                          &aud_header
                          )
                       )
              )
        {
          if (!audioHeaderWritten ) {
            byte *firstPacket;

            outputAudioInfo.cbSize = sizeof (AEEMovieSampleInfo);
            outputAudioInfo.dwBytes = (uint32)num_bytes + audioHeaderSize;
            outputAudioInfo.dwTime = (uint32)(aud_header.param1);
            outputAudioInfo.dwDelta = (uint32)(aud_header.param3);
            outputAudioInfo.bSync =  ((aud_header.param2==0)?FALSE:TRUE);
            firstPacket = (byte*) MALLOC(audioInfo.dwBytes +audioHeaderSize);
            (void)MEMCPY(firstPacket, pAudioHeaderContents, audioHeaderSize);
            (void)MEMCPY(firstPacket+audioHeaderSize, pAudioFrameContents, audioInfo.dwBytes);
            (void)IMOVIEFILE_WriteStream(pOutFile, (uint16)audioStreamID, 1,
                                   &outputAudioInfo, firstPacket);
            audioHeaderWritten = TRUE;
            FREE(firstPacket);
            firstPacket = NULL;


          } else {
            outputAudioInfo.dwTime  = (uint32)aud_header.param1;
            outputAudioInfo.bSync   = ((aud_header.param2==0)?FALSE:TRUE);
            outputAudioInfo.dwDelta = (uint32)aud_header.param3;
            outputAudioInfo.dwSegment = (uint32)aud_header.param4;
            outputAudioInfo.dwBytes  = (uint32)num_bytes;

            (void)IMOVIEFILE_WriteStream(pOutFile, (uint16)audioStreamID, 1,
                                   &outputAudioInfo, pAudioFrameContents);
          }

          pAudioFrameContents = NULL;
        } /* write audio loop */
      } /* if audio */

      /* advance the ping-pong indices */
      prevIdx = ppIdx;
      ppIdx   = 1-prevIdx;
    } /* main while loop */

  }

    /* If timed text stream is present, copy text samples and
    ** description atoms for cut region*/
    if(!bError && textStreamID >= 0)
    {
        double dblTextTimeScaleFactor;                           /* convert videoTimeScale to textTimeScale */
        uint32 nTextSample = 0, nTextSize = 0, nDescSize = 0;
        uint8 *pTextBuffer = NULL, *pDescBuffer = NULL;
        AEEMovieSampleInfo textInfo;
        uint32 runningtime = 0, delta = 0, current_desc = 0;
        uint32 textend;                                          /* cut end time */
        uint32 textstart;                                        /* cut start time */
        uint32 cycle=0;  /* 0 = searching for first frame to copy
                         ** 1 = writing out frames
                         ** 2 = exit
                         */


        /* Rewind it again before we process timed text */
        IMOVIEFILE_Close(pInFile);
        (void)IMOVIEFILE_OpenMp4Read(pInFile, pMe->sCurrentFileName, &mfi);

        textInfo.cbSize = sizeof(textInfo);

        dblTextTimeScaleFactor = ((double) textTimeScale ) / (double)videoTimeScale;
        textstart              =  dblTextTimeScaleFactor * latestSyncFrameTime;
        textend                =  target_video_time  * dblTextTimeScaleFactor;

        /* Find first text sample whose end time > starting marker */
        for (nTextSample=0;
             NULL!=(pTextBuffer = (uint8 *)
                      IMOVIEFILE_GetSampleStream(pInFile,textStreamID,1,
                                        nTextSample,&nTextSize,&textInfo)
                   );
             nTextSample++
            )
        {
            delta=textInfo.dwDelta;

            /* Searching for first frame to include and found it */
            if (cycle == 0 && textInfo.dwTime+textInfo.dwDelta>textstart)
            {
                runningtime = 0;
                delta = textInfo.dwTime+textInfo.dwDelta-textstart;
                cycle = 1; /* transition to writing frames state */
            }

            if (cycle==1) /* Write frames */
            {
                /* dwSegment is 0-based; current_desc is 1-based */
                if ((textInfo.dwSegment+1)>current_desc)
                {
                    /* read and write description tx3g atoms */
                    if (IMOVIEFILE_ReadSegmentHeader(pInFile, textStreamID,
                              textInfo.dwSegment, NULL, &nDescSize)!=SUCCESS)
                    {
                        QCAMCORDER_MSG_ERROR("reading description atom size failed ",textInfo.dwSegment,0,0);
                        bError = TRUE;
                        break;
                    }

                    if (pDescBuffer)
                        pDescBuffer = REALLOC(pDescBuffer, nDescSize);
                    else
                        pDescBuffer = MALLOC(nDescSize);

                    if (pDescBuffer == NULL)
                    {
                        QCAMCORDER_MSG_ERROR ("description buffer malloc failed",0,0,0);
                        bError = TRUE;
                        break;
                    }

                    if (IMOVIEFILE_ReadSegmentHeader(pInFile, textStreamID, textInfo.dwSegment, pDescBuffer, &nDescSize)!=SUCCESS)
                    {
                        QCAMCORDER_MSG_ERROR("reading description atom failed ",textInfo.dwSegment,0,0);
                        bError = TRUE;
                        break;
                    }

                    if (IMOVIEFILE_WriteSegmentHeader(pOutFile,textStreamID,pDescBuffer,nDescSize)!=SUCCESS)
                    {
                        QCAMCORDER_MSG_ERROR("writing description atom failed ",0,0,0);
                        bError = TRUE;
                        break;
                    }

                    current_desc=textInfo.dwSegment+1; /* keep track of current description atom written */
                }

                /* If we have reached the end marker,
                ** no need to write more frames after this one*/
                if (textInfo.dwTime+textInfo.dwDelta >= textend)
                {
                    delta=textend-textInfo.dwTime;
                    cycle=2;
                }

                /* If only one text frame is needed in cut section,
                ** it needs to be cropped from beginning and end */
                if (textInfo.dwTime <= textstart && textInfo.dwTime+textInfo.dwDelta >= textend)
                {
                    delta = textend-textstart;
                    cycle = 2;
                }

                /* Write text sample */
                textInfo.dwTime = runningtime;
                textInfo.dwDelta = delta;
                runningtime += delta;
                IMOVIEFILE_WriteStream(pOutFile,textStreamID,1,&textInfo,pTextBuffer);

            }

            if (cycle==2) /* exit */
            {
                break;
            }

            if(pTextBuffer) /* free text sample */
            {
                FREE(pTextBuffer);
                pTextBuffer = NULL;
            }
        }

        /* free text and description buffers */
        if(pTextBuffer)
        {
            FREE(pTextBuffer);
            pTextBuffer = NULL;
        }
        if (pDescBuffer)
        {
            FREE(pDescBuffer);
            pDescBuffer=NULL;
        }
  }

QCAMCORDER_VIDEOEDITING_CLEANEXIT:

  if (bError)
    QCAMCORDER_MSG_ERROR("VideoEditing exited with errors",0,0,0);

  (void)m4v2rgb_terminate(m4v2rgb_handle);

  if(audioStreamID >= 0)
    camcorderutil_llmo_destructor(audio_data_read);

  if ( newStream) {
    FREE(newStream);
  }

  if(pVideoHeaderContents)
    FREE(pVideoHeaderContents);

  if(pAudioHeaderContents)
    FREE(pAudioHeaderContents);

  if (pInFile) {
    (void)IMOVIEFILE_Release(pInFile);
    pInFile = NULL;
  }
  if (pOutFile) {
    (void)IMOVIEFILE_Release(pOutFile);
    pOutFile = NULL;
  }
  for(i=0; i<2; i++)
    if (pFrameContents[i]) {
      FREE (pFrameContents[i]);
      pFrameContents[i]= NULL;
    }
  QCAMCORDER_IMEDIA_STOP(pMe->pMedia);

  if(pMe->eState != QCAMCORDER_VIDEO_EDIT_DONE_ERROR)
    pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  return TRUE;
}
//lint +esym(644,aud_header)
#endif /* FEATURE_BREW_IMOVIEEDIT */
#endif
/* ==========================================================================
FUNCTION QCAMCORDER_DISPLAYMESSSAGE
DESCRIPTION
============================================================================= */
static void Qcamcorder_DisplayMesssage(Qcamcorder *pMe, char *title, char *text)
{
   AECHAR szBufTitle[TEXT_STRING_SIZE] = {0};
   AECHAR szBufText[PROMPT_SIZE]       = {0};
   if (title)
   {
      (void)STR_TO_WSTR(title, szBufTitle, sizeof(szBufTitle));
   }
   (void)STR_TO_WSTR(text, szBufText, sizeof(szBufText));
   (void)ISHELL_MessageBoxText(pMe->a.m_pIShell, szBufTitle, szBufText);
   return;
} /* END Qcamcorder_DisplayMesssage */

/* ==========================================================================
FUNCTION QCAMCORDER_BUILDPREVIEWMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_BuildPreviewMenu(Qcamcorder *pMe)
{
   AEERect qrc;
   CtlAddItem ai;
   IPosDet *pIPosDet;

#ifdef FEATURE_MMOVERLAY
   QCamcorder_MdpRegisterPreview(pMe, TRUE);

   /* Temporary fix to not display the video frame during audio only recording */

   if(pMe->m_pIMMOverlay_HideVideoFrm)
   {
     IMMOVERLAY_Release(pMe->m_pIMMOverlay_HideVideoFrm);
     pMe->m_pIMMOverlay_HideVideoFrm = NULL;
   }


   if(pMe->eState == QCAMCORDER_RECORDING && pMe->wVideoFormat == QCAMCORDER_VIDEO_FORMAT_NONE)
   {
     SETAEERECT (&qrc, 0,
                       QCAMCORDER_PICTURE_Y_POS + QCAMCORDER_TELOP_TEXT_SPACE,
                       pMe->di.cxScreen,
                       pMe->di.cyScreen - QCAMCORDER_SOFT_MENU_HEIGHT - QCAMCORDER_PROGRESS_BAR_HEIGHT);

     pMe->m_pIMMOverlay_HideVideoFrm = QCamcorder_MdpRegister(pMe->a.m_pIDisplay, 
                                                  pMe->m_pIMMOverlayMgr, 
                                                  qrc, 
                                                  FALSE);

   }
   /* End of - Temporary fix to not display the video frame during audio only recording */

#else

   if (pMe->eSize == QCAMCORDER_SIZE_QVGA)
     QCamcorder_MdpRegisterPreview(pMe, TRUE);
   else
     QCamcorder_MdpRegisterFullScreen(pMe, TRUE);
#endif /* FEATURE_MMOVERLAY */

   IMENUCTL_Reset(pMe->m_pIPreview);

   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 160, pMe->di.cxScreen, pMe->di.cyScreen-160);
   IMENUCTL_SetRect(pMe->m_pIPreview, &qrc);

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 0;
   ai.wText   = 0;

   //Add individual entries to the Menu

   //Add start/stop record movie
   if (pMe->eState == QCAMCORDER_RECORDING ||
       pMe->eState == QCAMCORDER_RESUME_RECORDING)
   {
      ai.wItemID = IDS_STOP;
      ai.wImage  = IDB_RECORDING;
   }
   else if (pMe->eState == QCAMCORDER_PAUSED)
   {
      ai.wItemID = IDS_STOP;
      ai.wImage  = IDB_RECORD_PAUSED;
   }
   else
   {
      ai.wItemID = IDS_START;
      ai.wImage  = IDB_IMAGE_ICON;
   }
   (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
   ai.dwData++;

   //Add pause
   if (pMe->eState == QCAMCORDER_RECORDING ||
       pMe->eState == QCAMCORDER_RESUME_RECORDING)
   {
      ai.wItemID = IDS_PAUSE;
      ai.wImage  = IDB_PAUSE;
      if (pMe->wPreviewMenuSel == IDS_RESUME)
      {
         pMe->wPreviewMenuSel = IDS_PAUSE;
      }
      (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
      ai.dwData++;
   }
   else if (pMe->eState == QCAMCORDER_PAUSED)
   {
      ai.wItemID = IDS_RESUME;
      ai.wImage  = IDB_RESUME;
      if (pMe->wPreviewMenuSel == IDS_PAUSE)
      {
         pMe->wPreviewMenuSel = IDS_RESUME;
      }
      (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
      ai.dwData++;
   }

   if ((pMe->eState == QCAMCORDER_RECORDING ||
       pMe->eState == QCAMCORDER_RESUME_RECORDING ||
       pMe->eState == QCAMCORDER_PAUSED) 
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
      )
   {
      //Add zoom
      ai.wItemID = IDS_ZOOM;
      ai.wImage  = IDB_ZOOM;
      (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
      ai.dwData++;
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
     //Add dynamic overlay menu
     if(pMe->bEnableFrameOverlay)
     {
       ai.wItemID = IDS_OVERLAY_MENU;
       ai.wImage = IDB_OVERLAY_MENU;
       (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
       ai.dwData++;
     }
#endif
   }

   if (pMe->eState == QCAMCORDER_PREVIEW)
   {
      //Add zoom
      ai.wItemID = IDS_ZOOM;
      ai.wImage  = IDB_ZOOM;
      (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
      ai.dwData++;

      //Add brightness
      ai.wItemID = IDS_BRIGHTNESS;
      ai.wImage  = IDB_BRIGHTNESS;
      (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
      ai.dwData++;

      //Add Focus
      ai.wItemID = IDS_FOCUS;
      ai.wImage  = IDB_FOCUS;
      (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
      ai.dwData++;

      //Add album
      ai.wItemID = IDS_ALBUM;
      ai.wImage = IDB_ALBUM;
      (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
      ai.dwData++;


#ifdef FEATURE_VIDEO_ENCODE_RAW
      //Add raw file album
      ai.wItemID = IDS_RAW_ALBUM;
      ai.wImage = IDB_RAW_ALBUM;
      (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
      ai.dwData++;
#endif

      // Check if GPS feature is available on this phone
      (void)ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_POSDET,
                            (void **)&pIPosDet);
      if (pIPosDet)
      {
         AEEGPSConfig config = {0};
         int result = IPOSDET_GetGPSConfig(pIPosDet, &config);
         if (result == SUCCESS)
         {
            //Add GPS
            ai.wItemID = IDS_GPS_SETUP;
            ai.wImage = (uint16)(pMe->eGPSState == QCAMCORDER_GPS_DATA_VALID ?
                                 IDB_GPS_ACTIVE : IDB_GPS_INACTIVE);

            (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
            ai.dwData++;
         }
         (void)IPOSDET_Release(pIPosDet);
         pIPosDet = NULL;
      }
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
      //Add dynamic overlay menu
      if(pMe->bEnableFrameOverlay)
      {
        ai.wItemID = IDS_OVERLAY_MENU;
        ai.wImage = IDB_OVERLAY_MENU;
        (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
        ai.dwData++;
      }
#endif
      //Add Settings
      ai.wItemID = IDS_SETTINGS;
      ai.wImage  = IDB_PROPERTIES;
      (void)IMENUCTL_AddItemEx(pMe->m_pIPreview, &ai);
      ai.dwData++;
   }

   // Activate menu
   IMENUCTL_SetActive(pMe->m_pIPreview, TRUE);
   IMENUCTL_SetSel(pMe->m_pIPreview, pMe->wPreviewMenuSel);
   Qcamcorder_DisplayPreviewMenuName(pMe, 0);
}

/* ==========================================================================
FUNCTION QCAMCORDER_BUILDSETTINGSMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_BuildSettingsMenu(Qcamcorder *pMe)
{
   AEERect qrc;
   CtlAddItem ai;

   QCamcorder_MdpRegisterFullScreen(pMe, FALSE);

   //Set the Rectangle for the Menu
   IMENUCTL_Reset(pMe->m_pISettings);
   (void)IMENUCTL_SetTitle(pMe->m_pIMenu, QCAMCORDER_RES_FILE, IDS_SIZE, NULL);
   SETAEERECT (&qrc, 0, 160, pMe->di.cxScreen, pMe->di.cyScreen-160);
   IMENUCTL_SetRect(pMe->m_pISettings, &qrc);

   // Add Items in the Menu
   ai.pText       = NULL;
   ai.pImage      = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont       = AEE_FONT_NORMAL;
   ai.dwData      = 0;

   //Add individual entries to the Menu

   //Add size
   ai.wText   = 0;
   ai.wItemID = IDS_SIZE;
   ai.wImage  = IDB_SIZE;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add Quality
   ai.wText   = 0;
   ai.wItemID = IDS_QUALITY;
   ai.wImage  = IDB_QUALITY;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add Audio Record Format
   ai.wText   = 0;
   ai.wItemID = IDS_AUDIO_FORMAT;
   ai.wImage  = IDB_AUDIO_FORMAT;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add Start sound
   ai.wText   = 0;
   ai.wItemID = IDS_START_SOUND;
   ai.wImage  = IDB_SOUND;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add Save location
   ai.wText   = 0;
   ai.wItemID = IDS_SAVE_LOC;
   ai.wImage  = IDB_SAVE;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add # of clips
   ai.wText   = 0;
   ai.wItemID = IDS_NUMCLIPS;
   ai.wImage  = IDB_NUMCLIPS;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add Clip length
   ai.wText   = 0;
   ai.wItemID = IDS_CLIPLENGTH;
   ai.wImage  = IDB_CLIPLENGTH;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add Frame Rate
   ai.wText = 0;
   ai.wItemID = IDS_FRAME_RATE;
   ai.wImage = IDB_CLIPLENGTH;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add White Balance
   ai.wText   = 0;
   ai.wItemID = IDS_WHITEBALANCE;
   ai.wImage  = IDB_WHITEBALANCE;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add File Format
   ai.wText   = 0;
   ai.wItemID = IDS_FILE_FORMAT;
   ai.wImage  = IDB_FILE_FORMAT;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add Video Format
   ai.wText   = 0;
   ai.wItemID = IDS_VIDEO_FORMAT;
   ai.wImage  = IDB_HEADER_MENU;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

#ifdef FEATURE_MP4_TRANSCODER
  //Add Short Header
  ai.wText   = 0;
  ai.wItemID = IDS_TRANSCODER_VIDEO_TYPE;
  ai.wImage  = IDB_HEADER_MENU;
  (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
  ai.dwData++;
//Add Short Header
  ai.wText   = 0;
  ai.wItemID = IDS_TRANSCODER_TARGET_AUDIO;
  ai.wImage  = IDB_HEADER_MENU;
  (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
  ai.dwData++;

  //Add Transcode Brand
  ai.wText   = 0;
  ai.wItemID = IDS_TRANSCODE_TARGET_BRAND;
  ai.wImage  = IDB_HEADER_MENU;
  (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
  ai.dwData++;

  //Add Transcoder video format
  ai.wText   = 0;
  ai.wItemID = IDS_TRANSCODER_VIDEO_FORMAT;
  ai.wImage  = IDB_HEADER_MENU;
  (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
  ai.dwData++;
#endif

#ifdef FEATURE_DVS
  //Add DVS margin menu
  ai.wText   = 0;
  ai.wItemID = IDS_DVSMARGIN;
  ai.wImage  = IDB_HEADER_MENU;
  (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
  ai.dwData++;
#endif
  //Add Inter-Record Timer
  ai.wText =0;
  ai.wItemID = IDS_TIMER;
  ai.wImage = IDB_TIMER;
  (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
  ai.dwData++;

  //Add Rotation Angle
  ai.wText =0;
  ai.wItemID = IDS_ROTATION;
  ai.wImage = IDB_ROTATE;
  (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
  ai.dwData++;

  //Add Special Effects
  ai.wText =0;
  ai.wItemID = IDS_EFFECT;
  ai.wImage = IDB_HEADER_MENU;
  (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
  ai.dwData++;

  //Add Antibanding
  ai.wText   = 0;
  ai.wItemID = IDS_ANTIBANDING;
  ai.wImage  = IDB_HEADER_MENU;
  (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
  ai.dwData++;

#ifdef FEATURE_QCAMCORDER_TELOP
   //Add Telop
   ai.wText   = 0;
   ai.wItemID = IDS_TELOP_TEXT;
   ai.wImage  = IDB_FILE_FORMAT;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;
#endif

   //Add Continuoue Recording mode
   ai.wText = 0;
   ai.wItemID = IDS_CONTINUOUS;
   ai.wImage  = IDB_UP_DOWN;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add Special Effects menu
   ai.wText = 0;
   ai.wItemID = IDS_SPECIAL_EFFECTS;
   ai.wImage  = IDB_FRAME;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add Space limit Threshold
   ai.wText   = 0;
   ai.wItemID = IDS_SPACE;
   ai.wImage  = IDB_HEADER_MENU;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   //Add Memory usage
   ai.wText   = 0;
   ai.wItemID = IDS_MEM_USAGE;
   ai.wImage  = IDB_MEMORY;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;


   //Add Delete all
   ai.wText   = 0;
   ai.wItemID = IDS_DELETEALL;
   ai.wImage  = IDB_DELETE;
   (void)IMENUCTL_AddItemEx(pMe->m_pISettings, &ai);
   ai.dwData++;

   // Activate menu
   IMENUCTL_SetActive(pMe->m_pISettings,TRUE);

   Qcamcorder_DoSettingsMenu(pMe, 0);
   return;
} /* END Qcamcorder_BuildSettingsMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDFILEFORMATMENU
DESCRIPTION
   Builds the FileFormat menu.
============================================================================= */
static void Qcamcorder_BuildPostEditMenu(Qcamcorder *pMe)
{
   //uint16 focus  = 0;

   CtlAddItem ai;
   uint16 i;
     int id = 0;
    AECHAR szBuf[TEXT_STRING_SIZE] = {0};
    (void)IMENUCTL_DeleteAll(pMe->m_pIEditMenu);

    (void)ISHELL_LoadResString(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, (uint16)PostEditIDList[1], szBuf, sizeof(szBuf));
    (void)STR_TO_WSTR(" [", &szBuf[WSTRLEN(szBuf)], (int)(sizeof(szBuf) - (uint32)WSTRLEN(szBuf)));
         switch (pMe->eGPSState)
         {
         case QCAMCORDER_GPS_DISABLED:
            id = IDS_GPS_DISABLED;
            break;
         case QCAMCORDER_GPS_ENABLED:
            id = IDS_GPS_ENABLED;
            break;
         case QCAMCORDER_GPS_DATA_VALID:
            id = IDS_GPS_DATA_VALID;
            break;
         case QCAMCORDER_GPS_ACQUIRING:
            id = IDS_GPS_ACQUIRING;
            break;
         case QCAMCORDER_GPS_FAILURE:
            id = IDS_GPS_FAILURE;
            break;
         case QCAMCORDER_GPS_TIMEOUT:
            id = IDS_GPS_TIMEOUT;
            break;
         case QCAMCORDER_GPS_INFO_UNAVAILABLE:
            id = IDS_GPS_INFO_UNAVAILABLE;
            break;
         case QCAMCORDER_GPS_STATUS_COUNT:
           break;

         }
         (void)ISHELL_LoadResString(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, (uint16)id,
            &szBuf[WSTRLEN(szBuf)], (int)(sizeof(szBuf) - (uint32)WSTRLEN(szBuf)));
         (void)STR_TO_WSTR("]", &szBuf[WSTRLEN(szBuf)], (int)(sizeof(szBuf) - (uint32)WSTRLEN(szBuf)));

   (void)IMENUCTL_SetTitle( pMe->m_pIEditMenu, NULL, NULL, szBuf);

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 0;

   for (i = 0; i < PostEditIDList[0]; ++i)
   {
      // Add Entries
      ai.wText    = PostEditIDList[i+2];
      ai.wItemID  = PostEditIDList[i+2];
      if(ai.wItemID == IDS_GPS_EDIT)
      {ai.wImage = IDB_GPS_ACTIVE;
      }
      else if(ai.wItemID == IDS_TELOP_EDIT)
      {ai.wImage = IDB_FILE_FORMAT;
      }
      //ai.wImage   = 5005;//(uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
      (void)IMENUCTL_AddItemEx(pMe->m_pIEditMenu, &ai);
      ai.dwData++;
   }

   IMENUCTL_SetActive(pMe->m_pIEditMenu, TRUE);
   (void)IMENUCTL_Redraw(pMe->m_pIEditMenu);
   (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
   pMe->m_activeMenu = QCAMCORDER_MENU_POST_EDIT;
   IDISPLAY_Update(pMe->a.m_pIDisplay);

   return;
} /* END QCAM_BUILDFILEFORMATMENU */

/* ==========================================================================
FUNCTION Qcamcorder_BuildFileFormatSubMenu
DESCRIPTION

============================================================================= */
static void Qcamcorder_BuildFileFormatSubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected)
{
   AEERect      qrc;
   CtlAddItem   ai;
   uint16       loopCounter = 0;

   // Delete any previous menu
   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);

   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 160 - qrc.y);

   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);

   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, idList[1], NULL);

   // Add Items in the Menu
   ai.pText       = NULL;
   ai.pImage      = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont       = AEE_FONT_NORMAL;
   ai.dwData      = 0;

   for(loopCounter = 2; loopCounter < idList[0] + 2; loopCounter++)
   {
     if((idList[loopCounter] == IDS_FILE_3G2_FRAG)
	 	&& ((pMe->wQuality == QCAMCORDER_QUALITY_MAX)
	 	|| (pMe->wQuality == QCAMCORDER_QUALITY_DYNAMIC))
	    )
     {
       continue;
     }

     ai.wText    = idList[loopCounter];
     ai.wItemID  = idList[loopCounter];
     ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
     (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
     ai.dwData++;

   }

   IMENUCTL_SetSel(pMe->m_pIMenu, selected);

   return;

} /* END Qcamcorder_BuildFileFormatSubMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDFILEFORMATMENU
DESCRIPTION
   Builds the FileFormat menu.
============================================================================= */
static void Qcamcorder_BuildFileFormatMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->wFileFormat)
   {
      case QCAMCORDER_FILE_MP4:
      focus = IDS_FILE_MP4;
      break;
   case QCAMCORDER_FILE_AMC:
      focus = IDS_FILE_AMC;
      break;
   case QCAMCORDER_FILE_3GP:
      focus = IDS_FILE_3GP;
      break;
   case QCAMCORDER_FILE_3G2:
      focus = IDS_FILE_3G2;
      break;
   case QCAMCORDER_FILE_3G2_FRAG:
      focus = IDS_FILE_3G2_FRAG;
      break;
   case QCAMCORDER_FILE_K3G:
      focus = IDS_FILE_K3G;
      break;
   case QCAMCORDER_FILE_SKM:
       focus = IDS_FILE_SKM;
       break;
     default:
       focus = IDS_FILE_MP4;
       break;
   }

   /* 3g2 frag is not supported for MAX & DYNAMIC */
   if ((focus == IDS_FILE_3G2_FRAG)
        && ((pMe->wQuality == QCAMCORDER_QUALITY_MAX)
	 	|| (pMe->wQuality == QCAMCORDER_QUALITY_DYNAMIC))
      )
     focus = IDS_FILE_3GP;

   Qcamcorder_BuildFileFormatSubMenu(pMe, FileFormatIDList, focus);

   return;
} /* END QCAM_BUILDFILEFORMATMENU */



/* ==========================================================================
FUNCTION QCAM_BUILDFVIDEOFORMATSUBMENU
DESCRIPTION
   Builds the Short Header submenu.
============================================================================= */
static void Qcamcorder_BuildVideoFormatSubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected)
{
   AEERect      qrc;
   CtlAddItem   ai;
   uint16       loopCounter = 0;

   // Delete any previous menu
   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);

   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 160 - qrc.y);

   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);

   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, idList[1], NULL);

   // Add Items in the Menu
   ai.pText       = NULL;
   ai.pImage      = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont       = AEE_FONT_NORMAL;
   ai.dwData      = 0;

   for(loopCounter = 2; loopCounter < idList[0] + 2; loopCounter++)
   {
     if(idList[loopCounter] == IDS_FORMAT_H263_SAMPLE && 
        !(pMe->wFileFormat == QCAMCORDER_FILE_3G2 || pMe->wFileFormat == QCAMCORDER_FILE_3G2_FRAG))
     {
       continue;
     }

     #ifdef FEATURE_H264_ENCODER
     if(idList[loopCounter] == IDS_FORMAT_H264 && pMe->wFileFormat == QCAMCORDER_FILE_AMC)
     {
       continue;
     }
     #endif // FEATURE_H264_ENCODER

     /* Do not display the video format none buttom when audio format is none*/
     if(idList[loopCounter] == IDS_NONE && (pMe->wAudioFormat == QCAMCORDER_AUDIO_FORMAT_NONE 
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
        ))
     {
       continue;
     }

     ai.wText    = idList[loopCounter];
     ai.wItemID  = idList[loopCounter];
     ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
     (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
     ai.dwData++;

   }

   IMENUCTL_SetSel(pMe->m_pIMenu, selected);

   return;

} /* END Qcamcorder_BuildVideoFormatSubMenu */



/* ==========================================================================
FUNCTION QCAM_BUILDVIDEOFORMATMENU
DESCRIPTION
   Builds the VideoFormat menu.
============================================================================= */
static void Qcamcorder_BuildVideoFormatMenu(Qcamcorder *pMe)
{
   uint16 focus = 0;

   switch (pMe->wVideoFormat)
   {
     case QCAMCORDER_VIDEO_FORMAT_MPEG4:
       focus = IDS_FORMAT_MPEG4;
       break;

     case QCAMCORDER_VIDEO_FORMAT_H263:
       focus = IDS_FORMAT_H263;
       break;

     case QCAMCORDER_VIDEO_FORMAT_H263_SAMPLE:
       focus = IDS_FORMAT_H263_SAMPLE;
       break;

#ifdef FEATURE_H264_ENCODER
     case QCAMCORDER_VIDEO_FORMAT_H264:
       focus = IDS_FORMAT_H264;
       break;
#endif /* FEATURE_H264_ENCODER */

   case QCAMCORDER_VIDEO_FORMAT_NONE:
     focus = IDS_NONE;
     break;

     default:
       focus = IDS_FORMAT_MPEG4;
       break;
   }

   Qcamcorder_BuildVideoFormatSubMenu(pMe, VideoFormatIDList, focus);

   return;
} /* END QCam_BuildVideoFormatMenu */


/* ==========================================================================
FUNCTION QCAMCORDER_BUILDEDITMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_BuildEditMenu(Qcamcorder *pMe)
{
   AEERect qrc;
   CtlAddItem ai;

   if (pMe->m_activeSoftMenu != QCAMCORDER_MENU_EDIT)
   {
     QCamcorder_MdpRegisterFullScreen(pMe, FALSE);
   }

   //Set the Rectangle for the Menu
   IMENUCTL_Reset(pMe->m_pISoftMenu);
   SETAEERECT (&qrc,
               0,
                     pMe->di.cyScreen - QCAMCORDER_SOFT_MENU_HEIGHT,
                      pMe->di.cxScreen,
                     QCAMCORDER_SOFT_MENU_HEIGHT);
   IMENUCTL_SetRect(pMe->m_pISoftMenu, &qrc);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Add Items in the Menu
   ai.pText       = NULL;
   ai.pImage      = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont       = AEE_FONT_NORMAL;
   ai.dwData      = 0;

   //Add individual entries to the Menu

   switch (pMe->eState)
   {
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
    case QCAMCORDER_CUT_MERGE:
//Add Stop
      ai.wText   = 0;
      ai.wItemID = IDS_STOP;
      ai.wImage  = IDB_STOP;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;

      if(pMe->m_bEditHit)
      {
        ai.wText   = 0;
        ai.wItemID = IDS_CUT_MERGE;
        ai.wImage  = IDB_VIDEO;
        (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
        ai.dwData++;
        IMENUCTL_SetSel(pMe->m_pISoftMenu, IDS_CUT_MERGE);
      }

      break;
#endif /* FEATURE_QCAMCORDER_CUT_N_MERGE */
#ifdef FEATURE_MP4_TRANSCODER
   case QCAMCORDER_TRANSPORTION:
//Add Stop
      ai.wText   = 0;
      ai.wItemID = IDS_STOP;
      ai.wImage  = IDB_STOP;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;

      if(pMe->m_bEditHit)
      {
        ai.wText   = 0;
        ai.wItemID = IDS_TRANSPORTION;
        ai.wImage  = IDB_DOWN;
        (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
        ai.dwData++;
        IMENUCTL_SetSel(pMe->m_pISoftMenu, IDS_TRANSPORTION);
      }
      break;
#endif
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
   case QCAMCORDER_AUDIODUB:
      ai.wText   = 0;
      ai.wItemID = IDS_STOP;
      ai.wImage  = IDB_STOP;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;
      break;
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */

   case QCAMCORDER_PLAYING:
      //Add Stop
      ai.wText   = 0;
      ai.wItemID = IDS_STOP;
      ai.wImage  = IDB_STOP;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;

      //Add Pause
      ai.wText   = 0;
      ai.wItemID = IDS_PAUSE;
      ai.wImage  = IDB_PAUSE;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;

#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
      ai.wText   = 0;
      ai.wItemID = IDS_START_EDITING;
      ai.wImage  = IDB_START_EDITING;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;
#endif /* FEATURE_QCAMCORDER_VIDEO_EDITING */

#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
      ai.wText   = 0;
      ai.wItemID = IDS_CUT_MERGE;
      ai.wImage  = IDB_VIDEO;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;
#endif /* FEATURE_QCAMCORDER_CUT_N_MERGE */
#ifdef FEATURE_MP4_TRANSCODER
//Transcode portion of clip
      ai.wText   = 0;
      ai.wItemID = IDS_TRANSPORTION;
      ai.wImage  = IDB_DOWN;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;
#endif
      break;
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
     case QCAMCORDER_VIDEO_EDIT_SELECTION:
       //Add Stop
      ai.wText   = 0;
      ai.wItemID = IDS_STOP;
      ai.wImage  = IDB_STOP;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;

      //Add Start Edit button
      ai.wText   = 0;
      ai.wItemID = IDS_START_EDITING;
      ai.wImage  = IDB_START_EDITING;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;
      IMENUCTL_SetSel(pMe->m_pISoftMenu, IDS_START_EDITING);
      break;
#endif /* FEATURE_QCAMCORDER_VIDEO_EDITING */

     case QCAMCORDER_PAUSED:
      //Add Stop
      ai.wText   = 0;
      ai.wItemID = IDS_STOP;
      ai.wImage  = IDB_STOP;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;

      //Add Resume button
      ai.wText   = 0;
      ai.wItemID = IDS_PAUSE;
      ai.wImage  = IDB_PLAY;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;

      IMENUCTL_SetSel(pMe->m_pISoftMenu, IDS_PAUSE);
      break;

#ifdef FEATURE_MP4_TRANSCODER
     case QCAMCORDER_TRANSCODE:
       //Add Stop
       ai.wText   = 0;
       ai.wItemID = IDS_STOP;
       ai.wImage  = IDB_STOP;
       (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
       ai.dwData++;
       break;
#endif /* FEATURE_MP4_TRANSCODER */

#ifdef FEATURE_VIDEO_ENCODE_RAW
   case QCAMCORDER_START_RAW_ENCODE:
      // Add Start Record button
      ai.wText   = 0;
      ai.wItemID = IDS_RAW_RECORD;
      ai.wImage  = IDB_RECORD;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;
      break;
   case QCAMCORDER_RAW_ENCODING:
      // Add Stop Record button
      ai.wText   = 0;
      ai.wItemID = IDS_RAW_STOP;
      ai.wImage  = IDB_STOP;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;
      break;
#endif /* FEATURE_VIDEO_ENCODE_RAW */
#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif//#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
   default:
      //Add play
      ai.wText   = 0;
      ai.wItemID = IDS_PLAY;
      ai.wImage  = IDB_PLAY;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;

#ifdef FEATURE_VIDEO_AUDIO_DUBBING
      //Add audio dubbing
      ai.wText   = 0;
      ai.wItemID = IDS_RECORD;
      ai.wImage  = IDB_RECORD;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */

#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif //#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE

#if defined(FEATURE_BREW_IMOVIEEDIT) || defined(FEATURE_BREW_IMOVIEFILE)
      //Add save
      ai.wText   = 0;
      ai.wItemID = IDS_SAVE;
      ai.wImage  = IDB_SAVE;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;
#endif //#ifdef FEATURE_BREW_IMOVIEEDIT

      //Add properties
      ai.wText   = 0;
      ai.wItemID = IDS_PROPERTIES;
      ai.wImage  = IDB_PROPERTIES;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;

#ifdef FEATURE_APP_MMS
#error code not present
#endif /* FEATURE_APP_MMS */

#ifdef FEATURE_MP4_TRANSCODER
     ai.wText = 0;
     ai.wItemID = IDS_TRANSCODE;
     ai.wImage = IDB_DOWN;
     (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
     ai.dwData++;
#endif /* FEATURE_MP4_TRANSCODER */
#ifdef FEATURE_DVS
     ai.wText = 0;
     ai.wItemID = IDS_DVS;
     ai.wImage = IDB_DVS;
     (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
     ai.dwData++;
#endif
      //Add Post Edit
#if defined(FEATURE_BREW_IMOVIEEDIT) || defined(FEATURE_BREW_IMOVIEFILE)
      ai.wText   = 0;
      ai.wItemID = IDS_POST_EDIT;
      ai.wImage  = IDB_POST_EDIT;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;
#endif //defined(FEATURE_BREW_IMOVIEEDIT) || defined(FEATURE_BREW_IMOVIEFILE)

      //Add Delete
      ai.wText   = 0;
      ai.wItemID = IDS_QCAMCORDER_DELETE;
      ai.wImage  = IDB_DELETE;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;

      ai.wText   = 0;
      ai.wItemID = IDS_PLAY_ALL;
      ai.wImage  = IDB_PLAY_ALL;
      (void)IMENUCTL_AddItemEx(pMe->m_pISoftMenu, &ai);
      ai.dwData++;
   }

   // Activate menu
   IMENUCTL_SetActive(pMe->m_pISoftMenu, TRUE);

   if(pMe->eState != QCAMCORDER_PLAYING
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
      && pMe->eState != QCAMCORDER_CUT_MERGE
      && pMe->eState != QCAMCORDER_VIDEO_EDIT_SELECTION
#endif /* FEATURE_QCAMCORDER_CUT_N_MERGE */
#ifdef FEATURE_VIDEO_ENCODE_RAW
      && pMe->eState != QCAMCORDER_RAW_ENCODING
      && pMe->eState != QCAMCORDER_PAUSED
#endif /* FEATURE_VIDEO_ENCODE_RAW */
#ifdef FEATURE_MP4_TRANSCODER
      && pMe->eState != QCAMCORDER_TRANSPORTION
#endif
     )
   {
    Qcamcorder_DisplayEditMenuName(pMe);
   }
   pMe->m_activeSoftMenu = QCAMCORDER_MENU_EDIT;

   return;
}

/* ==========================================================================
FUNCTION QCAMCORDER_BUILDGPSMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_BuildGPSMenu(Qcamcorder *pMe, boolean PostEdit)
{
   AEERect qrc;
   CtlAddItem ai;
   IMenuCtl *curMenu;

   QCamcorder_MdpRegisterFullScreen(pMe, FALSE);

   if(PostEdit)
   {curMenu = pMe->m_pIEditMenu;
   }
   else
   {curMenu = pMe->m_pIMenu;
   }

   IMENUCTL_Reset(curMenu);
   (void)IMENUCTL_DeleteAll(curMenu);
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen - qrc.y);
   IMENUCTL_SetRect(curMenu, &qrc);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( curMenu, QCAMCORDER_RES_FILE, IDS_GPS_SETUP, NULL);

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 0;

   //Add individual entries to the Menu

   // Add Disable GPS
   ai.wText    = IDS_GPS_DISABLE;
   ai.wItemID  = IDS_GPS_DISABLE;
   ai.wImage   = IDB_GPS_INACTIVE;
   ai.dwData++;
   (void)IMENUCTL_AddItemEx(curMenu, &ai);

   // Add Enable GPS - Get New
   ai.wText    = IDS_GPS_ENABLE_NEW;
   ai.wItemID  = IDS_GPS_ENABLE_NEW;
   ai.wImage   = IDB_GPS_ACTIVE;
   ai.dwData++;
   (void)IMENUCTL_AddItemEx(curMenu, &ai);

   // Add Enable GPS - Get New
   ai.wText    = IDS_GPS_ENABLE_PREV;
   ai.wItemID  = IDS_GPS_ENABLE_PREV;
   ai.wImage   = IDB_GPS_ACTIVE;
   ai.dwData++;
   (void)IMENUCTL_AddItemEx(curMenu, &ai);

   pMe->m_activeMenu = QCAMCORDER_MENU_GPS;
   IMENUCTL_SetActive(curMenu, TRUE);
   return;
} /* END QCam_BuildGPSMenu */

/* ==========================================================================
FUNCTION QCAMCORDER_BUILDSOUNDMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_BuildSoundMenu(Qcamcorder *pMe)
{
   AEERect qrc;
   CtlAddItem ai;
   uint16 focus = 0;
   FileInfo fi;
   int count = 1;
   AECHAR szBuf[FILE_NAME_SIZE];
   IFileMgr * pIFileMgr;
   char* pfileExt;
   uint16 max_file_count = 0;

   IMENUCTL_Reset(pMe->m_pIMenu);
   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 160 - qrc.y);
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   if (IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, IDS_START_SOUND, NULL)==FALSE)
      return;

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 1;

   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr) != SUCCESS)
      return;

   (void)IFILEMGR_EnumInit(pIFileMgr, QCAMCORDER_SOUNDS_DIR, FALSE);
   while (IFILEMGR_EnumNext(pIFileMgr, &fi))
   {
     QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);
     /* camcorder only supports .wav file for shutter sound playback */
     pfileExt=QCamcorder_GetFileNameExtension(fi.szName);
     if (pfileExt && STRCMP(pfileExt, "wav") == 0)
     {
        //Add individual entries to the Menu
        (void)STR_TO_WSTR(&fi.szName[QCAMCORDER_SOUNDS_DIR_LEN], szBuf, sizeof(szBuf));
        ai.pText    = szBuf;
        ai.wItemID  = (uint16)count++;
        if (STRCMP(fi.szName, pMe->sStartSoundFile) == 0)
        {
           focus = ai.wItemID;
           ai.wImage   = IDB_RADIO_ON;
        }
        else
        {
           ai.wImage   = IDB_RADIO_OFF;
        }
        (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
        ai.dwData++;
     }
   }

   /* Add "No Sound" menu */
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.wText = IDS_NO_SOUND;
   if (focus == 0)
   {
     focus = (uint16) IDS_NO_SOUND;
     ai.wImage = IDB_RADIO_ON;
   }
   else
     ai.wImage = IDB_RADIO_OFF;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData++;
   ai.wItemID  = (uint16) IDS_NO_SOUND;
   (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);

   (void)IFILEMGR_Release(pIFileMgr);
   pIFileMgr = NULL;
   IMENUCTL_SetSel(pMe->m_pIMenu, focus);
   pMe->m_activeMenu = QCAMCORDER_MENU_SOUND;
   IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
   return;
} /* END Qcamcorder_BuildSoundMenu */
/* ==========================================================================
FUNCTION QCAM_BUILDFILESIZEUBMENU
DESCRIPTION
   Builds the Size submenu.
============================================================================= */
static void Qcamcorder_BuildSizeSubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected)
{
   AEERect qrc;
   CtlAddItem ai;
   uint16 i=0;

   // Delete any previous menu
   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 160 - qrc.y);
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, idList[1], NULL);

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 0;

   if(pMe->wFileFormat != QCAMCORDER_FILE_AMC)
   {
#ifndef FEATURE_QVGANOTSUPPORTED
     /* QVGA Size */
     ai.wText    = idList[i+2];
     ai.wItemID  = idList[i+2];
     ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
     (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
     ai.dwData++;
#endif //#ifndef FEATURE_QVGANOTSUPPORTED

     /* QCIF Size */
     ai.wText    = idList[i+3];
     ai.wItemID  = idList[i+3];
     ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
     (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
     ai.dwData++;
   }

   /* sQCIF Size */
   ai.wText    = idList[i+4];
   ai.wItemID  = idList[i+4];
   ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
   (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
   ai.dwData++;

   /* MM frame size not supported by SH */
   if(pMe->wShortHeader == QCAMCORDER_SH_DISABLE)
   {
    ai.wText    = idList[i+5];
    ai.wItemID  = idList[i+5];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
   }

   ai.wText    = idList[i+6];
   ai.wItemID  = idList[i+6];
   if(pMe->m_displaySize == QCAMCORDER_DISPLAY_FULL_SCREEN)
     ai.wImage = (uint16)IDB_CHECK_ON;
   else
     ai.wImage = (uint16)IDB_CHECK_OFF;

   (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
   ai.dwData++;

   IMENUCTL_SetSel(pMe->m_pIMenu, selected);
   return;
} /* END QCam_BuildSizeSubMenu */
/* ==========================================================================
FUNCTION Qcamcorder_BuildSizeMenu
DESCRIPTION
   Builds the Size menu.
============================================================================= */
static void Qcamcorder_BuildSizeMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->eSize)
   {
   case QCAMCORDER_SIZE_QCIF:
      focus = IDS_SIZE_QCIF;
      break;
   case QCAMCORDER_SIZE_SQCIF:
      focus = IDS_SIZE_SQCIF;
      break;
#ifndef FEATURE_QVGANOTSUPPORTED
   case QCAMCORDER_SIZE_QVGA:
       focus = IDS_SIZE_QVGA;
       break;
#endif //#ifndef FEATURE_QVGANOTSUPPORTED
   case QCAMCORDER_SIZE_DISPLAY:
      focus = IDS_SIZE_DISPLAY;
      break;
   case QCAMCORDER_SIZE_MM:
   default:
      focus = IDS_SIZE_MM;
      break;
   }

   /* Make sure frame sizes are supported by short header */
   if((pMe->wShortHeader != QCAMCORDER_SH_DISABLE)
       && (pMe->eSize == QCAMCORDER_SIZE_MM))
   {
     pMe->eSize = QCAMCORDER_SIZE_SQCIF;
     focus = IDS_SIZE_SQCIF;
   }
   Qcamcorder_BuildSizeSubMenu(pMe, SizeIDList, focus);

   return;
} /* END QCam_BuildSizeMenu */
/* ==========================================================================
FUNCTION Qcamcorder_BuildAudioFormatSubMenu
DESCRIPTION
   Builds a generic menu.
============================================================================= */
static void Qcamcorder_BuildAudioFormatSubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected)
{
   AEERect qrc;
   CtlAddItem ai;
   uint16 i=0;
   uint16 j=2;

   // Delete any previous menu
   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 160 - qrc.y);
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, idList[1], NULL);

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 0;

   //13K Full
   if(pMe->wFileFormat == QCAMCORDER_FILE_MP4 ||
      pMe->wFileFormat == QCAMCORDER_FILE_3G2 ||
      pMe->wFileFormat == QCAMCORDER_FILE_3G2_FRAG ||
      pMe->wFileFormat == QCAMCORDER_FILE_AMC ||
      pMe->wFileFormat == QCAMCORDER_FILE_SKM ||
      pMe->wFileFormat == QCAMCORDER_FILE_K3G)
   {
    ai.wText    = idList[i+j];
    ai.wItemID  = idList[i+j];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
   }
   j++;

   //13K half
   if(pMe->wFileFormat == QCAMCORDER_FILE_MP4 ||
      pMe->wFileFormat == QCAMCORDER_FILE_3G2 ||
      pMe->wFileFormat == QCAMCORDER_FILE_3G2_FRAG ||
      pMe->wFileFormat == QCAMCORDER_FILE_AMC ||
      pMe->wFileFormat == QCAMCORDER_FILE_SKM ||
      pMe->wFileFormat == QCAMCORDER_FILE_K3G)
   {
    ai.wText    = idList[i+j];
    ai.wItemID  = idList[i+j];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
   }
   j++;

  //EVRC Full
   if(pMe->wFileFormat == QCAMCORDER_FILE_3G2 ||
      pMe->wFileFormat == QCAMCORDER_FILE_3G2_FRAG ||
      pMe->wFileFormat == QCAMCORDER_FILE_SKM ||
      pMe->wFileFormat == QCAMCORDER_FILE_K3G)
   {
    ai.wText    = idList[i+j];
    ai.wItemID  = idList[i+j];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
   }
   j++;

#ifdef FEATURE_CAMCORDER_AAC_REC
   //AAC
   if(pMe->wFileFormat == QCAMCORDER_FILE_MP4 ||
      pMe->wFileFormat == QCAMCORDER_FILE_3G2 ||
      pMe->wFileFormat == QCAMCORDER_FILE_3G2_FRAG ||
      pMe->wFileFormat == QCAMCORDER_FILE_3GP ||
      pMe->wFileFormat == QCAMCORDER_FILE_AMC ||
      pMe->wFileFormat == QCAMCORDER_FILE_SKM ||
      pMe->wFileFormat == QCAMCORDER_FILE_K3G)
   {
    ai.wText    = idList[i+j];
    ai.wItemID  = idList[i+j];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
   }
   j++;
#endif /* FEATURE_CAMCORDER_AAC_REC */

   //AMR 12.2
   if(pMe->wFileFormat == QCAMCORDER_FILE_3G2 ||
      pMe->wFileFormat == QCAMCORDER_FILE_3G2_FRAG ||
      pMe->wFileFormat == QCAMCORDER_FILE_3GP ||
      pMe->wFileFormat == QCAMCORDER_FILE_SKM ||
      pMe->wFileFormat == QCAMCORDER_FILE_K3G)
   {
    ai.wText    = idList[i+j];
    ai.wItemID  = idList[i+j];
    ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
   }
   j++;

  //None
   /* Do not display the Audio format none radio button when, Video format is none*/
   if(pMe->wVideoFormat != QCAMCORDER_VIDEO_FORMAT_NONE 
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
      )
   {
     ai.wText    = idList[i+j];
     ai.wItemID  = idList[i+j];
     ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
     (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
     ai.dwData++;
   }

   IMENUCTL_SetSel(pMe->m_pIMenu, selected);
   return;
} /* END QCam_BuildSubMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDAUDIOFORMATMENU
DESCRIPTION
   Builds the Audio Format menu.
============================================================================= */
static void Qcamcorder_BuildAudioFormatMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->wAudioFormat)
   {
   case QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K:
      focus = IDS_AUDIO_FORMAT_13K_FULL;
      break;
   case QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K:
      focus = IDS_AUDIO_FORMAT_13K_HALF;
      break;
   case QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_EVRC:
      focus = IDS_AUDIO_FORMAT_EVRC;
      break;
   case QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR:
      focus = IDS_AUDIO_FORMAT_AMR;
      break;
#ifdef FEATURE_CAMCORDER_AAC_REC
     case QCAMCORDER_AUDIO_FORMAT_VAR_AAC:
       focus = IDS_AUDIO_FORMAT_AAC;
       break;
#endif /* FEATURE_CAMCORDER_AAC_REC */
   case QCAMCORDER_AUDIO_FORMAT_NONE:
   default:
      focus = IDS_NONE;
      break;
   }

   Qcamcorder_BuildAudioFormatSubMenu(pMe, AudioFormatIDList, focus);

   return;
} /* END QCam_BuildAudioFormatMenu */
/**************************************************************************
QAMCORDER_BUILDSAVELOCSUBMENU
***************************************************************************/
static void Qcamcorder_BuildSaveLocSubMenu(Qcamcorder *pMe,  uint16 selected)
{  int nReturnStatus = EFAILED, i;
   AEERect qrc;
   CtlAddItem ai;
   boolean bAdd[4] = {TRUE, FALSE, FALSE, FALSE};

   // Delete any previous menu
   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 160 - qrc.y);
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, SaveLocIDList[1], NULL);

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 1;

   for(i = 1; i < ARR_SIZE(AEEDirList); i++)
   {
     // Check for the storage medium
     if(IFILEMGR_Test(pMe->pIFileMgr, AEEDirList[i])==SUCCESS)
     {
        bAdd[i] = TRUE;
     }
     else
     {
        nReturnStatus = IFILEMGR_MkDir(pMe->pIFileMgr, AEEDirList[i]);

       if ((nReturnStatus == SUCCESS) &&
           (IFILEMGR_Test(pMe->pIFileMgr, AEEDirList[i]) == SUCCESS))
       {
         bAdd[i] = TRUE;
       }
       else
       {
         if(selected == SaveLocIDList[i+2])
         {
           pMe->saveLoc = QCAMCORDER_SAVE_FLASH;
           selected = IDS_FLASH;
         }
       }
     }
   }

   for(i = 0; i < ARR_SIZE(AEEDirList); i++)
   {
     if(bAdd[i])
     {
       ai.wText    = SaveLocIDList[i+2];
       ai.wItemID  = SaveLocIDList[i+2];
       ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
       (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
       ai.dwData++;
     }
   }

   IMENUCTL_SetSel(pMe->m_pIMenu, selected);
   return;
} /* END QCam_BuildSubMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDSAVELOCMENU
DESCRIPTION
   Builds the Save Location menu.
============================================================================= */
static void Qcamcorder_BuildSaveLocMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   focus = SaveLocIDList[pMe->saveLoc + 2];
   Qcamcorder_BuildSaveLocSubMenu(pMe,  focus);

   return;
} /* END QCam_BuildSaveLocMenu */

/* ==========================================================================
FUNCTION Qcamcorder_BuildQualitySubMenu
DESCRIPTION

============================================================================= */
static void Qcamcorder_BuildQualitySubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected)
{
   AEERect      qrc;
   CtlAddItem   ai;
   uint16       loopCounter = 0;

   // Delete any previous menu
   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);

   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 160 - qrc.y);

   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);

   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, idList[1], NULL);

   // Add Items in the Menu
   ai.pText       = NULL;
   ai.pImage      = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont       = AEE_FONT_NORMAL;
   ai.dwData      = 0;

   for(loopCounter = 2; loopCounter < idList[0] + 2; loopCounter++)
   {
     /* MAX & Dynamic are not supported for 3g2 Frag & AMC file format */
     if((pMe->wFileFormat == QCAMCORDER_FILE_3G2_FRAG || pMe->wFileFormat == QCAMCORDER_FILE_AMC)
	 	&& ((idList[loopCounter] == IDS_QUALITY_DYNAMIC)
	 	|| (idList[loopCounter] == IDS_QUALITY_MAX))
	   )
     {
       continue;
     }

     ai.wText    = idList[loopCounter];
     ai.wItemID  = idList[loopCounter];
     ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
     (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
     ai.dwData++;

   }

   IMENUCTL_SetSel(pMe->m_pIMenu, selected);

   return;

} /* END Qcamcorder_BuildQualitySubMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDQUALITYMENU
DESCRIPTION
   Builds the Quality menu.
============================================================================= */
static void Qcamcorder_BuildQualityMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->wQuality)
   {
   case QCAMCORDER_QUALITY_LOW:
      focus = IDS_QUALITY_LOW;
      break;
   case QCAMCORDER_QUALITY_MED:
      focus = IDS_QUALITY_MED;
      break;
   case QCAMCORDER_QUALITY_HIGH:
      focus = IDS_QUALITY_HIGH;
      break;
#ifdef FEATURE_VIDEO_ENCODE_BITRATE
   case QCAMCORDER_QUALITY_USER:
      focus = IDS_QUALITY_USER;
      break;
#endif
   case QCAMCORDER_QUALITY_MAX:
      focus = IDS_QUALITY_MAX;
      break;
   case QCAMCORDER_QUALITY_DYNAMIC:
      focus = IDS_QUALITY_DYNAMIC;
      break;
   default:
       focus = IDS_QUALITY_LOW;
     break;
   }

   /* MAX & Dynamic are not supported for 3g2 Frag & AMC file format */
   if((pMe->wFileFormat == QCAMCORDER_FILE_3G2_FRAG || pMe->wFileFormat == QCAMCORDER_FILE_AMC)
      && ((focus == IDS_QUALITY_MAX) || (focus == IDS_QUALITY_DYNAMIC))
     )
   	 focus = IDS_QUALITY_HIGH;

   Qcamcorder_BuildQualitySubMenu(pMe, QualityIDList, focus);

   return;
} /* END QCam_QualityMenu */


/* ==========================================================================
FUNCTION QCAM_BUILDNUMCLIPSMENU
DESCRIPTION
   Builds the Size menu.
============================================================================= */
static void Qcamcorder_BuildNumClipsMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->m_maxNumClips)
   {
   case QCAMCORDER_NUMCLIPS_LOW:
      focus = IDS_NUMCLIPS_LOW;
      break;
   case QCAMCORDER_NUMCLIPS_MED:
      focus = IDS_NUMCLIPS_MED;
      break;
   case QCAMCORDER_NUMCLIPS_HIGH:
      focus = IDS_NUMCLIPS_HIGH;
      break;
   case QCAMCORDER_NUMCLIPS_NONE:
   default:
      focus = IDS_NO_LIMIT;
      break;
   }

   Qcamcorder_BuildSubMenu(pMe, NumClipsIDList, focus);

   return;
} /* END QCam_BuildNumClipsMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDCLIPLENGTHMENU
DESCRIPTION
   Builds the Size menu.
============================================================================= */
static void Qcamcorder_BuildClipLengthMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   if (pMe->wFileFormat == QCAMCORDER_FILE_AMC)
   {
      pMe->m_maxClipLength = QCAMCORDER_CLIPLENGTH_LOW;
      ClipLengthIDList[0] = 1;
   }
   else
   {
      ClipLengthIDList[0] = 4;
   }
   switch (pMe->m_maxClipLength)
   {
   case QCAMCORDER_CLIPLENGTH_LOW:
      focus = IDS_CLIPLENGTH_LOW;
      break;
   case QCAMCORDER_CLIPLENGTH_MED:
      focus = IDS_CLIPLENGTH_MED;
      break;
   case QCAMCORDER_CLIPLENGTH_HIGH:
      focus = IDS_CLIPLENGTH_HIGH;
      break;
   case QCAMCORDER_CLIPLENGTH_NONE:
   default:
      focus = IDS_NO_LIMIT;
      break;
   }

   Qcamcorder_BuildSubMenu(pMe, ClipLengthIDList, focus);

   return;
} /* END QCam_BuildClipLengthMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDCLIPLENGTHMENU
DESCRIPTION
   Builds the Size menu.
============================================================================= */
static void Qcamcorder_BuildFrameRateMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   if (pMe->wFileFormat == QCAMCORDER_FILE_AMC && pMe->eSize == QCAMCORDER_SIZE_MM
#ifdef FEATURE_QVGA_LOW_FPS_ONLY
       || pMe->eSize == QCAMCORDER_SIZE_QVGA
#endif /* FEATURE_QVGA_LOW_FPS_ONLY */
       )
   {
     FrameRateIDList[0] = 1;
     pMe->m_FrameRate = QCAMCORDER_FRAME_RATE_1;
   }
   else
     FrameRateIDList[0] = 3;

   switch (pMe->m_FrameRate)
   {
   case QCAMCORDER_FRAME_RATE_1:
      focus = IDS_FRAME_RATE_1;
      break;
   case QCAMCORDER_FRAME_RATE_2:
      focus = IDS_FRAME_RATE_2;
      break;
   case QCAMCORDER_FRAME_RATE_3:
      focus = IDS_FRAME_RATE_3;
      break;
     default:
       focus = IDS_FRAME_RATE_1;
       break;
   }

   Qcamcorder_BuildSubMenu(pMe, FrameRateIDList, focus);

   return;
} /* END QCam_BuildClipLengthMenu */
/* ==========================================================================
FUNCTION QCAMCORDER_BUILDWHITEBALANCEMENU
DESCRIPTION
   Builds the WhiteBalance menu.
============================================================================= */
static void Qcamcorder_BuildWhiteBalanceMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->m_nwhiteBalance)
   {
   case CAM_WB_AUTO:
      focus = IDS_AUTO;
      break;
   case CAM_WB_FLUORESCENT:
      focus = IDS_FLUORESCENT;
      break;
   case CAM_WB_INCANDESCENT:
      focus = IDS_INCANDESCENT;
      break;
   case CAM_WB_DAYLIGHT:
      focus = IDS_DAYLIGHT;
      break;
   default:
      focus = IDS_AUTO;
      break;
   }

   Qcamcorder_BuildSubMenu(pMe, WhiteBalanceIDList, focus);

   return;
} /* END QCam_BuildClipLengthMenu */
/* ==========================================================================
FUNCTION QCAM_BUILDDELETEALLMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_BuildDeleteAllMenu(Qcamcorder *pMe)
{
   AEERect qrc;
   CtlAddItem ai;
   int i;
   /* to find the list of devices available */
   boolean foundDevice[5] = {TRUE, FALSE, FALSE, FALSE,FALSE};

   if(Qcamcorder_GetFilecount(pMe) >0)
   {Qcamcorder_DrawIcon(pMe, IDB_QCAMCORDER_YES);

   }
   else
   {Qcamcorder_DrawIcon(pMe, IDB_QCAMCORDER_NO);
   }

   IMENUCTL_Reset(pMe->m_pIMenu);
   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 80, pMe->di.cxScreen, 200 - qrc.y);
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, DeleteallIDList[1], NULL);

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 0;

   /*Check the availability of storage media */
   for(i = 1; i < (int)ARR_SIZE(AEEDirList); i++)
   {
     if(IFILEMGR_Test(pMe->pIFileMgr, AEEDirList[i]) == SUCCESS)
     {
       foundDevice[i] = TRUE;
     }
   }

   //Add individual entries to the Menu
   for (i = 0; i < DeleteallIDList[0]; ++i)
   {
     boolean addEntry = FALSE;
     switch(DeleteallIDList[i+2])
     {
     case IDS_FLASH_DELETEALL :
         //lint -e415
         if(foundDevice[QCAMCORDER_SAVE_MMC]
#ifdef FEATURE_EFS_FTL
            || foundDevice[QCAMCORDER_SAVE_FTL] 
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
            || foundDevice[QCAMCORDER_SAVE_USB]
#endif
            )
         //lint +e415
           addEntry = TRUE;
         break;
       case IDS_MMC_DELETEALL   :
          if(foundDevice[QCAMCORDER_SAVE_MMC])
            addEntry = TRUE;
          break;
#ifdef FEATURE_EFS_FTL
       case IDS_FTL_DELETEALL   :
         if(foundDevice[QCAMCORDER_SAVE_FTL])
           addEntry = TRUE;
         break;
#endif /* FEATURE_EFS_FTL */
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
       case IDS_USB_DELETEALL   :
         if(foundDevice[QCAMCORDER_SAVE_USB])
           addEntry = TRUE;
             break;
#endif /* FEATURE_EFS_HOTPLUG_USB_HOST_MS */
       case IDS_QCAMCORDER_YES  :
       case IDS_QCAMCORDER_NO   :
         addEntry = TRUE;
        break;
       default:
        break;
     }
     if(addEntry)
     {
       ai.wText    = DeleteallIDList[i+2];
       ai.wItemID  = DeleteallIDList[i+2];
       ai.wImage   = (uint16)(IDS_QCAMCORDER_NO == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
       (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
       ai.dwData++;
     }
   }


   IMENUCTL_SetSel(pMe->m_pIMenu, IDS_QCAMCORDER_NO);
   IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
   return;
} /* END QCam_BuildDeleteAllMenu */


#ifdef FEATURE_CAMCORDER_AAC_REC
/* ==========================================================================
FUNCTION QCAMCORDER_BUILDAACCONFIGMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_BuildAACConfigMenu(Qcamcorder *pMe)
{
  uint16 focus = 0;

  switch (pMe->m_AACConfig)
  {
    case QCAMCORDER_AAC_CONFIG_FREQ:
      focus = IDS_AAC_CONFIG_FREQ;
      break;
    case QCAMCORDER_AAC_CONFIG_BITRATE:
      focus = IDS_AAC_CONFIG_RATE;
      break;
    case QCAMCORDER_AAC_CONFIG_MODE:
    default:
      focus = IDS_AAC_CONFIG_MODE;
      break;
  }

  Qcamcorder_BuildSubMenu(pMe, AACConfigIDList, focus);

}


/* ==========================================================================
FUNCTION QCAMCORDER_BUILDAACMODEMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_BuildAACModeMenu(Qcamcorder *pMe)
{
  uint16 focus = 0;

  switch ((QcamcorderAACModeType)((pMe->m_AACInfo >> 24) & 0xFF))
  {
    case QCAMCORDER_AAC_MONO2MONO:
      focus = IDS_AAC_MONO2MONO;
      break;
    case QCAMCORDER_AAC_STEREO2MONO:
      focus = IDS_AAC_STEREO2MONO;
      break;
    case QCAMCORDER_AAC_STEREO2STEREO:
    default:
      focus = IDS_AAC_STEREO2STEREO;
      break;
  }

  Qcamcorder_BuildSubMenu(pMe, AACModeIDList, focus);

}


/* ==========================================================================
FUNCTION QCAMCORDER_BUILDAACFREQMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_BuildAACFreqMenu(Qcamcorder *pMe)
{
  uint16 focus = 0;

  switch ((QcamcorderAACFreqType)((pMe->m_AACInfo >> 16) & 0xFF))
  {
    case QCAMCORDER_AAC_FREQ_11:
      focus = IDS_AAC_FREQ_11;
      break;
    case QCAMCORDER_AAC_FREQ_16:
      focus = IDS_AAC_FREQ_16;
      break;
    case QCAMCORDER_AAC_FREQ_22:
      focus = IDS_AAC_FREQ_22;
      break;
    case QCAMCORDER_AAC_FREQ_24:
      focus = IDS_AAC_FREQ_24;
      break;
    case QCAMCORDER_AAC_FREQ_8:
    default:
      focus = IDS_AAC_FREQ_8;
      break;
  }

  Qcamcorder_BuildSubMenu(pMe, AACFreqIDList, focus);

}


/* ==========================================================================
FUNCTION QCAMCORDER_BUILDAACBITRATEMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_BuildAACBitrateMenu(Qcamcorder *pMe)
{
  uint16 focus = 0;

  switch ((QcamcorderAACBitrateType)((pMe->m_AACInfo >> 8) & 0xFF))
  {
    case QCAMCORDER_AAC_BITRATE_MED:
      focus = IDS_AAC_BITRATE_MED;
      break;
    case QCAMCORDER_AAC_BITRATE_HIGH:
      focus = IDS_AAC_BITRATE_HIGH;
      break;
    case QCAMCORDER_AAC_BITRATE_LOW:
    default:
      focus = IDS_AAC_BITRATE_LOW;
      break;
  }

  Qcamcorder_BuildSubMenu(pMe, AACBitrateIDList, focus);

}
#endif /* FEATURE_CAMCORDER_AAC_REC */


/* ==========================================================================
FUNCTION QCAM_BUILDSUBMENU
DESCRIPTION
   Builds a generic menu.
============================================================================= */
static void Qcamcorder_BuildSubMenu(Qcamcorder *pMe, uint16 * idList, uint16 selected)
{
   AEERect qrc;
   CtlAddItem ai;
   uint16 i;

   // Delete any previous menu
   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
   //Set the Rectangle for the Menu
   SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 160 - qrc.y);

   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
   //lint +e570

   // Set Title
   (void)IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, idList[1], NULL);

   // Add Items in the Menu
   ai.pText = NULL;
   ai.pImage = NULL;
   ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.dwData = 0;

   /* Index for building menus - first assume we build all menu items */
   i = 0;

#ifndef FEATURE_VIDEO_ENCODE_LANDSCAPE_MOUNT
   /* Build only subset of menu items for certain menu settings */
   if(pMe->wShortHeader != QCAMCORDER_SH_DISABLE &&
      (selected == IDS_ROTATE_OFF || selected == IDS_ROTATE_90 ||
       selected == IDS_ROTATE_180 || selected == IDS_ROTATE_270))
   {
     i = 1;
   }
#endif /* FEATURE_VIDEO_ENCODE_LANDSCAPE_MOUNT */

   for (;i < idList[0];)
   {
     // Add Entries
     ai.wText    = idList[i+2];
     ai.wItemID  = idList[i+2];
     ai.wImage   = (uint16)(selected == ai.wItemID ? IDB_RADIO_ON : IDB_RADIO_OFF);
     (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
     ai.dwData++;

     if(pMe->wShortHeader != QCAMCORDER_SH_DISABLE &&
#ifdef FEATURE_VIDEO_ENCODE_LANDSCAPE_MOUNT
        (selected == IDS_ROTATE_OFF || selected == IDS_ROTATE_180)
#else
        (selected == IDS_ROTATE_90 || selected == IDS_ROTATE_270)
#endif /* FEATURE_VIDEO_ENCODE_LANDSCAPE_MOUNT */
     )
     {
       i+=2;
     }
     else
     {
       i++;
     }
   }

   IMENUCTL_SetSel(pMe->m_pIMenu, selected);
   return;
} /* END QCam_BuildSubMenu */

/* ==========================================================================
FUNCTION QCAMCORDER_BUILDMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_BuildMenu(Qcamcorder *pMe, QcamcorderMenuType type)
{
  // Make sure the pointers we'll be using are valid
  if (pMe == NULL || pMe->a.m_pIShell == NULL)
    return;

  //lint -e570  Suppress complaint on loss of sign
  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
  //lint +e570

  switch (type)
  {
    case QCAMCORDER_MENU_SOUND:
      Qcamcorder_BuildSoundMenu(pMe);
      break;
    case QCAMCORDER_MENU_SIZE:
      Qcamcorder_BuildSizeMenu(pMe);
      break;
    case QCAMCORDER_MENU_QUALITY:
      Qcamcorder_BuildQualityMenu(pMe);
      break;
    case QCAMCORDER_MENU_AUDIO_FORMAT:
      Qcamcorder_BuildAudioFormatMenu(pMe);
      break;
    case QCAMCORDER_MENU_SAVE_LOC:
      Qcamcorder_BuildSaveLocMenu(pMe);
      break;
    case QCAMCORDER_MENU_DELETEALL:
      Qcamcorder_BuildDeleteAllMenu(pMe);
      break;
    case QCAMCORDER_MENU_NUMCLIPS:
      Qcamcorder_BuildNumClipsMenu(pMe);
      break;
    case QCAMCORDER_MENU_CLIPLENGTH:
      Qcamcorder_BuildClipLengthMenu(pMe);
      break;
    case QCAMCORDER_MENU_WHITEBALANCE:
      Qcamcorder_BuildWhiteBalanceMenu(pMe);
      break;
    case QCAMCORDER_MENU_FRAMERATE:
      Qcamcorder_BuildFrameRateMenu(pMe);
      break;
    case QCAMCORDER_MENU_FILE_FORMAT:
      Qcamcorder_BuildFileFormatMenu(pMe);
      break;
    case QCAMCORDER_MENU_VIDEO_FORMAT:
      Qcamcorder_BuildVideoFormatMenu(pMe);
      break;
#ifdef FEATURE_MP4_TRANSCODER
    case QCAMCORDER_MENU_TRANSCODER_VIDEO_SIZE:
      Qcamcorder_BuildTranscoderSizeMenu(pMe);
      break;
    case QCAMCORDER_MENU_TRANSCODER_BRAND:
      Qcamcorder_BuildTranscoderBrandMenu(pMe);
      break;
    case QCAMCORDER_MENU_TRANSCODER_AUDIO_FORMAT:
      Qcamcorder_BuildTranscoderAudioFormatMenu(pMe);
      break;
    case QCAMCORDER_MENU_TRANSCODER_VIDEO_FORMAT:
      Qcamcorder_BuildTranscoderVideoCodecMenu(pMe);
      break;
#endif
#ifdef FEATURE_DVS
    case QCAMCORDER_MENU_DVSMARGIN:
      Qcamcorder_BuildDVSMarginMenu(pMe);
      break;
#endif
    case QCAMCORDER_MENU_TIMER:
      Qcamcorder_BuildTimerMenu(pMe);
      break;
    case QCAMCORDER_MENU_SPACE_THRESHOLD:
      Qcamcorder_BuildSpaceThresholdMenu(pMe);
      break;
    case QCAMCORDER_MENU_EFFECTS:
      Qcamcorder_BuildEffectsMenu(pMe);
      break;
    case QCAMCORDER_MENU_ANTIBANDING:
      Qcamcorder_BuildAntibandingMenu(pMe);
      break;
    case QCAMCORDER_MENU_ROTATE:
      Qcamcorder_BuildRotationMenu(pMe);
      break;
#ifdef FEATURE_QCAMCORDER_TELOP
    case QCAMCORDER_MENU_TELOP:
      Qcamcorder_BuildTelopMenu(pMe);
      break;
#endif
    case QCAMCORDER_MENU_CONT:
      Qcamcorder_BuildContMenu(pMe);
      break;
    case QCAMCORDER_MENU_SPECIAL_EFFECTS:
      Qcamcorder_BuildSpecialEffectsMenu(pMe, 0);
      break;
#ifdef FEATURE_CAMCORDER_AAC_REC
    case QCAMCORDER_MENU_AAC_CONFIG:
      Qcamcorder_BuildAACConfigMenu(pMe);
      break;
    case QCAMCORDER_MENU_AAC_MODE:
      Qcamcorder_BuildAACModeMenu(pMe);
      break;
    case QCAMCORDER_MENU_AAC_FREQ:
      Qcamcorder_BuildAACFreqMenu(pMe);
      break;
    case QCAMCORDER_MENU_AAC_BITRATE:
      Qcamcorder_BuildAACBitrateMenu(pMe);
      break;
#endif /* FEATURE_CAMCORDER_AAC_REC */
    case QCAMCORDER_MENU_FILELIST:
#ifdef FEATURE_VIDEO_ENCODE_RAW
    case QCAMCORDER_MENU_RAWFILELIST:
#endif /* FEATURE_VIDEO_ENCODE_RAW */
    case QCAMCORDER_MENU_GPS:
    case QCAMCORDER_MENU_DELETE:
    case QCAMCORDER_MENU_MEM:
      break;
    default:
      break;
  }

  IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
  (void)IMENUCTL_Redraw(pMe->m_pIMenu);
  pMe->m_activeMenu = type;
}

/* ==========================================================================
FUNCTION QCAMCORDER_BUILDSOFTMENU
DESCRIPTION
============================================================================= */
void Qcamcorder_BuildSoftMenu(Qcamcorder *pMe, QcamcorderSoftMenuType type)
{
   // Make sure the pointers we'll be using are valid
   if (pMe == NULL || pMe->a.m_pIShell == NULL)
      return;

   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);

   //lint +e570

   switch (type)
   {
   case QCAMCORDER_MENU_PREVIEW:
      pMe->m_bShowMenu = TRUE;
      Qcamcorder_BuildPreviewMenu(pMe);
      break;
   case QCAMCORDER_MENU_SETTINGS:
      Qcamcorder_BuildSettingsMenu(pMe);
      break;
   case QCAMCORDER_MENU_EDIT:
      pMe->m_bShowMenu = TRUE;
      Qcamcorder_BuildEditMenu(pMe);
      break;
   case QCAMCORDER_MENU_NONE:
      break;
   default:
       break;
   }

   pMe->m_activeSoftMenu = type;

}

/* ==========================================================================
FUNCTION QCAMCORDER_GETMEMUSAGE
DESCRIPTION
============================================================================= */
void Qcamcorder_GetMemUsage (Qcamcorder* pMe, Qcamcorder_MemUsageData *retVal,
                             QcamcorderSaveType saveLoc)
{
   IFileMgr *pIFileMgr = NULL;
   AEEFSFreeSpace FSSpace;
   int pnSize;
   pnSize = sizeof(AEEFSFreeSpace);
   retVal->dwFreeSpace = 0;
   retVal->dwTotalFileSystemSize = 0;
   retVal->dwUsedMemory = 0;
   retVal->dwRemainingVideoDuration = 0;

   (void)ISHELL_CreateInstance(pMe->a.m_pIShell,
                         AEECLSID_FILEMGR,
                         (void **)&pIFileMgr);
   if(pIFileMgr == NULL)
   {
     QCAMCORDER_MSG_ERROR("NULL pIFileMgr",0,0,0);
     return;
   }

   if(saveLoc == QCAMCORDER_SAVE_FLASH)
   {
     retVal->dwFreeSpace = IFILEMGR_GetFreeSpace(pIFileMgr,
                                     (uint32 *)&retVal->dwTotalFileSystemSize);
   }
#if MIN_BREW_VERSIONEx(3,1,3)
   else if(saveLoc == QCAMCORDER_SAVE_MMC)
   {
     if(SUCCESS != ISHELL_GetDeviceInfoEx (pMe->a.m_pIShell,
                                            AEE_DEVICEITEM_FSSPACE_CARD0 ,
                                            (void *)&FSSpace,
                                            &pnSize))
     {
       retVal->dwFreeSpace = 0;
     }
  }
#ifdef FEATURE_EFS_FTL
   else if(saveLoc == QCAMCORDER_SAVE_FTL)
   {
     if(SUCCESS != IFILEMGR_GetFreeSpaceEx(pIFileMgr,
                                           AEEFS_MOD_FTL_DIR,
                                           &(retVal->dwTotalFileSystemSize),
                                           &(retVal->dwFreeSpace)))
     {
       retVal->dwFreeSpace = 0;
     }
   }
#endif /* FEATURE_EFS_FTL */

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
   else if(saveLoc == QCAMCORDER_SAVE_USB)
   {
     if(SUCCESS != IFILEMGR_GetFreeSpaceEx(pIFileMgr,
                                           AEEFS_MOD_USB_DIR,
                                           &(retVal->dwTotalFileSystemSize),
                                           &(retVal->dwFreeSpace)))
     {
       retVal->dwFreeSpace = 0;
     }
   }
#endif /* FEATURE_EFS_HOTPLUG_USB_HOST_MS */
#endif /* MIN_BREW_VERSIONEx(3,1,3) */
   else
   {
     /* Invalid memory location */
     return;
   }

   if(saveLoc == QCAMCORDER_SAVE_MMC) 
   {
      retVal->dwFreeSpace = (uint32)(FSSpace.qwFreeSpace>>10);
      retVal->dwTotalFileSystemSize = (uint32)(FSSpace.qwTotalSpace>>10);
   }
   else
   {
      retVal->dwFreeSpace /= 1024;
      retVal->dwTotalFileSystemSize /=1024;
   }

   if(Qcamcorder_GetMovieSize(pMe))
   {
     retVal->dwRemainingVideoDuration =
                            retVal->dwFreeSpace / (uint32)Qcamcorder_GetMovieSize(pMe);
   }

   if(retVal->dwTotalFileSystemSize)
   {
     retVal->dwUsedMemory =
           (uint32)((retVal->dwTotalFileSystemSize - retVal->dwFreeSpace )*100 /
                   retVal->dwTotalFileSystemSize);
   }

   if(pIFileMgr)
   {
     (void)IFILEMGR_Release(pIFileMgr);
     pIFileMgr = NULL;
   }
}

static int32 Qcamcorder_MemUsageBlock(Qcamcorder* pMe, int32 startPoint, 
                                      int32 charHight, AEERect *pRect, 
                                      QcamcorderSaveType saveLoc, char *pMem)
{
   RGBVAL color = RGB_BLACK;
  Qcamcorder_MemUsageData usageData;
   char szBuf[TEXT_STRING_SIZE];
   AECHAR  szText[100];
   AEERect bar;
   int16 perct;

   Qcamcorder_GetMemUsage (pMe, &usageData, saveLoc);

   (void)STR_TO_WSTR(pMem, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText,
                  -1, 10, startPoint, pRect,
                     IDF_ALIGN_CENTER | IDF_TEXT_UNDERLINE);
   startPoint += (charHight+5);

   (void)STR_TO_WSTR("You can take:", szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
                  -1, 10, startPoint, pRect, 0);
   startPoint += charHight;

   (void)SNPRINTF(szBuf, sizeof(szBuf), " %u more seconds of video", usageData.dwRemainingVideoDuration);
   (void)STR_TO_WSTR(szBuf, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
                  -1, 10, startPoint, pRect, 0);
   startPoint += charHight;

   // Echo storage info.
   (void)STR_TO_WSTR("File Space Avail/Total [KB]:", szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
                  -1, 10, startPoint, pRect, 0);
   startPoint += charHight;

   (void)SNPRINTF(szBuf, sizeof(szBuf), "  %u / %u", usageData.dwFreeSpace, usageData.dwTotalFileSystemSize);
   (void)STR_TO_WSTR(szBuf, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
                  -1, 10, startPoint, pRect, 0);
   startPoint += charHight;

   // Draw usage bar
   // If the phone screen width is less than 100 pixel, then set the scroll
   // bar width to 100 pixels and adjust the percentage of progress accordingly.
   // If phone screen width is more than 100 pixels then set the scroll bar
   // width to 100 pixels.
   bar.x = 5;
   bar.dx = (int16)(pMe->di.cxScreen - 10);

   if(usageData.dwTotalFileSystemSize)
   {
     perct = (int16)usageData.dwUsedMemory;
   }
   else
   {
     perct = 1000; /* invalid value somethings wrong */
   }

   if (perct < (int16)75)
   {
     color = MAKE_RGB(0, 0xFF, 0);           // Green below 75% full
   }
   else if ( perct < (int16)90)
   {
     color = MAKE_RGB(0xFF, 0XFF, 0);        // Yellow 75% to 89% full
   }
   else
   {
     color = MAKE_RGB(0xFF,0,0);             // Red 90% or more
   }

   (void)SNPRINTF(szBuf, sizeof(szBuf), "memory usage...  %u %%", perct);
   perct = (int16)((perct*(pMe->di.cxScreen-10))/100);
   (void)STR_TO_WSTR(szBuf, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
                  -1, 10, startPoint, pRect, 0);
   startPoint += charHight;

   bar.dy = (int16)IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, AEE_FONT_BOLD, NULL, NULL);
   bar.y  = (int16)(startPoint+5);
   bar.dy = (int16)(bar.dy/2);

   (void)IDISPLAY_DrawFrame(pMe->a.m_pIDisplay, &bar, AEE_FT_INDENT,CLR_SYS_SCROLLBAR);
   startPoint = bar.y + bar.dy + 10;
  
   bar.dx = perct;
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_FillRect(pMe->a.m_pIDisplay, &bar, color);
   //lint +e570

  return startPoint;
}

/* ==========================================================================
FUNCTION QCAMCORDER_MEMUSAGE
DESCRIPTION
============================================================================= */
static void Qcamcorder_MemUsage(Qcamcorder* pMe)
{
   int i;
   int foundDevice[5] = {1, 0, 0, 0, 0};
   AECHAR  szText[100];
   AEERect rc;
   int32 charHight=IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL);
   int32 startPoint = 5;

   (void)IMENUCTL_DeleteAll(pMe->m_pIMenu);
   rc.x = rc.y = 0;
   rc.dx = (int16)pMe->di.cxScreen;
   rc.dy = (int16)(pMe->di.cyScreen-30);
   pMe->m_activeMenu = QCAMCORDER_MENU_MEM;

   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
   //lint +e570

   for(i = 1; i < (int)ARR_SIZE(AEEDirList); i++)
   {
     if(SUCCESS == IFILEMGR_Test(pMe->pIFileMgr, AEEDirList[i]))
       foundDevice[i] = TRUE;
   }

   // If we have external memory, let the user know which one is currently selected.
   //lint -e415
   if(foundDevice[QCAMCORDER_SAVE_MMC]
#ifdef FEATURE_EFS_FTL
      || foundDevice[QCAMCORDER_SAVE_FTL] 
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
      || foundDevice[QCAMCORDER_SAVE_USB]
#endif
      )
   //lint +e415
   {
     switch (pMe->saveLoc)
     {
       case QCAMCORDER_SAVE_MMC:
         (void)STR_TO_WSTR("Save Location: MMC", szText, sizeof(szText));
         break;
#ifdef FEATURE_EFS_FTL
       case QCAMCORDER_SAVE_FTL:
         (void)STR_TO_WSTR("Save Location: FTL", szText, sizeof(szText));
         break;
#endif /* FEATURE_EFS_FTL */

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
       case QCAMCORDER_SAVE_USB:
         (void)STR_TO_WSTR("Save Location: USB", szText, sizeof(szText));
         break;
#endif /* FEATURE_EFS_HOTPLUG_USB_HOST_MS */
       case QCAMCORDER_SAVE_FLASH:
       default:
         (void)STR_TO_WSTR("Save Location: FLASH", szText, sizeof(szText));
         break;
     }

     (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText,
                        -1, 10, startPoint, &rc, 0);
     startPoint += (charHight+5);
   }

   /* Show the usage details of Flash memory */
   startPoint = Qcamcorder_MemUsageBlock(pMe, startPoint, charHight, &rc,
                                         QCAMCORDER_SAVE_FLASH, 
                                         "[FLASH Memory Usage]");
   /* Show the usage details of MMC */
   if(foundDevice[QCAMCORDER_SAVE_MMC])
   {
     startPoint = Qcamcorder_MemUsageBlock(pMe, startPoint, charHight, &rc,
                                           QCAMCORDER_SAVE_MMC, 
                                           "[MMC Memory Usage]");
   }
#ifdef FEATURE_EFS_FTL
   /* Show the usage details of FTL */
   //lint -e661
   if(foundDevice[QCAMCORDER_SAVE_FTL])
   {
     startPoint = Qcamcorder_MemUsageBlock(pMe, startPoint, charHight, &rc,
                                           QCAMCORDER_SAVE_FTL, 
                                           "[FTL Memory Usage]");
   }
#endif /* FEATURE_EFS_FTL */

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
   /* Show the usage details of USB */
   if(foundDevice[QCAMCORDER_SAVE_USB])
   {
     startPoint = Qcamcorder_MemUsageBlock(pMe, startPoint, charHight, &rc,
                                           QCAMCORDER_SAVE_USB, 
                                           "[USB Memory Usage]");
   }
   //lint +e661
#endif /* FEATURE_EFS_HOTPLUG_USB_HOST_MS */
   IDISPLAY_Update(pMe->a.m_pIDisplay);
}


/* ==========================================================================
FUNCTION QCAMCORDER_SHOWPROPERTIES
DESCRIPTION Shows the properties of the current file being edited.
============================================================================= */
static uint8 Qcamcorder_ShowProperties(Qcamcorder* pMe)
{
   char szBuf[TEXT_STRING_SIZE];
   AECHAR  szText[100];
   AEERect rc;
   int32 charHight=IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL);
   int32 startPoint = 5;
   FileInfo fi;

#ifdef FEATURE_VIDEO_AUDIO_DUBBING
   IMovieFile * pFile;

   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE, (void **)&pFile) !=
       SUCCESS)
       return FALSE;
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */

   IMENUCTL_Reset(pMe->m_pISoftMenu);
   rc.x = rc.y = 0;
   rc.dx = (int16)(pMe->di.cxScreen);
   rc.dy = (int16)(pMe->di.cyScreen-30);

   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
   //lint +e570

   /* Title bar */
   (void)STR_TO_WSTR("[File Properties]", szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText,
                           -1, 10, startPoint, &rc,
                           IDF_ALIGN_CENTER|IDF_TEXT_UNDERLINE);
   startPoint += (charHight+5);

   /* File name */
   (void)SNPRINTF(szBuf, sizeof(szBuf), "Filename: %s", STRRCHR(pMe->sCurrentFileName, '/') + 1);
   (void)STR_TO_WSTR(szBuf, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
      -1, 10, startPoint, &rc, 0);
   startPoint += charHight;

   /* File size */
   pMe->pIFile = IFILEMGR_OpenFile(pMe->pIFileMgr, pMe->sCurrentFileName, _OFM_READ);
   /*Check if file open is successfull*/
   if(pMe->pIFile == NULL)
   {
       return (FALSE);
   }

   (void)IFILE_GetInfo(pMe->pIFile, &fi);
   (void)IFILE_Release(pMe->pIFile);
   pMe->pIFile = NULL;
   (void)SNPRINTF(szBuf, sizeof(szBuf),"Filesize: %u bytes", fi.dwSize);
   (void)STR_TO_WSTR(szBuf, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
      -1, 10, startPoint, &rc, 0);
   startPoint += charHight;


   /* Telop text */
   (void)SNPRINTF(szBuf,sizeof(szBuf), "Telop: %s", pMe->TelopTextInfo);
   (void)STR_TO_WSTR(szBuf, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
                           -1, 10, startPoint, &rc, 0);
   startPoint += charHight;


   /* GPS Longitude */
   (void)SNPRINTF(szBuf,sizeof(szBuf), "GPS Longitude: %d/%d %d/%d %d/%d",
                 (uint32)((pMe->ExtendedInfo.UUIDSpec.GPSLongitudeArray[0]>>32) & 0xFFFFFFFF),
                 (uint32)(pMe->ExtendedInfo.UUIDSpec.GPSLongitudeArray[0] & 0xFFFFFFFF),
                 (uint32)((pMe->ExtendedInfo.UUIDSpec.GPSLongitudeArray[1]>>32) & 0xFFFFFFFF),
                 (uint32)(pMe->ExtendedInfo.UUIDSpec.GPSLongitudeArray[1] & 0xFFFFFFFF),
                 (uint32)((pMe->ExtendedInfo.UUIDSpec.GPSLongitudeArray[2]>>32) & 0xFFFFFFFF),
                 (uint32)(pMe->ExtendedInfo.UUIDSpec.GPSLongitudeArray[2] & 0xFFFFFFFF));
   (void)STR_TO_WSTR(szBuf, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
                           -1, 10, startPoint, &rc, 0);
   startPoint += charHight;

   /* GPS Longtitude Reference */
   (void)SNPRINTF(szBuf, sizeof(szBuf), "GPS Longitude Ref: %c",
                 (char)((pMe->ExtendedInfo.UUIDSpec.dwGPSLongitudeRef)>>24) & 0xFF);
   (void)STR_TO_WSTR(szBuf, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
                           -1, 10, startPoint, &rc, 0);
   startPoint += charHight;


   /* GPS Latitude */
   (void)SNPRINTF(szBuf, sizeof(szBuf),"GPS Latitude: %d/%d %d/%d %d/%d",
                 (uint32)((pMe->ExtendedInfo.UUIDSpec.GPSLatitudeArray[0]>>32) & 0xFFFFFFFF),
                 (uint32)(pMe->ExtendedInfo.UUIDSpec.GPSLatitudeArray[0] & 0xFFFFFFFF),
                 (uint32)((pMe->ExtendedInfo.UUIDSpec.GPSLatitudeArray[1]>>32) & 0xFFFFFFFF),
                 (uint32)(pMe->ExtendedInfo.UUIDSpec.GPSLatitudeArray[1] & 0xFFFFFFFF),
                 (uint32)((pMe->ExtendedInfo.UUIDSpec.GPSLatitudeArray[2]>>32) & 0xFFFFFFFF),
                 (uint32)(pMe->ExtendedInfo.UUIDSpec.GPSLatitudeArray[2] & 0xFFFFFFFF));
   (void)STR_TO_WSTR(szBuf, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
                           -1, 10, startPoint, &rc, 0);
   startPoint += charHight;

   /* GPS Latitude Reference */
   (void)SNPRINTF(szBuf, sizeof(szBuf),"GPS Latitude Ref: %c",
                 (char)((pMe->ExtendedInfo.UUIDSpec.dwGPSLatitudeRef>>24) & 0xFF));
   (void)STR_TO_WSTR(szBuf, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
                           -1, 10, startPoint, &rc, 0);
   startPoint += charHight;


   /* GPS Altitude */
   (void)SNPRINTF(szBuf, sizeof(szBuf),"GPS Altitude: %d/%d",
                 (uint32)((pMe->ExtendedInfo.UUIDSpec.dwGPSAltitude>>32) & 0xFFFFFFFF),
                 (uint32)(pMe->ExtendedInfo.UUIDSpec.dwGPSAltitude & 0xFFFFFFFF));
   (void)STR_TO_WSTR(szBuf, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
                           -1, 10, startPoint, &rc, 0);
   startPoint += charHight;

   /* GPS Altitude Reference */
   (void)SNPRINTF(szBuf, sizeof(szBuf),"GPS Altitude Ref: %d",
                 pMe->ExtendedInfo.UUIDSpec.dwGPSAltitudeRef);
   (void)STR_TO_WSTR(szBuf, szText, sizeof(szText));
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szText,
                           -1, 10, startPoint, &rc, 0);
   startPoint += charHight;

   IDISPLAY_Update(pMe->a.m_pIDisplay);

   return TRUE;
}

/* ==========================================================================
FUNCTION QCAMCORDER_GETCONFIGRECORD
DESCRIPTION
============================================================================= */
static void Qcamcorder_GetConfigRecord(Qcamcorder* pMe)
{
   IFile*    pIFile = NULL;

   if(pMe->pIFileMgr)
   {
      boolean validConfigFile = FALSE;
      if(IFILEMGR_Test(pMe->pIFileMgr, QCAMCORDER_CONFIG_FILE)==SUCCESS)
      {
         pIFile = IFILEMGR_OpenFile(pMe->pIFileMgr, QCAMCORDER_CONFIG_FILE, _OFM_READ);
         if(pIFile)
         {
               char lastBuildID[MAXSIZE_OF_BUILD_ID]; //assuming worst case size
               int build_id_len = strlen(mob_sw_rev);
               (void)IFILE_Read(pIFile, lastBuildID, build_id_len);
               lastBuildID[build_id_len] = '\0';
               if(strcmp(lastBuildID, mob_sw_rev) == 0)
               {
                   validConfigFile = TRUE;
               }
         }
      }
      if(validConfigFile)
      {
         //cfg file:
         /****************************************
         Thumbnail   Autoplay start date\time   position
         ON/OFF      OFF/SLOW/   MENU/ ON/OFF      ON/OFF
         STD/FAST CAMERA/
         ALBUM
         ******************************************/
         (void)IFILE_Read(pIFile, &pMe->wCurrentMovie,   sizeof(uint16));
         pMe->sStartSoundFile[0] = '\0';
         (void)IFILE_Read(pIFile, pMe->sStartSoundFile,  FILE_NAME_SIZE);
         if (pMe->sStartSoundFile[0] == '\0')
         {
             (void)STRLCPY(pMe->sStartSoundFile, QCAMCORDER_SOUND_FILE
                           ,sizeof(pMe->sStartSoundFile));
         }
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
         (void)IFILE_Read(pIFile, pMe->sFrameOverlay,  FILE_NAME_SIZE);
#endif
         (void)IFILE_Read(pIFile, &pMe->eSize,  1);
         (void)IFILE_Read(pIFile, &pMe->m_maxNumClips,  sizeof(pMe->m_maxNumClips));
         (void)IFILE_Read(pIFile, &pMe->m_maxClipLength,  sizeof(pMe->m_maxClipLength));
         (void)IFILE_Read(pIFile, &pMe->wQuality,  sizeof(pMe->wQuality));
#ifdef FEATURE_VIDEO_ENCODE_BITRATE
         (void)IFILE_Read(pIFile, &pMe->wBitRate,  sizeof(pMe->wBitRate));
#endif
         (void)IFILE_Read(pIFile, &pMe->wAudioFormatClsId, sizeof(pMe->wAudioFormatClsId));
         (void)IFILE_Read(pIFile, &pMe->wAudioFormat,  sizeof(pMe->wAudioFormat));
         (void)IFILE_Read(pIFile, &pMe->saveLoc,  sizeof(pMe->saveLoc));
         (void)IFILE_Read(pIFile, &pMe->m_FrameRate, sizeof(pMe->m_FrameRate));
         (void)IFILE_Read(pIFile, &pMe->wVideoFormat, sizeof(pMe->wVideoFormat));
         (void)IFILE_Read(pIFile, &pMe->wShortHeader, sizeof(pMe->wShortHeader));
         (void)IFILE_Read(pIFile, &pMe->wFileFormat, sizeof(pMe->wFileFormat));
         (void)IFILE_Read(pIFile, &pMe->m_nwhiteBalance, sizeof(pMe->m_nwhiteBalance));
#ifdef FEATURE_MP4_TRANSCODER
         (void)IFILE_Read(pIFile, &pMe->wTranscodeAudioFormat, sizeof(pMe->wTranscodeAudioFormat));
         (void)IFILE_Read(pIFile, &pMe->eTranscoderSize, sizeof(pMe->eTranscoderSize));
#endif
#ifdef FEATURE_DVS
         (void)IFILE_Read(pIFile, &pMe->eDVSMargin, sizeof(pMe->eDVSMargin));
#endif
         (void)IFILE_Read(pIFile, &pMe->m_nTimer, sizeof(pMe->m_nTimer));
         (void)IFILE_Read(pIFile, &pMe->m_nEffect, sizeof(pMe->m_nEffect));
         (void)IFILE_Read(pIFile, &pMe->m_nAntibanding, sizeof(pMe->m_nAntibanding));
         (void)IFILE_Read(pIFile, &pMe->m_bRotate, sizeof(pMe->m_bRotate));
         (void)IFILE_Read(pIFile, &pMe->m_bCont, sizeof(pMe->m_bCont));
#ifdef FEATURE_CAMCORDER_AAC_REC
         (void)IFILE_Read(pIFile, &pMe->m_AACInfo, sizeof(pMe->m_AACInfo));
#endif /* FEATURE_CAMCORDER_AAC_REC */
         (void)IFILE_Read(pIFile, &pMe->nZoom, sizeof(pMe->nZoom));
         (void)IFILE_Read(pIFile, &pMe->nZoomMin, sizeof(pMe->nZoomMin));
         (void)IFILE_Read(pIFile, &pMe->nZoomMax, sizeof(pMe->nZoomMax));
         (void)IFILE_Read(pIFile, &pMe->nZoomStep, sizeof(pMe->nZoomStep));
         (void)IFILE_Read(pIFile, &pMe->m_displaySize, sizeof(pMe->m_displaySize));
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */

         (void)IFILE_Release(pIFile);
         pIFile = NULL;
      }
      else  //no per-records
      {
         pMe->wCurrentMovie   = 0;
         (void)STRLCPY(pMe->sStartSoundFile, QCAMCORDER_SOUND_FILE
                       , sizeof(pMe->sStartSoundFile));
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
         (void)STRLCPY(pMe->sFrameOverlay, DISABLE_FRAMEOVERLAY_STRING,
                        sizeof(pMe->sFrameOverlay));
#endif
         pMe->eSize = QCAMCORDER_SIZE_SQCIF;
         pMe->m_maxNumClips = QCAMCORDER_NUMCLIPS_NONE;
         pMe->m_maxClipLength = QCAMCORDER_CLIPLENGTH_NONE;
         pMe->wQuality = QCAMCORDER_QUALITY_HIGH;
#ifdef FEATURE_VIDEO_ENCODE_BITRATE
         pMe->wBitRate = 128000; /* Assume user bitrate of 128 kbps*/
#endif

         pMe->wAudioFormat = QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K;
         pMe->saveLoc = QCAMCORDER_SAVE_FLASH;
         pMe->m_FrameRate = QCAMCORDER_FRAME_RATE_1;
         pMe->wVideoFormat = QCAMCORDER_VIDEO_FORMAT_MPEG4;
         pMe->wShortHeader = QCAMCORDER_SH_DISABLE;
         pMe->m_bCont = FALSE;
#ifdef FEATURE_VIDEO_ENCODE_FADING
         pMe->mFadeInOut = QCAMCORDER_FADE_OFF;
#endif
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
         pMe->bEnableTimeStamp = FALSE;
#endif /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/  
#ifdef FEATURE_DVS
         pMe->eDVSMargin = QCAMCORDER_DVSMARGIN_MED;
#endif
         pMe->nZoom      = 0;
         pMe->nZoomMin   = 0;
         pMe->nZoomMax   = 0;
         pMe->nZoomStep  = 0;
         pMe->m_displaySize = QCAMCORDER_DISPLAY_MENU_SELECT;
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
      }
   }
}

/* ==========================================================================
FUNCTION QCAMCORDER_SETCONFIGRECORD
DESCRIPTION
============================================================================= */
void Qcamcorder_SetConfigRecord(Qcamcorder* pMe)
{
   IFile*    pIFile = NULL;

   if (pMe->pIFileMgr)
   {
      if (IFILEMGR_Test(pMe->pIFileMgr, QCAMCORDER_CONFIG_FILE) != SUCCESS)
      {
         pIFile = IFILEMGR_OpenFile(pMe->pIFileMgr, QCAMCORDER_CONFIG_FILE, _OFM_CREATE);
      }
      else
      {
         pIFile = IFILEMGR_OpenFile(pMe->pIFileMgr, QCAMCORDER_CONFIG_FILE, _OFM_READWRITE);
      }
      if (pIFile)
      {
         (void)IFILE_Write(pIFile, mob_sw_rev, strlen(mob_sw_rev));
         (void)IFILE_Write(pIFile, &pMe->wCurrentMovie,     sizeof(uint16));
         (void)IFILE_Write(pIFile, pMe->sStartSoundFile, FILE_NAME_SIZE);
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
         (void)IFILE_Write(pIFile, pMe->sFrameOverlay, FILE_NAME_SIZE);
#endif
         (void)IFILE_Write(pIFile, &pMe->eSize,  1);
         (void)IFILE_Write(pIFile, &pMe->m_maxNumClips,  sizeof(pMe->m_maxNumClips));
         (void)IFILE_Write(pIFile, &pMe->m_maxClipLength,  sizeof(pMe->m_maxClipLength));
         (void)IFILE_Write(pIFile, &pMe->wQuality,  sizeof(pMe->wQuality));
#ifdef FEATURE_VIDEO_ENCODE_BITRATE
         (void)IFILE_Write(pIFile, &pMe->wBitRate,  sizeof(pMe->wBitRate));
#endif
         (void)IFILE_Write(pIFile, &pMe->wAudioFormatClsId, sizeof(pMe->wAudioFormatClsId));
         (void)IFILE_Write(pIFile, &pMe->wAudioFormat,  sizeof(pMe->wAudioFormat));
         (void)IFILE_Write(pIFile, &pMe->saveLoc,  sizeof(pMe->saveLoc));
         (void)IFILE_Write(pIFile, &pMe->m_FrameRate, sizeof(pMe->m_FrameRate));
         (void)IFILE_Write(pIFile, &pMe->wVideoFormat, sizeof(pMe->wVideoFormat));
         (void)IFILE_Write(pIFile, &pMe->wShortHeader, sizeof(pMe->wShortHeader));
         (void)IFILE_Write(pIFile, &pMe->wFileFormat, sizeof(pMe->wFileFormat));
         (void)IFILE_Write(pIFile, &pMe->m_nwhiteBalance, sizeof(pMe->m_nwhiteBalance));
#ifdef FEATURE_MP4_TRANSCODER
         (void)IFILE_Write(pIFile, &pMe->wTranscodeAudioFormat, sizeof(pMe->wTranscodeAudioFormat));
         (void)IFILE_Write(pIFile, &pMe->eTranscoderSize, sizeof(pMe->eTranscoderSize));
#endif
#ifdef FEATURE_DVS
         (void)IFILE_Write(pIFile, &pMe->eDVSMargin, sizeof(pMe->eDVSMargin));
#endif
         (void)IFILE_Write(pIFile, &pMe->m_nTimer, sizeof(pMe->m_nTimer));
         (void)IFILE_Write(pIFile, &pMe->m_nEffect, sizeof(pMe->m_nEffect));
         (void)IFILE_Write(pIFile, &pMe->m_nAntibanding, sizeof(pMe->m_nAntibanding));
         (void)IFILE_Write(pIFile, &pMe->m_bRotate, sizeof(pMe->m_bRotate));
         (void)IFILE_Write(pIFile, &pMe->m_bCont, sizeof(pMe->m_bCont));
#ifdef FEATURE_CAMCORDER_AAC_REC
         (void)IFILE_Write(pIFile, &pMe->m_AACInfo, sizeof(pMe->m_AACInfo));
#endif /* FEATURE_CAMCORDER_AAC_REC */

         (void)IFILE_Write(pIFile, &pMe->nZoom, sizeof(pMe->nZoom));
         (void)IFILE_Write(pIFile, &pMe->nZoomMin, sizeof(pMe->nZoomMin));
         (void)IFILE_Write(pIFile, &pMe->nZoomMax, sizeof(pMe->nZoomMax));
         (void)IFILE_Write(pIFile, &pMe->nZoomStep, sizeof(pMe->nZoomStep));
         (void)IFILE_Write(pIFile, &pMe->m_displaySize, sizeof(pMe->m_displaySize));
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
         (void)IFILE_Release(pIFile);
         pIFile = NULL;
      }
   }
}
/* ==========================================================================
FUNCTION QCAMCORDER_GETVIDEOEDITFILENAME
DESCRIPTION
============================================================================= */
char* Qcamcorder_GetVideoEditFileName(Qcamcorder* pMe)
{
  char         dirName[FILE_NAME_SIZE] = {0};
  static char  fileName[FILE_NAME_SIZE] = {0};
  char         num[FILE_NAME_SIZE] = {0};
  char         s_version[FILE_NAME_SIZE] = {0};
  static uint8 version = 0;
  char         *filename = pMe->sCurrentFileName;
  char         *pExt = NULL;
  char         *newfile = NULL;

  pExt = STRSTR(filename, ".") ;
  newfile = STRCHR(filename, '.');
  newfile -=3;
  (void)STRLCPY(num, newfile, STD_MIN(4,sizeof(num)));

  (void)STRLCPY(dirName, Qcamcorder_GetDirectory(pMe), sizeof(dirName));
  (void)STRLCAT(dirName, DIRECTORY_STR, sizeof(dirName));

  newfile -=3;
  if(!(STRNCMP(newfile, "mov", 3)))
    version = 0;
  else
  {
    newfile+=2;
    (void)STRLCPY(s_version,newfile,STD_MIN(2,sizeof(s_version)));
    version = (uint8)((atoi(s_version)+1)%10);
  }

  (void)SNPRINTF(fileName,sizeof(fileName),"%sve%d%s%s",dirName,version,num,pExt);

  return fileName;
}
#if defined FEATURE_QCAMCORDER_CUT_N_MERGE || defined FEATURE_QCAMCORDER_VIDEO_EDITING
#if !defined(FEATURE_BREW_IMOVIEEDIT) && defined(FEATURE_BREW_IMOVIEFILE)
/* ==========================================================================
FUNCTION QCAMCORDER_GETVIDEOEDITFILENAME
DESCRIPTION
============================================================================= */
static uint32 Qcamcorder_GetFileBrand(Qcamcorder* pMe)
{
   char *pExt=NULL;
  char *filename;
#ifdef FEATURE_MP4_TRANSCODER
  if( pMe->eState == QCAMCORDER_TRANSCODE )
  {
    filename = pMe->sTranscodeFileName;
  }
  else
  {
#endif
    filename = pMe->sVideoEditFileName;
#ifdef FEATURE_MP4_TRANSCODER
  }
#endif
  pExt = STRSTR(filename, ".") ;
  if(!STRCMP(pExt, QCAMCORDER_3G_MOVIE_EXTENSION))
  {//3g2 brand
       return MOVIEFILE_BRAND_3G2;
  }
  else if(!STRCMP(pExt, QCAMCORDER_K3G_MOVIE_EXTENSION))
  {//K3g brand
      return MOVIEFILE_BRAND_K3G;
  }
  else if(!STRCMP(pExt, QCAMCORDER_MP4_MOVIE_EXTENSION))
  {//MP4 brand
      return MOVIEFILE_BRAND_MP4;
  }
  else if(!STRCMP(pExt, QCAMCORDER_AMC_MOVIE_EXTENSION))
  {//AMC brand
      return MOVIEFILE_BRAND_AMC;
  }
  else if(!STRCMP(pExt, QCAMCORDER_SKM_MOVIE_EXTENSION))
  {//SKM brand
      return MOVIEFILE_BRAND_SKM;
  }
  else if(!STRCMP(pExt, QCAMCORDER_3GP_MOVIE_EXTENSION))
  {//3GP brand
     return MOVIEFILE_BRAND_3GP;
  }
  else
  { return MOVIEFILE_BRAND_MP4;
  }
}
#endif
#endif /* FEATURE_QCAMCORDER_VIDEO_EDITING  || FEATURE_QCAMCORDER_CUT_N_MERGE */

#ifdef FEATURE_VIDEO_ENCODE_RAW
/* ==========================================================================
FUNCTION QCAMCORDER_GETRAWEMCODEDFILENAME
DESCRIPTION
============================================================================= */
static char* Qcamcorder_GetRawEncoderFileName(Qcamcorder* pMe,
                                              RawEncoderConfigType* cfg)
{
  char  dirName[FILE_NAME_SIZE]  = {0};
  char  num[FILE_NAME_SIZE]      = {0};
  char  *pExt     = NULL;
  char  *tempfile = NULL;


  /* Extract index of raw source file */
  tempfile = STRCHR(pMe->sCurrentFileName, '.');
  tempfile -= 3;
  (void)STRLCPY(num, tempfile, STD_MIN(4,sizeof(num)));


  if(cfg->save_location == QCAMCORDER_SAVE_FLASH)
    (void)STRLCPY(dirName, QCAMCORDER_MOVIES_DIR_FLASH, sizeof(dirName));
  else if(cfg->save_location == QCAMCORDER_SAVE_MMC)
  {
    if (IFILEMGR_Test(pMe->pIFileMgr, QCAMCORDER_MOVIES_DIR_MMC) == SUCCESS)
      (void)STRLCPY(dirName, QCAMCORDER_MOVIES_DIR_MMC, sizeof(dirName));
    else
    {
      QCAMCORDER_MSG_ERROR("IFILEMGR cannot find MMC directory",0,0,0);
      return NULL;
    }
  }
#ifdef FEATURE_EFS_FTL
  else if(cfg->save_location == QCAMCORDER_SAVE_FTL)
  {
    if (IFILEMGR_Test(pMe->pIFileMgr, QCAMCORDER_MOVIES_DIR_FTL) == SUCCESS)
      (void)STRLCPY(dirName, QCAMCORDER_MOVIES_DIR_FTL, sizeof(dirName));
    else
    {
      QCAMCORDER_MSG_ERROR("IFILEMGR cannot find FTL directory",0,0,0);
      return NULL;
    }
  }
#endif /* FEATURE_EFS_FTL */

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  else if(cfg->save_location == QCAMCORDER_SAVE_USB)
  {
    if (IFILEMGR_Test(pMe->pIFileMgr, QCAMCORDER_MOVIES_DIR_USB) == SUCCESS)
      (void)STRLCPY(dirName, QCAMCORDER_MOVIES_DIR_USB, sizeof(dirName));
    else
    {
      QCAMCORDER_MSG_ERROR("IFILEMGR cannot find USB directory",0,0,0);
      return NULL;
    }
  }
#endif /* FEATURE_EFS_HOTPLUG_USB_HOST_MS */
  else
    return NULL;

  (void)STRLCAT(dirName, DIRECTORY_STR,sizeof(dirName));


  /* Get the respective name extensions based on brand */
  switch (cfg->file_brand)
  {
    case QCAMCORDER_FILE_MP4:
      pExt = QCAMCORDER_MP4_MOVIE_EXTENSION;
      break;
    case QCAMCORDER_FILE_AMC:
      pExt = QCAMCORDER_AMC_MOVIE_EXTENSION;
      break;
    case QCAMCORDER_FILE_3GP:
      pExt = QCAMCORDER_3GP_MOVIE_EXTENSION;
      break;
    case QCAMCORDER_FILE_SKM:
      pExt = QCAMCORDER_SKM_MOVIE_EXTENSION;
      break;
    case QCAMCORDER_FILE_3G2_FRAG:
    case QCAMCORDER_FILE_3G2:
      pExt = QCAMCORDER_3G_MOVIE_EXTENSION;
      break;
    case QCAMCORDER_FILE_K3G:
      pExt = QCAMCORDER_K3G_MOVIE_EXTENSION;
      break;
    default:
      pExt = QCAMCORDER_MP4_MOVIE_EXTENSION;
      break;
  }

  /* Only allow one encoded clip per raw file */
  (void)SNPRINTF(pMe->sRawEncoderFileName, sizeof(pMe->sRawEncoderFileName),
                  "%srm%s%s", dirName, num, pExt);
  if(IFILEMGR_Test(pMe->pIFileMgr, pMe->sRawEncoderFileName) == SUCCESS)
  {
    (void)IFILEMGR_Remove(pMe->pIFileMgr, pMe->sRawEncoderFileName);
  }
  return pMe->sRawEncoderFileName;
}
#endif /* FEATURE_VIDEO_ENCODE_RAW */


/* ==========================================================================
FUNCTION QCAMCORDER_GETFILENAME
DESCRIPTION
============================================================================= */
static char* Qcamcorder_GetFileName(Qcamcorder* pMe)
{
   char dirName[FILE_NAME_SIZE] = {0};
   static char fileName[FILE_NAME_SIZE] = {0};
   char *pExt=NULL;

   switch (pMe->wFileFormat)
   {
   case QCAMCORDER_FILE_MP4:
       pExt = QCAMCORDER_MP4_MOVIE_EXTENSION;
      break;
   case QCAMCORDER_FILE_AMC:
       pExt = QCAMCORDER_AMC_MOVIE_EXTENSION;
      break;
   case QCAMCORDER_FILE_3GP:
       pExt = QCAMCORDER_3GP_MOVIE_EXTENSION;
      break;
   case QCAMCORDER_FILE_3G2_FRAG:
   case QCAMCORDER_FILE_3G2:
       pExt = QCAMCORDER_3G_MOVIE_EXTENSION;
      break;
   case QCAMCORDER_FILE_K3G:
       pExt = QCAMCORDER_K3G_MOVIE_EXTENSION;
      break;
   case QCAMCORDER_FILE_SKM:
       pExt = QCAMCORDER_SKM_MOVIE_EXTENSION;
      break;
     default:
       pExt = QCAMCORDER_MP4_MOVIE_EXTENSION;
      break;
   }

   (void)STRLCPY(dirName, Qcamcorder_GetDirectory(pMe), sizeof(dirName));
   (void)STRLCAT(dirName, DIRECTORY_STR,sizeof(dirName));

   do
   {
      (void)SNPRINTF(fileName, sizeof(fileName), "%smov%03u%s",
                    dirName, ++pMe->wCurrentMovie, pExt);
   }while(SUCCESS==IFILEMGR_Test(pMe->pIFileMgr, fileName));

   /* Store file name of next recorded clip to display it */
   (void)SNPRINTF(pMe->sRecordFileName, sizeof(pMe->sRecordFileName),
                   "mov%03u%s", pMe->wCurrentMovie, pExt);

   return fileName;
}

/* ==========================================================================
FUNCTION QCAMCORDER_GetTotalClips
DESCRIPTION Returns the number of movies recorded so far.
============================================================================= */
static uint16 Qcamcorder_GetTotalClips(Qcamcorder * pMe)
{
   uint16 count = 0;
   FileInfo fi;
   uint16 max_file_count = 0;

   (void)IFILEMGR_EnumInit(pMe->pIFileMgr, Qcamcorder_GetDirectory(pMe), FALSE);
   while (IFILEMGR_EnumNext(pMe->pIFileMgr, &fi))
   {
      QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);
      // Check if the extension of the file matches our movie extension
      // and it starts with "mov"
      if(Qcamcorder_IsValidFileName(fi.szName))
      {
         ++count;
      }
   }
   return count;
} /* END Qcamcorder_GetTotalClips */

/* ==========================================================================
FUNCTION QCAMCORDER_DeleteOldestFile
DESCRIPTION Deletes the oldest video clip.
============================================================================= */
static boolean Qcamcorder_DeleteOldestFile(Qcamcorder * pMe)
{
   FileInfo fi;
   char filename[MAX_FILE_NAME] ={'\0'};
   uint16 max_file_count = 0;

   (void)IFILEMGR_EnumInit(pMe->pIFileMgr, Qcamcorder_GetDirectory(pMe), FALSE);
   while (IFILEMGR_EnumNext(pMe->pIFileMgr, &fi))
   {
      QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);
      // Check if the extension of the file matches our movie extension
      // And the filename starts with mov
      if(Qcamcorder_IsValidFileName(fi.szName))
      {
#ifndef FEATURE_BREW_3_0
        if(pMe->saveLoc == QCAMCORDER_SAVE_MMC)
        {STRLCPY(filename, QCAMCORDER_MOVIES_MMC_MOUNT, sizeof(filename));
        }
#ifdef FEATURE_EFS_FTL
        else if(pMe->saveLoc == QCAMCORDER_SAVE_FTL)
        {STRLCPY(filename, QCAMCORDER_MOVIES_FTL_MOUNT, sizeof(filename));
        }
#endif /* FEATURE_EFS_FTL */

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
        else if(pMe->saveLoc == QCAMCORDER_SAVE_USB)
        {STRLCPY(filename, QCAMCORDER_MOVIES_USB_MOUNT, sizeof(filename));
        }
#endif /* FEATURE_EFS_HOTPLUG_USB_HOST_MS */
           else
        {STRLCPY(filename, QCAMCORDER_MOVIES_FLASH_MOUNT, sizeof(filename));
        }
#endif
        (void)STRLCAT(filename, fi.szName,sizeof(filename));
        (void)IFILEMGR_Remove(pMe->pIFileMgr, filename);
         return TRUE;
      }
   }
   return FALSE;
}

boolean Qcamcorder_SubDeleteAllClips(Qcamcorder *pMe, const char *pDir, const char *pOEMDir)
{
  FileInfo fi;
  char * p_newfile;
  boolean retval = TRUE;
  uint16 max_file_count = 0;

 (void)IFILEMGR_EnumInit(pMe->pIFileMgr, pDir, FALSE);
  while (IFILEMGR_EnumNext(pMe->pIFileMgr, &fi))
  {
    QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);
    // Check if the extension of the file matches our movie extension
    if(Qcamcorder_IsValidFileName(fi.szName))
    {
      p_newfile = MALLOC(FILE_NAME_SIZE);

      if(!p_newfile)
      {
        QCAMCORDER_MSG_HIGH("MALLOC FAILED",0,0,0);
      }

#ifdef FEATURE_BREW_3_0
      (void)STRLCAT(p_newfile, "", FILE_NAME_SIZE);
#else
      STRLCAT(p_newfile, pOEMDir, FILE_NAME_SIZE);
#endif
      (void)STRLCAT(p_newfile, fi.szName, FILE_NAME_SIZE);

      if(p_newfile )
      {
        if(IFILEMGR_Remove(pMe->pIFileMgr, p_newfile) != SUCCESS)
        retval = FALSE;
        FREE(p_newfile);
      }
    }
  }
  return retval;
  }

/* ==========================================================================
FUNCTION QCAMCORDER_DeleteAllClips
DESCRIPTION Deletes all movies.
============================================================================= */
boolean Qcamcorder_DeleteAllClips(Qcamcorder * pMe)
{
  FileInfo fi;
  uint16 selection = IMENUCTL_GetSel(pMe->m_pIMenu);
  boolean retval = TRUE;
  uint16 max_file_count = 0;

  if(selection==IDS_QCAMCORDER_YES||selection==IDS_FLASH_DELETEALL)//delete all or only flash
  {
    (void)IFILEMGR_EnumInit(pMe->pIFileMgr, QCAMCORDER_MOVIES_DIR_FLASH, FALSE);
    while (IFILEMGR_EnumNext(pMe->pIFileMgr, &fi))
    {
       QCAMCORDER_CHECK_TIMETOSLEEP(max_file_count);
      // Check if the extension of the file matches our movie extension
      if(Qcamcorder_IsValidFileName(fi.szName))
      {
          if(IFILEMGR_Remove(pMe->pIFileMgr, fi.szName) != SUCCESS)
            retval = FALSE;
      }
    }
  }

  if(selection==IDS_QCAMCORDER_YES||selection==IDS_MMC_DELETEALL)//delete all or only MMC
  {
    if(Qcamcorder_SubDeleteAllClips(pMe, AEEDirList[QCAMCORDER_SAVE_MMC], 
                                    OEMDirList[QCAMCORDER_SAVE_MMC]) == FALSE)
    {
      retval = FALSE;
    }
  }

#ifdef FEATURE_EFS_FTL
  if(selection==IDS_QCAMCORDER_YES||selection==IDS_FTL_DELETEALL)//delete all or only FTL
  {
    if(Qcamcorder_SubDeleteAllClips(pMe, AEEDirList[QCAMCORDER_SAVE_FTL], 
                                    OEMDirList[QCAMCORDER_SAVE_FTL]) == FALSE)
    {
        retval = FALSE;
    }
  }
#endif /* FEATURE_EFS_FTL */

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  if(selection==IDS_QCAMCORDER_YES||selection==IDS_USB_DELETEALL)//delete all or only USB
  {
    if(Qcamcorder_SubDeleteAllClips(pMe, AEEDirList[QCAMCORDER_SAVE_USB], 
                                    OEMDirList[QCAMCORDER_SAVE_USB]) == FALSE)
    {
      retval = FALSE;
    }
  }
#endif /* FEATURE_EFS_HOTPLUG_USB_HOST_MS */

  return retval;
} /* END Qcamcorder_DeleteAllClips */

/* ==========================================================================
FUNCTION QCAMCORDER_CAMEVENTNOTIFY
DESCRIPTION
============================================================================= */
static void Qcamcorder_CamEventNotify(Qcamcorder * pMe, AEECameraNotify * pcn)
{

_record_event_payload record_event_payload;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

   if (pcn->nCmd == CAM_CMD_START)
   {
     switch (pcn->nStatus)
     {
       case CAM_STATUS_START:
         // Restart preview
         if (pMe->eState == QCAMCORDER_NULL)
         {
           Qcamcorder_Preview(pMe);
         }
         else if (pMe->eState == QCAMCORDER_PREVIEW && pMe->m_bRecord_to_Preview)
         {
           /* When stop recording, pcn->nStatus set to CAM_STATUS_DONE and
           ** pMe->eState set to QCAMCORDER_PREVIEW indicating preview restart */
           AEEParmInfo pInfo;

           /* Update the zoom parameters which are calculated in the
           ** preview command by the camera driver.
           ** We update zoom here to make sure camera layer is in correct state before
           ** getting zoom parameter. */
           (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_ZOOM, &pMe->nZoom, (int32*)&pInfo);
           pMe->nZoomMin = pInfo.nMin;
           pMe->nZoomMax = pInfo.nMax;
           pMe->nZoomStep = pInfo.nStep;
           pMe->m_bRecord_to_Preview = FALSE;

           /* Start recording in continous mode if user didn't hit stop */
           if(pMe->m_bCont && !pMe->bStopRecording)
           {
             Qcamcorder_StartRecording(pMe);
           }
           else if(pMe->bStopRecording)
           {
             pMe->bStopRecording = FALSE;
           }
         }
         else if (pMe->eState == QCAMCORDER_RECORDING && pMe->m_bPreview_to_Record)
         {
           pMe->m_bPreview_to_Record = FALSE;
           pMe->nRecordStartTimeMs = GETUPTIMEMS(); 
           pMe->nRecordStartTime = (pMe->nRecordStartTimeMs/1000);
#ifndef FEATURE_CAMERA_LCD_DIRECT_MODE
           pMe->nPreviousFrameTimeMs  = pMe->nRecordStartTimeMs;
#endif
           Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_PREVIEW);
           Qcamcorder_DisplayRecordTime(pMe);
         }
         break;

       case CAM_STATUS_FRAME:
         {
           IBitmap *      pFrame = NULL;
           AEEBitmapInfo  bi;
           uint16 x;
           uint16 y;
#ifndef FEATURE_CAMERA_LCD_DIRECT_MODE
           if (pMe->eState == QCAMCORDER_RECORDING && pMe->bPausePressed)
           {
              uint32 currentTime;
              currentTime = GETUPTIMEMS();
              pMe->nRecordTotalTime += ((currentTime/1000) - (pMe->nPreviousFrameTimeMs/1000));
              pMe->nRecordTotalTimeMs += (currentTime - pMe->nPreviousFrameTimeMs);
              pMe->nPreviousFrameTimeMs = currentTime;
           }
#endif
           /* IMPORTANT NOTE: You need to do IBITMAP_Release(pFrame)
           ** after you're done with pFrame. */
           (void)ICAMERA_GetFrame(pMe->m_pICam, &pFrame);
           if (!pFrame)
             return;

           #ifdef FEATURE_MMOVERLAY
           if(!pMe->b_displayingSpaceErrorMsg)
           #endif /* FEATURE_MMOVERLAY */
           {
             // Get the bitmap info...this can be saved in app global structure.
             (void)IBITMAP_GetInfo(pFrame, &bi, sizeof(bi));

             x = (uint16)((pMe->di.cxScreen + BREW_OFFSET_X - bi.cx)/2);
             y = (uint16)((pMe->di.cyScreen + BREW_OFFSET_Y - bi.cy)/2);

             IDISPLAY_BitBlt(pMe->a.m_pIDisplay, x, y, (int)bi.cx, (int)bi.cy, pFrame, 0, 0, AEE_RO_COPY);
           }

           (void)IBITMAP_Release(pFrame);
           pFrame = NULL;
         }
         break;

       case CAM_STATUS_DONE:

#ifdef FEATURE_MMOVERLAY
         if(pMe->b_displayingSpaceErrorMsg)
           pMe->b_displayingSpaceErrorMsg = FALSE;
#endif /* FEATURE_MMOVERLAY */

         if (pMe->eState == QCAMCORDER_START_PREVIEW ||
             pMe->eState == QCAMCORDER_RECORDING
#ifdef FEATURE_VIDEO_ENCODE_FADING
             || pMe->eState == QCAMCORDER_FADING_OUT
#endif
            )
         {
           /* Note: If a CAM_STATUS_DONE event is received when
           ** pMe->eState == QCAMCORDER_RECORDING, this indicates that
           ** start recording was not successful and that the lower
           ** layers are expecting a call to restart preview.
           */

           if(pMe->eState == QCAMCORDER_RECORDING)
           {
             pMe->m_bPreview_to_Record = FALSE;
           }

           /*
           ** If pMe->eState == QCAMCORDER_START_PREVIEW or
           **    pMe->eState == QCAMCORDER_FADING_OUT, this indicates that
           ** recording has stopped successfully. Preview can be restarted
           */

           Qcamcorder_Preview(pMe);
         }
         else if (pMe->eState == QCAMCORDER_START_RECORDING)
         {
           AEEMediaData md;

           pMe->eState = QCAMCORDER_RECORDING;

#ifdef FEATURE_QCAMCORDER_TELOP
           /* For Telop enabled recording, ICAMERA_SetMediaData()
                        is done later in Qcamcorder_GetInputHandleEvent() */
           if(TELOP_OFF == pMe->m_bTelop)
#endif /* FEATURE_QCAMCORDER_TELOP */
           {
              md.clsData = MMD_FILE_NAME;
              md.pData = (void *)Qcamcorder_GetFileName(pMe);
              md.dwSize = 0;

              /* Set the file name to be used for next recorded clip */
              (void)ICAMERA_SetMediaData(pMe->m_pICam, &md, pMe->pFileFormat);
           }

           /* Select the correct video format: MPEG4 or H264 */
           (void)ICAMERA_SetVideoEncode(pMe->m_pICam, AEECLSID_MEDIAMPEG4,
                                  pMe->wVideoFormat);

           (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_WHITE_BALANCE, pMe->m_nwhiteBalance, 0);

           /* useratom details need to be allocated memory and have to be 
           ** filled up before sending that parameter to service layer
           */
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
           Qcamcorder_AddUdatAtom(pMe);
#endif

#ifdef FEATURE_VIDEO_MULTI_UUID
           (void)ICAMERA_SetVideoEncode(pMe->m_pICam,
                                        CAM_ENCODE_UUID_LIST_ATOM_INFO,
                                        NULL);
#else /* FEATURE_VIDEO_MULTI_UUID */
           (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_UUID, NULL);
           (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_UUID_DATA, NULL);
#endif /* FEATURE_VIDEO_MULTI_UUID */

           if (pMe->eGPSState == QCAMCORDER_GPS_DATA_VALID)
           {
              gpsi_struct *gpsi = MALLOC(sizeof(gpsi_struct) );
              /* Fill in the GPS data */
              Qcamcorder_FillGPSData(pMe, gpsi);

#ifdef FEATURE_VIDEO_MULTI_UUID
              {
                CameraEncodeUUIDAtomInfo *uuid_info;
                uuid_info = MALLOC(sizeof(CameraEncodeUUIDAtomInfo));
                uuid_info->pAtom = MALLOC(sizeof(UUID_GPSI));
                (void)MEMCPY(uuid_info->pAtom, UUID_GPSI, sizeof(UUID_GPSI));
                uuid_info->pData = MALLOC(sizeof(gpsi_struct) );
                uuid_info->dwDataSize = sizeof(gpsi_struct)-2 ;
                (void)MEMCPY(uuid_info->pData, gpsi, sizeof(gpsi_struct));
                (void)ICAMERA_SetVideoEncode(pMe->m_pICam,
                                             CAM_ENCODE_UUID_LIST_ATOM_INFO,
                                             uuid_info);
                FREEIF(uuid_info->pAtom);
                FREEIF(uuid_info->pData);
                FREEIF(uuid_info);
              }
#else /* FEATURE_VIDEO_MULTI_UUID */
              {
                CameraData *uuid_data, *uuid;
                uuid = MALLOC(sizeof(CameraData));
                uuid->pData = MALLOC(sizeof(UUID_GPSI));
                uuid->dwBytes = sizeof(UUID_GPSI);
                (void)MEMCPY(uuid->pData, UUID_GPSI, sizeof(UUID_GPSI));
                uuid_data = MALLOC(sizeof(CameraData));
                uuid_data->pData = MALLOC(sizeof(gpsi_struct) );
                uuid_data->dwBytes = sizeof(gpsi_struct)-2 ;

                /* Fill in the GPS data */
                Qcamcorder_FillGPSData(pMe, gpsi);

                (void)MEMCPY(uuid_data->pData, gpsi, sizeof(gpsi_struct));
                /* Pass it to camera layer */
                (void)ICAMERA_SetVideoEncode(pMe->m_pICam,
                                             CAM_ENCODE_UUID,
                                             uuid);
                (void)ICAMERA_SetVideoEncode(pMe->m_pICam,
                                             CAM_ENCODE_UUID_DATA,
                                             uuid_data);
                FREEIF(uuid_data->pData);
                FREEIF(uuid_data);
                FREEIF(uuid->pData);
                FREEIF(uuid);
              }
#endif /* FEATURE_VIDEO_MULTI_UUID */
              FREEIF(gpsi);
           }

           if(pMe->wFileFormat == QCAMCORDER_FILE_AMC)
           {
#ifdef FEATURE_VIDEO_MULTI_UUID
               CameraEncodeUUIDAtomInfo *uuid_info;
               uuid_info = MALLOC(sizeof(CameraEncodeUUIDAtomInfo));
               uuid_info->pAtom = MALLOC(sizeof(UUID_CPGD));
               (void)MEMCPY(uuid_info->pAtom, UUID_CPGD, sizeof(UUID_CPGD));
               uuid_info->pData = MALLOC(sizeof(cpgd));
               uuid_info->dwDataSize = sizeof(cpgd);
               (void)MEMCPY(uuid_info->pData, &cpgd, sizeof(cpgd));
               (void)ICAMERA_SetVideoEncode(pMe->m_pICam,
                                            CAM_ENCODE_UUID_LIST_ATOM_INFO,
                                            uuid_info);
               FREEIF(uuid_info->pAtom);
               FREEIF(uuid_info->pData);
               FREEIF(uuid_info);
#else /* FEATURE_VIDEO_MULTI_UUID */

               CameraData *uuid_data, *uuid;
               uuid = MALLOC(sizeof(CameraData));
               uuid->pData = MALLOC(sizeof(UUID_CPGD));
               uuid->dwBytes = sizeof(UUID_CPGD);
               (void)MEMCPY(uuid->pData, UUID_CPGD, sizeof(UUID_CPGD));
               uuid_data = MALLOC(sizeof(CameraData));
               uuid_data->pData = MALLOC(sizeof(cpgd));
               uuid_data->dwBytes = sizeof(cpgd);
               (void)MEMCPY(uuid_data->pData, &cpgd, sizeof(cpgd));
               (void)ICAMERA_SetVideoEncode(pMe->m_pICam,
                                            CAM_ENCODE_UUID,
                                            uuid);
               (void)ICAMERA_SetVideoEncode(pMe->m_pICam,
                                            CAM_ENCODE_UUID_DATA,
                                            uuid_data);
               FREEIF(uuid_data->pData);
               FREEIF(uuid_data);
               FREEIF(uuid->pData);
               FREEIF(uuid);
#endif /* FEATURE_VIDEO_MULTI_UUID */
           }

           if(pMe->m_bTelop)
           {
              if(ISHELL_CreateDialog(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, IDD_GET_INPUT, NULL)==AEE_SUCCESS)
              {
                QCAMCORDER_MSG_HIGH("CAME HERE",0,0,0);
              }
           }
           else
           {
             record_event_payload.record_file_format=pMe->wFileFormat;
             record_event_payload.record_video_format=pMe->wVideoFormat;
             record_event_payload.record_audio_format=(AudioFormatType)pMe->wAudioFormat;
             record_event_payload.record_frame_rate=(uint32)((pMe->m_FrameRate&0x0000FFFF)/((pMe->m_FrameRate&0xFFFF0000)>>16));
             record_event_payload.record_quality=pMe->wQuality;
             event_report_payload(EVENT_CAMCORDER_START_RECORD,sizeof(record_event_payload),&record_event_payload);
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
             {
               if(ICAMERA_RecordMovie(pMe->m_pICam) == EUNSUPPORTED)
               {
                 pMe->msg = IDB_ERROR_QCAMCORDER;
                 Qcamcorder_ErrorEventUserNotify(pMe);
               }
             }
           }
         }
         else if(pMe->eState == QCAMCORDER_SPACE_WARNING ||
                 pMe->eState == QCAMCORDER_CAMLAYER_ERROR)
         {
            Qcamcorder_ErrorEventUserNotify(pMe);
         }
         else if(pMe->eState == QCAMCORDER_PREVIEW_TO_GPS)
         {
            /*
            ** Build GPS menu after ICAMERA_Stop is complete
            ** Prevents GPS menu from viewfinder image overlay
            */

            pMe->eState = QCAMCORDER_NULL;
            Qcamcorder_BuildGPSMenu(pMe, FALSE);
         }
         else if(pMe->eState == QCAMCORDER_PREVIEW_TO_SETTINGS)
         {
            /*
            ** Build settings menu after ICAMERA_Stop is complete
            ** Prevents settings menu from viewfinder image overlay
            */
            pMe->eState = QCAMCORDER_NULL;
            Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_SETTINGS);
         }
         else if (pMe->eState == QCAMCORDER_PREVIEW_TO_ALBUM)
         {
            /*
            ** Build album menu after ICAMERA_Stop is complete
            ** Prevents album menu from viewfinder image overlay
            ** Prevents an empty album from trying to start preview
            ** prior to having stop_preview completing.
            */
            pMe->eState = QCAMCORDER_NULL;
            // Show the list of files
            pMe->b_focusFirstItem = TRUE;
            Qcamcorder_DisplayFileList(pMe);
         }
#ifdef FEATURE_VIDEO_ENCODE_RAW
         else if (pMe->eState == QCAMCORDER_PREVIEW_TO_RAW_ALBUM)
         {
            /*
            ** Build album menu after ICAMERA_Stop is complete
            ** Prevents raw album menu from viewfinder image overlay
            */
            pMe->eState = QCAMCORDER_NULL;
            Qcamcorder_RawEncoderDisplayFileList(pMe);
         }
#endif /* FEATURE_VIDEO_ENCODE_RAW */
         break;

       case CAM_STATUS_PAUSE:
         pMe->eState = QCAMCORDER_PAUSED;
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
         pMe->nRecordTotalTime += ((GETUPTIMEMS()/1000) - pMe->nRecordStartTime);
         pMe->nRecordStartTime = 0;
#endif
         Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_PREVIEW);
         Qcamcorder_DisplayRecordTime(pMe);
         QCAMCORDER_MSG_HIGH("Qcamcorder Recording Paused",0,0,0);
         break;

       case CAM_STATUS_RESUME:
         pMe->eState = QCAMCORDER_RECORDING;
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
         pMe->nRecordStartTime = (GETUPTIMEMS()/1000);
#endif
         Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_PREVIEW);
         Qcamcorder_DisplayRecordTime(pMe);
         QCAMCORDER_MSG_HIGH("Qcamcorder Recording Resuming",0,0,0);
         break;

       case CAM_STATUS_SPACE_ERROR:
       case CAM_STATUS_SPACE_ERROR_CHUNK_TABLE:
       case CAM_STATUS_SPACE_ERROR_SAMPLE_TABLE:
       case CAM_STATUS_SPACE_ERROR_MOVIE_DURATION:
       case CAM_STATUS_SPACE_ERROR_STORAGE:
       case CAM_STATUS_SPACE_ERROR_VIDEO:
         if(pMe->m_bCont)
         {
           pMe->bStopRecording = TRUE; /* Stop continuous mode */
         }

         /* Error event if recording has been initiated but the entire
         ** start record process did not complete due to some error */
         if (pMe->eState == QCAMCORDER_RECORDING && pMe->m_bPreview_to_Record)
         {
           pMe->m_bPreview_to_Record = FALSE;
         }

         Qcamcorder_DisplayWarningMessage(pMe, IDB_SPACE_REACHED);
         break;


       case CAM_STATUS_SPACE_WARNING:
       case CAM_STATUS_SPACE_WARNING_SAMPLE_TABLE:
       case CAM_STATUS_SPACE_WARNING_CHUNK_TABLE:
       case CAM_STATUS_SPACE_WARNING_STORAGE:
       case CAM_STATUS_SPACE_WARNING_MOVIE_DURATION:
         Qcamcorder_DisplayWarningMessage(pMe, IDB_SPACE_IMMINENT);
         break;

      case CAM_STATUS_FAIL:
      case CAM_STATUS_ABORT:
        if(pMe->m_bCont)
        {
          pMe->bStopRecording = TRUE; /* Stop continuous mode */
        }

        /* Error event if recording has been initiated but the entire
        ** start record process did not complete due to some error */
        if (pMe->eState == QCAMCORDER_RECORDING && pMe->m_bPreview_to_Record)
        {
          pMe->m_bPreview_to_Record = FALSE;
        }

        Qcamcorder_DisplayWarningMessage(pMe, IDB_CAM_ERROR);
        break;

      default:
         QCAMCORDER_MSG_HIGH("Case %d not handled",pcn->nStatus,0,0);
         break;
      }
   }
}
/* ==========================================================================
FUNCTION     Qcamcorder_AddUdatAtom
DESCRIPTION  User can set UDTA atom field values from this function
============================================================================= */
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
static void Qcamcorder_AddUdatAtom(Qcamcorder* pMe)
{
  CameraUdatAtomType *udat_atom = pMe->udat_atom;

  udat_atom = MALLOC(sizeof(CameraUdatAtomType));
  udat_atom->atom_is_valid = TRUE;
  
  udat_atom->author.isUTF16         = FALSE;
  udat_atom->author.length          = 7;
  udat_atom->author.language        = 10;
  udat_atom->author.string.utf8     = MALLOC(udat_atom->author.length);
 (void)STRLCPY((char *)udat_atom->author.string.utf8,"author",udat_atom->author.length);

  udat_atom->title.isUTF16          = FALSE;
  udat_atom->title.length           = 6;
  udat_atom->title.language         = 10;
  udat_atom->title.string.utf8      = MALLOC(udat_atom->title.length);
  (void)STRLCPY((char *)udat_atom->title.string.utf8,"title",udat_atom->title.length);

  udat_atom->description.isUTF16    = FALSE;
  udat_atom->description.length     = 12;
  udat_atom->description.language   = 10;
  udat_atom->description.string.utf8= MALLOC(udat_atom->description.length);
  (void)STRLCPY((char *)udat_atom->description.string.utf8,"description",udat_atom->description.length);

  udat_atom->copyright.isUTF16      = FALSE;
  udat_atom->copyright.length       = 10;
  udat_atom->copyright.language     = 10;
  udat_atom->copyright.string.utf8  = MALLOC(udat_atom->copyright.length);
  (void)STRLCPY((char *)udat_atom->copyright.string.utf8,"copyright",udat_atom->copyright.length);

  udat_atom->titl_auth_dscp_cprt_valid = TRUE;

  /* Role indicates role of the place 
	0 indicates "shooting location", 
	1 indicates "real location", 
	and 2 indicates "fictional location". Other values are reserved*/
  udat_atom->loci.role     			= 1;

  udat_atom->loci.name.isUTF16      = FALSE;
  udat_atom->loci.name.length       = 9;
  udat_atom->loci.name.language     = 10;
  udat_atom->loci.astr_body.isUTF16 = FALSE;
  udat_atom->loci.astr_body.length  = 6;
  udat_atom->loci.astr_body.language= 10;
  udat_atom->loci.add_notes.isUTF16 = FALSE;
  udat_atom->loci.add_notes.length  = 9;
  udat_atom->loci.add_notes.language= 10;
  udat_atom->loci.name.string.utf8       = MALLOC(udat_atom->loci.name.length);
  udat_atom->loci.astr_body.string.utf8  = MALLOC(udat_atom->loci.astr_body.length);
  udat_atom->loci.add_notes.string.utf8  = MALLOC(udat_atom->loci.add_notes.length);
  
  (void)STRLCPY((char *)udat_atom->loci.name.string.utf8,"sandiego",udat_atom->loci.name.length);
  (void)STRLCPY((char *)udat_atom->loci.astr_body.string.utf8,"earth",udat_atom->loci.astr_body.length);
  (void)STRLCPY((char *)udat_atom->loci.add_notes.string.utf8,"qualcomm",udat_atom->loci.add_notes.length);
 
  if(pMe->GPSInfo.dwLon < 0)
     udat_atom->loci.longitude = pMe->GPSInfo.dwLon * (-1);
  else
     udat_atom->loci.longitude = pMe->GPSInfo.dwLon;

  /* equivalent to (longitude * 360) / (2 << (26-1)) */
  udat_atom->loci.longitude = (udat_atom->loci.longitude * 45) / (2 << (23-1));

  if(pMe->GPSInfo.dwLat < 0) 
     udat_atom->loci.latitude = pMe->GPSInfo.dwLat * (-1); 
  else
     udat_atom->loci.latitude = pMe->GPSInfo.dwLat;

  /* equivalent to (latitude*180) / (2 << (25-1)); */
  udat_atom->loci.latitude = (udat_atom->loci.latitude*45) / (2 << (23-1));
  udat_atom->loci.altitude = pMe->GPSInfo.wAltitude;
  udat_atom->loci.loci_valid        = TRUE;
  
  (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_USER_ATOM, (int32)udat_atom, 0);

  FREEIF(udat_atom->description.string.utf8);
  FREEIF(udat_atom->author.string.utf8);
  FREEIF(udat_atom->title.string.utf8);
  FREEIF(udat_atom->copyright.string.utf8);
  FREEIF(udat_atom->loci.name.string.utf8);
  FREEIF(udat_atom->loci.astr_body.string.utf8);
  FREEIF(udat_atom->loci.add_notes.string.utf8);
  FREEIF(udat_atom);
}
#endif

/* ==========================================================================
FUNCTION QCAMCORDER_ERROREVENTUSERNOTIFY
DESCRIPTION
============================================================================= */
static void Qcamcorder_TimerClearScreen(Qcamcorder* pMe)
{
  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);

#ifdef FEATURE_MMOVERLAY
  pMe->b_displayingSpaceErrorMsg = FALSE;
#endif /* FEATURE_MMOVERLAY */

  /* Rebuild recording menu icons after clear screen */
  Qcamcorder_BuildPreviewMenu(pMe);

  /* Redisplay record timer and clip name */
  Qcamcorder_DisplayRecordTime(pMe);

}

/* ==========================================================================
FUNCTION QCAMCORDER_ERROREVENTUSERNOTIFY
DESCRIPTION
============================================================================= */
static void Qcamcorder_ErrorEventUserNotify(Qcamcorder* pMe)
{
  IImage       *pImage = NULL;
  AEEImageInfo pi      = {0};


#ifdef FEATURE_MMOVERLAY 
  if(pMe->msg == IDB_SPACE_NEAR 
     || pMe->msg == IDB_SPACE_IMMINENT 
     || pMe->msg == IDB_SPACE_REACHED)
  {
    pMe->b_displayingSpaceErrorMsg = TRUE;
  }
#endif /* FEATURE_MMOVERLAY */
  QCamcorder_MdpRegisterFullScreen(pMe, FALSE);

  //lint -e570  Suppress complaint on loss of sign
  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
  //lint +e570

  if((pImage = ISHELL_LoadResImage(pMe->a.m_pIShell,
                                   QCAMCORDER_RES_FILE,
                                   (uint16)(pMe->msg))) != NULL)
  {
    /* Get information of user display bitmap */
    IIMAGE_GetInfo(pImage, &pi);

    /* Center the bitmap */
    IIMAGE_Draw(pImage,
                (pMe->di.cxScreen - pi.cx)/2,
                (pMe->di.cyScreen - pi.cy)/2);

    (void)IIMAGE_Release(pImage);
    pImage = NULL;
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
  }

  /* Different recovery depending on type of error condition */
  switch(pMe->msg)
  {
    case IDB_STOPPED:
    case IDB_CAM_ERROR:
      if(ICAMERA_Stop(pMe->m_pICam) == SUCCESS)
      {
        (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1500,
                              (PFNNOTIFY)Qcamcorder_Preview,(void *)pMe);
      }
      pMe->eState = QCAMCORDER_START_PREVIEW;
      break;

    case IDB_POSTEDIT_ERROR:
    case IDB_POSTEDIT_UNSUPPORTED:
    case IDB_ERROR_QCAMCORDER:
      pMe->b_focusFirstItem = FALSE;
#ifdef FEATURE_VIDEO_ENCODE_RAW
         if(pMe->m_activeMenu == QCAMCORDER_MENU_RAWFILELIST)
         {
         (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1500,
                              (PFNNOTIFY)Qcamcorder_RawEncoderDisplayFileList,(void *)pMe);
         }
	  else
#endif /* FEATURE_VIDEO_ENCODE_RAW */	  
       if(pMe->eState == QCAMCORDER_RECORDING)
       {
           pMe->m_bPreview_to_Record = FALSE;
          (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1000,
                                  (PFNNOTIFY)Qcamcorder_Preview,(void *)pMe);
       }
       else
       {
          (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1500,
                            (PFNNOTIFY)Qcamcorder_DisplayFileList,(void *)pMe);
       }
      break;

    case IDB_SPACE_NEAR:
    case IDB_SPACE_IMMINENT:
      (void)ISHELL_SetTimer(pMe->a.m_pIShell, 500,
                            (PFNNOTIFY)Qcamcorder_TimerClearScreen,(void *)pMe);

      break;

    case IDB_SPACE_REACHED:
      pMe->eState = QCAMCORDER_START_PREVIEW;
      break;

  case IDB_BITRATE:

      pMe->wQuality = QCAMCORDER_QUALITY_HIGH;

     (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1000,
                            (PFNNOTIFY)Qcamcorder_ExecuteQualityMenu ,(void *)pMe);
      break;
  
    default:
      QCAMCORDER_MSG_ERROR("Unhandled service layer event %d",pMe->msg,0,0);
      break;

  }
}
#ifdef FEATURE_BREW_IMOVIEEDIT
static void Qcamcorder_CutReadOpenCB(void * pUser)
{
  Qcamcorder *pMe = (Qcamcorder *)pUser;
  QCAMCORDER_MSG_MED("Qcamcorder_CutReadOpenCB",0,0,0);
  if (pMe->m_movieeditStatus == AEEMOVIEEDIT_STATUS_DONE)
  {
       AEEMovieEditConfig mec;
       AEEMediaData dest;
       mec.movieTimescale = pMe->m_mei.movieTimescale;
       mec.videoTimescale = pMe->m_mei.videoTimescale;
       mec.videoBrand = pMe->m_mei.videoBrand;
       mec.videoWidth = pMe->m_mei.videoWidth;
       mec.videoHeight = pMe->m_mei.videoHeight;
       mec.videoFormat = pMe->m_mei.videoFormat;
       mec.videoBitrate = pMe->m_mei.videoAvgBitrate;
       mec.videoHeaderSize = pMe->m_mei.videoHeaderSize;
       mec.frame_rate  = pMe->m_mei.frame_rate;
       memcpy(mec.videoHeader,
              pMe->m_mei.videoHeader,
              (uint32)mec.videoHeaderSize);
// test only
       mec.audioFormat = pMe->m_mei.audioFormat;
//       mec.audioFormat = 0;
// end test only
       mec.audioTimescale = pMe->m_mei.audioTimescale;
       mec.adjustEStimestamp = TRUE;

       if (pMe->m_mei.textTotalSample > 0)
       {
         mec.textFormat = AEEMOVIEEDIT_TEXT_FORMAT_3GPP;
       }
       else
       {
         mec.textFormat = AEEMOVIEEDIT_TEXT_FORMAT_NONE;
       }

#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
       {
         dest.clsData = MMD_FILE_NAME;
         dest.pData = pMe->sVideoEditFileName;
         CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_CutWriteOpenCB, pMe);
         if (IMOVIEEDIT_WriteOpen(pMe->m_pOutEdit, &dest,
             &mec, &(pMe->m_movieeditStatus),
             &(pMe->m_movieeditCB)) != SUCCESS)
         {
           QCAMCORDER_MSG_ERROR("VIdeoEditing cannot open output file",0,0,0);
           QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
            pMe->eState = QCAMCORDER_VIDEO_EDIT_UNSUPPORTED;
           CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_CutReadCloseCB, pMe);
           if (IMOVIEEDIT_ReadClose(pMe->m_pInEdit, &(pMe->m_movieeditStatus),
                                    &(pMe->m_movieeditCB)) != SUCCESS)
           {
             QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
             pMe->eState = QCAMCORDER_VIDEO_EDIT_UNSUPPORTED;
             if(!ISHELL_SendEvent(pMe->a.m_pIShell,AEECLSID_QCAMCORDER,
                                   EVT_USER,QCC_EXECUTE_CONFIRM_MENU_EVENT, 0))
             {
               QCAMCORDER_MSG_ERROR("Not able to send the event",0,0,0);
               Qcamcorder_DisplayFileList(pMe);
             }
           }
         }
       }
  }
  else
  {
       QCAMCORDER_MSG_HIGH("Case %d not handled",pMe->m_movieeditStatus,0,0);
  }
}

static void Qcamcorder_CutWriteOpenCB(void * pUser)
{
  Qcamcorder *pMe = (Qcamcorder *)pUser;
  QCAMCORDER_MSG_MED("Qcamcorder_CutWriteOpenCB",0,0,0);
  if (pMe->m_movieeditStatus == AEEMOVIEEDIT_STATUS_DONE)
  {
    CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_MovieEditCutCB, pMe);
    if (IMOVIEEDIT_Cut(pMe->m_pOutEdit, pMe->m_pInEdit,
      pMe->m_nEditPortion, pMe->m_cutInfo, &(pMe->m_movieeditStatus),
      &(pMe->m_movieeditCB)) != SUCCESS)
    {
      QCAMCORDER_MSG_ERROR("VIdeoEditing cannot perform cut operation",0,0,0);
      CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_CutWriteCloseCB, pMe);
      pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
      if (IMOVIEEDIT_WriteClose(pMe->m_pInEdit, &(pMe->m_movieeditStatus),
        &(pMe->m_movieeditCB)) != SUCCESS)
      {
        QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
        CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_CutReadCloseCB, pMe);
        if (IMOVIEEDIT_ReadClose(pMe->m_pInEdit, &(pMe->m_movieeditStatus),
          &(pMe->m_movieeditCB)) != SUCCESS)
        {
          QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
          pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
           if(!ISHELL_SendEvent(pMe->a.m_pIShell,AEECLSID_QCAMCORDER,
                                  EVT_USER,QCC_EXECUTE_CONFIRM_MENU_EVENT, 0))
           {
             QCAMCORDER_MSG_ERROR("Not able to send the event",0,0,0);
             Qcamcorder_DisplayFileList(pMe);
           }
        }
      }
    }
    FREEIF(pMe->m_cutInfo);
  }
  else
  {
    QCAMCORDER_MSG_HIGH("Case %d not handled",pMe->m_movieeditStatus,0,0);
  }
}

static void Qcamcorder_MovieEditCutCB(void * pUser)
{
  Qcamcorder *pMe = (Qcamcorder *)pUser;
  QCAMCORDER_MSG_MED("Qcamcorder_MovieEditCutCB",0,0,0);
  if  (pMe->m_movieeditStatus != AEEMOVIEEDIT_STATUS_DONE)
    pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
  CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_CutWriteCloseCB, pMe);
  if (IMOVIEEDIT_WriteClose(pMe->m_pOutEdit, &(pMe->m_movieeditStatus),
    &(pMe->m_movieeditCB)) != SUCCESS)
  {
     QCAMCORDER_MSG_ERROR("VIdeoEditing close output file",0,0,0);
     QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
     pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
     CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_CutReadCloseCB, pMe);
     if (IMOVIEEDIT_ReadClose(pMe->m_pInEdit, &(pMe->m_movieeditStatus),
       &(pMe->m_movieeditCB)) != SUCCESS)
     {
       QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
       pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
       if(!ISHELL_SendEvent(pMe->a.m_pIShell,AEECLSID_QCAMCORDER,
                            EVT_USER,QCC_EXECUTE_CONFIRM_MENU_EVENT, 0))
       {
         QCAMCORDER_MSG_ERROR("Not able to send the event",0,0,0);
         Qcamcorder_DisplayFileList(pMe);
       }
     }
  }
}

static void Qcamcorder_CutWriteCloseCB(void * pUser)
{
  Qcamcorder *pMe = (Qcamcorder *)pUser;
  QCAMCORDER_MSG_MED("Qcamcorder_CutWriteCloseCB",0,0,0);
  QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
  CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_CutReadCloseCB, pMe);
  if (IMOVIEEDIT_ReadClose(pMe->m_pInEdit, &(pMe->m_movieeditStatus),
      &(pMe->m_movieeditCB)) != SUCCESS)
  {
       QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
       pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE_ERROR;
       if(!ISHELL_SendEvent(pMe->a.m_pIShell,AEECLSID_QCAMCORDER,
                            EVT_USER,QCC_EXECUTE_CONFIRM_MENU_EVENT, 0))
       {
         QCAMCORDER_MSG_ERROR("Not able to send the event",0,0,0);
         Qcamcorder_DisplayFileList(pMe);
       }
  }
}

static void Qcamcorder_CutReadCloseCB(void * pUser)
{
  Qcamcorder *pMe = (Qcamcorder *)pUser;
  QCAMCORDER_MSG_MED("Qcamcorder_CutReadCloseCB",0,0,0);
  QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
  if (pMe->eState != QCAMCORDER_VIDEO_EDIT_DONE_ERROR && 
      pMe->eState != QCAMCORDER_VIDEO_EDIT_UNSUPPORTED)
    pMe->eState = QCAMCORDER_VIDEO_EDIT_DONE;
  if(!ISHELL_SendEvent(pMe->a.m_pIShell,AEECLSID_QCAMCORDER,
                         EVT_USER,QCC_EXECUTE_CONFIRM_MENU_EVENT, 0))
  {
    QCAMCORDER_MSG_ERROR("Not able to send the event",0,0,0);
    Qcamcorder_DisplayFileList(pMe);
  }
}
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
static void Qcamcorder_AudioDubReadOpenCB(void * pUser)
{
  Qcamcorder *pMe = (Qcamcorder *)pUser;
  QCAMCORDER_MSG_MED("Qcamcorder_AudioDubReadOpenCB",0,0,0);
  if (pMe->m_movieeditStatus == AEEMOVIEEDIT_STATUS_DONE)
  {
      char *pData = (void *)QCamcorder_GetAuddubfileName(pMe);
      AEEMediaData dest;
      AEEMovieEditConfig mec;
      mec.movieTimescale = pMe->m_mei.movieTimescale;
      mec.videoTimescale = pMe->m_mei.videoTimescale;
      mec.videoBrand = pMe->m_mei.videoBrand;
      mec.videoWidth = pMe->m_mei.videoWidth;
      mec.videoHeight = pMe->m_mei.videoHeight;
      mec.videoFormat = pMe->m_mei.videoFormat;
      mec.videoBitrate = pMe->m_mei.videoAvgBitrate;
      mec.videoHeaderSize = pMe->m_mei.videoHeaderSize;
      mec.frame_rate  = pMe->m_mei.frame_rate;
      memcpy(
        mec.videoHeader, pMe->m_mei.videoHeader, (uint32)mec.videoHeaderSize);
      mec.audioTimescale = pMe->m_mei.audioTimescale;
      mec.adjustEStimestamp = TRUE;
      switch (pMe->wAudioFormatUsed)
      {
      case MM_QCP_FORMAT_FIXED_FULL_13K:
        mec.audioFormat = AEEMOVIEEDIT_AUDIO_FORMAT_FIXED_FULL_13K;
        break;
      case MM_QCP_FORMAT_FIXED_HALF_13K:
        mec.audioFormat = AEEMOVIEEDIT_AUDIO_FORMAT_FIXED_HALF_13K;
        break;
      case MM_QCP_FORMAT_FIXED_FULL_EVRC:
        mec.audioFormat = AEEMOVIEEDIT_AUDIO_FORMAT_FIXED_FULL_EVRC;
        break;
      case MM_QCP_FORMAT_FIXED_12_2_AMR:
        mec.audioFormat = AEEMOVIEEDIT_AUDIO_FORMAT_FIXED_12_2_AMR;
        break;
      default:
        mec.audioFormat = AEEMOVIEEDIT_AUDIO_FORMAT_NONE;
        break;
      }
      if (pMe->m_mei.textTotalSample > 0)
      {
        mec.textFormat = AEEMOVIEEDIT_TEXT_FORMAT_3GPP;
      }
      else
      {
        mec.textFormat = AEEMOVIEEDIT_TEXT_FORMAT_NONE;
      }

      dest.clsData = MMD_FILE_NAME;
      dest.pData = pData;
      CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_AudioDubWriteOpenCB, pMe);
      if (IMOVIEEDIT_WriteOpen(pMe->m_pOutEdit, &dest,
        &mec, &(pMe->m_movieeditStatus),
        &(pMe->m_movieeditCB)) != SUCCESS)
      {
        QCAMCORDER_MSG_ERROR("Audio Dubbing cannot open output file",0,0,0);
        QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
        CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_AudioDubReadCloseCB, pMe);
        if (IMOVIEEDIT_ReadClose(pMe->m_pInEdit, &(pMe->m_movieeditStatus),
            &(pMe->m_movieeditCB)) != SUCCESS)
        {
          QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
          pMe->eState = QCAMCORDER_EDIT; // Carol
          Qcamcorder_BuildEditMenu(pMe);
        }
      }
  }
  else
  {
      QCAMCORDER_MSG_HIGH("Case %d not handled",pMe->m_movieeditStatus,0,0);
  }
 }

static void Qcamcorder_AudioDubWriteOpenCB(void * pUser)
{
  Qcamcorder *pMe = (Qcamcorder *)pUser;
  AEEMediaData md;

  QCAMCORDER_MSG_MED("Qcamcorder_AudioDubWriteOpenCB",0,0,0);
  md.clsData = MMD_FILE_NAME;
  md.pData = QCAMCORDER_TMP_AUDIO_FILE;
  pMe->m_nEditPortion = 1;
  pMe->m_auddubInfo =
    MALLOC(sizeof(AEEMovieEditAudioDubInfo)*pMe->m_nEditPortion);
  pMe->m_auddubInfo[0].pAudioData = &md;
//  pMe->m_auddubInfo[0].dwStartTime = 0;
//  pMe->m_auddubInfo[0].dwEndTime = 4;
  CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_MovieEditAudioDubCB, pMe);
  if (IMOVIEEDIT_AudioDub(pMe->m_pOutEdit, pMe->m_pInEdit, TRUE,
    pMe->m_nEditPortion, pMe->m_auddubInfo, &(pMe->m_movieeditStatus),
    &(pMe->m_movieeditCB)) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("Movieedit Audio Dub operation error",0,0,0);
  }
  FREEIF(pMe->m_auddubInfo);
}

static void Qcamcorder_MovieEditAudioDubCB(void * pUser)
{
  Qcamcorder *pMe = (Qcamcorder *)pUser;
  QCAMCORDER_MSG_MED("Qcamcorder_MovieEditAudioDubCB",0,0,0);
  CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_AudioDubWriteCloseCB, pMe);
  if (IMOVIEEDIT_WriteClose(pMe->m_pOutEdit, &(pMe->m_movieeditStatus),
      &(pMe->m_movieeditCB)) != SUCCESS)
  {
    QCAMCORDER_MSG_ERROR("Movieedit Audio Dub write close error",0,0,0);
  }
}

static void Qcamcorder_AudioDubWriteCloseCB(void * pUser)
{
    Qcamcorder *pMe = (Qcamcorder *)pUser;
    QCAMCORDER_MSG_MED("Qcamcorder_AudioDubWriteCloseCB",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
    CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_AudioDubReadCloseCB, pMe);
    if (IMOVIEEDIT_ReadClose(pMe->m_pInEdit, &(pMe->m_movieeditStatus),
      &(pMe->m_movieeditCB)) != SUCCESS)
    {
      QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
      (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_AUDIO_FILE);
      pMe->AudioDubbing = FALSE;
    }
}

static void Qcamcorder_AudioDubReadCloseCB(void * pUser)
{
    Qcamcorder *pMe = (Qcamcorder *)pUser;
    QCAMCORDER_MSG_MED("Qcamcorder_AudioDubReadCloseCB",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
    (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_AUDIO_FILE);
    pMe->AudioDubbing = FALSE;
}
#endif //FEATURE_VIDEO_AUDIO_DUBBING

static void Qcamcorder_PostEditReadOpenCB(void * pUser)
{
    Qcamcorder *pMe = (Qcamcorder *)pUser;
    QCAMCORDER_MSG_MED("Qcamcorder_PostEditReadOpenCB",0,0,0);
    if (pMe->m_movieeditStatus == AEEMOVIEEDIT_STATUS_DONE)
    {
      AEEMovieEditConfig mec;
      AEEMediaData dest;
      mec.movieTimescale = pMe->m_mei.movieTimescale;
      mec.videoTimescale = pMe->m_mei.videoTimescale;
      mec.videoBrand = pMe->m_mei.videoBrand;
      mec.videoWidth = pMe->m_mei.videoWidth;
      mec.videoHeight = pMe->m_mei.videoHeight;
      mec.videoFormat = pMe->m_mei.videoFormat;
      mec.videoBitrate = pMe->m_mei.videoAvgBitrate;
      mec.videoHeaderSize = pMe->m_mei.videoHeaderSize;
	  mec.frame_rate = pMe->m_mei.frame_rate;
      memcpy(mec.videoHeader,
             pMe->m_mei.videoHeader,
             (uint32)mec.videoHeaderSize);
      mec.audioFormat = pMe->m_mei.audioFormat;
      mec.audioTimescale = pMe->m_mei.audioTimescale;
      if (pMe->m_mei.textTotalSample > 0)
      {
        mec.textFormat = AEEMOVIEEDIT_TEXT_FORMAT_3GPP;
      }
      else
      {
        mec.textFormat = AEEMOVIEEDIT_TEXT_FORMAT_NONE;
      }
      
      dest.clsData = MMD_FILE_NAME;
      dest.pData = pMe->sVideoEditFileName;

      CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_PostEditWriteOpenCB, pMe);
      if (IMOVIEEDIT_WriteOpen(pMe->m_pOutEdit, &dest,
                           &mec, &(pMe->m_movieeditStatus),
                           &(pMe->m_movieeditCB)) != SUCCESS)
      {
        QCAMCORDER_MSG_ERROR("PostEditing cannot open output file",0,0,0);
        pMe->bSaveInPrg = FALSE;
        QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
        CALLBACK_Init(&pMe->m_movieeditCB,Qcamcorder_PostEditReadCloseCB, pMe);
        if (IMOVIEEDIT_ReadClose(pMe->m_pInEdit, &(pMe->m_movieeditStatus),
                                 &(pMe->m_movieeditCB)) != SUCCESS)
        {
          QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
        }
      }
    }
    else
    {
      QCAMCORDER_MSG_HIGH("Case %d not handled",pMe->m_movieeditStatus,0,0);
    }
}

static void Qcamcorder_PostEditWriteOpenCB(void * pUser)
{
      Qcamcorder *pMe = (Qcamcorder *)pUser;
      uint32 i=0;
      uint32 * pExtensionSize;

      QCAMCORDER_MSG_MED("Qcamcorder_PostEditWriteOpenCB",0,0,0);
      pExtensionSize = MALLOC(pMe->m_mei.extensionTotal * sizeof(uint32));
      (void)IMOVIEEDIT_ReadExtensionSize(pMe->m_pInEdit, 0,
        pMe->m_mei.extensionTotal, pExtensionSize);
      pMe->m_extension = MALLOC(pMe->m_mei.extensionTotal *
        sizeof(AEEMovieEditExtension));
      for (i=0; i<pMe->m_mei.extensionTotal; i++)
      {
        pMe->m_extension[i].pUuid = MALLOC(16);
        pMe->m_extension[i].pData = MALLOC(pExtensionSize[i]);
      }
      CALLBACK_Init(
        &pMe->m_movieeditCB, Qcamcorder_PostEditReadExtensionCB, pMe);
      (void)IMOVIEEDIT_ReadExtension(pMe->m_pInEdit,
                                     0,
                                     pMe->m_mei.extensionTotal,
                                     pMe->m_extension,
                                     &(pMe->m_movieeditStatus),
                                     &(pMe->m_movieeditCB));
}

static void Qcamcorder_PostEditReadExtensionCB(void * pUser)
{
      Qcamcorder *pMe = (Qcamcorder *)pUser;
      uint32 i, j=0;
      uint32 size[10];
      uint8 *pUuid[10];
      uint8 *pData[10];
      uint8 *gpsData=NULL;
      AEEMovieEditExtension *extension;

      QCAMCORDER_MSG_MED("Qcamcorder_PostEditReadExtensionCB",0,0,0);
      for (i=0; i<pMe->m_mei.extensionTotal; i++)
      {
        if ( (MEMCMP(pMe->m_extension[i].pUuid, UUID_CPGD, 16) == 0)
          && (pMe->eGPSState == QCAMCORDER_GPS_DATA_VALID) )
        { // do not copy GPS info from input file as we have a new one
          continue;
        }
        if ( (MEMCMP(pMe->m_extension[i].pUuid, UUID_TSML, 16) == 0)
          && ((pMe->m_pPostEditTelopData!=NULL)
          && (pMe->m_nPostEditTelopSize>0)) )
        { // do not copy telop info from input file as we have a new one
          continue;
        }
        size[j] = pMe->m_extension[i].size;
        pUuid[j] = pMe->m_extension[i].pUuid;
        pData[j] = pMe->m_extension[i].pData;
        j++;
      }

      if ((pMe->m_pPostEditTelopData!=NULL) && (pMe->m_nPostEditTelopSize>0))
      {
        pUuid[j] = (uint8 *)UUID_TSML;
        size[j] = pMe->m_nPostEditTelopSize;
        pData[j] = pMe->m_pPostEditTelopData;
        j++;
      }

      if (pMe->eGPSState == QCAMCORDER_GPS_DATA_VALID)
      {
        pUuid[j] = (uint8 *)UUID_CPGD;
        size[j] = sizeof(gpsi_struct);
        pData[j] = gpsData = MALLOC(size[j]);
        Qcamcorder_FillGPSData(pMe, (gpsi_struct *)gpsData);
        j++;
      }

      extension = (AEEMovieEditExtension *)MALLOC(j * sizeof(AEEMovieEditExtension));
      for (i=0; i<j; i++)
      {
        extension[i].pUuid = pUuid[i]; /*lint !e771 */
        extension[i].size = size[i];   /*lint !e771 */
        extension[i].pData = pData[i]; /*lint !e771 */
      }
      CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_MovieEditPostEditCB, pMe);
      if (IMOVIEEDIT_PostEdit(pMe->m_pOutEdit, pMe->m_pInEdit,j,
        extension, &(pMe->m_movieeditStatus), &(pMe->m_movieeditCB)) != SUCCESS)
      {
        QCAMCORDER_MSG_ERROR("Movieedit post edit operation error",0,0,0);
        pMe->bSaveInPrg = FALSE;
      }
      FREEIF(gpsData);
      FREEIF(extension);
}

static void Qcamcorder_MovieEditPostEditCB(void * pUser)
{
    Qcamcorder *pMe = (Qcamcorder *)pUser;
    QCAMCORDER_MSG_MED("Qcamcorder_MovieEditPostEditCB",0,0,0);
    CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_PostEditWriteCloseCB, pMe);
    if (IMOVIEEDIT_WriteClose(pMe->m_pOutEdit, &(pMe->m_movieeditStatus),
      &(pMe->m_movieeditCB)) != SUCCESS)
    {
      QCAMCORDER_MSG_ERROR("Movieedit post edit write close error",0,0,0);
      pMe->bSaveInPrg = FALSE;
    }
}

static void Qcamcorder_PostEditWriteCloseCB(void * pUser)
{
    Qcamcorder *pMe = (Qcamcorder *)pUser;
    QCAMCORDER_MSG_MED("Qcamcorder_PostEditWriteCloseCB",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
    CALLBACK_Init(&pMe->m_movieeditCB, Qcamcorder_PostEditReadCloseCB, pMe);
    if (IMOVIEEDIT_ReadClose(pMe->m_pInEdit, &(pMe->m_movieeditStatus),
      &(pMe->m_movieeditCB)) != SUCCESS)
    {
      QCAMCORDER_MSG_ERROR("Movieedit post edit read close error",0,0,0);
      pMe->bSaveInPrg = FALSE;
    }
}

static void Qcamcorder_PostEditReadCloseCB(void * pUser)
{
    Qcamcorder *pMe = (Qcamcorder *)pUser;
    QCAMCORDER_MSG_MED("Qcamcorder_PostEditReadCloseCB",0,0,0);
    QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
    pMe->bSaveInPrg = FALSE;
}
#endif /* FEATURE_BREW_IMOVIEEDIT */


/* ==========================================================================
FUNCTION QCAM_DISPLAYSELFTIMERBAR
DESCRIPTION
============================================================================= */
static void Qcamcorder_DisplaySelfTimerBar(Qcamcorder* pMe)
{
   AEERect bar;
   AEERect rc;
   AECHAR szBuf[TEXT_STRING_SIZE] = {0};
   char cBuf[TEXT_STRING_SIZE];
   AECHAR acBuf[TEXT_STRING_SIZE];
   int len;

   rc.x   = 0;
   rc.y   = 0;
   rc.dx  = (int16)(pMe->di.cxScreen);
   rc.dy  = QCAMCORDER_PICTURE_Y_POS;
   bar.x  = 88;
   bar.y  = 4;
   bar.dx = (int16)(pMe->di.cxScreen - bar.x - 5);
   bar.dy = 5;

   if(pMe->m_activeMenu != QCAMCORDER_MENU_GPS       ||
      pMe->eState       == QCAMCORDER_PREVIEW_TO_GPS ||
      pMe->eState       != QCAMCORDER_NULL)
   {

     //lint -e570  Suppress complaint on loss of sign
     IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
     //lint +e570

     rc.x = 2;
     (void)ISHELL_LoadResString(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, IDS_START, szBuf, sizeof(szBuf));
     len = WSTRSIZE(szBuf) / 2;

     (void)SNPRINTF(cBuf, sizeof(cBuf)," %2d", pMe->nLeftSec, sizeof(cBuf));
     (void)STR_TO_WSTR(cBuf, acBuf, sizeof(szBuf));

     (void)WSTRNCOPY(&(szBuf[len - 1]), TEXT_STRING_SIZE - len, acBuf);

     (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szBuf, -1, 0, 1, &rc, IDF_ALIGN_LEFT);

     IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);

     Qcamcorder_selfTimerCB(pMe);
   }
}

/* ==========================================================================
FUNCTION QCAM_SELFTIMERCB
DESCRIPTION
============================================================================= */
static void Qcamcorder_selfTimerCB(Qcamcorder* pMe)
{
  if (pMe->nLeftSec)
  {
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1000,
                          (PFNNOTIFY)Qcamcorder_DisplaySelfTimerBar,
                          (void *)pMe);
    pMe->nLeftSec--;
  }
  else if (STRCMP(pMe->sStartSoundFile, "No Sound") == 0)
  {
    if(pMe->m_bTelop)                   /* If telop is enabled, preview has */
      Qcamcorder_RecordTelopTimer(pMe); /* already stopped, start recording */
    else                                /* If telop disabled, preview has   */
      Qcamcorder_RecordTimer(pMe);      /* not stopped, stop preview first  */
  }
  else
  {
    Qcamcorder_PlayStartSound(pMe);
  }
}

/* ==========================================================================
FUNCTION QCAMCORDER_UPDATEGPSICON
DESCRIPTION
============================================================================= */
static void Qcamcorder_UpdateGPSIcon(Qcamcorder *pMe)
{
   if ((pMe->m_activeSoftMenu == QCAMCORDER_MENU_PREVIEW) &&
      (pMe->m_activeMenu != QCAMCORDER_MENU_GPS))
   {
      CtlAddItem pai = {0};
      if(IMENUCTL_GetItem(pMe->m_pIPreview, IDS_GPS_SETUP, &pai))
      {
         pai.pszResImage = pai.pszResText = QCAMCORDER_RES_FILE;
         pai.pImage = NULL;
         pai.wImage = (uint16)(pMe->eGPSState == QCAMCORDER_GPS_DATA_VALID ? IDB_GPS_ACTIVE : IDB_GPS_INACTIVE);
         (void)IMENUCTL_SetItem(pMe->m_pIPreview, IDS_GPS_SETUP, MSIF_IMAGE, &pai);
         (void)IMENUCTL_Redraw(pMe->m_pIPreview);
         Qcamcorder_DisplayPreviewMenuName(pMe, 0);
      }
   }
   return;
} /* END QCam_UpdateGPSIcon */

/* ==========================================================================
FUNCTION QCAMCORDER_GETGPSINFOCB
DESCRIPTION
============================================================================= */
static void Qcamcorder_GetGPSInfoCB(void * pUser)
{
   Qcamcorder *pMe = (Qcamcorder *)pUser;
   boolean retry = FALSE;

   if (pMe->eGPSState == QCAMCORDER_GPS_ACQUIRING)
   {
      switch (pMe->GPSInfo.status)
      {
      case AEEGPS_ERR_NO_ERR:
         pMe->eGPSState = QCAMCORDER_GPS_DATA_VALID;
         break;
      case AEEGPS_ERR_GENERAL_FAILURE:
         pMe->eGPSState = QCAMCORDER_GPS_FAILURE;
         break;
      case AEEGPS_ERR_TIMEOUT:
         pMe->eGPSState = QCAMCORDER_GPS_TIMEOUT;
         break;
      case AEEGPS_ERR_ACCURACY_UNAVAIL:
         if (pMe->nGPSAccuracy > AEEGPS_ACCURACY_LOWEST)
         {
            pMe->nGPSAccuracy--;
            retry = TRUE;
         }
         else
         {
            pMe->eGPSState = QCAMCORDER_GPS_FAILURE;
         }
         break;
      case AEEGPS_ERR_INFO_UNAVAIL:
         pMe->eGPSState = QCAMCORDER_GPS_INFO_UNAVAILABLE;
         break;
      default: // Other general error
        pMe->eGPSState = QCAMCORDER_GPS_FAILURE;
        break;
      }

      // Re-draw the screen to clear messages due to GPS operations
//      Qcamcorder_Preview(pMe);

      if(pMe->eState == QCAMCORDER_POSTEDIT)
      {
        (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
      }
      else
      {
        (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
      }

      Qcamcorder_UpdateGPSIcon(pMe);
   }
   if (retry)
   {
      Qcamcorder_GetGPSInfo(pMe);
   }
   else if (pMe->pIPosDet)
   {
      (void)IPOSDET_Release(pMe->pIPosDet);
      pMe->pIPosDet = NULL;
   }
}

/* ==========================================================================
FUNCTION QCAMCORDER_GETGPSINFO
DESCRIPTION
============================================================================= */
#define CAMCORDER_GPS_QOS_VALUE 89 /* 89sec of Qos value for standalone GPS */

static void Qcamcorder_GetGPSInfo(Qcamcorder *pMe)
{
   if (pMe->eGPSState == QCAMCORDER_GPS_ENABLED)
   {
      // Get GPS info
      if (pMe->pIPosDet == NULL)
      {
         (void)ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_POSDET, (void **)&pMe->pIPosDet);
      }
      if (pMe->pIPosDet)
      {
         int result = 0;
         AEEGPSConfig config = {0};

         /* Draw an hour glass till gps info is available */
         Qcamcorder_DrawBusyImage(pMe);

         pMe->eGPSState = QCAMCORDER_GPS_ACQUIRING;

         /* Changing the default value (20 sec) of Qos Timer by new value */
         result = IPOSDET_GetGPSConfig(pMe->pIPosDet, &config);
         QCAMCORDER_MSG_HIGH("Get GPS Config..result is %d",result,0,0);
         if (result != SUCCESS)
         {
            (void)IPOSDET_Release(pMe->pIPosDet);
            pMe->pIPosDet = NULL;
            pMe->eGPSState = QCAMCORDER_GPS_DISABLED;

            /* Restart preview if attempt to get GPS fails */
            (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
            return;
         }

         /* Set the new qos value */
         config.qos = CAMCORDER_GPS_QOS_VALUE; 
         result = IPOSDET_SetGPSConfig(pMe->pIPosDet, &config);
         QCAMCORDER_MSG_HIGH("Set GPS Config..result is %d",result,0,0);
         if (result != SUCCESS)
         {
            (void)IPOSDET_Release(pMe->pIPosDet);
            pMe->pIPosDet = NULL;
            pMe->eGPSState = QCAMCORDER_GPS_DISABLED;

            /* Restart preview if attempt to get GPS fails */
            (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
            return;
         }

         CALLBACK_Init(&pMe->GPSInfoCB, Qcamcorder_GetGPSInfoCB, pMe);
         result = IPOSDET_GetGPSInfo(pMe->pIPosDet,
            AEEGPS_GETINFO_LOCATION | AEEGPS_GETINFO_ALTITUDE,
            pMe->nGPSAccuracy,  &pMe->GPSInfo, &pMe->GPSInfoCB);

         if (result != SUCCESS)
         {
            (void)IPOSDET_Release(pMe->pIPosDet);
            pMe->pIPosDet = NULL;
            pMe->eGPSState = QCAMCORDER_GPS_DISABLED;

            /* Restart preview if attempt to get GPS fails */
            (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_PREVIEW, 0);
         }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      }
   }
}

/* ==========================================================================
FUNCTION QCAMCORDER_GETDIRECTORY
DESCRIPTION Returns the correct directory to save to/load from
============================================================================= */
static const char * Qcamcorder_GetDirectory(Qcamcorder * pMe)
{
   if (IFILEMGR_Test(pMe->pIFileMgr, AEEDirList[pMe->saveLoc]) == SUCCESS)
   {
     return AEEDirList[pMe->saveLoc];
   }
   else
   {
     return QCAMCORDER_MOVIES_DIR_FLASH;
   }
}
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
/* ==========================================================================
FUNCTION QCAMCORDER_DELETEFILERESPONSE
DESCRIPTION
============================================================================= */
static void Qcamcorder_EditFileResponse(Qcamcorder * pMe, uint16 id)
{
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
   //lint +e570
   if (id == IDS_QCAMCORDER_NO)
   {
      if(pMe->eState != QCAMCORDER_TRANSPORTION)
        (void)IFILEMGR_Remove(pMe->pIFileMgr, pMe->sVideoEditFileName);
#ifdef FEATURE_MP4_TRANSCODER
      else
        (void)IFILEMGR_Remove(pMe->pIFileMgr, pMe->sTranscodeOutFile);
#endif
   }

   if(Qcamcorder_GetFilecount(pMe) >0)
   {
     pMe->b_focusFirstItem = TRUE;
     Qcamcorder_DisplayFileList(pMe);
   }

   else
   {  IMENUCTL_SetActive(pMe->m_pIPreview, TRUE);
      (void)ISHELL_PostEvent (pMe->a.m_pIShell, AEECLSID_QCAMCORDER, EVT_COMMAND, IDS_PREVIEW, (uint32) pMe);
   }
}
#endif
/* ==========================================================================
FUNCTION QCAMCORDER_DELETEFILERESPONSE
DESCRIPTION
============================================================================= */
void Qcamcorder_DeleteFileResponse(Qcamcorder * pMe, uint16 id)
{
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
   //lint +e570
   if (id == IDS_QCAMCORDER_YES)
   {
      (void)IFILEMGR_Remove(pMe->pIFileMgr, pMe->sCurrentFileName);
      (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_FILE);
      (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_MMC_FILE);
#ifdef FEATURE_EFS_FTL
      (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_FTL_FILE);
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
      (void)IFILEMGR_Remove(pMe->pIFileMgr, QCAMCORDER_TMP_VIDEO_USB_FILE);
#endif
   }

   if(Qcamcorder_GetFilecount(pMe) >0)
   {
     pMe->b_focusFirstItem = TRUE;
     Qcamcorder_DisplayFileList(pMe);
   }
   else
   {  IMENUCTL_SetActive(pMe->m_pIPreview, TRUE);
      (void)ISHELL_PostEvent (pMe->a.m_pIShell, AEECLSID_QCAMCORDER, EVT_COMMAND, IDS_PREVIEW, (uint32) pMe);
   }
}
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
/* the following function checks for a valid png file as well as 
 * whether the frame size matches encode size or not.
 * currently camcorder only supports landscape overlays
 * We parse the following from the start of file: 
 *  8 bytes: signature of png file
 *  4 bytes: length of chunk
 *  4 bytes: chunk type (should be IHDR)
 *  4 bytes: Width
 *  4 bytes: Height
*/
boolean Qcamcorder_validPNG(Qcamcorder* pMe, const char* pFile)
{
  IFile *pSrc = NULL;
  uint8 data[24];
  // signature of .png file
  const uint8 pngsign[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
  // IHDR chunk type
  const uint8 ihdr[] = {'I','H','D','R'};
  uint32  width,height;
  pSrc = IFILEMGR_OpenFile(pMe->pIFileMgr, pFile, _OFM_READ);
  if (pSrc)
  {
     if (IFILE_Read(pSrc, data, 24) == 24
         && MEMCMP(data,pngsign,8) == 0
         && MEMCMP(&data[12],ihdr,4) == 0)
     {
       width = data[16] << 24 | data[17] << 16 | data[18] << 8 | data[19];
       height = data[20] << 24 | data[21] << 16 | data[22] << 8 | data[23];
       if(width == QCam_Sizes[pMe->eSize].cy 
          && height == QCam_Sizes[pMe->eSize].cx)
       {
          (void)IFILE_Release(pSrc);
          return TRUE;
       }
     }
     (void)IFILE_Release(pSrc);
  }
  return FALSE;
}
/* This function clears overlay file name from the menu */
static void Qcamcorder_ClearOverlayFileName(Qcamcorder *pMe, AEERect *rc)
{
  SETAEERECT (rc, QCAMCORDER_OVERLAY_NAME_X, pMe->di.cyScreen - 
              QCAMCORDER_SOFT_MENU_HEIGHT - QCAMCORDER_OVERLAY_NAME_HEIGHT,
              pMe->di.cxScreen - QCAMCORDER_OVERLAY_NAME_X, QCAMCORDER_OVERLAY_NAME_HEIGHT);
  IDISPLAY_EraseRect(pMe->m_overlayDisplay, rc);
}

/* ==========================================================================
FUNCTION QCAMCORDER_LOADFRAME
DESCRIPTION
============================================================================= */

static int32 Qcamcorder_LoadFrame(Qcamcorder *pMe)
{
    IBitmap *bitmap;
    byte* pBmp;
    uint16 cx = QCam_Sizes[pMe->eSize].cy;
    uint16 cy = QCam_Sizes[pMe->eSize].cx;
    boolean allocated_before = TRUE;
	
    if (!pMe->pFrame)
    {
      QCAMCORDER_MSG_HIGH("LoadFrame::Creating Bitmap",0,0,0);
      (void)IDISPLAY_CreateDIBitmap(pMe->a.m_pIDisplay, (IDIB**)&(pMe->pFrame), 16, cx, cy);
      pBmp = pMe->pFrame->pBmp;
      pMe->pFrame->pBmp = (byte*)1;
      pMe->pFrame->ncTransparent = DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB565;
      bitmap = IDIB_TO_IBITMAP(pMe->pFrame);
      QCAMCORDER_MSG_HIGH("LoadFrame::Only allocating mem for Overlay",0,0,0);
      (void)ICAMERA_AddOverlay(pMe->m_pICam, bitmap); 
      pMe->pFrame->pBmp = pBmp;
      allocated_before = FALSE;
    }
    if (!STRCMP(pMe->sFrameOverlay, ENABLE_FRAMEOVERLAY_STRING))
    {      
      if (allocated_before)
      {
        pBmp = pMe->pFrame->pBmp;
        pMe->pFrame->pBmp = NULL;
        pMe->pFrame->ncTransparent = DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB565;
        bitmap = IDIB_TO_IBITMAP(pMe->pFrame);
        QCAMCORDER_MSG_HIGH("LoadFrame::Only allocating mem for Overlay",0,0,0);
        (void)ICAMERA_AddOverlay(pMe->m_pICam, bitmap); 
        pMe->pFrame->pBmp = pBmp;
      }
      return 0;
    }
    //decode PNG overlay frame
    if(IFILEMGR_Test(pMe->pIFileMgr,pMe->sFrameOverlay) == SUCCESS)
    {
       pMe->pIImage = ISHELL_LoadImage(pMe->a.m_pIShell,pMe->sFrameOverlay);
       IImage_Notify(pMe->pIImage,(PFNIMAGEINFO)Qcamcorder_Callback_LoadFrame,pMe);
    }
    return 0;
}

/*==========================================================================
FUNCTION QCAMCORDER_CALLBACK_LOADFRAME
DESCRIPTION
============================================================================= */
void Qcamcorder_Callback_LoadFrame(Qcamcorder *pMe)
{
    IDIB *inDIB = NULL;
    IBitmap *bitmap;
    uint16 transparent_565;

    inDIB = QCam_GetPNGImageData(pMe->a.m_pIDisplay,pMe->pIImage);
    if(inDIB == NULL)
    {
      QCAMCORDER_MSG_ERROR("NULL inDIB",0,0,0);
      return;
    }
    inDIB->ncTransparent = DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB565;
    /* Check that overlay frame size matches encode size */
    /* currently camcorder only supports landscape overlays */
    if(!(inDIB->cx == QCam_Sizes[pMe->eSize].cy &&
          inDIB->cy == QCam_Sizes[pMe->eSize].cx))
    {
        QCAMCORDER_MSG_ERROR("Overlay frame size does not match encode frame size",0,0,0);
        (void)IDIB_Release(inDIB); //ensure memory is freed
        inDIB = NULL;
        return;
    }

    if (inDIB)
    {
     IDIB * tempDIB = NULL;
     QCam_Get565TransparentVal(inDIB->ncTransparent, &transparent_565);
     switch (inDIB->nDepth)
     {
     case 8:
        //convert overlay frame to 565 format
        (void)ipl_RGB8882RGB565plt(inDIB->pBmp, (uint8 *)inDIB->pRGB,
                                   (uint16 *)pMe->pFrame->pBmp, 
                                   (int16)(inDIB->cx), (int16)(inDIB->cy));
        pMe->pFrame->ncTransparent = transparent_565;
        tempDIB = pMe->pFrame;
        break;
     case 16:
        pMe->pFrame->ncTransparent = transparent_565;
        tempDIB = inDIB;
        break;
     case 24:
        (void)ipl_RGB8882RGB565(inDIB->pBmp, (uint16 *)pMe->pFrame->pBmp,
                                 (int16)(inDIB->cx), (int16)(inDIB->cy));
        pMe->pFrame->ncTransparent = transparent_565;
        tempDIB = pMe->pFrame;
        break;
     default:
         QCAMCORDER_MSG_ERROR("Qcamcorder_LoadFrame() invalid inDIB->nDepth",0,0,0);
         break;
     }

     if(tempDIB)
     {
         bitmap = IDIB_TO_IBITMAP(tempDIB);
         QCAMCORDER_MSG_HIGH("LoadFrame::Calling Overlay with bitmap",0,0,0);
         (void)ICAMERA_AddOverlay(pMe->m_pICam, bitmap); //copy frame and setup MDP
     }
     //release DIB as it is no longer needed
     CAM_RELEASEIF(inDIB); //ensure memory is freed
   }
   return;
}
#endif
/* ==========================================================================
FUNCTION QCAMCORDER_PREVIEW
DESCRIPTION
============================================================================= */
void Qcamcorder_Preview(Qcamcorder* pMe)
{
  AEEParmInfo pInfo;
  AECHAR szBuf[TEXT_STRING_SIZE] = {0};
  int32 effect      = 0;
  int32 rotation    = 0;
  int32 antibanding = 0;
  int32 nZoom  = 0;

  // Check the BAR version first
  // For some reason this message doesn't appear
  (void)ISHELL_LoadResString(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, IDS_VERSION,
                       szBuf, sizeof(szBuf));
  if(WSTRCMP(szBuf, sVersion))
    Qcamcorder_DisplayMesssage(pMe, "BAR file", "Incorrect BAR File");

  pMe->eState = QCAMCORDER_PREVIEW;
  Qcamcorder_BuildSoftMenu(pMe, QCAMCORDER_MENU_PREVIEW);
  
  (void)ICAMERA_SetFramesPerSecond(pMe->m_pICam, pMe->m_FrameRate);

  switch(pMe->m_nEffect)
  {
    case QCAMCORDER_EFFECTS_OFF:
      effect = CAM_EFFECT_OFF;
      break;
    case QCAMCORDER_EFFECTS_MONO:
      effect = CAM_EFFECT_MONO;
      break;
    case QCAMCORDER_EFFECTS_SEPIA:
      effect = CAM_EFFECT_SEPIA;
      break;
    case QCAMCORDER_EFFECTS_NEGATIVE:
      effect = CAM_EFFECT_NEGATIVE;
      break;
    default:
      effect = CAM_EFFECT_OFF;
      break;
  }

  switch(pMe->m_bRotate)
  {
    case QCAMCORDER_ROTATE_90:
      rotation = 90;
      break;
    case QCAMCORDER_ROTATE_180:
      rotation = 180;
      break;
    case QCAMCORDER_ROTATE_270:
      rotation = 270;
      break;
    case QCAMCORDER_ROTATE_OFF:
    default:
      rotation=0;
      break;
  }

  switch(pMe->m_nAntibanding)
  {
    case QCAMCORDER_ANTIBANDING_50HZ:
      antibanding = CAM_ANTIBANDING_50HZ;
      break;

    case QCAMCORDER_ANTIBANDING_60HZ:
      antibanding = CAM_ANTIBANDING_60HZ;
      break;

    case QCAMCORDER_ANTIBANDING_OFF:
    default:
      antibanding = CAM_ANTIBANDING_OFF;
      break;
  }

  (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_EFFECT, effect, 0);
  (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_ROTATE_ENCODE, rotation, 0);
  (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_ANTIBANDING, antibanding, 0);

#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
  if(pMe->bUpdatePreviewFrame)
  {
    if (pMe->pFrame)
    {
      QCAMCORDER_MSG_HIGH("Preview::releasing pFrame",0,0,0);
      (void)IDIB_Release(pMe->pFrame);
      pMe->pFrame = NULL;
    }
    QCAMCORDER_MSG_HIGH("Preview::Clearing Overlay and memory",0,0,0);
    (void)ICAMERA_ClearOverlay(pMe->m_pICam);

    if(!pMe->bEnableFrameOverlay)
    {
      pMe->bUpdatePreviewFrame = FALSE;
    }
    else
    {
      (void)STRLCPY(pMe->sOverlayMenuFile,pMe->sFrameOverlay,sizeof(pMe->sOverlayMenuFile));
      QCAMCORDER_MSG_HIGH("Preview::Calling LoadFrame()",0,0,0);
      if (Qcamcorder_LoadFrame(pMe) > 0)
      {
        (void)STRLCPY(pMe->sFrameOverlay, ENABLE_FRAMEOVERLAY_STRING, sizeof(pMe->sFrameOverlay));
        (void)STRLCPY(pMe->sOverlayMenuFile, ENABLE_FRAMEOVERLAY_STRING, sizeof(pMe->sOverlayMenuFile));
      }
    }
  }
#endif /* FEATURE_VIDEO_ENCODE_OVERLAY_UI */
    (void)ICAMERA_Preview(pMe->m_pICam);

  // Get zoom parameters here only if we were not previously
  // recording a clip. Otherwise, get zoom parameters in
  // callback Qcamcorder_CamEventNotify (CAM_STATUS_START)
  if (!pMe->m_bRecord_to_Preview)
  {
    // Update the zoom parameters which are calculated in the
    // preview command by the camera driver.
    (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_ZOOM, &nZoom, (int32*)&pInfo);
    if((pInfo.nMin != pMe->nZoomMin) ||
      (pInfo.nMax != pMe->nZoomMax) ||
      (pInfo.nStep != pMe->nZoomStep)||
      (pMe->nZoom > pInfo.nMax) ||
      (pMe->nZoom < pInfo.nMin))
    {
      pMe->nZoomMin = pInfo.nMin;//0
      pMe->nZoomMax = pInfo.nMax;//79
      pMe->nZoomStep = pInfo.nStep;//1
      pMe->nZoom = nZoom;
    }
    if(nZoom != pMe->nZoom)
    {
      (void)ICAMERA_SetZoom(pMe->m_pICam, pMe->nZoom);
    }

  }
}
/* ==========================================================================
FUNCTION QCAM_EXECUTETELOPMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteContMenu(Qcamcorder* pMe)
{switch (Qcamcorder_ExecuteSubMenu(pMe, ContinuousIDList))
   {
   case IDS_CONTINUOUS_ON:
       pMe->m_bCont = TRUE;
      break;
   case IDS_CONTINUOUS_OFF:
      pMe->m_bCont = FALSE;
      break;
   default:
      pMe->m_bCont = FALSE;
      break;
   }


} /* END Qcamcorder_ExecuteTelopMenu */

/* ==========================================================================
FUNCTION Qcamcorder_BuildContMenu
DESCRIPTION
   Builds the Continuos Recording Test Menu.
============================================================================= */
static void Qcamcorder_BuildContMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->m_bCont)
   {
      case TRUE:
       focus = IDS_CONTINUOUS_ON;
      break;
      case FALSE:
      default:
       focus = IDS_CONTINUOUS_OFF;
      break;

   }

   Qcamcorder_BuildSubMenu(pMe, ContinuousIDList, focus);

   return;
} /* END QCAM_BUILDFILEFORMATMENU */


/* ==========================================================================
FUNCTION Qcamcorder_GetSpecialEffectsmenuItemImage
DESCRIPTION
   Return the image to be displayed for a given special effects menu Item. 
============================================================================= */
uint16 Qcamcorder_GetSpecialEffectsMenuItemImage(Qcamcorder *pMe,
                                                 uint16 menuItem)
{
  uint16 image = 0;
  switch(menuItem)
  {
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
    case IDS_ENABLE_FRAMEOVERLAY:
    {
      if(!pMe->bEnableFrameOverlay)
        image = IDB_CHECK_OFF;
      else
        image = IDB_CHECK_ON;
    }
    break;
#endif /* FEATURE_VIDEO_ENCODE_OVERLAY_UI */
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
    case IDS_TIME_STAMP:
    {
      if(pMe->bEnableTimeStamp)
        image = IDB_CHECK_ON;
      else
        image = IDB_CHECK_OFF;
    }
    break;
#endif /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/  
#ifdef FEATURE_VIDEO_ENCODE_FADING
    case IDS_FADE_IN:
    {
      if(pMe->mFadeInOut == QCAMCORDER_FADE_IN || pMe->mFadeInOut == QCAMCORDER_FADE_IN_FADE_OUT)
        image = IDB_CHECK_ON;
      else
        image = IDB_CHECK_OFF;
    }
    break;

    case IDS_FADE_OUT:
    {
      if(pMe->mFadeInOut == QCAMCORDER_FADE_OUT || pMe->mFadeInOut == QCAMCORDER_FADE_IN_FADE_OUT)
        image = IDB_CHECK_ON;
      else
        image = IDB_CHECK_OFF;
    }
    break;
#endif /* FEATURE_VIDEO_ENCODE_FADING */
#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
    case IDS_ALPHA_FADE:
    {
      if(pMe->mSpecialFade == QCAMCORDER_SPECIAL_FADE_ALPHA)
        image = IDB_RADIO_ON;
      else
        image = IDB_RADIO_OFF;
    }
    break;

    case IDS_CURTAIN_FADE:
    {
      if(pMe->mSpecialFade == QCAMCORDER_SPECIAL_FADE_CURTAIN)
        image = IDB_RADIO_ON;
      else
        image = IDB_RADIO_OFF;
    }
    break;

    case IDS_NONE:
    {
      if(pMe->mSpecialFade == QCAMCORDER_SPECIAL_FADE_NONE)
        image = IDB_RADIO_ON;
      else
        image = IDB_RADIO_OFF;
    }
    break;
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */

    default:
      image = (uint16)NULL;
      break;
  }
  return image;
}


/* ==========================================================================
FUNCTION Qcamcorder_BuildSpecialEffectsMenu
DESCRIPTION
   Builds the Special Effects Menu.
============================================================================= */
static void Qcamcorder_BuildSpecialEffectsMenu(Qcamcorder *pMe, uint16 selectMenuItem)
{

  AEERect qrc;
  CtlAddItem ai = {0};
  int count = 0;

  IMENUCTL_Reset(pMe->m_pIMenu);
  //Set the Rectangle for the Menu
  SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, 200 - qrc.y);
  IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);
  //lint -e570  Suppress complaint on loss of sign
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);
  //lint +e570

  // Set Title
  if (IMENUCTL_SetTitle( pMe->m_pIMenu, QCAMCORDER_RES_FILE, IDS_SPECIAL_EFFECTS, NULL)==FALSE)
    return;

  ai.pszResImage = ai.pszResText = QCAMCORDER_RES_FILE;
  ai.pText = NULL;
  ai.pImage = NULL;
  ai.wFont = AEE_FONT_NORMAL;

  for(count = 0; count < SpecialEffectsMenuIDList[0]; count++)
  {
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
    if((pMe->m_displaySize == QCAMCORDER_DISPLAY_FULL_SCREEN)&&
       (pMe->eSize != QCAMCORDER_SIZE_QVGA)&&
       (SpecialEffectsMenuIDList[count + 2] == IDS_ENABLE_FRAMEOVERLAY)
      )
    {
       continue;
    }
#endif	
	
    ai.wText = ai.wItemID = (uint16)SpecialEffectsMenuIDList[count + 2];
    ai.dwData++;
    ai.wImage = Qcamcorder_GetSpecialEffectsMenuItemImage(pMe, SpecialEffectsMenuIDList[count+2]);
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    
  }

  if(selectMenuItem != 0)
    IMENUCTL_SetSel(pMe->m_pIMenu, selectMenuItem);
  pMe->m_activeMenu = QCAMCORDER_MENU_SPECIAL_EFFECTS;
  IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
  return;
} /* END Qcamcorder_BuildSpecialEffectsMenu */


#ifdef FEATURE_QCAMCORDER_TELOP
/* ==========================================================================
FUNCTION QCAM_EXECUTETELOPMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteTelopMenu(Qcamcorder* pMe)
{switch (Qcamcorder_ExecuteSubMenu(pMe, TelopIDList))
   {
   case IDS_TELOP_ENABLED:
       pMe->m_bTelop = TELOP_ON;
      break;
   case IDS_TELOP_DISABLED:
      pMe->m_bTelop = TELOP_OFF;
      break;
   default:
      pMe->m_bTelop = TELOP_OFF;
      break;
   }


} /* END Qcamcorder_ExecuteTelopMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDFILEFORMATMENU
DESCRIPTION
   Builds the FileFormat menu.
============================================================================= */
static void Qcamcorder_BuildTelopMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->m_bTelop)
   {
      case TELOP_ON:
       focus = IDS_TELOP_ENABLED;
      break;
      case TELOP_OFF:
       focus = IDS_TELOP_DISABLED;
      break;
      default:
        focus = IDS_TELOP_DISABLED;
      break;
   }

   Qcamcorder_BuildSubMenu(pMe, TelopIDList, focus);

   return;
} /* END QCAM_BUILDFILEFORMATMENU */
#endif
#ifdef FEATURE_DVS
/* ==========================================================================
FUNCTION QCAMCORDER_ExecuteDVSMarginMenu
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteDVSMarginMenu(Qcamcorder* pMe)
{
  switch (Qcamcorder_ExecuteSubMenu(pMe, DVSMarginIDList))
  {
     case IDS_DVSMARGIN_LOW:
       pMe->eDVSMargin = QCAMCORDER_DVSMARGIN_LOW;
       break;
     case IDS_DVSMARGIN_MED:
       pMe->eDVSMargin = QCAMCORDER_DVSMARGIN_MED;
       break;
     case IDS_DVSMARGIN_HIGH:
       pMe->eDVSMargin = QCAMCORDER_DVSMARGIN_HIGH;
       break;
     default:
       pMe->eDVSMargin = QCAMCORDER_DVSMARGIN_MED;
       break;
   }
}

/* ==========================================================================
FUNCTION Qcamcorder_BuildDVSMarginMenu
DESCRIPTION
============================================================================= */
static void Qcamcorder_BuildDVSMarginMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->eDVSMargin)
   {
     case QCAMCORDER_DVSMARGIN_LOW:
       focus = IDS_DVSMARGIN_LOW;
       break;
     case QCAMCORDER_DVSMARGIN_MED:
       focus = IDS_DVSMARGIN_MED;
       break;
     case QCAMCORDER_DVSMARGIN_HIGH:
       focus = IDS_DVSMARGIN_HIGH;
       break;
     default:
       focus = IDS_DVSMARGIN_MED;
       break;
   }

   Qcamcorder_BuildSubMenu(pMe, DVSMarginIDList, focus);
   return;
}
#endif /* FEATURE_DVS */

/* ==========================================================================
FUNCTION QCAM_EXECUTESPACEPMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteSpaceMenu(Qcamcorder* pMe)
{
  switch (Qcamcorder_ExecuteSubMenu(pMe, SpaceIDList))
  {
    case IDS_SPACE_LOW:
      pMe->wSpaceLimit = CAM_SPACELIMIT_LOW;
      break;
    case IDS_SPACE_MED:
      pMe->wSpaceLimit = CAM_SPACELIMIT_MED;
      break;
    case IDS_SPACE_HIGH:
      pMe->wSpaceLimit = CAM_SPACELIMIT_HIGH;
      break;
    case IDS_SPACE_DEFAULT:
    default:
      pMe->wSpaceLimit = CAM_SPACELIMIT_DEFAULT;
      break;
  }
  (void)ICAMERA_SetSpaceLimit(pMe->m_pICam, pMe->wSpaceLimit);
} /* END Qcamcorder_ExecuteSpaceMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTETELOPMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteTimerMenu(Qcamcorder* pMe)
{
  switch (Qcamcorder_ExecuteSubMenu(pMe, TimerIDList))
  {
    case IDS_TIMER_OFF:
      pMe->m_nTimer = QCAMCORDER_TIMER_OFF;
      break;
    case IDS_TIMER_LOW:
      pMe->m_nTimer = QCAMCORDER_TIMER_LOW;
      break;
    case IDS_TIMER_MED:
      pMe->m_nTimer = QCAMCORDER_TIMER_MED;
      break;
    case IDS_TIMER_HIGH:
      pMe->m_nTimer = QCAMCORDER_TIMER_HI;
      break;
    default:
      pMe->m_nTimer = QCAMCORDER_TIMER_OFF;
      break;
  }
} /* END Qcamcorder_ExecuteTelopMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTEEFFECTSMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteEffectsMenu(Qcamcorder* pMe)
{
  switch (Qcamcorder_ExecuteSubMenu(pMe, EffectsIDList))
  {
    case IDS_EFFECT_OFF:
      pMe->m_nEffect = QCAMCORDER_EFFECTS_OFF;
      break;
    case IDS_EFFECT_MONO:
      pMe->m_nEffect = QCAMCORDER_EFFECTS_MONO;
      break;
    case IDS_EFFECT_SEPIA:
      pMe->m_nEffect = QCAMCORDER_EFFECTS_SEPIA;
      break;
    case IDS_EFFECT_NEGATIVE:
      pMe->m_nEffect = QCAMCORDER_EFFECTS_NEGATIVE;
      break;
    default:
      pMe->m_nEffect = QCAMCORDER_EFFECTS_OFF;
      break;
  }
} /* END Qcamcorder_ExecuteEffectsMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTEANTIBANDINGMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteAntibandingMenu(Qcamcorder* pMe)
{
  switch (Qcamcorder_ExecuteSubMenu(pMe, AntibandingIDList))
  {
    case IDS_ANTIBANDING_50HZ:
      pMe->m_nAntibanding = QCAMCORDER_ANTIBANDING_50HZ;
      break;

    case IDS_ANTIBANDING_60HZ:
      pMe->m_nAntibanding = QCAMCORDER_ANTIBANDING_60HZ;
      break;

    case IDS_ANTIBANDING_OFF:
    default:
      pMe->m_nAntibanding = QCAMCORDER_ANTIBANDING_OFF;
      break;
  }
} /* END Qcamcorder_ExecuteAntibandingMenu */


/* ==========================================================================
FUNCTION Qcamcorder_ExecuteRotateMenu
DESCRIPTION
============================================================================= */
static void Qcamcorder_ExecuteRotateMenu(Qcamcorder* pMe)
{
  switch (Qcamcorder_ExecuteSubMenu(pMe, RotateIDList))
  {
    case IDS_ROTATE_OFF:
      pMe->m_bRotate = QCAMCORDER_ROTATE_OFF;
      break;
    case IDS_ROTATE_90:
      pMe->m_bRotate = QCAMCORDER_ROTATE_90;
      break;
    case IDS_ROTATE_180:
      pMe->m_bRotate = QCAMCORDER_ROTATE_180;
      break;
    case IDS_ROTATE_270:
      pMe->m_bRotate = QCAMCORDER_ROTATE_270;
      break;
    default:
      pMe->m_bRotate = QCAMCORDER_ROTATE_OFF;
      break;
  }
} /* END Qcamcorder_ExecuteTelopMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDTIMERMENU
DESCRIPTION
   Builds the Timer menu.
============================================================================= */

static void Qcamcorder_BuildTimerMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->m_nTimer)
   {
      case QCAMCORDER_TIMER_OFF:
       focus = IDS_TIMER_OFF;
      break;
      case QCAMCORDER_TIMER_LOW:
       focus = IDS_TIMER_LOW;
      break;

      case QCAMCORDER_TIMER_MED:
       focus = IDS_TIMER_MED;
      break;
      case QCAMCORDER_TIMER_HI:
       focus = IDS_TIMER_HIGH;
      break;
      default:
        focus = IDS_TIMER_OFF;
      break;
   }

   Qcamcorder_BuildSubMenu(pMe, TimerIDList, focus);

   return;
} /* END QCAM_BUILDFILEFORMATMENU */

/* ==========================================================================
FUNCTION Qcamcorder_BuildSpaceThresholdMenu
DESCRIPTION
   Builds the Space Limit Threshold menu.
============================================================================= */

static void Qcamcorder_BuildSpaceThresholdMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->wSpaceLimit)
   {
      case CAM_SPACELIMIT_LOW:
       focus = IDS_SPACE_LOW;
       break;
      case CAM_SPACELIMIT_MED:
       focus = IDS_SPACE_MED;
       break;
      case CAM_SPACELIMIT_HIGH:
       focus = IDS_SPACE_HIGH;
       break;
     case CAM_SPACELIMIT_DEFAULT:
     default:
       focus = IDS_SPACE_DEFAULT;
      break;
   }

   Qcamcorder_BuildSubMenu(pMe, SpaceIDList, focus);
   return;
} /* END Qcamcorder_BuildSpaceThresholdMenu */

/* ==========================================================================
FUNCTION Qcamcorder_BuildEffectsMenu
DESCRIPTION
   Builds the Effects menu.
============================================================================= */
static void Qcamcorder_BuildEffectsMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->m_nEffect)
   {
      case QCAMCORDER_EFFECTS_OFF:
        focus = IDS_EFFECT_OFF;
        break;
      case QCAMCORDER_EFFECTS_MONO:
        focus = IDS_EFFECT_MONO;
        break;

      case QCAMCORDER_EFFECTS_SEPIA:
        focus = IDS_EFFECT_SEPIA;
        break;
      case QCAMCORDER_EFFECTS_NEGATIVE:
        focus = IDS_EFFECT_NEGATIVE;
        break;
      default:
        focus = IDS_EFFECT_OFF;
        break;
   }

   Qcamcorder_BuildSubMenu(pMe, EffectsIDList, focus);

   return;
} /* END Qcamcorder_BuildEffectsMenu */


/* ==========================================================================
FUNCTION Qcamcorder_BuildAntibandingMenu
DESCRIPTION
   Builds the Antibanding menu.
============================================================================= */
static void Qcamcorder_BuildAntibandingMenu(Qcamcorder *pMe)
{
  uint16 focus = 0;

  switch (pMe->m_nAntibanding)
  {
    case QCAMCORDER_ANTIBANDING_50HZ:
      focus = IDS_ANTIBANDING_50HZ;
      break;

    case QCAMCORDER_ANTIBANDING_60HZ:
      focus = IDS_ANTIBANDING_60HZ;
      break;

   case QCAMCORDER_ANTIBANDING_OFF:
   default:
        focus = IDS_ANTIBANDING_OFF;
        break;
  }

  Qcamcorder_BuildSubMenu(pMe, AntibandingIDList, focus);

  return;
} /* END Qcamcorder_BuildEffectsMenu */


/* ==========================================================================
FUNCTION Qcamcorder_BuildRotationMenu
DESCRIPTION
   Builds the Effects menu.
============================================================================= */

static void Qcamcorder_BuildRotationMenu(Qcamcorder *pMe)
{
   uint16 focus  = 0;

   switch (pMe->m_bRotate)
   {
      case QCAMCORDER_ROTATE_OFF:
       focus = IDS_ROTATE_OFF;
      break;
      case QCAMCORDER_ROTATE_90:
       focus = IDS_ROTATE_90;
      break;
      case QCAMCORDER_ROTATE_180:
       focus = IDS_ROTATE_180;
      break;
      case QCAMCORDER_ROTATE_270:
       focus = IDS_ROTATE_270;
      break;
      default:
        focus = IDS_ROTATE_OFF;
      break;
   }

   /* short header and H.263 do not support resolutions that are rotated
   ** versions of standard frame sizes (QCIF, SQCIF) */
   if (pMe->wShortHeader != QCAMCORDER_SH_DISABLE)
   {
#ifdef FEATURE_VIDEO_ENCODE_LANDSCAPE_MOUNT
     if((pMe->m_bRotate == QCAMCORDER_ROTATE_90) ||
        (pMe->m_bRotate == QCAMCORDER_ROTATE_270))
     {
       pMe->m_bRotate = QCAMCORDER_ROTATE_OFF;
       focus = IDS_ROTATE_OFF;
     }
#else
     if((pMe->m_bRotate == QCAMCORDER_ROTATE_OFF) ||
        (pMe->m_bRotate == QCAMCORDER_ROTATE_180))
     {
       pMe->m_bRotate = QCAMCORDER_ROTATE_90;
       focus = IDS_ROTATE_90;
        }
#endif /* FEATURE_VIDEO_ENCODE_LANDSCAPE_MOUNT */
   }

   Qcamcorder_BuildSubMenu(pMe, RotateIDList, focus);
   return;
} /* END Qcamcorder_BuildRotationMenu */

/*===========================================================================
   This function frames the rectangle with CLR_USER_FRAME and
   CLR_SYS_DK_SHADOW
===========================================================================*/
static void Qcamcorder_FrameRect(IDisplay * pd, AEERect * pRect)
{
   RGBVAL   clr = IDISPLAY_SetColor(pd, CLR_USER_FRAME, CLR_SYS_DK_SHADOW);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_FrameRect(pd, pRect);
   //lint +e570
   (void)IDISPLAY_SetColor(pd, CLR_USER_FRAME, clr);
}
/*===========================================================================
   This function updates the progress bar and the title.
===========================================================================*/
static void QProgCtl_SetPos(QProgCtl * pme, Qcamcorder *pCdr,  int16 wPct)
{
   QProgCtl_DrawHist(pme, pCdr,  wPct);
   Qcamcorder_FrameRect(pme->m_pIDisplay, &pme->m_rectMain);
}

/*===========================================================================
   This function updates the ProgCtl if full screen mode is not active
   or if progress timer is not running.
===========================================================================*/
static void Qcamcorder_UpdateProgCtl(Qcamcorder * pme)
{
   int16   wPct = 0;

   if (pme->m_dwTotalTime/1000)
     wPct = (int16)((100 * pme->m_dwPlayPos) / (pme->m_dwTotalTime/1000));

   QProgCtl_SetPos(&pme->m_ProgCtl, pme, wPct);

   if (pme->m_bShowMenu)
     IDISPLAY_UpdateEx(pme->a.m_pIDisplay, TRUE);
}

/*===========================================================================
   This funtion initializes the ProgCtl.
   ProgCtl contains two rectangles: main rectangle and within it the progress
   bar rectangle. The title is displayed on left of the progress bar within
   the main rectangle.
===========================================================================*/
static boolean QProgCtl_Init(QProgCtl * pme, AEERect * pRectMain)
{
  AEERect  rect;
  int      dxProg, dyProg;

  (void)MEMCPY(&pme->m_rectMain, pRectMain, sizeof(AEERect));

  if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_STATIC,
                            (void **)&pme->m_pTitle))
    return FALSE;

  // ProgBar rect
  dxProg = pRectMain->dx / QCAMCORDER_PROGBAR_DX_DIV;
  dyProg = pRectMain->dy / QCAMCORDER_PROGBAR_DY_DIV;

   if (dxProg > QCAMCORDER_PROGBAR_DX_OFFSET)
      dxProg -= QCAMCORDER_PROGBAR_DX_OFFSET;

   SETAEERECT(&pme->m_rectBar, 1, pRectMain->y + 4,  pRectMain->dx - 2, dyProg);

   SETAEERECT(&rect, pRectMain->x + 1, pRectMain->y + 1,
                     pRectMain->dx - dxProg, pRectMain->dy);

   ISTATIC_SetRect(pme->m_pTitle, &rect);
   ISTATIC_SetProperties(pme->m_pTitle, ST_CENTERTEXT | ST_NOSCROLL);

   return TRUE;
}

/*===========================================================================
   This function fills wPct of  progress bar rectangle.
===========================================================================*/
static void QProgCtl_DrawHist(QProgCtl * pme, Qcamcorder *pCdr, int16 wPct)
{
   AEERect  rc;
   int16 wStartPct;
   int16 dOrigWidth;

   (void)MEMCPY(&rc, &pme->m_rectBar, sizeof(AEERect));

   if(wPct > 100)
      wPct = 100;

   (void)IDISPLAY_DrawFrame(pme->m_pIDisplay, &rc, AEE_FT_RAISED, CLR_SYS_SCROLLBAR);
//rc.dx is the width of the big rectangle, dontwant to touch taht
   rc.dx = (int16)((rc.dx * wPct) / 100);

   if(pCdr->m_bEditHit){
       if(pCdr->m_dwTotalTime){
        wStartPct =  (int16)((100 * (pCdr->m_dStartPos-1)) / (pCdr->m_dwTotalTime/1000));
       }
       else
       {wStartPct =  (int16)(100 * (pCdr->m_dStartPos-1));
       }
       rc.dx = (int16)((pme->m_rectBar.dx * wStartPct) /100);
   }
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_FillRect(pme->m_pIDisplay, &rc, QC_CLR_PROG_FILL);
   //lint +e570

   if(pCdr->m_bEditHit)
   {dOrigWidth = rc.x;
       rc.x =rc.x + rc.dx;
    rc.dx = (int16)(((pme->m_rectBar.dx * wPct) /100) - (rc.dx) - (dOrigWidth));
    pCdr->EditRect.x = rc.x;
    pCdr->EditRect.y = rc.y;
    pCdr->EditRect.dx = rc.dx;
    pCdr->EditRect.dy = rc.dy;
    //lint -e570  Suppress complaint on loss of sign
    IDISPLAY_FillRect(pme->m_pIDisplay, &rc, QC1_CLR_PROG_FILL);
    //lint +e570
   }
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
   if (pCdr->eState == QCAMCORDER_CUT_MERGE && !pCdr->m_bEditHit)
   {
     //lint -e570  Suppress complaint on loss of sign
     IDISPLAY_FillRect(pme->m_pIDisplay, &(pCdr->EditRect), QC1_CLR_PROG_FILL);
     //lint +e570
   }
#endif
#ifdef FEATURE_MP4_TRANSCODER
   if (pCdr->eState == QCAMCORDER_TRANSPORTION && !pCdr->m_bEditHit)
   {
     //lint -e570  Suppress complaint on loss of sign
     IDISPLAY_FillRect(pme->m_pIDisplay, &(pCdr->EditRect), QC1_CLR_PROG_FILL);
     //lint +e570
   }
#endif

}

/* ==========================================================================
FUNCTION QCAMCORDER_DISPLAYWARNINGMESSAGE
DESCRIPTION
============================================================================= */
static void Qcamcorder_DisplayWarningMessage(Qcamcorder *pMe, uint32 msg)
{
  /* Assign msg indicating type of image to display */
  pMe->msg = msg;

  switch(msg)
  {
    case IDB_STOPPED:
    case IDB_CAM_ERROR:
      QCAMCORDER_MSG_ERROR("Error msg from service layer / insufficent space",0,0,0);
      pMe->eState = QCAMCORDER_CAMLAYER_ERROR;
      Qcamcorder_Record(pMe, QCAMCORDER_RECORD_ERROR);
      break;

    case IDB_SPACE_NEAR:
    case IDB_SPACE_IMMINENT:
      QCAMCORDER_MSG_HIGH("Received space limit message, record stopping soon",0,0,0);
      Qcamcorder_Record(pMe, QCAMCORDER_RECORD_SPACE_WARNING);
      break;

    case IDB_SPACE_REACHED:
      QCAMCORDER_MSG_HIGH("Insufficient space, recording stopped",0,0,0);
      pMe->eState = QCAMCORDER_SPACE_WARNING;
      Qcamcorder_Record(pMe, QCAMCORDER_RECORD_SPACE_WARNING);
      break;

    default:
      QCAMCORDER_MSG_ERROR("Did not catch this error - msg %d",msg,0,0);
      break;
  }
}

/* ==========================================================================
FUNCTION QCAMCORDER_CLEARANDBUILDEDITMENU
DESCRIPTION
============================================================================= */
static void Qcamcorder_ClearandBuildEditMenu(Qcamcorder *pMe)
{
  //lint -e570  Suppress complaint on loss of sign
  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
  //lint +e570
  pMe->eState = QCAMCORDER_EDIT;
  Qcamcorder_BuildEditMenu(pMe);
}

/* ==========================================================================
FUNCTION QCAMCORDER_DISPLAYWARNINGMESSAGE
DESCRIPTION
============================================================================= */
static void Qcamcorder_DisplayPostRecordingMessage(Qcamcorder *pMe)
{  IImage *pImage;
   int i;
   QCAMCORDER_MSG_MED("Got Space warning message in Post Record features. Stopping",0,0,0);
   //lint -e570  Suppress complaint on loss of sign
   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
   //lint +e570
   
   /*If file is selected from special storage medium ( FTL/MMC/USB) 
   and the medium is not accessible*/ 
   for(i = 1; i < (int)ARR_SIZE(AEEDirList); i++)
   {
     if(!STRNCMP(pMe->sCurrentFileName,AEEDirList[i],STRLEN(AEEDirList[i])) &&
         IFILEMGR_Test(pMe->pIFileMgr, AEEDirList[i])!=SUCCESS)
     {
       Qcamcorder_ClearandBuildEditMenu(pMe);
       return;
     }
   }

   //lint +e570
   if((pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, QCAMCORDER_RES_FILE, IDB_STOPPED)) == NULL)
   { (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1200, (PFNNOTIFY)Qcamcorder_ClearandBuildEditMenu,(void *)pMe);
   }
   else
   { IIMAGE_Draw(pImage, 30, 35);
     (void)IIMAGE_Release(pImage);
     pImage = NULL;
     IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
     (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1200, (PFNNOTIFY)Qcamcorder_ClearandBuildEditMenu,(void *)pMe);
   }

}


/*===========================================================================
FUNCTION: QCamcorder_ToggleMenuDisplay

  DESCRIPTION:
    This function toggles the take video menu and title on/off.

  PARAMETERS:
    QCam * pMe - Pointer to the applet instance

  RETURN VALUE:
    None
===========================================================================*/
static void QCamcorder_ToggleMenuDisplay(Qcamcorder *pMe, QcamcorderSoftMenuType type)
{

#ifndef FEATURE_MMOVERLAY
#ifdef FEATURE_MDP
  //disable/enable the mdp overlay first
  (void)mdp_enable_ovimg(pMe->m_mdpMenu_ovId, (pMe->m_bShowMenu==TRUE?0:1));
  (void)mdp_enable_ovimg(pMe->m_mdpTitle_ovId, (pMe->m_bShowMenu==TRUE?0:1));
#endif
#endif /* FEATURE_MMOVERLAY */

  /* Toggling of menu is not rqd for GPS menu */
  if ((pMe->m_activeMenu == QCAMCORDER_MENU_GPS)
      || (pMe->eGPSState == QCAMCORDER_GPS_ACQUIRING))
  {
    return;
  }

  if (pMe->m_bShowMenu)
  {
    //Hide the menu if it is on
    AEERect rc;
    pMe->m_bShowMenu = FALSE;

    SETAEERECT (&rc, 0,
                     pMe->di.cyScreen - QCAMCORDER_SOFT_MENU_HEIGHT - QCAMCORDER_PROGRESS_BAR_HEIGHT,
                     pMe->di.cxScreen,
                     QCAMCORDER_SOFT_MENU_HEIGHT + QCAMCORDER_PROGRESS_BAR_HEIGHT);

   //lint -e570  Suppress complaint on loss of sign
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
    //lint +e570

    SETAEERECT (&rc, 0,
                     0,
                     pMe->di.cxScreen,
                     QCAMCORDER_PICTURE_Y_POS + QCAMCORDER_TELOP_TEXT_SPACE);

   //lint -e570  Suppress complaint on loss of sign
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
    //lint +e570
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  }
  else
  {
    //Build the menu if it is off
    pMe->m_bShowMenu = TRUE;
    Qcamcorder_BuildSoftMenu(pMe, type);

  }
}



#ifdef FEATURE_MMOVERLAY
/*===========================================================================
FUNCTION: QCamcorder_InitMdp

  DESCRIPTION:
    This function register the device bitmap for mdp display.

  PARAMETERS:
    IDisplay *pIDisp - Pointer to the IDisplay interface.

  RETURN VALUE:
    NONE
===========================================================================*/
static void QCamcorder_InitMdp (IDisplay *pIDisp)
{

}

/*===========================================================================
FUNCTION: QCamcorder_MdpDeregister

  DESCRIPTION:
    This function deregister the MDP foreground display.

  PARAMETERS:
    None

  RETURN VALUE:
    None
===========================================================================*/
static void QCamcorder_MdpDeregister (void)
{

}

/*===========================================================================
FUNCTION: QCamcorder_MdpSetOverlay

  DESCRIPTION:
    This function creates overlay.

  PARAMETERS:
    IOverlayMgr *pIOverlayMgr - Interface pointer to IOverlay
    OverlayLayerData od - overlay parameters

  RETURN VALUE:
    None
===========================================================================*/
IMMOverlay *QCamcorder_MdpSetOverlay (IMMOverlayMgr *pIMMOverlayMgr, OverlayLayerData *od)
{
  AEEPoint pt;
  uint32 p1, p2;
  IMMOverlay *pMMOverlay = NULL;

  if (!pIMMOverlayMgr)
    return NULL;

  pt.x = od->rc.x;
  pt.y = od->rc.y;

  if(SUCCESS == IMMOVERLAYMGR_CreateOverlay(pIMMOverlayMgr, od->pBmp, AEECLSID_DISPLAY1, pt, &pMMOverlay))
  {
    p1 = od->layer;
    p2 = 0;
    IMMOVERLAY_SetParam(pMMOverlay, OV_PARM_LAYER, p1, p2);
    p1 = (uint32)&od->rc;
    p2 = NULL;
    IMMOVERLAY_SetParam(pMMOverlay, OV_PARM_DEST_REGION, p1, p2);
    p1 = (uint32)&od->rc;
    p2 = NULL;
    IMMOVERLAY_SetParam(pMMOverlay, OV_PARM_SRC_REGION, p1, p2);
    p1 = 1;
    p2 = od->transparent;
    IMMOVERLAY_SetParam(pMMOverlay, OV_PARM_TRANSPARENCY, p1, p2);
    p1 = od->alpha * 256;
    p2 = 0;
    IMMOVERLAY_SetParam(pMMOverlay, OV_PARM_ALPHA_BLEND_PCT, p1, p2);
  }

  return pMMOverlay;
}

/*===========================================================================
FUNCTION: QCamcorder_MdpRegister

  DESCRIPTION:
    This function register the specified foreground area for MDP overlay.
  
  PARAMETERS:
    IDisplay *pIDisp - Pointer to the IDisplay interface.
    IOverlayMgr *pIOverlay  - Interface pointer to IOverlay
    AEERect rc - area to register
    boolean transparency - if transparency required
    
  RETURN VALUE:
    None
===========================================================================*/
static IMMOverlay * QCamcorder_MdpRegister (IDisplay *pIDisp, IMMOverlayMgr *pIMMOverlayMgr, 
                                            AEERect rc, boolean transparency)
{
  IBitmap *bmp;
  OverlayLayerData  od;
  IMMOverlay *pMMOverlay;

  if (!pIMMOverlayMgr || !pIDisp)
    return NULL;

  IDISPLAY_GetDeviceBitmap (pIDisp, &bmp);
  od.rc = rc;
  od.pBmp = bmp;
  od.alpha = 0.75;
  od.layer = OV_LAYER_TOP_LAYER;

  if(transparency)
    od.transparent = RGB_WHITE;
  else
    od.transparent = RGB_NONE;
  
  pMMOverlay = QCamcorder_MdpSetOverlay(pIMMOverlayMgr, &od);
  CAM_RELEASEIF(bmp);

  if(NULL != pMMOverlay)
  {
     (void)IMMOVERLAY_SetParam(pMMOverlay, OV_PARM_SYSTEM_OVERLAY, 1, 0);
  }
  return pMMOverlay;
}

/*===========================================================================
FUNCTION: QCamcorder_MdpRegisterPreview

  DESCRIPTION:
    This function register the title bar and menu area for mdp overlay
    on preview mode.

  PARAMETERS:
    QCamcorder * pMe - Pointer to the applet instance
    boolean transparency - if transparency required    

  RETURN VALUE:
    None
===========================================================================*/
static void QCamcorder_MdpRegisterPreview (Qcamcorder * pMe,
                                           boolean transparency)
{
  AEERect rc;

  if(NULL != pMe->m_pIMMOverlay1)
  {
     IMMOVERLAY_Release(pMe->m_pIMMOverlay1);
     pMe->m_pIMMOverlay1 = NULL;
  }

  if(NULL != pMe->m_pIMMOverlay2)
  {
     IMMOVERLAY_Release(pMe->m_pIMMOverlay2);
     pMe->m_pIMMOverlay2 = NULL;
  }

  QCAMCORDER_MSG_HIGH("MDP Register Preview",0,0,0);
  
  /* Register for the icon menu area */
  SETAEERECT (&rc, 0,
                   pMe->di.cyScreen - QCAMCORDER_SOFT_MENU_HEIGHT -
                                      QCAMCORDER_PROGRESS_BAR_HEIGHT,
                   pMe->di.cxScreen,
                   QCAMCORDER_SOFT_MENU_HEIGHT + QCAMCORDER_PROGRESS_BAR_HEIGHT);

  pMe->m_pIMMOverlay1 = QCamcorder_MdpRegister(pMe->a.m_pIDisplay, 
                                               pMe->m_pIMMOverlayMgr, 
                                               rc, transparency);
  if(pMe->eState != QCAMCORDER_PLAYING)
  {
     /* Register for the menu title */
     SETAEERECT (&rc, 0,
                   0,
                   pMe->di.cxScreen,
                   QCAMCORDER_PICTURE_Y_POS + QCAMCORDER_TELOP_TEXT_SPACE);

     pMe->m_pIMMOverlay2 = QCamcorder_MdpRegister(pMe->a.m_pIDisplay, 
                                               pMe->m_pIMMOverlayMgr, 
                                               rc, transparency);
  }

}

/*===========================================================================
FUNCTION: QCamcorder_MdpRegisterFullScreen

  DESCRIPTION:
    This function register the full screen area for mdp overlay
    on display image or setting menus.

  PARAMETERS:
    QCamcorder * pMe - Pointer to the applet instance
    boolean transparency - if transparency required   

  RETURN VALUE:
      None
===========================================================================*/
static void QCamcorder_MdpRegisterFullScreen (Qcamcorder * pMe,
                                              boolean    transparency)
{
  QCAMCORDER_MSG_HIGH("MDP Register Full Screen",0,0,0);

  if(NULL != pMe->m_pIMMOverlay1)
  {
     IMMOVERLAY_Release(pMe->m_pIMMOverlay1);
     pMe->m_pIMMOverlay1 = NULL;
  }

  if(NULL != pMe->m_pIMMOverlay2)
  {
     IMMOVERLAY_Release(pMe->m_pIMMOverlay2);
     pMe->m_pIMMOverlay2 = NULL;
  }
}

#else /* FEATURE_MMOVERLAY */

/*===========================================================================
FUNCTION: QCamcorder_InitMdp

  DESCRIPTION:
    This function register the device bitmap for mdp display.

  PARAMETERS:
    IDisplay *pIDisp - Pointer to the IDisplay interface.

  RETURN VALUE:
    NONE
===========================================================================*/
static void QCamcorder_InitMdp (IDisplay *pIDisp)
{
  IDIB *pDib;
  IBitmap *bmp;

  (void)IDISPLAY_GetDeviceBitmap (pIDisp, &bmp);
  (void)IBITMAP_QueryInterface (bmp, AEECLSID_DIB, (void **) &pDib);
  CAM_RELEASEIF(bmp);

  mdp_set_sys_bmp((uint32 *) pDib->pBmp);
  CAM_RELEASEIF(pDib);
}

/*===========================================================================
FUNCTION: QCamcorder_MdpDeregister

  DESCRIPTION:
    This function deregister the MDP foregroung display.

  PARAMETERS:
    None

  RETURN VALUE:
    None
===========================================================================*/
static void QCamcorder_MdpDeregister (void)
{
  mdp_deregister_all_ovimg_in_layer(PRIMARY_LCD_TYPE, MDP_FGL);
  mdp_set_sys_bmp(NULL);
}

/*===========================================================================
FUNCTION: QCamcorder_MdpRegister

  DESCRIPTION:
    This function register the specified foreground area for MDP overlay.

  PARAMETERS:
    QCamcorder * pMe - Pointer to the applet instance
    AEERect rc - area to register

  RETURN VALUE:
    pointer to the registed mdp
===========================================================================*/
static uint32 QCamcorder_MdpRegister (IDisplay *pIDisp,
                                      AEERect rc,
                                      boolean transparency)
{
  MDPIMG  mdpImg;
  IDIB    *pDib;
  IBitmap *bmp;
  uint32  mdp_id;

  mdpImg.dest    = PRIMARY_LCD_TYPE;
  mdpImg.imgType = MDP_RGB565;

  (void)IDISPLAY_GetDeviceBitmap (pIDisp, &bmp);
  (void)IBITMAP_QueryInterface (bmp, AEECLSID_DIB, (void **) &pDib);
  CAM_RELEASEIF(bmp);

  mdpImg.bmy_addr = (uint32 *) pDib->pBmp;
  mdpImg.width    = (uint16)rc.dx;
  mdpImg.cwin.x1  = rc.x;
  mdpImg.cwin.x2  = rc.x+rc.dx-1;
  mdpImg.cwin.y1  = rc.y;
  mdpImg.cwin.y2  = rc.y+rc.dy-1;
  mdpImg.lcd_x    = rc.x;
  mdpImg.lcd_y    = rc.y;
  mdpImg.mdpOp    = MDPOP_NOP;
  mdpImg.tpVal    = 0xFFFF;
  mdpImg.alpha    = 0x7F;
  mdpImg.layer    = MDP_FGL;

  /* Control whether background should be transparent or not
  ** Prevent race condition if camera updates background after
  ** UI has already updated the foreground with new menus */
  if(transparency)
    mdpImg.mdpOp  = MDPOP_ALPHAB | MDPOP_TRANSP;

  mdp_id = mdp_register_ovimg (&mdpImg);
  CAM_RELEASEIF(pDib);

  return mdp_id;
}

/*===========================================================================
FUNCTION: QCamcorder_MdpRegisterPreview

  DESCRIPTION:
    This function register the title bar and menu area for mdp overlay
    on preview mode.

  PARAMETERS:
    QCamcorder * pMe - Pointer to the applet instance

  RETURN VALUE:
    None
===========================================================================*/
static void QCamcorder_MdpRegisterPreview (Qcamcorder * pMe,
                                           boolean transparency)
{
  AEERect rc;

  //Clear the former register first.
  mdp_deregister_all_ovimg_in_layer(PRIMARY_LCD_TYPE, MDP_FGL);

  //Register for the icon menu area
  SETAEERECT (&rc, 0,
                   pMe->di.cyScreen - QCAMCORDER_SOFT_MENU_HEIGHT -
                                      QCAMCORDER_PROGRESS_BAR_HEIGHT,
                   pMe->di.cxScreen,
                   QCAMCORDER_SOFT_MENU_HEIGHT + QCAMCORDER_PROGRESS_BAR_HEIGHT);
  pMe->m_mdpMenu_ovId = QCamcorder_MdpRegister(pMe->a.m_pIDisplay, rc, transparency);

  //Register for the menu title
  SETAEERECT (&rc, 0,
                   0,
                   pMe->di.cxScreen,
                   QCAMCORDER_PICTURE_Y_POS + QCAMCORDER_TELOP_TEXT_SPACE);
  pMe->m_mdpTitle_ovId = QCamcorder_MdpRegister(pMe->a.m_pIDisplay, rc, transparency);
}

/*===========================================================================
FUNCTION: QCamcorder_MdpRegisterFullScreen

  DESCRIPTION:
    This function register the full screen area for mdp overlay
    on display image or setting menus.

  PARAMETERS:
    QCamcorder * pMe - Pointer to the applet instance

  RETURN VALUE:
      None
===========================================================================*/
static void QCamcorder_MdpRegisterFullScreen (Qcamcorder * pMe,
                                              boolean    transparency)
{
  AEERect rc;

  //Clear the former register first.
  mdp_deregister_all_ovimg_in_layer(PRIMARY_LCD_TYPE, MDP_FGL);

  //Register full screen for mdp display
  SETAEERECT (&rc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen);
  (void) QCamcorder_MdpRegister(pMe->a.m_pIDisplay, rc, transparency);

}
#endif /* FEATURE_MMOVERLAY */


#ifdef FEATURE_VIDEO_ENCODE_RAW
/*===========================================================================
FUNCTION: QCamcorder_RawEncoderInit

  DESCRIPTION:
    This function starts the raw encoder by calling IMovieFile_RawEncoderStart
    and passing in encoding parameters that are obtained from user settings

  PARAMETERS:
    QCamcorder * pMe - Pointer to the applet instance

  RETURN VALUE:
      None
===========================================================================*/
boolean Qcamcorder_RawEncoderInit(Qcamcorder * pMe)
{
  int     status              = 0;
  char    *BREW_target_name   = NULL;
  char    *handle             = NULL;
  char    *input_filename     = NULL;
  char    *output_filename    = NULL;
  char    mmc_check[FILE_NAME_SIZE] = {0};
  char    target_name[FILE_NAME_SIZE] = {0};
  char    source_name[FILE_NAME_SIZE] = {0};
  char    temp_target_name[FILE_NAME_SIZE] = {0};
  char    temp_source_name[FILE_NAME_SIZE] = {0};
  uint16  cfg_method = QCAMCORDER_RAW_FILE_CONFIG;
  uint16  pathlen = 0;
  RawEncoderConfigType cfg = {0};

  /* Parse the contents of the config file */
  cfg_method = Qcamcorder_RawEncoderInfoParser(pMe, &cfg);

  if(cfg_method == QCAMCORDER_RAW_CONFIG_FAIL)
  {
    QCAMCORDER_MSG_ERROR(".info file config error",0,0,0);
    goto ENC_INIT_ERROR;
  }
  else if(cfg_method == QCAMCORDER_RAW_MENU_CONFIG)
  {
    if(Qcamcorder_RawEncoderMenuParser(pMe, &cfg) !=
       QCAMCORDER_RAW_MENU_CONFIG)
    {
      QCAMCORDER_MSG_ERROR("illegal menu settings error",0,0,0);
      goto ENC_INIT_ERROR;
    }
  }


  /* Determine if raw file is in MMC or FLASH */
  handle = (STRSTR(pMe->sCurrentFileName, ".")) - (PART_FILENAME_BYTES*3);
  pathlen = STRLEN(CARD_0_STRING) + 1;
  (void)STRLCPY(mmc_check, handle, STD_MIN(pathlen,sizeof(mmc_check)));
  if(!STRCMP(mmc_check,CARD_0_STRING))
    (void)STRLCPY(temp_source_name,MMC_DIR_PATH, sizeof(temp_source_name));
  else
    (void)STRLCPY(temp_source_name,FLASH_DIR_PATH, sizeof(temp_source_name));


  /* Obtain save location for target clip */
  if(cfg.save_location == QCAMCORDER_SAVE_FLASH)
    (void)STRLCPY(temp_target_name,FLASH_DIR_PATH, sizeof(temp_target_name));
  else if (cfg.save_location == QCAMCORDER_SAVE_MMC)
    (void)STRLCPY(temp_target_name,MMC_DIR_PATH, sizeof(temp_target_name));
  else
  {
    QCAMCORDER_MSG_ERROR("Invalid target directory path",0,0,0);
    goto ENC_INIT_ERROR;
  }


  /* Modify path of output filename */
  if((BREW_target_name = Qcamcorder_GetRawEncoderFileName(pMe,&cfg)) == NULL)
    goto ENC_INIT_ERROR;

  handle = (STRSTR(BREW_target_name, ".")) - PART_FILENAME_BYTES;
  (void)STRLCPY(target_name, handle,
                STD_MIN(FULL_FILENAME_BYTES,sizeof(target_name)));
  (void)STRLCAT(temp_target_name,target_name,sizeof(temp_target_name));
  output_filename = temp_target_name;

  /* Modify path of input filename */
  handle = (STRSTR(pMe->sCurrentFileName, ".")) - PART_FILENAME_BYTES;
  (void)STRLCPY(source_name, handle,
                STD_MIN(FULL_FILENAME_BYTES,sizeof(source_name)));
  (void)STRLCAT(temp_source_name,source_name,sizeof(temp_source_name));
  input_filename = temp_source_name;

  //if the IMovieFile object is not initialized, initialize it
  if( pMe->pRawStartFile == NULL )
  {
    if( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE,
                              (void **)&(pMe->pRawStartFile)) != SUCCESS )
      goto ENC_INIT_ERROR;
  }

  /* Pass in the parameters that are in the user settings
  ** We cast the address of the config structure to void*
  ** to reduce the number of of arguments */
  status = IMOVIEFILE_RawEncoderInit(pMe->pRawStartFile,
                                     input_filename,        /* i/p file ptr  */
                                     output_filename,       /* o/p file name */
                                     (void*)&cfg);

  if (status)
  {
    pMe->eState = QCAMCORDER_RAW_ENCODING;
    QCAMCORDER_MSG_HIGH("Starting raw encoding",0,0,0);
  }
  else
  {
    QCAMCORDER_MSG_ERROR("Raw encoding cannot initialize",0,0,0);

    if(pMe->pRawStartFile)
    {
      (void)IMOVIEFILE_Release(pMe->pRawStartFile);
      pMe->pRawStartFile = NULL;
    }
    goto ENC_INIT_ERROR;
  }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */
  /* Goes back to Qcamcorder_EditKeyHandler to rebuild edit/raw encoding menu */
 return TRUE;

ENC_INIT_ERROR :
    pMe->msg = IDB_ERROR_QCAMCORDER;
    Qcamcorder_ErrorEventUserNotify(pMe);
    return FALSE;
}

#ifdef FEATURE_VIDEO_ENCODE_RAW
/*===========================================================================
FUNCTION: QCamcorder_RawEncoderStop

  DESCRIPTION:
    This function stops the raw encoder by calling IMovieFile_RawEncoderStop

  PARAMETERS:
    QCamcorder * pMe - Pointer to the applet instance

  RETURN VALUE:
      None
===========================================================================*/
static void Qcamcorder_RawEncoderStop(Qcamcorder* pMe)
{

  //if the IMovieFile object is not initialized, initialize it
  if( pMe->pRawStopFile == NULL )
  {
    if( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MOVIEFILE,
                              (void **)&(pMe->pRawStopFile)) != SUCCESS )
      return;
  }

  (void)IMOVIEFILE_RawEncoderStop(pMe->pRawStopFile);

}

/**************************************************************************************************
* Qcamcorder_DisplayRawFrame
* This function is used to display the frames to the user
* ARGUMENTS:
* pMe: Qcamcorder structure
* pData: Data containing image data to be displayed
***************************************************************************************************/
static void Qcamcorder_DisplayRawFrame(Qcamcorder *pMe, raw_encoder_video_display_type * pData)
{
  IDIB *pDIB=NULL;
  IBitmap *pIBitmap = NULL;
  AEERect imageRC;

  //Set the display rectangle
  SETAEERECT (&imageRC,
              QCAMCORDER_PICTURE_X_POS, QCAMCORDER_PICTURE_Y_POS,
              pMe->di.cxScreen + BREW_OFFSET_X, pMe->di.cyScreen + BREW_OFFSET_Y);
  //lint -e570  Suppress complaint on loss of sign
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &imageRC);
  //lint +e570

  //Create the Device Independent Bitmap
  (void)IDISPLAY_CreateDIBitmap(pMe->a.m_pIDisplay, &pDIB,
                          DISPLAY_COLOR_DEPTH,
                          pData->width, pData->height);
  (void)MEMCPY(pDIB->pBmp, pData->rgb, pData->width * pData->height * 2);

  pDIB->nColorScheme = DISPLAY_COLOR_DEPTH;

  //get the IBitmap
  pIBitmap = IDIB_TO_IBITMAP(pDIB);

  // calculate the image position
  if(pData->height == MOVIEMAIL_HEIGHT && pData->width == MOVIEMAIL_WIDTH)
  {
    imageRC.x = MOVIEMAIL_DISPLAY_X;
    imageRC.y = MOVIEMAIL_DISPLAY_Y;
  }

  if( pDIB )
  { //blit the image on the screen
    IDISPLAY_BitBlt(pMe->a.m_pIDisplay, imageRC.x, imageRC.y,
                    pDIB->cx, pDIB->cy, (const void *)pIBitmap,
                    0, 0, AEE_RO_COPY);
  }

  //update the display
  IDISPLAY_Update(pMe->a.m_pIDisplay);


  if( pDIB )
  {
    (void)IDIB_Release(pDIB);
    pDIB = NULL;
  }
}


/*===========================================================================
FUNCTION: QCamcorder_RawEncoderInfoParser

  DESCRIPTION:
    This function parsers the encoding config parameter from the .info file

  PARAMETERS:
    QCamcorder * pMe - Pointer to the applet instance
    RawEncoderConfigType* cfg - Pointer to config structure

  RETURN VALUE:
      None
===========================================================================*/
static uint16 Qcamcorder_RawEncoderInfoParser(Qcamcorder *pMe,
                                              RawEncoderConfigType* cfg)
{
  IFileMgr    *pIFileMgr = NULL;
  IFile       *pIFile = NULL;
  ISourceUtil *pISourceUtil = NULL;
  ISource     *pISource = NULL;
  IGetLine    *pIGetLine = NULL;
  GetLine     gl;
  char*       handle;
  uint16      str_len;
  char        sData[GETLINE_MAX_CHARS] = {0};
  char        info_file[FILE_NAME_SIZE] = {0};
  uint16      fileextlen = 0;
  uint32      datalen = 0;
  QcamcorderRawEncoderConfigState configState = QCAMCORDER_RAW_FILE_CONFIG;


  /* Find path of .info file */
  (void)MEMSET(info_file,0,FILE_NAME_SIZE);
  (void)STRLCPY(info_file,pMe->sCurrentFileName, sizeof(info_file));
  handle = STRSTR(info_file, ".");
  str_len = sizeof(info_file) - (handle - info_file);
  fileextlen = STRLEN(INFO_FILE_EXT) + 1;
  str_len = STD_MIN(str_len,fileextlen);
  (void)STRLCPY(handle, INFO_FILE_EXT, str_len);


  /* Use IFileMgr to open the .info file (fs_open usage not allowed at app level */
  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr))
  {
    configState = QCAMCORDER_RAW_CONFIG_FAIL;
  }
  /* Open .info file */
  if ((pIFile = IFILEMGR_OpenFile(pIFileMgr,info_file,_OFM_READ)) == NULL)
  {
    QCAMCORDER_MSG_ERROR("Raw Encoding: open .info failed",0,0,0);
    configState = QCAMCORDER_RAW_CONFIG_FAIL;
    goto BREW_CLEANUP;
  }
  /* Create ISourceUtil so we can use IGetLine with pIFile */
  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOURCEUTIL, (void **)&pISourceUtil))
  {
    configState = QCAMCORDER_RAW_CONFIG_FAIL;
    goto BREW_CLEANUP;
  }
  /* ISourceUtil uses pIFile as the source of info to read from */
  if (ISOURCEUTIL_SourceFromAStream(pISourceUtil, (IAStream*)pIFile, &pISource))
  {
    configState = QCAMCORDER_RAW_CONFIG_FAIL;
    goto BREW_CLEANUP;
  }
  /* Indicate no. of chars to read using IGetLine */
  if (ISOURCEUTIL_GetLineFromSource(pISourceUtil, pISource, GETLINE_MAX_CHARS, &pIGetLine))
  {
    configState = QCAMCORDER_RAW_CONFIG_FAIL;
    goto BREW_CLEANUP;
  }

  if(configState == QCAMCORDER_RAW_CONFIG_FAIL)
    goto BREW_CLEANUP;


  /* Frame Width */
  (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
  cfg->frame_width = ATOI(gl.psz);

  /* Frame Height */
  (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
  cfg->frame_height = ATOI(gl.psz);


  if(!((cfg->frame_width == 320 && cfg->frame_height == 240) ||
       (cfg->frame_width == 176 && cfg->frame_height == 144) ||
       (cfg->frame_width == 128 && cfg->frame_height == 96)  ||
       (cfg->frame_width == 96  && cfg->frame_height == 80)))
  {
    QCAMCORDER_MSG_ERROR("Unsupported frame size %d %d",
               cfg->frame_width,cfg->frame_height,0);
    configState = QCAMCORDER_RAW_CONFIG_FAIL;
  }


  if(configState != QCAMCORDER_RAW_CONFIG_FAIL)
  {

    /* Config Source Mode - menu settings / .info file */
    (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
    (void)MEMSET(sData, 0, GETLINE_MAX_CHARS);
    datalen =  ((uint32)(STRSTR(gl.psz," ")-gl.psz))+1;
    (void)STRLCPY(sData, gl.psz, STD_MIN(datalen,sizeof(sData)));

    if(!STRCMP(sData,"CONT"))
    {

      /* User wants config settings in .info file */
      configState = QCAMCORDER_RAW_FILE_CONFIG;


      /* Start and End frames */
      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->start_frame = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->end_frame = (uint32)ATOI(gl.psz);

      if(cfg->start_frame > cfg->end_frame)
      {
        configState = QCAMCORDER_RAW_CONFIG_FAIL;
        QCAMCORDER_MSG_ERROR("Frame index error: Start %d End %d",
                             cfg->start_frame,cfg->end_frame,0);
      }


      /* Time Resolution */
      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->time_resolution = (uint32)ATOI(gl.psz);


      /* Time Increment */
      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->time_increment = (uint32)ATOI(gl.psz);


      /* CW Rotation */
      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->rotation = (uint32)ATOI(gl.psz);

      if(cfg->rotation == 90 || cfg->rotation == 270)
      {
        cfg->frame_width  ^= cfg->frame_height;
        cfg->frame_height ^= cfg->frame_width;
        cfg->frame_width  ^= cfg->frame_height;
      }


      /* Profile */
      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      (void)MEMSET(sData, 0, GETLINE_MAX_CHARS);
      datalen = ((uint32)(STRSTR(gl.psz," ")-gl.psz))+1;
      (void)STRLCPY(sData, gl.psz, STD_MIN(datalen,sizeof(sData)));

      if(!STRCMP(sData,"LOW"))
        cfg->profile = QCAMCORDER_QUALITY_LOW;
      else if(!STRCMP(sData,"MED"))
        cfg->profile = QCAMCORDER_QUALITY_MED;
      else if(!STRCMP(sData,"HIGH"))
        cfg->profile = QCAMCORDER_QUALITY_HIGH;
      else if(!STRCMP(sData,"USER"))
        cfg->profile = QCAMCORDER_QUALITY_USER;
      else if(!STRCMP(sData,"MAX"))
        cfg->profile = QCAMCORDER_QUALITY_MAX;
      else if(!STRCMP(sData,"DYNAMIC"))
        cfg->profile = QCAMCORDER_QUALITY_DYNAMIC;
      else
      {
        QCAMCORDER_MSG_ERROR("Unsupported profile %s",sData,0,0);
        configState = QCAMCORDER_RAW_CONFIG_FAIL;
      }


      /* Target Bitrate */
      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->user_bitrate = (uint32)ATOI(gl.psz);


      /* File Brand */
      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      (void)MEMSET(sData, 0, GETLINE_MAX_CHARS);
      datalen = ((uint32)(STRSTR(gl.psz," ")-gl.psz))+1;
      (void)STRLCPY(sData, gl.psz, STD_MIN(datalen,sizeof(sData)));

      if(!STRCMP(sData,"MP4"))
        cfg->file_brand = QCAMCORDER_FILE_MP4;
      else if(!STRCMP(sData,"AMC"))
        cfg->file_brand = QCAMCORDER_FILE_AMC;
      else if(!STRCMP(sData,"3GP"))
        cfg->file_brand = QCAMCORDER_FILE_3GP;
      else if(!STRCMP(sData,"K3G"))
        cfg->file_brand = QCAMCORDER_FILE_K3G;
      else if(!STRCMP(sData,"3G2"))
        cfg->file_brand = QCAMCORDER_FILE_3G2;
      else if(!STRCMP(sData,"3G2F"))
        cfg->file_brand = QCAMCORDER_FILE_3G2_FRAG;
      else
      {
        QCAMCORDER_MSG_ERROR("Unsupported file format %s",sData,0,0);
        configState = QCAMCORDER_RAW_CONFIG_FAIL;
      }


      /* Video Format */
      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      (void)MEMSET(sData, 0, GETLINE_MAX_CHARS);
      datalen = ((uint32)(STRSTR(gl.psz," ")-gl.psz))+1;
      (void)STRLCPY(sData, gl.psz, STD_MIN(datalen,sizeof(sData)));

      if(!STRCMP(sData,"MPEG4"))
      {
        cfg->videoformat = QCAMCORDER_VIDEO_STREAM_MPEG4;
      }
      else if (!STRCMP(sData,"H263"))
      {
        cfg->videoformat = QCAMCORDER_VIDEO_STREAM_H263;
      }
      else if (!STRCMP(sData,"H264"))
      {
        cfg->videoformat = QCAMCORDER_VIDEO_STREAM_H264;
      }
      else
      {
        QCAMCORDER_MSG_ERROR("Unsupported video format %s",sData,0,0);
        configState = QCAMCORDER_RAW_CONFIG_FAIL;
      }


      /* Target Destination */
      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      (void)MEMSET(sData, 0, GETLINE_MAX_CHARS);
      datalen = ((uint32)(STRSTR(gl.psz," ")-gl.psz))+1;
      (void)STRLCPY(sData, gl.psz, STD_MIN(datalen,sizeof(sData)));

      if(!STRCMP(sData,"FLASH"))
      {
        cfg->save_location = QCAMCORDER_SAVE_FLASH;
      }
      else if (!STRCMP(sData,"MMC"))
      {
        cfg->save_location = QCAMCORDER_SAVE_MMC;
      }
      else
      {
        QCAMCORDER_MSG_ERROR("Unsupported save location %s",sData,0,0);
        configState = QCAMCORDER_RAW_CONFIG_FAIL;
      }


      /* Ensure that config parameters are valid for brand */
      if(cfg->videoformat == QCAMCORDER_VIDEO_STREAM_H263 &&
         ((cfg->frame_width == 320 && cfg->frame_height == 240) ||
          (cfg->frame_width == 96  && cfg->frame_height == 80)  ||
          cfg->rotation == 90 || cfg->rotation == 270))
      {
        configState = QCAMCORDER_RAW_CONFIG_FAIL;
        QCAMCORDER_MSG_ERROR("H.263: QVGA/MM sizes or rotation unsupported",0,0,0);
      }

      if(cfg->file_brand == QCAMCORDER_FILE_AMC &&
         (cfg->frame_width > 128 || cfg->frame_height > 96))
      {
        configState = QCAMCORDER_RAW_CONFIG_FAIL;
        QCAMCORDER_MSG_ERROR("Frame size not supported for AMC %d %d",cfg->frame_width,
                                                           cfg->frame_height,0);
      }

      /* Advanced raw encoding firmware configurations */
      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->adv_cfg_enable = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->four_mv_threshold = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->intra_16x16_bias = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->intra_4x4_bias = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->inter_1mv_bias = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->inter_4mv_bias = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->mb_search_type = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->mb_min_blk_anchor = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->mb_anchor_blk_anchor = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->fractional_pel_enable = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->qp_I_VOP = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->qp_P_VOP = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->quant_precision = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->intra_period_ms = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->rc_mode = (uint32)ATOI(gl.psz);

      (void)IGETLINE_GetLine(pIGetLine, &gl, IGETLINE_CRLF);
      cfg->input_format = (uint32)ATOI(gl.psz);

    } /* if(!STRCMP(sData,"CONT")) */
    else if (!STRCMP(sData,"SKIP"))
    {
      configState = QCAMCORDER_RAW_MENU_CONFIG;
    }
    else
    {
      configState = QCAMCORDER_RAW_CONFIG_FAIL;
      QCAMCORDER_MSG_ERROR("Invalid config mode: %s",sData,0,0);
    }

  } /* if(configState != QCAMCORDER_RAW_CONFIG_FAIL) */



BREW_CLEANUP:

  /* Read all relevant data, decrement ref count */
  if(pIGetLine)
  {
    (void)IGETLINE_Release(pIGetLine);
    pIGetLine = NULL;
  }
  if(pISource)
  {
    (void)ISOURCE_Release(pISource);
    pISource = NULL;
  }
  if(pISourceUtil)
  {
    (void)ISOURCEUTIL_Release(pISourceUtil);
    pISourceUtil = NULL;
  }
  if(pIFile)
  {
    (void)IFILE_Release(pIFile);
    pIFile = NULL;
  }
  if(pIFileMgr)
  {
    (void)IFILEMGR_Release(pIFileMgr);
    pIFileMgr = NULL;
  }

  return configState;
}

/*===========================================================================
FUNCTION: QCamcorder_RawEncoderMenuParser

  DESCRIPTION:
    This function parsers the encoding config parameter from the menu

  PARAMETERS:
    QCamcorder * pMe - Pointer to the applet instance
    Raw EncoderConfigType *cfg - Pointer to config structure

  RETURN VALUE:
      None
===========================================================================*/
static uint16 Qcamcorder_RawEncoderMenuParser(Qcamcorder* pMe,
                                              RawEncoderConfigType* cfg)
{
  /* 1. Frame size should have been parsed using .info file */
  ASSERT(cfg->frame_width != 0 && cfg->frame_height != 0);
  if(pMe->wFileFormat == QCAMCORDER_FILE_AMC && (cfg->frame_width  > 128 ||
                                                 cfg->frame_height > 96))
  {
    QCAMCORDER_MSG_ERROR("QVGA/QCIF size not supported in AMC %d %d",
              cfg->frame_width,cfg->frame_height,0);
    return QCAMCORDER_RAW_CONFIG_FAIL;
  }


  /* 2. Parse the frame rate */
  cfg->time_resolution = (uint16)pMe->m_FrameRate;
  cfg->time_increment = (uint16)(pMe->m_FrameRate >> 16);


  /* 3. Parse the rotation angle */
  switch(pMe->m_bRotate)
  {
    case QCAMCORDER_ROTATE_90:
      cfg->rotation = 90;
      break;
    case QCAMCORDER_ROTATE_180:
      cfg->rotation = 180;
      break;
    case QCAMCORDER_ROTATE_270:
      cfg->rotation = 270;
      break;
    case QCAMCORDER_ROTATE_OFF:
    default:
      cfg->rotation=0;
      break;
  }

  if(pMe->m_bRotate == QCAMCORDER_ROTATE_90 ||
     pMe->m_bRotate == QCAMCORDER_ROTATE_270)
  {
    cfg->frame_width  ^= cfg->frame_height;
    cfg->frame_height ^= cfg->frame_width;
    cfg->frame_width  ^= cfg->frame_height;
  }

  /* 4. Parse the profile/quality setting */
  cfg->profile = pMe->wQuality;


#ifdef FEATURE_VIDEO_ENCODE_BITRATE
  /* 5. Parse the user target bitrate */
  if (pMe->wQuality == QCAMCORDER_QUALITY_USER)
    cfg->user_bitrate = (uint32)(pMe->wBitRate);
#endif /* FEATURE_VIDEO_ENCODE_BITRATE */


  /* 6. Parse the file brand */
  cfg->file_brand = pMe->wFileFormat;


  /* 7. Parse the video format */
  if(pMe->wShortHeader == QCAMCORDER_SH_ENABLE ||
     pMe->wShortHeader == QCAMCORDER_SH_TEST)
  {
    if(pMe->eSize == QCAMCORDER_SIZE_QVGA     ||
       pMe->eSize == QCAMCORDER_SIZE_MM       ||
       pMe->m_bRotate == QCAMCORDER_ROTATE_90 ||
       pMe->m_bRotate == QCAMCORDER_ROTATE_270)
    {
      QCAMCORDER_MSG_ERROR("H.263: QVGA or rotation unsupported",0,0,0);
      return QCAMCORDER_RAW_CONFIG_FAIL;
    }
    cfg->videoformat = QCAMCORDER_VIDEO_STREAM_H263;
  }
  else if (pMe->wShortHeader == QCAMCORDER_SH_DISABLE)
  {
    cfg->videoformat = QCAMCORDER_VIDEO_STREAM_MPEG4;
  }
  else
  {
    QCAMCORDER_MSG_ERROR("Video format not supported",0,0,0);
    return QCAMCORDER_RAW_CONFIG_FAIL;
  }


  /* 8. Parse save location setting */
  cfg->save_location = pMe->saveLoc;

  return QCAMCORDER_RAW_MENU_CONFIG;

}
#endif

#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif //#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE

#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
void Qcamcorder_Transcode(Qcamcorder * pMe)
{
  char *pFileName = NULL;
  boolean error   = FALSE;
  AEEMediaData src;
  uint32  rgbBuffersize =0;
    //uint32  height_width_prod = 0;
  int     status = 0;
#ifdef FEATURE_DVS
  int32   brand = MOVIEFILE_BRAND_AMC;

  if(pMe->bDVS)
    pFileName = Qcamcorder_GetDVSFileName(pMe, &brand);
  else
    pFileName = Qcamcorder_GetTranscodeFileName(pMe);
#else
  pFileName = Qcamcorder_GetTranscodeFileName(pMe);
#endif

  (void)STRLCPY(pMe->sTranscodeOutFile,pFileName, sizeof(pMe->sTranscodeOutFile));

  pMe->bTranscoderStoping = FALSE;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /*FEATURE_APP_TEST_AUTOMATION*/

  QCAMCORDER_IMEDIA_STOP(pMe->pMedia);
  QCAMCORDER_IMEDIA_RELEASE(pMe->pMedia);

  if (!pMe->pIFileMgr) 
  {
    (void)ISHELL_CreateInstance(
      pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pMe->pIFileMgr);
  }

  if (IFILEMGR_Test(pMe->pIFileMgr, pMe->sTranscodeOutFile) == SUCCESS) 
  {
    if (IFILEMGR_Remove(pMe->pIFileMgr, pMe->sTranscodeOutFile) != SUCCESS) 
    {
      error = TRUE;
      QCAMCORDER_MSG_HIGH("COULD NOT DELETE THE VIDEO EDITING FILE",0,0,0);
    }
  }

  if (!error  && ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IMOVIEEDIT,
                            (void **)&(pMe->m_pInEdit)) !=  SUCCESS)
  {
    error = TRUE;
    QCAMCORDER_MSG_ERROR("VideoEditing cannot create infile",0,0,0);
  }

  //Create Instance of  File for writing.
  if (!error && ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IMOVIEEDIT,
                            (void **)&(pMe->m_pOutEdit)) != SUCCESS)
  {
      error = TRUE;
      QCAMCORDER_MSG_ERROR("Can't create Instance for Writing",0,0,0);
  }

  src.clsData = MMD_FILE_NAME;
  src.pData = pMe->sCurrentFileName;

  //OPen the first File in read mode
  if (!error && IMOVIEEDIT_ReadOpen(pMe->m_pInEdit, &src,
                           &(pMe->m_mei), NULL,
                           NULL) != SUCCESS)
  {
    error = TRUE;
    QCAMCORDER_MSG_ERROR("VIdeoEditing cannot open input file",0,0,0);
  }

  if (!error) 
  {
      switch (pMe->eTranscoderSize)
      {
      case QCAMCORDER_SIZE_QVGA:
          rgbBuffersize = 320 * 240 * 2;
          pMe->targetparams.targetvideoresolution = AEEMOVIEEDIT_QVGA;
          break;
      case QCAMCORDER_SIZE_QCIF:
          rgbBuffersize = 176 * 144 * 2;
          pMe->targetparams.targetvideoresolution = AEEMOVIEEDIT_QCIF;
          break;
      case QCAMCORDER_SIZE_SQCIF:
          rgbBuffersize = 128 * 96 * 2;
          pMe->targetparams.targetvideoresolution = AEEMOVIEEDIT_SUBQCIF;
          break;
      case  QCAMCORDER_SIZE_MM:
          rgbBuffersize = 96 * 80 * 2;
          pMe->targetparams.targetvideoresolution = AEEMOVIEEDIT_MOVIEMAIL;
          break;
      default:
          error = TRUE;
      }

      switch (pMe->eTranscoderBrand)
      {
      case QCAMCORDER_FILE_XCODER_AMC:
          pMe->targetparams.targetvideobrand = AEEMOVIEEDIT_BRAND_AMC;
          break;
      case QCAMCORDER_FILE_XCODER_3GP:
          pMe->targetparams.targetvideobrand = AEEMOVIEEDIT_BRAND_3GP;
          break;
      case QCAMCORDER_FILE_XCODER_MP4:
          pMe->targetparams.targetvideobrand = AEEMOVIEEDIT_BRAND_MP4;
          break;
      case QCAMCORDER_FILE_XCODER_3G2:
          pMe->targetparams.targetvideobrand = AEEMOVIEEDIT_BRAND_3G2;
          break;
      case QCAMCORDER_FILE_XCODER_K3G:
          pMe->targetparams.targetvideobrand = AEEMOVIEEDIT_BRAND_K3G;
          break;
      case QCAMCORDER_FILE_XCODER_3G2_FRAG:
          pMe->targetparams.targetvideobrand = AEEMOVIEEDIT_BRAND_3G2_FRAG;
          break;
      default:
          error = TRUE;
      }

      switch (pMe->eTranscoderVideoCodec)
      {
      case QCAMCORDER_VIDEO_FORMAT_MPEG4:
          pMe->targetparams.targetvideocodec = AEEMOVIEEDIT_VIDEO_FORMAT_MPEG4;
          break;
      case QCAMCORDER_VIDEO_FORMAT_H263:
          pMe->targetparams.targetvideocodec = AEEMOVIEEDIT_VIDEO_FORMAT_H263;
          break;
      default:
          error = TRUE;
      }

      switch (pMe->wTranscodeAudioFormat)
      {
      case QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K:
          pMe->targetparams.targetaudioformat = AEEMOVIEEDIT_AUDIO_FORMAT_FIXED_FULL_13K;
        break;
      case QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K:
          pMe->targetparams.targetaudioformat = AEEMOVIEEDIT_AUDIO_FORMAT_FIXED_HALF_13K;
        break;
      case QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_EVRC:
          pMe->targetparams.targetaudioformat = AEEMOVIEEDIT_AUDIO_FORMAT_FIXED_FULL_EVRC;
        break;
      case QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR:
          pMe->targetparams.targetaudioformat = AEEMOVIEEDIT_AUDIO_FORMAT_FIXED_12_2_AMR;
        break;
      case QCAMCORDER_AUDIO_FORMAT_NONE:
          pMe->targetparams.targetaudioformat = AEEMOVIEEDIT_AUDIO_FORMAT_NONE;
          break;
      default:
          error = TRUE;
        break;
      }

      switch (pMe->m_FrameRate)
      {
      case QCAMCORDER_FRAME_RATE_1:
        pMe->targetparams.targetvideofps = AEEMOVIEEDIT_7_FPS; 
        break;
      case QCAMCORDER_FRAME_RATE_2:
        pMe->targetparams.targetvideofps = AEEMOVIEEDIT_10_FPS; 
        break;
      case QCAMCORDER_FRAME_RATE_3:
        pMe->targetparams.targetvideofps = AEEMOVIEEDIT_15_FPS;
        break;
      default:
        error = TRUE;
      }
  }

  pMe->targetparams.sourceclipInfo.audioFormat      =  pMe->m_mei.audioFormat;
  pMe->targetparams.sourceclipInfo.audioTimescale   =  pMe->m_mei.audioTimescale;
  pMe->targetparams.sourceclipInfo.audioTotalPkt    =  pMe->m_mei.audioTotalPkt;
  pMe->targetparams.sourceclipInfo.extensionTotal   =  pMe->m_mei.extensionTotal; 
  pMe->targetparams.sourceclipInfo.frame_rate       =  pMe->m_mei.frame_rate;
  pMe->targetparams.sourceclipInfo.movieTimescale   =  pMe->m_mei.movieTimescale;
  pMe->targetparams.sourceclipInfo.textTimescale    =  pMe->m_mei.textTimescale;
  pMe->targetparams.sourceclipInfo.textTotalSample  =  pMe->m_mei.textTotalSample;
  pMe->targetparams.sourceclipInfo.videoAvgBitrate  =  pMe->m_mei.videoAvgBitrate;
  pMe->targetparams.sourceclipInfo.videoBrand       =  pMe->m_mei.videoBrand;
  pMe->targetparams.sourceclipInfo.videoFormat      =  pMe->m_mei.videoFormat;
  pMe->targetparams.sourceclipInfo.videoHeaderSize  =  pMe->m_mei.videoHeaderSize;
  memcpy (pMe->targetparams.sourceclipInfo.videoHeader,pMe->m_mei.videoHeader,
          pMe->targetparams.sourceclipInfo.videoHeaderSize); 
  pMe->targetparams.sourceclipInfo.audioHeaderSize  =  pMe->m_mei.audioHeaderSize;
  memcpy (pMe->targetparams.sourceclipInfo.audioHeader,pMe->m_mei.audioHeader,
          pMe->targetparams.sourceclipInfo.audioHeaderSize);  
  pMe->targetparams.sourceclipInfo.videoHeight      =  pMe->m_mei.videoHeight;
  pMe->targetparams.sourceclipInfo.videoTimescale   =  pMe->m_mei.videoTimescale;
  pMe->targetparams.sourceclipInfo.videoTotalFrame  =  pMe->m_mei.videoTotalFrame;
  pMe->targetparams.sourceclipInfo.videoWidth       =  pMe->m_mei.videoWidth;

  CALLBACK_Init(&pMe->m_movieeditCB,Qcamcorder_TranscodeCb, pMe);
  (void)STRLCPY(pMe->targetparams.FileName,pMe->sTranscodeOutFile,sizeof(pMe->targetparams.FileName));
  
  pMe->targetparams.StartFrameNo = pMe->nStartFrameNo;
  pMe->targetparams.EndFrameNo = pMe->nEndFrameNo;
  pMe->targetparams.eProfile = (video_profile_type)pMe->wQuality;
  pMe->targetparams.videoAvgBitrate = pMe->wBitRate;

#ifdef FEATURE_DVS
  pMe->targetparams.dvsmode = pMe->bDVS;

  switch (pMe->eDVSMargin) 
  {
  case QCAMCORDER_DVSMARGIN_LOW: 
      pMe->targetparams.targetdvsmargin = AEEMOVIEEDIT_DVS_MARGIN_LOW;
      break;
  case QCAMCORDER_DVSMARGIN_MED:
      pMe->targetparams.targetdvsmargin = AEEMOVIEEDIT_DVS_MARGIN_MED;
      break;
  case QCAMCORDER_DVSMARGIN_HIGH:
      pMe->targetparams.targetdvsmargin = AEEMOVIEEDIT_DVS_MARGIN_HIGH;
      break;
  }
#endif

  if(!error)
  {
      pMe->m_transcoderstatus.pRgbdata = NULL;
#ifdef FEATURE_DVS
      if(pMe->targetparams.dvsmode)
      {
        pMe->m_transcoderstatus.pRgbdata 
            = (uint8 *) MALLOC (pMe->targetparams.sourceclipInfo.videoHeight *
                                pMe->targetparams.sourceclipInfo.videoWidth  * 2
                                );
      }
      else
#endif
      {
        pMe->m_transcoderstatus.pRgbdata = (uint8 *) MALLOC (rgbBuffersize);
      }
      if (!pMe->m_transcoderstatus.pRgbdata) 
      {
         error = TRUE; 
      }
  }

  if(!error)
  {
#ifdef FEATURE_DVS
      if(pMe->targetparams.dvsmode)
      {
          status= IMOVIEEDIT_DVS (pMe->m_pOutEdit,pMe->m_pInEdit,
                                  &pMe->targetparams,&pMe->m_transcoderstatus,
                                  &pMe->m_movieeditCB);
      }
      else
#endif
      {
          status= IMOVIEEDIT_Transcode (pMe->m_pOutEdit,pMe->m_pInEdit,
                                        &pMe->targetparams,&pMe->m_transcoderstatus,
                                        &pMe->m_movieeditCB);
      }
      if(status != SUCCESS)
          error = TRUE;
  }

  if (error) 
  {
           //lint -e570  Suppress complaint on loss of sign
     Qcamcorder_DrawIcon(pMe, IDB_POSTEDIT_UNSUPPORTED);
     //lint +e570
     IDISPLAY_Update(pMe->a.m_pIDisplay);
     FREEIF (pMe->m_transcoderstatus.pRgbdata);
     (void) IMOVIEEDIT_ReadClose (pMe->m_pInEdit,NULL,NULL);
     QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
     QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
     pMe->eState = QCAMCORDER_EDIT;
     (void)ISHELL_SetTimer(pMe->a.m_pIShell, 1000,
         (PFNNOTIFY) Qcamcorder_DisplayAlbum, (void*)pMe);
  }
  else
  {
      IMENUCTL_Reset(pMe->m_pISoftMenu);
      (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
      Qcamcorder_BuildEditMenu(pMe);
  }

}

static void Qcamcorder_TranscodeCb (void *pUser)
{
    xcoder_status_type stat;
    Qcamcorder *pMe = (Qcamcorder *)pUser;
    
    switch (pMe->m_transcoderstatus.statusmsg) 
    {
    case  IMOVIEEDIT_TRANSCODER_STATE_DONE:
        if(pMe->eState == QCAMCORDER_TRANSCODE_DONE)
        {
          QCAMCORDER_MSG_HIGH("Transcode Cb:Ignoring callback..."
                                    "Already processing error",0,0,0);
          return;/*Already processing error*/
        }
        pMe->eState = QCAMCORDER_TRANSCODE_DONE;
        Qcamcorder_transcoderAbort(pMe);
        break;
    case IMOVIEEDIT_TRANSCODER_STATE_AUDIO_FRAME_DONE:
    case IMOVIEEDIT_TRANSCODER_STATE_VIDEO_FRAME_DONE:
    case IMOVIEEDIT_TRANSCODER_STATE_AUDIO_FRAME_RESET:
    case IMOVIEEDIT_TRANSCODER_STATE_VIDEO_FRAME_REQUEST:
    case IMOVIEEDIT_TRANSCODER_STATE_AUDIO_FRAME_REQUEST:
        break;

    case IMOVIEEDIT_TRANSCODER_STATE_INIT_DONE:
		
             ISHELL_SendEvent (pMe->a.m_pIShell,
                    AEECLSID_QCAMCORDER,EVT_USER,
                    IMOVIEEDIT_TRANSCODER_STATE_INIT_DONE,
                     0); 
         break;


    case IMOVIEEDIT_TRANSCODER_STATE_AUDIO_DONE:
        pMe->eState = QCAMCORDER_TRANSCODE;
        //Qcamcorder_DisplayAlbum(pMe);
        Qcamcorder_DrawIcon(pMe, IDB_TRANSAUDIO_DONE);
        break;

    case IMOVIEEDIT_TRANSCODER_STATE_TERMINATED:
        FREEIF (pMe->m_transcoderstatus.pRgbdata);
        (void) IMOVIEEDIT_ReadClose (pMe->m_pInEdit,NULL,NULL);
        CALLBACK_Cancel(&pMe->m_movieeditCB);
        QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pInEdit);
        QCAMCORDER_IMOVIEEDIT_RELEASE(pMe->m_pOutEdit);
        pMe->bTranscoderStoping = FALSE;
        ISHELL_SendEvent (pMe->a.m_pIShell,
                            AEECLSID_QCAMCORDER,EVT_USER,
                            IMOVIEEDIT_TRANSCODER_STATE_TERMINATED,
                            0);       
        break;
    case  IMOVIEEDIT_TRANSCODER_STATE_VIDEO_CONT:
        stat.video.rgb = pMe->m_transcoderstatus.pRgbdata;
        stat.video.height = pMe->m_transcoderstatus.height;
        stat.video.width = pMe->m_transcoderstatus.width;
        if(pMe->eState == QCAMCORDER_TRANSCODE)
        {
          //Display the frame on the screen
          Qcamcorder_DisplayFrame(pMe, &(stat.video));
        }
        break;          
    case IMOVIEEDIT_TRANSCODER_STATE_AUDIO_START:
        ISHELL_SendEvent (pMe->a.m_pIShell,
                          AEECLSID_QCAMCORDER,EVT_USER,
                          IMOVIEEDIT_TRANSCODER_STATE_AUDIO_START,
                          0);       
        break;
    case IMOVIEEDIT_TRANSCODER_STATE_ERROR:
         if(pMe->eState == QCAMCORDER_TRANSCODE_ERROR)
         {
            QCAMCORDER_MSG_HIGH("Transcode Cb:Ignoring callback..."
                                      "Already processing error",0,0,0);
            return;/*Already processing error*/
         }
         pMe->eState = QCAMCORDER_TRANSCODE_ERROR;
         Qcamcorder_transcoderAbort (pMe);
         pMe->bTranscoderStoping = TRUE;
         ISHELL_SendEvent (pMe->a.m_pIShell,
                           AEECLSID_QCAMCORDER,EVT_USER,
                           IMOVIEEDIT_TRANSCODER_STATE_ERROR,
                           0);       
        break;
    case IMOVIEEDIT_TRANSCODER_STATE_UNSUPPORTED:
         if(pMe->eState == QCAMCORDER_TRANSCODE_UNSUPPORTED)
         {
            QCAMCORDER_MSG_HIGH("Transcode Cb:Ignoring callback..."
                                "Already processing unsupported",0,0,0);
            return;/*Already processing error*/
         }
         pMe->eState = QCAMCORDER_TRANSCODE_UNSUPPORTED;
         Qcamcorder_transcoderAbort (pMe);
         pMe->bTranscoderStoping = TRUE;
         ISHELL_SendEvent (pMe->a.m_pIShell,
                           AEECLSID_QCAMCORDER,EVT_USER,
                           IMOVIEEDIT_TRANSCODER_STATE_UNSUPPORTED,
                           0);       
        break;
    }
}

static void Qcamcorder_transcoderAbort (Qcamcorder *pMe)
{

    pMe->bTranscode_portion = FALSE;
    pMe->nStartFrameNo         = 0;
    pMe->nEndFrameNo           = 0;
#ifdef FEATURE_DVS
    pMe->bDVS                  = FALSE;
#endif
    if(pMe->m_pOutEdit)
    {
        IMOVIEEDIT_TranscodeAbort (pMe->m_pOutEdit);
    
        pMe->bTranscoderStoping = TRUE;
        if((pMe->eState != QCAMCORDER_TRANSCODE_UNSUPPORTED) &&
           (pMe->eState != QCAMCORDER_TRANSCODE_ERROR))
        {
           ISHELL_SendEvent (pMe->a.m_pIShell,
                           AEECLSID_QCAMCORDER,EVT_USER,
                           IMOVIEEDIT_TRANSCODER_STATE_DONE,
                           0);
        }

    }      
}
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE

#endif /* FEATURE_VIDEO_ENCODE_RAW */
//lint -restore
#endif //#if defined(FEATURE_APP_QCAMCORDER)

