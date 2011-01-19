/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Video Encode Engine")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     V I D E O   E N C O D E   E N G I N E

GENERAL DESCRIPTION
  This module contains video encode engine software.  This is layer which
  multiplexes compressed audio and video streams together into a multimedia
  file or sends them along to data services.

REFERENCES
  Video Encode Engine ISOD: 80-XXXXX-XX XX

EXTERNALIZED FUNCTIONS
  video_eng_init
    This function initializes the underlying audio and video compression
    layers, in preparation for recording.
  video_eng_start
    This function begins recording audio and video streams.
  video_eng_get_config
    This function gets the video engine config data.
  video_eng_set_config
    This function sets the video engine config data.
  video_eng_write_uuid
    This function is used to pass along user data atoms to be written to the
    movie file before the actual stream data.
  video_eng_pause
    This function pauses the current audio/video file being recorded.
  video_eng_resume
    This function resumes the recording of the current audio/video file.
  video_eng_stop
    This function stops recording audio and video, and finalizes the output
    data appropriately.
  video_eng_exit
    This function cleans up and exits the video engine.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2003-2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/rel/1.1.0/src/videoeng.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/10   kamit    Changes merged for simultaneous handling of failure and stop
05/11/09   kamit   Compiler Warning for VIDEO_ENG_FILE *fp removed. Variable intialised to null.
05/07/09   kamit   Temporary file writing for AMC moved to memory file.
04/28/09   sanal   NULL check for g_context.
01/30/09   sanal   Handling Fast pause resume 
01/30/09   ps       Added null check for g_context
01/28/09   sanal   Removed RVCT compiler warnings
12/17/08   sanal   Added return types for videoeng APIS to support synchronous
                   usage by client.
12/23/08   girip   Featurized audio recording for FTM camcorder
12/04/08   shiven  Fixed 3g2 frag recording if mmc is swapped out
11/23/08   Shiven  Updating handling of failure of efs_setvbuf
11/24/08   ps      Removed "u" flag from video_eng_efs_file_fopen as we dont need rollback
11/14/08   sanal   Store the temp files at the save location of movie clip
10/16/08   shiven  Klocwork fix
10/10/08   anil     RVCT compiler warning fixes
09/16/08   ps       Changed video_eng_space_check for grater than 4GB memory support.
07/16/08   sanal   Removed compilation error
07/16/08   sanal   Null Cheking for denominator variables in cmx_data_cb
07/08/08   shiven  Adding support for loci atom
07/04/08   ps      Removed RVCT compiler warnings
06/27/08   shiven  Fixed Klocwork errors
06/19/08   sanal   problems in video_eng_videofmt_callback, problems in 
                   avsync when audio frames dropped matches avsync duration
06/09/08   shiven  Max bit rate not working when EFS done
05/12/08   shiven  Changing from 16bit to 32bit ARM-DSP buffer alignment
05/08/08   shiven  Fix for swapped arguments of memset
04/22/08   sanal   Fix for crash while H263 post editing
04/18/08   shiven  Memory leak fix
04/04/08   sanal   Fix for last frame being dropped in post editing
04/01/08   anil    Fixed possible null pointer reference in video_eng_process_frame_done
03/17/08   anil    Added support for cprt field in udta atom
03/06/08   jdas    Klocwork error fix
02/11/08   sanal   Added frames dropped updation to idiag
01/29/08   Gopi    Added support for transcoder with movieedit interface
11/21/07   Yogi    Added Post Card Recording Support
11/20/07   gopi    Fixed last frame update problem for postediting.
11/19/07   dhaval  Removed compilation warnings
11/05/07   dhaval  Added copyright atom support
10/03/07   Yogi    Updating the file with review comments for Audio only recording 
                   related changes
09/28/07   dhaval  Delayed frame writing by one frame in offline encoding also
09/18/07   Gopi    Added support for File merge.
09/11/07   dhaval  Removed critical lint errors
09/03/07   Yogi    Replacing FEATURE_AAC_REC with FEATURE_CAMCORDER_AAC_REC in camcorder module 
08/10/07   dhaval  Skip delta calculation for offline encoding 
07/24/07   yogi    Added support for Audio only recording (Referred document- 80-VF034-1 A)
07/24/07   jdas    Wait for video_async task to complete all the 
                   pending request before terminate
06/21/07   dhaval  Do AV Sync when recording is resumed.
07/02/07   jdas    Workaround to fix Rapid start/stop recording crash
                   with AAC audio format
06/19/07   kadhir  Added FTL and USB as storage media
06/07/07   Kadhir  Corrected free space calculation for EFS
05/24/07   dhaval  Added null checking for g_context
05/09/07   dhaval  Moved profile and level writting block for H.264
05/09/07   dhaval  Do backend and frontend encoding sequentially for transcoding
05/02/07   gopi    Notify Client if re-order atom operation is not successfull.
04/27/07   jdas    Fixed problem of adding extra byte in framestat of skip atom
03/30/07   jdas    File opening in write mode made exclusive
03/19/07   gopi    Fixed SKM brand recording problems with AMR audio format.
03/18/07   dhaval  Compile file with CAMCORDER_MSG_ERROR
03/12/07   ali     Set the max_bitrate
02/27/07   jdas    Fix up 'ftyp' for only 1 fragment in fragmented format.
02/22/07   Gopi    Process stop commands in VIDEO_ENG_STATE_PAUSING state.
02/15/07   pavan   Merged fix for AMC file recording dog time out
02/09/07   ali     Changes for dynamic RC based on feedback from EFS
01/30/06   gopi    Added support to enable/disable audio/video truncation
01/09/07   Shahed  Modified camcorder audio stop/pause/resume to use
           MM API's. 
12/21/06   dhaval  Removed lint errors
12/06/06   pavan   Fix for Movieedit interface in case of AMC clips
12/01/06   dhaval  fix to record video clip with no audio format
11/30/06   Pavan   Added Support for Movieedit interface
11/30/06   Shahed  Move video_eng_cmx_sync_cb_func() outside feature
                   flag FEATURE_AAC_REC due to its usage in non-AAC cases.
11/21/06   Shahed  AV Sync related changes to fix fragmented file recording.
11/3/06    Gopi    Handling CMX_ERROR during recording
11/2/06    jdas    Send proper address in memset of video_eng_write_uuid
10/30/06   Pavan   Fixed data abort problem in case of transcoding with
                   AMC as target brand
10/30/06   dhaval  Setting space_limit_threshold and space_imminent_threshold
                   value according to user's settings from UI.
10/24/06   Gopi    Introduced memory allocation failure checks for uuid write
                   funcitons.
10/18/06   Gopi    Moved file close and unlink operations to Async Task.
09/06/06   dhaval  Removded the frame_callback_ptr usage which was added for
                   transcoder
09/04/06   dhaval  Added code to support VENC API for transcoder
09/01/06   kadhir  Removed unwanted Cross Compilation Dependency
                   When qcamcorder Feature Disabled
08/10/06   Nag     Error recovery added incase of error while Reordering atoms
                   in last fragment
08/07/06   dhaval  Audio recording is stopped for all formats while video engine fail
08/03/06   gopi    De-register g_frame timers before freeing g_frame context.
07/26/06   dw      Return from any file i/o errors when updating mehd atom for 3g2F files
07/20/06   Nag     Fixed the problem of 3G2 frag format due to multiple reordering of atoms
07/14/06   jdas    Added support for Qcom platform specific info in recorded clip
07/11/06   nag     Added init parameter to reorder of moov mdat atoms according to KDDI specification
06/06/06   jdas    Added support of fragment_duration in moov-mvex-mehd when
                   output set to VIDEO_METHOD_CLIENT
06/28/06   Gopi    Featurized VIDEO_FMT_REALLOC message due to branching in
                   Qtv.
06/28/06   Shahed  Fixed a divide-by-zero bug and checks for no-audio to
                   skip audio configuration to start recording (VFE).
06/22/06   dw      Prevent error recovery due to invalid DSP timer expiration
06/20/06   Gopi    Added support for dynamically updating titl,descp
                   auth atoms.
05/17/06   jdas    Added code to update H.264 profile/level appropriately
05/10/06   Nag     g_context->state changed to ensure callback is called once
                   we are in video_eng_handle_terminate
05/04/06   Shahed  New A/V sync implementation for AAC with CMX MM API sync signal.
05/04/06   cl      Changes to switch stream order.
04/20/06   Nag     wrapper added for malloc, free and realloc routines
04/17/06   Shahed  Added a new error handling case in video engine to recover
           /Wyh    from QDSP failure/dead in the middle of video encoding.
04/10/06   jdas    Removed lint errors
04/07/06   Shahed  Added code for audio/video sync at the beginning of a clip
                   to fix a/v sync issues due to VFE configuration delay.
03/27/06   dw      Allow video_encode_set/get_config through video engine
03/08/06   Nagesh  Video Async Buffer size set based on the Video Bit Rate
03/06/05   dw      Modified video engine behavior during space out events
02/15/06   ali     Set video_range to 1 to be spec compliant
02/10/06   Gopi    Featurized SPACE error/warning Changes to be consistent
                   with OEMCamera featurization(DETAILED_SPACE_OUT_MSG).
02/08/06   wyh     New levels for MPEG4 simple profile.
02/03/06   Gopi    Switched context of g_frame->clk callback to graph.
02/01/06   ali     Do not truncate audio in case of audio only recording
01/31/06   jk      Made aac_data_cb return right away if space_out is set.
01/04/06   ali     Prevent overflow of media_duration
01/04/06   ali     Change state to exiting for audio only record in
                   video_eng_exit_processing
12/23/05   jk      Write temporary files to the same location as the target
                   file with updated space check.
12/22/05   gm      Added events for DSP Download and Frame Drop
11/23/05   dw      Added error recovery support for VS packet shortages
11/23/05   Gopi    Added check for valid callback function address in function
                   video_eng_handshake.
11/18/05   gm      Remove divide-by-zero lint warnings.
11/14/05   Shahed New implementation of camcorder space status error/warning msgs.
11/11/05   Shahed Undo SPACE error/warning changes.
11/08/05   Shahed  Added support for more detailed messages for SPACE
                   error/warning.
11/09/05   wyh     Remove divide-by-zero lint warnings.
10/21/05   dw      Stop video recording when CMX returns any error status
10/17/05   gm      Automated script to modify the MSG macro's
10/15/05   wyh     For L4Linux compatibility, must set malloc'ed g_framre to 0
10/06/05   Gopi    1. Changes to video_eng_cmd_parser to handle exits in
                   error conditions for Init and start states
                   of Video engine.
                   2. Changes to check the status of video_encode_start and
                      control the code flow.
                   3. Changes to video_eng_audio_video_synchronize on setting
                     flag save_on_exit in different states of video enginge.
10/03/05   dw      Changes to video engine data cb for error handling / recovery
09/27/05   dw      Fixes for video engine state handling during file close
09/23/05   dw/aa   Enhancements for video engine error handling
09/09/05   dw      Changes for video driver to access N-frame video buffers
09/07/05   dw      Removed compiler warnings by initializing pointers
08/31/05   wyh     Video_eng_frame if called from ISR will detect if we're in
                   a REX task lock ... and complain loudly if it is.
08/25/05   wyh     1. Implement code review comments for video_eng_frame
                   2. Separate video_eng_frame from the rest of video_eng
                   3. Frame drop request drop the very next frame in pp buffer
08/22/05   Gopi    Coverity -Eliminated FORWARD_NULL in functions
                   video_eng_enc_normal_cb
08/17/05   kwa     Coverity - eliminated FORWARD_NULL in functions
                   video_eng_enc_req_cb and video_eng_cmx_normal_cb
                   by adding a return after an ERR_FATAL.  This change is
                   only required because Coverity does not recognize
                   ERR_FATAL as a fatal event.
8/17/05    gopi    Moved the video engine cleanup call back function of the
                   client to video_eng_handle_terminate.
07/18/05   jk      Merged from videoeng.c#92 in msmshared.
                   Added support for variable bitrate audio.
                   Added video_eng_get_config and video_eng_set_config.
                   Changed input argument to macro functions.
                   Simplified audio stream truncation.
                   Added deletion of the encoded file if video is enabled and
                   no video frame is encoded.
07/12/05   jk      Featurized CMX_REC_PAUSED/RESUMED for build compatibility.
07/05/05   jk      Added CMX callback status for AAC pause/resume.
                   Added support for CMX_QCP_SPACE_DONE.
                   Updated after code walk.
06/28/05   jk      Changed VIDEO_ENG_MAX_CHUNKS as per videofmt that make
                   every I-frame in fragments the first frame of a chunk.
06/14/05   jk      Added support for the AAC UI.
06/08/05   jk      Fixed video_eng_write_uuid.
05/31/05   jk      Removed video_eng_enc_normal_cb in video_encode_get_config.
                   Move variable initialization after video_fmt_create in
                   video_eng_start_processing.
05/31/05   jk      Added support for the AAC encoder.
                   Simplified video engine customization.
                   Moved video_fmt_create from init to start.
                   Added support to record very long clips, writing meta
                   data to a temporary storage.
                   Added space check for MMC in addition to flash.
                   Merged in a fix for MMC being pulled out during stopping.
                   - Added state check in video_eng_enc_failure
                   - If engine is stopping don't issue cmx stop again
05/12/05   jk      Limited profile@level update to MPEG4.
                   Merged in fix for audio samples table limit parameters.
04/28/05   jk      Replaced efs_statvfs() by fs_statfs().
04/12/05   jk      Fixed EFS free space check for AMC.
03/22/05   jk      Added support for 'fixed_fps_mode' and 'movie_size_limit'
                   to camera services layer. Used 'vide' and 'soun' handler.
03/16/05   jk      Fixed 'stts' atom related error in 3gp brand where rounding
                   error in delta accumulated.
03/07/05   jk      Included videoeng_fio.h.
03/04/05   jk      Replaced a call to video_eng_amc_fixup () by a call to
                   video_eng_fast_amc_fixup_reorder() for AMC.
03/03/05   jk      Fixed AMC.
03/01/05   jk      Fixed for fragmented 3g2 brand at 384Kbps.
                   Implemented video engine file object. Changed:
                   (1) to use video engine file object.
                   (2) to use temporary fragment either on EFS or on memory.
                   Fixed EFS free space calculation when temporary file is
                   on EFS.
02/24/05   wyh     Make video_eng_frame() more robust.
02/24/05   drl     Added support for user_bitrate.
02/22/05   jk      Fixed MPEG4 Profile@Level indication.
02/22/05   wyh     Raw Encoder API changes.
02/17/05   jk      Fixed offset problem in efs_fseek to a temporary file.
02/16/05   jk      Merged in changes from mainline #79. Major changes are:
                   - Added file size limit feature.
                   - Fixed file limit reached logic.
                   - Now collapsing all 'mdat' atoms together when recording.
                   - Replaced all TASKLOCK/TASKFREE with REX critical sections.
02/10/05   jk      Merged in changes from mainline #77. Major changes are:
                   - to set profile/level for H.263.
                   - to check for footer padding byte from audio stream and
                     discard it.
                   - to set video track media_timescale to 1000 for 3GP files.
                   - to support timed text encoding with telop to timed text
                     conversion.
01/31/05   wh      Moved video_eng_transcode_normal_cb to mp4_transcoder.c
01/25/05   wh      Cosmetic changes from code review of video_eng_frame api
01/21/05   wh      Make transcoder more robust. Pass encoder error to xcoder
01/21/05   wh      Eliminate all malloc()'s from the mp4 library.
01/20/05   wh      More robust video overlay by allowing for overlay changes
                   in preview mode.
01/15/05   wh      video_eng_frame() API.
01/14/05   jk      Made Lint Free. Made Changes for a new library.
12/29/04   wyh     Heap tracking support through videoeng_malloc();
12/10/04   jk      Changed video_enc_init_type.
12/07/04   jk      Updated audio stream truncation after code walk.
12/02/04   ali     Changes for transcoder
11/24/04   jk      Updated to truncate audio stream upon pause/stop.
11/23/04   jk      Updated for VIDEO_ENC_HEADER to be processed upon resuming
11/18/04   jk      Updated as per the code walk.
                   Deleted all video_eng_stream_XXX functions.
11/17/04   jk      Cleaned up and made it permanent to do without videoeng
                   video buffer. Replaced efs_statvfs per each frame by
                   calculation. Fixed AMR repeated header. Made DSP buffers
                   be 16 byte aligned. Used small buffer for ved_buf_ptr for
                   decoder specific header. Made it stop recording when video
                   encoder reports VIDEO_ABORTED.
11/12/04   jk      Fixed QCELP related bug in video_eng_handle_close.
                   Replaced fs_remove by efs_unlink, fs_statfs by efs_statvfs
11/03/04   jk      Buffer sharing among video encoder, video engine, and
                   videofmt.
                   Merged codes in from videoeng.c#71 in MSMSHARED except for
                   text stream related stuff.
10/01/04   jk      Added support for callbacks for partial frame and decoder
                   specific header
09/22/04   wyh     encoder packet size is in units of uint16's
09/14/04   pp      Get rid of too many CAMCORDER_MSG_HIGH()'s
09/06/04   pp/wyh  Added rate control support.  enc_pkt_size += 132 for
                   rho_qp_table.
04/21/04   rpw     Added VIDEO_FMT_BRAND_SKM brand type.
03/12/04   ali     Code cleanup
03/02/04   ali     Changes related to API change for video_encode_start
02/22/04   gr/sj   Added MMC support for buffer file I/O.
01/26/04   gr      Added support for buffered file I/O.
12/18/03   sa      Added code to close and remove the file if LIMIT_REACHED
                   set.
12/18/03   sa      Added check for VIDEO_ENG_STATE_STOPPING state at the top
                   of video_eng_stop_processing().
12/12/03   rpw     Added code to work around limitation in SFAT service where
                   writes cannot be larger than 65535 bytes.
12/02/03   rpw     Modified 3G2 and K3G QCELP audio descriptor to select
                   the 3GPP2 QCELP (QCELPSampleEntry).
11/24/03   rpw     Turned off repeating of VO/VOS headers for KDDI fragmented
                   file version of 3G2.
10/27/03   rpw     Added support for KDDI fragmented file version of 3G2.
09/20/03   sj      Normalize the delta time from the video encode driver
                   based on media_timescale
09/08/03   sj      Added support for variable frame rate.
08/11/03   sj      video_eng_stop may be called multiple times.
08/04/03   rpw     Revamp of code to separate out movie file writer and
                   integrate it into videofmt.
07/02/03   rpw     Fixed bug where CMX server data was not being returned
                   for audio data before the first packet, which the engine
                   handles differently (ignores rather than stores).
07/02/03   rpw     Fixed bug where engine was looking for VOL header even
                   for H.263 video.
06/30/03   rpw     Fixed bug where H.263 video had VOS/VO headers.
06/30/03   rpw     Added support for encoding H.263 video.
05/13/03   rpw     Removed erroneous VIDEO_SUCCESS callback when a failure
                   occurs while closing the movie file.
05/13/03   rpw     Changed fs_handle_open to output_open and moved outside
                   EFS featurization.
                   Made saving a movie with zero video frames a failure.
05/07/03   rpw     Added engine status codes VIDEO_LIMIT_NEAR and
                   VIDEO_LIMIT_REACHED.  The former is sent when the engine
                   determines it is about to run out of memory or flash.  The
                   latter is sent if the engine actually does run out of
                   memory or flash, in which case a video_eng_stop is
                   performed automatically.
                   Added code to save what the engine can of the file being
                   recorded, if a failure occurs.  If the file cannot be
                   saved, and is in the EFS, it is deleted before the engine
                   exits.
                   Added code to relax the constraint that every video chunk
                   past the first needed to have at most the number of
                   samples as in the previous chunk.
                   Tuned optimal chunk size and video stream buffer size.
04/14/03   rpw     Fixed OTI value for QCELP.
04/07/03   rpw     Added EVRCSampleEntry generation (when QTV interop
                   not enabled).
                   Added visual_object_sequence_end_code generation.
                   Replaced OEM_Malloc/OEM_Free with malloc/free.
                   Optimized writes of bitstream, combining 8-byte 'mdat'
                   header with video chunk.
                   Added calculations of average and maximum bitrates.
03/28/03   rpw     Removed "reflection_flag" from video_init_type.
03/27/03   rpw     Fixed bug in AMR header skipping, and code used to specify
                   AMR in cmx_qcp_record_reverse.
03/24/03   rpw     Changed frame rate to time increment and resolution.
03/24/03   rpw     Fixed bugs in writing audio data in offline mode.
03/17/03   rpw     Fixed bugs in "offline" mode found during unit testing
                   of IMovieFile interface.
03/10/03   rpw     New enhanced feature set added for "offline"
                   movie writing.
                   Added synchronous call capability for "offline" mode.
                   Added API for writing UUID atoms.
02/26/03   rpw     Added code to always throw away first frame of audio.
02/21/03   rpw     Renamed vdd_buf_ptr* to enc_frame_ptr* to reflect
                   actual usage, and increased enc_frame_ptr* buffers by 4
                   words to accomodate rate control parameters.
                   Fixed bug where CMX audio buffers weren't being released
                   in error conditions.
02/20/03   rpw     Fixed some ARM compiler warnings.
                   Changed 'ftyp' atom to indicate file type compatibility
                   with 3GPP2 rev 5.
                   Added some more CAMCORDER_MSG_HIGH messages to aid in debugging.
                   Revised rate set mask for AMR to indicate we only generate
                   12.2kbps frames, no SID or "no-data" frames.
                   Abbreviated some diag messages to allow whole message
                   to get through without being cut off.
02/18/03   rpw     Added 3GPP2 compliant AMR support.
                   Added work-around for CMX returning an eighth rate NULL
                   packet at the beginning of audio stream.
                   Added proper timestamp generation.
                   Added CAMCORDER_MSG_HIGH messages in callbacks for debugging
02/17/03   rpw     Added audio support.
11/01/02   rpw     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdlib.h>             /* Dynamic memory allocation               */
#include "AEEstd.h"
#include "camcorder_msg.h"
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Definitions for byte, word, etc.        */

#ifdef FEATURE_VIDEO_ENCODE
#include "videoengi_cust.h"     /* Customer specific engine definitions    */
#include "videoengi.h"          /* engine internal typedefs and prototypes */
#include "videoengi_macro.h"    /* engine macro functions                  */
#include "videoeng_amc.h"       /* AMC file format support functions       */
#ifdef FEATURE_VIDEO_ENG_TELOP_PARSER
#include "videoeng_telop.h"     /* telop to timed text conversion support  */
#endif /* FEATURE_VIDEO_ENG_TELOP_PARSER */
#include "video_enc.h"          /* Video Encode Interface                  */
#include "task.h"               /* task management services                */
#include "err.h"                /* Error logging/reporting services        */
#include <memory.h>             /* Memory manipulation routines            */
#include "clk.h"

#ifdef FEATURE_EFS2
#include "fs.h"                 /* EFS1 typedefs and prototypes            */
#include "fs_public.h"          /* File system typedefs and prototypes     */
#include "fs_stdio.h"           /* File system buffered I/O header         */
#include "fs_fcntl.h"           /* File control definitions                */
#include "video_async.h"        /* Asynchronous file I/O definitions       */
#include "videoeng_fio.h"       /* Video engine file I/O header            */
#endif /* FEATURE_EFS2 */

#ifdef FEATURE_MP4_TRANSCODER
#include "transcoder_task.h"
#endif //FEATURE_MP4_TRANSCODER

#ifdef FEATURE_VIDEO_CLIP_INFO
#include "mobile.h"
#include "camera.h"
#endif //FEATURE_VIDEO_CLIP_INFO
#ifdef FEATURE_VIDEO_MOVIEEDIT
#include "video_es_timestamp.h" /* Elementary Stream Support               */
#endif
#include "assert.h"
#include "event.h"

glb_avs_info_type glb_avs_info = {FALSE, FALSE, 0, 0};
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif//FEATURE_APP_TEST_AUTOMATION
/* <EJECT> */

/*lint -save -e655 bitwise operation uses compatible enumeration's */

/*===========================================================================

                          DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
** Video stream fixed part of bitstream header - these bytes are always
** pushed first into the video bitstream.  They comprise the video object
** sequence header and the visual object header
** (see ISO/IEC 14496-2 clause 6.2.2)
**---------------------------------------------------------------------------
*/
LOCAL const uint8 M4V_BITSTREAM_FIXED_HEADER [] = {
    0x00, 0x00, 0x01, 0xB0,  /* visual_object_sequence_start_code          */
    0x01,                    /* profile_and_level_indication = "Simple     */
                             /*   Profile/Level 1" (1)                     */
    0x00, 0x00, 0x01, 0xB5,  /* visual_object_start_code                   */
    0x0E,0xCF,                /* is_visual_object_identifer = 0             */
                             /* visual_object_type = "Video ID" (1)        */
                             /* video_signal_type = 1                      */
                             /* video_format = 101 "unspecified"           */
                             /* video_range = 1                            */
                             /* colour_description = 0                     */
                             /* next_start_code() = 0b01 (padding up to    */
                             /*   next byte boundary - see clause 5.2.4)   */
    0x00, 0x00, 0x01, 0x00   /* video_object_start_code                    */
};

/*---------------------------------------------------------------------------
** Video stream fixed part of bitstream footer - these bytes are always
** pushed last into the video bitstream.  They comprise the video object
** sequence end (see ISO/IEC 14496-2 clause 6.2.2)
**---------------------------------------------------------------------------
*/
LOCAL const uint8 M4V_BITSTREAM_FIXED_FOOTER [] = {
    0x00, 0x00, 0x01, 0xB1   /* visual_object_sequence_end_code            */
};

/*---------------------------------------------------------------------------
** Brand names - these correspond to the "major brand" and "compatible brands"
** to specify for different MP4 file brands indicated by the client.
**---------------------------------------------------------------------------
*/
LOCAL const char *MP4_MAJOR_BRAND = "isom";
LOCAL const char *MP4_COMPAT_BRANDS [] = {
    "mp41", "isom"
};
LOCAL const char *AMC_MAJOR_BRAND = "isom";
LOCAL const char *AMC_COMPAT_BRANDS [] = {
    "mp41"
};
LOCAL const char *_3GP_MAJOR_BRAND = "3gp5";
LOCAL const char *_3GP_COMPAT_BRANDS [] = {
    "3gp5", "isom"
};
LOCAL const char *_3G2_MAJOR_BRAND = "3g2a";
LOCAL const char *_3G2_COMPAT_BRANDS [] = {
    "3g2a"
};
LOCAL const char *K3G_MAJOR_BRAND = "k3g1";
LOCAL const char *K3G_COMPAT_BRANDS [] = {
    "k3g1"
};
LOCAL const char *SKM_MAJOR_BRAND = "skm2";
LOCAL const char *SKM_COMPAT_BRANDS [] = {
    "skm2", "k3g1", "3gp5"
};
LOCAL const char *FRAG_3G2_MAJOR_BRAND = "kddi";
LOCAL const char *FRAG_3G2_COMPAT_BRANDS [] = {
    "kddi", "3g2a"
};
LOCAL const char *FRAG_3G2_MAJOR_BRAND_NOT_AMR = "kddi";
LOCAL const char *FRAG_3G2_COMPAT_BRANDS_NOT_AMR [] = {
    "kddi", "3g2a", "mp42"
};


#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
/*---------------------------------------------------------------------------
** QCP File Headers - these arrays contain the data to store in the decoder
** specific information descriptor for an audio track containing QCELP-13K
** vocoded packets.
**---------------------------------------------------------------------------
*/
LOCAL const uint8 QCP_FILE_HEADER_13K_FIXED_FULL [QCP_FILE_HEADER_SIZE] = {
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
LOCAL const uint8 QCP_FILE_HEADER_13K_FIXED_HALF [QCP_FILE_HEADER_SIZE] = {
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

/* Array elements are the packet size in bytes for 13K. */
LOCAL const uint8 QCP_PACKET_SIZE_13K [] = {
    1,              /* Blank        */
    4,              /* Eighth rate  */
    8,              /* Quarter rate */
    17,             /* Half rate    */
    35,             /* Full rate    */
};

/* Array elements are the packet size in bytes for EVRC. */
LOCAL const uint8 QCP_PACKET_SIZE_EVRC [] = {
    1,              /* Blank        */
    3,              /* Eighth rate  */
    0,              /* Quarter rate */
    11,             /* Half rate    */
    23,             /* Full rate    */
};

#ifdef FEATURE_CAMCORDER_AAC_REC
#define AAC_SAMP_FREQ_ARRAY_SIZE 12
LOCAL const uint32 aac_sampling_frequencies [AAC_SAMP_FREQ_ARRAY_SIZE] = {
    96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
    16000, 12000, 11025, 8000
};

LOCAL const cmx_af_sample_rate_enum_type cmx_sample_rates [AAC_SAMP_FREQ_ARRAY_SIZE] = {
    CMX_AF_SAMPLE_RATE_96000,
    CMX_AF_SAMPLE_RATE_88200,
    CMX_AF_SAMPLE_RATE_64000,
    CMX_AF_SAMPLE_RATE_48000,
    CMX_AF_SAMPLE_RATE_44100,
    CMX_AF_SAMPLE_RATE_32000,
    CMX_AF_SAMPLE_RATE_24000,
    CMX_AF_SAMPLE_RATE_22050,
    CMX_AF_SAMPLE_RATE_16000,
    CMX_AF_SAMPLE_RATE_12000,
    CMX_AF_SAMPLE_RATE_11025,
    CMX_AF_SAMPLE_RATE_8000
};
LOCAL const cmx_af_aac_channel_enum_type cmx_channel_modes [3] = {
  CMX_AF_AAC_CHANNEL_UNKNOWN,
  CMX_AF_AAC_CHANNEL_MONO,       /* Single channel (mono) data    */
  CMX_AF_AAC_CHANNEL_DUAL        /* Stereo data                   */
};
#endif /* FEATURE_CAMCORDER_AAC_REC */
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/


LOCAL char *arr_dir[] =
{
  "/",
  "mmc1",
  "ftl1",
  "usb1"
};

/* For 32bit ARM-DSP Buffer Alignment*/
#define MEM_ALIGN_MASK 		    0xFFFFFFE0U
#define MEM_ALIGN_GUARD_BYTES	31

/*---------------------------------------------------------------------------
** Engine Context - this pointer is initialized to NULL and has the value
** NULL while the engine is unused.  When video_eng_init is called, memory is
** allocated for this structure, and the pointer indicates that the engine is
** in use.  After video_eng_exit returns, this pointer will return to NULL.
**---------------------------------------------------------------------------
*/
LOCAL video_eng_context_type  *g_context = NULL;

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
LOCAL video_eng_frame_context_type *g_frame = NULL;
LOCAL rex_crit_sect_type            videng_frame_cs;
#endif


#ifdef FEATURE_CAMCORDER_MALLOC
  camcorderutil_malloc_context_type *videoeng_malloc_context = NULL;
#endif


/* <EJECT> */
/*===========================================================================

                        LOCAL FUNCTIONS FOR MODULE

===========================================================================*/
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
LOCAL video_eng_frame_stat_type video_eng_frame_init(void);
LOCAL video_eng_frame_stat_type video_eng_frame_terminate(void);
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */
LOCAL void video_eng_handshake(void);
LOCAL void video_eng_process_frame_done(void);
LOCAL void video_eng_process_dsp_failure(void);
LOCAL video_status_type video_eng_detect_state(void);
LOCAL void video_eng_send_space_limit_status(void);
LOCAL void video_eng_process_clkcb(void);
extern uint32 mp4e_get_time_ms(void);
extern uint32 video_enc_get_bitrate(uint32 buffer_size, uint32 time);
extern void video_enc_log_start_drain(uint32 buffer_size, uint32 time);

#ifdef FEATURE_VIDEO_CLIP_INFO
LOCAL void video_eng_framedrop_stat(boolean);
#endif //FEATURE_VIDEO_CLIP_INFO
/*===========================================================================

FUNCTION n2hl

DESCRIPTION
  This function converts the given 32-bit unsigned integer from network
  order to host order.

DEPENDENCIES
  None

PARAMETERS
  IN unsigned long arg
    This is the network-order integer to converted.

RETURN VALUE
  The converted host-order integer is returned.

SIDE EFFECTS
  None

===========================================================================*/
static unsigned long n2hl (
  unsigned long arg
)
{
    unsigned char *p = (unsigned char *) &arg;
    return (((unsigned long) p[0]) << 24)
        | (((unsigned long) p[1]) << 16)
        | (((unsigned long) p[2]) << 8)
        | ((unsigned long) p[3]);
}

/* <EJECT> */
/*===========================================================================

                        PUBLIC FUNCTIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION video_eng_init

DESCRIPTION
  This procedure initializes the engine command queues.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  VIDEO_SUCCESS if command successfully queued to VS queue
  VIDEO_FAILURE elsewhere

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_init (
  const video_init_type   *init_data,
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
) {
    q_link_type            *link_item;
    int                    i;
    video_eng_packet_type  *packet;

    CAMCORDER_MSG_HIGH ("init: Entered function", 0, 0, 0);
    event_report(EVENT_DSP_VIDEO_ENC_DOWNLOAD_DONE);

    /* Verify that the engine is not already running. */
    if (g_context != NULL)
    {
        CAMCORDER_MSG_ERROR ("init: Video encode engine is already running!",
                   0, 0, 0);
        if (callback_ptr)
        {
          callback_ptr (VIDEO_FAILURE, client_data);
        }
        CAMCORDER_MSG_HIGH ("init: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Verify that either a callback function was given.
    */
    if (!callback_ptr)
    {
        CAMCORDER_MSG_ERROR ("init: no callback function!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("init: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    #ifdef FEATURE_CAMCORDER_MALLOC
    if (!videoeng_malloc_context)
    { static msg_const_type m;
      videoeng_malloc_context =
        camcorderutil_malloc_init(__MODULE__,__LINE__,&m);
    }
    #endif

    /* Allocate memory for the engine variables. */
    CAMCORDER_MSG_HIGH ("init: allocating context", 0, 0, 0);
    g_context = videoeng_malloc (sizeof (*g_context));
    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR ("init: Unable to allocate engine "
                   "variable structure!",
                   0, 0, 0);
        callback_ptr (VIDEO_FAILURE, client_data);
        CAMCORDER_MSG_HIGH ("init: Exiting function, early", 0, 0, 0);

        #ifdef FEATURE_CAMCORDER_MALLOC
        { static msg_const_type m;

          camcorderutil_malloc_terminate(videoeng_malloc_context,
                                         __LINE__,__func__,&m);
          videoeng_malloc_context = NULL;
        }
        #endif
        return VIDEO_FAILURE;
    }
    (void) memset (g_context, 0, sizeof (*g_context));

#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
    /* Copy Callback function address of the client.
    ** Clientcb function will be called by videoengine when recording
    ** ends.Client can pass the titl,auth and descp atoms.
    */
    g_context->useratom_cbptr = init_data->useratom_cbptr;
#endif /*FEATURE_VIDEO_SETATOMS_DYNAMIC*/

    /* Allocate memory for timed text input, if any. */
    if ((init_data->text != NULL)
        && (init_data->text_type != VIDEO_ENG_TEXT_INPUT_NONE))
    {
        CAMCORDER_MSG_HIGH ("init: allocating context", 0, 0, 0);
        g_context->text = videoeng_malloc (init_data->text_size);
        if (g_context->text == NULL)
        {
            videoeng_free (g_context);
            g_context = NULL;
            CAMCORDER_MSG_ERROR ("init: Unable to allocate timed text "
                       "input buffer!", 0, 0, 0);
            callback_ptr (VIDEO_FAILURE, client_data);
            CAMCORDER_MSG_HIGH ("init: Exiting function, early", 0, 0, 0);
            return VIDEO_FAILURE;
        }
        memcpy (g_context->text, init_data->text, init_data->text_size);
        g_context->text_size = init_data->text_size;
    }

    /* Initialize the command queues. */
    (void) q_init (&g_context->cmd_q);
    (void) q_init (&g_context->free_q);

    /* Add items to free_q. */
    for (i = 0; i < VIDEO_ENG_NUM_PACKETS; i++)
    {
        link_item = q_link (&g_context->free_packet [i],
                            &g_context->free_packet [i].hdr.link);
        q_put (&g_context->free_q, link_item);
    }

    /* Initialize engine critical section. */
    rex_init_crit_sect (&g_context->cs);

    if (init_data->frame_callback_ptr != NULL)
    {
        g_context->framecb_ptr = init_data->frame_callback_ptr;
        g_context->pEncFrame   = videoeng_malloc(sizeof(vencoder_frame_done_type));
    }
    else
    {
        g_context->framecb_ptr = NULL;
        g_context->pEncFrame   = NULL;
    }
    /* Pass along engine arguments to engine to finish initialization. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);
    if (packet != NULL)
    {
        packet->hdr.cmd = VIDEO_ENG_CMD_INIT;
        packet->init.cb_func = callback_ptr;
        packet->init.client_data = client_data;
        packet->init.args = *init_data;
        video_eng_cmd (packet);
    }
    else
    {
        if (g_context->text)
        {
            videoeng_free (g_context->text);
        }
        if(g_context->pEncFrame)
        {
          videoeng_free(g_context->pEncFrame);
        }
        videoeng_free (g_context);
        g_context = NULL;
        callback_ptr (VIDEO_FAILURE, client_data);
        CAMCORDER_MSG_HIGH ("init: Failure, exiting function", 0, 0, 0);
        #ifdef FEATURE_CAMCORDER_MALLOC
        {
          static msg_const_type m;
          camcorderutil_malloc_terminate(videoeng_malloc_context,
                                         __LINE__,__func__,&m);
          videoeng_malloc_context = NULL;
        }
        #endif

        return VIDEO_FAILURE;
    }

#ifdef FEATURE_EFS2
    /* Initialize the asynchronous file I/O task. */
    video_async_init ();
#endif /* FEATURE_EFS2 */

    CAMCORDER_MSG_HIGH ("init: Success, exiting function", 0, 0, 0);
    return VIDEO_SUCCESS;
} /* end video_eng_init */

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_start

DESCRIPTION
  This procedure starts recording audio/video.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  VIDEO_SUCCESS if command successfully queued to VS queue
  VIDEO_FAILURE elsewhere

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_start (
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
) {
    video_eng_packet_type  *packet;

    CAMCORDER_MSG_HIGH ("start: Entered function", 0, 0, 0);

    /* Verify that the engine is running. */
    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR ("start: Video encode engine is not running!", 0, 0, 0);
        if (callback_ptr)
        {
            callback_ptr (VIDEO_FAILURE, client_data);
        }
        CAMCORDER_MSG_HIGH ("start: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Verify that either a callback function was given.
    */
    if (!callback_ptr)
    {
        CAMCORDER_MSG_ERROR ("start: no callback function!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("start: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Pass command to engine for processing. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);
    if (packet != NULL)
    {
        packet->hdr.cmd = VIDEO_ENG_CMD_START;
        packet->base.cb_func = callback_ptr;
        packet->base.client_data = client_data;
        video_eng_cmd (packet);
    }
    else
    {
        callback_ptr (VIDEO_FAILURE, client_data);
        CAMCORDER_MSG_HIGH ("start: Failure, exiting function", 0, 0, 0);
        return VIDEO_FAILURE;
    }
    CAMCORDER_MSG_HIGH ("start: Success, exiting function", 0, 0, 0);
    return VIDEO_SUCCESS;
}



/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_get_config

DESCRIPTION
  This procedure gets the video engine config data.

DEPENDENCIES
  FEATURE_VS_TASK and FEATURE_VIDEO_ENCODE are defined.
  Must be called between calls to video_eng_init and video_eng_start.

RETURN VALUE
  The command is issued asynchronously, the function returns immediately,
  and the return value indicates whether or not the command was queued
  successfully.

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
video_status_type video_eng_get_config (
  video_config_type       *config_data,
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
) {
    video_eng_packet_type     *packet;

    CAMCORDER_MSG_HIGH ("get_config: Entered function", 0, 0, 0);

    /* Verify that the engine is running. */
    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR ("get_config: Video encode engine is not running!", 0, 0, 0);
        if (callback_ptr)
        {
            callback_ptr (VIDEO_FAILURE, client_data);
        }
        CAMCORDER_MSG_HIGH ("get_config: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Verify that config_data was given.
    */
    if (!config_data)
    {
        CAMCORDER_MSG_ERROR ("get_config: no buffer for config data!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("get_config: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Verify that either a callback function was given.
    */
    if (!callback_ptr)
    {
        CAMCORDER_MSG_ERROR ("get_config: no callback function!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("get_config: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Pass command to engine for processing. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);
    if (packet != NULL)
    {
        packet->hdr.cmd = VIDEO_ENG_CMD_GET_CONFIG;
        packet->get_config.cb_func = callback_ptr;
        packet->get_config.client_data = client_data;
        packet->get_config.p_args = config_data;
        video_eng_cmd (packet);
    }
    else
    {
        callback_ptr (VIDEO_FAILURE, client_data);
        CAMCORDER_MSG_HIGH ("get_config: Failure, exiting function", 0, 0, 0);
        return VIDEO_FAILURE;
    }
    CAMCORDER_MSG_HIGH ("get_config: Success, exiting function", 0, 0, 0);
    return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_set_config

DESCRIPTION
  This procedure sets the video engine config data.

DEPENDENCIES
  FEATURE_VS_TASK and FEATURE_VIDEO_ENCODE are defined.
  Must be called between calls to video_eng_init and video_eng_start.

RETURN VALUE
  The command is issued asynchronously, the function returns immediately,
  and the return value indicates whether or not the command was queued
  successfully.

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
video_status_type video_eng_set_config (
  const video_config_type *config_data,
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
) {
    video_eng_packet_type     *packet;

    CAMCORDER_MSG_HIGH ("set_config: Entered function", 0, 0, 0);

    /* Verify that the engine is running. */
    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR ("set_config: Video encode engine is not running!", 0, 0, 0);
        if (callback_ptr)
        {
            callback_ptr (VIDEO_FAILURE, client_data);
        }
        CAMCORDER_MSG_HIGH ("set_config: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Verify that config_data was given.
    */
    if (!config_data)
    {
        CAMCORDER_MSG_ERROR ("get_config: no config data!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("get_config: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Verify that either a callback function was given.
    */
    if (!callback_ptr)
    {
        CAMCORDER_MSG_ERROR ("set_config: no callback function!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("set_config: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Pass command to engine for processing. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);
    if (packet != NULL)
    {
        packet->hdr.cmd = VIDEO_ENG_CMD_SET_CONFIG;
        packet->set_config.cb_func = callback_ptr;
        packet->set_config.client_data = client_data;
        packet->set_config.args = *config_data;
        video_eng_cmd (packet);
    }
    else
    {
        callback_ptr (VIDEO_FAILURE, client_data);
        CAMCORDER_MSG_HIGH ("set_config: Failure, exiting function", 0, 0, 0);
        return VIDEO_FAILURE;
    }
    CAMCORDER_MSG_HIGH ("set_config: Success, exiting function", 0, 0, 0);
    return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_write_uuid

DESCRIPTION
  This procedure is used to pass along user data atom information to be
  written to the header of the movie file.  It is only available to be called
  after video_eng_init is called but before video_eng_start is called.  This
  is because once video_eng_start is called the header information will have
  been written out and the stream data will begin to be written out.

DEPENDENCIES
  FEATURE_VS_TASK and FEATURE_VIDEO_ENCODE are defined.
  Must be called between calls to video_eng_init and video_eng_start.

RETURN VALUE
  If a callback pointer is given, the command is issued asynchronously, the
  function returns immediately, and the return value indicates whether or not
  the command was queued successfully.  Otherwise, the command is issued
  synchronously, and the return value indicates the result of the command.

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
video_status_type video_eng_write_uuid (
  const uint8             *uuid,         /* must point to 16-element array */
  const void              *data,         /* data for atom contents */
  uint32                  size,          /* size of atom contents */
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
) {
    video_eng_packet_type     *packet = NULL;
    video_eng_udat_list_type  *udat   = NULL;

    CAMCORDER_MSG_HIGH ("write_uuid: Entered function", 0, 0, 0);

    /* Verify that the engine is running. */
    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR ("write_uuid: Video encode engine is not running!", 0, 0, 0);
        if (callback_ptr)
        {
            callback_ptr (VIDEO_FAILURE, client_data);
        }
        CAMCORDER_MSG_HIGH ("write_uuid: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /*
      Verify that either a callback function was given.
      if uuid is NULL or data pointer is NULL when size of data has been
      specified return.
    */
    if (!callback_ptr ||
        uuid == NULL  ||
    (data == NULL && size > 0)
        )
    {
        CAMCORDER_MSG_ERROR ("write_uuid: no callback function!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("write_uuid: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }
    /*Allocate memory for main structure*/
    udat = (video_eng_udat_list_type *)
            videoeng_malloc (sizeof (video_eng_udat_list_type));
    /*Check for allocation failure*/
    if (udat == NULL)
    {
        CAMCORDER_MSG_ERROR ("write_uuid: memory allocation failure!", 0, 0, 0);
        return VIDEO_FAILURE;
    }
    /*reset all the variables*/
    memset (udat,0,sizeof(video_eng_udat_list_type));

    /*
      If there is data that needs to copied allocate memory and
      copy the data.
    */
    if (size)
    {
        udat->data = videoeng_malloc (size);
        if (udat->data == NULL)
        {
           CAMCORDER_MSG_ERROR ("write_uuid: memory allocation failure!", 0, 0, 0);
           if (udat)
           {
               videoeng_free (udat);
           }
           return VIDEO_FAILURE;
        }
    }

    /* Pass command to engine for processing. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);
    if (packet != NULL)
    {
        packet->hdr.cmd = VIDEO_ENG_CMD_WRITE_UUID;
        packet->write_uuid.cb_func = callback_ptr;
        packet->write_uuid.client_data = client_data;
        memcpy (udat->uuid, uuid, 16);
        if (size)
        {
            memcpy (udat->data, data, size);
        }
        else
        {
            udat->data = NULL;
        }
        udat->size = size;
        packet->write_uuid.udat = udat;
        video_eng_cmd (packet);
    }
    else
    {
        /*Free allocated memory*/
        if (udat)
        {
            if (udat->data)
            {
                 videoeng_free (udat->data);
            }
            videoeng_free (udat);
        }
        callback_ptr (VIDEO_FAILURE, client_data);
        CAMCORDER_MSG_HIGH ("write_uuid: Failure, exiting function", 0, 0, 0);
        return VIDEO_FAILURE;
    }
    CAMCORDER_MSG_HIGH ("write_uuid: Success, exiting function", 0, 0, 0);
    return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_pause

DESCRIPTION
  This procedure pauses the current audio/video file being recorded.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  VIDEO_SUCCESS if command successfully queued to VS queue
  VIDEO_FAILURE elsewhere

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
video_status_type video_eng_pause (
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
) {
    video_eng_packet_type  *packet;

    CAMCORDER_MSG_HIGH ("pause: Entered function", 0, 0, 0);

    /* Verify that the engine is running. */
    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR ("pause: Video encode engine is not running!", 0, 0, 0);
        if (callback_ptr)
        {
            callback_ptr (VIDEO_FAILURE, client_data);
        }
        CAMCORDER_MSG_HIGH ("pause: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Verify that either a callback function was given.
    */
    if (!callback_ptr)
    {
        CAMCORDER_MSG_ERROR ("pause: no callback function!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("pause: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Pass command to engine for processing. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);
    if (packet != NULL)
    {
        packet->hdr.cmd = VIDEO_ENG_CMD_PAUSE;
        packet->base.cb_func = callback_ptr;
        packet->base.client_data = client_data;
        video_eng_cmd (packet);
    }
    else
    {
        callback_ptr (VIDEO_FAILURE, client_data);
        CAMCORDER_MSG_HIGH ("pause: Failure, exiting function", 0, 0, 0);
        return VIDEO_FAILURE;
    }
    CAMCORDER_MSG_HIGH ("pause: Success, exiting function", 0, 0, 0);
    return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_resume

DESCRIPTION
  This procedure resumes the recording of the current audio/video file.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

  VIDEO_SUCCESS if command successfully queued to VS queue
  VIDEO_FAILURE otherwise

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
video_status_type video_eng_resume (
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
) {
    video_eng_packet_type  *packet;

    CAMCORDER_MSG_HIGH ("resume: Entered function", 0, 0, 0);

    /* Verify that the engine is running. */
    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR ("resume: Video encode engine is not running!", 0, 0, 0);
        if (callback_ptr)
        {
            callback_ptr (VIDEO_FAILURE, client_data);
        }
        CAMCORDER_MSG_HIGH ("resume: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Verify that either a callback function was given.
    */
    if (!callback_ptr)
    {
        CAMCORDER_MSG_ERROR ("resume: no callback function!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("resume: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Pass command to engine for processing. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);
    if (packet != NULL)
    {
        packet->hdr.cmd = VIDEO_ENG_CMD_RESUME;
        packet->base.cb_func = callback_ptr;
        packet->base.client_data = client_data;
        video_eng_cmd (packet);
    }
    else
    {
        callback_ptr (VIDEO_FAILURE, client_data);
        CAMCORDER_MSG_HIGH ("resume: Failure, exiting function", 0, 0, 0);
        return VIDEO_FAILURE;
    }
    CAMCORDER_MSG_HIGH ("resume: Success, exiting function", 0, 0, 0);
    return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_stop

DESCRIPTION
  This procedure stops recording audio/video, saving the output file.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  VIDEO_SUCCESS if command successfully queued to VS queue
  VIDEO_DONE if video engine is already stopped. Client need not wait for 
              stop callback
  VIDEO_FAILURE elsewhere

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_stop (
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
) {
    video_eng_packet_type  *packet;

    CAMCORDER_MSG_HIGH ("stop: Entered function", 0, 0, 0);

    /* Verify that the engine is running. */
    if (g_context == NULL || g_context->state == VIDEO_ENG_STATE_EXITING)
    {
       /* If the engine isn't running, lets go ahead and declare
       ** exit as a success and exit
       */
        if (callback_ptr)
        {
            callback_ptr (VIDEO_SUCCESS, client_data);
        }
        CAMCORDER_MSG_HIGH ("stop: Success, exiting function", 0, 0, 0);
        return VIDEO_DONE;
    }

    /* Verify that either a callback function was given.
    */
    if (!callback_ptr)
    {
        CAMCORDER_MSG_ERROR ("stop: no callback function!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("stop: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Pass command to engine for processing. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);
    if (packet != NULL)
    {
        packet->hdr.cmd = VIDEO_ENG_CMD_STOP;
        packet->base.cb_func = callback_ptr;
        packet->base.client_data = client_data;
        video_eng_cmd (packet);
    }
    else
    {
        callback_ptr (VIDEO_FAILURE, client_data);
        CAMCORDER_MSG_HIGH ("stop: Failure, exiting function", 0, 0, 0);
        return VIDEO_FAILURE;
    }
    CAMCORDER_MSG_HIGH ("stop: Success, exiting function", 0, 0, 0);
    return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_exit

DESCRIPTION
  This function cleans up and exits the video engine.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_SUCCESS if command successfully queued to VS queue
  VIDEO_DONE if video engine has already exit or is exiting. Client need not 
              wait for callback
  VIDEO_FAILURE elsewhere

SIDE EFFECTS
  None.

===========================================================================*/
video_status_type video_eng_exit (
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
) {
    video_eng_packet_type  *packet;

    CAMCORDER_MSG_HIGH ("exit: Entered function", 0, 0, 0);

    /* Verify that the engine is running. */
    if (g_context == NULL || g_context->state == VIDEO_ENG_STATE_EXITING)
    {
       /* If the engine isn't running, lets go ahead and declare
       ** exit as a success and exit
       */
        if (callback_ptr)
        {
            callback_ptr (VIDEO_SUCCESS, client_data);
        }
        CAMCORDER_MSG_HIGH ("exit: Success, exiting function", 0, 0, 0);
        return VIDEO_DONE;
    }

    /* Verify that either a callback function was given.
    */
    if (!callback_ptr)
    {
        CAMCORDER_MSG_ERROR ("exit: no callback function!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("exit: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Pass command to engine for processing. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);
    if (packet != NULL)
    {
        packet->hdr.cmd = VIDEO_ENG_CMD_EXIT;
        packet->base.cb_func = callback_ptr;
        packet->base.client_data = client_data;
        video_eng_cmd (packet);
    }
    else
    {
        callback_ptr (VIDEO_FAILURE, client_data);
        CAMCORDER_MSG_HIGH ("exit: Failure, exiting function", 0, 0, 0);
        return VIDEO_FAILURE;
    }
    CAMCORDER_MSG_HIGH ("exit: Success, exiting function", 0, 0, 0);

    return VIDEO_SUCCESS;
}

/* <EJECT> */
#ifdef FEATURE_VIDEO_MOVIEEDIT
/*===========================================================================

FUNCTION video_eng_write_header

DESCRIPTION
  This procedure writes decoder specific info to output file.
  It is used in post editing recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_write_header (
  video_fmt_stream_type     stream_type,
  uint32                    header_size,
  const uint8              *header_data,
  video_cb_func_ptr_type    callback_ptr,
  void                     *client_data
)
{
    CAMCORDER_MSG_HIGH ("write_header: Entered function", 0, 0, 0);

    /* Verify that either a callback function was given.
    */
    if (!callback_ptr)
    {
        CAMCORDER_MSG_ERROR ("write_header: no callback function!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("write_header: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Verify that the engine is running. */
    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR (
          "write_header: Video encode engine is not running!",
          0, 0, 0);

        callback_ptr (VIDEO_FAILURE, client_data);

        CAMCORDER_MSG_HIGH ("write_header: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }


    if (stream_type == VIDEO_FMT_STREAM_VIDEO)
    {
        video_enc_enc_to_client_type  enc_to_client_data;
        video_enc_client_to_enc_type  client_to_enc_data;

        memcpy(g_context->ved_buf_ptr,
            header_data,
            header_size);

        enc_to_client_data.len = header_size;
        video_eng_enc_req_cb(
                  VIDEO_ENC_HEADER, &enc_to_client_data,
                  &client_to_enc_data,
                  client_data);

        if (client_to_enc_data.buf_ptr != NULL)
        {
          g_context->wvs.buf_ptr = client_to_enc_data.buf_ptr;
          g_context->wvs.buf_size = client_to_enc_data.buf_size;
          g_context->state = VIDEO_ENG_STATE_RECORDING;
          callback_ptr (VIDEO_SUCCESS, client_data);
        }
        else
        {
          callback_ptr (VIDEO_FAILURE, client_data);
        }
    }

    CAMCORDER_MSG_HIGH ("write_header: Success, exiting function", 0, 0, 0);
    return VIDEO_SUCCESS;
}

/*===========================================================================

FUNCTION video_eng_write_segment_header

DESCRIPTION
  This procedure queues a command which will writes text stream
  segment header to output file.
  It is used in post editing recording.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_write_segment_header (
  video_fmt_stream_type            stream_type,
  uint32                           header_size,
  uint8                            *header_data,
  video_cb_func_ptr_type           callback_ptr,
  void                             *client_data
) {
    video_eng_packet_type  *packet;

    CAMCORDER_MSG_HIGH ("write_segment_header: Entered function", 0, 0, 0);

    /* Verify that either a callback function was given.
    */
    if (!callback_ptr)
    {
        CAMCORDER_MSG_ERROR ("write_segment_header: no callback function!",
          0, 0, 0);
        CAMCORDER_MSG_HIGH ("write_segment_header: Exiting function, early",
          0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Verify that the engine is running. */
    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR (
          "write_segment_header: Video encode engine is not running!",
          0, 0, 0);
        callback_ptr (VIDEO_FAILURE, client_data);
        CAMCORDER_MSG_HIGH ("write_segment_header: Exiting function, early",
          0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Verify stream type is text stream. */
    if (stream_type != VIDEO_FMT_STREAM_TEXT)
    {
        CAMCORDER_MSG_ERROR ("write_segment_header: invalid stream type!",
          0, 0, 0);
        callback_ptr (VIDEO_FAILURE, client_data);
        CAMCORDER_MSG_HIGH ("write_segment_header: Exiting function, early",
          0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Pass command to engine for processing. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);
    if (packet != NULL)
    {
      packet->hdr.cmd = VIDEO_ENG_CMD_WRITE_SEGMENT_HEADER;
      packet->write_segment_header.type = stream_type;
      packet->write_segment_header.size = header_size;
      packet->write_segment_header.data = header_data;
      packet->write_segment_header.cb_func = callback_ptr;
      packet->write_segment_header.client_data = client_data;
      video_eng_cmd (packet);
    }
    else
    {
      callback_ptr (VIDEO_FAILURE, client_data);
      CAMCORDER_MSG_HIGH ("write_segment_header: Failure, exiting function", 0, 0, 0);
      return VIDEO_FAILURE;
    }
    return VIDEO_SUCCESS;
}


/*===========================================================================
FUNCTION video_eng_write_segment_header_processing

DESCRIPTION
  This procedure writes text stream segment header to output file
  in the appropriate task.
  It is used in post editing recording.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
//lint -esym(715,stream_type) Suppress stream_type not referenced
void video_eng_write_segment_header_processing (
  video_fmt_stream_type    stream_type,
  uint32                   header_size,
  const uint8             *header_data )
{
  /* Write the segment header to the movie file. */
  g_context->videofmt_write_header
    ((uint32)g_context->text_stream_num,
     header_size,
     header_data,
     g_context->videofmt_server_data,
     video_eng_videofmt_callback,
     NULL);

  while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE) &&
         (g_context->videofmt_status != VIDEO_FMT_FAILURE))
  {
    if (g_context->videofmt_continue == NULL)
    {
      CAMCORDER_MSG_ERROR ("write_segment_header_processing: bad continue "
        "function from videofmt!", 0, 0, 0);
      break;
    }
    else
    {
      g_context->videofmt_continue (g_context->videofmt_server_data);
    }
  }

  if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
  {
    g_context->videofmt_server_data = NULL;
    CAMCORDER_MSG_ERROR ("write_segment_header_processing: "
      "failed to write segment header", 0, 0, 0);
    video_eng_enc_failure (TRUE);
    g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
    return;
  }

  g_context->callback_ptr (VIDEO_SUCCESS, g_context->client_data);
}
//lint +esym(715,stream_type)

/*===========================================================================

FUNCTION video_eng_write_stream

DESCRIPTION
  This procedure queues a command which writes video/audio
  stream to output file.
  It is used in post editing recording.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_write_stream (
  video_fmt_stream_type            stream_type,
  uint32                           num_samples,
  video_fmt_sample_info_type      *sample_info,
  uint8                           *sample_data,
  video_cb_func_ptr_type           callback_ptr,
  void                            *client_data
)
{
    CAMCORDER_MSG_HIGH ("write_stream: Entered function", 0, 0, 0);

    /* Verify that either a callback function was given.
    */
    if (!callback_ptr)
    {
        CAMCORDER_MSG_ERROR ("write_stream: no callback function!", 0, 0, 0);
        CAMCORDER_MSG_HIGH ("write_stream: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    /* Verify that the engine is running. */
    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR (
          "write_stream: Video encode engine is not running!",
          0, 0, 0);

        callback_ptr (VIDEO_FAILURE, client_data);

        CAMCORDER_MSG_HIGH ("write_stream: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
    }

    switch (stream_type)
    {
    case VIDEO_FMT_STREAM_VIDEO:
       {
        g_context->wvs.curr_sample = 0;
        g_context->wvs.total_sample = num_samples;
        g_context->wvs.offset_base = sample_info->offset;
        g_context->wvs.sample_info = sample_info;
        g_context->wvs.sample_data = sample_data;
        return video_eng_write_video_stream (callback_ptr, client_data);
       }
    case VIDEO_FMT_STREAM_AUDIO:
       {
        uint32 i, len=0;

        for (i=0; i<num_samples; i++)
        {
          len += sample_info[i].size;
        }

        return video_eng_write_audio_stream (len, sample_data, 
                                             callback_ptr, client_data);
       }
    case VIDEO_FMT_STREAM_TEXT:
       {
         return video_eng_write_text_stream (num_samples,
                                      sample_info,
                                      sample_data,
                                      callback_ptr,
                                      client_data);
       }

    default:
        break;
    }
    CAMCORDER_MSG_ERROR ("write_stream: Invalid Stream!!!!", 0, 0, 0);
    return VIDEO_FAILURE;
}

/*===========================================================================
FUNCTION video_eng_audio_stream

DESCRIPTION
  This procedure queues a command which writes an audio
  stream to an output file.
  It is used in post editing recording.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_write_audio_stream (
  uint32                           len,
  uint8                           *sample_data,
  video_cb_func_ptr_type           callback_ptr,
  void                            *client_data
)
{
    video_eng_packet_type  *packet;

    /* Pass command to engine for processing. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);

    if (packet != NULL)
    {
      packet->hdr.cmd = VIDEO_ENG_CMD_WRITE_AUDIO_STREAM;
      packet->write_astream.len = len;
      packet->write_astream.data = sample_data;
      packet->write_astream.cb_func = callback_ptr;
      packet->write_astream.client_data = client_data;
      video_eng_cmd (packet);
    }
    else
    {
      callback_ptr (VIDEO_FAILURE, client_data);
      CAMCORDER_MSG_HIGH ("write_stream: Failure, exiting function", 0, 0, 0);
      return VIDEO_FAILURE;
    }
    return VIDEO_SUCCESS;
}

/*===========================================================================
FUNCTION video_eng_text_stream

DESCRIPTION
  This procedure queues a command which writes a text
  stream to an output file.
  It is used in post editing recording.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_write_text_stream (
  uint32                           num_samples,
  video_fmt_sample_info_type      *sample_info,
  uint8                           *sample_data,
  video_cb_func_ptr_type           callback_ptr,
  void                            *client_data
)
{
    video_eng_packet_type  *packet;

    /* Pass command to engine for processing. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);

    if (packet != NULL)
    {
      packet->hdr.cmd = VIDEO_ENG_CMD_WRITE_TEXT_STREAM;
      packet->write_tstream.num = num_samples;
      packet->write_tstream.info = sample_info;
      packet->write_tstream.data = sample_data;
      packet->write_tstream.cb_func = callback_ptr;
      packet->write_tstream.client_data = client_data;
      video_eng_cmd (packet);
    }
    else
    {
      callback_ptr (VIDEO_FAILURE, client_data);
      CAMCORDER_MSG_HIGH ("write_tstream: Failure, exiting function",
        0, 0, 0);
      return VIDEO_FAILURE;
    }
    return VIDEO_SUCCESS;
}

/*===========================================================================
FUNCTION video_eng_write_video_stream

DESCRIPTION
  This procedure queues a command which writes a video
  stream to an output file.
  It is used in post editing recording.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_write_video_stream (
  video_cb_func_ptr_type           callback_ptr,
  void                            *client_data
)
{
    video_eng_packet_type  *packet;

    /* Pass command to engine for processing. */
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);

    if (packet != NULL)
    {
      packet->hdr.cmd = VIDEO_ENG_CMD_WRITE_VIDEO_STREAM;
      packet->write_vstream.cb_func = callback_ptr;
      packet->write_vstream.client_data = client_data;
      video_eng_cmd (packet);
    }
    else
    {
      callback_ptr (VIDEO_FAILURE, client_data);
      CAMCORDER_MSG_HIGH ("write_stream: Failure, exiting function", 0, 0, 0);
      return VIDEO_FAILURE;
    }
    return VIDEO_SUCCESS;
}

/*===========================================================================
FUNCTION video_eng_convert_time_units

DESCRIPTION
  This function converts the given time units from one timescale to another.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
uint32 video_eng_convert_time_units
(
  uint32 units,
  uint32 from,
  uint32 to
)
{
    uint64 intermediate;

    /* For now, just use 64-bit integer math.  It might be more efficient to
    ** use Q-factor arithmetic in 32-bit integers, but this is more complex
    ** and currently beyond the developer's skills.
    */
    intermediate = (uint64) units;
    intermediate *= to;
    intermediate += from>>1;
    intermediate /= from;
    return (uint32) intermediate;
}
/*===========================================================================
FUNCTION video_eng_write_vstream_processing

DESCRIPTION
  This procedure writes a video stream to an output file
  in the appropriate task.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void video_eng_write_vstream_processing (video_status_type status)
{
    video_enc_enc_to_client_type enc_to_client_data;
    video_enc_client_to_enc_type client_to_enc_data;

    if ((g_context->wvs.curr_sample >= g_context->wvs.total_sample) ||
        (status == VIDEO_FAILURE))
    {
        g_context->callback_ptr (status, g_context->client_data);
    }
    else
    {
        uint32 size =
            g_context->wvs.sample_info[g_context->wvs.curr_sample].size;
        byte *pData = g_context->wvs.sample_data
            + g_context->wvs.sample_info[g_context->wvs.curr_sample].offset
            - g_context->wvs.offset_base;
        byte *esTimeData = NULL;

        if (g_context->video_format == VIDEO_FMT_STREAM_VIDEO_MPEG4)
        { /* remove footer in stream before elementary stream time stamp
             bit shifting */
          uint8 footer_size = sizeof(M4V_BITSTREAM_FIXED_FOOTER);
          if (memcmp(pData+size-footer_size, M4V_BITSTREAM_FIXED_FOOTER,
            footer_size) == 0)
            size -= footer_size;
        }

        // take care of elementary stream timestamp
        if (g_context->adjust_es_time)
        {
          // 8 bytes extra in case result of bit shifting is longer
          uint32 esTimeSize = size + 8;
          esTimeData = videoeng_malloc(esTimeSize);
          if (esTimeData == NULL)
          {
            g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
            return;
          }
          if(g_context->file_brand == VIDEO_FMT_BRAND_FRAG_3G2 
             || g_context->file_brand == VIDEO_FMT_BRAND_AMC
             || g_context->file_brand == VIDEO_FMT_BRAND_3GP)
          {
            g_context->wvs.sample_info[g_context->wvs.curr_sample].delta
              = video_eng_convert_time_units(
                                             g_context->wvs.sample_info[g_context->wvs.curr_sample].delta,
                                             g_context->media_timescale,
                                             g_context->video_time_resolution
                                           );
          }

          /* For post editing whatever delta we get is the time increment of the next frame
          So putting current frame's delta may not be a good idea. We can put the
          previous frame's delta added to overall video_duration 
          as current frame's time increment instead. */
          if ( video_eng_adjust_es_time (
            pData,
            size,
            esTimeData,
            &esTimeSize,
            g_context->video_duration,
            g_context->no_coding ? g_context->video_prev_frame_delta : 
            g_context->wvs.sample_info[g_context->wvs.curr_sample].delta,
            g_context->video_time_resolution) == VIDEO_SUCCESS)
          {
            pData = esTimeData;
            size = esTimeSize;
          }
        }

        enc_to_client_data.type =
          (g_context->wvs.sample_info[g_context->wvs.curr_sample].sync ==1?
          I_FRAME:P_FRAME);

        enc_to_client_data.delta =
           (int)g_context->wvs.sample_info[g_context->wvs.curr_sample].delta;

        if ( g_context->wvs.buf_size >= size )
        {
          enc_to_client_data.len = size;

          memcpy(g_context->wvs.buf_ptr, pData, enc_to_client_data.len);

          video_eng_enc_req_cb(
            VIDEO_ENC_FRAME,
            &enc_to_client_data,
            &client_to_enc_data,
            g_context->client_data);
        }
        else
        {
          enc_to_client_data.len = g_context->wvs.buf_size;
          if(g_context->wvs.buf_ptr)
          {
          memcpy(g_context->wvs.buf_ptr, pData,
            enc_to_client_data.len);
          }

          video_eng_enc_req_cb(
            VIDEO_ENC_PARTIAL_FRAME,
            &enc_to_client_data,
            &client_to_enc_data,
            g_context->client_data);

          if(client_to_enc_data.buf_ptr == NULL)
          {
              g_context->callback_ptr (VIDEO_FAILURE,
                                       g_context->client_data);
              return;
          }
          enc_to_client_data.len = size - g_context->wvs.buf_size;
          g_context->wvs.buf_ptr = client_to_enc_data.buf_ptr;
          g_context->wvs.buf_size = client_to_enc_data.buf_size;

          memcpy(g_context->wvs.buf_ptr,
            pData + size - enc_to_client_data.len,
            enc_to_client_data.len);

          video_eng_enc_req_cb(
            VIDEO_ENC_FRAME,
            &enc_to_client_data,
            &client_to_enc_data,
            g_context->client_data);
        }

        g_context->wvs.curr_sample++;
        g_context->wvs.buf_ptr = client_to_enc_data.buf_ptr;
        g_context->wvs.buf_size = client_to_enc_data.buf_size;
        if (esTimeData)
        {
          videoeng_free(esTimeData);
          esTimeData = NULL;
        }
    }
}

/* <EJECT> */
#endif /* FEATURE_VIDEO_MOVIEEDIT */
/*===========================================================================

FUNCTION video_eng_cmd_parser

DESCRIPTION
  This procedure processes requests received by video_eng_cmd().  See
  videoengi.h for a description of the actual Video Engine packets.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  video_eng_init() must have been called to init the command queue.

RETURN VALUE
  None.

SIDE EFFECTS
  cmd_q

===========================================================================*/
void video_eng_cmd_parser (void)
{
    video_eng_packet_type  *cmd_ptr = NULL;
    uint32                 processed_commands = 0;
#ifdef FEATURE_VIDEO_MOVIEEDIT
    video_status_type       rCode;
#endif
    //CAMCORDER_MSG_HIGH ("cmd_parser: Entered function", 0, 0, 0);

    /* Return without doing anything if the engine is running. */
    if (g_context == NULL)
    {
      CAMCORDER_MSG_HIGH ("cmd_parser: Exiting function, early", 0, 0, 0);
      return;
    }

    /* While there are commands to process, process each command. */
    while (!g_context->terminate
           && ((cmd_ptr = (video_eng_packet_type *) q_get (&g_context->cmd_q))
               != NULL))
    {
      /* If shortage of video engine packets, completely drain video engine queue */
      if(!g_context->packet_out)
      {
        /* Process each command type. */
        switch (cmd_ptr->hdr.cmd)
        {
        case VIDEO_ENG_CMD_INIT:
            CAMCORDER_MSG_HIGH ("cmd_parser: VIDEO_ENG_CMD_INIT", 0, 0, 0);
            g_context->callback_ptr = cmd_ptr->init.cb_func;
            g_context->client_data = cmd_ptr->init.client_data;
            video_eng_init_processing (&cmd_ptr->init.args);
            break;

        case VIDEO_ENG_CMD_START:
            CAMCORDER_MSG_HIGH ("cmd_parser: VIDEO_ENG_CMD_START", 0, 0, 0);
            g_context->callback_ptr = cmd_ptr->base.cb_func;
            g_context->client_data = cmd_ptr->base.client_data;
            video_eng_start_processing ();
            break;

        case VIDEO_ENG_CMD_GET_CONFIG:
            CAMCORDER_MSG_HIGH ("cmd_parser: VIDEO_ENG_CMD_GET_CONFIG", 0, 0, 0);
            g_context->callback_ptr = cmd_ptr->get_config.cb_func;
            g_context->client_data = cmd_ptr->get_config.client_data;
            video_eng_get_config_processing (cmd_ptr->get_config.p_args);
            break;

        case VIDEO_ENG_CMD_SET_CONFIG:
            CAMCORDER_MSG_HIGH ("cmd_parser: VIDEO_ENG_CMD_SET_CONFIG", 0, 0, 0);
            g_context->callback_ptr = cmd_ptr->set_config.cb_func;
            g_context->client_data = cmd_ptr->set_config.client_data;
            video_eng_set_config_processing (&cmd_ptr->set_config.args);
            break;

        case VIDEO_ENG_CMD_WRITE_UUID:
            CAMCORDER_MSG_HIGH ("cmd_parser: VIDEO_ENG_CMD_WRITE_UUID", 0, 0, 0);
            g_context->callback_ptr = cmd_ptr->write_uuid.cb_func;
            g_context->client_data = cmd_ptr->write_uuid.client_data;
            video_eng_write_uuid_processing (cmd_ptr->write_uuid.udat);
            break;

        case VIDEO_ENG_CMD_PAUSE:
            CAMCORDER_MSG_HIGH ("cmd_parser: VIDEO_ENG_CMD_PAUSE", 0, 0, 0);
            g_context->callback_ptr = cmd_ptr->base.cb_func;
            g_context->client_data = cmd_ptr->base.client_data;
            video_eng_pause_processing ();
            break;

        case VIDEO_ENG_CMD_RESUME:
            CAMCORDER_MSG_HIGH ("cmd_parser: VIDEO_ENG_CMD_RESUME", 0, 0, 0);
            g_context->callback_ptr = cmd_ptr->base.cb_func;
            g_context->client_data = cmd_ptr->base.client_data;
            video_eng_resume_processing ();
            break;

        case VIDEO_ENG_CMD_STOP:
            CAMCORDER_MSG_HIGH ("cmd_parser: VIDEO_ENG_CMD_STOP", 0, 0, 0);
            g_context->callback_ptr = cmd_ptr->base.cb_func;
            g_context->client_data = cmd_ptr->base.client_data;
            video_eng_stop_processing ();
            break;

        case VIDEO_ENG_CMD_EXIT:
            CAMCORDER_MSG_HIGH ("cmd_parser: VIDEO_ENG_CMD_EXIT", 0, 0, 0);
            g_context->callback_ptr = cmd_ptr->base.cb_func;
            g_context->client_data = cmd_ptr->base.client_data;
            video_eng_exit_processing ();
            break;

        case VIDEO_ENG_CMD_ENC_STATUS:
            CAMCORDER_MSG_HIGH ("cmd_parser: VIDEO_ENG_CMD_ENC_STATUS",
                      0, 0, 0);
            video_eng_enc_status_processing (cmd_ptr->enc_stat.status);
            break;

            case VIDEO_ENG_CMD_FAILURE:
            CAMCORDER_MSG_HIGH("cmd_parser: VIDEO_ENG_CMD_ENC_FAILURE",0,0,0);
            video_eng_enc_failure (cmd_ptr->failure.stop_audio);
            break;

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
        case VIDEO_ENG_CMD_CMX_STATUS:
            CAMCORDER_MSG_HIGH ("cmd_parser: VIDEO_ENG_CMD_CMX_STATUS",
                      0, 0, 0);
            video_eng_cmx_status_processing (cmd_ptr->cmx_stat.status);
            break;
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

        case VIDEO_ENG_CMD_PROCESS_VIDEO:
            CAMCORDER_MSG_HIGH ("cmd_parser: VIDEO_ENG_CMD_PROCESS_VIDEO len=%d",
                      cmd_ptr->process.len, 0, 0);
#ifdef FEATURE_VIDEO_MOVIEEDIT					
            rCode = video_eng_video_processing (cmd_ptr->process.len,
                                        cmd_ptr->process.sync,cmd_ptr->process.delta);
#else
            (void) video_eng_video_processing (cmd_ptr->process.len,
                                        cmd_ptr->process.sync,cmd_ptr->process.delta);
#endif							
#ifdef FEATURE_VIDEO_MOVIEEDIT
            if (g_context->no_coding)
            {
                video_eng_write_vstream_processing (rCode);
            }
#endif
            //CAMCORDER_MSG_HIGH ("cmd_parser: done processing video", 0, 0, 0);
            break;
#ifdef FEATURE_VIDEO_MOVIEEDIT
        case VIDEO_ENG_CMD_WRITE_VIDEO_STREAM:
            CAMCORDER_MSG_HIGH (
              "cmd_parser: VIDEO_ENG_CMD_WRITE_VIDEO_STREAM", 0, 0, 0);
            g_context->callback_ptr = cmd_ptr->write_vstream.cb_func;
            g_context->client_data = cmd_ptr->write_vstream.client_data;
            video_eng_write_vstream_processing (VIDEO_SUCCESS);
            break;

        case VIDEO_ENG_CMD_WRITE_AUDIO_STREAM:
            CAMCORDER_MSG_HIGH (
              "cmd_parser: VIDEO_ENG_CMD_WRITE_AUDIO_STREAM", 0, 0, 0);
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
            if (g_context->framecb_ptr == NULL) 
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
            {
            g_context->callback_ptr = cmd_ptr->write_astream.cb_func;
            g_context->client_data = cmd_ptr->write_astream.client_data;
            }
            video_eng_write_astream_processing ( cmd_ptr->write_astream.len,
              cmd_ptr->write_astream.data );
            break;

        case VIDEO_ENG_CMD_WRITE_TEXT_STREAM:
            CAMCORDER_MSG_HIGH (
              "cmd_parser: VIDEO_ENG_CMD_WRITE_TEXT_STREAM", 0, 0, 0);
            g_context->callback_ptr = cmd_ptr->write_tstream.cb_func;
            g_context->client_data = cmd_ptr->write_tstream.client_data;
            video_eng_write_tstream_processing ( cmd_ptr->write_tstream.num,
              cmd_ptr->write_tstream.info, cmd_ptr->write_tstream.data );
            break;

        case VIDEO_ENG_CMD_WRITE_SEGMENT_HEADER:
            CAMCORDER_MSG_HIGH (
              "cmd_parser: VIDEO_ENG_CMD_WRITE_SEGMENT_HEADER", 0, 0, 0);
            g_context->callback_ptr = cmd_ptr->write_segment_header.cb_func;
            g_context->client_data =
              cmd_ptr->write_segment_header.client_data;
            video_eng_write_segment_header_processing (
              cmd_ptr->write_segment_header.type,
              cmd_ptr->write_segment_header.size,
              cmd_ptr->write_segment_header.data );
            break;
#endif
            /* Illegal command, does not return. */
        case VIDEO_ENG_CMD_INVALID:
        default:
            CAMCORDER_MSG_ERROR ("cmd_parser: Illegal videoeng command %x",
                       cmd_ptr->hdr.cmd, 0, 0);
        }
      }

      /* Return VS packet to free q */
      q_put (&g_context->free_q, &cmd_ptr->hdr.link);

      /* Initiate error recovery if queue has been drained */
      if(g_context->packet_out && !q_cnt(&g_context->cmd_q))
      {
        CAMCORDER_MSG_HIGH("video engine queue emptied, initiate failure recovery",0,0,0);
        g_context->packet_out = FALSE;
        video_eng_enc_failure(g_context->audio_format ==
                              VIDEO_FMT_STREAM_AUDIO_NONE ? FALSE : TRUE);
      }

      /* Add a check to make sure that we video engine does not process
      ** more then 15 commands in one iteration
      ** If this check was not there, it would probably starve other
      ** processes
      */
      ++processed_commands;
      CAMCORDER_MSG_HIGH ("Engine processed %d commands", processed_commands, 0, 0);
      if ((processed_commands > 15) && !g_context->terminate
          && (g_context->cmd_q.cnt > 0))
      {
        (void) rex_set_sigs (&vs_tcb, VS_VIDEO_ENG_CMD_Q_SIG);
        break;
      }
    } /* end of while "commands to process" */

    /* If the terminate flag is set, free the engine variables. */
    if (g_context->terminate)
    {
        /*close the video encoder*/
        if (g_context->state != VIDEO_ENG_STATE_EXITING
            && g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE &&
            g_context->no_coding == FALSE)
        {
            (void)video_encode_exit(NULL,NULL);
        }

        CAMCORDER_MSG_HIGH ("cmd_parser: Terminate flag was set", 0, 0, 0);
        video_eng_handle_terminate ();
    }
    //CAMCORDER_MSG_HIGH ("cmd_parser: Exiting function", 0, 0, 0);
} /* end video_eng_cmd_parser */

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmd

DESCRIPTION
  Queue a command for processing by the Video Engine.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  video_eng_init() needs to have initialized the queues.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  vs_tcb

===========================================================================*/
void video_eng_cmd
(
  video_eng_packet_type *video_eng_cmd_ptr
    /* pointer to VS command */
)
{
    /* Init link field */
    (void) q_link (video_eng_cmd_ptr, &video_eng_cmd_ptr->hdr.link);

    /* Put on command queue */
    q_put (&g_context->cmd_q, &video_eng_cmd_ptr->hdr.link);

    /* Signal a queue event */
    (void) rex_set_sigs (&vs_tcb, VS_VIDEO_ENG_CMD_Q_SIG);
} /* end video_eng_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmd_get_pkt

DESCRIPTION
  Get an Engine Command Packet.  Use by callers of video_eng_cmd().

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  video_eng_init() needs to have initialized the queues.

RETURN VALUE
  A pointer to an available video_eng_packet_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  free_q (and the packet fetched off free_q)

===========================================================================*/
video_eng_packet_type *video_eng_cmd_get_pkt (
  video_eng_queue_priority_type  priority
) {
    video_eng_packet_type  *packet = NULL;
    int                    num_free;

    /* If we have earlier set g_context->packet_out, we ensure no more
    ** VS packets get queued to recover from this failure scenario */
    if(g_context->packet_out)
    {
      CAMCORDER_MSG_ERROR ("cmd_get_pkt: No more pkts, stop queueing", 0, 0, 0);
      return NULL;
    }

    /* If request is normal priority, make sure free queue has at least the
    ** reserve amount of packets in it.  Get a packet only if we have at least
    ** the reserve amount, or if the request is high priority.
    */
    rex_enter_crit_sect (&g_context->cs);
    switch (priority)
    {
      case VIDEO_ENG_QUEUE_PRI_NORMAL:
        /* Get a packet only if we have at least the reserve amount. */
        num_free = q_cnt (&g_context->free_q);
        if (num_free > VIDEO_ENG_QUEUE_RESERVE_URGENT +
                       VIDEO_ENG_QUEUE_RESERVE_CRITICAL)
        {
            packet = (video_eng_packet_type *) (q_get (&g_context->free_q));
        }
        break;

        case VIDEO_ENG_QUEUE_PRI_URGENT:
            num_free = q_cnt (&g_context->free_q);
        if (num_free > VIDEO_ENG_QUEUE_RESERVE_CRITICAL)
        {
          packet = (video_eng_packet_type *) (q_get (&g_context->free_q));
        }
        break;

      case VIDEO_ENG_QUEUE_PRI_CRITICAL:
        /* Always try to get a packet. */
        packet = (video_eng_packet_type *) (q_get (&g_context->free_q));
        break;

      default:
        CAMCORDER_MSG_ERROR ("cmd_get_pkt: Illegal priority %x",
                             (int) priority, 0, 0);
    }
    rex_leave_crit_sect (&g_context->cs);

    /* Report an error to stop recording if we were not able to get a packet. */
    if (packet == NULL)
    {
        CAMCORDER_MSG_ERROR ("cmd_get_pkt: Ran out of Video Engine packets!", 0, 0, 0);
        g_context->packet_out = TRUE;
        return NULL;
    }

    /* Otherwise, initialize the packet to a known initial state. */
    else
    {
        /* Fill the entire Video Engine packet with 0's to get default
        ** behavior on un-initialized fields.
        */
        (void) memset (packet, 0, sizeof (video_eng_packet_type));
        packet->hdr.cmd = VIDEO_ENG_CMD_INVALID;  /* Invalid Command Value */
    }

    return (packet);
} /* end video_eng_cmd_get_pkt */

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_init_processing

DESCRIPTION
  This function initializes the video and audio encoders in preparation for
  recording a video file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_init_processing (
  const video_init_type  *args
) {
    video_enc_init_type                  enc_init;
    video_status_type                    encode_init_status = VIDEO_SUCCESS;
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
    video_eng_frame_stat_type            eng_frame_init_status = VIDENG_FRAME_SUCCESS;
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */

#ifdef FEATURE_VIDEO_CLIP_INFO
    video_fmt_clipinfo_type * clipinfo = &g_context->clipinfo;
    camera_info_type sensor;
    memset(&g_context->clipinfo,0,sizeof(video_fmt_clipinfo_type));
    /* enabling to log message */
    log_msg = TRUE;
#endif //FEATURE_VIDEO_CLIP_INFO

    /*-----------------------------------------------------------------------
    ** Verify that the various format selections are supported.
    **-----------------------------------------------------------------------
    */
    g_context->video_start_time = 0;
    /* Only MP4 file format is supported at this time. */
    if (args->file_format != VIDEO_FMT_MP4)
    {
        CAMCORDER_MSG_ERROR ("init_processing: given file format "
                   "not supported!", 0, 0, 0);
        g_context->failure_on_exit = TRUE;
        g_context->terminate = TRUE;
        return;
    }

    /* If no video stream, verify audio stream. */
    if (args->video_format == VIDEO_FMT_STREAM_VIDEO_NONE)
    {
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE        
        if (args->audio_format == VIDEO_FMT_STREAM_AUDIO_NONE)
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
        {
            CAMCORDER_MSG_ERROR ("init_processing: cannot record movie "
                       "without both video and audio!", 0, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }
    }

    /* Otherwise, perform checks that apply to video. */
    else
    {
        /* Verify width and height of video are both multiples of 16. */
        if (args->frame_width & 15)
        {
            CAMCORDER_MSG_ERROR ("init_processing: given width is not a multiple of 16!",
                       0, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }
        if (args->frame_height & 15)
        {
            CAMCORDER_MSG_ERROR ("init_processing: given height is not a"
                       " multiple of 16!", 0, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }

        /* Verify sanity of time increment and resolution. */
        if (args->time_increment < 1)
        {
            CAMCORDER_MSG_ERROR ("init_processing: zero time increment!",
                       0, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }
        if (args->time_increment > args->time_resolution)
        {
            CAMCORDER_MSG_ERROR ("init_processing: too big: time increment",
                       0, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }
        if (args->time_resolution < 1)
        {
            CAMCORDER_MSG_ERROR ("init_processing: zero time resolution!",
                       0, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }

        if ((args->video_format != VIDEO_FMT_STREAM_VIDEO_MPEG4)
            && (args->video_format != VIDEO_FMT_STREAM_VIDEO_H263)
#ifdef FEATURE_H264_ENCODER
            && (args->video_format != VIDEO_FMT_STREAM_VIDEO_H264)
#endif
       )
        {
            CAMCORDER_MSG_ERROR ("init_processing: given video format "
                       "not supported!", 0, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }
    }
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE  
#ifndef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
    if (g_context->framecb_ptr == NULL)
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
    {
        /* Verify audio format is one of those supported. */
        if ((args->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
            && (args->audio_format != VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL)
            && (args->audio_format != VIDEO_FMT_STREAM_AUDIO_QCELP13K_HALF)
            && (args->audio_format != VIDEO_FMT_STREAM_AUDIO_EVRC)
            && (args->audio_format != VIDEO_FMT_STREAM_AUDIO_AMR)
#ifdef FEATURE_CAMCORDER_AAC_REC
            && (args->audio_format != VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
#endif /* FEATURE_CAMCORDER_AAC_REC */
           )
        {
            CAMCORDER_MSG_ERROR ("init_processing: given audio format "
                        "not supported!", 0, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }

#ifdef FEATURE_CAMCORDER_AAC_REC
        if (args->audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
        {
            switch (args->audio_channel)
            {
            case VIDEO_ENG_AUDIO_CHANNEL_MONO_2_MONO:
            case VIDEO_ENG_AUDIO_CHANNEL_STEREO_2_MONO:
                g_context->aac_channel_mode = 1;
                break;

            case VIDEO_ENG_AUDIO_CHANNEL_STEREO_2_STEREO:
                g_context->aac_channel_mode = 2;
                break;

            default:
                CAMCORDER_MSG_ERROR ("init_processing: given audio channel "
                          "type not supported!", 0, 0, 0);
                g_context->failure_on_exit = TRUE;
                g_context->terminate = TRUE;
                return;
            }

            switch (args->audio_sample_rate)
            {
            case VIDEO_ENG_AUDIO_SAMPLE_RATE_8000:
                g_context->aac_sampling_rate_index
                    = (uint32)VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_8000;
                break;

            case VIDEO_ENG_AUDIO_SAMPLE_RATE_11025:
                g_context->aac_sampling_rate_index
                    = (uint32)VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_11025;
                break;

            case VIDEO_ENG_AUDIO_SAMPLE_RATE_16000:
                g_context->aac_sampling_rate_index
                    = (uint32)VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_16000;
                break;

            case VIDEO_ENG_AUDIO_SAMPLE_RATE_22050:
                g_context->aac_sampling_rate_index
                    = (uint32)VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_22050;
                break;

            case VIDEO_ENG_AUDIO_SAMPLE_RATE_24000:
                g_context->aac_sampling_rate_index
                    = (uint32)VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_24000;
                break;

            default:
                CAMCORDER_MSG_ERROR ("init_processing: given audio sampling "
                          "rate not supported!", 0, 0, 0);
                g_context->failure_on_exit = TRUE;
                g_context->terminate = TRUE;
                return;
            }

            switch (args->audio_profile)
            {
            case VIDEO_ENG_AUDIO_PROFILE_LOW:
                g_context->aac_bits_per_sample = AAC_BITS_PER_SAMPLE_LOW;
                break;

            case VIDEO_ENG_AUDIO_PROFILE_MED:
                g_context->aac_bits_per_sample = AAC_BITS_PER_SAMPLE_MED;
                break;

            case VIDEO_ENG_AUDIO_PROFILE_HIGH:
                g_context->aac_bits_per_sample = AAC_BITS_PER_SAMPLE_HIGH;
                break;

            default:
                CAMCORDER_MSG_ERROR ("init_processing: given audio profile "
                          "not supported!", 0, 0, 0);
                g_context->failure_on_exit = TRUE;
                g_context->terminate = TRUE;
                return;
            }
        }

#ifdef FEATURE_VIDEO_CLIP_INFO
        clipinfo->audio_channel = (uint8)args->audio_channel;
        clipinfo->audio_profile = (uint8)args->audio_profile;
        clipinfo->audio_sample_rate = (uint8)args->audio_sample_rate;
#endif //FEATURE_VIDEO_CLIP_INFO

#endif /* FEATURE_CAMCORDER_AAC_REC */
    }
#ifndef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
    else
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    {
        g_context->client_to_enc_buffer = malloc(args->frame_width * args->frame_height * 3);
        g_context->enc_buffer_size = args->frame_width * args->frame_height * 3;
    }
    /* Verify timed text format is one of those supported. */
    if ((args->text_type != VIDEO_ENG_TEXT_INPUT_NONE)
#ifdef FEATURE_VIDEO_ENG_TELOP_PARSER
        && (args->text_type != VIDEO_ENG_TEXT_INPUT_TELOP)
#endif /* FEATURE_VIDEO_ENG_TELOP_PARSER */
        && (args->text_type != VIDEO_ENG_TEXT_INPUT_3GPP) )
    {
        CAMCORDER_MSG_ERROR (
          "init_processing: given timed text input format not supported!",
          0, 0, 0);
        g_context->failure_on_exit = TRUE;
        g_context->terminate = TRUE;
        return;
    }

    /* real-time recording does not support VIDEO_ENG_TEXT_INPUT_3GPP format */
    if ( (args->text_type == VIDEO_ENG_TEXT_INPUT_3GPP)
      && (args->no_coding == FALSE) )
    {
        CAMCORDER_MSG_ERROR ("init_processing: given timed text input format "
                   "not supported!", 0, 0, 0);
        g_context->failure_on_exit = TRUE;
        g_context->terminate = TRUE;
        return;
    }

    /* Verify file brand is one of those supported. */
    if ((args->file_brand != VIDEO_FMT_BRAND_MP4)
        && (args->file_brand != VIDEO_FMT_BRAND_AMC)
        && (args->file_brand != VIDEO_FMT_BRAND_3GP)
        && (args->file_brand != VIDEO_FMT_BRAND_3G2)
        && (args->file_brand != VIDEO_FMT_BRAND_K3G)
        && (args->file_brand != VIDEO_FMT_BRAND_SKM)
        && (args->file_brand != VIDEO_FMT_BRAND_FRAG_3G2))
    {
        CAMCORDER_MSG_ERROR ("init_processing: given file brand "
                   "not supported!", 0, 0, 0);
        g_context->failure_on_exit = TRUE;
        g_context->terminate = TRUE;
        return;
    }

    g_context->no_coding = args->no_coding;
    g_context->adjust_es_time = FALSE;

    /*-----------------------------------------------------------------------
    ** Allocate memory for video encoder and initialize video encoder.
    **-----------------------------------------------------------------------
    */
    /* Store interesting data from the argument structure. */
    g_context->file_format = args->file_format;
    g_context->file_brand = args->file_brand;
    g_context->video_format = args->video_format;
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    g_context->audio_format = args->audio_format;
#else /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
#error code not present
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    g_context->text_type = args->text_type;
    g_context->output = args->destination;

    /*If output method is EFS do strlcpy explicitly
      to ensure NULL termination of filename*/
    if(((args->destination.method == VIDEO_METHOD_EFS) &&
          std_strlcpy(g_context->output.efs.filename, 
                      args->destination.efs.filename,
                      FS_FILENAME_MAX_LENGTH_P) > FS_FILENAME_MAX_LENGTH_P) ||
        ((args->destination.method == VIDEO_METHOD_MEM)&&
            (args->destination.mem.buf == NULL ||
             args->destination.mem.len == 0))                               ||
        ((args->destination.method == VIDEO_METHOD_CLIENT)&&
            (args->destination.client.data_push_fn == NULL))) 
    {
        CAMCORDER_MSG_ERROR ("init_processing: movie destination "
                   "not initialized properly ", 0, 0, 0);
        g_context->failure_on_exit = TRUE;
        g_context->terminate = TRUE;
        return;        
    }
    g_context->video_time_increment = args->time_increment;
    g_context->video_time_resolution = args->time_resolution;
    g_context->frame_width = args->frame_width;
    g_context->frame_height = args->frame_height;
    g_context->movie_size_limit = args->movie_size_limit;
    g_context->size_cb = args->size_cb;
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    g_context->audio_data1_cb = args->audio_data1_cb;
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    g_context->reorder_atom = args->reorder_atom;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif//FEATURE_APP_TEST_AUTOMATION
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */

    switch(args->space_limit)
    {
    case CAMCORDER_SPACE_LIMIT_LOW:
        g_context->space_limit_threshold
                 = VIDEO_ENG_LIMIT_NEAR_THRESHOLD_LOW;
        g_context->space_imminent_threshold
                 = VIDEO_ENG_LIMIT_IMMINENT_THRESHOLD_LOW;
        break;
    case CAMCORDER_SPACE_LIMIT_MED:
        g_context->space_limit_threshold
                 = VIDEO_ENG_LIMIT_NEAR_THRESHOLD_MED;
        g_context->space_imminent_threshold
                 = VIDEO_ENG_LIMIT_IMMINENT_THRESHOLD_MED;
        break;
    case CAMCORDER_SPACE_LIMIT_HIGH:
        g_context->space_limit_threshold
                 = VIDEO_ENG_LIMIT_NEAR_THRESHOLD_HIGH;
        g_context->space_imminent_threshold
                 = VIDEO_ENG_LIMIT_IMMINENT_THRESHOLD_MED;
        break;
    case CAMCORDER_SPACE_LIMIT_DEFAULT:
    default:
        g_context->space_limit_threshold
                 = VIDEO_ENG_LIMIT_NEAR_THRESHOLD;
        g_context->space_imminent_threshold
                 = VIDEO_ENG_LIMIT_IMMINENT_THRESHOLD;
        break;
    }
g_context->dynamic_bitrate_enable = args->dynamic_bitrate_enable;
g_context->max_bitrate_enable = args->max_bitrate_enable;
if(g_context->no_coding == FALSE)
{
#ifdef FEATURE_VIDEO_CLIP_INFO
    /* Storing static informations*/
    clipinfo->build_id_len = strlen(mob_sw_rev);
    clipinfo->build_id = malloc (clipinfo->build_id_len);
    memcpy(clipinfo->build_id,mob_sw_rev,clipinfo->build_id_len);
    camera_get_info(&sensor);
    memcpy(clipinfo->sersor_model,sensor.sensor_model,VIDEO_FMT_SERSOR_MODEL_SIZE);
    clipinfo->vfe_dsp_id    = *QDSP_vfeVersion;
    clipinfo->backend_dsp_id = *QDSP_qcamcorderVersion;
    clipinfo->qdspmodule = (uint8)CAMQDSP_GetCurrentModule();

    /* Storing initialization information into clipinfo */
    clipinfo->file_format = (uint8)g_context->file_format;
    clipinfo->file_brand = (uint8)g_context->file_brand ;
    clipinfo->video_format = (uint8)g_context->video_format;
    clipinfo->audio_format = (uint8)g_context->audio_format;
    clipinfo->frame_width = g_context->frame_width;
    clipinfo->frame_height = g_context->frame_height;
    clipinfo->video_time_increment = (uint16)g_context->video_time_increment;
    clipinfo->video_time_resolution = (uint16)g_context->video_time_resolution;
    clipinfo->text_type = (uint8)g_context->text_type;
    clipinfo->method = (uint8)g_context->output.method;
    clipinfo->movie_size_limit = g_context->movie_size_limit;
    clipinfo->reorder_atom = g_context->reorder_atom;
    if(g_context->text)
    {
        clipinfo->text_size = g_context->text_size;
        clipinfo->text = g_context->text;
    }
#endif //FEATURE_VIDEO_CLIP_INFO
}
    if (args->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
    {
        /* Allocate memory for the various buffers the video encoder needs. */
        CAMCORDER_MSG_HIGH ("init_processing: Allocating buffer memory",
                  0, 0, 0);
        /* VIDEO_ENG_MB_BUF_SIZE is in words. Convert it into bytes */
        g_context->enc_pkt_size
            = (args->frame_width * args->frame_height * VIDEO_ENG_MB_BUF_SIZE
              * sizeof(uint16) / 256) + 132;
        g_context->enc_pkt_ptr1 = videoeng_malloc(g_context->enc_pkt_size + MEM_ALIGN_GUARD_BYTES);
        g_context->enc_pkt_ptr2 = videoeng_malloc(g_context->enc_pkt_size + MEM_ALIGN_GUARD_BYTES);
        g_context->vop_recon_buf_ptr1 = videoeng_malloc
            (args->frame_width * args->frame_height * 3 / 2 + MEM_ALIGN_GUARD_BYTES);
        g_context->vop_recon_buf_ptr2 = videoeng_malloc
            (args->frame_width * args->frame_height * 3 / 2 + MEM_ALIGN_GUARD_BYTES);
        g_context->ved_buf_ptr = videoeng_malloc (VIDEO_ENG_VSTREAM_HEADER_MAX);

        /* Check for memory allocation failures. */
        if ((g_context->enc_pkt_ptr1 == NULL)
            || (g_context->enc_pkt_ptr2 == NULL)
            || (g_context->vop_recon_buf_ptr1 == NULL)
            || (g_context->vop_recon_buf_ptr2 == NULL)
            || (g_context->ved_buf_ptr == NULL))
        {
            /* Report error and exit. */
            CAMCORDER_MSG_ERROR ("init_processing: Unable to allocate "
                      "one or more encoder buffers!", 0, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }

        if (g_context->no_coding == TRUE)
        {
            g_context->video_target_bitrate = (uint32)args->user_bitrate;
        }
        else
        {
        /* Initialize video encoder. */
        CAMCORDER_MSG_HIGH ("init_processing: initializing encoder", 0, 0, 0);
        enc_init.frame_width = args->frame_width;
        enc_init.frame_height = args->frame_height;
        enc_init.time_incr = args->time_increment;
        enc_init.time_incr_res = args->time_resolution;
        if(!args->max_bitrate_enable){
          enc_init.target_bitrate = args->user_bitrate;
        } 
        enc_init.profile = args->profile;
        enc_init.vbrand = args->file_brand;
        enc_init.vfmt = args->video_format;
        enc_init.rot_angle = args->rotation_flag;
        enc_init.fixed_fps_mode = args->fixed_fps_mode;
        enc_init.vop_recon_buf_ptr1
            = (uint8*) (((uint32)g_context->vop_recon_buf_ptr1 + MEM_ALIGN_GUARD_BYTES)
                        & MEM_ALIGN_MASK);
        enc_init.vop_recon_buf_ptr2
            = (uint8*) (((uint32)g_context->vop_recon_buf_ptr2 + MEM_ALIGN_GUARD_BYTES)
                        & MEM_ALIGN_MASK);
        enc_init.enc_pkt_ptr1
            = (uint8*) (((uint32)g_context->enc_pkt_ptr1 + MEM_ALIGN_GUARD_BYTES) & MEM_ALIGN_MASK);
        enc_init.enc_pkt_ptr2
            = (uint8*) (((uint32)g_context->enc_pkt_ptr2 + MEM_ALIGN_GUARD_BYTES) & MEM_ALIGN_MASK);
        enc_init.enc_pkt_size = g_context->enc_pkt_size;
        enc_init.bufferHandle = args->bufferHandle;
        enc_init.bufferCount  = args->bufferCount;
        enc_init.dynamic_bitrate_enable = args->dynamic_bitrate_enable;
        enc_init.max_bitrate_enable = args->max_bitrate_enable;

        /* Push onto video stream the first fixed part of the bitstream header,
        ** but only if the video format is MPEG-4.
        */
        if (args->video_format == VIDEO_FMT_STREAM_VIDEO_MPEG4)
        {
            CAMCORDER_MSG_HIGH ("start_processing: adding M4V header", 0, 0, 0);
            memcpy(g_context->ved_buf_ptr, M4V_BITSTREAM_FIXED_HEADER,
                    sizeof (M4V_BITSTREAM_FIXED_HEADER));
            enc_init.ved_buf_ptr = g_context->ved_buf_ptr
                                    + sizeof (M4V_BITSTREAM_FIXED_HEADER);
            enc_init.ved_buf_size = VIDEO_ENG_VSTREAM_HEADER_MAX
                                    - sizeof (M4V_BITSTREAM_FIXED_HEADER);
        }
        else
        {
            enc_init.ved_buf_ptr = g_context->ved_buf_ptr;
            enc_init.ved_buf_size = VIDEO_ENG_VSTREAM_HEADER_MAX;
        }

#ifdef FEATURE_H264_ENCODER
        if (args->video_format ==  VIDEO_FMT_STREAM_VIDEO_H264)
        {
            enc_init.codec_init.h264.avc_sample = TRUE;
            enc_init.inter_memory_size = 2* (VIDEO_ENC_INTERNAL_MEMORY_SIZE(args->frame_width,args->frame_height)+
                                         VIDEO_ENC_INTRA_MEMORY_SIZE(enc_init.time_incr_res,enc_init.time_incr));
        //VIDEO_ENC_INTRA_MEMORY_SIZE(enc_init.time_incr_res,enc_init.time_incr);
            g_context->encoder_internal_memory =
                                videoeng_malloc(enc_init.inter_memory_size);
            enc_init.internal_memory =
                              (int16 *) g_context->encoder_internal_memory;

        }
        else
#endif
        {
            enc_init.codec_init.mp4.short_header = args->short_header;
            enc_init.inter_memory_size = VIDEO_ENC_INTERNAL_MEMORY_SIZE(args->frame_width,args->frame_height)+
                                         VIDEO_ENC_INTRA_MEMORY_SIZE(enc_init.time_incr_res,enc_init.time_incr);
            g_context->encoder_internal_memory =
                videoeng_malloc(enc_init.inter_memory_size);
            enc_init.internal_memory =
              (int16 *) g_context->encoder_internal_memory;
        }

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API

        /* initialize video_eng_frame */
        eng_frame_init_status = video_eng_frame_init();

        if(VIDENG_FRAME_SUCCESS != eng_frame_init_status)
        {
          video_eng_enc_normal_cb(VIDEO_FAILURE,NULL);
        }
        else
#endif
        encode_init_status = video_encode_init (&enc_init, NULL,
            (video_cb_func_ptr_type) video_eng_enc_normal_cb, NULL);

        if (encode_init_status != VIDEO_SUCCESS
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
               || eng_frame_init_status != VIDENG_FRAME_SUCCESS
#endif //FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
            )
        {
           CAMCORDER_MSG_ERROR ("video_processing: failed to init.", 0, 0, 0);
           video_eng_enc_failure (FALSE);
           return;
        }
#ifdef FEATURE_VIDEO_CLIP_INFO
        /* Storing encoder init info into clipinfo */
        clipinfo->user_bitrate  = args->user_bitrate;
        clipinfo->profile = (uint8)args->profile;
        clipinfo->rotation_flag = args->rotation_flag;
        clipinfo->fixed_fps_mode = args->fixed_fps_mode;
        clipinfo->enc_pkt_size = g_context->enc_pkt_size;
        clipinfo->bufferCount  = args->bufferCount;
        clipinfo->short_header = args->short_header;
        clipinfo->ved_buf_size = enc_init.ved_buf_size ;
        clipinfo->enc_init_inter_memory_size = enc_init.inter_memory_size;
#endif //FEATURE_VIDEO_CLIP_INFO
    }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif//FEATURE_APP_TEST_AUTOMATION
#ifdef FEATURE_VIDEO_ENCODE_DIAG
#error code not present
#endif /* FEATURE_VIDEO_ENCODE_DIAG */
}
        /* If movie has no video, move directly to the INIT state. */
    if ((g_context->video_format == VIDEO_FMT_STREAM_VIDEO_NONE) ||
        (g_context->no_coding == TRUE) )
    {
        /* Move engine to idle state. */
        g_context->state = VIDEO_ENG_STATE_IDLE;

        /* Send a success status code back to our client. */
        g_context->callback_ptr
            (VIDEO_SUCCESS, g_context->client_data);
    }

    /* Otherwise, move engine to init state, waiting for encoder to
    ** initialize.
    */
    else
    {
        g_context->state = VIDEO_ENG_STATE_INIT;
        g_context->video_done = FALSE;
        g_context->audio_done = TRUE;  /* no audio yet at this point */
        g_context->video_error = FALSE;
        g_context->audio_error = FALSE;
    }
}


/* returns TRUE if the image size, or the rotated image size,
** is less than the max size
*/
static boolean video_eng_max_img_size(uint16 max_x, uint16 max_y)
{
  if(g_context == NULL )
    return FALSE;

  if(g_context->frame_width <= max_x &&
     g_context->frame_height <= max_y
    )
    return TRUE;

  if(g_context->frame_height <= max_x &&
     g_context->frame_width <= max_y
     )
    return TRUE;

  return FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_start_processing

DESCRIPTION
  This function starts recording a video file, with parameters given during
  initialization time.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_start_processing (void)
{
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE    
    cmx_handle_type                      cmx_handle;
#ifdef FEATURE_CAMCORDER_AAC_REC
    cmx_mm_rec_param_type                aac_rec_param;
#endif /* FEATURE_CAMCORDER_AAC_REC */
    cmx_mm_rec_param_type                qcp_rec_param;
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

    video_enc_config_type                config_data;
    video_fmt_create_params_type         vf_file;
    video_fmt_stream_create_params_type  vf_streams [3];
    video_eng_udat_list_type             *udat;
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    const uint8                          *audio_header = NULL;
    uint32                               audio_header_size = 0;
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    uint32                               video_str = 0;
    uint32                               audio_str = 0;
    uint32                               text_str = 0;
    #ifdef FEATURE_EFS2
    VIDEO_ENG_FILE                       *fp = NULL;
    VIDEO_ENG_FILE                       *output_fp = NULL;
    struct                         fs_statvfs stat;
    int                             stat_rsp;

    int                                  errno     = 0;
    #endif


    #ifdef FEATURE_H264_ENCODER
    uint32    framesize_in_mb,mb_rate;
    boolean   level_1b = FALSE;
    uint8     enc_profile = H264_BASE_LINE_PROFILE;
    uint8     level_idc = H264_LEVEL21;
    uint8     profile_comp = H264_DEFAULT_PROFILE_COM;
    uint8     *param_buf;
    uint32    param_size;
    #endif

#ifdef FEATURE_VIDEO_CLIP_INFO
    video_fmt_clipinfo_type * clipinfo = &g_context->clipinfo;
    video_enc_raw_config_type  adv;
#endif //FEATURE_VIDEO_CLIP_INFO

#ifdef DEBUG_AVSYNC
    uint32 time_ms[2]={0,0};
#endif

    /* Verify state is appropriate for this command. */
    if (g_context->state != VIDEO_ENG_STATE_IDLE)
    {
        CAMCORDER_MSG_ERROR ("start_processing: Engine not idle!", 0, 0, 0);
        g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
        return;
    }

    /*-----------------------------------------------------------------------
    ** Learn the target bit rate and fix the profile and level indication.
    ** The video target bitrate may be used to determine other parameters.
    **-----------------------------------------------------------------------
    */

    if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
    {
        if (g_context->no_coding == FALSE)
        {
        /* Don't provide video_eng_enc_normal_cb() as a callback to
        ** video_encode_get_config() because it can disrupt the video
        ** encode engine state machine.
        */
        (void) video_encode_get_config (&config_data, NULL, NULL, NULL);

        g_context->video_target_bitrate = (uint32)config_data.target_bitrate;
        }
        if (g_context->video_format == VIDEO_FMT_STREAM_VIDEO_MPEG4)
        {
            if (g_context->video_target_bitrate <= SIMPLE_L1_MAX_BIT_RATE
                && video_eng_max_img_size(QCIF_DX,QCIF_DY)
               )
            {
              if( g_context->file_brand == VIDEO_FMT_BRAND_3GP )
              {
                /* if brand is 3gp in mp4, it is assumed that
                ** mp4_encode_init() and mp4_encode_set_config would
                ** have set the proper parameters for
                ** Simple Profile Level 0 encoding according to
                ** Annex N number 11.
                */
                g_context->ved_buf_ptr [ PROFILE_LEVEL_OFFSET] =
                  SIMPLE_PROFILE_LEVEL0;

              }
              else
              {
                g_context->ved_buf_ptr [PROFILE_LEVEL_OFFSET] =
                            SIMPLE_PROFILE_LEVEL1;
              }
            }
            else if (g_context->video_target_bitrate <= SIMPLE_L2_MAX_BIT_RATE
                     && video_eng_max_img_size(CIF_DX,CIF_DY)
                    )
            {
                g_context->ved_buf_ptr [PROFILE_LEVEL_OFFSET] =
                            SIMPLE_PROFILE_LEVEL2;
            }
            else if (g_context->video_target_bitrate <= SIMPLE_L3_MAX_BIT_RATE
                     && video_eng_max_img_size(CIF_DX,CIF_DY)
                    )
            {
                g_context->ved_buf_ptr [PROFILE_LEVEL_OFFSET] =
                            SIMPLE_PROFILE_LEVEL3;
            }
            else if ( g_context->video_target_bitrate <= SIMPLE_L4a_MAX_BIT_RATE
                      && video_eng_max_img_size(VGA_DX,VGA_DY) )
            {
              g_context->ved_buf_ptr [PROFILE_LEVEL_OFFSET] =
                            SIMPLE_PROFILE_LEVEL4a;
            }
            else if ( g_context->video_target_bitrate <= SIMPLE_L5_MAX_BIT_RATE
                      && video_eng_max_img_size(PAL_DX,PAL_DY) )
            {
              g_context->ved_buf_ptr [PROFILE_LEVEL_OFFSET] =
                            SIMPLE_PROFILE_LEVEL5;
            }
            else
            {
              CAMCORDER_MSG_ERROR ("start_processing: No matching Simple Profile"
                         "level", 0, 0, 0);
              CAMCORDER_MSG_ERROR ("Defaulting to Main Level 4", 0, 0, 0);
              g_context->ved_buf_ptr[PROFILE_LEVEL_OFFSET] =
                            PROFILE_MAIN_L4;

            }
        }
        #ifdef FEATURE_H264_ENCODER
        else if(g_context->video_format ==  VIDEO_FMT_STREAM_VIDEO_H264)
        {
            /* we have always H.264 baseline profile */
            enc_profile = H264_BASE_LINE_PROFILE;
            framesize_in_mb =  g_context->frame_height * g_context->frame_width
                                  / H264_PIXELS_PER_MB;
            /* Instead of doing floating point calculation we will determine
            ** macro block rate as follows and compare the result with
            ** multiplication of macroblock processing rate and
            ** video_time_increment
            */
            mb_rate = framesize_in_mb * g_context->video_time_resolution;

            /* Determine level according to H.264 Spec Annex A Table A-1 */
            if(framesize_in_mb <= H264_MB_PER_FRAME_L1
                 && mb_rate <= H264_MB_PER_SEC_L1 * g_context->video_time_increment
                 && g_context->video_target_bitrate <= H264_L1_MAX_BIT_RATE )
            {
              level_idc = H264_LEVEL1;
            }
            else if(framesize_in_mb <= H264_MB_PER_FRAME_L1b
                     && mb_rate<= H264_MB_PER_SEC_L1b * g_context->video_time_increment
                     && g_context->video_target_bitrate <= H264_L1b_MAX_BIT_RATE)
            {
              level_idc = H264_LEVEL1b;
              level_1b = TRUE;
            }
            else if(framesize_in_mb <= H264_MB_PER_FRAME_L11
                     && mb_rate <= H264_MB_PER_SEC_L11 * g_context->video_time_increment
                     && g_context->video_target_bitrate <= H264_L11_MAX_BIT_RATE)
            {
              level_idc = H264_LEVEL11;
            }
            else if(framesize_in_mb <= H264_MB_PER_FRAME_L12
                     && mb_rate <= H264_MB_PER_SEC_L12 * g_context->video_time_increment
                     && g_context->video_target_bitrate <= H264_L12_MAX_BIT_RATE)
            {
              level_idc = H264_LEVEL12;
            }
            else if(framesize_in_mb <= H264_MB_PER_FRAME_L13
                     && mb_rate <= H264_MB_PER_SEC_L13 * g_context->video_time_increment
                     && g_context->video_target_bitrate <= H264_L13_MAX_BIT_RATE)
            {
              level_idc = H264_LEVEL13;
            }
            else if(framesize_in_mb <= H264_MB_PER_FRAME_L2
                     && mb_rate <= H264_MB_PER_SEC_L2 * g_context->video_time_increment
                     && g_context->video_target_bitrate <= H264_L2_MAX_BIT_RATE)
            {
               level_idc = H264_LEVEL2;
            }
            else if(framesize_in_mb <= H264_MB_PER_FRAME_L21
                     && mb_rate <= H264_MB_PER_SEC_L21 * g_context->video_time_increment
                     && g_context->video_target_bitrate <= H264_L21_MAX_BIT_RATE)
            {
              level_idc = H264_LEVEL21;
            }
            else if(framesize_in_mb <= H264_MB_PER_FRAME_L22
                     && mb_rate <= H264_MB_PER_SEC_L22 * g_context->video_time_increment
                     && g_context->video_target_bitrate <= H264_L22_MAX_BIT_RATE)
            {
              level_idc = H264_LEVEL22;
            }
            else
            {
              CAMCORDER_MSG_ERROR ("start_processing: No matching Baseline Profile"
                         "level", 0, 0, 0);
              CAMCORDER_MSG_ERROR ("Defaulting to Level 2.1", 0, 0, 0);
              level_idc = H264_LEVEL21;

            }

            /* In Profile compatibility flag, we have constraint_set [0,1,2,3] bits
            ** and rest four bits reserved
            ** constraint_set0_flag always set to 1 ( according to H.264 spec
            ** section 7.4.2.1 :: bitstream obeys all the constraints specified
            ** in subclause A.2.1 (Baseline profile)
            */
            if(level_1b)
            {
              /* constraint_set3_flag set to 1 to indicate level 1b
              ** (H.264 spec subclause A.3.1 and section 7.4.2.1 )
              */
              profile_comp = H264_L1b_PROFILE_COM;
            }
            else
            {
              profile_comp = H264_DEFAULT_PROFILE_COM;
            }

            /* Here we need to update the profile, profile compatibility and
            ** level of Sequence Parameter Set because sps was written at the
            ** time of video_encode_init and after that bitrate(and so level )
            ** can be changed due to call of video_encode_set_config()
            ** In fact, we are determining the level only here and
            ** and updating in sps parameter accordingly
            */
            if ( video_encode_get_decoder_specific_info((uint32**)&param_buf,
                                                         &param_size,
                                                         NULL,NULL) == VIDEO_SUCCESS)
            {
                /* writing the new value of profile/level into sps param
                ** first four bytes will be consisting of number of sps set,
                ** sps set length and nal unit type etc.
                */
                param_buf[4] = enc_profile;
                param_buf[5] = profile_comp;
                param_buf[6] = level_idc;
            }
        }
        #endif
        /* Initializing frame drop tracking variables in video_eng_start_processing*/
        g_context->frame_drop_info.prev_encode_frame_time = 0;
        g_context->frame_drop_info.frame_drop_duration = 0;
        g_context->frame_drop_info.frame_drop_duration_before_pause = 0;
        g_context->frame_drop_info.first_frame_encode_pending = FALSE;
        g_context->frame_drop_info.first_souce_frame_dropped = FALSE;
        /*Compute first frame default delta in ms*/
        g_context->frame_drop_info.first_frame_default_delta =
                          (g_context->video_time_increment * 1000 + 
                            (g_context->video_time_resolution >> 1))/
                              g_context->video_time_resolution;
    }
#ifndef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
    if(g_context->framecb_ptr == NULL)
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
    {
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_CAMCORDER_AAC_REC
    if (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
    {
        g_context->audio_target_bitrate = g_context->aac_channel_mode
            * aac_sampling_frequencies [g_context->aac_sampling_rate_index]
            * g_context->aac_bits_per_sample
            / AAC_BITS_PER_SAMPLE_DIVISOR;
        g_context->audio_channels = g_context->aac_channel_mode;
        g_context->audio_sampling_frequency
            = aac_sampling_frequencies [g_context->aac_sampling_rate_index];
        g_context->audio_samples_per_frame = AAC_SAMPLES_PER_FRAME;
    }
    else
#endif /* FEATURE_CAMCORDER_AAC_REC */
    {
        g_context->audio_target_bitrate
            = VIDEO_ENG_GET_VOCODER_BITRATE (g_context);
        g_context->audio_channels = 1;
        g_context->audio_sampling_frequency = VOCODER_SAMPLING_RATE;
        g_context->audio_samples_per_frame = VOCODER_SAMPLES_PER_PACKET;
    }

#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
#ifdef FEATURE_VIDEO_ENG_DEBUG
#ifndef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_VIDEO_AUDIO_INTERLACED =              %d",
                VIDEO_ENG_VIDEO_AUDIO_INTERLACED (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_VIDEO_BITRATE =                   %d",
                VIDEO_ENG_GET_VIDEO_BITRATE (g_context), 0,0);
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_AUDIO_BITRATE =                   %d",
                VIDEO_ENG_GET_AUDIO_BITRATE (g_context), 0,0);
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_STREAM_BITRATE =                  %d",
                VIDEO_ENG_GET_STREAM_BITRATE (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_VIDEO_SAMPLES_TABLE_SIZE =        %d",
                VIDEO_ENG_GET_VIDEO_SAMPLES_TABLE_SIZE (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_VIDEO_CHUNKS_TABLE_SIZE =         %d",
                VIDEO_ENG_GET_VIDEO_CHUNKS_TABLE_SIZE, 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_STREAM =     %d",
                VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_STREAM (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_FRAGMENT =   %d",
                VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_FRAGMENT (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_STORES =        %d",
                VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_STORES (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_STREAM =      %d",
                VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_STREAM (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_FRAGMENT =    %d",
                VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_FRAGMENT (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_STORES =         %d",
                VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_STORES (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_VIDEO_CHUNK_SIZE =                %d",
                VIDEO_ENG_GET_VIDEO_CHUNK_SIZE (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE =         %d",
                VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE (g_context), 0,0);
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_AUDIO_SAMPLES_TABLE_SIZE =        %d",
                VIDEO_ENG_GET_AUDIO_SAMPLES_TABLE_SIZE (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_STREAM =     %d",
                VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_STREAM
                (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_FRAGMENT =   %d",
                VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_FRAGMENT
                (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_STORES =        %d",
                VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_STORES (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_AUDIO_CHUNKS_TABLE_SIZE =         %d",
                VIDEO_ENG_GET_AUDIO_CHUNKS_TABLE_SIZE, 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_AUDIO_CHUNK_SIZE =                %d",
                VIDEO_ENG_GET_AUDIO_CHUNK_SIZE (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_ACTUAL_AUDIO_CHUNK_SIZE =         %d",
                VIDEO_ENG_GET_ACTUAL_AUDIO_CHUNK_SIZE (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_GET_MAX_AUDIO_CHUNKS_STORES =         %d",
                VIDEO_ENG_GET_MAX_AUDIO_CHUNKS_STORES (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("---", 0,0,0);
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
#endif /* FEATURE_VIDEO_ENG_FIXED_PARAMETERS */

    CAMCORDER_MSG_HIGH ("VIDEO_ENG_OPTIMAL_CHUNK_SIZE =                  %d",
                VIDEO_ENG_OPTIMAL_CHUNK_SIZE (g_context), 0,0);
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_SIZE =            %d",
                VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_SIZE (g_context), 0,0);
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_VIDEO_STREAM_BUF_SIZE =               %d",
                VIDEO_ENG_VIDEO_STREAM_BUF_SIZE (g_context), 0,0);
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_AUDIO_STREAM_BUF_SIZE =               %d",
                VIDEO_ENG_AUDIO_STREAM_BUF_SIZE (g_context), 0,0);
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_FRAGMENT_BUF_SIZE =                   %d",
                VIDEO_ENG_FRAGMENT_BUF_SIZE (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_ASYNC_WRITER_FRAGMENT_BUF_SIZE =      %d",
                VIDEO_ENG_ASYNC_WRITER_FRAGMENT_BUF_SIZE (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_MAX_VIDEO_FRAMES =                    %d",
                VIDEO_ENG_MAX_VIDEO_FRAMES (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_MAX_CHUNKS =                          %d",
                VIDEO_ENG_MAX_CHUNKS (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_MAX_VIDEO_SAMPLES_PER_SECOND =        %d",
                VIDEO_ENG_MAX_VIDEO_SAMPLES_PER_SECOND (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_MAX_VIDEO_TABLE_STORES =              %d",
                VIDEO_ENG_MAX_VIDEO_TABLE_STORES (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_MAX_AUDIO_FRAMES =                    %d",
                VIDEO_ENG_MAX_AUDIO_FRAMES (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_MAX_AUDIO_SAMPLES_PER_SECOND =        %d",
                VIDEO_ENG_MAX_AUDIO_SAMPLES_PER_SECOND (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_MAX_AUDIO_TABLE_STORES =              %d",
                VIDEO_ENG_MAX_AUDIO_TABLE_STORES (g_context), 0,0);
    CAMCORDER_MSG_HIGH ("VIDEO_ENG_ASYNC_WRITER_BUF_SIZE =               %d",
                VIDEO_ENG_ASYNC_WRITER_BUF_SIZE(g_context), 0,0);
#endif /* FEATURE_VIDEO_ENG_DEBUG */


#ifdef FEATURE_EFS2
    /* Determine the target destination based on the filename */
    video_eng_find_save_location(g_context->output.efs.filename);

    /* Temporary files have to be stored on the same location as the target file*/
    if(g_context->output.method == VIDEO_METHOD_EFS)
    {
       /* Temporary files have to be stored on the same location as the target file
       when output method is VIDEO_METHOD_EFS*/
       uint32 offset = 0;
       char *endpos; 
       if ((endpos = strrchr(g_context->output.efs.filename, FS_DIRECTORY_CHAR_P)) !=0)
       {
          offset = endpos - g_context->output.efs.filename;
       }
       /*Append "/temp1.mp4" etc. starting from the position of last occurance of'/'
        in the filename*/
       std_strlcpy(g_context->temp_file_name_1, g_context->output.efs.filename,offset+1);
       std_strlcat(g_context->temp_file_name_1, VIDEO_ENG_TEMP_FILE_1,
                                                             FS_FILENAME_MAX_LENGTH_P);
       std_strlcpy(g_context->temp_file_name_2, g_context->output.efs.filename,offset+1);
       std_strlcat(g_context->temp_file_name_2, VIDEO_ENG_TEMP_FILE_2,
                                                             FS_FILENAME_MAX_LENGTH_P);
       std_strlcpy(g_context->temp_file_name_3, g_context->output.efs.filename,offset+1);
       std_strlcat(g_context->temp_file_name_3, VIDEO_ENG_TEMP_FILE_3,
                                                             FS_FILENAME_MAX_LENGTH_P);

    }
    else
    {
       /*Use EFS for temp files for OUTPUT_METHOD_MEM and CLIENT*/
       std_strlcpy(g_context->temp_file_name_1, VIDEO_ENG_TEMP_FILE_1,
                                                      FS_FILENAME_MAX_LENGTH_P);
       std_strlcpy(g_context->temp_file_name_2, VIDEO_ENG_TEMP_FILE_2,
                                                      FS_FILENAME_MAX_LENGTH_P);
       std_strlcpy(g_context->temp_file_name_3, VIDEO_ENG_TEMP_FILE_3,
                                                      FS_FILENAME_MAX_LENGTH_P);

    }
    /* If file brand is AMC or KDDI fragmented 3G2, set up to write to
    ** temporary file.
    */
    if ((g_context->file_brand == VIDEO_FMT_BRAND_AMC)
        || (g_context->file_brand == VIDEO_FMT_BRAND_FRAG_3G2)
        || (g_context->reorder_atom == TRUE))
    {
        (void) efs_unlink (g_context->temp_file_name_1);

        /* Opening the temporary file for AMC & 3G2 Frag in temporary
           memory.*/
        if (VIDEO_ENG_TEMPORARY_FRAGMENT_ON_MEMORY
            && (g_context->file_brand == VIDEO_FMT_BRAND_FRAG_3G2  || g_context->file_brand == VIDEO_FMT_BRAND_AMC)) /*lint !e774 !e506
                                         left side of && is a const value boolean */
        {
            fp = video_eng_mem_file_fopen (NULL,
                    VIDEO_ENG_FRAGMENT_BUF_SIZE (g_context));
        }
        /*else
        {
            fp = video_eng_efs_file_fopen (g_context->temp_file_name_1, "w+");

            ++g_context->temp_file_size_factor;
        }*/
        if (fp == NULL)
        {
            errno = efs_errno; 
            CAMCORDER_MSG_ERROR ("start_processing: error '%d' from EFS2 "
                       "when opening temp video file",
                       errno, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }
        g_context->temp_handle_1 = fp;
        g_context->use_temp_file_1 = TRUE;
    }

    /* If file brand is KDDI fragmented 3G2 or AMC, and destination is not EFS, set
    ** up to write to a second temporary file as well.
    */
    if ((g_context->file_brand == VIDEO_FMT_BRAND_FRAG_3G2 
        || g_context->file_brand == VIDEO_FMT_BRAND_AMC)
        && (g_context->output.method != VIDEO_METHOD_EFS))
    {
        (void) efs_unlink (g_context->temp_file_name_2);
        fp = video_eng_efs_file_fopen (g_context->temp_file_name_2, "w+");
        if (fp == NULL)
        {
            errno = efs_errno; 
            CAMCORDER_MSG_ERROR ("start_processing: error '%d' from EFS2 "
                       "when opening temp video file",
                       errno, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }
        output_fp = fp;
        g_context->temp_handle_2 = fp;
        g_context->use_temp_file_2 = TRUE;
        ++g_context->temp_file_size_factor;
    }

    /* A third temporary file to store meta data.
    */
    {
        (void) efs_unlink (g_context->temp_file_name_3);
        fp = video_eng_efs_file_fopen (g_context->temp_file_name_3, "wb+");
        if (fp == NULL)
        {
            errno = efs_errno; 
            CAMCORDER_MSG_ERROR ("start_processing: error '%d' from EFS2 "
                       "when opening temp video file",
                       errno, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }
        g_context->temp_handle_3 = fp;
        g_context->use_temp_file_3 = TRUE;
        ++g_context->temp_file_size_factor_3;
    }

    /* If the output destination is a file in EFS, open the file now. */
    if (g_context->output.method == VIDEO_METHOD_EFS)
    {
        (void) efs_unlink (g_context->output.efs.filename);
        CAMCORDER_MSG_HIGH ("start_processing: Opening file", 0, 0, 0);
        fp = video_eng_efs_file_fopen (g_context->output.efs.filename, "w+");
        if (fp == NULL)
        {
            errno = efs_errno;  
            CAMCORDER_MSG_ERROR ("start_processing: error '%d' from EFS2 "
                       "when opening output video file",
                       errno, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }
        output_fp = fp;
        g_context->fs_handle = fp;

        // Chcking based on the file location.
        stat_rsp = efs_statvfs(g_context->output.efs.filename,&stat);
        if (stat_rsp == 0)
        {
           g_context->free_space 
             = (uint64)stat.f_bsize * (uint64)stat.f_bfree;
        }
    }
    if(output_fp != NULL)
    {
        /* mark the output file for asynchronous writing, and
        ** increase its buffer size. Note that the buffer size must be
        ** a power of two.
        */
        fp = output_fp;
        if (g_context->file_brand == VIDEO_FMT_BRAND_FRAG_3G2)
        {
            if (efs_setvbuf (fp->object, NULL, EFS_IOFBF,
                    VIDEO_ENG_ASYNC_WRITER_FRAGMENT_BUF_SIZE (g_context))
                != 0)
            {
                CAMCORDER_MSG_ERROR ("Setvbuf failed", 0, 0, 0);
            }
        }
        else
        {
            if (efs_setvbuf (fp->object, NULL, EFS_IOFBF,
                    VIDEO_ENG_ASYNC_WRITER_BUF_SIZE(g_context)) != 0)/*lint !e834*/
            {
                CAMCORDER_MSG_ERROR ("Setvbuf failed", 0, 0, 0);
            }
        }
        if (efs_set_async_params (fp->object, rex_self (),
                VS_VIDEO_ASYNC_WRITE_SIG,
                &video_async_tcb, VIDEO_ASYNC_SIG) != 0)
            CAMCORDER_MSG_ERROR ("Set async params failed", 0, 0, 0);
    }

#ifdef FEATURE_VIDEO_CLIP_INFO
    clipinfo->save_location = (uint8)g_context->save_location;
    clipinfo->free_space = g_context->free_space;
#endif //FEATURE_VIDEO_CLIP_INFO

    CAMCORDER_MSG_HIGH(" Save location = %d, Free Space = %x%x ",
       g_context->save_location, g_context->free_space>>32, g_context->free_space);

    CAMCORDER_MSG_HIGH("temp_file_size_factor = %d,"
                       "temp_file_size_factor_3 = %d ",
                       g_context->temp_file_size_factor,
                       g_context->temp_file_size_factor_3, 0);
#endif /* FEATURE_EFS2 */

#ifdef FEATURE_VIDEO_CLIP_INFO
    if(g_context->output.method == VIDEO_METHOD_MEM)
         clipinfo->bytes_available = g_context->output.mem.len;
#endif //FEATURE_VIDEO_CLIP_INFO

    /* Mark movie file status as open. */
    g_context->output_open = TRUE;

    /*-----------------------------------------------------------------------
    ** Create Video FMT writer.
    **-----------------------------------------------------------------------
    */

    /* Zero-initialize file and stream creation parameter structures. */
    (void) memset (&vf_file, 0, sizeof (vf_file));
    (void) memset (&vf_streams [0], 0, sizeof (vf_streams));

    /* Determine number of streams, and which stream to use for video, audio,
    ** and text.
    */
    vf_file.num_streams = 0;
    if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
    {
        video_str = vf_file.num_streams;
        ++vf_file.num_streams;
    }
    if (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
    {
        audio_str = vf_file.num_streams;
        ++vf_file.num_streams;
    }
    /*
     ** If both audio and video streams are present enable truncation
     ** In case if there is only one stream option is disabled.
    */
    if (vf_file.num_streams == 2 && !g_context->still_image)
    {
         vf_file.enable_fixupdata = TRUE;
    }

    if (g_context->text_type != VIDEO_ENG_TEXT_INPUT_NONE)
    {
        text_str = vf_file.num_streams;
        ++vf_file.num_streams;
    }


   if(vf_file.num_streams == 1)
   {
     g_context->no_interlace = TRUE;
   }
    /* Construct file level creation parameters structure for the video format
    ** services.
    */
    vf_file.streams = &vf_streams [0];
    vf_file.movie_size_warning_near_threshold
        = g_context->space_limit_threshold;
    vf_file.movie_size_warning_imminent_threshold
        = g_context->space_imminent_threshold;
    vf_file.file_size_limit = g_context->movie_size_limit;
    vf_file.include_drm = g_context->include_drm;
    vf_file.drm_distribution_rights = g_context->drm_distribution_rights;

    /* Check & update user data atom parameters */
    vf_file.include_user_data = g_context->include_user_atom;
    if (g_context->useratom_data.atom_is_valid)
    {
        vf_file.user_data.atom_is_valid = TRUE;

        /* Check & update title, author, desc, cprt data*/
        if (g_context->useratom_data.titl_auth_dscp_cprt_valid)
    {
            vf_file.user_data.titl_auth_desc_valid = g_context->useratom_data.titl_auth_dscp_cprt_valid;
            vf_file.user_data.title         = g_context->useratom_data.title;
            vf_file.user_data.author        = g_context->useratom_data.author;
            vf_file.user_data.description   = g_context->useratom_data.description;
            vf_file.user_data.copyright     = g_context->useratom_data.copyright;
        }

        /* Check & update location information atom */
        if (g_context->useratom_data.loci.loci_valid)
    {
            vf_file.user_data.loci = g_context->useratom_data.loci ;
        }
    }

    /* Set up video stream creation parameters. */
    if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
    {
        vf_streams [video_str].type = VIDEO_FMT_STREAM_VIDEO;
        vf_streams [video_str].subinfo.video.format = g_context->video_format;
        vf_streams [video_str].subinfo.video.width = g_context->frame_width;
        vf_streams [video_str].subinfo.video.height = g_context->frame_height;

        #ifdef FEATURE_H264_ENCODER
        if(g_context->video_format ==  VIDEO_FMT_STREAM_VIDEO_H264)
        {
          /* Passing the profile/level information to videofmt */
          vf_streams [video_str].subinfo.video.profile = enc_profile;
          vf_streams [video_str].subinfo.video.level = level_idc;
          vf_streams [video_str].subinfo.video.profile_comp = profile_comp;
        }
        #endif

        vf_streams [video_str].width = (((uint32) g_context->frame_width) << 16);
        vf_streams [video_str].height = (((uint32) g_context->frame_height) << 16);
        vf_streams [video_str].handler = "vide";
        vf_streams [video_str].media_timescale = g_context->video_time_resolution;
        vf_streams [video_str].interlace = 0;
        vf_streams [video_str].chunk_size = VIDEO_ENG_OPTIMAL_CHUNK_SIZE (g_context);
        vf_streams [video_str].buffer_size
            = VIDEO_ENG_VIDEO_STREAM_BUF_SIZE (g_context);
        vf_streams [video_str].max_header = VIDEO_ENG_VSTREAM_HEADER_MAX;
        vf_streams [video_str].max_footer = sizeof (M4V_BITSTREAM_FIXED_FOOTER);
        //vf_streams [0].sample_delta = g_context->video_time_increment;
        vf_streams [video_str].inter_frames = TRUE;
        vf_streams [video_str].max_samples = VIDEO_ENG_MAX_VIDEO_FRAMES (g_context);
        vf_streams [video_str].max_chunks =
          VIDEO_ENG_MAX_CHUNKS (g_context); /*lint !e414*/
          /* divide by zero will occur only if video_format == NONE*/
        vf_streams [video_str].max_samples_rate =
            VIDEO_ENG_MAX_VIDEO_SAMPLES_PER_SECOND (g_context);
        vf_streams [video_str].max_table_stores
            = VIDEO_ENG_MAX_VIDEO_TABLE_STORES (g_context); /*lint !e414*/
          /* divide by zero will occur only if video_format == NONE */
        if (!g_context->no_interlace)
        {
            vf_streams [video_str].chunks_out_near_threshold
                = g_context->space_limit_threshold * 1000
                / VIDEO_ENG_DESIRED_INTERLACE_RATE;
            vf_streams [video_str].chunks_out_imminent_threshold
                = g_context->space_imminent_threshold * 1000
                / VIDEO_ENG_DESIRED_INTERLACE_RATE;
        }
        else
        {
            vf_streams [video_str].chunks_out_near_threshold
                = (g_context->space_limit_threshold
                   + VIDEO_ENG_OPTIMAL_CHUNK_LENGTH - 1) /
                VIDEO_ENG_OPTIMAL_CHUNK_LENGTH;
            vf_streams [video_str].chunks_out_imminent_threshold
                = (g_context->space_imminent_threshold
                   + VIDEO_ENG_OPTIMAL_CHUNK_LENGTH - 1) /
                VIDEO_ENG_OPTIMAL_CHUNK_LENGTH;
        }
        vf_streams [video_str].samples_out_near_threshold
            = g_context->space_limit_threshold
            * g_context->video_time_resolution
            / g_context->video_time_increment;
        vf_streams [video_str].samples_out_imminent_threshold
            = g_context->space_imminent_threshold
            * g_context->video_time_resolution
            / g_context->video_time_increment;
        vf_streams [video_str].stsc_reset_rate = VIDEO_ENG_STSC_ALGO_RESET_RATE;
        if (g_context->still_image && audio_str)
        {
            vf_streams [video_str].match_duration = TRUE;
            vf_streams [video_str].match_duration_with = audio_str;
        }
        /* Remember which stream is for video */
        g_context->video_stream_num = video_str;

        /* For H.263, set profile and level indications. */
        if (g_context->video_format == VIDEO_FMT_STREAM_VIDEO_H263)
        {
          vf_streams [video_str].subinfo.video.profile = 0;
          vf_streams [video_str].subinfo.video.level = 45;
        }

    }

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    /* Set up audio stream creation parameters. */
    if (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
    {
        if (!g_context->no_interlace
            && (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE))
        {
            /* interlace video with audio, with video as master */
            vf_streams [video_str].interlace = audio_str;
        }
        vf_streams [audio_str].interlace = audio_str;  /* no interlace */
        vf_streams [audio_str].type = VIDEO_FMT_STREAM_AUDIO;
        vf_streams [audio_str].subinfo.audio.format = g_context->audio_format;
        vf_streams [audio_str].handler = "soun";
        vf_streams [audio_str].media_timescale = VOCODER_SAMPLING_RATE;
        vf_streams [audio_str].buffer_size
            = VIDEO_ENG_AUDIO_STREAM_BUF_SIZE (g_context);
        vf_streams [audio_str].max_header = QCP_FILE_HEADER_SIZE;
        vf_streams [audio_str].sample_delta
            = VOCODER_SAMPLING_RATE / VOCODER_PACKET_RATE;
        vf_streams [audio_str].max_samples
            = VIDEO_ENG_MAX_AUDIO_FRAMES (g_context);
        vf_streams [audio_str].chunk_size
            = VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_SIZE (g_context);
        vf_streams [audio_str].max_chunks = VIDEO_ENG_MAX_CHUNKS (g_context);/*lint !e414*/
          /* Supress divide by zero lint error, it will occur only if audio_format == NONE*/
        vf_streams [audio_str].max_samples_rate = 0;
        vf_streams [audio_str].max_table_stores
            = VIDEO_ENG_MAX_AUDIO_TABLE_STORES  (g_context);/*lint !e414*/
          /* Supress divide by zero lint error,it will occur only if audio_format == NONE*/
        if (!g_context->no_interlace)
        {
            vf_streams [audio_str].chunks_out_near_threshold
                = g_context->space_limit_threshold * 1000
                / VIDEO_ENG_DESIRED_INTERLACE_RATE;
            vf_streams [audio_str].chunks_out_imminent_threshold
                = g_context->space_imminent_threshold * 1000
                / VIDEO_ENG_DESIRED_INTERLACE_RATE;
        }
        else
        {
            vf_streams [audio_str].chunks_out_near_threshold
                = (g_context->space_limit_threshold
                   + VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_LENGTH - 1) /
                VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_LENGTH;
            vf_streams [audio_str].chunks_out_imminent_threshold
                = (g_context->space_imminent_threshold
                   + VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_LENGTH - 1) /
                VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_LENGTH;
        }
        vf_streams [audio_str].samples_out_near_threshold
            = g_context->space_limit_threshold * 50;
        vf_streams [audio_str].samples_out_imminent_threshold
            = g_context->space_imminent_threshold * 50;
        vf_streams [audio_str].stsc_reset_rate
            = VIDEO_ENG_STSC_ALGO_RESET_RATE;
        vf_streams [audio_str].frames_per_sample = 1;

        switch (g_context->audio_format)
        {
        case VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL:
            if (g_context->file_brand == VIDEO_FMT_BRAND_K3G)
            {
                vf_streams [audio_str].subinfo.audio.format
                    = VIDEO_FMT_STREAM_AUDIO_PUREVOICE;
            }
            audio_header = QCP_FILE_HEADER_13K_FIXED_FULL;
            audio_header_size = QCP_FILE_HEADER_SIZE;
            switch (g_context->audio_rate_set)
            {
            case VIDEO_FMT_RATE_SET_FIXED:
                vf_streams [audio_str].sample_size
                    = VOCODER_PACKET_SIZE_13K_FULL;
                break;
            case VIDEO_FMT_RATE_SET_VAR:
                vf_streams [audio_str].sample_size = 0;
                break;
            default:
                CAMCORDER_MSG_ERROR ("start_processing: given audio rate set "
                          "not supported!", 0, 0, 0);
                break;
            }
            break;

        case VIDEO_FMT_STREAM_AUDIO_QCELP13K_HALF:
            if (g_context->file_brand == VIDEO_FMT_BRAND_K3G)
            {
                vf_streams [audio_str].subinfo.audio.format
                    = VIDEO_FMT_STREAM_AUDIO_PUREVOICE;
            }
            audio_header = QCP_FILE_HEADER_13K_FIXED_HALF;
            audio_header_size = QCP_FILE_HEADER_SIZE;
            switch (g_context->audio_rate_set)
            {
            case VIDEO_FMT_RATE_SET_FIXED:
                vf_streams [audio_str].sample_size
                    = VOCODER_PACKET_SIZE_13K_HALF;
                break;
            case VIDEO_FMT_RATE_SET_VAR:
                vf_streams [audio_str].sample_size = 0;
                break;
            default:
                CAMCORDER_MSG_ERROR ("start_processing: given audio rate set "
                          "not supported!", 0, 0, 0);
                break;
            }
            break;

        case VIDEO_FMT_STREAM_AUDIO_EVRC:
            switch (g_context->audio_rate_set)
            {
            case VIDEO_FMT_RATE_SET_FIXED:
                vf_streams [audio_str].sample_size = VOCODER_PACKET_SIZE_EVRC;
                break;
            case VIDEO_FMT_RATE_SET_VAR:
                vf_streams [audio_str].sample_size = 0;
                break;
            default:
                CAMCORDER_MSG_ERROR ("start_processing: given audio rate set "
                          "not supported!", 0, 0, 0);
                break;
            }
            break;

        case VIDEO_FMT_STREAM_AUDIO_AMR:
            switch (g_context->audio_rate_set)
            {
              case VIDEO_FMT_RATE_SET_AMR_1220:
                  vf_streams [audio_str].sample_size
                      = VOCODER_PACKET_SIZE_AMR_1220;
                  break;
              case VIDEO_FMT_RATE_SET_AMR_1020:
                  vf_streams [audio_str].sample_size
                      = VOCODER_PACKET_SIZE_AMR_1020;
                  break;
              case VIDEO_FMT_RATE_SET_AMR_0795:
                  vf_streams [audio_str].sample_size
                      = VOCODER_PACKET_SIZE_AMR_0795;
                  break;
              case VIDEO_FMT_RATE_SET_AMR_0740:
                  vf_streams [audio_str].sample_size
                      = VOCODER_PACKET_SIZE_AMR_0740;
                  break;
              case VIDEO_FMT_RATE_SET_AMR_0670:
                  vf_streams [audio_str].sample_size
                      = VOCODER_PACKET_SIZE_AMR_0670;
                  break;
              case VIDEO_FMT_RATE_SET_AMR_0590:
                  vf_streams [audio_str].sample_size
                      = VOCODER_PACKET_SIZE_AMR_0590;
                  break;
              case VIDEO_FMT_RATE_SET_AMR_0515:
                  vf_streams [audio_str].sample_size
                      = VOCODER_PACKET_SIZE_AMR_0515;
                  break;
              case VIDEO_FMT_RATE_SET_AMR_0475:
                  vf_streams [audio_str].sample_size
                      = VOCODER_PACKET_SIZE_AMR_0475;
                  break;
              default:
                  vf_streams [audio_str].sample_size
                      = VOCODER_PACKET_SIZE_AMR_1220;
                  break;
            }
            break;

#ifdef FEATURE_CAMCORDER_AAC_REC
        case VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC:
            vf_streams [audio_str].media_timescale =
                aac_sampling_frequencies [g_context->aac_sampling_rate_index];
            vf_streams [audio_str].max_header = 2;
            vf_streams [audio_str].sample_delta = AAC_SAMPLES_PER_FRAME;
            vf_streams [audio_str].max_samples
                = VIDEO_ENG_MAX_AUDIO_FRAMES (g_context);
            vf_streams [audio_str].max_samples_rate
                = VIDEO_ENG_MAX_AUDIO_SAMPLES_PER_SECOND (g_context);
            vf_streams [audio_str].max_table_stores
                = VIDEO_ENG_MAX_AUDIO_TABLE_STORES (g_context); /*lint !e414*/
          /* divide by zero will occur only if audio_format == NONE*/
            vf_streams [audio_str].samples_out_near_threshold
                = g_context->space_limit_threshold * g_context->audio_channels
                * g_context->audio_sampling_frequency / AAC_SAMPLES_PER_FRAME;
            vf_streams [audio_str].samples_out_imminent_threshold
                = g_context->space_imminent_threshold
                * g_context->audio_channels * g_context->audio_sampling_frequency
                / AAC_SAMPLES_PER_FRAME;
             vf_streams [audio_str].sample_size = 0;
            break;
#endif /* FEATURE_CAMCORDER_AAC_REC */

        default:
            CAMCORDER_MSG_ERROR ("start_processing: wrong audio format!", 0, 0, 0);
            break;
        }

        g_context->audio_sample_size = vf_streams [audio_str].sample_size;
        g_context->audio_sample_delta
            = vf_streams [audio_str].sample_delta;

        /* Remember which stream is for audio. */
        g_context->audio_stream_num = audio_str;
    }
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

    /* Make changes to default creation parameters based on selected brand. */
    switch (g_context->file_brand)
    {
    case VIDEO_FMT_BRAND_MP4:
        vf_file.movie_timescale = 1000;
        vf_file.major_brand = MP4_MAJOR_BRAND;
        vf_file.num_compat_brands = sizeof (MP4_COMPAT_BRANDS)
            / sizeof (MP4_COMPAT_BRANDS [0]);
        vf_file.compat_brands = &MP4_COMPAT_BRANDS [0];
        break;

    case VIDEO_FMT_BRAND_AMC:
        vf_file.movie_timescale = 90000;  /* AMC requirement */
        vf_file.major_brand = AMC_MAJOR_BRAND;
        vf_file.num_compat_brands = sizeof (AMC_COMPAT_BRANDS)
            / sizeof (AMC_COMPAT_BRANDS [0]);
        vf_file.compat_brands = &AMC_COMPAT_BRANDS [0];
        if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
        {
            vf_streams [video_str].priority = 16;  /* AMC requirement */
            vf_streams [video_str].media_timescale = 90000;  /* AMC requirement */
            //vf_streams [0].sample_delta = g_context->video_time_increment
            //    * 90000 / g_context->video_time_resolution;
          vf_streams [video_str].handler = "";
          g_context->video_repeat_header = TRUE;
        }
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
        if ((g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
#ifdef FEATURE_CAMCORDER_AAC_REC
            && (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
#endif /* FEATURE_CAMCORDER_AAC_REC */
            )
        {
            vf_streams [audio_str].priority = 16;  /* AMC requirement */
            //vf_streams [audio_str].media_timescale = VOCODER_SAMPLING_RATE;
            vf_streams [audio_str].handler = "";
        }
        break;
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    case VIDEO_FMT_BRAND_3GP:
        vf_file.movie_timescale = 1000;
        vf_file.major_brand = _3GP_MAJOR_BRAND;
        if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
        {
            vf_streams [video_str].media_timescale = 1000;
        }
        vf_file.num_compat_brands = sizeof (_3GP_COMPAT_BRANDS)
            / sizeof (_3GP_COMPAT_BRANDS [0]);
        vf_file.compat_brands = &_3GP_COMPAT_BRANDS [0];
        break;

    case VIDEO_FMT_BRAND_3G2:
        vf_file.movie_timescale = 1000;
        vf_file.major_brand = _3G2_MAJOR_BRAND;
        vf_file.num_compat_brands = sizeof (_3G2_COMPAT_BRANDS)
            / sizeof (_3G2_COMPAT_BRANDS [0]);
        vf_file.compat_brands = &_3G2_COMPAT_BRANDS [0];
        break;

    case VIDEO_FMT_BRAND_K3G:
        vf_file.movie_timescale = 1000;
        vf_file.major_brand = K3G_MAJOR_BRAND;
        vf_file.num_compat_brands = sizeof (K3G_COMPAT_BRANDS)
            / sizeof (K3G_COMPAT_BRANDS [0]);
        vf_file.compat_brands = &K3G_COMPAT_BRANDS [0];
        break;

    case VIDEO_FMT_BRAND_SKM:
        vf_file.movie_timescale = 1000;
        vf_file.major_brand = SKM_MAJOR_BRAND;
        vf_file.force_stsz_table = TRUE;
        if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
        {
            vf_streams [video_str].handler = "vide";
        }
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
        if (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
        {
            vf_streams [audio_str].handler = "soun";
        }

        if (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_AMR)
        {
            vf_streams [audio_str].frames_per_sample = 10;
            g_context->audio_frames_per_sample =  vf_streams [audio_str].frames_per_sample;
            vf_streams [audio_str].max_table_stores
             = VIDEO_ENG_MAX_AUDIO_TABLE_STORES  (g_context);/*lint !e414*/
        }
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
        /* NOTE: the SKT VOD spec 2.3 (draft2) specifies '512' for the
        ** version, but there are 32-bits.  So we will guess they mean
        ** it to be a NULL-terminated ASCII character string.
        */
        vf_file.version_major = 0x3531;  /* '5', '1' */
        vf_file.version_minor = 0x3200;  /* '2', '\0' */

        vf_file.num_compat_brands = sizeof (SKM_COMPAT_BRANDS)
            / sizeof (SKM_COMPAT_BRANDS [0]);
        vf_file.compat_brands = &SKM_COMPAT_BRANDS [0];
        break;

    case VIDEO_FMT_BRAND_FRAG_3G2:
        vf_file.movie_timescale = 90000;  /* spec requirement */
        vf_file.fragment_size
            = VIDEO_ENG_FRAG_3G2_FRAGMENT_SIZE * vf_file.movie_timescale;
        if (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_AMR)
        {
            vf_file.major_brand = FRAG_3G2_MAJOR_BRAND;
            vf_file.num_compat_brands = sizeof (FRAG_3G2_COMPAT_BRANDS)
                / sizeof (FRAG_3G2_COMPAT_BRANDS [0]);
            vf_file.compat_brands = &FRAG_3G2_COMPAT_BRANDS [0];
        }
        else
        {
            vf_file.major_brand = FRAG_3G2_MAJOR_BRAND_NOT_AMR;
            vf_file.num_compat_brands
                = sizeof (FRAG_3G2_COMPAT_BRANDS_NOT_AMR)
                / sizeof (FRAG_3G2_COMPAT_BRANDS_NOT_AMR [0]);
            vf_file.compat_brands = &FRAG_3G2_COMPAT_BRANDS_NOT_AMR [0];
        }
        if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
        {
            vf_streams [video_str].priority = 16;  /* spec requirement */
            vf_streams [video_str].media_timescale = 90000;  /* spec requirement */
            vf_streams [video_str].handler = "";
        }
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
        if (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
        {
            vf_streams [audio_str].priority = 16;  /* spec requirement */
            //vf_streams [audio_str].media_timescale = VOCODER_SAMPLING_RATE;
            vf_streams [audio_str].handler = "";
        }
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
        break;

    default:
        CAMCORDER_MSG_ERROR ("start_processing: wrong file brand!", 0,0,0);
        break;
    }

    if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
    {
        g_context->video_sample_delta = vf_streams [video_str].sample_delta;
        g_context->media_timescale = vf_streams [video_str].media_timescale;
        vf_streams [video_str].interlace_rate = MAX
            (vf_streams [video_str].media_timescale
             * VIDEO_ENG_DESIRED_INTERLACE_RATE / 1000, 1);
    }
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    if (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
    {
        g_context->audio_time_resolution
            = vf_streams [audio_str].media_timescale;
    }
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

    /* Set file duration limit (if any) according to movie timescale. */
    vf_file.file_duration_limit = (uint32)
        ((uint64) g_context->movie_duration_limit
         * vf_file.movie_timescale / 1000);

    /* If an audio stream is included, save a copy of the frames per sample
    ** setting, which may have been set in the brand switch statement.
    */
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    if (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
    {
        /* Update some variables according to the number of
        ** frames per sample.
        */
        g_context->audio_frames_per_sample
            = vf_streams [audio_str].frames_per_sample;
        g_context->audio_leftover_bytes = 0;
        vf_streams [audio_str].samples_out_near_threshold
            /= vf_streams [audio_str].frames_per_sample;
        vf_streams [audio_str].samples_out_imminent_threshold
            /= vf_streams [audio_str].frames_per_sample;
        vf_streams [audio_str].max_samples
            /= vf_streams [audio_str].frames_per_sample;

        /* If we are packing more than one frame per sample, the samples
        ** cannot be fixed size, because the last sample may not have the same
        ** number of frames as the ones before it.
        **
        ** Also, we need to make sure the sample deltas are multiplied by the
        ** number of frames per sample.
        **
        ** Also, we need to allocate and initialize a memory buffer to hold
        ** left-over packets between audio stream writes.
        */
        if (g_context->audio_frames_per_sample != 1)
        {
            vf_streams [audio_str].sample_size = 0;
            vf_streams [audio_str].sample_delta= 0;
            g_context->audio_leftover = videoeng_malloc
                (g_context->audio_sample_size
                 * g_context->audio_frames_per_sample);
            if (g_context->audio_leftover == NULL)
            {
                CAMCORDER_MSG_ERROR ("start_processing: Unable to allocate "
                           "audio buffer memory!", 0, 0, 0);
                g_context->failure_on_exit = TRUE;
                g_context->terminate = TRUE;
                return;
            }
        }
    }
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

    /* Configure timed text encoder if there is a text stream. */
    if (g_context->text_type != VIDEO_ENG_TEXT_INPUT_NONE)
    {
        /* Force stream before text to interlace with text stream. */
        if (!g_context->no_interlace && text_str > 0)
        {
            vf_streams [text_str - 1].interlace = text_str;
        }

        /* Set up text stream creation parameters. */
        vf_streams [text_str].interlace = vf_file.num_streams;
        vf_streams [text_str].type = VIDEO_FMT_STREAM_TEXT;
        vf_streams [text_str].handler = "TimedTextHandler";
        vf_streams [text_str].media_timescale = 1000;
        vf_streams [text_str].chunk_size = VIDEO_ENG_TEXT_STREAM_BUF_SIZE / 2;
        vf_streams [text_str].buffer_size = VIDEO_ENG_TEXT_STREAM_BUF_SIZE;
        vf_streams [text_str].max_samples = VIDEO_ENG_MAX_TEXT_SAMPLES;
        vf_streams [text_str].max_chunks = VIDEO_ENG_MAX_TEXT_SAMPLES;
        vf_streams [text_str].stsc_reset_rate = VIDEO_ENG_STSC_ALGO_RESET_RATE;
        vf_streams [text_str].layer = -1;
        vf_streams [text_str].width
            = (((uint32) g_context->frame_width) << 16);
        vf_streams [text_str].height = (VIDEO_ENG_TEXT_BOX_HEIGHT << 16);
        vf_streams [text_str].origin_x = 0;
        vf_streams [text_str].origin_y
            = (((uint32) g_context->frame_height) << 16);

        /* Remember which stream is for timed text. */
        g_context->text_stream_num = text_str;

        /* Save a copy of the text box size, to use later in constructing the
        ** sample entry for the track.
        */
        g_context->text_box_width = (int16) g_context->frame_width;
        g_context->text_box_height = VIDEO_ENG_TEXT_BOX_HEIGHT;
    }

    /* Allocate video format services instance. */
    CAMCORDER_MSG_HIGH ("start_processing: "
              "allocating videofmt instance", 0, 0, 0);
    video_fmt_create (&vf_file, video_eng_videofmt_callback,
                      NULL, g_context->file_format);
    while ((g_context->videofmt_status != VIDEO_FMT_WRITER_READY)
           && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
    {
        if (g_context->videofmt_continue == NULL)
        {
            CAMCORDER_MSG_ERROR ("start_processing: bad continue "
                       "function from videofmt!", 0, 0, 0);
            break;
        }
        else
        {
            g_context->videofmt_continue (g_context->videofmt_server_data);
        }
    }
    if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
    {
        g_context->videofmt_server_data = NULL;
        CAMCORDER_MSG_ERROR ("start_processing: "
                   "failed to create movie file", 0, 0, 0);
        g_context->failure_on_exit = TRUE;
        g_context->terminate = TRUE;
        return;
    }

    /* If there is uuid data, it's time to write it to videofmt. */
    udat = g_context->udat_head;
    while (udat)
    {
        /* Write out atom header. */
        g_context->videofmt_write_uuid (udat->uuid, udat->data, udat->size,
                                        g_context->videofmt_server_data,
                                        video_eng_videofmt_callback, NULL);
        while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE)
              && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
        {
            if (g_context->videofmt_continue == NULL)
            {
                CAMCORDER_MSG_ERROR ("start_processing: bad continue "
                          "function from videofmt!", 0, 0, 0);
                break;
            }
            else
            {
                g_context->videofmt_continue (g_context->videofmt_server_data);
            }
        }
        if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
        {
            g_context->videofmt_server_data = NULL;
            CAMCORDER_MSG_ERROR ("start_processing: "
                      "failed to write UUID atom", 0, 0, 0);
            video_eng_enc_failure (FALSE);
            return;
        }

        if (udat->data)
        {
            videoeng_free (udat->data);
        }
        g_context->udat_head = udat->next;
        videoeng_free (udat);
        udat = g_context->udat_head;
    }
    g_context->udat_tail = NULL;

    /* If audio is enabled, write codec-specific header information. */
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    if (audio_header)
    {
        /* Write the header to the movie file. */
        g_context->videofmt_write_header
            (audio_str, audio_header_size, audio_header,
             g_context->videofmt_server_data,
             video_eng_videofmt_callback, NULL);
        while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE)
               && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
        {
            if (g_context->videofmt_continue == NULL)
            {
                CAMCORDER_MSG_ERROR ("start_processing: bad continue "
                           "function from videofmt!", 0, 0, 0);
                break;
            }
            else
            {
                g_context->videofmt_continue (g_context->videofmt_server_data);
            }
        }
        if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
        {
            g_context->videofmt_server_data = NULL;
            CAMCORDER_MSG_ERROR ("start_processing: "
                       "failed to write stream header", 0, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }
    }
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

    /* If timed text is enabled, parse it according to the input format into a
    ** list of sample entries and samples.
    */
    switch (g_context->text_type)
    {
#ifdef FEATURE_VIDEO_ENG_TELOP_PARSER
    case VIDEO_ENG_TEXT_INPUT_TELOP:
        if (!video_eng_telop_parse (g_context))
        {
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
        }
        break;
#endif /* FEATURE_VIDEO_ENG_TELOP_PARSER */
    case VIDEO_ENG_TEXT_INPUT_NONE:
        break;
    }

    /* Write the footer to the movie file. */
    if (g_context->video_format == VIDEO_FMT_STREAM_VIDEO_MPEG4)
    {
        g_context->videofmt_write_footer
            (video_str, sizeof(M4V_BITSTREAM_FIXED_FOOTER),
              M4V_BITSTREAM_FIXED_FOOTER,
              g_context->videofmt_server_data,
              video_eng_videofmt_callback, NULL);
        while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE)
                && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
        {
            if (g_context->videofmt_continue == NULL)
            {
                CAMCORDER_MSG_ERROR ("write_processing: bad continue "
                            "function from videofmt!", 0, 0, 0);
                break;
            }
            else
            {
                g_context->videofmt_continue (g_context->videofmt_server_data);
            }
        }
        if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
        {
            g_context->videofmt_server_data = NULL;
            CAMCORDER_MSG_ERROR ("video_processing: "
                        "failed to write stream footer", 0, 0, 0);
            video_eng_enc_failure (FALSE);
            return;
        }
    }
    }

    /*-----------------------------------------------------------------------
    ** Initialize variables and start video/audio encoder.
    **-----------------------------------------------------------------------
    */

    /* Initialize audio/video synchronization variables. */
    g_context->video_done = FALSE;
    g_context->audio_done = FALSE;
    g_context->video_error = FALSE;
    g_context->audio_error = FALSE;

    /* Initialize data processing variables. */
    g_context->video_frames_received = 0;
    g_context->space_out_near = FALSE;
    g_context->space_out_imminent = FALSE;
    g_context->space_out = FALSE;
    g_context->buffer_out = FALSE;
    g_context->packet_out = FALSE;
    g_context->first_audio_packet_dropped = FALSE;

    /* Initialize Audio Video Sync relates variables */
    g_context->avSync_info.first_audio_cb_received = FALSE;
    g_context->avSync_info.vfe_config_delay = 0;
    g_context->avSync_info.timelog_vfeConfig_cmd = 0;
    g_context->avSync_info.skip_audio_pkts = 0;
    g_context->avSync_info.skip_audio_sum_ms = 0;
    g_context->avSync_info.skip_audio_sum_ms_prev = 0;
    g_context->avSync_info.last_skip_ms = 0;
    g_context->avSync_info.last_skip_num_fr = 0;

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_CAMCORDER_AAC_REC
    /* This is the sync signal callback time error adjustment (msec).
    ** Since CMX MM API's very first sync shows a problem of
    ** timing error by 1 AAC frame, this adjustment was
    ** necessary. It should be 0 when CMX fixes the bug */
    if(g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
    {
       g_context->avSync_info.audio_sync_error_adjust =
         (g_context->audio_sample_delta * 1000) / g_context->audio_time_resolution;
    }
    else
#endif /* FEATURE_CAMCORDER_AAC_REC */
    {
          g_context->avSync_info.audio_sync_error_adjust = 0;
    }
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

    /* Set flag to track audio,video start, we do not track audio start other */
    glb_avs_info.audioConfig_done = FALSE;
    glb_avs_info.videoConfig_done = FALSE;
    glb_avs_info.timelog_video_start = 0;
    glb_avs_info.timelog_audio_start = 0;

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    if(g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_NONE)
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    {
        glb_avs_info.audioConfig_done = TRUE;
    }

    /* Move engine to record starting state, waiting for encoder to
    ** start.
    */
    g_context->state = VIDEO_ENG_STATE_STARTING;
    if (g_context->no_coding)
    {
        g_context->first_audio_packet_dropped = TRUE;

        /* Send a success status code back to our client. */
        g_context->callback_ptr
            (VIDEO_SUCCESS, g_context->client_data);
    }
    else
    {
    /* If movie has video, tell the video encoder to start recording.
    */
    if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
    {
        CAMCORDER_MSG_HIGH ("start_processing: starting encoder", 0, 0, 0);
        /* If there is a failure in Video, Then no need to go further
           return from here.
        */
        if(g_context->framecb_ptr == NULL)
        {
            if(video_encode_start (video_eng_enc_req_cb, NULL) != VIDEO_SUCCESS)
            {
               /*
                 Since Video has failed to start, we need to set this flag to initiate
                 Audio de-initialization.
               */
               g_context->audio_done = TRUE;
               return;
            }
        }
        else
        {
#ifndef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
           if(video_encode_start (video_eng_venc_req_cb, NULL) != VIDEO_SUCCESS)
#else
           if(video_encode_start (video_eng_enc_req_cb, NULL) != VIDEO_SUCCESS)
#endif 
           {
               /*
               Since Video has failed to start, we need to set this flag to initiate
               Audio de-initialization.
               */
               g_context->audio_done = TRUE;
               return;
           }
        }

        #ifdef FEATURE_VIDEO_CLIP_INFO
        if (video_encode_get_config (&config_data, &adv, NULL, NULL) == VIDEO_SUCCESS)
        {
          clipinfo->target_bitrate = config_data.target_bitrate;
          clipinfo->intra_period = config_data.intra_period;
          clipinfo->first_frame_delta = config_data.first_frame_delta;
          clipinfo->time_unit = (uint8)config_data.time_unit;
          clipinfo->rc_ctl = (uint8)config_data.rc_ctl;
   
          clipinfo->adv_config_cookie = adv.cookie;
          clipinfo->quantizer_scale_intra = adv.quantizer_scale_intra ;
          clipinfo->quantizer_scale_inter = adv.quantizer_scale_inter ;
          clipinfo->frac_pixel_enable = adv.frac_pixel_enable ;
          clipinfo->rcw_overflow_factor = adv.rcw_overflow_factor ;
          clipinfo->pframe_overflow_factor = adv.pframe_overflow_factor ;
          clipinfo->loose_pframe_overflow_factor = adv.loose_pframe_overflow_factor ;
          clipinfo->max_qp_frame_range_down = adv.max_qp_frame_range_down ;
          clipinfo->max_qp_frame_range_up = adv.max_qp_frame_range_up ;
          clipinfo->log2_nc_square = adv.log2_nc_square ;
          clipinfo->alpha = adv.alpha ;
          clipinfo->beta = adv.beta ;
          clipinfo->quantizer_scale_intra_low = adv.quantizer_scale_intra_low ;
          clipinfo->quantizer_scale_intra_high = adv.quantizer_scale_intra_high ;
          clipinfo->quantizer_scale_inter_low = adv.quantizer_scale_inter_low ;
          clipinfo->quantizer_scale_inter_high = adv.quantizer_scale_inter_high ;
   
          clipinfo->dsp_cfg_cookie = adv.dsp_cfg.cookie;
          clipinfo->intra_16x16_bias = adv.dsp_cfg.intra_16x16_bias;
          clipinfo->intra_4x4_bias = adv.dsp_cfg.intra_4x4_bias;
          clipinfo->inter_one_mv_bias = adv.dsp_cfg.inter_one_mv_bias;
          clipinfo->inter_four_mv_bias = adv.dsp_cfg.inter_four_mv_bias;
          clipinfo->four_mv_threshold = adv.dsp_cfg.four_mv_threshold;
          clipinfo->search_type = adv.dsp_cfg.search_type;
          clipinfo->use_16x16_srch_min= adv.dsp_cfg.use_16x16_srch_min;
          clipinfo->use_16x16_anch_pnt = adv.dsp_cfg.use_16x16_anch_pnt;
          clipinfo->ac_pred_flag = adv.dsp_cfg.ac_pred_flag;
          clipinfo->rounding_ctrl = adv.dsp_cfg.rounding_ctrl;
          clipinfo->video_hw_me_profile =  adv.dsp_cfg.video_hw_me_profile;
          clipinfo->search_method = (uint8)adv.dsp_cfg.search_method;
   
          clipinfo->luma_filter = (uint8)adv.luma_filter;
        }
        #endif //FEATURE_VIDEO_CLIP_INFO
    }

    /* Otherwise, mark video part as already done. */
    else
    {
      g_context->video_done = TRUE;
      g_context->reorder_atom = FALSE;
      g_context->first_audio_packet_dropped = TRUE;
    }

#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */

#ifdef FEATURE_VIDEO_CLIP_INFO
    /* Storing encoder config information into clipinfo */
    clipinfo->still_image = g_context->still_image;
    clipinfo->audio_rate_set = g_context->audio_rate_set;
    clipinfo->no_interlace = g_context->no_interlace;
    clipinfo->movie_duration_limit = g_context->movie_duration_limit;
    clipinfo->include_drm = g_context->include_drm;
    clipinfo->drm_distribution_rights = g_context->drm_distribution_rights;
    clipinfo->title_lang = g_context->useratom_data.title.language;
    clipinfo->author_lang = g_context->useratom_data.author.language;
    clipinfo->description_lang = g_context->useratom_data.description.language;
    clipinfo->copyright_lang = g_context->useratom_data.copyright.language;
    clipinfo->loci_lang = g_context->useratom_data.loci.name.language;
#endif //FEATURE_VIDEO_CLIP_INFO

    /* If audio is enabled, tell CMX to begin recording.
    */
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_CAMCORDER_AAC_REC
    if (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
    {
        cmx_handle.client.source_type = CMX_SOURCE_CLIENT;
        cmx_handle.client.data_config = CMX_CLIENT_DATA_RANDOM;
        cmx_handle.client.data_len = (uint32) -1;
        cmx_handle.client.data_req_ptr = (cmx_client_req_func_ptr_type)
            video_eng_cmx_aac_data_cb;

        aac_rec_param.mm_rec_link = CMX_MM_REC_LINK_REVERSE;
        aac_rec_param.mm_rec_param.mm_rec_format = CMX_MM_FORMAT_AAC;
        aac_rec_param.mm_rec_param.cmx_mm_rec_aac_para.aac_file_format
            = CMX_AF_AAC_DATA_FORMAT_ADTS;
        aac_rec_param.mm_rec_param.cmx_mm_rec_aac_para.sample_rate
            = cmx_sample_rates [g_context->aac_sampling_rate_index];
        aac_rec_param.mm_rec_param.cmx_mm_rec_aac_para.sync_cb_func
            = (cmx_mm_rec_av_sync_cb_ptr) video_eng_cmx_sync_cb_func;
        aac_rec_param.mm_rec_param.cmx_mm_rec_aac_para.chan
            = cmx_channel_modes [g_context->aac_channel_mode];
        aac_rec_param.mm_rec_param.cmx_mm_rec_aac_para.bit_per_spl
            = g_context->aac_bits_per_sample;
        aac_rec_param.mm_rec_param.cmx_mm_rec_aac_para.sync_ms = AV_SYNC_CMX_SIGNALING_TIME;

        cmx_mm_record(&cmx_handle,
                      CMX_MM_REC_OPERATION_NORMAL,
                      &aac_rec_param,
                      (cmx_cb_func_ptr_type) video_eng_cmx_normal_cb,
                      NULL);
    }
    else
#endif /* FEATURE_CAMCORDER_AAC_REC */

    if (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
    {
        cmx_handle.client.source_type = CMX_SOURCE_CLIENT;
        cmx_handle.client.data_config = CMX_CLIENT_DATA_RANDOM;
        cmx_handle.client.data_len = (uint32) -1;
        cmx_handle.client.data_req_ptr = (cmx_client_req_func_ptr_type)
            video_eng_cmx_data_cb;

        // Init params for mm_record for AMR
        cmx_handle.client.source_type = CMX_SOURCE_CLIENT;
        cmx_handle.client.data_config = CMX_CLIENT_DATA_RANDOM;
        cmx_handle.client.data_len = (uint32) -1;
        cmx_handle.client.data_req_ptr = (cmx_client_req_func_ptr_type)
        video_eng_cmx_data_cb;
        qcp_rec_param.mm_rec_link = CMX_MM_REC_LINK_REVERSE;
        qcp_rec_param.mm_rec_param.mm_rec_format = CMX_MM_FORMAT_QCP;

        switch (g_context->audio_format)
        {
           case VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL:
              switch (g_context->audio_rate_set)
              {
                case VIDEO_FMT_RATE_SET_FIXED:
                default:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_QCP_FORMAT_FIXED_FULL_13K;
                break;

                case VIDEO_FMT_RATE_SET_VAR:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_QCP_FORMAT_VAR_FULL_13K;
                   cmx_handle.client.data_req_ptr
                       = (cmx_client_req_func_ptr_type) video_eng_cmx_var_data_cb;
                break;
              }
           break;


           case VIDEO_FMT_STREAM_AUDIO_QCELP13K_HALF:
              switch (g_context->audio_rate_set)
              {
                case VIDEO_FMT_RATE_SET_FIXED:
                default:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_QCP_FORMAT_FIXED_HALF_13K;
                break;
                case VIDEO_FMT_RATE_SET_VAR:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_QCP_FORMAT_VAR_HALF_13K;
                   cmx_handle.client.data_req_ptr
                      = (cmx_client_req_func_ptr_type) video_eng_cmx_var_data_cb;
                break;
              }
           break;

           case VIDEO_FMT_STREAM_AUDIO_EVRC:
              switch (g_context->audio_rate_set)
        {
                case VIDEO_FMT_RATE_SET_FIXED:
                default:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_QCP_FORMAT_FIXED_FULL_EVRC;
                break;
                case VIDEO_FMT_RATE_SET_VAR:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_QCP_FORMAT_VAR_FULL_EVRC;
                   cmx_handle.client.data_req_ptr = (cmx_client_req_func_ptr_type)video_eng_cmx_var_data_cb;
                break;
              }
           break;

           case VIDEO_FMT_STREAM_AUDIO_AMR:
              switch (g_context->audio_rate_set)
              {
                case VIDEO_FMT_RATE_SET_AMR_1220:
                default:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_AMR_FORMAT_FIXED_12_2_AMR;
                break;
                case VIDEO_FMT_RATE_SET_AMR_1020:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_AMR_FORMAT_FIXED_10_2_AMR;
                break;
                case VIDEO_FMT_RATE_SET_AMR_0795:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_AMR_FORMAT_FIXED_7_95_AMR;
                break;
                case VIDEO_FMT_RATE_SET_AMR_0740:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_AMR_FORMAT_FIXED_7_4_AMR;
                break;
                case VIDEO_FMT_RATE_SET_AMR_0670:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_AMR_FORMAT_FIXED_6_7_AMR;
                break;
                case VIDEO_FMT_RATE_SET_AMR_0590:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_AMR_FORMAT_FIXED_5_9_AMR;
                break;
                case VIDEO_FMT_RATE_SET_AMR_0515:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_AMR_FORMAT_FIXED_5_15_AMR;
                break;
                case VIDEO_FMT_RATE_SET_AMR_0475:
                   qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.format = CMX_AMR_FORMAT_FIXED_4_75_AMR;
                break;
        }
           break;

           case VIDEO_FMT_STREAM_AUDIO_NONE:
           case VIDEO_FMT_STREAM_AUDIO_UNK:
           case VIDEO_FMT_STREAM_AUDIO_INVALID:
           default:
              CAMCORDER_MSG_HIGH ("start_processing: unexpected audio format %d",
              (int) g_context->audio_format, 0, 0);
           break;
    }
        qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.report_ms = 0;
        qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.auto_stop_ms = 0;
        qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.data_req_ms = AV_SYNC_CMX_SIGNALING_TIME;
        qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.mm_rec_format = CMX_MM_FORMAT_QCP;
        qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.rec_path = CMX_MM_REC_LINK_REVERSE;
        qcp_rec_param.mm_rec_param.cmx_mm_rec_qcp_para.sync_cb_func
            = (cmx_mm_rec_av_sync_cb_ptr) video_eng_cmx_sync_cb_func;
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
        if (g_context->framecb_ptr == NULL) 
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
        {
        cmx_mm_record(&cmx_handle,
             CMX_MM_REC_OPERATION_NORMAL,
             &qcp_rec_param,
             (cmx_cb_func_ptr_type) video_eng_cmx_normal_cb,
             NULL);
        }
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
        else
        {
           g_context->first_audio_packet_dropped = TRUE;
           g_context->audio_done = TRUE;
        }
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE

#ifdef DEBUG_AVSYNC
        clk_uptime_ms(time_ms);
        CAMCORDER_MSG_HIGH ("cmx_mm_record cmd sent_timelog = %d", time_ms[0], 0, 0);
#endif
    }
    /* Otherwise, mark audio part as already done. */
    else
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    {
        g_context->audio_done = TRUE;
    }
}
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_get_config_processing

DESCRIPTION
  This function gets the video engine config data.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_get_config_processing (
  video_config_type  *args
) {
    /* Verify state is appropriate for this command. */
    if (g_context->state != VIDEO_ENG_STATE_IDLE)
    {
        CAMCORDER_MSG_ERROR ("get_config_proc: Engine not idle!", 0, 0, 0);
        g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
        return;
    }

    args->still_image             =  g_context->still_image;
    args->audio_rate_set          =  g_context->audio_rate_set;
    args->no_interlace            =  g_context->no_interlace;
    args->include_drm             =  g_context->include_drm;
    args->drm_distribution_rights =  g_context->drm_distribution_rights;
    args->movie_duration_limit    =  g_context->movie_duration_limit;

    /* Check & Update user related data */
    args->title                   =  g_context->useratom_data.title.string.utf8;
    args->title_lang              =  g_context->useratom_data.title.language;
    args->author                  =  g_context->useratom_data.author.string.utf8;
    args->author_lang             =  g_context->useratom_data.author.language;
    args->description             =  g_context->useratom_data.description.string.utf8;
    args->description_lang        =  g_context->useratom_data.description.language;
    args->copyright               =  g_context->useratom_data.copyright.string.utf8;
    args->copyright_lang          =  g_context->useratom_data.copyright.language;
    args->loci.name               =  g_context->useratom_data.loci.name.string.utf8;
    args->loci.name_lang          =  g_context->useratom_data.loci.name.language;
    args->loci.astr_body          =  g_context->useratom_data.loci.astr_body.string.utf8;
    args->loci.astr_body_lang     =  g_context->useratom_data.loci.astr_body.language;
    args->loci.add_notes          =  g_context->useratom_data.loci.add_notes.string.utf8;
    args->loci.add_notes_lang     =  g_context->useratom_data.loci.add_notes.language;
    args->loci.latitude           =  g_context->useratom_data.loci.latitude;
    args->loci.longitude          =  g_context->useratom_data.loci.longitude;
    args->loci.altitude           =  g_context->useratom_data.loci.altitude;
    args->adjust_es_time          =  g_context->adjust_es_time;

    /* Retrieve all initialized encoder config parameters */
    if((g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE) && 
       (g_context->no_coding == FALSE) &&(video_encode_get_config(&args->enc_config,(void*)&args->enc_adv_config,
                               NULL,NULL) != VIDEO_SUCCESS))
    {
      g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
    }
    else
    {
      /* Issue success callback. */
      g_context->callback_ptr (VIDEO_SUCCESS, g_context->client_data);
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_set_config_processing

DESCRIPTION
  This function sets the video engine config data.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_set_config_processing (
  video_config_type  *args
) {
    /* Verify state is appropriate for this command. */
    if (g_context->state != VIDEO_ENG_STATE_IDLE)
    {
        CAMCORDER_MSG_ERROR ("set_config_proc: Engine not idle!", 0, 0, 0);
        g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
        return;
    }
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    /* Verify audio rate set is one of those supported. */
    if ((g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL)
        || (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_QCELP13K_HALF)
        || (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_EVRC))
    {
        switch (args->audio_rate_set)
        {
        case VIDEO_FMT_RATE_SET_FIXED:
        case VIDEO_FMT_RATE_SET_VAR:
            /* Test passed. */
            break;

        default:
            CAMCORDER_MSG_ERROR ("set_config_proc: given AMR rate set "
                      "not supported!", 0, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }
    }

    /* Verify audio rate set is one of those supported. */
    else if (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_AMR)
    {
        switch (args->audio_rate_set)
        {
        case VIDEO_FMT_RATE_SET_AMR_1220:
        case VIDEO_FMT_RATE_SET_AMR_1020:
        case VIDEO_FMT_RATE_SET_AMR_0795:
        case VIDEO_FMT_RATE_SET_AMR_0740:
        case VIDEO_FMT_RATE_SET_AMR_0670:
        case VIDEO_FMT_RATE_SET_AMR_0590:
        case VIDEO_FMT_RATE_SET_AMR_0515:
        case VIDEO_FMT_RATE_SET_AMR_0475:
            /* Test passed. */
            break;

        default:
            CAMCORDER_MSG_ERROR ("set_config_proc: given AMR rate set "
                      "not supported!", 0, 0, 0);
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;
            return;
        }
    }
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    /* If still image flag is set, or no video is to be encoded, make
    ** sure interlace rate is not set.
    */
    if (args->still_image && !args->no_interlace)
    {
        CAMCORDER_MSG_ERROR ("set_config_proc: cannot interlace streams in"
                   " still image mode!", 0, 0, 0);
        g_context->failure_on_exit = TRUE;
        g_context->terminate = TRUE;
        return;
    }
    if ((g_context->video_format == VIDEO_FMT_STREAM_VIDEO_NONE)
        && !args->no_interlace)
    {
        CAMCORDER_MSG_ERROR ("set_config_proc: cannot interlace streams when"
                   " not recording video!", 0, 0, 0);
        g_context->failure_on_exit = TRUE;
        g_context->terminate = TRUE;
        return;
    }

    /* Store data from the argument structure. */
    g_context->still_image             =  args->still_image;
    g_context->audio_rate_set          =  args->audio_rate_set;
    g_context->no_interlace            =  args->no_interlace;
    g_context->movie_duration_limit    =  args->movie_duration_limit;
    g_context->include_drm             =  args->include_drm;
    g_context->drm_distribution_rights =  args->drm_distribution_rights;
    g_context->adjust_es_time          =  args->adjust_es_time;

    /* Check & update the Parameters */
    g_context->include_user_atom =  TRUE;
    g_context->useratom_data.atom_is_valid = FALSE;

    if (args->title || args->author || 
    args->description || args->copyright)
    {
        g_context->useratom_data.atom_is_valid = TRUE;
        g_context->useratom_data.titl_auth_dscp_cprt_valid = TRUE;

        /* Title */
        g_context->useratom_data.title.language = args->title_lang;
        g_context->useratom_data.title.isUTF16 = FALSE;
        g_context->useratom_data.title.string.utf8 = (args->title ? args->title : "");
        /* Author */ g_context->useratom_data.author.language = args->author_lang;
        g_context->useratom_data.author.isUTF16 = FALSE;
        g_context->useratom_data.author.string.utf8 = (args->author ? args->author : "");
        /* Description */
        g_context->useratom_data.description.language = args->description_lang;
        g_context->useratom_data.description.isUTF16 = FALSE;
        g_context->useratom_data.description.string.utf8 = (args->description ? args->description : "");
        /* Copyright */
        g_context->useratom_data.copyright.language = args->copyright_lang;
        g_context->useratom_data.copyright.isUTF16 = FALSE;
        g_context->useratom_data.copyright.string.utf8 = (args->copyright ? args->copyright : "");
    }
    /* Update the location information*/
    if(args->loci.name || args->loci.add_notes || args->loci.astr_body)
    {
        g_context->useratom_data.atom_is_valid = TRUE;
        g_context->useratom_data.loci.loci_valid = TRUE;
        /* Name */
        g_context->useratom_data.loci.name.language = args->loci.name_lang;
        g_context->useratom_data.loci.name.isUTF16 = FALSE;
        g_context->useratom_data.loci.name.string.utf8 = (args->loci.name ? args->loci.name : "");
        /* Astr Body */
        g_context->useratom_data.loci.astr_body.language = args->loci.astr_body_lang;
        g_context->useratom_data.loci.astr_body.isUTF16 = FALSE;
        g_context->useratom_data.loci.astr_body.string.utf8 = (args->loci.astr_body ? args->loci.astr_body : "");
        /* Additional Notes*/
        g_context->useratom_data.loci.add_notes.language = args->loci.add_notes_lang;
        g_context->useratom_data.loci.add_notes.isUTF16 = FALSE;
        g_context->useratom_data.loci.add_notes.string.utf8 = (args->loci.add_notes ? args->loci.add_notes : "");
        /* Role, latitude,longitude,altitude */
        g_context->useratom_data.loci.role      = args->loci.role;
        g_context->useratom_data.loci.latitude  = args->loci.latitude;
        g_context->useratom_data.loci.longitude = args->loci.longitude;
        g_context->useratom_data.loci.altitude  = args->loci.altitude;
    }

    /* Overwrite current config with new (user specified) config */
    if((g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE) && 
       (g_context->no_coding == FALSE) && (video_encode_set_config(&args->enc_config,(void*)&args->enc_adv_config,
                               NULL,NULL) != VIDEO_SUCCESS ))
    {
      g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
    }
    else
    {
      /* Issue success callback. */
      g_context->callback_ptr (VIDEO_SUCCESS, g_context->client_data);
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_write_uuid_processing

DESCRIPTION
  This function writes a user data atom to the video file.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_write_uuid_processing (
  video_eng_udat_list_type     *udat
) {
    /* Verify state is appropriate for this command. */
    if (g_context->state != VIDEO_ENG_STATE_IDLE)
    {
        CAMCORDER_MSG_ERROR ("write_uuid_proc: Engine not idle!",
                   0, 0, 0);
        g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
        return;
    }

    /* Add user data to the linked list. */
    if (g_context->udat_tail == NULL)
    {
        g_context->udat_head = udat;
        g_context->udat_tail = udat;
        udat->next = NULL;
    }
    else
    {
        g_context->udat_tail->next = udat;
        g_context->udat_tail = udat;
        udat->next = NULL;
    }

    /* Issue success callback. */
    g_context->callback_ptr (VIDEO_SUCCESS, g_context->client_data);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_pause_processing

DESCRIPTION
  This function pauses recording the current video file being recorded.

DEPENDENCIES
  video_eng_start_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_pause_processing (void)
{
    /* Verify state is appropriate for this command. */
    if (g_context->state != VIDEO_ENG_STATE_RECORDING)
    {
        CAMCORDER_MSG_ERROR ("pause_processing: Engine not recording!",
                   0, 0, 0);
        g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
        return;
    }

    /* Initialize audio/video synchronization variables. */
    g_context->video_done = FALSE;
    g_context->audio_done = FALSE;
    g_context->video_error = FALSE;
    g_context->audio_error = FALSE;

    /* If movie has video, tell the video encoder to stop recording.
    */
    if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
    {
        CAMCORDER_MSG_HIGH ("pause_processing: stopping encoder", 0, 0, 0);
        (void) video_encode_stop ((video_cb_func_ptr_type)
                                  video_eng_enc_normal_cb, NULL);
    }

    /* Otherwise, mark video part as already done. */
    else
    {
        g_context->video_done = TRUE;
    }

    /* If audio is enabled, tell CMX to pause recording.
    */
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_CAMCORDER_AAC_REC
    if (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
    {
        cmx_mm_record_pause (CMX_MM_REC_LINK_REVERSE,
            (cmx_cb_func_ptr_type) video_eng_cmx_normal_cb, NULL);
    }
    else
#endif /* FEATURE_CAMCORDER_AAC_REC */

    if (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
    {
           cmx_mm_record_pause (CMX_MM_REC_LINK_REVERSE,
                (cmx_cb_func_ptr_type) video_eng_cmx_normal_cb, NULL);

    }

    /* Otherwise, mark audio part as already done. */
    else
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    {
        g_context->audio_done = TRUE;
    }

    /* Move the engine to pausing state, waiting for encoder to stop.
    */
    g_context->state = VIDEO_ENG_STATE_PAUSING;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_resume_processing

DESCRIPTION
  This function resumes recording the current video file that was recording
  but was paused.

DEPENDENCIES
  video_eng_start_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_resume_processing (void)
{
    /* Verify state is appropriate for this command. */
    if (g_context->state != VIDEO_ENG_STATE_PAUSED)
    {
        CAMCORDER_MSG_ERROR ("resume_processing: Engine not paused!",
                   0, 0, 0);
        g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
        return;
    }

    /* Initialize audio/video synchronization variables. */
    g_context->video_done = FALSE;
    g_context->audio_done = FALSE;
    g_context->video_error = FALSE;
    g_context->audio_error = FALSE;

    /* Move engine to record resuming state, waiting for encoder to start.
    */
    g_context->state = VIDEO_ENG_STATE_RESUMING;

    /* If movie has video, tell the video encoder to resume recording.
    */
    if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
    {
        CAMCORDER_MSG_HIGH ("resume_processing: starting encoder", 0, 0, 0);
        /*If starting of Video Fails no Need to proceed further
          return.
        */
        if(video_encode_start (video_eng_enc_req_cb, NULL) != VIDEO_SUCCESS)
        {
            /*
              Since Video has failed to start, we need to set this flag to initiate
              Audio de-initailization.
            */
           g_context->audio_done = TRUE;
           return;
        }

        g_context->first_audio_packet_dropped = FALSE;
        glb_avs_info.timelog_video_start = 0;
        glb_avs_info.timelog_audio_start = 0;

        /* Initialize Audio Video Sync relates variables */
        g_context->avSync_info.vfe_config_delay = 0;
        g_context->avSync_info.timelog_vfeConfig_cmd = 0;
        g_context->avSync_info.skip_audio_pkts = 0;
        g_context->avSync_info.skip_audio_sum_ms = 0;
        g_context->avSync_info.skip_audio_sum_ms_prev = 0;
        g_context->avSync_info.last_skip_ms = 0;
        g_context->avSync_info.last_skip_num_fr = 0;

        /*Initialize AV Sync error adjustment if there is a mismatch in
        audio video duration during pausing. 
        This compensation is essential as other wise audio packets from time
         of resume will appear to be drawn back or pushed ahead in time

                                   pause - resume
        audio stream : -----------------||-----------------------
        video stream : ----------------------||------------------
                        AV Sync error-> |----|<----         
        */
        if (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
        {

            uint64 temp64;
            uint32 audio_duration;
            uint32 video_duration;

            /*Convert video delta to ms*/
            temp64 = (uint64) g_context->av_sync_duration;
            temp64 *= 1000;
            temp64 += (g_context->video_time_resolution >> 1);
            temp64 /= g_context->video_time_resolution;
            video_duration = (uint32) temp64;
            video_duration += (g_context->frame_drop_info.frame_drop_duration +
               g_context->frame_drop_info.frame_drop_duration_before_pause);

            /*Convert audio num packets to ms*/
            temp64 = (uint64) g_context->audio_num_packets;
            temp64 *= g_context->audio_sample_delta;
            temp64 *= 1000;
            temp64 += (g_context->audio_time_resolution >> 1);
            temp64 /= g_context->audio_time_resolution;
            audio_duration = (uint32)temp64;
#ifdef FEATURE_CAMCORDER_AAC_REC
            if(g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
            {
               /* We get AV Sync callback from cmx delayed for AAC.
               This has to be accounted for as well as we do in
               start processing*/
               g_context->avSync_info.audio_sync_error_adjust =
               (g_context->audio_sample_delta * 1000) / g_context->audio_time_resolution;
               g_context->avSync_info.audio_sync_error_adjust +=
                        audio_duration - video_duration;
            }
            else
#endif /*FEATURE_CAMCORDER_AAC_REC*/
            g_context->avSync_info.audio_sync_error_adjust = 
                                  audio_duration - video_duration;
#ifdef DEBUG_AVSYNC
            CAMCORDER_MSG_HIGH("Sa Dbg: A/V Time Diff while pause = %d", 
            g_context->avSync_info.audio_sync_error_adjust,0,0);
#endif /*DEBUG_AVSYNC*/
        }
        g_context->frame_drop_info.first_frame_encode_pending = TRUE;
        g_context->frame_drop_info.first_souce_frame_dropped = FALSE;

    }

    /* Otherwise, mark video part as already done. */
    else
    {
        g_context->video_done = TRUE;
    }


    /* If audio is enabled, tell CMX to resume recording.
    */
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_CAMCORDER_AAC_REC
    if (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
    {
        cmx_mm_record_resume (CMX_MM_REC_LINK_REVERSE,
            (cmx_cb_func_ptr_type) video_eng_cmx_normal_cb, NULL);
    }
    else
#endif /* FEATURE_CAMCORDER_AAC_REC */

    if (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
    {
        cmx_mm_record_resume (CMX_MM_REC_LINK_REVERSE,
            (cmx_cb_func_ptr_type) video_eng_cmx_normal_cb, NULL);
    }

    /* Otherwise, mark audio part as already done. */
    else
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    {
        g_context->audio_done = TRUE;
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_stop_processing

DESCRIPTION
  This function stops recording a video file, writes the file meta-data, and
  closes the recorded video file.

DEPENDENCIES
  video_eng_start_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_stop_processing (void)
{

    if (g_context->state == VIDEO_ENG_STATE_STOPPING)
    {
        CAMCORDER_MSG_HIGH ("stop_processing: stopping already in progress",
                   0, 0, 0);
        return;
    }

    /* If engine is idle, stop is a NO-OP, so simply return SUCCESS code. */
    if (g_context->state == VIDEO_ENG_STATE_IDLE)
    {
        g_context->callback_ptr (VIDEO_SUCCESS, g_context->client_data);
        return;
    }

    /* Verify state is appropriate for this command. */
    if ((g_context->state != VIDEO_ENG_STATE_RECORDING)
        && (g_context->state != VIDEO_ENG_STATE_PAUSED)
        /* 
           If a pause command is not successful due to CMX/Videoencoder
           then a stop command to close the modules is allowed.
        */
        && (g_context->state != VIDEO_ENG_STATE_PAUSING)
        )
    {
        CAMCORDER_MSG_ERROR ("stop_processing: Engine not recording!",
                   0, 0, 0);
        g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
        return;
    }

    /* Initialize audio/video synchronization variables. */
    g_context->video_done = FALSE;
    g_context->audio_done = FALSE;
    g_context->video_error = FALSE;
    g_context->audio_error = FALSE;

    if (g_context->no_coding)
    {
        /* Perform file close operations & move engine to idle state */
        video_eng_handle_close(FALSE);
        g_context->state = VIDEO_ENG_STATE_IDLE;
    }
    else
    {
    /* Move engine to stopping state, waiting for encoder to stop. */
    g_context->state = VIDEO_ENG_STATE_STOPPING;

    /* If movie has video, tell the video encoder to stop recording.
    */
    if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
    {
        CAMCORDER_MSG_HIGH ("stop_processing: stopping encoder", 0, 0, 0);
        (void) video_encode_stop ((video_cb_func_ptr_type)
                                  video_eng_enc_normal_cb, NULL);
    }

    /* Otherwise, mark video part as already done. */
    else
    {
        g_context->video_done = TRUE;
    }

    /* If audio is enabled, tell CMX to stop recording.
    */
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_CAMCORDER_AAC_REC
    if (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC 
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
        && g_context->framecb_ptr == NULL
#endif//FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
       )
    {
        cmx_mm_record_stop (CMX_MM_REC_LINK_REVERSE,
            (cmx_cb_func_ptr_type) video_eng_cmx_normal_cb, NULL);
    }
    else
#endif /* FEATURE_CAMCORDER_AAC_REC */

    if (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE 
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
        && g_context->framecb_ptr == NULL
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
       )
    {
        cmx_mm_record_stop (CMX_MM_REC_LINK_REVERSE,
            (cmx_cb_func_ptr_type) video_eng_cmx_normal_cb, NULL);
    }

    /* Otherwise, mark audio part as already done. */
    else
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    {
        g_context->audio_done = TRUE;
    }
}
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_exit_processing

DESCRIPTION
  This function cleans up the video and audio encoders after recording a video
  file.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_exit_processing (void)
{
    /* Verify state is appropriate for this command. */
    if (g_context->state != VIDEO_ENG_STATE_IDLE)
    {
        CAMCORDER_MSG_ERROR ("exit_processing: Engine not idle!",
                   0, 0, 0);
        g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
        return;
    }

    /* Move engine to exiting state, waiting for encoder to exit.
    */
    g_context->state = VIDEO_ENG_STATE_EXITING;


    /* If movie has no video, terminate engine immediately. */
    if ((g_context->video_format == VIDEO_FMT_STREAM_VIDEO_NONE) ||
        (g_context->no_coding == TRUE) )
    {
        video_eng_handle_exit ();
    }

    /* Otherwise, tell the video encoder to exit. */
    else
    {
        CAMCORDER_MSG_HIGH ("exit_processing: exiting encoder", 0, 0, 0);
        if(video_encode_exit ((video_cb_func_ptr_type)
                             video_eng_enc_normal_cb, NULL) != VIDEO_SUCCESS)
        {
          video_eng_enc_normal_cb(VIDEO_FAILURE,NULL);
        }

        g_context->video_done = FALSE;
        g_context->audio_done = TRUE;  /* no audio anymore by this point */
        g_context->video_error = FALSE;
        g_context->audio_error = FALSE;
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_enc_status_processing

DESCRIPTION
  This function processes a status change from the video encoder.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_enc_status_processing (
  video_status_type  status
) {
    /* Update video synchronization variables according to status. */
    switch (status)
    {
    case VIDEO_SUCCESS:
        g_context->video_done = TRUE;
        CAMCORDER_MSG_HIGH ("enc_status_processing: VIDEO_SUCCESS", 0, 0, 0);
        break;

    case VIDEO_ABORTED:
        g_context->video_done = TRUE;
        //g_context->video_error = TRUE;
        CAMCORDER_MSG_HIGH ("enc_status_processing: VIDEO_ABORTED", 0, 0, 0);
        break;

    case VIDEO_FAILURE:
        g_context->video_done = TRUE;
        g_context->video_error = TRUE;
        CAMCORDER_MSG_HIGH ("enc_status_processing: VIDEO_FAILURE", 0, 0, 0);
        break;

    default:
        CAMCORDER_MSG_ERROR ("enc_status_processing: Status = %d", status, 0, 0);
        break;
    }

    /* Update state machine according to the latest state information. */
    video_eng_audio_video_synchronize ();
}

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmx_status_processing

DESCRIPTION
  This function processes a status change from CMX.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_cmx_status_processing (
  cmx_status_type  status
) {
    boolean audio_video_synchronize = TRUE;

    /* Update audio synchronization variables according to status. */
    switch (status)
    {
    case CMX_SUCCESS:
        /* Unless we're in the "starting" state, ignore this - CMX_SUCCESS
        ** in all but the "starting" state means the command will be
        ** attempted, not that it is done or even that it will necessarily
        ** work.
        */
        CAMCORDER_MSG_HIGH ("cmx_status_processing: CMX_SUCCESS", 0, 0, 0);
        if (g_context->state == VIDEO_ENG_STATE_STARTING 
#ifdef FEATURE_CAMCORDER_AAC_REC
            && g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC
#endif /* FEATURE_CAMCORDER_AAC_REC */
            )
        {
            g_context->audio_done = TRUE;
        }
        break;
    case CMX_REC_ENABLED:
        CAMCORDER_MSG_HIGH ("cmx_status_processing: CMX_REC_ENABLED", 0, 0, 0);
        /* For AAC, we should do audio done here
        if (g_context->state == VIDEO_ENG_STATE_STARTING)
        {
            g_context->audio_done = TRUE;
        }*/
        break;

    case CMX_DONE:
    case CMX_PAUSE:
    case CMX_RESUME:
#ifdef FEATURE_CAMCORDER_AAC_REC
    case CMX_REC_PAUSED:  // AAC
    case CMX_REC_RESUMED: // AAC
#endif /* FEATURE_CAMCORDER_AAC_REC */
        /* All of these status codes indicate the last operation was
        ** successful.  We're not strict enough to check to make sure the
        ** success code received matches the command we last sent.
        */
        CAMCORDER_MSG_HIGH ("cmx_status_processing: CMX_DONE/PAUSE/RESUME", 0, 0, 0);
        g_context->audio_done = TRUE;
        break;

    case CMX_CONTINUE:
        /* This status doesn't tell us anything important, so we can ignore
        ** it.
        */
        CAMCORDER_MSG_HIGH ("cmx_status_processing: CMX_CONTINUE", 0, 0, 0);
        break;

    case CMX_ABORTED:
    case CMX_ERROR:
    case CMX_FAILURE:
    case CMX_INVALID_DATA_SOURCE:
        /* These are all possible error codes. */
        CAMCORDER_MSG_ERROR ("cmx_status_processing: CMX_ABORTED/ERROR/"
                             "FAILURE/INVALID_DATA_SOURCE %d", status, 0, 0);
        g_context->audio_done = TRUE;

        if(g_context->state != VIDEO_ENG_STATE_EXITING)
        {
          video_eng_enc_failure (FALSE);
          audio_video_synchronize = FALSE;
        }

        break;

    case CMX_QCP_SPACE_DONE:
        CAMCORDER_MSG_ERROR ("cmx_status_processing: CMX_QCP_SPACE_DONE", 0, 0, 0);

        audio_video_synchronize = FALSE;
        g_context->audio_done = TRUE;

        g_context->save_on_exit = TRUE;/* Save the clip */

        if(g_context->state != VIDEO_ENG_STATE_EXITING)
          video_eng_enc_failure (FALSE);

        break;

    default:
        CAMCORDER_MSG_ERROR ("cmx_status_processing: Status = %d", status, 0, 0);
        break;
    }

    /* Update state machine according to the latest state information. */
    if (audio_video_synchronize)
    {
        video_eng_audio_video_synchronize ();
    }
}
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_audio_video_synchronize

DESCRIPTION
  This function moves the state machine according to the current operation in
  progress and the current state of the audio/video synchronization
  variables.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_audio_video_synchronize (void)
{
    /* Do nothing unless both audio and video have finished. */
    if (!g_context->video_done || !g_context->audio_done)
    {
        return;
    }

    /* Process any errors that occur, unless we're already in the process of
    ** exiting.
    */
    if (g_context->state != VIDEO_ENG_STATE_EXITING)
    {
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE	    
        /* If the audio encoder failed, exit gracefully from the
        ** video encoder.
        */
        if (g_context->audio_error)
        {
            g_context->save_on_exit = TRUE;
            if ((g_context->state == VIDEO_ENG_STATE_IDLE)
                || (g_context->video_format == VIDEO_FMT_STREAM_VIDEO_NONE))
            {
                CAMCORDER_MSG_HIGH ("av_sync: audio error - exiting",
                          0, 0, 0);
                video_eng_enc_failure (FALSE);
            }
            else
            {
                /* If the encoder is already running, we need to stop it first,
                ** but then remember to exit immediately after stopping.
                */
                CAMCORDER_MSG_HIGH ("av_sync: audio error - aborting",
                          0, 0, 0);
                (void) video_encode_stop ((video_cb_func_ptr_type)
                                          video_eng_enc_normal_cb, NULL);
                g_context->state = VIDEO_ENG_STATE_ABORTING;
                g_context->video_done = FALSE;
                g_context->audio_done = TRUE;
                g_context->video_error = FALSE;
                g_context->audio_error = FALSE;
            }
            return;
        }
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
        /* If the video encoder failed, exit gracefully from the
        ** audio encoder.
        */
        if (g_context->video_error)
        {
            /* If the engine is initializing or exiting, terminate the engine
            ** immediately after sending failure callback.  Otherwise,
            ** initiate clean-up of encoder(s).
            */
            g_context->save_on_exit = TRUE;
            switch (g_context->state)
            {
            case VIDEO_ENG_STATE_INIT:
                /* Set up engine to terminate after this function returns. */
                g_context->terminate = TRUE;

                /* Send a failure status code back to our client. */
                g_context->callback_ptr
                    (VIDEO_FAILURE, g_context->client_data);
                break;

            case VIDEO_ENG_STATE_ABORTING:
                /* Clean up and exit engine with failure status, but do not
                ** mess with the audio - it is already gone.
                */
                video_eng_enc_failure (FALSE);
                break;
            /*In this state we haven't started recording any thing hence
            no need to save anything*/
            case VIDEO_ENG_STATE_STARTING:
                g_context->save_on_exit = FALSE;

                /* Clean up and exit engine with failure status. */
                video_eng_enc_failure (TRUE);
                break;
            default:
                /* Clean up and exit engine with failure status. */
                video_eng_enc_failure (TRUE);
            }
            return;
        }
    }

    /* Branch according to current state. */
    switch (g_context->state)
    {
    case VIDEO_ENG_STATE_INIT:
        /* Move engine to idle state. */
        g_context->state = VIDEO_ENG_STATE_IDLE;

        /* Send a success status code back to our client. */
        g_context->callback_ptr
            (VIDEO_SUCCESS, g_context->client_data);
        break;

    case VIDEO_ENG_STATE_IDLE:
        /* We shouldn't receive any callbacks when in the idle state - ignore
        ** any that happen to reach us during this time.
        */
        CAMCORDER_MSG_ERROR ("av_sync: in IDLE state", 0, 0, 0);
        break;

    case VIDEO_ENG_STATE_STARTING:
        /* Move engine to recording state. */
        g_context->state = VIDEO_ENG_STATE_RECORDING;

        /* Send a success status code back to our client. */
        g_context->callback_ptr
            (VIDEO_SUCCESS, g_context->client_data);
        break;

    case VIDEO_ENG_STATE_RECORDING:
        /* We shouldn't receive any callbacks when in the recording state -
        ** ignore any that happen to reach us during this time.
        */
        CAMCORDER_MSG_ERROR ("av_sync: in RECORDING state", 0, 0, 0);
        break;

    case VIDEO_ENG_STATE_PAUSING:
        /* Move engine to paused state. */
        g_context->state = VIDEO_ENG_STATE_PAUSED;

        /* Send a success status code back to our client. */
        g_context->callback_ptr
            (VIDEO_SUCCESS, g_context->client_data);
        break;

    case VIDEO_ENG_STATE_PAUSED:
        /* We shouldn't receive any callbacks when in the paused state -
        ** ignore any that happen to reach us during this time.
        */
        CAMCORDER_MSG_ERROR ("av_sync: in PAUSED state", 0, 0, 0);
        break;

    case VIDEO_ENG_STATE_RESUMING:
        /* Move engine to recording state. */
        g_context->state = VIDEO_ENG_STATE_RECORDING;

        /* Send a success status code back to our client. */
        g_context->callback_ptr
            (VIDEO_SUCCESS, g_context->client_data);
        break;

    case VIDEO_ENG_STATE_STOPPING:
        /* Perform file close operations & move engine to idle state */
        video_eng_handle_close (FALSE);
        g_context->state = VIDEO_ENG_STATE_IDLE;
        break;

    case VIDEO_ENG_STATE_ABORTING:
        /* Clean up and exit engine with failure status, but do not mess
        ** with the audio - it is already gone.
        */
        video_eng_enc_failure (FALSE);
        break;

    case VIDEO_ENG_STATE_EXITING:
        /* Finish exiting now that video/audio is shut down. */
        video_eng_handle_exit ();
        break;

    case VIDEO_ENG_STATE_INVALID:
    default:
        CAMCORDER_MSG_ERROR ("av_sync: "
                   "Illegal state %x", g_context->state, 0, 0);
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_video_processing

DESCRIPTION
  This function processes the video bitstream after it has been updated.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
video_status_type video_eng_video_processing (
  uint32   len,
  boolean  sync,
  int32    delta
) {
    uint32                      i;
    boolean                     b;
    video_fmt_sample_info_type  sample_info;
    uint32                      media_duration=0;
    uint64                      temp64 = 0;

    /* Unless running, ignore data - simply return. */
    if ((g_context->state != VIDEO_ENG_STATE_RECORDING &&
         g_context->state != VIDEO_ENG_STATE_PAUSING   &&
         g_context->state != VIDEO_ENG_STATE_PAUSED    &&
         g_context->state != VIDEO_ENG_STATE_RESUMING  &&
         g_context->state != VIDEO_ENG_STATE_STARTING  &&
         g_context->state != VIDEO_ENG_STATE_STOPPING) ||
        g_context->space_out)
    {
        return VIDEO_FAILURE;
    }

    /* If there was a last frame, process it instead.
    **
    ** NOTE: Use this frame's delta time for previous frame, because encoder
    **       returns t(n)-t(n-1) and the MP4 writer requires t(n+1)-t(n).
    */
    /*We can use this frame's delta as such when this function is called for post editing*/
    if(!g_context->no_coding)
    {
        if (g_context->video_prev_frame_size > 0)
        {
            i = len;
            len = g_context->video_prev_frame_size;
            g_context->video_prev_frame_size = i;
            b = sync;
            sync = g_context->video_prev_frame_sync;
            g_context->video_prev_frame_sync = b;
        }
        /* Otherwise, store frame size and sync for now, and
        ** return without processing it.
        **
        ** EXCEPTION: If in still_image mode, write out the frame now with a bogus
        ** timestamp, because since we know the timestamp will be altered
        ** afterwards anyway, and if we don't write the frame out now, we might
        ** not get a chance later if a movie file size limit is reached.
        */
        else if (!g_context->still_image)
        {
           g_context->video_prev_frame_size = len;
           g_context->video_prev_frame_sync = sync;
           return VIDEO_SUCCESS;
        }
        g_context->video_duration += (uint32)delta;
    }
    else
    {
        /*For post editing. we should start writing frames starting from first frame without 
        waiting for the next frames time increment.We should not increment video_duration with delta 
        starting from first frame, it should be zero for first frame and should increment starting 
        from second frame to reflect exact video_duration*/
        g_context->video_duration += g_context->video_prev_frame_delta;
        g_context->video_prev_frame_delta     = (uint32)delta;
    }
    /* Update video track length. */


    /* Update text stream, if present. */
    if ((g_context->text_type != VIDEO_ENG_TEXT_INPUT_NONE) &&
        (g_context->no_coding == FALSE))
    {
        uint32  duration_ms;

        /* Convert video track length to milliseconds. */
        duration_ms = g_context->video_duration * 1000
            / g_context->video_time_resolution;

        /* Push out any text samples that have time stamps before the current
        ** point in the video track.
        */
        while (g_context->text_samples)
        {
            video_eng_text_sample_type  *sample = g_context->text_samples;

            /* Break out of loop if next sample starts after current
            ** position.
            */
            if (sample->start > duration_ms)
            {
                break;
            }

            /* Write out text sample entry (header) atoms until the
            ** current one matches the index in the next sample.
            */
            while (g_context->text_current_header < sample->sample_entry)
            {
                video_eng_text_sample_entry_type *entry
                    = g_context->text_headers;

                /* Verify we have enough headers. */
                if (!entry)
                {
                    CAMCORDER_MSG_ERROR ("video_processing: "
                               "not enough text sample entries",
                               0, 0, 0);
                    video_eng_enc_failure (TRUE);
                    return VIDEO_FAILURE;
                }

                /* Write out the next sample entry to the movie file. */
                g_context->videofmt_write_header_text
                    (g_context->text_stream_num, &entry->entry,
                     g_context->videofmt_server_data,
                     video_eng_videofmt_callback, NULL);
                while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE)
                       && (g_context->videofmt_status
                           != VIDEO_FMT_FAILURE))
                {
                    if (g_context->videofmt_continue == NULL)
                    {
                        CAMCORDER_MSG_ERROR ("video_processing: bad continue "
                                   "function from videofmt!", 0, 0, 0);
                        break;
                    }
                    else
                    {
                        g_context->videofmt_continue
                            (g_context->videofmt_server_data);
                    }
                }
                if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
                {
                    g_context->videofmt_server_data = NULL;
                    CAMCORDER_MSG_ERROR ("video_processing: "
                               "failed to write text stream "
                               "sample entry", 0, 0, 0);
                    video_eng_enc_failure (TRUE);
                    return VIDEO_FAILURE;
                }

                /* Update sample entry index and free header just
                ** written to the movie.
                */
                ++g_context->text_current_header;
                g_context->text_headers = entry->next;
                if (!g_context->text_headers)
                {
                    g_context->text_headers_end = NULL;
                }
				// hence no interface is present for timed text, better not to free "entry->entry.fonts"
                /*
                if (entry->entry.fonts)
                {
                    videoeng_free ((void *) entry->entry.fonts);
                }
                */
                videoeng_free (entry);
            }


            /* Now write out the actual text sample. */
            g_context->videofmt_write_text
                (g_context->text_stream_num, sample->handle,
                 sample->end - sample->start,
                 g_context->videofmt_server_data,
                 video_eng_videofmt_callback, NULL);
            while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE)
                   && (g_context->videofmt_status != VIDEO_FMT_LIMIT_REACHED)
                   && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
            {
                if (g_context->videofmt_continue == NULL)
                {
                    CAMCORDER_MSG_ERROR ("video_processing: bad continue "
                               "function from videofmt!", 0, 0, 0);
                    break;
                }
                else
                {
                    g_context->videofmt_continue
                        (g_context->videofmt_server_data);
                }
            }
            if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
            {
                g_context->videofmt_server_data = NULL;
                CAMCORDER_MSG_ERROR ("video_processing: "
                           "failed to write text sample", 0, 0, 0);
                video_eng_enc_failure (TRUE);
                return VIDEO_FAILURE;
            }

            /* Remove sample from list and free it. */
            g_context->text_samples = sample->next;
            if (!g_context->text_samples)
            {
                g_context->text_samples_end = NULL;
            }
            videoeng_free (sample);
        }
    }

    /* Store frame length and sync flag in sample information structure we
    ** will pass to video format services.
    */
    sample_info.size = len;
    sample_info.sync = sync;

    // for recording
    //if(g_context->no_coding == FALSE)
    {
      /* The delta is converted to media_timescale resolution */
      temp64 = (uint64)g_context->video_duration;
      if(g_context->no_coding)
      {
          /*Here we have to increment with delta since the first frame that we are 
          going to write is when video_duration = 0 unlike recording where we start 
          writing first frame when we receive second. If we dont do this delta will become
           invalid for first postedited frame*/
          temp64 += (uint64)delta;
      }
      temp64 *= g_context->media_timescale;
      temp64 += ( g_context->video_time_resolution / 2 );
      temp64 /= g_context->video_time_resolution;
      media_duration = (uint32)temp64;
      sample_info.delta = media_duration - g_context->video_media_duration;
      g_context->video_media_duration = media_duration;
    }

    /* Write the frame to the movie, via video format services.  Once video
    ** format services is done, pull the data out of the stream, dropping it
    ** (since the video format services keeps its own buffer.
    */
    g_context->videofmt_write_stream (g_context->video_stream_num, 1, &sample_info,
                                      NULL,
                                      g_context->videofmt_server_data,
                                      video_eng_videofmt_callback, NULL);
    while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE)
           && (g_context->videofmt_status != VIDEO_FMT_LIMIT_REACHED)
           && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
    {
        if (g_context->videofmt_continue == NULL)
        {
            CAMCORDER_MSG_ERROR ("video_processing: bad continue "
                       "function from videofmt!", 0, 0, 0);
            break;
        }
        else
        {
            g_context->videofmt_continue (g_context->videofmt_server_data);
        }
    }
    if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
    {
        g_context->videofmt_server_data = NULL;
        CAMCORDER_MSG_ERROR ("video_processing: "
                   "failed to write frame", 0, 0, 0);
        video_eng_enc_failure (TRUE);
        return VIDEO_FAILURE;
    }
#ifdef FEATURE_VIDEO_CLIP_INFO
    if (g_context->videofmt_status == VIDEO_FMT_IO_DONE)
    {
        video_eng_framedrop_stat(FALSE);
    }
#endif

    /* If we just finished a fragment, reorder fragment. */
    if ((g_context->videofmt_fragment_done)
        && (g_context->file_brand == VIDEO_FMT_BRAND_FRAG_3G2))
    {
#ifdef FEATURE_EFS2
       struct fs_stat stat;
       int    stat_rsp = 0;
       if (g_context->output.method == VIDEO_METHOD_EFS)
       {
           stat_rsp = efs_stat(g_context->output.efs.filename,&stat);
       }
       if(stat_rsp)
       {       
           CAMCORDER_MSG_ERROR ("video_processing: "
                           "status failed %d", efs_errno, 0, 0);
           video_eng_enc_failure (TRUE);
           return VIDEO_FAILURE;         
       }
       else
#endif
       {
           (void) video_eng_reorder_fragment (FALSE);
       }
    }

    /* Send any limit status messages if necessary */
    video_eng_send_space_limit_status();
    return VIDEO_SUCCESS;
}

/*===========================================================================

FUNCTION video_eng_write_astream_processing

DESCRIPTION
  This function writes audio stream in a video file
  in the appropriate task.
  This is used in post editing recording

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_write_astream_processing (uint32 len, const uint8 *data)
{
    /* Verify state is appropriate for this command. */
    if (g_context->state != VIDEO_ENG_STATE_RECORDING)
    {
        CAMCORDER_MSG_ERROR (
                  "write_astream_processing: Engine not recording!",
                  0, 0, 0);
        g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
        return;
    }
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    // only support fixed rate audio currently
    video_eng_cmx_data_cb(0, len, NULL, data, g_context->client_data);

    if (g_context->no_coding)
    {
    // todo should return status from cmx_data_cb instead of VIDEO_SUCCESS
    g_context->callback_ptr (VIDEO_SUCCESS, g_context->client_data);
}
#else /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
#error code not present
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
}

/*===========================================================================

FUNCTION video_eng_write_tstream_processing

DESCRIPTION
  This function writes text stream in a video file
  in the appropriate task.
  This is used in post editing recording

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_write_tstream_processing (
  uint32 num,
  const video_fmt_sample_info_type *info,
  const uint8 *data)
{
    /* Verify state is appropriate for this command. */
    if (g_context->state != VIDEO_ENG_STATE_RECORDING)
    {
        CAMCORDER_MSG_ERROR (
                   "write_tstream_processing: Engine not recording!",
                   0, 0, 0);
        g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
        return;
    }

    /* Write the text to the movie, via video format services. */
    g_context->videofmt_write_stream ((uint32)g_context->text_stream_num,
                                      num,
                                      info,
                                      data,
                                      g_context->videofmt_server_data,
                                      video_eng_videofmt_callback,
                                      NULL);

    while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE) &&
           (g_context->videofmt_status != VIDEO_FMT_FAILURE))
    {
        if (g_context->videofmt_continue == NULL)
        {
            CAMCORDER_MSG_ERROR ("cmx_data_cb: bad continue "
              "function from videofmt!", 0, 0, 0);
            break;
        }
        else
        {
            g_context->videofmt_continue (g_context->videofmt_server_data);
        }
    }

    if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
    {
        g_context->videofmt_server_data = NULL;
        CAMCORDER_MSG_ERROR ("write_tstream_processing: "
                   "failed to write text", 0, 0, 0);
        video_eng_enc_failure (TRUE);
        return;
    }
    g_context->callback_ptr (VIDEO_SUCCESS, g_context->client_data);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_enc_normal_cb

DESCRIPTION
  This function handles normal status callbacks from the video encoder
  interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_enc_normal_cb (
  video_status_type  status,
  const void         *client_data
) {
    video_eng_packet_type  *packet;

    /* Send status back to the video engine for processing. */
    CAMCORDER_MSG_HIGH ("enc_normal_cb: status=%d", (uint32) status, 0, 0);
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_URGENT);
    if (packet == NULL)
    {
        CAMCORDER_MSG_ERROR ("cmx_normal_cb: out of pkts!", 0, 0, 0);
        return;
    }
    packet->hdr.cmd = VIDEO_ENG_CMD_ENC_STATUS;
    packet->enc_stat.status = status;
    video_eng_cmd (packet);
} /*lint !e715 'client_data' is not used */

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_enc_req_cb

DESCRIPTION
  This function handles data request callbacks from the video encoder
  interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_enc_req_cb (
  video_status_type                   status,
  const video_enc_enc_to_client_type  *enc_to_client_data,
  video_enc_client_to_enc_type        *client_to_enc_data,
  const void                          *client_data
) {
    video_eng_packet_type  *packet;
    static uint32          prev_buf_size = 0;
    static uint32          length = 0;
    uint32                 header_length;
    uint32 counter =0;
    uint32 cur_time = 0;
#ifdef DEBUG_AVSYNC
    uint32                 time_ms[2]={0,0};
#endif
#ifdef FEATURE_VIDEO_ENCODE_DIAG
#error code not present
#endif /* FEATURE_VIDEO_ENCODE_DIAG */

    if(g_context == NULL)
    {
      /* Flags inform video encoder that error has occured */
      client_to_enc_data->buf_size = 0;
      client_to_enc_data->buf_ptr = NULL;

      CAMCORDER_MSG_ERROR("Engine not running: g_context is NULL",0,0,0);
      return;
    }

    /* Branch according to status. */
    switch (status)
    {
    case VIDEO_SUCCESS:
    case VIDEO_FAILURE:
        /* Ignore the callback if already in the midst of exiting */
        if (g_context->state == VIDEO_ENG_STATE_EXITING)
        {
            /* Flags inform video encoder that error has occured */
            client_to_enc_data->buf_size = 0;
            client_to_enc_data->buf_ptr = NULL;
            return;
        }
        CAMCORDER_MSG_HIGH ("enc_req_cb: status=%d", (uint32) status, 0, 0);

        /* Pass this status back to the engine asynchronously. */
        packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_URGENT);
        if (packet == NULL)
        {
            CAMCORDER_MSG_ERROR ("cmx_normal_cb: out of pkts!", 0, 0, 0);
            return;
        }
        packet->hdr.cmd = VIDEO_ENG_CMD_ENC_STATUS;
        packet->enc_stat.status = status;
        video_eng_cmd (packet);
        break;

    case VIDEO_ABORTED:
        /* This event can be received in the middle of encoding one frame, but
        ** it indicates that the driver will stop immediately. Proceed to issue
        ** callback to upper layers to stop video engine gracefully */
        g_context->buffer_out = TRUE;
        g_context->callback_ptr (VIDEO_LIMIT_IMMINENT, g_context->client_data);
        break;

    case VIDEO_ERROR:
        /* This event should only be received at the end of encoding
        ** one frame. There should be no more error callbacks from lower
        ** layers, proceed to issue callback to upper layers */
        g_context->buffer_out = TRUE;
        g_context->callback_ptr (VIDEO_ERROR, g_context->client_data);
        break;

    case VIDEO_ENC_FRAME:
        CAMCORDER_MSG_HIGH ("enc_req_cb: status=VIDEO_ENC_FRAME, len=%d pf=%d d=%d",
                  enc_to_client_data->len, enc_to_client_data->type,
                  enc_to_client_data->delta);

        if(g_context->dynamic_bitrate_enable || 
           g_context->max_bitrate_enable)
        {
         counter = efs_fpending(g_context->fs_handle->object);
         MSG_HIGH("Buf size in enc_frame is %u", counter,0,0);
         cur_time = mp4e_get_time_ms();
         if(!g_context->reorder_atom)
            g_context->dynamic_bitrate = video_enc_get_bitrate(counter, cur_time);
        
        }

        /* Ignore the callback if already in the midst of exiting, or if
        ** still image mode is set and we already have an image.
        */
        if ((g_context->state == VIDEO_ENG_STATE_EXITING)
            || (g_context->still_image
                && (g_context->video_frames_received > 0)))
        {
            /* Flags inform video encoder that error has occured */
            client_to_enc_data->buf_size = 0;
            client_to_enc_data->buf_ptr = NULL;
            return;
        }
        ++g_context->video_frames_received;

#ifdef DEBUG_AVSYNC
        if(g_context->video_frames_received == 1)
        {
           clk_uptime_ms(time_ms);
           CAMCORDER_MSG_HIGH ("shdbg First video frame ready to be sent to videofmt at time = %d",
               (int)time_ms[0], 0, 0);
        }
#endif

        /* Update video track length. */
        g_context->av_sync_duration += (uint32) enc_to_client_data->delta;

        /* Check for video stream buffer overflow.  We add
        ** the size of the bitstream footer in case this ends up being the
        ** last frame.
        */
        if (enc_to_client_data->len > prev_buf_size)
        {
            CAMCORDER_MSG_ERROR ("enc_req_cb: VideoFmt buffer overflow!",
                       0, 0, 0);
            g_context->save_on_exit = TRUE;
            packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_CRITICAL);
            if (packet != NULL)
            {
               packet->hdr.cmd = VIDEO_ENG_CMD_FAILURE;
               packet->failure.stop_audio = TRUE;
               video_eng_cmd (packet);
            }
            else
            {
              CAMCORDER_MSG_ERROR ("enc_req_cb: out of pkts!", 0, 0, 0);
            }

            /* client_to_enc_data->buf_size is already 0 and
            ** client_to_enc_data->buf_ptr is already NULL */
            return;
        }
        client_to_enc_data->buf_ptr = g_context->videofmt_seek (g_context->video_stream_num,
                                          enc_to_client_data->len, FALSE,
                                          &client_to_enc_data->buf_size,
                                          g_context->videofmt_server_data,
                                          video_eng_videofmt_callback, NULL);
        if (!client_to_enc_data->buf_ptr)
        {
            /* If videofmt_seek has returned NULL, pass it along to video
            ** encoder. Video encoder will stop encoding. Video engine needs
            ** to stop recording, close the file and save it.
            */
            CAMCORDER_MSG_ERROR ("enc_req_cb: VideoFmt seek Error!", 0, 0, 0);
            g_context->buffer_out = TRUE;

            /* Flags inform video encoder that error has occured */
            client_to_enc_data->buf_size = 0;
            client_to_enc_data->buf_ptr = NULL;
            return;
        }
        client_to_enc_data->buf_size -= sizeof (M4V_BITSTREAM_FIXED_FOOTER);
        prev_buf_size = client_to_enc_data->buf_size;
        length += enc_to_client_data->len;

#ifdef FEATURE_VIDEO_ENCODE_DIAG
#error code not present
#endif /* FEATURE_VIDEO_ENCODE_DIAG */

        /* Process bitstream asynchronously. */
        packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_NORMAL);
        if (packet != NULL)
        {
            packet->hdr.cmd = VIDEO_ENG_CMD_PROCESS_VIDEO;
            packet->process.len = length;
            packet->process.sync = ((enc_to_client_data->type == I_FRAME)
                                    ? TRUE : FALSE);
            packet->process.delta = enc_to_client_data->delta;

            video_eng_cmd (packet);
        }
        else
        {
            g_context->save_on_exit = TRUE;
            CAMCORDER_MSG_ERROR ("enc_req_cb: out of pkts!", 0, 0, 0);
            return;
        }
        length = 0;
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
        if (g_context->framecb_ptr && g_context->pEncFrame) 
        {
            g_context->pEncFrame->delta = enc_to_client_data->delta;
            g_context->pEncFrame->len = enc_to_client_data->len;
            g_context->pEncFrame->type = enc_to_client_data->type;
            g_context->framecb_ptr(VIDEO_ENC_FRAME,g_context->pEncFrame,(void*)g_context->client_data);
        }
#endif//FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE        
        break;

    case VIDEO_ENC_PARTIAL_FRAME:
        CAMCORDER_MSG_HIGH ("enc_req_cb: status=VIDEO_ENC_PARTIAL_FRAME, "
                  "len=%d pf=%d d=%d",
                  enc_to_client_data->len, enc_to_client_data->type,
                  enc_to_client_data->delta);

        /* Ignore the callback if already in the midst of exiting, or if
        ** still image mode is set and we already have an image.
        */
        if ((g_context->state == VIDEO_ENG_STATE_EXITING)
            || (g_context->still_image
                && (g_context->video_frames_received > 0)))
        {
            /* Flags inform video encoder that error has occured */
            client_to_enc_data->buf_size = 0;
            client_to_enc_data->buf_ptr = NULL;
            return;
        }

        /* Check for video stream buffer overflow.  We add
        ** the size of the bitstream footer in case this ends up being the
        ** last frame.
        */
        if (enc_to_client_data->len > prev_buf_size)
        {
            CAMCORDER_MSG_ERROR ("enc_req_cb: VideoFmt buffer overflow!",
                       0, 0, 0);
            g_context->save_on_exit = TRUE;
            packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_CRITICAL);
            if (packet != NULL)
            {
              packet->hdr.cmd = VIDEO_ENG_CMD_FAILURE;
              packet->failure.stop_audio = TRUE;
              video_eng_cmd (packet);
            }
            else
            {
              CAMCORDER_MSG_ERROR ("enc_req_cb: out of pkts!", 0, 0, 0);
            }

            /* Flags inform video encoder that error has occured */
            client_to_enc_data->buf_size = 0;
            client_to_enc_data->buf_ptr = NULL;
            return;
        }
        client_to_enc_data->buf_ptr = g_context->videofmt_seek (g_context->video_stream_num,
                                          enc_to_client_data->len, TRUE,
                                          &client_to_enc_data->buf_size,
                                          g_context->videofmt_server_data,
                                          video_eng_videofmt_callback, NULL);
        if (!client_to_enc_data->buf_ptr)
        {
            /* If videofmt_seek has returned NULL, pass it along to video
            ** encoder. Video encoder will stop encoding. Video engine needs
            ** to stop recording, close the file and save it.
            */
            CAMCORDER_MSG_ERROR ("enc_req_cb: VideoFmt seek Error!", 0, 0, 0);
            g_context->buffer_out = TRUE;

            /* Flags inform video encoder that error has occured */
            client_to_enc_data->buf_size = 0;
            client_to_enc_data->buf_ptr = NULL;
            return;
        }
        client_to_enc_data->buf_size -= sizeof (M4V_BITSTREAM_FIXED_FOOTER);
        prev_buf_size = client_to_enc_data->buf_size;
        length += enc_to_client_data->len;
        break;

    case VIDEO_ENC_HEADER:
        CAMCORDER_MSG_HIGH ("enc_req_cb: status=VIDEO_ENC_HEADER, len=%d pf=%d d=%d",
                  enc_to_client_data->len, enc_to_client_data->type,
                  enc_to_client_data->delta);

        /* Ignore the callback if NOT in the midst of starting. */
        if ((g_context->state != VIDEO_ENG_STATE_STARTING)
            && (g_context->state != VIDEO_ENG_STATE_RESUMING))
        {
            return;
        }

        /* "length" needs to be initialized here. Otherwise it keeps old
        ** value for the previous recording.
        */
        length = 0;

        client_to_enc_data->buf_ptr = g_context->videofmt_seek (g_context->video_stream_num, 0, FALSE,
                                          &client_to_enc_data->buf_size,
                                          g_context->videofmt_server_data,
                                          video_eng_videofmt_callback, NULL);
        if (!client_to_enc_data->buf_ptr)
        {
            /* If videofmt_seek has returned NULL as soon as video encoder
            ** returned VIDEO_ENC_HEADER, there is no video frame to save.
            ** Terminate everything.
            */
            CAMCORDER_MSG_ERROR ("enc_req_cb: VideoFmt seek Error!", 0, 0, 0);
            g_context->save_on_exit = TRUE;
            packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_CRITICAL);
            if (packet != NULL)
            {
              packet->hdr.cmd = VIDEO_ENG_CMD_FAILURE;
              packet->failure.stop_audio = TRUE;
              video_eng_cmd (packet);
            }
            else
            {
              CAMCORDER_MSG_ERROR ("enc_req_cb: out of pkts!", 0, 0, 0);
            }

            /* client_to_enc_data->buf_ptr already NULL but reset it anyway */
            client_to_enc_data->buf_size = 0;
            client_to_enc_data->buf_ptr = NULL;
            return;
        }
        client_to_enc_data->buf_size -= sizeof (M4V_BITSTREAM_FIXED_FOOTER);
        prev_buf_size = client_to_enc_data->buf_size;


        /* Break if we came here while resuming the recording. */
        if (g_context->state == VIDEO_ENG_STATE_RESUMING)
        {
            return;
        }

        /*
        ** Process bitstream header synchronously.
        */

        /* enc_to_client_data->len == 0 in the short header mode (H.263). */
        header_length = enc_to_client_data->len;
        if ((g_context->video_format == VIDEO_FMT_STREAM_VIDEO_MPEG4) &&
            (g_context->no_coding == FALSE))
        {
            header_length += sizeof (M4V_BITSTREAM_FIXED_HEADER);
        }

        /* Generate an error if the bitstream header is too big to fit in
        ** our buffer.
        */
        if (header_length > VIDEO_ENG_VSTREAM_HEADER_MAX)
        {
            CAMCORDER_MSG_ERROR ("video_processing: bitstream header "
                       "exceeds size allotted (%d > %d)",
                       header_length, VIDEO_ENG_VSTREAM_HEADER_MAX, 0);
            packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_CRITICAL);
            if (packet != NULL)
            {
               packet->hdr.cmd = VIDEO_ENG_CMD_FAILURE;
               packet->failure.stop_audio = TRUE;
               video_eng_cmd (packet);
            }
            else
            {
               CAMCORDER_MSG_ERROR ("enc_req_cb: out of pkts!", 0, 0, 0);
            }

            /* Flags inform video encoder that error has occured */
            client_to_enc_data->buf_size = 0;
            client_to_enc_data->buf_ptr = NULL;
            return;
        }

        /* Write the header to the movie file. */
        g_context->videofmt_write_header
            (g_context->video_stream_num, header_length,
             g_context->ved_buf_ptr,
             g_context->videofmt_server_data,
             video_eng_videofmt_callback, NULL);
        while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE)
               && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
        {
            if (g_context->videofmt_continue == NULL)
            {
                CAMCORDER_MSG_ERROR ("write_processing: bad continue "
                           "function from videofmt!", 0, 0, 0);
                break;
            }
            else
            {
                g_context->videofmt_continue (g_context->videofmt_server_data);
            }
        }
        if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
        {
            g_context->videofmt_server_data = NULL;
            CAMCORDER_MSG_ERROR ("video_processing: "
                       "failed to write stream header", 0, 0, 0);
            packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_CRITICAL);
            if (packet != NULL)
            {
              packet->hdr.cmd = VIDEO_ENG_CMD_FAILURE;
              packet->failure.stop_audio = TRUE;
              video_eng_cmd (packet);
            }
            else
            {
              CAMCORDER_MSG_ERROR ("enc_req_cb: out of pkts!", 0, 0, 0);
            }

            /* Flags inform video encoder that error has occured */
            client_to_enc_data->buf_size = 0;
            client_to_enc_data->buf_ptr = NULL;
            return;
        }

        /* Unless the video repeat header flag is set, exclude the decoder
        ** specific information from the actual bitstream.
        */
        if (g_context->video_repeat_header)
        {
            /* Write header into the Videofmt buffer. */
            memcpy (client_to_enc_data->buf_ptr, g_context->ved_buf_ptr,
                    header_length);

            /* Notify Videofmt of the offset */
            client_to_enc_data->buf_ptr = g_context->videofmt_seek (g_context->video_stream_num,
                                            header_length, FALSE,
                                            &client_to_enc_data->buf_size,
                                            g_context->videofmt_server_data,
                                            video_eng_videofmt_callback, NULL);
            length = header_length;
        }
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
        if (g_context->framecb_ptr) 
        {
            g_context->framecb_ptr(VIDEO_ENC_HEADER,g_context->pEncFrame,(void*)g_context->client_data);
        }
#endif//#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
        break;

    default:
        CAMCORDER_MSG_ERROR ("enc_req_cb: Illegal video "
                   "encoder status %x", status, 0, 0);
    }
} /*lint !e715 'client_data' is not used */

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmx_normal_cb

DESCRIPTION
  This function handles status callbacks from the CMX interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_cmx_normal_cb (
  cmx_status_type  status,
  const void       *client_data
) {
    video_eng_packet_type  *packet;

    /* Verify that the engine is running. */
    if (g_context == NULL)
    {
        CAMCORDER_MSG_HIGH ("cmx_normal_cb: Video encode engine is not running!",
                    0, 0, 0);
        return;
    }

    /* Send status back to the video engine for processing. */
    CAMCORDER_MSG_HIGH ("cmx_normal_cb: status=%d", (uint32) status, 0, 0);
    packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_URGENT);
    if (packet == NULL)
    {
        CAMCORDER_MSG_ERROR ("cmx_normal_cb: out of pkts!", 0, 0, 0);
        return;
    }
    packet->hdr.cmd = VIDEO_ENG_CMD_CMX_STATUS;
    packet->cmx_stat.status = status;
    video_eng_cmd (packet);
} /*lint !e715 'client_data' is not used */

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmx_data_cb

DESCRIPTION
  This function handles data request callbacks from the CMX interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_cmx_data_cb (
  uint32                           position,
  uint32                           max_len,
  cmx_client_req_cb_func_ptr_type  data_req_cb,
  const void                       *server_client_data,
  const void                       *client_data
) {
    const uint8  *pkts = (const uint8 *) server_client_data;
    uint32       len = max_len;
    uint32       fullSampleSize;
    avSync_type  *avs;
    uint32       video_total_time;
    uint32       audio_optimal_num_pkts;
    uint64       audio_less;
    uint64       temp32;
    uint64       temp64;
#ifdef DEBUG_AVSYNC
    uint32 time_ms[2]={0,0};
#endif
    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR ("cmx_data_cb: exiting, no engine context", 0, 0, 0);
        if (data_req_cb)
        {
            CAMCORDER_MSG_HIGH ("cmx_data_cb: cb(CMX_SUCCESS)", 0, 0, 0);
            data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data, max_len,
                         (uint8 *) server_client_data);
        }
        return;
    }
    avs = &(g_context->avSync_info);
    fullSampleSize = g_context->audio_frames_per_sample
                                  * g_context->audio_sample_size;
    if(!fullSampleSize || 
       !g_context->audio_sample_size || 
       !g_context->audio_sample_delta
       )
    {
        CAMCORDER_MSG_ERROR ("Video engine not initialized properly for audio transcoding", 0, 0, 0);
        if (data_req_cb)
        {
            CAMCORDER_MSG_HIGH ("cmx_data_cb: cb(CMX_SUCCESS)", 0, 0, 0);
            data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data, max_len,
                         (uint8 *) server_client_data);
        }
        return;
    }
    /* If not recording, or we lost the video format services, ignore data -
    ** simply give it back to CMX and return.
    */
    if ((g_context->state != VIDEO_ENG_STATE_RECORDING &&
         g_context->state != VIDEO_ENG_STATE_STARTING  &&
         g_context->state != VIDEO_ENG_STATE_PAUSING   &&
         g_context->state != VIDEO_ENG_STATE_RESUMING  &&
         g_context->state != VIDEO_ENG_STATE_STOPPING) ||
        g_context->videofmt_server_data == NULL        ||
        g_context->space_out)
    {
        CAMCORDER_MSG_HIGH ("cmx_data_cb: exiting, movie limit reached, or no "
                            "writer context", 0, 0, 0);
        if (data_req_cb)
        {
            CAMCORDER_MSG_HIGH ("cmx_data_cb: cb(CMX_SUCCESS)", 0, 0, 0);
            data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data, max_len,
                         (uint8 *) server_client_data);
        }
        return;
    }

    /* Ignore writes to QCP (or AMR) header/footer portions.  NOTE: This
    ** depends on the fact that CMX does not write any portion of the
    ** header/footer with any portion of the packet data in the same write
    ** call.  If it ever does, this will break!
    */
    if ((g_context->no_coding == FALSE) &&
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
        (g_context->framecb_ptr == NULL) &&
#endif //#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
         (((g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_AMR) &&
          (position < AMR_FILE_HEADER_SIZE)) ||
          ((g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_AMR) &&
           ((position < CMX_QCP_HEADER_SIZE) ||
            (len == 1)))))
    {
        /* Release audio buffer back to CMX. */
        CAMCORDER_MSG_HIGH ("cmx_data_cb: header/footer - ignoring", 0, 0, 0);
        if (data_req_cb)
        {
            data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data,
                         max_len, (uint8 *) server_client_data);
        }
        return;
    }

    CAMCORDER_MSG_HIGH ("cmx_data_cb: pos=%d, len=%d", position, max_len, 0);
#ifdef DEBUG_AVSYNC
    clk_uptime_ms(time_ms);
    CAMCORDER_MSG_HIGH ("cmx_data_cb_timelog = %d", time_ms[0], 0, 0);
#endif
    if (!avs->first_audio_cb_received)
    {
       avs->first_audio_cb_received = TRUE;
       if (g_context->audio_data1_cb)
       {
          //calls camera_audio_ready_cb, sync = video_eng_cmx_sync_cb_func
          g_context->audio_data1_cb ();
       }
    }

    /* Drop first packet. */
    if (!g_context->first_audio_packet_dropped)
    {
        g_context->first_audio_packet_dropped = TRUE;

        /* VFE Config Delay Handling Anjustment */

        // Add to packets to skip based on new delay
        if(glb_avs_info.timelog_video_start >=
              (glb_avs_info.timelog_audio_start - avs->audio_sync_error_adjust))
        {
           // Unit is in (msec * bpf) where bpf = bytes per audio frame
           avs->vfe_config_delay = (glb_avs_info.timelog_video_start -
                          (glb_avs_info.timelog_audio_start - avs->audio_sync_error_adjust))
                          * g_context->audio_sample_size;
           avs->skip_audio_sum_ms_prev = avs->skip_audio_sum_ms;
           avs->skip_audio_sum_ms += len*20;
           /* if already dropped enough frames */
           if(avs->vfe_config_delay <= avs->skip_audio_sum_ms_prev)
           {
#ifdef DEBUG_AVSYNC
              CAMCORDER_MSG_HIGH ("shdbg ALREADY SKIPPED more-sufficient data- delay was %d, skipped delay %d",
                                  avs->vfe_config_delay,avs->skip_audio_sum_ms_prev,0);
#endif
           }
           else if((avs->vfe_config_delay > avs->skip_audio_sum_ms_prev)
              && (avs->vfe_config_delay < avs->skip_audio_sum_ms))  // last skip reached
            {
              avs->last_skip_ms = avs->vfe_config_delay - avs->skip_audio_sum_ms_prev;
              temp64 = (uint64) avs->last_skip_ms * g_context->audio_time_resolution;
              temp64 /= g_context->audio_sample_delta;
              avs->skip_audio_pkts = (uint32)temp64/1000;
              // make multiple bpf by truncate
              avs->skip_audio_pkts = (avs->skip_audio_pkts / g_context->audio_sample_size)
                                          * g_context->audio_sample_size;
              pkts += avs->skip_audio_pkts;                       // skip for last skip audio
              len -= avs->skip_audio_pkts;
#ifdef DEBUG_AVSYNC
              CAMCORDER_MSG_HIGH ("shdbg SKIP LAST, worth = %d msec, Video_start = %d, Audio_start = %d",
                 (int)((avs->skip_audio_pkts)*20/g_context->audio_sample_size),
                 (int)glb_avs_info.timelog_video_start, (int)glb_avs_info.timelog_audio_start);
#endif
            }
           else
           {
              // to enable skip until video config done happens
              g_context->first_audio_packet_dropped = FALSE;
#ifdef DEBUG_AVSYNC
              CAMCORDER_MSG_HIGH ("shdbg SKIP ALL, Video started, worth = %d msec,Video_start = %d, Audio_start = %d",
                 (int)(len*20/g_context->audio_sample_size),
                 (int)glb_avs_info.timelog_video_start,
                 (int)glb_avs_info.timelog_audio_start);
#endif
              pkts += len;                                            // skip all audio
              len = 0;
            }

        }
        else if(glb_avs_info.timelog_video_start == 0) 
         /* Video start is not done, thus skip all audio */
        {
           avs->skip_audio_sum_ms += len * 20;
           // to enable skip until video start happens
           g_context->first_audio_packet_dropped = FALSE;
#ifdef DEBUG_AVSYNC
           CAMCORDER_MSG_HIGH ("shdbg SKIP ALL, Video not started, worth = %d msec, Video_start = %d, Audio_start = %d",
              (int)(len*20/g_context->audio_sample_size),
              (int)glb_avs_info.timelog_video_start,
              (int)glb_avs_info.timelog_audio_start);
#endif
           pkts += len;                                            // skip all audio
           len = 0;
        }

    }

    /* Get the optimal number of audio packets whose lengh in time is equal
    ** to the video length.  Take the floor in order for the audio stream
    ** to be not longer than the video stream.
    */
    if (((g_context->state == VIDEO_ENG_STATE_PAUSING)
         || (g_context->state == VIDEO_ENG_STATE_STOPPING)) &&
             g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
    {
        /* Get video total time in [ms] */
        temp64 = (uint64) g_context->av_sync_duration;
        temp64 *= 1000;
        temp64 += (g_context->video_time_resolution >> 1);
        temp64 /= g_context->video_time_resolution;
        video_total_time = (uint32) temp64;
        video_total_time += (g_context->frame_drop_info.frame_drop_duration +
               g_context->frame_drop_info.frame_drop_duration_before_pause);

        /* Get optimal number of audio packets. */
        temp64 = (uint64) video_total_time;
        temp64 *= g_context->audio_time_resolution;
        temp64 /= g_context->audio_sample_delta;
        temp64 /= 1000;
        audio_optimal_num_pkts = (uint32) temp64;

        CAMCORDER_MSG_HIGH("video[ms]= %d, audio[pkts]= %d, audio optimal[pkts]= %d",
                  video_total_time, g_context->audio_num_packets,
                  audio_optimal_num_pkts);

        /* If audio stream is longer than the optimal audio size, truncate
        ** the audio stream.
        */
        temp32 = len / g_context->audio_sample_size;
        if (g_context->audio_num_packets + temp32 > audio_optimal_num_pkts)
        {
            /* audio_less in bytes */
            audio_less = g_context->audio_sample_size
                       * MIN (g_context->audio_num_packets + temp32
                              - audio_optimal_num_pkts, temp32);
            len -= (uint32)audio_less;
            CAMCORDER_MSG_HIGH("audio trimming %d bytes to %d bytes",
                      audio_less, len, 0);
        }
    }

    /* Repeat until all audio frames written. */
    while (len)
    {
        const uint8  *pktsOut = NULL;
        uint32       numSamples = 0;

        /* If there is left-over data in the audio buffer, append frames to
        ** it and write from it.
        */
        if (g_context->audio_leftover_bytes)
        {
            /* Append frames to left-over buffer. */
            const uint32 appendBytes = MIN
                (len, fullSampleSize - g_context->audio_leftover_bytes);
            memcpy (g_context->audio_leftover
                    + g_context->audio_leftover_bytes,
                    pkts, appendBytes);
            g_context->audio_leftover_bytes += appendBytes;
            pkts += appendBytes;
            len -= appendBytes;

            /* If the left-over buffer is now full, output it. */
            if (g_context->audio_leftover_bytes == fullSampleSize)
            {
                pktsOut = g_context->audio_leftover;
                g_context->audio_leftover_bytes = 0;
                numSamples = 1;
            }
        }

        /* Otherwise, take data from input, if there is at least one sample. */
        else if (len >= fullSampleSize)
        {
            numSamples = MIN (MAX_AUDIO_SAMPLES_PER_WRITE,
                              len / fullSampleSize);
            pktsOut = pkts;
            pkts += numSamples * fullSampleSize;
            len -= numSamples * fullSampleSize;
        }

        /* Break out early if there are no more samples to write. */
        if (numSamples == 0)
        {
            /* Save any left-over frames. */
            if (len && g_context->audio_leftover)
            {
                memcpy (g_context->audio_leftover, pkts, len);
                g_context->audio_leftover_bytes = len;
            }
            break;
        }

        /* If writing more than one frame per sample, we must put valid
        ** size information in the sample information structures.
        */
        if (g_context->audio_frames_per_sample > 1)
        {
            uint32 i;
            for (i = 0; i < numSamples; ++i)
            {
                g_context->audio_sample_info [i].size = fullSampleSize;
                g_context->audio_sample_info [i].delta
                    = (VOCODER_SAMPLING_RATE / VOCODER_PACKET_RATE)
                    * g_context->audio_frames_per_sample;
                g_context->audio_sample_info [i].sync = 1;
            }
        }

        g_context->audio_num_packets +=
            (numSamples * g_context->audio_frames_per_sample);

        /* Write the audio to the movie, via video format services. */
        g_context->videofmt_write_stream
            (g_context->audio_stream_num, numSamples,
             &g_context->audio_sample_info [0], pktsOut,
             g_context->videofmt_server_data,
             video_eng_videofmt_callback, NULL);
        while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE)
               && (g_context->videofmt_status != VIDEO_FMT_LIMIT_REACHED)
               && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
        {
            if (g_context->videofmt_continue == NULL)
            {
                CAMCORDER_MSG_ERROR ("cmx_data_cb: bad continue "
                           "function from videofmt!", 0, 0, 0);
                break;
            }
            else
            {
                g_context->videofmt_continue (g_context->videofmt_server_data);
            }
        }
        if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
        {
            break;
        }

        /* If we are low on space, pass along notification to the client. */
        video_eng_send_space_limit_status();
    }

    CAMCORDER_MSG_HIGH ("cmx_data_cb: audio_num_packets = %d",
                        g_context->audio_num_packets, 0, 0);

    /* Release audio buffer back to CMX. */
    if (data_req_cb)
    {
        CAMCORDER_MSG_HIGH ("cmx_data_cb: cb(CMX_SUCCESS)", 0, 0, 0);
        data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data, max_len,
                     (uint8 *) server_client_data);
    }

    /* Handle any errors from video format services. */
    if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
    {
        g_context->videofmt_server_data = NULL;
        CAMCORDER_MSG_ERROR ("cmx_data_cb: "
                   "failed to write audio", 0, 0, 0);
        g_context->save_on_exit = TRUE;
        video_eng_enc_failure (TRUE);
        return;
    }

	/* If we just finished a fragment, reorder fragment.
		Check is done for the case when only audio is present*/
    if ((g_context->videofmt_fragment_done)
        && (g_context->file_brand == VIDEO_FMT_BRAND_FRAG_3G2)
		&& (g_context->video_format == VIDEO_FMT_STREAM_VIDEO_NONE))
    {
       (void)video_eng_reorder_fragment (FALSE);
    }

} /*lint !e715 'client_data' is not used */

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmx_var_data_cb

DESCRIPTION
  This function handles data request callbacks from the CMX interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_cmx_var_data_cb (
  uint32                           position,
  uint32                           max_len,
  cmx_client_req_cb_func_ptr_type  data_req_cb,
  const void                       *server_client_data,
  const void                       *client_data
) {
    const uint8  *pkts = (const uint8 *) server_client_data;
    const uint8  *pkt_len_table;
    uint32       pkt_len_table_size;
    uint32       pkt_len;
    uint32       len = max_len;
    uint32       video_total_time;
    uint32       audio_optimal_num_pkts = 0;
    uint64       temp64;

    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR ("cmx_var_data_cb: exiting, no engine context", 0, 0, 0);
        if (data_req_cb)
        {
            CAMCORDER_MSG_HIGH ("cmx_var_data_cb: cb(CMX_SUCCESS)", 0, 0, 0);
            data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data, max_len,
                         (uint8 *) server_client_data);
        }
        return;
    }

    /* If not recording, or we lost the video format services, ignore data -
    ** simply give it back to CMX and return.
    */
    if ((g_context->state != VIDEO_ENG_STATE_RECORDING &&
         g_context->state != VIDEO_ENG_STATE_STARTING  &&
         g_context->state != VIDEO_ENG_STATE_PAUSING   &&
         g_context->state != VIDEO_ENG_STATE_RESUMING  &&
         g_context->state != VIDEO_ENG_STATE_STOPPING) ||
        g_context->videofmt_server_data == NULL        ||
        g_context->space_out)
    {
        CAMCORDER_MSG_HIGH ("cmx_var_data_cb: exiting, movie limit reached, or no "
                  "writer context", 0, 0, 0);
        if (data_req_cb)
        {
            CAMCORDER_MSG_HIGH ("cmx_var_data_cb: cb(CMX_SUCCESS)", 0, 0, 0);
            data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data, max_len,
                         (uint8 *) server_client_data);
        }
        return;
    }

    switch (g_context->audio_format)
    {
    case VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL:
    case VIDEO_FMT_STREAM_AUDIO_QCELP13K_HALF:
        pkt_len_table = &QCP_PACKET_SIZE_13K [0];
        pkt_len_table_size = sizeof (QCP_PACKET_SIZE_13K)
                           / sizeof (QCP_PACKET_SIZE_13K [0]);
        break;

    case VIDEO_FMT_STREAM_AUDIO_EVRC:
        pkt_len_table = &QCP_PACKET_SIZE_EVRC [0];
        pkt_len_table_size = sizeof (QCP_PACKET_SIZE_EVRC)
                           / sizeof (QCP_PACKET_SIZE_EVRC [0]);
        break;

    default:
        CAMCORDER_MSG_HIGH ("cmx_var_data_cb: exiting, audio format not supported",
                  0, 0, 0);
        if (data_req_cb)
        {
            CAMCORDER_MSG_HIGH ("cmx_var_data_cb: cb(CMX_SUCCESS)", 0, 0, 0);
            data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data, max_len,
                         (uint8 *) server_client_data);
        }
        return;
    }

    /* Ignore writes to QCP (or AMR) header/footer portions.  NOTE: This
    ** depends on the fact that CMX does not write any portion of the
    ** header/footer with any portion of the packet data in the same write
    ** call.  If it ever does, this will break!
    */
    if ((position < CMX_QCP_HEADER_SIZE) || (len == 1))
    {
        /* Release audio buffer back to CMX. */
        CAMCORDER_MSG_HIGH ("cmx_var_data_cb: header/footer - ignoring", 0, 0, 0);
        if (data_req_cb)
        {
            data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data,
                         max_len, (uint8 *) server_client_data);
        }
        return;
    }

    CAMCORDER_MSG_HIGH ("cmx_var_data_cb: pos=%d, len=%d", position, max_len, 0);

    /* Drop first packet. */
    if (!g_context->first_audio_packet_dropped)
    {
        /* Determine size of packet based on vocoder and rate. */
        uint32 skip = 0;
        g_context->first_audio_packet_dropped = TRUE;

        skip = pkt_len_table [((const uint8 *) server_client_data) [0]];

        /* Skip past packet in input. */
        pkts += skip;
        len -= skip;
    }

    /* Get the optimal number of audio packets whose lengh in time is equal
    ** to the video length.  Take the floor in order for the audio stream
    ** to be not longer than the video stream.
    */
    if ((g_context->state == VIDEO_ENG_STATE_PAUSING)
        || (g_context->state == VIDEO_ENG_STATE_STOPPING))
    {
        /* Get video total time in [ms] */
        temp64 = (uint64) g_context->av_sync_duration;
        temp64 *= 1000;
        temp64 += (g_context->video_time_resolution >> 1);
        temp64 /= g_context->video_time_resolution;
        video_total_time = (uint32) temp64;

        temp64 = (uint64) video_total_time;
        temp64 *= g_context->audio_time_resolution;
        temp64 /= g_context->audio_sample_delta;
        temp64 /= 1000;
        audio_optimal_num_pkts = (uint32) temp64;

        CAMCORDER_MSG_HIGH("video[ms]= %d, audio[pkts]= %d, audio optimal[pkts]= %d",
                  video_total_time, g_context->audio_num_packets,
                  audio_optimal_num_pkts);
    }

    /* Repeat until all audio frames written. */
    while (len)
    {
        const uint8  *pktsOut = NULL;
        uint32       numSamples = 0;

        /* if audio_optimal_num_pkts is > 0 and the total number of
        ** packets becomes greater than that, stop.
        */
        if ((audio_optimal_num_pkts > 0)
            && (g_context->audio_num_packets + numSamples
                >= audio_optimal_num_pkts))
        {
            CAMCORDER_MSG_HIGH ("audio trimming %d bytes", len, 0,0);
            CAMCORDER_MSG_HIGH ("audio_optimal_num_pkts = %d, audio_num_packets = %d, "
                      "numSamples = %d",
                      audio_optimal_num_pkts, g_context->audio_num_packets,
                      numSamples);
            break;
        }

        //CAMCORDER_MSG_HIGH ("cmx_var_data_cb: audio pkt idx %d -> len %d, left: %d",
        //          pkts [0], pkt_len_table [pkts [0]], len);
        //CAMCORDER_MSG_HIGH ("audio_optimal_num_pkts = %d, audio_num_packets = %d, "
        //          "numSamples = %d",
        //          audio_optimal_num_pkts, g_context->audio_num_packets,
        //          numSamples);

        pktsOut = pkts;
        if ((pkts [0] >= pkt_len_table_size)
            || (pkt_len_table [pkts [0]] > len))
        {
            /* Release audio buffer back to CMX. */
            if (data_req_cb)
            {
                data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data,
                            max_len, (uint8 *) server_client_data);
            }
            CAMCORDER_MSG_ERROR ("cmx_var_data_cb: bad frame length!", 0, 0, 0);
            return;
        }

        pkt_len = pkt_len_table [pkts [0]];
        pkts += pkt_len;
        len -= pkt_len;

        g_context->audio_sample_info [0].size = pkt_len;
        g_context->audio_sample_info [0].delta
            = VOCODER_SAMPLING_RATE / VOCODER_PACKET_RATE;
        g_context->audio_sample_info [0].sync = 1;

        numSamples = 1;

        /* Add all the audio frames until all audio data is consumed or the
        ** optimal number of audio packets is reached.
        */
        for ( ; numSamples < MAX_AUDIO_SAMPLES_PER_WRITE; ++numSamples)
        {
            if (len == 0)
            {
                break;
            }
            /* if audio_optimal_num_pkts is > 0 and the total number of
            ** packets becomes greater than that, stop.
            */
            if ((audio_optimal_num_pkts > 0)
                && (g_context->audio_num_packets + numSamples
                    >= audio_optimal_num_pkts))
            {
                CAMCORDER_MSG_HIGH ("audio trimming %d bytes", len, 0,0);
                CAMCORDER_MSG_HIGH ("audio_optimal_num_pkts = %d, "
                          "audio_num_packets = %d, numSamples = %d",
                          audio_optimal_num_pkts,
                          g_context->audio_num_packets, numSamples);
                break;
            }
            if ((pkts [0] >= pkt_len_table_size)
                || (pkt_len_table [pkts [0]] > len))
            {
                /* Release audio buffer back to CMX. */
                if (data_req_cb)
                {
                    data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data,
                                max_len, (uint8 *) server_client_data);
                }
                CAMCORDER_MSG_ERROR ("cmx_var_data_cb: bad frame length!", 0, 0, 0);
                return;
            }

            //CAMCORDER_MSG_HIGH ("cmx_var_data_cb: * adding pkt idx %d -> len %d, "
            //          "left: %d",
            //          pkts [0], pkt_len_table [pkts [0]], len);
            //CAMCORDER_MSG_HIGH ("audio_optimal_num_pkts = %d, audio_num_packets = %d, "
            //          "numSamples = %d",
            //          audio_optimal_num_pkts, g_context->audio_num_packets,
            //          numSamples);

            pkt_len = pkt_len_table [pkts [0]];
            pkts += pkt_len;
            len -= pkt_len;

            g_context->audio_sample_info [numSamples].size = pkt_len;
            g_context->audio_sample_info [numSamples].delta
                = VOCODER_SAMPLING_RATE / VOCODER_PACKET_RATE;
            g_context->audio_sample_info [numSamples].sync = 1;
        }

        g_context->audio_num_packets += numSamples;

        /* Write the audio to the movie, via video format services. */
        g_context->videofmt_write_stream
            (g_context->audio_stream_num, numSamples,
             &g_context->audio_sample_info [0], pktsOut,
             g_context->videofmt_server_data,
             video_eng_videofmt_callback, NULL);
        while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE)
               && (g_context->videofmt_status != VIDEO_FMT_LIMIT_REACHED)
               && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
        {
            if (g_context->videofmt_continue == NULL)
            {
                CAMCORDER_MSG_ERROR ("cmx_var_data_cb: bad continue "
                           "function from videofmt!", 0, 0, 0);
                break;
            }
            else
            {
                g_context->videofmt_continue (g_context->videofmt_server_data);
            }
        }
        if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
        {
            break;
        }

        /* If we are low on space, pass along notification to the client. */
        video_eng_send_space_limit_status();
    }

    CAMCORDER_MSG_HIGH ("cmx_var_data_cb: audio_num_packets = %d",
              g_context->audio_num_packets, 0, 0);

    /* Release audio buffer back to CMX. */
    if (data_req_cb)
    {
        CAMCORDER_MSG_HIGH ("cmx_var_data_cb: cb(CMX_SUCCESS)", 0, 0, 0);
        data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data, max_len,
                     (uint8 *) server_client_data);
    }

    /* Handle any errors from video format services. */
    if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
    {
        g_context->videofmt_server_data = NULL;
        CAMCORDER_MSG_ERROR ("cmx_var_data_cb: "
                   "failed to write audio", 0, 0, 0);
        g_context->save_on_exit = TRUE;
        video_eng_enc_failure (TRUE);
        return;
    }
} /*lint !e715 'client_data' is not used */


/*===========================================================================

FUNCTION video_eng_cmx_sync_cb_func

DESCRIPTION
  This function handles sync callbacks from the CMX interface for
  A/V Sync purpose.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_cmx_sync_cb_func(
  cmx_av_report_type *av_report,
  const void         *client_data
)
{
   /* Assumption: No race condition between CMX sync callback
   ** and data callback */
   if(!glb_avs_info.timelog_audio_start)
   {
#ifdef DEBUG_AVSYNC
      uint32 time_ms[2] = {0,0};
      clk_uptime_ms(time_ms);
      CAMCORDER_MSG_HIGH ("shdbg First audio sync cb received at time = %d",
          (int)time_ms[0], 0, 0);
#endif
      glb_avs_info.timelog_audio_start = av_report->time_stamp[0];
   }
}

#ifdef FEATURE_CAMCORDER_AAC_REC
/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmx_aac_data_cb

DESCRIPTION
  This function handles data request callbacks from the CMX interface for
  the AAC encoding.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_cmx_aac_data_cb (
  uint32                           position,
  uint32                           max_len,
  cmx_client_req_cb_func_ptr_type  data_req_cb,
  const void                       *server_client_data,
  const void                       *client_data
) {
    const uint8  *pkts = (const uint8 *) server_client_data;
    avSync_type  *avs = &(g_context->avSync_info);
    const uint8  *aac_pkts = pkts;
    uint32       len = max_len;
    uint32       aac_frame_len = len;
    uint16       aac_fr_len_array[50];
    uint32       aac_num_fr = 0;
    uint32       aac_frame_length;
#ifdef DEBUG_AVSYNC
    uint32 time_ms[2]={0,0};
#endif

    if (g_context == NULL)
    {
        CAMCORDER_MSG_ERROR ("aac_data_cb: exiting, no engine context", 0, 0, 0);
        if (data_req_cb)
        {
            CAMCORDER_MSG_HIGH ("aac_data_cb: cb(CMX_SUCCESS)", 0, 0, 0);
            data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data, max_len,
                         (uint8 *) server_client_data);
        }
        return;
    }

    /* If not recording, or we lost the video format services, ignore data -
    ** simply give it back to CMX and return.
    */
    if ((g_context->state != VIDEO_ENG_STATE_RECORDING &&
         g_context->state != VIDEO_ENG_STATE_STARTING  &&
         g_context->state != VIDEO_ENG_STATE_PAUSING   &&
         g_context->state != VIDEO_ENG_STATE_RESUMING  &&
         g_context->state != VIDEO_ENG_STATE_STOPPING) ||
        g_context->videofmt_server_data == NULL        ||
        g_context->space_out)
    {
        CAMCORDER_MSG_HIGH ("aac_data_cb: exiting or no writer context", 0, 0, 0);
        if (data_req_cb)
        {
            CAMCORDER_MSG_HIGH ("aac_data_cb: cb(CMX_SUCCESS)", 0, 0, 0);
            data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data, max_len,
                         (uint8 *) server_client_data);
        }
        return;
    }

    if (g_context->state == VIDEO_ENG_STATE_STARTING)
    {
        /* work around to fix arm reset in case of rapid start/stop
        recording issue with AAC audio format */
        g_context->audio_done = TRUE;
        CAMCORDER_MSG_HIGH ("aac_data_cb: doing audio done for AAC format",0,0,0);
        video_eng_audio_video_synchronize();
    }
    CAMCORDER_MSG_HIGH ("aac_data_cb: pos=%d, len=%d", position, max_len, 0);

    /* For the first frame, parse additional fields to build decoder
    ** specific information.
    */
    if (g_context->aac_dsi_written == FALSE)
    {
        /* Parse fields from header. */
        const uint8 audio_object_type = ((pkts [2] >> 6) & 0x03) + 1;
        const uint8 sampling_frequency_index = ((pkts [2] >> 2) & 0x0F);
        const uint8 channel_configuration
            = ((pkts [2] << 2) & 0x04)
            | ((pkts [3] >> 6) & 0x03);
        uint32 sampling_frequency;
        uint8 aac_specific_info [2];

        g_context->aac_dsi_written = TRUE;

        /* Verify sync word and layer field. */
        if ((pkts [0] != 0xFF) || ((pkts [1] & 0xF0) != 0xF0)
            || ((pkts [1] & 0x06) != 0x00))
        {
            /* Release audio buffer back to CMX. */
            if (data_req_cb)
            {
                data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data,
                            max_len, (uint8 *) server_client_data);
            }

            CAMCORDER_MSG_ERROR ("aac_data_cb: bad ADTS sync word!", 0, 0, 0);
            return;
        }

        /* Parse frame length. */
        aac_frame_length = ((pkts [3] & 0x03) << 11)
                         | ((pkts [4]) << 3)
                         | ((pkts [5] & 0xE0) >> 5);
        if (aac_frame_length > len)
        {
            /* Release audio buffer back to CMX. */
            if (data_req_cb)
            {
                data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data,
                            max_len, (uint8 *) server_client_data);
            }

            CAMCORDER_MSG_ERROR ("aac_data_cb: bad frame length!", 0, 0, 0);
            return;
        }

        /* Build decoder specific information. */
        /*lint ++fpm supressing unnecessary loss of precesion */
        if (sampling_frequency_index >= AAC_SAMP_FREQ_ARRAY_SIZE)
        {
            /* Release audio buffer back to CMX. */
            if (data_req_cb)
            {
                data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data,
                            max_len, (uint8 *) server_client_data);
            }

            CAMCORDER_MSG_ERROR ("aac_data_cb: sampling_frequency_index incorrect", 0, 0, 0);
            return;
        }

        aac_specific_info [0]
            = (audio_object_type << 3)
            | ((sampling_frequency_index & 0x0E) >> 1);
        aac_specific_info [1]
            = ((sampling_frequency_index & 0x01) << 7)
            | (channel_configuration << 3);
        /*lint --fpm */

        /* Decode sampling frequency and convert to frame period. */
        sampling_frequency
            = aac_sampling_frequencies [sampling_frequency_index];
        if (sampling_frequency == 0)
        {
            /* Release audio buffer back to CMX. */
            if (data_req_cb)
            {
                data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data,
                            max_len, (uint8 *) server_client_data);
            }

            CAMCORDER_MSG_ERROR ("aac_data_cb: bad frequency index!", 0, 0, 0);
            return;
        }

        /* Write the header to the movie file. */
        g_context->videofmt_write_header
            (g_context->audio_stream_num,
             sizeof (aac_specific_info), aac_specific_info,
             g_context->videofmt_server_data,
             video_eng_videofmt_callback, NULL);
        while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE)
               && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
        {
            if (g_context->videofmt_continue == NULL)
            {
                /* Release audio buffer back to CMX. */
                if (data_req_cb)
                {
                    data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data,
                                max_len, (uint8 *) server_client_data);
                }

                CAMCORDER_MSG_ERROR ("aac_data_cb: bad continue "
                           "function from videofmt!", 0, 0, 0);
                return;
            }
            else
            {
                g_context->videofmt_continue (g_context->videofmt_server_data);
            }
        }
        if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
        {
            g_context->videofmt_server_data = NULL;
            g_context->failure_on_exit = TRUE;
            g_context->terminate = TRUE;

            /* Release audio buffer back to CMX. */
            if (data_req_cb)
            {
                data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data,
                            max_len, (uint8 *) server_client_data);
            }

            CAMCORDER_MSG_ERROR ("aac_data_cb: "
                       "failed to write stream header", 0, 0, 0);
            return;
        }
    }

    if (!avs->first_audio_cb_received)
    {
       avs->first_audio_cb_received = TRUE;
       if (g_context->audio_data1_cb)
       {
          g_context->audio_data1_cb ();   //calls camera_audio_ready_cb, sync = video_eng_cmx_sync_cb_func
       }
    }
#ifdef DEBUG_AVSYNC
    clk_uptime_ms(time_ms);
    CAMCORDER_MSG_HIGH ("cmx_aac_data_cb_timelog = %d", time_ms[0], 0, 0);
#endif
    /* Drop first packet. */
    if (!g_context->first_audio_packet_dropped)
    {
        g_context->first_audio_packet_dropped = TRUE;

        /* VFE Config Delay Handling Anjustment */
        /* !C A U T I O N! -  glb_avs_info.timelog_video_start must be
        ** set from a lower priority task compared to the task pri of this
        ** function. */

        // Add to packets to skip based on new delay
        if(glb_avs_info.timelog_video_start >= (glb_avs_info.timelog_audio_start - avs->audio_sync_error_adjust))
        {
          // Parse cumulative length (bytes) of all frames from this cmx cb
          aac_fr_len_array [aac_num_fr] = ((aac_pkts [3] & 0x03) << 11) | ((aac_pkts [4]) << 3) | ((aac_pkts [5] & 0xE0) >> 5);
          aac_pkts += aac_fr_len_array [aac_num_fr];
          aac_frame_len -= aac_fr_len_array [aac_num_fr++];
          while(aac_frame_len)
          {
             aac_fr_len_array [aac_num_fr] = ((aac_pkts [3] & 0x03) << 11) | ((aac_pkts [4]) << 3) | ((aac_pkts [5] & 0xE0) >> 5);
             aac_pkts += aac_fr_len_array [aac_num_fr];
             aac_frame_len -= aac_fr_len_array [aac_num_fr];
             aac_fr_len_array [aac_num_fr] += aac_fr_len_array [aac_num_fr-1];
             aac_num_fr++;
          }

          // Unit is in msec, 64 less is to compensate for mm api bug of the first cb where they send
          // 1st data cb in the same time as sync1, thus data should be 0,  but it is 6s msec wide
          avs->vfe_config_delay = glb_avs_info.timelog_video_start -
             (glb_avs_info.timelog_audio_start - avs->audio_sync_error_adjust);
          avs->skip_audio_sum_ms_prev = avs->skip_audio_sum_ms;
          avs->skip_audio_sum_ms += aac_num_fr * g_context->audio_sample_delta
             * 1000 / g_context->audio_time_resolution;

          // last skip reached
           /* if already dropped enough frames */
           if(avs->vfe_config_delay < avs->skip_audio_sum_ms_prev)
           {
#ifdef DEBUG_AVSYNC
              CAMCORDER_MSG_HIGH ("shdbg ALREADY SKIPPED more data- delay was %d, skipped delay %d",
                                  avs->vfe_config_delay,avs->skip_audio_sum_ms_prev,0);
#endif
           }
           else if((avs->vfe_config_delay >= avs->skip_audio_sum_ms_prev)
             && (avs->vfe_config_delay <= avs->skip_audio_sum_ms))
          {
             avs->last_skip_ms = avs->vfe_config_delay - avs->skip_audio_sum_ms_prev;   //in msec
             // skip this many frames
             avs->last_skip_num_fr = ((avs->last_skip_ms * g_context->audio_time_resolution)
                  / g_context->audio_sample_delta) / 1000;
             if (avs->last_skip_num_fr > 0)
             {
                //skip this many bytes, this will always line up with frame boundary
                avs->skip_audio_pkts = aac_fr_len_array [avs->last_skip_num_fr-1];
             }
             else
             {
                avs->skip_audio_pkts = 0;
             }
             // skip for last skip audio
             pkts += avs->skip_audio_pkts;
             len -= avs->skip_audio_pkts;
          }
          else
          {
             g_context->first_audio_packet_dropped = FALSE; // to enable skip until video config done happens
             pkts += len;          // skip all audio
             len = 0;
             CAMCORDER_MSG_HIGH ("cmx_data_cb: VFE_DONE NOT BETWEEN CB's sh_vfe_delay = %d, sum_msx32_prev=%d, sum_msx32=%d",
                (int) avs->vfe_config_delay,
                   (int)avs->skip_audio_sum_ms_prev, (int)avs->skip_audio_sum_ms);
          }
        }
        else if (glb_avs_info.timelog_video_start == 0) 
        /* Video config is not done, thus skip all audio and renew the vfeCmd init time*/
        {
          // Parse cumulative length (bytes) of all frames from this cmx cb
          aac_fr_len_array [aac_num_fr] = ((aac_pkts [3] & 0x03) << 11) | ((aac_pkts [4]) << 3)
                                        | ((aac_pkts [5] & 0xE0) >> 5);
          aac_pkts += aac_fr_len_array [aac_num_fr];
          aac_frame_len -= aac_fr_len_array [aac_num_fr++];
          while(aac_frame_len)
          {
             aac_fr_len_array [aac_num_fr] = ((aac_pkts [3] & 0x03) << 11) | ((aac_pkts [4]) << 3)
                                        | ((aac_pkts [5] & 0xE0) >> 5);
             aac_pkts += aac_fr_len_array [aac_num_fr];
             aac_frame_len -= aac_fr_len_array [aac_num_fr];
             aac_fr_len_array [aac_num_fr] += aac_fr_len_array [aac_num_fr-1];
             aac_num_fr++;
          }

          avs->skip_audio_sum_ms += aac_num_fr * g_context->audio_sample_delta
             * 1000 / g_context->audio_time_resolution;

          // to enable skip until video config done happens
          g_context->first_audio_packet_dropped = FALSE;
          pkts += len;                                            // skip all audio
          len = 0;
        }
        CAMCORDER_MSG_HIGH ("cmx_data_cb sh_vfe_delay=%d, sh_skip_audio_pkts=%d, after_skip len=%d",
            (int)avs->vfe_config_delay,(int)avs->skip_audio_pkts, (int)len);
    }

    /* Repeat until all audio frames written. */
    while (len)
    {
        /* Verify sync word and layer field. */
        if ((pkts[0] != 0xFF) || ((pkts[1] & 0xF0) != 0xF0) || ((pkts[1] & 0x06) != 0x00))
        {
            CAMCORDER_MSG_ERROR ("aac_data_cb: bad ADTS sync word!", 0, 0, 0);
            break;
        }

        /* Parse frame length. */
        aac_frame_length = ((pkts [3] & 0x03) << 11)
                         | ((pkts [4]) << 3)
                         | ((pkts [5] & 0xE0) >> 5);
        if (aac_frame_length > len)
        {
            CAMCORDER_MSG_ERROR ("aac_data_cb: bad frame length!", 0, 0, 0);
            break;
        }

        g_context->audio_sample_info [0].size
            = aac_frame_length - 7;
        g_context->audio_sample_info [0].delta
            = g_context->audio_sample_delta;
        g_context->audio_sample_info [0].sync = 1;

        /* Write the audio to the movie, via video format services. */
        g_context->videofmt_write_stream
            (g_context->audio_stream_num, 1,
             &g_context->audio_sample_info [0], pkts + 7,
             g_context->videofmt_server_data,
             video_eng_videofmt_callback, NULL);
        while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE)
               && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
        {
            if (g_context->videofmt_continue == NULL)
            {
                CAMCORDER_MSG_ERROR ("aac_data_cb: bad continue "
                           "function from videofmt!", 0, 0, 0);
                break;
            }
            else
            {
                g_context->videofmt_continue (g_context->videofmt_server_data);
            }
        }
        if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
        {
            break;
        }
        g_context->audio_num_packets += 1;
        /* If we are low on space, pass along notification to the client. */
        video_eng_send_space_limit_status();

        pkts += aac_frame_length;
        len  -= aac_frame_length;
    }

    /* Release audio buffer back to CMX. */
    if (data_req_cb)
    {
        CAMCORDER_MSG_HIGH ("aac_data_cb: cb(CMX_SUCCESS)", 0, 0, 0);
        data_req_cb (CMX_SUCCESS, (uint8 *) server_client_data, max_len,
                     (uint8 *) server_client_data);
    }
} /*lint !e715 'client_data' is not used */


/* =========================================================================== */

#endif /* FEATURE_CAMCORDER_AAC_REC */
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

/* <EJECT> */
/*===========================================================================
FUNCTION video_eng_space_check

DESCRIPTION
  This function calculates the available space

DEPENDENCIES
  Should be called after video_eng_start_processing and
  before video_eng_stop_processing

RETURN VALUE
  space left

SIDE EFFECTS
  None.

===========================================================================*/
uint64 video_eng_space_check(void)
{
    uint64 bytes_available;
    uint64 free_space;

    /* If writing to memory buffer, bytes available is total memory buffer
    ** size minus the bytes we've already used, with a margin.
    */
    if (g_context->output.method == VIDEO_METHOD_MEM)
    {
        bytes_available = g_context->output.mem.len - g_context->output_size;
        bytes_available -= MIN ((uint64)bytes_available, (uint64)VIDEO_ENG_MEM_MARGIN);
    }

#ifdef FEATURE_EFS2
    /* If writing to EFS, return space available on the EFS using movie
    ** file path, minus some margin amount.
    */
    else if ((g_context->output.method == VIDEO_METHOD_EFS)
                 || g_context->use_temp_file_1)
    {
        free_space = g_context->free_space;
        if (g_context->output.method == VIDEO_METHOD_EFS)
        {
          free_space -= g_context->output_size;
        }
        free_space -= g_context->temp_file_size 
                      * g_context->temp_file_size_factor;
        free_space -= g_context->temp_file_size_3
                      * 2 * g_context->temp_file_size_factor_3;

        free_space -= MIN ((uint64)free_space, (uint64)VIDEO_ENG_EFS_MARGIN);
        bytes_available = free_space;

    }
#endif /* FEATURE_EFS2 */
    else
    {
        /* If we reach here, we have no way of knowing how much space is left
        ** on the recording device, so we will return a very large number to
        ** keep the movie writer from stopping.  It is up to the client to
        ** make sure we don't actually run out of space!
        */
        bytes_available = (uint32) -1;
    }
    return bytes_available;
}
/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_videofmt_callback

DESCRIPTION
  This function handles callbacks from the video format services.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_videofmt_callback (
  video_fmt_status_type                  status,
  void                                   *client_data,
  union video_fmt_status_cb_info_type_t  *info,
  video_fmt_end_cb_func_type             end
) {
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
    video_eng_user_data_atom_type *puseratominfo;
#endif /*FEATURE_VIDEO_SETATOMS_DYNAMIC*/

    /* Store the status code for later inspection. */
    g_context->videofmt_status = status;

    /* Store session end callback function. */
    g_context->videofmt_end = end;

    /* Branch according to the status code received. */
    switch (status)
    {
    case VIDEO_FMT_ALLOC:
        info->alloc.ptr = videoeng_malloc (info->alloc.size);
        break;

    case VIDEO_FMT_FREE:
        videoeng_free (info->free.ptr);
        break;

#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
    case VIDEO_FMT_REALLOC:
        info->alloc.ptr = videoeng_realloc(info->alloc.ptr,info->alloc.size);
        break;
#endif /*FEATURE_VIDEO_SETATOMS_DYNAMIC*/

    case VIDEO_FMT_PUT_DATA:
        {
            /* Store continue callback and server data for later use. */
            g_context->videofmt_continue = info->put_data.callback_ptr;
            g_context->videofmt_server_data = info->put_data.server_data;

            /* Write the given number of bytes from the given buffer to the
            ** file, starting at the given offset.
            */
            if (!video_eng_output (info->put_data.buffer,
                                   info->put_data.offset,
                                   info->put_data.num_bytes))
            {
                info->put_data.num_bytes = 0;
            }

            /* If a size callback is set up, report file size. */
            if (g_context->size_cb)
            {
                g_context->size_cb (g_context->output_size,
                                    g_context->client_data);
            }
        }
        break;

    case VIDEO_FMT_PUT_META_DATA:
        {
            /* Store continue callback and server data for later use. */
            g_context->videofmt_continue = info->put_meta_data.callback_ptr;
            g_context->videofmt_server_data = info->put_meta_data.server_data;

            info->put_meta_data.id = g_context->temp_file_size_3;

            /* Write the given number of bytes from the given buffer to the
            ** file, starting at the given offset.
            */
            if (!video_eng_write_meta_data (info->put_meta_data.buffer,
                                            info->put_meta_data.num_bytes))
            {
                info->put_meta_data.num_bytes = 0;
            }
        }
        break;

    case VIDEO_FMT_GET_META_DATA:
        {
            /* Store continue callback and server data for later use. */
            g_context->videofmt_continue = info->get_meta_data.callback_ptr;
            g_context->videofmt_server_data = info->get_meta_data.server_data;

            /* Write the given number of bytes from the given buffer to the
            ** file, starting at the given offset.
            */
            if (!video_eng_read_meta_data (info->get_meta_data.buffer,
                                           info->get_meta_data.id,
                                           info->get_meta_data.num_bytes))
            {
                info->get_meta_data.num_bytes = 0;
            }
        }
        break;

    case VIDEO_FMT_CONTINUE:
        /* Store continue callback and server data for later use. */
        g_context->videofmt_continue = info->cont.callback_ptr;
        g_context->videofmt_server_data = info->cont.server_data;
        break;

    case VIDEO_FMT_WRITER_READY:
        /* Save stream hook functions. */
        g_context->videofmt_write_stream = info->writer_ready.write_cb;
        g_context->videofmt_write_text = info->writer_ready.write_text_cb;
        g_context->videofmt_write_header = info->writer_ready.write_header_cb;
        g_context->videofmt_write_footer = info->writer_ready.write_footer_cb;
        g_context->videofmt_write_header_text
            = info->writer_ready.write_header_text_cb;
        g_context->videofmt_write_uuid = info->writer_ready.write_uuid_cb;
        g_context->videofmt_seek = info->writer_ready.seek_cb;
        g_context->videofmt_alloc_text = info->writer_ready.alloc_text_cb;
        g_context->videofmt_free_text = info->writer_ready.free_text_cb;
        g_context->videofmt_modify_style_text
            = info->writer_ready.modify_style_text_cb;
        g_context->videofmt_modify_hl_text
            = info->writer_ready.modify_hl_text_cb;
        g_context->videofmt_modify_link_text
            = info->writer_ready.modify_link_text_cb;
        g_context->videofmt_server_data = info->writer_ready.server_data;
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
        g_context->videofmt_modify_user_atoms = info->writer_ready.modify_user_atoms_cb;
#endif /*FEATURE_VIDEO_SETATOMS_DYNAMIC*/
        break;

    case VIDEO_FMT_IO_DONE:
        /* Store fragment done flag for later use. */
        g_context->videofmt_fragment_done = info->io_done.fragment_done;
        break;

    case VIDEO_FMT_DONE:
        /* Store total duration for later use. */
        g_context->frag_total_duration = info->done.total_duration;
        break;

    case VIDEO_FMT_LIMIT_NEAR:
        g_context->space_out_near = TRUE;
#ifdef DETAILED_SPACE_OUT_MSG
        g_context->space_out_status = info->done.space_out_status;
#endif /*DETAILED_SPACE_OUT_MSG*/
        break;

    case VIDEO_FMT_LIMIT_IMMINENT:
        g_context->space_out_imminent = TRUE;
#ifdef DETAILED_SPACE_OUT_MSG
        g_context->space_out_status = info->done.space_out_status;
#endif/*DETAILED_SPACE_OUT_MSG*/
        break;

    case VIDEO_FMT_LIMIT_REACHED:
        g_context->space_out = TRUE;
#ifdef DETAILED_SPACE_OUT_MSG
        g_context->space_out_status = info->done.space_out_status;
#endif/*DETAILED_SPACE_OUT_MSG*/
        break;

    case VIDEO_FMT_SPACE_CHECK:
        info->space_check.bytes_available = video_eng_space_check();
        break;
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
    case VIDEO_FMT_UDTA_ATOM_REQ:
        /*Check for callback function address*/
        if(g_context->useratom_cbptr == NULL)
        {
            return;
        }
        puseratominfo = &g_context->useratom_data;

        /*Call the client function to get the titl,auth,descp and loci atoms*/
        g_context->useratom_cbptr(puseratominfo,g_context->client_data);

        /*Check if user data atom is valid */ 
        if(puseratominfo->atom_is_valid)
        {
            /* Update the information at g_context as well as passed ptr */
            g_context->include_user_atom = TRUE; /* Indicate that user atom has to be included */
            info->user_data.atom_is_valid = TRUE;
            /* Update title, desc & coyright information */
            if(puseratominfo->titl_auth_dscp_cprt_valid)
            {
                info->user_data.titl_auth_desc_valid = TRUE;

                /*Point to title data*/
                info->user_data.title       = puseratominfo->title;
                /*Point to author data*/
                info->user_data.author       = puseratominfo->author;
                /*Point to description data*/
                info->user_data.description  = puseratominfo->description;
                /*Point to copyright data*/
                info->user_data.copyright    = puseratominfo->copyright;
            }

            /* Update the location information */
            if(puseratominfo->loci.loci_valid)
            {
                /*Point to location data*/
                info->user_data.loci        = puseratominfo->loci;
            }
        }
        break;
#endif /*FEATURE_VIDEO_SETATOMS_DYNAMIC*/

#ifdef FEATURE_VIDEO_CLIP_INFO
    case VIDEO_FMT_CLIPINFO_REQ:
        CAMCORDER_MSG_LOW("videofmt_callback: clipinfo!", 0,0,0);
        g_context->clipinfo.space_left = video_eng_space_check();
        /* Since videofmt does not know about camcorder_msg.h file,
        ** so we are passing all the messages at one go to videofmt
        */
        if(msg_head != NULL)
         {
           char * msg_index;
           log_msg = FALSE;
           msg_curr = msg_head;
           g_context->clipinfo.err_msg_size = 0;
           while(msg_curr != NULL)
           {
               g_context->clipinfo.err_msg_size += (msg_curr->msg_size + 2);
               msg_curr = msg_curr->next;
           }
           g_context->clipinfo.err_msg =
               videoeng_malloc(g_context->clipinfo.err_msg_size);
           if (NULL == g_context->clipinfo.err_msg)
           {
               CAMCORDER_MSG_ERROR ("videofmt_callback: Unable to allocate err_msg!!",0,0,0);
               /* Cleanup the msg entry*/
               while(msg_head != NULL)
               {
                   msg_curr = msg_head;
                   msg_head = msg_head->next;
                   free(msg_curr->msg);
                   free(msg_curr);
               }
               msg_curr = NULL;
               g_context->clipinfo.err_msg_size = 0;
           }
           msg_index = g_context->clipinfo.err_msg;
           while(msg_head != NULL)
           {
               *msg_index++ = '<'; // Start of message
               memcpy(msg_index,msg_head->msg,msg_head->msg_size);
               msg_index += msg_head->msg_size;
               *msg_index++ = '>'; // End of message
               msg_curr = msg_head;
               msg_head = msg_head->next;
               free(msg_curr->msg);
               free(msg_curr);
           }
           msg_curr = NULL;
        }
        info->clipinfo.data =  &g_context->clipinfo;
        CAMCORDER_MSG_LOW("videofmt_callback: clipinfo ends!", 0,0,0);
        break;
#endif //FEATURE_VIDEO_CLIP_INFO

    default:
        CAMCORDER_MSG_ERROR("videofmt_callback: wrong status!", 0,0,0);
        break;
    }
} /*lint !e715 !e818 'client_data' is not used and can declared as
    pointing to const object */

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_enc_failure

DESCRIPTION
  This function exits the engine with a failure status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_enc_failure (
  boolean  stop_audio
) {
    if (g_context->state == VIDEO_ENG_STATE_STOPPING)
    {
        /* Move engine to failing state, waiting for encoder to exit. */
        g_context->failure_on_exit = TRUE;
        g_context->state = VIDEO_ENG_STATE_EXITING;

        /* Set audio_done to true, because we won't be making any more calls
        ** to CMX.
        */
        g_context->audio_done = TRUE;

        CAMCORDER_MSG_HIGH("enc_failure: eng already stopping, exiting engine", 0,0,0);

        if(video_encode_exit (NULL, NULL) != VIDEO_SUCCESS)
        {
          CAMCORDER_MSG_HIGH("enc_failure: video_encode_exit failed",0,0,0);
        }
        return;
    }

    /* Move engine to exiting state, waiting for encoder to exit. */
    g_context->failure_on_exit = TRUE;
    g_context->state = VIDEO_ENG_STATE_EXITING;
    g_context->audio_done = TRUE;
    g_context->video_done = FALSE;
    g_context->video_error = FALSE;
    g_context->audio_error = FALSE;

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    /* Stop audio recording if given flag is set. */
    if (stop_audio && (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE)
         && (g_context->no_coding == FALSE) 
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
         && (g_context->framecb_ptr == NULL)
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
        )
    {
        g_context->audio_done = FALSE;
#ifdef FEATURE_CAMCORDER_AAC_REC
        if (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
        {
            cmx_mm_record_stop (CMX_MM_REC_LINK_REVERSE,
                (cmx_cb_func_ptr_type) video_eng_cmx_normal_cb, NULL);
        }
        else
#endif /* FEATURE_CAMCORDER_AAC_REC */
        {
            cmx_mm_record_stop (CMX_MM_REC_LINK_REVERSE,
                (cmx_cb_func_ptr_type) video_eng_cmx_normal_cb, NULL);
        }
    }
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
    /* If movie has video, tell the video encoder to exit.
    */
    if (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE &&
        (g_context->no_coding == FALSE))
    {
        CAMCORDER_MSG_HIGH ("enc_failure: exiting engine", 0, 0, 0);
        if(video_encode_exit ((video_cb_func_ptr_type)
                                  video_eng_enc_normal_cb, NULL) != VIDEO_SUCCESS)
        {
          video_eng_enc_normal_cb(VIDEO_FAILURE,NULL);
        }
    }

    /* Otherwise, mark video part as already done.  If audio part is also
    ** already done, terminate the engine immediately.
    */
    else
    {
        g_context->video_done = TRUE;
        if (g_context->audio_done)
        {
            video_eng_handle_exit ();
        }
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_free_enc_buffers

DESCRIPTION
  This function frees any video encoder buffers that are still allocated.
  stream.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_free_enc_buffers (void)
{
    CAMCORDER_MSG_HIGH ("free_enc_buffers: freeing buffer memory", 0, 0, 0);

    /* Free any memory allocations that succeeded. */
    if (g_context->audio_leftover != NULL)
    {
        videoeng_free (g_context->audio_leftover);
        g_context->audio_leftover = NULL;
    }
    if (g_context->enc_pkt_ptr1 != NULL)
    {
        videoeng_free (g_context->enc_pkt_ptr1);
        g_context->enc_pkt_ptr1 = NULL;
    }
    if (g_context->enc_pkt_ptr2 != NULL)
    {
        videoeng_free (g_context->enc_pkt_ptr2);
        g_context->enc_pkt_ptr2 = NULL;
    }
    if (g_context->vop_recon_buf_ptr1 != NULL)
    {
        videoeng_free (g_context->vop_recon_buf_ptr1);
        g_context->vop_recon_buf_ptr1 = NULL;
    }
    if (g_context->vop_recon_buf_ptr2 != NULL)
    {
        videoeng_free (g_context->vop_recon_buf_ptr2);
        g_context->vop_recon_buf_ptr2 = NULL;
    }
    if (g_context->ved_buf_ptr != NULL)
    {
        videoeng_free (g_context->ved_buf_ptr);
        g_context->ved_buf_ptr = NULL;
    }

    if ( g_context->encoder_internal_memory != NULL )
    {
      videoeng_free(g_context->encoder_internal_memory);
      g_context->encoder_internal_memory = NULL;
    }

    /* Cleanup the useratom_data structure*/
    memset(&(g_context->useratom_data),0,sizeof(video_eng_user_data_atom_type));
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_output

DESCRIPTION
  This function writes the given data to the output destination set up
  for the engine.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned if the data was output successfully.  Otherwise,
  the value FALSE is returned.

SIDE EFFECTS
  None.

===========================================================================*/
boolean video_eng_output (
  const uint8  *data,
  uint32       offset,
  uint32       len
) {
#ifdef FEATURE_EFS2
    fs_size_t        write_count;
    uint32           chunk;
    int              errno = 0;
    uint32           start_time =0, counter=0;
#endif /* FEATURE_EFS2 */
    uint32 cur_time=0;

#ifdef FEATURE_EFS2
    /* Write to temporary file if "use temporary file" flag is set. */
    if (g_context->use_temp_file_1)
    {
        /* If offset is different from last offset, perform seek operation. */
        if (offset != g_context->output_offset)
        {
            if (video_eng_file_fseek (g_context->temp_handle_1,
                           (int32)(offset - g_context->frag_in_offset), SEEK_SET) != 0)
            {
                errno = efs_errno; 
                CAMCORDER_MSG_ERROR ("output: error '%d' from EFS for "
                           "video_eng_file_fseek to offset %d",
                           errno, offset, 0);
                return FALSE;
            }
            g_context->output_offset = offset;
        }

        CAMCORDER_MSG_HIGH("Got %u bytes of data from video engine. Writing to FS", len,0,0);
        if(g_context->dynamic_bitrate_enable ||
           g_context->max_bitrate_enable)
        {
            if(len > sizeof(M4V_BITSTREAM_FIXED_HEADER) ){
            
             counter = efs_fpending(g_context->fs_handle->object);
             CAMCORDER_MSG_HIGH("Buf size in enc_frame is %u", counter,0,0);
             start_time = mp4e_get_time_ms();
             CAMCORDER_MSG_HIGH("Start time is %u", start_time,0,0);
             video_enc_log_start_drain(len, start_time);
            
            }
            else {
            
             CAMCORDER_MSG_HIGH("output: Ignored data",0,0,0);
            
            }
        }
        /* Write in 32KB chunks since SFAT service cannot handle larger than
        ** 65535 bytes at a time, and so we will pick the next power of 2
        ** smaller than this.
        */
        while (len > 0)
        {
            /* Select next chunk size. */
            chunk = MIN (32768, len);

            /* Write out the chunk. */
            CAMCORDER_MSG_HIGH ("output: writing %d bytes to temp file", chunk, 0, 0);
            write_count = video_eng_file_fwrite ((void *) data, 1, chunk,
                                      g_context->temp_handle_1);
            if (write_count != chunk)
            {
                errno = efs_errno; 
                CAMCORDER_MSG_ERROR ("output: error '%d' from EFS for video_eng_file_fwrite "
                           "of %d bytes",
                           errno, chunk, 0);
                return FALSE;
            }

            /* Move on to next chunk. */
            len -= chunk;
            data += chunk;
            g_context->output_offset += chunk;
            g_context->output_size = MAX (g_context->output_size,
                                          g_context->output_offset);
        }

        cur_time = mp4e_get_time_ms();
        g_context->dynamic_bitrate = video_enc_get_bitrate(0, cur_time);
        g_context->temp_file_size =
              MAX(g_context->output_size - g_context->frag_in_offset,
                  g_context->temp_file_size);
        return TRUE;
    }
#endif /* FEATURE_EFS2 */

    /* Branch according to method. */
    switch (g_context->output.method)
    {
    case VIDEO_METHOD_MEM:
        /* Check for memory buffer overflow. */
        if (offset + len > g_context->output.mem.len)
        {
            CAMCORDER_MSG_ERROR ("output: output memory buffer overflow!",
                       0, 0, 0);
            return FALSE;
        }

        /* Write data out to memory buffer. */
        memcpy (g_context->output.mem.buf + offset, data, len);
        g_context->output_offset = offset + len;
        g_context->output_size = MAX (g_context->output_size,
                                      g_context->output_offset);
        return TRUE;

#ifdef FEATURE_EFS2
    case VIDEO_METHOD_EFS:
        /* If offset is different from last offset, perform seek operation. */
        if (offset != g_context->output_offset)
        {
            if (video_eng_file_fseek (g_context->fs_handle, (int32)offset, SEEK_SET)
                != 0)
            {
                errno = efs_errno; 
                CAMCORDER_MSG_ERROR ("output: error '%d' from EFS for "
                           "video_eng_file_fseek to offset %d",
                           errno, offset, 0);
                return FALSE;
            }
            g_context->output_offset = offset;
        }

        CAMCORDER_MSG_HIGH("Got %u bytes of data from video engine. Writing to FS", len,0,0);
        if(g_context->dynamic_bitrate_enable 
           || g_context->max_bitrate_enable)
        {
           if(len > sizeof(M4V_BITSTREAM_FIXED_HEADER) ){
            
            cur_time = mp4e_get_time_ms();
            video_enc_log_start_drain(len, cur_time);
           
           }
           else
           {
             CAMCORDER_MSG_HIGH(" Ignored data ",0,0,0);
           }
        }

        /* Write data out to the filesystem.  Write in 32KB chunks since SFAT
        ** service cannot handle larger than 65535 bytes at a time, and so we
        ** will pick the next power of 2 smaller than this.
        */
        while (len > 0)
        {
            /* Select next chunk size. */
            chunk = MIN (32768, len);

            /* Write out the chunk. */
            CAMCORDER_MSG_HIGH ("output: writing %d bytes to output file", chunk, 0, 0);
            write_count = video_eng_file_fwrite ((void *) data, 1, chunk,
                                      g_context->fs_handle);
            if (write_count != chunk)
            {
                errno = efs_errno; 
                CAMCORDER_MSG_ERROR ("output: error '%d' from EFS for "
                           "video_eng_file_fwrite of %d bytes",
                           errno, chunk, 0);
                return FALSE;
            }

            /* Move on to next chunk. */
            len -= chunk;
            data += chunk;
            g_context->output_offset += chunk;
        }
        g_context->output_size = MAX (g_context->output_size,
                                      g_context->output_offset);
        return TRUE;
#endif /* FEATURE_EFS2 */

    case VIDEO_METHOD_CLIENT:
        /* Send data to client callback. */
        (void)g_context->output.client.data_push_fn
            ((uint8 *) data, len, offset,
             g_context->output.client.client_data);
        g_context->output_offset = offset + len;
        g_context->output_size = MAX (g_context->output_size,
                                      g_context->output_offset);
        return TRUE;

    case VIDEO_METHOD_INVALID:
    default:
        CAMCORDER_MSG_ERROR ("output: Invalid method type %d set!",
                   g_context->output.method, 0, 0);
        return FALSE;
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_write_meta_data

DESCRIPTION
  This function writes the given data to the temporary destination set up
  for the engine.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned if the data was written successfully.  Otherwise,
  the value FALSE is returned.

SIDE EFFECTS
  None.

===========================================================================*/
boolean video_eng_write_meta_data (
  const uint8  *data,
  uint32       len
) {
#ifdef FEATURE_EFS2
    fs_size_t        write_count;
    uint32           chunk;
    int              errno  = 0;
#endif /* FEATURE_EFS2 */

#ifdef FEATURE_EFS2
    /* Write to temporary file if "use temporary file" flag is set. */
    if (g_context->use_temp_file_3)
    {
        /* Write in 32KB chunks since SFAT service cannot handle larger than
        ** 65535 bytes at a time, and so we will pick the next power of 2
        ** smaller than this.
        */
        while (len > 0)
        {
            /* Select next chunk size. */
            chunk = MIN (32768, len);

            /* Write out the chunk. */
            CAMCORDER_MSG_HIGH ("write_meta_data: writing %d bytes to temp file 3",
                        chunk, 0, 0);
            write_count = video_eng_file_fwrite ((void *) data, 1, chunk,
                                      g_context->temp_handle_3);
            if (write_count != chunk)
            {
                errno = efs_errno; 
                CAMCORDER_MSG_ERROR ("write_meta_data: error '%d' from EFS for "
                            "video_eng_file_fwrite of %d bytes",
                           errno, chunk, 0);
                return FALSE;
            }

            /* Move on to next chunk. */
            len -= chunk;
            data += chunk;
        }

        g_context->temp_file_size_3
            = (uint32)video_eng_file_ftell (g_context->temp_handle_3);
        return TRUE;
    }
#endif /* FEATURE_EFS2 */

    return FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_read_meta_data

DESCRIPTION
  This function reads data from the temporary destination set up
  for the engine.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned if the data was read successfully.  Otherwise,
  the value FALSE is returned.

SIDE EFFECTS
  None.

===========================================================================*/
boolean video_eng_read_meta_data (
  const uint8  *data,
  uint32       offset,
  uint32       len
) {
#ifdef FEATURE_EFS2
    fs_size_t        read_count;
    uint32           chunk;
    int              errno = 0;
#endif /* FEATURE_EFS2 */

#ifdef FEATURE_EFS2
    /* Write to temporary file if "use temporary file" flag is set. */
    if (g_context->use_temp_file_3)
    {
        if (video_eng_file_fseek (g_context->temp_handle_3, (int32)offset,
                                SEEK_SET) != 0)
        {
            errno = efs_errno;
            CAMCORDER_MSG_ERROR ("read_meta_data: error '%d' from EFS for "
                       "video_eng_file_fseek to offset %d",
                       errno, offset, 0);
            return FALSE;
        }

        /* Write in 32KB chunks since SFAT service cannot handle larger than
        ** 65535 bytes at a time, and so we will pick the next power of 2
        ** smaller than this.
        */
        while (len > 0)
        {
            /* Select next chunk size. */
            chunk = MIN (32768, len);

            /* Write out the chunk. */
            CAMCORDER_MSG_HIGH ("read_meta_data: reading %d bytes from temp file",
                        chunk, 0, 0);
            read_count = video_eng_file_fread ((void *) data, 1, chunk,
                                              g_context->temp_handle_3);
            if (read_count != chunk)
            {
                errno = efs_errno; 
                CAMCORDER_MSG_ERROR ("output: error '%d' from EFS for "
                           "video_eng_file_fwrite of %d bytes",
                           errno, chunk, 0);
                return FALSE;
            }

            /* Move on to next chunk. */
            len -= chunk;
            data += chunk;
        }
        return TRUE;
    }
#endif /* FEATURE_EFS2 */

    return FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_handle_exit

DESCRIPTION
  This function finishes an exit operation after the video/audio systems
  have finished shutting down.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  g_context->terminated is set, and a callback (either VIDEO_SUCCESS or
  VIDEO_FAILURE) is sent to the client, to indicate either that a file was
  saved or not.

===========================================================================*/
void video_eng_handle_exit (void)
{
    /* Set up engine to terminate after this function returns. */
    g_context->terminate = TRUE;
    g_context->delete_on_exit = g_context->failure_on_exit;
    g_context->failure_on_exit = FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_handle_terminate

DESCRIPTION
  This function shuts down the video engine.  It is called when the engine is
  in the midst of terminating.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  All video engine variables, including g_context, are freed.

===========================================================================*/
void video_eng_handle_terminate (void)
{
    video_eng_udat_list_type             *udat;
    /* Save off no_coding value so that we can use it
       after we delete g_context */
    boolean no_coding = g_context->no_coding;
#ifdef FEATURE_VIDEO_CLIP_INFO
    video_fmt_framedrop_list_type  *fdat;
#endif //FEATURE_VIDEO_CLIP_INFO
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif//#FEATURE_APP_TEST_AUTOMATION

    /* Attempt to save rest of file, if save on exit flag is set. */
    if (g_context->output_open && g_context->save_on_exit)
    {
      video_eng_handle_close (TRUE);
    }
    else
    {
      /* Release movie writer, if not yet released. */
      video_eng_close_writer ();
    }
    /* Close movie file if it is still open. */
    if (g_context->output_open)
    {
      g_context->output_open = FALSE;
#ifdef FEATURE_EFS2
      if ((g_context->output.method == VIDEO_METHOD_EFS)
           && (g_context->fs_handle != NULL))
      {
        CAMCORDER_MSG_HIGH ("terminate: Closing file", 0, 0, 0);
        (void) video_async_file_close (g_context->fs_handle);
        g_context->fs_handle = NULL;
      }
#endif /* FEATURE_EFS2 */
    }

#ifdef FEATURE_EFS2
    /* Delete the file in EFS if delete flag is set, save flag is not set,
     ** and we were saving to EFS.
     */
    if (g_context->delete_on_exit && !g_context->save_on_exit
        && (g_context->output.method == VIDEO_METHOD_EFS))
    {
      CAMCORDER_MSG_HIGH ("terminate: Deleting file", 0, 0, 0); 
      (void) video_async_file_unlink (g_context->output.efs.filename,strlen (g_context->output.efs.filename)); 
    }
#endif /* FEATURE_EFS2 */

#ifdef FEATURE_EFS2
    /* Delete the file in EFS if we were recording video and no video frame
    ** came in and we were saving to EFS.
    */
    if ((g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
        && (g_context->video_frames_received == 0)
        && (g_context->output.method == VIDEO_METHOD_EFS))
    {
        CAMCORDER_MSG_HIGH ("terminate: No video frame. Deleting file", 0, 0, 0);
        (void) video_async_file_unlink (g_context->output.efs.filename,strlen (g_context->output.efs.filename));
    }
#endif /* FEATURE_EFS2 */

#ifdef FEATURE_EFS2
    /* Close and delete temporary files, if any. */
    if (g_context->use_temp_file_1)
    {
        (void) video_async_file_close (g_context->temp_handle_1);
        (void) video_async_file_unlink (g_context->temp_file_name_1,strlen (g_context->temp_file_name_1));
        g_context->use_temp_file_1 = FALSE;
    }
    if (g_context->use_temp_file_2)
    {
        (void) video_async_file_close (g_context->temp_handle_2);
        (void) video_async_file_unlink (g_context->temp_file_name_2,strlen (g_context->temp_file_name_2));
        g_context->use_temp_file_2 = FALSE;
    }
    if (g_context->use_temp_file_3)
    {
        (void) video_async_file_close (g_context->temp_handle_3);
        (void) video_async_file_unlink (g_context->temp_file_name_3,strlen (g_context->temp_file_name_3));
        g_context->use_temp_file_3 = FALSE;
    }
    if (g_context->output.method == VIDEO_METHOD_EFS)
    {
      (void) rex_clr_sigs (&vs_tcb, VS_VIDEO_ASYNC_REQ_COMPLETE);
      (void) video_aysnc_request_complete(&vs_tcb,VS_VIDEO_ASYNC_REQ_COMPLETE);
      (void) rex_wait (VS_VIDEO_ASYNC_REQ_COMPLETE);
      (void) rex_clr_sigs (&vs_tcb, VS_VIDEO_ASYNC_REQ_COMPLETE);
    }
#endif /* FEATURE_EFS2 */

    /* If the failure on exit flag is still set at this time, it is a
    ** reminder that we need to give the client a failure callback.
    */
    if (g_context->failure_on_exit)
    {
        g_context->callback_ptr
            (VIDEO_FAILURE, g_context->client_data);
    }

    /* Deallocate buffers used by the engine. */
    video_eng_free_enc_buffers ();

    /* Send a success or failure status code back to our client, based
    ** on the state of the engine failure flag.
    */
    if(g_context->state == VIDEO_ENG_STATE_EXITING )
    {
       if (g_context->delete_on_exit)
        {
             g_context->callback_ptr
            (VIDEO_FAILURE, g_context->client_data);
        }
        else
        {
            g_context->callback_ptr
              (VIDEO_SUCCESS, g_context->client_data);
        }
    }

    /* Deallocate user data if any. */
    udat = g_context->udat_head;
    while (udat)
    {
        if (udat->data)
        {
            videoeng_free (udat->data);
        }
        g_context->udat_head = udat->next;
        videoeng_free (udat);
        udat = g_context->udat_head;
    }
    g_context->udat_tail = NULL;

#ifdef FEATURE_VIDEO_CLIP_INFO
    /* Dealloacte various clipinfo specific data */
    if (g_context->clipinfo.build_id) {
        free(g_context->clipinfo.build_id);
        g_context->clipinfo.build_id = NULL;
    }
    fdat = g_context->clipinfo.framedrop_head;
    while(fdat != NULL)
    {
       g_context->clipinfo.framedrop_head = fdat->next;
       videoeng_free(fdat);
       fdat = g_context->clipinfo.framedrop_head;
    }
    g_context->clipinfo.framedrop_current = NULL;

    if(g_context->clipinfo.err_msg && g_context->clipinfo.err_msg_size)
    {
        videoeng_free(g_context->clipinfo.err_msg);
        g_context->clipinfo.err_msg_size = 0;
    }
    log_msg = FALSE;
    while(msg_head != NULL)
    {
        msg_curr = msg_head;
        msg_head = msg_head->next;
        free(msg_curr->msg);
        free(msg_curr);
    }
    msg_curr = NULL;
#endif //FEATURE_VIDEO_CLIP_INFO

#ifndef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
    //Release memory allocated to enc buffer
    if(g_context->framecb_ptr)
#endif
    {
        if(g_context->client_to_enc_buffer)
        {
           free(g_context->client_to_enc_buffer);
           g_context->client_to_enc_buffer = NULL;
        }
	}
    if(g_context->pEncFrame)
	{
		videoeng_free(g_context->pEncFrame); 
		g_context->pEncFrame = NULL; 
	}

    /* Deallocate engine variables. */
    CAMCORDER_MSG_HIGH ("terminate: freeing context", 0, 0, 0);
    if (g_context->text)
    {
        videoeng_free (g_context->text);
    }
    videoeng_free (g_context);
    g_context = NULL;

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
    if (no_coding == FALSE)
    (void)video_eng_frame_terminate();
#else
#ifdef FEATURE_CAMCORDER_MALLOC
  { static msg_const_type m;

    camcorderutil_malloc_terminate(videoeng_malloc_context,
                                   __LINE__,__func__,&m);
    videoeng_malloc_context = NULL;
  }
#endif  /* ifdef FEATURE_CAMCORDER_MALLOC               */
#endif  /* ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_handle_close

DESCRIPTION
  This function finishes up and closes the movie file being written.  It is
  called once the video encoder has finished stopping, or the movie file is
  closed for any other reason.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The movie file meta data is written and the file is closed.

===========================================================================*/
void video_eng_handle_close (boolean suppress_callback)
{
#ifdef FEATURE_EFS2
    char             *outFileName = NULL;
    VIDEO_ENG_FILE   *temp_fp = NULL;
    uint32           temp_size;
    uint32           i;
    uint8            *temp_buffer;
    uint8            result;
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    uint32           numberofaudioframes = 1;
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

#endif /* FEATURE_EFS2 */
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
    /* If there is any left-over audio, write it out now. */
    if (g_context->audio_leftover && g_context->audio_leftover_bytes
        && !g_context->space_out)
    {
        /* If writing more than one frame per sample, we must put valid
        ** size information in the sample information structures.
        */
        g_context->audio_sample_info [0].size
            = g_context->audio_leftover_bytes;
        /*
           If sample size is set then calculate the number of audio frames
           in the left-over audio
        */ 
        numberofaudioframes = (g_context->audio_sample_size > 0)? 
         (g_context->audio_sample_info [0].size/g_context->audio_sample_size):
         1;                       
        g_context->audio_sample_info [0].delta
            = (VOCODER_SAMPLING_RATE / VOCODER_PACKET_RATE) * numberofaudioframes;
        g_context->audio_sample_info [0].sync = 1;

        /* Write the audio to the movie, via video format services. */
        g_context->videofmt_write_stream
            (g_context->audio_stream_num, 1,
             &g_context->audio_sample_info [0],
             g_context->audio_leftover,
             g_context->videofmt_server_data,
             video_eng_videofmt_callback, NULL);
        while ((g_context->videofmt_status != VIDEO_FMT_IO_DONE)
               && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
        {
            if (g_context->videofmt_continue == NULL)
            {
                CAMCORDER_MSG_ERROR ("handle_close: bad continue "
                           "function from videofmt!", 0, 0, 0);
                break;
            }
            else
            {
                g_context->videofmt_continue (g_context->videofmt_server_data);
            }
        }
        if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
        {
            g_context->videofmt_server_data = NULL;
            CAMCORDER_MSG_ERROR ("handle_close: failed to write audio", 0, 0, 0);
        }
    }
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

    /* Process the very last frame, if not in still-image mode. */
    if ((g_context->videofmt_status != VIDEO_FMT_FAILURE)
        && !g_context->space_out
        && !g_context->still_image
        && !g_context->no_coding
        && !g_context->framecb_ptr
        )
    {

        int32  last_frame_delta;
        last_frame_delta = (int32)g_context->video_time_increment;

        /* If any frames are dropped towards the end put that much time as
        delta for last frame. This is essential for AV duration match */
        if(g_context->frame_drop_info.frame_drop_duration ||
           g_context->frame_drop_info.frame_drop_duration_before_pause)
        {
           last_frame_delta = ((g_context->frame_drop_info.frame_drop_duration +
               g_context->frame_drop_info.frame_drop_duration_before_pause) *
               g_context->video_time_resolution + 500) / 1000;
        }
        video_eng_video_processing (0, FALSE, last_frame_delta);

    }

    /* Release movie writer, if not yet released. */
    video_eng_close_writer ();

    /* If writing a KDDI fragmented 3G2 file, reorder the last
    ** fragment, and set total duration in 'mehd' atom as described in the
    ** videofmt API.
    */
    if (g_context->file_brand == VIDEO_FMT_BRAND_FRAG_3G2)
    {
        /* Reorder last fragment and set total duration in movie. */
        result = video_eng_reorder_fragment (TRUE);
        if(!result)
        {
             g_context->delete_on_exit = TRUE;
        }
    }

    /* Mark movie file status as closed. */
    g_context->output_open = FALSE;

#ifdef FEATURE_EFS2
    /* Close the file now. */
    if ((g_context->output.method == VIDEO_METHOD_EFS)
        && (g_context->file_brand == VIDEO_FMT_BRAND_FRAG_3G2
           || (g_context->file_brand != VIDEO_FMT_BRAND_AMC 
              && g_context->reorder_atom == FALSE)))
    {
        CAMCORDER_MSG_HIGH ("handle_close: " "Closing file", 0, 0, 0);
        if (g_context->fs_handle != NULL)
        {
          (void) video_async_file_close (g_context->fs_handle);
          g_context->fs_handle = NULL;
        }
    }

    /* Delete the file in case of failure. */
    if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
    {
        if (g_context->output.method == VIDEO_METHOD_EFS)
        {
            CAMCORDER_MSG_HIGH ("handle_close: Deleting file", 0, 0, 0);
            (void) video_async_file_unlink (g_context->output.efs.filename, strlen (g_context->output.efs.filename));
        }
    }

    /* Otherwise, if writing an AMC file, re-write the output file to
    ** conform to AMC file format rules.
    */
    else if (((g_context->file_brand == VIDEO_FMT_BRAND_AMC)
            ||(g_context->reorder_atom == TRUE)) && (g_context->file_brand != VIDEO_FMT_BRAND_FRAG_3G2))
    {
        /* Select output file name based on output method. */
        switch (g_context->output.method)
        {
        case VIDEO_METHOD_EFS:
            /* Convert directly to temporary file. */
            outFileName = g_context->output.efs.filename;
            temp_fp = g_context->fs_handle;
            break;

        case VIDEO_METHOD_MEM:
        case VIDEO_METHOD_CLIENT:
            /* Write to second temporary file. */
            outFileName = g_context->temp_file_name_2;
            temp_fp = g_context->temp_handle_2;
            break;

        default:
            CAMCORDER_MSG_ERROR ("handle_close: wrong output method!", 0, 0, 0);
            break;
        }

        /* Convert file now. */
        (void)video_eng_file_fseek (g_context->temp_handle_1, 0, SEEK_SET);
        if (temp_fp == NULL)
        {
           CAMCORDER_MSG_ERROR ("handle_close: output/temp2 file is not opened",
                        0, 0, 0);
        }
        else
        {
           /* During transcoding, file writing is not done through videoeng
              So, in that case there is no need, to re-arrange atom for AMC file brand
              and to remove the temporary file which is not created*/
#ifndef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
           if(g_context->framecb_ptr != NULL)
           {
             (void) video_async_file_close (temp_fp);
             (void)video_async_file_unlink (outFileName,strlen (outFileName));
           }
           else
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
           {
             /* Invoke the reorder routine for the entire file. */
             result = video_eng_fast_amc_fixup_reorder
                      (g_context->temp_handle_1, 0,
                        g_context->output_size, temp_fp, 0, 
                        NULL, 0);

             (void) video_async_file_close (temp_fp);
             (void) video_async_file_close (g_context->temp_handle_1);
             g_context->use_temp_file_1 = FALSE;
             (void) video_async_file_unlink (g_context->temp_file_name_1,strlen (g_context->temp_file_name_1));
             g_context->temp_handle_1 = NULL;
             if (!result)
             {
                video_async_file_unlink (outFileName,strlen (outFileName));
             }
             /*
               Warn the user that re-order was not successfull.
               File is not deleted in case of fixup error case.
               This part of code provides information that an fixup error has
               occured.Will not take any action in response to error.
             */ 
             else if (result == FIXUPERROR)
             {
                 CAMCORDER_MSG_ERROR ("Fix up Error In camcorder",0, 0, 0);
                 g_context->callback_ptr (VIDEO_LIMIT_REACHED, g_context->client_data);
             }
           }
        }

        /* Depending on output method, deliver data to client. */
        switch (g_context->output.method)
        {
        case VIDEO_METHOD_MEM:
            /* Read file into memory buffer. */
            temp_fp = video_eng_efs_file_fopen
                          (g_context->temp_file_name_2, "r");
            if (temp_fp == NULL) break;
            (void) video_eng_file_fseek (temp_fp, 0, SEEK_END);
            temp_size = (uint32) video_eng_file_ftell (temp_fp);
            (void) video_eng_file_fread (g_context->output.mem.buf, 1,
                              MIN (g_context->output.mem.len, temp_size),
                              temp_fp);
            (void) video_async_file_close (temp_fp);
            (void) video_async_file_unlink (g_context->temp_file_name_2,strlen (g_context->temp_file_name_2));
            g_context->use_temp_file_2 = FALSE;
            break;

        case VIDEO_METHOD_CLIENT:
            /* Open temporary file and deliver data to client in chunks. */
            temp_fp = video_eng_efs_file_fopen
                          (g_context->temp_file_name_2, "r");
            if (temp_fp == NULL) break;
            (void) video_eng_file_fseek (temp_fp, 0, SEEK_END);
            temp_size = (uint32) video_eng_file_ftell (temp_fp);
            (void) video_eng_file_fseek (temp_fp, 0, SEEK_SET);
            temp_buffer = videoeng_malloc (1000);
            for (i = 0; i < temp_size; i += 1000)
            {
                (void) video_eng_file_fread (temp_buffer, 1,
                                  MIN (temp_size - i, 1000),
                                  temp_fp);
                (void) g_context->output.client.data_push_fn
                            (temp_buffer, MIN (temp_size - i, 1000), i,
                            g_context->output.client.client_data);
            }
            videoeng_free (temp_buffer);
            (void) video_async_file_close (temp_fp);
            (void) video_async_file_unlink (g_context->temp_file_name_2,strlen (g_context->temp_file_name_2));
            g_context->use_temp_file_2 = FALSE;
            break;

        default:
            /* Nothing wrong. Break. */
            break;
        }
    }
#endif /* FEATURE_EFS2 */

    /* If close failed, go through failure processing. */
    if (g_context->videofmt_status == VIDEO_FMT_FAILURE)
    {
        video_eng_enc_failure (FALSE);
    }

        /* Send a success status code back to our client. */
    if (!suppress_callback)
    {
      if(g_context->videofmt_status != VIDEO_FMT_FAILURE)
      {
         g_context->callback_ptr (VIDEO_SUCCESS, g_context->client_data);
      }
      else if(g_context->state == VIDEO_ENG_STATE_STOPPING || 
              g_context->state == VIDEO_ENG_STATE_EXITING    )
      { 
        /* When user has sent stop event also while there is already an error occured
         * e.g. user pressed stop immediately after removing MMC card while recording
         */
         g_context->callback_ptr (VIDEO_FAILURE, g_context->client_data);
      }
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_close_writer

DESCRIPTION
  This function releases all video format services writer resources.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The movie file meta data is written and the file is closed.

===========================================================================*/
void video_eng_close_writer (void)
{
    /* Free text sample entries, if allocated. */
    while (g_context->text_headers)
    {
        video_eng_text_sample_entry_type  *entry;

        entry = g_context->text_headers;
        g_context->text_headers = entry->next;

		// hence no interface is present for timed text, better not to free "entry->entry.fonts"
        /*
        if (entry->entry.fonts)
        {
            videoeng_free ((void *) entry->entry.fonts);
        }
        videoeng_free (entry);
        */
    }

    /* Free any text samples allocated. */
    while (g_context->text_samples)
    {
        video_eng_text_sample_type  *sample;

        sample = g_context->text_samples;
        g_context->text_samples = sample->next;
        if (sample->handle && g_context->videofmt_server_data)
        {
            g_context->videofmt_free_text
                (sample->handle, video_eng_videofmt_callback, NULL);
        }
        videoeng_free (sample);
    }

    /* Terminate video format services, if it is still running. */
    if (g_context->videofmt_server_data)
    {
        /* Terminate video format services. */
        g_context->videofmt_end (g_context->videofmt_server_data);
        while ((g_context->videofmt_status != VIDEO_FMT_DONE)
               && (g_context->videofmt_status != VIDEO_FMT_FAILURE))
        {
            if (g_context->videofmt_continue == NULL)
            {
                CAMCORDER_MSG_ERROR ("close_writer: bad continue "
                           "function from videofmt!", 0, 0, 0);
                break;
            }
            else
            {
                g_context->videofmt_continue (g_context->videofmt_server_data);
            }
        }
        g_context->videofmt_server_data = NULL;
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_reorder_fragment

DESCRIPTION
  This function reorders the fragment just written out by videofmt and passes
  the data along to its final destination.

DEPENDENCIES
  None.

RETURN VALUE
 TRUE : On sucess
 False: On failure

SIDE EFFECTS
  The movie file meta data is written and the file is closed.

===========================================================================*/
boolean video_eng_reorder_fragment (boolean last_fragment)
{
#ifdef FEATURE_EFS2
    VIDEO_ENG_FILE   *dest;
    uint32           out_size;
    uint32           i;
    uint8            *temp_buffer = NULL;
    uint32           ftyp_size = 0;
    uint8            result;

    CAMCORDER_MSG_HIGH ("reorder_fragment: ", 0, 0, 0);

    /* Select reorder output file - either the final destination, or the
    ** second temporary file.
    */
    if (g_context->output.method == VIDEO_METHOD_EFS)
    {
        dest = g_context->fs_handle;
    }
    else
    {
        dest = g_context->temp_handle_2;
    }
    if((dest == NULL) || (g_context->temp_handle_1 == NULL))
    {
     return FALSE;
    }
    /* Reset file pointer in temporary file back to the beginning, to read
    ** back the last fragment.
    */
    (void) video_eng_file_fseek (g_context->temp_handle_1, 0, SEEK_SET);

    if (last_fragment && g_context->fragment_number == 0)
    {
        /* Rewrite the 'ftyp' and fix the brands if the fragmented file has
        ** only 1 fragment.
        */
        const char  *major_brand;
        uint16      version_major;
        uint16      version_minor;
        uint32      num_compat_brands;
        const char  **compat_brands;

        #define STORE_ATOM_UINT16(buffer, offset, value) \
            (buffer) [(offset)] = (uint8) (((value) >> 8) & 0xFF); \
            (buffer) [(offset) + 1] = (uint8) ((value) & 0xFF)

        #define STORE_ATOM_UINT32(buffer, offset, value) \
            (buffer) [(offset)] = (uint8) (((value) >> 24) & 0xFF); \
            (buffer) [(offset) + 1] = (uint8) (((value) >> 16) & 0xFF); \
            (buffer) [(offset) + 2] = (uint8) (((value) >> 8) & 0xFF); \
            (buffer) [(offset) + 3] = (uint8) ((value) & 0xFF)

        #define STORE_ATOM_ID(buffer, offset, id) \
            (buffer) [(offset)] = (id) [0]; \
            (buffer) [(offset) + 1] = (id) [1]; \
            (buffer) [(offset) + 2] = (id) [2]; \
            (buffer) [(offset) + 3] = (id) [3]

        #define MAKE_ATOM_HEADER(buffer, size, type) \
            STORE_ATOM_UINT32 (buffer, 0, size); \
            STORE_ATOM_ID (buffer, 4, type)

        major_brand = _3G2_MAJOR_BRAND;
        num_compat_brands = sizeof (_3G2_COMPAT_BRANDS)
                          / sizeof (_3G2_COMPAT_BRANDS [0]);
        compat_brands = &_3G2_COMPAT_BRANDS [0];
        version_major = 0;
        version_minor = 0;

        ftyp_size = 16 + 4 * num_compat_brands;
        temp_buffer = (uint8 *)videoeng_malloc (ftyp_size);
        if(temp_buffer == NULL)
        {
          CAMCORDER_MSG_ERROR ("video_eng_reorder_fragment: malloc failure!!", 0, 0, 0);
          return FALSE;
        }
        /* Generate 'ftyp' atom containing brand information. */
        MAKE_ATOM_HEADER (temp_buffer, ftyp_size, "ftyp");
        memcpy (temp_buffer + 8, major_brand, 4);
        STORE_ATOM_UINT16 (temp_buffer, 12, version_major);
        STORE_ATOM_UINT16 (temp_buffer, 14, version_minor);
        for (i = 0; i < num_compat_brands; ++i)
        {
            memcpy (temp_buffer + 16 + 4 * i, compat_brands [i], 4);
        }
    }

    /* Reorder atoms in last fragment. */
    result = video_eng_fast_amc_fixup_reorder
                (g_context->temp_handle_1,
                g_context->frag_in_offset,
         g_context->output_size - g_context->frag_in_offset,
                dest, g_context->frag_out_offset,
                temp_buffer, ftyp_size);
    if (temp_buffer)
    {
        videoeng_free (temp_buffer);
        temp_buffer = NULL;
    }
    if(!result)
    {
      return FALSE;
    }

    /* Increment fragment number */
    ++g_context->fragment_number;

    /* Adjust fragment input offset. */
    g_context->frag_in_offset = g_context->output_size;

    /* Reset file pointer in temporary file back to the beginning, so that
    ** next fragment will start at beginning of temporary file.
    */
    (void) video_eng_file_fseek (g_context->temp_handle_1, 0, SEEK_SET);

    if (!last_fragment)
    {
        /* Return from here since reordering is done for this fragment
        ** and we need to hold this output to update mehd atom
        */
        return TRUE;
    }

    /* Determine amount of total output generated if this is the last fragment
    ** and if there are multiple fragments. 
    */
    out_size = (uint32) video_eng_file_ftell (dest);
    if (last_fragment && g_context->fragment_number > 1)
    {
        /* Set total duration in movie. */
        video_eng_update_mehd_atom(g_context->frag_total_duration);
    }
    g_context->frag_out_offset = out_size;

    /* If final destination is not EFS, deliver data to client.
    */
    if (g_context->output.method != VIDEO_METHOD_EFS)
    {
        (void) video_eng_file_fseek (dest, 0, SEEK_SET);

        /* Deliver data to client. */
        switch (g_context->output.method)
        {
        case VIDEO_METHOD_MEM:
            /* Read file into memory buffer. */
            (void) video_eng_file_fread (g_context->output.mem.buf, 1,
                                  MIN (g_context->output.mem.len,
                                        g_context->frag_out_offset),
                                  dest);
            break;

        case VIDEO_METHOD_CLIENT:
            /* Deliver data to client in chunks. */
            temp_buffer = videoeng_malloc (1000);
            if (temp_buffer)
            {
                for (i = 0; i < out_size; i += 1000)
                {
                    (void) video_eng_file_fread (temp_buffer, 1,
                                      MIN (out_size - i, 1000),
                                      dest);
                    (void) g_context->output.client.data_push_fn
                                (temp_buffer, MIN (out_size - i, 1000), i,
                                g_context->output.client.client_data);
                }
                videoeng_free (temp_buffer);
            }
            else
            {
                video_eng_enc_failure (TRUE);
            }
            break;

        default:
            /* Nothing wrong. Break. */
            break;
        }

    }
#endif /* FEATURE_EFS2 */
   return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_update_mehd_atom

DESCRIPTION
  This function locates the 'mehd' atom in the output file, and writes the
  given value in for the "fragment_duration" field of that atom.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_update_mehd_atom (uint32 total_duration)
{
#ifdef FEATURE_EFS2
    uint32           file_pos = 0;
    uint32           atom_pos = 0;
    const char       * const atom_list[] = {"moov", "mvex", "mehd"};
    const uint32     num_atoms = sizeof (atom_list) / sizeof (*atom_list);
    char             buffer [8];
    uint32           atom_size;
    uint32           temp32;
    VIDEO_ENG_FILE   *dest;
    uint32           frag_out_offset;
    int              errno = 0;

    CAMCORDER_MSG_HIGH ("update_mehd: ", 0, 0, 0);

    /* Select the output file - either the final destination, or the
    ** second temporary file.
    */
    if (g_context->output.method == VIDEO_METHOD_EFS)
    {
        dest = g_context->fs_handle;
    }
    else
    {
        dest = g_context->temp_handle_2;
    }

    /* Get size of output */
    frag_out_offset = (uint32) video_eng_file_ftell (dest);

    /* Loop until 'mehd' atom found.  It is located inside 'mvex', which in
    ** turn is located inside 'moov'.
    */
    while (atom_pos < num_atoms)
    {
        /* Check for unexpected end of file. */
        if (file_pos > frag_out_offset - 8)
        {
            CAMCORDER_MSG_HIGH ("update_mehd: There's no 'mehd' atom", 0, 0, 0);
            return;
        }

        /* Seek to atom position */
        if (video_eng_file_fseek (dest, (int32) file_pos, SEEK_SET) != 0)
        {
            errno = efs_errno;
            CAMCORDER_MSG_ERROR ("update_mehd: seek error '%d'",
                                  errno, 0, 0);
            return;
        }

        /* Get atom size and ID */
        if (video_eng_file_fread (buffer, 1, 8, dest) != 8)
        {
            errno = efs_errno;
            CAMCORDER_MSG_ERROR ("update_mehd: read error '%d'",
                                 errno, 0, 0);
            return;
        }

        /* Extract atom size. */
        /* Ignore lint error in the following line:
           "'buffer' may not have been initialized"
        */
        atom_size = n2hl (* (const uint32 *) buffer); /*lint !e644 */

        /* Skip to next atom unless its ID matches the ID of the atom for
        ** which we're currently looking.
        */
        if (memcmp (buffer + 4, atom_list [atom_pos], 4))
        {
            file_pos += atom_size;
        }

        /* Otherwise, open this atom. */
        else
        {
            file_pos += 8;
            ++atom_pos;
        }
    }

    /* Move to the offset of the 'total_duration' field in the atom, as
    ** described in the videofmt API.
    */
    file_pos += 4;  /* skip version and flags fields */
    /* Seek to atom position */
    if (video_eng_file_fseek (dest, (int32) file_pos,
                              SEEK_SET) != 0)
    {
        errno = efs_errno;
        CAMCORDER_MSG_ERROR ("update_mehd: seek error '%d'",
                             errno, 0, 0);
        return;
    }

    /* Store the given total duration in the atom
    */
    temp32 = n2hl (total_duration);

    if (video_eng_file_fwrite ((void *) &temp32, 1, 4,
                        dest) != 4)
    {
        errno = efs_errno;
        CAMCORDER_MSG_ERROR ("update_mehd: write error '%d'",
                             errno, 0, 0);
        return;
    }
#endif /* FEATURE_EFS2 */
}
/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_find_save_location

DESCRIPTION
  Determines the save location

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_find_save_location(const char *path)
{
  if(!strncmp (path, arr_dir[1], strlen (arr_dir[1])))
    g_context->save_location  = VIDEO_ENG_DEST_MMC;
  else if(!strncmp (path, arr_dir[2], strlen (arr_dir[2])))
    g_context->save_location  = VIDEO_ENG_DEST_FTL;
  else if(!strncmp (path, arr_dir[3], strlen (arr_dir[3])))
    g_context->save_location  = VIDEO_ENG_DEST_USB;
  else
    g_context->save_location  = VIDEO_ENG_DEST_FLASH;
  return;
}


#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API


//#define FEATURE_FRAME_DEBUG

#ifdef FEATURE_FRAME_DEBUG
  /* Most functions in video eng frame provide detailed error message already
  */
  #define MSG_FRAME_ERROR CAMCORDER_MSG_ERROR
  #define MSG_FRAMEPROC(x,a,b,c) CAMCORDER_MSG_ERROR(x,a,b,c)

#else

  #define MSG_FRAMEPROC(x,a,b,c)
  #define MSG_FRAME_ERROR CAMCORDER_MSG_ERROR

#endif

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_frame_init

DESCRIPTION
  Initializes global variables for video_eng_frame.
  Returns VIDENG_FRAME_SUCCESS if everthing was OK.
  Can be called from any task.

DEPENDENCIES
  FEATURE_VIDEO_ENG_FRAME_PROCESSING_API is defined.

RETURN VALUE
  VIDEO_FAILURE if something went wrong.

SIDE EFFECTS


===========================================================================*/
LOCAL video_eng_frame_stat_type video_eng_frame_init(void)
{
  video_eng_frame_stat_type             retStat = VIDENG_FRAME_SUCCESS;
  video_eng_frame_context_type *local_g_frame = NULL;

  if(g_context == NULL )
  {
    /* don't even think about initializing video_eng_frame if video_eng is not
    ** started
    */
    MSG_FRAME_ERROR("vidoe_eng_frame: must initializing video_eng first",
                    0,0,0);
    retStat =  VIDENG_FRAME_INIT_BAD_CONTEXT;
  }
  else if ( g_frame != NULL ) {
    if(g_frame->terminate)
    { /* previous call to video_eng_frame_terminate() was unsuccessful
      */
      MSG_FRAME_ERROR("previous video_eng_frame not terminated",0,0,0);
      MSG_FRAMEPROC("FRAME:.... proceeding with caution",0,0,0);
      /* terminate flag will be reset later */
    }
    else
    {
      MSG_FRAME_ERROR("video_eng_frame_init: already initialized",0,0,0);
      retStat = VIDENG_FRAME_SECOND_INIT;
    }
  }

  if(retStat  == VIDENG_FRAME_SUCCESS )
  {
    if(g_frame == NULL )
      local_g_frame = (video_eng_frame_context_type *)
                videoeng_malloc(sizeof(*g_frame));
    if(local_g_frame == NULL )
    {
      MSG_FRAME_ERROR("Video_eng_frame_init allocate failure",0,0,0);
      retStat = VIDENG_FRAME_MALLOC_CONTEXT;
    }
    else
    {
      memset(local_g_frame,0,sizeof(video_eng_frame_context_type));
    }
  }

  if(retStat == VIDENG_FRAME_SUCCESS )
  {
    rex_init_crit_sect(&videng_frame_cs);

    rex_enter_crit_sect(&videng_frame_cs);
    INTLOCK();

    /* allocate the global frame context */
    if(g_frame == NULL )
      g_frame = local_g_frame;

    /* initialization success ... set initial variables */
    g_frame->be_state  = VIDEO_ENG_RAW_ENCODE_IDLE;
    g_frame->enc_state = VIDEO_ENG_RAW_ENCODE_IDLE;
    g_frame->terminate = FALSE;

    INTFREE();
    rex_leave_crit_sect(&videng_frame_cs);
  }
  return retStat;
}  /* video_eng_frame_init */

void video_eng_process_clkcb()
{
  video_eng_frame_context_type *cached_g_frame = NULL;

  MSG_FRAMEPROC("FRAME: video_eng_frame_clk expired",0,0,0);

  rex_enter_crit_sect(&videng_frame_cs);
  INTLOCK();

  if(g_frame != NULL )
  {
    clk_dereg (&g_frame->clk);
    clk_dereg(&g_frame->backend_timeout_clk);
    g_frame->be_timer_blocked = FALSE;
    cached_g_frame = g_frame;
    g_frame = NULL;
  }

  INTFREE();
  rex_leave_crit_sect(&videng_frame_cs);

  if(cached_g_frame != NULL )
  {
    MSG_FRAMEPROC("FRAME: terminated by clocked termination",0,0,0);
    cached_g_frame->raw_encode_client_cb(VIDEO_FAILURE,cached_g_frame->raw_encode_client_data);
    videoeng_free(cached_g_frame);
  }
  else
  {
    MSG_FRAMEPROC("FRAME: clock did nothing upon expiration",0,0,0);
  }
}

void video_eng_frame_clk_cb(int32 ms)
{

   MSG_FRAMEPROC("FRAME: video_eng_frame_clk expired",0,0,0);
   graph_video_eng_queue_cmd(VIDENG_GRAPH_TIMER_CB,NULL);
}  /* video_eng_frame_clk_cb */ /*lint !e715 'ms'  not used */


LOCAL video_eng_frame_stat_type video_eng_frame_terminate(void)
{
  video_eng_frame_stat_type retStat = VIDENG_FRAME_SUCCESS;
  video_eng_frame_context_type *cached_g_frame = NULL;

  INTLOCK();
  rex_enter_crit_sect(&videng_frame_cs);

  if(g_frame == NULL )
  {
    MSG_FRAME_ERROR("video_eng_frame_terminate already terminated",0,0,0);
    retStat = VIDENG_FRAME_ALREADY_TERMINATED;
  }
  else
  {

    if ( VIDEO_ENG_RAW_ENCODE_BUSY == g_frame->be_state ||
         VIDEO_ENG_RAW_ENCODE_BUSY == g_frame->enc_state )
    {
      MSG_FRAMEPROC("FRAME: process busy ... video_eng cannot terminate now",0,0,0);


      if(!g_frame->terminate)
      {
        /* if we're terminating and there is a request to terminate then
        ** we set to VIDENG_FRAME_TERMNATE_REQUESTED to request a clocked
        ** termination
        */
        retStat            = VIDENG_FRAME_TERMINATE_REQUESTED;
        g_frame->terminate = TRUE;
      }
      else
      {
        /* This is the case when we alread started the clock and a
        ** terminate came in before the clock is hit. But this one, too,
        ** is not successful
        */
        MSG_FRAMEPROC("FRAME: ... but clock already set ... do nothing",0,0,0);
        retStat            = VIDENG_FRAME_CLOCKED_TERMINATE_ALREADY_SET;
      }
    }
    else
    {
      /* successful termination */
      MSG_FRAMEPROC("FRAME: terminate successful",0,0,0);
      cached_g_frame = g_frame;
      clk_dereg(&g_frame->backend_timeout_clk);
      clk_dereg (&g_frame->clk);
      g_frame->be_timer_blocked = FALSE;
      g_frame = NULL;
    }
  }
  rex_leave_crit_sect(&videng_frame_cs);
  INTFREE();

  if( retStat == VIDENG_FRAME_SUCCESS)
  {
    videoeng_free(cached_g_frame);
  }
  else if(retStat == VIDENG_FRAME_TERMINATE_REQUESTED )
  {
    MSG_FRAMEPROC("FRAME: setting clock",0,0,0);

    clk_def(&g_frame->clk);

    /* give the process 300 msec to complete.
    ** if not terminated in 300 msec then terminate right away
    */
    clk_reg(&g_frame->clk,video_eng_frame_clk_cb, 300,0,FALSE);

  }

  return retStat;
}  /* video_eng_frame_terminate */


LOCAL static void video_eng_be_timeout_cb(int32 ms)
{
  boolean bISR = rex_is_in_irq_mode();

  if(!bISR)
    INTLOCK();

  /* Initiate timer BE failure if DSP has not responded at all */
  if(!g_frame->be_timer_blocked)
  {
    /* timeout expired for BE --> assume DSP is dead */
    graph_video_eng_queue_cmd(VIDENG_GRAPH_BE_FAILURE,NULL);
  }

  if(!bISR)
    INTFREE();

}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_frame

DESCRIPTION
  This procedure encodes a single raw frame.
  The input values and data format are specified in an IPL matrix.

  Allowed function:
     Process Special Effects and return: The input memory buffer will
        be replaced by the IPL output.

     Process Special Effects and send to video_encoder.  In this case
        video_eng_init() and video_eng_start() must first have been called.
        If special effects pointer is NULL the frame will be passed to
        video_encoder without any special effects.

  Special effects will be processed in the VS task, video_encoder will run
  in the GRAPH task. When finished callback_ptr() will be called with the
  appropriate result (VIDEO_SUCCESS, etc..) and the client_data will be
  returned.

  This function can be called from the ISR.


DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
void video_eng_frame(
  uint8           *Y,       /* input image buffer */
  uint8           *CrCb,    /* chroma component of image */
  ipl_col_for_type type,    /* image type         */
  uint32          msec,    /* render time -- neded by video_encode and     */
                           /* certain special effects engines              */
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
)
{
   boolean bISR   = rex_is_in_irq_mode();       /* true if we're in the ISR */
   boolean bError = FALSE;

   /* provide a QXDM message with the rex_self task_name
   ** But how will we know if we're in the ISR?
   */
#ifdef FEATURE_FRAME_DEBUG
   if(bISR)
   {
     MSG_FRAMEPROC("FRAME: video_eng_frame from ISR",0,0,0);
   }
   else
   {
       msg_const_type m1;

       m1.desc.line    = rex_self()->pri;
       m1.desc.ss_id   = MSG_SSID_DFLT;
       m1.desc.ss_mask = MSG_LEGACY_ERROR;
       m1.fname        = "video_eng_frame taskpri and name:";
       m1.fmt          = rex_self()->task_name;

       msg_send_3(&m1,0,0,0);
   }
#endif

  MSG_FRAMEPROC("FRAME: video_eng_frame %d Y= %x",mp4e_get_time_ms(),Y,0);

  if(callback_ptr == NULL )
  { MSG_FRAMEPROC("FRAME: video_eng_frame called with NULL callback function",0,0,0);
    ASSERT(0);  /* hopefully this will generate an abort */
    return;
  }

  /* If video engine has indicated space out, return immediately without cb */
  if(g_context == NULL || g_context->space_out)
  {
    CAMCORDER_MSG_HIGH("video_eng_frame detects video engine space out",0,0,0);
    return;
  }

  if(!bISR)
    rex_enter_crit_sect(&videng_frame_cs);
  else
  {
    if ( videng_frame_cs.lock_count > 0 )
    {
      /* video_eng_frame was called from the ISR while another REX task has
      ** critical section lock
      */
      MSG_FRAMEPROC("FRAME: video_eng_frame called from ISR while in rex lock",
                    0,0,0);
      /* print out a QXDM error message from the GRAPH context */
      graph_video_eng_queue_cmd(VIDENG_GRAPH_FRAME_ISR_DURING_REX_LOCK,NULL);

      bError = TRUE;
    }
  }

  if(!bError &&
     ( NULL == g_context ||      /* video_eng terminated */
       g_context->terminate ||   /* video_eng error occurred, terminate reqt'd */
       NULL == g_frame ||        /* video_eng_frame not initialized */
       g_frame->terminate ||     /* terminate requested */
       g_context->state != VIDEO_ENG_STATE_RECORDING
     )
    )
  {
    MSG_FRAMEPROC("FRAME: video_eng_frame failed to start",0,0,0);

    /* if MSG_FRAMEPROC is defined we give all the gory details
    ** Usually, after debugging we turn off MSG_FRAMEPROC to aviod this waste
    **   of codespace.
    */
    #ifdef FEATURE_FRAME_DEBUG /* Added to avoid lint warning# 548 */
    if(g_context == NULL )    MSG_FRAMEPROC("FRAME: g_context is NULL"       ,0,0,0);
    if(g_frame == NULL )      MSG_FRAMEPROC("FRAME: g_frame is NULL"         ,0,0,0);
    if(g_context->terminate ) MSG_FRAMEPROC("FRAME: g_context->terminate set",0,0,0);
    if(g_frame->terminate)    MSG_FRAMEPROC("FRAME: g_frame->terminate set"  ,0,0,0);
    if(g_context->state != VIDEO_ENG_STATE_RECORDING)
                              MSG_FRAMEPROC("FRAME: video engine wront state",0,0,0);
    #endif

    /* print out a QXDM error message from the GRAPH context */
    graph_video_eng_queue_cmd(VIDENG_GRAPH_CANNOT_START_FRAME,NULL);

    bError = TRUE;
  }

  /* if ENC is in READY state, that means there is a BE output in Ping-Pong
  ** ready to process.  Otherwise if ENC is in IDLE state it will stay
  ** idle in this cycle.
  */
  if (!bError && g_frame->enc_state == VIDEO_ENG_RAW_ENCODE_READY ) {
    MSG_FRAMEPROC("FRAME: enc_state_waiting",0,0,0);
    g_frame->enc_state = VIDEO_ENG_RAW_ENCODE_WAITING;
  }

  /* BE state should never be BUSY at this point.
  ** Actually the only allowed state at this point is IDLE
  */
  if(!bError )
  {
    if (  VIDEO_ENG_RAW_ENCODE_BUSY == g_frame->be_state ) {
      /* can't process frame because we're busy */
      MSG_FRAMEPROC("FRAME: video_eng_frame() called before handshake sent",0,0,0);

      /* print out a QXDM error message from the GRAPH context */
      graph_video_eng_queue_cmd(VIDENG_GRAPH_FRAME_CALLED_BEFORE_HANDSHAKE,NULL);
      bError = TRUE;
    }
    else
    {
      uint16 current_idx;

      /* this is a temporary cache for calling parameters associated with this
      ** frame.  This will be attached to the ping-pong buffer for the frame
      ** when BE completes
      */
      g_frame->raw_encode_client_cb   = callback_ptr;
      g_frame->raw_encode_client_data = client_data;
      g_frame->raw_encode_msec        = msec;

      current_idx = g_frame->next_enc_pingpong_location;

      if (Y != NULL) {

        g_frame->be_state = VIDEO_ENG_RAW_ENCODE_BUSY;

        /* the client_cb is pointing to what it was from the last time this ping
        ** pong buffer was used (probably 2 frames ago).  Clear this in case
        ** a bug fails to catch and it gives a callback using the wrong cb-func.
        */
        g_frame->enc_pingpong_bufs[current_idx].client_cb
                          = NULL;

        /* get the backend going */
        if ( CAMQDSP_SUCCESS !=
             VideoEncDSP_VideoEncRawFrame(Y,CrCb,type,VideoEncDSP_rawEncodeCb))
        {
          g_frame->be_state = VIDEO_ENG_RAW_ENCODE_FAILED;

          MSG_FRAMEPROC("FRAME: videoengRawFrame failed",0,0,0);

          /* print out a QXDM error message from the GRAPH context */
          graph_video_eng_queue_cmd(VIDENG_GRAPH_RAW_FRAME_FAILED,NULL);

          /* don't know why this failed */
          bError = TRUE;
        }
        else
        {
           /* To make this code robust to DSP failure we give the DSP
           ** 900 msec to respond
           */
           clk_def(&g_frame->backend_timeout_clk);
           clk_reg(&g_frame->backend_timeout_clk,
                     video_eng_be_timeout_cb,900,0,FALSE);
        }

      } else {

        /* video_eng_frame() is called without a valid YUV buffer. One reason
        ** is this could be a call to flush the existing frame in ping-pong
        ** buffer becaues the last frame was already sent to video_eng_frame().
        ** But there could be other reasons for this too.
        */

        g_frame->be_state = VIDEO_ENG_RAW_ENCODE_IDLE;
        g_frame->enc_pingpong_bufs[current_idx].client_cb   = callback_ptr;
        g_frame->enc_pingpong_bufs[current_idx].client_data = client_data;

        if(g_frame->enc_state == VIDEO_ENG_RAW_ENCODE_IDLE )
        {
          /* there is nothing to do  ... just handshake and leave*/
          graph_video_eng_queue_cmd(VIDENG_GRAPH_HANDSHAKE,NULL);
        }
      }
    }
    /* ENC will be processed in the GRAPH task */
    if (g_frame->enc_state != VIDEO_ENG_RAW_ENCODE_IDLE &&
        g_context->framecb_ptr == NULL)
    {
      graph_video_eng_queue_cmd(VIDENG_GRAPH_FRAME_CMD,NULL);
    }

  }
  if(!bISR)
    rex_leave_crit_sect(&videng_frame_cs);

  if(bError )
    callback_ptr(VIDEO_FAILURE,client_data);

} /* end of video_eng_frame() */


/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_process_frame

DESCRIPTION
  GRAPH-task routine to start the video_encoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_process_frame(void)
{
  uint16 current_idx;
  boolean bSendHandshake = FALSE;
  boolean bError         = FALSE;
  boolean bEncodeFrame   = FALSE;

  /* argument to video_encode_frame*/
  //video_encode_frame_arg_type encode_arg;
  struct {
    uint16 *buf_ptr;
    uint32  time;
    uint32  buffer_throughput;
  } encode_arg;


  MSG_FRAMEPROC("FRAME: Entered video_eng_process_frame",0,0,0);

  rex_enter_crit_sect(&videng_frame_cs);

  if(NULL == g_frame )
  {
    /* g_frame was terminated before videng_frame was called
    ** This should be protected but it could happen if the termination
    ** occurred as a result of video_eng_frame() terminating timer
    */
    CAMCORDER_MSG_ERROR("video_eng_process_frame: g_frame is NULL",0,0,0);
    CAMCORDER_MSG_ERROR("?? terminated by the g_frame terminating timer?",0,0,0);
    bError = TRUE;
  }
  else if(NULL == g_context)
  {
    /*while running in this code if due to some error(say backend failure) 
        * cleanup starts then g_context will be freed before being accessed here
        * so need to check whether its NULL or Not
        */
    CAMCORDER_MSG_ERROR("video_eng_process_frame : g_context is NULL",0,0,0);
    bError = TRUE;
  }
  else
  {
    int32 cur_time;

    ASSERT(g_frame->enc_state != VIDEO_ENG_RAW_ENCODE_BUSY);
    ASSERT(g_frame->enc_state != VIDEO_ENG_RAW_ENCODE_READY);

    if (g_frame->enc_state == VIDEO_ENG_RAW_ENCODE_WAITING ) {

      MSG_FRAMEPROC("FRAME: encoding frame %d",mp4e_get_time_ms(),0,0);

      g_frame->enc_state = VIDEO_ENG_RAW_ENCODE_BUSY;

      /* the frame we want to encode is in the other pingpong buffer */
      current_idx = 1 - g_frame->next_enc_pingpong_location;
      cur_time = mp4e_get_time_ms();

      encode_arg.buf_ptr =
        g_frame->enc_pingpong_bufs[current_idx].encode_buffer;
      encode_arg.time =
        g_frame->enc_pingpong_bufs[current_idx].msec;
        if(cur_time - g_context->video_start_time < 3000 || !g_context->dynamic_bitrate){
           /* Less than 3 seconds since recording started */
           /* or invalid bitrate                          */
           encode_arg.buffer_throughput = g_context->video_target_bitrate;
        }
        else{
      if( g_context->max_bitrate_enable)
          { 
            encode_arg.buffer_throughput = g_context->dynamic_bitrate *1024;
          }
          else if(g_context->dynamic_bitrate * 1024 > g_context->video_target_bitrate)
          { 
            /* if bitrate we got is more than target bitrate */
            /* use target bitrate                            */
            encode_arg.buffer_throughput = g_context->video_target_bitrate;
          }
          else if(g_context->dynamic_bitrate_enable)
          {
            /* use the updated bitrate                       */
            encode_arg.buffer_throughput = g_context->dynamic_bitrate *1024;
          }
          else{
           encode_arg.buffer_throughput = g_context->video_target_bitrate * 1024;
          }
       }

      bEncodeFrame = TRUE;
    }
  }
  rex_leave_crit_sect(&videng_frame_cs);

  if( /* !bError && */ bEncodeFrame)
    video_encode_frame((uint8 *) &encode_arg);

  if(!bError)
  {
    rex_enter_crit_sect(&videng_frame_cs);
    /* if an error is encountered in video_encode_frame()
    ** it will go through the frame callback which will
    ** notify the client through the VS-task.
    **
    ** So we're never notified of a video_encode_frame() failure.
      */

    /* However if the error is caused by video engine terminating
    ** then we need to catch it.
    */
    if(NULL == g_frame )
    {
      /* this might happen because of a delayed timer-induced termination
      ** .... and there is nothing we can do about it.
      */
      bError = TRUE;
    }
    else
    {
      if(NULL == g_context        /* video_eng terminated */
         // || g_context->state != VIDEO_ENG_STATE_RECORDING
        )
      {
        CAMCORDER_MSG_ERROR("Encoder result is suspect",0,0,0);
        g_frame->enc_state = VIDEO_ENG_RAW_ENCODE_FAILED;
      }

      /*
      ** At this point ENC state could be VIDEO_ENG_RAW_ENCODE_BUSY
      ** or VIDEO_ENG_RAW_ENCODE_IDLE (if there is nothing to do)
      ** or VIDEO_ENG_RAW_ENCODE_FAILED in the case of a failure detectable
      ** by video_eng_process_frame.
      */
      if(g_frame->enc_state == VIDEO_ENG_RAW_ENCODE_BUSY)
      {
        MSG_FRAMEPROC("FRAME: done encoding frame %d",mp4e_get_time_ms(),0,0);
        g_frame->enc_state = VIDEO_ENG_RAW_ENCODE_DONE;
      }
      /* if BE is also done then we need to send the handshake */
      if ( g_frame->be_state & ( VIDEO_ENG_RAW_ENCODE_DONE |
                               VIDEO_ENG_RAW_ENCODE_FAILED |
                               VIDEO_ENG_RAW_ENCODE_IDLE
                             )
      ) {
        MSG_FRAMEPROC("FRAME: going to handshake from video_eng_process_frame()",0,0,0);
        bSendHandshake = TRUE;
      }
    }

    rex_leave_crit_sect(&videng_frame_cs);
  }

  if(bSendHandshake &&
     g_context && g_context->framecb_ptr == NULL)
    video_eng_handshake();
}


/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_graph_cmd_parser

DESCRIPTION
  Graph-task command parser to process commands sent from the DSP interface
  layer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_graph_cmd_parser(graph_video_eng_msg_enum msg,
                                const void *resp_input)
{
  static int error_count = 0;
  LOCAL void video_eng_handshake(void);


  switch(msg)
  {
  case VIDENG_GRAPH_FRAME_CMD:
    video_eng_process_frame();
    break;
  case VIDENG_GRAPH_FRAME_DONE:
    video_eng_process_frame_done();
    break;
  case VIDENG_GRAPH_HANDSHAKE:
    video_eng_handshake();
    break;

  case VIDENG_GRAPH_BE_FAILURE:
    video_eng_process_dsp_failure();
    break;
  case VIDENG_GRAPH_TIMER_CB:
      video_eng_process_clkcb();
    break;
  case VIDENG_GRAPH_SELECTION_ERROR:
  case VIDENG_GRAPH_ERROR:
  case VIDENG_GRAPH_DSP_ERROR_AHB_OVERFLOW:
  case VIDENG_GRAPH_DSP_ERROR_ILLEGAL_CMD:
    error_count++;
    CAMCORDER_MSG_LOW("DSP error",0,0,0);
    if ((error_count & 0x7) == 0 ) {
      CAMCORDER_MSG_ERROR("DSP error",0,0,0);
    }
    break;

  case VIDENG_GRAPH_INIT:
    CAMCORDER_MSG_LOW("initializing video engine",0,0,0);
    break;

  case VIDENG_GRAPH_INIT_ERROR:
    CAMCORDER_MSG_ERROR("InitializaTION FAILED",0,0,0);
    break;

  case VIDENG_GRAPH_SELECTION_DONE:
    error_count = 0;
    CAMCORDER_MSG_LOW ("Video Encode Selection Done.", 0, 0, 0);
    break;

  case VIDENG_GRAPH_SLEEP_DONE:
    CAMCORDER_MSG_LOW ("VIdeo Encode Sleep Done.",0,0,0);
    break;

  case VIDENG_GRAPH_FRAME_FAIL:
    break;

  case VIDENG_GRAPH_CANNOT_START_FRAME:
    CAMCORDER_MSG_ERROR("video_eng_frame: Unable to process frame",0,0,0);
    break;

  case VIDENG_GRAPH_RAW_FRAME_FAILED:
    CAMCORDER_MSG_ERROR("video_eng_frame(): VideoEncDSP_VideoEncRawFrame failed"
              ,0,0,0);
    break;

  case VIDENG_GRAPH_FRAME_ISR_DURING_REX_LOCK:
    CAMCORDER_MSG_FATAL("video_eng_frame called from ISR while in REX critical section",
              0,0,0);
    break;

  case VIDENG_GRAPH_FRAME_CALLED_BEFORE_HANDSHAKE:
    CAMCORDER_MSG_ERROR("video_eng_frame() called before handshake sent",0,0,0);
    break;
  case VIDENG_GRAPH_MAX:
    break;

  }

}  /* video_eng_graph_cmd_parser */ /*lint !e715 'resp_input' not used*/


/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_rawEncodeFrameDoneCb_isr

DESCRIPTION
  Decode Done callback for BackEnd initiated from video_encode_frame().

  This routine runs in the ISR context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_rawEncodeFrameDoneCb_isr(
  CAMQDSP_ReturnCodeType result,
  CAMQDSP_ResponseType *response
)
{
  uint16 next_location;
  int i;

  MSG_FRAMEPROC("FRAME: frame_done (ISR)",0,0,0);


  if(g_frame == NULL || result != CAMQDSP_SUCCESS )
  {
    MSG_FRAMEPROC("FRAME: backend failed of g_frame NULL",0,0,0);

    graph_video_eng_queue_cmd(VIDENG_GRAPH_BE_FAILURE,NULL);
    return;
  }

  /* There was a mempcy in CAMQDSP_isr to get this data from QDSP to ARM.
  ** Unfortunately, this memory resides as a local variable of CAMQDSP_isr.
  ** We will loose this as soon as we return from ISR.
  **
  ** Therefore we need to copy it
  */
  next_location = g_frame->next_enc_pingpong_location;
  for(i=0; i<8; i++)
    g_frame->enc_pingpong_bufs[next_location].encode_buffer[i] =
    response->responsePayload.mybuf[i];
  //memcpy(g_frame->enc_pingpong_bufs[next_location].encode_buffer,
  //       &response->responsePayload.mybuf[0],
  //       sizeof(response->responsePayload.mybuf)
  //       );

  /* The rest can be done in GRAPH */
  graph_video_eng_queue_cmd(VIDENG_GRAPH_FRAME_DONE,(void *) response);

  /* Set flag to indicate lossless encoding done */
  g_frame->be_timer_blocked = TRUE;
}


LOCAL void video_eng_process_frame_done(void)
{
  boolean bSendHandshake = FALSE;
  uint16 next_location;

  if(g_frame == NULL)
  {
    MSG_FRAMEPROC("FRAME: g_frame NULL",0,0,0);
    return;
  }

  next_location = g_frame->next_enc_pingpong_location;

  /* Reset the timer, indication of QDSP came back ok */
  clk_dereg(&g_frame->backend_timeout_clk);
  g_frame->be_timer_blocked = FALSE;


  /* If be_state set to anything else but BUSY, some error recovery
  ** mechanism has started. Ignore DSP response so recovery can complete */
  if(g_frame->be_state != VIDEO_ENG_RAW_ENCODE_BUSY)
  {
    CAMCORDER_MSG_ERROR("FRAME: g_frame->be_state %d, ignore DSP frame done",
                        g_frame->be_state,0,0);
    return;
  }

  ASSERT(g_frame->enc_pingpong_bufs[next_location].client_cb == NULL);

  /* the encode_buffer is already done at the ISR level.
  */
  g_frame->enc_pingpong_bufs[next_location].client_cb =
     g_frame->raw_encode_client_cb;
  g_frame->enc_pingpong_bufs[next_location].client_data =
    g_frame->raw_encode_client_data;
  g_frame->enc_pingpong_bufs[next_location].msec =
    g_frame->raw_encode_msec;

  rex_enter_crit_sect(&videng_frame_cs);

  g_frame->be_state = VIDEO_ENG_RAW_ENCODE_DONE;

  if (g_frame->enc_state &
        ( VIDEO_ENG_RAW_ENCODE_DONE |   /* encoder finished successfully */
          VIDEO_ENG_RAW_ENCODE_IDLE |   /* encoder never started this cycle */
          VIDEO_ENG_RAW_ENCODE_FAILED ) /* encoder finished with error */
     )
  {
    MSG_FRAMEPROC("FRAME: queing handshake from ISR %d",mp4e_get_time_ms(),0,0);
    bSendHandshake = TRUE;
  }

  rex_leave_crit_sect(&videng_frame_cs);

  if(bSendHandshake)
    video_eng_handshake();

  if (g_frame && g_frame->enc_state != VIDEO_ENG_RAW_ENCODE_IDLE &&
      g_context && g_context->framecb_ptr != NULL)
  {
       g_frame->enc_state = VIDEO_ENG_RAW_ENCODE_WAITING;
       graph_video_eng_queue_cmd(VIDENG_GRAPH_FRAME_CMD,NULL);
  }

}
/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_rawEncodeConfigCompleteErrorCb_isr

DESCRIPTION
  ConfigDone callback returned an error.

  This routine runs in the ISR context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_rawEncodeConfigCompleteErrorCb_isr(void)
{
  MSG_FRAMEPROC("FRAME: error ISR",0,0,0);

  graph_video_eng_queue_cmd(VIDENG_GRAPH_BE_FAILURE,NULL);
}

void video_eng_process_dsp_failure(void)
{
  boolean bSendHandshake = FALSE;

  rex_enter_crit_sect(&videng_frame_cs);

  if(g_frame != NULL )
  {
    g_frame->be_state = VIDEO_ENG_RAW_ENCODE_FAILED;

    if (g_frame->enc_state & ( VIDEO_ENG_RAW_ENCODE_DONE |
                               VIDEO_ENG_RAW_ENCODE_IDLE |
                               VIDEO_ENG_RAW_ENCODE_FAILED )
        )
    {
      bSendHandshake = TRUE;
    }
  }

  rex_leave_crit_sect(&videng_frame_cs);

  if(bSendHandshake)
    graph_video_eng_queue_cmd(VIDENG_GRAPH_HANDSHAKE,NULL);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_handshake

DESCRIPTION
  GRAPH-task routine to send handshake to client, and perform assorted
  housekeeping routines.

  It also handles the swapping of the Ping-Pong encode and prediction VOPs.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void video_eng_handshake(void)
{
  uint16 i;
  boolean fail_sent_flag = FALSE;
  boolean terminate_flag = FALSE;
  boolean error_flag     = FALSE;
  video_cb_func_ptr_type   cached_cb = NULL;
  void *                   cached_client_data = NULL;
#ifdef FEATURE_VIDEO_CLIP_INFO
  boolean need_framestat = FALSE;
#endif //FEATURE_VIDEO_CLIP_INFO

  rex_enter_crit_sect(&videng_frame_cs);
  MSG_FRAMEPROC("FRAME: video_eng_handshake %d",mp4e_get_time_ms(),0,0);

  if (g_frame == NULL)
  {
    CAMCORDER_MSG_ERROR("video_eng_handshake: g_frame NULL",0,0,0);
    error_flag = TRUE;
  } else
  {

    i =  g_frame->next_enc_pingpong_location;

    /* time to ping the pong and pong the ping */
    g_frame->next_enc_pingpong_location = 1-i;

    /* check for encoder failed condition
    ** Some encoder failuer will be reported directly to video_engine through
    ** the callback function sent during video_eng_init().  This is only
    ** failures that are detectable by video_eng_process_frame.
    */
    if (g_frame->enc_state == VIDEO_ENG_RAW_ENCODE_FAILED)
    {
      MSG_FRAMEPROC("FRAME: Sending Video Failure",0,0,0);
      g_frame->enc_state = VIDEO_ENG_RAW_ENCODE_IDLE;
      fail_sent_flag = TRUE;
    }

    /* If there is an DSP failure, then be_state would
    ** be FAILED, and since nothing is written
    ** to the ping-pong buffers the callback pointer at
    ** the ping-pong buffer will be NULL (which was set to
    ** video_eng_frame() for this frame).
    */
    if (g_frame->be_state == VIDEO_ENG_RAW_ENCODE_FAILED )
    {
      ASSERT(g_frame->enc_pingpong_bufs[i].client_cb == NULL);
      g_frame->enc_pingpong_bufs[i].client_cb   = g_frame->raw_encode_client_cb;
      g_frame->enc_pingpong_bufs[i].client_data = g_frame->raw_encode_client_data;

      MSG_FRAMEPROC("FRAME: BE failure ... Sending Video Failure",0,0,0);

      /* set the enc_state to IDLE so it will not process from the PingPong
      ** in the next iteration
      */
      g_frame->enc_state = VIDEO_ENG_RAW_ENCODE_IDLE;
      if (!fail_sent_flag)
      {
        fail_sent_flag = TRUE;
      }
    }
    else
    {
      /* enc_state will be READY or IDLE depending on the be_state.
      ** please see state transition diagram
      */

      if (g_frame->be_state == VIDEO_ENG_RAW_ENCODE_DONE )
      {
        MSG_FRAMEPROC("FRAME: handshake: setting enc_state to READY",0,0,0);
        g_frame->enc_state = VIDEO_ENG_RAW_ENCODE_READY;
      } else
      {
        MSG_FRAMEPROC("FRAME: handshake: setting enc_state to IDLE",0,0,0);
        g_frame->enc_state = VIDEO_ENG_RAW_ENCODE_IDLE;
      }

    }

    /* the back-end always return to idle state */
    g_frame->be_state = VIDEO_ENG_RAW_ENCODE_IDLE;


    if ( VIDEO_ENG_RAW_ENCODE_READY == g_frame->enc_state )
    {
      /* If the rate control tells us to drop a frame, the first frame
      ** we need to drop is the one that's in the Ping-Pong buffer right
      ** now.
      */

      if (!video_eng_check_frame_encode_permitted
                       (NULL,
                        g_frame->enc_pingpong_bufs[i].msec) &&
          g_context &&
          g_context->framecb_ptr == NULL
         )
      {

        MSG_FRAMEPROC("FRAME: dropped frame ... enc_state --> IDLE",0,0,0);
        event_report(EVENT_CAMCORDER_FRAME_DROP);

        /* the DSP has its own internal ping-pong status. We will manually
        ** ping-pong it in our config command
        */
        if (VIDEO_SUCCESS != video_encode_swap())
        {
          CAMCORDER_MSG_ERROR("encoder failed to swap",0,0,0);
        } else
        {
          /* drop the frame that's in the ping-pong */
          MSG_FRAMEPROC("FRAME: frame dropped",0,0,0);
          g_frame->enc_state = VIDEO_ENG_RAW_ENCODE_IDLE;
        }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif//FEATURE_APP_TEST_AUTOMATION
#ifdef FEATURE_VIDEO_CLIP_INFO
        need_framestat = TRUE;
#endif //FEATURE_VIDEO_CLIP_INFO
      } /* if )!video_encode_permitted(NULL)) */
    }  /* if enc_state == READY */

    ASSERT(g_frame->enc_pingpong_bufs[i].client_cb != NULL);

    terminate_flag     = g_frame->terminate;
      cached_cb          = g_frame->enc_pingpong_bufs[i].client_cb;
      cached_client_data = g_frame->enc_pingpong_bufs[i].client_data;
    }

  rex_leave_crit_sect(&videng_frame_cs);

  if ( !error_flag && terminate_flag )
  {
    /* this flag is set because a previous attempt to terminate
    ** failed.  But it should be OK now
    */
    (void)video_eng_frame_terminate();
  }
#ifdef FEATURE_VIDEO_CLIP_INFO
  else if (need_framestat)
  {
      video_eng_framedrop_stat(TRUE);
  }
#endif //FEATURE_VIDEO_CLIP_INFO

  /* During space out, don't send callback because video engine
  ** and video engine client is already cleaning up */
  if(g_context != NULL && g_context->space_out)
    return;

  /*Check for valid callback function address*/
  if(cached_cb)
  {
    /* no failure .... send the handshake */
    if(!error_flag                      &&          /* no BE/ARM encode error */
       !fail_sent_flag                  &&          /* no failure callbacks   */
       video_eng_detect_state() == VIDEO_SUCCESS)   /* check video eng state  */
    {
      cached_cb(VIDEO_SUCCESS,cached_client_data);
    }
    else
    {
      cached_cb(VIDEO_FAILURE,cached_client_data);
    }
  }

}

#endif /* ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */

static void video_eng_set_first_frame_delta(uint32 frame_duration)
{
     video_enc_dynamic_config_type new_cfg_ptr;

     new_cfg_ptr.initdelta.delta_ms = frame_duration;
     new_cfg_ptr.initdelta.op       = VIDEO_ENC_CONFIG_INITIAL_DELTA;
     new_cfg_ptr.op = VIDEO_ENC_CONFIG_INITIAL_DELTA;
     (void)video_encode_dynamic_set_config(&new_cfg_ptr,NULL,NULL,NULL);
     return;
}

boolean video_eng_check_frame_encode_permitted(VideoBufferHandle *bufferHandle,
                                               uint32 time_ms)
{
    boolean encode_permitted;
    video_eng_drop_frame_time_info_type *frame_drop_info;

    if(g_context == NULL)
    {
       return FALSE;
    }

    frame_drop_info = &g_context->frame_drop_info;

    encode_permitted = video_encode_permitted (NULL);

    if(encode_permitted)/*Most probable case is this being TRUE*/
    {
        /* If this is the first permitted frame while resume */
        if(frame_drop_info->first_frame_encode_pending)
        {
            uint32 drop_duration;

            /*1. If first frame is dropped after resume we have the dropped
            time before current resume accumulated in frame_drop_duration_before_pause
            including delta for first frame after this resume. In this case
            we just need to add frame_drop_duration_before_pause and the difference of 
            current time and time of first frame.

             2. If first frame after resume is not dropped the delta for current 
             frame i.e the first frame after resume, should be frame_drop_duration +
             frame_drop_duration_before_pause + default delta for first frame.
            */

            drop_duration = (frame_drop_info->first_souce_frame_dropped)?
                            /*Case 1*/
                          ((time_ms - frame_drop_info->prev_encode_frame_time) + 
                            frame_drop_info->frame_drop_duration_before_pause):
                            /*Case 2*/
                            frame_drop_info->frame_drop_duration +
                            frame_drop_info->frame_drop_duration_before_pause +
                            frame_drop_info->first_frame_default_delta;

            video_eng_set_first_frame_delta(drop_duration);
            frame_drop_info->frame_drop_duration_before_pause = 0;
            CAMCORDER_MSG_HIGH("Sa Dbg: Set Initial frame delta = %d",drop_duration,0,0);
        }
        frame_drop_info->first_frame_encode_pending = FALSE;
        frame_drop_info->prev_encode_frame_time = time_ms;
        frame_drop_info->frame_drop_duration = 0;
        return encode_permitted;
    }
    else
    {

        /* 1. first_frame_encode_pending == TRUE means we are resuming and waiting to
              encode first frame  
           2. first_souce_frame_dropped == FALSE means we have not dropped any frame 
              during resuming yet. SInce we execute this point now rate control drops
              the first frame while we are resuming. 
          prev_encode_frame_time does not hold good here as it is way back in time due 
          to pause. Therefore we need to store the frame_drop_duration into
          frame_drop_duration_before_pause and start tracking frame_drop_duration again.
        */
        if(frame_drop_info->first_frame_encode_pending &&
                                            !frame_drop_info->first_souce_frame_dropped)
        {
            CAMCORDER_MSG_HIGH("1st frame dropped,frame_drop_duration = %d"
                               "frame_drop_duration_before_pause = %d",
                               frame_drop_info->frame_drop_duration,
                               frame_drop_info->frame_drop_duration_before_pause,0);

            frame_drop_info->first_souce_frame_dropped = TRUE;
            /*
             Note: Its not necessary that we get a chance to write 
             frame_drop_duration_before_pause  while resuming as pause-resume can be 
             so fast that we may not encode any frame during previous resume. So we need 
             to accumulate frame_drop_duration_before_pause over each resume.
            */
            frame_drop_info->frame_drop_duration_before_pause +=
                   frame_drop_info->frame_drop_duration;
            frame_drop_info->frame_drop_duration_before_pause += 
                   frame_drop_info->first_frame_default_delta;

            frame_drop_info->frame_drop_duration = 0;
            /*change the value of prev_encode_frame_time to current frame time*/
            frame_drop_info->prev_encode_frame_time = time_ms; 

            return FALSE;
        }
        frame_drop_info->frame_drop_duration = time_ms - 
                                frame_drop_info->prev_encode_frame_time;
    }
    return FALSE;

}




/* <EJECT> */
#ifdef FEATURE_VIDEO_CLIP_INFO
/*===========================================================================
FUNCTION video_eng_framedrop_stat
DESCRIPTION
    It sets the bit of an array to mark that the frame being dropped by RC
DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void video_eng_framedrop_stat(boolean drop_it)
{
    video_fmt_clipinfo_type *clipinfo = NULL;
    video_fmt_framedrop_list_type * fdat = NULL;

    if(g_context == NULL)
    {
       return;
    }

    clipinfo = &g_context->clipinfo;

    if(clipinfo->framedrop_current == NULL)
    {
        fdat = (video_fmt_framedrop_list_type *)
                  videoeng_malloc(sizeof(video_fmt_framedrop_list_type));
        if (NULL == fdat)
        {
            CAMCORDER_MSG_ERROR ("framedrop_stat: Unable to alloc fdat!!",0,0,0);
            return;
        }
        memset(fdat,0,sizeof(video_fmt_framedrop_list_type));
        fdat->next = NULL;
        clipinfo->framedrop_current = fdat;
        clipinfo->framedrop_head = fdat;
    } else if(clipinfo->framedrop_current->used_bits + 1 > 8 * VIDEO_FMT_MAX_FRAMEDROP)
    {
        fdat = (video_fmt_framedrop_list_type *)
                  videoeng_malloc(sizeof(video_fmt_framedrop_list_type));
        if (NULL == fdat)
        {
            CAMCORDER_MSG_ERROR ("framedrop_stat: Unable to alloc fdat!!",0,0,0);
            return;
        }
        memset(fdat,0,sizeof(video_fmt_framedrop_list_type));
        fdat->next = NULL;
        clipinfo->framedrop_current->next = fdat;
        clipinfo->framedrop_current = fdat;
    }
    clipinfo->framedrop_current->used_bits++;
    if(drop_it)
    {
        uint32 index;
        uint8  bit_position;
        clipinfo->frame_dropped++;
        index = (clipinfo->framedrop_current->used_bits - 1) / 8;
        bit_position = (clipinfo->framedrop_current->used_bits - 1) % 8;
        // set the particular bit as '1' to mark it as frame dropped
        clipinfo->framedrop_current->data[index] |= (0x80 >> bit_position);

    }else
    {
        clipinfo->frame_encoded++;
        //nothing to do as the designated bit already set as '0'
    }
}
/* <EJECT> */
#endif //FEATURE_VIDEO_CLIP_INFO
/*===========================================================================

FUNCTION video_eng_detect_state

DESCRIPTION
  Task independent routine that returns video engine state so that video engine
  clients can determine the course of error recovery if video engine has already
  began to clean itself up.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_SUCCESS
  VIDEO_FAILURE

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL video_status_type video_eng_detect_state(void)
{
  /* Check to ensure the following flags indicative of errors are not set   */
  if (g_context               != NULL  &&          /* videoeng still valid  */
      g_context->terminate    == FALSE &&          /* no terminate flag set */
      g_context->buffer_out   == FALSE &&          /* sufficient buffer     */
      g_context->save_on_exit == FALSE &&          /* no file save needed   */
      g_context->state != VIDEO_ENG_STATE_EXITING) /* videoeng not deleting */
  {
    return VIDEO_SUCCESS;
  }

  return VIDEO_FAILURE;
}


/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_send_limit_status

DESCRIPTION
  Task independent routine that provides space event callbacks to video engine
  clients.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void video_eng_send_space_limit_status(void)
{
  /* If we are low on space, pass along notification to the client. */
  if (g_context->space_out_near)
  {
#ifdef DETAILED_SPACE_OUT_MSG
    switch(g_context->space_out_status)
    {
      case VIDEO_FMT_SPACE_MSG_SAMPLE_TABLE:
        g_context->callback_ptr (VIDEO_LIMIT_NEAR_SAMPLE_TABLE, g_context->client_data);
        break;

      case VIDEO_FMT_SPACE_MSG_CHUNK_TABLE:
        g_context->callback_ptr (VIDEO_LIMIT_NEAR_CHUNK_TABLE, g_context->client_data);
        break;

      case VIDEO_FMT_SPACE_MSG_STORAGE:
        g_context->callback_ptr (VIDEO_LIMIT_NEAR_STORAGE, g_context->client_data);
        break;

      case VIDEO_FMT_SPACE_MSG_MOVIE_DURATION:
        g_context->callback_ptr (VIDEO_LIMIT_NEAR_MOVIE_DURATION, g_context->client_data);
        break;

      case VIDEO_FMT_SPACE_MSG_INFO_UNAVAILABLE:
      default:
        g_context->callback_ptr (VIDEO_LIMIT_NEAR, g_context->client_data);
        break;

    }
#else
    g_context->callback_ptr (VIDEO_LIMIT_NEAR, g_context->client_data);
#endif /* DETAILED_SPACE_OUT_MSG */
    g_context->space_out_near = FALSE;
  }

  if (g_context->space_out_imminent)
  {
#ifdef DETAILED_SPACE_OUT_MSG
    switch(g_context->space_out_status)
    {
      case VIDEO_FMT_SPACE_MSG_SAMPLE_TABLE:
        g_context->callback_ptr (VIDEO_LIMIT_IMMINENT_SAMPLE_TABLE, g_context->client_data);
        break;

      case VIDEO_FMT_SPACE_MSG_CHUNK_TABLE:
        g_context->callback_ptr (VIDEO_LIMIT_IMMINENT_CHUNK_TABLE, g_context->client_data);
        break;

      case VIDEO_FMT_SPACE_MSG_STORAGE:
        g_context->callback_ptr (VIDEO_LIMIT_IMMINENT_STORAGE, g_context->client_data);
        break;

      case VIDEO_FMT_SPACE_MSG_MOVIE_DURATION:
        g_context->callback_ptr (VIDEO_LIMIT_IMMINENT_MOVIE_DURATION, g_context->client_data);
        break;

      case VIDEO_FMT_SPACE_MSG_INFO_UNAVAILABLE:
      default:
        g_context->callback_ptr (VIDEO_LIMIT_IMMINENT, g_context->client_data);
        break;

    }
#else
    g_context->callback_ptr (VIDEO_LIMIT_IMMINENT, g_context->client_data);
#endif /* DETAILED_SPACE_OUT_MSG */
    g_context->space_out_imminent = FALSE;
  }

  if (g_context->space_out)
  {
#ifdef DETAILED_SPACE_OUT_MSG
    if (g_context->space_out)
    {
      switch(g_context->space_out_status)
      {
        case VIDEO_FMT_SPACE_MSG_CHUNK_TABLE:
          g_context->callback_ptr (VIDEO_LIMIT_REACHED_CHUNK_TABLE , g_context->client_data);
          break;

        case VIDEO_FMT_SPACE_MSG_SAMPLE_TABLE:
          g_context->callback_ptr (VIDEO_LIMIT_REACHED_SAMPLE_TABLE , g_context->client_data);
          break;

        case VIDEO_FMT_SPACE_MSG_MOVIE_DURATION:
          g_context->callback_ptr (VIDEO_LIMIT_REACHED_MOVIE_DURATION , g_context->client_data);
          break;

        case VIDEO_FMT_SPACE_MSG_STORAGE:
          g_context->callback_ptr (VIDEO_LIMIT_REACHED_STORAGE, g_context->client_data);
          break;

        case VIDEO_FMT_SPACE_MSG_INFO_UNAVAILABLE:
        default:
          g_context->callback_ptr (VIDEO_LIMIT_REACHED , g_context->client_data);
          break;

      }
    }
    else
    {
      g_context->callback_ptr (VIDEO_SUCCESS, g_context->client_data);
    }
#else
    g_context->callback_ptr((g_context->space_out ? VIDEO_LIMIT_REACHED : VIDEO_SUCCESS),
                             g_context->client_data);
#endif /* DETAILED_SPACE_OUT_MSG */
  }
}

/*===========================================================================

FUNCTION video_eng_venc_req_cb

DESCRIPTION
  This function handles data request callbacks from the video encoder
  interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_venc_req_cb (
  video_status_type                   status,
  const video_enc_enc_to_client_type  *enc_to_client_data,
  video_enc_client_to_enc_type        *client_to_enc_data,
  const void                          *client_data
) {
    video_eng_packet_type  *packet;
    static uint32          length = 0;
    uint32                 header_length;

    if(g_context == NULL)
    {
      /* Flags inform video encoder that error has occured */
      client_to_enc_data->buf_size = 0;
      client_to_enc_data->buf_ptr = NULL;

      CAMCORDER_MSG_ERROR("Engine not running: g_context is NULL",0,0,0);
      return;
    }

    /* Branch according to status. */
    switch (status)
    {
    case VIDEO_SUCCESS:
    case VIDEO_FAILURE:
        /* Ignore the callback if already in the midst of exiting */
        if (g_context->state == VIDEO_ENG_STATE_EXITING)
        {
            /* Flags inform video encoder that error has occured */
            client_to_enc_data->buf_size = 0;
            client_to_enc_data->buf_ptr = NULL;
            return;
        }
        CAMCORDER_MSG_HIGH ("enc_req_cb: status=%d", (uint32) status, 0, 0);

        /* Pass this status back to the engine asynchronously. */
        packet = video_eng_cmd_get_pkt (VIDEO_ENG_QUEUE_PRI_URGENT);
        if (packet == NULL)
        {
            CAMCORDER_MSG_ERROR ("cmx_normal_cb: out of pkts!", 0, 0, 0);
            return;
        }
        packet->hdr.cmd = VIDEO_ENG_CMD_ENC_STATUS;
        packet->enc_stat.status = status;
        video_eng_cmd (packet);
        break;

    case VIDEO_ABORTED:
        /* This event can be received in the middle of encoding one frame, but
        ** it indicates that the driver will stop immediately. Proceed to issue
        ** callback to upper layers to stop video engine gracefully */
        g_context->buffer_out = TRUE;
        g_context->callback_ptr (VIDEO_LIMIT_IMMINENT, g_context->client_data);
        break;

    case VIDEO_ERROR:
        /* This event should only be received at the end of encoding
        ** one frame. There should be no more error callbacks from lower
        ** layers, proceed to issue callback to upper layers */
        g_context->buffer_out = TRUE;
        g_context->callback_ptr (VIDEO_ERROR, g_context->client_data);
        break;

    case VIDEO_ENC_FRAME:
        CAMCORDER_MSG_HIGH ("enc_req_cb: status=VIDEO_ENC_FRAME, len=%d pf=%d d=%d",
                  enc_to_client_data->len, enc_to_client_data->type,
                  enc_to_client_data->delta);

        /* Ignore the callback if already in the midst of exiting, or if
        ** still image mode is set and we already have an image.
        */
        if ((g_context->state == VIDEO_ENG_STATE_EXITING)
            || (g_context->still_image
                && (g_context->video_frames_received > 0)))
        {
            /* Flags inform video encoder that error has occured */
            client_to_enc_data->buf_size = 0;
            client_to_enc_data->buf_ptr = NULL;
            return;
        }
        ++g_context->video_frames_received;

        /* Update video track length. */
        g_context->av_sync_duration += (uint32) enc_to_client_data->delta;
        client_to_enc_data->buf_ptr = (uint8 *)g_context->client_to_enc_buffer;
        client_to_enc_data->buf_size = g_context->enc_buffer_size;
        client_to_enc_data->buf_size -= sizeof (M4V_BITSTREAM_FIXED_FOOTER);
        length += enc_to_client_data->len;

        g_context->pEncFrame->client_data = (vencoder_frame_address_type *)client_to_enc_data->buf_ptr;
        g_context->pEncFrame->len = enc_to_client_data->len;
        g_context->pEncFrame->type =  enc_to_client_data->type;
        g_context->pEncFrame->delta = enc_to_client_data->delta;
        g_context->framecb_ptr(VIDEO_ENC_FRAME,g_context->pEncFrame,(void*)g_context->client_data);
        break;

    case VIDEO_ENC_PARTIAL_FRAME:
        CAMCORDER_MSG_HIGH ("enc_req_cb: status=VIDEO_ENC_FRAME, len=%d pf=%d d=%d",
                  enc_to_client_data->len, enc_to_client_data->type,
                  enc_to_client_data->delta);

        /* Ignore the callback if already in the midst of exiting, or if
        ** still image mode is set and we already have an image.
        */
        if ((g_context->state == VIDEO_ENG_STATE_EXITING)
            || (g_context->still_image
                && (g_context->video_frames_received > 0)))
        {
            /* Flags inform video encoder that error has occured */
            client_to_enc_data->buf_size = 0;
            client_to_enc_data->buf_ptr = NULL;
            return;
        }
        ++g_context->video_frames_received;

        /* Update video track length. */
        g_context->av_sync_duration += (uint32) enc_to_client_data->delta;

        client_to_enc_data->buf_ptr = (uint8 *)g_context->client_to_enc_buffer;
        client_to_enc_data->buf_size = g_context->enc_buffer_size;
        client_to_enc_data->buf_size -= sizeof (M4V_BITSTREAM_FIXED_FOOTER);
        length += enc_to_client_data->len;

        g_context->pEncFrame->client_data = (vencoder_frame_address_type *)client_to_enc_data->buf_ptr;
        g_context->pEncFrame->len = enc_to_client_data->len;
        g_context->pEncFrame->type = enc_to_client_data->type;
        g_context->pEncFrame->delta = enc_to_client_data->delta;
        g_context->framecb_ptr(VIDEO_ENC_PARTIAL_FRAME,g_context->pEncFrame,g_context->client_data);
        break;

    case VIDEO_ENC_HEADER:
        CAMCORDER_MSG_HIGH ("enc_req_cb: status=VIDEO_ENC_HEADER, len=%d pf=%d d=%d",
                  enc_to_client_data->len, enc_to_client_data->type,
                  enc_to_client_data->delta);

        /* Ignore the callback if NOT in the midst of starting. */
        if ((g_context->state != VIDEO_ENG_STATE_STARTING)
            && (g_context->state != VIDEO_ENG_STATE_RESUMING))
        {
            return;
        }

        /* "length" needs to be initialized here. Otherwise it keeps old
        ** value for the previous recording.
        */
        length = 0;

        client_to_enc_data->buf_ptr = (uint8 *)g_context->client_to_enc_buffer;
        client_to_enc_data->buf_size = g_context->enc_buffer_size;

        client_to_enc_data->buf_size -= sizeof (M4V_BITSTREAM_FIXED_FOOTER);

        /*
        ** Process bitstream header synchronously.
        */

        /* enc_to_client_data->len == 0 in the short header mode (H.263). */
        header_length = enc_to_client_data->len;
        if (g_context->video_format == VIDEO_FMT_STREAM_VIDEO_MPEG4)
        {
            header_length += sizeof (M4V_BITSTREAM_FIXED_HEADER);
        }
        g_context->pEncFrame->len = header_length;
        g_context->pEncFrame->client_data = (void *) g_context->ved_buf_ptr;
        g_context->framecb_ptr(VIDEO_ENC_HEADER,g_context->pEncFrame,g_context->client_data);
        break;

    default:
        CAMCORDER_MSG_ERROR ("enc_req_cb: Illegal video "
                   "encoder status %x", status, 0, 0);
    }
}
#endif /* FEATURE_VIDEO_ENCODE */
/*lint -restore */
