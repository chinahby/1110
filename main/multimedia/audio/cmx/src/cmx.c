/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("CMX Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           C M X    S E R V I C E S

GENERAL DESCRIPTION
  This module contains CMX services software.

REFERENCES
  CMX Services ISOD: 80-24875-12 X2

EXTERNALIZED FUNCTIONS
  cmx_audfmt_play_codec
    This function issues a command to play a specified audio codec type.
  cmx_audfmt_play
    This function issues a command to play a midi/WebAudio file.
  cmx_audfmt_ringer
    This function issues a command to play a midi/WebAudio ringer.
  cmx_audfmt_stop
    This function issues a command to stop midi/WebAudio playback.
  cmx_audfmt_rewind
    This function issues a command to rewind midi/WebAudio playback.
  cmx_audfmt_fforward
    This function issues a command to fast forward midi/WebAudio playback.
  cmx_audfmt_pause
    This function issues a command to pause midi/WebAudio playback.
  cmx_audfmt_resume
    This function issues a command to resume midi/WebAudio playback.
  cmx_audfmt_get_time
    This function issues a command to calculate the time of the indicated
    midi/WebAudio file.
  cmx_png_decode
    This function issues a command to start the PNG decoder.
  cmx_png_abort
    This function issues a command to abort the PNG decoder.
  cmx_jpeg_decode
    This function issues a command to start the JPEG decoder.
  cmx_jpeg_decode_abort
    This function issues a command to abort the JPEG decoder.
  cmx_jpeg_get_specs
    This function issues a command to get the JPEG image header information.
  cmx_jpeg_encode
    This function issues a command to start the JPEG encoder.
  cmx_jpeg_encode_abort
    This function issues a command to abort the JPEG encoder.
  cmx_mm_record
    This function starts a multimedia recording session.
  cmx_mm_record_stop
    This function issues a command to stop a multimedia recording session.
  cmx_mm_record_pause
    This function issues a command to pause a multimedia recording session.
  cmx_mm_record_resume
    This function issues a command to resume a multimedia recording session.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2000 - 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/cmx/main/latest/src/cmx.c#17 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/09    gs     Added sanity check before decrementing cmx_af_gt_active
08/20/09    kk     Added missing Null pointer checks.
08/17/09    rk     CMX buffer requests which are not processed by the client
                   are handled properly
06/05/09  gs/sk    Modified the code to remove
                   handle->svg_data->svg_handle != NULL conditional check in
                   cmx_audfmt_wa_cleanup() before calling graph_svg_abort()
                   to send abort command to graph task even svh handle is null.
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
04/03/09   skr     Changes to introduce new API cmx_audfmt_avsync_playback()
                   that will send the type of ABORT to the client and retain
                   backward compatibilty of the cmx_aufmt_playcodec_av_sync()
02/09/09    bk     Featurised for removing Lint error.
01/19/09    bk     Changed the #ifdef condition for properly featurizing
                   for removing web audio.
09/24/08    kd     fixed CR 157905 and reverted some klockwork fixes.
09/18/08    kd     fixed klocwork warnings
09/11/08    kk     Fixed MM warnings due to FEATURE_AUDFMT_EVB.
08/26/08   anb     Added support for DSP based WMAPRO
07/17/08    kk     Added support to play Qtv 4gv nb clips.
06/23/08    ry     Added SMAF support for LED/VIB event synchronization.
02/12/08   wfd     CR 134800 - in cmx_af_client_read_var_cb(), only use hi
                   command que if client buffer length > 0
01/03/08    sk     Added Null pointer check in cmx_qcp_record_processing()
                   if source is CLIENT or MEMORY. Earlier handle_ptr are
                   cleared and used later in the code. CR 132759.
12/19/07    hs     Added support ISDB-T dual mono output control
11/05/07    sk     Updated the CMX_STARTED command in
                   cmx_audfmt_support_processing() to send the CMX STARTED
                   status to OEM laytes and other Clients.
                   CR 120253,124336,124337,120853.
11/26/07    kk     Fixed issues due to fix for CMX ran out buffers.
11/19/07    kk     Fixed CR 128379 - Audio sound repetition problem
                   for ISDB-T audio streaming (fix for CMX ran out buffers).
11/05/07   sk/lg   Added SVG Scaling support.
10/26/07    hs     Added EVW file format support
10/06/07    hs     Fixed memory leak in gframes
09/17/07   sk/lg   Added support to allow specifying SVG display
                   when with PMD.CR 118438.
09/14/07   pl/ss   Fixed minor compilation errors while FEATURE_XMF is undefined.
09/10/07   pl/ss   Added a macro in cmx_xmf_terminate() which is related to
                   XMF+DLS playback to resolve compilation issue.
08/24/07    hs     Integrated: Added support for PCR clk sync feature.
08/06/07    kk     Merged the Fix CR 124244 - Full ID3V2.3 support,
                   support for URL link (W)frames .
08/01/07    pl     Reverted change made in revision #119 as it is not the right
                   solution I was trying to solve.
07/19/07    st     Modified code to ensure buffer overrun does not occur,
                   CR111048, CR111050, CR111052; modified code to use
                   statically defined variables for audio specification
                   callbacks.
07/13/07    pl     Modified cmx_af_client_read_cb() and cmx_af_client_read_var_cb()
                   to return NULL buffer to lower layer when length of buffer returned
                   from the client.
07/06/07    ss     Increased CMX_PIC_BUFFER_SIZE to 3200 for supporting
                   PNG Images of size upto 800x800. (CR_116764)
06/17/07    hs     Fixed CR 119401 Initialized multiseq flag properly for ringer playback
06/18/07    hs     Fixed CR 121762 - crash on mp3 file because of size mismatch
05/25/07    kk     Fixed compiler error due to mismerging.
05/23/07    kk     Fixed  CR 113932 - CMX get spec problem due to APIC
                   frame in MP3 ID3V2.3 tag.
04/01/07   st/pl   Modified code to properly abort on-going xmf operation
                   upon receiving of new commands.
03/21/07    at     Added EVRC-B file format support.
03/15/07    kk     Fixed issues related to ID3V1 tag processing for MP3.
03/15/07    kan    Added 4GV-WB Code
03/14/07    kk     Fixed warning C2874W: 'num_bytes' may be used before
                   being set
02/07/07   lg/act  Featurizing JPEGD components under FEATURE_WEBAUDIO since
            anb    standalone JPEG decode via CMX is no longer supported.
02/06/07    at     Made changes to cmx_audfmt_play_codec_processing() for
                   AMR-WB decoder support.
02/10/07    sk     Included F3 messages for QAudioFx.
02/05/07    st     Removed use of strncpy() function.
01/12/07    az     Fixed the CR: 108715 CMX: MM API Stop does not work for
                   QCELP Codecs (AMR, EVRC, 13k)
11/09/06    az     Merged the code for MM API changes.
10/27/06    pl     Remodified cmx_midi_out_qcp_processing to call cmx_free_handle()
                   if client does not support random data access.
10/16/06    pl     Modified cmx_midi_out_qcp_processing() to reject playback
                   with source type set to client which does not support random
                   access.
09/28/06    sk     Merged SVG Fix frame rendering issue.
09/25/06    KK     Fixed the merge mistake for the CR 99779 fix.
09/20/06    pl     Modified cmx_audfmt_read_processing() &
                   cmx_audfmt_read_var_processing() to return NULL buffer
                   if seeking beyond end of file or end of client memory buffer.
09/14/06    sg     Fixed CR#99779 which can cause recording to fail on EFS if
                   file does not already exist.
09/08/06    lg     Increased Number of Adec buffers to 8 for PMD files.
09/04/06    ay     Added AMR-WB+ support.
08/24/06    hz     Cleaned to fix lint issues.
07/31/06    hz     Fixed a compilation issue.
07/28/06    hz     Added support for ID3V2 and AAC ADTS/VBR song length.
07/27/06    lg     Fixed a typo in SVG callback for CR#97721
07/26/06    pl     Fixed compilation error in cmx_midi_out_qcp_reset_processing().
07/17/06    pl     Modified cmx_midi_out_qcp_reset_processing() to return
                   different status based on the reason of stopping of playback.
06/27/06    pl     Added environment & source 3D enable/disable support.
                   Added environment & source 3D reset support.
06/26/06    pl     Added support for doppler and reverb.
06/23/06   sk/lg   Reverted the Memory leak issue(CR 90874).
                   Fixed issue with get_time and get_spec processing.
06/14/06   sk/lg   Fixed Memory leak issue.Added CMX_FREE() in
                   cmx_af_play_callback() when status is SND_CMX_AF_FREE_BUF
                   (CR90874).
06/14/06    hz     cmx_af_active needs to be cleared when MM recoding is done.
06/08/06    lg     Fixed an issue with PMD+SVG file cleanup (CR#89340)
06/07/06    ay     Fixed AAC record not stopping.
05/24/06    sg     Fixed featurization of recording handles.
05/09/06    lg     Fixed a race condition with SMAF meta data.(CR#91026)
04/24/06    lg     Fixed compiler warning with SVGT1.2 checkin
04/20/06    st     Minor bug fixes: added check for NULL callback before
                   sending recording space warning to client; modified code
                   to properly send back failure status and tick updates to
                   client for RINGER playback, CR87342.
04/02/06    sg     Added fs space underrun checks when using MMC cards for
                   recording.
03/24/06    lg     Added support for SVGT 1.2
03/13/06    sg     Added debug messages to most commonly used public APIs and
                   callbacks.
                   Optimized so that cmx_audfmt_callback_processing() and
                   cmx_audfmt_play_done_processing() share common code.
03/13/06    lg     Added support for returning SMAF meta data in AUDIO_SPEC callback
03/13/06    st     Added support for delayed start for playback of audio
                   decoder files to MIDI device; added support for repeat
                   playback mode for audio decoder files to MIDI device;
                   added support for query of 3D positional source settings
                   at the end of playback; modified midi_out_qcp() API
                   functions to call callback with failure status if CMX
                   packet is not available.
02/03/06    pl     Fixed freeing client buffer issue for the case when stop command
                   is issued before parser gets the buffer back from CMX.
01/30/06    pl     Added support to provide a way to select hybrid mode
                   for multisequencer mode and MIDI device.
01/13/06   act     Fixed compile warning with low-tier featureset.
01/12/06    lg     Modified code to return NULL buffer when 0 bytes are read from
                   EFS. CR# 85798.
12/02/05    pl     Fixed cmx_midi_out_qcp_cb() not to free memory buffer that
                   is not allocated by CMX when source type is MEM.
11/22/05    hz     Added fixes for CR78125 in cmx_mm_rec_snd_cb.
11/15/05    st     Modified code to return ABORTED status for stop commands
                   to individual sequences, CR82257.
11/15/05    lg     Initialized all parameters for SVG decoder. CR#81267.
10/21/05    st     Modified code to include client buffer request code under
                   FEATURE_SVG and FEATURE_SAF; modified code to delay
                   SUCCESS callback for midi_out_qcp() until after signal from
                   lower layer.
10/18/05    st     Modified code to pass correct parameter to
                   jpeg_ARM_based_decode(), CR78823.
10/07/05    aw     Fixed compiler warning and QCP Record Auto Stop out of range.
09/18/05    st     Added support for QAudioFX: 3D positional commands and query
                   interface; added support for variable length ADEC read
                   command.
09/02/05    st     Fixed coverity errors in cmx_client_buf_req_cb_processing(),
                   cmx_audfmt_read_var_processing() and
                   cmx_saf_info_processing(); modified code to save pending
                   active state to cover race condition.
08/29/05    aw     Modified code to feature-rized extended_status in
                   cmx_af_play_callback.
08/24/05   ayu     Modified code to delete recorded QCP files containing only
                   header information, for CR62770.
08/16/05    hz     Fixed coverity error in cmx_mm_record_processing,
                   cmx_mm_rec_snd_cb and cmx_pcm_record_processing.
08/11/05    hz     Added support for DTX and data requrest time for AMR
                   recording.
08/04/05    sp     Added support for mixing of qcp audio and voice call.
07/30/05    lg     Added checks for NULL ptr in cmx_af_play_callback
07/25/05    sg     Modified code to reduce the QCP record buffer if
                   FEATURE_AMR_ENABLE_DTX_FOR_RECORDING is enabled.
07/22/05    sm     Added Wav A/ULaw support.
07/19/05    pl     Added support for returning more informative status
                   for aborted playbacks.
07/08/05    pl     Added the code to set supported operation bit mask for
                   get spec callback.
06/15/05    st     Added support for SVG commands within c-MIDI files.
06/03/05    st     Modified code to support SND_CMX_AF_TIME_TICK callback
                   status for PCM recording.
06/01/05    aw     Enhanced PCM playback in client data mode.
05/20/05    hz     Added support for MultiMedia AAC record functionality.
05/18/05    ac     Modified code to enhance AV sync functionality to allow
                   clients to specify callback interval in ms or
                   samples/packets.
04/28/05   ymc     Added 4GV NB support.
04/25/05    lg     Modified code to handle SND_ABORT callback from snd layer.
04/21/05    lg     Modified code to not call cmx_af_free_buf if handle_ptr is
                   NULL.(CR# 57402)
04/20/05    lg     Fixed a crash issue with SVG decoder.
04/20/05    lg     Modified code to pass filename to SVG decoder
04/19/05    lg     Modified code to correct SVG client frame generation
04/05/05    lg     Modified code to copy SVG event_data to local buffers.
03/25/05   ac/sg   Added cmx_audfmt_play_codec_av_sync() API to support A/V
                   synchronization.
03/17/05    lg     Added support for SVG.
03/10/05    st     Added support for MFi LED control.
02/14/05    hz     Modified code to support Windows Media Audio decoding.
02/11/05    sp     Added code to gracefully stop playback if EFS read errors
                   occur during playback (CR35673).
            aw     Modified code to only do play codec function on supported
                   codec type.
02/05/05    st     Modified code to support DLS and XMF file processing;
                   modified code to use status_mask parameter in
                   cmx_af_handle_type; removed the support file in
                   cmx_af_handle_type as it is no longer necessary.
01/11/05    st     Modified code to handle MFi file type; modified code to
                   return NULL buffer if no data is read from file.
11/09/04    aw     Modified code to use new ARM JPEG decoder interface.
10/12/04    st     Modified code to remove requirement that client executes
                   before CMX task for data buffer request commands.
08/11/04    aw     Modified to not free unallocated memory for variable
                   length buffers, using SOURCE_MEM.
07/02/04    st     Modified code to allocate an extra 3 bytes for DLS waveform.
06/21/04    lg     Fixed compilation error and initialized cmx_saf_frame_width &
                   cmx_saf_frame_height to 0.
05/19/04    lg     Modified code to fix CMX APIs for Standalone SAF decoder.
04/23/04    lg     Modifies code to support ARM-based JPEG decoder.
                   Added changes to support dynamic allocation of SAF object buffer.
                   Added changes to cmx_saf_frame_buf to  allocate memory dynamically.
                   Added changes to decode SAF frames of the actual size of frame for
                   QVGA displays.
            st     Modified code to support animation positioning and animation
                   control; added support for cue and jump point status.
04/08/04    st     Removed JPEG encoder software, this is no longer supported
                   through CMX layer.
03/22/04    st     Added support for PCM recording and playback; updated
                   get_specs command processing; modified code to use malloc
                   and free instead of tmc_heap.
                   Modified code to support pausing and resuming individual
                   MIDI out QCP channels; modified code to handle tick callbacks
                   in cmx_af_play_callback(); modified SMAF commands to use
                   updated interface.
                   Modified code to support requesting buffers from client.
02/02/04    aw     Modified code to free DLS data for EFS source type properly.
01/29/04    aw     Fix feature-rization.
01/26/04    lg     Added support for tune, pan & volume adjustments for
                   multi-sequencer.
01/26/04    aw     Added support to get file specs (similar to get_time).
            sg     Modified cmx_af_client_read_cb() to reject data is the
                   data length exceeds the buffer limit (CR 39065).
                   Fixed bug where unallocated memory was being freed for
                   DLS sounds.
01/21/04  lg/aw    Added support for SMAF led/vibrator/graphics info events.
12/08/03    sg     Modified to not attempt playback of input which is actually
                   a directory name (instead of a file name).
                   Clear cmx_af_stopped before playing any new file - otherwise
                   the client will always CMX_ABORTED when playing is done
                   instead of getting CMX_DONE.
                   Clear cmx_af_active flag when audio postcard creation fails.
12/02/03    sg     Added support for linear PCM playback.
12/02/03    sg     Deleted hard coded FEATURE_AUDFMT_MULTISEQUENCER definition.
11/22/03    sg     Added support for RealAudio codec.
11/17/03    aw     Modified code to support 16bit word of DLS data.
10/16/03  aw/st    Modified code to support multi-sequence and SMAF.
06/20/03    sg     Modified to support additional AAC parameters required by the
                   DSP. Added support for a 'psuedo-raw' AAC mode.
08/09/03    st     Modified code to only send AUDIO_SPEC callback when doing
                   play/ringer command.
04/30/03    st     Added new language support for WebAudio files.
02/19/03   aw/st   Added suport for ADPCM file format playback; added code
                   to reserve a midi_out_qcp() channel for ADPCM playback.
02/04/03    sg     Modified cmx_audfmt_read_processing() to set the correct
                   data length for CMX_SOURCE_MEM.
01/25/03    st     Modified code to handle JPEG_CMX_GET_BUF callback during
                   WebAudio playback.
01/07/03    st     Modified code to handle image continue flags differently.
12/04/02    sa     Added JPEG_CMX_GET_BUF to switch statement in function
                   cmx_jpeg_callback.
11/05/02    sg     Added support for iMelody format.
10/28/02   aw/st   Modified code to ensure only one frame ID command from
                   webaudio file is sent to SAF module; Modified code to
                   support processing get time callback command in CMX context;
09/19/02    st     Added support for pixel poistioning for WebAudio picture
                   commands; added support to return content elements
                   information to client.
08/13/02    yl     Modified code to handle automatic stop of qcp recording.
08/10/02   aw/st   Modified code to add support for ADPCM audio decoder on
                   MIDI device API.
07/25/02   sg/st   Added support for MP4 and AAC.
                   Added cmx_audfmt_play_codec(). Added support for flushing.
                   Added support for audio postcard.
                   Modified code to delay PNG/SAF decode command from CMX files
                   if a SAF/PNG decode command is active; general enhancements.
07/05/02    st     Modified code to handle PNG get spec and SAF get info
                   commands properly.
06/11/02    st     Modified code to check if recording is active before
                   allowing playback to start; modified code to reset the
                   proper flag in cmx_qcp_get_func_processing(); modified code
                   to consolidate common graphics processing routines;
                   modified PNG and SAF callbacks to check current command
                   flag before executing the callback commmand; fixed file_type
                   audio spec callback for MP3 file; modified code to free
                   handle in play_processing and ringer_processing.
04/17/02    sa     Modified code to support larger cmx_image_buf for JPEG.
03/22/02    sa     Added support for JPEG decode and encode.
03/16/02    st     Modified code to only abort PNG decoder at the end of
                   a WebAudio playback if the current PNG decode command
                   originated from the WebAudio file; modified code to ensure
                   that PNG data callbacks stop after playback ends; modified
                   code to check command packet is not NULL before using.
02/06/02    st     Modified code to not send PNG data back to client if data is
                   for a previous PNG decode command; modified code to call
                   the cmx_png_cont_cb_func_ptr and cmx_saf_cont_cb_func_ptr
                   if there is a callback outstanding; modified
                   cmx_png_read_processing() and cmx_saf_read_processing()
                   to do reads from EFS if there are any outstanding read
                   commands in order to ensure the correct ordering of data
                   read commands.
01/21/02    aw     Modified code for ads110 compiler.
12/03/01    st     Modified cmx_audfmt_setup_handle() to only open files for
                   write access when necessary.
11/21/01    st     Fixed featurization.
11/10/01    st     Modified code to consider audio decoder processing during
                   PNG and SAF activity; updated featurization.
10/15/01    st     Modified code to support bitmap display builds.
10/11/01    aw     Modified code to support graphics task.
10/08/01    st     Merged back rev1.7.
10/08/01    st     Modified code to callback the cmx_midi_out_open() callback
                   for MIDI out operations.
10/03/01    st     Added cmx_qcp_fs_space_available() function.
10/01/01    st     Modified code to support multiple audio decoder channels
                   for the MIDI device; added pause, resume, volume and panning
                   support for the MIDI device audio decoder; added
                   cmx_audio_spec_buffer to use with the CMX_AF_CMD_AUDIO_SPEC
                   command.
09/16/01    st     Added support for setting cnfg chunk in a QCP file.
09/03/01    st     Added support for QCP recording and support functions
                   (get/set information); modified code to better allocate
                   snd_af_access_type handle structures; modified code to
                   support playback of QCP files to reverse/both links;
                   modified code to release allocated handles; modified code
                   to return number of bytes read in cmx_midi_out_efs_cb().
07/17/01    st     Modified code to add support for managing image buffer;
                   modified code to reset SAF parameters on a SAF abort;
                   modified code to call saf_decode() before setting up local
                   state parameters to accommodate SAF decoder updates;
                   added calls to saf_decode_stop() to let SAF decoder know
                   when SAF is done playing; modified code to call the proper
                   function in cmx_saf_read_processing().
06/13/01    sm     Return read count in cmx_af_efs_read_cb().
05/12/01    st     Modified code to add featurization to enable FEATURE_SAF and
                   FEATURE_PNG_DECODER to be disabled while FEATURE_WEBAUDIO
                   is enabled.
04/04/01    st     Added cmx_audio_playback_control(); modified code to abort
                   SAF if necessary when finished audio playback.
                   Modified code to return the actual length of PNG and SAF
                   data from WebAudio files.
                   Modified cmx_png_callback() and cmx_saf_callback() to
                   handle file size command; added SAF object buffer.
                   Added support for SAF (simple animation format); added
                   support for direct MIDI device output interface; added
                   support for client controlled data source; removed scaling
                   support from PNG decoder; added cmx_af_callback_enum_type
                   to allow client to indicate what format they want the
                   callbacks to be made; added support for ISO8859 char code
                   set; added cmx_audio_playback() interface for remote
                   systems; renamed some types and parameters in order to
                   expand their usage.
02/28/01    st     Added support for the audio format preparse mechanism;
                   added support for a high priority CMX command queue which
                   will execute at VS_PRI_HIGH priority; modified code to
                   properly close file at the end of a cmx_png_get_specs()
                   command; lint updates.
12/10/00    st     Modified code to use one global text buffer.
12/04/00    sm     Added pan function, reworked cmx_audfmt_read_processing().
11/27/00  st/sm    Featurized EFS, CMX, added 1600 byte buffers, failed EFS
                   reads will re-try, removed RESET_MINOR command, added
                   SND_CMX_AF_GET_FILE_LEN processing.
                   Modified code to handle the SND_PROMPT_BUSY status in
                   cmx_audfmt_callback_processing().
                   Added cmx_af_do_audio_spec_mp3().
09/15/00    sm     Include comdef.h to fix warning when feature is disabled.
09/05/00    st     Modified code to use cmx_af_server_data_type in playback
                   callback function; added support for picture and text info
                   callbacks.
08/07/00    st     Added code to support the client specifying the number of
                   cue point iterations to perform before stopping; added
                   support to inform client with a CMX_REPEAT when the end cue
                   point is reached.
07/27/00    st     Initialized handle_ptr to NULL in cmx_png_spec_processing(),
                   cmx_audfmt_ringer_processing() and
                   cmx_png_decoder_processing(); CR14096.
07/25/00    st     Modified cmx_audfmt_ringer_processing() such that
                   start_position is now set correctly.
07/25/00    st     Modified code to support a starting position parameter in
                   the cmx_audfmt_play() and cmx_audfmt_ringer() functions.
07/22/00    st     Added code to support hangul character set for korean text.
07/18/00    st     Added proper featurization.
07/15/00    st     Added code to accumulate text advance commands so as to not
                   overflow the CMX command buffer; added support for tempo,
                   tuning, LED and audio spec functions; added minor reset
                   command; modified code to do data request callback in
                   cmx_af_efs_read_cb() for all statuses except FS_EOF_S and
                   FS_BUSY_S; lint updates.
06/21/00    st     Remerged change from 06/13.
06/13/00    st     Added code to handle the returning of non-PNG images
                   in WebAudio files to the client; modified code to handle
                   text positioning.
06/05/00    st     Added proper featurization.
05/18/00    st     Featurized WebAudio and PNG decoder specific code; changed
                   adv_start to seek_pause; added audfmt play/ring pending
                   mechanism; added get PNG specification and PNG text
                   functionality; modified code to allow up to 6 PNG data
                   reads at once.
05/09/00    st     Added interface functions for the PNG decoder module and
                   integrated with WebAudio; changed all strncpy() calls to memcpy().
04/28/00    st     Added code to copy handles to local structures from API
                   function calls; added code to support the rewind/ff init
                   function; modified code to support new cmx_handle_type
                   structure.
04/13/00    st     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Definitions for byte, word, etc.        */

#ifdef FEATURE_CMX
#include "cmx.h"                /* CMX typedefs and prototypes             */
#include "cmxi.h"               /* CMX internal typedefs and prototypes    */
#include "task.h"               /* CMX internal typedefs and prototypes    */
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include "assert.h"
#include <memory.h>             /* Memory manipulation routines            */
#include <stdlib.h>             /* Dynamic memory allocation routines      */
#include <string.h>

#ifdef FEATURE_EFS
#include "fs.h"                 /* File system typedefs and prototypes     */
#endif

#ifdef FEATURE_XMF
#include "xmf.h"                /* XMF typedefs and prototypes             */
#endif
/* <EJECT> */
/*===========================================================================

                          DECLARATIONS FOR MODULE

===========================================================================*/

/* Dynamic buffer alocation definitions
*/
#define CMX_MALLOC(size)     malloc(size)
#define CMX_FREE(buffer)     free(buffer)

/*---------------------------------------------------------------------------
** CMX Command Queue, Free Queue and Available Packets
**---------------------------------------------------------------------------
*/
LOCAL q_type           cmx_cmd_q;        /* Request Packets Queue           */
LOCAL q_type           cmx_cmd_q_hi;     /* Request Packets Priority Queue  */
LOCAL q_type           cmx_free_q;       /* Available Packets Queue         */
LOCAL cmx_packet_type  cmx_free_packet [CMX_NUM_PACKETS];

/* This parameter is used to indicate whether we have increased the VS task
** priority or not. This is useful in order to not call REX unnecessarily.
*/
boolean cmx_vs_in_high_pri = FALSE;

/* CMX handles used for midi/WebAudio playback.
*/
#define CMX_NUM_AF_HANDLES  10
cmx_af_handle_type cmx_audfmt_handle[CMX_NUM_AF_HANDLES];

/* Local CMX handles */
#define CMX_NUM_HANDLES  10
cmx_handle_type cmx_handle[CMX_NUM_HANDLES];

#ifdef FEATURE_AUDIO_FORMAT
/* Structure to keep track of data related to the sequence of data read
** commands issued. This is necessary because some data read callbacks have no
** indication of which read command the callback is in reference to.
*/
typedef struct {
  cmx_af_handle_type
                   *handle;
  snd_af_read_cb_func_type          /* Callback function registered by sound */
                   snd_cb_func;     /* server for the read data command      */
  void             *client_data;    /* Client (sound server) data associated */
                                    /* with the read data command            */
  uint8            *buffer;         /* Pointer to buffer issued to the EFS   */
                                    /* read command                          */
  uint32           position;        /* Starting position in the file of the  */
                                    /* requested data (temp debug variable)  */
  uint32           offset;          /* Offset in buffer to start data        */
  uint32           buffer_size;     /* Size of buffer                        */
} cmx_af_data_cb_struct_type;

/* Structure to manage list of data related to read commands. This is
** implemented as a circular buffer of size 33 (maximum number of simultaneous
** read requests is 33, one for each of 32 tracks supported and one more for
** one track). Plus one for QCP function reads.
*/
#define MAX_NUM_DATA_READ 34
typedef struct {
  uint32             index;                      /* Index of head of list    */
  uint32             number;                     /* Number of entries in use */
  cmx_af_data_cb_struct_type
                     data_cb[MAX_NUM_DATA_READ]; /* Data structures          */
} cmx_af_data_cb_type;

/* Holds info for data read callbacks */
cmx_af_data_cb_type cmx_af_data_cb_array;

/* This structure is used to retain information related to the various
** buffer sizes/numbers supported.
*/
typedef struct {
  uint32 max_tracks;    /* Maximum number of tracks that can utilize  */
                        /* this buffer size                           */
  uint32 num_buffers;   /* Number of buffers of this buffer size      */
  uint32 num_bytes;     /* Size, in bytes, of this buffer             */
} cmx_af_buf_info_type;

/* Data related to the various supported buffer sizes */
const cmx_af_buf_info_type cmx_af_buf_info[CMX_AUDFMT_BUF_INVALID] =
        { {0,  2,  CMX_MIN_BUF_SIZE*32},
          {1,  4,  CMX_MIN_BUF_SIZE*16},
          {3,  8,  CMX_MIN_BUF_SIZE*8 },
          {7,  16, CMX_MIN_BUF_SIZE*4 },
          {15, 32, CMX_MIN_BUF_SIZE*2 },
          {31, 64, CMX_MIN_BUF_SIZE   } };

#ifdef FEATURE_AUDFMT_MULTISEQUENCER
#define CMX_MAX_NUM_SEQUENCES 4
#else
#define CMX_MAX_NUM_SEQUENCES 1
#endif /* FEATURE_AUDFMT_MULTISEQUENCER */

/* Structure of buffers available for midi/WebAudio data retrieval */
cmx_audfmt_buf_struct cmx_af_buffers[CMX_MAX_NUM_SEQUENCES];

#ifdef FEATURE_WEBAUDIO
/* Maximum size of text buffer */
#define CMX_MAX_TEXT_STRING  400

#ifdef FEATURE_EFS
/* The following variables are used to keep track of the parameters necessary
** for returning raw picture data to the client. In order to minimize the
** memory usage, this is done over several passes.
*/
cmx_af_pic_enum_type cmx_pic_type;      /* Format of picture data            */
cmx_af_handle_type   *cmx_pic_handle;   /* Handle: client information        */
uint32               cmx_pic_position;  /* Current position in WebAudio file */
uint32               cmx_pic_remain;    /* Number of bytes remaining for pic */

/* Buffer for text data */
uint8  cmx_text_buffer[CMX_MAX_TEXT_STRING];
uint32 cmx_text_counting_semaphore;
#endif /* FEATURE_EFS */

/* This parameter is used to accumulate text advance commands so as to
** not overflow the CMX command buffer.
*/
uint32 cmx_text_advance_count;

/* Settings for Anim frame width and height for Anim frame commands within
** WebAudio files.
*/

#define CMX_AUDFMT_SCREEN_WIDTH    240
#define CMX_AUDFMT_SCREEN_HEIGHT   216

/*
   Set the global variable for Scaling the Anim frame height and width.
*/

uint32 cmx_af_screen_width = CMX_AUDFMT_SCREEN_WIDTH;
uint32 cmx_af_screen_height = CMX_AUDFMT_SCREEN_HEIGHT;

#endif /* FEATURE_WEBAUDIO */

/* Flag indicating whether midi/WebAudio is currently playing */
cmx_af_active_type cmx_af_active = CMX_AF_NONE;
cmx_af_active_type cmx_af_pend   = CMX_AF_NONE;

/* Count of active get time commands */
uint32 cmx_af_gt_active = 0;

/* Flag indicating whether midi/WebAudio is has been aborted */
cmx_af_stop_enum_type cmx_af_stopped = CMX_AF_STOP_NONE;

/* The following parameters are used to buffer up the next CMX play/ring
** command to be executed if there is a play/ring abort pending. The use of
** this one deep command buffer prevents the client from issuing too many
** consecutive play/ring commands before the current play/ring has been
** properly aborted in the sound task. There can currently only be one play/
** ring command executed at a time; and therefore only one play/ring command
** pending.
*/
typedef struct {
  cmx_cmd_type       cmd_pend;   /* Command pending */
  uint8              play_count;
  union {
    struct {
      cmx_handle_type                  *handle;
      cmx_af_play_param_type           param;
      cmx_audfmt_play_cb_func_ptr_type cb_func;
      void                             *client_data;
    } play[CMX_MAX_NUM_SEQUENCES];    /* Parameters for play command */
    struct {
      cmx_handle_type                  *handle;
      uint16                           repeat_timer;
      cmx_audfmt_start_enum_type       start_position;
      cmx_cb_func_ptr_type             cb_func;
      void                             *client_data;
    } ringer;     /* Parameters for ringer command */
    struct {
      cmx_handle_type                  *handle;
      cmx_internal_codec_spec_type     codec_spec;
      cmx_internal_av_sync_type        av_sync;
      cmx_cb_func_ptr_type             cb_func;
      void                             *client_data;
      cmx_interface_enum_type          interface_src;
    } play_codec; /* Parameters for play codec command */
    struct {
      cmx_handle_type                  *handle;
      cmx_internal_codec_spec_type     codec_spec;
      cmx_internal_av_sync_type        av_sync;
      cmx_audfmt_play_cb_func_ptr_type cb_func;
      void                             *client_data;
      cmx_interface_enum_type          interface_src;
    } av_sync_playback; /* Parameters for av_sync playback  command */
  } para;
} cmx_af_pending_command_type;

cmx_af_pending_command_type cmx_af_cmd_pend;

/* Buffer for audio spec command */
#define CMX_NUM_AUDIO_SPEC 4
cmx_af_audio_spec_type cmx_audio_spec_buffer[CMX_NUM_AUDIO_SPEC];

#ifdef FEATURE_QCP
/* The following flag is used to indicate that a QCP function is
** currently active. There may only be one QCP function active at a time
** but QCP functions may be active during playback.
*/
boolean cmx_qcp_func_active = FALSE;

/* The following is a buffer allocated for use with QCP fucntions.
*/
#define CMX_QCP_FUNC_BUFFER_SIZE 200
uint8 cmx_qcp_func_data_buffer[CMX_QCP_FUNC_BUFFER_SIZE];

/* The following parameters are used for QCP function get/set commands.
** They are used to save the paramters from the client.
*/
uint8   *cmx_qcp_func_buffer;
uint32  *cmx_qcp_func_length;

/* The following parameters are used in the case where a set labl function
** is called while recording the file. Normally this would not be supported
** because we are unable to actually write the labl chunk before we finish
** recording. This was added to be backwards compatible with existing
** voice memo functionality.
*/
boolean      cmx_qcp_set_labl_pend       = FALSE;
uint32       cmx_qcp_labl_pending_length = 0;
uint8        cmx_qcp_labl_pending_buffer[48];
cmx_cmd_type cmx_qcp_record_status_pending;

/* The following defines are used to determine the requirements for QCP
** file recording.
*/
#define CMX_QCP_RECORD_MIN_SIZE     (1750 + 194)
#define CMX_QCP_RECORD_WARNING_SIZE 8750
#define CMX_QCP_RECORD_DONE_SIZE    1750
#define CMX_QCP_MEM_OVERHEAD        200
#define CMX_QCP_FS_OVERHEAD         65535

/* The following flag is used to indicate whether a record warning has
** been issued to the client. We only want to warn once per recording session.
*/
boolean cmx_qcp_rec_warning_done;

/* The following structures are used to keep track of buffers that have
** been sent to be written to a QCP file. After the write is done, the
** buffer will be freed.
*/
typedef struct {
  cmx_af_handle_type *handle;
  uint8              *buffer; /* Pointer to buffer issued to write command */
#ifdef FEATURE_EFS
  fs_rsp_msg_type    rsp;     /* EFS response buffer                       */
#endif /* FEATURE_EFS */
} cmx_qcp_rec_write_cb_struct_type;

#define MAX_NUM_DATA_WRITE 20
typedef struct {
  uint32             index;                      /* Index of head of list    */
  uint32             number;                     /* Number of entries in use */
  cmx_qcp_rec_write_cb_struct_type
                     data_cb[MAX_NUM_DATA_WRITE];/* Data structures          */
} cmx_qcp_rec_write_cb_type;

cmx_qcp_rec_write_cb_type cmx_qcp_rec_write_cb_array;

/* The following variables are used to keep track of any buffer requests
** for QCP recording when buffers are currently not available. There should
** be a maximum of two pending get buffers.
*/
snd_qcp_buf_cb_func_type cmx_qcp_rec_get_buf_pending     = NULL;
snd_qcp_buf_cb_func_type cmx_qcp_rec_get_buf_pending_res = NULL;
cmx_af_handle_type       *cmx_qcp_rec_buf_handle         = NULL;
cmx_af_handle_type       *cmx_qcp_rec_buf_handle_res     = NULL;
#endif /* FEATURE_QCP */

#ifdef FEATURE_PCM_REC
/* Size of buffers to use for PCM recording
*/
#define CMX_PCM_REC_BUF_LENGTH    16000

/* These defines are used in determining memory space requirements for
** PCM recording.
*/
#define CMX_PCM_RECORD_HDR_SIZE     44
#define CMX_PCM_RECORD_MIN_SIZE     ((CMX_PCM_REC_BUF_LENGTH * 2) + \
                                     CMX_PCM_RECORD_HDR_SIZE)
#define CMX_PCM_RECORD_WARNING_SIZE ((CMX_PCM_REC_BUF_LENGTH * 5) + \
                                     CMX_PCM_RECORD_HDR_SIZE)
#define CMX_PCM_FS_OVERHEAD         65535
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/*
 * Size of buffers to use for MM recording
 */
#define CMX_MM_REC_BUF_LENGTH    1024 * 4

/* These defines are used in determining memory space requirements for
** MultiMedia recording.
*/
#define CMX_MM_RECORD_MIN_SIZE     (CMX_MM_REC_BUF_LENGTH * 2)

#define CMX_MM_RECORD_WARNING_SIZE (CMX_MM_REC_BUF_LENGTH * 5)

/* Threshold for file space check */
#define CMX_MM_FS_OVERHEAD         65535
#endif /* FEATURE_MM_REC */
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_CMX_IMAGE
/* The following variables are used to keep track of the image callback
** parameters for data reads. A maximum of 6 image data reads at a time
** will be supported.
*/
#define MAX_NUM_DATA_IMAGE_READ 6
typedef struct {
  cmx_image_enum_type       image_type;
  cmx_image_command_cb_type cb_func;
  void                      *client_data;
  uint8                     *buffer;
} cmx_image_read_cb_type;

typedef struct {
  uint32                 index;               /* Index of head of list    */
  uint32                 number;              /* Number of entries in use */
  cmx_image_read_cb_type image_read[MAX_NUM_DATA_IMAGE_READ];
} cmx_image_data_cb_type;

cmx_image_data_cb_type cmx_image_data_cb_array;

#ifdef FEATURE_WEBAUDIO
/* The following variable is used to keep track of the image callback
** parameters for buffer requests.
*/
typedef struct {
  cmx_image_enum_type       image_type;    /* Image decode type            */
  cmx_image_command_cb_type cb_func;       /* Image callback function ptr  */
  void                      *client_data;  /* Image client data            */
  uint8                     **buf_handle;  /* Buffer handle                */
} cmx_image_buf_req_cb_type;

cmx_image_buf_req_cb_type cmx_image_buf_req_cb_info;
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_CMX_IMAGE */

#ifdef FEATURE_WEBAUDIO
/* This value must be as large as two times the largest data buffer in order
** to not lose any image data
*/
#define CMX_MAX_PICTURE_BUFFER  (2*CMX_AF_MAX_BUFFER_SIZE)

/* This is the maximum buffer size of picture data being passed back
** to the client.
*/
#define CMX_PIC_MAX_INCREMENT   2048

/* Buffer for picture data */
uint8 cmx_picture_buffer[MAX(CMX_MAX_PICTURE_BUFFER, CMX_PIC_MAX_INCREMENT)];

/* This is the size of the buffer to request for picture commands
*/
#define CMX_PIC_BUFFER_SIZE  3200

#ifdef FEATURE_PNG_DECODER
/* The following variables are used to keep track of the PNG continue callback
** parameters. The PNG decoder will be resumed only after all outstanding
** data reads (both PNG and WebAudio) received at the time of the continue
** are serviced and returned.
*/
png_cmx_command_cb_func_type cmx_png_cont_cb_func_ptr = NULL;
void                         *cmx_png_cont_cb_data;

/* The following variables are used in PNG decoding where part of the PNG data
** may be in a buffer and part may be in EFS or another source.
*/
uint32 cmx_png_size;          /* Size of PNG image that is in WebAudio file   */
uint32 cmx_png_buf_bytes;     /* Number of PNG data bytes found in buffer     */
uint8  *cmx_png_buffer;       /* Buffer containing PNG data                   */
uint32 cmx_png_offset;        /* Offset into the PNG file where the remaining */
                              /* PNG data can be found                        */

/* Flag indicates that PNG will perform new decode */
boolean cmx_png_start_new = TRUE;

#ifdef FEATURE_WEBAUDIO
/* This flag indicates whether a PNG decode session from a WebAudio file is
** current.
*/
boolean cmx_png_audfmt_current = FALSE;

#if defined(FEATURE_PNG_DECODER) && defined(FEATURE_SAF)
/* These parameters are used to delay PNG/SAF decode commands
** from WebAudio files in case a SAF/PNG decode is currently active
*/
cmx_af_handle_type *cmx_saf_delay_handle = NULL;
cmx_af_handle_type *cmx_png_delay_handle = NULL;
#endif /* FEATURE_PNG_DECODER && FEATURE_SAF */
#endif /* FEATURE_WEBAUDIO */

#endif /* FEATURE_PNG_DECODER */

#if defined(FEATURE_PNG_DECODER) || defined(FEATURE_JPEG_DECODER)
/* This is the image decoder buffer that is used by the PNG and MLZ
** entropy decoders.   JPEG encoder and decoder also uses this buffer.
*/
#ifdef FEATURE_JPEG_DECODER
#define CMX_IMAGE_BUFFER_SIZE 88000
#else
#define CMX_IMAGE_BUFFER_SIZE 33280
#endif

uint8 cmx_image_buf[CMX_IMAGE_BUFFER_SIZE];

#endif /* FEATURE_PNG_DECODER || FEATURE_JPEG_DECODER */

#ifdef FEATURE_JPEG_DECODER
/* The following variables are used to keep track of the JPEG continue callback
** parameters. The JPEG decoder/encoder will be resumed only after all
** outstanding data reads received at the time of the continue are serviced
** and returned.
*/
jpeg_cmx_command_cb_func_type cmx_jpeg_cont_cb_func_ptr = NULL;
void                          *cmx_jpeg_cont_cb_data;

/* The following variables are used in JPEG decoding/encoding where part of the
** JPEG data may be in a buffer and part may be in EFS or another source.
*/
uint32 cmx_jpeg_size;       /* Size of JPEG image that is in WebAudio file   */
uint32 cmx_jpeg_buf_bytes;  /* Number of JPEG data bytes found in buffer     */
uint8  *cmx_jpeg_buffer;    /* Buffer containing JPEG data                   */
uint32 cmx_jpeg_offset;     /* Offset into the file where the remaining JPEG */
                            /* data can be found                             */
#endif /* FEATURE_JPEG_DECODER */
#endif /* FEATURE_WEBAUDIO */

#ifdef FEATURE_SAF
/* The following parameters are used to drive the SAF module.
*/
/* The width, height and bitdepth of a SAF frame are defined here. These
** values should be set to the desired maximum supported image settings.
*/
#ifdef FEATURE_BITMAP
#define CMX_SAF_FRAME_WIDTH     160
#define CMX_SAF_FRAME_HEIGHT    160
#else
#define CMX_SAF_FRAME_WIDTH     128
#define CMX_SAF_FRAME_HEIGHT    128
#endif /* FEATURE_BITMAP */
#define CMX_SAF_FRAME_BITDEPTH  8
#define CMX_SAF_BUFFER_SIZE     ((CMX_SAF_FRAME_WIDTH * CMX_SAF_FRAME_HEIGHT) \
                                 * (CMX_SAF_FRAME_BITDEPTH / 8))

/* This is the buffer size required for SAF get info command.
*/
#define CMX_SAF_INFO_BUFFER_SIZE 2048

/* This is the default object buffer size that is used by the SAF decoder.
*/
#define CMX_SAF_OBJECT_BUFFER_SIZE 65535

/* This parameter is used to indicate the handle of the currently playing
** SAF file. This is only used for the stand-alone SAF player.
*/
cmx_af_handle_type  *cmx_saf_handle_ptr = NULL;

/* This paramter is used to indicate the state of the SAF player. */
cmx_saf_status_type cmx_saf_status      = CMX_SAF_STATUS_STOPPED;

/* This paramter is used to indicate whether the stand-alone SAF player should
** be prompted for frames or whether the SAF player should free-run.
*/
cmx_saf_ctl_type cmx_saf_ctl;

boolean cmx_saf_audfmt_current = FALSE;

/* This callback function pointer is used to save the frame callback
** from the SAF module.
*/
saf_cmx_frame_cb_func_ptr_type cmx_saf_frame_cb = NULL;
void                           *cmx_saf_frame_client_data = NULL;

/* This parameter is used to indicate the next frame to get from the
** SAF module for the stand-alone free-running player.
*/
uint32 cmx_saf_frame_number;

/* This timer is used to time the delay between frames for the
** stand-alone free-running player.
*/
rex_timer_type cmx_saf_frame_timer;

/* The following parameters are used to store frame information while
** the stand-alone SAF player is paused. When the player resumes, this
** frame command, if any, will be issued.
*/
cmx_af_handle_type  *cmx_saf_pending_handle_ptr = NULL;
cmx_saf_status_type cmx_saf_pending_status;
uint8               *cmx_saf_pending_buffer;
uint32              cmx_saf_pending_num_bytes;
uint32              cmx_saf_pending_delay;
boolean             cmx_saf_pending_last_frame;

/* The following variables are used to keep track of the SAF continue callback
** parameters. The SAF module will be resumed only after all outstanding
** data reads (both SAF and WebAudio/MIDI) received at the time of the continue
** are serviced and returned.
*/
saf_cmx_command_cb_func_ptr_type cmx_saf_cont_cb_func_ptr = NULL;
void                             *cmx_saf_cont_cb_data;

uint32 cmx_saf_size;          /* Size of SAF file that is in WebAudio file    */
uint32 cmx_saf_buf_bytes;     /* Number of SAF data bytes found in buffer     */
uint8  *cmx_saf_buffer;       /* Buffer containing SAF data                   */
uint32 cmx_saf_offset;        /* Offset into the SAF file where the remaining */

#ifdef FEATURE_WEBAUDIO
/* The following variables are used to keep track of SAF frame ID commands from
** WebAudio files. These variables will be used to ensure that only one frame
** command is sent to the SAF module at any given time.
*/
typedef enum {
  CMX_SAF_FRAME_NONE,         /* No frame has sent to SAF decoder             */
  CMX_SAF_FRAME_PROC,         /* One frame has been sent to SAF decoder       */
  CMX_SAF_FRAME_PEND          /* A pending frame ID is stored                 */
} cmx_saf_frame_status_type;
cmx_saf_frame_status_type cmx_saf_audfmt_frame_status = CMX_SAF_FRAME_NONE;
uint32                    cmx_saf_audfmt_frame_id;
cmx_af_pos_struct_type    cmx_saf_audfmt_frame_pos;
cmx_af_pos_struct_type    cmx_saf_audfmt_frame_pos_pending;
#endif /* FEATURE_WEBUADIO */
#endif /* FEATURE_SAF */

#ifdef FEATURE_SVG
/* Size of Memory pool passed to SVG decoder */
#define CMX_SVG_MEM_BUFFER_SIZE    (250 * 1024) /* 250Kb of memory */

/* Max number of external file requests supported by CMX */
#define CMX_SVG_MAX_REQUESTS           10 /* Max num of external references */

/* Max time for rendering frames, if timer expires playback of SVG is aborted */
#define CMX_SVG_MAX_TIME           (24 * 60 * 1000)  /* Set to 24 hrs */

/* Min delay between successive frames. This controls max FPS supported.
  If this value is too low, CMX/GRAPH tasks might take up too much time and
  lower priority tasks will starve */
#define CMX_SVG_MIN_DELAY          50           /* ms, tunes performance */

/* String for creating empty SVG document, used by SVGDOM later on */
char   cmx_svg_empty_document[]  = "<svg></svg>";

cmx_svg_req_pk_type       cmx_svg_req;

/* Local SVG Decode data */
LOCAL q_type           cmx_svg_in_use_q;     /* Request Packets Priority Queue  */
LOCAL q_type           cmx_svg_free_q;       /* Available Packets Queue         */
cmx_svg_decode_pk_type cmx_svg_decode_handles[CMX_NUM_HANDLES];

uint32 cmx_svg_offset;   /* Offset into the file where SVG data is found */
uint32 cmx_svg_size;     /* Size of SVG file that is in WebAudio file    */

#ifdef FEATURE_SVGDOM
/* Local SVG DOM string data */
LOCAL q_type           cmx_svgdom_in_use_q;     /* Request Packets Priority Queue  */
LOCAL q_type           cmx_svgdom_free_q;       /* Available Packets Queue         */
cmx_svgdom_strpool_type cmx_svgdom_strings[CMX_NUM_HANDLES*3];
#endif /* FEATURE_SVGDOM */

#ifdef FEATURE_WEBAUDIO
/* The following variables are used to keep track of positions of SVG frame
** commands from WebAudio files.
*/
cmx_af_pos_struct_type    cmx_svg_audfmt_frame_pos;
cmx_af_pos_struct_type    cmx_svg_audfmt_frame_pos_pending;
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_SVG */

#ifdef FEATURE_MIDI_OUT
/* This size is based on a maximum of 32 midi commands (32 * 3 = 96). */
#define CMX_MIDI_OUT_BUFF_SIZE 96

/* Buffer to hold the list of midi commands */
uint8                cmx_midi_out_buffer[CMX_MIDI_OUT_BUFF_SIZE];

/* The following enum is used to indicate the state of MIDI device */
typedef enum {
  CMX_MIDI_OUT_STATUS_DISABLED,        /* MIDI device API is disabled */
  CMX_MIDI_OUT_STATUS_DISABLE_PENDING, /* API disable is pending      */
  CMX_MIDI_OUT_STATUS_ENABLED,         /* MIDI device API is enabled  */
  CMX_MIDI_OUT_STATUS_ENABLE_PENDING   /* API enable is pending       */
} cmx_midi_out_status_enum_type;

/* This structure contains information about MIDI device session */
typedef struct {
  cmx_midi_out_status_enum_type status;       /* State of MIDI device */
  cmx_cb_func_ptr_type          cb_func;      /* Callback function    */
  void                          *client_data; /* Client data          */
} cmx_midi_out_status_type;

/* MIDI device session status */
cmx_midi_out_status_type cmx_midi_out_status = {CMX_MIDI_OUT_STATUS_DISABLED,
                                                NULL, NULL};

#ifdef FEATURE_MIDI_OUT_QCP
#ifdef FEATURE_MULTIPLE_ADEC_CHANNELS
#define CMX_MIDI_OUT_QCP_CHANNELS 4
#else
#define CMX_MIDI_OUT_QCP_CHANNELS 1
#endif /* FEATURE_MULTIPLE_ADEC_CHANNELS */

LOCAL q_type cmx_midi_out_free_q;    /* Available MIDI out QCP handles Queue */
LOCAL q_type cmx_midi_out_in_use_q;  /* In use MIDI out QCP handles Queue    */

/* MIDI device QCP handles. These handles are used to control the MIDI
** device audio decoder channels.
*/
cmx_midi_out_qcp_handle_type cmx_midi_out_handles[CMX_MIDI_OUT_QCP_CHANNELS];

#ifdef FEATURE_WAVE_PB
/* This parameter is used to keep track of the midi_out_qcp handle
** that has been reserved for ADPCM playback. When ADPCM file is being
** played via the cmx_audfmt_play() interface, a midi_out_qcp channel must
** be reserved so that midi_out_qcp() commands will not cancel ADPCM playback.
** ADPCM played via play command is considered higher priority than midi_out
** API.
*/
cmx_midi_out_qcp_handle_type *cmx_midi_out_adpcm_handle_ptr = NULL;
#endif /* FEATURE_WAVE_PB */
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
/* Buffer and information to be used for data reads for QCP playback for
** audio decoder.
*/
#ifdef FEATURE_MULTIPLE_ADEC_CHANNELS
#define CMX_ADEC_MAX_NUM_BUFFERS 8
#else
#define CMX_ADEC_MAX_NUM_BUFFERS 3
#endif /* FEATURE_MULTIPLE_ADEC_CHANNELS */

#ifdef FEATURE_QSYNTH_ADPCM
/* ADPCM requires a larger buffer due to the higher throughput requirement.
*/
#define CMX_ADEC_READ_BUFF_SIZE  1000
#else
#define CMX_ADEC_READ_BUFF_SIZE  300
#endif /* FEATURE_QSYNTH_ADPCM */
/* This structure is used to keep track of the MIDI device audio decoder
** buffers.
*/
typedef struct {
  uint32  in_use;    /* In use bitmask flag. */
  uint8   buffer[CMX_ADEC_MAX_NUM_BUFFERS][CMX_ADEC_READ_BUFF_SIZE];
} cmx_adec_buffer_type;

/* MIDI device audio decoder buffers. */
cmx_adec_buffer_type cmx_adec_read_buffer;

/* This type is used to control the MIDI device audio decoder data read
** command callbacks.
*/
#define CMX_ADEC_MAX_CB_STRUCTS 10
typedef struct {
  uint32             index;                      /* Index of head of list    */
  uint32             number;                     /* Number of entries in use */
  cmx_af_data_cb_struct_type
                     data_cb[CMX_ADEC_MAX_CB_STRUCTS]; /* Data structures   */
} cmx_adec_data_cb_type;

/* Holds info for MIDI device audio decoder data read callbacks */
cmx_adec_data_cb_type cmx_adec_data_cb;
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

#ifdef FEATURE_QAUDIOFX
/* The following defines the number of 3D positional callback structures
** that will be pre-allocated.
*/
#define CMX_3D_POS_CB_NUM_ENTRIES 20

/* 3D positional callback structures */
cmx_3d_pos_cb_struct_type cmx_3d_pos_cb_array[CMX_3D_POS_CB_NUM_ENTRIES];

#endif /* FEATURE_QAUDIOFX */

#ifdef FEATURE_AUDIO_POSTCARD
/* Size of header for Audio Postcard file is 93 bytes
*/
#define CMX_AUDIOP_HEADER_SIZE  93

/* Generic header for Audio Postcard file.
*/
const uint8 cmx_audiop_header [CMX_AUDIOP_HEADER_SIZE] = {
  0x63, 0x6d, 0x69, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41,
  0x02, 0x0a, 0x01, 0x73, 0x6f, 0x72, 0x63, 0x00, 0x01, 0x00,
  0x63, 0x6f, 0x64, 0x65, 0x00, 0x01, 0x00, 0x76, 0x65, 0x72,
  0x73, 0x00, 0x04, 0x30, 0x33, 0x30, 0x30, 0x6e, 0x6f, 0x74,
  0x65, 0x00, 0x02, 0x00, 0x01, 0x65, 0x78, 0x73, 0x74, 0x00,
  0x02, 0x00, 0x00, 0x70, 0x63, 0x70, 0x69, 0x00, 0x01, 0x00,
  0x63, 0x6e, 0x74, 0x73, 0x00, 0x09, 0x57, 0x41, 0x56, 0x45,
  0x3b, 0x50, 0x49, 0x43, 0x54, 0x74, 0x72, 0x61, 0x63, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xff, 0xf3, 0x00, 0x00, 0x00, 0x42,
  0x00, 0x66, 0x66
};

/* Length in bytes of fullrate 13k QCP frame */
#define CMX_QCP_FULLRATE_SIZE 35

/* Rate byte value for fullrate frame */
#define CMX_QCP_FULLRATE_BYTE 4

/* Block size of 500 ms of fullrate 13k QCP frames */
#define CMX_QCP_BLOCK_SIZE    (CMX_QCP_FULLRATE_SIZE * 25)

/* Size of buffer space available to read full 500ms blocks of QCP data */
#define CMX_AUDIOP_QCP_BLOCK_SIZE \
         ((CMX_MIN_BUF_SIZE*64) - ((CMX_MIN_BUF_SIZE*64) % CMX_QCP_BLOCK_SIZE))

#endif /* FEATURE_AUDIO_POSTCARD */

#ifdef FEATURE_XMF
cmx_af_handle_type *cmx_active_xmf_handle = NULL;
#endif /* FEATURE_XMF */
/* <EJECT> */
/*===========================================================================

FUNCTION cmx_init

DESCRIPTION
  This procedure initializes the CMX command queues.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet

===========================================================================*/
void cmx_init ( void )
{
  q_link_type *link_item;
  int         i;

  /* Initialize the command queues.
  */
  (void) q_init (&cmx_cmd_q);              /* Initialize the command queue  */
  (void) q_init (&cmx_cmd_q_hi);           /* Initialize the Pri Cmd queue  */
  (void) q_init (&cmx_free_q);             /* Initialize the free queue     */

  /* Add items to cmx_free_q.
  */
  for (i = 0; i < CMX_NUM_PACKETS; i++)
  {
    link_item = q_link( &cmx_free_packet[i], &cmx_free_packet[i].hdr.link );
    q_put( &cmx_free_q, link_item );
  }

#ifdef FEATURE_SAF
  /* Initialize timer */
  rex_def_timer( &cmx_saf_frame_timer, &vs_tcb, VS_CMX_SAF_FRAME_SIG );
#endif

#ifdef FEATURE_AUDIO_FORMAT
  /* Initialize audio pending command to none */
  cmx_af_cmd_pend.cmd_pend   = CMX_CMD_INVALID;
  cmx_af_cmd_pend.play_count = 0;

  for(i = 0; i < CMX_MAX_NUM_SEQUENCES; i++) {
    cmx_af_buffers[i].buf_type = CMX_AUDFMT_BUF_INVALID;
  }

  for(i = 0; i < CMX_NUM_AUDIO_SPEC; i++) {
    cmx_audio_spec_buffer[i].file_type = CMX_AF_FILE_UNKNOWN;
  }
#endif /* FEATURE_AUDIO_FORMAT */

  /* Initialize local CMX handles */
  for(i = 0; i < CMX_NUM_AF_HANDLES; i++) {
    cmx_audfmt_handle[i].cmd = CMX_CMD_INVALID;
  }

  for(i = 0; i < CMX_NUM_HANDLES; i++) {
    cmx_handle[i].source_type = CMX_SOURCE_INVALID;
  }

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
  /* Initialize MIDI device audio decoder data read callback structure.
  */
  cmx_adec_data_cb.number     = 0;
  cmx_adec_data_cb.index      = 0;
  cmx_adec_read_buffer.in_use = 0;
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

#ifdef FEATURE_MIDI_OUT_QCP
  (void) q_init (&cmx_midi_out_free_q);      /* Initialize free queue   */
  (void) q_init (&cmx_midi_out_in_use_q);    /* Initialize in use queue */

  /* Add items to cmx_midi_out_free_q.
  */
  for (i = 0; i < CMX_MIDI_OUT_QCP_CHANNELS; i++)
  {
    cmx_midi_out_handles[i].channel = i;
    link_item = q_link( &cmx_midi_out_handles[i],
                        &cmx_midi_out_handles[i].link );
    q_put( &cmx_midi_out_free_q, link_item );
  }
#endif /* FEATURE_MIDI_OUT_QCP */

#ifdef FEATURE_QAUDIOFX
  /* Initialize 3D positional callback structures */
  for (i = 0; i < CMX_3D_POS_CB_NUM_ENTRIES; i++) {
    cmx_3d_pos_cb_array[i].cb_func.base_cb = NULL;
  }
#endif /* FEATURE_QAUDIOFX */

#ifdef FEATURE_SVG
  (void) q_init( &cmx_svg_free_q);
  (void) q_init( &cmx_svg_in_use_q);

  for (i = 0; i < CMX_NUM_HANDLES; i++)
  {
    link_item = q_link( &cmx_svg_decode_handles[i],
                        &cmx_svg_decode_handles[i].link );
    q_put( &cmx_svg_free_q, link_item );
  }

#ifdef FEATURE_SVGDOM
  (void) q_init( &cmx_svgdom_free_q);
  (void) q_init( &cmx_svgdom_in_use_q);

  memset( (void*)cmx_svgdom_strings, 0,
          sizeof(cmx_svgdom_strpool_type)*CMX_NUM_HANDLES*3);

  for (i = 0; i < CMX_NUM_HANDLES*3; i++)
  {

    link_item = q_link( &cmx_svgdom_strings[i],
                        &cmx_svgdom_strings[i].link );
    q_put( &cmx_svgdom_free_q, link_item );
  }
#endif /* FEATURE_SVGDOM */

#endif
} /* end cmx_init */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_cmd

DESCRIPTION
  Queue a command for processing by the CMX Services.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  cmx_init() needs to have initialized the queues.

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  vs_tcb

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void cmx_cmd
(
  cmx_packet_type *cmx_cmd_ptr
    /* pointer to VS command */
)
{
  (void) q_link (cmx_cmd_ptr, &cmx_cmd_ptr->hdr.link);    /* Init link field */

  q_put (&cmx_cmd_q, &cmx_cmd_ptr->hdr.link);       /* Put on command queue  */
  (void) rex_set_sigs (&vs_tcb, VS_CMX_CMD_Q_SIG);  /* Signal a queue event  */

} /* end cmx_cmd */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_cmd_hi

DESCRIPTION
  Queue a command in the priority queue for processing by the CMX Services.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  cmx_init() needs to have initialized the queues.

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q_hi
  vs_tcb

===========================================================================*/
void cmx_cmd_hi
(
  cmx_packet_type *cmx_cmd_ptr
    /* pointer to VS command */
)
{
  (void) q_link (cmx_cmd_ptr, &cmx_cmd_ptr->hdr.link);    /* Init link field */

  q_put (&cmx_cmd_q_hi, &cmx_cmd_ptr->hdr.link);    /* Put on command queue  */

  if( !cmx_vs_in_high_pri ) {
    /* Increase the priority of the VS task */
    cmx_vs_in_high_pri = TRUE;
    (void) rex_task_pri(&vs_tcb, VS_PRI_HI);
  }

  (void) rex_set_sigs (&vs_tcb, VS_CMX_CMD_Q_SIG);  /* Signal a queue event  */

} /* end cmx_cmd_hi */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_cmd_get_pkt

DESCRIPTION
  Get a CMX Command Packet.  Use by callers of cmx_cmd().

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  cmx_init() needs to have initialized the queues.

RETURN VALUE
  A pointer to an available cmx_packet_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  cmx_free_q (and the packet fetched off cmx_free_q)

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
cmx_packet_type *cmx_cmd_get_pkt (void)
{
  cmx_packet_type *packet;

  /* Get a packet and then either report an error, or clear the packet out
  ** so the previous user's garbage doesn't do something unexpected.
  */
  packet = (cmx_packet_type *) (q_get (&cmx_free_q));

  if (packet == NULL)
  {
    ERR ("Ran Out of CMX Packets!", 0, 0, 0);
  }
  else
  {
    /* Fill the entire CMX packet with 0's to get default behavior on
    ** un-initialized fields.
    */
    (void) memset (packet, 0, sizeof (cmx_packet_type));
    packet->hdr.cmd = CMX_CMD_INVALID;     /* Invalid Command Value   */
  }

  return (packet);

} /* end cmx_cmd_get_pkt */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_cmd_parser

DESCRIPTION
  This procedure processes requests received by cmx_cmd().  See cmxi.h for
  a description of the actual CMX packets.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  cmx_init() must have been called to init the command queue.

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_cmd_q
  cmx_cmd_q_hi

===========================================================================*/
void cmx_cmd_parser (void)
{
  cmx_packet_type *cmd_ptr;

  /* While there are commands to process, process each command.
  */
  while (((cmd_ptr = (cmx_packet_type *) q_get (&cmx_cmd_q_hi)) != NULL) ||
         ((cmd_ptr = (cmx_packet_type *) q_get (&cmx_cmd_q)) != NULL)) {

    /* Process each command type.
    */
    switch (cmd_ptr->hdr.cmd)
    {
#ifdef FEATURE_AUDIO_FORMAT
      case CMX_AF_CMD_PREPARSE:
        cmd_ptr->audfmt_preparse.preparse_func
                                (cmd_ptr->audfmt_preparse.client_data);
        break;

      case CMX_AF_CMD_PLAY_CODEC:
        cmx_audfmt_play_codec_processing(cmd_ptr->audfmt_play_codec.handle,
                                      &cmd_ptr->audfmt_play_codec.codec_spec,
                                      &cmd_ptr->audfmt_play_codec.av_sync,
                                      cmd_ptr->audfmt_play_codec.cb_func,
                                     cmd_ptr->audfmt_play_codec.client_data,
                                     cmd_ptr->audfmt_play_codec.interface_src);
        break;
        case CMX_AF_CMD_AVSYNC_PLAYBACK:
          cmx_audfmt_avsync_playback_processing(cmd_ptr->audfmt_play_codec.handle,
                                      &cmd_ptr->audfmt_play_codec.codec_spec,
                                      &cmd_ptr->audfmt_play_codec.av_sync,
                                      cmd_ptr->audfmt_play_codec.av_sync_cb_func,
                                     cmd_ptr->audfmt_play_codec.client_data,
                                     cmd_ptr->audfmt_play_codec.interface_src);
          break;

      case CMX_AF_CMD_PLAY:
        cmx_audfmt_play_processing(cmd_ptr->audfmt_play.handle,
                                 &cmd_ptr->audfmt_play.param,
                                 cmd_ptr->audfmt_play.cb_func,
                                 cmd_ptr->audfmt_play.client_data);
        break;

      case CMX_AF_CMD_RINGER:
        cmx_audfmt_ringer_processing(cmd_ptr->audfmt_ringer.handle,
                                     cmd_ptr->audfmt_ringer.repeat_timer,
                                     cmd_ptr->audfmt_ringer.start_position,
                                     cmd_ptr->audfmt_ringer.cb_func,
                                     cmd_ptr->audfmt_ringer.client_data);
        break;

      case CMX_AF_CMD_STOP:
        cmx_audfmt_stop_processing(cmd_ptr->base.cb_func,
                                   cmd_ptr->base.client_data);
        break;

      case CMX_AF_CMD_REWIND:
        cmx_audfmt_rewind_processing(cmd_ptr->audfmt_play_ctl.time_ms,
                                     cmd_ptr->audfmt_play_ctl.cb_func,
                                     cmd_ptr->audfmt_play_ctl.client_data);
        break;

      case CMX_AF_CMD_FFORWARD:
        cmx_audfmt_fforward_processing(cmd_ptr->audfmt_play_ctl.time_ms,
                                       cmd_ptr->audfmt_play_ctl.cb_func,
                                       cmd_ptr->audfmt_play_ctl.client_data);
        break;

      case CMX_AF_CMD_PAUSE:
        cmx_audfmt_pause_processing(TRUE, cmd_ptr->base.cb_func,
                                          cmd_ptr->base.client_data);
        break;

      case CMX_AF_CMD_SEEK_PAUSE:
        cmx_audfmt_seek_pause_processing(cmd_ptr->base.cb_func,
                                         cmd_ptr->base.client_data);
        break;

      case CMX_AF_CMD_RESUME:
        cmx_audfmt_pause_processing(FALSE, cmd_ptr->base.cb_func,
                                           cmd_ptr->base.client_data);
        break;

      case CMX_AF_CMD_FLUSH:
        cmx_audfmt_flush_processing(cmd_ptr->base.cb_func,
                                    cmd_ptr->base.client_data);
        break;

      case CMX_AF_CMD_TEMPO:
        cmx_audfmt_tempo_processing(cmd_ptr->audfmt_play_ctl.time_ms,
                                    cmd_ptr->audfmt_play_ctl.cb_func,
                                    cmd_ptr->audfmt_play_ctl.client_data);
        break;

      case CMX_AF_CMD_TUNE:
        cmx_audfmt_tune_processing(cmd_ptr->audfmt_play_ctl.time_ms,
                                   cmd_ptr->audfmt_play_ctl.cb_func,
                                   cmd_ptr->audfmt_play_ctl.client_data);
        break;

      case CMX_AF_CMD_PAN:
        cmx_audfmt_pan_processing(cmd_ptr->audfmt_play_ctl.time_ms,
                                  cmd_ptr->audfmt_play_ctl.cb_func,
                                  cmd_ptr->audfmt_play_ctl.client_data);
        break;

      case CMX_AF_CMD_SEQ_STOP:
        cmx_audfmt_seq_stop_processing(cmd_ptr->audfmt_seq_cmd.sequence,
                                       cmd_ptr->audfmt_seq_cmd.cb_func,
                                       cmd_ptr->audfmt_seq_cmd.client_data);
        break;

      case CMX_AF_CMD_SEQ_PAUSE:
        cmx_audfmt_seq_pause_processing(cmd_ptr->audfmt_seq_cmd.sequence, TRUE,
                                        cmd_ptr->audfmt_seq_cmd.cb_func,
                                        cmd_ptr->audfmt_seq_cmd.client_data);
        break;

      case CMX_AF_CMD_SEQ_RESUME:
        cmx_audfmt_seq_pause_processing(cmd_ptr->audfmt_seq_cmd.sequence, FALSE,
                                        cmd_ptr->audfmt_seq_cmd.cb_func,
                                        cmd_ptr->audfmt_seq_cmd.client_data);
        break;

      case CMX_AF_CMD_SEQ_FFORWARD:
        cmx_audfmt_seq_fforward_processing(cmd_ptr->audfmt_seq_cmd.sequence,
                                           cmd_ptr->audfmt_seq_cmd.num,
                                           cmd_ptr->audfmt_seq_cmd.cb_func,
                                           cmd_ptr->audfmt_seq_cmd.client_data);
        break;

      case CMX_AF_CMD_SEQ_REWIND:
        cmx_audfmt_seq_rewind_processing(cmd_ptr->audfmt_seq_cmd.sequence,
                                         cmd_ptr->audfmt_seq_cmd.num,
                                         cmd_ptr->audfmt_seq_cmd.cb_func,
                                         cmd_ptr->audfmt_seq_cmd.client_data);
        break;

      case CMX_AF_CMD_SEQ_TEMPO:
        cmx_audfmt_seq_tempo_processing(cmd_ptr->audfmt_seq_cmd.sequence,
                                        cmd_ptr->audfmt_seq_cmd.num,
                                        cmd_ptr->audfmt_seq_cmd.cb_func,
                                        cmd_ptr->audfmt_seq_cmd.client_data);
        break;

      case CMX_AF_CMD_SEQ_TUNE:
        cmx_audfmt_seq_tune_processing(cmd_ptr->audfmt_seq_cmd.sequence,
                                       cmd_ptr->audfmt_seq_cmd.num,
                                       cmd_ptr->audfmt_seq_cmd.cb_func,
                                       cmd_ptr->audfmt_seq_cmd.client_data);
        break;

      case CMX_AF_CMD_SEQ_PAN:
        cmx_audfmt_seq_pan_processing(cmd_ptr->audfmt_seq_cmd.sequence,
                                      (uint16)cmd_ptr->audfmt_seq_cmd.num,
                                      cmd_ptr->audfmt_seq_cmd.cb_func,
                                      cmd_ptr->audfmt_seq_cmd.client_data);
        break;

      case CMX_AF_CMD_SEQ_VOL:
        cmx_audfmt_seq_vol_processing(cmd_ptr->audfmt_seq_cmd.sequence,
                                      (uint16)cmd_ptr->audfmt_seq_cmd.num,
                                      cmd_ptr->audfmt_seq_cmd.cb_func,
                                      cmd_ptr->audfmt_seq_cmd.client_data);
        break;

      case CMX_AF_CMD_GET_TIME:
        cmx_audfmt_get_time_processing(cmd_ptr->audfmt_time.handle,
                                       cmd_ptr->audfmt_time.cb_func,
                                       cmd_ptr->audfmt_time.client_data);
        break;

      case CMX_AF_CMD_GET_TIME_ABORT:
        cmx_audfmt_get_time_abort_processing();
        break;

      case CMX_AF_CMD_GET_SPEC:
        cmx_audfmt_get_spec_processing(cmd_ptr->audfmt_get_spec.handle,
                                       cmd_ptr->audfmt_get_spec.cb_func,
                                       cmd_ptr->audfmt_get_spec.client_data);
        break;

      case CMX_AF_CMD_READ:
        cmx_audfmt_read_processing(cmd_ptr->audfmt_read.handle,
                                   cmd_ptr->audfmt_read.position,
                                   cmd_ptr->audfmt_read.client_data,
                                   cmd_ptr->audfmt_read.cb_func);
        break;

      case CMX_AF_CMD_READ_VAR:
        cmx_audfmt_read_var_processing(cmd_ptr->audfmt_read.cmd,
                                       cmd_ptr->audfmt_read.handle,
                                       cmd_ptr->audfmt_read.position,
                                       cmd_ptr->audfmt_read.buffer_size,
                                       cmd_ptr->audfmt_read.offset,
                                       cmd_ptr->audfmt_read.client_data,
                                       cmd_ptr->audfmt_read.cb_func);
        break;

      case CMX_AF_CMD_GET_DLS:
        cmx_audfmt_dls_processing(cmd_ptr->audfmt_get_dls.handle,
                                  cmd_ptr->audfmt_get_dls.length,
                                  cmd_ptr->audfmt_get_dls.position,
                                  cmd_ptr->audfmt_get_dls.client_data,
                                  cmd_ptr->audfmt_get_dls.cb_func);
        break;

      case CMX_AF_CMD_READ_RSP:
        if(cmd_ptr->audfmt_read_rsp.cb_func != NULL) {
          cmd_ptr->audfmt_read_rsp.cb_func (
                                       cmd_ptr->audfmt_read_rsp.client_data,
                                       cmd_ptr->audfmt_read_rsp.buff,
                                       cmd_ptr->audfmt_read_rsp.length,
                                       cmd_ptr->audfmt_read_rsp.position);
        }
        break;

      case CMX_AF_CMD_CB:
        cmx_audfmt_callback_processing( cmd_ptr->audfmt_cb.client_data,
                                        cmd_ptr->audfmt_cb.status );
        break;

      case CMX_AF_CMD_PLAY_DONE_CB:
        cmx_audfmt_play_done_processing( cmd_ptr->audfmt_cb.status,
                                         cmd_ptr->audfmt_cb.client_data );
        break;

      case CMX_AF_CMD_SUPPORT:
        cmx_audfmt_support_processing(cmd_ptr->audfmt_support.command,
                                      cmd_ptr->audfmt_support.handle,
                                      cmd_ptr->audfmt_support.position,
                                      cmd_ptr->audfmt_support.number);
        break;

      case CMX_AF_CMD_AUDIO_SPEC:
        cmx_audfmt_audio_spec_processing(cmd_ptr->audfmt_audio_spec.handle,
                                         cmd_ptr->audfmt_audio_spec.file_type,
                                         cmd_ptr->audfmt_audio_spec.audio_spec);
        break;

      case CMX_AF_CMD_GET_TIME_CB:
        cmx_audfmt_get_time_cb_processing(cmd_ptr->audfmt_get_time_cb.handle,
                                          cmd_ptr->audfmt_get_time_cb.status,
                                          cmd_ptr->audfmt_get_time_cb.time_ms);
        break;

      case CMX_AF_CMD_GET_SPEC_CB:
        cmx_audfmt_get_spec_cb_processing(cmd_ptr->audfmt_get_spec_cb.handle,
                                          cmd_ptr->audfmt_get_spec_cb.status);
        break;
#ifdef FEATURE_WEBAUDIO
      case CMX_AF_CMD_PIC_INIT:
        cmx_audfmt_pic_init_processing(cmd_ptr->audfmt_pic_init.handle,
                                       cmd_ptr->audfmt_pic_init.draw_speed,
                                       cmd_ptr->audfmt_pic_init.xpos_type,
                                       cmd_ptr->audfmt_pic_init.ypos_type,
                                       cmd_ptr->audfmt_pic_init.xpos_percent,
                                       cmd_ptr->audfmt_pic_init.ypos_percent);
        break;

      case CMX_AF_CMD_PIC_DATA:
        cmx_audfmt_pic_data_processing(cmd_ptr->audfmt_pic_data.pic_type,
                                       cmd_ptr->audfmt_pic_data.buffer,
                                       cmd_ptr->audfmt_pic_data.buf_len,
                                       cmd_ptr->audfmt_pic_data.handle,
                                       cmd_ptr->audfmt_pic_data.position,
                                       cmd_ptr->audfmt_pic_data.remain_bytes);
        break;

      case CMX_AF_CMD_PIC_CTL:
        cmx_audfmt_pic_ctl_processing(cmd_ptr->audfmt_pic_ctl.command,
                                      cmd_ptr->audfmt_pic_ctl.handle,
                                      cmd_ptr->audfmt_pic_ctl.speed );
        break;

#ifdef FEATURE_EFS
      case CMX_AF_CMD_PIC_READ:
        cmx_af_pic_read_processing();
        break;
#endif /* FEATURE_EFS */

      case CMX_AF_CMD_TEXT_CTL:
        cmx_audfmt_text_ctl_processing(cmd_ptr->audfmt_text_ctl.command,
                                       cmd_ptr->audfmt_text_ctl.buffer,
                                       cmd_ptr->audfmt_text_ctl.buf_len,
                                       cmd_ptr->audfmt_text_ctl.handle,
                                       cmd_ptr->audfmt_text_ctl.position,
                                       cmd_ptr->audfmt_text_ctl.remain_bytes,
                                       cmd_ptr->audfmt_text_ctl.xpos_type,
                                       cmd_ptr->audfmt_text_ctl.ypos_type);
        break;

      case CMX_AF_CMD_ANIM:
        cmx_audfmt_anim_data_processing(cmd_ptr->audfmt_anim_data.type,
                                        cmd_ptr->audfmt_anim_data.buffer,
                                        cmd_ptr->audfmt_anim_data.buf_len,
                                        cmd_ptr->audfmt_anim_data.handle,
                                        cmd_ptr->audfmt_anim_data.position,
                                        cmd_ptr->audfmt_anim_data.remain_bytes);
        break;

      case CMX_AF_CMD_ANIM_FRAME:
        cmx_audfmt_anim_frame_processing(cmd_ptr->audfmt_anim_data.type,
                                        cmd_ptr->audfmt_anim_data.buffer,
                                        cmd_ptr->audfmt_anim_data.buf_len,
                                        cmd_ptr->audfmt_anim_data.handle,
                                        cmd_ptr->audfmt_anim_data.position,
                                        cmd_ptr->audfmt_anim_data.remain_bytes,
                                        &cmd_ptr->audfmt_anim_data.frame_pos);
        break;

      case CMX_AF_CMD_ANIM_FRAME_ID:
        cmx_audfmt_anim_frame_id_processing(cmd_ptr->audfmt_anim_data.type,
                                          cmd_ptr->audfmt_anim_data.handle,
                                          cmd_ptr->audfmt_anim_data.position,
                                          &cmd_ptr->audfmt_anim_data.frame_pos);
        break;

#ifdef FEATURE_SMAF
      case CMX_AF_CMD_SMAF_GRAPH_INFO:
        cmx_audfmt_smaf_graph_info_processing(cmd_ptr->audfmt_graph_info.handle,
                                              cmd_ptr->audfmt_graph_info.info);
        break;

      case CMX_AF_CMD_SMAF_LED_CTL:
        cmx_audfmt_smaf_led_ctl_processing(cmd_ptr->audfmt_smaf_led_vib_ctl.handle,
                                           cmd_ptr->audfmt_smaf_led_vib_ctl.led_ctl);
        break;

      case CMX_AF_CMD_SMAF_VIB_CTL:
        cmx_audfmt_smaf_vib_ctl_processing(cmd_ptr->audfmt_smaf_led_vib_ctl.handle,
                                           cmd_ptr->audfmt_smaf_led_vib_ctl.vib_ctl);
        break;
#endif /* FEATURE_SMAF */

      case CMX_AF_CMD_MFI_LED_CTL:
        cmx_audfmt_mfi_led_ctl_processing(cmd_ptr->audfmt_mfi_led_ctl.handle,
                                          cmd_ptr->audfmt_mfi_led_ctl.mask);
        break;
#endif /* FEATURE_WEBAUDIO */

#if defined(FEATURE_IMELODY) || defined(FEATURE_WEBAUDIO)
      case CMX_AF_CMD_LED_CTL:
        cmx_audfmt_led_ctl_processing(cmd_ptr->audfmt_led_ctl.handle,
                                      cmd_ptr->audfmt_led_ctl.led_ctl);
        break;
#endif /* FEATURE_IMELODY || FEATURE_WEBAUDIO */

#ifdef FEATURE_IMELODY
      case CMX_AF_CMD_BACKLIGHT_CTL:
        cmx_audfmt_blight_ctl_processing(cmd_ptr->audfmt_blight_ctl.handle,
                                      cmd_ptr->audfmt_blight_ctl.backlight_ctl);
        break;
#endif

#ifdef FEATURE_QCP
      case CMX_QCP_GET_DATA_SIZE:
        cmx_qcp_get_func_processing ( CMX_QCP_GET_DATA_SIZE,
                                      cmd_ptr->qcp_func.handle,
                                      cmd_ptr->qcp_func.num_bytes,
                                      cmd_ptr->qcp_func.buffer,
                                      cmd_ptr->qcp_func.cb_func,
                                      cmd_ptr->qcp_func.client_data );
        break;

      case CMX_QCP_GET_FRAMES:
        cmx_qcp_get_func_processing ( CMX_QCP_GET_FRAMES,
                                      cmd_ptr->qcp_func.handle,
                                      cmd_ptr->qcp_func.num_bytes,
                                      cmd_ptr->qcp_func.buffer,
                                      cmd_ptr->qcp_func.cb_func,
                                      cmd_ptr->qcp_func.client_data );
        break;

      case CMX_QCP_GET_CNFG:
        cmx_qcp_get_func_processing ( CMX_QCP_GET_CNFG,
                                      cmd_ptr->qcp_func.handle,
                                      cmd_ptr->qcp_func.num_bytes,
                                      cmd_ptr->qcp_func.buffer,
                                      cmd_ptr->qcp_func.cb_func,
                                      cmd_ptr->qcp_func.client_data );
        break;

      case CMX_QCP_SET_CNFG:
        cmx_qcp_set_func_processing ( CMX_QCP_SET_CNFG,
                                      cmd_ptr->qcp_func.handle,
                                      cmd_ptr->qcp_func.length,
                                      cmd_ptr->qcp_func.buffer,
                                      cmd_ptr->qcp_func.cb_func,
                                      cmd_ptr->qcp_func.client_data );
        break;

      case CMX_QCP_GET_LABL:
        cmx_qcp_get_func_processing ( CMX_QCP_GET_LABL,
                                      cmd_ptr->qcp_func.handle,
                                      cmd_ptr->qcp_func.num_bytes,
                                      cmd_ptr->qcp_func.buffer,
                                      cmd_ptr->qcp_func.cb_func,
                                      cmd_ptr->qcp_func.client_data );
        break;

      case CMX_QCP_SET_LABL:
        cmx_qcp_set_func_processing ( CMX_QCP_SET_LABL,
                                      cmd_ptr->qcp_func.handle,
                                      cmd_ptr->qcp_func.length,
                                      cmd_ptr->qcp_func.buffer,
                                      cmd_ptr->qcp_func.cb_func,
                                      cmd_ptr->qcp_func.client_data );
        break;

      case CMX_QCP_GET_TEXT:
        cmx_qcp_get_func_processing ( CMX_QCP_GET_TEXT,
                                      cmd_ptr->qcp_func.handle,
                                      cmd_ptr->qcp_func.num_bytes,
                                      cmd_ptr->qcp_func.buffer,
                                      cmd_ptr->qcp_func.cb_func,
                                      cmd_ptr->qcp_func.client_data );
        break;

      case CMX_QCP_SET_TEXT:
        cmx_qcp_set_func_processing ( CMX_QCP_SET_TEXT,
                                      cmd_ptr->qcp_func.handle,
                                      cmd_ptr->qcp_func.length,
                                      cmd_ptr->qcp_func.buffer,
                                      cmd_ptr->qcp_func.cb_func,
                                      cmd_ptr->qcp_func.client_data );
        break;

      case CMX_QCP_FUNC_FAIL:
        cmx_qcp_func_done(CMX_FAILURE, cmd_ptr->qcp_func_cb.handle,
                          cmd_ptr->qcp_func_cb.buffer,
                          cmd_ptr->qcp_func_cb.num_bytes);
        break;

      case CMX_QCP_FUNC_DONE:
        cmx_qcp_func_done(CMX_DONE, cmd_ptr->qcp_func_cb.handle,
                          cmd_ptr->qcp_func_cb.buffer,
                          cmd_ptr->qcp_func_cb.num_bytes);
        break;

      case CMX_QCP_CMD_REC:
        cmx_qcp_record_processing ( cmd_ptr->qcp_record.tx_handle,
                                    cmd_ptr->qcp_record.rx_handle,
                                    &cmd_ptr->qcp_record.rec_param,
                                    cmd_ptr->qcp_record.cb_func,
                                    cmd_ptr->qcp_record.client_data );
        break;

      case CMX_QCP_CMD_REC_STOP:
        cmx_qcp_record_stop_processing ( cmd_ptr->base.cb_func,
                                         cmd_ptr->base.client_data );
        break;

      case CMX_QCP_CMD_REC_PAUSE:
        cmx_qcp_record_pause_processing ( cmd_ptr->base.cb_func,
                                          cmd_ptr->base.client_data );
        break;

      case CMX_QCP_CMD_REC_RESUME:
        cmx_qcp_record_resume_processing ( cmd_ptr->base.cb_func,
                                           cmd_ptr->base.client_data );
        break;

      case CMX_QCP_CMD_REC_GET_BUF:
        cmx_qcp_rec_get_buf_processing ( cmd_ptr->qcp_rec_get_buf.handle,
                                         cmd_ptr->qcp_rec_get_buf.cb_func );
        break;

      case CMX_QCP_CMD_REC_DONE:
      case CMX_QCP_CMD_REC_AUTO_STOP:
      case CMX_QCP_CMD_REC_EFS_DONE:
      case CMX_QCP_CMD_REC_ERROR:
        cmx_qcp_rec_done_processing(cmd_ptr->hdr.cmd,
                                    cmd_ptr->qcp_rec_data.handle);
        break;

      case CMX_QCP_CMD_REC_TIME:
        cmx_qcp_rec_status_processing(cmd_ptr->qcp_rec_data.handle,
                                      CMX_TICK_UPDATE);
        break;

      case CMX_QCP_CMD_REC_RESUME_S:
        cmx_qcp_rec_status_processing(cmd_ptr->qcp_rec_data.handle, CMX_RESUME);
        break;

      case CMX_QCP_CMD_REC_PAUSE_S:
        cmx_qcp_rec_status_processing(cmd_ptr->qcp_rec_data.handle, CMX_PAUSE);
        break;

      case CMX_QCP_CMD_REC_DATA:
        cmx_qcp_rec_data_processing ( cmd_ptr->qcp_rec_data.handle,
                                      cmd_ptr->qcp_rec_data.buffer,
                                      cmd_ptr->qcp_rec_data.num_bytes,
                                      cmd_ptr->qcp_rec_data.position,
                                      cmd_ptr->qcp_rec_data.link,
                                      cmd_ptr->qcp_rec_data.content );
        break;
#endif /* FEATURE_QCP */

#ifdef FEATURE_PCM_REC
      case CMX_PCM_CMD_REC:
        cmx_pcm_record_processing ( cmd_ptr->pcm_record.handle,
                                    &cmd_ptr->pcm_record.rec_param,
                                    cmd_ptr->pcm_record.cb_func,
                                    cmd_ptr->pcm_record.client_data );
        break;

      case CMX_PCM_CMD_REC_STOP:
        cmx_pcm_record_stop_processing ( cmd_ptr->pcm_rec_cmd.link,
                                         cmd_ptr->pcm_rec_cmd.cb_func,
                                         cmd_ptr->pcm_rec_cmd.client_data );
        break;

      case CMX_PCM_CMD_REC_PAUSE:
        cmx_pcm_record_pause_processing ( cmd_ptr->pcm_rec_cmd.link,
                                          cmd_ptr->pcm_rec_cmd.cb_func,
                                          cmd_ptr->pcm_rec_cmd.client_data );
        break;

      case CMX_PCM_CMD_REC_RESUME:
        cmx_pcm_record_resume_processing ( cmd_ptr->pcm_rec_cmd.link,
                                           cmd_ptr->pcm_rec_cmd.cb_func,
                                           cmd_ptr->pcm_rec_cmd.client_data );
        break;

      case CMX_PCM_CMD_REC_DATA:
      case CMX_PCM_CMD_REC_HEADER_DATA:
        cmx_pcm_record_data_processing ( cmd_ptr->hdr.cmd,
                                         cmd_ptr->pcm_rec_data.handle,
                                         cmd_ptr->pcm_rec_data.buffer,
                                         cmd_ptr->pcm_rec_data.num_bytes,
                                         cmd_ptr->pcm_rec_data.position,
                                         cmd_ptr->pcm_rec_data.buf_cb,
                                         cmd_ptr->pcm_rec_data.client_data );
        break;

      case CMX_PCM_CMD_REC_STATUS:
        cmx_pcm_record_status_processing ( cmd_ptr->pcm_rec_status.handle,
                                           cmd_ptr->pcm_rec_status.status  );
        break;

      case CMX_PCM_CMD_REC_DONE:
        cmx_pcm_record_done_processing ( cmd_ptr->pcm_rec_status.handle,
                                         cmd_ptr->pcm_rec_status.status  );
        break;
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
      case CMX_MM_CMD_REC:
        cmx_mm_record_processing (cmd_ptr->mm_record.mm_handle,
                                  &cmd_ptr->mm_record.rec_param,
                                  cmd_ptr->mm_record.cb_func,
                                  cmd_ptr->mm_record.client_data);
        break;

      case CMX_MM_CMD_REC_STOP:
        cmx_mm_record_stop_processing (cmd_ptr->mm_rec_cmd.link,
                                       cmd_ptr->mm_rec_cmd.cb_func,
                                       cmd_ptr->mm_rec_cmd.client_data);
        break;

      case CMX_MM_CMD_REC_PAUSE:
        cmx_mm_record_pause_processing (cmd_ptr->mm_rec_cmd.link,
                                        cmd_ptr->mm_rec_cmd.cb_func,
                                        cmd_ptr->mm_rec_cmd.client_data);
        break;

      case CMX_MM_CMD_REC_RESUME:
        cmx_mm_record_resume_processing (cmd_ptr->mm_rec_cmd.link,
                                         cmd_ptr->mm_rec_cmd.cb_func,
                                         cmd_ptr->mm_rec_cmd.client_data);
        break;

      case CMX_MM_CMD_REC_DATA:
      case CMX_MM_CMD_REC_HEADER_DATA:
        cmx_mm_record_data_processing (cmd_ptr->hdr.cmd,
                                       cmd_ptr->mm_rec_data.handle,
                                       cmd_ptr->mm_rec_data.buffer,
                                       cmd_ptr->mm_rec_data.num_bytes,
                                       cmd_ptr->mm_rec_data.position,
                                       cmd_ptr->mm_rec_data.buf_cb,
                                       cmd_ptr->mm_rec_data.client_data);
        break;

      case CMX_MM_CMD_REC_STATUS:
        cmx_mm_record_status_processing (cmd_ptr->mm_rec_status.handle,
                                         cmd_ptr->mm_rec_status.status);
        break;

      case CMX_MM_CMD_REC_DONE:
        cmx_mm_record_done_processing (cmd_ptr->mm_rec_status.handle,
                                       cmd_ptr->mm_rec_status.status);
        break;
#endif /* FEATURE_MM_REC */
#endif /* FEATURE_AUDIO_FORMAT */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SAF) || defined(FEATURE_SVG)
      case CMX_CMD_CLIENT_BUF_REQ:
        cmx_client_buf_req_processing(cmd_ptr->client_buf_req.handle,
                                      cmd_ptr->client_buf_req.buf_handle,
                                      cmd_ptr->client_buf_req.num_bytes);
        break;

      case CMX_CMD_CLIENT_BUF_REQ_CB:
        cmx_client_buf_req_cb_processing(cmd_ptr->client_buf_req_cb.buffer,
                                         cmd_ptr->client_buf_req_cb.buf_handle);
        break;
#endif /* FEATURE_WEBAUDIO || FEATURE_SAF || FEATURE_SVG */

#ifdef FEATURE_CMX_IMAGE
      case CMX_IMAGE_CMD_CONTINUE:
        cmx_image_continue_processing( cmd_ptr->image_cb.image_type,
                                       cmd_ptr->image_cb.cb_func,
                                       cmd_ptr->image_cb.client_data );
        break;

      case CMX_IMAGE_CMD_DO_CB:
        cmx_image_cb_processing( cmd_ptr->image_cb.image_type,
                                 cmd_ptr->image_cb.cb_func,
                                 cmd_ptr->image_cb.client_data );
        break;

      case CMX_IMAGE_CMD_READ:
        cmx_image_read_processing ( cmd_ptr->image_read.image_type,
                                    cmd_ptr->image_read.handle,
                                    cmd_ptr->image_read.buffer,
                                    cmd_ptr->image_read.num_bytes,
                                    cmd_ptr->image_read.position,
                                    cmd_ptr->image_read.cb_func,
                                    cmd_ptr->image_read.client_data);
        break;

#ifdef FEATURE_WEBAUDIO
      case CMX_IMAGE_CMD_BUF_REQ:
        cmx_image_buf_req_processing( cmd_ptr->image_buf_req.image_type,
                                      cmd_ptr->image_buf_req.handle,
                                      cmd_ptr->image_buf_req.buf_handle,
                                      cmd_ptr->image_buf_req.num_bytes,
                                      cmd_ptr->image_buf_req.cb_func,
                                      cmd_ptr->image_buf_req.client_data );
        break;
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_CMX_IMAGE */

#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_PNG_DECODER
      case CMX_PNG_CMD_DECODE:
        cmx_png_decode_processing(cmd_ptr->image_decode.handle,
                                  cmd_ptr->image_decode.cb_func,
                                  cmd_ptr->image_decode.client_data);
        break;

      case CMX_PNG_CMD_ABORT:
        cmx_png_abort_processing(cmd_ptr->base.cb_func,
                                 cmd_ptr->base.client_data);
        break;

      case CMX_PNG_CMD_SPEC:
        cmx_png_spec_processing(cmd_ptr->image_decode.handle,
                                cmd_ptr->image_decode.cb_func,
                                cmd_ptr->image_decode.client_data);
        break;
#endif /* FEATURE_PNG_DECODER */

#ifdef FEATURE_JPEG_DECODER
      case CMX_JPEG_CMD_DECODE:
        cmx_jpeg_decode_processing(cmd_ptr->image_decode.handle,
                                   cmd_ptr->image_decode.cb_func,
                                   cmd_ptr->image_decode.client_data);
        break;

      case CMX_JPEG_CMD_SPEC:
        cmx_jpeg_spec_processing(cmd_ptr->image_decode.handle,
                                 cmd_ptr->image_decode.cb_func,
                                 cmd_ptr->image_decode.client_data);
        break;

      case CMX_JPEG_CMD_ABORT_DECODE:
        cmx_jpeg_decode_abort_processing(cmd_ptr->base.cb_func,
                                         cmd_ptr->base.client_data);
        break;
#endif /* FEATURE_JPEG_DECODER */
#endif /* FEATURE_WEBAUDIO */

#ifdef FEATURE_SAF
      case CMX_SAF_CMD_DECODE:
        cmx_saf_decode_processing(cmd_ptr->image_decode.handle,
                                  cmd_ptr->image_decode.saf_ctl,
                                  cmd_ptr->image_decode.cb_func,
                                  cmd_ptr->image_decode.client_data);
        break;

      case CMX_SAF_CMD_DECODE_CONT:
        cmx_saf_decode_cont_processing(cmd_ptr->saf_cont.handle,
                                       cmd_ptr->saf_cont.saf_ctl);
        break;

      case CMX_SAF_CMD_FRAME_CONT:
        cmx_saf_frame_cont_processing(cmd_ptr->saf_cont.handle);
        break;

      case CMX_SAF_CMD_DO_FRAME_CB:
        if((cmx_saf_frame_cb != NULL) &&
           (cmd_ptr->saf_frame_id.frame_buffer != NULL)) {
          cmx_saf_frame_cb((int32) cmd_ptr->saf_frame_id.frame_id,
                           cmd_ptr->saf_frame_id.frame_buffer,
                           cmx_saf_frame_client_data);
        }
        break;

      case CMX_SAF_CMD_FRAME_DONE:
        cmx_saf_frame_done_processing(cmd_ptr->saf_frame_done.handle,
                                      cmd_ptr->saf_frame_done.buffer,
                                      cmd_ptr->saf_frame_done.num_bytes,
                                      cmd_ptr->saf_frame_done.delay,
                                      cmd_ptr->saf_frame_done.last_frame);
        break;

      case CMX_SAF_CMD_ABORT:
        cmx_saf_abort_processing(cmd_ptr->base.cb_func,
                                 cmd_ptr->base.client_data);
        break;

      case CMX_SAF_CMD_FRAME_ID:
        cmx_saf_frame_id_cmd_processing(cmd_ptr->saf_frame_id.frame_id,
                                        cmd_ptr->saf_frame_id.cb_func,
                                        cmd_ptr->saf_frame_id.client_data);
        break;

      case CMX_SAF_CMD_PAUSE:
        cmx_saf_pause_processing(cmd_ptr->base.cb_func,
                                 cmd_ptr->base.client_data);
        break;

      case CMX_SAF_CMD_RESUME:
        cmx_saf_resume_processing(cmd_ptr->base.cb_func,
                                  cmd_ptr->base.client_data);
        break;

      case CMX_SAF_CMD_INFO:
        cmx_saf_info_processing(cmd_ptr->image_decode.handle,
                                cmd_ptr->image_decode.cb_func,
                                cmd_ptr->image_decode.client_data);
        break;

      case CMX_SAF_CMD_INFO_CONT:
        cmx_saf_info_cont_processing(cmd_ptr->saf_cont.handle);
        break;
#endif /* FEATURE_SAF */

#ifdef FEATURE_MIDI_OUT
      case CMX_MIDI_OUT_OPEN:
        cmx_midi_out_processing(CMX_MIDI_OUT_ENABLE,
                                cmd_ptr->midi_out_open.cb_func,
                                cmd_ptr->midi_out_open.adec_mode,
                                cmd_ptr->midi_out_open.client_data);
        break;

      case CMX_MIDI_OUT_CLOSE:
        cmx_midi_out_processing(CMX_MIDI_OUT_DISABLE,
                                cmd_ptr->midi_out_open.cb_func,
                                cmd_ptr->midi_out_open.adec_mode,
                                cmd_ptr->midi_out_open.client_data);
        break;

      case CMX_MIDI_OUT_MSG:
        cmx_midi_out_msg_processing(cmd_ptr->midi_out.handle,
                                    cmd_ptr->midi_out.cb_func,
                                    cmd_ptr->midi_out.client_data);
        break;

#ifdef FEATURE_MIDI_OUT_QCP
      case CMX_MIDI_OUT_QCP:
        cmx_midi_out_qcp_processing(cmd_ptr->midi_out_qcp.handle,
                                    &cmd_ptr->midi_out_qcp.adec_param,
                                    cmd_ptr->midi_out_qcp.qcp_cb_func,
                                    cmd_ptr->midi_out_qcp.client_data);
        break;

      case CMX_MIDI_OUT_ADEC_START_CHAN:
        cmx_midi_out_adec_start_chan_processing(
                                          cmd_ptr->midi_out_qcp.midi_out_handle,
                                          cmd_ptr->midi_out_qcp.cb_func,
                                          cmd_ptr->midi_out_qcp.client_data);
        break;

      case CMX_MIDI_OUT_QCP_STOP:
        cmx_midi_out_qcp_stop_processing(cmd_ptr->midi_out_qcp.midi_out_handle,
                                         cmd_ptr->midi_out_qcp.cb_func,
                                         cmd_ptr->midi_out_qcp.client_data);
        break;

      case CMX_MIDI_OUT_QCP_PAUSE:
        cmx_midi_out_qcp_pause_processing(TRUE,
                                          cmd_ptr->midi_out_qcp.cb_func,
                                          cmd_ptr->midi_out_qcp.client_data);
        break;

      case CMX_MIDI_OUT_QCP_RESUME:
        cmx_midi_out_qcp_pause_processing(FALSE,
                                          cmd_ptr->midi_out_qcp.cb_func,
                                          cmd_ptr->midi_out_qcp.client_data);
        break;

      case CMX_MIDI_OUT_QCP_PAUSE_CHAN:
        cmx_midi_out_qcp_pause_chan_processing(
                                          TRUE,
                                          cmd_ptr->midi_out_qcp.midi_out_handle,
                                          cmd_ptr->midi_out_qcp.cb_func,
                                          cmd_ptr->midi_out_qcp.client_data);
        break;

      case CMX_MIDI_OUT_QCP_RESUME_CHAN:
        cmx_midi_out_qcp_pause_chan_processing(
                                          FALSE,
                                          cmd_ptr->midi_out_qcp.midi_out_handle,
                                          cmd_ptr->midi_out_qcp.cb_func,
                                          cmd_ptr->midi_out_qcp.client_data);
        break;

      case CMX_MIDI_OUT_QCP_PAN:
        cmx_midi_out_qcp_pan_processing(cmd_ptr->midi_out_qcp.midi_out_handle,
                                        cmd_ptr->midi_out_qcp.adec_param.pan,
                                        cmd_ptr->midi_out_qcp.cb_func,
                                        cmd_ptr->midi_out_qcp.client_data);
        break;

      case CMX_MIDI_OUT_QCP_VOLUME:
        cmx_midi_out_qcp_vol_processing(cmd_ptr->midi_out_qcp.midi_out_handle,
                                        cmd_ptr->midi_out_qcp.adec_param.volume,
                                        cmd_ptr->midi_out_qcp.cb_func,
                                        cmd_ptr->midi_out_qcp.client_data);
        break;

      case CMX_MIDI_OUT_QCP_RESET:
        cmx_midi_out_qcp_reset_processing(cmd_ptr->midi_out_qcp.af_handle);
        break;

#ifdef FEATURE_WAVE_PB
      case CMX_MIDI_OUT_QCP_RESERVE_CHANNEL:
        cmx_midi_out_qcp_reserve_channel_processing();
        break;
#endif /* FEATURE_WAVE_PB */

#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */


#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
      case CMX_ADEC_READ:
        cmx_adec_read_processing(cmd_ptr->audfmt_read.handle,
                                 cmd_ptr->audfmt_read.position,
                                 cmd_ptr->audfmt_read.client_data,
                                 cmd_ptr->audfmt_read.cb_func);
        break;
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

#ifdef FEATURE_QAUDIOFX
      case CMX_CMD_3D_POS_ENABLE:
        cmx_3d_pos_enable_processing(cmd_ptr->cmd_3d_pos.param.enable_cmd.mode,
                                     cmd_ptr->cmd_3d_pos.cb_func,
                                     cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_POS_RESET:
        cmx_3d_pos_reset_processing(cmd_ptr->cmd_3d_pos.cb_func,
                                    cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_POS_SET_LSNR_POS:
        cmx_3d_pos_lsnr_pos_processing(&cmd_ptr->cmd_3d_pos.param.lsnr_pos_cmd,
                                       cmd_ptr->cmd_3d_pos.cb_func,
                                       cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_POS_SET_LSNR_ORIENT:
        cmx_3d_pos_lsnr_orient_processing(
                                    &cmd_ptr->cmd_3d_pos.param.lsnr_orient_cmd,
                                    cmd_ptr->cmd_3d_pos.cb_func,
                                    cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_POS_SET_LSNR_VEL:
        cmx_3d_pos_lsnr_vel_processing(&cmd_ptr->cmd_3d_pos.param.lsnr_vel_cmd,
                                       cmd_ptr->cmd_3d_pos.cb_func,
                                       cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_REVERB_ENABLE:
        cmx_3d_reverb_enable_processing(TRUE, cmd_ptr->cmd_3d_pos.cb_func,
                                      cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_REVERB_SET_PRESET:
      case CMX_CMD_3D_REVERB_SET_LEVEL:
      case CMX_CMD_3D_REVERB_SET_TIME:
      case CMX_CMD_3D_REVERB_SET_RATIO:
        cmx_3d_reverb_cmd_processing( cmd_ptr->hdr.cmd,
                                     &cmd_ptr->cmd_3d_pos.param,
                                      cmd_ptr->cmd_3d_pos.cb_func,
                                      cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_REVERB_DISABLE:
        cmx_3d_reverb_enable_processing(FALSE, cmd_ptr->cmd_3d_pos.cb_func,
                                              cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_POS_SRC_ENABLE:
        cmx_3d_pos_src_enable_processing(
                                   &cmd_ptr->cmd_3d_pos.param.src_enable_cmd,
                                   cmd_ptr->cmd_3d_pos.cb_func,
                                   cmd_ptr->cmd_3d_pos.client_data
                                   );
        break;

      case CMX_CMD_3D_POS_SRC_RESET:
        cmx_3d_pos_src_reset_processing(
                                   &cmd_ptr->cmd_3d_pos.param.src_reset_cmd,
                                   cmd_ptr->cmd_3d_pos.cb_func,
                                   cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_POS_SET_SRC_POS:
        cmx_3d_pos_src_pos_processing(&cmd_ptr->cmd_3d_pos.param.src_pos_cmd,
                                      cmd_ptr->cmd_3d_pos.cb_func,
                                      cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_POS_SET_SRC_VOL:
        cmx_3d_pos_src_vol_processing(&cmd_ptr->cmd_3d_pos.param.src_volume_cmd,
                                      cmd_ptr->cmd_3d_pos.cb_func,
                                      cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_POS_SET_SRC_VOL_ATTEN:
        cmx_3d_pos_src_vol_atten_processing(
                              &cmd_ptr->cmd_3d_pos.param.src_vol_atten_cmd,
                              cmd_ptr->cmd_3d_pos.cb_func,
                              cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_POS_SET_SRC_VEL:
        cmx_3d_pos_src_vel_processing(&cmd_ptr->cmd_3d_pos.param.src_vel_cmd,
                                      cmd_ptr->cmd_3d_pos.cb_func,
                                      cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_REVERB_SET_SRC_VOL:
        cmx_3d_reverb_src_vol_processing(&cmd_ptr->cmd_3d_pos.param.src_reverb_vol_cmd,
                                      cmd_ptr->cmd_3d_pos.cb_func,
                                      cmd_ptr->cmd_3d_pos.client_data);
        break;

      case CMX_CMD_3D_POS_QUERY_ENV:
        cmx_3d_pos_query_env_processing(cmd_ptr->query_3d_pos.param,
                                        cmd_ptr->query_3d_pos.cb_func,
                                        cmd_ptr->query_3d_pos.client_data);
        break;

      case CMX_CMD_3D_POS_QUERY_SRC:
        cmx_3d_pos_query_src_processing(cmd_ptr->query_3d_pos.param,
                                        cmd_ptr->query_3d_pos.id,
                                        cmd_ptr->query_3d_pos.cb_func,
                                        cmd_ptr->query_3d_pos.client_data);
        break;

      case CMX_CMD_3D_POS_DO_CB:
        cmx_3d_pos_cb_processing(cmd_ptr->cb_3d_pos.status,
                                 cmd_ptr->cb_3d_pos.cb_struct);
        break;

      case CMX_CMD_3D_POS_DO_QUERY_CB:
        cmx_3d_pos_query_cb_processing(cmd_ptr->query_cb_3d_pos.status,
                                       cmd_ptr->query_cb_3d_pos.param,
                                       &cmd_ptr->query_cb_3d_pos.setting,
                                       cmd_ptr->query_cb_3d_pos.cb_struct);
        break;
#endif /* FEATURE_QAUDIOFX */

#ifdef FEATURE_AUDIO_POSTCARD
      case CMX_CMD_CREATE_AUDIOP:
        cmx_create_audiop_processing(cmd_ptr->create_audiop.jpeg,
                                     cmd_ptr->create_audiop.qcp,
                                     cmd_ptr->create_audiop.output,
                                     cmd_ptr->create_audiop.cb_func,
                                     cmd_ptr->create_audiop.client_data);
        break;

      case CMX_CMD_COMPLETE_AUDIOP:
        cmx_complete_audiop_processing(cmd_ptr->complete_audiop.status,
                                       cmd_ptr->complete_audiop.handle,
                                       cmd_ptr->complete_audiop.num_bytes,
                                       cmd_ptr->complete_audiop.position);
        break;
#endif /* FEATURE_AUDIO_POSTCARD */

#ifdef FEATURE_DLS
      case CMX_AF_CMD_DLS_CB:
        cmx_af_dls_callback_processing(cmd_ptr->audfmt_support.command,
                                       cmd_ptr->audfmt_support.handle);
        break;

      case CMX_AF_CMD_LOAD_DLS:
        cmx_audfmt_load_dls_processing(cmd_ptr->audfmt_dls.handle,
                                       cmd_ptr->audfmt_dls.cb_func,
                                       cmd_ptr->audfmt_dls.client_data);
        break;

      case CMX_AF_CMD_UNLOAD_DLS:
        cmx_audfmt_unload_dls_processing(cmd_ptr->audfmt_dls.cb_func,
                                         cmd_ptr->audfmt_dls.client_data);
        break;
#endif /* FEATURE_DLS */

#ifdef FEATURE_XMF
      case CMX_XMF_DATA_CB:
        cmx_xmf_client_read_cb_processing(cmd_ptr->xmf_cmd.handle,
                                          cmd_ptr->xmf_cmd.status,
                                          cmd_ptr->xmf_cmd.num_bytes);
        break;

      case CMX_XMF_CONTINUE_PROCESS:
        cmx_xmf_continue_processing(cmd_ptr->xmf_cmd.handle);
        break;
#endif /* FEATURE_XMF */

#if defined( FEATURE_SVG ) && defined(FEATURE_WEBAUDIO)
    case CMX_SVG_CMD_DECODE:
      cmx_svg_decode_processing( cmd_ptr->image_decode.handle,
                                 &cmd_ptr->image_decode.svg_decode->svg_decode,
                                 cmd_ptr->image_decode.cb_func,
                                 cmd_ptr->image_decode.client_data);
      q_put (&cmx_svg_free_q, &cmd_ptr->image_decode.svg_decode->link);
      break;

    case CMX_SVG_CMD_INFO:
      cmx_svg_info_processing( cmd_ptr->image_decode.handle,
                               &cmd_ptr->image_decode.svg_decode->svg_decode,
                               cmd_ptr->image_decode.cb_func,
                               cmd_ptr->image_decode.client_data);
      q_put (&cmx_svg_free_q, &cmd_ptr->image_decode.svg_decode->link);
      break;

    case CMX_SVG_CMD_START_PLAY:
      cmx_svg_play_processing( cmd_ptr->svg_cont.handle, &cmd_ptr->svg_cont);
      break;

    case CMX_SVG_CMD_SEEK:
      cmx_svg_frame_processing( cmd_ptr->svg_frame.handle,
                                cmd_ptr->svg_frame.time_ms,
                                TRUE,
                                cmd_ptr->svg_frame.callback_ptr,
                                cmd_ptr->svg_frame.client_data);
      break;

    case CMX_SVG_CMD_FRAME:
      cmx_svg_frame_processing( cmd_ptr->svg_frame.handle,
                                cmd_ptr->svg_frame.time_ms,
                                FALSE,
                                cmd_ptr->svg_frame.callback_ptr,
                                cmd_ptr->svg_frame.client_data);
      break;

    case CMX_SVG_CMD_ABORT:
      cmx_svg_abort_processing( cmd_ptr->svg_base.handle,
                                cmd_ptr->svg_base.cb_func,
                                cmd_ptr->svg_base.client_data );

      break;

    case CMX_SVG_CMD_PAUSE:
      cmx_svg_pause_processing( cmd_ptr->svg_base.handle,
                                cmd_ptr->svg_base.cb_func,
                                cmd_ptr->svg_base.client_data );
      break;

    case CMX_SVG_CMD_RESUME:
      cmx_svg_resume_processing( cmd_ptr->svg_base.handle,
                                 cmd_ptr->svg_base.cb_func,
                                 cmd_ptr->svg_base.client_data );
      break;

    case CMX_SVG_CMD_REWIND:
      cmx_svg_rewind_processing( cmd_ptr->svg_seek.handle,
                                 cmd_ptr->svg_seek.time_ms,
                                 cmd_ptr->svg_seek.callback_ptr,
                                 cmd_ptr->svg_seek.client_data );
      break;

    case CMX_SVG_CMD_FFORWARD:
      cmx_svg_fforward_processing( cmd_ptr->svg_seek.handle,
                                 cmd_ptr->svg_seek.time_ms,
                                 cmd_ptr->svg_seek.callback_ptr,
                                 cmd_ptr->svg_seek.client_data );
      break;

      case CMX_SVG_CMD_RESET:
        cmx_svg_reset_processing( cmd_ptr->svg_base.handle,
                                   cmd_ptr->svg_base.cb_func,
                                   cmd_ptr->svg_base.client_data );
        break;

    case CMX_SVG_CMD_EVENT:
      graph_svg_process_event( cmd_ptr->svg_event.handle,
                               cmd_ptr->svg_event.event_type,
                               (void*)&cmd_ptr->svg_event.event_data,
                               cmd_ptr->svg_event.time_ms);
      break;

    case CMX_SVG_CMD_REQ:
      cmx_svg_request_processing( cmd_ptr->svg_req.svg_req->handle,
                                  cmd_ptr->svg_req.svg_req );
      break;

    case CMX_SVG_CMD_REQ_DATA:
      cmx_svg_req_data_processing( cmd_ptr->svg_req_data.handle,
                                   cmd_ptr->svg_req_data.buffer,
                                   cmd_ptr->svg_req_data.num_bytes,
                                   cmd_ptr->svg_req_data.position);
      break;

    case CMX_SVG_CMD_REQ_CB:
      cmx_svg_req_cb_processing( cmd_ptr->svg_req_cb.handle,
                                 cmd_ptr->svg_req_cb.buffer,
                                 cmd_ptr->svg_req_cb.num_bytes);
      break;

    case CMX_SVG_CMD_SET_VIEWPORT:
      cmx_svg_set_viewport_processing( cmd_ptr->svg_viewport.handle,
                                       cmd_ptr->svg_viewport.width,
                                       cmd_ptr->svg_viewport.height,
                                       cmd_ptr->svg_viewport.callback_ptr,
                                       cmd_ptr->svg_viewport.client_data);
      break;

    case CMX_SVG_CMD_FREE:
      if( cmd_ptr->svg_base.handle != NULL ){
        if( cmd_ptr->svg_base.handle->cmd == CMX_SVG_CMD_DECODE){
          cmx_svg_cleanup( cmd_ptr->svg_base.handle);
        } else if( cmd_ptr->svg_base.handle->cmd == CMX_AF_CMD_PLAY){
          cmx_audfmt_svg_cleanup( cmd_ptr->svg_base.handle);
        }
      }
      break;

#ifdef FEATURE_SVGDOM
    case CMX_SVG_CMD_DOM:
      cmx_svgdom_processing( cmd_ptr->svgdom_data.handle,
                              cmd_ptr->svgdom_data.type,
                              &cmd_ptr->svgdom_data.data );
      break;
#endif /* FEATURE_SVGDOM */

#ifdef FEATURE_WEBAUDIO
      case CMX_CMD_SVG_FRAME_DONE:
        cmx_af_svg_frame_done_processing(cmd_ptr->svg_frame_done.handle,
                                         cmd_ptr->svg_frame_done.frame_buffer,
                                         cmd_ptr->svg_frame_done.frame_size);
        break;

      case CMX_CMD_SVG_FRAME_BUFFER:
        cmx_af_svg_frame_buffer_processing(cmd_ptr->svg_frame_buffer.handle);
        break;
#endif /* FEATURE_WEBAUDIO */
#endif /* defined( FEATURE_SVG ) && defined(FEATURE_WEBAUDIO) */

#ifdef FEATURE_AUDFMT_HYBRID_MODE
      case CMX_AF_CMD_SET_POLY_MODE:
        cmx_af_multi_poly_mode_processing(
          cmd_ptr->audfmt_poly_mode.mode,
          cmd_ptr->audfmt_poly_mode.cb_func,
          cmd_ptr->audfmt_poly_mode.client_data
        );
        break;
      case CMX_AF_CMD_GET_POLY_MODE:
        cmx_af_get_multi_poly_mode_processing(
          cmd_ptr->audfmt_get_poly_mode.mode,
          cmd_ptr->audfmt_get_poly_mode.cb_func,
          cmd_ptr->audfmt_get_poly_mode.client_data
        );
        break;
      case CMX_AF_CMD_POLY_MODE_CB:
        cmx_af_multi_poly_mode_cb_processing(
          cmd_ptr->audfmt_poly_mode_cb.pm_status,
          cmd_ptr->audfmt_poly_mode_cb.handle
        );
        break;
      case CMX_AF_CMD_GET_POLY_MODE_CB:
        cmx_af_multi_get_poly_mode_cb_processing(
          cmd_ptr->audfmt_get_pm_cb.pm_status,
          cmd_ptr->audfmt_get_pm_cb.mode,
          cmd_ptr->audfmt_get_pm_cb.handle
        );
        break;
#endif /* FEATURE_AUDFMT_HYBRID_MODE */

#ifdef FEATURE_AUDIO_PCR_SUPPORT
      case CMX_AF_CMD_ADD_SAMPLE:
        cmx_af_adjust_sample_processing(TRUE,
                                        cmd_ptr->base.cb_func,
                                        cmd_ptr->base.client_data);
        break;

      case CMX_AF_CMD_DROP_SAMPLE:
        cmx_af_adjust_sample_processing(FALSE,
                                        cmd_ptr->base.cb_func,
                                        cmd_ptr->base.client_data);
        break;
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
      case CMX_AF_CMD_INFO_UPDATE:
        cmx_af_codec_info_update_processing(&cmd_ptr->codec_update.codec_info,
                                            cmd_ptr->codec_update.handle);
        break;

      case CMX_AF_CMD_CHANNEL_DUAL_MONO:
        cmx_af_dual_mono_mode_processing(cmd_ptr->dual_mono_mode.mode,
                                         cmd_ptr->dual_mono_mode.cb_func,
                                         cmd_ptr->dual_mono_mode.client_data);
        break;

      case CMX_AF_CMD_SBR_MODE:
        cmx_af_sbr_mode_processing(cmd_ptr->sbr_mode.mode,
                                   cmd_ptr->sbr_mode.cb_func,
                                   cmd_ptr->sbr_mode.client_data);
        break;
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

      /* Illegal command, does not return.
      */
      case CMX_CMD_INVALID:
      default:
        ERR_FATAL ("Illegal vs command %x", cmd_ptr->hdr.cmd, 0, 0);
    }
    /* Return VS packet to free q
    */
    q_put (&cmx_free_q, &cmd_ptr->hdr.link);

    if((cmx_vs_in_high_pri) && (q_cnt(&cmx_cmd_q_hi) == 0)) {
      /* Return the priority of the VS task to normal */
      cmx_vs_in_high_pri = FALSE;
      (void) rex_set_pri(VS_PRI);
    }

  } /* end of while "commands to process" */
} /* end cmx_cmd_parser */

/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION cmx_free_buffer_struct

DESCRIPTION
  This function returns a local access handle to be used, if there
  is one available.

DEPENDENCIES
  cmx_handle

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_free_buffer_struct (
  cmx_audfmt_buf_struct *buf
) {
  int counter  = 0;

  while(counter < CMX_MAX_NUM_SEQUENCES) {
    if(&cmx_af_buffers[counter] == buf) {
      buf->buf_type         = CMX_AUDFMT_BUF_INVALID;
      buf->in_use_bit_mask  = 0;
      buf->in_use_bit_mask2 = 0;
      MSG_MED("cmx_free_buffer_struct(): Freed cmx_af_buffers[%d]",
              counter, 0, 0);
      break;
    }
    counter++;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_get_buffer_struct

DESCRIPTION
  This function returns a local access handle to be used, if there
  is one available.

DEPENDENCIES
  cmx_handle

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
cmx_audfmt_buf_struct *cmx_get_buffer_struct ()
{
  cmx_audfmt_buf_struct *ret     = NULL;
  int                   counter  = 0;

  while(counter < CMX_MAX_NUM_SEQUENCES) {
    if(cmx_af_buffers[counter].buf_type == CMX_AUDFMT_BUF_INVALID) {
      ret           = &cmx_af_buffers[counter];
      ret->buf_type = CMX_AUDFMT_BUF_32;
      break;
    }
    counter ++;
  }

  if (ret == NULL) {
    MSG_ERROR("cmx_get_buffer_struct(): Ran out of cmx_af_buffers",0,0,0);
  }
  else {
    MSG_MED("cmx_get_buffer_struct(): cmx_af_buffers[%d] allocated",
            counter, 0, 0);
  }
  return(ret);
}
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_free_af_handle

DESCRIPTION
  This function frees the indicated local handle.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void cmx_free_af_handle (
  cmx_af_handle_type *handle
) {
  if(handle != NULL) {
    handle->cmd    = CMX_CMD_INVALID;
#ifdef FEATURE_AUDIO_FORMAT
    if(handle->buffer != NULL) {
      cmx_free_buffer_struct(handle->buffer);
      handle->buffer = NULL;
    }
#endif /* FEATURE_AUDIO_FORMAT */
    MSG_MED("cmx_free_af_handle(): 0x%x freed", handle, 0, 0);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_get_af_handle

DESCRIPTION
  This function returns a local handle to be used, if there is one available.

DEPENDENCIES
  cmx_handle

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
cmx_af_handle_type *cmx_get_af_handle ()
{
  cmx_af_handle_type  *ret    = NULL;
  int                 counter = 0;

  while(counter < CMX_NUM_AF_HANDLES) {
    if(cmx_audfmt_handle[counter].cmd == CMX_CMD_INVALID) {
      ret                          = &cmx_audfmt_handle[counter];
      ret->status_mask             = 0;
#ifdef FEATURE_WEBAUDIO
      ret->content_mask            = 0;
      ret->text_buffer.buffer      = NULL;
      ret->text_buffer.pending_cmd = NULL;
      ret->pict_buffer.buffer      = NULL;
      ret->pict_buffer.pending_cmd = NULL;
#endif /* FEATURE_WEBAUDIO */
#ifdef FEATURE_SAF
      ret->saf_buffer.buffer       = NULL;
      ret->saf_buffer.pending_cmd  = NULL;
      ret->saf_obj_buf             = NULL;
      ret->saf_obj_buf_size        = 0;
#endif /* FEATURE_SAF */
#ifdef FEATURE_SVG
      ret->svg_buffer.buffer       = NULL;
      ret->svg_buffer.pending_cmd  = NULL;
#endif /* FEATURE_SVG */
#if defined(FEATURE_PCM_REC) || defined(FEATURE_MM_REC) || defined(FEATURE_QCP)
#ifdef FEATURE_EFS
      ret->rec_tx_handle           = NULL;
      ret->rec_rx_handle           = NULL;
#endif /* FEATURE_EFS */
#endif /* FEATURE_PCM_REC || FEATURE_MM_REC || FEATURE_QCP */
      break;
    }
    counter ++;
  }
  if (ret == NULL) {
    MSG_ERROR("cmx_get_af_handle: Ran out free af_handles",0,0,0);
  }
  else {
    MSG_MED("cmx_get_af_handle(): 0x%x allocated", ret, 0, 0);
  }
  return(ret);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_free_handle

DESCRIPTION
  This function frees the indicated local handle.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void cmx_free_handle (
  cmx_handle_type *handle
) {
  if(handle != NULL) {
    handle->source_type = CMX_SOURCE_INVALID;
    MSG_MED("cmx_free_handle(): 0x%x freed", handle, 0, 0);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_get_handle

DESCRIPTION
  This function returns a local handle to be used, if there is one available.

DEPENDENCIES
  cmx_handle

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
cmx_handle_type *cmx_get_handle ()
{
  cmx_handle_type  *ret    = NULL;
  int              counter = 0;

  while(counter < CMX_NUM_HANDLES) {
    if(cmx_handle[counter].source_type == CMX_SOURCE_INVALID) {
      ret = &cmx_handle[counter];
      break;
    }
    counter ++;
  }
  if (ret == NULL) {
    MSG_ERROR("cmx_get_handle(): Ran out free handles",0,0,0);
  }
  else {
    MSG_MED("cmx_get_handle(): 0x%x allocated", ret, 0, 0);
  }
  return(ret);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_setup_handle

DESCRIPTION
  This function sets up the handle information for a CMX file.

DEPENDENCIES
  cmx_handle_index

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_audfmt_handle
  cmx_handle_index

===========================================================================*/
void cmx_audfmt_setup_handle (
  cmx_handle_type     *handle,
  cmx_af_handle_type  **phandle_ptr,
  cmx_cmd_type        cmd
) {
  cmx_af_handle_type    *handle_ptr = NULL;
#ifdef FEATURE_EFS
  fs_rsp_msg_type       rsp;
  fs_access_option_type open_type = FS_OA_READONLY;
#endif

  if(handle->source_type == CMX_SOURCE_MEM) {
    handle_ptr                             = cmx_get_af_handle();
    if(handle_ptr != NULL) {
      handle_ptr->source_info.source       = CMX_SOURCE_MEM;
      handle_ptr->source_info.handle.buf   = handle->mem.buf;
      handle_ptr->source_info.cmx_len      = handle->mem.len;
      *phandle_ptr                         = handle_ptr;
    }
  } else if(handle->source_type == CMX_SOURCE_CLIENT) {
    handle_ptr                              = cmx_get_af_handle();
    if(handle_ptr != NULL) {
      handle_ptr->source_info.source          = CMX_SOURCE_CLIENT;
      handle_ptr->source_info.client_cfg      = handle->client.data_config;
      handle_ptr->source_info.handle.data_fn  = handle->client.data_req_ptr;
      handle_ptr->source_info.cmx_len         = handle->client.data_len;
      *phandle_ptr                            = handle_ptr;
    }
  }
#ifdef FEATURE_EFS
  else if(handle->source_type == CMX_SOURCE_EFS) {
#ifdef FEATURE_QCP
    if((cmd == CMX_QCP_SET_CNFG) || (cmd == CMX_QCP_SET_LABL) ||
       (cmd == CMX_QCP_SET_TEXT)) {
      open_type = FS_OA_READWRITE;
    }
#endif /* FEATURE_QCP */
    /* Make sure that we don't try to play a directory! */
    fs_nametest(handle->efs.filename, FS_TEST_DIR, NULL, &rsp);
    if (rsp.nametest.status == FS_OKAY_S && rsp.nametest.name_found == FALSE) {
      fs_open(handle->efs.filename, open_type, NULL, NULL, &rsp);
      if(rsp.open.status == FS_OKAY_S) {
        handle_ptr = cmx_get_af_handle();

        if(handle_ptr != NULL) {
          handle_ptr->source_info.source       = CMX_SOURCE_EFS;
          handle_ptr->source_info.handle.file  = rsp.open.handle;

          /* Load and store the file size */
          fs_file_size(handle->efs.filename, NULL, &rsp);
          handle_ptr->source_info.cmx_len      = rsp.file_size.size;

          *phandle_ptr = handle_ptr;
        } else {
          fs_close(rsp.open.handle, NULL, &rsp);
        }
      }
    }
  }
#endif

  if(handle_ptr != NULL) {
    handle_ptr->gen_handle = NULL;
  }

  cmx_free_handle(handle);
}

#if defined(FEATURE_EFS) && \
  (defined(FEATURE_PCM_REC) || defined(FEATURE_MM_REC) || defined(FEATURE_QCP))
/*===========================================================================

FUNCTION cmx_fs_space_available_using_handle

DESCRIPTION
  Returns the amount of available space in the file system, given the file
  handle. This is the minimum of both the tx and rx handles if both are active.

DEPENDENCIES
  None

RETURN VALUE
  Available space in bytes.

SIDE EFFECTS
  None

===========================================================================*/
uint32 cmx_fs_space_available_using_handle (
  cmx_af_handle_type  *handle_ptr
) {
  fs_rsp_msg_type statfs_rsp_msg;
  statfs_info     info;
  cmx_handle_type *handle;
  uint32          space_left = 0;
  boolean         tx_handle_in_use = FALSE;

  if ((handle_ptr != NULL) &&
      (handle_ptr->source_info.source == CMX_SOURCE_EFS)) {

    if (handle_ptr->rec_tx_handle != NULL) {
      handle = handle_ptr->rec_tx_handle;
      fs_statfs(handle->efs.filename, &info, NULL, &statfs_rsp_msg);
      if(statfs_rsp_msg.statfs.status == FS_OKAY_S) {
        space_left = info.f_bsize * info.f_bavail;
        tx_handle_in_use = TRUE;
      }
    }
    if (handle_ptr->rec_rx_handle != NULL) {
      handle = handle_ptr->rec_rx_handle;
      fs_statfs(handle->efs.filename, &info, NULL, &statfs_rsp_msg);
      if(statfs_rsp_msg.statfs.status == FS_OKAY_S) {
        if (tx_handle_in_use) {
          space_left = MIN((info.f_bsize * info.f_bavail), space_left);
        }
        else {
          space_left = info.f_bsize * info.f_bavail;
        }
      }
    }
  }
  return space_left;
}

/*===========================================================================

FUNCTION cmx_fs_space_available_using_name

DESCRIPTION
  Returns the amount of available space in the file system, given the file
  name.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 cmx_fs_space_available_using_name (
  cmx_handle_type  *handle
) {
  fs_rsp_msg_type statfs_rsp_msg;
  statfs_info     info;

  if ((handle != NULL) && (handle->source_type == CMX_SOURCE_EFS)) {
    fs_statfs(handle->efs.filename, &info, NULL, &statfs_rsp_msg);
    if(statfs_rsp_msg.statfs.status == FS_OKAY_S) {
      return(info.f_bsize * info.f_bavail);
    }
  }
  return 0;
}
#endif

/* <EJECT> */
#ifdef FEATURE_CMX_IMAGE
/*===========================================================================

FUNCTION cmx_image_check_cont_cb

DESCRIPTION
  This function checks if image continue callbacks need to be executed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_image_check_cont_cb (
  cmx_image_enum_type image_type
) {
  cmx_packet_type           *packet;
  cmx_image_command_cb_type cb_func = {NULL};
  void                      *client_data = NULL;
  boolean                   do_cb = FALSE;

  switch (image_type) {
#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_PNG_DECODER
    case CMX_IMAGE_TYPE_PNG:
      if(cmx_png_cont_cb_func_ptr != NULL) {
        cb_func.png_cb           = cmx_png_cont_cb_func_ptr;
        client_data              = cmx_png_cont_cb_data;
        cmx_png_cont_cb_func_ptr = NULL;
        do_cb                    = TRUE;
      }
      break;
#endif /* FEATURE_PNG_DECODER */

#ifdef FEATURE_JPEG_DECODER
    case CMX_IMAGE_TYPE_JPEG:
      if(cmx_jpeg_cont_cb_func_ptr != NULL) {
        cb_func.jpeg_cb           = cmx_jpeg_cont_cb_func_ptr;
        client_data               = cmx_jpeg_cont_cb_data;
        cmx_jpeg_cont_cb_func_ptr = NULL;
        do_cb                     = TRUE;
      }
      break;
#endif /* FEATURE_JPEG_DECODER */
#endif /* FEATURE_WEBAUDIO */

#ifdef FEATURE_SAF
    case CMX_IMAGE_TYPE_SAF:
      if(cmx_saf_cont_cb_func_ptr != NULL) {
        cb_func.saf_cb           = cmx_saf_cont_cb_func_ptr;
        client_data              = cmx_saf_cont_cb_data;
        cmx_saf_cont_cb_func_ptr = NULL;
        do_cb                    = TRUE;
      }
      break;
#endif /* FEATURE_SAF */

#ifdef FEATURE_SVG
  case CMX_IMAGE_TYPE_SVG:
     /* cb func is already stored in cb_func */
     do_cb                       = TRUE;
     break;
#endif /* FEATURE_SVG */

    default:
      MSG_ERROR("Invalid image type: %d", image_type, 0, 0);
      return;
  }

  if(do_cb) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      packet->hdr.cmd              = CMX_IMAGE_CMD_DO_CB;
      packet->image_cb.image_type  = image_type;
      packet->image_cb.cb_func     = cb_func;
      packet->image_cb.client_data = client_data;
      cmx_cmd (packet);
    }
  }
}
#endif /* FEATURE_CMX_IMAGE */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SAF) || defined(FEATURE_SVG)
/*===========================================================================

FUNCTION cmx_client_buf_free

DESCRIPTION
  This function de-allocates the indicated buffer properly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_client_buf_free (
  cmx_af_handle_type  *handle_ptr,
  cmx_client_buf_type *buf_handle
) {
#ifdef FEATURE_WEBAUDIO
  cmx_af_server_data_type  s_data;
#endif /* FEATURE_WEBAUDIO */
  cmx_packet_type          *packet;

  if((handle_ptr != NULL) && (buf_handle != NULL)) {
    if(buf_handle->buffer != NULL) {
      if(!(handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF)) {
        CMX_FREE(buf_handle->buffer);
      } else {
#ifdef FEATURE_WEBAUDIO
        if(handle_ptr->cmd == CMX_AF_CMD_PLAY) {
          if(handle_ptr->cb_func.play != NULL) {
            s_data.client_buf_free = buf_handle->buffer;
            handle_ptr->cb_func.play(CMX_CLIENT_BUF_FREE,
                                     handle_ptr->client_data, &s_data);
          }
        }
#endif /* FEATURE_WEBAUDIO */
#ifdef FEATURE_SAF
        if((handle_ptr->cmd == CMX_SAF_CMD_DECODE) ||
           (handle_ptr->cmd == CMX_SAF_CMD_INFO)) {
          if(handle_ptr->cb_func.image != NULL) {
            handle_ptr->cb_func.image(CMX_CLIENT_BUF_FREE,
                                      handle_ptr->client_data,
                                      buf_handle->buffer,
                                      sizeof(buf_handle->buffer));
          }
        }
#endif /* FEATURE_SAF */
      }
    }

    if(buf_handle->pending_cmd != NULL) {
      packet = (cmx_packet_type *) buf_handle->pending_cmd;
      q_put (&cmx_free_q, &packet->hdr.link);
      buf_handle->pending_cmd = NULL;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_client_buf_req_cb_processing

DESCRIPTION
  This function processes buffer request callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_client_buf_req_cb_processing (
  uint8               *buffer,
  cmx_client_buf_type *buf_handle
) {
  cmx_af_handle_type *handle_ptr;
  cmx_packet_type    *packet;

  if((buf_handle != NULL) && (buf_handle->handle != NULL)) {
    handle_ptr = (cmx_af_handle_type *) buf_handle->handle;
    packet     = (cmx_packet_type *) buf_handle->pending_cmd;
    if(handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF) {
      buf_handle->buffer = buffer;
      if(packet != NULL) {
        if((buffer == NULL)
#ifdef FEATURE_SAF
           && (handle_ptr->cmd != CMX_SAF_CMD_DECODE)
#endif /* FEATURE_SAF */
          ) {
          q_put (&cmx_free_q, &packet->hdr.link);
        } else {
#ifdef FEATURE_WEBAUDIO
          if((buffer != NULL) && (packet->hdr.cmd == CMX_AF_CMD_TEXT_CTL)) {
            if((packet->audfmt_text_ctl.buffer != NULL) &&
               (packet->audfmt_text_ctl.buf_len != 0)) {
              if(packet->audfmt_text_ctl.buf_len > CMX_MAX_TEXT_STRING) {
                packet->audfmt_text_ctl.buf_len = CMX_MAX_TEXT_STRING;
              }
              memcpy(buffer, packet->audfmt_text_ctl.buffer,
                     packet->audfmt_text_ctl.buf_len);
            }
            packet->audfmt_text_ctl.buffer = buffer;
          }
#endif /* FEATURE_WEBAUDIO */
          cmx_cmd(packet);
        }
      }
    } else if(packet != NULL) {
      q_put (&cmx_free_q, &packet->hdr.link);
    }
    buf_handle->pending_cmd = NULL;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_client_buf_req_cb

DESCRIPTION
  This function is the callback for buffer requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_client_buf_req_cb (
  uint8  *buffer,        /* Pointer to buffer returned             */
  void   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();
  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_CMD_CLIENT_BUF_REQ_CB;
    packet->client_buf_req_cb.buf_handle = (cmx_client_buf_type *) client_data;
    packet->client_buf_req_cb.buffer     = buffer;
    cmx_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_client_buf_req_processing

DESCRIPTION
  This function will issue a buffer request command to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_client_buf_req_processing (
  cmx_af_handle_type  *handle,
  cmx_client_buf_type *buf_handle,
  uint32              num_bytes
) {
#ifdef FEATURE_WEBAUDIO
  cmx_af_server_data_type server_data;
#endif /* FEATURE_WEBAUDIO */
#ifdef FEATURE_SAF
  cmx_client_buf_req_type buf_req;
#endif /* FEATURE_SAF */

  if(handle != NULL) {
#ifdef FEATURE_SAF
    if((handle->cmd == CMX_SAF_CMD_DECODE) && (handle->cb_func.image != NULL)) {
      buf_req.buffer_size     = num_bytes;
      buf_req.client_data     = buf_handle;
      buf_req.cb_func         = (cmx_client_buf_req_cb_func_ptr_type)
                                                     cmx_client_buf_req_cb;
      buf_req.buffer_supplied = FALSE;

      handle->cb_func.image(CMX_CLIENT_BUF_REQ, handle->client_data, &buf_req,
                            sizeof(cmx_client_buf_req_type));

      if(!buf_req.buffer_supplied) {
        cmx_cmd((cmx_packet_type *) buf_handle->pending_cmd);
        buf_handle->pending_cmd = NULL;
      } else {
        handle->status_mask |= CMX_AF_STATUS_MASK_USE_CLIENT_BUF;
      }
    }
#endif /* FEATURE_SAF */
#ifdef FEATURE_WEBAUDIO
    if((handle->cmd == CMX_AF_CMD_PLAY) && (handle->cb_func.play != NULL)) {
      server_data.client_buf_req.buffer_size = num_bytes;
      server_data.client_buf_req.client_data = buf_handle;
      server_data.client_buf_req.cb_func     =
                 (cmx_client_buf_req_cb_func_ptr_type) cmx_client_buf_req_cb;
      server_data.client_buf_req.buffer_supplied = FALSE;

      handle->cb_func.play(CMX_CLIENT_BUF_REQ, handle->client_data,
                           &server_data);

      if(!server_data.client_buf_req.buffer_supplied) {
        cmx_cmd((cmx_packet_type *) buf_handle->pending_cmd);
        buf_handle->pending_cmd = NULL;
      } else {
        handle->status_mask |= CMX_AF_STATUS_MASK_USE_CLIENT_BUF;
      }
    }
#endif /* FEATURE_WEBAUDIO */
  }
}
#endif /* FEATURE_WEBAUDIO || FEATURE_SAF || FEATURE_SVG */

/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION cmx_af_get_buff

DESCRIPTION
  This function returns a free buffer.

DEPENDENCIES
  cmx_af_buf_info

RETURN VALUE
  A pointer to the free buffer is returned.

SIDE EFFECTS
  None

===========================================================================*/
byte *cmx_af_get_buff(cmx_audfmt_buf_struct *buffer)
{
  uint8    *ret_val = NULL;
  uint32   counter, index;
  uint32   bit_counter;
  TASKLOCK();
  if(buffer != NULL) {
    bit_counter = 1;
    if (buffer->buf_type < CMX_AUDFMT_BUF_INVALID) {

    index       = (cmx_af_buf_info[buffer->buf_type].num_buffers < 32) ?
                   cmx_af_buf_info[buffer->buf_type].num_buffers : 32;

    if(buffer->in_use_bit_mask != (unsigned) 0xFFFFFFFF) {
      for(counter = 0; counter < index; counter++) {
        if((buffer->in_use_bit_mask & bit_counter) != bit_counter) {
          buffer->in_use_bit_mask |= bit_counter;
          switch(buffer->buf_type) {
            case CMX_AUDFMT_BUF_32:
              ret_val = (uint8 *) &(buffer->buf.buf32[counter]);
              break;
            case CMX_AUDFMT_BUF_16:
              ret_val = (uint8 *) &(buffer->buf.buf16[counter]);
              break;
            case CMX_AUDFMT_BUF_08:
              ret_val = (uint8 *) &(buffer->buf.buf08[counter]);
              break;
            case CMX_AUDFMT_BUF_04:
              ret_val = (uint8 *) &(buffer->buf.buf04[counter]);
              break;
            case CMX_AUDFMT_BUF_02:
              ret_val = (uint8 *) &(buffer->buf.buf02[counter]);
              break;
            case CMX_AUDFMT_BUF_01:
              ret_val = (uint8 *) &(buffer->buf.buf01[counter]);
              break;
            case CMX_AUDFMT_BUF_INVALID:
            default:
              ERR("Not valid state",0,0,0);
              break;
          }
          break;
        }
        bit_counter <<= 1;
      }
    } else if((buffer->buf_type == CMX_AUDFMT_BUF_01) &&
              (buffer->in_use_bit_mask2 != (unsigned) 0xFFFFFFFF)) {
      for(counter = index;
          counter < cmx_af_buf_info[buffer->buf_type].num_buffers;
          counter++)
      {
        if((buffer->in_use_bit_mask2 & bit_counter) != bit_counter) {
          buffer->in_use_bit_mask2 |= bit_counter;
          ret_val = (uint8 *) &(buffer->buf.buf01[counter]);
          break;
        }
        bit_counter <<= 1;
      }
    }
   } else {
       MSG_ERROR("cmx_af_get_buff() : Invalid buffer type",0,0,0);
	   ret_val = NULL;
   }
  }
  TASKFREE();
  if (ret_val == NULL) {
    MSG_ERROR("cmx_af_get_buff(): Ran out buffers",0,0,0);
  }
  return(ret_val);
} /* end cmx_af_get_buff */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_free_buff

DESCRIPTION
  This function frees the indicated buffer.

DEPENDENCIES
  cmx_af_buf_info

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean cmx_af_free_buff (
  cmx_audfmt_buf_struct *buffer,
  uint8                 *audfmt_data
) {
  uint32 index;
  boolean buff_free = FALSE;
  TASKLOCK();
  if(buffer != NULL) {
    if((audfmt_data >= (uint8 *) (&buffer->buf)) &&
       (audfmt_data <= ((uint8 *) (&buffer->buf) +
                        sizeof(cmx_audfmt_var_buf_type)))) {

      if(buffer->buf_type < CMX_AUDFMT_BUF_INVALID ) {
      index = ((unsigned) ((audfmt_data - (uint8 *)&(buffer->buf)))) /
                               cmx_af_buf_info[buffer->buf_type].num_bytes;

      if(index < 32) {
        buffer->in_use_bit_mask  &= (~((unsigned)1<<index));
      } else {
        index -= 32;
        buffer->in_use_bit_mask2 &= (~((unsigned)1<<index));
      }
      buff_free = TRUE;
      } else {
        MSG_ERROR(" Exceeding the array limit of the Buffer",0,0,0);
        buff_free = FALSE;
      }
    }
  }
  TASKFREE();
  return(buff_free);
} /* end cmx_af_free_buff */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

FUNCTION cmx_adec_get_buffer

DESCRIPTION
  This function returns a free adec buffer.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to the free buffer is returned.

SIDE EFFECTS
  None

===========================================================================*/
byte *cmx_adec_get_buffer(void)
{
  byte   *ret_val = NULL;
  uint32 counter;
  uint32 index = 1;

  for(counter = 0; counter < CMX_ADEC_MAX_NUM_BUFFERS; counter++) {
    if((cmx_adec_read_buffer.in_use & index) == 0) {
      ret_val = cmx_adec_read_buffer.buffer[counter];
      cmx_adec_read_buffer.in_use |= index;
      break;
    }
    index = index<<1;
  }
  if (ret_val == NULL) {
    MSG_ERROR("cmx_adec_get_buffer(): Ran out free buffers",0,0,0);
  }
  else {
    MSG_MED("cmx_adec_get_buffer(): 0x%x allocated", ret_val, 0, 0);
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_adec_free_buffer

DESCRIPTION
  This function frees the indicated buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean cmx_adec_free_buffer (
  uint8 *buffer
) {
  uint32 counter;

  for(counter = 0; counter < CMX_ADEC_MAX_NUM_BUFFERS; counter++) {
    if(cmx_adec_read_buffer.buffer[counter] == buffer) {
      cmx_adec_read_buffer.in_use &= ~(1<<counter);
      MSG_MED("cmx_adec_free_buffer(): Freed 0x%x", buffer, 0, 0);
      return(TRUE);
    }
  }
  return(FALSE);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_adec_client_cb

DESCRIPTION
  This function is used as the callback for midi/WebAudio EFS read calls.

DEPENDENCIES
  cmx_af_data_cb_array

RETURN VALUE
  None

SIDE EFFECTS
  cmx_af_data_cb_array

===========================================================================*/
void cmx_adec_client_cb (
  cmx_status_type  status,
  uint8            *data_ptr,
  uint32           len,
  const void       *client_data
    /*lint -esym(715,client_data) Tell Lint we know client_data parameter
      is not used */
) {
  cmx_packet_type            *packet;
  cmx_af_data_cb_struct_type *adec_data;

  if((client_data != NULL) && (cmx_adec_data_cb.number != 0)) {
    adec_data = (cmx_af_data_cb_struct_type *) client_data;
    INTLOCK();
    cmx_adec_data_cb.index++;
    if(cmx_adec_data_cb.index == CMX_ADEC_MAX_CB_STRUCTS) {
      cmx_adec_data_cb.index = 0;
    }
    cmx_adec_data_cb.number--;
    INTFREE();
    if(adec_data->buffer != NULL) {
      packet = cmx_cmd_get_pkt ();

      if(packet != NULL) {
        packet->hdr.cmd                     = CMX_AF_CMD_READ_RSP;
        packet->audfmt_read_rsp.cb_func     = adec_data->snd_cb_func;
        packet->audfmt_read_rsp.client_data = adec_data->client_data;
        packet->audfmt_read_rsp.position    = adec_data->position;

        if((status == CMX_SUCCESS) && (len <= adec_data->buffer_size)) {
          memcpy(&adec_data->buffer[adec_data->offset], data_ptr, len);
          packet->audfmt_read_rsp.buff        = adec_data->buffer;
          packet->audfmt_read_rsp.length      = len;
        } else {
          packet->audfmt_read_rsp.buff   = NULL;
          packet->audfmt_read_rsp.length = 0;

          if(!cmx_adec_free_buffer(adec_data->buffer)) {
            CMX_FREE(adec_data->buffer);
          }
        }

        adec_data->snd_cb_func = NULL;

        cmx_cmd (packet);
        return;
      } else {
        MSG_ERROR("Out of CMX packets!",0,0,0);
      }
      if(!cmx_adec_free_buffer(adec_data->buffer)) {
        CMX_FREE(adec_data->buffer);
      }
    }
    adec_data->snd_cb_func = NULL;
  }
}
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#ifdef FEATURE_EFS
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION cmx_af_pic_read_processing

DESCRIPTION
  This function performs the appropriate action upon receiving a picture data
  read callback.

DEPENDENCIES
  cmx_pic_remain
  cmx_pic_position
  cmx_pic_handle
  cmx_pic_type
  cmx_picture_buffer

RETURN VALUE
  None

SIDE EFFECTS
  cmx_pic_remain
  cmx_pic_position

===========================================================================*/
void cmx_af_pic_read_processing()
{
  cmx_af_server_data_type  server_data;
  fs_handle_type           file;
  uint32                   num_bytes;
  fs_rsp_msg_type          rsp;
  cmx_packet_type          *packet;

  num_bytes = MIN(CMX_PIC_MAX_INCREMENT, cmx_pic_remain);
  file      = (cmx_pic_handle->source_info.handle.file);

  fs_seek_read(file, FS_SEEK_SET, (fs_file_position_type) cmx_pic_position,
               cmx_picture_buffer, num_bytes, NULL, &rsp);

  if(rsp.read.status == FS_OKAY_S) {
    if(num_bytes == cmx_pic_remain) {
      server_data.pic_data.pic_data = CMX_AF_PIC_DATA_DONE;
    } else {
      server_data.pic_data.pic_data = CMX_AF_PIC_DATA_APPEND;
    }
    server_data.pic_data.pic_type = cmx_pic_type;
    server_data.pic_data.num      = num_bytes;
    server_data.pic_data.buffer   = cmx_picture_buffer;

    cmx_pic_handle->cb_func.play(CMX_PICTURE_DATA, cmx_pic_handle->client_data,
                                 &server_data);

    if(num_bytes != cmx_pic_remain) {
      cmx_pic_remain   -= num_bytes;
      cmx_pic_position += num_bytes;

      packet = cmx_cmd_get_pkt ();
      if (packet != NULL) {
        packet->hdr.cmd = CMX_AF_CMD_PIC_READ;
        cmx_cmd (packet);
      }
    }
  }
}
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_EFS */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_client_read_var_cb

DESCRIPTION
  Temporary solution to resolve buffer issue in BREW PCM interface.
  The problem occure dure to BREW/UI is running at a higher priority task
  then CMX. so the client callback is not being process by the CMX task
  util the BREW/UI task was free up. This caused the PCM decoder may run
  out of buffer. Added this function to incresed the CMX priority for PCM read
  callback will enhanced the system performance.

  This function is used as the callback for CMX read var calls to a data server.

  This command is usually placed in the High Cmx Cmd Queue.  However, when
  this function is called with the data length zero, the Snd task immediate
  turns around and requests more data from the client.  If it was unconditionally
  placed in the high command queue it would be parsed ahead of all other commands,
  thus causes a tight loop, never allowing the commands in the regular queue,
  such as STOP, to be processed.

DEPENDENCIES
  cmx_af_data_cb_array

RETURN VALUE
  None

SIDE EFFECTS
  cmx_af_data_cb_array

===========================================================================*/
void cmx_af_client_read_var_cb (
  cmx_status_type  status,
  uint8            *data_ptr,
  uint32           len,
  const void       *client_data
) {
  cmx_packet_type              *packet;
  cmx_af_data_cb_struct_type   *data_cb_ptr;
  boolean                      free_buf = FALSE;

  ASSERT(client_data != NULL);

  if(cmx_af_data_cb_array.number != 0) {
    data_cb_ptr = (cmx_af_data_cb_struct_type *) client_data;

    if(data_cb_ptr->handle == NULL)
    {
      /* should not process requests with NULL handle from client*/
      MSG_ERROR("Data callback without request!",0,0,0);
      return;
    }

    INTLOCK();
    cmx_af_data_cb_array.index++;
    if (cmx_af_data_cb_array.index == MAX_NUM_DATA_READ) {
      cmx_af_data_cb_array.index = 0;
    }
    cmx_af_data_cb_array.number--;
    INTFREE();

    if((data_cb_ptr->handle != NULL) &&
       (data_cb_ptr->handle->buffer != NULL)) {
      if((!(data_cb_ptr->handle->status_mask &
                        CMX_AF_STATUS_MASK_DISABLE_PENDING)) &&
         (data_cb_ptr->handle->cmd != CMX_CMD_INVALID)) {
        packet = cmx_cmd_get_pkt ();

        if (packet != NULL) {
          if(status == CMX_SUCCESS) {
            if (len <= data_cb_ptr->buffer_size) {
              memcpy(&data_cb_ptr->buffer[data_cb_ptr->offset], data_ptr, len);
              packet->audfmt_read_rsp.buff = data_cb_ptr->buffer;
              MSG_MED("cmx_af_client_read_var_cb: len=%d, buf=0x%x", len,
                       &data_cb_ptr->buffer[data_cb_ptr->offset], 0);
            } else {
              MSG_ERROR("Client data length %d greater than limit %d",
                        len, data_cb_ptr->buffer_size, 0);

              free_buf = TRUE;
              packet->audfmt_read_rsp.buff = NULL;
            }
          } else {
            free_buf = TRUE;
            packet->audfmt_read_rsp.buff   = NULL;
          }

          packet->hdr.cmd                     = CMX_AF_CMD_READ_RSP;
          packet->audfmt_read_rsp.cb_func     = data_cb_ptr->snd_cb_func;
          packet->audfmt_read_rsp.client_data = data_cb_ptr->client_data;
          packet->audfmt_read_rsp.position    = data_cb_ptr->position;
          packet->audfmt_read_rsp.length      = len;

          data_cb_ptr->snd_cb_func = NULL;

		  if (len > 0) {
			cmx_cmd_hi (packet);
		  }
		  else {
			/* When data length 0, place this cmd in regular queue so */
			/* that other cmds get a chance to be processed.          */
			cmx_cmd (packet);
		  }
        } else {
          MSG_ERROR("Out of CMX packets!",0,0,0);
          free_buf = TRUE;
        }
      } else {
        free_buf = TRUE;
      }

      if(free_buf) {
        if(!cmx_af_free_buff(data_cb_ptr->handle->buffer, data_cb_ptr->buffer))
        {
          CMX_FREE(data_cb_ptr->buffer);
        }
      }
    }
    data_cb_ptr->snd_cb_func = NULL;
    data_cb_ptr->handle      = NULL;
  } else {
    ERR("Data callback without request!",0,0,0);
  }
} /* end cmx_af_client_read_var_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_client_read_cb

DESCRIPTION
  This function is used as the callback for CMX read calls to a data server.

DEPENDENCIES
  cmx_af_data_cb_array

RETURN VALUE
  None

SIDE EFFECTS
  cmx_af_data_cb_array

===========================================================================*/
void cmx_af_client_read_cb (
  cmx_status_type  status,
  uint8            *data_ptr,
  uint32           len,
  const void       *client_data
) {
  cmx_packet_type              *packet;
  cmx_af_data_cb_struct_type   *data_cb_ptr;
  boolean                      free_buf = FALSE;

  ASSERT(client_data != NULL);

  if(cmx_af_data_cb_array.number != 0) {
    data_cb_ptr = (cmx_af_data_cb_struct_type *) client_data;

    if(data_cb_ptr->handle == NULL)
    {
      /* should not process requests with NULL handle from client*/
      MSG_ERROR("Data callback without request!",0,0,0);
      return;
    }

    INTLOCK();
    cmx_af_data_cb_array.index++;
    if (cmx_af_data_cb_array.index == MAX_NUM_DATA_READ) {
      cmx_af_data_cb_array.index = 0;
    }
    cmx_af_data_cb_array.number--;
    INTFREE();

    if((data_cb_ptr->handle != NULL) &&
       (data_cb_ptr->handle->buffer != NULL)) {
      if((!(data_cb_ptr->handle->status_mask &
                        CMX_AF_STATUS_MASK_DISABLE_PENDING)) &&
         (data_cb_ptr->handle->cmd != CMX_CMD_INVALID)) {
        packet = cmx_cmd_get_pkt ();

        if (packet != NULL) {
          if(status == CMX_SUCCESS) {
            if (len <= data_cb_ptr->buffer_size) {
              memcpy(&data_cb_ptr->buffer[data_cb_ptr->offset], data_ptr, len);
              packet->audfmt_read_rsp.buff = data_cb_ptr->buffer;
              MSG_MED("cmx_af_client_read_cb: len=%d, buf=0x%x", len,
                       &data_cb_ptr->buffer[data_cb_ptr->offset], 0);
            } else {
              MSG_ERROR("Client data length %d greater than limit %d",
                        len, data_cb_ptr->buffer_size, 0);

              free_buf = TRUE;
              packet->audfmt_read_rsp.buff = NULL;
            }
          } else {
            free_buf = TRUE;
            packet->audfmt_read_rsp.buff   = NULL;
          }

          packet->hdr.cmd                     = CMX_AF_CMD_READ_RSP;
          packet->audfmt_read_rsp.cb_func     = data_cb_ptr->snd_cb_func;
          packet->audfmt_read_rsp.client_data = data_cb_ptr->client_data;
          packet->audfmt_read_rsp.position    = data_cb_ptr->position;
          packet->audfmt_read_rsp.length      = len;

          data_cb_ptr->snd_cb_func = NULL;

          cmx_cmd (packet);
        } else {
          MSG_ERROR("Out of CMX packets!",0,0,0);
          free_buf = TRUE;
        }
      } else {
        MSG_HIGH("cmx_af_client_read_cb: Dropping buffer = 0x%x, handle = 0x%x",
                 data_cb_ptr->buffer, data_cb_ptr->handle, 0);

        free_buf = TRUE;
      }

      if(free_buf) {
        if(!cmx_af_free_buff(data_cb_ptr->handle->buffer, data_cb_ptr->buffer))
        {
          CMX_FREE(data_cb_ptr->buffer);
        }
      }
    }
    data_cb_ptr->snd_cb_func = NULL;
    data_cb_ptr->handle      = NULL;
  } else {
    ERR("Data callback without request!",0,0,0);
  }
} /* end cmx_af_client_read_cb */

/* <EJECT> */
#ifdef FEATURE_QCP
#ifdef FEATURE_EFS
/*===========================================================================

FUNCTION cmx_qcp_rec_write_cb

DESCRIPTION
  This function is the callback for the EFS write command done for QCP
  recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_qcp_rec_write_cb (
  fs_rsp_msg_type *rsp
) {
  cmx_qcp_rec_write_cb_struct_type *write_cb;
  cmx_packet_type                  *packet;

  if(rsp->seek_write.status == FS_OKAY_S) {
    write_cb = &cmx_qcp_rec_write_cb_array.data_cb
                                       [cmx_qcp_rec_write_cb_array.index];
    cmx_af_free_buff (write_cb->handle->buffer, write_cb->buffer);

    INTLOCK();
    cmx_qcp_rec_write_cb_array.index++;
    if (cmx_qcp_rec_write_cb_array.index == MAX_NUM_DATA_WRITE) {
      cmx_qcp_rec_write_cb_array.index = 0;
    }
    cmx_qcp_rec_write_cb_array.number--;
    INTFREE();

    if(cmx_qcp_rec_get_buf_pending != NULL) {

      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd                 = CMX_QCP_CMD_REC_GET_BUF;
        packet->qcp_rec_get_buf.handle  = cmx_qcp_rec_buf_handle;
        packet->qcp_rec_get_buf.cb_func = cmx_qcp_rec_get_buf_pending;
        cmx_qcp_rec_get_buf_pending     = cmx_qcp_rec_get_buf_pending_res;
        cmx_qcp_rec_get_buf_pending_res = NULL;
        cmx_qcp_rec_buf_handle          = cmx_qcp_rec_buf_handle_res;
        cmx_qcp_rec_buf_handle_res      = NULL;
        cmx_cmd (packet);
      }
    }
  } else {
    MSG_ERROR("write failed: %d", rsp->seek_write.status,0,0);
  }
}
#endif /* FEATURE_EFS */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_client_write_cb

DESCRIPTION
  This function is used as the callback for CMX QCP write calls to a
  data server.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_af_client_write_cb (
  cmx_status_type  status,
  uint8            *data_ptr,
  uint32           len,
  const void       *client_data
) {
  cmx_packet_type                  *packet;
  cmx_qcp_rec_write_cb_struct_type *write_cb;

  write_cb = &cmx_qcp_rec_write_cb_array.data_cb
                                     [cmx_qcp_rec_write_cb_array.index];

  if(client_data == write_cb->buffer) {
    cmx_af_free_buff (write_cb->handle->buffer, write_cb->buffer);

    INTLOCK();
    cmx_qcp_rec_write_cb_array.index++;
    if (cmx_qcp_rec_write_cb_array.index == MAX_NUM_DATA_WRITE) {
      cmx_qcp_rec_write_cb_array.index = 0;
    }
    cmx_qcp_rec_write_cb_array.number--;
    INTFREE();
  }

  if(cmx_qcp_rec_get_buf_pending != NULL) {

    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      packet->hdr.cmd                 = CMX_QCP_CMD_REC_GET_BUF;
      packet->qcp_rec_get_buf.handle  = cmx_qcp_rec_buf_handle;
      packet->qcp_rec_get_buf.cb_func = cmx_qcp_rec_get_buf_pending;
      cmx_qcp_rec_get_buf_pending     = cmx_qcp_rec_get_buf_pending_res;
      cmx_qcp_rec_get_buf_pending_res = NULL;
      cmx_qcp_rec_buf_handle          = cmx_qcp_rec_buf_handle_res;
      cmx_qcp_rec_buf_handle_res      = NULL;
      cmx_cmd (packet);
    }
  }
}
#endif /* FEATURE_QCP */
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
#ifdef FEATURE_CMX_IMAGE
/*===========================================================================

FUNCTION cmx_image_client_read_cb

DESCRIPTION
  This function is used as the callback for PNG EFS read calls.

DEPENDENCIES
  cmx_image_data_cb_array
  cmx_png_cont_cb_func_ptr
  cmx_png_cont_cb_data
  cmx_af_data_cb_array

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_image_data_cb_array
  cmx_png_cont_cb_func_ptr

===========================================================================*/
void cmx_image_client_read_cb (
  cmx_status_type  status,
  uint8            *data_ptr,
  uint32           len,
  const void       *client_data
) {
  cmx_packet_type         *packet;
  cmx_image_read_cb_type  *image_cb;

  if(status != CMX_SUCCESS) {
    MSG_ERROR("Bad client data get, %d",status,0,0);
  }

  image_cb = (cmx_image_read_cb_type *) client_data;
  cmx_image_data_cb_array.index++;
  if (cmx_image_data_cb_array.index == MAX_NUM_DATA_IMAGE_READ) {
    cmx_image_data_cb_array.index = 0;
  }
  cmx_image_data_cb_array.number--;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    memcpy(image_cb->buffer, data_ptr, len);
    packet->hdr.cmd              = CMX_IMAGE_CMD_DO_CB;
    packet->image_cb.image_type  = image_cb->image_type;
    packet->image_cb.cb_func     = image_cb->cb_func;
    packet->image_cb.client_data = image_cb->client_data;

#ifdef FEATURE_PNG_DECODER
    image_cb->cb_func.png_cb = NULL;
#elif defined(FEATURE_SAF)
    image_cb->cb_func.saf_cb = NULL;
#elif defined(FEATURE_JPEG_DECODER)
    image_cb->cb_func.jpeg_cb = NULL;
#endif /* FEATURE_PNG_DECODER */

    cmx_cmd (packet);
  }

  cmx_image_check_cont_cb(image_cb->image_type);
} /* end cmx_image_client_read_cb */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION cmx_image_buf_req_cb

DESCRIPTION
  This function is the callback function for image buffer requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_image_buf_req_cb (
  uint8      *buffer,
  void       *client_data
) {
  cmx_image_buf_req_cb_type *buf_req;
  cmx_packet_type           *packet;

  buf_req = (cmx_image_buf_req_cb_type *) client_data;

  *(buf_req->buf_handle) = buffer;

  packet = cmx_cmd_get_pkt ();
  if (packet != NULL) {
    packet->hdr.cmd              = CMX_IMAGE_CMD_DO_CB;
    packet->image_cb.image_type  = buf_req->image_type;
    packet->image_cb.cb_func     = buf_req->cb_func;
    packet->image_cb.client_data = buf_req->client_data;

    cmx_cmd (packet);
  }
}
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_CMX_IMAGE */

#ifdef FEATURE_WEBAUDIO
/* <EJECT> */
#ifdef FEATURE_PNG_DECODER
/*===========================================================================

FUNCTION cmx_png_callback

DESCRIPTION
  This function is used as a callback for the PNG decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_png_callback (
  png_cmx_status_enum_type png_status,
  void                     *handle,
  void                     *server_data
) {
  cmx_af_handle_type    *handle_ptr;
  png_cmx_get_data_type *png_get_data;
  png_cmx_data_type     *png_data;
  png_cmx_spec_type     *png_spec;
  png_cmx_callback_type *png_cb;
  cmx_packet_type       *packet;
  cmx_png_spec_type     cmx_spec;
  cmx_status_type       status;
#ifdef FEATURE_EFS
  fs_rsp_msg_type       rsp;
#endif

#ifdef FEATURE_WEBAUDIO
  cmx_af_server_data_type  s_data;
#endif

  handle_ptr = (cmx_af_handle_type *) handle;
  switch(png_status) {
    case PNG_CMX_TEXT:
      if(!cmx_png_start_new) {
        png_data = (png_cmx_data_type *) server_data;
        if(handle_ptr->cmd == CMX_PNG_CMD_DECODE) {
          if(handle_ptr->cb_func.image != NULL) {
            handle_ptr->cb_func.image(CMX_PNG_TEXT, handle_ptr->client_data,
                                      png_data->buffer, png_data->num_bytes);
          }
        }
#ifdef FEATURE_WEBAUDIO
        else if((handle_ptr->cmd == CMX_AF_CMD_PLAY) &&
                (handle_ptr->cb_func.play != NULL)) {
          s_data.pic_data.pic_data = CMX_AF_PIC_DATA_TEXT;
          s_data.pic_data.pic_type = CMX_AF_PIC_PNG;
          s_data.pic_data.num      = png_data->num_bytes;
          s_data.pic_data.buffer   = png_data->buffer;
          handle_ptr->cb_func.play(CMX_PICTURE_DATA, handle_ptr->client_data,
                                   &s_data);
        }
#endif
      }
      break;

    case PNG_CMX_SPEC:
      if((handle_ptr->cmd == CMX_PNG_CMD_DECODE) ||
         (handle_ptr->cmd == CMX_PNG_CMD_SPEC)) {
        png_spec = (png_cmx_spec_type *) server_data;

        cmx_spec.width       = png_spec->width;
        cmx_spec.height      = png_spec->height;
        cmx_spec.bit_depth   = png_spec->bit_depth;
        cmx_spec.color_type  = png_spec->color_type;
        cmx_spec.compression = png_spec->compression;
        cmx_spec.filter      = png_spec->filter;
        cmx_spec.interlace   = png_spec->interlace;

        if(handle_ptr->cmd == CMX_PNG_CMD_SPEC) {
          status = CMX_SUCCESS;
        } else {
          status = CMX_PNG_SPEC;
        }

        if(handle_ptr->cb_func.image != NULL) {
          handle_ptr->cb_func.image(status, handle_ptr->client_data,
                                    &cmx_spec, sizeof(cmx_png_spec_type));
        }

        if(handle_ptr->cmd == CMX_PNG_CMD_SPEC) {
#ifdef FEATURE_EFS
          if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
            fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
          }
#endif
          cmx_free_af_handle(handle_ptr);
        }
      }
      cmx_png_start_new = FALSE;
      break;

    case PNG_CMX_GET_FILE_SIZE:
      if(server_data != NULL) {
#ifdef FEATURE_WEBAUDIO
        if(handle_ptr->cmd == CMX_AF_CMD_PLAY) {
          *(uint32 *) server_data = cmx_png_size;
        } else
#endif
        {
          *(uint32 *) server_data = handle_ptr->source_info.cmx_len;
        }
      }
      break;

    case PNG_CMX_DATA:
      if(!cmx_png_start_new) {
        png_data = (png_cmx_data_type *) server_data;
        if(handle_ptr->cmd == CMX_PNG_CMD_DECODE) {
          if(handle_ptr->cb_func.image != NULL) {
            handle_ptr->cb_func.image(CMX_CONTINUE, handle_ptr->client_data,
                                      png_data->buffer, png_data->num_bytes);
          }
        }
#ifdef FEATURE_WEBAUDIO
        else if((handle_ptr->cmd == CMX_AF_CMD_PLAY) &&
                (handle_ptr->cb_func.play != NULL)) {
          s_data.pic_data.pic_data = CMX_AF_PIC_DATA_APPEND;
          s_data.pic_data.pic_type = CMX_AF_PIC_PNG;
          s_data.pic_data.num      = png_data->num_bytes;
          s_data.pic_data.buffer   = png_data->buffer;
          handle_ptr->cb_func.play(CMX_PICTURE_DATA, handle_ptr->client_data,
                                   &s_data);
        }
#endif
      }
      break;

    case PNG_CMX_GET_DATA:
      png_get_data = (png_cmx_get_data_type *) server_data;

      packet = cmx_cmd_get_pkt ();
      if (packet != NULL) {
        packet->hdr.cmd                   = CMX_IMAGE_CMD_READ;
        packet->image_read.image_type     = CMX_IMAGE_TYPE_PNG;
        packet->image_read.handle         = handle_ptr;
        packet->image_read.buffer         = png_get_data->buffer;
        packet->image_read.num_bytes      = png_get_data->num_bytes;
        packet->image_read.position       = png_get_data->position;
        packet->image_read.cb_func.png_cb = png_get_data->callback_ptr;
        packet->image_read.client_data    = png_get_data->client_data;
        cmx_cmd (packet);
      }
      break;

    case PNG_CMX_CONTINUE:
      png_cb = (png_cmx_callback_type *) server_data;
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd                 = CMX_IMAGE_CMD_CONTINUE;
        packet->image_cb.image_type     = CMX_IMAGE_TYPE_PNG;
        packet->image_cb.cb_func.png_cb = png_cb->callback_ptr;
        packet->image_cb.client_data    = png_cb->client_data;
        cmx_cmd (packet);
      }

      break;

    case PNG_CMX_ABORT:
      if(handle_ptr->cmd == CMX_PNG_CMD_DECODE) {
#ifdef FEATURE_EFS
        if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
          fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
        }
#endif
        if(handle_ptr->cb_func.image != NULL) {
          handle_ptr->cb_func.image(CMX_ABORTED, handle_ptr->client_data,
                                    NULL, 0);
        }

        cmx_free_af_handle(handle_ptr);
      }
      break;

    case PNG_CMX_DONE:
      if(handle_ptr->cmd == CMX_PNG_CMD_DECODE) {
#ifdef FEATURE_EFS
        if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
          fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
        }
#endif
        if(!cmx_png_start_new) {
          if(handle_ptr->cb_func.image != NULL) {
            handle_ptr->cb_func.image(CMX_DONE, handle_ptr->client_data,
                                      NULL, 0);
          }
        }

        cmx_free_af_handle(handle_ptr);

      }
#ifdef FEATURE_WEBAUDIO
      else if((handle_ptr->cmd == CMX_AF_CMD_PLAY) &&
              (handle_ptr->cb_func.play != NULL)) {
        cmx_png_audfmt_current = FALSE;
        if(!cmx_png_start_new) {
          s_data.pic_data.pic_data = CMX_AF_PIC_DATA_DONE;
          s_data.pic_data.pic_type = CMX_AF_PIC_PNG;
          s_data.pic_data.num      = 0;
          s_data.pic_data.buffer   = NULL;
          handle_ptr->cb_func.play(CMX_PICTURE_DATA, handle_ptr->client_data,
                                   &s_data);
        }
#ifdef FEATURE_SAF
        if(cmx_saf_delay_handle != NULL) {
          packet = cmx_cmd_get_pkt ();
          if (packet != NULL) {
            packet->hdr.cmd                   = CMX_AF_CMD_ANIM;
            packet->audfmt_anim_data.type     = CMX_AF_ANIM_SAF;
            packet->audfmt_anim_data.handle   = cmx_saf_delay_handle;
            packet->audfmt_anim_data.buffer   = cmx_saf_buffer;
            packet->audfmt_anim_data.buf_len  = cmx_saf_buf_bytes;
            packet->audfmt_anim_data.position = cmx_saf_offset;
            packet->audfmt_anim_data.remain_bytes
                                           = cmx_saf_size - cmx_saf_buf_bytes;
            cmx_saf_delay_handle = NULL;
            cmx_cmd (packet);
          }
        }
#endif /* FEATURE_SAF */
      }
#endif
      break;

    case PNG_CMX_FAILURE:
#ifdef FEATURE_WEBAUDIO
      cmx_png_audfmt_current = FALSE;
#endif /* FEATURE_WEBAUDIO */
      if(handle_ptr->cmd == CMX_PNG_CMD_DECODE) {
#ifdef FEATURE_EFS
        if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
          fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
        }
#endif
        if(handle_ptr->cb_func.image != NULL) {
          handle_ptr->cb_func.image(CMX_FAILURE, handle_ptr->client_data,
                                    NULL, 0);
        }

        cmx_free_af_handle(handle_ptr);
      }
      break;

    default:
      break;
  }
} /* end cmx_png_callback */
#endif /* FEATURE_PNG_DECODER */

/* <EJECT> */
#ifdef FEATURE_JPEG_DECODER
/*===========================================================================

FUNCTION cmx_jpeg_callback

DESCRIPTION
  This function is used as a callback for the JPEG decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_jpeg_callback (
  jpeg_cmx_status_enum_type jpeg_status,
  void                     *handle,
  void                     *server_data
) {
  cmx_af_handle_type    *handle_ptr;
  jpeg_cmx_get_data_type *jpeg_get_data;
  jpeg_cmx_data_type     *jpeg_data;
  jpeg_cmx_buf_req_type  *jpeg_buf_req;
  jpeg_cmx_spec_type     *jpeg_spec;
  jpeg_cmx_callback_type *jpeg_cb;
  cmx_packet_type       *packet;
  cmx_jpeg_spec_type     cmx_spec;
  cmx_status_type       status;
#ifdef FEATURE_EFS
  fs_rsp_msg_type       rsp;
#endif

#ifdef FEATURE_WEBAUDIO
  cmx_af_server_data_type  s_data;
#endif

  handle_ptr = (cmx_af_handle_type *) handle;
  switch(jpeg_status) {

    case JPEG_CMX_SPEC:
      if((handle_ptr->cmd == CMX_JPEG_CMD_DECODE) ||
         (handle_ptr->cmd == CMX_JPEG_CMD_SPEC)) {
        jpeg_spec = (jpeg_cmx_spec_type *) server_data;

        cmx_spec.width          = jpeg_spec->width;
        cmx_spec.height         = jpeg_spec->height;
        cmx_spec.num_components = jpeg_spec->num_components;
        cmx_spec.color_space    = jpeg_spec->color_space;
        cmx_spec.sample_factor  = jpeg_spec->sample_factor;

        if(handle_ptr->cmd == CMX_JPEG_CMD_SPEC) {
          status = CMX_SUCCESS;
        } else {
          status = CMX_JPEGD_SPEC;
        }

        if(handle_ptr->cb_func.image != NULL) {
          handle_ptr->cb_func.image(status, handle_ptr->client_data,
                                    &cmx_spec, sizeof(cmx_jpeg_spec_type));
        }

        if(handle_ptr->cmd == CMX_JPEG_CMD_SPEC) {
#ifdef FEATURE_EFS
          if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
            fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
          }
#endif
          cmx_free_af_handle(handle_ptr);
        }
      }
      break;

    case JPEG_CMX_GET_FILE_SIZE:
      if(server_data != NULL) {
#ifdef FEATURE_WEBAUDIO
        if(handle_ptr->cmd == CMX_AF_CMD_PLAY) {
          *(uint32 *) server_data = cmx_jpeg_size;
        } else
#endif
        {
          *(uint32 *) server_data = handle_ptr->source_info.cmx_len;
        }
      }
      break;

    case JPEG_CMX_GET_BUF:
      jpeg_buf_req = (jpeg_cmx_buf_req_type *) server_data;
      if(handle_ptr->cmd == CMX_JPEG_CMD_DECODE) {
        if(handle_ptr->cb_func.image != NULL) {
          handle_ptr->cb_func.image(CMX_GET_BUFFER, handle_ptr->client_data,
                                    jpeg_buf_req->buf_handle,
                                    jpeg_buf_req->num_bytes);
        }
        if(jpeg_buf_req->callback_ptr != NULL) {
          jpeg_buf_req->callback_ptr(jpeg_buf_req->client_data);
        }
      }
#ifdef FEATURE_WEBAUDIO
      else if((handle_ptr->cmd == CMX_AF_CMD_PLAY) &&
              (handle_ptr->cb_func.play != NULL)) {
        packet = cmx_cmd_get_pkt ();
        if (packet != NULL) {
          packet->hdr.cmd                       = CMX_IMAGE_CMD_BUF_REQ;
          packet->image_buf_req.image_type      = CMX_IMAGE_TYPE_JPEG;
          packet->image_buf_req.handle          = handle_ptr;
          packet->image_buf_req.buf_handle      = jpeg_buf_req->buf_handle;
          packet->image_buf_req.num_bytes       = jpeg_buf_req->num_bytes;
          packet->image_buf_req.cb_func.jpeg_cb = jpeg_buf_req->callback_ptr;
          packet->image_buf_req.client_data     = jpeg_buf_req->client_data;
          cmx_cmd (packet);
        }
      }
#endif
      break;

    case JPEG_CMX_DATA:
      jpeg_data = (jpeg_cmx_data_type *) server_data;
      if(handle_ptr->cmd == CMX_JPEG_CMD_DECODE) {
        if(handle_ptr->cb_func.image != NULL) {
          handle_ptr->cb_func.image(CMX_CONTINUE, handle_ptr->client_data,
                                      jpeg_data->buffer, jpeg_data->num_bytes);
        }
      }
#ifdef FEATURE_WEBAUDIO
      else if((handle_ptr->cmd == CMX_AF_CMD_PLAY) &&
              (handle_ptr->cb_func.play != NULL)) {
        s_data.pic_data.pic_data = CMX_AF_PIC_DATA_APPEND;
        s_data.pic_data.pic_type = CMX_AF_PIC_JPEG;
        s_data.pic_data.num      = jpeg_data->num_bytes;
        s_data.pic_data.buffer   = jpeg_data->buffer;
        handle_ptr->cb_func.play(CMX_PICTURE_DATA, handle_ptr->client_data,
                                 &s_data);
      }
#endif
      break;

    case JPEG_CMX_GET_DATA:
      jpeg_get_data = (jpeg_cmx_get_data_type *) server_data;

      packet = cmx_cmd_get_pkt ();
      if (packet != NULL) {
        packet->hdr.cmd                    = CMX_IMAGE_CMD_READ;
        packet->image_read.image_type      = CMX_IMAGE_TYPE_JPEG;
        packet->image_read.handle          = handle_ptr;
        packet->image_read.buffer          = jpeg_get_data->buffer;
        packet->image_read.num_bytes       = jpeg_get_data->num_bytes;
        packet->image_read.position        = jpeg_get_data->position;
        packet->image_read.cb_func.jpeg_cb = jpeg_get_data->callback_ptr;
        packet->image_read.client_data     = jpeg_get_data->client_data;
        cmx_cmd (packet);
      }
      break;

    case JPEG_CMX_CONTINUE:
      jpeg_cb = (jpeg_cmx_callback_type *) server_data;
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd                  = CMX_IMAGE_CMD_CONTINUE;
        packet->image_cb.image_type      = CMX_IMAGE_TYPE_JPEG;
        packet->image_cb.cb_func.jpeg_cb = jpeg_cb->callback_ptr;
        packet->image_cb.client_data     = jpeg_cb->client_data;
        cmx_cmd (packet);
      }
      break;

    case JPEG_CMX_ABORT:
      if(handle_ptr->cmd == CMX_JPEG_CMD_DECODE) {
#ifdef FEATURE_EFS
        if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
          fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
        }
#endif
        if(handle_ptr->cb_func.image != NULL) {
          handle_ptr->cb_func.image(CMX_ABORTED, handle_ptr->client_data,
                                      NULL, 0);
        }

        cmx_free_af_handle(handle_ptr);
      }
      break;

    case JPEG_CMX_DONE:
      if(handle_ptr->cmd == CMX_JPEG_CMD_DECODE) {
#ifdef FEATURE_EFS
        if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
          fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
        }
#endif
        if(handle_ptr->cb_func.image != NULL) {
          handle_ptr->cb_func.image(CMX_DONE, handle_ptr->client_data,
                                      NULL, 0);
        }

        cmx_free_af_handle(handle_ptr);
      }
#ifdef FEATURE_WEBAUDIO
      else if((handle_ptr->cmd == CMX_AF_CMD_PLAY) &&
              (handle_ptr->cb_func.play != NULL)) {
        s_data.pic_data.pic_data = CMX_AF_PIC_DATA_DONE;
        s_data.pic_data.pic_type = CMX_AF_PIC_JPEG;
        s_data.pic_data.num      = 0;
        s_data.pic_data.buffer   = NULL;
        handle_ptr->cb_func.play(CMX_PICTURE_DATA, handle_ptr->client_data,
                                 &s_data);
      }
#endif
      break;

    case JPEG_CMX_FAILURE:
      if(handle_ptr->cmd == CMX_JPEG_CMD_DECODE) {
#ifdef FEATURE_EFS
        if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
          fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
        }
#endif
        if(handle_ptr->cb_func.image != NULL) {
          handle_ptr->cb_func.image(CMX_FAILURE, handle_ptr->client_data,
                                      NULL, 0);
        }

        cmx_free_af_handle(handle_ptr);
      }
      break;

    default:
      break;
  }
} /* end cmx_jpeg_callback */
#endif /* FEATURE_JPEG_DECODER */
#endif /* FETAURE_WEBAUDIO */

/* <EJECT> */
#ifdef FEATURE_SAF
/*===========================================================================

FUNCTION cmx_saf_init

DESCRIPTION
  This function initializes parameters and buffers for SAF module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_saf_init (
  uint8  *object_buf,
  uint32 object_buf_size,
  uint8  *info_buf
) {
  saf_display_resource_info_type saf_info;

  saf_info.width          = CMX_SAF_FRAME_WIDTH;
  saf_info.height         = CMX_SAF_FRAME_HEIGHT;
  saf_info.bitdepth       = CMX_SAF_FRAME_BITDEPTH;
  saf_info.pOperationBuf1 = NULL;
  saf_info.pOperationBuf2 = NULL;

  /* Initialize SAF module */
  graph_saf_global_init(&saf_info, object_buf, object_buf_size, cmx_image_buf,
                        CMX_IMAGE_BUFFER_SIZE, info_buf);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_cleanup

DESCRIPTION
  This function cleans up a SAF decode session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_saf_cleanup (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type rsp;
#endif /* FEATURE_EFS */

  cmx_saf_handle_ptr         = NULL;
  cmx_saf_status             = CMX_SAF_STATUS_STOPPED;
  cmx_saf_frame_cb           = NULL;
  cmx_saf_pending_handle_ptr = NULL;
  cmx_saf_audfmt_current     = FALSE;
  (void) rex_clr_timer( &cmx_saf_frame_timer );

  if((handle_ptr != NULL) && (handle_ptr->cmd == CMX_SAF_CMD_DECODE)) {
#ifdef FEATURE_EFS
    if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
      fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
    }
#endif
    if(handle_ptr->saf_obj_buf != NULL) {
      CMX_FREE(handle_ptr->saf_obj_buf);
    }
    cmx_client_buf_free(handle_ptr, &handle_ptr->saf_buffer);
    if(handle_ptr->cb_func.image != NULL) {
      handle_ptr->cb_func.image(status, handle_ptr->client_data,
                                NULL, 0);
    }

    cmx_free_af_handle(handle_ptr);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_frame

DESCRIPTION
  This procedure gets the next SAF frame.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_saf_frame_timer

===========================================================================*/
void cmx_saf_frame()
{
  cmx_packet_type       *packet;

  if((cmx_saf_status == CMX_SAF_STATUS_PLAYING) &&
     (cmx_saf_handle_ptr != NULL) &&
     (cmx_saf_handle_ptr->saf_buffer.buffer != NULL)) {
    (void) rex_clr_timer( &cmx_saf_frame_timer );

    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      packet->hdr.cmd                   = CMX_SAF_CMD_DO_FRAME_CB;
      packet->saf_frame_id.frame_id     = cmx_saf_frame_number;
      packet->saf_frame_id.frame_buffer = cmx_saf_handle_ptr->saf_buffer.buffer;
      cmx_cmd (packet);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_callback

DESCRIPTION
  This function is used as a callback for the SAF decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_callback (
  saf_cmx_status_type saf_status,
  void                *handle,
  void                *server_data
) {
  cmx_af_handle_type            *handle_ptr;
  saf_cmx_get_data_type         *saf_get_data;
  saf_cmx_frame_done_type       *frame_done;
  saf_cmx_prepare_done_type     *prepare_done;
  saf_cmx_continue_type         *saf_continue;
  saf_cmx_global_info_type      *saf_info;

  cmx_packet_type       *packet;
  cmx_saf_info_type     cmx_saf_info;
  cmx_status_type       status;
  uint32                frame_buf_size;
#ifdef FEATURE_EFS
  fs_rsp_msg_type       rsp;
#endif /* FEATURE_EFS */

#ifdef FEATURE_WEBAUDIO
  cmx_af_server_data_type  s_data;
#endif /* FEATURE_WEBAUDIO */

  handle_ptr = (cmx_af_handle_type *) handle;
  switch(saf_status) {
    case SAF_CMX_INFO:
      saf_info = (saf_cmx_global_info_type *) server_data;

      if((handle_ptr->cmd == CMX_SAF_CMD_DECODE) ||
         (handle_ptr->cmd == CMX_SAF_CMD_INFO)) {
        if(handle_ptr->cb_func.image != NULL) {
          cmx_saf_info.width             = saf_info->width;
          cmx_saf_info.height            = saf_info->height;
          cmx_saf_info.bitdepth          = (uint32) saf_info->bitdepth;
          cmx_saf_info.glob_trans_count  = saf_info->global_transparency_count;
          cmx_saf_info.global_trans      = saf_info->global_transparency;
          cmx_saf_info.loopcount         = saf_info->loopcount;
          cmx_saf_info.num_frames        = saf_info->number_of_frames;
          cmx_saf_info.num_palette_entry = saf_info->number_palette_entry;
          cmx_saf_info.palette           = saf_info->palette;

          if(handle_ptr->cmd == CMX_SAF_CMD_INFO) {
            status = CMX_SUCCESS;
          } else {
            status = CMX_SAF_INFO;
          }

          handle_ptr->cb_func.image(status, handle_ptr->client_data,
                                    &cmx_saf_info, sizeof(cmx_saf_info_type));
        }

        if(handle_ptr->cmd == CMX_SAF_CMD_INFO) {
#ifdef FEATURE_EFS
          if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
            fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
          }
#endif
          cmx_client_buf_free(handle_ptr, &handle_ptr->saf_buffer);

          cmx_free_af_handle(handle_ptr);
          break;
        }
      }
#ifdef FEATURE_WEBAUDIO
      else if(handle_ptr->cmd == CMX_AF_CMD_PLAY) {
        if(handle_ptr->cb_func.play != NULL) {
          s_data.saf_info.width             = saf_info->width;
          s_data.saf_info.height            = saf_info->height;
          s_data.saf_info.bitdepth          = (uint32) saf_info->bitdepth;
          s_data.saf_info.glob_trans_count  = saf_info->global_transparency_count;
          s_data.saf_info.global_trans      = saf_info->global_transparency;
          s_data.saf_info.loopcount         = saf_info->loopcount;
          s_data.saf_info.num_frames        = saf_info->number_of_frames;
          s_data.saf_info.num_palette_entry = saf_info->number_palette_entry;
          s_data.saf_info.palette           = saf_info->palette;

          handle_ptr->cb_func.play(CMX_SAF_INFO, handle_ptr->client_data,
                                   &s_data);
        }
      }
#endif /* FEATURE_WEBAUDIO */

      frame_buf_size = saf_info->width * saf_info->height;
      if(handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF) {
        if(handle_ptr->saf_buffer.buffer != NULL) {
          if(frame_buf_size > CMX_SAF_BUFFER_SIZE) {
            cmx_client_buf_free(handle_ptr, &handle_ptr->saf_buffer);
            handle_ptr->saf_buffer.buffer = NULL;

            if(handle_ptr->cmd == CMX_SAF_CMD_DECODE) {
              packet = cmx_cmd_get_pkt();
              if(packet != NULL) {
                packet->hdr.cmd                    = CMX_SAF_CMD_FRAME_CONT;
                packet->saf_cont.handle            = handle_ptr;
                handle_ptr->saf_buffer.pending_cmd = packet;
              } else {
                cmx_saf_cleanup(handle_ptr, CMX_FAILURE);
                break;
              }
            }

            packet = cmx_cmd_get_pkt ();
            if(packet != NULL) {
              handle_ptr->saf_buffer.handle     = handle_ptr;
              packet->hdr.cmd                   = CMX_CMD_CLIENT_BUF_REQ;
              packet->client_buf_req.handle     = handle_ptr;
              packet->client_buf_req.buf_handle = &handle_ptr->saf_buffer;
              packet->client_buf_req.num_bytes  = frame_buf_size;
              cmx_cmd(packet);
            } else if(handle_ptr->cmd == CMX_SAF_CMD_DECODE) {
              cmx_saf_cleanup(handle_ptr, CMX_FAILURE);
            }
          }
        }
      } else {
        handle_ptr->saf_buffer.buffer = (uint8 *) CMX_MALLOC(frame_buf_size);
        if((handle_ptr->saf_buffer.buffer == NULL) &&
           (handle_ptr->cmd == CMX_SAF_CMD_DECODE)) {
          MSG_ERROR("Fail to allocate memory", 0,0,0);
          cmx_saf_cleanup(handle_ptr, CMX_FAILURE);
        }
      }
      break;

    case SAF_CMX_GET_FILE_SIZE:
      if(server_data != NULL) {
#ifdef FEATURE_WEBAUDIO
        if(handle_ptr->cmd == CMX_AF_CMD_PLAY) {
          *(uint32 *) server_data = cmx_saf_size;
        } else
#endif /* FEATURE_WEBAUDIO */
        {
          *(uint32 *) server_data = handle_ptr->source_info.cmx_len;
        }
      }
      break;

    case SAF_CMX_PREPARE_DONE:
      prepare_done               = (saf_cmx_prepare_done_type *) server_data;
      cmx_saf_frame_cb           = prepare_done->callback_ptr;
      cmx_saf_frame_client_data  = prepare_done->client_data;
      if(handle_ptr->cmd == CMX_SAF_CMD_DECODE) {
        if(cmx_saf_status == CMX_SAF_STATUS_PAUSED) {
          cmx_saf_pending_status = CMX_SAF_STATUS_PLAYING;
        } else {
          cmx_saf_status         = CMX_SAF_STATUS_PLAYING;
          cmx_saf_frame_number   = 0;
          if(cmx_saf_ctl == CMX_SAF_CTL_FREE_RUN) {
            cmx_saf_frame();
          }
        }
      }
#ifdef FEATURE_WEBAUDIO
      else if(handle_ptr->cmd == CMX_AF_CMD_PLAY) {
        cmx_saf_status = CMX_SAF_STATUS_PLAYING;
#ifdef FEATURE_PNG_DECODER
        if(cmx_png_delay_handle != NULL) {
          packet = cmx_cmd_get_pkt ();
          if (packet != NULL) {
            packet->hdr.cmd                      = CMX_AF_CMD_PIC_DATA;
            packet->audfmt_pic_data.handle       = cmx_png_delay_handle;
            packet->audfmt_pic_data.pic_type     = CMX_AF_PIC_PNG;
            packet->audfmt_pic_data.buffer       = cmx_png_buffer;
            packet->audfmt_pic_data.buf_len      = cmx_png_buf_bytes;
            packet->audfmt_pic_data.position     = cmx_png_offset;
            packet->audfmt_pic_data.remain_bytes =
                                            cmx_png_size - cmx_png_buf_bytes;

            cmx_png_delay_handle = NULL;
            cmx_cmd (packet);
          }
        }
#endif /* FEATURE_PNG_DECODER */
        if (cmx_saf_audfmt_frame_status == CMX_SAF_FRAME_PEND) {
          if (cmx_saf_frame_cb != NULL) {
            packet = cmx_cmd_get_pkt();
            if (packet != NULL) {
              packet->hdr.cmd                   = CMX_SAF_CMD_DO_FRAME_CB;
              packet->saf_frame_id.frame_id     = cmx_saf_audfmt_frame_id;
              packet->saf_frame_id.frame_buffer = handle_ptr->saf_buffer.buffer;

              cmx_saf_audfmt_frame_status = CMX_SAF_FRAME_PROC;
              cmx_saf_audfmt_frame_pos    = cmx_saf_audfmt_frame_pos_pending;
              cmx_cmd (packet);
            } else {
              cmx_saf_audfmt_frame_status = CMX_SAF_FRAME_NONE;
            }
          } else {
            cmx_saf_audfmt_frame_status = CMX_SAF_FRAME_NONE;
          }
        }
      }
#endif /* FEATURE_WEBAUDIO */
      break;

    case SAF_CMX_FRAME_DONE:
      if((handle_ptr->cmd == CMX_SAF_CMD_DECODE)
#ifdef FEATURE_WEBAUDIO
         || (handle_ptr->cmd == CMX_AF_CMD_PLAY)
#endif /* FEATURE_WEBAUDIO */
        ) {
        frame_done = (saf_cmx_frame_done_type *) server_data;
        packet     = cmx_cmd_get_pkt ();
        if (packet != NULL) {
          packet->hdr.cmd                   = CMX_SAF_CMD_FRAME_DONE;
          packet->saf_frame_done.handle     = handle_ptr;
          packet->saf_frame_done.buffer     = frame_done->framebuf;
          packet->saf_frame_done.num_bytes  = frame_done->size_frame;
          packet->saf_frame_done.delay      = frame_done->delay_to_nextframe;
          packet->saf_frame_done.last_frame =
                                        (boolean) frame_done->flag_last_frame;
          cmx_cmd (packet);
        }
      }
      break;

    case SAF_CMX_GET_DATA:
      saf_get_data = (saf_cmx_get_data_type *) server_data;

      packet = cmx_cmd_get_pkt ();
      if (packet != NULL) {
        packet->hdr.cmd                   = CMX_IMAGE_CMD_READ;
        packet->image_read.image_type     = CMX_IMAGE_TYPE_SAF;
        packet->image_read.handle         = handle_ptr;
        packet->image_read.buffer         = saf_get_data->buffer;
        packet->image_read.num_bytes      = saf_get_data->num_bytes;
        packet->image_read.position       = saf_get_data->position;
        packet->image_read.cb_func.saf_cb = saf_get_data->callback_ptr;
        packet->image_read.client_data    = saf_get_data->client_data;
        cmx_cmd (packet);
      }
      break;

    case SAF_CMX_CONTINUE:
      saf_continue = (saf_cmx_continue_type *) server_data;
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd                 = CMX_IMAGE_CMD_CONTINUE;
        packet->image_cb.image_type     = CMX_IMAGE_TYPE_SAF;
        packet->image_cb.cb_func.saf_cb = saf_continue->callback_ptr;
        packet->image_cb.client_data    = saf_continue->client_data;
        cmx_cmd (packet);
      }
      break;

    case SAF_CMX_ABORT:
      cmx_saf_cleanup(handle_ptr, CMX_ABORTED);
      break;

    case SAF_CMX_FAIL:
      cmx_saf_cleanup(handle_ptr, CMX_FAILURE);
      break;

    default:
      break;
  }
} /* end cmx_saf_callback */
#endif /* FEATURE_SAF */

/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION cmx_af_do_callback

DESCRIPTION
  This function performs the proper play callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_af_do_callback (
  cmx_status_type     status,
  cmx_af_handle_type  *handle,
  void                *data
) {
  if(handle->cmd == CMX_AF_CMD_PLAY) {
    if(handle->cb_func.play != NULL) {
      handle->cb_func.play(status, handle->client_data, (cmx_af_server_data_type*) data);
    }
  } else if(handle->cmd == CMX_AF_CMD_PLAY_CODEC) {
    if(handle->cb_func.play_codec != NULL) {
      handle->cb_func.play_codec(status, handle->client_data);
    }
  }else if (handle->cmd == CMX_AF_CMD_AVSYNC_PLAYBACK ){
      if (handle ->cb_func.av_sync_playback != NULL)
        handle->cb_func.av_sync_playback (status,handle->client_data,
                                         (cmx_af_server_data_type*) data);
  }else if(handle->cmd == CMX_AF_CMD_RINGER) {
    if(handle->cb_func.ringer != NULL) {
      handle->cb_func.ringer(status, handle->client_data);
    }
  } else if(handle->cmd == CMX_AF_CMD_GET_TIME) {
    if(handle->cb_func.time != NULL) {
      handle->cb_func.time(status, handle->client_data, 0);
    }
  }
} /* end cmx_af_do_callback */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_check_pending

DESCRIPTION
  This function checks if there are any pending play commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_check_pending ()
{
  uint32 counter;

  if(cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_RINGER) {
    cmx_af_cmd_pend.cmd_pend = CMX_CMD_INVALID;
    cmx_audfmt_ringer_processing(cmx_af_cmd_pend.para.ringer.handle,
                                 cmx_af_cmd_pend.para.ringer.repeat_timer,
                                 cmx_af_cmd_pend.para.ringer.start_position,
                                 cmx_af_cmd_pend.para.ringer.cb_func,
                                 cmx_af_cmd_pend.para.ringer.client_data);
  } else if(cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_PLAY) {
    cmx_af_cmd_pend.cmd_pend = CMX_CMD_INVALID;
    for(counter = 0; counter < cmx_af_cmd_pend.play_count; counter++) {
      cmx_audfmt_play_processing(cmx_af_cmd_pend.para.play[counter].handle,
                            &cmx_af_cmd_pend.para.play[counter].param,
                            cmx_af_cmd_pend.para.play[counter].cb_func,
                            cmx_af_cmd_pend.para.play[counter].client_data);
    }
    cmx_af_cmd_pend.play_count = 0;
  } else if(cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_PLAY_CODEC) {
    cmx_af_cmd_pend.cmd_pend = CMX_CMD_INVALID;
    cmx_audfmt_play_codec_processing(cmx_af_cmd_pend.para.play_codec.handle,
                              &cmx_af_cmd_pend.para.play_codec.codec_spec,
                              &cmx_af_cmd_pend.para.play_codec.av_sync,
                              cmx_af_cmd_pend.para.play_codec.cb_func,
                              cmx_af_cmd_pend.para.play_codec.client_data,
                              cmx_af_cmd_pend.para.play_codec.interface_src);
  }else if (cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_AVSYNC_PLAYBACK) {
   cmx_af_cmd_pend.cmd_pend = CMX_CMD_INVALID;
   cmx_audfmt_avsync_playback_processing(cmx_af_cmd_pend.para.play_codec.handle,
                    &cmx_af_cmd_pend.para.av_sync_playback.codec_spec,
                    &cmx_af_cmd_pend.para.av_sync_playback.av_sync,
                    cmx_af_cmd_pend.para.av_sync_playback.cb_func,
                    cmx_af_cmd_pend.para.av_sync_playback.client_data,
                    cmx_af_cmd_pend.para.av_sync_playback.interface_src);
  }
}

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_SVG
/*===========================================================================

FUNCTION cmx_audfmt_svg_cleanup

DESCRIPTION
  This function cleans up SVG components in a WebAudio playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_svg_cleanup (
  cmx_af_handle_type *handle
) {
   if( handle != NULL){
     cmx_client_buf_free(handle, &handle->svg_buffer);
     if(handle->svg_data != NULL) {
       if(handle->svg_data->mem_buffer != NULL) {
         CMX_FREE(handle->svg_data->mem_buffer);
       }
       if(!(handle->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF) &&
          (handle->svg_data->frame_buf != NULL)) {
         CMX_FREE(handle->svg_data->frame_buf);
       }
       CMX_FREE(handle->svg_data);
       handle->svg_data = NULL;
     }
     if( handle->status_mask & CMX_AF_STATUS_MASK_DISABLE_PENDING &&
         !( handle->status_mask & CMX_AF_STATUS_MASK_DLS_LOADED)
       ){
       cmx_free_af_handle( handle);
     }
   }
}
#endif /* FEATURE_SVG */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_wa_cleanup

DESCRIPTION
  This function cleans up multimedia components in a WebAudio playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_wa_cleanup (
  cmx_af_handle_type *handle
) {
  if( handle->cmd == CMX_AF_CMD_PLAY &&
     !(handle->status_mask & CMX_AF_STATUS_MASK_DISABLED)
    ) {
#ifdef FEATURE_PNG_DECODER
    if((handle->content_mask & CMX_AF_WA_CONTENTS_PIC) &&
       (cmx_png_audfmt_current)) {
      graph_png_decode_abort();
      cmx_png_audfmt_current = FALSE;

      /* Stop any further PNG data callbacks to client. */
      cmx_png_start_new      = TRUE;
    }
#endif /* FEATURE_PNG_DECODER */
#ifdef FEATURE_SAF
    if((handle->content_mask & CMX_AF_WA_CONTENTS_ANIM) &&
       (cmx_saf_audfmt_current)) {
      graph_saf_decode_abort();
      graph_saf_decode_stop();
      cmx_saf_audfmt_current     = FALSE;
      cmx_saf_status             = CMX_SAF_STATUS_STOPPED;
      cmx_saf_frame_cb           = NULL;
      cmx_saf_pending_handle_ptr = NULL;
      (void) rex_clr_timer( &cmx_saf_frame_timer );
    }
#endif /* FEATURE_SAF */
#if defined(FEATURE_PNG_DECODER) && defined(FEATURE_SAF)
    if(handle->content_mask &
       (CMX_AF_WA_CONTENTS_ANIM | CMX_AF_WA_CONTENTS_PIC)) {
      cmx_png_delay_handle = NULL;
      cmx_saf_delay_handle = NULL;
    }
#endif /* FEATURE_PNG_DECODER && FEATURE_SAF */

    if(handle->content_mask & CMX_AF_WA_CONTENTS_TEXT) {
      /* Clear text advance accumulator */
      cmx_text_advance_count      = 0;
#ifdef FEATURE_EFS
      cmx_text_counting_semaphore = 0;
#endif /* FEATURE_EFS */
    }

    cmx_client_buf_free(handle, &handle->text_buffer);
    cmx_client_buf_free(handle, &handle->pict_buffer);
#ifdef FEATURE_SAF
    cmx_client_buf_free(handle, &handle->saf_buffer);
    if(handle->saf_obj_buf != NULL) {
      CMX_FREE(handle->saf_obj_buf);
    }
#endif /* FEATURE_SAF */
#ifdef FEATURE_SVG
    if(handle->svg_data != NULL) {
      graph_svg_abort(handle->svg_data->svg_handle);
    }
#endif /* FEATURE_SVG */
    handle->status_mask |= CMX_AF_STATUS_MASK_DISABLED;
  }
}
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#ifdef FEATURE_DLS
/*===========================================================================

FUNCTION cmx_af_dls_callback

DESCRIPTION
  This function is used as the callback function for DLS commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_af_dls_callback (
  snd_af_data_cmd_enum  command,
  void                  *file,
  uint32                num,
  void                  *data
) {
  cmx_af_handle_type       *handle_ptr;
  cmx_packet_type          *packet;

  handle_ptr = (cmx_af_handle_type *) file;

  switch(command) {
    case SND_CMX_AF_READ:
      if((handle_ptr->cmd == CMX_AF_CMD_LOAD_DLS) ||
         (handle_ptr->cmd == CMX_AF_CMD_PLAY) ||
         (handle_ptr->cmd == CMX_AF_CMD_RINGER)) {
        packet = cmx_cmd_get_pkt ();

        if (packet != NULL) {
          packet->hdr.cmd                  = CMX_AF_CMD_READ;
          packet->audfmt_read.position     = num;
          packet->audfmt_read.handle       = handle_ptr;
          packet->audfmt_read.client_data  =
                                   ((snd_af_read_cb_type *) data)->client_data;
          packet->audfmt_read.cb_func      =
                                   ((snd_af_read_cb_type *) data)->cb_func;
          cmx_cmd (packet);
        }
      }
      break;

    case SND_CMX_AF_READ_VAR_DLS:
      if((handle_ptr->cmd == CMX_AF_CMD_LOAD_DLS) ||
         (handle_ptr->cmd == CMX_AF_CMD_PLAY) ||
         (handle_ptr->cmd == CMX_AF_CMD_RINGER)) {
        packet = cmx_cmd_get_pkt ();

        if (packet != NULL) {
          packet->hdr.cmd                  = CMX_AF_CMD_READ_VAR;
          packet->audfmt_read.cmd          = SND_CMX_AF_READ_VAR_DLS;
          packet->audfmt_read.position     = num;
          packet->audfmt_read.handle       = handle_ptr;
          packet->audfmt_read.client_data  =
                                   ((snd_af_read_cb_type *) data)->client_data;
          packet->audfmt_read.cb_func      =
                                   ((snd_af_read_cb_type *) data)->cb_func;
          packet->audfmt_read.buffer_size  =
                                   ((snd_af_read_cb_type *) data)->buffer_size;
          packet->audfmt_read.offset       =
                                   ((snd_af_read_cb_type *) data)->offset;
          cmx_cmd (packet);
        }
      }
      break;

    case SND_CMX_AF_FREE_BUF:
      if(((handle_ptr->cmd == CMX_AF_CMD_LOAD_DLS) ||
          (handle_ptr->cmd == CMX_AF_CMD_PLAY) ||
          (handle_ptr->cmd == CMX_AF_CMD_RINGER)) && (data != NULL)) {
        cmx_af_free_buff(handle_ptr->buffer, (uint8 *) data);
      }
      break;

    case SND_CMX_AF_FREE_VAR_BUF:
      if(data != NULL) {
        CMX_FREE(data);
      }
      break;

    case SND_CMX_AF_DLS_DONE:
    case SND_CMX_AF_DLS_FAILURE:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd                = CMX_AF_CMD_DLS_CB;
        packet->audfmt_support.command = command;
        packet->audfmt_support.handle  = handle_ptr;
        cmx_cmd (packet);
      }
      break;
  }
}
#endif /* FEATURE_DLS */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_callback_processing

DESCRIPTION
  This function takes the appropriate action for sound server callbacks
  for audio format sounds.

DEPENDENCIES
  cmx_af_stopped

RETURN VALUE
  None

SIDE EFFECTS
  cmx_af_active
  cmx_af_stopped

===========================================================================*/
void cmx_audfmt_callback_processing (
  void            *client_data,
  snd_status_type status
) {
  cmx_af_handle_type *handle;

  handle = (cmx_af_handle_type *) client_data;
  switch (status) {
    case SND_PLAY_DONE:
    case SND_PROMPT_BUSY:
    case SND_FAILURE:
    case SND_ABORT:
    case SND_ABORT_DATA_ACCESS_ERR:
    case SND_ABORT_DATA_ERR:
    case SND_ABORT_RESOURCE_ERR:
#if defined(FEATURE_XMF) && defined(FEATURE_DLS)
      if(handle->cmd != CMX_AF_CMD_UNLOAD_DLS)
#endif /* FEATURE_XMF && FEATURE_DLS */
      {
        cmx_audfmt_play_done_processing(status, client_data);

#if defined(FEATURE_WAVE_PB) && defined(FEATURE_MIDI_OUT_QCP)
        /* Free any reserved midi_out_qcp handle */
        if (cmx_midi_out_adpcm_handle_ptr != NULL) {
          q_put(&cmx_midi_out_free_q, &cmx_midi_out_adpcm_handle_ptr->link);
          cmx_midi_out_adpcm_handle_ptr = NULL;
        }
#endif /* FEATURE_WAVE_PB && FEATURE_MIDI_OUT_QCP */
      }

      cmx_af_active = CMX_AF_NONE;
      cmx_audfmt_check_pending();
      break;

    case SND_AF_TIME_TICK:
      MSG_MED("cmx_af_do_callback(CMX_TICK_UPDATE, 0x%x)", handle, 0, 0);
      cmx_af_do_callback(CMX_TICK_UPDATE, handle, NULL);
      break;

    case SND_REPEAT:
      MSG_MED("cmx_af_do_callback(CMX_REPEAT, 0x%x)", handle, 0, 0);
      cmx_af_do_callback(CMX_REPEAT, handle, NULL);
      break;

    case SND_SUCCESS:
      if(cmx_af_active == CMX_AF_NONE) {
        cmx_af_active = cmx_af_pend;
      }
      break;

    default:
      break;
  }
} /* end cmx_audfmt_callback_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play_done_processing

DESCRIPTION
  This function processes a play done status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_play_done_processing (
  snd_status_type status,
  void            *client_data
) {
 cmx_af_server_data_type server_data;
 boolean abort = TRUE;
#ifdef FEATURE_EFS
  fs_rsp_msg_type    rsp;
#endif
  cmx_af_handle_type *handle;
#if defined(FEATURE_XMF) && defined(FEATURE_DLS)
  snd_af_access_type *access_ptr;
#endif /* FEATURE_XMF && FEATURE_DLS */

  handle = (cmx_af_handle_type *) client_data;

  if((handle != NULL) && (handle->cmd != CMX_CMD_INVALID) &&
      !(handle->status_mask & CMX_AF_STATUS_MASK_DISABLED)) {
#ifdef FEATURE_WEBAUDIO
    cmx_audfmt_wa_cleanup(handle);
#endif /* FEATURE_WEBAUDIO */

#ifdef FEATURE_EFS
    if(handle->source_info.source == CMX_SOURCE_EFS) {
      fs_close(handle->source_info.handle.file, NULL, &rsp);
    }
#endif
    if(handle->source_info.source == CMX_SOURCE_CLIENT)
    {
      int i;
      for(i = 0; i < MAX_NUM_DATA_READ; i++)
      {
        cmx_af_data_cb_struct_type *data_cb_ptr;
        data_cb_ptr = &cmx_af_data_cb_array.data_cb[i];

        if((data_cb_ptr != NULL) && (data_cb_ptr->handle == handle) &&
            (cmx_af_data_cb_array.number > 0))
        {
          /* The requests by CMX for input data which are not serviced
             need to be handled properly at the time of stop*/
          INTLOCK();
          cmx_af_data_cb_array.index ++;
          if (cmx_af_data_cb_array.index == MAX_NUM_DATA_READ)
          {
            cmx_af_data_cb_array.index = 0;
          }
          cmx_af_data_cb_array.number --;
          if(data_cb_ptr->handle->buffer != NULL)
          {
            if(!cmx_af_free_buff(data_cb_ptr->handle->buffer, data_cb_ptr->buffer))
            {
              CMX_FREE(data_cb_ptr->buffer);
            }
          }
          data_cb_ptr->handle = NULL;
          INTFREE();
        }
      }
    }
    switch(status){
    case SND_PLAY_DONE:
      if(cmx_af_stopped == CMX_AF_STOP_PREEMPTED) {
        server_data.abort_status = CMX_ABORT_STATUS_PREEMPTED;
        MSG_HIGH("cmx_af_do_callback(CMX_ABORTED(PREEMPTED), 0x%x)",
                 handle, 0, 0);
        cmx_af_do_callback(CMX_ABORTED, handle, &server_data);
        cmx_af_stopped = CMX_AF_STOP_NONE;
      } else if((cmx_af_stopped == CMX_AF_STOP_CLIENT_ABORT) ||
                (handle->status_mask & CMX_AF_STATUS_MASK_PLAY_ABORTED)) {
        server_data.abort_status = CMX_ABORT_STATUS_CLIENT_ABORT;
        MSG_HIGH("cmx_af_do_callback(CMX_ABORTED(CLIENT_ABORT), 0x%x)",
                 handle, 0, 0);
        cmx_af_do_callback(CMX_ABORTED, handle, &server_data);
        cmx_af_stopped = CMX_AF_STOP_NONE;
      } else {
        MSG_HIGH("cmx_af_do_callback(CMX_DONE, 0x%x)", handle, 0, 0);
        cmx_af_do_callback(CMX_DONE, handle, NULL);
      }
      abort = FALSE;
      break;

    case SND_FAILURE:
    case SND_PROMPT_BUSY:
      MSG_HIGH("cmx_af_do_callback(CMX_FAILURE, 0x%x)", handle, 0, 0);
      cmx_af_do_callback(CMX_FAILURE, handle, NULL);
      abort = FALSE;
      break;

    case SND_ABORT:
      server_data.abort_status = CMX_ABORT_STATUS_REASON_UNKNOWN;
      MSG_HIGH("cmx_af_do_callback(CMX_ABORTED(REASON_UNKNOWN), 0x%x)",
               handle, 0, 0);
      break;

    case SND_ABORT_DATA_ERR:
      server_data.abort_status = CMX_ABORT_STATUS_DATA_ERR;
      MSG_HIGH("cmx_af_do_callback(CMX_ABORTED(DATA_ERR), 0x%x)",
               handle, 0, 0);
      break;

    case SND_ABORT_DATA_ACCESS_ERR:
      server_data.abort_status = CMX_ABORT_STATUS_DATA_ACCESS_ERR;
      MSG_HIGH("cmx_af_do_callback(CMX_ABORTED(DATA_ACCESS_ERR), 0x%x)",
               handle, 0, 0);
      break;

    case SND_ABORT_RESOURCE_ERR:
      MSG_HIGH("cmx_af_do_callback(CMX_ABORTED(RESOURCE_ERR), 0x%x)",
               handle, 0, 0);
      server_data.abort_status = CMX_ABORT_STATUS_RESOURCE_ERR;
      break;

    default:
      server_data.abort_status = CMX_ABORT_STATUS_MAX;
      MSG_HIGH("cmx_af_do_callback(CMX_ABORTED(MAX), 0x%x)",
               handle, 0, 0);
      break;
    }

    if (abort) {
      cmx_af_do_callback(CMX_ABORTED, handle, &server_data);
    }

#if defined(FEATURE_XMF) && defined(FEATURE_DLS)
    if(handle->status_mask & CMX_AF_STATUS_MASK_DLS_LOADED) {
      handle->cmd          = CMX_AF_CMD_UNLOAD_DLS;
      handle->cb_func.base = NULL;
      handle->client_data  = NULL;

      access_ptr = &handle->sound.audfmt_sound.audfmt_handle;
      access_ptr->audfmt_ptr = (void *) handle;
      access_ptr->data_func  =
                     (snd_af_data_cb_func_type) cmx_af_dls_callback;

      snd_audfmt_unload_dls(access_ptr);
    } else
#endif /* FEATURE_XMF && FEATURE_DLS */
#ifdef FEATURE_SVG
    if(handle->svg_data == NULL)
#endif /* FEATURE_SVG */
    {
      cmx_free_af_handle(handle);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_read_processing

DESCRIPTION
  This function issues a command to read from a midi/WebAudio file.

DEPENDENCIES
  cmx_af_buf_info
  cmx_af_data_cb_array

RETURN VALUE
  None

SIDE EFFECTS
  cmx_af_data_cb_array

===========================================================================*/
void cmx_audfmt_read_processing (
  cmx_af_handle_type       *handle,
  uint32                   position,
  void                     *client_data,
  snd_af_read_cb_func_type cb_func
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type    rsp;
#endif
  cmx_af_data_cb_struct_type *data_cb_ptr;
  uint32                     entry, num_bytes;
  uint8                      *buffer;

  if(!(handle->status_mask & CMX_AF_STATUS_MASK_DISABLE_PENDING)) {
    num_bytes = cmx_af_buf_info[handle->buffer->buf_type].num_bytes;

    if((handle->source_info.source == CMX_SOURCE_MEM) && (cb_func != NULL)) {
      if(position >= handle->source_info.cmx_len) {
        MSG_ERROR("read_processing(MEM): seek beyond client buffer", 0, 0, 0);
        cb_func(client_data, NULL, 0, position);
      }
      else {
        num_bytes = MIN(num_bytes, handle->source_info.cmx_len - position);
        buffer = (uint8 *) &(handle->source_info.handle.buf[position]);
        MSG_MED("cmx_audfmt_read_processing(MEM) at pos=%d, nbytes=%d",
                position, num_bytes, 0);
        cb_func(client_data, buffer, num_bytes, position);
      }
    } else if(handle->source_info.source == CMX_SOURCE_CLIENT) {
#ifdef FEATURE_QCP
      if((handle->cmd == CMX_QCP_GET_DATA_SIZE) ||
#ifdef FEATURE_AUDIO_POSTCARD
         (handle->cmd == CMX_CMD_CREATE_AUDIOP) ||
#endif /* FEATURE_AUDIO_POSTCARD */
         (handle->cmd == CMX_QCP_GET_FRAMES) ||
         (handle->cmd == CMX_QCP_GET_CNFG) ||
         (handle->cmd == CMX_QCP_SET_CNFG) ||
         (handle->cmd == CMX_QCP_GET_LABL) ||
         (handle->cmd == CMX_QCP_SET_LABL) ||
         (handle->cmd == CMX_QCP_GET_TEXT) ||
         (handle->cmd == CMX_QCP_SET_TEXT)) {
        buffer    = cmx_qcp_func_data_buffer;
        num_bytes = CMX_QCP_FUNC_BUFFER_SIZE;
      } else
#endif /* FEATURE_QCP */
      if((buffer = cmx_af_get_buff(handle->buffer)) == NULL) {
        MSG_ERROR("Bad buffer: %d",position,0,0);
        if(cb_func != NULL) {
          cb_func(client_data, NULL, 0, position);
        }
        return;
      }

      if(cmx_af_data_cb_array.number < MAX_NUM_DATA_READ) {
        entry = cmx_af_data_cb_array.index + cmx_af_data_cb_array.number;
        if(entry >= MAX_NUM_DATA_READ) {
          entry -= MAX_NUM_DATA_READ;
        }
        data_cb_ptr               = &cmx_af_data_cb_array.data_cb[entry];
        data_cb_ptr->handle       = handle;
        data_cb_ptr->snd_cb_func  = cb_func;
        data_cb_ptr->client_data  = client_data;
        data_cb_ptr->buffer       = buffer;
        data_cb_ptr->position     = position;
        data_cb_ptr->offset       = 0;
        data_cb_ptr->buffer_size  = num_bytes;
        cmx_af_data_cb_array.number++;

        /* Request client for more data */
        MSG_MED("cmx_audfmt_read_processing(CLIENT) at pos=%d, nbytes=%d",
                 position, num_bytes, 0);
        handle->source_info.handle.data_fn(position, num_bytes,
                                           cmx_af_client_read_cb,
                                           (const void *)data_cb_ptr,
                                           handle->client_data );
      } else {
        MSG_ERROR("Too many requests",0,0,0);
      }
    }
#ifdef FEATURE_EFS
    else if(handle->source_info.source == CMX_SOURCE_EFS) {
#ifdef FEATURE_QCP
      if((handle->cmd == CMX_QCP_GET_DATA_SIZE) ||
#ifdef FEATURE_AUDIO_POSTCARD
         (handle->cmd == CMX_CMD_CREATE_AUDIOP) ||
#endif /* FEATURE_AUDIO_POSTCARD */
         (handle->cmd == CMX_QCP_GET_FRAMES) ||
         (handle->cmd == CMX_QCP_GET_CNFG) ||
         (handle->cmd == CMX_QCP_SET_CNFG) ||
         (handle->cmd == CMX_QCP_GET_LABL) ||
         (handle->cmd == CMX_QCP_SET_LABL) ||
         (handle->cmd == CMX_QCP_GET_TEXT) ||
         (handle->cmd == CMX_QCP_SET_TEXT)) {
        buffer    = cmx_qcp_func_data_buffer;
        num_bytes = CMX_QCP_FUNC_BUFFER_SIZE;
      } else
#endif /* FEATURE_QCP */
      if((buffer = cmx_af_get_buff(handle->buffer)) == NULL) {
        MSG_ERROR("Bad buffer: %d",position,0,0);
        fs_close(handle->source_info.handle.file, NULL, &rsp);
        if(cb_func != NULL) {
          cb_func(client_data, NULL,
                 cmx_af_buf_info[handle->buffer->buf_type].num_bytes, position);
        }
        return;
      }
      MSG_MED("cmx_audfmt_read_processing(EFS) at pos=%d, nbytes=%d",
               position, num_bytes, 0);
      fs_seek_read(handle->source_info.handle.file, FS_SEEK_SET,
                   (fs_file_position_type) position, buffer, num_bytes,
                   NULL, &rsp);
      MSG_MED("cmx_audfmt_read_processing: Done reading from EFS",0,0,0);
      if(rsp.read.status == FS_OKAY_S && rsp.read.count != 0 )
      {
        if(cb_func != NULL) {
          cb_func(client_data,buffer,rsp.read.count,position);
        }
      }
      else if ((rsp.read.count == 0) ||
               ((rsp.read.status != FS_EOF_S) && (rsp.read.status != FS_BUSY_S))
              )
      {
        /* If read count is 0, there is a problem and parsers should be identified
        ** If the status is Not EOF and Not Busy, it means that the file system
        ** returned an error, so to terminate this playback we will send a NULL
        ** buffer and free the current buffer.
        ** audfmt parser will stop playback when NULL buffer is detected.
        */
        cmx_af_free_buff(handle->buffer, buffer);
        if(cb_func != NULL) {
          cb_func(client_data, NULL, 0, position);
        }
        MSG_ERROR("cmx_audfmt_read_processing: Read Error from EFS",0,0,0);
      }
    }
#endif /* FEATURE_EFS */
  }
} /* end cmx_audfmt_read_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_read_var_processing

DESCRIPTION
  This function issues a command to read a variable amount of data from a file.

DEPENDENCIES
  cmx_af_buf_info
  cmx_af_data_cb_array

RETURN VALUE
  None

SIDE EFFECTS
  cmx_af_data_cb_array

===========================================================================*/
void cmx_audfmt_read_var_processing (
  snd_af_data_cmd_enum     cmd,
  cmx_af_handle_type       *handle,
  uint32                   position,
  uint32                   buffer_size,
  uint32                   offset,
  void                     *client_data,
  snd_af_read_cb_func_type cb_func
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type    rsp;
#endif
  cmx_af_data_cb_struct_type *data_cb_ptr;
  uint32                     entry, num_bytes;
  uint8                      *buffer;

  if((!(handle->status_mask & CMX_AF_STATUS_MASK_DISABLE_PENDING)) &&
     (cb_func != NULL)) {
    if(offset > buffer_size) {
      ERR_FATAL("Invalid buffer request: %d, %d", offset, buffer_size, 0);
    }
    if(cmd == SND_CMX_AF_READ_VAR_DLS) {
      buffer = (uint8 *) CMX_MALLOC(buffer_size + 10);
    } else if((cmd == SND_CMX_AF_READ_VAR) &&
              (handle->buffer != NULL) &&
              (buffer_size <=
               cmx_af_buf_info[handle->buffer->buf_type].num_bytes)) {
      buffer = cmx_af_get_buff(handle->buffer);
    } else {
      buffer = (uint8 *) CMX_MALLOC(buffer_size);
    }

    if(buffer == NULL) {
      MSG_ERROR("Bad buffer: %d",position,0,0);
#ifdef FEATURE_EFS
      if(handle->source_info.source == CMX_SOURCE_EFS) {
        fs_close(handle->source_info.handle.file, NULL, &rsp);
      }
#endif /* FEATURE_EFS */
      cb_func(client_data, NULL, 0, position);
      return;
    } else if(buffer_size == offset) {
      cb_func(client_data, buffer, 0, position);
      return;
    }

    if(handle->source_info.source == CMX_SOURCE_MEM) {
      if(position >= handle->source_info.cmx_len) {
        if ((cmd == SND_CMX_AF_READ_VAR_DLS) ||
            (handle->buffer == NULL) ||
            !(cmx_af_free_buff(handle->buffer, buffer))) {
          CMX_FREE(buffer);
        }
        MSG_ERROR("Read_var_processing: seeking beyond client buffer", 0,0,0);
        if (cb_func != NULL) {
          cb_func(client_data, NULL, 0, position);
        }
      } else {
        num_bytes = MIN(buffer_size - offset,
                        handle->source_info.cmx_len - position);
        memcpy(&buffer[offset], &handle->source_info.handle.buf[position],
               num_bytes);
        MSG_MED("cmx_audfmt_read_var_processing(MEM) at pos=%d,nbytes=%d",
                position, num_bytes, 0);
        if (cb_func != NULL) {
          cb_func(client_data, buffer, num_bytes, position);
        }
      }
    } else if(handle->source_info.source == CMX_SOURCE_CLIENT) {
#ifdef FEATURE_MIDI_OUT_QCP
      if (handle->cmd != CMX_MIDI_OUT_QCP)
#endif /* FEATURE_MIDI_OUT_QCP */
      {
        if(cmx_af_data_cb_array.number < MAX_NUM_DATA_READ) {
          num_bytes = MIN(buffer_size - offset,
                          handle->source_info.cmx_len - position);

          entry = cmx_af_data_cb_array.index + cmx_af_data_cb_array.number;
          if(entry >= MAX_NUM_DATA_READ) {
            entry -= MAX_NUM_DATA_READ;
          }
          data_cb_ptr               = &cmx_af_data_cb_array.data_cb[entry];
          data_cb_ptr->handle       = handle;
          data_cb_ptr->snd_cb_func  = cb_func;
          data_cb_ptr->client_data  = client_data;
          data_cb_ptr->buffer       = buffer;
          data_cb_ptr->position     = position;
          data_cb_ptr->offset       = offset;
          data_cb_ptr->buffer_size  = num_bytes;
          cmx_af_data_cb_array.number++;

          /* Request client for more data */
          MSG_MED("cmx_audfmt_read_var_processing(CLIENT) at pos=%d,nbytes=%d",
                   position, num_bytes, 0);
          handle->source_info.handle.data_fn(position, num_bytes,
                                             cmx_af_client_read_var_cb,
                                             (const void *) data_cb_ptr,
                                             handle->client_data );
        } else {
          CMX_FREE(buffer);
          MSG_ERROR("Too many requests",0,0,0);
        }
      }
#ifdef FEATURE_MIDI_OUT_QCP
      else {
        if(cmx_adec_data_cb.number < CMX_ADEC_MAX_CB_STRUCTS) {
          entry = cmx_adec_data_cb.index + cmx_adec_data_cb.number;
          if(entry >= CMX_ADEC_MAX_CB_STRUCTS) {
            entry -= CMX_ADEC_MAX_CB_STRUCTS;
          }
          cmx_adec_data_cb.number++;

          num_bytes = buffer_size - offset;

          data_cb_ptr               = &cmx_adec_data_cb.data_cb[entry];
          data_cb_ptr->snd_cb_func  = cb_func;
          data_cb_ptr->client_data  = client_data;
          data_cb_ptr->buffer       = buffer;
          data_cb_ptr->position     = position;
          data_cb_ptr->offset       = offset;
          data_cb_ptr->buffer_size  = num_bytes;

          MSG_MED("cmx_audfmt_read_var_processing(CLIENT) at pos=%d,nbytes=%d",
                   position, num_bytes, 0);
          handle->source_info.handle.data_fn(position, num_bytes,
                                             cmx_adec_client_cb,
                                             (const void *) data_cb_ptr,
                                             handle->client_data);
        } else {
          CMX_FREE(buffer);
          MSG_ERROR("Data cb or buffer not available!!!",0,0,0);
        }
      }
#endif /* FEATURE_MIDI_OUT_QCP */
    }
#ifdef FEATURE_EFS
    else if(handle->source_info.source == CMX_SOURCE_EFS) {
      MSG_MED("cmx_audfmt_read_var_processing(EFS) pos=%d,nbytes=%d,offset=%d",
               position, (buffer_size - offset), offset);

      fs_seek_read(handle->source_info.handle.file, FS_SEEK_SET,
                   (fs_file_position_type) position, &buffer[offset],
                   buffer_size - offset, NULL, &rsp);

      MSG_MED("cmx_audfmt_read_var_processing: Done reading from EFS",0,0,0);
      if((rsp.read.status == FS_OKAY_S) && (rsp.read.count != 0)) {
        if(cb_func != NULL) {
          cb_func(client_data, buffer, rsp.read.count, position);
        }
      } else if ((rsp.read.count == 0) ||
               ((rsp.read.status != FS_EOF_S) && (rsp.read.status != FS_BUSY_S))
                 ) {
        if ((cmd == SND_CMX_AF_READ_VAR_DLS) ||
            (handle->buffer == NULL) ||
            !(cmx_af_free_buff(handle->buffer, buffer))) {
          CMX_FREE(buffer);
        }

        /* If the status is Not EOF and Not Busy, it means that the file system
        ** returned an error, so to terminate this playback we will send a NULL
        ** buffer.
        ** audfmt parser will stop playback when NULL buffer is detected.
        */
        if(cb_func != NULL) {
          cb_func(client_data,NULL,0,position);
        }
        MSG_ERROR("cmx_audfmt_read_var_processing: Read Error from EFS",
                  0,0,0);

      }
    }
#endif /* FEATURE_EFS */
  }
} /* end cmx_audfmt_read_var_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_dls_processing

DESCRIPTION
  This function issues a command to get DLS waveform from file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_dls_processing (
  cmx_af_handle_type          *handle,
  uint32                      length,
  uint32                      position,
  void                        *client_data,
  snd_af_get_dls_cb_func_type cb_func
) {
#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_EFS
  fs_rsp_msg_type    rsp;
#endif
  uint8 *buffer = NULL, *data = NULL;

  if((!(handle->status_mask & CMX_AF_STATUS_MASK_DISABLE_PENDING)) &&
     (cb_func != NULL)) {
    buffer = (uint8 *) CMX_MALLOC((length + 3) << 2);

    if(buffer != NULL) {
      if(handle->source_info.source == CMX_SOURCE_MEM) {
        data = (uint8 *) &(handle->source_info.handle.buf[position]);
        cb_func(client_data, data, buffer, length);
        return;
      }
#ifdef FEATURE_EFS
      else if(handle->source_info.source == CMX_SOURCE_EFS) {
        data = (uint8 *) CMX_MALLOC(length);

        if(data != NULL) {
          fs_seek_read(handle->source_info.handle.file, FS_SEEK_SET,
                       (fs_file_position_type) position, data, length,
                       NULL, &rsp);

          if(rsp.read.status == FS_OKAY_S)
          {
            cb_func(client_data, data, buffer, length);
            return;
          }
        } else {
          MSG_ERROR("Failed to allocate memory", 0,0,0);
        }
      }
#endif /* FEATURE_EFS */
    } else {
      MSG_ERROR("Failed to allocate memory", 0,0,0);
    }

    if(buffer != NULL) {
      CMX_FREE(buffer);
    }
    if(data != NULL) {
      CMX_FREE(data);
    }
    cb_func(client_data, NULL, NULL, length);
  }
#else
  if(cb_func != NULL) {
    cb_func(client_data, NULL, NULL, length);
  }
#endif /* FEATURE_WEBAUDIO */
} /* end cmx_audfmt_dls_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_support_processing

DESCRIPTION
  This function performs the appropriate WebAudio support.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_support_processing (
  snd_af_data_cmd_enum command,
  cmx_af_handle_type   *handle,
  uint32               position,
  uint32               num
  /*lint -esym(715,num) Suppress "unused" Message */
) {
  cmx_af_server_data_type  server_data;
  cmx_status_type          audio_command = CMX_ERROR;
#ifdef FEATURE_WEBAUDIO
  boolean                  do_text = FALSE;
#endif

  if((handle->cb_func.play == NULL) && (handle->cb_func.play_codec == NULL)) {
    return;
  }

  switch(command) {
#ifdef FEATURE_WEBAUDIO
    case SND_CMX_AF_TEXT_CLEAR:
      server_data.text_ctl.text_ctl = CMX_AF_TEXT_CTL_CLEAR;
      do_text                       = TRUE;
      MSG_MED("cmx_audfmt_support_processing(CMX_AF_TEXT_CTL_CLEAR)",0,0,0);
      break;

    case SND_CMX_AF_TEXT_BACK:
      server_data.text_ctl.text_ctl = CMX_AF_TEXT_CTL_REWIND;
      server_data.text_ctl.num      = position;
      do_text                       = TRUE;
      MSG_MED("cmx_audfmt_support_processing(CMX_AF_TEXT_CTL_REWIND)",0,0,0);
      break;

    case SND_CMX_AF_TEXT_ADVANCE:
      server_data.text_ctl.text_ctl = CMX_AF_TEXT_CTL_ADVANCE;
      do_text                       = TRUE;
      INTLOCK();
      server_data.text_ctl.num      = cmx_text_advance_count;
      cmx_text_advance_count        = 0;
      INTFREE();
      MSG_MED("cmx_audfmt_support_processing(CMX_AF_TEXT_CTL_ADVANCE)",0,0,0);
      break;

    case SND_CMX_AF_TEXT_DISABLE:
      server_data.text_ctl.text_ctl = CMX_AF_TEXT_CTL_DISABLE;
      do_text                       = TRUE;
      MSG_MED("cmx_audfmt_support_processing(CMX_AF_TEXT_CTL_DISABLE)",0,0,0);
      break;

    case SND_CMX_AF_TEXT_ENABLE:
      server_data.text_ctl.text_ctl = CMX_AF_TEXT_CTL_ENABLE;
      do_text                       = TRUE;
      MSG_MED("cmx_audfmt_support_processing(CMX_AF_TEXT_CTL_ENABLE)",0,0,0);
      break;

    case SND_CMX_AF_ANIM_CLEAR:
      audio_command = CMX_ANIM_CLEAR;
      MSG_MED("cmx_audfmt_support_processing(CMX_ANIM_CLEAR)",0,0,0);
      break;

    case SND_CMX_AF_ANIM_DISABLE:
      audio_command = CMX_ANIM_DISABLE;
      MSG_MED("cmx_audfmt_support_processing(CMX_ANIM_DISABLE)",0,0,0);
      break;

    case SND_CMX_AF_ANIM_ENABLE:
      audio_command = CMX_ANIM_ENABLE;
      MSG_MED("cmx_audfmt_support_processing(CMX_ANIM_ENABLE)",0,0,0);
      break;

    case SND_CMX_AF_CUE_POINT_END:
      audio_command = CMX_CUE_POINT;
      MSG_MED("cmx_audfmt_support_processing(CMX_CUE_POINT)",0,0,0);
      break;

    case SND_CMX_AF_JUMP_POINT_END:
      audio_command = CMX_JUMP_POINT;
      MSG_MED("cmx_audfmt_support_processing(CMX_JUMP_POINT)",0,0,0);
      break;
#endif

    case SND_CMX_AF_FLUSHED:
      server_data.elapsed_time_ms = position;
      audio_command               = CMX_FLUSHED;
      MSG_MED("cmx_audfmt_support_processing(CMX_FLUSHED)", 0,0,0);
      break;

    case SND_CMX_AF_REWIND:
      server_data.elapsed_time_ms = position;
      audio_command               = CMX_REWIND;
      MSG_MED("cmx_audfmt_support_processing(CMX_REWIND: elapsed_time=%d)",
               position,0,0);
      break;

    case SND_CMX_AF_FFORWARD:
      server_data.elapsed_time_ms = position;
      audio_command               = CMX_FFORWARD;
      MSG_MED("cmx_audfmt_support_processing(CMX_FFORWARD: elapsed_time=%d)",
               position,0,0);
      break;

    case SND_CMX_AF_PAUSE:
      server_data.elapsed_time_ms = position;
      audio_command               = CMX_PAUSE;
      MSG_MED("cmx_audfmt_support_processing(CMX_PAUSE: elapsed_time=%d)",
               position,0,0);
      break;

    case SND_CMX_AF_RESUME:
      server_data.elapsed_time_ms = position;
      audio_command               = CMX_RESUME;
      MSG_MED("cmx_audfmt_support_processing(CMX_RESUME: elapsed_time=%d)",
               position,0,0);
      break;

    case SND_CMX_AF_FA_DELAY:
      server_data.elapsed_time_ms = position;
      audio_command               = CMX_DATA_ACCESS_DELAY;
      MSG_HIGH(
        "cmx_audfmt_support_processing(CMX_DATA_ACCESS_DELAY: elapsed_time=%d)",
        position,0,0);
      break;

    case SND_CMX_AF_TEMPO:
      server_data.tempo = position;
      audio_command     = CMX_TEMPO;
      MSG_MED("cmx_audfmt_support_processing(CMX_TEMPO: tempo=%d)",
               position,0,0);
      break;

    case SND_CMX_AF_TUNE:
      server_data.tune  = position;
      audio_command     = CMX_TUNE;
      MSG_MED("cmx_audfmt_support_processing(CMX_TUNE: tune=%d)",
               position,0,0);
      break;

    case SND_CMX_AF_PAN:
      server_data.pan   = position;
      audio_command     =  CMX_PAN;
      MSG_MED("cmx_audfmt_support_processing(CMX_PAN: pan=%d)", position,0,0);
      break;

    case SND_CMX_AF_STARTED:
      audio_command = CMX_STARTED;
      MSG_MED("cmx_audfmt_support_processing(CMX_STARTED)",0,0,0);
      break;

    case SND_CMX_AF_SBR_MODE:
      audio_command = CMX_SBR_MODE;
      MSG_MED("cmx_audfmt_support_processing(CMX_SBR_MODE)",0,0,0);
      break;

    case SND_CMX_AF_DUAL_MONO_MODE:
      audio_command = CMX_DUAL_MONO_MODE;
      MSG_MED("cmx_audfmt_support_processing(CMX_DUAL_MONO_MODE)",0,0,0);
      break;

    default:
      break;
  }

  if (audio_command != CMX_ERROR) {
    if(handle->cmd == CMX_AF_CMD_PLAY) {
      handle->cb_func.play(audio_command, handle->client_data, &server_data);
    } else if (handle->cmd == CMX_AF_CMD_PLAY_CODEC) {
      handle->cb_func.play_codec(audio_command, handle->client_data);
    }else if (handle->cmd == CMX_AF_CMD_AVSYNC_PLAYBACK){
       handle->cb_func.av_sync_playback(audio_command,handle->client_data ,
                                                              &server_data);
    }
  }
#ifdef FEATURE_WEBAUDIO
  else if(do_text) {
    handle->cb_func.play(CMX_TEXT_CTL, handle->client_data, &server_data);
  }
#endif
} /* end cmx_audfmt_support_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_get_audio_spec

DESCRIPTION
  This function returns an audio spec structure if there is one available.

DEPENDENCIES
  cmx_audio_spec_buffer

RETURN VALUE
  cmx_af_audio_spec_type * - pointer to an audio spec structure; NULL is
                             returned if one is not available.

SIDE EFFECTS
  None

===========================================================================*/
cmx_af_audio_spec_type * cmx_get_audio_spec (void)
{
  cmx_af_audio_spec_type *ret_val = NULL;
  uint32 counter;

  for(counter = 0; counter < CMX_NUM_AUDIO_SPEC; counter++) {
    if(cmx_audio_spec_buffer[counter].file_type == CMX_AF_FILE_UNKNOWN) {
      ret_val = &cmx_audio_spec_buffer[counter];
      break;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_free_audio_spec

DESCRIPTION
  This function frees the indicated audio spec structure.

DEPENDENCIES
  cmx_audio_spec_buffer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_free_audio_spec (
  cmx_af_audio_spec_type *spec
) {
  uint32 counter;

  for(counter = 0; counter < CMX_NUM_AUDIO_SPEC; counter++) {
    if(spec == &cmx_audio_spec_buffer[counter]) {
      spec->file_type = CMX_AF_FILE_UNKNOWN;
      break;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_audio_spec_processing

DESCRIPTION
  This function calls back client with the audio specs of the WebAudio file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_audio_spec_processing (
  cmx_af_handle_type       *handle,
  cmx_af_file_enum_type    file_type,
  cmx_af_audio_spec_type   *audio_spec
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type rsp;
#endif

#ifdef FEATURE_MP3
  int cnt = 0;
#endif

  if(handle->cmd == CMX_AF_CMD_GET_SPEC) {
    if(handle->cb_func.spec != NULL) {
      if((file_type == CMX_AF_FILE_UNKNOWN) || (audio_spec == NULL) ||
         (file_type != audio_spec->file_type)) {
        cmx_af_audio_spec_type local_spec;

        local_spec.file_type = CMX_AF_FILE_UNKNOWN;

        handle->cb_func.spec(CMX_DONE, handle->client_data, &local_spec);
      } else {
        handle->cb_func.spec(CMX_DONE, handle->client_data, audio_spec);
      }
    }
#ifdef FEATURE_EFS
    if(handle->source_info.source == CMX_SOURCE_EFS) {
      fs_close(handle->source_info.handle.file, NULL, &rsp);
      if(rsp.close.status != FS_OKAY_S) {
        MSG_ERROR("Unable to close file: %d", rsp.close.status, 0, 0);
      }
    }
#endif
    cmx_free_af_handle(handle);
    cmx_af_active = CMX_AF_NONE;
  } else {
    cmx_af_server_data_type server_data;

    if(handle->cb_func.play != NULL) {
      if((file_type == CMX_AF_FILE_UNKNOWN) || (audio_spec == NULL) ||
         (file_type != audio_spec->file_type)) {
        server_data.audio_spec.file_type = CMX_AF_FILE_UNKNOWN;
      } else {
        *((cmx_af_audio_spec_type *) &server_data.audio_spec) = *audio_spec;
      }

      MSG_MED("Issued CMX_AUDIO_SPEC: client_data=0x%x",
               handle->client_data,0,0);
      handle->cb_func.play(CMX_AUDIO_SPEC, handle->client_data, &server_data);
    }
  }

#ifdef FEATURE_MP3
  if((file_type == CMX_AF_FILE_MP3) && (audio_spec != NULL) &&
     (audio_spec->mp3_spec.v2_tag_found)) {

    cmx_id3v2_tag_frame_type *frame_ptr = &(audio_spec->mp3_spec.v2_tag.title);

    for (cnt = 0; cnt < CMX_ID3V2_TAG_FRAME_TYPE_MAX; cnt++, frame_ptr++) {
      if (frame_ptr->content != NULL) {
        free(frame_ptr->content);
        frame_ptr->content = NULL;
      }

      frame_ptr->size = 0;
    }

    /* free the memory allocated for gframes */
    if(audio_spec->mp3_spec.v2_tag.gframes != NULL) {
      uint32 gframes_cnt = audio_spec->mp3_spec.v2_tag.gframes_cnt;
      cmx_id3v2_generic_frame_type *gframes =
                                          audio_spec->mp3_spec.v2_tag.gframes;
      /* free all gereral frames */
      for (cnt = 0; cnt < gframes_cnt; cnt++) {
        if ( gframes[cnt].info_type == CMX_ID3V2_TAG_INFO_BY_CONTENT ) {
          free(gframes[cnt].frame_data.frame_content);
          gframes[cnt].frame_data.frame_content = NULL;
        }
      }

      free(audio_spec->mp3_spec.v2_tag.gframes);
      audio_spec->mp3_spec.v2_tag.gframes = NULL;
    }

  }
#endif /* FEATURE_MP3 */

  cmx_free_audio_spec(audio_spec);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_time_cb_processing

DESCRIPTION
  This function processes the get time command callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_get_time_cb_processing (
  cmx_af_handle_type *handle,
  snd_status_type    status,
  uint32             time_ms
) {
#ifdef FEATURE_EFS
  fs_handle_type     fd;
  fs_rsp_msg_type    rsp;
#endif

  if(handle->cmd != CMX_CMD_INVALID) {
    if(handle->cb_func.time != NULL) {
      switch(status){
      case SND_SUCCESS:
        break;
      case SND_PLAY_DONE:
        MSG_HIGH("get_time_cb(): Song Length: %ld", time_ms,0,0);
        handle->cb_func.time(CMX_DONE, handle->client_data, time_ms);
        break;
      case SND_ABORT:
      case SND_ABORT_DATA_ACCESS_ERR:
      case SND_ABORT_DATA_ERR:
      case SND_ABORT_RESOURCE_ERR:
        MSG_HIGH("get_time_cb(CMX_ABORTED)", 0,0,0);
        handle->cb_func.time(CMX_ABORTED, handle->client_data, 0);
        break;
      default:
        MSG_HIGH("get_time_cb(CMX_FAILURE)", 0,0,0);
        handle->cb_func.time(CMX_FAILURE, handle->client_data, 0);
        break;
      }
    }

    if(status != SND_SUCCESS) {
#ifdef FEATURE_EFS
      if(handle->source_info.source == CMX_SOURCE_EFS) {
        fd = handle->source_info.handle.file;
        fs_close(fd, NULL, &rsp);
        if(rsp.close.status != FS_OKAY_S) {
          MSG_ERROR("Unable to close file: %d", rsp.close.status, 0, 0);
        }
      }
#endif
      cmx_free_af_handle(handle);

      if( cmx_af_gt_active )
        cmx_af_gt_active--;

      if(cmx_af_gt_active == 0) {
        cmx_af_active = CMX_AF_NONE;
        cmx_audfmt_check_pending();
      }
    }
  }
} /* end cmx_audfmt_get_time_cb_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_spec_cb_processing

DESCRIPTION
  This function processes the get time command callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_get_spec_cb_processing (
  cmx_af_handle_type *handle,
  snd_status_type    status
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type    rsp;
#endif

  if(handle->cmd == CMX_AF_CMD_GET_SPEC) {
    if (handle->cb_func.spec != NULL) {
      if(status == SND_SUCCESS) {
        MSG_MED("get_spec_cb(CMX_SUCCESS): client_data=0x%x",
                 handle->client_data,0,0);
        handle->cb_func.spec(CMX_SUCCESS, handle->client_data, NULL);
      } else if(status == SND_FAILURE) {
        MSG_ERROR("get_spec_cb(CMX_FAILURE): client_data=0x%x",
                 handle->client_data,0,0);
        handle->cb_func.spec(CMX_FAILURE, handle->client_data, NULL);
#ifdef FEATURE_EFS
        if(handle->source_info.source == CMX_SOURCE_EFS) {
          fs_close(handle->source_info.handle.file, NULL, &rsp);
          if(rsp.close.status != FS_OKAY_S) {
            MSG_ERROR("Unable to close file: %d", rsp.close.status, 0, 0);
          }
        }
#endif
        cmx_free_af_handle(handle);
        cmx_af_active = CMX_AF_NONE;
      }
    }
  }
} /* end cmx_audfmt_get_spec_cb_processing */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION cmx_audfmt_text_ctl_processing

DESCRIPTION
  This function gets the complete text string and returns it to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_text_ctl_processing (
  cmx_af_text_ctl_enum_type  command,
  uint8                      *buffer,
  uint32                     buf_len,
  cmx_af_handle_type         *handle,
  uint32                     position,
  uint32                     remain_bytes,
  cmx_af_pos_enum_type       xpos_type,
  cmx_af_pos_enum_type       ypos_type
) {
  cmx_af_server_data_type server_data;
#ifdef FEATURE_EFS
  fs_rsp_msg_type         rsp;
  fs_handle_type          file;
#endif

  if(handle->cb_func.play != NULL) {
    if(handle->cb_type == CMX_AF_CALLBACK_REFERENCE) {
      server_data.text_info.text_ctl  = command;
      server_data.text_info.size      = remain_bytes;
      server_data.text_info.offset    = position;
      server_data.text_info.xpos_type = xpos_type;
      server_data.text_info.ypos_type = ypos_type;

      if(handle->cb_func.play != NULL) {
        handle->cb_func.play(CMX_TEXT_INFO, handle->client_data, &server_data);
      }
    } else {
      server_data.text_ctl.text_ctl  = command;
      server_data.text_ctl.xpos_type = xpos_type;
      server_data.text_ctl.ypos_type = ypos_type;

      if(handle->source_info.source == CMX_SOURCE_MEM) {
        server_data.text_ctl.buffer =
                 (uint8 *) &(handle->source_info.handle.buf[position-buf_len]);
        server_data.text_ctl.num    = buf_len + remain_bytes;
      }
#ifdef FEATURE_EFS
      else if(handle->source_info.source == CMX_SOURCE_EFS) {
        if((remain_bytes != 0) && (buf_len < CMX_MAX_TEXT_STRING)) {
          file = (handle->source_info.handle.file);

          fs_seek_read(file, FS_SEEK_SET, (fs_file_position_type) position,
                       &(buffer[buf_len]),
                       MIN(CMX_MAX_TEXT_STRING-buf_len, remain_bytes),
                       NULL, &rsp);

          if((rsp.read.status != FS_OKAY_S) && (rsp.read.status != FS_EOF_S)) {
            MSG_ERROR("Bad read response: %d",rsp.read.status,0,0);
            return;
          }
        }

        server_data.text_ctl.num    = MIN(CMX_MAX_TEXT_STRING,
                                          buf_len + remain_bytes);
        server_data.text_ctl.buffer = buffer;

        INTLOCK();
        cmx_text_counting_semaphore--;
        INTFREE();
      }
#endif

      handle->cb_func.play(CMX_TEXT_CTL, handle->client_data, &server_data);
    }
  }

} /* end cmx_audfmt_text_ctl_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pic_init_processing

DESCRIPTION
  This function returns the setup information of the current picture to
  the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_pic_init_processing (
  cmx_af_handle_type         *handle,
  cmx_af_pic_speed_enum_type draw_speed,
  cmx_af_pos_enum_type       xpos_type,
  cmx_af_pos_enum_type       ypos_type,
  uint8                      xpos_percent,
  uint8                      ypos_percent
) {
  cmx_af_server_data_type   server_data;

  if(handle->cb_func.play != NULL) {
    server_data.pic_status.pic_status    = CMX_AF_PIC_STATUS_INIT;
    server_data.pic_status.draw_speed    = draw_speed;
    server_data.pic_status.xpos_type     = xpos_type;
    server_data.pic_status.ypos_type     = ypos_type;
    server_data.pic_status.xpos_percent  = xpos_percent;
    server_data.pic_status.ypos_percent  = ypos_percent;

    handle->cb_func.play(CMX_PICTURE_STATUS, handle->client_data, &server_data);
  }
} /* end cmx_audfmt_pic_init_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pic_data_processing

DESCRIPTION
  This function sets up the appropriate mechanism to pass picture data
  back to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_pic_data_processing (
  cmx_af_pic_enum_type pic_type,
  uint8                *buffer,
  uint32               buf_len,
  cmx_af_handle_type   *handle,
  uint32               position,
  uint32               remain_bytes
) {
  cmx_af_server_data_type  server_data;
#ifdef FEATURE_JPEG_DECODER
  jpeg_decodeInfo_type     jpegdInfo;
#endif

  if(handle->cb_type == CMX_AF_CALLBACK_REFERENCE) {
    server_data.pic_info.pic_type = pic_type;
    server_data.pic_info.size     = remain_bytes;
    server_data.pic_info.offset   = position;

    if(handle->cb_func.play != NULL) {
      handle->cb_func.play(CMX_PICTURE_INFO, handle->client_data, &server_data);
    }

#ifdef FEATURE_PNG_DECODER
  } else if(pic_type == CMX_AF_PIC_PNG) {
    cmx_png_offset     = position;
    cmx_png_buffer     = buffer;
    cmx_png_buf_bytes  = buf_len;
    cmx_png_size       = buf_len + remain_bytes;

#ifdef FEATURE_SAF
    if (cmx_saf_status == CMX_SAF_STATUS_PREPARING) {
      cmx_png_delay_handle = handle;
    } else
#endif /* FEATURE_SAF */
    {
      cmx_png_audfmt_current = TRUE;

      graph_png_set_buf(cmx_image_buf, CMX_IMAGE_BUFFER_SIZE,
                        handle->pict_buffer.buffer, CMX_PIC_BUFFER_SIZE);
      graph_png_decode(handle, (png_cmx_decode_cb_func_type) cmx_png_callback);
    }
#endif /* FEATURE_PNG_DECODER */
#ifdef FEATURE_JPEG_DECODER
  } else if(pic_type == CMX_AF_PIC_JPEG) {
    cmx_jpeg_offset     = position;
    cmx_jpeg_buffer     = buffer;
    cmx_jpeg_buf_bytes  = buf_len;
    cmx_jpeg_size       = buf_len + remain_bytes;

    graph_jpeg_set_buf(cmx_image_buf, CMX_IMAGE_BUFFER_SIZE,
                       handle->pict_buffer.buffer);
    /*graph_jpeg_decode*/
    jpegdInfo.isSync    = FALSE;
    jpegdInfo.outFormat = JPEGD_RGB888;
    jpegdInfo.arm_based = TRUE;
    jpeg_ARM_based_decode(handle, (jpeg_cmx_cb_func_type) cmx_jpeg_callback,
                          jpegdInfo);
#endif
  } else {
    if(handle->cb_func.play != NULL) {
      if(buf_len != 0) {
        server_data.pic_data.pic_data = CMX_AF_PIC_DATA_APPEND;
        server_data.pic_data.pic_type = pic_type;
        server_data.pic_data.num      = buf_len;
        server_data.pic_data.buffer   = buffer;
        handle->cb_func.play(CMX_PICTURE_DATA, handle->client_data,
                             &server_data);
      }

      if(handle->source_info.source == CMX_SOURCE_MEM) {
        server_data.pic_data.pic_data = CMX_AF_PIC_DATA_DONE;
        server_data.pic_data.pic_type = pic_type;
        server_data.pic_data.num      = remain_bytes;
        server_data.pic_data.buffer   =
             (uint8 *) &(handle->source_info.handle.buf[position]);
        handle->cb_func.play(CMX_PICTURE_DATA, handle->client_data,
                             &server_data);
      }
#ifdef FEATURE_EFS
      else if(handle->source_info.source == CMX_SOURCE_EFS) {
        if(remain_bytes != 0) {
          cmx_pic_type     = pic_type;
          cmx_pic_position = position;
          cmx_pic_remain   = remain_bytes;
          cmx_pic_handle   = handle;
          cmx_af_pic_read_processing();
        }
      }
#endif
    }
  }
} /* end cmx_audfmt_pic_data_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pic_ctl_processing

DESCRIPTION
  This function sends a picture control command back to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_pic_ctl_processing (
  cmx_af_pic_status_enum_type command,
  cmx_af_handle_type          *handle,
  cmx_af_pic_speed_enum_type  speed
) {
  cmx_af_server_data_type server_data;

  if(handle->cb_func.play != NULL) {
    server_data.pic_status.pic_status = command;
    server_data.pic_status.draw_speed = speed;

    handle->cb_func.play(CMX_PICTURE_STATUS, handle->client_data, &server_data);
  }

} /* end cmx_audfmt_pic_ctl_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_anim_data_processing

DESCRIPTION
  This function sets up the appropriate mechanism to initiate SAF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_anim_data_processing (
  cmx_af_anim_enum_type   type,
  uint8                   *buffer,
  uint32                  buf_len,
  cmx_af_handle_type      *handle,
  uint32                  position,
  uint32                  remain_bytes
) {
  cmx_af_server_data_type  server_data;
#ifdef FEATURE_SVG
  svg_load_data_type       svg_load_data;
#endif /* FEATURE_SVG */

  if(handle->cb_type == CMX_AF_CALLBACK_REFERENCE) {
    server_data.anim_info.anim_type = type;
    server_data.anim_info.anim_cmd  = CMX_AF_ANIM_CMD_DATA;
    server_data.anim_info.size      = remain_bytes;
    server_data.anim_info.offset    = position;

    if(handle->cb_func.play != NULL) {
      handle->cb_func.play(CMX_ANIM_INFO, handle->client_data, &server_data);
    }

  }
#ifdef FEATURE_SAF
  else if(type == CMX_AF_ANIM_SAF) {
    cmx_saf_offset     = position;
    cmx_saf_buffer     = buffer;
    cmx_saf_buf_bytes  = buf_len;
    cmx_saf_size       = buf_len + remain_bytes;

    if(cmx_saf_status != CMX_SAF_STATUS_STOPPED) {
      graph_saf_decode_abort();
    }

    (void) rex_clr_timer( &cmx_saf_frame_timer );
#ifdef FEATURE_PNG_DECODER
    if(cmx_png_audfmt_current) {
      cmx_saf_delay_handle = handle;
    } else
#endif /* FEATURE_PNG_DECODER */
    {
      if(handle->saf_obj_buf != NULL) {
        CMX_FREE(handle->saf_obj_buf);
        handle->saf_obj_buf = NULL;
      }

      if(handle->saf_obj_buf_size != 0) {
        handle->saf_obj_buf = (uint8 *) CMX_MALLOC(handle->saf_obj_buf_size);
      }

      if(handle->saf_obj_buf != NULL) {
        cmx_saf_init(handle->saf_obj_buf, handle->saf_obj_buf_size,
                     handle->saf_buffer.buffer);
        MSG_HIGH("cmx_audfmt_anim_data_processing: graph_saf_decode",0,0,0);
        graph_saf_decode(handle,
                         (saf_cmx_decode_cb_func_ptr_type) cmx_saf_callback);
        cmx_saf_handle_ptr          = NULL;
        cmx_saf_frame_cb            = NULL;
        cmx_saf_pending_handle_ptr  = NULL;
        cmx_saf_status              = CMX_SAF_STATUS_PREPARING;
        cmx_saf_audfmt_current      = TRUE;
        cmx_saf_audfmt_frame_status = CMX_SAF_FRAME_NONE;
        cmx_saf_audfmt_frame_id     = 0;
      } else {
        MSG_ERROR("Failed to allocate memory", 0,0,0);
      }
    }
  }
#endif
#ifdef FEATURE_SVG
  else if(type == CMX_AF_ANIM_SVG) {
    cmx_svg_offset = position;
    cmx_svg_size   = remain_bytes;
    if(handle->svg_data == NULL) {
      handle->svg_data = CMX_MALLOC(sizeof(cmx_svg_data_type));

      MSG_HIGH("cmx_audfmt_anim_data_processing:svg_data: %x",handle->svg_data,0,0);
      if(handle->svg_data != NULL) {
        memset((void*)handle->svg_data, 0, sizeof(cmx_svg_data_type));
        handle->svg_data->mem_buffer = CMX_MALLOC(CMX_SVG_MEM_BUFFER_SIZE);

        if(handle->svg_data->mem_buffer != NULL) {
          svg_load_data.svg_frame_width  = cmx_af_screen_width;
          svg_load_data.svg_frame_height = cmx_af_screen_height;
          svg_load_data.dpi              = 72;
          svg_load_data.transparency     = 0;
          svg_load_data.bk_color         = -1;

          svg_load_data.cb_func          = (svg_cmx_cb_type) cmx_svg_callback;
          svg_load_data.cb_data          = (void*) handle;
          svg_load_data.mem_buffer       = handle->svg_data->mem_buffer;
          svg_load_data.mem_buffer_size  = CMX_SVG_MEM_BUFFER_SIZE;
          svg_load_data.max_requests     = 0;
          svg_load_data.filename         = 0;
          graph_svg_decode(&svg_load_data);
        } else {
          CMX_FREE(handle->svg_data);
          MSG_HIGH("cmx_audfmt_anim_data_processing:svg_data NULL",0,0,0);
          handle->svg_data = NULL;
        }
      }
    }
  }
#endif /* FEATURE_SVG */
} /* end cmx_audfmt_anim_data_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_anim_frame_processing

DESCRIPTION
  This function sets up the appropriate mechanism to initiate SAF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_anim_frame_processing (
  cmx_af_anim_enum_type   type,
  uint8                   *buffer,
  uint32                  buf_len,
  cmx_af_handle_type      *handle,
  uint32                  position,
  uint32                  remain_bytes,
  cmx_af_pos_struct_type  *frame_pos
    /*lint -esym(715,buffer) Tell Lint we know buffer parameter is not used */
    /*lint -esym(715,buf_len) Tell Lint we know buf_len parameter is not used */
) {
  cmx_af_server_data_type  server_data;

  if(handle->cb_type == CMX_AF_CALLBACK_REFERENCE) {
    server_data.anim_info.anim_type = type;
    server_data.anim_info.anim_cmd  = CMX_AF_ANIM_CMD_FRAME;
    server_data.anim_info.size      = remain_bytes;
    server_data.anim_info.offset    = position;
    server_data.anim_info.frame_pos = *frame_pos;

    if(handle->cb_func.play != NULL) {
      handle->cb_func.play(CMX_ANIM_INFO, handle->client_data, &server_data);
    }

  } else if(type == CMX_AF_ANIM_SAF) {
#ifdef FEATURE_SAF
    /* This command is not yet supported */
#endif
  }
} /* end cmx_audfmt_anim_frame_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_anim_frame_id_processing

DESCRIPTION
  This function sets up the appropriate mechanism to initiate SAF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_anim_frame_id_processing (
  cmx_af_anim_enum_type   type,
  cmx_af_handle_type      *handle,
  uint32                  frame_id,
  cmx_af_pos_struct_type  *frame_pos
) {
  cmx_af_server_data_type  server_data;

  if(handle->cb_type == CMX_AF_CALLBACK_REFERENCE) {
    server_data.anim_info.anim_type = type;
    server_data.anim_info.anim_cmd  = CMX_AF_ANIM_CMD_FRAME_ID;
    server_data.anim_info.size      = frame_id;
    server_data.anim_info.frame_pos = *frame_pos;

    if(handle->cb_func.play != NULL) {
      handle->cb_func.play(CMX_ANIM_INFO, handle->client_data, &server_data);
    }

  } else if(type == CMX_AF_ANIM_SAF) {
#ifdef FEATURE_SAF
    if(cmx_saf_status == CMX_SAF_STATUS_PLAYING) {
      if (cmx_saf_audfmt_frame_status == CMX_SAF_FRAME_NONE) {
        if((cmx_saf_frame_cb != NULL) && (handle->saf_buffer.buffer != NULL)) {
          cmx_saf_audfmt_frame_pos = *frame_pos;
          cmx_saf_frame_cb((int32) frame_id, handle->saf_buffer.buffer,
                           cmx_saf_frame_client_data);
          cmx_saf_audfmt_frame_status = CMX_SAF_FRAME_PROC;
        }
      } else {
        cmx_saf_audfmt_frame_pos_pending = *frame_pos;
        cmx_saf_audfmt_frame_id          = frame_id;
        cmx_saf_audfmt_frame_status      = CMX_SAF_FRAME_PEND;
      }
    } else if (cmx_saf_status == CMX_SAF_STATUS_PREPARING) {
      cmx_saf_audfmt_frame_pos_pending = *frame_pos;
      cmx_saf_audfmt_frame_id          = frame_id;
      cmx_saf_audfmt_frame_status      = CMX_SAF_FRAME_PEND;
    }
#endif
  }
#ifdef FEATURE_SVG
  else if(type == CMX_AF_ANIM_SVG) {
    cmx_af_svg_frame_cmd_processing (handle, frame_id, frame_pos);
  }
#endif /* FEATURE_SVG */
} /* end cmx_audfmt_anim_frame_id_processing */

/* <EJECT> */
#ifdef FEATURE_SMAF
/*===========================================================================

FUNCTION cmx_audfmt_smaf_graph_info_processing

DESCRIPTION
  This function calls back the client to indicate graphics track chunk information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_smaf_graph_info_processing (
  cmx_af_handle_type       *handle,
  cmx_smaf_graph_info_type graph_info
) {
  cmx_af_server_data_type server_data;

  server_data.smaf_graph_info = graph_info;
  if (handle->cb_func.play != NULL) {
    handle->cb_func.play(CMX_SMAF_GRAPH_INFO, handle->client_data,
                         &server_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_smaf_led_ctl_processing

DESCRIPTION
  This function calls back the client to indicate LED control information
  specific to SMAF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_smaf_led_ctl_processing (
  cmx_af_handle_type     *handle,
  cmx_smaf_led_ctl_status_enum_type smaf_led_ctl_status
) {
  cmx_af_server_data_type server_data;

  server_data.smaf_led_ctl_status = smaf_led_ctl_status;
  if(handle->cb_func.play != NULL)
  {
     handle->cb_func.play(CMX_SMAF_LED_CONTROL, handle->client_data,
                          &server_data);
  }
}

/*===========================================================================

FUNCTION cmx_audfmt_smaf_vib_ctl_processing

DESCRIPTION
  This function calls back the client to indicate vibrator control information
  specific to SMAF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_smaf_vib_ctl_processing (
  cmx_af_handle_type     *handle,
  cmx_smaf_vib_ctl_status_enum_type smaf_vib_ctl_status
) {
  cmx_af_server_data_type server_data;

  server_data.smaf_vib_ctl_status = smaf_vib_ctl_status;
  if(handle->cb_func.play != NULL)
  {
      handle->cb_func.play(CMX_SMAF_VIB_CONTROL, handle->client_data,
                           &server_data);
  }
}
#endif /* FEATURE_SMAF */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_mfi_led_ctl_processing

DESCRIPTION
  This function calls back the client to indicate LED control information
  specific to MFi.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_mfi_led_ctl_processing (
  cmx_af_handle_type *handle,
  uint32             mask
) {
  cmx_af_server_data_type server_data;

  if(handle->cb_func.play != NULL) {
    server_data.mfi_led_ctl.led_ctl_mask = 0;

    if(mask & SND_CMX_AF_MFI_LED_MASK_TYPE1) {
      server_data.mfi_led_ctl.led_ctl_mask |= CMX_MFI_LED_MASK_TYPE1;
    }
    if(mask & SND_CMX_AF_MFI_LED_MASK_TYPE2) {
      server_data.mfi_led_ctl.led_ctl_mask |= CMX_MFI_LED_MASK_TYPE2;
    }
    if(mask & SND_CMX_AF_MFI_LED_MASK_TYPE3) {
      server_data.mfi_led_ctl.led_ctl_mask |= CMX_MFI_LED_MASK_TYPE3;
    }
    handle->cb_func.play(CMX_MFI_LED_CONTROL, handle->client_data,
                         &server_data);
  }
}
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#if defined(FEATURE_IMELODY) || defined(FEATURE_WEBAUDIO)
/*===========================================================================

FUNCTION cmx_audfmt_led_ctl_processing

DESCRIPTION
  This function calls back the client to indicate LED control information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_led_ctl_processing (
  cmx_af_handle_type         *handle,
  cmx_af_led_ctl_enum_type   led_ctl
) {
  cmx_af_server_data_type server_data;

  server_data.led_ctl = led_ctl;
  if(handle->cb_func.play != NULL) {
    handle->cb_func.play(CMX_LED_CONTROL, handle->client_data, &server_data);
  }
}
#endif /* FEATURE_IMELODY || FEATURE_WEBAUDIO */

/* <EJECT> */
#ifdef FEATURE_IMELODY
/*===========================================================================

FUNCTION cmx_audfmt_blight_ctl_processing

DESCRIPTION
  This function calls back the client to indicate Backlight control information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_blight_ctl_processing (
  cmx_af_handle_type         *handle,
  boolean                    backlight_ctl
) {
  cmx_af_server_data_type server_data;

  server_data.backlight_ctl = backlight_ctl;
  if(handle->cb_func.play != NULL) {
    handle->cb_func.play(CMX_BACKLIGHT_CONTROL,
                         handle->client_data, &server_data);
  }
}
#endif /* FEATURE_IMELODY */

/* <EJECT> */
#ifdef FEATURE_SMAF
/*===========================================================================

FUNCTION cmx_af_do_audio_spec_smaf

DESCRIPTION
  This function gets the requested data from the indicated SMAF file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
    None.

===========================================================================*/
void cmx_af_do_audio_spec_smaf(
  snd_cmx_af_spec_type  *snd_spec,
  cmx_af_smaf_spec_type *cmx_spec
) {
  snd_smaf_spec_type *smaf_spec = (snd_smaf_spec_type *)snd_spec;

  if((snd_spec != NULL) && (cmx_spec != NULL)) {
    memcpy(cmx_spec->title, smaf_spec->title, sizeof(cmx_spec->title));
    memcpy(cmx_spec->artist, smaf_spec->artist, sizeof(cmx_spec->artist));
    memcpy(cmx_spec->copyright, smaf_spec->copyright,
                                             sizeof(cmx_spec->copyright));

    cmx_spec->code_type     = snd_spec->smaf_spec.code_type;
    cmx_spec->supported_ops = (cmx_af_suprted_ops_mask_type)
                                        snd_spec->smaf_spec.supported_ops;
  }
}
#endif /* FEATURE_SMAF */

/* <EJECT> */
#ifdef FEATURE_MP3
/*===========================================================================

FUNCTION cmx_af_do_audio_spec_mp3

DESCRIPTION
  This function gets the requested data from the indicated MP3 file.

DEPENDENCIES
  cmx_png_buf_bytes
  cmx_png_buffer
  cmx_png_offset

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_image_data_cb_array

===========================================================================*/
void cmx_af_do_audio_spec_mp3(
  snd_cmx_af_spec_type    *snd_spec,
  cmx_af_mp3_spec_type    *cmx_spec
)
{
  int cnt = 0;
  snd_mp3_spec_type *mp3_spec = (snd_mp3_spec_type *)snd_spec;
  cmx_id3v2_tag_frame_type *cmx_tag_ptr = &(cmx_spec->v2_tag.title);
  snd_id3v2_tag_frame_type *snd_tag_ptr = &(mp3_spec->id3_tag.v2_tag.title);

  cmx_spec->version = (cmx_af_mp3_ver_enum_type)mp3_spec->version;
  cmx_spec->layer = (cmx_af_mpeg_layer_enum_type)mp3_spec->layer;
  cmx_spec->crc_f = (boolean)mp3_spec->crc_f;
  cmx_spec->bitrate = (cmx_af_mp3_bitrate_enum_type)mp3_spec->bitrate;
  cmx_spec->sample_rate = (cmx_af_sample_rate_enum_type)mp3_spec->sample_rate;
  cmx_spec->channel = (cmx_af_mp3_channel_enum_type)mp3_spec->channel;
  cmx_spec->extension = (cmx_af_mp3_ext_enum_type)mp3_spec->extension;
  cmx_spec->copyright_f = (boolean)mp3_spec->copyright_f;
  cmx_spec->original_f = (boolean)mp3_spec->original_f;
  cmx_spec->emphasis = (cmx_af_mp3_emphasis_enum_type)mp3_spec->emphasis;
  memcpy(cmx_spec->title, mp3_spec->id3_tag.v1_tag.title,
         sizeof(cmx_spec->title));
  memcpy(cmx_spec->artist, mp3_spec->id3_tag.v1_tag.artist,
         sizeof(cmx_spec->artist));
  memcpy(cmx_spec->album, mp3_spec->id3_tag.v1_tag.album,
         sizeof(cmx_spec->album));
  memcpy(cmx_spec->year, mp3_spec->id3_tag.v1_tag.year,
         sizeof(cmx_spec->year));
  memcpy(cmx_spec->comment, mp3_spec->id3_tag.v1_tag.comment,
         sizeof(cmx_spec->comment));
  cmx_spec->genre = (byte)mp3_spec->id3_tag.v1_tag.genre;

  /* Copy the ID3V2 content */
  cmx_spec->v2_tag_found = mp3_spec->id3_tag.v2_tag_found;
  cmx_spec->v2_tag.tag_ver = mp3_spec->id3_tag.v2_tag.tag_ver;

  for (cnt = 0; cnt < CMX_ID3V2_TAG_FRAME_TYPE_MAX;
         cnt++, cmx_tag_ptr++, snd_tag_ptr++) {

      if (snd_tag_ptr->content != NULL) {
        cmx_tag_ptr->content = malloc(snd_tag_ptr->size);
        if (cmx_tag_ptr->content == NULL) {
          break;
        }
      memcpy(cmx_tag_ptr->content,snd_tag_ptr->content,
               snd_tag_ptr->size);
        cmx_tag_ptr->size = snd_tag_ptr->size;
      }
    } /* for */

  /* copy the APIC frame Information */
  cmx_spec->v2_tag.apic.found = mp3_spec->id3_tag.v2_tag.apic.found;
  cmx_spec->v2_tag.apic.flags = mp3_spec->id3_tag.v2_tag.apic.flags;
  cmx_spec->v2_tag.apic.file_position =
                          mp3_spec->id3_tag.v2_tag.apic.file_position;
  cmx_spec->v2_tag.apic.size = mp3_spec->id3_tag.v2_tag.apic.size;

  /* copy the Generic frames information */
  cmx_spec->v2_tag.gframes =
      (cmx_id3v2_generic_frame_type*)mp3_spec->id3_tag.v2_tag.gframes;
  cmx_spec->v2_tag.gframes_cnt = mp3_spec->id3_tag.v2_tag.gframes_cnt;

  cmx_spec->supported_ops =
    (cmx_af_suprted_ops_mask_type) snd_spec->mp3_spec.supported_ops;
}
#endif /* FEATURE_MP3 */

/* <EJECT> */
#ifdef FEATURE_AAC
/*===========================================================================

FUNCTION cmx_af_do_audio_spec_aac

DESCRIPTION
  This function gets the requested data from the indicated AAC file.

DEPENDENCIES
  cmx_png_buf_bytes
  cmx_png_buffer
  cmx_png_offset

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_image_data_cb_array

===========================================================================*/
void cmx_af_do_audio_spec_aac(
  snd_cmx_af_spec_type    *snd_spec,
  cmx_af_aac_spec_type    *cmx_spec
)
{
  snd_aac_spec_type *aac_spec = (snd_aac_spec_type *)snd_spec;

  cmx_spec->layer = (cmx_af_mpeg_layer_enum_type)aac_spec->layer;
  cmx_spec->crc_f = aac_spec->crc_f;
  cmx_spec->audio_object = (cmx_af_mp4_audio_object_type)aac_spec->audio_object;
  cmx_spec->sample_rate = (cmx_af_sample_rate_enum_type)aac_spec->sample_rate;
  cmx_spec->private_f = aac_spec->private_f;
  cmx_spec->channel = (cmx_af_aac_channel_enum_type)aac_spec->channel;
  cmx_spec->original_f = aac_spec->original_f;
  cmx_spec->home_f = aac_spec->home_f;
  cmx_spec->bit_rate = aac_spec->bit_rate;
  memcpy(cmx_spec->title, aac_spec->title, sizeof(cmx_spec->title));
  memcpy(cmx_spec->artist, aac_spec->artist, sizeof(cmx_spec->artist));
  memcpy(cmx_spec->album, aac_spec->album, sizeof(cmx_spec->album));
  memcpy(cmx_spec->year, aac_spec->year, sizeof(cmx_spec->year));
  memcpy(cmx_spec->comment, aac_spec->comment, sizeof(cmx_spec->comment));
  cmx_spec->genre = (byte)aac_spec->genre;
  cmx_spec->supported_ops =
    (cmx_af_suprted_ops_mask_type) snd_spec->aac_spec.supported_ops;
}
#endif /* FEATURE_AAC */

/* <EJECT> */
#ifdef FEATURE_IMELODY
/*===========================================================================

FUNCTION cmx_af_do_audio_spec_imy

DESCRIPTION
  This function gets the requested data from the indicated IMY file.

DEPENDENCIES
  cmx_png_buf_bytes
  cmx_png_buffer
  cmx_png_offset

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_image_data_cb_array

===========================================================================*/
void cmx_af_do_audio_spec_imy(
  snd_cmx_af_spec_type    *snd_spec,
  cmx_af_imy_spec_type    *cmx_spec
)
{
  snd_imy_spec_type *imy_spec = (snd_imy_spec_type *)snd_spec;

  memcpy(cmx_spec->name, imy_spec->name, sizeof(cmx_spec->name));
  memcpy(cmx_spec->composer, imy_spec->composer, sizeof(cmx_spec->composer));
  cmx_spec->supported_ops =
    (cmx_af_suprted_ops_mask_type) snd_spec->imy_spec.supported_ops;
}
#endif /* FEATURE_IMELODY */

/* <EJECT> */
#ifdef FEATURE_WAVE_PB
/*===========================================================================

FUNCTION cmx_af_do_audio_spec_wave

DESCRIPTION
  This function processes the specifications for ADPCM/PCM format and
  reserves a midi_out_qcp channel for ADPCM/PCM playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_af_do_audio_spec_wave (
  snd_cmx_af_spec_type    *snd_spec,
  cmx_af_wave_spec_type   *cmx_spec,
  cmx_af_wave_format_type wave_format
) {
#ifdef FEATURE_MIDI_OUT_QCP
  cmx_packet_type *packet;
#endif /* FEATURE_MIDI_OUT_QCP */
  snd_wave_spec_type *wave_spec = (snd_wave_spec_type *) snd_spec;

  cmx_spec->file_type       = CMX_AF_FILE_WAVE;
  cmx_spec->channels        = wave_spec->channels;
  cmx_spec->sample_rate     = wave_spec->sample_rate;
  cmx_spec->bytes_rate      = wave_spec->bytes_rate;
  cmx_spec->frame_size      = wave_spec->frame_size;
  cmx_spec->bits_per_sample = wave_spec->bits_per_sample;
  cmx_spec->format          = wave_format;
  cmx_spec->supported_ops =
    (cmx_af_suprted_ops_mask_type) snd_spec->wave_spec.supported_ops;
#ifdef FEATURE_MIDI_OUT_QCP
  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd = CMX_MIDI_OUT_QCP_RESERVE_CHANNEL;
    cmx_cmd (packet);
  }
#endif /* FEATURE_MIDI_OUT_QCP */
}
#endif /* FEATURE_WAVE_PB */

/* <EJECT> */
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION cmx_qcp_func_done

DESCRIPTION
  This function cleans up after a QCP get/set comamnd.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_func_done (
  cmx_status_type      status,
  cmx_af_handle_type   *handle,
  uint8                *buffer,
  uint32               num_bytes
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type rsp;
#endif

  if(handle != NULL) {
#ifdef FEATURE_EFS
    if(handle->source_info.source == CMX_SOURCE_EFS) {
      fs_close(handle->source_info.handle.file, NULL, &rsp);
    }
#endif /* FEATURE_EFS */
    if((handle->cmd == CMX_QCP_GET_LABL) ||
       (handle->cmd == CMX_QCP_GET_TEXT) ||
       (handle->cmd == CMX_QCP_GET_FRAMES) ||
       (handle->cmd == CMX_QCP_GET_DATA_SIZE)) {
      *cmx_qcp_func_length = num_bytes;
    } else if (handle->cmd == CMX_QCP_GET_CNFG) {
      *((uint16 *) cmx_qcp_func_length) = num_bytes;
    }
    if(handle->cb_func.base != NULL) {
      handle->cb_func.base(status, handle->client_data);
    }
  }

  cmx_free_af_handle(handle);

  /* clear buffer in use flags */
  cmx_qcp_func_length = NULL;
  cmx_qcp_func_buffer = NULL;
  cmx_qcp_func_active = FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_rec_get_buf_processing

DESCRIPTION
  This function allocates a buffer for QCP recording and returns it to
  the requestor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_qcp_rec_get_buf_processing (
  cmx_af_handle_type       *handle,
  snd_qcp_buf_cb_func_type cb_func
) {
  uint32 space_left = 0;
  uint8  *buffer;

  if((handle->source_info.source == CMX_SOURCE_MEM) ||
     (handle->source_info.source == CMX_SOURCE_CLIENT)) {
    space_left = handle->source_info.cmx_len - CMX_QCP_MEM_OVERHEAD;
  }
#ifdef FEATURE_EFS
  else if(handle->source_info.source == CMX_SOURCE_EFS) {
    space_left = cmx_fs_space_available_using_handle(handle);
    if(space_left <= CMX_QCP_FS_OVERHEAD) {
      space_left  = 0;
    } else {
      space_left -= CMX_QCP_FS_OVERHEAD;
    }
  }
#endif /* FEATURE_EFS */

  if(!cmx_qcp_rec_warning_done) {
    if(space_left <= CMX_QCP_RECORD_WARNING_SIZE) {
      if(handle->cb_func.base != NULL) {
        handle->cb_func.base(CMX_QCP_SPACE_WARNING, handle->client_data);
      }
      cmx_qcp_rec_warning_done = TRUE;
    }
  }

    if(space_left > 0){
    if((buffer = cmx_af_get_buff(handle->buffer)) != NULL) {
      space_left = MIN(space_left,
                       cmx_af_buf_info[handle->buffer->buf_type].num_bytes);
      if((handle->source_info.source == CMX_SOURCE_MEM) ||
         (handle->source_info.source == CMX_SOURCE_CLIENT)) {
        handle->source_info.cmx_len -= space_left;
      }
      if(cb_func != NULL) {
        cb_func(buffer, space_left);
      }
    } else if(cmx_qcp_rec_get_buf_pending == NULL) {
      cmx_qcp_rec_get_buf_pending = cb_func;
      cmx_qcp_rec_buf_handle      = handle;
    } else if(cmx_qcp_rec_get_buf_pending_res == NULL) {
      cmx_qcp_rec_get_buf_pending_res = cb_func;
      cmx_qcp_rec_buf_handle_res  = handle;
    } else {
      MSG_ERROR("Too many pending buffer gets!!!",0,0,0);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_rec_data_processing

DESCRIPTION
  This function saves the recorded QCP data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_qcp_rec_data_processing (
  cmx_af_handle_type                *handle,
  uint8                             *buffer,
  uint32                             num_bytes,
  uint32                             position,
  cmx_qcp_dir_type                   link,
  cmx_qcp_rec_data_buffer_enum_type  content
) {
#ifdef FEATURE_EFS
  uint32 entry;
#endif /* FEATURE_EFS */
  snd_qcp_buf_cb_func_type cb_func;

  if(handle != NULL) {
    if((handle->cmd == CMX_QCP_SET_CNFG) || (handle->cmd == CMX_QCP_SET_LABL)
       || (handle->cmd == CMX_QCP_SET_TEXT)) {
      link = CMX_QCP_DIR_REVERSE;
    }

    if (content == CMX_QCP_REC_DATA_BUFFER_RECDATA) {
      if (link == CMX_QCP_DIR_REVERSE) {
        handle->status_mask |= CMX_AF_STATUS_MASK_TX_RECORDED;
      } else {
        handle->status_mask |= CMX_AF_STATUS_MASK_RX_RECORDED;
      }
    }

    if(handle->source_info.source == CMX_SOURCE_MEM) {
      if(link == CMX_QCP_DIR_REVERSE) {
        memcpy(&handle->source_info.handle.buf[position], buffer, num_bytes);
      } else {
        memcpy(&handle->source_info.support.buf[position], buffer, num_bytes);
      }
      cmx_af_free_buff ( handle->buffer, buffer );
      if(cmx_qcp_rec_get_buf_pending != NULL) {
        cb_func = cmx_qcp_rec_get_buf_pending;
        cmx_qcp_rec_get_buf_pending     = cmx_qcp_rec_get_buf_pending_res;
        cmx_qcp_rec_get_buf_pending_res = NULL;
        cmx_qcp_rec_buf_handle          = cmx_qcp_rec_buf_handle_res;
        cmx_qcp_rec_buf_handle_res      = NULL;
        cmx_qcp_rec_get_buf_processing (handle, cb_func);
      }
    } else if(handle->source_info.source == CMX_SOURCE_CLIENT) {
      entry = cmx_qcp_rec_write_cb_array.index +
                                 cmx_qcp_rec_write_cb_array.number;
      if(entry >= MAX_NUM_DATA_WRITE) {
        entry -= MAX_NUM_DATA_WRITE;
      }
      cmx_qcp_rec_write_cb_array.data_cb[entry].handle = handle;
      cmx_qcp_rec_write_cb_array.data_cb[entry].buffer = buffer;
      cmx_qcp_rec_write_cb_array.number++;
      if(link == CMX_QCP_DIR_REVERSE) {
        handle->source_info.handle.data_fn(position, num_bytes,
                                           cmx_af_client_write_cb,
                                           (const void *) buffer,
                                           handle->client_data );
      } else {
        handle->source_info.support.data_fn(position, num_bytes,
                                            cmx_af_client_write_cb,
                                            (const void *) buffer,
                                            handle->client_data );
      }
    }
#ifdef FEATURE_EFS
    else if(handle->source_info.source == CMX_SOURCE_EFS) {
      if(cmx_qcp_rec_write_cb_array.number < MAX_NUM_DATA_WRITE) {
        entry = cmx_qcp_rec_write_cb_array.index +
                                   cmx_qcp_rec_write_cb_array.number;
        if(entry >= MAX_NUM_DATA_WRITE) {
          entry -= MAX_NUM_DATA_WRITE;
        }
        cmx_qcp_rec_write_cb_array.data_cb[entry].handle = handle;
        cmx_qcp_rec_write_cb_array.data_cb[entry].buffer = buffer;
        cmx_qcp_rec_write_cb_array.number++;
        if(link == CMX_QCP_DIR_REVERSE) {
          fs_seek_write(handle->source_info.handle.file, FS_SEEK_SET,
                        (fs_file_position_type) position, buffer, num_bytes,
                        (fs_callback_p_type) cmx_qcp_rec_write_cb,
                        &(cmx_qcp_rec_write_cb_array.data_cb[entry].rsp));
        } else {
          fs_seek_write(handle->source_info.support.file, FS_SEEK_SET,
                        (fs_file_position_type) position, buffer, num_bytes,
                        (fs_callback_p_type) cmx_qcp_rec_write_cb,
                        &(cmx_qcp_rec_write_cb_array.data_cb[entry].rsp));
        }
      }
    }
#endif /* FEATURE_EFS */
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_rec_done_processing

DESCRIPTION
  This function cleans up the QCP recording state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_qcp_rec_done_processing (
  cmx_cmd_type             cmd,
  cmx_af_handle_type       *handle_ptr
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type rsp;
#endif
  snd_af_access_type *access_ptr;

  if(cmx_qcp_set_labl_pend) {
    handle_ptr->cmd        = CMX_QCP_SET_LABL;
    access_ptr             = &handle_ptr->sound.audfmt_sound.audfmt_handle;
    access_ptr->audfmt_ptr = (void *) handle_ptr;
    access_ptr->data_func  = (snd_af_data_cb_func_type) cmx_af_play_callback;

    cmx_qcp_record_status_pending = cmd;
    if(handle_ptr->source_info.handle.buf == NULL) {
      handle_ptr->source_info.handle = handle_ptr->source_info.support;
    }
    snd_qcp_set_func(SND_QCP_FUNC_LABL, cmx_qcp_labl_pending_buffer,
                     cmx_qcp_labl_pending_length, access_ptr,
                     (snd_qcp_rec_cb_func_ptr_type) cmx_qcp_set_func_cb,
                     (void *) handle_ptr);
    return;
  }

  if(handle_ptr != NULL) {
#ifdef FEATURE_EFS
    if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
      fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
      fs_close(handle_ptr->source_info.support.file, NULL, &rsp);

      if (handle_ptr->rec_tx_handle != NULL) {
        if (!(handle_ptr->status_mask & CMX_AF_STATUS_MASK_TX_RECORDED) &&
            (handle_ptr->rec_tx_handle->source_type == CMX_SOURCE_EFS)) {
          fs_remove(handle_ptr->rec_tx_handle->efs.filename, NULL, &rsp);
        }
        cmx_free_handle(handle_ptr->rec_tx_handle);
      }
      if (handle_ptr->rec_rx_handle != NULL) {
        if (!(handle_ptr->status_mask & CMX_AF_STATUS_MASK_RX_RECORDED) &&
            (handle_ptr->rec_rx_handle->source_type == CMX_SOURCE_EFS)) {
          fs_remove(handle_ptr->rec_rx_handle->efs.filename, NULL, &rsp);
        }
        cmx_free_handle(handle_ptr->rec_rx_handle);
      }
    }
#endif /* FEATURE_EFS */
    if(handle_ptr->cb_func.base != NULL) {
      if(cmd == CMX_QCP_CMD_REC_EFS_DONE) {
        handle_ptr->cb_func.base(CMX_QCP_SPACE_DONE, handle_ptr->client_data);
      } else if(cmd == CMX_QCP_CMD_REC_AUTO_STOP) {
        handle_ptr->cb_func.base(CMX_QCP_REC_AUTO_STOP,
                                                  handle_ptr->client_data);
      } else if(cmd == CMX_QCP_CMD_REC_ERROR) {
        handle_ptr->cb_func.base(CMX_ERROR, handle_ptr->client_data);
      } else {
        handle_ptr->cb_func.base(CMX_DONE, handle_ptr->client_data);
      }
    }

    if(handle_ptr->cmd == CMX_QCP_SET_LABL) {
      cmx_qcp_func_length = NULL;
      cmx_qcp_func_buffer = NULL;
      cmx_qcp_func_active = FALSE;
    }
    cmx_free_af_handle(handle_ptr);
  }

  /* clear buffer in use flags */
  cmx_qcp_rec_write_cb_array.number   = 0;
  cmx_qcp_rec_get_buf_pending         = NULL;
  cmx_qcp_rec_get_buf_pending_res     = NULL;
  cmx_qcp_rec_buf_handle              = NULL;
  cmx_qcp_rec_buf_handle_res          = NULL;
  cmx_af_active                       = CMX_AF_NONE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_rec_status_processing

DESCRIPTION
  This function returns recording status to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_qcp_rec_status_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
) {
  if((handle_ptr != NULL) && (handle_ptr->cb_func.base != NULL)) {
    handle_ptr->cb_func.base(status, handle_ptr->client_data);
  }
}
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION cmx_pcm_record_data_processing

DESCRIPTION
  This function records the buffer of PCM data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_pcm_record_data_processing (
  cmx_cmd_type              cmd,
  cmx_af_handle_type        *handle,
  uint8                     *buffer,
  uint32                    num_bytes,
  uint32                    position,
  snd_pcm_buf_cb_func_type  buf_cb,
  const void                *client_data
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type rsp;
#endif
  uint32          tlong, space_left = 0;
  boolean         do_buf = FALSE, rec_done = FALSE;
  uint8           *buf = NULL;

  if(handle != NULL) {
    if((cmd == CMX_PCM_CMD_REC_DATA) && (buf_cb != NULL)) {
      if(buffer == handle->buf1) {
        switch(handle->source_info.source) {
          case CMX_SOURCE_MEM:
          case CMX_SOURCE_CLIENT:
            tlong      = position + num_bytes + CMX_PCM_REC_BUF_LENGTH;
            space_left = handle->source_info.cmx_len;
            if(space_left > tlong) {
              space_left -= tlong;
            } else {
              space_left  = 0;
            }
            rec_done = (handle->source_info.cmx_len <=
                                       (position + CMX_PCM_REC_BUF_LENGTH));
            break;

#ifdef FEATURE_EFS
          case CMX_SOURCE_EFS:
            tlong      = CMX_PCM_FS_OVERHEAD + num_bytes +
                         CMX_PCM_REC_BUF_LENGTH;
            space_left = cmx_fs_space_available_using_handle(handle);

            if(space_left > tlong) {
              space_left -= tlong;
            } else {
              space_left = 0;
              rec_done   = TRUE;
            }
            break;
#endif /* FEATURE_EFS */
        }

        if((!(handle->status_mask & CMX_AF_STATUS_MASK_WARNING_SENT)) &&
           (space_left < CMX_PCM_RECORD_WARNING_SIZE)) {
          handle->status_mask |= CMX_AF_STATUS_MASK_WARNING_SENT;
          if(handle->cb_func.base != NULL) {
            handle->cb_func.base(CMX_PCM_SPACE_WARNING, handle->client_data);
          }
        }

        if(space_left != 0) {
          tlong = MIN(CMX_PCM_REC_BUF_LENGTH, space_left);
          buf = (uint8 *) CMX_MALLOC(tlong);
          if(buf != NULL) {
            handle->buf1 = handle->buf2;
            handle->buf2 = buf;
            buf_cb(buf, tlong, client_data);
          } else {
            MSG_ERROR("Failed to allocate memory", 0,0,0);
            do_buf = TRUE;
          }
        } else {
          handle->buf1 = handle->buf2;
          handle->buf2 = NULL;
        }
      }
    } else if(buffer == handle->buf1) {
      handle->buf1 = handle->buf2;
      handle->buf2 = NULL;
    }

    if(handle->source_info.source == CMX_SOURCE_MEM) {
      memcpy(&handle->source_info.handle.buf[position], buffer, num_bytes);
    } else if(handle->source_info.source == CMX_SOURCE_CLIENT) {
      handle->source_info.handle.data_fn(position, num_bytes,
                                         NULL, (const void *) buffer,
                                         handle->client_data );
    }
#ifdef FEATURE_EFS
    else if(handle->source_info.source == CMX_SOURCE_EFS) {
      fs_seek_write(handle->source_info.handle.file, FS_SEEK_SET,
                    (fs_file_position_type) position, buffer, num_bytes,
                    (fs_callback_p_type) NULL, &rsp);

      if(rsp.seek_write.status != FS_OKAY_S) {
        rec_done = TRUE;
      }
    }
#endif /* FEATURE_EFS */

    if(rec_done) {
      CMX_FREE(buffer);
      if(handle->link == CMX_PCM_REC_LINK_REVERSE) {
        snd_pcm_record_stop(SND_PCM_REC_DIR_REVERSE, NULL, NULL);
      }else if(handle->link == CMX_PCM_REC_LINK_BOTH) {
        snd_pcm_record_stop(SND_PCM_REC_DIR_BOTH, NULL, NULL);
      }else {
        snd_pcm_record_stop(SND_PCM_REC_DIR_FORWARD, NULL, NULL);
      }
    } else if(do_buf) {
      handle->buf1 = handle->buf2;
      handle->buf2 = buffer;
      buf_cb(buffer, CMX_PCM_REC_BUF_LENGTH, client_data);
    } else if(cmd == CMX_PCM_CMD_REC_DATA) {
      CMX_FREE(buffer);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_done_processing

DESCRIPTION
  This function cleans up the PCM recording state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_pcm_record_done_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type rsp;
#endif

  if(handle_ptr != NULL) {
#ifdef FEATURE_EFS
    if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
      fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
    }

    if (handle_ptr->link == CMX_PCM_REC_LINK_REVERSE &&
        handle_ptr->rec_tx_handle != NULL) {
      cmx_free_handle(handle_ptr->rec_tx_handle);
    }
    if (handle_ptr->link == CMX_PCM_REC_LINK_FORWARD &&
        handle_ptr->rec_rx_handle != NULL) {
      cmx_free_handle(handle_ptr->rec_rx_handle);
    }
    if (handle_ptr->link == CMX_PCM_REC_LINK_BOTH&&
        handle_ptr->rec_rx_handle != NULL) {
      cmx_free_handle(handle_ptr->rec_rx_handle);
    }
#endif /* FEATURE_EFS */

    if(handle_ptr->buf1 != NULL) {
      CMX_FREE(handle_ptr->buf1);
    }
    if(handle_ptr->buf2 != NULL) {
      CMX_FREE(handle_ptr->buf2);
    }

    if(handle_ptr->cb_func.base != NULL) {
      handle_ptr->cb_func.base(status, handle_ptr->client_data);
    }
    cmx_free_af_handle(handle_ptr);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_status_processing

DESCRIPTION
  This function returns recording status to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_pcm_record_status_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
) {
  if((handle_ptr != NULL) && (handle_ptr->cmd == CMX_PCM_CMD_REC) &&
     (handle_ptr->cb_func.base != NULL)) {
    handle_ptr->cb_func.base(status, handle_ptr->client_data);
  }
}
#endif /* FEATURE_PCM_REC */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_snd_play_cb

DESCRIPTION
  This function is used as the midi/WebAudio sound server playback callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_af_snd_play_cb( void *client_data, snd_status_type status )
{
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                = CMX_AF_CMD_CB;
    packet->audfmt_cb.status       = status;
    packet->audfmt_cb.client_data  = client_data;
    cmx_cmd (packet);
  }
} /* cmx_af_snd_play_cb */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION cmx_mm_record_data_processing

DESCRIPTION
  This function records the buffer of MultiMedia data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_mm_record_data_processing (
  cmx_cmd_type              cmd,
  cmx_af_handle_type        *handle,
  uint8                     *buffer,
  uint32                    num_bytes,
  uint32                    position,
  snd_mm_buf_cb_func_type   buf_cb,
  const void                *client_data
)
{
#ifdef FEATURE_EFS
  fs_rsp_msg_type rsp;
#endif
  uint32          tlong, space_left = 0;
  boolean         rec_done = FALSE;

  if (handle != NULL) {

    if ((cmd == CMX_MM_CMD_REC_DATA) && (buf_cb != NULL)) {

      if (buffer == handle->buf1) {

        switch (handle->source_info.source) {

          case CMX_SOURCE_MEM:
          case CMX_SOURCE_CLIENT: {
            tlong      = position + num_bytes + CMX_MM_REC_BUF_LENGTH;
            space_left = handle->source_info.cmx_len;

            if (space_left > tlong) {
              space_left -= tlong;
            } else {
              space_left  = 0;
            }

            rec_done = (handle->source_info.cmx_len <=
                                       (position + CMX_MM_REC_BUF_LENGTH));
          }
            break;

#ifdef FEATURE_EFS
          case CMX_SOURCE_EFS: {

            tlong      = num_bytes + CMX_MM_REC_BUF_LENGTH;

            space_left = cmx_fs_space_available_using_handle(handle);

            if (space_left > tlong) {

              space_left -= tlong;

            } else {

              space_left = 0;
              rec_done   = TRUE;

            }
          }
            break;
#endif /* FEATURE_EFS */
        } /* switch (handle->source_info.source) */

        if ((!(handle->status_mask & CMX_AF_STATUS_MASK_WARNING_SENT)) &&
            (space_left < CMX_MM_RECORD_WARNING_SIZE)) {

          handle->status_mask |= CMX_AF_STATUS_MASK_WARNING_SENT;
          if(handle->cb_func.base != NULL) {
            handle->cb_func.base(CMX_MM_SPACE_WARNING, handle->client_data);
          }
        }
      }

    }

    if (handle->source_info.source == CMX_SOURCE_MEM) {

      memcpy(&handle->source_info.handle.buf[position],
             buffer,
             num_bytes);

    } else if (handle->source_info.source == CMX_SOURCE_CLIENT) {

      handle->source_info.handle.data_fn(position,
                                         num_bytes,
                                         NULL,
                                         (const void *)buffer,
                                         handle->client_data );
    }
#ifdef FEATURE_EFS
    else if (handle->source_info.source == CMX_SOURCE_EFS) {

      fs_seek_write(handle->source_info.handle.file,
                    FS_SEEK_SET,
                    (fs_file_position_type)position,
                    buffer,
                    num_bytes,
                    (fs_callback_p_type) NULL,
                    &rsp);

      if (rsp.seek_write.status != FS_OKAY_S) {

        rec_done = TRUE;

      }

    } /* else if (handle->source_info.source == CMX_SOURCE_EFS) */
#endif /* FEATURE_EFS */

    if (rec_done) {

      CMX_FREE(buffer);

      snd_mm_record_stop(SND_MM_REC_DIR_REVERSE, NULL, NULL);

    } else if (buf_cb != NULL) {

      if (space_left != 0) {

        tlong = MIN(CMX_MM_REC_BUF_LENGTH, space_left);

        handle->buf1 = handle->buf2;
        handle->buf2 = buffer;
        buf_cb(buffer, tlong, client_data);

      }
    } /* else */
  }
} /* cmx_mm_record_data_processing */

/*===========================================================================

FUNCTION cmx_mm_record_done_processing

DESCRIPTION
  This function cleans up the MultiMedia recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_mm_record_done_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
)
{
  if (handle_ptr != NULL) {

#ifdef FEATURE_EFS
    if (handle_ptr->source_info.source == CMX_SOURCE_EFS) {

      fs_rsp_msg_type rsp;

      fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);

      if (handle_ptr->rec_tx_handle != NULL) {
        cmx_free_handle(handle_ptr->rec_tx_handle);
      }
    }
#endif /* FEATURE_EFS */

    if (handle_ptr->buf1 != NULL) {
      CMX_FREE(handle_ptr->buf1);
    }

    if (handle_ptr->buf2 != NULL) {
      CMX_FREE(handle_ptr->buf2);
    }

    cmx_af_active = CMX_AF_NONE;

    if (handle_ptr->cb_func.base != NULL) {
      handle_ptr->cb_func.base(status, handle_ptr->client_data);
    }

    cmx_free_af_handle(handle_ptr);
  }
} /* cmx_mm_record_done_processing */

/*===========================================================================

FUNCTION cmx_mm_record_status_processing

DESCRIPTION
  This function returns MultiMedia recording status to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_mm_record_status_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
)
{
  if ((handle_ptr != NULL)                 &&
      (handle_ptr->cmd == CMX_MM_CMD_REC)  &&
      (handle_ptr->cb_func.base != NULL)) {

    handle_ptr->cb_func.base(status, handle_ptr->client_data);

  }
} /* cmx_mm_record_status_processing */
#endif /* FEATURE_MM_REC */
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
#ifdef FEATURE_CMX_IMAGE
/*===========================================================================

FUNCTION cmx_image_cb_processing

DESCRIPTION
  This function determines when the PNG decoder may continue decoding the
  current image.

DEPENDENCIES
  cmx_image_data_cb_array
  cmx_af_data_cb_array

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_image_data_cb_array

===========================================================================*/
void cmx_image_cb_processing (
  cmx_image_enum_type       image_type,
  cmx_image_command_cb_type cb_func,
  void                      *client_data
) {
  switch (image_type) {
#ifdef FEATURE_PNG_DECODER
    case CMX_IMAGE_TYPE_PNG:
      if(cb_func.png_cb != NULL) {
        cb_func.png_cb(client_data);
      }
      break;
#endif /* FEATURE_PNG_DECODER */

#ifdef FEATURE_SAF
    case CMX_IMAGE_TYPE_SAF:
      if(cb_func.saf_cb != NULL) {
        cb_func.saf_cb(client_data);
      }
      break;
#endif /* FEATURE_SAF */

#ifdef FEATURE_JPEG_DECODER
    case CMX_IMAGE_TYPE_JPEG:
      if(cb_func.jpeg_cb != NULL) {
        cb_func.jpeg_cb(client_data);
      }
      break;
#endif /* FEATURE_JPEG_DECODER */
#ifdef FEATURE_SVG
  case CMX_IMAGE_TYPE_SVG:
      if(cb_func.svg_cb != NULL){
        cb_func.svg_cb( client_data);
      }
      break;
#endif /*FEATURE_SVG */
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_image_continue_processing

DESCRIPTION
  This function determines when the PNG decoder may continue decoding the
  current image.

DEPENDENCIES
  cmx_image_data_cb_array
  cmx_af_data_cb_array

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_image_data_cb_array

===========================================================================*/
void cmx_image_continue_processing (
  cmx_image_enum_type       image_type,
  cmx_image_command_cb_type cb_func,
  void                      *client_data
) {
  if(cmx_image_data_cb_array.number != 0) {
    switch (image_type) {
#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_PNG_DECODER
      case CMX_IMAGE_TYPE_PNG:
        if(cmx_png_cont_cb_func_ptr != NULL) {
          cmx_png_cont_cb_func_ptr(cmx_png_cont_cb_data);
        }
        cmx_png_cont_cb_func_ptr = cb_func.png_cb;
        cmx_png_cont_cb_data     = client_data;
        break;
#endif /* FEATURE_PNG_DECODER */

#ifdef FEATURE_JPEG_DECODER
      case CMX_IMAGE_TYPE_JPEG:
        if(cmx_jpeg_cont_cb_func_ptr != NULL) {
          cmx_jpeg_cont_cb_func_ptr(cmx_jpeg_cont_cb_data);
        }
        cmx_jpeg_cont_cb_func_ptr = cb_func.jpeg_cb;
        cmx_jpeg_cont_cb_data     = client_data;
        break;
#endif /* FEATURE_JPEG_DECODER */
#endif /* FEATURE_WEBAUDIO */

#ifdef FEATURE_SAF
      case CMX_IMAGE_TYPE_SAF:
        if(cmx_saf_cont_cb_func_ptr != NULL) {
          cmx_saf_cont_cb_func_ptr(cmx_saf_cont_cb_data);
        }
        cmx_saf_cont_cb_func_ptr = cb_func.saf_cb;
        cmx_saf_cont_cb_data     = client_data;
        break;
#endif /* FEATURE_SAF */

      default:
        MSG_ERROR("Invalid image type: %d", image_type, 0, 0);
        return;
    }
  } else {
    cmx_image_cb_processing(image_type, cb_func, client_data);
  }
} /* end cmx_image_continue_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_image_read_processing

DESCRIPTION
  This function gets the requested data from the indicated PNG file.

DEPENDENCIES
  cmx_png_buf_bytes
  cmx_png_buffer
  cmx_png_offset

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_image_data_cb_array

===========================================================================*/
void cmx_image_read_processing (
  cmx_image_enum_type       image_type,
  cmx_af_handle_type        *handle,
  uint8                     *buffer,
  uint32                    num_bytes,
  uint32                    position,
  cmx_image_command_cb_type cb_func,
  void                      *client_data
) {

#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_EFS
  static fs_rsp_msg_type rsp;
  uint32 entry;
#endif
  cmx_image_read_cb_type *image_cb;
  uint32                 buf_bytes = 0;
#ifdef FEATURE_WEBAUDIO
  uint32                 offset = 0;
#endif
  uint8                  *buf_ptr = NULL;

  switch (image_type) {
#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_PNG_DECODER
    case CMX_IMAGE_TYPE_PNG:
      buf_bytes = cmx_png_buf_bytes;
      offset    = cmx_png_offset;
      buf_ptr   = cmx_png_buffer;
      break;
#endif /* FEATURE_PNG_DECODER */

#ifdef FEATURE_JPEG_DECODER
    case CMX_IMAGE_TYPE_JPEG:
      buf_bytes = cmx_jpeg_buf_bytes;
      offset    = cmx_jpeg_offset;
      buf_ptr   = cmx_jpeg_buffer;
      break;
#endif /* FEATURE_JPEG_DECODER */
#endif /* FEATURE_WEBAUDIO */

#ifdef FEATURE_SAF
    case CMX_IMAGE_TYPE_SAF:
      buf_bytes = cmx_saf_buf_bytes;
#ifdef FEATURE_WEBAUDIO
      offset    = cmx_saf_offset;
#endif
      buf_ptr   = cmx_saf_buffer;
      break;
#endif /* FEATURE_SAF */

#ifdef FEATURE_SVG
    case CMX_IMAGE_TYPE_SVG:
      buf_bytes  = 0;
#ifdef FEATURE_WEBAUDIO
      offset     = cmx_svg_offset;
#endif
      buf_ptr    = NULL;
      break;
#endif /* FEATURE_SVG */
  }

  if(cmx_image_data_cb_array.number != 0) {
    if(position < buf_bytes) {
      if((position + num_bytes) <= buf_bytes) {
        memcpy(buffer, &(buf_ptr[position]), num_bytes);
        cmx_image_continue_processing ( image_type, cb_func, client_data );
        return;
      } else {
        memcpy(buffer, &(buf_ptr[position]), buf_bytes - position);
        num_bytes -= (buf_bytes - position);
        buffer     = &(buffer[buf_bytes - position]);
        position   = buf_bytes;
      }
    }
  }

#ifdef FEATURE_WEBAUDIO
  if(handle->cmd == CMX_AF_CMD_PLAY) {
    position += offset - buf_bytes;
  }
#endif

  if(handle->source_info.source == CMX_SOURCE_MEM) {
    memcpy(buffer, &(handle->source_info.handle.buf[position]), num_bytes);
    cmx_image_continue_processing ( image_type, cb_func, client_data );
  } else if(handle->source_info.source == CMX_SOURCE_CLIENT) {
    if(cmx_image_data_cb_array.number < MAX_NUM_DATA_IMAGE_READ) {
      entry = cmx_image_data_cb_array.index + cmx_image_data_cb_array.number;
      if(entry >= MAX_NUM_DATA_IMAGE_READ) {
        entry -= MAX_NUM_DATA_IMAGE_READ;
      }

      image_cb              = &cmx_image_data_cb_array.image_read[entry];
      image_cb->image_type  = image_type;
      image_cb->cb_func     = cb_func;
      image_cb->client_data = client_data;
      image_cb->buffer      = buffer;
      cmx_image_data_cb_array.number++;

      /* Request client for more data */
      handle->source_info.handle.data_fn(position, num_bytes,
                                         cmx_image_client_read_cb,
                                         (const void *) image_cb,
                                         handle->client_data );
    } else {
      MSG_ERROR("Too many requests",0,0,0);
    }
  }
#ifdef FEATURE_EFS
  else if(handle->source_info.source == CMX_SOURCE_EFS) {
    fs_seek_read(handle->source_info.handle.file, FS_SEEK_SET,
                 (fs_file_position_type) position, buffer, num_bytes,
                 (fs_callback_p_type) NULL, &rsp);

    cmx_image_continue_processing ( image_type, cb_func, client_data );
  }
#endif
#endif /*FEATURE_WEBAUDIO*/

} /* end cmx_image_read_processing */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION cmx_image_buf_req_processing

DESCRIPTION
  This function will request a buffer from the client for image processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_image_buf_req_processing (
  cmx_image_enum_type       image_type,
  cmx_af_handle_type        *handle,
  uint8                     **buf_handle,
  uint32                    num_bytes,
  cmx_image_command_cb_type cb_func,
  void                      *client_data
) {
  cmx_af_server_data_type s_data;

  if((handle->cmd == CMX_AF_CMD_PLAY) && (handle->cb_func.play != NULL)) {
    cmx_image_buf_req_cb_info.image_type  = image_type;
    cmx_image_buf_req_cb_info.cb_func     = cb_func;
    cmx_image_buf_req_cb_info.client_data = client_data;
    cmx_image_buf_req_cb_info.buf_handle  = buf_handle;

    s_data.client_buf_req.buffer_size = num_bytes;
    s_data.client_buf_req.client_data = &(cmx_image_buf_req_cb_info);
    s_data.client_buf_req.cb_func     =
                 (cmx_client_buf_req_cb_func_ptr_type) cmx_image_buf_req_cb;

    handle->cb_func.play(CMX_GET_BUFFER, handle->client_data, &s_data);
  }
}
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_CMX_IMAGE */

/* <EJECT> */
#ifdef FEATURE_SAF
/*===========================================================================

FUNCTION cmx_saf_frame_done_processing

DESCRIPTION
  This function is used to handle SAF frames.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_frame_done_processing (
  cmx_af_handle_type    *handle_ptr,
  uint8                 *buffer,
  uint32                num_bytes,
  uint32                delay,
  boolean               last_frame
) {
#ifdef FEATURE_WEBAUDIO
  cmx_af_server_data_type s_data;
#endif

#ifdef FEATURE_WEBAUDIO
  if(handle_ptr->cmd != CMX_AF_CMD_PLAY)
#endif
  {
    if(cmx_saf_status == CMX_SAF_STATUS_PAUSED) {
      cmx_saf_pending_handle_ptr = handle_ptr;
      cmx_saf_pending_buffer     = buffer;
      cmx_saf_pending_num_bytes  = num_bytes;
      cmx_saf_pending_delay      = delay;
      cmx_saf_pending_last_frame = last_frame;
    } else {
      if(cmx_saf_status != CMX_SAF_STATUS_STOPPED) {
        if(handle_ptr->cb_func.image != NULL) {
          handle_ptr->cb_func.image(CMX_SAF_FRAME, handle_ptr->client_data,
                                    buffer, num_bytes);
        }
      }

      if(cmx_saf_ctl == CMX_SAF_CTL_FREE_RUN) {
        cmx_saf_frame_number++;
        if((!last_frame) && (cmx_saf_status != CMX_SAF_STATUS_STOPPED)) {
          if(delay == 0) {
            if(handle_ptr->cb_func.image != NULL) {
              cmx_saf_pending_status = cmx_saf_status;
              cmx_saf_status         = CMX_SAF_STATUS_PAUSED;
              if(handle_ptr->cb_func.image != NULL) {
                handle_ptr->cb_func.image(CMX_PAUSE, handle_ptr->client_data,
                                          NULL, 0);
              }
            }
            (void) rex_clr_timer( &cmx_saf_frame_timer );
          } else {
            (void) rex_set_timer( &cmx_saf_frame_timer, delay );
          }
        } else {
          cmx_saf_cleanup(handle_ptr, CMX_DONE);
          graph_saf_decode_stop();
        }
      }
    }
#ifdef FEATURE_WEBAUDIO
  } else {
    s_data.saf_data.num       = num_bytes;
    s_data.saf_data.buffer    = buffer;
    s_data.saf_data.frame_pos = cmx_saf_audfmt_frame_pos;
    handle_ptr->cb_func.play(CMX_SAF_FRAME, handle_ptr->client_data, &s_data);
    if ((cmx_saf_status == CMX_SAF_STATUS_PLAYING) &&
        (cmx_saf_audfmt_frame_status == CMX_SAF_FRAME_PEND) &&
        (cmx_saf_frame_cb != NULL) &&
        (handle_ptr->saf_buffer.buffer != NULL)) {
      cmx_saf_audfmt_frame_pos = cmx_saf_audfmt_frame_pos_pending;
      cmx_saf_frame_cb((int32) cmx_saf_audfmt_frame_id,
                       handle_ptr->saf_buffer.buffer,
                       cmx_saf_frame_client_data);
      cmx_saf_audfmt_frame_status = CMX_SAF_FRAME_PROC;
    } else {
      cmx_saf_audfmt_frame_status = CMX_SAF_FRAME_NONE;
    }
#endif
  }
}
#endif /* FEATURE_SAF */

/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
#if defined(FEATURE_WEBAUDIO) && defined(FEATURE_EFS)
/*===========================================================================

FUNCTION cmx_af_copy_pic_buffer

DESCRIPTION
  This function is used to copy data buffers into cmx_picture_buffer[].

DEPENDENCIES
  None

RETURN VALUE
  uint32 - the number of bytes copied into cmx_picture_buffer[].

SIDE EFFECTS
  cmx_picture_buffer

===========================================================================*/
uint32 cmx_af_copy_pic_buffer (
  snd_cmx_af_buf_rtn_type *buf_rtn
) {
  uint32 ret_val = 0, tlong;

  if(buf_rtn != NULL) {
    if((buf_rtn->buf1 != NULL) && (buf_rtn->length1 > 0)) {
      ret_val = MIN(sizeof(cmx_picture_buffer), buf_rtn->length1);
      memcpy(cmx_picture_buffer, buf_rtn->buf1, ret_val);
    }
    if((buf_rtn->buf2 != NULL) && (buf_rtn->length2 > 0)) {
      tlong = MIN(sizeof(cmx_picture_buffer) - ret_val, buf_rtn->length2);

      memcpy(&(cmx_picture_buffer[ret_val]), buf_rtn->buf2, tlong);

      ret_val += tlong;
    }
  }
  return(ret_val);
}
#endif /* FEATURE_WEBAUDIO && FEATURE_EFS */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_play_callback

DESCRIPTION
  This function is used as a callback to request/send data from the
  midi/WebAudio file.

DEPENDENCIES
  cmx_af_buf_info

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_af_play_callback (
  snd_af_data_cmd_enum  command,
  void                  *file,
  uint32                num,
  void                  *data
) {
  cmx_audfmt_buf_enum_type     index        = CMX_AUDFMT_BUF_32;
  cmx_af_handle_type           *handle_ptr;
  cmx_packet_type              *packet;
  snd_cmx_af_spec_type         *audio_spec;
  cmx_af_audio_spec_type       *dest_spec;
  snd_af_preparse_type         *preparse_ptr;
  cmx_af_source_info_type      *src_info_ptr = NULL;
  snd_status_type              extended_status;

#if defined(FEATURE_IMELODY) || defined(FEATURE_WEBAUDIO)
  snd_cmx_af_led_ctl_type        *led_ctl;
#endif /* FEATURE_IMELODY || FEATURE_WEBAUDIO */
#ifdef FEATURE_WEBAUDIO
  cmx_client_buf_type            *client_buf = NULL;
  uint8                          *text_buffer;
  cmx_af_text_ctl_enum_type      text_command = CMX_AF_TEXT_CTL_TITLE;
  cmx_af_pic_status_enum_type    pic_command  = CMX_AF_PIC_STATUS_ERROR;
  snd_cmx_af_buf_rtn_type        *buf_rtn     = NULL;
  snd_cmx_af_text_type           *text_state  = NULL;
  snd_cmx_af_pic_new_type        *pic_state;
  snd_cmx_mfi_led_ctl_type       *mfi_led_ctl;
#ifdef FEATURE_SAF
  snd_cmx_af_saf_type            *anim_state;
#endif
#ifdef FEATURE_SVG
  snd_cmx_af_svg_data_type       *svg_data;
  snd_cmx_af_svg_frame_type      *svg_frame;
#endif
  uint32                         tlong;
#ifdef FEATURE_SMAF
  snd_cmx_smaf_graph_info_type   *graph_info;
  snd_cmx_smaf_led_vib_ctl_type  *smaf_ctl;
#endif /* FEATURE_SMAF */
#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
  snd_cmx_af_info_type           *af_info;
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

  switch(command) {
    case SND_CMX_AF_TEXT_NEW:
      text_command = CMX_AF_TEXT_CTL_NEW;
      text_state   = (snd_cmx_af_text_type *) data;
      buf_rtn      = text_state->buf_rtn;
      break;

    case SND_CMX_AF_TEXT_APPEND:
      text_command = CMX_AF_TEXT_CTL_APPEND;
      text_state   = (snd_cmx_af_text_type *) data;
      buf_rtn      = text_state->buf_rtn;
      break;

    case SND_CMX_AF_TITLE:
      text_command = CMX_AF_TEXT_CTL_TITLE;
      text_state   = NULL;
      buf_rtn      = (snd_cmx_af_buf_rtn_type *) data;
      break;

    case SND_CMX_AF_PIC_CLEAR:
      pic_command  = CMX_AF_PIC_STATUS_CLEAR;
      break;

    case SND_CMX_AF_PIC_ENABLE:
      pic_command  = CMX_AF_PIC_STATUS_ENABLE;
      break;

    case SND_CMX_AF_PIC_DISABLE:
      pic_command  = CMX_AF_PIC_STATUS_DISABLE;
      break;

    default:
      break;
  }
#endif /* FEATURE_WEBAUDIO */

  handle_ptr = (cmx_af_handle_type *) file;

  switch(command) {
#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
    case SND_CMX_AF_CODEC_UPDATE:
      if ((handle_ptr != NULL) &&
          (handle_ptr->codec_update_cb != NULL) &&
          (data != NULL)) {
        packet = cmx_cmd_get_pkt();
        if (packet != NULL) {
          packet->hdr.cmd = CMX_AF_CMD_INFO_UPDATE;
          af_info = (snd_cmx_af_info_type*) data;
          switch(af_info->codec_type) {
            case SND_CMX_AF_FILE_AAC:
              packet->codec_update.codec_info.codec_type = CMX_AF_FILE_AAC;
              packet->codec_update.codec_info.aac_info.sample_rate =
                af_info->aac_info.sample_rate;
              switch(af_info->aac_info.channel_mode) {
                case SND_AAC_CHANNEL_MONO:
                  packet->codec_update.codec_info.aac_info.channel_mode =
                                                CMX_AF_AAC_CHANNEL_MONO;
                  break;
                case SND_AAC_CHANNEL_DUAL:
                  packet->codec_update.codec_info.aac_info.channel_mode =
                                                CMX_AF_AAC_CHANNEL_DUAL;
                  break;
                case SND_AAC_CHANNEL_DUAL_MONO:
                  packet->codec_update.codec_info.aac_info.channel_mode =
                                                CMX_AF_AAC_CHANNEL_DUAL_MONO;
                  break;
                default:
                  packet->codec_update.codec_info.aac_info.channel_mode =
                                                CMX_AF_AAC_CHANNEL_UNSUPPORTED;
                  break;
              }
              packet->codec_update.handle = handle_ptr;
              break;
            default:
              MSG_ERROR("Invalid codec info update callback", 0, 0, 0);
              q_put (&cmx_free_q, &packet->hdr.link);
              return;
          }
          cmx_cmd(packet);
        }
      }
      break;
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */
  case SND_CMX_AF_READ:
      if (handle_ptr != NULL) {
        packet = cmx_cmd_get_pkt ();

        if (packet != NULL) {
          packet->hdr.cmd                  = CMX_AF_CMD_READ;
          packet->audfmt_read.position     = num;
          packet->audfmt_read.handle       = handle_ptr;
          packet->audfmt_read.client_data  =
                                     ((snd_af_read_cb_type *) data)->client_data;
          packet->audfmt_read.cb_func      =
                                     ((snd_af_read_cb_type *) data)->cb_func;
          cmx_cmd (packet);
        }
      } else {
        MSG_FATAL( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_READ_VAR:
    case SND_CMX_AF_READ_VAR_DLS:
#ifdef FEATURE_WEBAUDIO
    case SND_CMX_ADEC_READ_VAR:
#endif
      if (handle_ptr != NULL) {
        packet = cmx_cmd_get_pkt ();

        if (packet != NULL) {
          packet->hdr.cmd                  = CMX_AF_CMD_READ_VAR;
          packet->audfmt_read.cmd          = command;
          packet->audfmt_read.position     = num;
          packet->audfmt_read.handle       = handle_ptr;
          packet->audfmt_read.client_data  =
                                     ((snd_af_read_cb_type *) data)->client_data;
          packet->audfmt_read.cb_func      =
                                     ((snd_af_read_cb_type *) data)->cb_func;
          packet->audfmt_read.buffer_size  =
                                     ((snd_af_read_cb_type *) data)->buffer_size;
          packet->audfmt_read.offset       =
                                     ((snd_af_read_cb_type *) data)->offset;

          if (handle_ptr->source_info.source == CMX_SOURCE_CLIENT) {
            cmx_cmd_hi (packet);
          } else {
            cmx_cmd (packet);
          }
        }
      } else {
        MSG_FATAL( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_PREPARSE:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        preparse_ptr                          = (snd_af_preparse_type *) data;
        packet->hdr.cmd                       = CMX_AF_CMD_PREPARSE;
        packet->audfmt_preparse.client_data   = preparse_ptr->client_data;
        packet->audfmt_preparse.preparse_func = preparse_ptr->preparse_func;
        cmx_cmd_hi (packet);
      }
      break;

    case SND_CMX_AF_FREE_BUF:
      if(data != NULL) {
        if( handle_ptr != NULL){ /* handle_ptr can be NULL for adec buffers */
          cmx_af_free_buff(handle_ptr->buffer, (uint8 *) data);
        }
#ifdef FEATURE_WEBAUDIO
        cmx_adec_free_buffer((uint8 *) data);
#endif /* FEATURE_WEBAUDIO */
      }
      break;

    case SND_CMX_AF_FREE_VAR_BUF:
      if (data != NULL) {
        if (handle_ptr != NULL) {
          src_info_ptr = &handle_ptr->source_info;

          if((src_info_ptr->source == CMX_SOURCE_MEM) &&
             (((uint8*)data >= src_info_ptr->handle.buf) &&
              ((uint8*)data <
                         (src_info_ptr->handle.buf + src_info_ptr->cmx_len)))) {
            break;
          }
          if(!(cmx_af_free_buff(handle_ptr->buffer, (uint8 *) data))) {
            CMX_FREE(data);
          }
        } else {
          CMX_FREE(data);
        }
      }
      break;

    case SND_CMX_AF_GET_DLS:
      if (handle_ptr != NULL) {
        packet = cmx_cmd_get_pkt ();

        if (packet != NULL) {
          packet->hdr.cmd                     = CMX_AF_CMD_GET_DLS;
          packet->audfmt_get_dls.handle       = handle_ptr;
          packet->audfmt_get_dls.length       =
                                  ((snd_af_get_dls_cb_type *) data)->length;
          packet->audfmt_get_dls.position     =
                                  ((snd_af_get_dls_cb_type *) data)->position;
          packet->audfmt_get_dls.client_data  =
                                  ((snd_af_get_dls_cb_type *) data)->client_data;
          packet->audfmt_get_dls.cb_func      =
                                  ((snd_af_get_dls_cb_type *) data)->cb_func;
          cmx_cmd (packet);
        }
      } else {
        MSG_FATAL( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_FREE_DLS:
#ifdef FEATURE_WEBAUDIO
      if(data != NULL) {
        if (handle_ptr != NULL) {
          src_info_ptr = &handle_ptr->source_info;
          /* Don't free the source buffer, for CMX_SOURCE_MEM since it not
           * malloc-ed.
           */
          if((src_info_ptr->source == CMX_SOURCE_MEM) &&
             (((uint8*)data < src_info_ptr->handle.buf) ||
              ((uint8*)data >=
                         (src_info_ptr->handle.buf + src_info_ptr->cmx_len)))) {
            CMX_FREE(data);
          } else if (src_info_ptr->source == CMX_SOURCE_EFS) {
            CMX_FREE(data);
          }
        } else {
          CMX_FREE(data);
        }
      }
#endif /* FEATURE_WEBAUDIO */
      break;

    case SND_CMX_AF_SET_TRACKS:
      if (handle_ptr != NULL) {
        if(handle_ptr->source_info.source == CMX_SOURCE_MEM) {
          if(handle_ptr->cmd == CMX_AF_CMD_RINGER) {
            *(uint32 *) data = 0xffff;   /* Arbitrarily large number */
          } else {
            *(uint32 *) data = CMX_AF_MAX_BUFFER_SIZE;
          }
        }
        else if((handle_ptr->source_info.source == CMX_SOURCE_CLIENT)
  #ifdef FEATURE_EFS
             || (handle_ptr->source_info.source == CMX_SOURCE_EFS)
  #endif
        ) {
          while(index != CMX_AUDFMT_BUF_INVALID) {
            if(num <= cmx_af_buf_info[index].max_tracks) {
              *(uint32 *) data             = cmx_af_buf_info[index].num_bytes;
              handle_ptr->buffer->buf_type = index;
              break;
            }
            index = (cmx_audfmt_buf_enum_type) (index + 1);
          }
        }
      } else {
        MSG_FATAL( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

   case SND_CMX_AF_GET_FILE_LEN:
     if (handle_ptr != NULL) {
       *(uint32 *)data = handle_ptr->source_info.cmx_len;
     } else {
       *(uint32 *)data = 0;
     }
     break;

#ifdef FEATURE_WEBAUDIO
    case SND_CMX_AF_TEXT_ADVANCE:
      if(cmx_text_advance_count != 0) {
        cmx_text_advance_count += num;
        break;
      } else {
        cmx_text_advance_count += num;
      }
      /* Fall thru */
#endif
    case SND_CMX_AF_STARTED:
    case SND_CMX_AF_SBR_MODE:
    case SND_CMX_AF_DUAL_MONO_MODE:
    case SND_CMX_AF_FLUSHED:
    case SND_CMX_AF_REWIND:
    case SND_CMX_AF_FFORWARD:
    case SND_CMX_AF_PAUSE:
    case SND_CMX_AF_RESUME:
    case SND_CMX_AF_FA_DELAY:
    case SND_CMX_AF_TEMPO:
    case SND_CMX_AF_TUNE:
    case SND_CMX_AF_PAN:
#ifdef FEATURE_WEBAUDIO
    case SND_CMX_AF_TEXT_CLEAR:
    case SND_CMX_AF_TEXT_BACK:
    case SND_CMX_AF_TEXT_DISABLE:
    case SND_CMX_AF_TEXT_ENABLE:
    case SND_CMX_AF_ANIM_CLEAR:
    case SND_CMX_AF_ANIM_DISABLE:
    case SND_CMX_AF_ANIM_ENABLE:
    case SND_CMX_AF_CUE_POINT_END:
    case SND_CMX_AF_JUMP_POINT_END:
#endif
      if (handle_ptr != NULL) {
        if(handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) {
          packet = cmx_cmd_get_pkt ();

          if (packet != NULL) {
            packet->hdr.cmd                 = CMX_AF_CMD_SUPPORT;
            packet->audfmt_support.handle   = handle_ptr;
            packet->audfmt_support.command  = command;
            packet->audfmt_support.position = num;
            if(data != NULL) {
              packet->audfmt_support.number = *((uint32 *) data);
            } else {
              packet->audfmt_support.number = 0;
            }
            cmx_cmd (packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_AUDIO_SPEC:
      if((handle_ptr != NULL) &&
         ((handle_ptr->cmd == CMX_AF_CMD_GET_SPEC) ||
          (((handle_ptr->cmd == CMX_AF_CMD_PLAY) ||
            (handle_ptr->cmd == CMX_AF_CMD_RINGER)) &&
           (handle_ptr->cb_type != CMX_AF_CALLBACK_NONE))) &&
         (data != NULL)) {
        packet = cmx_cmd_get_pkt ();
        if(packet != NULL) {
          audio_spec = (snd_cmx_af_spec_type *) data;
          dest_spec  = cmx_get_audio_spec();
          if(dest_spec != NULL) {
            packet->hdr.cmd                      = CMX_AF_CMD_AUDIO_SPEC;
            packet->audfmt_audio_spec.handle     = handle_ptr;
            packet->audfmt_audio_spec.audio_spec = dest_spec;

            switch(audio_spec->file_type) {
#ifdef FEATURE_MIDI
              case SND_CMX_AF_FILE_MIDI:
#ifdef FEATURE_XMF
                if(handle_ptr->status_mask & CMX_AF_STATUS_MASK_XMF_RECOGNIZED)
                {
                  cmx_free_audio_spec(dest_spec);
                  q_put(&cmx_free_q, &packet->hdr.link);
                  return;
                }
#endif /* FEATURE_XMF */
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_MIDI;
                dest_spec->file_type                = CMX_AF_FILE_MIDI;
                dest_spec->midi_spec.sequence       = handle_ptr;
                dest_spec->midi_spec.supported_ops  =
                                   (cmx_af_suprted_ops_mask_type)
                                          audio_spec->midi_spec.supported_ops;
                break;

#ifdef FEATURE_WEBAUDIO
              case SND_CMX_AF_FILE_MFI:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_MFI;
                dest_spec->file_type                = CMX_AF_FILE_MFI;
                dest_spec->midi_spec.sequence       = handle_ptr;
                dest_spec->midi_spec.supported_ops  =
                                   (cmx_af_suprted_ops_mask_type)
                                          audio_spec->mfi_spec.supported_ops;
                break;

              case SND_CMX_AF_FILE_CMID:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_CMID;

                dest_spec->file_type        = CMX_AF_FILE_CMID;
                dest_spec->wa_spec.sequence = handle_ptr;
                dest_spec->wa_spec.contents = audio_spec->cmid_spec.contents;
                dest_spec->wa_spec.supported_ops =
                                   (cmx_af_suprted_ops_mask_type)
                                          audio_spec->cmid_spec.supported_ops;

                switch(audio_spec->cmid_spec.char_code) {
                  case SND_CMX_AF_CS_ANSI:
                    dest_spec->wa_spec.char_set = CMX_AF_CHAR_SET_ANSI;
                    break;

                  case SND_CMX_AF_CS_ISO8859_1:
                  case SND_CMX_AF_CS_ISO8859_2:
                  case SND_CMX_AF_CS_ISO8859_3:
                  case SND_CMX_AF_CS_ISO8859_4:
                  case SND_CMX_AF_CS_ISO8859_5:
                  case SND_CMX_AF_CS_ISO8859_6:
                  case SND_CMX_AF_CS_ISO8859_7:
                  case SND_CMX_AF_CS_ISO8859_8:
                  case SND_CMX_AF_CS_ISO8859_9:
                  case SND_CMX_AF_CS_ISO8859_10:
                    dest_spec->wa_spec.char_set = (cmx_af_char_set_enum_type)
                                        (CMX_AF_CHAR_SET_ISO8859_1 +
                                         (audio_spec->cmid_spec.char_code -
                                          SND_CMX_AF_CS_ISO8859_1));
                    break;

                  case SND_CMX_AF_CS_SHIFTJIS:
                    dest_spec->wa_spec.char_set = CMX_AF_CHAR_SET_SHIFTJIS;
                    break;

                  case SND_CMX_AF_CS_HANGUL:
                    dest_spec->wa_spec.char_set = CMX_AF_CHAR_SET_HANGUL;
                    break;

                  case SND_CMX_AF_CS_CHINESE_SIMP:
                    dest_spec->wa_spec.char_set = CMX_AF_CHAR_SET_CHINESE_SIMP;
                    break;

                  case SND_CMX_AF_CS_CHINESE_TRAD:
                    dest_spec->wa_spec.char_set = CMX_AF_CHAR_SET_CHINESE_TRAD;
                    break;

                  case SND_CMX_AF_CS_HINDI:
                    dest_spec->wa_spec.char_set = CMX_AF_CHAR_SET_HINDI;
                    break;

                  case SND_CMX_AF_CS_UNKNOWN:
                  default:
                    dest_spec->wa_spec.char_set = CMX_AF_CHAR_SET_UNKNOWN;
                    break;
                }
                break;

#ifdef FEATURE_SMAF
              case SND_CMX_AF_FILE_SMAF:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_SMAF;
                dest_spec->file_type                = CMX_AF_FILE_SMAF;
                dest_spec->smaf_spec.sequence       = handle_ptr;
                cmx_af_do_audio_spec_smaf(audio_spec, &dest_spec->smaf_spec);
                break;
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_MIDI */

#ifdef FEATURE_MP3
              case SND_CMX_AF_FILE_MP3:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_MP3;
                dest_spec->file_type                = CMX_AF_FILE_MP3;
                cmx_af_do_audio_spec_mp3(audio_spec, &dest_spec->mp3_spec);
                break;
#endif

#ifdef FEATURE_AAC
              case SND_CMX_AF_FILE_AAC:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_AAC;
                dest_spec->file_type                = CMX_AF_FILE_AAC;
                cmx_af_do_audio_spec_aac(audio_spec, &dest_spec->aac_spec);
                break;
#endif

#ifdef FEATURE_IMELODY
              case SND_CMX_AF_FILE_IMY:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_IMY;
                dest_spec->file_type                = CMX_AF_FILE_IMY;
                cmx_af_do_audio_spec_imy(audio_spec, &dest_spec->imy_spec);
                break;
#endif

#ifdef FEATURE_ADPCM
              case SND_CMX_AF_FILE_ADPCM:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_WAVE;
                cmx_af_do_audio_spec_wave(audio_spec, &dest_spec->adpcm_spec,
                                          CMX_AF_WAVE_FORMAT_IMA_ADPCM);
                break;
#endif

#ifdef FEATURE_PCM
              case SND_CMX_AF_FILE_PCM:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_WAVE;
                cmx_af_do_audio_spec_wave(audio_spec, &dest_spec->wave_spec,
                                          CMX_AF_WAVE_FORMAT_LINEAR_PCM);
                break;
#endif

#ifdef FEATURE_VOC_G711
              case SND_CMX_AF_FILE_WAV_ALAW:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_WAVE;
                cmx_af_do_audio_spec_wave(audio_spec, &dest_spec->wave_spec,
                                          CMX_AF_WAVE_FORMAT_ALAW);
                break;

              case SND_CMX_AF_FILE_WAV_MULAW:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_WAVE;
                cmx_af_do_audio_spec_wave(audio_spec, &dest_spec->wave_spec,
                                          CMX_AF_WAVE_FORMAT_MULAW);
                break;
#endif /* FEATURE_VOC_G711 */

#ifdef FEATURE_REAL_AUDIO
              case SND_CMX_AF_FILE_RA:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_RA;
                dest_spec->file_type                = CMX_AF_FILE_RA;
                break;
#endif

#ifdef FEATURE_QCP
              case SND_CMX_AF_FILE_QCP_13K:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_QCP_13K;
                dest_spec->file_type                = CMX_AF_FILE_QCP_13K;
                dest_spec->qcp_spec.supported_ops   =
                                   (cmx_af_suprted_ops_mask_type)
                                          audio_spec->qcp_spec.supported_ops;
                break;

              case SND_CMX_AF_FILE_QCP_EVRC:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_QCP_EVRC;
                dest_spec->file_type                = CMX_AF_FILE_QCP_EVRC;
                dest_spec->qcp_spec.supported_ops   =
                                   (cmx_af_suprted_ops_mask_type)
                                          audio_spec->qcp_spec.supported_ops;
                break;

              case SND_CMX_AF_FILE_QCP_4GV_NB:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_QCP_4GV_NB;
                dest_spec->file_type                = CMX_AF_FILE_QCP_4GV_NB;
                dest_spec->qcp_spec.supported_ops   =
                                   (cmx_af_suprted_ops_mask_type)
                                          audio_spec->qcp_spec.supported_ops;
                break;

              case SND_CMX_AF_FILE_QCP_4GV_WB:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_QCP_4GV_WB;
                dest_spec->file_type                = CMX_AF_FILE_QCP_4GV_WB;
                dest_spec->qcp_spec.supported_ops   =
                                   (cmx_af_suprted_ops_mask_type)
                                          audio_spec->qcp_spec.supported_ops;
                break;

              case SND_CMX_AF_FILE_QCP_AMR:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_QCP_AMR;
                dest_spec->file_type                = CMX_AF_FILE_QCP_AMR;
                dest_spec->qcp_spec.supported_ops   =
                                   (cmx_af_suprted_ops_mask_type)
                                          audio_spec->qcp_spec.supported_ops;
                break;

              case SND_CMX_AF_FILE_AMR:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_AMR;
                dest_spec->file_type                = CMX_AF_FILE_AMR;
                dest_spec->amr_spec.supported_ops   =
                                   (cmx_af_suprted_ops_mask_type)
                                          audio_spec->qcp_spec.supported_ops;
                break;

#ifdef FEATURE_AUDFMT_EVB
              case SND_CMX_AF_FILE_EVB:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_EVB;
                dest_spec->file_type                = CMX_AF_FILE_EVB;
                dest_spec->evb_spec.supported_ops   =
                                   (cmx_af_suprted_ops_mask_type)
                                          audio_spec->qcp_spec.supported_ops;
                break;
#endif /* FEATURE_AUDFMT_EVB */


#ifdef FEATURE_AUDFMT_EVW
              case SND_CMX_AF_FILE_EVW:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_EVW;
                dest_spec->file_type                = CMX_AF_FILE_EVW;
                dest_spec->evw_spec.supported_ops   =
                                   (cmx_af_suprted_ops_mask_type)
                                          audio_spec->qcp_spec.supported_ops;
                break;
#endif /* FEATURE_AUDFMT_EVW */

#endif /* FEATURE_QCP */

#ifdef FEATURE_WMA
              case SND_CMX_AF_FILE_WMA:
              case SND_CMX_AF_FILE_WMA_PRO:
                packet->audfmt_audio_spec.file_type = CMX_AF_FILE_WMA;
                dest_spec->file_type                = CMX_AF_FILE_WMA;
                break;
#endif /* FEATURE_WMA */

              case SND_CMX_AF_FILE_UNKNOWN:
              default:
                packet->audfmt_audio_spec.file_type  = CMX_AF_FILE_UNKNOWN;
                packet->audfmt_audio_spec.audio_spec = NULL;
                cmx_free_audio_spec(dest_spec);
                break;
            }

            cmx_cmd (packet);
          } else {
            MSG_ERROR("Ran out of audio spec buffers!!!", 0, 0, 0);
            q_put(&cmx_free_q, &packet->hdr.link);
          }
        }
      }
      break;

#ifdef FEATURE_WEBAUDIO
    case SND_CMX_ADEC_READ:
      if (handle_ptr != NULL) {
        packet = cmx_cmd_get_pkt ();
        if (packet != NULL) {
          packet->hdr.cmd                  = CMX_ADEC_READ;
          packet->audfmt_read.position     = num;
          packet->audfmt_read.handle       = handle_ptr;
          packet->audfmt_read.client_data  =
                                   ((snd_af_read_cb_type *) data)->client_data;
          packet->audfmt_read.cb_func      =
                                   ((snd_af_read_cb_type *) data)->cb_func;
          cmx_cmd (packet);
        }
      } else {
        MSG_FATAL( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_TEXT_NEW:
    case SND_CMX_AF_TEXT_APPEND:
    case SND_CMX_AF_TITLE:
      if (handle_ptr != NULL) {
        if((handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) && (buf_rtn != NULL)) {
          if((handle_ptr->cb_type == CMX_AF_CALLBACK_VALUE) &&
             (handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF) &&
             (handle_ptr->content_mask & CMX_AF_WA_CONTENTS_TEXT) &&
             (handle_ptr->text_buffer.buffer == NULL) &&
             (handle_ptr->text_buffer.pending_cmd == NULL)) {
            break;
          }

          if((handle_ptr->cb_type == CMX_AF_CALLBACK_VALUE) &&
             (handle_ptr->content_mask & CMX_AF_WA_CONTENTS_TEXT) == 0) {
            if((handle_ptr->text_buffer.buffer == NULL) &&
               (handle_ptr->text_buffer.pending_cmd == NULL)) {
              packet = cmx_cmd_get_pkt ();
              if (packet != NULL) {
                handle_ptr->text_buffer.handle    = handle_ptr;
                packet->hdr.cmd                   = CMX_CMD_CLIENT_BUF_REQ;
                packet->client_buf_req.handle     = handle_ptr;
                packet->client_buf_req.buf_handle = &(handle_ptr->text_buffer);
                packet->client_buf_req.num_bytes  = CMX_MAX_TEXT_STRING;

                cmx_cmd(packet);

                client_buf = &handle_ptr->text_buffer;
                if((command == SND_CMX_AF_TEXT_NEW) ||
                   (command == SND_CMX_AF_TEXT_APPEND)) {
                  handle_ptr->content_mask |= CMX_AF_WA_CONTENTS_TEXT;
                }
              }
            } else if((command == SND_CMX_AF_TEXT_NEW) ||
                      (command == SND_CMX_AF_TEXT_APPEND)) {
              handle_ptr->content_mask |= CMX_AF_WA_CONTENTS_TEXT;
            }
          }

          if(handle_ptr->text_buffer.pending_cmd == NULL) {
            packet     = cmx_cmd_get_pkt ();
          } else {
            packet     = handle_ptr->text_buffer.pending_cmd;
            client_buf = &handle_ptr->text_buffer;
          }

          if (packet != NULL) {
            tlong = buf_rtn->length1 + buf_rtn->length2;

            packet->hdr.cmd                  = CMX_AF_CMD_TEXT_CTL;
            packet->audfmt_text_ctl.command  = text_command;
            packet->audfmt_text_ctl.handle   = handle_ptr;

            if(text_state != NULL) {
              if(text_state->xpos_type == SND_CMX_AF_POS_LT) {
                packet->audfmt_text_ctl.xpos_type  = CMX_AF_POS_LT;
              } else if(text_state->xpos_type == SND_CMX_AF_POS_RB) {
                packet->audfmt_text_ctl.xpos_type  = CMX_AF_POS_RB;
              } else {
                packet->audfmt_text_ctl.xpos_type  = CMX_AF_POS_CENTER;
              }

              if(text_state->ypos_type == SND_CMX_AF_POS_LT) {
                packet->audfmt_text_ctl.ypos_type  = CMX_AF_POS_LT;
              } else if(text_state->ypos_type == SND_CMX_AF_POS_RB) {
                packet->audfmt_text_ctl.ypos_type  = CMX_AF_POS_RB;
              } else {
                packet->audfmt_text_ctl.ypos_type  = CMX_AF_POS_CENTER;
              }
            }

            if((handle_ptr->cb_type == CMX_AF_CALLBACK_REFERENCE) ||
               (handle_ptr->source_info.source == CMX_SOURCE_MEM))
            {
              packet->audfmt_text_ctl.buffer       = NULL;
              packet->audfmt_text_ctl.buf_len      = 0;
              packet->audfmt_text_ctl.position     = num - tlong;
              packet->audfmt_text_ctl.remain_bytes = buf_rtn->remain_bytes +
                                                     tlong;
            }
#ifdef FEATURE_EFS
            else if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
              if(handle_ptr->text_buffer.buffer != NULL) {
                text_buffer = handle_ptr->text_buffer.buffer;
              } else {
                text_buffer = cmx_text_buffer;
              }
              if((cmx_text_counting_semaphore) && (client_buf == NULL)) {
                packet->audfmt_text_ctl.buf_len      = 0;
                packet->audfmt_text_ctl.position     = num - tlong;
                packet->audfmt_text_ctl.remain_bytes = buf_rtn->remain_bytes +
                                                       tlong;
              } else {
                if((buf_rtn->buf1 != NULL) && (buf_rtn->length1 != 0)) {
                  memcpy( text_buffer, buf_rtn->buf1,
                          MIN(CMX_MAX_TEXT_STRING,buf_rtn->length1) );
                }
                if((buf_rtn->buf2 != NULL) && (buf_rtn->length2 != 0) &&
                   (buf_rtn->length1 < CMX_MAX_TEXT_STRING)) {
                  memcpy( &(text_buffer[buf_rtn->length1]), buf_rtn->buf2,
                          MIN(buf_rtn->length2,
                              CMX_MAX_TEXT_STRING - buf_rtn->length1) );
                }
                packet->audfmt_text_ctl.buf_len      = MIN(CMX_MAX_TEXT_STRING,
                                                           tlong);
                packet->audfmt_text_ctl.position     = num;
                packet->audfmt_text_ctl.remain_bytes = buf_rtn->remain_bytes;
              }
              packet->audfmt_text_ctl.buffer         = text_buffer;
              if(client_buf == NULL) {
                cmx_text_counting_semaphore++;
              } else {
                cmx_text_counting_semaphore = 1;
              }
            }
#endif
            if(client_buf == NULL) {
              cmx_cmd (packet);
            } else {
              client_buf->pending_cmd = packet;
            }
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_PIC_NEW:
      if(handle_ptr != NULL) {
        if(handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) {
          if((handle_ptr->cb_type == CMX_AF_CALLBACK_VALUE) &&
             (handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF) &&
             (handle_ptr->content_mask & CMX_AF_WA_CONTENTS_PIC) &&
             (handle_ptr->pict_buffer.buffer == NULL) &&
             (handle_ptr->pict_buffer.pending_cmd == NULL)) {
            break;
          }

          if((handle_ptr->cb_type == CMX_AF_CALLBACK_VALUE) &&
             (handle_ptr->content_mask & CMX_AF_WA_CONTENTS_PIC) == 0) {
            packet = cmx_cmd_get_pkt ();
            if (packet != NULL) {
              handle_ptr->pict_buffer.handle    = handle_ptr;
              packet->hdr.cmd                   = CMX_CMD_CLIENT_BUF_REQ;
              packet->client_buf_req.handle     = handle_ptr;
              packet->client_buf_req.buf_handle = &(handle_ptr->pict_buffer);
              packet->client_buf_req.num_bytes  = CMX_PIC_BUFFER_SIZE;

              cmx_cmd(packet);

              client_buf = &handle_ptr->pict_buffer;

              handle_ptr->content_mask |= CMX_AF_WA_CONTENTS_PIC;
            }
          }

#ifdef FEATURE_PNG_DECODER
          cmx_png_start_new = TRUE;
#endif /* FEATURE_PNG_DECODER */
          pic_state         = (snd_cmx_af_pic_new_type *) data;

          packet = cmx_cmd_get_pkt ();
          if(packet != NULL) {
            packet->hdr.cmd                  = CMX_AF_CMD_PIC_INIT;
            packet->audfmt_pic_init.handle   = handle_ptr;

            if(pic_state->draw_speed == SND_CMX_AF_PIC_SPEED_SLOW) {
              packet->audfmt_pic_init.draw_speed = CMX_AF_PIC_SPEED_SLOW;
            } else if(pic_state->draw_speed == SND_CMX_AF_PIC_SPEED_MIDDLE) {
              packet->audfmt_pic_init.draw_speed = CMX_AF_PIC_SPEED_MIDDLE;
            } else if(pic_state->draw_speed == SND_CMX_AF_PIC_SPEED_FAST) {
              packet->audfmt_pic_init.draw_speed = CMX_AF_PIC_SPEED_FAST;
            } else {
              packet->audfmt_pic_init.draw_speed = CMX_AF_PIC_SPEED_NORMAL;
            }

            if(pic_state->xpos_type == SND_CMX_AF_POS_PIXEL) {
              packet->audfmt_pic_init.xpos_type    = CMX_AF_POS_PIXEL;
              packet->audfmt_pic_init.xpos_percent = pic_state->xpos_percent;
            } else if(pic_state->xpos_type == SND_CMX_AF_POS_PERCENT) {
              packet->audfmt_pic_init.xpos_type    = CMX_AF_POS_PERCENT;
              packet->audfmt_pic_init.xpos_percent = pic_state->xpos_percent;
            } else if(pic_state->xpos_type == SND_CMX_AF_POS_LT) {
              packet->audfmt_pic_init.xpos_type    = CMX_AF_POS_LT;
            } else if(pic_state->xpos_type == SND_CMX_AF_POS_RB) {
              packet->audfmt_pic_init.xpos_type    = CMX_AF_POS_RB;
            } else {
              packet->audfmt_pic_init.xpos_type    = CMX_AF_POS_CENTER;
            }

            if(pic_state->ypos_type == SND_CMX_AF_POS_PIXEL) {
              packet->audfmt_pic_init.ypos_type    = CMX_AF_POS_PIXEL;
              packet->audfmt_pic_init.ypos_percent = pic_state->ypos_percent;
            } else if(pic_state->ypos_type == SND_CMX_AF_POS_PERCENT) {
              packet->audfmt_pic_init.ypos_type    = CMX_AF_POS_PERCENT;
              packet->audfmt_pic_init.ypos_percent = pic_state->ypos_percent;
            } else if(pic_state->ypos_type == SND_CMX_AF_POS_LT) {
              packet->audfmt_pic_init.ypos_type    = CMX_AF_POS_LT;
            } else if(pic_state->ypos_type == SND_CMX_AF_POS_RB) {
              packet->audfmt_pic_init.ypos_type    = CMX_AF_POS_RB;
            } else {
              packet->audfmt_pic_init.ypos_type    = CMX_AF_POS_CENTER;
            }

            cmx_cmd (packet);
          }

          if(handle_ptr->pict_buffer.pending_cmd == NULL) {
            packet     = cmx_cmd_get_pkt ();
          } else {
            packet     = handle_ptr->pict_buffer.pending_cmd;
            client_buf = &handle_ptr->pict_buffer;
          }
          if(packet != NULL) {
            buf_rtn = pic_state->buf_rtn;

            packet->hdr.cmd                = CMX_AF_CMD_PIC_DATA;
            packet->audfmt_pic_data.handle = handle_ptr;

            if(pic_state->pic_type == SND_CMX_AF_PIC_TYPE_BMP) {
              packet->audfmt_pic_data.pic_type = CMX_AF_PIC_BMP;
            } else if(pic_state->pic_type == SND_CMX_AF_PIC_TYPE_PNG) {
              packet->audfmt_pic_data.pic_type = CMX_AF_PIC_PNG;
            } else if(pic_state->pic_type == SND_CMX_AF_PIC_TYPE_JPEG) {
              packet->audfmt_pic_data.pic_type = CMX_AF_PIC_JPEG;
            } else {
              packet->audfmt_pic_data.pic_type = CMX_AF_PIC_UNKNOWN;
            }

            if((handle_ptr->cb_type == CMX_AF_CALLBACK_REFERENCE) ||
               (handle_ptr->source_info.source == CMX_SOURCE_MEM)) {
              tlong  = buf_rtn->length1 + buf_rtn->length2;

              packet->audfmt_pic_data.buffer       = NULL;
              packet->audfmt_pic_data.buf_len      = 0;
              packet->audfmt_pic_data.position     = num - tlong;
              packet->audfmt_pic_data.remain_bytes = buf_rtn->remain_bytes+tlong;
            }
#ifdef FEATURE_EFS
            else if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
              /* tlong will be used as a count of the number of bytes
              ** copied into the local buffer.
              */
              tlong = cmx_af_copy_pic_buffer(buf_rtn);

              /* At this point, tlong is guaranteed to be less than
              ** buf_rtn->length1 + buf_rtn->length2 and a maximum of
              ** sizeof(cmx_picture_buffer).
              */
              packet->audfmt_pic_data.buffer  = cmx_picture_buffer;
              packet->audfmt_pic_data.buf_len = tlong;

              /* With the calculation below, tlong represents the number of
              ** bytes that were not copied into the local buffer. Data
              ** pointers are adjusted accordingly.
              */
              tlong = (buf_rtn->length1 + buf_rtn->length2) - tlong;
              packet->audfmt_pic_data.position     = num - tlong;
              packet->audfmt_pic_data.remain_bytes = buf_rtn->remain_bytes +
                                                     tlong;
            }
#endif
            if(client_buf == NULL) {
              cmx_cmd (packet);
            } else {
              client_buf->pending_cmd = packet;
            }
          }
        }
      } else {
        MSG_ERROR("Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_PIC_CLEAR:
    case SND_CMX_AF_PIC_ENABLE:
    case SND_CMX_AF_PIC_DISABLE:
      if (handle_ptr != NULL) {
        if(handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) {
          packet = cmx_cmd_get_pkt ();

          if (packet != NULL) {
            packet->hdr.cmd                 = CMX_AF_CMD_PIC_CTL;
            packet->audfmt_pic_ctl.command  = pic_command;
            packet->audfmt_pic_ctl.handle   = handle_ptr;
            packet->audfmt_pic_ctl.speed    =
                                      *((cmx_af_pic_speed_enum_type *) data);
            cmx_cmd (packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

#ifdef FEATURE_SAF
    case SND_CMX_AF_SAF:
      if (handle_ptr != NULL) {
        if(handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) {
          if((handle_ptr->cb_type == CMX_AF_CALLBACK_VALUE) &&
             (handle_ptr->content_mask & CMX_AF_WA_CONTENTS_ANIM) == 0) {
            packet = cmx_cmd_get_pkt ();
            if (packet != NULL) {
              handle_ptr->saf_buffer.handle     = handle_ptr;
              packet->hdr.cmd                   = CMX_CMD_CLIENT_BUF_REQ;
              packet->client_buf_req.handle     = handle_ptr;
              packet->client_buf_req.buf_handle = &(handle_ptr->saf_buffer);
              packet->client_buf_req.num_bytes  = CMX_SAF_BUFFER_SIZE;

              cmx_cmd(packet);

              client_buf = &handle_ptr->saf_buffer;

              handle_ptr->content_mask |= CMX_AF_WA_CONTENTS_ANIM;
            }
          }

          if(handle_ptr->saf_buffer.pending_cmd == NULL) {
            packet     = cmx_cmd_get_pkt ();
          } else {
            packet     = handle_ptr->saf_buffer.pending_cmd;
            client_buf = &handle_ptr->saf_buffer;
          }
          if (packet != NULL) {
            anim_state = (snd_cmx_af_saf_type *) data;
            buf_rtn    = anim_state->buf_rtn;

            handle_ptr->saf_obj_buf_size = anim_state->objbuf_size;

            packet->hdr.cmd                 = CMX_AF_CMD_ANIM;
            packet->audfmt_anim_data.type   = CMX_AF_ANIM_SAF;
            packet->audfmt_anim_data.handle = handle_ptr;

            if((handle_ptr->cb_type == CMX_AF_CALLBACK_REFERENCE) ||
               (handle_ptr->source_info.source == CMX_SOURCE_MEM)) {
              tlong = buf_rtn->length1 + buf_rtn->length2;

              packet->audfmt_anim_data.buffer       = NULL;
              packet->audfmt_anim_data.buf_len      = 0;
              packet->audfmt_anim_data.position     = num - tlong;
              packet->audfmt_anim_data.remain_bytes = buf_rtn->remain_bytes+tlong;
            }
#ifdef FEATURE_EFS
            else if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
              /* tlong will be used as a count of the number of bytes
              ** copied into the local buffer.
              */
              tlong = cmx_af_copy_pic_buffer(buf_rtn);

              /* At this point, tlong is guaranteed to be less than
              ** buf_rtn->length1 + buf_rtn->length2 and a maximum of
              ** sizeof(cmx_picture_buffer).
              */
              packet->audfmt_anim_data.buffer  = cmx_picture_buffer;
              packet->audfmt_anim_data.buf_len = tlong;

              /* With the calculation below, tlong represents the number of
              ** bytes that were not copied into the local buffer. Data
              ** pointers are adjusted accordingly.
              */
              tlong = (buf_rtn->length1 + buf_rtn->length2) - tlong;
              packet->audfmt_anim_data.position     = num - tlong;
              packet->audfmt_anim_data.remain_bytes = buf_rtn->remain_bytes +
                                                      tlong;
            }
#endif
            if(client_buf == NULL) {
              cmx_cmd (packet);
            } else {
              client_buf->pending_cmd = packet;
            }
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_SAF_FRAME:
      if (handle_ptr != NULL) {
        if(handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) {
          packet = cmx_cmd_get_pkt ();
          if (packet != NULL) {
            anim_state = (snd_cmx_af_saf_type *) data;
            buf_rtn    = anim_state->buf_rtn;

            packet->hdr.cmd                 = CMX_AF_CMD_ANIM_FRAME;
            packet->audfmt_anim_data.type   = CMX_AF_ANIM_SAF;
            packet->audfmt_anim_data.handle = handle_ptr;

            if(anim_state->xpos_type == SND_CMX_AF_POS_PIXEL) {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_PIXEL;
              packet->audfmt_anim_data.frame_pos.xpos_num  = anim_state->xpos_num;
            } else if(anim_state->xpos_type == SND_CMX_AF_POS_PERCENT) {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_PERCENT;
              packet->audfmt_anim_data.frame_pos.xpos_num  = anim_state->xpos_num;
            } else if(anim_state->xpos_type == SND_CMX_AF_POS_LT) {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_LT;
            } else if(anim_state->xpos_type == SND_CMX_AF_POS_RB) {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_RB;
            } else {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_CENTER;
            }

            if(anim_state->ypos_type == SND_CMX_AF_POS_PIXEL) {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_PIXEL;
              packet->audfmt_anim_data.frame_pos.ypos_num  = anim_state->ypos_num;
            } else if(anim_state->ypos_type == SND_CMX_AF_POS_PERCENT) {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_PERCENT;
              packet->audfmt_anim_data.frame_pos.ypos_num  = anim_state->ypos_num;
            } else if(anim_state->ypos_type == SND_CMX_AF_POS_LT) {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_LT;
            } else if(anim_state->ypos_type == SND_CMX_AF_POS_RB) {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_RB;
            } else {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_CENTER;
            }

            if((handle_ptr->cb_type == CMX_AF_CALLBACK_REFERENCE) ||
               (handle_ptr->source_info.source == CMX_SOURCE_MEM)) {
              tlong = buf_rtn->length1 + buf_rtn->length2;

              packet->audfmt_anim_data.buffer       = NULL;
              packet->audfmt_anim_data.buf_len      = 0;
              packet->audfmt_anim_data.position     = num - tlong;
              packet->audfmt_anim_data.remain_bytes = buf_rtn->remain_bytes +
                                                      tlong;
            }
#ifdef FEATURE_EFS
            else if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
              /* tlong will be used as a count of the number of bytes
              ** copied into the local buffer.
              */
              tlong = cmx_af_copy_pic_buffer(buf_rtn);

              /* At this point, tlong is guaranteed to be less than
              ** buf_rtn->length1 + buf_rtn->length2 and a maximum of
              ** sizeof(cmx_picture_buffer).
              */
              packet->audfmt_anim_data.buffer  = cmx_picture_buffer;
              packet->audfmt_anim_data.buf_len = tlong;

              /* With the calculation below, tlong represents the number of
              ** bytes that were not copied into the local buffer. Data
              ** pointers are adjusted accordingly.
              */
              tlong = (buf_rtn->length1 + buf_rtn->length2) - tlong;
              packet->audfmt_anim_data.position     = num - tlong;
              packet->audfmt_anim_data.remain_bytes = buf_rtn->remain_bytes +
                                                      tlong;
            }
#endif
            cmx_cmd (packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_SAF_FRAME_ID:
      if (handle_ptr != NULL) {
        if(handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) {
          packet = cmx_cmd_get_pkt ();
          if (packet != NULL) {
            anim_state = (snd_cmx_af_saf_type *) data;

            if(anim_state->xpos_type == SND_CMX_AF_POS_PIXEL) {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_PIXEL;
              packet->audfmt_anim_data.frame_pos.xpos_num  = anim_state->xpos_num;
            } else if(anim_state->xpos_type == SND_CMX_AF_POS_PERCENT) {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_PERCENT;
              packet->audfmt_anim_data.frame_pos.xpos_num  = anim_state->xpos_num;
            } else if(anim_state->xpos_type == SND_CMX_AF_POS_LT) {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_LT;
            } else if(anim_state->xpos_type == SND_CMX_AF_POS_RB) {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_RB;
            } else {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_CENTER;
            }

            if(anim_state->ypos_type == SND_CMX_AF_POS_PIXEL) {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_PIXEL;
              packet->audfmt_anim_data.frame_pos.ypos_num  = anim_state->ypos_num;
            } else if(anim_state->ypos_type == SND_CMX_AF_POS_PERCENT) {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_PERCENT;
              packet->audfmt_anim_data.frame_pos.ypos_num  = anim_state->ypos_num;
            } else if(anim_state->ypos_type == SND_CMX_AF_POS_LT) {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_LT;
            } else if(anim_state->ypos_type == SND_CMX_AF_POS_RB) {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_RB;
            } else {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_CENTER;
            }

            packet->hdr.cmd                   = CMX_AF_CMD_ANIM_FRAME_ID;
            packet->audfmt_anim_data.type     = CMX_AF_ANIM_SAF;
            packet->audfmt_anim_data.handle   = handle_ptr;
            packet->audfmt_anim_data.position = num;
            cmx_cmd (packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;
#endif /* FEATURE_SAF */

#ifdef FEATURE_SVG
    case SND_CMX_AF_SVG:
      if (handle_ptr != NULL) {
        if(handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) {
          packet = cmx_cmd_get_pkt ();
          if (packet != NULL) {
            svg_data = (snd_cmx_af_svg_data_type *) data;

            packet->hdr.cmd                 = CMX_AF_CMD_ANIM;
            packet->audfmt_anim_data.type   = CMX_AF_ANIM_SVG;
            packet->audfmt_anim_data.handle = handle_ptr;

            packet->audfmt_anim_data.buf_len      = 0;
            packet->audfmt_anim_data.position     = num;
            packet->audfmt_anim_data.remain_bytes = svg_data->size;

            MSG_HIGH("cmx_af_play_callback: posting SND_CMX_AF_SVG to cmx",0,0,0);
            cmx_cmd (packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_SVG_FRAME_ID:
      if (handle_ptr != NULL) {
        if(handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) {
          packet = cmx_cmd_get_pkt ();
          if (packet != NULL) {
            svg_frame = (snd_cmx_af_svg_frame_type *) data;

            if(svg_frame->xpos_type == SND_CMX_AF_POS_PIXEL) {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_PIXEL;
              packet->audfmt_anim_data.frame_pos.xpos_num  = svg_frame->xpos_num;
            } else if(svg_frame->xpos_type == SND_CMX_AF_POS_PERCENT) {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_PERCENT;
              packet->audfmt_anim_data.frame_pos.xpos_num  = svg_frame->xpos_num;
            } else if(svg_frame->xpos_type == SND_CMX_AF_POS_LT) {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_LT;
            } else if(svg_frame->xpos_type == SND_CMX_AF_POS_RB) {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_RB;
            } else {
              packet->audfmt_anim_data.frame_pos.xpos_type = CMX_AF_POS_CENTER;
            }

            if(svg_frame->ypos_type == SND_CMX_AF_POS_PIXEL) {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_PIXEL;
              packet->audfmt_anim_data.frame_pos.ypos_num  = svg_frame->ypos_num;
            } else if(svg_frame->ypos_type == SND_CMX_AF_POS_PERCENT) {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_PERCENT;
              packet->audfmt_anim_data.frame_pos.ypos_num  = svg_frame->ypos_num;
            } else if(svg_frame->ypos_type == SND_CMX_AF_POS_LT) {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_LT;
            } else if(svg_frame->ypos_type == SND_CMX_AF_POS_RB) {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_RB;
            } else {
              packet->audfmt_anim_data.frame_pos.ypos_type = CMX_AF_POS_CENTER;
            }

            packet->hdr.cmd                   = CMX_AF_CMD_ANIM_FRAME_ID;
            packet->audfmt_anim_data.type     = CMX_AF_ANIM_SVG;
            packet->audfmt_anim_data.handle   = handle_ptr;
            packet->audfmt_anim_data.position = num;
            cmx_cmd (packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;
#endif /* FEATURE_SVG */

#ifdef FEATURE_SMAF
    case SND_CMX_SMAF_GRAPH_INFO:
      if (handle_ptr != NULL) {
        if (handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) {
          packet = cmx_cmd_get_pkt();
          if (packet != NULL) {
            graph_info = (snd_cmx_smaf_graph_info_type *)data;
            packet->hdr.cmd                         = CMX_AF_CMD_SMAF_GRAPH_INFO;
            packet->audfmt_graph_info.handle        = handle_ptr;
            packet->audfmt_graph_info.info.num      = graph_info->num;
            packet->audfmt_graph_info.info.pos      = graph_info->pos;
            packet->audfmt_graph_info.info.size     = graph_info->size;
            cmx_cmd(packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_SMAF_LED_CONTROL:
      if (handle_ptr != NULL) {
        if((handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) && (data != NULL)) {
          packet  = cmx_cmd_get_pkt ();
          if(packet != NULL) {
			smaf_ctl = (snd_cmx_smaf_led_vib_ctl_type *)data;
            packet->hdr.cmd                          = CMX_AF_CMD_SMAF_LED_CTL;
            packet->audfmt_smaf_led_vib_ctl.handle   = handle_ptr;
			packet->audfmt_smaf_led_vib_ctl.led_ctl  = smaf_ctl->smaf_led_ctl;
            cmx_cmd(packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_SMAF_VIB_CONTROL:
      if (handle_ptr != NULL) {
        if((handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) && (data != NULL)) {
          packet  = cmx_cmd_get_pkt ();
          if(packet != NULL) {
			smaf_ctl = (snd_cmx_smaf_led_vib_ctl_type *)data;
			packet->hdr.cmd                          = CMX_AF_CMD_SMAF_VIB_CTL;
            packet->audfmt_smaf_led_vib_ctl.handle   = handle_ptr;
			packet->audfmt_smaf_led_vib_ctl.vib_ctl  = smaf_ctl->smaf_vib_ctl;
            cmx_cmd(packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;
#endif /* FEATURE_SMAF */

    case SND_CMX_MFI_LED_CONTROL:
      if (handle_ptr != NULL) {
        if((handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) && (data != NULL)) {
          mfi_led_ctl = (snd_cmx_mfi_led_ctl_type *) data;
          packet  = cmx_cmd_get_pkt ();
          if(packet != NULL) {
            packet->hdr.cmd                   = CMX_AF_CMD_MFI_LED_CTL;
            packet->audfmt_mfi_led_ctl.handle = handle_ptr;
            packet->audfmt_mfi_led_ctl.mask   = mfi_led_ctl->led_ctl_mask;
            cmx_cmd(packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;
#endif /* FEATURE_WEBAUDIO */

#if defined(FEATURE_IMELODY) || defined(FEATURE_WEBAUDIO)
    case SND_CMX_AF_LED_CONTROL:
      if (handle_ptr != NULL) {
        if((handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) && (data != NULL)) {
          led_ctl = (snd_cmx_af_led_ctl_type *) data;
          packet  = cmx_cmd_get_pkt ();
          if(packet != NULL) {
            packet->hdr.cmd                 = CMX_AF_CMD_LED_CTL;
            packet->audfmt_led_ctl.handle   = handle_ptr;
            if(!led_ctl->led_enable) {
              packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_OFF;
            } else {
              switch (led_ctl->led_ctl) {
                case SND_CMX_AF_LED_DARK_BLACK:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_DARK_BLACK;
                  break;
                case SND_CMX_AF_LED_DARK_BLUE:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_DARK_BLUE;
                  break;
                case SND_CMX_AF_LED_DARK_GREEN:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_DARK_GREEN;
                  break;
                case SND_CMX_AF_LED_DARK_CYAN:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_DARK_CYAN;
                  break;
                case SND_CMX_AF_LED_DARK_RED:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_DARK_RED;
                  break;
                case SND_CMX_AF_LED_DARK_PURPLE:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_DARK_PURPLE;
                  break;
                case SND_CMX_AF_LED_DARK_YELLOW:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_DARK_YELLOW;
                  break;
                case SND_CMX_AF_LED_DARK_WHITE:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_DARK_WHITE;
                  break;
                case SND_CMX_AF_LED_BRIGHT_BLACK:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_BRIGHT_BLACK;
                  break;
                case SND_CMX_AF_LED_BRIGHT_BLUE:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_BRIGHT_BLUE;
                  break;
                case SND_CMX_AF_LED_BRIGHT_GREEN:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_BRIGHT_GREEN;
                  break;
                case SND_CMX_AF_LED_BRIGHT_CYAN:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_BRIGHT_CYAN;
                  break;
                case SND_CMX_AF_LED_BRIGHT_RED:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_BRIGHT_RED;
                  break;
                case SND_CMX_AF_LED_BRIGHT_PURPLE:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_BRIGHT_PURPLE;
                  break;
                case SND_CMX_AF_LED_BRIGHT_YELLOW:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_BRIGHT_YELLOW;
                  break;
                case SND_CMX_AF_LED_BRIGHT_WHITE:
                  packet->audfmt_led_ctl.led_ctl = CMX_AF_LED_CTL_BRIGHT_WHITE;
                  break;
              }
            }
            cmx_cmd (packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;
#endif /* FEATURE_IMELODY || FEATURE_WEBAUDIO */

#ifdef FEATURE_IMELODY
  case SND_CMX_AF_BACKLIGHT_CONTROL:
    if (handle_ptr != NULL) {
      if((handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) && (data != NULL)) {
        packet  = cmx_cmd_get_pkt ();
        if(packet != NULL) {
          packet->hdr.cmd                         = CMX_AF_CMD_BACKLIGHT_CTL;
          packet->audfmt_blight_ctl.handle        = handle_ptr;
          packet->audfmt_blight_ctl.backlight_ctl = *( (boolean*)data);
          cmx_cmd (packet);
        }
      }
    }
    break;
#endif /* FEATURE_IMELODY */

#ifdef FEATURE_QCP
    case SND_CMX_QCP_SUCCESS:
      if (handle_ptr != NULL) {
        if((handle_ptr->cmd == CMX_QCP_GET_LABL) ||
           (handle_ptr->cmd == CMX_QCP_GET_TEXT) ||
           (handle_ptr->cmd == CMX_QCP_GET_FRAMES) ||
           (handle_ptr->cmd == CMX_QCP_GET_CNFG) ||
           (handle_ptr->cmd == CMX_QCP_GET_DATA_SIZE)) {
          packet = cmx_cmd_get_pkt ();
          if (packet != NULL) {
            packet->hdr.cmd               = CMX_QCP_FUNC_DONE;
            packet->qcp_func_cb.handle    = handle_ptr;
            if(handle_ptr->cmd == CMX_QCP_GET_TEXT) {
              num = MIN(num, *cmx_qcp_func_length);
            }
            packet->qcp_func_cb.num_bytes = num;
            if((handle_ptr->cmd == CMX_QCP_GET_LABL) ||
               (handle_ptr->cmd == CMX_QCP_GET_TEXT)) {
              memcpy(cmx_qcp_func_buffer, data, num);
            }
            packet->qcp_func_cb.buffer    = cmx_qcp_func_buffer;
            cmx_cmd (packet);
          }
        }
#ifdef FEATURE_AUDIO_POSTCARD
        else if(handle_ptr->cmd == CMX_CMD_CREATE_AUDIOP) {
          packet = cmx_cmd_get_pkt ();
          if (packet != NULL) {
            packet->hdr.cmd                   = CMX_CMD_COMPLETE_AUDIOP;
            packet->complete_audiop.status    = CMX_SUCCESS;
            packet->complete_audiop.handle    = handle_ptr;
            packet->complete_audiop.num_bytes = num;
            packet->complete_audiop.position  = *((uint32 *) data);
            cmx_cmd (packet);
          }
        }
#endif /* FEATURE_AUDIO_POSTCARD */
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_QCP_FAILURE:
      if (handle_ptr != NULL) {
        if((handle_ptr->cmd == CMX_QCP_GET_LABL) ||
           (handle_ptr->cmd == CMX_QCP_GET_TEXT) ||
           (handle_ptr->cmd == CMX_QCP_GET_FRAMES) ||
           (handle_ptr->cmd == CMX_QCP_GET_CNFG) ||
           (handle_ptr->cmd == CMX_QCP_GET_DATA_SIZE)) {
          packet = cmx_cmd_get_pkt ();
          if (packet != NULL) {
            packet->hdr.cmd               = CMX_QCP_FUNC_FAIL;
            packet->qcp_func_cb.handle    = handle_ptr;
            packet->qcp_func_cb.num_bytes = 0;
            packet->qcp_func_cb.buffer    = NULL;
            cmx_cmd (packet);
          }
        }
  #ifdef FEATURE_AUDIO_POSTCARD
        else if(handle_ptr->cmd == CMX_CMD_CREATE_AUDIOP) {
          packet = cmx_cmd_get_pkt ();
          if (packet != NULL) {
            packet->hdr.cmd                   = CMX_CMD_COMPLETE_AUDIOP;
            packet->complete_audiop.status    = CMX_FAILURE;
            packet->complete_audiop.handle    = handle_ptr;
            packet->complete_audiop.num_bytes = num;
            packet->complete_audiop.position  = *((uint32 *) data);
            cmx_cmd (packet);
          }
        }
  #endif /* FEATURE_AUDIO_POSTCARD */
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;
#endif

    case SND_CMX_AF_REPEAT:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd                = CMX_AF_CMD_CB;
        packet->audfmt_cb.status       = SND_REPEAT;
        packet->audfmt_cb.client_data  = (void *) handle_ptr;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_AF_FAILURE:
    case SND_CMX_AF_ABORT:
      MSG_HIGH("cmx_af_play_callback SND_CMX_AF_ABORT()",0,0,0);
      if (handle_ptr != NULL) {
        if (data != NULL) {
          extended_status = *((snd_status_type*) data);
        } else if (command == SND_CMX_AF_FAILURE) {
          extended_status = SND_FAILURE;
        } else {
          extended_status = SND_ABORT;
        }

        handle_ptr->status_mask |= CMX_AF_STATUS_MASK_DISABLE_PENDING;
        if(handle_ptr->cmd == CMX_AF_CMD_GET_SPEC) {
          packet = cmx_cmd_get_pkt ();

          if (packet != NULL) {
            packet->hdr.cmd                    = CMX_AF_CMD_GET_SPEC_CB;
            packet->audfmt_get_spec_cb.status  = SND_FAILURE;
            packet->audfmt_get_spec_cb.handle  = handle_ptr;
            cmx_cmd (packet);
          }
        } else if(handle_ptr->cmd == CMX_AF_CMD_GET_TIME) {
          packet = cmx_cmd_get_pkt ();

          if (packet != NULL) {
            packet->hdr.cmd                    = CMX_AF_CMD_GET_TIME_CB;
            packet->audfmt_get_time_cb.status  = extended_status;
            packet->audfmt_get_time_cb.handle  = handle_ptr;
            cmx_cmd (packet);
          }
        } else if((handle_ptr->cmd == CMX_AF_CMD_PLAY) ||
                  (handle_ptr->cmd == CMX_AF_CMD_RINGER)) {
          packet = cmx_cmd_get_pkt ();

          if (packet != NULL) {
            packet->hdr.cmd                = CMX_AF_CMD_PLAY_DONE_CB;
            packet->audfmt_cb.status       = extended_status;
            packet->audfmt_cb.client_data  = handle_ptr;
            cmx_cmd (packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_PLAY_DONE:
      if (handle_ptr != NULL) {
        if (handle_ptr->cmd != CMX_AF_CMD_GET_SPEC) {
          handle_ptr->status_mask |= CMX_AF_STATUS_MASK_DISABLE_PENDING;
          packet = cmx_cmd_get_pkt ();

          if (packet != NULL) {
            packet->hdr.cmd                = CMX_AF_CMD_PLAY_DONE_CB;
            packet->audfmt_cb.client_data  = handle_ptr;
            packet->audfmt_cb.status       = SND_PLAY_DONE;
            cmx_cmd (packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_TIME_TICK:
      if (handle_ptr != NULL) {
        if ((handle_ptr->cmd == CMX_AF_CMD_PLAY) ||
            (handle_ptr->cmd == CMX_AF_CMD_RINGER)) {
          packet = cmx_cmd_get_pkt ();

          if (packet != NULL) {
            packet->hdr.cmd                = CMX_AF_CMD_CB;
            packet->audfmt_cb.client_data  = handle_ptr;
            packet->audfmt_cb.status       = SND_AF_TIME_TICK;
            cmx_cmd (packet);
          }
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_CALC_TIME_RESULT:
      if (handle_ptr != NULL) {
        handle_ptr->status_mask |= CMX_AF_STATUS_MASK_DISABLE_PENDING;
        packet = cmx_cmd_get_pkt ();

        if (packet != NULL) {
          packet->hdr.cmd                    = CMX_AF_CMD_GET_TIME_CB;
          packet->audfmt_get_time_cb.status  = SND_PLAY_DONE;
          packet->audfmt_get_time_cb.handle  = handle_ptr;
          packet->audfmt_get_time_cb.time_ms = num;
          cmx_cmd (packet);
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    case SND_CMX_AF_CALC_TIME_ABORT:
      if (handle_ptr != NULL) {
        handle_ptr->status_mask |= CMX_AF_STATUS_MASK_DISABLE_PENDING;
        packet = cmx_cmd_get_pkt ();

        if (packet != NULL) {
          packet->hdr.cmd                    = CMX_AF_CMD_GET_TIME_CB;
          packet->audfmt_get_time_cb.status  = SND_ABORT;
          packet->audfmt_get_time_cb.handle  = handle_ptr;
          packet->audfmt_get_time_cb.time_ms = num;
          cmx_cmd (packet);
        }
      } else {
        MSG_ERROR( " Command called with NULL handle %d", command, 0, 0);
      }
      break;

    default:
      MSG_ERROR(" Unknown callback", 0, 0, 0);
      break;
  }
} /* end cmx_af_play_callback */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_get_time_cb

DESCRIPTION
  This function is used as a callback function for the midi/WebAudio get
  time service.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_af_get_time_cb (
  const void         *file_access,
  snd_status_type    status,
  uint32             time_ms
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();
  if (packet != NULL) {
    packet->hdr.cmd                    = CMX_AF_CMD_GET_TIME_CB;
    packet->audfmt_get_time_cb.status  = status;
    packet->audfmt_get_time_cb.handle  = (cmx_af_handle_type *) file_access;
    packet->audfmt_get_time_cb.time_ms = time_ms;
    cmx_cmd (packet);
  }
} /* cmx_af_get_time_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_get_spec_cb

DESCRIPTION
  This function is used as a callback function for the midi/WebAudio get
  time service.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_af_get_spec_cb (
  const void         *file_access,
  snd_status_type    status
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();
  if (packet != NULL) {
    packet->hdr.cmd                    = CMX_AF_CMD_GET_SPEC_CB;
    packet->audfmt_get_spec_cb.status  = status;
    packet->audfmt_get_spec_cb.handle  = (cmx_af_handle_type *) file_access;
    cmx_cmd (packet);
  }
} /* cmx_af_get_spec_cb */
/* <EJECT> */
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION cmx_qcp_set_func_cb

DESCRIPTION
  This function is the callback function for QCP set cnfg, labl and
  text command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_qcp_set_func_cb (
  snd_af_data_cmd_enum  command,
  void                  *data,
  void                  *client_data
) {
  snd_qcp_rec_data_struct_type *qcp_data;
  cmx_af_handle_type           *handle_ptr;
  cmx_packet_type              *packet;

  handle_ptr = (cmx_af_handle_type *) client_data;

  switch (command) {
    case SND_CMX_QCP_REC_DATA:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        qcp_data = (snd_qcp_rec_data_struct_type *) data;

        packet->hdr.cmd                 = CMX_QCP_CMD_REC_DATA;
        packet->qcp_rec_data.handle     = handle_ptr;
        packet->qcp_rec_data.link       = (cmx_qcp_dir_type) qcp_data->link;

        /* Convert "content" from SND enum. to CMX enum. */
        switch (qcp_data->content)
        {
          case SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED:
            packet->qcp_rec_data.content =
              CMX_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
            break;

          case SND_QCP_REC_DATA_BUFFER_RECDATA:
            packet->qcp_rec_data.content = CMX_QCP_REC_DATA_BUFFER_RECDATA;
            break;

          default:
            packet->qcp_rec_data.content =
              CMX_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
            MSG_ERROR("No equivalent enum. conversion value.", 0, 0, 0);
            break;
        }

        packet->qcp_rec_data.buffer     = qcp_data->buffer;
        packet->qcp_rec_data.num_bytes  = qcp_data->num_bytes;
        packet->qcp_rec_data.position   = qcp_data->position;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_QCP_SUCCESS:
      packet = cmx_cmd_get_pkt ();
      if (packet != NULL) {
        if(cmx_qcp_set_labl_pend) {
          cmx_qcp_set_labl_pend       = FALSE;
          packet->hdr.cmd             = cmx_qcp_record_status_pending;
          packet->qcp_rec_data.handle = handle_ptr;
        } else {
          packet->hdr.cmd             = CMX_QCP_FUNC_DONE;
          packet->qcp_func_cb.handle  = handle_ptr;
        }
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_QCP_FAILURE:
      packet = cmx_cmd_get_pkt ();
      if (packet != NULL) {
        if(cmx_qcp_set_labl_pend) {
          cmx_qcp_set_labl_pend       = FALSE;
          packet->hdr.cmd             = cmx_qcp_record_status_pending;
          packet->qcp_rec_data.handle = handle_ptr;
        } else {
          packet->hdr.cmd             = CMX_QCP_FUNC_FAIL;
          packet->qcp_func_cb.handle  = handle_ptr;
        }
        cmx_cmd (packet);
      }
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_rec_snd_cb

DESCRIPTION
  This function is the callback function for QCP recording for the sound
  server.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_qcp_rec_snd_cb (
  snd_af_data_cmd_enum  command,
  void                  *data,
  void                  *client_data
) {
  snd_qcp_rec_data_struct_type *qcp_data;
  cmx_af_handle_type           *handle_ptr;
  cmx_packet_type              *packet;

  handle_ptr = (cmx_af_handle_type *) client_data;

  switch (command) {
    case SND_CMX_QCP_REC_GET_BUF:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd                 = CMX_QCP_CMD_REC_GET_BUF;
        packet->qcp_rec_get_buf.handle  = handle_ptr;
        packet->qcp_rec_get_buf.cb_func = (snd_qcp_buf_cb_func_type) data;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_QCP_REC_DATA:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        qcp_data = (snd_qcp_rec_data_struct_type *) data;

        packet->hdr.cmd                 = CMX_QCP_CMD_REC_DATA;
        packet->qcp_rec_data.handle     = handle_ptr;
        packet->qcp_rec_data.link       = (cmx_qcp_dir_type) qcp_data->link;

        /* Convert "content" from SND enum. to CMX enum. */
        switch (qcp_data->content)
        {
          case SND_QCP_REC_DATA_BUFFER_UNCATAGORIZED:
            packet->qcp_rec_data.content =
              CMX_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
            break;

          case SND_QCP_REC_DATA_BUFFER_RECDATA:
            packet->qcp_rec_data.content = CMX_QCP_REC_DATA_BUFFER_RECDATA;
            break;

          default:
            packet->qcp_rec_data.content =
              CMX_QCP_REC_DATA_BUFFER_UNCATAGORIZED;
            MSG_ERROR("No equivalent enum. conversion value.", 0, 0, 0);
            break;
        }

        packet->qcp_rec_data.buffer     = qcp_data->buffer;
        packet->qcp_rec_data.num_bytes  = qcp_data->num_bytes;
        packet->qcp_rec_data.position   = qcp_data->position;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_QCP_REC_STOP:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd             = CMX_QCP_CMD_REC_DONE;
        packet->qcp_rec_data.handle = handle_ptr;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_QCP_REC_AUTO_STOP:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd             = CMX_QCP_CMD_REC_AUTO_STOP;
        packet->qcp_rec_data.handle = handle_ptr;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_QCP_REC_BUFF_ERR:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd             = CMX_QCP_CMD_REC_EFS_DONE;
        packet->qcp_rec_data.handle = handle_ptr;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_QCP_REC_ERROR:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd             = CMX_QCP_CMD_REC_ERROR;
        packet->qcp_rec_data.handle = handle_ptr;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_AF_PAUSE:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd             = CMX_QCP_CMD_REC_PAUSE_S;
        packet->qcp_rec_data.handle = handle_ptr;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_AF_RESUME:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd             = CMX_QCP_CMD_REC_RESUME_S;
        packet->qcp_rec_data.handle = handle_ptr;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_QCP_INT_TIME:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd             = CMX_QCP_CMD_REC_TIME;
        packet->qcp_rec_data.handle = handle_ptr;
        cmx_cmd (packet);
      }
      break;
  }
}
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION cmx_pcm_rec_snd_cb

DESCRIPTION
  This function is the callback function for PCM recording for the sound
  server.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_pcm_rec_snd_cb (
  snd_af_data_cmd_enum  command,
  void                  *data,
  void                  *client_data
) {
  snd_pcm_rec_data_struct_type *pcm_data;
  cmx_af_handle_type           *handle_ptr;
  cmx_packet_type              *packet;

  handle_ptr = (cmx_af_handle_type *) client_data;

  switch (command) {
    case SND_CMX_PCM_REC_DATA:
    case SND_CMX_PCM_REC_HEADER_DATA:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        pcm_data = (snd_pcm_rec_data_struct_type *) data;

        if(command == SND_CMX_PCM_REC_DATA) {
          packet->hdr.cmd                = CMX_PCM_CMD_REC_DATA;
        } else {
          packet->hdr.cmd                = CMX_PCM_CMD_REC_HEADER_DATA;
        }
        packet->pcm_rec_data.handle      = handle_ptr;
        packet->pcm_rec_data.buffer      = pcm_data->rec_data_buf;
        packet->pcm_rec_data.num_bytes   = pcm_data->rec_data_num;
        packet->pcm_rec_data.position    = pcm_data->rec_data_pos;
        packet->pcm_rec_data.buf_cb      = pcm_data->buf_cb_func;
        packet->pcm_rec_data.client_data = pcm_data->client_data;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_PCM_REC_DONE:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd               = CMX_PCM_CMD_REC_DONE;
        packet->pcm_rec_status.handle = handle_ptr;
        packet->pcm_rec_status.status = CMX_DONE;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_AF_FAILURE:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd               = CMX_PCM_CMD_REC_DONE;
        packet->pcm_rec_status.handle = handle_ptr;
        packet->pcm_rec_status.status = CMX_FAILURE;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_PCM_REC_DROP_RESUME:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd               = CMX_PCM_CMD_REC_STATUS;
        packet->pcm_rec_status.handle = handle_ptr;
        packet->pcm_rec_status.status = CMX_REC_DROP_RESUME;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_PCM_REC_DROP_FRAMES:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd               = CMX_PCM_CMD_REC_STATUS;
        packet->pcm_rec_status.handle = handle_ptr;
        packet->pcm_rec_status.status = CMX_REC_DROP_FRAMES;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_PCM_REC_PAUSED:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd               = CMX_PCM_CMD_REC_STATUS;
        packet->pcm_rec_status.handle = handle_ptr;
        packet->pcm_rec_status.status = CMX_REC_PAUSED;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_PCM_REC_RESUMED:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd               = CMX_PCM_CMD_REC_STATUS;
        packet->pcm_rec_status.handle = handle_ptr;
        packet->pcm_rec_status.status = CMX_REC_RESUMED;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_PCM_REC_INTERRUPTED:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd               = CMX_PCM_CMD_REC_STATUS;
        packet->pcm_rec_status.handle = handle_ptr;
        packet->pcm_rec_status.status = CMX_REC_INTERRUPTED;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_PCM_REC_INT_RESUME:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd               = CMX_PCM_CMD_REC_STATUS;
        packet->pcm_rec_status.handle = handle_ptr;
        packet->pcm_rec_status.status = CMX_REC_INT_RESUME;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_AF_TIME_TICK:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd               = CMX_PCM_CMD_REC_STATUS;
        packet->pcm_rec_status.handle = handle_ptr;
        packet->pcm_rec_status.status = CMX_TICK_UPDATE;
        cmx_cmd (packet);
      }
      break;
  }
}
#endif /* FEATURE_PCM_REC */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play_codec_processing

DESCRIPTION
  This function plays audio data whose format must be specified by the user.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_af_active
  cmx_af_stopped

===========================================================================*/
void cmx_audfmt_play_codec_processing
(
  cmx_handle_type              *handle,
  cmx_internal_codec_spec_type *codec_spec,
  cmx_internal_av_sync_type    *av_sync,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data,
  cmx_interface_enum_type      interface_src
) {
  uint32                   counter;
  snd_format_sound_type    *format_sound;
  cmx_af_handle_type       *handle_ptr          = NULL;
  cmx_audfmt_buf_struct    *buffer;
#if defined(FEATURE_AAC)
  snd_aac_format_spec_type  *aac_spec = NULL;
#endif /* FEATURE_AAC */

#ifdef FEATURE_AUDFMT_EVB
  snd_cmx_format_spec_type *snd_codec_spec =
                                        (snd_cmx_format_spec_type*) codec_spec;
#endif /* FEATURE_AUDFMT_EVB */

  if((cmx_af_active != CMX_AF_RINGER) &&
     (cmx_af_active != CMX_AF_QCP_REC) &&
     (cmx_af_active != CMX_AF_AUDIOP) &&
     (cmx_af_active != CMX_AF_GET_SPEC) &&
     (cmx_af_cmd_pend.cmd_pend != CMX_AF_CMD_RINGER) &&
     (handle->source_type < CMX_SOURCE_INVALID)) {

#ifdef FEATURE_XMF
    if(cmx_active_xmf_handle != NULL) {
      cmx_xmf_terminate(cmx_active_xmf_handle, CMX_AF_STOP_PREEMPTED);
      cmx_active_xmf_handle = NULL;
    } else
#endif /* FEATURE_XMF */
    if(cmx_af_active == CMX_AF_PLAY) {
      if(cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_PLAY) {
        for(counter = 0; counter < cmx_af_cmd_pend.play_count; counter++) {
          cmx_free_handle(cmx_af_cmd_pend.para.play[counter].handle);
          if(cmx_af_cmd_pend.para.play[counter].cb_func != NULL) {
            cmx_af_cmd_pend.para.play[counter].cb_func ( CMX_ABORTED,
                         cmx_af_cmd_pend.para.play[counter].client_data, NULL );
          }
        }
      } else if(cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_PLAY_CODEC) {
        cmx_free_handle(cmx_af_cmd_pend.para.play_codec.handle);
        if(cmx_af_cmd_pend.para.play_codec.cb_func != NULL) {
          cmx_af_cmd_pend.para.play_codec.cb_func (
               CMX_ABORTED, cmx_af_cmd_pend.para.play_codec.client_data );
        }
      }
      cmx_af_cmd_pend.cmd_pend                    = CMX_AF_CMD_PLAY_CODEC;
      cmx_af_cmd_pend.play_count                  = 0;
      cmx_af_cmd_pend.para.play_codec.handle      = handle;
      cmx_af_cmd_pend.para.play_codec.cb_func     = cb_func;
      cmx_af_cmd_pend.para.play_codec.client_data = client_data;
      cmx_af_cmd_pend.para.play_codec.codec_spec  = *codec_spec;
      cmx_af_cmd_pend.para.play_codec.av_sync     = *av_sync;
      cmx_af_cmd_pend.para.play_codec.interface_src = interface_src;
      cmx_af_stopped = CMX_AF_STOP_PREEMPTED;
      snd_sound_stop(SND_PRIO_LOW, NULL, NULL);
      return;
    } else if(cmx_af_active == CMX_AF_GET_TIME) {
      cmx_audfmt_get_time_abort_processing ();
    }

    buffer = cmx_get_buffer_struct();
    if(buffer != NULL) {
      cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_AF_CMD_PLAY_CODEC);

      if(handle_ptr != NULL) {
        handle_ptr->cmd                 = CMX_AF_CMD_PLAY_CODEC;
        handle_ptr->buffer              = buffer;
        handle_ptr->cb_type             = CMX_AF_CALLBACK_REFERENCE;
        handle_ptr->cb_func.play_codec  = cb_func;
        handle_ptr->client_data         = client_data;
        format_sound                    = &handle_ptr->sound.format_sound;
        format_sound->type              = SND_FORMAT_SOUND;

        if (interface_src == CMX_INTERFACE_EXTENDED) {
          switch (codec_spec->codec_type.file_type) {
 #ifdef FEATURE_AAC
            case CMX_AF_FILE_AAC_BROADCAST:
              aac_spec = &format_sound->input_spec.aac_spec;
              switch(codec_spec->spec_extended.aac_bc_codec.dual_mono_mode) {
                case CMX_AF_DUAL_MONO_MODE_FL_FR:
                  aac_spec->dual_mono_mode = SND_AF_DUAL_MONO_MODE_FL_FR;
                  break;
                case CMX_AF_DUAL_MONO_MODE_SL_SR:
                  aac_spec->dual_mono_mode = SND_AF_DUAL_MONO_MODE_SL_SR;
                  break;
                case CMX_AF_DUAL_MONO_MODE_SL_FR:
                  aac_spec->dual_mono_mode = SND_AF_DUAL_MONO_MODE_SL_FR;
                  break;
                case CMX_AF_DUAL_MONO_MODE_FL_SR:
                default:
                  aac_spec->dual_mono_mode = SND_AF_DUAL_MONO_MODE_FL_SR;
                  break;
              }
              if (codec_spec->spec_extended.aac_bc_codec.sbr_mode ==
                  CMX_AF_SBR_MODE_ENABLE) {
                aac_spec->sbr_mode = AUDIODEF_AF_SBR_MODE_ENABLE;
              } else {
                aac_spec->sbr_mode = AUDIODEF_AF_SBR_MODE_DISABLE;
              }
              aac_spec->codec_type.file_type = SND_CMX_AF_FILE_AAC;
              aac_spec->format = SND_AAC_DATA_FORMAT_ADTS;
              aac_spec->audio_object = SND_MP4_AUDIO_OBJECT_AAC_LC;
              aac_spec->channel = SND_AAC_CHANNEL_DUAL;
              aac_spec->bit_rate = 512;
              aac_spec->ep_config = 0;
              aac_spec->aac_scalefactor_data_resilience_flag = 0;
              aac_spec->aac_section_data_resilience_flag = 0;
              aac_spec->aac_spectral_data_resilience_flag = 0;
              aac_spec->sample_rate = (snd_sample_rate_type)
                        codec_spec->spec_extended.aac_bc_codec.sample_rate;
              #ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
              handle_ptr->codec_update_cb =
                codec_spec->spec_extended.aac_bc_codec.codec_update_cb_func;
              #endif
              break;
 #endif /* FEATURE_AAC */

            default:
              MSG_ERROR("PLAY_CODEC_EX Not supported codec type",0,0,0);
              cmx_free_af_handle(handle_ptr);
              cmx_free_buffer_struct(buffer);
              if(cb_func != NULL) {
                cb_func(CMX_FAILURE, client_data);
              }
              return;
          }

          format_sound->av_sync.av_sync_interval =
            av_sync->av_sync_extended.av_sync_interval;
          format_sound->av_sync.av_sync_mode =
            (snd_av_sync_interval_type) av_sync->av_sync_extended.av_sync_mode;
          format_sound->av_sync.client_data =
            av_sync->av_sync_extended.client_data;
          format_sound->av_sync.av_sync_ex_cb_func =
            (snd_af_av_sync_ex_cb_func_ptr_type)
            av_sync->av_sync_extended.av_sync_cb_func;
          format_sound->av_sync.av_sync_cb_func = NULL;

        } else {

        switch (codec_spec->codec_type.file_type) {
#ifdef FEATURE_MP3
          case CMX_AF_FILE_MP3:
            format_sound->input_spec.codec_type.file_type = SND_CMX_AF_FILE_MP3;
            break;
#endif /* FEATURE_MP3 */
#ifdef FEATURE_AAC
          case CMX_AF_FILE_AAC:
            aac_spec = &format_sound->input_spec.aac_spec;
            aac_spec->codec_type.file_type = SND_CMX_AF_FILE_AAC;
            aac_spec->dual_mono_mode = SND_AF_DUAL_MONO_MODE_DEFAULT;
            aac_spec->sbr_mode = AUDIODEF_AF_SBR_MODE_DEFAULT;
            aac_spec->format = (snd_aac_data_format_enum_type)
            codec_spec->spec_original.aac_codec.format;
            aac_spec->audio_object = (snd_mp4_audio_object_enum_type)
            codec_spec->spec_original.aac_codec.audio_object;
            aac_spec->channel = (snd_aac_channel_enum_type)
            codec_spec->spec_original.aac_codec.num_channels;
            aac_spec->bit_rate = codec_spec->spec_original.aac_codec.bit_rate;
            aac_spec->ep_config = codec_spec->spec_original.aac_codec.ep_config;
            aac_spec->aac_scalefactor_data_resilience_flag =
       codec_spec->spec_original.aac_codec.aac_scalefactor_data_resilience_flag;
            aac_spec->aac_section_data_resilience_flag =
           codec_spec->spec_original.aac_codec.aac_section_data_resilience_flag;
            aac_spec->aac_spectral_data_resilience_flag =
          codec_spec->spec_original.aac_codec.aac_spectral_data_resilience_flag;
            aac_spec->sample_rate = (snd_sample_rate_type)
              codec_spec->spec_original.aac_codec.sample_rate;
            break;
#endif /* FEATURE_AAC */
#ifdef FEATURE_QCP
          case CMX_AF_FILE_QCP_EVRC:
            format_sound->input_spec.codec_type.file_type =
                                                       SND_CMX_AF_FILE_QCP_EVRC;
            break;
          case CMX_AF_FILE_QCP_4GV_NB:
            format_sound->input_spec.codec_type.file_type =
                                                     SND_CMX_AF_FILE_QCP_4GV_NB;
            break;
          case CMX_AF_FILE_QCP_4GV_WB:
            format_sound->input_spec.codec_type.file_type =
                                                     SND_CMX_AF_FILE_QCP_4GV_WB;
            break;
          case CMX_AF_FILE_QCP_13K:
            format_sound->input_spec.codec_type.file_type =
                                                        SND_CMX_AF_FILE_QCP_13K;
            break;
          case CMX_AF_FILE_QCP_AMR:
            format_sound->input_spec.codec_type.file_type =
                                                        SND_CMX_AF_FILE_QCP_AMR;
            break;
          case CMX_AF_FILE_QCP_EFR:
            format_sound->input_spec.codec_type.file_type =
                                                        SND_CMX_AF_FILE_QCP_EFR;
            break;
          case CMX_AF_FILE_QCP_FR:
            format_sound->input_spec.codec_type.file_type =
                                                         SND_CMX_AF_FILE_QCP_FR;
            break;
          case CMX_AF_FILE_QCP_HR:
            format_sound->input_spec.codec_type.file_type =
                                                         SND_CMX_AF_FILE_QCP_HR;
            break;
          case CMX_AF_FILE_AMR:
            format_sound->input_spec.codec_type.file_type = SND_CMX_AF_FILE_AMR;
            break;
#endif /* FEATURE_QCP */
#ifdef FEATURE_REAL_AUDIO
          case CMX_AF_FILE_RA:
            memcpy(&(format_sound->input_spec.ra_spec),
                   &(codec_spec->spec_original.ra_codec),
                   sizeof(format_sound->input_spec.ra_spec));
            format_sound->input_spec.codec_type.file_type = SND_CMX_AF_FILE_RA;
            break;
#endif /* FEATURE_REAL_AUDIO */
#ifdef FEATURE_WMA
          case CMX_AF_FILE_WMA:
             memcpy( &(format_sound->input_spec.wma_spec),
                     &(codec_spec->spec_original.wma_codec),
                     sizeof(format_sound->input_spec.wma_spec));
            if(format_sound->input_spec.wma_spec.version == 3){
             format_sound->input_spec.codec_type.file_type = SND_CMX_AF_FILE_WMA_PRO;
            }
            else{
             format_sound->input_spec.codec_type.file_type = SND_CMX_AF_FILE_WMA;
            }
            break;
#endif /* FEATURE_WMA */
#ifdef FEATURE_PCM
          case CMX_AF_FILE_WAVE:
            switch(codec_spec->spec_original.wave_codec.format) {
              case CMX_AF_WAVE_FORMAT_LINEAR_PCM:
                format_sound->input_spec.wave_codec.file_type =
                                                            SND_CMX_AF_FILE_PCM;
                break;
              case CMX_AF_WAVE_FORMAT_IMA_ADPCM:
                format_sound->input_spec.wave_codec.file_type =
                                                          SND_CMX_AF_FILE_ADPCM;
                break;
#ifdef FEATURE_VOC_G711
              case CMX_AF_WAVE_FORMAT_ALAW:
                format_sound->input_spec.wave_codec.file_type =
                                                       SND_CMX_AF_FILE_WAV_ALAW;
                break;
              case CMX_AF_WAVE_FORMAT_MULAW:
                format_sound->input_spec.wave_codec.file_type =
                                                      SND_CMX_AF_FILE_WAV_MULAW;
                break;
#endif /* FEATURE_VOC_G711 */
            }
            if(codec_spec->spec_original.wave_codec.sample_format ==
                                                CMX_AF_WAVE_SAMPLE_FMT_UNSIGNED)
            {
              format_sound->input_spec.wave_codec.sample_fmt =
                                            SND_WAVE_SAMPLE_FORMAT_UNSIGNED;
            } else {
              format_sound->input_spec.wave_codec.sample_fmt =
                                              SND_WAVE_SAMPLE_FORMAT_SIGNED;
            }
            format_sound->input_spec.wave_codec.sample_rate     =
                 codec_spec->spec_original.wave_codec.sample_rate;
            format_sound->input_spec.wave_codec.channels        =
                 codec_spec->spec_original.wave_codec.channels;
            format_sound->input_spec.wave_codec.bits_per_sample =
                 codec_spec->spec_original.wave_codec.bits_per_sample;
            break;
#endif /* FEATURE_PCM */
#ifdef FEATURE_AMR_WB_AUDIO_DEC
          case CMX_AF_FILE_AWB:
            format_sound->input_spec.ext_speech_spec.channel_mode =
                                                             SND_CHANNEL_MONO;
            format_sound->input_spec.ext_speech_spec.sample_rate =
                                                        SND_SAMPLE_RATE_16000;
            format_sound->input_spec.codec_type.file_type =
                                                          SND_CMX_AF_FILE_AWB;
            break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
          case CMX_AF_FILE_AMR_WBPLUS:
            memcpy(&(format_sound->input_spec.ext_speech_spec),
                   &(codec_spec->spec_original.ext_speech_codec),
                   sizeof(format_sound->input_spec.ext_speech_spec));
            format_sound->input_spec.codec_type.file_type =
                                                   SND_CMX_AF_FILE_AMR_WBPLUS;
            break;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
#ifdef FEATURE_AUDFMT_EVB
          case CMX_AF_FILE_EVB:
   	     MSG_HIGH("cmx_audfmt_play_codec_processing():CMX request to start EVB playback",0,0,0);
            format_sound->input_spec                      = *snd_codec_spec;
            format_sound->input_spec.codec_type.file_type =
                                                   SND_CMX_AF_FILE_EVB;
	    break;
#endif /* FEATURE_AUDFMT_EVB */
          default:
            MSG_ERROR("Not supported codec type",0,0,0);
            cmx_free_af_handle(handle_ptr);
            cmx_free_buffer_struct(buffer);
            if(cb_func != NULL) {
              cb_func(CMX_FAILURE, client_data);
            }
            return;
        }
          format_sound->av_sync.av_sync_interval =
             av_sync->av_sync_original.av_sync_interval;
          format_sound->av_sync.av_sync_mode =
             (snd_av_sync_interval_type) av_sync->av_sync_original.av_sync_mode;
          format_sound->av_sync.client_data =
             av_sync->av_sync_original.client_data;
          format_sound->av_sync.av_sync_cb_func =
             (snd_af_av_sync_cb_func_ptr_type)
          av_sync->av_sync_original.av_sync_cb_func;
          format_sound->av_sync.av_sync_ex_cb_func = NULL;
        }/* end of if else (interface_src == CMX_INTERFACE_EXTENDED) */

        format_sound->audfmt_handle.audfmt_ptr  = (void *) handle_ptr;
        format_sound->audfmt_handle.data_func   =
                               (snd_af_data_cb_func_type) cmx_af_play_callback;


        cmx_af_active = CMX_AF_PLAY;
        cmx_af_pend   = CMX_AF_PLAY;

        cmx_af_stopped = CMX_AF_STOP_NONE;
        snd_sound_start ( SND_DEVICE_CURRENT, SND_METHOD_MIDI,
                          (snd_sound_type *) format_sound,
                          SND_PRIO_LOW, SND_APATH_LOCAL,
                          (snd_cb_func_ptr_type) cmx_af_snd_play_cb,
                          (void *) handle_ptr                        );

        if(cb_func != NULL) {
          cb_func(CMX_SUCCESS, client_data);
        }
        return;
      } else {
        cmx_free_buffer_struct(buffer);
      }
    }
  }

  cmx_free_handle(handle);
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
} /* end cmx_audfmt_play_codec_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_av_sync_playback_processing

DESCRIPTION
  This function plays audio data whose format must be specified by the user.
  This function is called when cmx_audfmt_avsync_playback() is called by the
  client.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_af_active
  cmx_af_stopped

===========================================================================*/
void cmx_audfmt_avsync_playback_processing
(
  cmx_handle_type              *handle,
  cmx_internal_codec_spec_type *codec_spec,
  cmx_internal_av_sync_type    *av_sync,
  cmx_audfmt_play_cb_func_ptr_type         cb_func,
  void                         *client_data,
  cmx_interface_enum_type      interface_src
) {
  uint32                   counter;
  snd_format_sound_type    *format_sound;
  cmx_af_handle_type       *handle_ptr          = NULL;
  cmx_audfmt_buf_struct    *buffer;
#if defined(FEATURE_AAC)
  snd_aac_format_spec_type  *aac_spec = NULL;
#endif /* FEATURE_AAC */

#ifdef FEATURE_AUDFMT_EVB
  snd_cmx_format_spec_type *snd_codec_spec =
                                        (snd_cmx_format_spec_type*) codec_spec;
#endif /* FEATURE_AUDFMT_EVB */

  if((cmx_af_active != CMX_AF_RINGER) &&
     (cmx_af_active != CMX_AF_QCP_REC) &&
     (cmx_af_active != CMX_AF_AUDIOP) &&
     (cmx_af_active != CMX_AF_GET_SPEC) &&
     (cmx_af_cmd_pend.cmd_pend != CMX_AF_CMD_RINGER) &&
     (handle->source_type < CMX_SOURCE_INVALID)) {

#ifdef FEATURE_XMF
    if(cmx_active_xmf_handle != NULL) {
      cmx_xmf_terminate(cmx_active_xmf_handle, CMX_AF_STOP_PREEMPTED);
      cmx_active_xmf_handle = NULL;
    } else
#endif /* FEATURE_XMF */
    if(cmx_af_active == CMX_AF_PLAY) {
      if(cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_PLAY) {
        for(counter = 0; counter < cmx_af_cmd_pend.play_count; counter++) {
          cmx_free_handle(cmx_af_cmd_pend.para.play[counter].handle);
          if(cmx_af_cmd_pend.para.play[counter].cb_func != NULL) {
            cmx_af_cmd_pend.para.play[counter].cb_func ( CMX_ABORTED,
                         cmx_af_cmd_pend.para.play[counter].client_data, NULL );
          }
        }
      } else if(cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_AVSYNC_PLAYBACK) {
        cmx_free_handle(cmx_af_cmd_pend.para.av_sync_playback.handle);
        if(cmx_af_cmd_pend.para.av_sync_playback.cb_func != NULL) {
          cmx_af_cmd_pend.para.av_sync_playback.cb_func (
               CMX_ABORTED, cmx_af_cmd_pend.para.av_sync_playback.client_data,NULL );
        }
      }
      cmx_af_cmd_pend.cmd_pend                    = CMX_AF_CMD_AVSYNC_PLAYBACK;
      cmx_af_cmd_pend.play_count                  = 0;
      cmx_af_cmd_pend.para.av_sync_playback.handle      = handle;
      cmx_af_cmd_pend.para.av_sync_playback.cb_func     = cb_func;
      cmx_af_cmd_pend.para.av_sync_playback.client_data = client_data;
      cmx_af_cmd_pend.para.av_sync_playback.codec_spec  = *codec_spec;
      cmx_af_cmd_pend.para.av_sync_playback.av_sync     = *av_sync;
      cmx_af_cmd_pend.para.av_sync_playback.interface_src = interface_src;
      cmx_af_stopped = CMX_AF_STOP_PREEMPTED;
      snd_sound_stop(SND_PRIO_LOW, NULL, NULL);
      return;
    } else if(cmx_af_active == CMX_AF_GET_TIME) {
      cmx_audfmt_get_time_abort_processing ();
    }

    buffer = cmx_get_buffer_struct();
    if(buffer != NULL) {
      cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_AF_CMD_AVSYNC_PLAYBACK);

      if(handle_ptr != NULL) {
        handle_ptr->cmd                 = CMX_AF_CMD_AVSYNC_PLAYBACK;
        handle_ptr->buffer              = buffer;
        handle_ptr->cb_type             = CMX_AF_CALLBACK_REFERENCE;
        handle_ptr->cb_func.av_sync_playback = cb_func;
        handle_ptr->client_data         = client_data;
        format_sound                    = &handle_ptr->sound.format_sound;
        format_sound->type              = SND_FORMAT_SOUND;

        if (interface_src == CMX_INTERFACE_EXTENDED) {
          switch (codec_spec->codec_type.file_type) {
 #ifdef FEATURE_AAC
            case CMX_AF_FILE_AAC_BROADCAST:
              aac_spec = &format_sound->input_spec.aac_spec;
              switch(codec_spec->spec_extended.aac_bc_codec.dual_mono_mode) {
                case CMX_AF_DUAL_MONO_MODE_FL_FR:
                  aac_spec->dual_mono_mode = SND_AF_DUAL_MONO_MODE_FL_FR;
                  break;
                case CMX_AF_DUAL_MONO_MODE_SL_SR:
                  aac_spec->dual_mono_mode = SND_AF_DUAL_MONO_MODE_SL_SR;
                  break;
                case CMX_AF_DUAL_MONO_MODE_SL_FR:
                  aac_spec->dual_mono_mode = SND_AF_DUAL_MONO_MODE_SL_FR;
                  break;
                case CMX_AF_DUAL_MONO_MODE_FL_SR:
                default:
                  aac_spec->dual_mono_mode = SND_AF_DUAL_MONO_MODE_FL_SR;
                  break;
              }
              if (codec_spec->spec_extended.aac_bc_codec.sbr_mode ==
                  CMX_AF_SBR_MODE_ENABLE) {
                aac_spec->sbr_mode = AUDIODEF_AF_SBR_MODE_ENABLE;
              } else {
                aac_spec->sbr_mode = AUDIODEF_AF_SBR_MODE_DISABLE;
              }
              aac_spec->codec_type.file_type = SND_CMX_AF_FILE_AAC;
              aac_spec->format = SND_AAC_DATA_FORMAT_ADTS;
              aac_spec->audio_object = SND_MP4_AUDIO_OBJECT_AAC_LC;
              aac_spec->channel = SND_AAC_CHANNEL_DUAL;
              aac_spec->bit_rate = 512;
              aac_spec->ep_config = 0;
              aac_spec->aac_scalefactor_data_resilience_flag = 0;
              aac_spec->aac_section_data_resilience_flag = 0;
              aac_spec->aac_spectral_data_resilience_flag = 0;
              aac_spec->sample_rate = (snd_sample_rate_type)
                        codec_spec->spec_extended.aac_bc_codec.sample_rate;
              #ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
              handle_ptr->codec_update_cb =
                codec_spec->spec_extended.aac_bc_codec.codec_update_cb_func;
              #endif
              break;
 #endif /* FEATURE_AAC */

            default:
              MSG_ERROR("PLAY_CODEC_EX Not supported codec type",0,0,0);
              cmx_free_af_handle(handle_ptr);
              cmx_free_buffer_struct(buffer);
              if(cb_func != NULL) {
                cb_func(CMX_FAILURE, client_data,NULL);
              }
              return;
          }

          format_sound->av_sync.av_sync_interval =
            av_sync->av_sync_extended.av_sync_interval;
          format_sound->av_sync.av_sync_mode =
            (snd_av_sync_interval_type) av_sync->av_sync_extended.av_sync_mode;
          format_sound->av_sync.client_data =
            av_sync->av_sync_extended.client_data;
          format_sound->av_sync.av_sync_ex_cb_func =
            (snd_af_av_sync_ex_cb_func_ptr_type)
            av_sync->av_sync_extended.av_sync_cb_func;
          format_sound->av_sync.av_sync_cb_func = NULL;

        } else {

        switch (codec_spec->codec_type.file_type) {
#ifdef FEATURE_MP3
          case CMX_AF_FILE_MP3:
            format_sound->input_spec.codec_type.file_type = SND_CMX_AF_FILE_MP3;
            break;
#endif /* FEATURE_MP3 */
#ifdef FEATURE_AAC
          case CMX_AF_FILE_AAC:
            aac_spec = &format_sound->input_spec.aac_spec;
            aac_spec->codec_type.file_type = SND_CMX_AF_FILE_AAC;
            aac_spec->dual_mono_mode = SND_AF_DUAL_MONO_MODE_DEFAULT;
            aac_spec->sbr_mode = AUDIODEF_AF_SBR_MODE_DEFAULT;
            aac_spec->format = (snd_aac_data_format_enum_type)
            codec_spec->spec_original.aac_codec.format;
            aac_spec->audio_object = (snd_mp4_audio_object_enum_type)
            codec_spec->spec_original.aac_codec.audio_object;
            aac_spec->channel = (snd_aac_channel_enum_type)
            codec_spec->spec_original.aac_codec.num_channels;
            aac_spec->bit_rate = codec_spec->spec_original.aac_codec.bit_rate;
            aac_spec->ep_config = codec_spec->spec_original.aac_codec.ep_config;
            aac_spec->aac_scalefactor_data_resilience_flag =
       codec_spec->spec_original.aac_codec.aac_scalefactor_data_resilience_flag;
            aac_spec->aac_section_data_resilience_flag =
           codec_spec->spec_original.aac_codec.aac_section_data_resilience_flag;
            aac_spec->aac_spectral_data_resilience_flag =
          codec_spec->spec_original.aac_codec.aac_spectral_data_resilience_flag;
            aac_spec->sample_rate = (snd_sample_rate_type)
              codec_spec->spec_original.aac_codec.sample_rate;
            break;
#endif /* FEATURE_AAC */
#ifdef FEATURE_QCP
          case CMX_AF_FILE_QCP_EVRC:
            format_sound->input_spec.codec_type.file_type =
                                                       SND_CMX_AF_FILE_QCP_EVRC;
            break;
          case CMX_AF_FILE_QCP_4GV_NB:
            format_sound->input_spec.codec_type.file_type =
                                                     SND_CMX_AF_FILE_QCP_4GV_NB;
            break;
          case CMX_AF_FILE_QCP_4GV_WB:
            format_sound->input_spec.codec_type.file_type =
                                                     SND_CMX_AF_FILE_QCP_4GV_WB;
            break;
          case CMX_AF_FILE_QCP_13K:
            format_sound->input_spec.codec_type.file_type =
                                                        SND_CMX_AF_FILE_QCP_13K;
            break;
          case CMX_AF_FILE_QCP_AMR:
            format_sound->input_spec.codec_type.file_type =
                                                        SND_CMX_AF_FILE_QCP_AMR;
            break;
          case CMX_AF_FILE_QCP_EFR:
            format_sound->input_spec.codec_type.file_type =
                                                        SND_CMX_AF_FILE_QCP_EFR;
            break;
          case CMX_AF_FILE_QCP_FR:
            format_sound->input_spec.codec_type.file_type =
                                                         SND_CMX_AF_FILE_QCP_FR;
            break;
          case CMX_AF_FILE_QCP_HR:
            format_sound->input_spec.codec_type.file_type =
                                                         SND_CMX_AF_FILE_QCP_HR;
            break;
          case CMX_AF_FILE_AMR:
            format_sound->input_spec.codec_type.file_type = SND_CMX_AF_FILE_AMR;
            break;
#endif /* FEATURE_QCP */
#ifdef FEATURE_REAL_AUDIO
          case CMX_AF_FILE_RA:
            memcpy(&(format_sound->input_spec.ra_spec),
                   &(codec_spec->spec_original.ra_codec),
                   sizeof(format_sound->input_spec.ra_spec));
            format_sound->input_spec.codec_type.file_type = SND_CMX_AF_FILE_RA;
            break;
#endif /* FEATURE_REAL_AUDIO */
#ifdef FEATURE_WMA
          case CMX_AF_FILE_WMA:
             memcpy( &(format_sound->input_spec.wma_spec),
                     &(codec_spec->spec_original.wma_codec),
                     sizeof(format_sound->input_spec.wma_spec));
            if(format_sound->input_spec.wma_spec.version == 3){
             format_sound->input_spec.codec_type.file_type = SND_CMX_AF_FILE_WMA_PRO;
            }
            else{
             format_sound->input_spec.codec_type.file_type = SND_CMX_AF_FILE_WMA;
            }
            break;
#endif /* FEATURE_WMA */
#ifdef FEATURE_PCM
          case CMX_AF_FILE_WAVE:
            switch(codec_spec->spec_original.wave_codec.format) {
              case CMX_AF_WAVE_FORMAT_LINEAR_PCM:
                format_sound->input_spec.wave_codec.file_type =
                                                            SND_CMX_AF_FILE_PCM;
                break;
              case CMX_AF_WAVE_FORMAT_IMA_ADPCM:
                format_sound->input_spec.wave_codec.file_type =
                                                          SND_CMX_AF_FILE_ADPCM;
                break;
#ifdef FEATURE_VOC_G711
              case CMX_AF_WAVE_FORMAT_ALAW:
                format_sound->input_spec.wave_codec.file_type =
                                                       SND_CMX_AF_FILE_WAV_ALAW;
                break;
              case CMX_AF_WAVE_FORMAT_MULAW:
                format_sound->input_spec.wave_codec.file_type =
                                                      SND_CMX_AF_FILE_WAV_MULAW;
                break;
#endif /* FEATURE_VOC_G711 */
            }
            if(codec_spec->spec_original.wave_codec.sample_format ==
                                                CMX_AF_WAVE_SAMPLE_FMT_UNSIGNED)
            {
              format_sound->input_spec.wave_codec.sample_fmt =
                                            SND_WAVE_SAMPLE_FORMAT_UNSIGNED;
            } else {
              format_sound->input_spec.wave_codec.sample_fmt =
                                              SND_WAVE_SAMPLE_FORMAT_SIGNED;
            }
            format_sound->input_spec.wave_codec.sample_rate     =
                 codec_spec->spec_original.wave_codec.sample_rate;
            format_sound->input_spec.wave_codec.channels        =
                 codec_spec->spec_original.wave_codec.channels;
            format_sound->input_spec.wave_codec.bits_per_sample =
                 codec_spec->spec_original.wave_codec.bits_per_sample;
            break;
#endif /* FEATURE_PCM */
#ifdef FEATURE_AMR_WB_AUDIO_DEC
          case CMX_AF_FILE_AWB:
            format_sound->input_spec.ext_speech_spec.channel_mode =
                                                             SND_CHANNEL_MONO;
            format_sound->input_spec.ext_speech_spec.sample_rate =
                                                        SND_SAMPLE_RATE_16000;
            format_sound->input_spec.codec_type.file_type =
                                                          SND_CMX_AF_FILE_AWB;
            break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
          case CMX_AF_FILE_AMR_WBPLUS:
            memcpy(&(format_sound->input_spec.ext_speech_spec),
                   &(codec_spec->spec_original.ext_speech_codec),
                   sizeof(format_sound->input_spec.ext_speech_spec));
            format_sound->input_spec.codec_type.file_type =
                                                   SND_CMX_AF_FILE_AMR_WBPLUS;
            break;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
#ifdef FEATURE_AUDFMT_EVB
          case CMX_AF_FILE_EVB:
   	     MSG_HIGH("cmx_audfmt_play_codec_processing():CMX request to start EVB playback",0,0,0);
            format_sound->input_spec                      = *snd_codec_spec;
            format_sound->input_spec.codec_type.file_type =
                                                   SND_CMX_AF_FILE_EVB;
	    break;
#endif /* FEATURE_AUDFMT_EVB */
          default:
            MSG_ERROR("Not supported codec type",0,0,0);
            cmx_free_af_handle(handle_ptr);
            cmx_free_buffer_struct(buffer);
            if(cb_func != NULL) {
              cb_func(CMX_FAILURE, client_data,NULL);
            }
            return;
        }
          format_sound->av_sync.av_sync_interval =
             av_sync->av_sync_original.av_sync_interval;
          format_sound->av_sync.av_sync_mode =
             (snd_av_sync_interval_type) av_sync->av_sync_original.av_sync_mode;
          format_sound->av_sync.client_data =
             av_sync->av_sync_original.client_data;
          format_sound->av_sync.av_sync_cb_func =
             (snd_af_av_sync_cb_func_ptr_type)
          av_sync->av_sync_original.av_sync_cb_func;
          format_sound->av_sync.av_sync_ex_cb_func = NULL;
        }/* end of if else (interface_src == CMX_INTERFACE_EXTENDED) */

        format_sound->audfmt_handle.audfmt_ptr  = (void *) handle_ptr;
        format_sound->audfmt_handle.data_func   =
                               (snd_af_data_cb_func_type) cmx_af_play_callback;


        cmx_af_active = CMX_AF_PLAY;
        cmx_af_pend   = CMX_AF_PLAY;

        cmx_af_stopped = CMX_AF_STOP_NONE;
        snd_sound_start ( SND_DEVICE_CURRENT, SND_METHOD_MIDI,
                          (snd_sound_type *) format_sound,
                          SND_PRIO_LOW, SND_APATH_LOCAL,
                          (snd_cb_func_ptr_type) cmx_af_snd_play_cb,
                          (void *) handle_ptr                        );

        if(cb_func != NULL) {
          cb_func(CMX_SUCCESS, client_data,NULL);
        }
        return;
      } else {
        cmx_free_buffer_struct(buffer);
      }
    }
  }

  cmx_free_handle(handle);
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data,NULL);
  }
} /* end cmx_av_sync_playback_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play_processing

DESCRIPTION
  This function issues a command to play an audio file, using header info
  to determine the format.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_af_active
  cmx_af_stopped

===========================================================================*/
void cmx_audfmt_play_processing
(
  cmx_handle_type                  *handle,
  cmx_af_play_param_type           *param,
  cmx_audfmt_play_cb_func_ptr_type cb_func,
  void                             *client_data
) {
  snd_audfmt_sound_type  *audfmt_sound;
  cmx_af_handle_type     *handle_ptr = NULL;
  cmx_audfmt_buf_struct  *buffer;
  uint32                 counter;
  boolean                do_wait = FALSE;

#ifdef FEATURE_XMF
  boolean                do_xmf = FALSE;
#endif /* FEATURE_XMF */

  if((cmx_af_active != CMX_AF_RINGER) &&
     (cmx_af_active != CMX_AF_QCP_REC) &&
     (cmx_af_active != CMX_AF_AUDIOP) &&
     (cmx_af_active != CMX_AF_GET_SPEC) &&
     (cmx_af_cmd_pend.cmd_pend != CMX_AF_CMD_RINGER) &&
     (handle->source_type < CMX_SOURCE_INVALID)) {

#ifdef FEATURE_XMF
    if(cmx_active_xmf_handle != NULL) {
      cmx_xmf_terminate(cmx_active_xmf_handle, CMX_AF_STOP_PREEMPTED);
      cmx_active_xmf_handle = NULL;
    } else
#endif /* FEATURE_XMF */
    {
      if(cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_PLAY_CODEC) {
        cmx_free_handle(cmx_af_cmd_pend.para.play_codec.handle);
        if(cmx_af_cmd_pend.para.play_codec.cb_func != NULL) {
          cmx_af_cmd_pend.para.play_codec.cb_func (
                 CMX_ABORTED, cmx_af_cmd_pend.para.play_codec.client_data );
        }
        cmx_af_cmd_pend.cmd_pend = CMX_CMD_INVALID;
        do_wait                  = TRUE;
      }else if (cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_AVSYNC_PLAYBACK){
        cmx_free_handle(cmx_af_cmd_pend.para.av_sync_playback.handle);
        if(cmx_af_cmd_pend.para.av_sync_playback.cb_func != NULL) {
          cmx_af_cmd_pend.para.av_sync_playback.cb_func (
                 CMX_ABORTED, cmx_af_cmd_pend.para.av_sync_playback.client_data,NULL );
        }
        cmx_af_cmd_pend.cmd_pend = CMX_CMD_INVALID;
        do_wait                  = TRUE;
      } else if(cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_PLAY) {
        if((!param->multiseq_flag) ||
           (!cmx_af_cmd_pend.para.play[0].param.multiseq_flag)) {
          for(counter = 0; counter < cmx_af_cmd_pend.play_count; counter++) {
            cmx_free_handle(cmx_af_cmd_pend.para.play[counter].handle);
            if(cmx_af_cmd_pend.para.play[counter].cb_func != NULL) {
              cmx_af_cmd_pend.para.play[counter].cb_func ( CMX_ABORTED,
                        cmx_af_cmd_pend.para.play[counter].client_data, NULL );
            }
          }
          cmx_af_cmd_pend.cmd_pend   = CMX_CMD_INVALID;
          cmx_af_cmd_pend.play_count = 0;
        }
        do_wait = TRUE;
      } else if((cmx_af_active == CMX_AF_PLAY) ||
                ((cmx_af_active == CMX_AF_PLAY_MULTI) &&
                 (!param->multiseq_flag))) {
        cmx_af_stopped = CMX_AF_STOP_PREEMPTED;
        do_wait        = TRUE;
        snd_sound_stop(SND_PRIO_LOW, NULL, NULL);
      } else if(cmx_af_active == CMX_AF_GET_TIME) {
        do_wait = TRUE;
        cmx_audfmt_get_time_abort_processing ();
      }

      if(do_wait) {
        if(cmx_af_cmd_pend.play_count < CMX_MAX_NUM_SEQUENCES) {
          cmx_af_cmd_pend.cmd_pend = CMX_AF_CMD_PLAY;
          counter                  = cmx_af_cmd_pend.play_count;
          cmx_af_cmd_pend.para.play[counter].handle       = handle;
          cmx_af_cmd_pend.para.play[counter].param        = *param;
          cmx_af_cmd_pend.para.play[counter].cb_func      = cb_func;
          cmx_af_cmd_pend.para.play[counter].client_data  = client_data;
          cmx_af_cmd_pend.play_count++;
        } else {
          cmx_free_handle(handle);
          if(cb_func != NULL) {
            cb_func(CMX_FAILURE, client_data, NULL);
          }
        }
        return;
      }
    }

    buffer = cmx_get_buffer_struct();
    if(buffer != NULL) {
#ifdef FEATURE_XMF
      if( (handle->source_type != CMX_SOURCE_CLIENT) ||
          ( handle->source_type == CMX_SOURCE_CLIENT &&
            handle->client.data_config == CMX_CLIENT_DATA_RANDOM )){
        do_xmf = TRUE;
      }
#endif /* FEATURE_XMF */
      cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_AF_CMD_PLAY);

      if(handle_ptr != NULL) {
        handle_ptr->cmd          = CMX_AF_CMD_PLAY;
        handle_ptr->buffer       = buffer;
        handle_ptr->cb_type      = param->cb_type;
        handle_ptr->cb_func.play = cb_func;
        handle_ptr->client_data  = client_data;
        audfmt_sound             = &handle_ptr->sound.audfmt_sound;
        audfmt_sound->type       = SND_AUDIO_FORMAT_SOUND;
        audfmt_sound->repeat_ms  = 0;

        switch (param->start_position) {
          case CMX_AUDFMT_START_CUE_POINT:
            audfmt_sound->start_position = SND_AUDFMT_START_CUE_POINT;
            break;

          case CMX_AUDFMT_START_CUE_POINT_1:
            audfmt_sound->start_position = SND_AUDFMT_START_CUE_POINT_1;
            break;

          case CMX_AUDFMT_START_CUE_POINT_2:
            audfmt_sound->start_position = SND_AUDFMT_START_CUE_POINT_2;
            break;

          case CMX_AUDFMT_START_CUE_POINT_3:
            audfmt_sound->start_position = SND_AUDFMT_START_CUE_POINT_3;
            break;

          case CMX_AUDFMT_START_CUE_POINT_4:
            audfmt_sound->start_position = SND_AUDFMT_START_CUE_POINT_4;
            break;

          case CMX_AUDFMT_START_CUE_POINT_5:
            audfmt_sound->start_position = SND_AUDFMT_START_CUE_POINT_5;
            break;

          case CMX_AUDFMT_START_PATH_FORWARD:
            audfmt_sound->start_position = SND_AUDFMT_START_PATH_FORWARD;
            break;

          case CMX_AUDFMT_START_PATH_REVERSE:
            audfmt_sound->start_position = SND_AUDFMT_START_PATH_REVERSE;
            break;

          case CMX_AUDFMT_START_PATH_BOTH:
            audfmt_sound->start_position = SND_AUDFMT_START_PATH_BOTH;
            break;

          case CMX_AUDFMT_START_PATH_FORWARD_INCALL:
            audfmt_sound->start_position = SND_AUDFMT_START_PATH_FORWARD_INCALL;
            break;

          case CMX_AUDFMT_START_PATH_BOTH_INCALL:
            audfmt_sound->start_position = SND_AUDFMT_START_PATH_BOTH_INCALL;
            break;

          case CMX_AUDFMT_START_NORMAL:
          default:
            audfmt_sound->start_position = SND_AUDFMT_START_NORMAL;
            break;
        }

        if(param->repeat_state == CMX_AUDFMT_REPEAT_CUEP) {
          audfmt_sound->param.cuepoint_enable = TRUE;
          audfmt_sound->param.repeat_enable   = FALSE;
        } else if(param->repeat_state == CMX_AUDFMT_REPEAT_ENABLED) {
          audfmt_sound->param.cuepoint_enable = FALSE;
          audfmt_sound->param.repeat_enable   = TRUE;
        } else {
          audfmt_sound->param.cuepoint_enable = FALSE;
          audfmt_sound->param.repeat_enable   = FALSE;
        }
        audfmt_sound->param.repeat_ms   = param->repeat_ms;
        audfmt_sound->param.repeat_cnt  = param->repeat_count;
        audfmt_sound->param.byte_offset = 0;

        audfmt_sound->audfmt_handle.audfmt_ptr  = (void *) handle_ptr;
        audfmt_sound->audfmt_handle.data_func   =
                               (snd_af_data_cb_func_type) cmx_af_play_callback;

        if(param->multiseq_flag) {
          cmx_af_active                     = CMX_AF_PLAY_MULTI;
          cmx_af_pend                       = CMX_AF_PLAY_MULTI;
          audfmt_sound->param.multiseq_flag = TRUE;
        } else {
          cmx_af_active                     = CMX_AF_PLAY;
          cmx_af_pend                       = CMX_AF_PLAY;
          audfmt_sound->param.multiseq_flag = FALSE;
        }

        cmx_af_stopped = CMX_AF_STOP_NONE;

#ifdef FEATURE_XMF
        if((!param->multiseq_flag) && (do_xmf)) {
          cmx_active_xmf_handle = handle_ptr;
          xmf_verify(handle_ptr);
        } else
#endif /* FEATURE_XMF */
        {
          snd_sound_start ( SND_DEVICE_CURRENT, SND_METHOD_MIDI,
                            (snd_sound_type *)audfmt_sound,
                            SND_PRIO_LOW, SND_APATH_LOCAL,
                            (snd_cb_func_ptr_type) cmx_af_snd_play_cb,
                            (void *) handle_ptr );

          if(cb_func != NULL) {
            cb_func(CMX_SUCCESS, client_data, NULL);
          }
        }
        return;
      } else {
        cmx_free_buffer_struct(buffer);
      }
    }
  }

  cmx_free_handle(handle);
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data, NULL);
  }
} /* end cmx_audfmt_play_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_ringer_processing

DESCRIPTION
  This function plays a midi ringer.


DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_af_active
  cmx_af_stopped

===========================================================================*/
void cmx_audfmt_ringer_processing
(
  cmx_handle_type               *handle,
  uint16                        repeat_timer,
  cmx_audfmt_start_enum_type    start_position,
  cmx_cb_func_ptr_type          cb_func,
  void                          *client_data
) {
  snd_audfmt_sound_type *audfmt_sound;
  cmx_af_handle_type    *handle_ptr = NULL;
  cmx_audfmt_buf_struct *buffer;
  uint32                counter;
#ifdef FEATURE_XMF
  boolean                do_xmf = FALSE;
#endif /* FEATURE_XMF */

  if((handle->source_type < CMX_SOURCE_INVALID) &&
     (cmx_af_active != CMX_AF_QCP_REC) &&
     (cmx_af_active != CMX_AF_GET_SPEC) &&
     (cmx_af_active != CMX_AF_AUDIOP)) {
#ifdef FEATURE_XMF
    if(cmx_active_xmf_handle != NULL) {
      cmx_xmf_terminate(cmx_active_xmf_handle, CMX_AF_STOP_PREEMPTED);
      cmx_active_xmf_handle = NULL;
    } else
#endif /* FEATURE_XMF */
    if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_RINGER)) {
      if(cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_RINGER) {
        cmx_free_handle(cmx_af_cmd_pend.para.ringer.handle);
        if(cmx_af_cmd_pend.para.ringer.cb_func != NULL) {
          cmx_af_cmd_pend.para.ringer.cb_func (
               CMX_ABORTED, cmx_af_cmd_pend.para.ringer.client_data );
        }
      } else if(cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_PLAY) {
        for(counter = 0; counter < cmx_af_cmd_pend.play_count; counter++) {
          cmx_free_handle(cmx_af_cmd_pend.para.play[counter].handle);
          if(cmx_af_cmd_pend.para.play[counter].cb_func != NULL) {
            cmx_af_cmd_pend.para.play[counter].cb_func ( CMX_ABORTED,
                         cmx_af_cmd_pend.para.play[counter].client_data, NULL );
          }
        }
      } else if(cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_PLAY_CODEC) {
        cmx_free_handle(cmx_af_cmd_pend.para.play_codec.handle);
        if(cmx_af_cmd_pend.para.play_codec.cb_func != NULL) {
          cmx_af_cmd_pend.para.play_codec.cb_func (
               CMX_ABORTED, cmx_af_cmd_pend.para.play_codec.client_data );
        }
      } else if (cmx_af_cmd_pend.cmd_pend == CMX_AF_CMD_AVSYNC_PLAYBACK){
        cmx_free_handle(cmx_af_cmd_pend.para.av_sync_playback.handle);
        if(cmx_af_cmd_pend.para.av_sync_playback.cb_func != NULL) {
          cmx_af_cmd_pend.para.av_sync_playback.cb_func (
                 CMX_ABORTED, cmx_af_cmd_pend.para.av_sync_playback.client_data,NULL );
        }
      }
      cmx_af_cmd_pend.cmd_pend                    = CMX_AF_CMD_RINGER;
      cmx_af_cmd_pend.play_count                  = 0;
      cmx_af_cmd_pend.para.ringer.handle          = handle;
      cmx_af_cmd_pend.para.ringer.repeat_timer    = repeat_timer;
      cmx_af_cmd_pend.para.ringer.start_position  = start_position;
      cmx_af_cmd_pend.para.ringer.cb_func         = cb_func;
      cmx_af_cmd_pend.para.ringer.client_data     = client_data;

      cmx_af_stopped = CMX_AF_STOP_PREEMPTED;
      if(cmx_af_active == CMX_AF_PLAY) {
        snd_sound_stop(SND_PRIO_LOW, NULL, NULL);
      } else {
        snd_sound_stop(SND_PRIO_MED, NULL, NULL);
      }
      return;
    } else if(cmx_af_active == CMX_AF_GET_TIME) {
      cmx_audfmt_get_time_abort_processing ();
    }

    buffer = cmx_get_buffer_struct();
    if(buffer != NULL) {
#ifdef FEATURE_XMF
      if( (handle->source_type != CMX_SOURCE_CLIENT) ||
          ( handle->source_type == CMX_SOURCE_CLIENT &&
            handle->client.data_config == CMX_CLIENT_DATA_RANDOM )){
        do_xmf = TRUE;
      }
#endif /* FEATURE_XMF */

      cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_AF_CMD_RINGER);

      if(handle_ptr != NULL) {
        handle_ptr->cmd              = CMX_AF_CMD_RINGER;
        handle_ptr->buffer           = buffer;
        handle_ptr->cb_type          = CMX_AF_CALLBACK_NONE;
        handle_ptr->cb_func.ringer   = cb_func;
        handle_ptr->client_data      = client_data;
        audfmt_sound                 = &handle_ptr->sound.audfmt_sound;
        audfmt_sound->type           = SND_AUDIO_FORMAT_SOUND;
        audfmt_sound->repeat_ms      = repeat_timer;
        audfmt_sound->param.multiseq_flag = FALSE;

        switch (start_position) {
          case CMX_AUDFMT_START_CUE_POINT:
            audfmt_sound->start_position = SND_AUDFMT_START_CUE_POINT;
            break;

          case CMX_AUDFMT_START_CUE_POINT_1:
            audfmt_sound->start_position = SND_AUDFMT_START_CUE_POINT_1;
            break;

          case CMX_AUDFMT_START_CUE_POINT_2:
            audfmt_sound->start_position = SND_AUDFMT_START_CUE_POINT_2;
            break;

          case CMX_AUDFMT_START_CUE_POINT_3:
            audfmt_sound->start_position = SND_AUDFMT_START_CUE_POINT_3;
            break;

          case CMX_AUDFMT_START_CUE_POINT_4:
            audfmt_sound->start_position = SND_AUDFMT_START_CUE_POINT_4;
            break;

          case CMX_AUDFMT_START_CUE_POINT_5:
            audfmt_sound->start_position = SND_AUDFMT_START_CUE_POINT_5;
            break;

          case CMX_AUDFMT_START_PATH_FORWARD:
            audfmt_sound->start_position = SND_AUDFMT_START_PATH_FORWARD;
            break;

          case CMX_AUDFMT_START_PATH_REVERSE:
            audfmt_sound->start_position = SND_AUDFMT_START_PATH_REVERSE;
            break;

          case CMX_AUDFMT_START_PATH_BOTH:
            audfmt_sound->start_position = SND_AUDFMT_START_PATH_BOTH;
            break;

          case CMX_AUDFMT_START_PATH_FORWARD_INCALL:
            audfmt_sound->start_position = SND_AUDFMT_START_PATH_FORWARD_INCALL;
            break;

          case CMX_AUDFMT_START_PATH_BOTH_INCALL:
            audfmt_sound->start_position = SND_AUDFMT_START_PATH_BOTH_INCALL;
            break;

          case CMX_AUDFMT_START_NORMAL:
          default:
            audfmt_sound->start_position = SND_AUDFMT_START_NORMAL;
            break;
        }
        audfmt_sound->param.byte_offset = 0;

        audfmt_sound->audfmt_handle.audfmt_ptr  = (void *) handle_ptr;
        audfmt_sound->audfmt_handle.data_func   =
                               (snd_af_data_cb_func_type) cmx_af_play_callback;

        cmx_af_active  = CMX_AF_RINGER;
        cmx_af_pend    = CMX_AF_RINGER;
        cmx_af_stopped = CMX_AF_STOP_NONE;

#ifdef FEATURE_XMF
        if(do_xmf) {
          cmx_active_xmf_handle = handle_ptr;
          xmf_verify(handle_ptr);
        } else
#endif /* FEATURE_XMF */
        {
          snd_sound_start ( SND_DEVICE_CURRENT, SND_METHOD_MIDI,
                          (snd_sound_type *)audfmt_sound,
                          SND_PRIO_MED, SND_APATH_LOCAL,
                          (snd_cb_func_ptr_type) cmx_af_snd_play_cb,
                          (void *) handle_ptr                        );

          if(cb_func != NULL) {
            cb_func(CMX_SUCCESS, client_data);
          }
        }
        return;
      } else {
        cmx_free_buffer_struct(buffer);
      }
    }
  }

  cmx_free_handle(handle);
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
} /* end cmx_audfmt_ringer_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_stop_processing

DESCRIPTION
  This function stops midi/WebAudio playback.


DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_af_active
  cmx_af_stopped

===========================================================================*/
void cmx_audfmt_stop_processing (
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI) ||
     (cmx_af_active == CMX_AF_RINGER)) {
    if(cb_func != NULL) {
      cb_func(CMX_SUCCESS, client_data);
    }
#ifdef FEATURE_XMF
    if(cmx_active_xmf_handle != NULL) {
      cmx_xmf_terminate(cmx_active_xmf_handle, CMX_AF_STOP_CLIENT_ABORT);
      cmx_active_xmf_handle = NULL;
      cmx_af_active         = CMX_AF_NONE;
    } else
#endif /* FEATURE_XMF */
    {
      cmx_af_stopped = CMX_AF_STOP_CLIENT_ABORT;
      if((cmx_af_active == CMX_AF_PLAY) ||
         (cmx_af_active == CMX_AF_PLAY_MULTI)) {
        snd_sound_stop(SND_PRIO_LOW, NULL, NULL);
      } else {
        snd_sound_stop(SND_PRIO_MED, NULL, NULL);
      }
    }
  } else {
    if(cb_func != NULL) {
      cb_func(CMX_FAILURE, client_data);
    }
  }
} /* end cmx_audfmt_stop_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_rewind_processing

DESCRIPTION
  This function rewinds midi/WebAudio playback the indicated number of
  milliseconds.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_rewind_processing
(
  uint32               time_ms,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cb_func != NULL) {
    if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
      cb_func(CMX_SUCCESS, client_data);
    } else {
      cb_func(CMX_FAILURE, client_data);
    }
  }
  if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
    snd_sound_rewind(SND_PRIO_LOW, time_ms, NULL, NULL);
  }
} /* end cmx_audfmt_rewind_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_fforward_processing

DESCRIPTION
  This function fast forwards midi/WebAudio playback the indicated number
  of milliseconds.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_fforward_processing
(
  uint32               time_ms,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cb_func != NULL) {
    if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
      cb_func(CMX_SUCCESS, client_data);
    } else {
      cb_func(CMX_FAILURE, client_data);
    }
  }
  if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
    snd_sound_fforward(SND_PRIO_LOW,time_ms,NULL,NULL);
  }
} /* end cmx_audfmt_fforward_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pause_processing

DESCRIPTION
  This function pauses/resumes midi/WebAudio playback.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_pause_processing
(
  boolean              pause,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cb_func != NULL) {
    if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
      cb_func(CMX_SUCCESS, client_data);
    } else {
      cb_func(CMX_FAILURE, client_data);
    }
  }
  if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
    snd_sound_pause(SND_PRIO_LOW,pause,NULL,NULL);
  }
} /* end cmx_audfmt_pause_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_flush_processing

DESCRIPTION
  This functions discards the current data buffers and restarts playing,
  by requesting the CMX client for more data.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_flush_processing
(
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cb_func != NULL) {
    if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
      cb_func(CMX_SUCCESS, client_data);
    } else {
      cb_func(CMX_FAILURE, client_data);
    }
  }
  if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
    snd_sound_flush(SND_PRIO_LOW, NULL, NULL);
  }
} /* end cmx_audfmt_flush_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seek_pause_processing

DESCRIPTION
  This function prepares for a rewind/fast-forward command for
  midi/WebAudio playback.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_seek_pause_processing (
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cb_func != NULL) {
    if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
      cb_func(CMX_SUCCESS, client_data);
    } else {
      cb_func(CMX_FAILURE, client_data);
    }
  }
  if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
    snd_sound_seek_pause(SND_PRIO_LOW,NULL,NULL);
  }
} /* end cmx_audfmt_seek_pause_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_tempo_processing

DESCRIPTION
  This function modifies the midi/WebAudio playback tempo.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_tempo_processing
(
  uint32               tempo_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cb_func != NULL) {
    if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
      cb_func(CMX_SUCCESS, client_data);
    } else {
      cb_func(CMX_FAILURE, client_data);
    }
  }
  if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
    snd_sound_tempo(SND_PRIO_LOW,tempo_factor,NULL,NULL);
  }
} /* end cmx_audfmt_tempo_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_tune_processing

DESCRIPTION
  This function modifies the midi/WebAudio playback tune.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_tune_processing
(
  uint32               tune_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cb_func != NULL) {
    if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
      cb_func(CMX_SUCCESS, client_data);
    } else {
      cb_func(CMX_FAILURE, client_data);
    }
  }
  if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
    snd_sound_tune(SND_PRIO_LOW, (uint8) tune_factor,NULL,NULL);
  }
} /* end cmx_audfmt_tune_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pan_processing

DESCRIPTION
  This function modifies the playback stereo pan setting.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_pan_processing
(
  uint32               pan_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cb_func != NULL) {
    if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
      cb_func(CMX_SUCCESS, client_data);
    } else {
      cb_func(CMX_FAILURE, client_data);
    }
  }
  if((cmx_af_active == CMX_AF_PLAY) || (cmx_af_active == CMX_AF_PLAY_MULTI)) {
    snd_sound_pan(SND_PRIO_LOW, (uint8) pan_factor, NULL,NULL);
  }
} /* end cmx_audfmt_pan_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_stop_processing

DESCRIPTION
  This function stops the indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_seq_stop_processing
(
  void                 *sequence,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  cmx_af_handle_type *handle_ptr = (cmx_af_handle_type *) sequence;

  if(cmx_af_active == CMX_AF_PLAY_MULTI) {
    if((sequence >= (void *) cmx_audfmt_handle) &&
       (sequence <= (void *) (cmx_audfmt_handle + CMX_NUM_AF_HANDLES))) {

      handle_ptr->status_mask |= CMX_AF_STATUS_MASK_PLAY_ABORTED;
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_sequence_stop(sequence, NULL, NULL);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
} /* end cmx_audfmt_seq_stop_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_pause_processing

DESCRIPTION
  This function pauses/resumes the indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_seq_pause_processing
(
  void                 *sequence,
  boolean              pause_flag,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cmx_af_active == CMX_AF_PLAY_MULTI) {
    if((sequence >= (void *) cmx_audfmt_handle) &&
       (sequence <= (void *) (cmx_audfmt_handle + CMX_NUM_AF_HANDLES))) {

      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_sequence_pause(sequence, pause_flag, NULL, NULL);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
} /* end cmx_audfmt_seq_pause_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_fforward_processing

DESCRIPTION
  This function fast-forwards the indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_seq_fforward_processing
(
  void                 *sequence,
  uint32               ms_forward,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cmx_af_active == CMX_AF_PLAY_MULTI) {
    if((sequence >= (void *) cmx_audfmt_handle) &&
       (sequence <= (void *) (cmx_audfmt_handle + CMX_NUM_AF_HANDLES))) {

      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_sequence_fforward(sequence, ms_forward, NULL, NULL);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
} /* end cmx_audfmt_seq_fforward_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_rewind_processing

DESCRIPTION
  This function rewinds the indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_seq_rewind_processing
(
  void                 *sequence,
  uint32               ms_rewind,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cmx_af_active == CMX_AF_PLAY_MULTI) {
    if((sequence >= (void *) cmx_audfmt_handle) &&
       (sequence <= (void *) (cmx_audfmt_handle + CMX_NUM_AF_HANDLES))) {

      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_sequence_rewind(sequence, ms_rewind, NULL, NULL);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
} /* end cmx_audfmt_seq_rewind_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_tempo_processing

DESCRIPTION
  This function modifies the playback tempo of indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_seq_tempo_processing
(
  void                 *sequence,
  uint32               tempo_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cmx_af_active == CMX_AF_PLAY_MULTI) {
    if((sequence >= (void *) cmx_audfmt_handle) &&
       (sequence <= (void *) (cmx_audfmt_handle + CMX_NUM_AF_HANDLES))) {

      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_sequence_set_tempo(sequence, tempo_factor, NULL, NULL);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
} /* end cmx_audfmt_seq_tempo_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_tune_processing

DESCRIPTION
  This function modifies the playback tune of indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_seq_tune_processing
(
  void                 *sequence,
  uint32               tune_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cmx_af_active == CMX_AF_PLAY_MULTI) {
    if((sequence >= (void *) cmx_audfmt_handle) &&
       (sequence <= (void *) (cmx_audfmt_handle + CMX_NUM_AF_HANDLES))) {

      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_sequence_set_tune(sequence, tune_factor, NULL, NULL);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
} /* end cmx_audfmt_seq_tune_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_pan_processing

DESCRIPTION
  This function modifies the playback pan of indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_seq_pan_processing
(
  void                 *sequence,
  uint16               pan_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cmx_af_active == CMX_AF_PLAY_MULTI) {
    if((sequence >= (void *) cmx_audfmt_handle) &&
       (sequence <= (void *) (cmx_audfmt_handle + CMX_NUM_AF_HANDLES))) {

      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_sequence_set_pan(sequence, pan_factor, NULL, NULL);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
} /* end cmx_audfmt_seq_pan_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_vol_processing

DESCRIPTION
  This function modifies the playback vol of indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_seq_vol_processing
(
  void                 *sequence,
  uint16               vol_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  if(cmx_af_active == CMX_AF_PLAY_MULTI) {
    if((sequence >= (void *) cmx_audfmt_handle) &&
       (sequence <= (void *) (cmx_audfmt_handle + CMX_NUM_AF_HANDLES))) {

      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_sequence_set_vol(sequence, vol_factor, NULL, NULL);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
} /* end cmx_audfmt_seq_vol_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_time_processing

DESCRIPTION
  This function gets the playback time of the indicated midi/WebAudio file.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_get_time_processing
(
  cmx_handle_type                   *handle,
  cmx_audfmt_time_cb_func_ptr_type  cb_func,
  void                              *client_data
) {
  snd_audfmt_sound_type *audfmt_sound;
  cmx_af_handle_type    *handle_ptr = NULL;
  cmx_audfmt_buf_struct *buffer;
#ifdef FEATURE_XMF
  boolean                do_xmf = FALSE;
#endif /* FEATURE_XMF */

  if(((cmx_af_active == CMX_AF_NONE) || (cmx_af_active == CMX_AF_GET_TIME)) &&
     (cmx_af_cmd_pend.cmd_pend == CMX_CMD_INVALID)) {
    if(handle->source_type < CMX_SOURCE_INVALID) {

#ifdef FEATURE_XMF
      if(cmx_active_xmf_handle != NULL) {
        cmx_xmf_terminate(cmx_active_xmf_handle, CMX_AF_STOP_PREEMPTED);
        cmx_active_xmf_handle = NULL;
      }
#endif /* FEATURE_XMF */

      buffer = cmx_get_buffer_struct();
      if(buffer != NULL) {
#ifdef FEATURE_XMF
        if( (handle->source_type != CMX_SOURCE_CLIENT) ||
            ( handle->source_type == CMX_SOURCE_CLIENT &&
              handle->client.data_config == CMX_CLIENT_DATA_RANDOM )){
          do_xmf = TRUE;
        }
#endif /* FEATURE_XMF */
        cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_AF_CMD_GET_TIME);

        if(handle_ptr != NULL) {
          handle_ptr->cmd                            = CMX_AF_CMD_GET_TIME;
          handle_ptr->buffer                         = buffer;
          handle_ptr->cb_func.time                   = cb_func;
          handle_ptr->client_data                    = client_data;

          audfmt_sound = &handle_ptr->sound.audfmt_sound;
          audfmt_sound->type                      = SND_AUDIO_FORMAT_SOUND;
          audfmt_sound->repeat_ms                 = 0;
          audfmt_sound->start_position            = SND_AUDFMT_START_NORMAL;
          audfmt_sound->param.byte_offset         = 0;
          audfmt_sound->audfmt_handle.audfmt_ptr  = (void *) handle_ptr;
          audfmt_sound->audfmt_handle.data_func   =
                              (snd_af_data_cb_func_type) cmx_af_play_callback;

          cmx_af_active = CMX_AF_GET_TIME;
          cmx_af_gt_active++;

#ifdef FEATURE_XMF
          if(do_xmf) {
            cmx_active_xmf_handle = handle_ptr;
            xmf_verify(handle_ptr);
          } else
#endif /* FEATURE_XMF */
          {
            snd_sound_get_time ( (snd_sound_type *) audfmt_sound,
                              (snd_time_cb_func_ptr_type) cmx_af_get_time_cb,
                              (void *) handle_ptr );
          }
          return;
        } else {
          cmx_free_buffer_struct(buffer);
        }
      }
    }
  }

  cmx_free_handle(handle);
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data, 0);
  }

} /* end cmx_audfmt_get_time_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_time_abort_processing

DESCRIPTION
  This function aborts the midi/WebAudio get time service.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_get_time_abort_processing ( void )
{
  if(cmx_af_active == CMX_AF_GET_TIME) {
#ifdef FEATURE_XMF
    if(cmx_active_xmf_handle != NULL) {
      cmx_xmf_terminate(cmx_active_xmf_handle, CMX_AF_STOP_CLIENT_ABORT);
      cmx_active_xmf_handle = NULL;
      if(cmx_af_gt_active == 0) {
        cmx_af_active = CMX_AF_NONE;
      }
    }
#endif /* FEATURE_XMF */
    snd_audfmt_get_time_abort(NULL, NULL);
  }
} /* end cmx_audfmt_get_time_abort_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_spec_processing

DESCRIPTION
  This function gets the playback time of the indicated midi/WebAudio file.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_get_spec_processing
(
  cmx_handle_type                  *handle,
  cmx_audfmt_spec_cb_func_ptr_type cb_func,
  void                             *client_data
) {
  snd_audfmt_sound_type *audfmt_sound;
  cmx_af_handle_type    *handle_ptr = NULL;
  cmx_audfmt_buf_struct *buffer;
#ifdef FEATURE_XMF
  boolean                do_xmf = FALSE;
#endif /* FEATURE_XMF */

  if((cmx_af_active == CMX_AF_NONE) &&
     (cmx_af_cmd_pend.cmd_pend == CMX_CMD_INVALID)) {
    if(handle->source_type < CMX_SOURCE_INVALID) {

#ifdef FEATURE_XMF
      if(cmx_active_xmf_handle != NULL) {
        cmx_xmf_terminate(cmx_active_xmf_handle, CMX_AF_STOP_PREEMPTED);
        cmx_active_xmf_handle = NULL;
      }
#endif /* FEATURE_XMF */

      buffer = cmx_get_buffer_struct();
      if(buffer != NULL) {
#ifdef FEATURE_XMF
        if( (handle->source_type != CMX_SOURCE_CLIENT) ||
            ( handle->source_type == CMX_SOURCE_CLIENT &&
              handle->client.data_config == CMX_CLIENT_DATA_RANDOM )){
          do_xmf = TRUE;
        }
#endif /* FEATURE_XMF */

        cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_AF_CMD_GET_SPEC);

        if(handle_ptr != NULL) {
          handle_ptr->cmd                         = CMX_AF_CMD_GET_SPEC;
          handle_ptr->buffer                      = buffer;
          handle_ptr->cb_func.spec                = cb_func;
          handle_ptr->client_data                 = client_data;

          audfmt_sound = &handle_ptr->sound.audfmt_sound;
          audfmt_sound->type                      = SND_AUDIO_FORMAT_SOUND;
          audfmt_sound->repeat_ms                 = 0;
          audfmt_sound->start_position            = SND_AUDFMT_START_NORMAL;
          audfmt_sound->param.byte_offset         = 0;
          audfmt_sound->audfmt_handle.audfmt_ptr  = (void *) handle_ptr;
          audfmt_sound->audfmt_handle.data_func   =
                              (snd_af_data_cb_func_type) cmx_af_play_callback;

          cmx_af_active = CMX_AF_GET_SPEC;

#ifdef FEATURE_XMF
          if( do_xmf ) {
            cmx_active_xmf_handle = handle_ptr;
            xmf_verify(handle_ptr);
          } else
#endif /* FEATURE_XMF */
          {
            snd_sound_get_specs((snd_sound_type *)audfmt_sound,
                              cmx_af_get_spec_cb, handle_ptr );
          }
          return;
        } else {
          cmx_free_buffer_struct(buffer);
        }
      }
    }
  }

  cmx_free_handle(handle);
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data, 0);
  }
} /* end cmx_audfmt_get_spec_processing */

/* <EJECT> */
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION cmx_qcp_get_func_processing

DESCRIPTION
  This function retrieves the requested information from a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_get_func_processing (
  cmx_cmd_type           cmd,
  cmx_handle_type        *handle,
  uint32                 *length,
  uint8                  *buffer,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
) {
  cmx_af_handle_type     *handle_ptr = NULL;
  snd_af_access_type     *access_ptr;
  cmx_audfmt_buf_struct  *buf_struct;
  snd_qcp_func_enum_type snd_func = SND_QCP_FUNC_INVALID;

  if((!cmx_qcp_func_active) && (length != NULL)) {
    buf_struct = cmx_get_buffer_struct();
    if(buf_struct != NULL) {
      cmx_audfmt_setup_handle(handle, &handle_ptr, cmd);

      if(handle_ptr != NULL) {
        handle_ptr->cmd            = cmd;
        handle_ptr->buffer         = buf_struct;
        handle_ptr->cb_func.base   = cb_func;
        handle_ptr->client_data    = (void *) client_data;

        cmx_qcp_func_buffer    = buffer;
        cmx_qcp_func_length    = length;
        access_ptr             = &handle_ptr->sound.audfmt_sound.audfmt_handle;
        access_ptr->audfmt_ptr = (void *) handle_ptr;
        access_ptr->data_func  =
                            (snd_af_data_cb_func_type) cmx_af_play_callback;

        cmx_qcp_func_active    = TRUE;

        if(cmd == CMX_QCP_GET_DATA_SIZE) {
          snd_func = SND_QCP_FUNC_GET_DATA_SIZE;
        } else if(cmd == CMX_QCP_GET_FRAMES) {
          snd_func = SND_QCP_FUNC_GET_FRAMES;
        } else if(cmd == CMX_QCP_GET_CNFG) {
          snd_func = SND_QCP_FUNC_CNFG;
        } else if((cmd == CMX_QCP_GET_LABL) && (buffer != NULL)) {
          snd_func = SND_QCP_FUNC_LABL;
        } else if((cmd == CMX_QCP_GET_TEXT) && (buffer != NULL)) {
          snd_func = SND_QCP_FUNC_TEXT;
        } else {
          cmx_qcp_func_active = FALSE;
          cmx_free_af_handle(handle_ptr);
          if(cb_func != NULL) {
            cb_func(CMX_FAILURE, client_data);
          }
          return;
        }

        snd_qcp_get_func(snd_func, access_ptr);

        if(cb_func != NULL) {
          cb_func(CMX_SUCCESS, client_data);
        }
        return;
      } else {
        cmx_free_buffer_struct(buf_struct);
      }
    } else {
      cmx_free_handle(handle);
    }
  } else {
    cmx_free_handle(handle);
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_set_func_processing

DESCRIPTION
  This function sets the cnfg, labl or text chunk in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_set_func_processing (
  cmx_cmd_type           cmd,
  cmx_handle_type        *handle,
  uint32                 length,
  uint8                  *buffer,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
) {
  cmx_af_handle_type     *handle_ptr = NULL;
  cmx_audfmt_buf_struct  *buf_struct;
  snd_af_access_type     *access_ptr;
  snd_qcp_func_enum_type snd_func = SND_QCP_FUNC_INVALID;

  if((!cmx_qcp_func_active) && ((cmd == CMX_QCP_SET_CNFG) || (buffer != NULL)))
  {
    if(cmx_af_active == CMX_AF_QCP_REC) {
      if (cmd == CMX_QCP_SET_LABL) {
        memcpy(cmx_qcp_labl_pending_buffer, buffer, MIN(length, 48));
        cmx_qcp_labl_pending_length = length;
        cmx_qcp_set_labl_pend       = TRUE;
        cmx_qcp_func_active         = TRUE;
        if(cb_func != NULL) {
          cb_func(CMX_DONE, client_data);
        }
      }
      cmx_free_handle(handle);
    } else {
      buf_struct = cmx_get_buffer_struct();
      if(buf_struct != NULL) {
        cmx_audfmt_setup_handle(handle, &handle_ptr, cmd);

        if(handle_ptr != NULL) {
          handle_ptr->cmd            = cmd;
          handle_ptr->buffer         = buf_struct;
          handle_ptr->cb_func.base   = cb_func;
          handle_ptr->client_data    = (void *) client_data;

          access_ptr = &handle_ptr->sound.audfmt_sound.audfmt_handle;
          access_ptr->audfmt_ptr = (void *) handle_ptr;
          access_ptr->data_func  =
                               (snd_af_data_cb_func_type) cmx_af_play_callback;

          cmx_qcp_func_active    = TRUE;

          if(cmd == CMX_QCP_SET_CNFG) {
            snd_func = SND_QCP_FUNC_CNFG;
          } else if(cmd == CMX_QCP_SET_LABL) {
            snd_func = SND_QCP_FUNC_LABL;
          } else {
            snd_func = SND_QCP_FUNC_TEXT;
          }

          snd_qcp_set_func(snd_func, buffer, length, access_ptr,
                           (snd_qcp_rec_cb_func_ptr_type) cmx_qcp_set_func_cb,
                           (void *) handle_ptr);

          if(cb_func != NULL) {
            cb_func(CMX_SUCCESS, client_data);
          }
          return;
        } else {
          cmx_free_buffer_struct(buf_struct);
        }
      } else {
        cmx_free_handle(handle);
      }
    }
  } else {
    cmx_free_handle(handle);
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_processing

DESCRIPTION
  This function starts recording QCP data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_record_processing (
  cmx_handle_type          *tx_handle,
  cmx_handle_type          *rx_handle,
  cmx_mm_rec_qcp_para_type *rec_param,
  cmx_cb_func_ptr_type     cb_func,
  const void               *client_data
) {
  cmx_af_handle_type     *handle_ptr  = NULL;
  snd_qcp_dir_type       link         = SND_QCP_DIR_INVALID;
  cmx_status_type        status       = CMX_FAILURE;
  cmx_source_type        source       = CMX_SOURCE_INVALID;
  boolean                exit         = FALSE;
  boolean                warning_flag = FALSE;
  uint32                 space_left   = 0;
  uint32                 rx_space_left = 0;
  cmx_audfmt_buf_struct  *buffer;

#ifdef FEATURE_EFS
  fs_rsp_msg_type     rsp;
  fs_open_xparms_type open_parms;
  boolean             free_txrx_handle_flag = FALSE;
#endif /* FEATURE_EFS */

  if((cmx_af_active == CMX_AF_NONE) &&
     (rec_param->format < CMX_QCP_FORMAT_INVALID)) {

    if((tx_handle != NULL) && (rx_handle != NULL)) {
      if((tx_handle->source_type >= CMX_SOURCE_INVALID) ||
         (tx_handle->source_type != rx_handle->source_type)) {
        exit = TRUE;
      }
      source = tx_handle->source_type;
      link   = SND_QCP_DIR_BOTH;
    } else if(tx_handle != NULL) {
      if(tx_handle->source_type >= CMX_SOURCE_INVALID) {
        exit = TRUE;
      }
      source = tx_handle->source_type;
      link   = SND_QCP_DIR_REVERSE;
    } else if(rx_handle != NULL) {
      if(rx_handle->source_type >= CMX_SOURCE_INVALID) {
        exit = TRUE;
      }
      source = rx_handle->source_type;
      link   = SND_QCP_DIR_FORWARD;
    } else {
      exit = TRUE;
    }

    if((!exit) && ((buffer = cmx_get_buffer_struct()) != NULL)) {
      handle_ptr = cmx_get_af_handle();
      if(handle_ptr != NULL) {
        handle_ptr->buffer = buffer;
        if(tx_handle != NULL) {
#ifdef FEATURE_EFS
          free_txrx_handle_flag = TRUE;
#endif /* FEATURE_EFS */
          if(tx_handle->source_type == CMX_SOURCE_MEM) {
            handle_ptr->source_info.source     = CMX_SOURCE_MEM;
            handle_ptr->source_info.handle.buf = tx_handle->mem.buf;
            handle_ptr->source_info.cmx_len    = tx_handle->mem.len;
          } else if(tx_handle->source_type == CMX_SOURCE_CLIENT) {
            handle_ptr->source_info.source  = CMX_SOURCE_CLIENT;
            handle_ptr->source_info.cmx_len = tx_handle->client.data_len;
            handle_ptr->source_info.handle.data_fn =
                                               tx_handle->client.data_req_ptr;
          }
#ifdef FEATURE_EFS
          else if(tx_handle->source_type == CMX_SOURCE_EFS) {
            /* Make sure that we don't try to open a directory! */
            fs_nametest(tx_handle->efs.filename, FS_TEST_DIR, NULL, &rsp);
            if (rsp.nametest.status == FS_OKAY_S &&
                rsp.nametest.name_found == FALSE) {
              fs_remove(tx_handle->efs.filename, NULL, &rsp);
              open_parms.create.cleanup_option   = FS_OC_CLOSE;
              open_parms.create.buffering_option = FS_OB_PROHIBIT;
              open_parms.create.attribute_mask   = FS_FA_UNRESTRICTED;

              fs_open(tx_handle->efs.filename, FS_OA_CREATE, &open_parms,
                      NULL, &rsp);
              if(rsp.open.status == FS_OKAY_S) {
                handle_ptr->source_info.source      = CMX_SOURCE_EFS;
                handle_ptr->source_info.handle.file = rsp.open.handle;
                space_left = cmx_fs_space_available_using_name(tx_handle);
                if(space_left < (CMX_QCP_RECORD_MIN_SIZE+CMX_QCP_FS_OVERHEAD)) {
                  status = CMX_QCP_SPACE_DONE;
                  fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
                  exit = TRUE;
                } else {
                  /* success! */
                  /* Can't free the handle for SOURCE_EFS since we'll use it
                   * during the recording. */
                  free_txrx_handle_flag = FALSE;
                }
              } else {
                exit = TRUE;
              }
            } else {
              exit = TRUE;
            }
            if (exit) {
              cmx_free_af_handle(handle_ptr);
              handle_ptr = NULL;
            }
          }
#endif /* FEATURE_EFS */

#ifdef FEATURE_EFS
          if (free_txrx_handle_flag)
#endif /* FEATURE_EFS */
          {
            cmx_free_handle(tx_handle);
          }

        } else {
          memset(&handle_ptr->source_info.handle, 0,
                                            sizeof(cmx_af_access_type));
        }
      }

      if(handle_ptr != NULL) {
        if(rx_handle != NULL) {
#ifdef FEATURE_EFS
          free_txrx_handle_flag = TRUE;
#endif /* FEATURE_EFS */
          if(rx_handle->source_type == CMX_SOURCE_MEM) {
            handle_ptr->source_info.source       = CMX_SOURCE_MEM;
            handle_ptr->source_info.support.buf  = rx_handle->mem.buf;
            if((tx_handle == NULL) ||
               (handle_ptr->source_info.cmx_len > rx_handle->mem.len)) {
              handle_ptr->source_info.cmx_len    = rx_handle->mem.len;
            }
          } else if(rx_handle->source_type == CMX_SOURCE_CLIENT) {
            handle_ptr->source_info.source    = CMX_SOURCE_CLIENT;
            if((tx_handle == NULL) ||
               (handle_ptr->source_info.cmx_len > rx_handle->mem.len)) {
              handle_ptr->source_info.cmx_len = rx_handle->client.data_len;
            }
            handle_ptr->source_info.support.data_fn =
                                               rx_handle->client.data_req_ptr;
          }
#ifdef FEATURE_EFS
          else if(rx_handle->source_type == CMX_SOURCE_EFS) {
            /* Make sure that we don't try to open a directory! */
            fs_nametest(rx_handle->efs.filename, FS_TEST_DIR, NULL, &rsp);
            if (rsp.nametest.status == FS_OKAY_S &&
                rsp.nametest.name_found == FALSE) {
              fs_remove(rx_handle->efs.filename, NULL, &rsp);
              open_parms.create.cleanup_option   = FS_OC_CLOSE;
              open_parms.create.buffering_option = FS_OB_PROHIBIT;
              open_parms.create.attribute_mask   = FS_FA_UNRESTRICTED;

              fs_open(rx_handle->efs.filename, FS_OA_CREATE, &open_parms,
                      NULL, &rsp);
              if(rsp.open.status == FS_OKAY_S) {
                handle_ptr->source_info.source       = CMX_SOURCE_EFS;
                handle_ptr->source_info.support.file = rsp.open.handle;
                rx_space_left = cmx_fs_space_available_using_name(rx_handle);
                if (tx_handle) {
                  space_left = MIN(rx_space_left, space_left);
                }
                else {
                  space_left = rx_space_left;
                }
                if(space_left < (CMX_QCP_RECORD_MIN_SIZE+CMX_QCP_FS_OVERHEAD)) {
                  status = CMX_QCP_SPACE_DONE;
                  fs_close(handle_ptr->source_info.support.file, NULL, &rsp);
                  exit = TRUE;
                } else {
                  /* success! */
                  /* Can't free the handle for SOURCE_EFS since we'll use it
                   * during the recording. */
                  free_txrx_handle_flag = FALSE;
                }
              } else {
                exit = TRUE;
                }
            } else {
              exit = TRUE;
              }
            if (exit) {
              if(tx_handle) {
                fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
            }
            cmx_free_af_handle(handle_ptr);
            handle_ptr = NULL;
            }
          }
#endif /* FEATURE_EFS */
#ifdef FEATURE_EFS
          if (free_txrx_handle_flag)
#endif /* FEATURE_EFS */
          {
            cmx_free_handle(rx_handle);
          }
        } else {
          memset(&handle_ptr->source_info.support, 0,
                                            sizeof(cmx_af_access_type));
        }
      }

      if(handle_ptr != NULL) {
#ifdef FEATURE_EFS
        if(source == CMX_SOURCE_EFS) {
          if(space_left <=
                    (CMX_QCP_RECORD_WARNING_SIZE + CMX_QCP_FS_OVERHEAD)) {
            cmx_qcp_rec_warning_done = TRUE;
            warning_flag             = TRUE;
          } else {
            cmx_qcp_rec_warning_done = FALSE;
          }
        }
        else
#endif /* FEATURE_EFS */
        if((source == CMX_SOURCE_MEM) || (source == CMX_SOURCE_CLIENT)) {
          space_left = handle_ptr->source_info.cmx_len;

          if(space_left < CMX_QCP_RECORD_MIN_SIZE) {
            status = CMX_QCP_SPACE_DONE;
            cmx_free_af_handle(handle_ptr);
            handle_ptr = NULL;
          } else if(space_left <= CMX_QCP_RECORD_WARNING_SIZE) {
            cmx_qcp_rec_warning_done = TRUE;
            warning_flag             = TRUE;
          } else {
            cmx_qcp_rec_warning_done = FALSE;
          }
        }

      }
      if(handle_ptr != NULL) {
        handle_ptr->cmd           = CMX_QCP_CMD_REC;
        handle_ptr->cb_func.base  = cb_func;
        handle_ptr->client_data   = (void *) client_data;

#ifdef FEATURE_EFS
        handle_ptr->rec_tx_handle = tx_handle;
        handle_ptr->rec_rx_handle = rx_handle;
#endif /* FEATURE_EFS */

        cmx_af_active = CMX_AF_QCP_REC;

        if(link == SND_QCP_DIR_BOTH) {
          buffer->buf_type = CMX_AUDFMT_BUF_16;
        } else {
            buffer->buf_type = CMX_AUDFMT_BUF_32;
        }

        if ( (rec_param->auto_stop_ms != 0) &&
             (rec_param->auto_stop_ms < MIN_AUTO_STOP_VAL) ) {
           rec_param->auto_stop_ms = MIN_AUTO_STOP_VAL;
        }

        snd_qcp_record ((snd_qcp_format_type)rec_param->format,
                        link,
                        (snd_mm_qcp_rec_para_type *)rec_param,
                        (snd_qcp_rec_cb_func_ptr_type)cmx_qcp_rec_snd_cb,
                        (void *) handle_ptr);

        if(cb_func != NULL) {
          cb_func(CMX_SUCCESS, client_data);
          if(warning_flag) {
            cb_func(CMX_QCP_SPACE_WARNING, client_data);
          }
        }
        return;
      }
      cmx_free_buffer_struct(buffer);
    }
  }

  if(tx_handle != NULL) {
    cmx_free_handle(tx_handle);
  }
  if(rx_handle != NULL) {
    cmx_free_handle(rx_handle);
  }
  if(cb_func != NULL) {
    cb_func(status, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_stop_processing

DESCRIPTION
  This function stops a QCP recording session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_record_stop_processing (
  cmx_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  if(cmx_af_active == CMX_AF_QCP_REC) {
    if(cb_func != NULL) {
      cb_func(CMX_SUCCESS, client_data);
    }
    snd_qcp_record_stop(NULL, NULL);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_pause_processing

DESCRIPTION
  This function pauses QCP recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_record_pause_processing (
  cmx_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  if(cmx_af_active == CMX_AF_QCP_REC) {
    if(cb_func != NULL) {
      cb_func(CMX_SUCCESS, client_data);
    }
    snd_qcp_record_pause(NULL, NULL);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_resume_processing

DESCRIPTION
  This function resumes QCP recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_record_resume_processing (
  cmx_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  if(cmx_af_active == CMX_AF_QCP_REC) {
    if(cb_func != NULL) {
      cb_func(CMX_SUCCESS, client_data);
    }
    snd_qcp_record_resume(NULL, NULL);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION cmx_pcm_record_processing

DESCRIPTION
  This function starts recording PCM data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_pcm_record_processing (
  cmx_handle_type        *handle,
  cmx_pcm_rec_para_type  *rec_param,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
) {
  snd_pcm_rec_cmd_struct_type snd_rec_cmd  = {NULL, 0, NULL, 0};
  cmx_af_handle_type          *handle_ptr  = NULL;
  cmx_status_type             status       = CMX_FAILURE;
  boolean                     warning_flag = FALSE;
  uint32                      space_left   = 0;

#ifdef FEATURE_EFS
  fs_rsp_msg_type     rsp;
  fs_open_xparms_type open_parms;
  fs_handle_type      fs_handle = 0;
  boolean             file_created = FALSE;
#endif /* FEATURE_EFS */

  if (handle == NULL || rec_param == NULL) {
    if (cb_func != NULL) {
      cb_func (status, client_data);
    }
    return;
  }

  if((rec_param->link == CMX_PCM_REC_LINK_REVERSE) ||
     (rec_param->link == CMX_PCM_REC_LINK_FORWARD) ||
     (rec_param->link == CMX_PCM_REC_LINK_BOTH)) {

    switch(handle->source_type) {
      case CMX_SOURCE_MEM:
        space_left = handle->mem.len;
        break;

      case CMX_SOURCE_CLIENT:
        space_left = handle->client.data_len;
        break;

#ifdef FEATURE_EFS
      case CMX_SOURCE_EFS:
        /* Make sure that we don't try to open a directory! */
        fs_nametest(handle->efs.filename, FS_TEST_DIR, NULL, &rsp);
        if ((rsp.nametest.status == FS_OKAY_S) &&
            (rsp.nametest.name_found == FALSE)) {
          fs_remove(handle->efs.filename, NULL, &rsp);
          open_parms.create.cleanup_option   = FS_OC_CLOSE;
          open_parms.create.buffering_option = FS_OB_PROHIBIT;
          open_parms.create.attribute_mask   = FS_FA_UNRESTRICTED;

          fs_open(handle->efs.filename, FS_OA_CREATE, &open_parms,
                  NULL, &rsp);
          if(rsp.open.status == FS_OKAY_S) {
            fs_handle = rsp.open.handle;

        space_left = cmx_fs_space_available_using_name(handle);
        if(space_left > CMX_PCM_FS_OVERHEAD) {
          space_left -= CMX_PCM_FS_OVERHEAD;
        } else {
          space_left = 0;
        }
        if(space_left < CMX_PCM_RECORD_MIN_SIZE) {
          fs_close(fs_handle, NULL, &rsp);
        }
        else {
          file_created = TRUE;
        }
      }
    }
    break;
#endif /* FEATURE_EFS */
    }

    if(space_left < CMX_PCM_RECORD_MIN_SIZE) {
      status = CMX_PCM_INSUFF_SPACE;
    } else if(space_left < CMX_PCM_RECORD_WARNING_SIZE) {
      warning_flag = TRUE;
    }

    if((handle != NULL) && (status != CMX_PCM_INSUFF_SPACE)) {
      snd_rec_cmd.buffer1 = (uint8 *) CMX_MALLOC(CMX_PCM_REC_BUF_LENGTH);
      snd_rec_cmd.buffer2 = (uint8 *) CMX_MALLOC(CMX_PCM_REC_BUF_LENGTH);
      if((snd_rec_cmd.buffer1 != NULL) && (snd_rec_cmd.buffer2 != NULL)) {
        snd_rec_cmd.num_buf1 = CMX_PCM_REC_BUF_LENGTH;
        snd_rec_cmd.num_buf2 = CMX_PCM_REC_BUF_LENGTH;

        handle_ptr = cmx_get_af_handle();
        if(handle_ptr != NULL) {
          handle_ptr->buf1         = snd_rec_cmd.buffer1;
          handle_ptr->buf2         = snd_rec_cmd.buffer2;
          if(warning_flag) {
            handle_ptr->status_mask |= CMX_AF_STATUS_MASK_WARNING_SENT;
          }
          if(handle->source_type == CMX_SOURCE_MEM) {
            handle_ptr->source_info.source     = CMX_SOURCE_MEM;
            handle_ptr->source_info.handle.buf = handle->mem.buf;
            handle_ptr->source_info.cmx_len    = handle->mem.len;
          } else if(handle->source_type == CMX_SOURCE_CLIENT) {
            handle_ptr->source_info.source  = CMX_SOURCE_CLIENT;
            handle_ptr->source_info.cmx_len = handle->client.data_len;
            handle_ptr->source_info.handle.data_fn =
                                               handle->client.data_req_ptr;
          }
#ifdef FEATURE_EFS
          else if(handle->source_type == CMX_SOURCE_EFS) {
            handle_ptr->source_info.source      = CMX_SOURCE_EFS;
            handle_ptr->source_info.handle.file = fs_handle;
          }
#endif /* FEATURE_EFS */
        }
      } else {
        MSG_ERROR("Failed to allocate memory", 0,0,0);
      }
#ifdef FEATURE_EFS
      if (handle->source_type != CMX_SOURCE_EFS)
#endif /* FEATURE_EFS */
      {
        cmx_free_handle(handle);
      }
    }

    if(handle_ptr != NULL) {
      handle_ptr->cmd           = CMX_PCM_CMD_REC;
      handle_ptr->cb_func.base  = cb_func;
      handle_ptr->client_data   = (void *) client_data;
      handle_ptr->link          = rec_param->link;

      if(rec_param->link == CMX_PCM_REC_LINK_REVERSE) {
#ifdef FEATURE_EFS
        if (handle->source_type == CMX_SOURCE_EFS) {
          handle_ptr->rec_tx_handle = handle;
        }
#endif /* FEATURE_EFS */
        snd_pcm_record ( SND_PCM_REC_DIR_REVERSE, &snd_rec_cmd,
                         (snd_pcm_rec_cb_func_ptr_type) cmx_pcm_rec_snd_cb,
                         (void *) handle_ptr );

      }else if(rec_param->link == CMX_PCM_REC_LINK_BOTH) {
#ifdef FEATURE_EFS
        if (handle->source_type == CMX_SOURCE_EFS) {
          handle_ptr->rec_tx_handle = handle;
        }
#endif /* FEATURE_EFS */
        snd_pcm_record ( SND_PCM_REC_DIR_BOTH, &snd_rec_cmd,
                         (snd_pcm_rec_cb_func_ptr_type) cmx_pcm_rec_snd_cb,
                         (void *) handle_ptr );

      }else {
#ifdef FEATURE_EFS
        if (handle->source_type == CMX_SOURCE_EFS) {
          handle_ptr->rec_rx_handle = handle;
        }
#endif /* FEATURE_EFS */
        snd_pcm_record ( SND_PCM_REC_DIR_FORWARD, &snd_rec_cmd,
                         (snd_pcm_rec_cb_func_ptr_type) cmx_pcm_rec_snd_cb,
                         (void *) handle_ptr );
      }

      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
        if(warning_flag) {
          cb_func(CMX_PCM_SPACE_WARNING, client_data);
        }
      }
      return;
    } else {
      if(snd_rec_cmd.buffer1 != NULL) {
        CMX_FREE(snd_rec_cmd.buffer1);
      }
      if(snd_rec_cmd.buffer2 != NULL) {
        CMX_FREE(snd_rec_cmd.buffer2);
      }
#ifdef FEATURE_EFS
      if (file_created) {
        fs_close(fs_handle, NULL, &rsp);
      }
#endif /* FEATURE_EFS */
    }
  }

  if(handle != NULL) {
    cmx_free_handle(handle);
  }
  if(cb_func != NULL) {
    cb_func(status, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_stop_processing

DESCRIPTION
  This function stops a PCM recording session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_pcm_record_stop_processing (
  cmx_pcm_rec_link_type link,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
) {
  switch(link) {
    case CMX_PCM_REC_LINK_REVERSE:
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_pcm_record_stop(SND_PCM_REC_DIR_REVERSE, NULL, NULL);
      break;

    case CMX_PCM_REC_LINK_FORWARD:
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_pcm_record_stop(SND_PCM_REC_DIR_FORWARD, NULL, NULL);
      break;

    case CMX_PCM_REC_LINK_BOTH:
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_pcm_record_stop(SND_PCM_REC_DIR_BOTH, NULL, NULL);
      break;

    default:
      if(cb_func != NULL) {
        cb_func(CMX_FAILURE, client_data);
      }
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_pause_processing

DESCRIPTION
  This function pauses PCM recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_pcm_record_pause_processing (
  cmx_pcm_rec_link_type link,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
) {
  switch(link) {
    case CMX_PCM_REC_LINK_REVERSE:
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_pcm_record_pause(SND_PCM_REC_DIR_REVERSE, NULL, NULL);
      break;

    case CMX_PCM_REC_LINK_FORWARD:
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_pcm_record_pause(SND_PCM_REC_DIR_FORWARD, NULL, NULL);
      break;

    case CMX_PCM_REC_LINK_BOTH:
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_pcm_record_pause(SND_PCM_REC_DIR_BOTH, NULL, NULL);
      break;

    default:
      if(cb_func != NULL) {
        cb_func(CMX_FAILURE, client_data);
      }
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_resume_processing

DESCRIPTION
  This function resumes PCM recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_pcm_record_resume_processing (
  cmx_pcm_rec_link_type link,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
) {
  switch(link) {
    case CMX_PCM_REC_LINK_REVERSE:
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_pcm_record_resume(SND_PCM_REC_DIR_REVERSE, NULL, NULL);
      break;

    case CMX_PCM_REC_LINK_FORWARD:
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_pcm_record_resume(SND_PCM_REC_DIR_FORWARD, NULL, NULL);
      break;

    case CMX_PCM_REC_LINK_BOTH:
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_pcm_record_resume(SND_PCM_REC_DIR_BOTH, NULL, NULL);
      break;

    default:
      if(cb_func != NULL) {
        cb_func(CMX_FAILURE, client_data);
      }
      break;
  }
}
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION cmx_mm_rec_snd_cb

DESCRIPTION
  This function is the callback function for PCM recording for the sound
  server.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_mm_rec_snd_cb (
  snd_af_data_cmd_enum  command,
  void                  *data,
  const void            *client_data
)
{
  snd_mm_rec_data_struct_type  *mm_data;
  cmx_af_handle_type           *handle_ptr;
  cmx_packet_type              *packet;

  handle_ptr = (cmx_af_handle_type *) client_data;

  switch (command) {

    case SND_CMX_MM_REC_DATA:
    case SND_CMX_MM_REC_HEADER_DATA:

      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {

        if (data != NULL) {

          mm_data = (snd_mm_rec_data_struct_type *) data;

          if (command == SND_CMX_MM_REC_DATA) {
            packet->hdr.cmd                = CMX_MM_CMD_REC_DATA;
          } else {
            packet->hdr.cmd                = CMX_MM_CMD_REC_HEADER_DATA;
          }

          packet->mm_rec_data.handle      = handle_ptr;
          packet->mm_rec_data.buffer      = mm_data->rec_data_buf;
          packet->mm_rec_data.num_bytes   = mm_data->rec_data_num;
          packet->mm_rec_data.position    = mm_data->rec_data_pos;
          packet->mm_rec_data.buf_cb      = mm_data->buf_cb_func;
          packet->mm_rec_data.client_data = mm_data->client_data;
          cmx_cmd (packet);
        } else {
          packet->hdr.cmd                 = CMX_MM_CMD_REC_DATA;
          packet->mm_rec_status.handle    = handle_ptr;
          packet->mm_rec_status.status    = CMX_FAILURE;
          cmx_cmd (packet);
        }
      }
      break;

    case SND_CMX_MM_REC_DONE:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {

        packet->hdr.cmd              = CMX_MM_CMD_REC_DONE;
        packet->mm_rec_status.handle = handle_ptr;
        packet->mm_rec_status.status = CMX_DONE;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_MM_REC_FAILURE:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {

        packet->hdr.cmd              = CMX_MM_CMD_REC_STATUS;
        packet->mm_rec_status.handle = handle_ptr;
        packet->mm_rec_status.status = CMX_FAILURE;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_MM_REC_PAUSED:

      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {

        packet->hdr.cmd              = CMX_MM_CMD_REC_STATUS;
        packet->mm_rec_status.handle = handle_ptr;
        packet->mm_rec_status.status = CMX_REC_PAUSED;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_MM_REC_RESUMED:

      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd              = CMX_MM_CMD_REC_STATUS;
        packet->mm_rec_status.handle = handle_ptr;
        packet->mm_rec_status.status = CMX_REC_RESUMED;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_MM_REC_INTERRUPTED:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd               = CMX_MM_CMD_REC_STATUS;
        packet->pcm_rec_status.handle = handle_ptr;
        packet->pcm_rec_status.status = CMX_REC_INTERRUPTED;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_MM_REC_INT_RESUME:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd               = CMX_MM_CMD_REC_STATUS;
        packet->pcm_rec_status.handle = handle_ptr;
        packet->pcm_rec_status.status = CMX_REC_INT_RESUME;
        cmx_cmd (packet);
      }
      break;

    case SND_CMX_MM_REC_ENABLED:
      packet = cmx_cmd_get_pkt ();

      if (packet != NULL) {
        packet->hdr.cmd               = CMX_MM_CMD_REC_STATUS;
        packet->pcm_rec_status.handle = handle_ptr;
        packet->pcm_rec_status.status = CMX_REC_ENABLED;
        cmx_cmd (packet);
      }
      break;
  }
} /* cmx_mm_rec_snd_cb */

/*===========================================================================

FUNCTION cmx_mm_record_aac_processing

DESCRIPTION
  This function starts recording AAC data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_mm_record_aac_processing (
  cmx_handle_type            *handle,
  cmx_mm_rec_param_type      *rec_param,
  cmx_cb_func_ptr_type       cb_func,
  const void                 *client_data
)
{
  snd_mm_rec_cmd_struct_type  snd_rec_cmd  = {NULL, 0, NULL, 0};
  cmx_af_handle_type          *handle_ptr  = NULL;
  cmx_status_type             status       = CMX_FAILURE;
  boolean                     warning_flag = FALSE;
  uint32                      space_left   = 0;

#ifdef FEATURE_EFS
  fs_rsp_msg_type     rsp;
  fs_open_xparms_type open_parms;
  fs_handle_type      fs_handle = 0;
  boolean             file_created = FALSE;
#endif /* FEATURE_EFS */

  if (handle == NULL || rec_param == NULL) {
    if (cb_func != NULL) {
      cb_func (status, client_data);
    }
    return;
  }

  switch (handle->source_type) {

    case CMX_SOURCE_MEM:
      space_left = handle->mem.len;
      break;

    case CMX_SOURCE_CLIENT:
      space_left = handle->client.data_len;
      break;

#ifdef FEATURE_EFS
    case CMX_SOURCE_EFS:
      /* Make sure that we don't try to open a directory! */
      fs_nametest(handle->efs.filename, FS_TEST_DIR, NULL, &rsp);
      if ((rsp.nametest.status == FS_OKAY_S) &&
          (rsp.nametest.name_found == FALSE)) {
        fs_remove(handle->efs.filename, NULL, &rsp);
        open_parms.create.cleanup_option   = FS_OC_CLOSE;
        open_parms.create.buffering_option = FS_OB_PROHIBIT;
        open_parms.create.attribute_mask   = FS_FA_UNRESTRICTED;

        fs_open(handle->efs.filename, FS_OA_CREATE, &open_parms, NULL, &rsp);
        if(rsp.open.status == FS_OKAY_S) {
          fs_handle = rsp.open.handle;

          space_left = cmx_fs_space_available_using_name(handle);
      if(space_left > CMX_MM_FS_OVERHEAD) {
        space_left -= CMX_MM_FS_OVERHEAD;
      } else {
        space_left = 0;
      }
      if(space_left < CMX_MM_RECORD_MIN_SIZE) {
        fs_close(fs_handle, NULL, &rsp);
      }
      else {
        file_created = TRUE;
      }
    }
  }
  break;
#endif /* FEATURE_EFS */
  }

  if (space_left < CMX_MM_RECORD_MIN_SIZE) {
    status = CMX_MM_INSUFF_SPACE;
  } else if(space_left < CMX_MM_RECORD_WARNING_SIZE) {
    warning_flag = TRUE;
  }

  if (status != CMX_MM_INSUFF_SPACE) {

    snd_rec_cmd.buffer1 = (uint8 *)CMX_MALLOC(CMX_MM_REC_BUF_LENGTH);

    if (snd_rec_cmd.buffer1 != NULL) {

      snd_rec_cmd.buffer2 = (uint8 *)CMX_MALLOC(CMX_MM_REC_BUF_LENGTH);

      if (snd_rec_cmd.buffer2 != NULL) {

        snd_rec_cmd.num_buf1 = CMX_MM_REC_BUF_LENGTH;
        snd_rec_cmd.num_buf2 = CMX_MM_REC_BUF_LENGTH;

        handle_ptr = cmx_get_af_handle();

        if (handle_ptr != NULL) {
          handle_ptr->buf1         = snd_rec_cmd.buffer1;
          handle_ptr->buf2         = snd_rec_cmd.buffer2;

          if(warning_flag) {
            handle_ptr->status_mask |= CMX_AF_STATUS_MASK_WARNING_SENT;
          }

          if (handle->source_type == CMX_SOURCE_MEM) {

            handle_ptr->source_info.source     = CMX_SOURCE_MEM;
            handle_ptr->source_info.handle.buf = handle->mem.buf;
            handle_ptr->source_info.cmx_len    = handle->mem.len;

          } else if (handle->source_type == CMX_SOURCE_CLIENT) {

            handle_ptr->source_info.source  = CMX_SOURCE_CLIENT;
            handle_ptr->source_info.cmx_len = handle->client.data_len;
            handle_ptr->source_info.handle.data_fn =
                                               handle->client.data_req_ptr;
          }
#ifdef FEATURE_EFS
          else if (handle->source_type == CMX_SOURCE_EFS) {
                handle_ptr->source_info.source      = CMX_SOURCE_EFS;
                handle_ptr->source_info.handle.file = rsp.open.handle;
          }
#endif /* FEATURE_EFS */
        }
      } /* if (snd_rec_cmd.buffer2 != NULL) */
    } /* if (snd_rec_cmd.buffer1 != NULL) */

    if ((snd_rec_cmd.buffer1 == NULL) ||
        (snd_rec_cmd.buffer2 == NULL)) {

        cmx_free_af_handle(handle_ptr);
        handle_ptr = NULL;
        status     = CMX_MM_INSUFF_SPACE;
    }
  }

  if (handle_ptr != NULL) {

    handle_ptr->cmd           = CMX_MM_CMD_REC;
    handle_ptr->cb_func.base  = cb_func;
    handle_ptr->format        = rec_param->mm_rec_param.mm_rec_format;
    handle_ptr->client_data   = (void *) client_data;
    handle_ptr->link          = rec_param->mm_rec_link;

#ifdef FEATURE_EFS
    if (handle->source_type == CMX_SOURCE_EFS) {
      handle_ptr->rec_tx_handle = handle;
    } else
#endif /* FEATURE_EFS */
    {
      cmx_free_handle(handle);
    }

    cmx_af_active = CMX_AF_MM_REC;

    snd_mm_record ((snd_mm_rec_dir_type)rec_param->mm_rec_link,
                   (snd_mm_rec_aud_type *)(&(rec_param->mm_rec_param)),
                   &snd_rec_cmd,
                   (snd_mm_rec_cb_func_ptr_type)cmx_mm_rec_snd_cb,
                   (const void *)handle_ptr);

    if (cb_func != NULL) {

      cb_func(CMX_SUCCESS, client_data);

      if (warning_flag) {
        cb_func(CMX_PCM_SPACE_WARNING, client_data);
      }
    }

    return;

  } else {

    if (snd_rec_cmd.buffer1 != NULL) {
      CMX_FREE(snd_rec_cmd.buffer1);
    }

    if (snd_rec_cmd.buffer2 != NULL) {
      CMX_FREE(snd_rec_cmd.buffer2);
    }

#ifdef FEATURE_EFS
    if (file_created) {
      fs_close(fs_handle, NULL, &rsp);
    }
#endif /* FEATURE_EFS */
  }

  if (handle != NULL) {
    cmx_free_handle(handle);
  }

  if (cb_func != NULL) {
    cb_func(status, client_data);
  }
}

/*===========================================================================

FUNCTION cmx_mm_record_processing

DESCRIPTION
  This function starts recording MultiMedia data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_mm_record_processing (
  cmx_handle_type            *handle,
  cmx_mm_rec_param_type      *rec_param,
  cmx_cb_func_ptr_type       cb_func,
  const void                 *client_data
)
{
  if (handle != NULL && rec_param != NULL) {
    switch (rec_param->mm_rec_param.mm_rec_format) {
      case CMX_MM_FORMAT_QCP:

        if (rec_param->mm_rec_link == CMX_MM_REC_LINK_REVERSE) {

          cmx_qcp_record_processing(handle,
                                    NULL,
                                    (cmx_mm_rec_qcp_para_type *)
                                    &(rec_param->mm_rec_param),
                                    cb_func,
                                    client_data);

        } else if (rec_param->mm_rec_link == CMX_MM_REC_LINK_FORWARD) {

          cmx_qcp_record_processing(NULL,
                                    handle,
                                    (cmx_mm_rec_qcp_para_type *)
                                    &(rec_param->mm_rec_param),
                                    cb_func,
                                    client_data);
        } else if (rec_param->mm_rec_link == CMX_MM_REC_LINK_BOTH) {

          cmx_qcp_record_processing(NULL,
                                    handle,
                                    (cmx_mm_rec_qcp_para_type *)
                                    &(rec_param->mm_rec_param),
                                    cb_func,
                                    client_data);
        }
        break;

      case CMX_MM_FORMAT_AAC:
        cmx_mm_record_aac_processing(handle, rec_param, cb_func, client_data);
        break;

      default:
        MSG_ERROR("Unsupported recording format specified: %d",
                  (int)rec_param->mm_rec_param.mm_rec_format, 0, 0);
        if (cb_func != NULL) {
          cb_func (CMX_FAILURE, client_data);
        }
        break;
    } /* switch (rec_param->mm_rec_param.mm_rec_format) */
  }
  else {
    if (cb_func != NULL) {
      cb_func (CMX_FAILURE, client_data);
    }
  }
}

/*===========================================================================

FUNCTION cmx_mm_record_stop_processing

DESCRIPTION
  This function stops a MultiMedia recording session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_mm_record_stop_processing (
  cmx_mm_rec_link_type   link,        /* Direction to record           */
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
)
{
  if(cmx_af_active == CMX_AF_MM_REC) {
    if(cb_func != NULL) {
      cb_func(CMX_SUCCESS, client_data);
    }
    snd_mm_record_stop((snd_mm_rec_dir_type)link,
                       NULL,
                       NULL);
  } else if(cmx_af_active == CMX_AF_QCP_REC) {
    if(cb_func != NULL) {
      cb_func(CMX_SUCCESS, client_data);
    }
    snd_qcp_record_stop(NULL, NULL);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/*===========================================================================

FUNCTION cmx_mm_record_pause_processing

DESCRIPTION
  This function pauses MultiMedia recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_mm_record_pause_processing (
  cmx_mm_rec_link_type   link,            /* Direction to record            */
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
)
{
  snd_mm_record_pause((snd_mm_rec_dir_type)link,
                      NULL,
                      NULL);

  if (cb_func != NULL) {
    cb_func(CMX_SUCCESS, client_data);
  }
}

/*===========================================================================

FUNCTION cmx_mm_record_resume_processing

DESCRIPTION
  This function resumes MultiMedia record a session from pause.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_mm_record_resume_processing (
  cmx_mm_rec_link_type    link,            /* Direction to record            */
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
)
{
  snd_mm_record_resume((snd_mm_rec_dir_type)link,
                       NULL,
                       NULL);

  if (cb_func != NULL) {
    cb_func(CMX_SUCCESS, client_data);
  }
}
#endif /* FEATURE_MM_REC */
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_WEBAUDIO
/* <EJECT> */
#ifdef FEATURE_PNG_DECODER
/*===========================================================================

FUNCTION cmx_png_decode_processing

DESCRIPTION
  This function starts the PNG decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_png_offset
  cmx_png_buf_bytes

===========================================================================*/
void cmx_png_decode_processing (
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
  cmx_af_handle_type  *handle_ptr = NULL;

  cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_PNG_CMD_DECODE);

  if(handle_ptr != NULL) {
    if(callback_ptr != NULL) {
      callback_ptr(CMX_SUCCESS, client_data, NULL, 0);
    }
    handle_ptr->cmd            = CMX_PNG_CMD_DECODE;
    handle_ptr->cb_func.image  = callback_ptr;
    handle_ptr->client_data    = client_data;

    cmx_png_offset    = 0;
    cmx_png_buf_bytes = 0;

#ifdef FEATURE_WEBAUDIO
    cmx_png_audfmt_current = FALSE;
#endif /* FEATURE_WEBAUDIO */

    graph_png_set_buf(cmx_image_buf, CMX_IMAGE_BUFFER_SIZE, NULL, 0);
    graph_png_decode(handle_ptr, (png_cmx_decode_cb_func_type) cmx_png_callback);

  } else if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_png_abort_processing

DESCRIPTION
  This function aborts the PNG decoder.


DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void cmx_png_abort_processing
(
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  graph_png_decode_abort();

  if(callback_ptr != NULL) {
    callback_ptr(CMX_SUCCESS, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_png_spec_processing

DESCRIPTION
  This function gets the specifications of a PNG file.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void cmx_png_spec_processing
(
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
  cmx_af_handle_type  *handle_ptr = NULL;

  cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_PNG_CMD_SPEC);

  if(handle_ptr != NULL) {
    handle_ptr->cmd            = CMX_PNG_CMD_SPEC;
    handle_ptr->cb_func.image  = callback_ptr;
    handle_ptr->client_data    = client_data;

    cmx_png_offset    = 0;
    cmx_png_buf_bytes = 0;

    graph_png_get_image_specs(handle_ptr,
                        (png_cmx_decode_cb_func_type) cmx_png_callback);

  } else if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }
}
#endif /* FEATURE_PNG_DECODER */

/* <EJECT> */
#ifdef FEATURE_JPEG_DECODER
/*===========================================================================

FUNCTION cmx_jpeg_decode_processing

DESCRIPTION
  This function starts the JPEG decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_jpeg_offset
  cmx_jpeg_buf_bytes

===========================================================================*/
void cmx_jpeg_decode_processing (
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
  cmx_af_handle_type   *handle_ptr = NULL;
  jpeg_decodeInfo_type jpegdInfo;

  cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_JPEG_CMD_DECODE);

  if(handle_ptr != NULL) {
    if(callback_ptr != NULL) {
      callback_ptr(CMX_SUCCESS, client_data, NULL, 0);
    }
    handle_ptr->cmd            = CMX_JPEG_CMD_DECODE;
    handle_ptr->cb_func.image  = callback_ptr;
    handle_ptr->client_data    = client_data;

    cmx_jpeg_offset    = 0;
    cmx_jpeg_buf_bytes = 0;

    graph_jpeg_set_buf(cmx_image_buf, CMX_IMAGE_BUFFER_SIZE, NULL);
    /*graph_jpeg_decode*/
    jpegdInfo.isSync    = FALSE;
    jpegdInfo.outFormat = JPEGD_RGB888;
    jpegdInfo.arm_based = TRUE;
    jpeg_ARM_based_decode(handle_ptr, (jpeg_cmx_cb_func_type) cmx_jpeg_callback,
                          jpegdInfo);

  } else if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_jpeg_spec_processing

DESCRIPTION
  This function gets the specifications of a JPEG file.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void cmx_jpeg_spec_processing
(
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
  cmx_af_handle_type  *handle_ptr = NULL;

  cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_JPEG_CMD_SPEC);

  if(handle_ptr != NULL) {
    handle_ptr->cmd            = CMX_JPEG_CMD_SPEC;
    handle_ptr->cb_func.image  = callback_ptr;
    handle_ptr->client_data    = client_data;

    cmx_jpeg_offset    = 0;
    cmx_jpeg_buf_bytes = 0;

    graph_jpeg_get_image_specs(handle_ptr,
                        (jpeg_cmx_cb_func_type) cmx_jpeg_callback);

  } else if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_jpeg_decode_abort_processing

DESCRIPTION
  This function aborts the JPEG decode.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void cmx_jpeg_decode_abort_processing
(
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  graph_jpeg_abort_decode();

  if(callback_ptr != NULL) {
    callback_ptr(CMX_SUCCESS, client_data);
  }
}
#endif /* FEATURE_JPEG_DECODER */
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#ifdef FEATURE_SAF
/*===========================================================================

FUNCTION cmx_saf_frame_cont_processing

DESCRIPTION
  This function continues with frame processing if everything is setup
  properly.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_frame_cont_processing (
  cmx_af_handle_type *handle_ptr
) {
  if(handle_ptr != NULL) {
    if(handle_ptr->saf_buffer.buffer != NULL) {
      if((cmx_saf_status == CMX_SAF_STATUS_PLAYING) &&
         (cmx_saf_frame_cb != NULL)) {
        (void) rex_clr_timer( &cmx_saf_frame_timer );
        cmx_saf_frame_cb(cmx_saf_frame_number, handle_ptr->saf_buffer.buffer,
                         cmx_saf_frame_client_data);
      }
    } else {
      cmx_saf_cleanup(handle_ptr, CMX_FAILURE);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_decode_cont_processing

DESCRIPTION
  This function continues with starting SAF decode session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_saf_offset
  cmx_saf_buf_bytes

===========================================================================*/
void cmx_saf_decode_cont_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_saf_ctl_type   saf_ctl
) {
  if(handle_ptr != NULL) {
    if(!(handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF) ||
       (handle_ptr->saf_buffer.buffer != NULL)) {
      cmx_saf_offset    = 0;
      cmx_saf_buf_bytes = 0;

      (void) rex_clr_timer( &cmx_saf_frame_timer );

      cmx_saf_init(handle_ptr->saf_obj_buf, CMX_SAF_OBJECT_BUFFER_SIZE,
                   handle_ptr->saf_buffer.buffer);

      graph_saf_decode(handle_ptr,
                       (saf_cmx_decode_cb_func_ptr_type) cmx_saf_callback);
      cmx_saf_handle_ptr         = handle_ptr;
      cmx_saf_frame_cb           = NULL;
      cmx_saf_pending_handle_ptr = NULL;
      cmx_saf_status             = CMX_SAF_STATUS_PREPARING;
      cmx_saf_ctl                = saf_ctl;
      cmx_saf_audfmt_current     = FALSE;
    } else {
      if(handle_ptr->saf_obj_buf != NULL) {
        CMX_FREE(handle_ptr->saf_obj_buf);
      }
      cmx_client_buf_free(handle_ptr, &handle_ptr->saf_buffer);
      if(handle_ptr->cb_func.image != NULL) {
        handle_ptr->cb_func.image(CMX_FAILURE, handle_ptr->client_data,
                                  NULL, 0);
      }
      cmx_free_af_handle(handle_ptr);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_decode_processing

DESCRIPTION
  This function starts the SAF decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_decode_processing (
  cmx_handle_type              *handle,
  cmx_saf_ctl_type             saf_ctl,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
  cmx_client_buf_req_type buf_req;
  cmx_af_handle_type      *handle_ptr = NULL;
  cmx_packet_type         *packet;

  cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_SAF_CMD_DECODE);

  if(handle_ptr != NULL) {
    handle_ptr->cmd            = CMX_SAF_CMD_DECODE;
    handle_ptr->cb_func.image  = callback_ptr;
    handle_ptr->client_data    = client_data;

    handle_ptr->saf_obj_buf_size = CMX_SAF_OBJECT_BUFFER_SIZE;
    handle_ptr->saf_obj_buf      =
                        (uint8 *) CMX_MALLOC(CMX_SAF_OBJECT_BUFFER_SIZE);

    if(handle_ptr->saf_obj_buf != NULL) {
      packet = cmx_cmd_get_pkt ();
      if(packet != NULL) {
        packet->hdr.cmd          = CMX_SAF_CMD_DECODE_CONT;
        packet->saf_cont.handle  = handle_ptr;
        packet->saf_cont.saf_ctl = saf_ctl;

        handle_ptr->saf_buffer.handle      = handle_ptr;
        handle_ptr->saf_buffer.pending_cmd = packet;

        buf_req.buffer_size     = CMX_SAF_BUFFER_SIZE;
        buf_req.client_data     = &(handle_ptr->saf_buffer);
        buf_req.cb_func         = (cmx_client_buf_req_cb_func_ptr_type)
                                                       cmx_client_buf_req_cb;
        buf_req.buffer_supplied = FALSE;

        if(callback_ptr != NULL) {
          callback_ptr(CMX_SUCCESS, client_data, NULL, 0);
          callback_ptr(CMX_CLIENT_BUF_REQ, client_data, &buf_req,
                       sizeof(cmx_client_buf_req_type));
        }

        if(buf_req.buffer_supplied) {
          handle_ptr->status_mask |= CMX_AF_STATUS_MASK_USE_CLIENT_BUF;
        } else {
          q_put(&cmx_free_q, &packet->hdr.link);
          handle_ptr->saf_buffer.pending_cmd = NULL;

          cmx_saf_decode_cont_processing(handle_ptr, saf_ctl);
        }
        return;
      }
    } else {
      MSG_ERROR("Failed to allocate memory", 0,0,0);
    }
  }

  if(handle_ptr != NULL) {
    if(handle_ptr->saf_obj_buf != NULL) {
      CMX_FREE(handle_ptr->saf_obj_buf);
    }
    cmx_client_buf_free(handle_ptr, &handle_ptr->saf_buffer);
    cmx_free_af_handle(handle_ptr);
  }
  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_abort_processing

DESCRIPTION
  This function aborts the SAF decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_abort_processing
(
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  if(cmx_saf_status != CMX_SAF_STATUS_STOPPED) {
    graph_saf_decode_abort();

    if(callback_ptr != NULL) {
      callback_ptr(CMX_SUCCESS, client_data);
    }
  } else if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_frame_id_cmd_processing

DESCRIPTION
  This function aborts the SAF decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_frame_id_cmd_processing
(
  uint32                 frame_id,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  if((cmx_saf_status == CMX_SAF_STATUS_PLAYING) &&
     (cmx_saf_handle_ptr != NULL) &&
     (cmx_saf_handle_ptr->saf_buffer.buffer != NULL) &&
     (cmx_saf_frame_cb != NULL)) {
    if(callback_ptr != NULL) {
      callback_ptr(CMX_SUCCESS, client_data);
    }

    (void) rex_clr_timer( &cmx_saf_frame_timer );
    cmx_saf_frame_cb((int32) frame_id, cmx_saf_handle_ptr->saf_buffer.buffer,
                     cmx_saf_frame_client_data);
  } else if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_pause_processing

DESCRIPTION
  This function pauses the SAF decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_pause_processing
(
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  if((cmx_saf_handle_ptr != NULL) &&
     (cmx_saf_status != CMX_SAF_STATUS_STOPPED)) {
    (void) rex_clr_timer( &cmx_saf_frame_timer );
    cmx_saf_pending_status = cmx_saf_status;
    cmx_saf_status         = CMX_SAF_STATUS_PAUSED;

    if(callback_ptr != NULL) {
      callback_ptr(CMX_SUCCESS, client_data);
    }

    if(cmx_saf_handle_ptr->cb_func.image != NULL) {
      cmx_saf_handle_ptr->cb_func.image(CMX_PAUSE,
                                        cmx_saf_handle_ptr->client_data,
                                        NULL, 0);
    }
  } else if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_resume_processing

DESCRIPTION
  This function resumes the SAF decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_resume_processing
(
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  if((cmx_saf_handle_ptr != NULL) &&
     (cmx_saf_status == CMX_SAF_STATUS_PAUSED)) {
    cmx_saf_status = cmx_saf_pending_status;

    if(callback_ptr != NULL) {
      callback_ptr(CMX_SUCCESS, client_data);
    }

    if(cmx_saf_handle_ptr->cb_func.image != NULL) {
      cmx_saf_handle_ptr->cb_func.image(CMX_RESUME,
                                  cmx_saf_handle_ptr->client_data, NULL, 0);
    }

    if(cmx_saf_status == CMX_SAF_STATUS_PLAYING) {
      if(cmx_saf_pending_handle_ptr != NULL) {
        cmx_saf_frame_done_processing(cmx_saf_pending_handle_ptr,
                                      cmx_saf_pending_buffer,
                                      cmx_saf_pending_num_bytes,
                                      cmx_saf_pending_delay,
                                      cmx_saf_pending_last_frame);
        cmx_saf_pending_handle_ptr = NULL;
      } else if(cmx_saf_ctl == CMX_SAF_CTL_FREE_RUN) {
        cmx_saf_frame();
      }
    }
  } else if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_info_cont_processing

DESCRIPTION
  This function continues with SAF get info command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_info_cont_processing (
  cmx_af_handle_type *handle_ptr
) {
  if(handle_ptr != NULL) {
    if(!(handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF) ||
       (handle_ptr->saf_buffer.buffer != NULL)) {
      cmx_saf_offset    = 0;
      cmx_saf_buf_bytes = 0;

      cmx_saf_init(NULL, 0, handle_ptr->saf_buffer.buffer);

      graph_saf_get_info(handle_ptr,
                         (saf_cmx_decode_cb_func_ptr_type) cmx_saf_callback);
    } else {
      if(handle_ptr->cb_func.image != NULL) {
        handle_ptr->cb_func.image(CMX_FAILURE, handle_ptr->client_data,
                                  NULL, 0);
      }
      cmx_free_af_handle(handle_ptr);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_info_processing

DESCRIPTION
  This function gets the specifications of a SAF file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_info_processing
(
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
  cmx_client_buf_req_type buf_req;
  cmx_af_handle_type      *handle_ptr = NULL;
  cmx_packet_type         *packet;

  cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_SAF_CMD_INFO);

  if(handle_ptr != NULL) {
    handle_ptr->cmd            = CMX_SAF_CMD_INFO;
    handle_ptr->cb_func.image  = callback_ptr;
    handle_ptr->client_data    = client_data;

    packet = cmx_cmd_get_pkt ();
    if(packet != NULL) {
      packet->hdr.cmd         = CMX_SAF_CMD_INFO_CONT;
      packet->saf_cont.handle = handle_ptr;

      handle_ptr->saf_buffer.handle      = handle_ptr;
      handle_ptr->saf_buffer.pending_cmd = packet;

      buf_req.buffer_size     = CMX_SAF_INFO_BUFFER_SIZE;
      buf_req.client_data     = &(handle_ptr->saf_buffer);
      buf_req.cb_func         = (cmx_client_buf_req_cb_func_ptr_type)
                                                     cmx_client_buf_req_cb;
      buf_req.buffer_supplied = FALSE;

      if(callback_ptr != NULL) {
        callback_ptr(CMX_CLIENT_BUF_REQ, client_data, &buf_req,
                     sizeof(cmx_client_buf_req_type));
      }

      if(buf_req.buffer_supplied) {
        handle_ptr->status_mask |= CMX_AF_STATUS_MASK_USE_CLIENT_BUF;
      } else {
        q_put(&cmx_free_q, &packet->hdr.link);
        handle_ptr->saf_buffer.pending_cmd = NULL;

        cmx_saf_info_cont_processing(handle_ptr);
      }
      return;
    }

    cmx_client_buf_free(handle_ptr, &handle_ptr->saf_buffer);
    cmx_free_af_handle(handle_ptr);
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }
}
#endif /* FEATURE_SAF */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

FUNCTION cmx_adec_read_processing

DESCRIPTION
  This function issues a command to read from a QCP file.

DEPENDENCIES
  cmx_af_data_cb_array

RETURN VALUE
  None

SIDE EFFECTS
  cmx_af_data_cb_array

===========================================================================*/
void cmx_adec_read_processing (
  cmx_af_handle_type       *handle,
  uint32                   position,
  void                     *client_data,
  snd_af_read_cb_func_type cb_func
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type    rsp;
#endif /* FEATURE_EFS */
  cmx_af_data_cb_struct_type *adec_data;
  uint32                     entry, num_bytes = 0;
  uint8                      *buffer;

  if(handle->source_info.source == CMX_SOURCE_MEM) {
    if(cb_func != NULL) {
        buffer = cmx_adec_get_buffer();

        if (buffer != NULL) {
          if(position >= handle->source_info.cmx_len) {
            num_bytes = 0;
          } else {
            num_bytes = MIN(CMX_ADEC_READ_BUFF_SIZE,
                            handle->source_info.cmx_len - position);
            memcpy(buffer, &handle->source_info.handle.buf[position],
                   num_bytes);
          }
        }
        cb_func(client_data, buffer, num_bytes, position);
    }
  } else if(handle->source_info.source == CMX_SOURCE_CLIENT) {
    buffer = cmx_adec_get_buffer();
    if((cmx_adec_data_cb.number < CMX_ADEC_MAX_CB_STRUCTS) &&
       (buffer != NULL)) {
      entry = cmx_adec_data_cb.index + cmx_adec_data_cb.number;
      if(entry >= CMX_ADEC_MAX_CB_STRUCTS) {
        entry -= CMX_ADEC_MAX_CB_STRUCTS;
      }
      cmx_adec_data_cb.number++;
      adec_data              = &cmx_adec_data_cb.data_cb[entry];
      adec_data->snd_cb_func = cb_func;
      adec_data->client_data = client_data;
      adec_data->buffer      = buffer;
      adec_data->position    = position;
      adec_data->offset      = 0;
      adec_data->buffer_size = CMX_ADEC_READ_BUFF_SIZE;

      handle->source_info.handle.data_fn(position, CMX_ADEC_READ_BUFF_SIZE,
                                         cmx_adec_client_cb,
                                         (const void *) adec_data,
                                         handle->client_data);
    } else {
      MSG_ERROR("Data cb or buffer not available!!!",0,0,0);
    }

  }

#ifdef FEATURE_EFS
  else if(handle->source_info.source == CMX_SOURCE_EFS) {
    buffer = cmx_adec_get_buffer();

    if(buffer != NULL) {
      MSG_MED("cmx_adec_read_processing: Trying to read from EFS",0,0,0);
      fs_seek_read(handle->source_info.handle.file, FS_SEEK_SET,
                   (fs_file_position_type) position, buffer,
                   CMX_ADEC_READ_BUFF_SIZE, NULL, &rsp);
      MSG_MED("cmx_adec_read_processing: Done reading from EFS",0,0,0);
      if(rsp.read.status == FS_OKAY_S)
      {
        if(cb_func != NULL) {
          cb_func(client_data, buffer, rsp.read.count, position);
        }
      }
      else if ((rsp.read.status != FS_EOF_S) && (rsp.read.status != FS_BUSY_S))
      {
        /* If the status is Not EOF and Not Busy, it means that the file system
        ** returned an error, so to terminate this playback we will send a NULL
        ** buffer and free the current buffer.
        ** audfmt parser will stop playback when NULL buffer is detected.
        */
        cmx_af_free_buff ( handle->buffer, buffer );
        if(cb_func != NULL) {
          cb_func(client_data,NULL,0,position);
        }
        MSG_ERROR("cmx_adec_read_processing: Read Error from EFS",0,0,0);
      }
    }
  }
#endif /* FEATURE_EFS */

} /* end cmx_adec_read_processing */
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT
/*===========================================================================

FUNCTION cmx_midi_out_snd_cb

DESCRIPTION
  This function issues a command to enable/disable midi API.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_snd_cb
(
  void            *client_data,
  snd_status_type status
    /*lint -esym(715,client_data) Tell Lint we know client_data parameter
      is not used */
) {
  switch(status) {
    case SND_ENABLED:
      if(cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLE_PENDING) {
        cmx_midi_out_status.status = CMX_MIDI_OUT_STATUS_ENABLED;
        if(cmx_midi_out_status.cb_func != NULL) {
          cmx_midi_out_status.cb_func(CMX_ENABLED,
                                      cmx_midi_out_status.client_data);
        }
      }
      break;

    case SND_DISABLED:
      if(cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_DISABLE_PENDING) {
        cmx_midi_out_status.status = CMX_MIDI_OUT_STATUS_DISABLED;
        if(cmx_midi_out_status.cb_func != NULL) {
          cmx_midi_out_status.cb_func(CMX_DISABLED,
                                      cmx_midi_out_status.client_data);
        }

        cmx_midi_out_status.cb_func = NULL;
      }
      break;

    case SND_SUCCESS:
      if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
         (cmx_midi_out_status.cb_func != NULL)) {
        cmx_midi_out_status.cb_func(CMX_SUCCESS,
                                    cmx_midi_out_status.client_data);
      }
      break;

    case SND_FAILURE:
      if(cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_DISABLE_PENDING) {
        cmx_midi_out_status.status = CMX_MIDI_OUT_STATUS_ENABLED;
      }

      if(cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLE_PENDING) {
        cmx_midi_out_status.status = CMX_MIDI_OUT_STATUS_DISABLED;
      }

      if(cmx_midi_out_status.cb_func != NULL) {
        cmx_midi_out_status.cb_func(CMX_FAILURE,
                                    cmx_midi_out_status.client_data);
      }
      break;

    default:
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_processing

DESCRIPTION
  This function issues a command to enable/disable midi API.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_processing
(
  cmx_midi_out_type       enable,
  cmx_cb_func_ptr_type    cb_func,
  cmx_adec_mode_enum_type adec_mode,
  void                    *client_data
) {
  cmx_midi_out_qcp_handle_type *mo_ptr;

  if(cb_func != NULL) {
    cb_func(CMX_SUCCESS, client_data);
  }

  cmx_midi_out_status.cb_func     = cb_func;
  cmx_midi_out_status.client_data = client_data;

  if(enable == CMX_MIDI_OUT_ENABLE) {
    if(cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) {
      cmx_midi_out_status.status = CMX_MIDI_OUT_STATUS_ENABLED;

      if(cb_func != NULL) {
        cb_func(CMX_ENABLED, client_data);
      }
    } else if(cmx_midi_out_status.status ==
                               CMX_MIDI_OUT_STATUS_ENABLE_PENDING) {
      cmx_midi_out_status.status = CMX_MIDI_OUT_STATUS_ENABLE_PENDING;
    } else {
      cmx_midi_out_status.status = CMX_MIDI_OUT_STATUS_ENABLE_PENDING;

    snd_midi_out_open ((snd_cb_func_ptr_type) cmx_midi_out_snd_cb,
                       (snd_qsynth_adec_mode_enum_type) adec_mode,
                       NULL);
    }
  } else {
    if(cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_DISABLED) {
      cmx_midi_out_status.cb_func = NULL;
      if(q_cnt(&cmx_midi_out_in_use_q) != 0) {
        mo_ptr =
              (cmx_midi_out_qcp_handle_type *) q_get (&cmx_midi_out_in_use_q);
        while(mo_ptr != NULL) {
          if(mo_ptr->af_handle != NULL) {
            mo_ptr->af_handle->gen_handle = NULL;
          }
          q_put( &cmx_midi_out_free_q, &mo_ptr->link );
          snd_midi_out_qcp_stop(mo_ptr->channel);
          mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                           q_get (&cmx_midi_out_in_use_q);
        }
      }
      cmx_midi_out_status.status = CMX_MIDI_OUT_STATUS_DISABLED;
      if(cb_func != NULL) {
        cb_func(CMX_DISABLED, client_data);
      }
    } else if(cmx_midi_out_status.status !=
                                CMX_MIDI_OUT_STATUS_DISABLE_PENDING) {
      cmx_midi_out_status.status = CMX_MIDI_OUT_STATUS_DISABLE_PENDING;

    snd_midi_out_close ((snd_cb_func_ptr_type) cmx_midi_out_snd_cb, NULL);
  }
}
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_msg_processing

DESCRIPTION
  This function issues midi commands.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_msg_processing
(
  cmx_handle_type          *handle,
  cmx_cb_func_ptr_type     cb_func,
  void                     *client_data
) {
  uint8  *buffer = NULL;
  uint32 length  = 0;

#ifdef FEATURE_EFS
  fs_handle_type      file;
  fs_rsp_msg_type     rsp;
#endif /* FEATURE_EFS */

  if(cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) {
    if(handle->source_type == CMX_SOURCE_MEM) {
      length = MIN(handle->mem.len, CMX_MIDI_OUT_BUFF_SIZE);
      memcpy(cmx_midi_out_buffer, handle->mem.buf, length);
      buffer = cmx_midi_out_buffer;
    }
#ifdef FEATURE_EFS
    else if(handle->source_type == CMX_SOURCE_EFS) {
      /* Make sure that we don't try to open a directory! */
      fs_nametest(handle->efs.filename, FS_TEST_DIR, NULL, &rsp);
      if (rsp.nametest.status == FS_OKAY_S &&
          rsp.nametest.name_found == FALSE) {
        fs_open(handle->efs.filename, FS_OA_READONLY, NULL, NULL, &rsp);
        if(rsp.open.status == FS_OKAY_S) {
          file = rsp.open.handle;
          /* Load and store the file size */
          fs_file_size(handle->efs.filename, NULL, &rsp);
          if(rsp.file_size.status == FS_OKAY_S) {
            length = MIN(rsp.file_size.size, CMX_MIDI_OUT_BUFF_SIZE);
            fs_read(file, cmx_midi_out_buffer, length, NULL, &rsp);
            if(rsp.read.status == FS_OKAY_S) {
              fs_close(file, NULL, &rsp);
              buffer = cmx_midi_out_buffer;
            }
          }
        }
      }
    }
#endif /* FEATURE_EFS */
  }

  cmx_free_handle(handle);

  if(buffer != NULL) {
    if(cb_func != NULL) {
      cb_func(CMX_SUCCESS, client_data);
    }
    snd_midi_out_msg(buffer, length);
  } else {
    if(cb_func != NULL) {
      cb_func(CMX_FAILURE, client_data);
    }
    if(cmx_midi_out_status.cb_func != NULL) {
      cmx_midi_out_status.cb_func(CMX_FAILURE, cmx_midi_out_status.client_data);
    }
  }
}

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
/*===========================================================================

FUNCTION cmx_midi_out_qcp_reset_processing

DESCRIPTION
  This function issues a command to reset QCP file.
  1. If the application calls Stop() on the media, return CMX_ABORTED.
  2. If the playback finishes on its own, return CMX_DONE.
  3. If MIDI-OUT interface is disabled, return CMX_ABORTED.
  4. If number of playbacks exceeds number of adec channels, return CMX_ABORTED
     for the playback that is replaced.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_midi_out_qcp_reset_processing (
  cmx_af_handle_type       *handle
) {
  cmx_status_type              status = CMX_DONE;
  cmx_midi_out_qcp_handle_type *mo_ptr;
#ifdef FEATURE_EFS
  fs_rsp_msg_type     rsp;

  if(handle != NULL) {
    if(handle->source_info.source == CMX_SOURCE_EFS) {
      fs_close(handle->source_info.handle.file, NULL, &rsp);
    }
  }
#endif /* FEATURE_EFS */

  if(handle != NULL) {
    if(handle->gen_handle != NULL) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *) handle->gen_handle;
#ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete( &cmx_midi_out_in_use_q, &mo_ptr->link );
#else
      q_delete( &mo_ptr->link );
#endif
      q_put( &cmx_midi_out_free_q, &mo_ptr->link );
    } else {
      status = CMX_ABORTED;
    }

    if(handle->cb_func.midi_out_qcp != NULL) {
      handle->cb_func.midi_out_qcp(status, NULL, handle->client_data);
    }
    cmx_free_af_handle(handle);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_cb

DESCRIPTION
  This function is used as a callback to request/send data from the
  midi/WebAudio file.

DEPENDENCIES
  cmx_af_buf_info

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_midi_out_qcp_cb (
  snd_af_data_cmd_enum  command,
  void                  *file,
  uint32                num,
  void                  *data
) {
  cmx_af_handle_type *handle_ptr;
  cmx_packet_type    *packet;
  handle_ptr = (cmx_af_handle_type *) file;

  switch(command) {
    case SND_CMX_MIDI_OUT_QCP_START:
      if((handle_ptr != NULL) && (handle_ptr->cb_func.midi_out_qcp != NULL)) {
        handle_ptr->cb_func.midi_out_qcp(CMX_SUCCESS, handle_ptr->gen_handle,
                                         handle_ptr->client_data);
      }
      break;

    case SND_CMX_MIDI_OUT_ADEC_START_CMD:
      if((handle_ptr != NULL) && (handle_ptr->cb_func.midi_out_qcp != NULL)) {
        handle_ptr->cb_func.midi_out_qcp(CMX_STARTED, handle_ptr->gen_handle,
                                         handle_ptr->client_data);
      }
      break;

    case SND_CMX_ADEC_READ:
      if(handle_ptr->cmd == CMX_MIDI_OUT_QCP) {
        packet = cmx_cmd_get_pkt ();
        if (packet != NULL) {
          packet->hdr.cmd                  = CMX_ADEC_READ;
          packet->audfmt_read.position     = num;
          packet->audfmt_read.handle       = handle_ptr;
          packet->audfmt_read.client_data  =
                                   ((snd_af_read_cb_type *) data)->client_data;
          packet->audfmt_read.cb_func      =
                                   ((snd_af_read_cb_type *) data)->cb_func;
          cmx_cmd (packet);
        }
      }
      break;

    case SND_CMX_ADEC_READ_VAR:
      if(handle_ptr->cmd == CMX_MIDI_OUT_QCP) {
        packet = cmx_cmd_get_pkt ();

        if (packet != NULL) {
          packet->hdr.cmd                  = CMX_AF_CMD_READ_VAR;
          packet->audfmt_read.cmd          = SND_CMX_ADEC_READ_VAR;
          packet->audfmt_read.position     = num;
          packet->audfmt_read.handle       = handle_ptr;
          packet->audfmt_read.client_data  =
                                   ((snd_af_read_cb_type *) data)->client_data;
          packet->audfmt_read.cb_func      =
                                   ((snd_af_read_cb_type *) data)->cb_func;
          packet->audfmt_read.buffer_size  =
                                   ((snd_af_read_cb_type *) data)->buffer_size;
          packet->audfmt_read.offset       =
                                   ((snd_af_read_cb_type *) data)->offset;
          cmx_cmd (packet);
        }
      }
      break;

    case SND_CMX_MIDI_OUT_QCP_RESET:
      if(handle_ptr->cmd == CMX_MIDI_OUT_QCP) {
        packet = cmx_cmd_get_pkt ();

        if (packet != NULL) {
          packet->hdr.cmd                = CMX_MIDI_OUT_QCP_RESET;
          packet->midi_out_qcp.af_handle = handle_ptr;
          cmx_cmd (packet);
        }
      }
      break;

    case SND_CMX_AF_GET_FILE_LEN:
      *(uint32 *)data = handle_ptr->source_info.cmx_len;
      break;

    case SND_CMX_AF_FREE_VAR_BUF:
    case SND_CMX_AF_FREE_BUF:
      if(data != NULL) {
        if(!cmx_adec_free_buffer((uint8 *) data)) {
          CMX_FREE(data);
        }
      }
      break;

    case SND_CMX_MIDI_OUT_QCP_ABORT:
      if (cmx_midi_out_status.cb_func != NULL) {
        cmx_midi_out_status.cb_func(CMX_FAILURE,
                                    cmx_midi_out_status.client_data);
      }
      break;

    default:
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_processing

DESCRIPTION
  This function plays a QCP file to MIDI device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_processing
(
  cmx_handle_type               *handle,
  cmx_midi_out_adec_param_type  *param,
  cmx_midi_out_cb_func_ptr_type cb_func,
  void                          *client_data
) {
  cmx_af_handle_type           *handle_ptr = NULL;
  snd_af_access_type           *access_ptr;
  snd_midi_out_adec_param_type snd_param;
  cmx_midi_out_qcp_handle_type *mo_ptr;

  if(cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) {
    if (handle != NULL) {
      if ((handle->source_type == CMX_SOURCE_CLIENT) &&
          (handle->client.data_config != CMX_CLIENT_DATA_RANDOM)) {
        MSG_HIGH("Client does not support random access",0,0,0);
        cmx_free_handle(handle);
      }
      else {
        cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_MIDI_OUT_QCP);
      }
    }
    if(handle_ptr != NULL) {
      handle_ptr->cmd                  = CMX_MIDI_OUT_QCP;
      handle_ptr->cb_func.midi_out_qcp = cb_func;
      handle_ptr->client_data          = client_data;

      mo_ptr = (cmx_midi_out_qcp_handle_type *) q_get (&cmx_midi_out_free_q);
      if(mo_ptr == NULL) {
        mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                              q_get (&cmx_midi_out_in_use_q);
        if((mo_ptr != NULL) && (mo_ptr->af_handle != NULL)) {
          mo_ptr->af_handle->gen_handle = NULL;
        }
      }
      if(mo_ptr != NULL) {
        q_put (&cmx_midi_out_in_use_q, &mo_ptr->link);
        handle_ptr->gen_handle = (void *) mo_ptr;
        mo_ptr->af_handle      = handle_ptr;
        access_ptr             = &handle_ptr->sound.audfmt_sound.audfmt_handle;
        access_ptr->audfmt_ptr = (void *) handle_ptr;
        access_ptr->data_func  = (snd_af_data_cb_func_type) cmx_midi_out_qcp_cb;

        snd_param.volume     = param->volume;
        snd_param.pan        = param->pan;
        snd_param.repeat_num = param->repeat_num;

        switch (param->start_mode) {
          case CMX_ADEC_START_MODE_DELAYED:
            snd_param.start_mode = SND_ADEC_START_MODE_DELAYED;
            break;

          case CMX_ADEC_START_MODE_NORMAL:
          default:
            snd_param.start_mode = SND_ADEC_START_MODE_NORMAL;
            break;
        }

        switch (param->repeat_mode) {
          case CMX_ADEC_REPEAT_MODE_ENABLED:
            snd_param.repeat_mode = SND_ADEC_REPEAT_MODE_ENABLED;
            break;

          case CMX_ADEC_REPEAT_MODE_DISABLED:
          default:
            snd_param.repeat_mode = SND_ADEC_REPEAT_MODE_DISABLED;
            break;
        }

        snd_midi_out_qcp(mo_ptr->channel, access_ptr, &snd_param);

        return;
      }
    }
  } else {
    cmx_free_handle(handle);
  }

  if(handle_ptr != NULL) {
    cmx_free_af_handle(handle_ptr);
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, NULL, client_data);
  }

  if(cmx_midi_out_status.cb_func != NULL) {
    cmx_midi_out_status.cb_func(CMX_FAILURE, cmx_midi_out_status.client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_adec_start_chan_processing

DESCRIPTION
  This function starts playback of the indicated audio file that is in
  delayed start mode to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_adec_start_chan_processing
(
  cmx_midi_out_qcp_handle_type *midi_out_handle,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
) {
  cmx_midi_out_qcp_handle_type *mo_ptr;

  if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
     (midi_out_handle != NULL)) {
    mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                     q_check (&cmx_midi_out_in_use_q);
    while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                        q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
    }
    if(mo_ptr != NULL) {
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_midi_out_adec_start_chan(mo_ptr->channel);
      return;
    }
  }
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
  if(cmx_midi_out_status.cb_func != NULL) {
    cmx_midi_out_status.cb_func(CMX_FAILURE, cmx_midi_out_status.client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_stop_processing

DESCRIPTION
  This function stops a QCP file to MIDI device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_stop_processing
(
  cmx_midi_out_qcp_handle_type *midi_out_handle,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
) {
  cmx_midi_out_qcp_handle_type *mo_ptr;

  if(cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) {
    if(midi_out_handle == NULL) {
      if(q_cnt(&cmx_midi_out_in_use_q) != 0) {
        if(cb_func != NULL) {
          cb_func(CMX_SUCCESS, client_data);
        }
        mo_ptr =
              (cmx_midi_out_qcp_handle_type *) q_get (&cmx_midi_out_in_use_q);
        while(mo_ptr != NULL) {
          if(mo_ptr->af_handle != NULL) {
            mo_ptr->af_handle->gen_handle = NULL;
          }
          q_put( &cmx_midi_out_free_q, &mo_ptr->link );
          snd_midi_out_qcp_stop(mo_ptr->channel);
          mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                           q_get (&cmx_midi_out_in_use_q);
        }
        return;
      }
    } else {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                       q_check (&cmx_midi_out_in_use_q);
      while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
        mo_ptr = (cmx_midi_out_qcp_handle_type *)
                          q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
      }
      if(mo_ptr != NULL) {
        if(mo_ptr->af_handle != NULL) {
          mo_ptr->af_handle->gen_handle = NULL;
        }
#ifdef FEATURE_Q_NO_SELF_QPTR
        q_delete( &cmx_midi_out_in_use_q, &mo_ptr->link );
#else
        q_delete( &mo_ptr->link );
#endif
        q_put( &cmx_midi_out_free_q, &mo_ptr->link );
        if(cb_func != NULL) {
          cb_func(CMX_SUCCESS, client_data);
        }
        snd_midi_out_qcp_stop(mo_ptr->channel);
        return;
      }
    }
  }

  if (cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

  if(cmx_midi_out_status.cb_func != NULL) {
    cmx_midi_out_status.cb_func(CMX_FAILURE, cmx_midi_out_status.client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_pause_processing

DESCRIPTION
  This function pauses/resumes QCP files to MIDI device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_pause_processing
(
  boolean                      pause,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
) {
  if(cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) {
    if(q_cnt(&cmx_midi_out_in_use_q) != 0) {
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      if(pause) {
        snd_midi_out_qcp_pause();
      } else {
        snd_midi_out_qcp_resume();
      }
      return;
    }
  }
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
  if(cmx_midi_out_status.cb_func != NULL) {
    cmx_midi_out_status.cb_func(CMX_FAILURE, cmx_midi_out_status.client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_pause_chan_processing

DESCRIPTION
  This function pauses/resumes QCP files to MIDI device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_pause_chan_processing
(
  boolean                      pause,
  cmx_midi_out_qcp_handle_type *midi_out_handle,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
) {
  cmx_midi_out_qcp_handle_type *mo_ptr;

  if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
     (midi_out_handle != NULL)) {
    mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                     q_check (&cmx_midi_out_in_use_q);
    while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                        q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
    }
    if(mo_ptr != NULL) {
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      if(pause) {
        snd_midi_out_qcp_pause_chan(mo_ptr->channel);
      } else {
        snd_midi_out_qcp_resume_chan(mo_ptr->channel);
      }
      return;
    }
  }
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
  if(cmx_midi_out_status.cb_func != NULL) {
    cmx_midi_out_status.cb_func(CMX_FAILURE, cmx_midi_out_status.client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_pan_processing

DESCRIPTION
  This function modifies the panning of a QCP file to MIDI device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_pan_processing
(
  cmx_midi_out_qcp_handle_type *midi_out_handle,
  sint15                       pan,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
) {
  cmx_midi_out_qcp_handle_type *mo_ptr;

  if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
     (midi_out_handle != NULL)) {
    mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                     q_check (&cmx_midi_out_in_use_q);
    while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                        q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
    }
    if(mo_ptr != NULL) {
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_midi_out_qcp_pan(mo_ptr->channel, pan);
      return;
    }
  }
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
  if(cmx_midi_out_status.cb_func != NULL) {
    cmx_midi_out_status.cb_func(CMX_FAILURE, cmx_midi_out_status.client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_vol_processing

DESCRIPTION
  This function modifies the volume of a QCP file to MIDI device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_vol_processing
(
  cmx_midi_out_qcp_handle_type *midi_out_handle,
  uint16                       volume,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
) {
  cmx_midi_out_qcp_handle_type *mo_ptr;

  if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
     (midi_out_handle != NULL)) {
    mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                     q_check (&cmx_midi_out_in_use_q);
    while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                        q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
    }
    if(mo_ptr != NULL) {
      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      snd_midi_out_qcp_volume(mo_ptr->channel, volume);
      return;
    }
  }
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
  if(cmx_midi_out_status.cb_func != NULL) {
    cmx_midi_out_status.cb_func(CMX_FAILURE, cmx_midi_out_status.client_data);
  }
}

/* <EJECT> */
#ifdef FEATURE_WAVE_PB
/*===========================================================================

FUNCTION cmx_midi_out_qcp_channel_search

DESCRIPTION
  This function is used by the q_linear_seach to look for specific
  midi_out_qcp handle on the free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
int cmx_midi_out_qcp_channel_search (
  void *item_ptr,
  void *compare_val
) {
  if (((cmx_midi_out_qcp_handle_type *)item_ptr)->channel == 0) {
    return TRUE;
  }
  return FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_reserve_channel_processing

DESCRIPTION
  This function reserves channel 0 of MIDI_OUT_QCP for ADPCM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_midi_out_qcp_reserve_channel_processing (void)
{
  cmx_midi_out_qcp_handle_type *handle;

  handle = (cmx_midi_out_qcp_handle_type *)
                               q_linear_search(&cmx_midi_out_free_q,
                                               cmx_midi_out_qcp_channel_search,
                                               0);
  if(handle != NULL) {
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_delete( &cmx_midi_out_free_q, &handle->link );
#else
    q_delete( &handle->link );
#endif
    cmx_midi_out_adpcm_handle_ptr = handle;
  } else {
    MSG_ERROR("Should always have channel 0",0,0,0);
  }
}
#endif /* FEATURE_WAVE_PB */
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

/* <EJECT> */
#ifdef FEATURE_AUDIO_POSTCARD
/*===========================================================================

FUNCTION cmx_audiop_read_data

DESCRIPTION
  This function reads data from file for audio postcard authoring.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cmx_status_type cmx_audiop_read_data (
  cmx_af_handle_type *handle,
  uint8              *buffer,
  uint32             num_bytes,
  uint32             position
) {
  cmx_status_type  ret_val = CMX_FAILURE;
#ifdef FEATURE_EFS
  fs_rsp_msg_type  rsp;             /* EFS response buffer                   */
#endif /* FEATURE_EFS */

  if(handle != NULL) {
    if(handle->source_info.source == CMX_SOURCE_MEM) {
      if((position + num_bytes) < handle->source_info.cmx_len) {
        memcpy(buffer, &(handle->source_info.handle.buf[position]), num_bytes);
        ret_val = CMX_SUCCESS;
      }
    } else if(handle->source_info.source == CMX_SOURCE_CLIENT) {
      if((position + num_bytes) < handle->source_info.cmx_len) {
        /* Request client for more data */
        handle->source_info.handle.data_fn(position, num_bytes, NULL,
                                           (const void *) buffer,
                                           handle->client_data );
        ret_val = CMX_SUCCESS;
      }
    }
#ifdef FEATURE_EFS
    else if(handle->source_info.source == CMX_SOURCE_EFS) {
      fs_seek_read(handle->source_info.handle.file, FS_SEEK_SET,
                   (fs_file_position_type) position, buffer, num_bytes,
                   NULL, &rsp);

      if(rsp.seek_read.status == FS_OKAY_S) {
        ret_val = CMX_SUCCESS;
      }
    }
#endif /* FEATURE_EFS */
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audiop_write_data

DESCRIPTION
  This function writes data to audio postcard file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cmx_status_type cmx_audiop_write_data (
  cmx_af_handle_type *handle,
  uint8              *buffer,
  uint32             num_bytes,
  uint32             position
) {
  cmx_status_type  ret_val = CMX_FAILURE;
#ifdef FEATURE_EFS
  fs_rsp_msg_type  rsp;             /* EFS response buffer                   */
#endif /* FEATURE_EFS */

  if(handle != NULL) {
    if(handle->source_info.source == CMX_SOURCE_MEM) {
      if((position + num_bytes) < handle->source_info.cmx_len) {
        memcpy(&handle->source_info.handle.buf[position], buffer, num_bytes);
        ret_val = CMX_SUCCESS;
      }
    } else if(handle->source_info.source == CMX_SOURCE_CLIENT) {
      if((position + num_bytes) < handle->source_info.cmx_len) {
        handle->source_info.handle.data_fn(position, num_bytes, NULL,
                                           (const void *) buffer,
                                           handle->client_data );
        ret_val = CMX_SUCCESS;
      }
    }
#ifdef FEATURE_EFS
    else if(handle->source_info.source == CMX_SOURCE_EFS) {
      fs_seek_write(handle->source_info.handle.file, FS_SEEK_SET,
                    (fs_file_position_type) position, buffer, num_bytes,
                    NULL, &rsp);

      if(rsp.seek_write.status == FS_OKAY_S) {
        ret_val = CMX_SUCCESS;
      }
    }
#endif /* FEATURE_EFS */
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_verify_qcp_rate

DESCRIPTION
  This function verifies QCP is fixed full rate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
cmx_status_type cmx_verify_qcp_rate (
  uint8  *buffer,
  uint32 num_bytes
) {
  cmx_status_type ret_val = CMX_SUCCESS;
  uint32          counter;

  for(counter = 0; counter < num_bytes; counter += CMX_QCP_FULLRATE_SIZE) {
    if(buffer[counter] != CMX_QCP_FULLRATE_BYTE) {
      ret_val = CMX_FAILURE;
      break;
    }
  }
  return (ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_complete_audiop_processing

DESCRIPTION
  This function completes an audio postcard creation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_complete_audiop_processing
(
  cmx_status_type      status,
  cmx_af_handle_type   *handle,
  uint32               num_bytes,
  uint32               position
) {
  cmx_af_handle_type *output_handle_ptr;
  uint32             output_index, temp1, temp2, temp3;
  uint8              ap_end[4]   = {0x64, 0xff, 0xdf, 0x00};
  uint8              tbuffer[12] = {0x32, 0xff, 0xf1, 0x03, 0x72, 0x00,
                                    0x44, 0x00, 0x00, 0x03, 0x77, 0x00};
#ifdef FEATURE_EFS
  fs_rsp_msg_type    rsp;         /* EFS response buffer                   */
#endif /* FEATURE_EFS */

  if(handle != NULL) {
    output_handle_ptr = handle->gen_handle;

    if(output_handle_ptr != NULL) {
      if(status == CMX_SUCCESS) {
        output_index  = handle->cur_length;

        while((num_bytes != 0) && (status == CMX_SUCCESS)) {
          if(num_bytes < CMX_AUDIOP_QCP_BLOCK_SIZE) {
            temp1 = num_bytes;
          } else {
            temp1 = CMX_AUDIOP_QCP_BLOCK_SIZE;
          }

          status = cmx_audiop_read_data(handle, (uint8 *) &handle->buffer->buf,
                                        temp1, position);

          if(status != CMX_FAILURE) {
            status = cmx_verify_qcp_rate((uint8 *) &handle->buffer->buf, temp1);
          }

          if(status != CMX_FAILURE) {
            num_bytes -= temp1;
            position  += temp1;
            temp2      = 0;
            while((temp1 != 0) && (status == CMX_SUCCESS)) {
              if((temp1 <= CMX_QCP_BLOCK_SIZE) && (num_bytes == 0)) {
                tbuffer[9]  = 0;
                tbuffer[10] = 0;
              }

              if(temp1 >= CMX_QCP_BLOCK_SIZE) {
                status = cmx_audiop_write_data(output_handle_ptr, tbuffer, 12,
                                               output_index);
                if(status == CMX_SUCCESS) {
                  output_index += 12;
                  status = cmx_audiop_write_data(output_handle_ptr,
                                    &(((uint8 *) &handle->buffer->buf)[temp2]),
                                    CMX_QCP_BLOCK_SIZE, output_index);
                  output_index += CMX_QCP_BLOCK_SIZE;
                  temp1        -= CMX_QCP_BLOCK_SIZE;
                  temp2        += CMX_QCP_BLOCK_SIZE;
                }
              } else if(num_bytes == 0) {
                temp3      = temp1 + 7;
                tbuffer[3] = (temp3 >> 8) & 0xff;
                tbuffer[4] = temp3 & 0xff;
                status     = cmx_audiop_write_data(output_handle_ptr, tbuffer,
                                                   12, output_index);
                if(status == CMX_SUCCESS) {
                  output_index += 12;
                  status = cmx_audiop_write_data(output_handle_ptr,
                                    &(((uint8 *) &handle->buffer->buf)[temp2]),
                                    temp1, output_index);
                  output_index += temp1;
                  temp1         = 0;
                }
              } else {
                status = CMX_FAILURE;
              }
              tbuffer[11] = 1;
            }
          }
        }

        if(status == CMX_SUCCESS) {
          status = cmx_audiop_write_data(output_handle_ptr, ap_end, 4,
                                         output_index);
          if(status == CMX_SUCCESS) {
            output_index -= 4;
            tbuffer[0] = (output_index >> 24) & 0xff;
            tbuffer[1] = (output_index >> 16) & 0xff;
            tbuffer[2] = (output_index >> 8)  & 0xff;
            tbuffer[3] = output_index & 0xff;
            status = cmx_audiop_write_data(output_handle_ptr, tbuffer, 4, 4);
            if(status == CMX_SUCCESS) {
              output_index -= 75;
              tbuffer[0] = (output_index >> 24) & 0xff;
              tbuffer[1] = (output_index >> 16) & 0xff;
              tbuffer[2] = (output_index >> 8)  & 0xff;
              tbuffer[3] = output_index & 0xff;
              status = cmx_audiop_write_data(output_handle_ptr, tbuffer, 4, 79);
            }
          }
        }
      }

#ifdef FEATURE_EFS
      if(output_handle_ptr->source_info.source == CMX_SOURCE_EFS) {
        fs_close(output_handle_ptr->source_info.handle.file, NULL, &rsp);

        if(status == CMX_FAILURE) {
          fs_remove(output_handle_ptr->file_desc->efs.filename, NULL, &rsp);
        }
      }
#endif /* FEATURE_EFS */

      if(output_handle_ptr->cb_func.base != NULL) {
        output_handle_ptr->cb_func.base(status, output_handle_ptr->client_data);
      }

      cmx_free_handle(output_handle_ptr->file_desc);
      cmx_free_af_handle(output_handle_ptr);
    }
#ifdef FEATURE_EFS
    if(handle->source_info.source == CMX_SOURCE_EFS) {
      fs_close(handle->source_info.handle.file, NULL, &rsp);
    }
#endif
    cmx_free_af_handle(handle);
  }
  cmx_qcp_func_active = FALSE;
  cmx_af_active       = CMX_AF_NONE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_create_audiop_jpeg

DESCRIPTION
  This function does JPEG processing for audio postcard creation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
cmx_status_type cmx_create_audiop_jpeg
(
  cmx_handle_type      *jpeg,
  cmx_af_handle_type   *output_handle_ptr,
  uint32               *total_size
) {
  cmx_status_type       ret_val = CMX_FAILURE;
  cmx_af_handle_type    *jpeg_handle_ptr = NULL;
  cmx_audfmt_buf_struct *buffer;
  uint32                length, index, temp32;
  uint8                 tbuffer[4];
#ifdef FEATURE_EFS
  fs_rsp_msg_type    rsp;         /* EFS response buffer                   */
#endif /* FEATURE_EFS */

  if(jpeg != NULL) {
    buffer = cmx_get_buffer_struct();
    if(buffer != NULL) {
      cmx_audfmt_setup_handle(jpeg, &jpeg_handle_ptr, CMX_CMD_CREATE_AUDIOP);

      if(jpeg_handle_ptr != NULL) {
        length = jpeg_handle_ptr->source_info.cmx_len + 5;

        if(cmx_audiop_write_data(output_handle_ptr, (uint8 *) cmx_audiop_header,
                                 CMX_AUDIOP_HEADER_SIZE, 0) == CMX_SUCCESS) {

          tbuffer[0] = (length >> 8) & 0xff;
          tbuffer[1] = (length & 0xff);
          if(cmx_audiop_write_data(output_handle_ptr, tbuffer, 2, 0x56) ==
             CMX_SUCCESS) {

            length = jpeg_handle_ptr->source_info.cmx_len;
            index  = 0;
            while(length != 0) {
              if(length < CMX_MIN_BUF_SIZE*64) {
                temp32 = length;
              } else {
                temp32 = CMX_MIN_BUF_SIZE*64;
              }

              if((cmx_audiop_read_data(jpeg_handle_ptr, (uint8 *) &buffer->buf,
                                       temp32, index) == CMX_FAILURE) ||
                 (cmx_audiop_write_data(output_handle_ptr,
                                        (uint8 *) &buffer->buf, temp32,
                                        index + CMX_AUDIOP_HEADER_SIZE) ==
                  CMX_FAILURE)) {
                break;
              }

              length -= temp32;
              index  += temp32;
            }

            if(length == 0) {
              *total_size  = CMX_AUDIOP_HEADER_SIZE +
                             jpeg_handle_ptr->source_info.cmx_len;
              ret_val      = CMX_SUCCESS;
            }
          }
        }
#ifdef FEATURE_EFS
        if(jpeg_handle_ptr->source_info.source == CMX_SOURCE_EFS) {
          fs_close(jpeg_handle_ptr->source_info.handle.file, NULL, &rsp);
        }
#endif
        cmx_free_af_handle(jpeg_handle_ptr);
      }
      cmx_free_buffer_struct(buffer);
    }
  }

  return (ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_create_audiop_qcp

DESCRIPTION
  This function does QCP processing for audio postcard creation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
cmx_status_type cmx_create_audiop_qcp
(
  cmx_handle_type      *qcp,
  cmx_af_handle_type   *output_handle_ptr,
  uint32               total_size
) {
  cmx_af_handle_type    *qcp_handle_ptr = NULL;
  snd_af_access_type    *access_ptr;
  cmx_audfmt_buf_struct *buffer;

  if(qcp != NULL) {
    buffer = cmx_get_buffer_struct();
    if(buffer != NULL) {
      cmx_audfmt_setup_handle(qcp, &qcp_handle_ptr, CMX_CMD_CREATE_AUDIOP);

      if(qcp_handle_ptr != NULL) {
        cmx_qcp_func_active          = TRUE;
        qcp_handle_ptr->cmd          = CMX_CMD_CREATE_AUDIOP;
        qcp_handle_ptr->buffer       = buffer;
        qcp_handle_ptr->cb_func.base = output_handle_ptr->cb_func.base;
        qcp_handle_ptr->client_data  = output_handle_ptr->client_data;
        qcp_handle_ptr->gen_handle   = output_handle_ptr;
        qcp_handle_ptr->cur_length   = total_size;


        access_ptr = &qcp_handle_ptr->sound.audfmt_sound.audfmt_handle;
        access_ptr->audfmt_ptr = (void *) qcp_handle_ptr;
        access_ptr->data_func  =
                        (snd_af_data_cb_func_type) cmx_af_play_callback;

        snd_qcp_get_func(SND_QCP_FUNC_GET_DATA_SIZE, access_ptr);
        return(CMX_SUCCESS);
      }
      cmx_free_buffer_struct(buffer);
    }
  }

  return(CMX_FAILURE);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_create_audiop_processing

DESCRIPTION
  This function creates an audio postcard file.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_af_active

===========================================================================*/
void cmx_create_audiop_processing
(
  cmx_handle_type      *jpeg,
  cmx_handle_type      *qcp,
  cmx_handle_type      *output,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type     rsp;         /* EFS response buffer                   */
  fs_open_xparms_type open_parms;
#endif /* FEATURE_EFS */
  cmx_af_handle_type  *output_handle_ptr = NULL;
  uint32              total;

  if((cmx_af_active == CMX_AF_NONE) && (!cmx_qcp_func_active) &&
     (jpeg->source_type < CMX_SOURCE_INVALID) &&
     (qcp->source_type < CMX_SOURCE_INVALID) &&
     (output->source_type < CMX_SOURCE_INVALID)) {

    cmx_af_active     = CMX_AF_AUDIOP;
    output_handle_ptr = cmx_get_af_handle();
    if(output_handle_ptr != NULL) {
      if(output->source_type == CMX_SOURCE_MEM) {
        output_handle_ptr->source_info.source     = CMX_SOURCE_MEM;
        output_handle_ptr->source_info.handle.buf = output->mem.buf;
        output_handle_ptr->source_info.cmx_len    = output->mem.len;
      } else if(output->source_type == CMX_SOURCE_CLIENT) {
        output_handle_ptr->source_info.source  = CMX_SOURCE_CLIENT;
        output_handle_ptr->source_info.cmx_len = output->client.data_len;
        output_handle_ptr->source_info.handle.data_fn =
                                           output->client.data_req_ptr;
      }
#ifdef FEATURE_EFS
      else if(output->source_type == CMX_SOURCE_EFS) {
        /* Make sure that we don't try to open a directory! */
        fs_nametest(output->efs.filename, FS_TEST_DIR, NULL, &rsp);
        if (rsp.nametest.status == FS_OKAY_S &&
            rsp.nametest.name_found == FALSE) {
          fs_remove(output->efs.filename, NULL, &rsp);
          open_parms.create.cleanup_option   = FS_OC_CLOSE;
          open_parms.create.buffering_option = FS_OB_PROHIBIT;
          open_parms.create.attribute_mask   = FS_FA_UNRESTRICTED;

          fs_open(output->efs.filename, FS_OA_CREATE, &open_parms, NULL, &rsp);
          if(rsp.open.status == FS_OKAY_S) {
            output_handle_ptr->source_info.source      = CMX_SOURCE_EFS;
            output_handle_ptr->source_info.handle.file = rsp.open.handle;
          } else {
            cmx_free_af_handle(output_handle_ptr);
            output_handle_ptr = NULL;
          }
        }
      }
#endif /* FEATURE_EFS */
    }

    if(output_handle_ptr != NULL) {
      output_handle_ptr->cmd          = CMX_CMD_CREATE_AUDIOP;
      output_handle_ptr->cb_func.base = cb_func;
      output_handle_ptr->client_data  = (void *) client_data;
      output_handle_ptr->file_desc    = output;
      if((cmx_create_audiop_jpeg(jpeg, output_handle_ptr, &total) ==
          CMX_SUCCESS) &&
         (cmx_create_audiop_qcp(qcp, output_handle_ptr, total) ==
          CMX_SUCCESS)) {
        return;
      } else {
#ifdef FEATURE_EFS
        if(output_handle_ptr->source_info.source == CMX_SOURCE_EFS) {
          fs_close(output_handle_ptr->source_info.handle.file, NULL, &rsp);
          fs_remove(output->efs.filename, NULL, &rsp);
        }
#endif /* FEATURE_EFS */
      }
    }
  }

  cmx_af_active = CMX_AF_NONE;
  cmx_free_handle(jpeg);
  cmx_free_handle(qcp);
  cmx_free_handle(output);
  cmx_free_af_handle(output_handle_ptr);

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}
#endif /* FEATURE_AUDIO_POSTCARD */

/* <EJECT> */
#ifdef FEATURE_DLS
/*===========================================================================

FUNCTION cmx_af_dls_callback_processing

DESCRIPTION
  This function processes callbacks for DLS functions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_af_dls_callback_processing (
  snd_af_data_cmd_enum  command,
  cmx_af_handle_type    *handle
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type rsp;
#endif

  if(handle->cmd != CMX_CMD_INVALID) {
#ifdef FEATURE_XMF
    if((handle->cmd == CMX_AF_CMD_PLAY) ||
       (handle->cmd == CMX_AF_CMD_RINGER)) {
      if( handle->status_mask & CMX_AF_STATUS_MASK_DISABLE_PENDING){
        handle->status_mask &= ~CMX_AF_STATUS_MASK_DLS_LOADED;
#ifdef FEATURE_SVG
        if( handle->svg_data == NULL)
#endif /* FEATURE_SVG */
        {
          cmx_free_af_handle( handle);
        }
      } else {
        if(handle->gen_handle != NULL) {
          if(command == SND_CMX_AF_DLS_DONE) {
            handle->status_mask |= CMX_AF_STATUS_MASK_DLS_LOADED;
          }
          xmf_load_and_play_cont(handle->gen_handle);
        } else {
#ifdef FEATURE_EFS
          if(handle->source_info.source == CMX_SOURCE_EFS) {
            fs_close(handle->source_info.handle.file, NULL, &rsp);
          }
#endif /* FEATURE_EFS */
          cmx_free_af_handle(handle);
        }
      }
    } else
#endif /* FEATURE_XMF */
    {
#ifdef FEATURE_EFS
      if(handle->cmd == CMX_AF_CMD_LOAD_DLS) {
        if(handle->source_info.source == CMX_SOURCE_EFS) {
          fs_close(handle->source_info.handle.file, NULL, &rsp);
        }
      }
#endif
      if(handle->cb_func.base != NULL) {
        if(command == SND_CMX_AF_DLS_DONE) {
          handle->cb_func.base(CMX_DONE, handle->client_data);
        } else {
          handle->cb_func.base(CMX_FAILURE, handle->client_data);
        }
      }
      cmx_free_af_handle(handle);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_load_dls_processing

DESCRIPTION
  This function loads a DLS file to be used by all MIDI-bsaed sequences.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_load_dls_processing (
  cmx_handle_type       *handle,
  cmx_cb_func_ptr_type  cb_func,
  void                  *client_data
) {
  cmx_af_handle_type    *handle_ptr = NULL;
  cmx_audfmt_buf_struct *buffer;
  snd_af_access_type    *access_ptr;

  buffer = cmx_get_buffer_struct();
  if(buffer != NULL) {
    cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_AF_CMD_LOAD_DLS);

    if(handle_ptr != NULL) {
      handle_ptr->cmd          = CMX_AF_CMD_LOAD_DLS;
      handle_ptr->buffer       = buffer;
      handle_ptr->cb_func.base = cb_func;
      handle_ptr->client_data  = client_data;

      access_ptr = &handle_ptr->sound.audfmt_sound.audfmt_handle;
      access_ptr->audfmt_ptr = (void *) handle_ptr;
      access_ptr->data_func  = (snd_af_data_cb_func_type) cmx_af_dls_callback;

      snd_audfmt_load_dls(access_ptr, 0);

      if(cb_func != NULL) {
        cb_func(CMX_SUCCESS, client_data);
      }
      return;
    } else {
      cmx_free_buffer_struct(buffer);
    }
  }

  cmx_free_handle(handle);
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_unload_dls_processing

DESCRIPTION
  This function unloads all DLS resources.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_unload_dls_processing (
  cmx_cb_func_ptr_type  cb_func,
  void                  *client_data
) {
  cmx_af_handle_type *handle_ptr = NULL;
  snd_af_access_type *access_ptr;

  handle_ptr = cmx_get_af_handle();
  if(handle_ptr != NULL) {
    handle_ptr->cmd          = CMX_AF_CMD_UNLOAD_DLS;
    handle_ptr->cb_func.base = cb_func;
    handle_ptr->client_data  = client_data;

    access_ptr = &handle_ptr->sound.audfmt_sound.audfmt_handle;
    access_ptr->audfmt_ptr = (void *) handle_ptr;
    access_ptr->data_func  = (snd_af_data_cb_func_type) cmx_af_dls_callback;

    snd_audfmt_unload_dls(access_ptr);

    if(cb_func != NULL) {
      cb_func(CMX_SUCCESS, client_data);
    }
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}
#endif /* FEATURE_DLS */

/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION cmx_audfmt_play_codec

DESCRIPTION
  This function issues a command to play a specified audio codec type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_play_codec
(
  cmx_handle_type                  *handle,
  cmx_codec_spec_type              *codec_spec,
  cmx_cb_func_ptr_type             cb_func,
  void                             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                              = *handle;
      packet->hdr.cmd                       = CMX_AF_CMD_PLAY_CODEC;
      packet->audfmt_play_codec.handle      = phandle;
      packet->audfmt_play_codec.codec_spec.spec_original  = *codec_spec;
      packet->audfmt_play_codec.cb_func     = cb_func;
      packet->audfmt_play_codec.client_data = client_data;
      packet->audfmt_play_codec.interface_src = CMX_INTERFACE_ORIGINAL;
      /* Disable the A/V sync interface */
      packet->audfmt_play_codec.av_sync.av_sync_original.av_sync_interval = 0;
      packet->audfmt_play_codec.av_sync.av_sync_original.av_sync_mode
                                                        = CMX_AV_SYNC_INVALID;
      packet->audfmt_play_codec.av_sync.av_sync_original.av_sync_cb_func
                                                        = NULL;
      MSG_HIGH("cmx_audfmt_play_codec(file_type=%d)",
                (uint16 *) codec_spec->codec_type.file_type,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_play_codec */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play_codec_av_sync_ex

DESCRIPTION
  This function issues a command to play a specified audio codec type and
  provides feedback that can be used to synchronize the audio with other media.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_play_codec_av_sync_ex
(
  cmx_handle_type           *handle,
  cmx_codec_spec_ex_type    *codec_spec,
  cmx_av_sync_ex_type       *av_sync,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  if ((av_sync == NULL) ||
      ((av_sync->av_sync_cb_func != NULL) &&
       (av_sync->av_sync_interval != 0)   &&
       (av_sync->av_sync_mode < CMX_AV_SYNC_INVALID))) {
    phandle = cmx_get_handle();
    if(phandle != NULL) {
      packet = cmx_cmd_get_pkt ();
      if(packet != NULL) {
        *phandle                              = *handle;
        packet->hdr.cmd                       = CMX_AF_CMD_PLAY_CODEC;
        packet->audfmt_play_codec.handle       = phandle;
        packet->audfmt_play_codec.interface_src = CMX_INTERFACE_EXTENDED;
        packet->audfmt_play_codec.codec_spec.spec_extended = *codec_spec;
        if (av_sync != NULL) {
          packet->audfmt_play_codec.av_sync.av_sync_extended = *av_sync;
        } else {
          av_sync = &packet->audfmt_play_codec.av_sync.av_sync_extended;
          av_sync->av_sync_interval = 0;
          av_sync->av_sync_mode     = CMX_AV_SYNC_INVALID;
          av_sync->av_sync_cb_func  = NULL;
        }
        packet->audfmt_play_codec.cb_func     = cb_func;
        packet->audfmt_play_codec.client_data = client_data;
        MSG_HIGH(
          "cmx_audfmt_play_codec_av_sync_ex(file_type=%d, Interval=%d, Mode=%d",
          (uint16 *) codec_spec->codec_type.file_type,
          av_sync->av_sync_interval, (uint16 *) av_sync->av_sync_mode);
        cmx_cmd (packet);
        return;
      } else {
        cmx_free_handle(phandle);
      }
    }
  }


  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION cmx_audfmt_play_codec_av_sync

DESCRIPTION
  This function issues a command to play a specified audio codec type and
  provide feedback that can be used to synchronize the audio with other media.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_play_codec_av_sync
(
  cmx_handle_type                  *handle,
  cmx_codec_spec_type              *codec_spec,
  cmx_av_sync_type                 *av_sync,
  cmx_cb_func_ptr_type             cb_func,
  void                             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  if ((av_sync == NULL) ||
      ((av_sync->av_sync_cb_func != NULL) &&
       (av_sync->av_sync_interval != 0)   &&
       (av_sync->av_sync_mode < CMX_AV_SYNC_INVALID))) {
    phandle = cmx_get_handle();
    if(phandle != NULL) {
      packet = cmx_cmd_get_pkt ();
      if(packet != NULL) {
        *phandle                              = *handle;
        packet->hdr.cmd                       = CMX_AF_CMD_PLAY_CODEC;
        packet->audfmt_play_codec.handle      = phandle;
        packet->audfmt_play_codec.interface_src = CMX_INTERFACE_ORIGINAL;
        packet->audfmt_play_codec.codec_spec.spec_original = *codec_spec;
        if (av_sync != NULL) {
          packet->audfmt_play_codec.av_sync.av_sync_original = *av_sync;
        } else {
          av_sync = &packet->audfmt_play_codec.av_sync.av_sync_original;
          av_sync->av_sync_interval = 0;
          av_sync->av_sync_mode     = CMX_AV_SYNC_INVALID;
          av_sync->av_sync_cb_func  = NULL;
        }
        packet->audfmt_play_codec.cb_func     = cb_func;
        packet->audfmt_play_codec.client_data = client_data;
        MSG_HIGH(
          "cmx_audfmt_play_codec_av_sync(file_type=%d, Interval=%d, Mode=%d",
          (uint16 *) codec_spec->codec_type.file_type,
          av_sync->av_sync_interval, (uint16 *) av_sync->av_sync_mode);
        cmx_cmd (packet);
        return;
      } else {
        cmx_free_handle(phandle);
      }
    }
  }


  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_play_codec_av_sync */

/* <EJECT> */
/*===========================================================================
FUNCTION cmx_audfmt_avsync_playback

DESCRIPTION
  This function issues a command to play a specified audio codec type and
  provide feedback that can be used to synchronize the audio with other media.
  This API is different from cmx_audfmt_play_codec_av_sync as it provides the
  type of ABORT to the client (caller) via callback function of type
  cmx_audfmt_play_cb_func_ptr_type

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_avsync_playback
(
  cmx_handle_type                  *handle,
  cmx_codec_spec_type              *codec_spec,
  cmx_av_sync_type                 *av_sync,
  cmx_audfmt_play_cb_func_ptr_type  cb_func,
  void                             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  if ((av_sync == NULL) ||
      ((av_sync->av_sync_cb_func != NULL) &&
       (av_sync->av_sync_interval != 0)   &&
       (av_sync->av_sync_mode < CMX_AV_SYNC_INVALID))) {
    phandle = cmx_get_handle();
    if(phandle != NULL) {
      packet = cmx_cmd_get_pkt ();
      if(packet != NULL) {
        *phandle                              = *handle;
        packet->hdr.cmd                       = CMX_AF_CMD_AVSYNC_PLAYBACK;
        packet->audfmt_play_codec.handle      = phandle;
        packet->audfmt_play_codec.interface_src = CMX_INTERFACE_ORIGINAL;
        packet->audfmt_play_codec.codec_spec.spec_original = *codec_spec;
        if (av_sync != NULL) {
          packet->audfmt_play_codec.av_sync.av_sync_original = *av_sync;
        } else {
          av_sync = &packet->audfmt_play_codec.av_sync.av_sync_original;
          av_sync->av_sync_interval = 0;
          av_sync->av_sync_mode     = CMX_AV_SYNC_INVALID;
          av_sync->av_sync_cb_func  = NULL;
        }
        packet->audfmt_play_codec.av_sync_cb_func     = cb_func;
        packet->audfmt_play_codec.client_data = client_data;
        MSG_HIGH(
          "cmx_audfmt_avsync_playback(file_type=%d, Interval=%d, Mode=%d",
          (uint16 *) codec_spec->codec_type.file_type,
          av_sync->av_sync_interval, (uint16 *) av_sync->av_sync_mode);
        cmx_cmd (packet);
        return;
      } else {
        cmx_free_handle(phandle);
      }
    }
  }


  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data,NULL);
  }

} /* end cmx_audfmt_avsync_playback */


/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play

DESCRIPTION
  This function issues a command to play an audio file, using header info
  to determine the format.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_play
(
  cmx_handle_type                  *handle,
  cmx_af_callback_enum_type        cb_type,
  cmx_audfmt_start_enum_type       start_position,
  cmx_audfmt_play_cb_func_ptr_type cb_func,
  void                             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                                 = *handle;
      packet->hdr.cmd                          = CMX_AF_CMD_PLAY;
      packet->audfmt_play.handle               = phandle;
      packet->audfmt_play.param.multiseq_flag  = FALSE;
      packet->audfmt_play.param.cb_type        = cb_type;
      packet->audfmt_play.param.start_position = start_position;
      packet->audfmt_play.param.repeat_state   = CMX_AUDFMT_REPEAT_DISABLED;
      packet->audfmt_play.param.repeat_count   = 0;
      packet->audfmt_play.param.repeat_ms      = 0;
      packet->audfmt_play.cb_func              = cb_func;
      packet->audfmt_play.client_data          = client_data;
      MSG_HIGH("cmx_audfmt_play() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data, NULL);
  }

} /* end cmx_audfmt_play */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play_multi

DESCRIPTION
  This function issues a command to play an audio file in multisequencer
  mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_play_multi (
  cmx_handle_type                  *handle,
  cmx_audfmt_multi_param_type      *param,
  cmx_audfmt_play_cb_func_ptr_type cb_func,
  void                             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                                 = *handle;
      packet->hdr.cmd                          = CMX_AF_CMD_PLAY;
      packet->audfmt_play.handle               = phandle;
      packet->audfmt_play.param.multiseq_flag  = TRUE;
      packet->audfmt_play.param.cb_type        = param->cb_type;
      packet->audfmt_play.param.repeat_state   = param->repeat_state;
      packet->audfmt_play.param.repeat_count   = param->repeat_count;
      packet->audfmt_play.param.repeat_ms      = param->repeat_ms;
      packet->audfmt_play.param.start_position = CMX_AUDFMT_START_NORMAL;
      packet->audfmt_play.cb_func              = cb_func;
      packet->audfmt_play.client_data          = client_data;
      MSG_HIGH("cmx_audfmt_play_multi() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data, NULL);
  }

} /* end cmx_audfmt_play_multi */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_ringer

DESCRIPTION
  This function issues a command to play a midi ring.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_ringer
(
  cmx_handle_type             *handle,
  uint16                      repeat_timer,
  cmx_audfmt_start_enum_type  start_position,
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                      = CMX_AF_CMD_RINGER;
      packet->audfmt_ringer.handle         = phandle;
      packet->audfmt_ringer.repeat_timer   = repeat_timer;
      packet->audfmt_ringer.start_position = start_position;
      packet->audfmt_ringer.cb_func        = cb_func;
      packet->audfmt_ringer.client_data    = client_data;
      MSG_HIGH("cmx_audfmt_ringer() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_ringer */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_stop

DESCRIPTION
  This function issues a command to stop midi/WebAudio playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_stop (
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd           = CMX_AF_CMD_STOP;
    packet->base.cb_func      = cb_func;
    packet->base.client_data  = client_data;
    MSG_HIGH("cmx_audfmt_stop() called",0,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_stop */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_rewind

DESCRIPTION
  This function issues a command to rewind midi/WebAudio playback the
  indicated number of milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_rewind
(
  uint32               time_ms,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_AF_CMD_REWIND;
    packet->audfmt_play_ctl.time_ms      = time_ms;
    packet->audfmt_play_ctl.cb_func      = cb_func;
    packet->audfmt_play_ctl.client_data  = client_data;
    MSG_MED("cmx_audfmt_rewind(time_ms = %d) called",time_ms,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_rewind */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_fforward

DESCRIPTION
  This function issues a command to fast forward midi/WebAudio playback the
  indicated number of milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_fforward
(
  uint32               time_ms,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_AF_CMD_FFORWARD;
    packet->audfmt_play_ctl.time_ms      = time_ms;
    packet->audfmt_play_ctl.cb_func      = cb_func;
    packet->audfmt_play_ctl.client_data  = client_data;
    MSG_MED("cmx_audfmt_fforward(time_ms = %d) called",time_ms,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_fforward */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pause

DESCRIPTION
  This function issues a command to pause midi/WebAudio playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_pause
(
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd           = CMX_AF_CMD_PAUSE;
    packet->base.cb_func      = cb_func;
    packet->base.client_data  = client_data;
    MSG_MED("cmx_audfmt_pause() called",0,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_pause */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seek_pause

DESCRIPTION
  This function issues a command to prepare for a rewind/fast-forward
  command for midi/WebAudio playback.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_seek_pause (
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd           = CMX_AF_CMD_SEEK_PAUSE;
    packet->base.cb_func      = cb_func;
    packet->base.client_data  = client_data;
    MSG_MED("cmx_audfmt_seek_pause() called",0,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_seek_pause */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_resume

DESCRIPTION
  This function issues a command to resume midi/WebAudio playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_resume
(
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd           = CMX_AF_CMD_RESUME;
    packet->base.cb_func      = cb_func;
    packet->base.client_data  = client_data;
    MSG_MED("cmx_audfmt_resume() called",0,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_resume */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_tempo

DESCRIPTION
  This function issues a command to adjust the midi/WebAudio playback tempo.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_tempo
(
  uint32               tempo_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_AF_CMD_TEMPO;
    packet->audfmt_play_ctl.time_ms      = tempo_factor;
    packet->audfmt_play_ctl.cb_func      = cb_func;
    packet->audfmt_play_ctl.client_data  = client_data;
    MSG_MED("cmx_audfmt_tempo(tempo_factor=%d) called",tempo_factor,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_tempo */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_tune

DESCRIPTION
  This function issues a command to adjust the midi/WebAudio playback tune.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_tune
(
  uint32               tune_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_AF_CMD_TUNE;
    packet->audfmt_play_ctl.time_ms      = tune_factor;
    packet->audfmt_play_ctl.cb_func      = cb_func;
    packet->audfmt_play_ctl.client_data  = client_data;
    MSG_MED("cmx_audfmt_tune(tune_factor=%d) called",tune_factor,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_tune */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pan

DESCRIPTION
  This function issues a command to adjust the playback stereo pan.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_pan
(
  uint32               pan_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_AF_CMD_PAN;
    packet->audfmt_play_ctl.time_ms      = pan_factor;
    packet->audfmt_play_ctl.cb_func      = cb_func;
    packet->audfmt_play_ctl.client_data  = client_data;
    MSG_MED("cmx_audfmt_pan(pan_factor=%d) called",pan_factor,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_pan */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_sequence_cmd

DESCRIPTION
  This function issues a command to adjust the playback of a sequence.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_sequence_cmd
(
  void                      *sequence,
  cmx_audfmt_seq_param_type *seq_param,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
) {
  cmx_packet_type *packet;

  if(seq_param != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      switch (seq_param->cmd) {
        case CMX_AUDFMT_SEQ_CMD_STOP:
          packet->hdr.cmd                   = CMX_AF_CMD_SEQ_STOP;
          MSG_MED("cmx_audfmt_sequence_cmd(seq=0x%x, CMX_AF_CMD_SEQ_STOP)",
                   sequence,0,0);
          break;

        case CMX_AUDFMT_SEQ_CMD_PAUSE:
          packet->hdr.cmd                   = CMX_AF_CMD_SEQ_PAUSE;
          MSG_MED("cmx_audfmt_sequence_cmd(seq=0x%x, CMX_AF_CMD_SEQ_PAUSE)",
                   sequence,0,0);
          break;

        case CMX_AUDFMT_SEQ_CMD_RESUME:
          packet->hdr.cmd                   = CMX_AF_CMD_SEQ_RESUME;
          MSG_MED("cmx_audfmt_sequence_cmd(seq=0x%x, CMX_AF_CMD_SEQ_RESUME)",
                   sequence,0,0);
          break;

        case CMX_AUDFMT_SEQ_CMD_FFORWARD:
          packet->hdr.cmd                   = CMX_AF_CMD_SEQ_FFORWARD;
          packet->audfmt_seq_cmd.num        = seq_param->num;
          MSG_MED(
            "cmx_audfmt_sequence_cmd(seq=0x%x, CMX_AF_CMD_SEQ_FFORWARD=%d)",
            sequence, seq_param->num,0);
          break;

        case CMX_AUDFMT_SEQ_CMD_REWIND:
          packet->hdr.cmd                   = CMX_AF_CMD_SEQ_REWIND;
          packet->audfmt_seq_cmd.num        = seq_param->num;
          MSG_MED(
            "cmx_audfmt_sequence_cmd(seq=0x%x, CMX_AF_CMD_SEQ_REWIND=%d)",
            sequence, seq_param->num,0);
          break;

        case CMX_AUDFMT_SEQ_CMD_TEMPO:
          packet->hdr.cmd                   = CMX_AF_CMD_SEQ_TEMPO;
          packet->audfmt_seq_cmd.num        = seq_param->num;
          MSG_MED("cmx_audfmt_sequence_cmd(seq=0x%x, CMX_AF_CMD_SEQ_TEMPO=%d)",
                   sequence, seq_param->num,0);
          break;

        case CMX_AUDFMT_SEQ_CMD_TUNE:
          packet->hdr.cmd                   = CMX_AF_CMD_SEQ_TUNE;
          packet->audfmt_seq_cmd.num        = seq_param->num;
          MSG_MED("cmx_audfmt_sequence_cmd(seq=0x%x, CMX_AF_CMD_SEQ_TUNE=%d)",
                   sequence, seq_param->num,0);
          break;

        case CMX_AUDFMT_SEQ_CMD_PAN:
          packet->hdr.cmd                   = CMX_AF_CMD_SEQ_PAN;
          packet->audfmt_seq_cmd.num        = seq_param->num;
          MSG_MED("cmx_audfmt_sequence_cmd(seq=0x%x, CMX_AF_CMD_SEQ_PAN=%d)",
                   sequence, seq_param->num,0);
          break;

        case CMX_AUDFMT_SEQ_CMD_VOLUME:
          packet->hdr.cmd                   = CMX_AF_CMD_SEQ_VOL;
          packet->audfmt_seq_cmd.num        = seq_param->num;
          MSG_MED("cmx_audfmt_sequence_cmd(seq=0x%x, CMX_AF_CMD_SEQ_VOL=%d)",
                   sequence, seq_param->num,0);
          break;

        default:
          q_put (&cmx_free_q, &packet->hdr.link);
          if(cb_func != NULL) {
            cb_func(CMX_FAILURE, client_data);
          }
          return;
      }

      packet->audfmt_seq_cmd.sequence     = sequence;
      packet->audfmt_seq_cmd.cb_func      = cb_func;
      packet->audfmt_seq_cmd.client_data  = client_data;
      cmx_cmd (packet);
      return;
    }
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_audfmt_sequence_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_time

DESCRIPTION
  This function issues a command to calculate the time of the indicated
  midi/WebAudio file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_get_time
(
  cmx_handle_type                   *handle,
  cmx_audfmt_time_cb_func_ptr_type  cb_func,
  void                              *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                 = CMX_AF_CMD_GET_TIME;
      packet->audfmt_time.handle      = phandle;
      packet->audfmt_time.cb_func     = cb_func;
      packet->audfmt_time.client_data = client_data;
      MSG_HIGH("cmx_audfmt_get_time() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data, 0);
  }
} /* end cmx_audfmt_get_time */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_time_abort

DESCRIPTION
  This function issues a command to abort the midi/WebAudio get time service.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_get_time_abort ( void )
{
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd = CMX_AF_CMD_GET_TIME_ABORT;
    MSG_HIGH("cmx_audfmt_get_time_abort() called",0,0,0);
    cmx_cmd (packet);
  }

} /* end cmx_audfmt_get_time_abort */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_audio_spec

DESCRIPTION
  This function issues a command to get audio spec.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_get_specs
(
  cmx_handle_type                   *handle,
  cmx_audfmt_spec_cb_func_ptr_type  cb_func,
  void                              *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                     = CMX_AF_CMD_GET_SPEC;
      packet->audfmt_get_spec.handle      = phandle;
      packet->audfmt_get_spec.cb_func     = cb_func;
      packet->audfmt_get_spec.client_data = client_data;
      MSG_HIGH("cmx_audfmt_get_specs() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }
  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data, 0);
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audio_playback

DESCRIPTION
  This function issues a command to play an audio file.

  Note: This function has been added to support remote client controlled data
        source audio playback. This interface is very similar to
        cmx_audfmt_play() except that cmx_client_req_func_ptr_type is
        included in the parameter list. It is assumed that callbacks are to be
        returned by reference (see cmx_audfmt_play()).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audio_playback (
  cmx_handle_type                  *handle,
  cmx_audfmt_play_cb_func_ptr_type cb_func,
  cmx_client_req_func_ptr_type     data_req_ptr,
  void                             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                           = *handle;
      if(phandle->source_type == CMX_SOURCE_CLIENT) {
        phandle->client.data_req_ptr     = data_req_ptr;
      }
      packet->hdr.cmd                          = CMX_AF_CMD_PLAY;
      packet->audfmt_play.handle               = phandle;
      packet->audfmt_play.param.multiseq_flag  = FALSE;
      packet->audfmt_play.param.cb_type        = CMX_AF_CALLBACK_REFERENCE;
      packet->audfmt_play.param.start_position = CMX_AUDFMT_START_NORMAL;
      packet->audfmt_play.param.repeat_state   = CMX_AUDFMT_REPEAT_DISABLED;
      packet->audfmt_play.param.repeat_count   = 0;
      packet->audfmt_play.param.repeat_ms      = 0;
      packet->audfmt_play.cb_func              = cb_func;
      packet->audfmt_play.client_data          = client_data;

      MSG_HIGH("cmx_audio_playback() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data, NULL);
  }

}

/* <EJECT> */
#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*===========================================================================

FUNCTION cmx_audio_playback_control_ex

DESCRIPTION
This function issues a command to a currently active audio playback. Some of
commands could come with command  parameters. Caller of this function needs to
make sure CMX_SUCCESS has been returned for playback command before issuing
command to this function.

DEPENDENCIES
  This function should be called after play command has been carried out.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audio_playback_control_ex (
  cmx_audio_playback_cmd_enum_type   command,
  cmx_audio_playback_cmd_param_type  *cmd_param,
  cmx_cb_func_ptr_type               cb_func,
  const void                         *client_data
) {
  cmx_packet_type *packet;
  boolean         valid_cmd = TRUE;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    switch(command) {
      case CMX_AUDIO_PB_CMD_DUAL_MONO_MODE:
        if (cmd_param == NULL) {
          valid_cmd = FALSE;
          break;
        }
        packet->hdr.cmd = CMX_AF_CMD_CHANNEL_DUAL_MONO;
        packet->dual_mono_mode.mode = cmd_param->dm_mode_cmd.chan_mode;
        packet->dual_mono_mode.cb_func      = cb_func;
        packet->dual_mono_mode.client_data  = (void *) client_data;
        cmx_cmd (packet);
        MSG_MED("cmx_audio_playback_control_ex(PB_CMD_DUAL_MONO_MODE)",0,0,0);
        break;

      case CMX_AUDIO_PB_CMD_SBR_MODE:
        if (cmd_param == NULL) {
          valid_cmd = FALSE;
          break;
        }
        packet->hdr.cmd = CMX_AF_CMD_SBR_MODE;
        packet->sbr_mode.mode = cmd_param->sbr_mode_cmd.mode;
        packet->sbr_mode.cb_func      = cb_func;
        packet->sbr_mode.client_data  = (void *) client_data;
        cmx_cmd (packet);
        MSG_MED("cmx_audio_playback_control_ex(PB_CMD_SBR_MODE)",0,0,0);
        break;

      default:
        MSG_ERROR("cmx_audio_playback_control_ex(UNKNOWN_CMD)", 0, 0, 0);
        valid_cmd = FALSE;
        return;
    }
  } else if (cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
    return;
  }

  if (valid_cmd == FALSE) {
    q_put (&cmx_free_q, &packet->hdr.link);
    if(cb_func != NULL) {
      cb_func(CMX_FAILURE, client_data);
    }
  }
}
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audio_playback_control

DESCRIPTION
  This function issues a command to a currently active audio file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audio_playback_control (
  cmx_audio_playback_cmd_enum_type command,
  cmx_cb_func_ptr_type             cb_func,
  const void                       *client_data
) {
  cmx_packet_type *packet;
  cmx_cmd_type    cmd;

  switch(command) {
    case CMX_AUDIO_PB_CMD_STOP:
      cmd = CMX_AF_CMD_STOP;
      MSG_HIGH("cmx_audio_playback_control(CMX_AUDIO_PB_CMD_STOP)",0,0,0);
      break;

    case CMX_AUDIO_PB_CMD_PAUSE:
      cmd = CMX_AF_CMD_PAUSE;
      MSG_MED("cmx_audio_playback_control(CMX_AUDIO_PB_CMD_PAUSE)",0,0,0);
      break;

    case CMX_AUDIO_PB_CMD_RESUME:
      cmd = CMX_AF_CMD_RESUME;
      MSG_MED("cmx_audio_playback_control(CMX_AUDIO_PB_CMD_RESUME)",0,0,0);
      break;

    case CMX_AUDIO_PB_CMD_FLUSH:
      cmd = CMX_AF_CMD_FLUSH;
      MSG_MED("cmx_audio_playback_control(CMX_AUDIO_PB_CMD_FLUSH)",0,0,0);
      break;

#ifdef FEATURE_AUDIO_PCR_SUPPORT
    case CMX_AUDIO_PB_CMD_ADD_SAMPLE:
      cmd = CMX_AF_CMD_ADD_SAMPLE;
      MSG_MED("cmx_audio_playback_control(CMX_AUDIO_PB_CMD_ADD_SAMPLE)",0,0,0);
      break;

    case CMX_AUDIO_PB_CMD_DROP_SAMPLE:
      cmd = CMX_AF_CMD_DROP_SAMPLE;
      MSG_MED("cmx_audio_playback_control(CMX_AUDIO_PB_CMD_DROP_SAMPLE)",0,0,0);
      break;
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

    default:
      if(cb_func != NULL) {
        cb_func(CMX_FAILURE, client_data);
      }
      return;
  }

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd           = cmd;
    packet->base.cb_func      = cb_func;
    packet->base.client_data  = (void *) client_data;
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION cmx_qcp_get_data_size

DESCRIPTION
  This function retrieves the size of the data chunk in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_get_data_size (
  cmx_handle_type        *handle,
  uint32                 *num_bytes,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle  = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                        = *handle;
      packet->hdr.cmd                 = CMX_QCP_GET_DATA_SIZE;
      packet->qcp_func.handle         = phandle;
      packet->qcp_func.num_bytes      = num_bytes;
      packet->qcp_func.cb_func        = cb_func;
      packet->qcp_func.client_data    = (void *) client_data;
      MSG_MED("cmx_qcp_get_data_size() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_get_frames

DESCRIPTION
  This function retrieves the number of frames in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_get_frames (
  cmx_handle_type        *handle,
  uint32                 *num_frames,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle  = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                        = *handle;
      packet->hdr.cmd                 = CMX_QCP_GET_FRAMES;
      packet->qcp_func.handle         = phandle;
      packet->qcp_func.num_bytes      = num_frames;
      packet->qcp_func.cb_func        = cb_func;
      packet->qcp_func.client_data    = (void *) client_data;
      MSG_MED("cmx_qcp_get_frames() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_get_cnfg

DESCRIPTION
  This function retrieves the cnfg chunk in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_get_cnfg (
  cmx_handle_type        *handle,
  uint16                 *cnfg,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle  = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                        = *handle;
      packet->hdr.cmd                 = CMX_QCP_GET_CNFG;
      packet->qcp_func.handle         = phandle;
      packet->qcp_func.num_bytes      = (uint32 *) cnfg;
      packet->qcp_func.cb_func        = cb_func;
      packet->qcp_func.client_data    = (void *) client_data;
      MSG_MED("cmx_qcp_get_cnfg() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_set_cnfg

DESCRIPTION
  This function sets the cnfg chunk in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_set_cnfg (
  cmx_handle_type        *handle,
  uint16                 cnfg,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle  = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                        = *handle;
      packet->hdr.cmd                 = CMX_QCP_SET_CNFG;
      packet->qcp_func.handle         = phandle;
      packet->qcp_func.length         = cnfg;
      packet->qcp_func.cb_func        = cb_func;
      packet->qcp_func.client_data    = (void *) client_data;
      MSG_MED("cmx_qcp_set_cnfg() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_get_labl

DESCRIPTION
  This function retrieves the labl chunk from a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_get_labl (
  cmx_handle_type        *handle,
  uint32                 *labl_length,
  uint8                  *labl_buffer,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle  = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                        = *handle;
      packet->hdr.cmd                 = CMX_QCP_GET_LABL;
      packet->qcp_func.handle         = phandle;
      packet->qcp_func.num_bytes      = labl_length;
      packet->qcp_func.buffer         = labl_buffer;
      packet->qcp_func.cb_func        = cb_func;
      packet->qcp_func.client_data    = (void *) client_data;
      MSG_MED("cmx_qcp_get_labl() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_set_labl

DESCRIPTION
  This function sets the labl chunk in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_set_labl (
  cmx_handle_type        *handle,
  uint32                 labl_length,
  uint8                  *labl_buffer,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle  = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                        = *handle;
      packet->hdr.cmd                 = CMX_QCP_SET_LABL;
      packet->qcp_func.handle         = phandle;
      packet->qcp_func.length         = labl_length;
      packet->qcp_func.buffer         = labl_buffer;
      packet->qcp_func.cb_func        = cb_func;
      packet->qcp_func.client_data    = (void *) client_data;
      MSG_MED("cmx_qcp_set_labl() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_get_text

DESCRIPTION
  This function retrieves the text chunk from a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_get_text (
  cmx_handle_type        *handle,
  uint32                 *text_length,
  uint8                  *text_buffer,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle  = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                        = *handle;
      packet->hdr.cmd                 = CMX_QCP_GET_TEXT;
      packet->qcp_func.handle         = phandle;
      packet->qcp_func.num_bytes      = text_length;
      packet->qcp_func.buffer         = text_buffer;
      packet->qcp_func.cb_func        = cb_func;
      packet->qcp_func.client_data    = (void *) client_data;
      MSG_MED("cmx_qcp_get_text() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_set_text

DESCRIPTION
  This function sets the text chunk in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_set_text (
  cmx_handle_type        *handle,
  uint32                 text_length,
  uint8                  *text_buffer,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle  = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                        = *handle;
      packet->hdr.cmd                 = CMX_QCP_SET_TEXT;
      packet->qcp_func.handle         = phandle;
      packet->qcp_func.length         = text_length;
      packet->qcp_func.buffer         = text_buffer;
      packet->qcp_func.cb_func        = cb_func;
      packet->qcp_func.client_data    = (void *) client_data;
      MSG_MED("cmx_qcp_set_text() called",0,0,0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
    return;
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
#ifdef FEATURE_EFS
/*===========================================================================

FUNCTION cmx_qcp_fs_space_available

DESCRIPTION
  This function checks the EFS to see if there is enough space available
  to start recording a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if there is space available to start recording.
  FALSE if there is not sufficient space available to start recording.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmx_qcp_fs_space_available ( void )
{
  return(fs_space_available() >=
                         (CMX_QCP_RECORD_MIN_SIZE + CMX_QCP_FS_OVERHEAD));
}
#endif /* FEATURE_EFS */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_reverse

DESCRIPTION
  This function sends a command to start QCP recording on reverse link.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_record_reverse (
  cmx_handle_type       *handle,
  cmx_qcp_rec_para_type *rec_param,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle  = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                         = *handle;
      packet->hdr.cmd                  = CMX_QCP_CMD_REC;
      packet->qcp_record.tx_handle     = phandle;
      packet->qcp_record.rx_handle     = NULL;

      packet->qcp_record.rec_param.mm_rec_format = CMX_MM_FORMAT_QCP;
      packet->qcp_record.rec_param.rec_path      = CMX_MM_REC_LINK_REVERSE;
      packet->qcp_record.rec_param.format        = rec_param->format;
      packet->qcp_record.rec_param.report_ms     = rec_param->report_ms;
      packet->qcp_record.rec_param.auto_stop_ms  = rec_param->auto_stop_ms;
      packet->qcp_record.rec_param.data_req_ms   = 0;

      packet->qcp_record.cb_func       = cb_func;
      packet->qcp_record.client_data   = client_data;
      MSG_HIGH("cmx_qcp_record_reverse(format=%d, report_ms=%d, auto_stop=%d)",
               (uint16)rec_param->format,
               rec_param->report_ms, rec_param->auto_stop_ms);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_forward

DESCRIPTION
  This function sends a command to start QCP recording on forward link.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_record_forward (
  cmx_handle_type       *handle,
  cmx_qcp_rec_para_type *rec_param,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle  = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                          = *handle;
      packet->hdr.cmd                   = CMX_QCP_CMD_REC;
      packet->qcp_record.tx_handle      = NULL;
      packet->qcp_record.rx_handle      = phandle;

      packet->qcp_record.rec_param.mm_rec_format = CMX_MM_FORMAT_QCP;
      packet->qcp_record.rec_param.rec_path      = CMX_MM_REC_LINK_FORWARD;
      packet->qcp_record.rec_param.format        = rec_param->format;
      packet->qcp_record.rec_param.report_ms     = rec_param->report_ms;
      packet->qcp_record.rec_param.auto_stop_ms  = rec_param->auto_stop_ms;
      packet->qcp_record.rec_param.data_req_ms   = 0;

      packet->qcp_record.cb_func        = cb_func;
      packet->qcp_record.client_data    = client_data;

      MSG_HIGH("cmx_qcp_record_forward(format=%d, report_ms=%d, auto_stop=%d)",
               (uint16)rec_param->format,
               rec_param->report_ms, rec_param->auto_stop_ms);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_forward_both

DESCRIPTION
  This function sends a command to start QCP recording on forward link.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_record_both (
  cmx_handle_type       *handle,
  cmx_qcp_rec_para_type *rec_param,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle  = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                          = *handle;
      packet->hdr.cmd                   = CMX_QCP_CMD_REC;
      packet->qcp_record.tx_handle      = NULL;
      packet->qcp_record.rx_handle      = phandle;

      packet->qcp_record.rec_param.mm_rec_format = CMX_MM_FORMAT_QCP;
      packet->qcp_record.rec_param.rec_path      = CMX_MM_REC_LINK_BOTH;
      packet->qcp_record.rec_param.format        = rec_param->format;
      packet->qcp_record.rec_param.report_ms     = rec_param->report_ms;
      packet->qcp_record.rec_param.auto_stop_ms  = rec_param->auto_stop_ms;
      packet->qcp_record.rec_param.data_req_ms   = 0;

      packet->qcp_record.cb_func        = cb_func;
      packet->qcp_record.client_data    = client_data;

      MSG_HIGH("cmx_qcp_record_forward(format=%d, report_ms=%d, auto_stop=%d)",
               (uint16)rec_param->format,
               rec_param->report_ms, rec_param->auto_stop_ms);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_stop

DESCRIPTION
  This function sends a command to stop QCP recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_record_stop (
  cmx_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd           = CMX_QCP_CMD_REC_STOP;
    packet->base.cb_func      = cb_func;
    packet->base.client_data  = (void *) client_data;
    MSG_HIGH("cmx_qcp_record_stop() called",0,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_pause

DESCRIPTION
  This function sends a command to pause QCP recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_record_pause (
  cmx_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd           = CMX_QCP_CMD_REC_PAUSE;
    packet->base.cb_func      = cb_func;
    packet->base.client_data  = (void *) client_data;
    MSG_MED("cmx_qcp_record_pause() called",0,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_resume

DESCRIPTION
  This function sends a command to resume QCP recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_record_resume (
  cmx_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd           = CMX_QCP_CMD_REC_RESUME;
    packet->base.cb_func      = cb_func;
    packet->base.client_data  = (void *) client_data;
    MSG_MED("cmx_qcp_record_resume() called",0,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION cmx_pcm_record

DESCRIPTION
  This function sends a command to start PCM recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_pcm_record (
  cmx_handle_type       *handle,
  cmx_pcm_rec_para_type *rec_param,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle  = cmx_get_handle();
  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle                          = *handle;
      packet->hdr.cmd                   = CMX_PCM_CMD_REC;
      packet->pcm_record.handle         = phandle;
      packet->pcm_record.rec_param      = *rec_param;
      packet->pcm_record.cb_func        = cb_func;
      packet->pcm_record.client_data    = client_data;
      MSG_HIGH("cmx_pcm_record(link=%d) called", (uint16)rec_param->link, 0, 0);
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_stop

DESCRIPTION
  This function sends a command to stop PCM recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_pcm_record_stop (
  cmx_pcm_rec_link_type link,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                  = CMX_PCM_CMD_REC_STOP;
    packet->pcm_rec_cmd.link         = link;
    packet->pcm_rec_cmd.cb_func      = cb_func;
    packet->pcm_rec_cmd.client_data  = client_data;
    MSG_HIGH("cmx_pcm_record_stop() called", 0,0,0);
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_pause

DESCRIPTION
  This function sends a command to pause PCM recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_pcm_record_pause (
  cmx_pcm_rec_link_type link,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                  = CMX_PCM_CMD_REC_PAUSE;
    packet->pcm_rec_cmd.link         = link;
    packet->pcm_rec_cmd.cb_func      = cb_func;
    packet->pcm_rec_cmd.client_data  = client_data;
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_resume

DESCRIPTION
  This function sends a command to resume PCM recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_pcm_record_resume (
  cmx_pcm_rec_link_type link,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                  = CMX_PCM_CMD_REC_RESUME;
    packet->pcm_rec_cmd.link         = link;
    packet->pcm_rec_cmd.cb_func      = cb_func;
    packet->pcm_rec_cmd.client_data  = client_data;
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION cmx_mm_record

DESCRIPTION
  This function sends a command to start a MultiMedia recording session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_mm_record (
  cmx_handle_type                *handle,
  cmx_mm_rec_operation_mode_type op,
  cmx_mm_rec_param_type          *rec_param,
  cmx_cb_func_ptr_type           cb_func,
  const void                     *client_data
)
{
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  if (rec_param->mm_rec_param.mm_rec_format == CMX_MM_FORMAT_QCP) {

    if (((rec_param->mm_rec_param.cmx_mm_rec_qcp_para.format <
         CMX_QCP_FORMAT_FIXED_FULL_13K) ||
         (rec_param->mm_rec_param.cmx_mm_rec_qcp_para.format >=
          CMX_QCP_FORMAT_INVALID)) ||
        ((rec_param->mm_rec_param.cmx_mm_rec_qcp_para.sync_cb_func != NULL) &&
         (rec_param->mm_rec_param.cmx_mm_rec_qcp_para.data_req_ms  != 0)    &&
         (rec_param->mm_rec_param.cmx_mm_rec_qcp_para.data_req_ms  < 20 ||
         (rec_param->mm_rec_param.cmx_mm_rec_qcp_para.data_req_ms -
         (rec_param->mm_rec_param.cmx_mm_rec_qcp_para.data_req_ms/20)*20))))

    {

      if (cb_func != NULL) {
        cb_func(CMX_ERROR, client_data);
      }

      return;
    }
  }

  phandle  = cmx_get_handle();

  if (phandle != NULL) {

    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {

      *phandle                       = *handle;

      packet->mm_record.hdr.cmd      = CMX_MM_CMD_REC;
      packet->mm_record.mm_handle    = phandle;
      packet->mm_record.rec_param    = *rec_param;
      packet->mm_record.cb_func      = cb_func;
      packet->mm_record.client_data  = client_data;

      cmx_cmd (packet);

      return;

    } else {
      cmx_free_handle(phandle);
    }
  }

  if (cb_func != NULL) {
    cb_func (CMX_FAILURE, client_data);
  }
} /* cmx_mm_record */

/*===========================================================================

FUNCTION cmx_mm_record_stop

DESCRIPTION
  This function sends a command to stop a MultiMedia record session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_mm_record_stop (
  cmx_mm_rec_link_type   link,        /* Direction to record           */
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
)
{
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {

    packet->mm_rec_cmd.hdr.cmd     = CMX_MM_CMD_REC_STOP;
    packet->mm_rec_cmd.link        = link;
    packet->mm_rec_cmd.cb_func     = cb_func;
    packet->mm_rec_cmd.client_data = client_data;

    cmx_cmd (packet);

  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
} /* cmx_mm_record_stop */

/*===========================================================================

FUNCTION cmx_mm_record_pause

DESCRIPTION
  This function sends a command to pause a MultiMedia record session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_mm_record_pause (
  cmx_mm_rec_link_type   link,        /* Direction to record           */
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
)
{
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {

    packet->mm_rec_cmd.hdr.cmd      = CMX_MM_CMD_REC_PAUSE;
    packet->mm_rec_cmd.link         = link;
    packet->mm_rec_cmd.cb_func      = cb_func;
    packet->mm_rec_cmd.client_data  = client_data;

    cmx_cmd (packet);

  } else if(cb_func != NULL) {

    cb_func(CMX_FAILURE, client_data);

  }
} /* cmx_mm_record_pause */

/*===========================================================================

FUNCTION cmx_mm_record_resume

DESCRIPTION
  This function sends a command to resume a MultiMedia record session from
  pause.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_mm_record_resume (
  cmx_mm_rec_link_type  link,        /* Direction to record           */
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
)
{
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {

    packet->mm_rec_cmd.hdr.cmd      = CMX_MM_CMD_REC_RESUME;
    packet->mm_rec_cmd.link         = link;
    packet->mm_rec_cmd.cb_func      = cb_func;
    packet->mm_rec_cmd.client_data  = client_data;

    cmx_cmd (packet);

  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* cmx_mm_record_resume */
#endif /* FEATURE_MM_REC */
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
#ifdef FEATURE_PNG_DECODER
/*===========================================================================

FUNCTION cmx_png_decode

DESCRIPTION
  This function issues a command to decode a PNG file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_png_decode
(
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
#ifdef FEATURE_WEBAUDIO
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                     = CMX_PNG_CMD_DECODE;
      packet->image_decode.handle         = phandle;
      packet->image_decode.cb_func        = callback_ptr;
      packet->image_decode.client_data    = client_data;
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }
#endif /* FEATURE_WEBAUDIO */

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }

} /* end cmx_png_decode */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_png_abort

DESCRIPTION
  This function issues a command to decode a PNG file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_png_abort (
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
) {
#ifdef FEATURE_WEBAUDIO
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd             = CMX_PNG_CMD_ABORT;
    packet->base.cb_func        = callback_ptr;
    packet->base.client_data    = client_data;
    cmx_cmd (packet);
  } else
#endif /* FEATURE_WEBAUDIO */

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }

} /* end cmx_png_abort */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_png_get_specs

DESCRIPTION
  This function issues a command to retrieve the specifications of the
  PNG file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_png_get_specs (
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
#ifdef FEATURE_WEBAUDIO
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                     = CMX_PNG_CMD_SPEC;
      packet->image_decode.handle         = phandle;
      packet->image_decode.cb_func        = callback_ptr;
      packet->image_decode.client_data    = client_data;
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }
#endif /* FEATURE_WEBAUDIO */

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }
} /* end cmx_png_get_specs */
#endif /* FEATURE_PNG_DECODER */

/* <EJECT> */
#ifdef FEATURE_JPEG_DECODER
/*===========================================================================

FUNCTION cmx_jpeg_decode

DESCRIPTION
  This function issues a command to decode a JPEG file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_jpeg_decode
(
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
#ifdef FEATURE_WEBAUDIO
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                     = CMX_JPEG_CMD_DECODE;
      packet->image_decode.handle         = phandle;
      packet->image_decode.cb_func        = callback_ptr;
      packet->image_decode.client_data    = client_data;
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }
#endif /* FEATURE_WEBAUDIO */

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }

} /* end cmx_jpeg_decode */

/*===========================================================================

FUNCTION cmx_jpeg_get_specs

DESCRIPTION
  This function issues a command to retrieve the specifications of the
  JPEG file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_jpeg_get_specs (
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
#ifdef FEATURE_WEBAUDIO
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                     = CMX_JPEG_CMD_SPEC;
      packet->image_decode.handle         = phandle;
      packet->image_decode.cb_func        = callback_ptr;
      packet->image_decode.client_data    = client_data;
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }
#endif /* FEATURE_WEBAUDIO */

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }
} /* end cmx_jpeg_get_specs */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_jpeg_decode_abort

DESCRIPTION
  This function issues a command to abort the current JPEG decode process.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_jpeg_decode_abort (
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
) {
#ifdef FEATURE_WEBAUDIO
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd             = CMX_JPEG_CMD_ABORT_DECODE;
    packet->base.cb_func        = callback_ptr;
    packet->base.client_data    = client_data;
    cmx_cmd (packet);
  } else
#endif /* FEATURE_WEBAUDIO */
  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* end cmx_jpeg_decode_abort */
#endif /* FEATURE_JPEG_DECODER */

/* <EJECT> */
#ifdef FEATURE_JPEG_ENCODER
/*===========================================================================

FUNCTION cmx_jpeg_encode

DESCRIPTION
  This function issues a command to encode a JPEG file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_jpeg_encode
(
  cmx_jpeg_spec_type           image_spec,
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }
} /* end cmx_jpeg_encode */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_jpeg_encode_abort

DESCRIPTION
  This function issues a command to abort the current JPEG encode process.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_jpeg_encode_abort (
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
) {
  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* end cmx_jpeg_encode_abort */
#endif /* FEATURE_JPEG_ENCODER */

/* <EJECT> */
#ifdef FEATURE_SAF
/*===========================================================================

FUNCTION cmx_saf_decode

DESCRIPTION
  This function issues a command to decode a SAF file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_decode
(
  cmx_handle_type              *handle,
  cmx_saf_ctl_type             saf_ctl,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet  = cmx_cmd_get_pkt ();

    if(packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                  = CMX_SAF_CMD_DECODE;
      packet->image_decode.handle      = phandle;
      packet->image_decode.saf_ctl     = saf_ctl;
      packet->image_decode.cb_func     = callback_ptr;
      packet->image_decode.client_data = client_data;
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }

} /* end cmx_saf_decode */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_abort

DESCRIPTION
  This function issues a command to abort a SAF file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_abort (
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd             = CMX_SAF_CMD_ABORT;
    packet->base.cb_func        = callback_ptr;
    packet->base.client_data    = client_data;
    cmx_cmd (packet);
  } else if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }

} /* end cmx_saf_abort */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_frame_id_cmd

DESCRIPTION
  This function issues a SAF frame ID command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_frame_id_cmd (
  uint32                 frame_id,
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                  = CMX_SAF_CMD_FRAME_ID;
    packet->saf_frame_id.frame_id    = frame_id;
    packet->saf_frame_id.cb_func     = callback_ptr;
    packet->saf_frame_id.client_data = client_data;
    cmx_cmd (packet);
  } else if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }

} /* end cmx_saf_frame_id_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_pause

DESCRIPTION
  This function issues a command to pause a SAF file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_pause (
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd             = CMX_SAF_CMD_PAUSE;
    packet->base.cb_func        = callback_ptr;
    packet->base.client_data    = client_data;
    cmx_cmd (packet);
  } else if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }

} /* end cmx_saf_pause */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_resume

DESCRIPTION
  This function issues a command to resume a SAF file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_resume (
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd             = CMX_SAF_CMD_RESUME;
    packet->base.cb_func        = callback_ptr;
    packet->base.client_data    = client_data;
    cmx_cmd (packet);
  } else if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }

} /* end cmx_saf_resume */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_get_info

DESCRIPTION
  This function issues a command to retrieve the specifications of the
  SAF file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_saf_get_info (
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet  = cmx_cmd_get_pkt ();

    if(packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                     = CMX_SAF_CMD_INFO;
      packet->image_decode.handle         = phandle;
      packet->image_decode.cb_func        = callback_ptr;
      packet->image_decode.client_data    = client_data;
      cmx_cmd(packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }
} /* end cmx_saf_get_info */
#endif /* FEATURE_SAF */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT
/*===========================================================================

FUNCTION cmx_midi_out_open

DESCRIPTION
  This function issues a command to enable midi commands.
  Note: Audio decoder type will be defaulted to QCP for this function.
        Use cmx_midi_out_open_adec_format() to explicitly set ADEC type.
        This function is being kept around for backwards compatibility.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_open
(
  cmx_cb_func_ptr_type    cb_func,
  void                    *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                   = CMX_MIDI_OUT_OPEN;
    packet->midi_out_open.cb_func     = cb_func;
    packet->midi_out_open.client_data = client_data;
    packet->midi_out_open.adec_mode   = CMX_ADEC_MODE_QCP;
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_midi_out_open */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_open_adec_format

DESCRIPTION
  This function issues a command to enable midi commands and allows
  client to specify the audio decoder type to use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_open_adec_format
(
  cmx_adec_mode_enum_type adec_mode,
  cmx_cb_func_ptr_type    cb_func,
  void                    *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                    = CMX_MIDI_OUT_OPEN;
    packet->midi_out_open.cb_func      = cb_func;
    packet->midi_out_open.client_data  = client_data;
    packet->midi_out_open.adec_mode    = adec_mode;
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_midi_out_open_adec_format */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_close

DESCRIPTION
  This function issues a command to disable midi commands.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_close
(
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                    = CMX_MIDI_OUT_CLOSE;
    packet->midi_out_open.cb_func      = cb_func;
    packet->midi_out_open.client_data  = client_data;
    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_midi_out_close */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_msg

DESCRIPTION
  This function issues midi commands to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_msg
(
  cmx_handle_type        *handle,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd               = CMX_MIDI_OUT_MSG;
      packet->midi_out.handle       = phandle;
      packet->midi_out.cb_func      = cb_func;
      packet->midi_out.client_data  = client_data;
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cmx_midi_out_status.cb_func != NULL) {
    cmx_midi_out_status.cb_func(CMX_FAILURE, cmx_midi_out_status.client_data);
  }

} /* end cmx_midi_out_msg */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
/*===========================================================================

FUNCTION cmx_midi_out_qcp

DESCRIPTION
  This function plays a QCP file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp
(
  cmx_handle_type                 *handle,
  uint16                          volume,
  sint15                          pan,
  cmx_midi_out_cb_func_ptr_type   cb_func,
  void                            *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                   = CMX_MIDI_OUT_QCP;
      packet->midi_out_qcp.handle       = phandle;
      packet->midi_out_qcp.adec_param.volume      = volume;
      packet->midi_out_qcp.adec_param.pan         = pan;
      packet->midi_out_qcp.adec_param.start_mode  = CMX_ADEC_START_MODE_NORMAL;
      packet->midi_out_qcp.adec_param.repeat_mode =
                                                 CMX_ADEC_REPEAT_MODE_DISABLED;
      packet->midi_out_qcp.qcp_cb_func  = cb_func;
      packet->midi_out_qcp.client_data  = client_data;
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cmx_midi_out_status.cb_func != NULL) {
    cmx_midi_out_status.cb_func(CMX_FAILURE, cmx_midi_out_status.client_data);
  }

} /* end cmx_midi_out_qcp */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_adec

DESCRIPTION
  This function plays an audio file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_adec
(
  cmx_handle_type                 *handle,
  cmx_midi_out_adec_param_type    *param,
  cmx_midi_out_cb_func_ptr_type   cb_func,
  void                            *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                   = CMX_MIDI_OUT_QCP;
      packet->midi_out_qcp.handle       = phandle;
      packet->midi_out_qcp.adec_param   = *param;
      packet->midi_out_qcp.qcp_cb_func  = cb_func;
      packet->midi_out_qcp.client_data  = client_data;
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }

  if(cmx_midi_out_status.cb_func != NULL) {
    cmx_midi_out_status.cb_func(CMX_FAILURE, cmx_midi_out_status.client_data);
  }

} /* end cmx_midi_out_adec */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_adec_start_chan

DESCRIPTION
  This function starts playback of the indicated audio file that is in
  delayed start mode to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_adec_start_chan (
  void                   *id,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_MIDI_OUT_ADEC_START_CHAN;
    packet->midi_out_qcp.cb_func         = cb_func;
    packet->midi_out_qcp.client_data     = client_data;
    packet->midi_out_qcp.midi_out_handle = (cmx_midi_out_qcp_handle_type *) id;
    cmx_cmd (packet);
  } else if (cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_stop

DESCRIPTION
  This function stops a QCP file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_stop
(
  void                   *id,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_MIDI_OUT_QCP_STOP;
    packet->midi_out_qcp.cb_func         = cb_func;
    packet->midi_out_qcp.client_data     = client_data;
    packet->midi_out_qcp.midi_out_handle = (cmx_midi_out_qcp_handle_type *) id;
    cmx_cmd (packet);
  } else if (cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
} /* end cmx_midi_out_qcp_stop */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_pause

DESCRIPTION
  This function pauses QCP files to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_pause (
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_MIDI_OUT_QCP_PAUSE;
    packet->midi_out_qcp.cb_func         = cb_func;
    packet->midi_out_qcp.client_data     = client_data;
    cmx_cmd (packet);
  } else if (cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_resume

DESCRIPTION
  This function resumes QCP files to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_resume (
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_MIDI_OUT_QCP_RESUME;
    packet->midi_out_qcp.cb_func         = cb_func;
    packet->midi_out_qcp.client_data     = client_data;
    cmx_cmd (packet);
  } else if (cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_pause_chan

DESCRIPTION
  This function pauses the indicated QCP file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_pause_chan (
  void                   *id,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_MIDI_OUT_QCP_PAUSE_CHAN;
    packet->midi_out_qcp.cb_func         = cb_func;
    packet->midi_out_qcp.client_data     = client_data;
    packet->midi_out_qcp.midi_out_handle = (cmx_midi_out_qcp_handle_type *) id;
    cmx_cmd (packet);
  } else if (cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_resume_chan

DESCRIPTION
  This function resumes indicated QCP file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_resume_chan (
  void                   *id,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_MIDI_OUT_QCP_RESUME_CHAN;
    packet->midi_out_qcp.cb_func         = cb_func;
    packet->midi_out_qcp.client_data     = client_data;
    packet->midi_out_qcp.midi_out_handle = (cmx_midi_out_qcp_handle_type *) id;
    cmx_cmd (packet);
  } else if (cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_pan

DESCRIPTION
  This function changes the panning for a QCP file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_pan (
  void                   *id,
  sint15                 pan,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_MIDI_OUT_QCP_PAN;
    packet->midi_out_qcp.cb_func         = cb_func;
    packet->midi_out_qcp.client_data     = client_data;
    packet->midi_out_qcp.adec_param.pan  = pan;
    packet->midi_out_qcp.midi_out_handle = (cmx_midi_out_qcp_handle_type *) id;
    cmx_cmd (packet);
  } else if (cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_volume

DESCRIPTION
  This function changes the volume for a QCP file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_midi_out_qcp_volume (
  void                   *id,
  uint16                 volume,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->hdr.cmd                      = CMX_MIDI_OUT_QCP_VOLUME;
    packet->midi_out_qcp.cb_func         = cb_func;
    packet->midi_out_qcp.client_data     = client_data;
    packet->midi_out_qcp.adec_param.volume = volume;
    packet->midi_out_qcp.midi_out_handle = (cmx_midi_out_qcp_handle_type *) id;
    cmx_cmd (packet);
  } else if (cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

/* <EJECT> */
#ifdef FEATURE_QAUDIOFX
/*===========================================================================

FUNCTION cmx_3d_pos_free_cb_struct

DESCRIPTION
  This function frees the 3D positional callback structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_free_cb_struct (
  cmx_3d_pos_cb_struct_type *cb_struct
) {
  uint32 counter;

  if(cb_struct != NULL) {
    for(counter = 0; counter < CMX_3D_POS_CB_NUM_ENTRIES; counter++) {
      if(cb_struct == &cmx_3d_pos_cb_array[counter]) {
        cb_struct->cb_func.base_cb = NULL;
        break;
      }
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_get_cb_struct

DESCRIPTION
  This function gets a 3D positional callback structure.

DEPENDENCIES
  cmx_3d_pos_cb_array

RETURN VALUE
  Pointer to cmx_3d_pos_cb_struct_type.

SIDE EFFECTS
  None.

===========================================================================*/
cmx_3d_pos_cb_struct_type *cmx_3d_pos_get_cb_struct ()
{
  cmx_3d_pos_cb_struct_type *ret_val = NULL;
  uint32 counter;

  for(counter = 0; counter < CMX_3D_POS_CB_NUM_ENTRIES; counter++) {
    if(cmx_3d_pos_cb_array[counter].cb_func.base_cb == NULL) {
      ret_val = &cmx_3d_pos_cb_array[counter];
      break;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_setup_cb

DESCRIPTION
  This function gets a 3D positional callback structure and sets the
  callback function and client data fields.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to cmx_3d_pos_cb_struct_type.

SIDE EFFECTS
  None.

===========================================================================*/
cmx_3d_pos_cb_struct_type *cmx_3d_pos_setup_cb (
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  cmx_3d_pos_cb_struct_type *cb_struct = NULL;

  if(cb_func != NULL) {
    cb_struct = cmx_3d_pos_get_cb_struct();
    if(cb_struct != NULL) {
      cb_struct->cb_func.base_cb = cb_func;
      cb_struct->client_data     = client_data;
    }
  }

  return(cb_struct);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_setup_query_cb

DESCRIPTION
  This function gets a 3D positional callback structure and sets the
  query callback function and client data fields.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to cmx_3d_pos_cb_struct_type.

SIDE EFFECTS
  None.

===========================================================================*/
cmx_3d_pos_cb_struct_type *cmx_3d_pos_setup_query_cb (
  cmx_3d_pos_get_cb_ptr_type cb_func,
  void                       *client_data
) {
  cmx_3d_pos_cb_struct_type *cb_struct = NULL;

  if(cb_func != NULL) {
    cb_struct = cmx_3d_pos_get_cb_struct();
    if(cb_struct != NULL) {
      cb_struct->cb_func.get_cb = cb_func;
      cb_struct->client_data    = client_data;
    }
  }

  return(cb_struct);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_cb_processing

DESCRIPTION
  This function processes a 3D positional command callback command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_cb_processing (
  cmx_status_type           status,
  cmx_3d_pos_cb_struct_type *cb_struct
) {
  if(cb_struct != NULL) {
    if(cb_struct->cb_func.base_cb != NULL) {
      cb_struct->cb_func.base_cb(status, cb_struct->client_data);
    }
    cmx_3d_pos_free_cb_struct(cb_struct);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_query_cb_processing

DESCRIPTION
  This function processes a 3D positional query callback command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_query_cb_processing (
  cmx_status_type               status,
  cmx_3d_pos_param_enum_type    param,
  cmx_3d_pos_param_setting_type *setting,
  cmx_3d_pos_cb_struct_type     *cb_struct
) {
  if(cb_struct != NULL) {
    if(cb_struct->cb_func.get_cb != NULL) {
      cb_struct->cb_func.get_cb(status, param, setting, cb_struct->client_data);
      MSG_MED( "cmx_3d_pos_query_cb_processing() called ",0,0,0 );
    }
    cmx_3d_pos_free_cb_struct(cb_struct);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_cmd_cb_func

DESCRIPTION
  This function is used as the callback function for 3D positional commands.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_cmd_cb_func (
  const void      *client_data,
  snd_status_type status
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if(packet != NULL) {
    packet->hdr.cmd             = CMX_CMD_3D_POS_DO_CB;
    packet->cb_3d_pos.cb_struct = (cmx_3d_pos_cb_struct_type *) client_data;

    if(status == SND_SUCCESS) {
      packet->cb_3d_pos.status = CMX_SUCCESS;
    } else {
      packet->cb_3d_pos.status = CMX_FAILURE;
    }

    cmx_cmd (packet);
  } else if(client_data != NULL) {
    cmx_3d_pos_free_cb_struct((cmx_3d_pos_cb_struct_type *) client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_query_cb_func

DESCRIPTION
  This function is used as the callback function for 3D positional
  query commands.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_query_cb_func (
  snd_status_type               status,
  snd_3d_pos_param_enum_type    param,
  snd_3d_pos_param_setting_type *setting,
  const void                    *client_data
) {
  cmx_packet_type *packet;
  cmx_3d_pos_param_setting_type *cmx_setting;

  packet = cmx_cmd_get_pkt ();

  if(packet != NULL) {
    packet->hdr.cmd                   = CMX_CMD_3D_POS_DO_QUERY_CB;
    packet->query_cb_3d_pos.cb_struct =
                               (cmx_3d_pos_cb_struct_type *) client_data;

    switch(param) {
      case SND_3D_POS_PARAM_ENV_ENABLED:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_ENV_ENABLED;
        if(setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->env_enable = setting->env_enabled_flag;
        }
        break;

      case SND_3D_POS_PARAM_LSNR_POS:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_LSNR_POS;
        if(setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->position.x_pos = setting->position.x_pos;
          cmx_setting->position.y_pos = setting->position.y_pos;
          cmx_setting->position.z_pos = setting->position.z_pos;
        }
        break;

      case SND_3D_POS_PARAM_LSNR_ORIENT:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_LSNR_ORIENT;
        if(setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->lsnr_orient.x_forward = setting->lsnr_orient.x_forward;
          cmx_setting->lsnr_orient.y_forward = setting->lsnr_orient.y_forward;
          cmx_setting->lsnr_orient.z_forward = setting->lsnr_orient.z_forward;
          cmx_setting->lsnr_orient.x_up      = setting->lsnr_orient.x_up;
          cmx_setting->lsnr_orient.y_up      = setting->lsnr_orient.y_up;
          cmx_setting->lsnr_orient.z_up      = setting->lsnr_orient.z_up;
        }
        break;

      case SND_3D_POS_PARAM_LSNR_VEL:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_LSNR_VEL;
        if(setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->lsnr_vel.x_vel = setting->lsnr_vel.x_vel;
          cmx_setting->lsnr_vel.y_vel = setting->lsnr_vel.y_vel;
          cmx_setting->lsnr_vel.z_vel = setting->lsnr_vel.z_vel;
        }
        break;

      case SND_3D_POS_PARAM_REVERB_ENABLED:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_REVERB_ENABLED;
        if (setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->reverb_enable = setting->reverb_flag;
        }
        break;

      case SND_3D_POS_PARAM_REVERB:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_REVERB;
        if(setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->reverb_param.env_type   = setting->reverb.env_type;
          cmx_setting->reverb_param.decay_ms   = setting->reverb.decay_ms;
          cmx_setting->reverb_param.room_gain  = setting->reverb.room_gain;
          cmx_setting->reverb_param.decay_ratio= setting->reverb.decay_ratio;
          cmx_setting->reverb_param.ref_delay  = setting->reverb.ref_delay;
          cmx_setting->reverb_param.reverb_delay  = setting->reverb.reverb_delay;
          cmx_setting->reverb_param.reverb_gain = setting->reverb.reverb_gain;
        }
        break;

      case SND_3D_POS_PARAM_SRC_ENABLED:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_SRC_ENABLED;
        if (setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->src_enable = setting->src_enable_flag;
        }
        break;

      case SND_3D_POS_PARAM_SRC_POS:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_SRC_POS;
        if(setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->position.x_pos = setting->position.x_pos;
          cmx_setting->position.y_pos = setting->position.y_pos;
          cmx_setting->position.z_pos = setting->position.z_pos;
        }
        break;

      case SND_3D_POS_PARAM_SRC_VOLUME:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_SRC_VOLUME;
        if(setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->src_volume.volume = setting->src_volume.volume;
        }
        break;

      case SND_3D_POS_PARAM_SRC_VOL_ATTEN:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_SRC_VOL_ATTEN;
        if(setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->src_vol_atten.min_distance =
                                         setting->src_vol_atten.min_distance;
          cmx_setting->src_vol_atten.max_distance =
                                         setting->src_vol_atten.max_distance;

          cmx_setting->src_vol_atten.scale = setting->src_vol_atten.scale;
          cmx_setting->src_vol_atten.mute_after_max = setting->src_vol_atten.mute;
        }
        break;
      case SND_3D_POS_PARAM_SRC_VEL:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_SRC_VEL;
        if(setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->src_vel.x_vel = setting->src_vel.x_vel;
          cmx_setting->src_vel.y_vel = setting->src_vel.y_vel;
          cmx_setting->src_vel.z_vel = setting->src_vel.z_vel;
        }
        break;

      case SND_3D_POS_PARAM_SRC_REV_VOL:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_SRC_REVERB_VOL;
        if(setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->src_reverb_vol.volume = setting->rev_vol.volume;
        }
        break;

      case SND_3D_POS_PARAM_SRC_FINAL_SETTINGS:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_SRC_FINAL_SETTINGS;
        if(setting != NULL) {
          cmx_setting = &packet->query_cb_3d_pos.setting;
          cmx_setting->src_all.src_enable     = setting->src_all.src_enable;
          cmx_setting->src_all.position.x_pos = setting->src_all.position.x_pos;
          cmx_setting->src_all.position.y_pos = setting->src_all.position.y_pos;
          cmx_setting->src_all.position.z_pos = setting->src_all.position.z_pos;
          cmx_setting->src_all.volume.volume  = setting->src_all.volume.volume;

          cmx_setting->src_all.vol_atten.min_distance =
                                       setting->src_all.vol_atten.min_distance;
          cmx_setting->src_all.vol_atten.max_distance =
                                       setting->src_all.vol_atten.max_distance;
          cmx_setting->src_all.vol_atten.scale        =
                                       setting->src_all.vol_atten.scale;
          cmx_setting->src_all.vol_atten.mute_after_max        =
                                       setting->src_all.vol_atten.mute;
          cmx_setting->src_all.src_vel_cmd.x_vel  =
                                       setting->src_all.vel_vector.x_vel;
          cmx_setting->src_all.src_vel_cmd.y_vel  =
                                       setting->src_all.vel_vector.y_vel;
          cmx_setting->src_all.src_vel_cmd.z_vel  =
                                       setting->src_all.vel_vector.z_vel;
          cmx_setting->src_reverb_vol.volume = setting->rev_vol.volume;
        }
        break;

      default:
        packet->query_cb_3d_pos.param = CMX_3D_POS_PARAM_INVALID;
        status                        = SND_FAILURE;
        break;
    }

    if(status == SND_SUCCESS) {
      packet->query_cb_3d_pos.status = CMX_SUCCESS;
    } else if(status == SND_ABORT) {
      packet->query_cb_3d_pos.status = CMX_ABORTED;
    } else {
      packet->query_cb_3d_pos.status = CMX_FAILURE;
    }
    cmx_cmd (packet);
  } else if(client_data != NULL) {
    cmx_3d_pos_free_cb_struct((cmx_3d_pos_cb_struct_type *) client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_enable_cmd

DESCRIPTION
  This function processes a 3D positional enable/disable command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_enable_cmd (
  cmx_3d_pos_mode_enum_type mode,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
) {
  cmx_3d_pos_cb_struct_type *cb_struct;

  cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

  if(cb_struct != NULL) {
    if(mode == CMX_3D_POS_MODE_ENABLE) {
      snd_3d_pos_enable(SND_3D_POS_MODE_ENABLE, cmx_3d_pos_cmd_cb_func,
                        cb_struct);
    } else {
      snd_3d_pos_enable(SND_3D_POS_MODE_DISABLE, cmx_3d_pos_cmd_cb_func,
                        cb_struct);
    }
  } else if(cb_func == NULL) {
    if(mode == CMX_3D_POS_MODE_ENABLE) {
      snd_3d_pos_enable(SND_3D_POS_MODE_ENABLE, NULL, NULL);
    } else {
      snd_3d_pos_enable(SND_3D_POS_MODE_DISABLE, NULL, NULL);
    }
  } else {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_enable_processing

DESCRIPTION
  This function processes a 3D positional enable/disable command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_enable_processing (
  cmx_3d_pos_mode_enum_type mode,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
) {
  snd_3d_pos_mode_enum_type    snd_mode = SND_3D_POS_MODE_DISABLE;
  cmx_3d_pos_cb_struct_type    *cb_struct;

  cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);
  MSG_MED("cmx_3d_pos_enable_processing(mode = %d )called ", mode,0,0 );
  if (mode == CMX_3D_POS_MODE_ENABLE) {
    snd_mode = SND_3D_POS_MODE_ENABLE;
  }

  if(cb_struct != NULL) {
    snd_3d_pos_enable(snd_mode, cmx_3d_pos_cmd_cb_func, cb_struct);
  } else if(cb_func == NULL) {
    snd_3d_pos_enable(snd_mode, NULL, NULL);
  } else {
    if (cb_func != NULL) {
      MSG_ERROR("Failed to get 3d callback structure",0,0,0 );
      cb_func(CMX_FAILURE, client_data);
    }
  }
}

/*===========================================================================

FUNCTION cmx_3d_pos_reset_processing

DESCRIPTION
  This function processes a 3D environment reset command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_reset_processing (
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
) {
  cmx_3d_pos_cb_struct_type    *cb_struct;

  cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

  if(cb_struct != NULL) {
    snd_3d_pos_reset(cmx_3d_pos_cmd_cb_func, cb_struct);
    MSG_MED( "cmx_3d_pos_reset_processing()called",0,0,0 );
  } else if(cb_func == NULL) {
    snd_3d_pos_reset(NULL, NULL);
  } else {
    if (cb_func != NULL) {
      MSG_ERROR(" Failed to get 3d callback structure",0,0,0 );
      cb_func(CMX_FAILURE, client_data);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_lsnr_pos_processing

DESCRIPTION
  This function processes a 3D positional listener position command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_lsnr_pos_processing (
  cmx_3d_pos_lsnr_pos_cmd_type *lsnr_pos,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
) {
  snd_3d_pos_position_cmd_type pos;
  cmx_3d_pos_cb_struct_type    *cb_struct;

  if(lsnr_pos != NULL) {
    cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

    pos.x_pos        = lsnr_pos->x_pos;
    pos.y_pos        = lsnr_pos->y_pos;
    pos.z_pos        = lsnr_pos->z_pos;
    pos.duration_ms  = lsnr_pos->duration_ms;
    MSG_MED( "cmx_3d_pos_lsnr_pos_processing(X = %d,Y = %d ,Z = %d)called ",
            pos.x_pos ,pos.y_pos ,pos.z_pos );
    MSG_MED("cmx_3d_pos_lsnr_pos_processing(duration = %d)",
            pos.duration_ms,0,0 );
    if(cb_struct != NULL) {
      snd_3d_pos_lsnr_pos(&pos, cmx_3d_pos_cmd_cb_func, cb_struct);
    } else if(cb_func == NULL) {
      snd_3d_pos_lsnr_pos(&pos, NULL, NULL);
    } else {
      cb_func(CMX_FAILURE, client_data);
    }
  } else if(cb_func != NULL) {
    MSG_ERROR( "Unable to set the Listner position ",0,0,0 );
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_lsnr_orient_processing

DESCRIPTION
  This function processes a 3D positional listener orientation command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_lsnr_orient_processing (
  cmx_3d_pos_lsnr_orient_cmd_type *lsnr_orient,
  cmx_cb_func_ptr_type            cb_func,
  void                            *client_data
) {
  snd_3d_pos_lsnr_orient_cmd_type orient;
  cmx_3d_pos_cb_struct_type       *cb_struct;

  if(lsnr_orient != NULL) {
    cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

    orient.x_forward    = lsnr_orient->x_forward;
    orient.y_forward    = lsnr_orient->y_forward;
    orient.z_forward    = lsnr_orient->z_forward;
    orient.x_up         = lsnr_orient->x_up;
    orient.y_up         = lsnr_orient->y_up;
    orient.z_up         = lsnr_orient->z_up;
    orient.duration_ms  = lsnr_orient->duration_ms;
    MSG_MED("cmx_3d_pos_lsnr_orient_processing(X = %d,Y = %d,Z = %d)",
             orient.x_forward ,orient.y_forward ,orient.z_forward);
    MSG_MED("cmx_3d_pos_lsnr_orient_processing(X_up= %d,Y_up= %d,Z_up= %d)",
             orient.x_up ,orient.y_up,orient.z_up);
    MSG_MED("cmx_3d_pos_lsnr_orient_processing(duration = %d)",
            orient.duration_ms,0,0);
    if(cb_struct != NULL) {
      snd_3d_pos_lsnr_orient(&orient, cmx_3d_pos_cmd_cb_func, cb_struct);
    } else if(cb_func == NULL) {
      snd_3d_pos_lsnr_orient(&orient, NULL, NULL);
    } else {
      cb_func(CMX_FAILURE, client_data);
    }
  } else if(cb_func != NULL) {
    MSG_ERROR(" Failed to get 3d callback structure",0,0,0 );
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION cmx_3d_pos_lsnr_vel_processing

DESCRIPTION
  This function processes a 3D positional listener velocity command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_lsnr_vel_processing (
  cmx_3d_pos_lsnr_vel_cmd_type *lsnr_vel,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
){
  snd_3d_pos_velocity_cmd_type vel;
  cmx_3d_pos_cb_struct_type    *cb_struct;

  if(lsnr_vel != NULL) {
    cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);
    vel.x_vel        = lsnr_vel->x_vel;
    vel.y_vel        = lsnr_vel->y_vel;
    vel.z_vel        = lsnr_vel->z_vel;
    vel.duration_ms  = lsnr_vel->duration_ms;
    MSG_MED("cmx_3d_pos_lsnr_vel_processing(X_vel= %d,Y_vel = %d ,Z_vel = %d)",
            vel.x_vel,vel.y_vel,vel.z_vel );
    MSG_MED("cmx_3d_pos_lsnr_vel_processing(duration = %d)",
             vel.duration_ms,0,0 );
    if(cb_struct != NULL) {
      snd_3d_pos_lsnr_vel(&vel, cmx_3d_pos_cmd_cb_func, cb_struct);
    } else if(cb_func == NULL) {
      snd_3d_pos_lsnr_vel(&vel, NULL, NULL);
    } else {
      cb_func(CMX_FAILURE, client_data);
    }
  } else if(cb_func != NULL) {
    MSG_ERROR(" Failed to get 3d callback structure ",0,0,0 );
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_reverb_processing

DESCRIPTION
  This function processes a 3D Enable/Disable Reverb command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_reverb_enable_processing (
  boolean                      enable,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
){
  cmx_3d_pos_cb_struct_type    *cb_struct = NULL;


    cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

    if(cb_struct != NULL) {
     MSG_MED("cmx_3d_reverb_enable_processing = %d ",enable,0,0 );
    snd_3d_reverb_enable(enable, cmx_3d_pos_cmd_cb_func, cb_struct);
    } else if(cb_func == NULL) {
    snd_3d_reverb_enable(enable, NULL, NULL);
    } else {
      MSG_ERROR(" Failed to get 3d callback structure",0,0,0 );
      cb_func(CMX_FAILURE, client_data);
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_reverb_cmd_processing

DESCRIPTION
  This function processes a 3D Set Reverb command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_reverb_cmd_processing (
  cmx_cmd_type              cmd,
  cmx_3d_pos_param_type    *param,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
){
  snd_3d_reverb_enum_type   reverb_type;
  snd_3d_reverb_cmd_type    reverb;
  cmx_3d_pos_cb_struct_type    *cb_struct = NULL;

  if(param != NULL) {
    MSG_MED("cmx_3d_reverb_cmd_processing() called ",0,0,0 );
    switch(cmd){
    case CMX_CMD_3D_REVERB_SET_PRESET:
      if (param->reverb_cmd.env_type < CMX_3D_REVERB_INVALID ) {
        reverb_type = SND_3D_POS_REVERB_SET_PRESET;
        reverb.env_type        = param->reverb_cmd.env_type;
        MSG_MED("CMX_CMD_3D_REVERB_SET_PRESET(env_type = %d)called",
                  reverb.env_type,0,0  );
        if( reverb.env_type == CMX_3D_REVERB_USER ){
          reverb.decay_ms        = param->reverb_cmd.reverb_param.decay_ms;
          reverb.decay_ratio     = param->reverb_cmd.reverb_param.decay_ratio;
          reverb.ref_delay       = param->reverb_cmd.reverb_param.ref_delay;
          reverb.reverb_delay    = param->reverb_cmd.reverb_param.reverb_delay;
          reverb.reverb_gain     = param->reverb_cmd.reverb_param.reverb_gain;
          reverb.room_gain       = param->reverb_cmd.reverb_param.room_gain;
          MSG_MED("REVERB_USER(decay time = %d,decay_ratio = %d ,delay = %d)",
	           reverb.decay_ms,reverb.decay_ratio,reverb.ref_delay);
          MSG_MED("REVERB_USER(reverbdelay= %d,reverbgain= %d,roomgain= %d)",
	            reverb.ref_delay,reverb.reverb_gain,reverb.room_gain);
        }
      } else {
        MSG_ERROR("Invalid reverb preset environment", 0, 0, 0);
        cb_func(CMX_FAILURE, client_data);
        return;
      }
      break;

    case CMX_CMD_3D_REVERB_SET_LEVEL:
        reverb_type           = SND_3D_POS_REVERB_SET_LEVEL;
        reverb.reverb_gain    = param->reverb_level_cmd.reverb_gain;
        MSG_MED("CMX_CMD_3D_REVERB_SET_LEVEL = %d",reverb.reverb_gain,0,0);
        break;

    case CMX_CMD_3D_REVERB_SET_TIME:
        reverb_type            = SND_3D_POS_REVERB_SET_TIME;
        reverb.decay_ms        = param->reverb_decay_cmd.decay_ms;
        MSG_MED("CMX_CMD_3D_REVERB_SET_TIME = %d",reverb.decay_ms,0,0);
        break;

    case CMX_CMD_3D_REVERB_SET_RATIO:
        reverb_type            = SND_3D_POS_REVERB_SET_RATIO;
        reverb.decay_ratio     = param->reverb_ratio_cmd.decay_ratio;
        MSG_MED("CMX_CMD_3D_REVERB_SET_RATIO = %d",reverb.decay_ratio,0,0);
        break;

    default:
     if(cb_func != NULL) {
       MSG_ERROR(" Invalid set reverb environment commands ",0,0,0);
       cb_func(CMX_FAILURE, client_data);
     }
     return;
    }

    cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

    if(cb_struct != NULL) {
      snd_3d_reverb_set(reverb_type, &reverb, cmx_3d_pos_cmd_cb_func, cb_struct);
    } else if(cb_func == NULL) {
      snd_3d_reverb_set(reverb_type, &reverb, NULL, NULL);
    } else {
      MSG_ERROR(" Failed to get 3d callback structure ",0,0,0);
      cb_func(CMX_FAILURE, client_data);
    }
  } else if(cb_func != NULL) {
    MSG_ERROR(" Invalid reverb environment parameters ",0,0,0);
    cb_func(CMX_FAILURE, client_data);
  }
}

/*===========================================================================

FUNCTION cmx_3d_pos_src_enable_processing

DESCRIPTION
  This function processes a 3D positional enable/disable command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_src_enable_processing (
  cmx_3d_pos_src_enable_cmd_type *src_mode,
  cmx_cb_func_ptr_type           cb_func,
  void                           *client_data
) {
  snd_3d_pos_mode_enum_type      snd_mode = SND_3D_POS_MODE_DISABLE;
  cmx_midi_out_qcp_handle_type   *midi_out_handle;
  cmx_midi_out_qcp_handle_type   *mo_ptr = NULL;
  cmx_3d_pos_cb_struct_type      *cb_struct;

  if(src_mode != NULL) {
    MSG_MED("cmx_3d_pos_src_enable_processing(ID= %d)called ",
            src_mode->id,0,0 );
    midi_out_handle = (cmx_midi_out_qcp_handle_type*) src_mode->id;
    if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
       (midi_out_handle != NULL)) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                       q_check (&cmx_midi_out_in_use_q);
      while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
        mo_ptr = (cmx_midi_out_qcp_handle_type *)
                          q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
      }
    }
    if(mo_ptr != NULL) {
      cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

      if (src_mode->enable_mode == CMX_3D_POS_MODE_ENABLE) {
        snd_mode = SND_3D_POS_MODE_ENABLE;
      }

      if(cb_struct != NULL) {
        snd_3d_pos_src_enable(mo_ptr->channel, snd_mode, cmx_3d_pos_cmd_cb_func,
                           cb_struct);
      } else if(cb_func == NULL) {
        snd_3d_pos_src_enable(mo_ptr->channel, snd_mode, NULL, NULL);
      } else {
        cb_func(CMX_FAILURE, client_data);
      }
    } else if(cb_func != NULL) {
      MSG_ERROR( "Failed to get 3d callback structure ",0,0,0 );
      cb_func(CMX_FAILURE, client_data);
    }
  } else if(cb_func != NULL) {
    MSG_ERROR(" Invalid 3d source enable parameter ",0,0,0 );
    cb_func(CMX_FAILURE, client_data);
  }
}

/*===========================================================================

FUNCTION cmx_3d_pos_src_reset_processing

DESCRIPTION
  This function processes a sourcd 3D reset command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_src_reset_processing (
  cmx_3d_pos_src_reset_cmd_type *src_reset,
  cmx_cb_func_ptr_type           cb_func,
  void                           *client_data
) {
  cmx_midi_out_qcp_handle_type   *midi_out_handle;
  cmx_midi_out_qcp_handle_type   *mo_ptr = NULL;
  cmx_3d_pos_cb_struct_type      *cb_struct;

  if(src_reset != NULL) {
    MSG_MED( "cmx_3d_pos_src_reset_processing(ID= %d)called ",
             src_reset->id,0,0 );
    midi_out_handle = (cmx_midi_out_qcp_handle_type*) src_reset->id;
    if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
       (midi_out_handle != NULL)) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                       q_check (&cmx_midi_out_in_use_q);
      while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
        mo_ptr = (cmx_midi_out_qcp_handle_type *)
                          q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
      }
    }
    if(mo_ptr != NULL) {
      cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

      if(cb_struct != NULL) {
        snd_3d_pos_src_reset(mo_ptr->channel,cmx_3d_pos_cmd_cb_func,
                           cb_struct);
      } else if(cb_func == NULL) {
        snd_3d_pos_src_reset(mo_ptr->channel, NULL, NULL);
      } else {
        cb_func(CMX_FAILURE, client_data);
      }
    } else if(cb_func != NULL) {
      MSG_ERROR(" Failed to get 3d callback structure",0,0,0 );
      cb_func(CMX_FAILURE, client_data);
    }
  } else if(cb_func != NULL) {
    MSG_ERROR("Invalid source 3d reset parameter ",0,0,0 );
    cb_func(CMX_FAILURE, client_data);
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_src_vol_processing

DESCRIPTION
  This function processes a 3D positional source volume command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_src_vol_processing (
  cmx_3d_pos_src_volume_cmd_type *src_vol,
  cmx_cb_func_ptr_type           cb_func,
  void                           *client_data
) {
  snd_3d_pos_src_volume_cmd_type vol;
  cmx_midi_out_qcp_handle_type   *midi_out_handle;
  cmx_midi_out_qcp_handle_type   *mo_ptr = NULL;
  cmx_3d_pos_cb_struct_type      *cb_struct;

  if(src_vol != NULL) {
    MSG_MED("cmx_3d_pos_src_vol_processing(ID = %d)",src_vol->id,0,0 );
    midi_out_handle = (cmx_midi_out_qcp_handle_type*) src_vol->id;
    if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
       (midi_out_handle != NULL)) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                       q_check (&cmx_midi_out_in_use_q);
      while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
        mo_ptr = (cmx_midi_out_qcp_handle_type *)
                          q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
      }
    }
    if(mo_ptr != NULL) {
      cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

      vol.volume       = src_vol->volume;
      vol.duration_ms  = src_vol->duration_ms;
      MSG_MED("cmx_3d_pos_src_vol_processing(volume = %d,duration = %d)",
               vol.volume,vol.duration_ms,0);
      if(cb_struct != NULL) {
        snd_3d_pos_src_vol(mo_ptr->channel, &vol, cmx_3d_pos_cmd_cb_func,
                           cb_struct);
      } else if(cb_func == NULL) {
        snd_3d_pos_src_vol(mo_ptr->channel, &vol, NULL, NULL);
      } else {
        cb_func(CMX_FAILURE, client_data);
      }
    } else if(cb_func != NULL) {
      MSG_ERROR(" Failed to get 3d callback structure ",0,0,0);
      cb_func(CMX_FAILURE, client_data);
    }
  } else if(cb_func != NULL) {
    MSG_ERROR(" Invalid source 3d volume parameter ",0,0,0 );
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_src_vol_atten_processing

DESCRIPTION
  This function processes a 3D positional source volume attenuation command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_src_vol_atten_processing (
  cmx_3d_pos_src_vol_atten_cmd_type *src_vol_atten,
  cmx_cb_func_ptr_type              cb_func,
  void                              *client_data
) {
  snd_3d_pos_src_vol_atten_cmd_type vol_atten;
  cmx_midi_out_qcp_handle_type      *midi_out_handle;
  cmx_midi_out_qcp_handle_type      *mo_ptr = NULL;
  cmx_3d_pos_cb_struct_type         *cb_struct;

  if(src_vol_atten != NULL) {
    MSG_MED(" cmx_3d_pos_src_vol_atten_processing(ID = %d ) ",
             src_vol_atten->id ,0,0 );
    midi_out_handle = (cmx_midi_out_qcp_handle_type*) src_vol_atten->id;
    if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
       (midi_out_handle != NULL)) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                       q_check (&cmx_midi_out_in_use_q);
      while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
        mo_ptr = (cmx_midi_out_qcp_handle_type *)
                          q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
      }
    }
    if(mo_ptr != NULL) {
      cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

      vol_atten.min_distance = src_vol_atten->min_distance;
      vol_atten.max_distance = src_vol_atten->max_distance;
      vol_atten.scale        = src_vol_atten->scale;
      vol_atten.mute         = src_vol_atten->mute_after_max;
      MSG_MED("cmx_3d_pos_src_vol_atten_processing(Max = %d,Min = %d )",
	       vol_atten.min_distance,vol_atten.max_distance,0);
      MSG_MED("cmx_3d_pos_src_vol_atten_processing(scale = %d,mute = %d)",
               vol_atten.scale,vol_atten.mute,0 );
      if(cb_struct != NULL) {
        snd_3d_pos_src_vol_atten(mo_ptr->channel, &vol_atten,
                                 cmx_3d_pos_cmd_cb_func, cb_struct);
      } else if(cb_func == NULL) {
        snd_3d_pos_src_vol_atten(mo_ptr->channel, &vol_atten, NULL, NULL);
      } else {
        cb_func(CMX_FAILURE, client_data);
      }
    } else if(cb_func != NULL) {
      MSG_ERROR(" Failed to get 3d callback structure ",0,0,0);
      cb_func(CMX_FAILURE, client_data);
    }
  } else if(cb_func != NULL) {
    MSG_ERROR(" Invalid 3d source volume attenuation parameter ",0,0,0);
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_src_pos_processing

DESCRIPTION
  This function processes a 3D positional source position command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_src_pos_processing (
  cmx_3d_pos_src_pos_cmd_type *src_pos,
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
) {
  snd_3d_pos_position_cmd_type pos;
  cmx_midi_out_qcp_handle_type *midi_out_handle;
  cmx_midi_out_qcp_handle_type *mo_ptr = NULL;
  cmx_3d_pos_cb_struct_type    *cb_struct;

  if(src_pos != NULL) {
    MSG_MED(" cmx_3d_pos_src_pos_processing(ID = %d ) ",
             src_pos->id,0,0);
    midi_out_handle = (cmx_midi_out_qcp_handle_type*) src_pos->id;
    if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
       (midi_out_handle != NULL)) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                       q_check (&cmx_midi_out_in_use_q);
      while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
        mo_ptr = (cmx_midi_out_qcp_handle_type *)
                          q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
      }
    }
    if(mo_ptr != NULL) {
      cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

      pos.x_pos        = src_pos->x_pos;
      pos.y_pos        = src_pos->y_pos;
      pos.z_pos        = src_pos->z_pos;
      pos.duration_ms  = src_pos->duration_ms;
      MSG_MED("cmx_3d_pos_src_pos_processing(X_vel= %d,Y_vel= %d ,Z_vel = %d)",
               pos.x_pos,pos.y_pos,pos.z_pos );
      MSG_MED("cmx_3d_pos_src_pos_processing(duration = %d)",
               pos.duration_ms,0,0 );
      if(cb_struct != NULL) {
        snd_3d_pos_src_pos(mo_ptr->channel, &pos,
                                 cmx_3d_pos_cmd_cb_func, cb_struct);
      } else if(cb_func == NULL) {
        snd_3d_pos_src_pos(mo_ptr->channel, &pos, NULL, NULL);
      } else {
        cb_func(CMX_FAILURE, client_data);
      }
    } else if(cb_func != NULL) {
      MSG_ERROR(" Failed to get 3d callback structure",0,0,0 );
      cb_func(CMX_FAILURE, client_data);
    }
  } else if(cb_func != NULL) {
    MSG_ERROR("Invalid source 3d position parameter ",0,0,0 );
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION cmx_3d_pos_src_vel_processing

DESCRIPTION
  This function processes a 3D positional source velocity command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_src_vel_processing (
  cmx_3d_pos_src_vel_cmd_type *src_vel,
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
) {
  snd_3d_pos_velocity_cmd_type vel;
  cmx_midi_out_qcp_handle_type *midi_out_handle;
  cmx_midi_out_qcp_handle_type *mo_ptr = NULL;
  cmx_3d_pos_cb_struct_type    *cb_struct;

  if(src_vel != NULL) {
    MSG_MED("cmx_3d_pos_src_vel_processing(ID = %d ) ",src_vel->id,0,0);
    midi_out_handle = (cmx_midi_out_qcp_handle_type*) src_vel->id;
    if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
       (midi_out_handle != NULL)) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                       q_check (&cmx_midi_out_in_use_q);
      while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
        mo_ptr = (cmx_midi_out_qcp_handle_type *)
                          q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
      }
    }
    if(mo_ptr != NULL) {
      cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

      vel.x_vel        = src_vel->x_vel;
      vel.y_vel        = src_vel->y_vel;
      vel.z_vel        = src_vel->z_vel;
      vel.duration_ms  = src_vel->duration_ms;
      MSG_MED("cmx_3d_pos_src_vel_processing(X_vel= %d,Y_vel = %d,Z_vel = %d)",
               vel.x_vel,vel.y_vel,vel.z_vel);
      MSG_MED("cmx_3d_pos_src_vel_processing(duration = %d)",
               vel.duration_ms,0,0 );
      if(cb_struct != NULL) {
        snd_3d_pos_src_vel(mo_ptr->channel, &vel,
                                 cmx_3d_pos_cmd_cb_func, cb_struct);
      } else if(cb_func == NULL) {
        snd_3d_pos_src_vel(mo_ptr->channel, &vel, NULL, NULL);
      } else {
        cb_func(CMX_FAILURE, client_data);
      }
    } else if(cb_func != NULL) {
      MSG_ERROR(" Failed to get 3d callback structure",0,0,0);
      cb_func(CMX_FAILURE, client_data);
    }
  } else if(cb_func != NULL) {
    MSG_ERROR(" Invalid source 3d velocity parameter ",0,0,0);
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_reverb_src_vol_processing

DESCRIPTION
  This function processes a 3D positional source reverb volume command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_reverb_src_vol_processing (
  cmx_3d_reverb_src_vol_cmd_type *src_reverb_vol,
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
) {
  snd_3d_reverb_src_volume_cmd_type vol;
  cmx_midi_out_qcp_handle_type *midi_out_handle;
  cmx_midi_out_qcp_handle_type *mo_ptr = NULL;
  cmx_3d_pos_cb_struct_type    *cb_struct;

  if(src_reverb_vol != NULL) {
    MSG_MED(" cmx_3d_reverb_src_vol_processing(ID = %d) ",
             src_reverb_vol->id,0,0);
    midi_out_handle = (cmx_midi_out_qcp_handle_type*) src_reverb_vol->id;
    if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
       (midi_out_handle != NULL)) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                       q_check (&cmx_midi_out_in_use_q);
      while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
        mo_ptr = (cmx_midi_out_qcp_handle_type *)
                          q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
      }
    }
    if(mo_ptr != NULL) {
      cb_struct = cmx_3d_pos_setup_cb(cb_func, client_data);

      vol.volume        = src_reverb_vol->volume;
      MSG_MED(" cmx_3d_reverb_src_vol_processing(vol = %d)",vol.volume,0,0 );
      if(cb_struct != NULL) {
        snd_3d_reverb_src_vol(mo_ptr->channel, &vol,
                                 cmx_3d_pos_cmd_cb_func, cb_struct);
      } else if(cb_func == NULL) {
        snd_3d_reverb_src_vol(mo_ptr->channel, &vol, NULL, NULL);
      } else {
        cb_func(CMX_FAILURE, client_data);
      }
    } else if(cb_func != NULL) {
      MSG_ERROR(" Failed to get 3d callback structure",0,0,0);
      cb_func(CMX_FAILURE, client_data);
    }
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_query_env_processing

DESCRIPTION
  This function processes a 3D positional environment query command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_query_env_processing (
  cmx_3d_pos_param_enum_type param,
  cmx_3d_pos_get_cb_ptr_type cb_func,
  void                       *client_data
) {
  snd_3d_pos_param_enum_type cmd;
  cmx_3d_pos_cb_struct_type  *cb_struct;
  MSG_MED("cmx_3d_pos_query_env_processing() called ",0,0,0);
  switch(param) {
    case CMX_3D_POS_PARAM_ENV_ENABLED:
      cmd = SND_3D_POS_PARAM_ENV_ENABLED;
      MSG_MED("CMX_3D_POS_PARAM_ENV_ENABLED",0,0,0);
      break;

    case CMX_3D_POS_PARAM_LSNR_POS:
      cmd = SND_3D_POS_PARAM_LSNR_POS;
      MSG_MED("CMX_3D_POS_PARAM_LSNR_POS",0,0,0);
      break;

    case CMX_3D_POS_PARAM_LSNR_ORIENT:
      cmd = SND_3D_POS_PARAM_LSNR_ORIENT;
      MSG_MED("CMX_3D_POS_PARAM_LSNR_ORIENT",0,0,0);
      break;

    case CMX_3D_POS_PARAM_LSNR_VEL:
      cmd = SND_3D_POS_PARAM_LSNR_VEL;
      MSG_MED("CMX_3D_POS_PARAM_LSNR_VEL",0,0,0);
      break;

    case CMX_3D_POS_PARAM_REVERB_ENABLED:
      cmd = SND_3D_POS_PARAM_REVERB_ENABLED;
      MSG_MED("CMX_3D_POS_PARAM_REVERB_ENABLED",0,0,0);
      break;

    case CMX_3D_POS_PARAM_REVERB:
      cmd = SND_3D_POS_PARAM_REVERB;
      MSG_MED("CMX_3D_POS_PARAM_REVERB",0,0,0);
      break;

    default:
      cmd = SND_3D_POS_PARAM_INVALID;
      break;
  }

  if(cmd != SND_3D_POS_PARAM_INVALID) {
    cb_struct = cmx_3d_pos_setup_query_cb(cb_func, client_data);

    if(cb_struct != NULL) {
      snd_3d_pos_query_env(cmd, cmx_3d_pos_query_cb_func, cb_struct);
    } else {
      cb_func(CMX_FAILURE, param, NULL, client_data);
    }
  } else if(cb_func != NULL) {
     MSG_ERROR("Invalid 3d Position query env processing ",0,0,0);
    cb_func(CMX_FAILURE, param, NULL, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_query_src_processing

DESCRIPTION
  This function processes a 3D positional source query command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_query_src_processing (
  cmx_3d_pos_param_enum_type param,
  void                       *id,
  cmx_3d_pos_get_cb_ptr_type cb_func,
  void                       *client_data
) {
  snd_3d_pos_param_enum_type   cmd;
  cmx_3d_pos_cb_struct_type    *cb_struct;
  cmx_midi_out_qcp_handle_type *midi_out_handle;
  cmx_midi_out_qcp_handle_type *mo_ptr = NULL;

  MSG_MED("cmx_3d_pos_query_src_processing()called",0,0,0);
  switch(param) {
    case CMX_3D_POS_PARAM_SRC_FINAL_SETTINGS:
      cmd = SND_3D_POS_PARAM_SRC_FINAL_SETTINGS;
      MSG_MED("CMX_3D_POS_PARAM_SRC_FINAL_SETTINGS",0,0,0);
      break;

    case CMX_3D_POS_PARAM_SRC_ENABLED:
      cmd = SND_3D_POS_PARAM_SRC_ENABLED;
      MSG_MED("CMX_3D_POS_PARAM_SRC_ENABLED",0,0,0);
      break;

    case CMX_3D_POS_PARAM_SRC_POS:
      cmd = SND_3D_POS_PARAM_SRC_POS;
      MSG_MED("CMX_3D_POS_PARAM_SRC_POS",0,0,0);
      break;

    case CMX_3D_POS_PARAM_SRC_VOLUME:
      cmd = SND_3D_POS_PARAM_SRC_VOLUME;
      MSG_MED("CMX_3D_POS_PARAM_SRC_VOLUME",0,0,0);
      break;

    case CMX_3D_POS_PARAM_SRC_VOL_ATTEN:
      cmd = SND_3D_POS_PARAM_SRC_VOL_ATTEN;
      MSG_MED("CMX_3D_POS_PARAM_SRC_VOL_ATTEN",0,0,0);
      break;

    case CMX_3D_POS_PARAM_SRC_VEL:
      cmd = SND_3D_POS_PARAM_SRC_VEL;
      MSG_MED("CMX_3D_POS_PARAM_SRC_VEL",0,0,0);
      break;

    case CMX_3D_POS_PARAM_SRC_REVERB_VOL:
      cmd = SND_3D_POS_PARAM_SRC_REV_VOL;
      MSG_MED("CMX_3D_POS_PARAM_SRC_REVERB_VOL ",0,0,0);
      break;

    default:
      cmd = SND_3D_POS_PARAM_INVALID;
      break;
  }

  if(cmd != SND_3D_POS_PARAM_INVALID) {
    midi_out_handle = (cmx_midi_out_qcp_handle_type*) id;
    if((cmx_midi_out_status.status == CMX_MIDI_OUT_STATUS_ENABLED) &&
       (midi_out_handle != NULL)) {
      mo_ptr = (cmx_midi_out_qcp_handle_type *)
                                       q_check (&cmx_midi_out_in_use_q);
      while((mo_ptr != NULL) && (mo_ptr != midi_out_handle)) {
        mo_ptr = (cmx_midi_out_qcp_handle_type *)
                          q_next(&cmx_midi_out_in_use_q, &mo_ptr->link);
      }
    }
    if(mo_ptr != NULL) {
      cb_struct = cmx_3d_pos_setup_query_cb(cb_func, client_data);

      if(cb_struct != NULL) {
        snd_3d_pos_query_src(cmd, mo_ptr->channel, cmx_3d_pos_query_cb_func,
                             cb_struct);
        return;
      }
    }
  }

  if(cb_func != NULL) {
    MSG_ERROR("Invalid 3d Position query processing ",0,0,0);
    cb_func(CMX_FAILURE, param, NULL, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_cmd

DESCRIPTION
  This function is used to issue a 3D positional command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_cmd (
  cmx_3d_pos_cmd_enum_type cmd,
  cmx_3d_pos_param_type    *param,
  cmx_cb_func_ptr_type     cb_func,
  void                     *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    switch(cmd) {
      case CMX_3D_POS_CMD_ENABLE:
        packet->hdr.cmd = CMX_CMD_3D_POS_ENABLE;
        break;

      case CMX_3D_POS_CMD_RESET:
        packet->hdr.cmd = CMX_CMD_3D_POS_RESET;
        break;

      case CMX_3D_POS_CMD_SET_LSNR_POS:
        packet->hdr.cmd = CMX_CMD_3D_POS_SET_LSNR_POS;
        break;

      case CMX_3D_POS_CMD_SET_LSNR_ORIENT:
        packet->hdr.cmd = CMX_CMD_3D_POS_SET_LSNR_ORIENT;
        break;

      case CMX_3D_POS_CMD_SET_LSNR_VEL:
        packet->hdr.cmd = CMX_CMD_3D_POS_SET_LSNR_VEL;
        break;

      case CMX_3D_POS_CMD_ENABLE_REVERB:
        packet->hdr.cmd = CMX_CMD_3D_REVERB_ENABLE;
        break;

      case CMX_3D_POS_CMD_DISABLE_REVERB:
        packet->hdr.cmd = CMX_CMD_3D_REVERB_DISABLE;
        break;

      case CMX_3D_POS_CMD_SET_REVERB_PRESET:
        packet->hdr.cmd = CMX_CMD_3D_REVERB_SET_PRESET;
        break;

      case CMX_3D_POS_CMD_SET_REVERB_LEVEL:
        packet->hdr.cmd = CMX_CMD_3D_REVERB_SET_LEVEL;
        break;

      case CMX_3D_POS_CMD_SET_REVERB_TIME:
        packet->hdr.cmd = CMX_CMD_3D_REVERB_SET_TIME;
        break;

      case CMX_3D_POS_CMD_SET_REVERB_RATIO:
        packet->hdr.cmd = CMX_CMD_3D_REVERB_SET_RATIO;
        break;

      case CMX_3D_POS_CMD_SRC_ENABLE:
        packet->hdr.cmd = CMX_CMD_3D_POS_SRC_ENABLE;
        break;

      case CMX_3D_POS_CMD_SRC_RESET:
        packet->hdr.cmd = CMX_CMD_3D_POS_SRC_RESET;
        break;

      case CMX_3D_POS_CMD_SET_SRC_POS:
        packet->hdr.cmd = CMX_CMD_3D_POS_SET_SRC_POS;
        break;

      case CMX_3D_POS_CMD_SET_SRC_VOL:
        packet->hdr.cmd = CMX_CMD_3D_POS_SET_SRC_VOL;
        break;

      case CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN:
        packet->hdr.cmd = CMX_CMD_3D_POS_SET_SRC_VOL_ATTEN;
        break;

      case CMX_3D_POS_CMD_SET_SRC_VEL:
        packet->hdr.cmd = CMX_CMD_3D_POS_SET_SRC_VEL;
        break;

      case CMX_3D_POS_CMD_SET_SRC_REVERB_VOL:
        packet->hdr.cmd = CMX_CMD_3D_REVERB_SET_SRC_VOL;
        break;

      default:
        q_put(&cmx_free_q, &packet->hdr.link);
        if(cb_func != NULL) {
          cb_func(CMX_FAILURE, client_data);
        }
        return;
    }

    packet->cmd_3d_pos.param       = *param;
    packet->cmd_3d_pos.cb_func     = cb_func;
    packet->cmd_3d_pos.client_data = client_data;

    cmx_cmd (packet);
  } else if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_get_param

DESCRIPTION
  This function is used to issue a 3D positional query command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_get_param (
  cmx_3d_pos_get_param_type  *param,
  cmx_3d_pos_get_cb_ptr_type cb_func,
  void                       *client_data
) {
  cmx_packet_type *packet;
  boolean         valid_cmd;

  if(param != NULL) {
    packet = cmx_cmd_get_pkt ();

    if(packet != NULL) {
      switch(param->param) {
        case CMX_3D_POS_PARAM_SRC_FINAL_SETTINGS:
        case CMX_3D_POS_PARAM_SRC_POS:
        case CMX_3D_POS_PARAM_SRC_VOLUME:
        case CMX_3D_POS_PARAM_SRC_VOL_ATTEN:
        case CMX_3D_POS_PARAM_SRC_VEL:
        case CMX_3D_POS_PARAM_SRC_REVERB_VOL:
        case CMX_3D_POS_PARAM_SRC_ENABLED:
          packet->hdr.cmd            = CMX_CMD_3D_POS_QUERY_SRC;
          packet->query_3d_pos.param = param->src_param.param;
          packet->query_3d_pos.id    = param->src_param.id;

          valid_cmd = TRUE;
          break;

        case CMX_3D_POS_PARAM_LSNR_POS:
        case CMX_3D_POS_PARAM_LSNR_ORIENT:
        case CMX_3D_POS_PARAM_LSNR_VEL:
        case CMX_3D_POS_PARAM_REVERB_ENABLED:
        case CMX_3D_POS_PARAM_REVERB:
        case CMX_3D_POS_PARAM_ENV_ENABLED:
          packet->hdr.cmd            = CMX_CMD_3D_POS_QUERY_ENV;
          packet->query_3d_pos.param = param->param;

          valid_cmd = TRUE;
          break;

        default:
          valid_cmd = FALSE;
          break;
      }

      if(valid_cmd) {
        packet->query_3d_pos.cb_func     = cb_func;
        packet->query_3d_pos.client_data = client_data;

        cmx_cmd (packet);
        return;
      } else {
        q_put(&cmx_free_q, &packet->hdr.link);
      }
    }
  }

  if(cb_func != NULL) {
    if(param != NULL) {
      cb_func(CMX_FAILURE, param->param, NULL, client_data);
    } else {
      cb_func(CMX_FAILURE, CMX_3D_POS_PARAM_INVALID, NULL, client_data);
    }
  }
}
#endif /* FEATURE_QAUDIOFX */

/* <EJECT> */
#ifdef FEATURE_AUDIO_POSTCARD
/*===========================================================================

FUNCTION cmx_create_audiop

DESCRIPTION
  This function creates an audio postcard.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_create_audiop
(
  cmx_handle_type      *jpeg,
  cmx_handle_type      *qcp,
  cmx_handle_type      *output,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle1 = NULL, *phandle2 = NULL, *phandle3 = NULL;

  phandle1 = cmx_get_handle();
  if(phandle1 != NULL) {
    *phandle1 = *jpeg;
    phandle2  = cmx_get_handle();

    if(phandle2 != NULL) {
      *phandle2 = *qcp;
      phandle3  = cmx_get_handle();

      if(phandle3 != NULL) {
        *phandle3 = *output;
        packet    = cmx_cmd_get_pkt ();

        if (packet != NULL) {
          packet->hdr.cmd                     = CMX_CMD_CREATE_AUDIOP;
          packet->create_audiop.jpeg          = phandle1;
          packet->create_audiop.qcp           = phandle2;
          packet->create_audiop.output        = phandle3;
          packet->create_audiop.cb_func       = cb_func;
          packet->create_audiop.client_data   = client_data;
          cmx_cmd (packet);
          return;
        }
      }
    }
  }

  cmx_free_handle(phandle1);
  cmx_free_handle(phandle2);
  cmx_free_handle(phandle3);

  if(cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

} /* end cmx_create_audiop */
#endif /* FEATURE_AUDIO_POSTCARD */

/* <EJECT> */
#ifdef FEATURE_DLS
/*===========================================================================

FUNCTION cmx_audfmt_load_dls

DESCRIPTION
  This function queues a command to load the indicated DLS file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_load_dls (
  cmx_handle_type             *handle,
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
) {
  cmx_packet_type *packet;
  cmx_handle_type *phandle;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet = cmx_cmd_get_pkt ();

    if (packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                   = CMX_AF_CMD_LOAD_DLS;
      packet->audfmt_dls.handle         = phandle;
      packet->audfmt_dls.cb_func        = cb_func;
      packet->audfmt_dls.client_data    = client_data;
      cmx_cmd (packet);
      return;
    } else {
      cmx_free_handle(phandle);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_unload_dls

DESCRIPTION
  This function queues a command to unload DLS resources.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_unload_dls (
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();
  if (packet != NULL) {
    packet->hdr.cmd                = CMX_AF_CMD_UNLOAD_DLS;
    packet->audfmt_dls.cb_func     = cb_func;
    packet->audfmt_dls.client_data = client_data;
    cmx_cmd (packet);
  }
}
#endif /* FEATURE_DLS */

/* <EJECT> */
#ifdef FEATURE_XMF
/*===========================================================================

FUNCTION cmx_af_process_play

DESCRIPTION
  This function issues the proper play command after done XMF processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_af_process_play (
  cmx_af_handle_type *handle_ptr
) {
  if(handle_ptr != NULL) {
    if(handle_ptr->cmd == CMX_AF_CMD_PLAY) {
      snd_sound_start ( SND_DEVICE_CURRENT, SND_METHOD_MIDI,
                        (snd_sound_type *) &handle_ptr->sound.audfmt_sound,
                        SND_PRIO_LOW, SND_APATH_LOCAL,
                        (snd_cb_func_ptr_type) cmx_af_snd_play_cb,
                        (void *) handle_ptr );
    } else if(handle_ptr->cmd == CMX_AF_CMD_RINGER) {
      snd_sound_start ( SND_DEVICE_CURRENT, SND_METHOD_MIDI,
                        (snd_sound_type *) &handle_ptr->sound.audfmt_sound,
                        SND_PRIO_MED, SND_APATH_LOCAL,
                        (snd_cb_func_ptr_type) cmx_af_snd_play_cb,
                        (void *) handle_ptr );
    } else if(handle_ptr->cmd == CMX_AF_CMD_GET_TIME) {
      snd_sound_get_time ( (snd_sound_type *) &handle_ptr->sound.audfmt_sound,
                           (snd_time_cb_func_ptr_type) cmx_af_get_time_cb,
                           (void *) handle_ptr );
    } else if(handle_ptr->cmd == CMX_AF_CMD_GET_SPEC) {
      snd_sound_get_specs( (snd_sound_type *) &handle_ptr->sound.audfmt_sound,
                           cmx_af_get_spec_cb, (void *) handle_ptr );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_recognized

DESCRIPTION
  This function performs the necessary setup for a positive XMF file
  recognition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cmx_xmf_status_type cmx_xmf_recognized (
  void *handle
) {
  cmx_af_server_data_type server_data;
  cmx_af_handle_type      *handle_ptr = (cmx_af_handle_type *) handle;
  cmx_xmf_status_type     ret_val = CMX_XMF_SUCCESS;
#ifdef FEATURE_EFS
  fs_rsp_msg_type         rsp;
#endif

  if(handle_ptr != NULL) {
    handle_ptr->status_mask |= CMX_AF_STATUS_MASK_XMF_RECOGNIZED;
    if((handle_ptr->cmd == CMX_AF_CMD_PLAY) ||
       (handle_ptr->cmd == CMX_AF_CMD_RINGER)) {
      cmx_af_do_callback(CMX_SUCCESS, handle_ptr, NULL);
    }
    if((handle_ptr->cb_type != CMX_AF_CALLBACK_NONE) &&
       (handle_ptr->cmd == CMX_AF_CMD_PLAY) &&
       (handle_ptr->cb_func.play != NULL)) {
      server_data.audio_spec.file_type          = CMX_AF_FILE_XMF;
      server_data.audio_spec.midi_spec.sequence = handle_ptr;
      MSG_MED("Issued CMX_AUDIO_SPEC: client_data=0x%x",
               handle_ptr->client_data,0,0);
      handle_ptr->cb_func.play(CMX_AUDIO_SPEC, handle_ptr->client_data,
                               &server_data);
    } else if(handle_ptr->cmd == CMX_AF_CMD_GET_SPEC) {
      server_data.audio_spec.file_type = CMX_AF_FILE_XMF;
      if(handle_ptr->cb_func.spec != NULL) {
        handle_ptr->cb_func.spec
                        (CMX_DONE, handle_ptr->client_data,
                         (cmx_af_audio_spec_type*) &server_data.audio_spec);
      }
#ifdef FEATURE_EFS
      if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
        fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
      }
#endif
      cmx_free_af_handle(handle_ptr);
      cmx_af_active         = CMX_AF_NONE;
      cmx_active_xmf_handle = NULL;

      ret_val = CMX_XMF_DONE;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_unrecognized

DESCRIPTION
  This function indicates that the XMF module does not recognize the file as
  an XMF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_unrecognized (
  void *handle
) {
  cmx_af_handle_type *handle_ptr = (cmx_af_handle_type *) handle;

  if(handle_ptr != NULL) {
    handle_ptr->buffer->in_use_bit_mask  = 0;
    handle_ptr->buffer->in_use_bit_mask2 = 0;

    cmx_active_xmf_handle = NULL;
    cmx_af_process_play(handle_ptr);
    if((handle_ptr->cmd == CMX_AF_CMD_PLAY) ||
       (handle_ptr->cmd == CMX_AF_CMD_RINGER)) {
      cmx_af_do_callback(CMX_SUCCESS, handle_ptr, NULL);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_free_buff

DESCRIPTION
  This function frees the indicated buffer that was being used by the XMF
  module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_free_buff (
  void  *handle,
  uint8 *buffer
) {
  cmx_af_handle_type *handle_ptr = (cmx_af_handle_type *) handle;

  if((handle_ptr != NULL) && (handle_ptr->buffer != NULL)) {
    cmx_af_free_buff(handle_ptr->buffer, buffer);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_get_buff

DESCRIPTION
  This function allocates a buffer to be used by XMF module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint8 *cmx_xmf_get_buff (
  void   *handle,
  uint32 *length
) {
  cmx_af_handle_type *handle_ptr = (cmx_af_handle_type *) handle;
  uint8              *ret_val = NULL;

  if((handle_ptr != NULL) && (handle_ptr->buffer != NULL)) {
    ret_val = cmx_af_get_buff(handle_ptr->buffer);
     if(handle_ptr->buffer->buf_type < CMX_AUDFMT_BUF_INVALID ) {
    *length = cmx_af_buf_info[handle_ptr->buffer->buf_type].num_bytes;
     } else {
       ret_val = NULL;
     }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_client_read_cb_processing

DESCRIPTION
  This function handles a client data request callback for XMF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_client_read_cb_processing (
  cmx_af_handle_type  *handle,
  cmx_xmf_status_type status,
  uint32              len
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type rsp;
#endif /* FEATURE_EFS */

  if(handle != NULL) {
    if(handle->gen_handle != NULL) {
      xmf_data_req_cb(status, handle->gen_handle, len);
    } else {
#ifdef FEATURE_EFS
      if(handle->source_info.source == CMX_SOURCE_EFS) {
        fs_close(handle->source_info.handle.file, NULL, &rsp);
      }
#endif /* FEATURE_EFS */
      cmx_free_af_handle(handle);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_client_read_cb

DESCRIPTION
  This function handles a client data request callback for XMF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_client_read_cb (
  cmx_status_type  status,
  uint8            *data_ptr,
  uint32           len,
  const void       *client_data
) {
  cmx_af_handle_type *handle_ptr = (cmx_af_handle_type *) client_data;
  cmx_packet_type    *packet;

  if(handle_ptr != NULL) {
    packet = cmx_cmd_get_pkt();

    if(packet != NULL) {
      packet->hdr.cmd           = CMX_XMF_DATA_CB;
      packet->xmf_cmd.handle    = handle_ptr;
      packet->xmf_cmd.num_bytes = len;

      if(status == CMX_SUCCESS) {
        if(handle_ptr->data_buf != NULL) {
          memcpy(handle_ptr->data_buf, data_ptr, len);
        }
        packet->xmf_cmd.status = CMX_XMF_SUCCESS;
      } else {
        packet->xmf_cmd.status = CMX_XMF_FAILURE;
      }
      cmx_cmd(packet);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_get_data

DESCRIPTION
  This function reads the indicated data from the XMF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cmx_xmf_status_type cmx_xmf_get_data (
  void   *handle,
  uint8  *buffer,
  uint32 position,
  uint32 *num_bytes,
  void   *state
) {
  cmx_af_handle_type  *handle_ptr = (cmx_af_handle_type *) handle;
  cmx_xmf_status_type ret_val = CMX_XMF_FAILURE;
#ifdef FEATURE_EFS
  fs_rsp_msg_type     rsp;
#endif

  if(handle_ptr != NULL) {
    if(handle_ptr->source_info.source == CMX_SOURCE_MEM) {
      if(position < handle_ptr->source_info.cmx_len) {
        *num_bytes = MIN(*num_bytes, handle_ptr->source_info.cmx_len -
                                     position);
        memcpy(buffer, &(handle_ptr->source_info.handle.buf[position]),
               *num_bytes);

        ret_val = CMX_XMF_SUCCESS;
      }
    } else if(handle_ptr->source_info.source == CMX_SOURCE_CLIENT) {
      handle_ptr->gen_handle = state;
      handle_ptr->data_buf   = buffer;

      /* Request client for more data */
      handle_ptr->source_info.handle.data_fn(position, *num_bytes,
                                             cmx_xmf_client_read_cb,
                                             handle_ptr,
                                             handle_ptr->client_data );

      ret_val = CMX_XMF_BUFF_PENDING;
    }
#ifdef FEATURE_EFS
    else if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
      fs_seek_read(handle_ptr->source_info.handle.file, FS_SEEK_SET,
                   (fs_file_position_type) position, buffer, *num_bytes,
                   NULL, &rsp);

      if(rsp.read.status == FS_OKAY_S) {
        *num_bytes = rsp.read.count;
        ret_val    = CMX_XMF_SUCCESS;
      }
    }
#endif /* FEATURE_EFS */
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_continue_processing

DESCRIPTION
  This function continues XMF processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_continue_processing (
  cmx_af_handle_type *handle
) {
#ifdef FEATURE_EFS
  fs_rsp_msg_type rsp;
#endif /* FEATURE_EFS */

  if(handle != NULL) {
    if(handle->gen_handle != NULL) {
      xmf_process_continue(handle->gen_handle);
    } else {
#ifdef FEATURE_EFS
      if(handle->source_info.source == CMX_SOURCE_EFS) {
        fs_close(handle->source_info.handle.file, NULL, &rsp);
      }
#endif /* FEATURE_EFS */
      cmx_free_af_handle(handle);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_continue

DESCRIPTION
  This function queues a command to continue XMF processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_continue (
  void *handle,
  void *state
) {
  cmx_af_handle_type *handle_ptr = (cmx_af_handle_type *) handle;
  cmx_packet_type    *packet;

  packet = cmx_cmd_get_pkt ();
  if(packet != NULL) {
    handle_ptr->gen_handle = state;
    packet->hdr.cmd        = CMX_XMF_CONTINUE_PROCESS;
    packet->xmf_cmd.handle = handle_ptr;
    cmx_cmd (packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_failure

DESCRIPTION
  This function fails the XMF file processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_failure (
  void *handle
) {
  cmx_af_handle_type  *handle_ptr = (cmx_af_handle_type *) handle;
#ifdef FEATURE_EFS
  fs_rsp_msg_type     rsp;
#endif /* FEATURE_EFS */

  if(handle_ptr != NULL) {
    if(handle_ptr->cmd != CMX_CMD_INVALID) {
#ifdef FEATURE_EFS
      if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
        fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
      }
#endif /* FEATURE_EFS */
    }
    cmx_af_do_callback(CMX_FAILURE, handle_ptr, NULL);
    if(handle_ptr->cmd == CMX_AF_CMD_GET_TIME) {
      if( cmx_af_gt_active )
        cmx_af_gt_active--;

      if(cmx_af_gt_active == 0) {
        cmx_af_active = CMX_AF_NONE;
      }
    } else {
      cmx_af_active = CMX_AF_NONE;
    }
    cmx_free_af_handle(handle_ptr);
  }
  cmx_active_xmf_handle = NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_terminate

DESCRIPTION
  This function terminates the XMF file processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_terminate (
  cmx_af_handle_type    *handle,
  cmx_af_stop_enum_type stop_reason
) {
  cmx_af_server_data_type server_data;

  if(handle != NULL) {
#ifdef FEATURE_DLS
    if(handle->status_mask & CMX_AF_STATUS_MASK_XMF_LOADING_DLS) {
      cmx_audfmt_unload_dls_processing(NULL, NULL);
    }
#endif /* FEATURE_DLS */

    if(handle->gen_handle != NULL) {
      xmf_terminate(handle->gen_handle);
      handle->gen_handle = NULL;
    }
    if((handle->cmd == CMX_AF_CMD_GET_TIME) && (cmx_af_gt_active > 0)) {
      cmx_af_gt_active--;
    }
    handle->data_buf = NULL;

    server_data.abort_status = stop_reason;
    cmx_af_do_callback(CMX_ABORTED, handle, &server_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_load_and_play

DESCRIPTION
  This function loads and/or plays the DLS/MIDI file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cmx_xmf_status_type cmx_xmf_load_and_play (
  void   *handle,
  uint32 dls_position,
  uint32 smf_position,
  void   *state
) {
  cmx_af_handle_type  *handle_ptr = (cmx_af_handle_type *) handle;
  snd_af_access_type  *access_ptr;
  cmx_xmf_status_type ret_val = CMX_XMF_SUCCESS;

  if(handle_ptr != NULL) {
    handle_ptr->buffer->in_use_bit_mask  = 0;
    handle_ptr->buffer->in_use_bit_mask2 = 0;

#ifdef FEATURE_DLS
    if((dls_position != 0) && (handle_ptr->cmd != CMX_AF_CMD_GET_TIME)) {
      handle_ptr->gen_handle   = state;
      handle_ptr->status_mask |= CMX_AF_STATUS_MASK_XMF_LOADING_DLS;

      access_ptr = &handle_ptr->sound.audfmt_sound.audfmt_handle;
      access_ptr->audfmt_ptr = (void *) handle_ptr;
      access_ptr->data_func  = (snd_af_data_cb_func_type) cmx_af_dls_callback;

      snd_audfmt_load_dls(access_ptr, dls_position);
      ret_val = CMX_XMF_BUFF_PENDING;
    } else
#endif /* FEATURE_DLS */
    {
      handle_ptr->sound.audfmt_sound.param.byte_offset = smf_position;

      access_ptr = &handle_ptr->sound.audfmt_sound.audfmt_handle;
      access_ptr->audfmt_ptr = (void *) handle_ptr;
      access_ptr->data_func  = (snd_af_data_cb_func_type) cmx_af_play_callback;

      cmx_active_xmf_handle = NULL;
      cmx_af_process_play(handle_ptr);
    }
  }

  return(ret_val);
}
#endif /* FEATURE_XMF */

/* <EJECT> */
#ifdef FEATURE_SVG
#ifdef FEATURE_SVGDOM
/*===========================================================================

FUNCTION cmx_svg_evthandler_cb

DESCRIPTION
  This function is the callback for event handler

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void cmx_svg_evthandler_cb
(
  void* client_data,
  cmx_svgdom_evtlistener_type listener,
  cmx_svgdom_event_type event
){
  cmx_af_handle_type* handle_ptr = (cmx_af_handle_type*)client_data;

  if( handle_ptr->svg_data->event_handler != NULL){
    handle_ptr->svg_data->event_handler( handle_ptr->svg_data->event_handler_data,
                                        listener,
                                        event);
  }
}
#endif /* FEATURE_SVGDOM */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_decode

DESCRIPTION
  This function issues a command to start playback of SVG file

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_svg_decode_handles
===========================================================================*/
void cmx_svg_decode
(
  cmx_handle_type              *handle,
  cmx_svg_decode_data_type     *cmx_svg_decode_data,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
){
  cmx_packet_type *packet;
  cmx_handle_type *phandle;
  cmx_svg_decode_pk_type  *svg_packet;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet  = cmx_cmd_get_pkt ();

    if(packet != NULL) {
      *phandle = *handle;

      if( cmx_svg_decode_data->empty ){
        phandle->source_type = CMX_SOURCE_MEM;
        phandle->mem.len     = strlen( cmx_svg_empty_document);
        phandle->mem.buf     = (uint8*)cmx_svg_empty_document;
      }

      packet->hdr.cmd                         = CMX_SVG_CMD_DECODE;
      packet->image_decode.handle             = phandle;

      svg_packet   = (cmx_svg_decode_pk_type *) (q_get (&cmx_svg_free_q));

      if( svg_packet != NULL){

        (void) q_link (svg_packet, &svg_packet->link);
        q_put (&cmx_svg_in_use_q, &svg_packet->link);

        packet->image_decode.svg_decode         = svg_packet;
        memcpy( (void*) &packet->image_decode.svg_decode->svg_decode, cmx_svg_decode_data,
                sizeof(cmx_svg_decode_data_type));
        packet->image_decode.cb_func            = callback_ptr;
        packet->image_decode.client_data        = client_data;
        cmx_cmd (packet);
        return;
      }
    }
    cmx_free_handle(phandle);
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }
} /* cmx_svg_decode */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_frame_cmd

DESCRIPTION
  This function issues a command to render a SVG frame

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_frame_cmd
(
  void*                            handle,     /* CMX handle */
  uint32                           frame_time, /*Rendering time of frame */
  cmx_cb_func_ptr_type             callback_ptr,
  void                             *client_data
){
  cmx_packet_type *packet;

  packet  = cmx_cmd_get_pkt ();

  if(packet != NULL) {
    packet->hdr.cmd                         = CMX_SVG_CMD_SEEK;
    packet->svg_frame.handle                = handle;
    packet->svg_frame.time_ms               = frame_time;
    packet->svg_frame.callback_ptr          = callback_ptr;
    packet->svg_frame.client_data           = client_data;
    cmx_cmd (packet);
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* cmx_svg_frame_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_get_info

DESCRIPTION
  This function issues a command to get info of a SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_get_info
(
  cmx_handle_type              *handle,
  cmx_svg_decode_data_type     *cmx_svg_decode_data,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
){
  cmx_packet_type *packet;
  cmx_handle_type *phandle;
  cmx_svg_decode_pk_type  *svg_packet;

  phandle = cmx_get_handle();

  if(phandle != NULL) {
    packet  = cmx_cmd_get_pkt ();

    if(packet != NULL) {
      *phandle = *handle;

      packet->hdr.cmd                         = CMX_SVG_CMD_INFO;
      packet->image_decode.handle             = phandle;

      svg_packet   = (cmx_svg_decode_pk_type *) (q_get (&cmx_svg_free_q));

      if( svg_packet != NULL){

        (void) q_link (svg_packet, &svg_packet->link);
        q_put (&cmx_svg_in_use_q, &svg_packet->link);

        packet->image_decode.svg_decode         = svg_packet;
        memcpy( (void*) &(packet->image_decode.svg_decode->svg_decode), cmx_svg_decode_data,
                sizeof(cmx_svg_decode_data_type));
        packet->image_decode.cb_func            = callback_ptr;
        packet->image_decode.client_data        = client_data;
        cmx_cmd (packet);
        return;
      }
    }
    cmx_free_handle(phandle);
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }
} /* cmx_svg_get_info */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_process_event

DESCRIPTION
  This function issues a command to process user events for SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_process_event
(
  void*                     handle,
  cmx_svg_event_enum_type   event_type,
  void*                     event_data,
  uint32                    num_bytes,
  uint32                    time_ms
){
  cmx_packet_type *packet;
  cmx_af_handle_type*          handle_ptr = (cmx_af_handle_type*)handle;
  cmx_svg_event_done_type     event_done;

  event_done.handled     = FALSE;
  event_done.event       = event_type;

  if( (handle_ptr != NULL) && (handle_ptr->svg_data != NULL) &&
      (handle_ptr->svg_data->svg_handle != NULL)){
    packet  = cmx_cmd_get_pkt ();

    if(packet != NULL) {
      packet->hdr.cmd                 = CMX_SVG_CMD_EVENT;
      packet->svg_event.handle        = handle_ptr->svg_data->svg_handle;
      packet->svg_event.event_type    = event_type;
      //packet->svg_event.event_data    = (cmx_svg-event_type*)event_data;
      //packet->svg_event.num_bytes     = num_bytes;
      packet->svg_event.time_ms       = time_ms;
      (void)memcpy( (void*)&packet->svg_event.event_data, (void*)event_data,
                    num_bytes);
      cmx_cmd (packet);
      return;
    }
  }
  if( handle_ptr != NULL && handle_ptr->cb_func.image != NULL){
    handle_ptr->cb_func.image( CMX_SVG_EVENT, handle_ptr->client_data,
                               (void*)&event_done, sizeof( cmx_svg_event_done_type));
  }

} /* cmx_svg_process_event */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_abort

DESCRIPTION
  This function issues a command to abort SVG file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_abort
(
  void*                    handle,
  cmx_cb_func_ptr_type     callback_ptr,
  void                    *client_data
){
  cmx_packet_type *packet;

  packet  = cmx_cmd_get_pkt ();

  if(packet != NULL) {
    packet->hdr.cmd                 = CMX_SVG_CMD_ABORT;
    packet->svg_base.handle         = handle;
    packet->svg_base.cb_func        = callback_ptr;
    packet->svg_base.client_data    = client_data;
    cmx_cmd (packet);
    MSG_HIGH("cmx_svg_abort: posted cmd to cmx",0,0,0);
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* cmx_svg_abort */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_pause

DESCRIPTION
  This function issues a command to pause SVG file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_pause
(
  void*                    handle,
  cmx_cb_func_ptr_type     callback_ptr,
  void                    *client_data
){
  cmx_packet_type *packet;

  packet  = cmx_cmd_get_pkt ();

  if(packet != NULL) {
    packet->hdr.cmd                 = CMX_SVG_CMD_PAUSE;
    packet->svg_base.handle         = handle;
    packet->svg_base.cb_func        = callback_ptr;
    packet->svg_base.client_data    = client_data;
    cmx_cmd (packet);
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* cmx_svg_pause */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_resume

DESCRIPTION
  This function issues a command to resume playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_resume
(
  void*                    handle,
  cmx_cb_func_ptr_type     callback_ptr,
  void                    *client_data
){
  cmx_packet_type *packet;

  packet  = cmx_cmd_get_pkt ();

  if(packet != NULL) {
    packet->hdr.cmd                 = CMX_SVG_CMD_RESUME;
    packet->svg_base.handle         = handle;
    packet->svg_base.cb_func        = callback_ptr;
    packet->svg_base.client_data    = client_data;
    cmx_cmd (packet);
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* cmx_svg_resume */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_rewind

DESCRIPTION
  This function issues a command to rewind SVG file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_rewind
(
  void*                   handle,
  uint32                  time_ms,
  cmx_cb_func_ptr_type     callback_ptr,
  void                    *client_data
){
  cmx_packet_type *packet;

  packet  = cmx_cmd_get_pkt ();

  if(packet != NULL) {
    packet->hdr.cmd                 = CMX_SVG_CMD_REWIND;
    packet->svg_seek.handle         = handle;
    packet->svg_seek.time_ms        = time_ms;
    packet->svg_seek.callback_ptr   = callback_ptr;
    packet->svg_seek.client_data    = client_data;
    cmx_cmd (packet);
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* cmx_svg_rewind */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_fforward

DESCRIPTION
  This function issues a command to fast-forward SVG file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_fforward
(
  void*                   handle,
  uint32                  time_ms,
  cmx_cb_func_ptr_type     callback_ptr,
  void                    *client_data
){
  cmx_packet_type *packet;

  packet  = cmx_cmd_get_pkt ();

  if(packet != NULL) {
    packet->hdr.cmd                 = CMX_SVG_CMD_FFORWARD;
    packet->svg_seek.handle         = handle;
    packet->svg_seek.time_ms        = time_ms;
    packet->svg_seek.callback_ptr   = callback_ptr;
    packet->svg_seek.client_data    = client_data;
    cmx_cmd (packet);
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* cmx_svg_fforward */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_reset_frame

DESCRIPTION
  This function issues a command to reset the Zoom and Pan factors of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_reset_frame(
  void*                    handle,
  cmx_cb_func_ptr_type     callback_ptr,
  void                     *client_data
){
  cmx_packet_type *packet;

  packet  = cmx_cmd_get_pkt ();

  if(packet != NULL) {
    packet->hdr.cmd                 = CMX_SVG_CMD_RESET;
    packet->svg_base.handle         = handle;
    packet->svg_base.cb_func        = callback_ptr;
    packet->svg_base.client_data    = client_data;
    cmx_cmd (packet);
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_set_viewport

DESCRIPTION
  This function sets the viewport of SVG session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_set_viewport(
  void*                  handle,
  uint32                 width,
  uint32                 height,
  cmx_cb_func_ptr_type   callback_ptr,
  void                  *client_data
){
  cmx_packet_type *packet;

  packet  = cmx_cmd_get_pkt ();

  if(packet != NULL) {
    packet->hdr.cmd                 = CMX_SVG_CMD_SET_VIEWPORT;
    packet->svg_viewport.handle     = handle;
    packet->svg_viewport.width      = width;
    packet->svg_viewport.height     = height;
    packet->svg_viewport.callback_ptr = callback_ptr;
    packet->svg_viewport.client_data  = client_data;
    cmx_cmd (packet);
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_free

DESCRIPTION
  This function frees the SVG session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Queues a command in CMX context
===========================================================================*/
void cmx_svg_free(
  void*                  handle
){
  cmx_packet_type *packet;

  packet  = cmx_cmd_get_pkt ();

  if(packet != NULL) {
    packet->hdr.cmd                 = CMX_SVG_CMD_FREE;
    packet->svg_base.handle         = handle;
    cmx_cmd (packet);
    return;
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_req_cb

DESCRIPTION
  This function is used as a callback by clients for returning external file
  requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Queues a command in CMX context
===========================================================================*/
void cmx_svg_req_cb(
  void* handle,
  uint8** buffer,            /* Buffer with URL data in one chunk */
  uint32 num_bytes
){
  cmx_packet_type *packet;

  packet  = cmx_cmd_get_pkt ();

  if(packet != NULL) {
    packet->hdr.cmd               = CMX_SVG_CMD_REQ_CB;
    packet->svg_req_cb.handle     = handle;
    packet->svg_req_cb.buffer     = buffer;
    packet->svg_req_cb.num_bytes  = num_bytes;
    cmx_cmd (packet);
    return;
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_frame_done_cb

DESCRIPTION
  This function is used as a callback by clients for returning Frame buffer for
  rendering next frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Can be run in UI context.
===========================================================================*/
void cmx_svg_frame_done_cb( void* data
){
  cmx_af_handle_type*    handle_ptr = (cmx_af_handle_type*)data;

  if( handle_ptr != NULL && handle_ptr->svg_data != NULL){
    handle_ptr->svg_data->frame_buf_busy = FALSE;
  }
}

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION cmx_af_svg_frame_cmd_processing

DESCRIPTION
  This function processes an SVG frame command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_af_svg_frame_cmd_processing (
  cmx_af_handle_type     *handle_ptr,
  uint32                 frame_id,
  cmx_af_pos_struct_type *frame_pos
) {
  svg_frame_data_type    frame_data;

  if((handle_ptr != NULL) && (handle_ptr->svg_data != NULL)) {
    switch (handle_ptr->svg_data->status) {
      case CMX_SVG_STATUS_PLAYING:
        if(handle_ptr->svg_data->frame_buf != NULL) {
          cmx_svg_audfmt_frame_pos     = *frame_pos;
          handle_ptr->svg_data->status = CMX_SVG_STATUS_RENDERING;

          frame_data.frame_buf  = handle_ptr->svg_data->frame_buf;
          frame_data.frame_size = handle_ptr->svg_data->frame_bufsize;
          frame_data.time_ms    = frame_id;

          graph_svg_frame_cmd(handle_ptr->svg_data->svg_handle, &frame_data);
        } else {
          handle_ptr->svg_data->frame_time = frame_id;
          handle_ptr->svg_data->status     = CMX_SVG_STATUS_PENDING;
          cmx_svg_audfmt_frame_pos_pending = *frame_pos;
        }
        break;

      case CMX_SVG_STATUS_RENDERING:
      case CMX_SVG_STATUS_PENDING:
        handle_ptr->svg_data->frame_time = frame_id;
        handle_ptr->svg_data->status     = CMX_SVG_STATUS_PENDING;
        cmx_svg_audfmt_frame_pos_pending = *frame_pos;
        break;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_svg_frame_buffer_processing

DESCRIPTION
  This function sets up a memory buffer to be used as an SVG frame buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_af_svg_frame_buffer_processing (
  cmx_af_handle_type *handle_ptr
) {
  if((handle_ptr != NULL) && (handle_ptr->svg_data != NULL)) {
    if(handle_ptr->svg_buffer.buffer != NULL) {
      handle_ptr->svg_data->frame_buf = handle_ptr->svg_buffer.buffer;
    } else if(!(handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF)) {
      handle_ptr->svg_data->frame_buf = CMX_MALLOC(
                                          handle_ptr->svg_data->frame_bufsize);
    }
    if(handle_ptr->svg_data->status == CMX_SVG_STATUS_PENDING) {
      handle_ptr->svg_data->status = CMX_SVG_STATUS_PLAYING;
      cmx_af_svg_frame_cmd_processing (handle_ptr,
                                       handle_ptr->svg_data->frame_time,
                                       &cmx_svg_audfmt_frame_pos_pending);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_svg_frame_done_processing

DESCRIPTION
  This function processes a frame for SVG.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_af_svg_frame_done_processing (
  cmx_af_handle_type *handle_ptr,
  uint8              *frame_buffer,
  uint32             frame_size
) {
  cmx_af_server_data_type  s_data;

  if((handle_ptr != NULL) && (handle_ptr->svg_data != NULL)) {
    if(handle_ptr->cb_func.play != NULL) {
      memset( (void*)&s_data.svg_frame, 0, sizeof(s_data.svg_frame));
      s_data.svg_frame.frame_buf       = frame_buffer;
      s_data.svg_frame.frame_bufsize   = frame_size;
      s_data.svg_frame.frame_pos       = cmx_svg_audfmt_frame_pos;
      handle_ptr->cb_func.play(CMX_SVG_FRAME, handle_ptr->client_data, &s_data);
    }

    if(handle_ptr->svg_data->status == CMX_SVG_STATUS_PENDING) {
      handle_ptr->svg_data->status = CMX_SVG_STATUS_PLAYING;
      cmx_af_svg_frame_cmd_processing (handle_ptr,
                                       handle_ptr->svg_data->frame_time,
                                       &cmx_svg_audfmt_frame_pos_pending);
    } else {
      handle_ptr->svg_data->status = CMX_SVG_STATUS_PLAYING;
    }
  }
}
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_callback

DESCRIPTION
  This function is used as a callback for the SVG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_callback (
  svg_cmx_status_type status,
  void*               handle,
  void*               server_data
) {
  cmx_af_handle_type*          handle_ptr = (cmx_af_handle_type*)handle;
  cmx_status_type              ret_val;
  cmx_packet_type              *packet;
  cmx_client_buf_req_type      buf_req;
  cmx_svg_frame_done_type      frame_done;
  cmx_svg_request_free_type    ext_free;
  cmx_image_cb_func_ptr_type   callback_ptr;
  void*                        client_data;

  svg_cmx_get_data_type*   svg_get_data;
  svg_cmx_info_type*       svg_info;
  svg_cmx_request_type*    svg_request;
  svg_cmx_req_data_type*   svg_req_data;
  svg_cmx_frame_done_type* svg_frame_done;

#ifdef FEATURE_SVGDOM
  cmx_svgdom_data_type         dom_data;
  svg_dom_cmx_data_type*       svg_dom_data;
#endif /* FEATURE_SVGDOM */

#ifdef FEATURE_EFS
  fs_rsp_msg_type rsp;
#endif
#ifdef FEATURE_WEBAUDIO
  cmx_packet_type *packet2;
#endif /* FEATURE_WEBAUDIO */

  if(handle_ptr != NULL) {

    callback_ptr  = handle_ptr->cb_func.image;
    client_data   = handle_ptr->client_data;

    switch(status) {
      case SVG_CMX_GET_FILE_SIZE:
        /* Get size of content */
        if(server_data != NULL) {
#ifdef FEATURE_WEBAUDIO
          if(handle_ptr->cmd == CMX_AF_CMD_PLAY) {
            *((uint32 *) server_data) = cmx_svg_size;
          } else
#endif /* FEATURE_WEBAUDIO */
          {
            *((uint32 *) server_data) = handle_ptr->source_info.cmx_len;
          }
        }
        break;

      case SVG_CMX_GET_DATA:
        /* Read data */
        svg_get_data = (svg_cmx_get_data_type *) server_data;

        packet = cmx_cmd_get_pkt();
        if(packet != NULL) {
          packet->hdr.cmd                   = CMX_IMAGE_CMD_READ;
          packet->image_read.image_type     = CMX_IMAGE_TYPE_SVG;
          packet->image_read.handle         = handle_ptr;
          packet->image_read.buffer         = svg_get_data->buffer;
          packet->image_read.num_bytes      = svg_get_data->num_bytes;
          packet->image_read.position       = svg_get_data->position;
          packet->image_read.cb_func.svg_cb = svg_get_data->callback_ptr;
          packet->image_read.client_data    = svg_get_data->client_data;
          cmx_cmd (packet);
        }
        break;

      case SVG_CMX_INFO:
        /* SVG specification */

        if((handle_ptr == NULL) || (handle_ptr->svg_data == NULL)) {
          return;
        }

        svg_info  = (svg_cmx_info_type*) server_data;
        handle_ptr->svg_data->svg_handle = svg_info->handle;
        MSG_HIGH("cmx_svg_callback: svg_handle populated:%x",svg_info->handle,0,0);
        handle_ptr->svg_data->width      = svg_info->width;
        handle_ptr->svg_data->height     = svg_info->height;

        if( handle_ptr->cmd == CMX_SVG_CMD_DECODE ||
            handle_ptr->cmd == CMX_SVG_CMD_INFO )
        {
          if(callback_ptr != NULL) {
            cmx_svg_info_type cmx_svg_info;

            cmx_svg_info.animated        = svg_info->animated;
            cmx_svg_info.color_scheme    = svg_info->color_scheme;
            cmx_svg_info.text_only       = svg_info->text_only;
            cmx_svg_info.zoom_pan_events = svg_info->zoom_pan_events;
            cmx_svg_info.handle          = handle_ptr;
            cmx_svg_info.transparency    = handle_ptr->svg_data->transparent;
            cmx_svg_info.width           = svg_info->width;
            cmx_svg_info.height          = svg_info->height;
            cmx_svg_info.document        = svg_info->doc;
            memcpy(cmx_svg_info.title, svg_info->title, 127);
            cmx_svg_info.title[127] = 0;

            if(handle_ptr->cmd == CMX_SVG_CMD_INFO) {
              ret_val = CMX_SUCCESS;
            } else {
              ret_val = CMX_SVG_INFO;
            }

            callback_ptr(ret_val, client_data, (void*) &cmx_svg_info,
                          sizeof(cmx_svg_info_type));
          }

        if( handle_ptr->cmd == CMX_SVG_CMD_INFO ||
            handle_ptr->svg_data->status == CMX_SVG_STATUS_STOPPED){
            graph_svg_stop(handle_ptr->svg_data->svg_handle);
            cmx_svg_free(handle_ptr);
            return;
          }
        }
#ifdef FEATURE_WEBAUDIO
        else if(handle_ptr->cmd == CMX_AF_CMD_PLAY) {
          if(handle_ptr->cb_func.play != NULL) {
            cmx_af_server_data_type  s_data;

            s_data.svg_info.animated        = svg_info->animated;
            s_data.svg_info.color_scheme    = svg_info->color_scheme;
            s_data.svg_info.text_only       = svg_info->text_only;
            s_data.svg_info.zoom_pan_events = svg_info->zoom_pan_events;
            s_data.svg_info.handle          = handle_ptr;
            s_data.svg_info.transparency    = handle_ptr->svg_data->transparent;
            s_data.svg_info.width           = svg_info->width;
            s_data.svg_info.height          = svg_info->height;
            s_data.svg_info.document        = svg_info->doc;
            memcpy(s_data.svg_info.title, svg_info->title, 127);
            s_data.svg_info.title[127] = 0;

            handle_ptr->cb_func.play(CMX_SVG_INFO, handle_ptr->client_data,
                                     &s_data);
          }
        }
#endif /* FEATURE_WEBAUDIO */

        //fall thru

    case SVG_CMX_GET_BUF:
            /* Request frame buffer */
      if( handle_ptr->svg_data != NULL && handle_ptr->svg_data->frame_bufsize == 0){

        if( status == SVG_CMX_INFO){
            handle_ptr->svg_data->frame_bufsize =
              (handle_ptr->svg_data->width * handle_ptr->svg_data->height) *
              ((handle_ptr->svg_data->color_scheme == CMX_COLOR_SCHEME_RGB565)?2:4);
        } else{
          handle_ptr->svg_data->frame_bufsize = *(uint32*)server_data;
        }

        if( handle_ptr->cmd == CMX_SVG_CMD_DECODE ||
            handle_ptr->cmd == CMX_SVG_CMD_INFO )
        {
            packet = cmx_cmd_get_pkt ();
            if (packet != NULL) {
              packet->hdr.cmd                     = CMX_SVG_CMD_START_PLAY;
              packet->svg_cont.handle             = handle_ptr;
              packet->svg_cont.buffer.handle      = handle_ptr;
              packet->svg_cont.buffer.pending_cmd = packet;
              packet->svg_cont.buffer.buffer      = NULL;
              packet->svg_cont.request            = TRUE;
            }

            handle_ptr->status_mask |= CMX_AF_STATUS_MASK_USE_CLIENT_BUF;
            buf_req.buffer_size       = handle_ptr->svg_data->frame_bufsize;
            buf_req.client_data       = &(packet->svg_cont.buffer);
            buf_req.cb_func           = (cmx_client_buf_req_cb_func_ptr_type)
                                                   cmx_client_buf_req_cb;
            buf_req.buffer_supplied = FALSE;

            if(callback_ptr != NULL) {
              callback_ptr(CMX_CLIENT_BUF_REQ, client_data, &buf_req,
                           sizeof(cmx_client_buf_req_type));
            }

            if(!buf_req.buffer_supplied) {
              handle_ptr->status_mask &= ~CMX_AF_STATUS_MASK_USE_CLIENT_BUF;
          }
          }/* end of standalone decode */
#ifdef FEATURE_WEBAUDIO
        else if(handle_ptr->cmd == CMX_AF_CMD_PLAY) {

          if((handle_ptr->svg_buffer.buffer == NULL) &&
             (handle_ptr->svg_buffer.pending_cmd == NULL)) {
            packet  = cmx_cmd_get_pkt ();
            packet2 = cmx_cmd_get_pkt ();
            if ((packet != NULL) && (packet2 != NULL)) {
              handle_ptr->svg_buffer.handle      = handle_ptr;
              handle_ptr->svg_buffer.pending_cmd = packet2;
              packet->hdr.cmd                    = CMX_CMD_CLIENT_BUF_REQ;
              packet->client_buf_req.handle      = handle_ptr;
              packet->client_buf_req.buf_handle  = &(handle_ptr->svg_buffer);
              packet->client_buf_req.num_bytes   =
                                          handle_ptr->svg_data->frame_bufsize;

              packet2->hdr.cmd                 = CMX_CMD_SVG_FRAME_BUFFER;
              packet2->svg_frame_buffer.handle = handle_ptr;
              cmx_cmd(packet);
            } else {
              if(packet != NULL) {
                q_put (&cmx_free_q, &packet->hdr.link);
              }
              if(packet2 != NULL) {
                q_put (&cmx_free_q, &packet2->hdr.link);
              }
            }
          }
        }
#endif /* FEATURE_WEBAUDIO */
       }
        break;

      case SVG_CMX_PREPARE_DONE:
        /* SVG content loaded, ready for rendering frames */
        if((handle_ptr == NULL) || (handle_ptr->svg_data == NULL)) {
          return;
        }

        handle_ptr->svg_data->status = CMX_SVG_STATUS_PLAYING;

        if( handle_ptr->cmd == CMX_SVG_CMD_DECODE ||
            handle_ptr->cmd == CMX_SVG_CMD_INFO ) {
          if(callback_ptr != NULL) {
            callback_ptr( CMX_SVG_PREPARE_DONE, client_data, NULL, 0);
          }

          if(handle_ptr->svg_data->cmx_svg_decode_mode == CMX_SVG_MODE_CMX) {
            /* Render frames */
            packet = cmx_cmd_get_pkt ();
            if (packet != NULL) {
              packet->hdr.cmd                   = CMX_SVG_CMD_START_PLAY;
              packet->svg_cont.handle           = handle_ptr;
              packet->svg_cont.buffer.handle    = handle_ptr;
              packet->svg_cont.request          = FALSE;
              cmx_cmd (packet);
            } else {
              if(callback_ptr != NULL) {
                callback_ptr(CMX_FAILURE, client_data, NULL, 0);
              }
              graph_svg_stop(handle_ptr->svg_data->svg_handle);
              cmx_svg_free(handle_ptr);
              return;
            }
          }
        }
        break;

      case SVG_CMX_PAUSE:
        if((handle_ptr->cmd == CMX_SVG_CMD_DECODE) && (callback_ptr != NULL)) {
          callback_ptr(CMX_PAUSE, client_data, NULL, 0);
        }
        break;

      case SVG_CMX_RESUME:
        if((handle_ptr->cmd == CMX_SVG_CMD_DECODE) && (callback_ptr != NULL)) {
          callback_ptr(CMX_RESUME, client_data, NULL, 0);
        }
        break;

      case SVG_CMX_REWIND:
        if((handle_ptr->cmd == CMX_SVG_CMD_DECODE) && (callback_ptr != NULL)) {
          callback_ptr(CMX_REWIND, client_data, NULL, 0);
        }
        break;

      case SVG_CMX_FFORWARD:
        if((handle_ptr->cmd == CMX_SVG_CMD_DECODE) && (callback_ptr != NULL)) {
          callback_ptr(CMX_FFORWARD, client_data, NULL, 0);
        }
        break;

      case SVG_CMX_ABORT:
        MSG_HIGH("cmx_svg_callback: SVG_CMX_ABORT recd, cmd: %d",handle_ptr->cmd,0,0);
        if(handle_ptr->cmd == CMX_SVG_CMD_DECODE) {
          if(callback_ptr != NULL) {
            callback_ptr(CMX_ABORTED, client_data, NULL, 0);
          }
          cmx_svg_free(handle_ptr);
        } else if( handle_ptr->cmd == CMX_AF_CMD_PLAY){
          cmx_svg_free(handle_ptr);
        }
        break;

      case SVG_CMX_FAIL:
        MSG_HIGH("cmx_svg_callback: SVG_CMX_FAIL recd, cmd: %d",handle_ptr->cmd,0,0);
        if(handle_ptr->cmd == CMX_SVG_CMD_DECODE) {
          if(callback_ptr != NULL) {
            callback_ptr(CMX_FAILURE, client_data, NULL, 0);
          }
          cmx_svg_free(handle_ptr);
        }
        break;

      case SVG_CMX_FRAME_DONE:
        if((handle_ptr == NULL) || (handle_ptr->svg_data == NULL) ||
           ((handle_ptr->svg_data->status != CMX_SVG_STATUS_RENDERING) &&
            (handle_ptr->svg_data->status != CMX_SVG_STATUS_PENDING) &&
            (handle_ptr->svg_data->status != CMX_SVG_STATUS_PLAYING))) {
          return;
        }

        svg_frame_done = (svg_cmx_frame_done_type*) server_data;
        if(handle_ptr->cmd == CMX_SVG_CMD_DECODE) {
          frame_done.frame_buf       = svg_frame_done->frame_buf;
          frame_done.frame_bufsize   = svg_frame_done->frame_size;
          frame_done.bitmask_buf     = NULL;
          frame_done.bitmask_bufsize = 0;
          frame_done.time_ms         = svg_frame_done->time_ms;
          frame_done.last_flag       = svg_frame_done->flag_last_frame;
          frame_done.frame_done_cb     = cmx_svg_frame_done_cb;
          frame_done.frame_done_data   = (void*) handle_ptr;

          handle_ptr->svg_data->status = CMX_SVG_STATUS_PLAYING;
          handle_ptr->svg_data->frame_buf_busy = TRUE;

          if(callback_ptr != NULL) {
            callback_ptr(CMX_SVG_FRAME, client_data, &frame_done,
                         sizeof(cmx_svg_frame_done_type));
          }

          if(frame_done.last_flag) {
            if( handle_ptr->svg_data->auto_stop == TRUE){
            /* Last frame so do cleanup */
            if(callback_ptr != NULL) {
              callback_ptr(CMX_DONE, client_data, NULL, 0);
            }
            graph_svg_stop(handle_ptr->svg_data->svg_handle);
            cmx_svg_free(handle_ptr);
            }
          } else if(handle_ptr->svg_data->cmx_svg_decode_mode == CMX_SVG_MODE_CMX) {
            rex_set_timer(&handle_ptr->svg_data->frame_timer, handle_ptr->svg_data->delay);
          }
        }
#ifdef FEATURE_WEBAUDIO
        else if(handle_ptr->cmd == CMX_AF_CMD_PLAY) {
          packet = cmx_cmd_get_pkt ();
          if(packet != NULL) {
            packet->hdr.cmd                     = CMX_CMD_SVG_FRAME_DONE;
            packet->svg_frame_done.handle       = handle_ptr;
            packet->svg_frame_done.frame_buffer = svg_frame_done->frame_buf;
            packet->svg_frame_done.frame_size   = svg_frame_done->frame_size;
            cmx_cmd( packet);
          }
        }
#endif /* FEATURE_WEBAUDIO */
        break;

      case SVG_CMX_EVENT_DONE:
        if((handle_ptr->cmd == CMX_SVG_CMD_DECODE) && (callback_ptr != NULL)) {
          callback_ptr(CMX_SVG_EVENT, client_data, server_data,
                       sizeof(cmx_svg_event_done_type));
        }
        break;

      case SVG_CMX_EXT_REQ:
        if(handle_ptr->cmd == CMX_SVG_CMD_DECODE) {
          svg_request = (svg_cmx_request_type*) server_data;

          if(svg_request->type > 1) { /* URL request */
            if(callback_ptr != NULL) {
              callback_ptr(CMX_SVG_URL_REQ, client_data, svg_request->filename,
                           strlen(svg_request->filename));
            }
            /* Current SVG session is done
               URL requests must be handled seperately by clients */
            if( handle_ptr->svg_data->auto_stop == TRUE){
              if(callback_ptr != NULL) {
                callback_ptr( CMX_DONE, client_data, NULL, 0);
              }
            graph_svg_stop(handle_ptr->svg_data->svg_handle);
            cmx_svg_free(handle_ptr);
            }
            break;
          } else {
            packet = cmx_cmd_get_pkt ();
            if(packet != NULL) {
              packet->hdr.cmd          = CMX_SVG_CMD_REQ;
              packet->svg_req.svg_req  = &cmx_svg_req;
              if(packet->svg_req.svg_req != NULL) {
                packet->svg_req.svg_req->handle     = handle_ptr;
                packet->svg_req.svg_req->type       = svg_request->type;
                packet->svg_req.svg_req->cb_func    = svg_request->callback_ptr;
                packet->svg_req.svg_req->cb_data    = svg_request->client_data;
                packet->svg_req.svg_req->buf_ptr    = svg_request->buf_ptr;
                packet->svg_req.svg_req->num_bytes  = svg_request->num_bytes;
                packet->svg_req.svg_req->next_cb    = svg_request->next_cb;
                memcpy(packet->svg_req.svg_req->filename,
                       svg_request->filename, FS_FILENAME_MAX_LENGTH_P - 1);
                packet->svg_req.svg_req->filename
                                         [FS_FILENAME_MAX_LENGTH_P - 1] = 0;

                cmx_cmd( packet);

                break;
              }
            }

            if(svg_request->type == 0) {
              if(callback_ptr != NULL) {
                callback_ptr(CMX_FAILURE, client_data, NULL, 0);
              }
              graph_svg_stop(handle_ptr->svg_data->svg_handle);
              cmx_svg_free(handle_ptr);
            }
          }
        }
        break;

      case SVG_CMX_EXT_FILE_SIZE:
        if(handle_ptr->cmd == CMX_SVG_CMD_DECODE) {
          if(server_data != NULL) {
            *((uint32*)server_data) = handle_ptr->svg_req.data.efs.file_size;
          }
        }
        break;

      case SVG_CMX_EXT_DATA:
        if(handle_ptr->cmd == CMX_SVG_CMD_DECODE) {
          svg_req_data = (svg_cmx_req_data_type*) server_data;

          packet = cmx_cmd_get_pkt ();
          if (packet != NULL) {
            packet->hdr.cmd                = CMX_SVG_CMD_REQ_DATA;
            packet->svg_req_data.handle    = handle_ptr;
            packet->svg_req_data.buffer    = svg_req_data->buffer;
            packet->svg_req_data.num_bytes = svg_req_data->num_bytes;
            packet->svg_req_data.position  = svg_req_data->position;

            cmx_cmd(packet);
          } else if(handle_ptr->svg_req.type == 0) {
            if(callback_ptr != NULL) {
              callback_ptr(CMX_FAILURE, client_data, NULL, 0);
            }
            graph_svg_stop(handle_ptr->svg_data->svg_handle);
            cmx_svg_free(handle_ptr);
          }
        }
        break;

      case SVG_CMX_EXT_FREE:
        if(handle_ptr->cmd == CMX_SVG_CMD_DECODE) {
#ifdef FEATURE_EFS
          if(handle_ptr != NULL && handle_ptr->svg_req.handle != NULL &&
             handle_ptr->svg_req.type != 0xFF &&
             handle_ptr->svg_req.data.efs.fs_handle != NULL) {
            fs_close(handle_ptr->svg_req.data.efs.fs_handle, NULL, &rsp);
            handle_ptr->svg_req.type = 0;
            handle_ptr->svg_req.data.efs.fs_handle = 0;

            if( handle_ptr->svg_req.next_cb != NULL)
        {
              handle_ptr->svg_req.next_cb ( handle_ptr->svg_req.cb_data);
            }
            break;
          }
#endif
          ext_free.loaded     = ((svg_cmx_request_done_type*)server_data)->handled;
          memcpy(ext_free.filename,
                 ((svg_cmx_request_done_type *) server_data)->filename,
                 FS_FILENAME_MAX_LENGTH_P - 1);
          ext_free.filename[FS_FILENAME_MAX_LENGTH_P - 1] = 0;
          if(callback_ptr != NULL) {
            callback_ptr( CMX_SVG_EXT_FREE, client_data, &ext_free, sizeof(cmx_svg_request_free_type));
          }
        }
        break;

#ifdef FEATURE_SVGDOM
    case SVG_CMX_DOM_DATA:

      svg_dom_data        = (svg_dom_cmx_data_type*)server_data;
      dom_data.api        = svg_dom_data->api;
      dom_data.type       = svg_dom_data->type;
      dom_data.error      = svg_dom_data->error;
      dom_data.num_bytes  = svg_dom_data->num_bytes;
      memcpy( &dom_data.data, &svg_dom_data->data, svg_dom_data->num_bytes);
      cmx_svgdom_free_strings(handle_ptr, dom_data.api);
      if(callback_ptr != NULL) {
        callback_ptr( CMX_SVG_DOM_DATA, client_data, &dom_data, sizeof( dom_data));
      }
      break;

#endif /* FEATURE_SVGDOM */

    }/* switch(status) */
  } /* handle_ptr != NULL */
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_decode_processing

DESCRIPTION
  This function starts playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_decode_processing(
  cmx_handle_type*                 handle,
  cmx_svg_decode_data_type*         svg_decode,
  cmx_image_cb_func_ptr_type       callback_ptr,
  void*                            client_data
){
  cmx_af_handle_type      *handle_ptr = NULL;
  svg_load_data_type      svg_load_data;

  cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_SVG_CMD_DECODE);

  if(handle_ptr != NULL && svg_decode != NULL) {
    handle_ptr->svg_data               = CMX_MALLOC( sizeof( cmx_svg_data_type));
    if( handle_ptr->svg_data != NULL){
      handle_ptr->cmd                           = CMX_SVG_CMD_DECODE;
      handle_ptr->cb_func.image                 = callback_ptr;
      handle_ptr->client_data                   = client_data;

      memset( (void*)handle_ptr->svg_data, 0, sizeof( cmx_svg_data_type));
      handle_ptr->svg_data->cmx_svg_decode_mode  = svg_decode->cmx_svg_decode_mode;
      handle_ptr->svg_data->width                = svg_decode->width;
      handle_ptr->svg_data->height               = svg_decode->height;
      handle_ptr->svg_data->dpi                  = svg_decode->dpi;
      handle_ptr->svg_data->transparent          = svg_decode->transparency;
      handle_ptr->svg_data->progressive          = svg_decode->progressive;
      handle_ptr->svg_data->auto_stop            = svg_decode->auto_stop;
      handle_ptr->svg_data->bk_color             = svg_decode->bk_color;
      handle_ptr->svg_data->delay                =
        ( svg_decode->delay == 0 ? CMX_SVG_MIN_DELAY : svg_decode->delay);
      memcpy(handle_ptr->svg_data->directory, svg_decode->directory,
             FS_FILENAME_MAX_LENGTH_P - 1);

      handle_ptr->svg_data->mem_buffer           = CMX_MALLOC( CMX_SVG_MEM_BUFFER_SIZE);

      if( handle_ptr->svg_data->mem_buffer != NULL){
        cmx_svg_offset = 0;

        svg_load_data.svg_frame_width             = svg_decode->width;
        svg_load_data.svg_frame_height            = svg_decode->height;
        svg_load_data.dpi                         = svg_decode->dpi;
        svg_load_data.transparency                = svg_decode->transparency;
        svg_load_data.bk_color                    = svg_decode->bk_color;
        svg_load_data.cb_func                     = (svg_cmx_cb_type) cmx_svg_callback;
        svg_load_data.cb_data                     = (void*) handle_ptr;
        svg_load_data.mem_buffer                  = handle_ptr->svg_data->mem_buffer;
        svg_load_data.mem_buffer_size             = CMX_SVG_MEM_BUFFER_SIZE;
        svg_load_data.max_requests                = CMX_SVG_MAX_REQUESTS;
        svg_load_data.progressive                 = svg_decode->progressive;

        svg_load_data.filename                    = handle_ptr->svg_data->directory;
        graph_svg_decode( &svg_load_data);

        return;
      } else {
        MSG_ERROR("Failed to allocate memory", 0,0,0);
      } /* mem_buffer != NULL */
    } else {
      MSG_ERROR("Failed to allocate memory", 0,0,0);
    } /* svg_data != NULL */
  }

  if(handle_ptr != NULL) {
    if( handle_ptr->svg_data != NULL){
      if( handle_ptr->svg_data->mem_buffer != NULL){
        CMX_FREE( handle_ptr->svg_data->mem_buffer);
      }
      CMX_FREE( handle_ptr->svg_data);
    }
    cmx_free_af_handle(handle_ptr);
  }else {
    cmx_free_handle(handle);
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_abort_processing

DESCRIPTION
  This function aborts playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_abort_processing
(
  void*                  handle,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  cmx_af_handle_type*  handle_ptr = (cmx_af_handle_type*)handle;

  if( handle_ptr != NULL  && handle_ptr->svg_data != NULL &&
      handle_ptr->cmd != CMX_CMD_INVALID){
    if( handle_ptr->svg_data->status != CMX_SVG_STATUS_STOPPED) {
      if( handle_ptr->svg_data->status != CMX_SVG_STATUS_PREPARING){
        graph_svg_abort( handle_ptr->svg_data->svg_handle);
      }
      handle_ptr->svg_data->status = CMX_SVG_STATUS_STOPPED;

      if(callback_ptr != NULL) {
        callback_ptr(CMX_SUCCESS, client_data);
      }
    }
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_pause_processing

DESCRIPTION
  This function pauses the playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_pause_processing
(
  void*                  handle,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  cmx_af_handle_type*  handle_ptr = (cmx_af_handle_type*)handle;

  if( handle_ptr != NULL && handle_ptr->svg_data != NULL){
    if( handle_ptr->svg_data->status == CMX_SVG_STATUS_PLAYING ||
        handle_ptr->svg_data->status == CMX_SVG_STATUS_RENDERING ) {
      graph_svg_pause( handle_ptr->svg_data->svg_handle);
      handle_ptr->svg_data->status = CMX_SVG_STATUS_PAUSED;

      rex_clr_timer( &handle_ptr->svg_data->actual_time);
      rex_clr_timer( &handle_ptr->svg_data->frame_timer);

      if(callback_ptr != NULL) {
        callback_ptr(CMX_SUCCESS, client_data);
      }
    }
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* cmx_svg_pause_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_resume_processing

DESCRIPTION
  This function resumes playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_resume_processing
(
  void*                  handle,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  cmx_af_handle_type*  handle_ptr = (cmx_af_handle_type*)handle;

  if( handle_ptr != NULL && handle_ptr->svg_data != NULL ){
    if( handle_ptr->svg_data->status == CMX_SVG_STATUS_PAUSED) {
      graph_svg_resume( handle_ptr->svg_data->svg_handle);
      handle_ptr->svg_data->status = CMX_SVG_STATUS_PLAYING;

      /*Generate a new frame */
      rex_set_timer( &handle_ptr->svg_data->frame_timer, 10);
      rex_set_timer( &handle_ptr->svg_data->actual_time, CMX_SVG_MAX_TIME);

      if(callback_ptr != NULL) {
        callback_ptr(CMX_SUCCESS, client_data);
      }
    }
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* cmx_svg_resume_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_reset_processing

DESCRIPTION
  This function resets the Pan and Zoom factors of a SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_reset_processing
(
  void*                  handle,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  cmx_af_handle_type*  handle_ptr = (cmx_af_handle_type*)handle;

  if( handle_ptr != NULL  && handle_ptr->svg_data != NULL){
    if( handle_ptr->svg_data->status != CMX_SVG_STATUS_STOPPED) {
      graph_svg_frame_reset( handle_ptr->svg_data->svg_handle);

      if(callback_ptr != NULL) {
        callback_ptr(CMX_SUCCESS, client_data);
      }
    }
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* cmx_svg_reset_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_rewind_processing

DESCRIPTION
  This function rewinds playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_rewind_processing
(
  void*                  handle,
  uint32                 time_ms,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  cmx_af_handle_type*  handle_ptr = (cmx_af_handle_type*)handle;

  if( handle_ptr != NULL && handle_ptr->svg_data != NULL ){
    if( handle_ptr->svg_data->status != CMX_SVG_STATUS_STOPPED) {
      graph_svg_rewind(handle_ptr->svg_data->svg_handle, time_ms );

      if(callback_ptr != NULL) {
        callback_ptr(CMX_SUCCESS, client_data);
      }
    }
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* cmx_svg_rewind_processing */


/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_fforward_processing

DESCRIPTION
  This function fast-forwards playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_fforward_processing
(
  void*                  handle,
  uint32                 time_ms,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  cmx_af_handle_type*  handle_ptr = (cmx_af_handle_type*)handle;

  if( handle_ptr != NULL && handle_ptr->svg_data != NULL ){
    if( handle_ptr->svg_data->status != CMX_SVG_STATUS_STOPPED) {
      graph_svg_fforward(handle_ptr->svg_data->svg_handle, time_ms );

      if(callback_ptr != NULL) {
        callback_ptr(CMX_SUCCESS, client_data);
      }
    }
    return;
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* cmx_svg_fforward_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_info_processing

DESCRIPTION
  This function reads data from files for external file requests made by

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_info_processing(
  cmx_handle_type*                 handle,
  cmx_svg_decode_data_type*         svg_decode,
  cmx_image_cb_func_ptr_type       callback_ptr,
  void*                            client_data
){
  cmx_af_handle_type      *handle_ptr = NULL;
  svg_load_data_type      svg_load_data;

  cmx_audfmt_setup_handle(handle, &handle_ptr, CMX_SVG_CMD_DECODE);

  if(handle_ptr != NULL) {
    handle_ptr->svg_data    = CMX_MALLOC( sizeof(cmx_svg_data_type));
    if( handle_ptr->svg_data != NULL){
      handle_ptr->cmd                           = CMX_SVG_CMD_INFO;
      handle_ptr->cb_func.image                 = callback_ptr;
      handle_ptr->client_data                   = client_data;

      memset( (void*)handle_ptr->svg_data, 0, sizeof( cmx_svg_data_type));
      handle_ptr->svg_data->cmx_svg_decode_mode  = svg_decode->cmx_svg_decode_mode;
      handle_ptr->svg_data->width                = svg_decode->width;
      handle_ptr->svg_data->height               = svg_decode->height;
      handle_ptr->svg_data->dpi                  = svg_decode->dpi;
      handle_ptr->svg_data->transparent          = svg_decode->transparency;
      handle_ptr->svg_data->bk_color             = svg_decode->bk_color;
      memcpy(handle_ptr->svg_data->directory, svg_decode->directory,
             FS_FILENAME_MAX_LENGTH_P - 1);

      handle_ptr->svg_data->mem_buffer           = CMX_MALLOC( CMX_SVG_MEM_BUFFER_SIZE);

      if( handle_ptr->svg_data->mem_buffer != NULL){
        svg_load_data.svg_frame_width             = svg_decode->width;
        svg_load_data.svg_frame_height            = svg_decode->height;
        svg_load_data.dpi                         = svg_decode->dpi;
        svg_load_data.transparency                = svg_decode->transparency;
        svg_load_data.bk_color                    = svg_decode->bk_color;
        svg_load_data.cb_func                     = (svg_cmx_cb_type) cmx_svg_callback;
        svg_load_data.cb_data                     = (void*) handle_ptr;
        svg_load_data.mem_buffer                  = handle_ptr->svg_data->mem_buffer;
        svg_load_data.mem_buffer_size             = CMX_SVG_MEM_BUFFER_SIZE;
        svg_load_data.max_requests                = CMX_SVG_MAX_REQUESTS;
        svg_load_data.filename                    = handle_ptr->svg_data->directory;
        graph_svg_decode( &svg_load_data);

        return;
      } else {
        MSG_ERROR("Failed to allocate memory", 0,0,0);
      } /* mem_buffer != NULL */
    } else {
      MSG_ERROR("Failed to allocate memory", 0,0,0);
    } /* svg_data != NULL */
  }

  if(handle_ptr != NULL) {
    if( handle_ptr->svg_data != NULL){
      if( handle_ptr->svg_data->mem_buffer != NULL){
        CMX_FREE( handle_ptr->svg_data->mem_buffer);
      }
      CMX_FREE(handle_ptr->svg_data);
    }
    cmx_free_af_handle(handle_ptr);
  }else {
    cmx_free_handle(handle);
  }
  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data, NULL, 0);
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_set_viewport_processing

DESCRIPTION
  This function sets the size of viewport of SVG Decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_set_viewport_processing
(
  void*                  handle,
  uint32                 width,
  uint32                 height,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
) {
  cmx_af_handle_type*          handle_ptr = (cmx_af_handle_type*)handle;
  uint32                       frame_size;
  uint8*                       frame_buf;
  cmx_client_buf_req_type      buf_req;
  cmx_packet_type*             packet;

  if( handle_ptr != NULL  && handle_ptr->svg_data != NULL){
    if( handle_ptr->svg_data->status == CMX_SVG_STATUS_PLAYING
      ) {
      frame_size = width * height *
        ((handle_ptr->svg_data->color_scheme == CMX_COLOR_SCHEME_RGB565)?2:4);

      if( frame_size != handle_ptr->svg_data->frame_bufsize){
        /* Free the old buffer and Request a new buffer */

        packet = cmx_cmd_get_pkt ();
        if (packet != NULL) {
          packet->hdr.cmd                   = CMX_SVG_CMD_START_PLAY;
          packet->svg_cont.handle           = handle_ptr;
          packet->svg_cont.buffer.handle    = handle_ptr;
          packet->svg_cont.buffer.pending_cmd = NULL;
          packet->svg_cont.buffer.buffer    = NULL;
          packet->svg_cont.request          = TRUE;
        }

        buf_req.buffer_size     = frame_size;
        buf_req.client_data     = &(packet->svg_cont.buffer);
        buf_req.cb_func         = (cmx_client_buf_req_cb_func_ptr_type)
                                     cmx_client_buf_req_cb;
        buf_req.buffer_supplied = FALSE;

        if( handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF){
          handle_ptr->cb_func.image( CMX_CLIENT_BUF_FREE, handle_ptr->client_data,
                     handle_ptr->svg_data->frame_buf,
                     sizeof(handle_ptr->svg_data->frame_buf));

          if(callback_ptr != NULL) {
            handle_ptr->cb_func.image(CMX_CLIENT_BUF_REQ, handle_ptr->client_data,
                                      &buf_req, sizeof(cmx_client_buf_req_type));
          }
        }

        if( !(handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF) ||
             (buf_req.buffer_supplied == FALSE)){
          frame_buf = handle_ptr->svg_data->frame_buf;

          handle_ptr->svg_data->frame_buf = CMX_MALLOC( frame_size);
          if( handle_ptr->svg_data->frame_buf == NULL){
            MSG_ERROR("Failed to allocate memory", 0,0,0);
            callback_ptr( CMX_FAILURE, client_data );
            handle_ptr->svg_data->frame_buf = frame_buf;
            return;
          }
          handle_ptr->svg_data->frame_bufsize = frame_size;
          CMX_FREE( frame_buf);
        }

      }

      graph_svg_set_viewport( handle_ptr->svg_data->svg_handle, width, height);

      if(callback_ptr != NULL) {
        callback_ptr(CMX_SUCCESS, client_data);
      }
      return;
    }
  }

  if(callback_ptr != NULL) {
    callback_ptr(CMX_FAILURE, client_data);
  }
} /* cmx_svg_set_viewport_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_cleanup

DESCRIPTION
  This function frees all resources used by SVG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_svg_cleanup(
  cmx_af_handle_type* handle_ptr
){
#ifdef FEATURE_EFS
  fs_rsp_msg_type       rsp;
#endif

  if( handle_ptr != NULL && handle_ptr->cmd != CMX_CMD_INVALID){
#ifdef FEATURE_EFS
    if(handle_ptr->source_info.source == CMX_SOURCE_EFS) {
      fs_close(handle_ptr->source_info.handle.file, NULL, &rsp);
    }
#endif

    if( handle_ptr->svg_data != NULL){
      if( handle_ptr->svg_data->mem_buffer != NULL){
        CMX_FREE( handle_ptr->svg_data->mem_buffer);
        handle_ptr->svg_data->mem_buffer = NULL;
      }
      if( handle_ptr->svg_data->frame_buf != NULL){
        if(handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF){
          handle_ptr->cb_func.image( CMX_CLIENT_BUF_FREE, handle_ptr->client_data,
                                   handle_ptr->svg_data->frame_buf,
                                   sizeof(handle_ptr->svg_data->frame_buf));
        }else{
          CMX_FREE( handle_ptr->svg_data->frame_buf);
        }
      }
      rex_clr_timer( &handle_ptr->svg_data->frame_timer);
      rex_clr_timer( &handle_ptr->svg_data->actual_time);
      CMX_FREE( handle_ptr->svg_data);
      handle_ptr->svg_data = NULL;
    }
    cmx_free_af_handle(handle_ptr);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_frame_processing

DESCRIPTION
  This function processes Frame commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_svg_frame_processing(
  cmx_af_handle_type*  handle_ptr,
  uint32               frame_time, /*Rendering time of frame */
  boolean              seek,
  cmx_cb_func_ptr_type callback_ptr,
  void*                client_data
){
  svg_frame_data_type    frame_data;
  cmx_svg_data_type*     svg_data;

  if( handle_ptr == NULL || handle_ptr->svg_data == NULL ||
      handle_ptr->cmd == CMX_CMD_INVALID){
    if( callback_ptr != NULL){
      callback_ptr( CMX_FAILURE, client_data);
    }
    return;
  }

  svg_data = handle_ptr->svg_data;
  if( svg_data->status != CMX_SVG_STATUS_PLAYING ){
    if( callback_ptr != NULL){
      callback_ptr( CMX_FAILURE, client_data);
    }
    return;
  }

  rex_clr_timer( &svg_data->frame_timer);

  if( !svg_data->frame_buf ){

    svg_data->frame_time = frame_time;
    svg_data->status     = CMX_SVG_STATUS_PENDING;
    if( callback_ptr != NULL){
      callback_ptr( CMX_FAILURE, client_data);
    }
    return;
  }

  frame_data.frame_buf      = svg_data->frame_buf;
  frame_data.frame_size     = svg_data->frame_bufsize;
  frame_data.seek           = seek;
  if( seek){
    frame_data.time_ms        = svg_data->frame_time ;
  } else{
    frame_data.time_ms        = frame_time - svg_data->frame_time ;
  }
  svg_data->frame_time      = frame_time;
  svg_data->status          = CMX_SVG_STATUS_RENDERING;

  graph_svg_frame_cmd( svg_data->svg_handle, &frame_data);

  if( callback_ptr != NULL){
    callback_ptr( CMX_SUCCESS, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_timer_cb

DESCRIPTION
  This function is a callback for frame timer in CMX controlled mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void cmx_svg_timer_cb( unsigned long handle){
  cmx_af_handle_type*         handle_ptr;
  cmx_packet_type *packet;

  handle_ptr = (cmx_af_handle_type*) handle;

  if( handle_ptr != NULL && handle_ptr->svg_data != NULL &&
      handle_ptr->cmd != CMX_CMD_INVALID){
    /*Valid SVG state */

    rex_clr_timer( &handle_ptr->svg_data->frame_timer);

    switch(handle_ptr->svg_data->status){
    case CMX_SVG_STATUS_PAUSED:
      /* Render frame after another delay time */
      rex_set_timer( &handle_ptr->svg_data->frame_timer,
                     handle_ptr->svg_data->delay);
      return;

    case CMX_SVG_STATUS_PLAYING:
    case CMX_SVG_STATUS_RENDERING:
      packet  = cmx_cmd_get_pkt ();

      if(packet != NULL) {
        uint32 delay = rex_get_timer( &handle_ptr->svg_data->actual_time);

        if( delay == 0){
          packet->hdr.cmd                 = CMX_SVG_CMD_ABORT;
          packet->svg_base.handle         = handle_ptr;
          packet->svg_base.cb_func        = NULL;
          packet->svg_base.client_data    = NULL;

        }else{
          packet->hdr.cmd                         = CMX_SVG_CMD_FRAME;
          packet->svg_frame.handle                = handle_ptr;
          packet->svg_frame.time_ms               =
             handle_ptr->svg_data->frame_time + ( CMX_SVG_MAX_TIME - delay);
          packet->svg_frame.callback_ptr          = NULL;
          packet->svg_frame.client_data           = NULL;

        }
        cmx_cmd (packet);
      }

      rex_clr_timer( &handle_ptr->svg_data->actual_time);
      rex_set_timer( &handle_ptr->svg_data->actual_time, CMX_SVG_MAX_TIME);
      return;

    case CMX_SVG_STATUS_STOPPED:
      /*Clear all timers */
      rex_clr_timer( &handle_ptr->svg_data->actual_time);
    case CMX_SVG_STATUS_PENDING:
      /*Wait for frame buffer from client */
    default:
      /*Do nothing */
      return;
    }

  }

}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_play_processing

DESCRIPTION
  This function requests frames from SVG decoder after all the required callbacks
  are received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_play_processing(
  cmx_af_handle_type*    handle_ptr,
  cmx_svg_cont_packet_type  *svg_cont
){
  cmx_svg_data_type*           svg_data;

  if( handle_ptr != NULL && handle_ptr->svg_data != NULL){

    svg_data = handle_ptr->svg_data;

    if( svg_cont->request == 1){  /* Frame buffer request */
      if(handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF){
        svg_data->frame_buf = svg_cont->buffer.buffer;
      }
    }

    if( handle_ptr->svg_data->frame_buf == NULL &&
        !( handle_ptr->status_mask & CMX_AF_STATUS_MASK_USE_CLIENT_BUF) ){

       handle_ptr->svg_data->frame_buf =
       CMX_MALLOC( handle_ptr->svg_data->frame_bufsize);

      if( !handle_ptr->svg_data->frame_buf){
        if( handle_ptr->cb_func.image != NULL){
          handle_ptr->cb_func.image( CMX_FAILURE, handle_ptr->client_data,  NULL, 0);
        }
        graph_svg_stop( handle_ptr->svg_data->svg_handle);
        cmx_svg_cleanup(handle_ptr);
        return;
      } else {
        MSG_ERROR("Failed to allocate memory", 0,0,0);
      }
    }

    /* buffer available and (Prepare done or Progressive Rendering) */
    if( (svg_data->frame_buf != NULL) &&
        /* (handle_ptr->svg_data->status == CMX_SVG_STATUS_PLAYING) */
        ( (handle_ptr->svg_data->status == CMX_SVG_STATUS_PLAYING) ||
          (handle_ptr->svg_data->progressive == TRUE) )
      ){

      if( svg_data->cmx_svg_decode_mode == CMX_SVG_MODE_CMX ){
        /* Request first frame */
        rex_def_timer_ex( &svg_data->frame_timer, cmx_svg_timer_cb,
                          (unsigned long)handle_ptr);
        rex_def_timer( &svg_data->actual_time, &vs_tcb, 0);
        svg_data->frame_time  = 0;
        rex_set_timer( &handle_ptr->svg_data->actual_time, CMX_SVG_MAX_TIME);
        cmx_svg_frame_processing(handle_ptr, svg_data->frame_time, FALSE, NULL, 0);
      }

    }
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_request_processing

DESCRIPTION
  This function opens handle for external file requests made by SVG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_request_processing(
  void* handle,
    cmx_svg_req_pk_type  *svg_req
){
  cmx_af_handle_type *handle_ptr = (cmx_af_handle_type*) handle;
  cmx_svg_request_type req_data;

#ifdef FEATURE_EFS
  fs_rsp_msg_type       rsp;
  fs_access_option_type open_type = FS_OA_READONLY;
#endif

  memset( &handle_ptr->svg_req, 0, sizeof(cmx_svg_req_type));
  handle_ptr->svg_req.handle                = handle_ptr;
  handle_ptr->svg_req.type                  = svg_req->type;
  memcpy(handle_ptr->svg_req.filename, svg_req->filename,
         FS_FILENAME_MAX_LENGTH_P - 1);
  handle_ptr->svg_req.cb_func               = svg_req->cb_func;
  handle_ptr->svg_req.cb_data               = svg_req->cb_data;
  handle_ptr->svg_req.next_cb               = svg_req->next_cb;

#ifdef FEATURE_EFS

  /* Make sure that don't try to play a directory! */
  fs_nametest(svg_req->filename, FS_TEST_DIR, NULL, &rsp);
  if (rsp.nametest.status == FS_OKAY_S && rsp.nametest.name_found == FALSE) {
    fs_open(svg_req->filename, open_type, NULL, NULL, &rsp);
    if(rsp.open.status == FS_OKAY_S) {

      handle_ptr->svg_req.data.efs.fs_handle  = rsp.open.handle;

      /* Load and store the file size */
      fs_file_size(svg_req->filename, NULL, &rsp);
      handle_ptr->svg_req.data.efs.file_size      = rsp.file_size.size;
      svg_req->cb_func( svg_req->cb_data, handle_ptr, svg_req->filename);
      return;
    }
  }
#endif /* FEATURE_EFS */

  /* Sent request to client */
  handle_ptr->svg_req.data.client.buf_ptr   = svg_req->buf_ptr;
  handle_ptr->svg_req.data.client.num_bytes = svg_req->num_bytes;

  req_data.cb_func = (cmx_svg_request_cb_func_ptr)cmx_svg_req_cb;
  req_data.cb_data  = (void*) &handle_ptr->svg_req;
  req_data.filename = svg_req->filename;
  req_data.num_bytes = strlen( svg_req->filename);
  req_data.more_requests = (svg_req->next_cb == NULL)? FALSE : TRUE;

  handle_ptr->cb_func.image( CMX_SVG_EXT_REQ, handle_ptr->client_data,
                             (void*) &req_data, NULL);

  if( svg_req->next_cb != NULL){
    svg_req->next_cb( svg_req->cb_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_req_data_processing

DESCRIPTION
  This function reads data from files for external file requests made by
  SVG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_req_data_processing(
  void*           handle,
  uint8*          buffer,
  uint32          num_bytes,
  uint32          position
){
  cmx_af_handle_type*  handle_ptr = (cmx_af_handle_type*) handle;

#ifdef FEATURE_EFS
  static fs_rsp_msg_type rsp;

  if( handle_ptr != NULL && handle_ptr->svg_req.handle != NULL &&
      handle_ptr->svg_req.data.efs.fs_handle != NULL){
    /* Request for read data is valid, read from file */
    fs_seek_read(handle_ptr->svg_req.data.efs.fs_handle, FS_SEEK_SET,
               (fs_file_position_type) position, buffer, num_bytes,
               (fs_callback_p_type) NULL, &rsp);

    if( rsp.read.status == FS_OKAY_S || rsp.read.status == FS_EOF_S){
      handle_ptr->svg_req.cb_func( handle_ptr->svg_req.cb_data, handle_ptr,
                                   handle_ptr->svg_req.filename);
      return;
    }
  }
#endif

  if( handle_ptr != NULL && handle_ptr->svg_req.handle != NULL &&
      handle_ptr->svg_req.type == 0){
    handle_ptr->cb_func.image( CMX_FAILURE, handle_ptr->client_data,  NULL, 0);
    graph_svg_stop( handle_ptr->svg_req.handle);
    cmx_svg_cleanup(handle_ptr);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_req_cb_processing

DESCRIPTION
  This function is used as a callback by clients for returning external file
  requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Queues a command in Graph context
===========================================================================*/
void cmx_svg_req_cb_processing(
  void* handle,
  uint8** buffer,            /* Buffer with URL data in one chunk */
  uint32 num_bytes
){

  cmx_svg_req_type* svg_req = (cmx_svg_req_type*)handle;
  cmx_af_handle_type* handle_ptr = (cmx_af_handle_type*)svg_req->handle;

  if( svg_req != NULL){

    if( buffer == NULL || num_bytes == 0){
      if( svg_req->type == 0){
        if( handle_ptr->cb_func.image != NULL){
          handle_ptr->cb_func.image( CMX_FAILURE, handle_ptr->client_data,  NULL, 0);
        }
        graph_svg_stop( handle_ptr->svg_data->svg_handle);
        cmx_svg_cleanup(handle_ptr);
      }else{
        memset( svg_req, 0, sizeof(cmx_svg_req_type));
      }
      return;
    }else{
      svg_req->data.client.buf_ptr = buffer;
      *svg_req->data.client.num_bytes = num_bytes;

      if( svg_req->cb_func != NULL){
        svg_req->cb_func( svg_req->cb_data, handle_ptr, svg_req->filename);
      }
    }
  }

}

#ifdef FEATURE_SVGDOM
/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svgdom_apis

DESCRIPTION
  This function calls the corresponding DOM API in SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svgdom_apis(
  void*           handle,
  cmx_svgdom_api_enum_type type,
  cmx_svgdom_para_type*    data
){
   cmx_packet_type            *packet;
   cmx_af_handle_type         *handle_ptr = (cmx_af_handle_type*)handle;
   cmx_svgdom_strpool_type    *strptr = NULL;
   cmx_svgdom_data_type       dom_cb_data;

   dom_cb_data.api    = type;
   dom_cb_data.error  = CMX_SVGDOM_NOT_FOUND_ERR;
   dom_cb_data.type   = CMX_SVGDOM_DATA_NONE;
   dom_cb_data.num_bytes = 0;


   packet  = cmx_cmd_get_pkt ();

   if(packet != NULL) {

     if( handle_ptr != NULL && handle_ptr->svg_data != NULL && data != NULL){

       packet->hdr.cmd                         = CMX_SVG_CMD_DOM;
       packet->svgdom_data.type                = type;
       packet->svgdom_data.handle              = handle_ptr;
       memcpy( (void*)&packet->svgdom_data.data, (void*)data,
               sizeof( cmx_svgdom_para_type));

        /* Copy strings if any */
       switch(type){

       case CMX_SVGDOM_API_DOC_CREATE:
        strptr = (cmx_svgdom_strpool_type *) (q_get (&cmx_svgdom_free_q));

        if( strptr == NULL){
           cmx_svgdom_free_strings(handle_ptr, type);
           q_put (&cmx_free_q, &packet->hdr.link);
           if(handle_ptr != NULL && handle_ptr->cb_func.image != NULL) {
             handle_ptr->cb_func.image( CMX_SVG_DOM_DATA, handle_ptr->client_data, &dom_cb_data,
                                   sizeof( cmx_svgdom_data_type));
           }
           return;
      }

         (void) q_link (strptr, &strptr->link);
         q_put (&cmx_svgdom_in_use_q, &strptr->link);

         memset( strptr->str, 0, sizeof(uint16)*CMX_SVG_MAX_STRING_LENGTH);
         memcpy( (void*)strptr->str, (void*)data->doc_data.data.URI,
                 sizeof(uint16) * CMX_SVG_MAX_STRING_LENGTH);

         strptr->api           = type;
         strptr->handle        = handle_ptr;
         packet->svgdom_data.data.doc_data.data.URI = strptr->str;
         /* Fall through for name*/

       case CMX_SVGDOM_API_DOC_GETBYID:
       case CMX_SVGDOM_API_DOC_CREATEEVENT:
         strptr = (cmx_svgdom_strpool_type *) (q_get (&cmx_svgdom_free_q));

         if( strptr == NULL){
           cmx_svgdom_free_strings(handle_ptr, type);
           q_put (&cmx_free_q, &packet->hdr.link);
           if(handle_ptr != NULL && handle_ptr->cb_func.image != NULL) {
             handle_ptr->cb_func.image( CMX_SVG_DOM_DATA, handle_ptr->client_data,
                                        &dom_cb_data,
                                        sizeof( cmx_svgdom_data_type));
           }
           return;
         }

         (void) q_link (strptr, &strptr->link);
         q_put (&cmx_svgdom_in_use_q, &strptr->link);

         memset( strptr->str, 0, sizeof(uint16)*CMX_SVG_MAX_STRING_LENGTH);
         memcpy( (void*)strptr->str, (void*)data->doc_data.name,
                 sizeof(uint16) * CMX_SVG_MAX_STRING_LENGTH);

         strptr->api           = type;
         strptr->handle        = handle_ptr;
         packet->svgdom_data.data.doc_data.name   = strptr->str;

         break;

       case CMX_SVGDOM_API_ELEMENT_SETID:
         strptr = (cmx_svgdom_strpool_type *) (q_get (&cmx_svgdom_free_q));
         if( strptr == NULL){
           cmx_svgdom_free_strings(handle_ptr, type);
           q_put (&cmx_free_q, &packet->hdr.link);
           if(handle_ptr != NULL && handle_ptr->cb_func.image != NULL) {
             handle_ptr->cb_func.image( CMX_SVG_DOM_DATA, handle_ptr->client_data, &dom_cb_data,
                                   sizeof( cmx_svgdom_data_type));
           }
           return;
         }

         (void) q_link (strptr, &strptr->link);
         q_put (&cmx_svgdom_in_use_q, &strptr->link);

         memset( strptr->str, 0, sizeof(uint16)*CMX_SVG_MAX_STRING_LENGTH);
         memcpy( (void*)strptr->str, (void*)data->element_data.name,
                 sizeof(uint16) * CMX_SVG_MAX_STRING_LENGTH);

         strptr->api           = type;
         strptr->handle        = handle_ptr;
         packet->svgdom_data.data.element_data.name   = strptr->str;

         break;

       case CMX_SVGDOM_API_TRAIT_SETTRAIT:
         strptr = (cmx_svgdom_strpool_type *) (q_get (&cmx_svgdom_free_q));
         if( strptr == NULL){
           cmx_svgdom_free_strings(handle_ptr, type);
           q_put (&cmx_free_q, &packet->hdr.link);
           if(handle_ptr != NULL && handle_ptr->cb_func.image != NULL) {
             handle_ptr->cb_func.image( CMX_SVG_DOM_DATA, handle_ptr->client_data, &dom_cb_data,
                                   sizeof( cmx_svgdom_data_type));
           }
           return;
    }

         (void) q_link (strptr, &strptr->link);
         q_put (&cmx_svgdom_in_use_q, &strptr->link);

         memset( strptr->str, 0, sizeof(uint16)*CMX_SVG_MAX_STRING_LENGTH);
         memcpy( (void*)strptr->str, (void*)data->element_data.data.string,
                 sizeof(uint16) * CMX_SVG_MAX_STRING_LENGTH);

         strptr->api           = type;
         strptr->handle        = handle_ptr;
         packet->svgdom_data.data.element_data.data.string   = strptr->str;
         /* Fall through */

       case CMX_SVGDOM_API_TRAIT_GETTRAIT:
       case CMX_SVGDOM_API_TRAIT_GETTRAITNS: /* Other fileds not supported yet */
       case CMX_SVGDOM_API_TRAIT_SETTRAITNS:
       case CMX_SVGDOM_API_TRAIT_GETFLOAT:
       case CMX_SVGDOM_API_TRAIT_SETFLOAT:
       case CMX_SVGDOM_API_TRAIT_GETMATRIX:
       case CMX_SVGDOM_API_TRAIT_SETMATRIX:
       case CMX_SVGDOM_API_TRAIT_GETRECT:
       case CMX_SVGDOM_API_TRAIT_SETRECT:
       case CMX_SVGDOM_API_TRAIT_GETPATH:
       case CMX_SVGDOM_API_TRAIT_SETPATH:
       case CMX_SVGDOM_API_TRAIT_GETRGB:
       case CMX_SVGDOM_API_TRAIT_SETRGB:
         strptr = (cmx_svgdom_strpool_type *) (q_get (&cmx_svgdom_free_q));
         if( strptr == NULL){
           cmx_svgdom_free_strings(handle_ptr, type);
           q_put (&cmx_free_q, &packet->hdr.link);
           if(handle_ptr != NULL && handle_ptr->cb_func.image != NULL) {
             handle_ptr->cb_func.image( CMX_SVG_DOM_DATA, handle_ptr->client_data, &dom_cb_data,
                                   sizeof( cmx_svgdom_data_type));
  }
           return;
}

         (void) q_link (strptr, &strptr->link);
         q_put (&cmx_svgdom_in_use_q, &strptr->link);

         memset( strptr->str, 0, sizeof(uint16)*CMX_SVG_MAX_STRING_LENGTH);
         memcpy( (void*)strptr->str, (void*)data->element_data.name,
                 sizeof(uint16) * CMX_SVG_MAX_STRING_LENGTH);

         strptr->api           = type;
         strptr->handle        = handle_ptr;
         packet->svgdom_data.data.element_data.name   = strptr->str;

         break;

       case CMX_SVGDOM_API_EVTTARGET_ADD:
       case CMX_SVGDOM_API_EVTTARGET_RMV:
         strptr = (cmx_svgdom_strpool_type *) (q_get (&cmx_svgdom_free_q));

         if( strptr == NULL){
           cmx_svgdom_free_strings(handle_ptr, type);
           q_put (&cmx_free_q, &packet->hdr.link);
           if(handle_ptr != NULL && handle_ptr->cb_func.image != NULL) {
             handle_ptr->cb_func.image( CMX_SVG_DOM_DATA, handle_ptr->client_data, &dom_cb_data,
                                   sizeof( cmx_svgdom_data_type));
           }
           return;
         }

         (void) q_link (strptr, &strptr->link);
         q_put (&cmx_svgdom_in_use_q, &strptr->link);

         memset( strptr->str, 0, sizeof(uint16)*CMX_SVG_MAX_STRING_LENGTH);
         memcpy( (void*)strptr->str, (void*)data->evttarget_data.type,
                 sizeof(uint16) * CMX_SVG_MAX_STRING_LENGTH);

         strptr->api           = type;
         strptr->handle        = handle_ptr;
         packet->svgdom_data.data.evttarget_data.type   = strptr->str;

         break;

       case CMX_SVGDOM_API_EVENT_INIT:
         strptr = (cmx_svgdom_strpool_type *) (q_get (&cmx_svgdom_free_q));

         if( strptr == NULL){
           cmx_svgdom_free_strings(handle_ptr, type);
           q_put (&cmx_free_q, &packet->hdr.link);
           if(handle_ptr != NULL && handle_ptr->cb_func.image != NULL) {
             handle_ptr->cb_func.image( CMX_SVG_DOM_DATA, handle_ptr->client_data, &dom_cb_data,
                                   sizeof( cmx_svgdom_data_type));
           }
           return;
         }

         (void) q_link (strptr, &strptr->link);
         q_put (&cmx_svgdom_in_use_q, &strptr->link);

         memset( strptr->str, 0, sizeof(uint16)*CMX_SVG_MAX_STRING_LENGTH);
         memcpy( (void*)strptr->str, (void*)data->event_data.evttarget_type,
                 sizeof(uint16) * CMX_SVG_MAX_STRING_LENGTH);

         strptr->api           = type;
         strptr->handle        = handle_ptr;
         packet->svgdom_data.data.event_data.evttarget_type = strptr->str;

         break;

       case CMX_SVGDOM_API_EVTLISTENER_CREATE:
         handle_ptr->svg_data->event_handler = data->evthandler.handler;
         handle_ptr->svg_data->event_handler_data = data->evthandler.client_data;
         packet->svgdom_data.data.evthandler.handler = (cmx_svgdom_handle_event_type)cmx_svg_evthandler_cb;
         packet->svgdom_data.data.evthandler.client_data = handle_ptr;
         break;

       default:
         /* Do nothing */
         break;
       }

       cmx_cmd (packet);
       return;
     }
}

   if(handle_ptr != NULL && handle_ptr->cb_func.image != NULL) {
     handle_ptr->cb_func.image( CMX_SVG_DOM_DATA, handle_ptr->client_data, &dom_cb_data,
                           sizeof( cmx_svgdom_data_type));
   }
}/* cmx_svgdom_apis */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svgdom_processing

DESCRIPTION
  This function issues a DOM command to SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svgdom_processing(
  void* handle,
  cmx_svgdom_api_enum_type type,
  cmx_svgdom_para_type* dom_para
){
   cmx_af_handle_type    *handle_ptr = (cmx_af_handle_type*)handle;
   cmx_svgdom_data_type  dom_cb_data;

   if( handle != NULL && dom_para != NULL){

     graph_svg_dom_apis(handle_ptr->svg_data->svg_handle,
                        (svg_dom_api_enum_type)type,
                        dom_para);
     return;
   }

   if( handle_ptr != NULL && handle_ptr->svg_data != NULL &&
       handle_ptr->cb_func.image != NULL){
     dom_cb_data.api    = type;
     dom_cb_data.error  = CMX_SVGDOM_NOT_FOUND_ERR;
     dom_cb_data.type   = CMX_SVGDOM_DATA_NONE;
     dom_cb_data.num_bytes = 0;

     handle_ptr->cb_func.image( CMX_SVG_DOM_DATA, handle_ptr->client_data, &dom_cb_data,
                                sizeof( cmx_svgdom_data_type));
   }
}

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svgdom_free_strings

DESCRIPTION
  This function issues a DOM command to SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svgdom_free_strings(
  cmx_af_handle_type* handle_ptr,
  cmx_svgdom_api_enum_type api
){
    cmx_svgdom_strpool_type*  strptr;

    while( (strptr = (cmx_svgdom_strpool_type *) q_get (&cmx_svgdom_in_use_q)) != NULL ) {
      if( strptr->api == api && strptr->handle == handle_ptr){
        q_put( &cmx_svgdom_free_q, &strptr->link);
      }
    }
}

#endif /* FEATURE_SVGDOM */
#endif /* FEATURE_SVG */

#ifdef FEATURE_AUDFMT_HYBRID_MODE
/*==========================================================================
 FUNCTION cmx_audfmt_multi_poly_mode_cb_processing

 DESCRPTION
   This function is call back function for the lower layer to pass back the
   status of hybrid mode command

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_af_multi_poly_mode_cb_processing(
  cmx_status_type    status,
  cmx_af_handle_type *handle
) {

  if (handle != NULL) {
    if (handle->cb_func.base != NULL) {
      handle->cb_func.base(status, handle->client_data);
    }
    cmx_free_af_handle(handle);
  } else {
    MSG_ERROR("NULL client data", 0, 0, 0);
  }

}

/*==========================================================================
 FUNCTION cmx_af_multi_get_poly_mode_cb_processing

 DESCRPTION
   This function is call back function for the lower layer to pass back the
   current hybrid mode.

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_af_multi_get_poly_mode_cb_processing(
  cmx_status_type status,
  cmx_audfmt_poly_mode_type mode,
  cmx_af_handle_type *handle
) {

  if (handle != NULL) {
    if (handle->cb_func.base != NULL) {
      if (handle->client_poly_ptr != NULL &&
          status == CMX_SUCCESS) {
        *(handle->client_poly_ptr) = mode;
      }
      handle->cb_func.base(status, handle->client_data);
    }
    cmx_free_af_handle(handle);
  } else {
    MSG_ERROR("NULL client_data", 0, 0, 0);
  }

}

/*==========================================================================
 FUNCTION cmx_audfmt_multi_poly_mode_cb

 DESCRPTION
   This function is call back function for the lower layer to pass back the
   status of hybrid mode command

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_audfmt_multi_poly_mode_cb(
  const void *client_data,
  snd_status_type status
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->audfmt_poly_mode.hdr.cmd = CMX_AF_CMD_POLY_MODE_CB;
    if (status == SND_SUCCESS) {
      packet->audfmt_poly_mode_cb.pm_status = CMX_SUCCESS;
    } else {
      packet->audfmt_poly_mode_cb.pm_status = CMX_FAILURE;
    }
    packet->audfmt_poly_mode_cb.handle = (cmx_af_handle_type*) client_data;
    cmx_cmd (packet);
  } else {
    MSG_ERROR("Out of CMX command packet", 0, 0, 0);
  }

}

/*==========================================================================
 FUNCTION cmx_audfmt_multi_get_poly_mode_cb

 DESCRPTION
   This function is call back function for the lower layer to pass back the
   current hybrid mode.

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_audfmt_multi_get_poly_mode_cb(
  const void *client_data,
  snd_status_type status,
  snd_audfmt_poly_mode_type mode
) {
  cmx_packet_type *packet;
  boolean valid_mode = TRUE;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->audfmt_poly_mode.hdr.cmd = CMX_AF_CMD_GET_POLY_MODE_CB;

    switch(mode) {
      case SND_AUDFMT_POLY_MODE_0:
        packet->audfmt_get_pm_cb.mode = CMX_AUDFMT_POLY_MODE_0;
        break;
      case SND_AUDFMT_POLY_MODE_1:
        packet->audfmt_get_pm_cb.mode = CMX_AUDFMT_POLY_MODE_1;
        break;
      case SND_AUDFMT_POLY_MODE_2:
        packet->audfmt_get_pm_cb.mode = CMX_AUDFMT_POLY_MODE_2;
        break;
      case SND_AUDFMT_POLY_MODE_3:
        packet->audfmt_get_pm_cb.mode = CMX_AUDFMT_POLY_MODE_3;
        break;
      case SND_AUDFMT_POLY_MODE_4:
        packet->audfmt_get_pm_cb.mode = CMX_AUDFMT_POLY_MODE_4;
        break;
      case SND_AUDFMT_POLY_MODE_5:
        packet->audfmt_get_pm_cb.mode = CMX_AUDFMT_POLY_MODE_5;
        break;
      case SND_AUDFMT_POLY_MODE_6:
        packet->audfmt_get_pm_cb.mode = CMX_AUDFMT_POLY_MODE_6;
        break;
      case SND_AUDFMT_POLY_MODE_7:
        packet->audfmt_get_pm_cb.mode = CMX_AUDFMT_POLY_MODE_7;
        break;
      default:
        valid_mode = FALSE;
        break;
    }

    if (status == SND_SUCCESS && valid_mode == TRUE) {
      packet->audfmt_get_pm_cb.pm_status = CMX_SUCCESS;
    } else {
      packet->audfmt_get_pm_cb.pm_status = CMX_FAILURE;
    }
    packet->audfmt_get_pm_cb.handle = (cmx_af_handle_type*) client_data;
    cmx_cmd (packet);
  } else {
    MSG_ERROR("Out of CMX command packet", 0, 0, 0);
  }

}

/*==========================================================================
 FUNCTION cmx_audfmt_multi_poly_mode

 DESCRPTION
   This function sets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_audfmt_multi_poly_mode(
  cmx_audfmt_poly_mode_type mode,
  cmx_cb_func_ptr_type      cb_ptr,
  void                      *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->audfmt_poly_mode.hdr.cmd = CMX_AF_CMD_SET_POLY_MODE;
    packet->audfmt_poly_mode.mode = mode;
    packet->audfmt_poly_mode.cb_func = cb_ptr;
    packet->audfmt_poly_mode.client_data = client_data;
    cmx_cmd (packet);
  } else if(cb_ptr != NULL) {
    cb_ptr(CMX_FAILURE, client_data);
  }

}

/*==========================================================================
 FUNCTION cmx_audfmt_get_multi_poly_mode

 DESCRPTION
   This function gets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_audfmt_get_multi_poly_mode(
  cmx_audfmt_poly_mode_type *mode,
  cmx_cb_func_ptr_type      cb_ptr,
  void                      *client_data
) {
  cmx_packet_type *packet;

  packet = cmx_cmd_get_pkt ();

  if (packet != NULL) {
    packet->audfmt_get_poly_mode.hdr.cmd = CMX_AF_CMD_GET_POLY_MODE;
    packet->audfmt_get_poly_mode.mode = mode;
    packet->audfmt_get_poly_mode.cb_func = cb_ptr;
    packet->audfmt_get_poly_mode.client_data = client_data;
    cmx_cmd (packet);
  } else if(cb_ptr != NULL) {
    cb_ptr(CMX_FAILURE, client_data);
  }

}

/*==========================================================================
 FUNCTION cmx_af_multi_poly_mode_processing

 DESCRPTION
   This function sets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_af_multi_poly_mode_processing (
  cmx_audfmt_poly_mode_type mode,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
) {
  cmx_af_handle_type *handle = NULL;
  snd_audfmt_poly_mode_type trans_mode = SND_AUDFMT_POLY_MODE_0;

  if (mode >= CMX_AUDFMT_POLY_MODE_0 &&
      mode <= CMX_AUDFMT_POLY_MODE_7) {

    handle = cmx_get_af_handle();

    if (handle != NULL) {
      handle->cb_func.base = cb_func;
      handle->client_data = client_data;
      switch(mode){
        case CMX_AUDFMT_POLY_MODE_0:
          trans_mode = SND_AUDFMT_POLY_MODE_0;
          break;
        case CMX_AUDFMT_POLY_MODE_1:
          trans_mode = SND_AUDFMT_POLY_MODE_1;
          break;
        case CMX_AUDFMT_POLY_MODE_2:
          trans_mode = SND_AUDFMT_POLY_MODE_2;
          break;
        case CMX_AUDFMT_POLY_MODE_3:
          trans_mode = SND_AUDFMT_POLY_MODE_3;
          break;
        case CMX_AUDFMT_POLY_MODE_4:
          trans_mode = SND_AUDFMT_POLY_MODE_4;
          break;
        case CMX_AUDFMT_POLY_MODE_5:
          trans_mode = SND_AUDFMT_POLY_MODE_5;
          break;
        case CMX_AUDFMT_POLY_MODE_6:
          trans_mode = SND_AUDFMT_POLY_MODE_6;
          break;
        case CMX_AUDFMT_POLY_MODE_7:
          trans_mode = SND_AUDFMT_POLY_MODE_7;
          break;
        default:
          break;
      }
      snd_multi_set_poly_mode(trans_mode,cmx_audfmt_multi_poly_mode_cb,
                              (const void*) handle);
    } else if (cb_func != NULL) {
      cb_func(CMX_FAILURE, client_data);
    }

  } else if (cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

}

/*==========================================================================
 FUNCTION cmx_af_get_multi_poly_mode_processing

 DESCRPTION
   This function gets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_af_get_multi_poly_mode_processing (
  cmx_audfmt_poly_mode_type *mode,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
) {
  cmx_af_handle_type *handle;

  if (mode != NULL) {
    handle = cmx_get_af_handle();

    if (handle != NULL) {
      handle->cb_func.base    = cb_func;
      handle->client_poly_ptr = mode;
      handle->client_data     = client_data;
      snd_multi_get_poly_mode(cmx_audfmt_multi_get_poly_mode_cb,
                              (const void*) handle);
    } else if (cb_func != NULL) {
      cb_func(CMX_FAILURE, client_data);
    }
  } else if (cb_func != NULL) {
    cb_func(CMX_FAILURE, client_data);
  }

}

#endif /* FEATURE_AUDFMT_HYBRID_MODE */

#ifdef FEATURE_AUDIO_PCR_SUPPORT
/*==========================================================================
 FUNCTION cmx_af_adjust_sample_processing

 DESCRPTION
   This function either Add/Drop a Sample

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
==========================================================================*/
void cmx_af_adjust_sample_processing(
  boolean                   add_sample,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
) {
    if((cmx_af_active == CMX_AF_PLAY) ||
       (cmx_af_active == CMX_AF_PLAY_MULTI)) {
      snd_audfmt_adjust_sample(SND_PRIO_LOW, add_sample, NULL, NULL );
    }

    if(cb_func != NULL) {
      if((cmx_af_active == CMX_AF_PLAY) ||
         (cmx_af_active == CMX_AF_PLAY_MULTI)) {
        cb_func(CMX_SUCCESS, client_data);
      } else {
        cb_func(CMX_FAILURE, client_data);
      }
    }
}
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*==========================================================================
 FUNCTION cmx_af_codec_info_update_processing

 DESCRPTION
   This function updates client of cmx with new
   codec information.

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
==========================================================================*/
void cmx_af_codec_info_update_processing(
  cmx_af_codec_info_type *codec_info,
  cmx_af_handle_type     *handle
) {
  if ((codec_info != NULL) &&
      (handle != NULL) &&
      (handle->codec_update_cb != NULL)) {
    MSG_MED("cmx_af_codec_info_update_processing", 0, 0, 0);
    handle->codec_update_cb(*codec_info, handle->client_data);
  }
}

/*==========================================================================
 FUNCTION cmx_af_dual_mono_mode_processing

 DESCRPTION
   This function sets dual mono channel mode

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
==========================================================================*/
void cmx_af_dual_mono_mode_processing(
  cmx_af_dual_mono_mode_enum_type dual_mono_mode,
  cmx_cb_func_ptr_type            cb_func,
  void                            *client_data
) {
  snd_af_dual_mono_mode_enum_type snd_dual_mono_mode;

  switch(dual_mono_mode) {
    case CMX_AF_DUAL_MONO_MODE_FL_FR:
      snd_dual_mono_mode = SND_AF_DUAL_MONO_MODE_FL_FR;
      break;
    case CMX_AF_DUAL_MONO_MODE_SL_SR:
      snd_dual_mono_mode = SND_AF_DUAL_MONO_MODE_SL_SR;
      break;
    case CMX_AF_DUAL_MONO_MODE_SL_FR:
      snd_dual_mono_mode = SND_AF_DUAL_MONO_MODE_SL_FR;
      break;
    case CMX_AF_DUAL_MONO_MODE_FL_SR:
    default:
      snd_dual_mono_mode = SND_AF_DUAL_MONO_MODE_FL_SR;
      break;
  }
  snd_audfmt_dual_mono_mode(SND_PRIO_LOW,
                            snd_dual_mono_mode,
                            NULL, NULL );

  if (cb_func != NULL) {
    cb_func( CMX_SUCCESS, client_data);
  }
}

/*==========================================================================
 FUNCTION cmx_af_sbr_mode_processing

 DESCRPTION
   This function sets sbr mode

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
==========================================================================*/
void cmx_af_sbr_mode_processing(
  cmx_af_sbr_mode_enum_type  sbr_mode,
  cmx_cb_func_ptr_type       cb_func,
  void                       *client_data
) {
  snd_af_sbr_mode_enum_type snd_sbr_mode;

  if (sbr_mode == CMX_AF_SBR_MODE_ENABLE) {
    snd_sbr_mode = AUDIODEF_AF_SBR_MODE_ENABLE;
  } else {
    snd_sbr_mode = AUDIODEF_AF_SBR_MODE_DISABLE;
  }
  snd_audfmt_sbr_mode(SND_PRIO_LOW,
                      snd_sbr_mode,
                      NULL, NULL );

  if (cb_func != NULL) {
    cb_func( CMX_SUCCESS, client_data);
  }
}
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_CMX_SCREEN_DIMENSIONS

/*==========================================================================
 FUNCTION cmx_audfmt_set_screen_size

 DESCRPTION
   This function supports SVG Scaling
 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
==========================================================================*/
void cmx_audfmt_set_screen_size (
  uint32                    width,
  uint32                    height
) {
  cmx_af_screen_width  = width;
  cmx_af_screen_height = height;
}
#endif /* FEATURE_CMX_SCREEN_DIMENSIONS */
#endif /* FEATURE_WEBAUDIO */

#endif /* FEATURE_CMX */
