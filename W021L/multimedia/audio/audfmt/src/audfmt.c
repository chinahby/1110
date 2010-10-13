/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Audio Formats Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 A U D I O   F O R M A T s   S E R V I C E S

GENERAL DESCRIPTION
  These functions implement a midi/WebAudio player.

EXTERNALIZED FUNCTIONS
  audfmt_do_play_cb
    Does a playback callback with the indicated status.
  audfmt_reset
    Resets the audfmt parser state.
  audfmt_set_tempo
    Sets the playback tempo to the indicated tempo.
  audfmt_tempo_adjust_up
    Calculates the input time adjusted up for the tempo.
  audfmt_tempo_adjust_down
    Calculates the input time adjusted down for the tempo.
  audfmt_set_tune
    Sets the tune factor for the current playback.
  audfmt_get_data_cb_req
    Handles the get/read data callback.
  audfmt_parse_head
    Parses the head of a midi/WebAudio file.
  audfmt_age
    Ages the specified midi/WebAudio file by a number of milliseconds.
  audfmt_parse
   Parses one event of the indicated midi/WebAudio file.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2000 - 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/multimedia/audio/audfmt/src/audfmt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/10   sk      Modified the code to add div by zero check in 
                   audfmt_calc_routine().
10/23/09    kk      Fixed issues related to GET_SPECS & GET_TIME callback 
                    not returning failure for some corrupt midi files.
07/20/09   kk      Removed Sending CMX_STARTED After pause and rewind 
                   operation in midi playback.
06/16/09   hks     in audfmt_reset_sequence function while loop condition was wrong due to 
                   which in Sequence Playback while stopping issues were coming (only first 
                   sequence was stopped). now condition is updated.
05/28/09   rk      MMF file EOS is properly taken care
03/31/09   gs      For cmx5p0, removed redundant channel pitch,vol,pan during
                   seek, to reduce the number of cmds sent to dsp
03/24/09   kd      Ported SMAF related chagnes from release branch to mainline. 
03/08/09   sk      Modified the code to set loop_type_wave_depth = 0x20 if 
                   loop length is 0. in wave control.
                   Set the wavelength value depends up on bits_per_sample 
                   as same as CMX 4.4
02/09/09   bk      Modified audfmt_parse_head, audfmt_get_sequence_num etc for
                   removing Lint Errors.
02/03/09   kd      Featurized the code for setting flw_key to wave_id for CMX4.x only.
01/30/09   kd      Fixed volume differences by taking TL value into 
                   consideration while playing the smaf clips containing
                   ROM DLS waveforms. 
01/29/09   kd      Fixed CR# 170412 for smaf playback issues with 
                   rom dls instruments with different TL than the predefined TL 
                   values of the above instruments.
01/19/09   bk      Added Support for 32 polyphony standard midi format.
                   changed maximum number of midi voices to 32 for std midi.
01/16/09  kd/vsud  Modify the code for CMX 5.0 requirements and also 
                   fixed CR-167820.  
12/11/08  ry/ss/sk Modify the code to set the Hydrid mode to HQ always if 
                   FEATURE_AUDIO_CMX_5P0 is defined.
11/18/08   vsud    Handle CC10/SYS-EX Pan properly.
11/17/08   vsud    Handle discrete panning values while play back.
11/14/08   vsud    CR 162672 ,162673,163789 ,162674 ,162677 ,162678,162680,163002.
11/11/08   vsud    corrected the merge error & compilation issue for CMX 5_0 SMAF.
10/11/08    ry     Ported the fix of CR 138929 - defer activating adec chan playback
                   if DSP is not ready to accept chan vol or pan cmds. audfmt_parser()
                   sets midi_status and time_remain variables so adec channel
                   activatization is retried immediately.
10/02/08    vsud   Fixed CR# 157201 for SMAF file play back issues.
06/25/08    kd     Fixed CR# 137293 add support for playback of offset binary pcm data.
06/23/08    ry     Added SMAF support for LED/VIB event synchronization.
02/28/08   wfd     Integrate ARM changes for DLS CMX 5.0 support.
12/20/07    sk     Modified code to have conditional check in tbyte2 does not 
                   cause read outside the array boundaries. parse_smaf_hp_cmd()
                   CR 113693 
11/05/07    sk     Udpated the changes in audfmt_parse() to send 
                   SND_CMX_AF_STARTED status to CMX layer.when the parser 
                   is not set to init done flag. 
                   CR 120253,124336,124337,120853. 
09/25/07    st     Updated SMAF DLS out-of-range drum mapping for each type of
                   DLS lookup table, CR120902.
08/21/07    pl     Merged CR 117285 fix CMX: APP Audfmt.c crash at line 05045 observed 
                   during repositioning of PMD+ADPCM clip.
                   Modified audfmt_calc_routine returns AUDFMT_MAX_32BIT when overflow 
                   and audfmt_fforward_single() caps seek time at AUDFMT_MAX_32_BIT
06/12/07   pl/sk   Modified qsynth output sample rate to 48KHz for 65nm or 
                   newer target. 
05/31/07    st     Modified code to handle the invalid octave shift command 
                   present in SMAF content, CR 119568;Modified code to support
                   additional SMAF master volume message ID spider bug 4012.   
05/10/07    sk     Added support for Phrase L2 and updated support for Phrase
                   L1; modified code to support a special entry in the FM DLS
                   2op tables, modified code to remap SMAF drums outside of
                   GM drum range; added support for SMAF HP note 
                   velocity command.
04/12/07  st/ss    Modified code to calculate time remaining until next event
                   based on time already elapsed, CR113412.
03/30/07    sk     Added support for handling master volume command in 
                   SMAF files CR 111604
03/05/07    ss     Critical Lint Error fix.Typecast to uint32 in function
                   DescaleAbsoluteTimeL9().
02/05/07    st     Removed use of strncpy() function.
01/22/07   sk      Prevent performing seeking operation before DSP resource is 
                   acquired.CR 95597
01/18/07    ss     Modified the table values of audfmt_prog_def_default and 
                   ModLFO changes in getLfoAegDepthIndex()
01/18/07    st     Modified code to parse through SMAF OPDA chunk properly,
                   CR110034.
01/12/07    st     Modified code to perform AEG depth mapping for SMAF PCM
                   DLS (spider bug #3152); modified code to ensure array
                   overrun does not occur in audfmt_get_track_offset(),
                   CR109152; modified code to extend playback duration for
                   SMAF HP notes that expire beyond the end of track, CR108685.
12/11/06    pl     Modified the weight of Attack Rate parameter from 67 to 120 in 
                   the SMAF FM DLS mapping algorithm.  
11/09/06    ss     Modified code in audfmt sequence functions to fix CR 103058
10/18/06    st     Modified code to fix SMAF Dch chunk parsing in
                   smaf_do_opda(), CR102668.
09/13/06    lg     Fixed a merge issue in do_cmid_extension
09/08/06    pl     Modified audfmt_get_sample_rate() to return 44K instead 
                   of 441K when FEATURE_AUDIO_44K_SAMPLE_RATE is defined.
08/31/06    st     Modified code to reset state properly when get_spec
                   operation fails in audfmt_header_do_failure(), CR100385.
08/08/06   lg/sk   Modified FEATURE_DLS implemntations in parse_midi_cmd(),
                   audfmt_dls_process_insh() for XMF+DLS drum instruments
                   should be supported on channel 10 (CR88396)
07/29/06    pl     Modified for ping-pong buffer scheme.
06/29/06    st     Modified code to abort header parsing when SMAF chunk
                   error occurs, CR96648.
06/15/06    st     Modified code to use correct program number in MFi
                   device control comparisons, CR95611.
06/09/06    st     Added support for sound effects bank for MFi file format.
06/02/06    st     Added support to process wave data chunk after wsmp chunk
                   is found in DLS parser, CR88398.
06/01/06    sk     Modified the funtion getTune()and audfmt_dls_prepare_wavedata()
                   to fix CR 89756.
06/01/06    st     Modified code to check for PPQN value of zero when parsing
                   MIDI header, CR94592.
04/18/06    st     Updated calls to audfadec_wa_reset() to indicate when
                   main file playback is actually ending, CR90915.
03/01/06    lg     Added support for SMAF meta data in AUDIO_SPEC callback
02/27/06    st     Modified code to properly handle waveform looping for SMAF
                   PCM DLS, CR88405.
02/10/06   pk      Added threshold for dls error value of FM DLS 4op 
                   and 2op drum mapping to avoid wrong drum selection
02/01/06   pk      Fine tuned the SMAF FM DLS mapping algorithm by scaling the 
                   weights to avoid precission loss. Added smaf_is_valid_fm_drum
                   function to check for PCM DRUM by verifying the note number 
                   in the SMAF DLS
01/30/06    pl     Added support to provide a way to select hybrid mode 
                   for multisequencer mode and MIDI device.
01/18/06    pk     Added code to select instruments for SMAF files based on
                   variance of FM DLS information from expected values.
01/13/06    lg     Modified code to end MFI normally even if EndOfTrack is missing.
                   Fix for CR#85684.
01/04/06    lg     Modified code to check for track_end before reading data 
                   Fix for CR#85588.
12/22/05   lg/sk   Modified code to ignore duplicate Jump Ids in MFI.(CR#85509) 
11/18/05    st     Modified code to return an estimated elapsed time with
                   PAUSE status, CR82255; modified code to handle seek commands
                   properly when they are received before
                   audfmt_finish_header().
                   Modified code to zero pad DLS waveform properly in
                   audfmt_dls_waveform_to_signed(); modified code to call
                   getPegDepthIndex() at the end of processing region.
11/11/05    st     Modified code for lint updates.
10/21/05    st     Modified code to featurize audfmt_preparse() properly;
                   modified code to initialize note_off_buffer in
                   parse_midi_head().
10/18/05    st     Modified code to update DLS code: updated default value of
                   lfo1_freq and lfo2_freq to 0x5e; updated the LFO freq
                   support to 0.1Hz to 20Hz; changed getPegDepthIndex() to
                   use simpler method in getting the index; added
                   getLfoPegDepthIndex(); updated code to support art2 chunk.
                   Modified code to check for correct type when calling
                   audmain_get_async_data().
09/18/05    pl     Modified code to return sampling rate for game audio synth.
08/18/05    st     Modified code to featurize audfmt_get_track_offset()
                   properly; modified code to check for overflow situation in
                   smaf_do_track(), CR72319; modified code to fix algorithm
                   for freeing DLS drum resources, CR73099.
08/12/05    lg     Fixed featurization in audfmt_get_track_offset.
07/22/05    lg     Added SMAF fixes: Removed unpacking of waveforms.
                   Added fix for holding decay when sustain rate and sustain level are high.
                   Added fixes introduced by start and stop offset support.
                   Added fixes for waveform looping.
                   Added support for using HQ banks for FM16 files.
07/19/05    pl     Added the support for setting the error status at the point 
                   where the error is encountered. The error status would 
                   eventually be relayed back to the upper layer to provide
                   more information for the aborted playback.
07/08/05    pl     Modified audfmt_send_spec() to set supported operation bit 
                   mask.
06/15/05    st     Modified code to support ANIM chunk and animation chunk
                   SVG and SAF frame commands.
05/31/05    st     Modified code to implement a retry counter for MIDI command
                   queue and fail if excessive retries attempted with no
                   success, CR 63195.
05/25/05    st     Modified code to add support for start and end offsets for
                   SMAF MA3/MA5 files; modified code to support SMAF HP octave
                   shift command of +4.
05/08/05    st     Modified code to set proper frame size for MFi ADPCM clips;
                   modified code to not update tempo for MFi relative tempo
                   command if seeking; modified code to reset repeat timer for
                   rewind operation; modified code to end seeking if no seek
                   time is remaining; modified code to update DLS tuning
                   routines.
04/06/05    st     Modified code to adjust ADEC volume in reference to master
                   volume setting for MFi playback; modified code to properly
                   skip over inactive cuepoint tracks; modified code to not
                   update tempo setting in command parser while seeking.
04/02/05    st     Modified code to end search for poly command if first
                   track ends.
03/22/05    st     Modified code to properly reset flag in smaf_do_mtsu();
                   modified code to fix comment in getLfoFreqIndex().
03/21/05    st     Modified code to use expression controller for MFi channel
                   volume commands; modified code to use correct channel
                   for MFi note off commands.
03/10/05    st     Added support for MFi CMX extension commands: WAV commands,
                   polyphony mode, device controls; modified code to use new
                   ADEC status structure instead of adec_mask; modified code
                   to set initial bank status correctly for MFi playback.
02/22/05    st     Added handyphone (ATS-MA2 1.3.1) and MA5 (ATS-MA5 1.3.3.14)
                   SMAF FM DLS lookup tables; added API to require client to
                   enable MFi processing; added MFi featurization; modified
                   code to set channel 9 bank properly for MA2 files; modified
                   code to ignore unsupported handyphone commands.
02/08/05    st     Fixed featurization in audfmt_end_sequence().
02/08/05    st     Modified code to fix dls_config typos in
                   audfmt_finish_header().
02/05/05    st     Modified code to support HQ DLS; added support for DLS
                   file processing; modified code to parse and load DLS chunk
                   in CMX files; modified audfmt_get_sample_rate() to find
                   an active sequence; modified code to fail header parsing
                   if a NULL or empty buffer is returned; modified code to
                   reset status for calc_time failure while parsing header;
                   modified code to support functions on files that start at
                   an offset; removed FEATURE_SAF featurization; modified
                   audfmt_restart() to keep MULTISEQ status.
01/25/05    st     Modified code to keep a count of the number of track
                   buffers to skip as it may exceed one, CR55197.
01/11/05    st     Added support for playback of MFi format; modified code to
                   abort playback if a NULL or empty buffer is returned;
                   modified code to update resume time properly if seeking into
                   the last command that is long in duration, CR45899; modified
                   code to adjust the remaining tick properly after seeking.
01/06/05    aw     Modified code to remove the buffer contents to AAC module.
12/21/04    st     Modified fix to account for tempo changes at time zero.
12/14/04    st     Modified code to adjust for tempo change at time zero;
                   modified code to remove check for illegal MIDI commands,
                   parser now simply rails illegal values to 0x7f.
11/16/04    st     Modified code to not do stored callbacks while seeking;
                   modified code to set SMAF playback to HQ mode for FM-16.
11/01/04    st     Modified code to properly handle SP-MIDI files for the
                   various playback modes; modified code to allow hybrid
                   mode to be set for MIDI playback; modified code to support
                   pitch bend commands for SMAF MA2 files.
10/15/04    st     Restored old behavior to skip silence for SMAF files with
                   graphics chunk.
09/17/04    st     Modified code to removed HQ setting for MA5-FM16 files.
09/13/04    st     Modified code to add support for SMAF FM DLS files; modified
                   code to recognize FM16 MA5 SMAF files; modified code to
                   reset RPN setting after issuing an RPN command; modified
                   code to exit main parse loop upon error.
08/19/04    st     Modified code to not do silence skip for SMAF files.
08/17/04    st     Modified code to initialize calc_ms_of and calc_tick_of
                   for SMAF phrase playback.
08/11/04    st     Modified code to not do silence skip for SMAF files with
                   graphics chunk; modified code to use new separated command
                   buffers for each sequence; modified code to update tempo
                   setting in main parse routine in case seek/calc time needs
                   to update their settings; modified code to reset state
                   parameters properly after a get_time() function.
07/30/04    st     Modified code to only remove note on from command list
                   for MIDI playback while seeking.
07/02/04    st     Modified code to set aeg_decay_time to decay_ratio for DLS
                   instrument def.
                   Modified code to support YADPCM clips being played in 3D
                   mode; modified code to start aging properly; modified code
                   to not start WAV commands until properly initialized;
                   modified code to do pause and volume pending in the correct
                   places; added support for other DLS commands in SMAF files;
                   modified code to distinguish ADEC note off commands from
                   regular note off commands.
05/19/04    lg     Modified code to not write Midi reset and note off commands 
                   to snd_array's audfmt_buf if playing Animated-AAC ringers.
05/12/04    st     Modified code to support SMAF phrase playback; updated
                   SMAF parser to recognize default bank on channel 9 as DRUM;
                   modified code to skip initial silence in SMAF and MIDI files;
                   minor bug fixes.
04/27/04    aw     Modified code to check buffer size in audfmt_get_final_cmd.
04/23/04    lg     Added support for safo subchunk in CMIDI file, for dynamic
                   SAF object buffer size.
04/22/04    lg     Modified code to support Animated AAC Ringers.
04/16/04    st     Modified code to support animation positioning and control;
                   modified code to return proper status for repeat and jump;
                   minor bug fixes.
03/22/04    st     Modified code to implement delayed pause and volume settings
                   if external commands fail; modified code to not perform
                   external commands if simply doing a calc_time function;
                   modified code to support get_specs function; updated code to
                   use midi_status2 parameter.
                   Modified code to implement a global command queue to be 
                   shared by all sequences and executed immediately; updated
                   SMAF LED and VIB control code; removed functions that are
                   no longer needed; updated pause, pan, volume, tuning
                   processing for multiseq; modified code to do seek pause
                   functionality; modified code to disqualify sequences from
                   getting cb_store for more consistent behaviour; modified
                   code to restart loop counter properly; minor bug fixes.
                   Modified code to remove reference to SND_CMX_AF_WA_PLAY_DONE
                   and SND_CMX_AF_WA_FAILURE callbacks.
03/04/03    st     Modified MIDI parser to not reset running status on META
                   events; modified code to look ahead and skip "empty" 
                   commands when parsing MIDI files; Implement better error 
                   checking for invalid MIDI files; modified code to return 
                   correct location for smaf graphics track.
02/05/04    aw     Disable yadpcm adec volume setting. Reset cb_store if parse
                   header failed.
01/29/04    aw     Modified code to reset the jump_ptr instead of return DONE
                   for infinite repeat in calc time.
01/27/04    aw     Return status DONE for infinite repeat in calc time. 
                   Assigned the callback structure to midi control if it is 
                   available even the header information does not specified.
01/27/04    lg     Fixed a bug in calculating wav_chan_vol in set_vol_sequence
01/26/04    lg     Modified code to support tune & pan adjustments; pause, 
                   volume & stop functionality. Added code for sending tick 
                   update for each sequence.
01/21/04  lg/aw    Added support for SMAF LED/Vibrator event, return 
                   graphics track chunk information in a SMAF file.
                   Set adec volume base on the velocity of the note.
12/01/03    sg     Modified to specify sign of the PCM to playback.
11/20/03    aw     Removed unsupported old banks. Modified code to return
                   different output sampling rate base on hybrid mode value.
11/17/03    aw     Modified code to check DLS sample size. Do bank change to
                   BANK_0 for Webaudio file. Added SMAF file mode.
10/16/03  aw/st    Modified code to support SMAF and Multi-sequences.
07/17/03    st     Modified code to check if buffer is valid before adding
                   to free buffer queue; CR33138.
05/14/03    st     Modified code to not send bank change commands for MIDI
                   files.
05/13/03    st     Modified code to support setting drum bank on any channel.
04/30/03    st     Added new language support for WebAudio files; added code
                   to skip SAF positioning bytes.
03/10/03    st     Added support for SP-MIDI format; modified code to verify
                   that synth_mvol and adec_mvol commands have been executed
                   before clearing flag; fixed bug in time calculations where
                   extra time is added at the end.
11/14/02    st     Added support for cmid and midi master volume commands.
09/25/02  aw/st    Added support for wave subchunks; added support to playback
                   ADPCM wave data in webaudio file.
09/19/02    st     Modified code to support pcpi and cnts subchunks; modified
                   code to issue wave volume and panning command to the DSP at
                   the correct times; added checks for valid number of cmidi
                   tracks; added code to verify the length of a cmidi header;
                   modified code to do the audio spec callback for cmidi
                   playback at the correct place; fixed fine pitchbend command;
                   modified code to issue all controllers reset command for a
                   cmidi reset command; modified code to fix bug with multiple
                   track cuepoint playback; modified code to only accept initial
                   jump commands for each jump ID.
08/10/02    st     Added code to set adec mode for MIDI/WebAudio playback;
                   modified code to reset WAV playback on a cue point and jump
                   point.
08/07/02    st     Fixed ordering of pitchbend command in do_cmid_pitch_bend();
                   added support for setting bank values for older CMX files;
                   modified code to change lower end range of PB range command
                   to zero.
07/25/02    st     Added support for exsn, exsa, exsb and exsc subchunks; added
                   support for new pitch bend and pitch bend range command;
                   added support for JPEG in WebAudio files.
06/13/02    st     Modified code to change sampling rate of MIDI image to 32kHz;
                   modified code to skip extension messages for calc time of
                   WebAudio files; modified code to not store WAV command if
                   prev flag is set in the command; modified code to return
                   upon failure in do_cmid_extension().
06/11/02    sm     Include "qw.h".
03/21/02    st     Modified code to support reduced MIDI image.
03/12/02    st     Modified code to support CMX3.0; obsoleted
                   FEATURE_QSYNTH_ADRC going forward.
02/06/02    st     Modified code to only clear the stored callbacks when a
                   rewind status is received; modified code to account for
                   the repeat timer while seeking.
01/08/02    st     Modified code to clear WebAudio command buffer during rewind
                   and fast_forward operation.
11/10/01    st     Modified code to delay VIBE commands until the proper time;
                   added check for an invalid situation and return error
                   condition.
10/08/01    st     Modified code to verify subchunk length for WebAudio files;
                   modified code to verify cue point and jump commands occur
                   on master track; modified code to check for jump destination
                   before jumping; modified code to handle jumping back on
                   a since ended track.
10/01/01    st     Modified code to support using multiple WAV channels;
                   modified code to support a picture clear and picture
                   new command that occurs at the same time.
09/04/01    st     Added proper featurization.
09/04/01    st     Modified code to support bank change commands for drum
                   channel.
09/03/01    st     Modified code to move all the audio decoder code to audfadec
                   module; modified code to add checks for NULL callback
                   pointers; modified audfmt_parse() to handle repeat timer
                   properly.
07/12/01    st     Modified code to return zero time if we reach the end of
                   a file in audfmt_do_seek().
07/04/01    st     Modified code to handle storing multiple text append
                   commands.
06/28/01    st     Modified code to not send back the done status until
                   the delta time has elapsed.
06/20/01    st     Modified code to remove check for length in 
                   audfmt_get_data_cb_req(); added proper featurization.
04/25/01    st     Modified code to not reset the pause flag during rewind.
04/04/01    st     Modified code to properly pass back speed parameter in
                   PIC_ENABLE, PIC_DISABLE and PIC_CLEAR callbacks.
                   Modified code in do_cmid_extension() to not store WAV 
                   commands if the WAV command is not the first one of the
                   WAV; added proper featurization; modified code to reset
                   adec buffer management if sending a reset commmand to client.
                   Modified code in do_cmid_extension() to do track skipping
                   in proper place; modified code in do_cmid_statusB() to 
                   guard against setting the WAVE volume beyond maximum.
                   Modified functionality of audfmt_set_reset_cmd(); added
                   audfmt_get_note_off_cmd(); added length parameter
                   to audfmt_get_data_cb_req(); added support for playing QCP
                   files to the direct MIDI device output interface; added
                   support for new WebAudio commands (pitchbend range, ISO8859
                   character code set; SAF commands); updated audio decoder 
                   code; lint updates; modified code to fix jump, cue point
                   and pause WebAudio commands to work with preparsing
                   structure.
03/18/01    st     Modified code to handle the case when audfmt_do_seek() is
                   done after a preparse to ensure the status is checked
                   before doing a seek.
03/09/01    st     Modified code to add audfmt_store_cb_get_data_count() in
                   order to verify if the required data get is currently
                   buffered or if it has already been sent.
02/28/01    st     Modified code to support the audio format preparse mechanism;
                   modified audfmt_tempo_adjust_down() and 
                   audfmt_tempo_adjust_up() to not do any calculations if tempo
                   is normal; modified code to return proper Y-axis location
                   for picture data; lint updates.
01/26/01    st     Modified code to remove call to track_skip_ahead in
                   do_cmid_extension() because this is done later; modified
                   code to return the proper time in audfmt_parse() in the
                   case where the command buffer overflows and the repeat
                   timer is active.
12/10/00    st     Modified code to perform wave command when NOT skipping
                   in do_cmid_extension().
12/04/00    st     Added CMX2.0 adec access functions and defaults for vol/pan. 
                   Added audfmt_adec_reset(), volume and balance controls.
11/27/00  st/sm    Moved much code to audmain*.  Featurized EFS/MIDI/CMX2.0.
09/26/00    st     Modified code in order to skip over unsupported chunk types.
09/18/00    st     Modified code such that the channels are preassigned when
                   the WebAudio file has multiple tracks, in order to comply 
                   with the WebAudio spec.
08/08/00    st     Fixed bug where files with more than one track do not play
                   properly.
08/07/00    st     Modified code to inform client when a end cue point is 
                   reached and about to loopback; added code to perform the 
                   cue point only a specified number of iterations before 
                   stopping.
07/26/00    st     Moved vibrator patterns to audvib.c.
07/25/00    st     Modified code to support playing file from the cue point
                   instead of starting from beginning; modified vibrator
                   patterns to simply be on all the time for all patterns.
07/22/00    st     Modified code such that user-controlled tuning and tempo
                   are reset after every file is finished; added support for
                   korean text; modified user-controlled tuning such that the
                   command is now done in audfmt_parse(); modified code to
                   have a minimum tempo for WebAudio files.
07/18/00    st     Added proper featurization.
07/15/00    st     Added support for user-controlled tuning and tempo; added
                   support for WebAudio vibrate, LED, cue point, jump messages;
                   added audio specifications to be returned to client; moved
                   some get data functionality to sound server; added support
                   for a minor reset which only resets certain parameters;
                   use qword functions in time calculations for more precision;
                   added support for master volume, master tune, reset, stop,
                   pause, bank change Webaudio commands; bug fixes and lint 
                   updates.
06/13/00    st     Added code to handle the positioning of text messages;
                   modified code to use new snd_cmx_af_pos_enum_type;
                   replaced snd_cmd() with call to snd_audfmt_new_data().
05/19/00    st     Featurized WebAudio specific code; removed WebAudio text 
                   and picture commands that are no longer supported; modified
                   WebAudio bank shift to treat even number as bank 0 and odd 
                   number as bank 1; added compact midi pitch wheel command.
05/09/00    st     Modified audfmt_get_repeat_ms() to audfmt_get_mute().
04/28/00    st     Enabled drum channel messages to get to synthesizer;
                   added fast-forward pause flag processing.
04/13/00    st     Created

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "task.h"               /* Task definitions and prototypes         */
#include "comdef.h"             /* Definitions for byte, word, etc.        */

#ifdef FEATURE_MIDI
#include "audfmt.h"             /* Audio Format definitions and prototypes */
#include "audfmti.h"            /* Internal Audio Format definitions       */
#include "audmain.h"            /* Audio Format definitions and prototypes */
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include "qw.h"                 /* Quadword math routines                  */
#include <string.h>             /* String routines                         */
#include <memory.h>             /* Memory manipulation routines            */
#include <stdlib.h>             /* Dynamic memory allocation routines      */
#include <string.h>
#ifdef FEATURE_WEBAUDIO
#include "audvib.h"             /* Audio Format vibrator routines          */
#include "audfadec.h"           /* Audio Format audio decoder prototypes   */
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

There are definitions for LOCAL data.  There is no global data.

===========================================================================*/
/* This structure contains information about a midi/WebAudio file
** that is being parsed. This module currently only supports parsing
** one file at a time.
*/
#ifdef FEATURE_AUDFMT_MULTISEQUENCER
#define AUDFMT_NUM_SEQUENCES  4
#else
#define AUDFMT_NUM_SEQUENCES  1
#endif /* FEATURE_AUDFMT_MULTISEQUENCER */
static audfmt_parse_type audfmt_parse_ctl[AUDFMT_NUM_SEQUENCES];

static audfmt_cmd_buf_type audfmt_global_cmd = {0, 0};

static boolean preparse_sent = FALSE;

/* The following parameters and defines are used to vary the user-controlled 
** playback tempo. This define is used to vary the resolution of the user-
** controlled tempo changes. For example, a TEMPO_MULT_FACTOR of 100 give 1% 
** resolution (user can modify playback rates in 1% increments), 10 gives 10% 
** resolution, 200 gives .5% resolution.
*/
#define TEMPO_MULT_FACTOR     100
#define MAX_TEMPO_MULT_FACTOR 500
#define MIN_TEMPO_MULT_FACTOR 1

/* These defines are used to parse the MIDI master volume SYS EX command */
#define AUDFMT_MIDI_MVOL_CMD_SIZE 0x7
const uint8 midi_mvol_cmd[] = {0x7f, 0x7f, 0x04, 0x01};

const uint8 audfmt_seq_reset[] = {0xf0, 0x7f, 0x7f, 0x04, 0x7f, 0xf7};

/* These defines/variables are used to support the SP-MIDI commands */
#define AUDFMT_MAX_HP_VOICES   72
#define AUDFMT_MAX_HQ_VOICES   32
#ifdef FEATURE_WEBAUDIO
#define AUDFMT_MAX_MIDI_VOICES 18
#else
#ifdef FEATURE_STD_MIDI
#define AUDFMT_MAX_MIDI_VOICES 32 
#else
#define AUDFMT_MAX_MIDI_VOICES 16
#endif /*FEATURE_STD_MIDI*/
#endif
const uint8 midi_mip_cmd[] = {0x7f, 0x7f, 0x0b, 0x01};

/* These defines are used to indicate the max and min user-controlled 
** tuning levels.
*/
#define AUDFMT_MAX_TUNE_FACTOR  0x4C
#define AUDFMT_MIN_TUNE_FACTOR  0x34
#define AUDFMT_TUNE_DEFAULT     0x40

/* These defines are used to indicate the max and min user-controlled 
** panning levels.
*/
#define AUDFMT_MIN_PAN_FACTOR   0x00
#define AUDFMT_MAX_PAN_FACTOR   0x7f
#define AUDFMT_PAN_DEFAULT      0x40

#define AUDFMT_VOL_DEFAULT      0x2000

#ifdef FEATURE_WEBAUDIO
/* This structure holds various callbacks to be issued at a later time.
*/
audfmt_cb_store_type audfmt_cb_store = {FALSE};

/* This is the compact midi vibrator control structure */
audfmt_vib_type audfmt_cmid_vib_ctl = {FALSE};

/* These macros will determine the channel from the channel index and the track
** number. If there is only 1 track in the cmid file then only channel indexes
** 0-3 are used; if there is more than 1 track then the 16 channels are 
** preassigned and cannot be changed.
*/
#define CMID_GET_CHANNEL_INDEX(v,track,num) \
                              ((num != 1) ? ((track<<2)+v) : (v))

#define CMID_CHANNEL(v,track,num,chan) \
                              (chan[CMID_GET_CHANNEL_INDEX(v,track,num)])

/* Compact midi timebase values. */
const uint32 cmid_timebase_table[ 16 ] = {
  6, 12, 24, 48, 96, 192, 384, 48, 15, 30, 60, 120, 240, 480, 960, 48
};

/* This variable is used to indicate the parse ctl pointer to the lower
** layers of command parsing.
*/
audmain_parse_ctl_type *main_audfmt_parse_ctl;

#define AUDFMT_NUM_ADEC_CHANNELS 4
uint32 audfmt_adec_mask = 0;

#ifdef FEATURE_SMAF
uint32 pan_seq_counter =1;
const uint8 smaf_short_mod_values[14] = {0x00, 0x08, 0x10, 0x18, 0x20,
                                         0x28, 0x30, 0x38, 0x40, 0x48,
                                         0x50, 0x60, 0x70, 0x7f};
#ifdef FEATURE_AUDIO_CMX_5P0
const uint8  smaf_inst_id[]     = {0x43, 0x79, 0x06, 0x7f, 0x01};
const uint8  smaf_inst_id2[]    = {0x43, 0x79, 0x07, 0x7f, 0x01};
const uint8  smaf_wave_id[]     = {0x43, 0x79, 0x06, 0x7f, 0x03};
const uint8  smaf_wave_id2[]    = {0x43, 0x79, 0x07, 0x7f, 0x03};
const sint31 smaf_aeg_attack[]  = { 418533456,  235929600,   46192262,  -78643200,
                                   -137549979, -242949869, -286564511, -319204388,
                                   -365207711, -407719738, -443850911, -443850911,
                                   -555133988, -555133988, -679780511, -679780511};
const sint31 smaf_aeg_release[] = { 418533456,  238973057,  235929600,   73121739,
                                     46192262,  -11954010,  -78643200, -182603856,
                                   -286564511, -365207711, -443850911, -679780511,
                                   -679780511, -679780511, -679780511, -679780511};
const sint31 smaf_aeg_sustain[] = {         0,   32747611,   40967737,   45776202,
                                     49187863,   51834152,   53997595,   55825352,
                                     57408701,   58805919,   60055190,   61185328,
                                     62217404,   63166501,   64045478,   64863554};

const sint31 audfmt_smaf_map_total_level[64] = {
          0,         0,         0,         0,  -1040069,  -1546032,  -2075668,  -2517848,
   -2977901,  -3457334,  -4086644,  -4481394,  -5030184,  -5606779,  -6059067,  -6530371,
   -7021783,  -7535736,  -8073704,  -8446478,  -9030342,  -9437816, -10076298, -10522354,
  -10986868, -11471456, -11977965, -12506918, -13063773, -13646037, -13950899, -14582353,
  -14914316, -15604210, -15965753, -16722533, -17122648, -17537631, -17968655, -18412554,
  -18879405, -19361453, -19870733, -20398239, -20951591, -21541239, -22155680, -22805637,
  -22805637, -23495531, -23495531, -25017543, -25017543, -25864123, -25864123, -26761003,
  -26761003, -27756266, -28849918, -28849918, -28849918, -30029725, -30029725, -31386852
};

/*LCU Decay changes added*/

const uint8 smaf_aeg_decay[]  = {127, 96, 90, 80, 60, 50, 30, 15, 8,
                                 4, 2, 0, 0, 0, 0, 0};

#else
const uint8 smaf_inst_id[]    = {0x43, 0x79, 0x06, 0x7f, 0x01};
const uint8 smaf_inst_id2[]   = {0x43, 0x79, 0x07, 0x7f, 0x01};
const uint8 smaf_wave_id[]    = {0x43, 0x79, 0x06, 0x7f, 0x03};
const uint8 smaf_wave_id2[]   = {0x43, 0x79, 0x07, 0x7f, 0x03};
const uint8 smaf_aeg_attack[] = {127, 71, 61, 46, 25, 15, 10, 5, 4,
                                 3, 2, 1, 0, 0, 0, 0};
const uint8 smaf_aeg_decay[]  = {127, 117, 108, 84, 70, 60, 41, 25, 17,
                                 11, 6, 4, 4, 3, 2, 0};

/* The following table is used to map AEG depth for SMAF PCM DLS */
const uint8 audfmt_smaf_map_total_level[64] = {
  0x7F, 0x7A, 0x74, 0x70, 0x6B, 0x66, 0x62, 0x5E,
  0x5A, 0x57, 0x52, 0x4F, 0x4C, 0x48, 0x45, 0x42,
  0x40, 0x3D, 0x3A, 0x38, 0x36, 0x33, 0x31, 0x2F,
  0x2d, 0x2B, 0x29, 0x28, 0x26, 0x24, 0x23, 0x21,
  0x20, 0x1F, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
  0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x11,
  0x10, 0x0F, 0x0F, 0x0E, 0x0D, 0x0D, 0x0C, 0x0C,
  0x0B, 0x0B, 0x0A, 0x0A, 0x09, 0x09, 0x09, 0x08
};
#endif /* FEATURE_AUDIO_CMX_5P0 */

const sint31 audfmt_smaf_diff_lookup[16] = {
  1, 3, 5, 7, 9, 11, 13, 15, -1, -3, -5, -7, -9, -11, -13, -15
};

const sint31 audfmt_smaf_index_scale[16] = {
  0x0e6, 0x0e6, 0x0e6, 0x0e6, 0x133, 0x199, 0x200, 0x266,
  0x0e6, 0x0e6, 0x0e6, 0x0e6, 0x133, 0x199, 0x200, 0x266
};

const uint64 audfmt_smaf_dls_drum_exp2TableQ20[12]= {
  0x100000, 0x10F38F, 0x11F59A, 0x1306FE, 0x1428A2, 0x155B81,
  0x16A09E, 0x17F910, 0x1965FE, 0x1AE89F, 0x1C823E, 0x1E3437
};
#define SMAF_ADEC_VOL_TO_DSP(x) (x<<6)

/* The following definitions are used to calculate SMAF MA2 pitch bend
** values from the sys ex commands.
*/
#define AUDFMT_SMAF_PITCH_LO_B_OFFSET 88
#define AUDFMT_SMAF_PITCH_HI_B_OFFSET 25
#define AUDFMT_SMAF_PITCH_HI_B_MAX    216

const uint8 audfmt_smaf_pitch_lo_b_values[] = {
  0, 21, 43, 66, 90, 116, 143, 172
};

const uint8 audfmt_smaf_pitch_hi_b_values[] = {
  0, 29, 60, 93, 127, 164, 202, 241
};

/* The following defintions are used to figure out if instruments 
** should be used over drums for SMAF Phrase L1 files.
*/
#define DELTA_DRUM_ERROR_2OP 600
#define SCALE_DRUM_ERROR_2OP 2
#define MAX_DRUM_INSTR_DELTA_2OP 3000

#define SMAF_USE_INSTR_2OP(drum_err, inst_err) \
          ((inst_err < ((SCALE_DRUM_ERROR_2OP * drum_err) + \
                        DELTA_DRUM_ERROR_2OP)) && \
           (inst_err < (drum_err + MAX_DRUM_INSTR_DELTA_2OP)))
                 
#define DELTA_DRUM_ERROR_4OP 1200
#define SCALE_DRUM_ERROR_4OP 2
#define MAX_DRUM_INSTR_DELTA_4OP 6000

#define SMAF_USE_INSTR_4OP(drum_err, inst_err) \
          ((inst_err < ((SCALE_DRUM_ERROR_4OP * drum_err) + \
                        DELTA_DRUM_ERROR_4OP)) && \
           (inst_err < drum_err + MAX_DRUM_INSTR_DELTA_4OP))

typedef uint32 (* smaf_fm_dls_error_fn_type) (
  uint8       *scratch_buf,
  const uint8 *lookup_buf,
  int         bufsize
);

/* The following variables are used to lookup and process SMAF FM DLS
** instruments for HP and Phrase L1 files
*/
uint8 audfmt_smaf_scratch_buf[0x2a];

const uint8 audfmt_smaf_hp_dls_lookup[128][12] = {
  0x60, 0x01, 0x14, 0x52, 0xf4, 0x3a, 0xa0, 0x14, 0x62, 0xf7, 0x00, 0xa0, 
  0x68, 0x01, 0x15, 0x42, 0xf5, 0x3e, 0xa0, 0x14, 0x62, 0xf5, 0x00, 0xa0, 
  0x60, 0x01, 0x15, 0x51, 0xf5, 0x19, 0xa0, 0x14, 0x62, 0xf7, 0x00, 0xa0, 
  0x58, 0x01, 0x15, 0xb2, 0xf3, 0x38, 0xa8, 0x1c, 0xb3, 0xff, 0x00, 0xa0, 
  0x70, 0x01, 0x10, 0x81, 0xf3, 0x4d, 0xa0, 0x14, 0x81, 0xf2, 0x00, 0xa0, 
  0x60, 0x01, 0x30, 0x81, 0xf3, 0x66, 0xa0, 0x14, 0x81, 0xf2, 0x00, 0xa0, 
  0x60, 0x01, 0x14, 0x12, 0xa0, 0x02, 0xa0, 0x44, 0x55, 0xfd, 0x00, 0xa0, 
  0x68, 0x01, 0x10, 0x82, 0xca, 0x3a, 0xa0, 0x10, 0x82, 0xc5, 0x00, 0xa0, 
  0x40, 0x01, 0xc0, 0x46, 0xf5, 0x71, 0xa0, 0x10, 0x53, 0xfb, 0x00, 0xa8, 
  0x50, 0x01, 0x70, 0x26, 0xf3, 0x6a, 0xa0, 0x11, 0x35, 0xf1, 0x02, 0xa0, 
  0x48, 0x01, 0x70, 0x46, 0x90, 0x74, 0xa0, 0x10, 0x32, 0xf0, 0x00, 0xa0, 
  0x40, 0x01, 0xb1, 0x63, 0xfe, 0xa5, 0xa0, 0x10, 0x42, 0xff, 0x00, 0xa8, 
  0x40, 0x01, 0x81, 0x56, 0xf5, 0x94, 0xa0, 0x14, 0x65, 0xed, 0x00, 0xa0, 
  0x58, 0x01, 0x51, 0x66, 0xfa, 0x44, 0xa0, 0x10, 0x66, 0xfe, 0x00, 0xa0, 
  0x68, 0x01, 0x98, 0x22, 0xd8, 0x5c, 0xa0, 0x10, 0x32, 0xae, 0x02, 0x88, 
  0x50, 0x01, 0x30, 0x54, 0x75, 0x15, 0xa1, 0x10, 0x53, 0xf0, 0x02, 0xa8, 
  0x48, 0x01, 0x1d, 0x46, 0xf1, 0x42, 0xa0, 0x15, 0x71, 0xf0, 0x00, 0x98, 
  0x48, 0x01, 0x4d, 0x87, 0xc5, 0x38, 0xa0, 0x15, 0x87, 0xc0, 0x00, 0x80, 
  0x58, 0x01, 0x1d, 0x8a, 0xa1, 0x11, 0xa0, 0x25, 0x8a, 0x80, 0x00, 0x88, 
  0x50, 0x01, 0x34, 0x27, 0x92, 0x26, 0xa1, 0x15, 0x35, 0x41, 0x00, 0xa0, 
  0x40, 0x01, 0x1c, 0x47, 0x70, 0x40, 0xa1, 0x15, 0x45, 0x40, 0x02, 0xa0, 
  0x70, 0x01, 0x44, 0xa8, 0x92, 0x21, 0xa1, 0x15, 0xa6, 0x41, 0x00, 0xa8, 
  0x68, 0x01, 0x3c, 0x61, 0x90, 0x80, 0xa1, 0x14, 0x71, 0x60, 0x00, 0xa0, 
  0x58, 0x01, 0x14, 0x61, 0x70, 0x4c, 0xa0, 0x14, 0x71, 0x60, 0x26, 0x88, 
  0x70, 0x01, 0x20, 0x43, 0xf9, 0x5e, 0xa1, 0x18, 0x83, 0xfc, 0x02, 0xa0, 
  0x78, 0x01, 0x30, 0x28, 0xf1, 0x44, 0xa1, 0x11, 0x3b, 0xf2, 0x00, 0xa0, 
  0x78, 0x01, 0x34, 0xa1, 0xf3, 0x7d, 0xa0, 0x14, 0x82, 0xff, 0x00, 0xa0, 
  0x58, 0x01, 0x30, 0x37, 0xf2, 0x1e, 0xa1, 0x14, 0x82, 0x96, 0x02, 0xa0, 
  0x40, 0x01, 0x30, 0x49, 0xf5, 0x39, 0xa0, 0x14, 0xa6, 0x83, 0x00, 0xa0, 
  0x60, 0x01, 0x34, 0x95, 0x91, 0x21, 0xa1, 0x14, 0x94, 0x81, 0x00, 0xa0, 
  0x78, 0x01, 0x34, 0x95, 0xd1, 0x34, 0xa1, 0x14, 0x94, 0xf1, 0x00, 0xa0, 
  0x60, 0x01, 0x90, 0xf0, 0x24, 0xb2, 0xa0, 0x40, 0x81, 0xdf, 0x02, 0xa0, 
  0x68, 0x01, 0x14, 0x64, 0x90, 0x5c, 0xa0, 0x24, 0x63, 0xca, 0x00, 0xa8, 
  0x60, 0x01, 0x15, 0x81, 0xf2, 0x40, 0xa0, 0x15, 0x81, 0xf1, 0x00, 0xa0, 
  0x60, 0x01, 0x15, 0x81, 0xfe, 0x1e, 0xa0, 0x15, 0x81, 0xa7, 0x00, 0xa0, 
  0x70, 0x01, 0x15, 0x81, 0x62, 0x5d, 0xa0, 0x25, 0x81, 0xe4, 0x00, 0xa0, 
  0x60, 0x01, 0x10, 0xa1, 0xe9, 0x1e, 0xa0, 0x14, 0xf2, 0xfd, 0x01, 0xa0, 
  0x60, 0x01, 0x10, 0xa1, 0x99, 0x22, 0xa0, 0x14, 0xf2, 0xfd, 0x01, 0xa0, 
  0x68, 0x01, 0x15, 0x84, 0xfe, 0x2a, 0xa0, 0x15, 0x81, 0xf7, 0x00, 0xa0, 
  0x68, 0x01, 0x15, 0x81, 0xf2, 0x48, 0xa0, 0x15, 0x81, 0xf1, 0x00, 0xa0, 
  0x68, 0x01, 0x15, 0x3d, 0xd1, 0x38, 0xa1, 0x1c, 0x66, 0x52, 0x00, 0x00, 
  0xa8, 0x01, 0x15, 0x3d, 0xd1, 0x48, 0x89, 0x14, 0x66, 0x70, 0x00, 0x00, 
  0xa0, 0x01, 0x15, 0xc1, 0xd1, 0x25, 0x09, 0x15, 0xc1, 0x60, 0x00, 0x00, 
  0x48, 0x01, 0x14, 0x21, 0x71, 0x25, 0xa1, 0x34, 0x62, 0x70, 0x02, 0xa0, 
  0x88, 0x01, 0x15, 0x11, 0xf2, 0x01, 0xa1, 0x14, 0x6f, 0x61, 0x00, 0xa8, 
  0x40, 0x01, 0x20, 0x55, 0x97, 0x88, 0xa3, 0x10, 0x55, 0xf3, 0x02, 0xa0, 
  0x40, 0x01, 0x21, 0x51, 0xf7, 0x74, 0xa1, 0x11, 0x41, 0xff, 0x02, 0xa0, 
  0x48, 0x01, 0x11, 0x31, 0xf0, 0x1d, 0xa1, 0x21, 0x32, 0xfc, 0x00, 0xa0, 
  0x40, 0x01, 0x14, 0x41, 0xb1, 0x6e, 0x80, 0x2c, 0x72, 0x70, 0x04, 0xe4, 
  0x40, 0x01, 0x14, 0x3f, 0x70, 0x4e, 0xa9, 0x14, 0x7f, 0x60, 0x00, 0xa1, 
  0x50, 0x01, 0x14, 0x21, 0x61, 0x42, 0x08, 0x1c, 0x5f, 0x60, 0x00, 0x00, 
  0x70, 0x01, 0x14, 0x21, 0x82, 0x70, 0xa0, 0x1c, 0x5f, 0x60, 0x00, 0xa0, 
  0x70, 0x01, 0x15, 0x54, 0xf1, 0x5d, 0xa0, 0x2d, 0x5a, 0x60, 0x02, 0xa0, 
  0x40, 0x01, 0x14, 0x94, 0x73, 0x29, 0xa8, 0x1c, 0x71, 0xa6, 0x00, 0xa0, 
  0x70, 0x01, 0x1d, 0x54, 0x30, 0x51, 0xa0, 0x2d, 0x5a, 0x60, 0x00, 0xa0, 
  0x60, 0x01, 0x11, 0x34, 0x56, 0x00, 0xa8, 0x28, 0x55, 0xa4, 0x00, 0xa6, 
  0x70, 0x01, 0x14, 0x75, 0x91, 0x46, 0xa0, 0x14, 0x9f, 0xe0, 0x00, 0xa0, 
  0x70, 0x01, 0x14, 0x75, 0x81, 0x4e, 0xa0, 0x14, 0x9f, 0x80, 0x00, 0xa0, 
  0xa8, 0x01, 0x14, 0x56, 0x81, 0x42, 0xa0, 0x1c, 0x72, 0xb3, 0x00, 0xe0, 
  0x48, 0x01, 0x75, 0x7e, 0x91, 0x15, 0xa1, 0x14, 0xc2, 0x62, 0x00, 0xa1, 
  0x70, 0x01, 0x14, 0xa1, 0x66, 0x66, 0xa0, 0x14, 0xaf, 0x70, 0x00, 0xa0, 
  0x60, 0x01, 0x1c, 0xf5, 0x71, 0x22, 0xa0, 0x24, 0xf4, 0x70, 0x00, 0xa0, 
  0x50, 0x01, 0x14, 0x44, 0xd4, 0x2e, 0x81, 0x14, 0x71, 0x91, 0x00, 0xa0, 
  0x40, 0x01, 0x14, 0x15, 0x67, 0x2e, 0xa0, 0x14, 0x76, 0xc1, 0x00, 0xa0, 
  0x68, 0x01, 0x15, 0x23, 0x90, 0x4a, 0xa1, 0x1c, 0xb2, 0x70, 0x00, 0xa0, 
  0x60, 0x01, 0x15, 0x33, 0x90, 0x32, 0xa1, 0x1c, 0x92, 0x70, 0x00, 0xa0, 
  0x58, 0x01, 0x15, 0x33, 0x70, 0x16, 0xa1, 0x1c, 0x92, 0x70, 0x00, 0xa0, 
  0x58, 0x01, 0x15, 0xf3, 0x90, 0x06, 0xa1, 0x1c, 0xf2, 0x70, 0x00, 0xa0, 
  0x80, 0x01, 0x14, 0x41, 0xd2, 0x72, 0x88, 0x3c, 0x72, 0xd0, 0x00, 0xa1, 
  0x58, 0x01, 0x15, 0x2e, 0x71, 0x42, 0x80, 0x1c, 0x8b, 0xc0, 0x00, 0x05, 
  0x80, 0x01, 0x1d, 0x57, 0xc1, 0x5e, 0xc1, 0x3d, 0x81, 0x71, 0x00, 0xc9, 
  0x50, 0x01, 0x25, 0x1b, 0x92, 0x42, 0xa0, 0x14, 0x72, 0x71, 0x00, 0xa0, 
  0x40, 0x01, 0x1c, 0xf5, 0xa5, 0x5c, 0xa8, 0x1c, 0xa5, 0x61, 0x00, 0xa8, 
  0x80, 0x01, 0x14, 0xf8, 0x95, 0x19, 0x00, 0x14, 0xa5, 0x61, 0x00, 0x28, 
  0x88, 0x01, 0x44, 0xf5, 0x71, 0xa2, 0x88, 0x14, 0xa5, 0x70, 0x00, 0x88, 
  0x40, 0x01, 0x45, 0x84, 0xc5, 0x94, 0xa0, 0x14, 0xa5, 0x73, 0x00, 0xa0, 
  0x40, 0x01, 0x1c, 0x66, 0x65, 0x29, 0x08, 0x14, 0x65, 0x62, 0x00, 0x98, 
  0xb0, 0x01, 0x14, 0x68, 0x58, 0x1c, 0xa0, 0x14, 0x64, 0x53, 0x00, 0x98, 
  0x69, 0x01, 0x2c, 0x77, 0x50, 0x5c, 0xa0, 0x14, 0x78, 0x50, 0x00, 0xa8, 
  0x59, 0x01, 0x14, 0x77, 0x70, 0x10, 0x88, 0x14, 0x76, 0x70, 0x00, 0x88, 
  0x40, 0x01, 0x24, 0x3f, 0xf0, 0x65, 0xa2, 0x14, 0x8f, 0xf0, 0x00, 0xa0, 
  0x40, 0x01, 0x14, 0x5f, 0xf0, 0x38, 0xa1, 0x14, 0x8f, 0xf0, 0x00, 0xa1, 
  0x40, 0x01, 0x24, 0x56, 0x85, 0x19, 0xa0, 0x14, 0x84, 0x61, 0x02, 0xa0, 
  0x40, 0x01, 0x14, 0x26, 0x61, 0x15, 0xa0, 0x14, 0x86, 0x90, 0x00, 0x88, 
  0x40, 0x01, 0x14, 0x62, 0x92, 0x2e, 0xa1, 0x24, 0x81, 0x92, 0x00, 0xa0, 
  0x48, 0x01, 0x34, 0x51, 0x51, 0x13, 0x88, 0x1c, 0x7f, 0x80, 0x02, 0xa0, 
  0x69, 0x01, 0x34, 0x5f, 0xa0, 0x2d, 0xa4, 0x1c, 0x6f, 0xb0, 0x00, 0xa4, 
  0x68, 0x01, 0x14, 0x91, 0xf1, 0x3e, 0xa0, 0x14, 0x94, 0xf0, 0x02, 0xa0, 
  0x41, 0x01, 0x2d, 0x41, 0xf6, 0x02, 0x22, 0x14, 0x50, 0x30, 0x3c, 0x94, 
  0x58, 0x01, 0x1c, 0x18, 0xb5, 0x38, 0x00, 0x15, 0x35, 0x10, 0x00, 0x80, 
  0x70, 0x01, 0x1c, 0x41, 0x93, 0x5c, 0xa0, 0x1c, 0x65, 0x81, 0x00, 0xa0, 
  0x40, 0x01, 0x15, 0x14, 0x50, 0x68, 0x00, 0x25, 0x3a, 0x60, 0x00, 0x00, 
  0x60, 0x01, 0x74, 0x31, 0x24, 0x6a, 0xa0, 0x14, 0x52, 0x33, 0x00, 0x08, 
  0x60, 0x01, 0x14, 0x71, 0xa7, 0x40, 0x89, 0x14, 0x71, 0x44, 0x00, 0xa1, 
  0x58, 0x01, 0x14, 0x31, 0x33, 0x32, 0x08, 0x14, 0x52, 0x62, 0x00, 0x00, 
  0x40, 0x01, 0x14, 0x11, 0x14, 0x54, 0x89, 0x14, 0x6f, 0x20, 0x00, 0xa0, 
  0x48, 0x01, 0xa5, 0x68, 0xff, 0x0f, 0xa0, 0x19, 0x26, 0x80, 0x00, 0xa0, 
  0x41, 0x01, 0x24, 0x31, 0x42, 0x2c, 0xa4, 0x14, 0x31, 0x41, 0x00, 0xa5, 
  0x68, 0x01, 0x60, 0x54, 0x79, 0x1b, 0xa0, 0x10, 0x22, 0xa7, 0x00, 0xa0, 
  0x70, 0x01, 0x34, 0x11, 0xf8, 0x56, 0xa0, 0x1c, 0x62, 0xf2, 0x02, 0xa0, 
  0x40, 0x01, 0x18, 0x11, 0xf5, 0x1a, 0xa1, 0x28, 0x52, 0xff, 0x00, 0xa0, 
  0x58, 0x01, 0x1c, 0x11, 0x15, 0x52, 0x01, 0x34, 0x31, 0x11, 0x02, 0x0d, 
  0x58, 0x01, 0x14, 0x11, 0x33, 0x32, 0x00, 0x14, 0x3d, 0xd0, 0x02, 0x48, 
  0x50, 0x01, 0x64, 0x33, 0x57, 0x31, 0x01, 0x14, 0x41, 0x62, 0x00, 0x00, 
  0x58, 0x01, 0x20, 0x32, 0xd5, 0x16, 0xa0, 0x70, 0x62, 0xff, 0x00, 0xa1, 
  0x40, 0x01, 0x11, 0x13, 0xa1, 0x30, 0xa1, 0x31, 0x52, 0xae, 0x02, 0xa0, 
  0x68, 0x01, 0x10, 0x47, 0xf7, 0x08, 0xa1, 0x10, 0x65, 0xfe, 0x00, 0xa2, 
  0x60, 0x01, 0x30, 0x45, 0xd3, 0x46, 0x00, 0x10, 0x44, 0xe1, 0x00, 0x01, 
  0x70, 0x01, 0x40, 0x6a, 0xf5, 0x15, 0xa0, 0x10, 0x52, 0xc0, 0x00, 0xa0, 
  0x68, 0x01, 0x14, 0x0c, 0x72, 0x15, 0xa0, 0x24, 0xcf, 0x60, 0x00, 0xa1, 
  0x68, 0x01, 0x15, 0x3d, 0xd3, 0x16, 0xa1, 0x14, 0x66, 0x51, 0x00, 0xa0, 
  0x40, 0x01, 0x04, 0x5a, 0xd0, 0x10, 0xa2, 0x14, 0xbf, 0x80, 0x06, 0xa0, 
  0x58, 0x01, 0x80, 0x51, 0xfe, 0x9d, 0xa0, 0x50, 0x53, 0xce, 0x02, 0xa0, 
  0x68, 0x01, 0x70, 0x6c, 0xe2, 0x54, 0xa0, 0x20, 0x68, 0xf1, 0x00, 0xa0, 
  0x60, 0x01, 0x50, 0x57, 0x63, 0x76, 0xa0, 0x10, 0x5f, 0xd0, 0x00, 0xa0, 
  0x68, 0x01, 0x81, 0x8a, 0xf2, 0x5a, 0xa0, 0x21, 0x58, 0xfe, 0x00, 0xa0, 
  0x58, 0x01, 0x00, 0x70, 0xb0, 0x0a, 0x01, 0x00, 0x3d, 0xf0, 0x00, 0xa0, 
  0x50, 0x01, 0x10, 0x58, 0xf4, 0x05, 0xa2, 0x00, 0x66, 0xf0, 0x00, 0xa1, 
  0x78, 0x01, 0x10, 0x61, 0xf0, 0x48, 0xa2, 0x01, 0x23, 0xf0, 0x00, 0xa0, 
  0x78, 0x01, 0xe0, 0x0f, 0x10, 0x00, 0xa0, 0x08, 0xff, 0x1f, 0x00, 0xab, 
  0x78, 0x01, 0x60, 0x48, 0xf2, 0x02, 0xa0, 0x30, 0x46, 0x58, 0x22, 0xa2, 
  0x78, 0x01, 0xe0, 0x08, 0xf0, 0x2c, 0xa0, 0x09, 0x44, 0x30, 0x14, 0xab, 
  0x38, 0x01, 0xe0, 0x06, 0xf0, 0x00, 0x00, 0x08, 0x2f, 0x10, 0x00, 0x0b, 
  0x40, 0x01, 0x59, 0x37, 0x3a, 0x56, 0xa8, 0xa9, 0x76, 0x53, 0x01, 0xa8, 
  0x68, 0x01, 0x55, 0x02, 0xb6, 0x71, 0xa2, 0x41, 0x34, 0xf1, 0x00, 0xa0, 
  0xf8, 0x01, 0xfc, 0x06, 0xf0, 0x00, 0xae, 0x0d, 0x5f, 0x40, 0x00, 0xaf, 
  0xf8, 0x01, 0x64, 0x1f, 0xf0, 0x00, 0x00, 0x4c, 0x72, 0x51, 0x00, 0xc9, 
  0x78, 0x01, 0x5c, 0x03, 0xff, 0x10, 0x00, 0xf8, 0x86, 0xfb, 0x00, 0x0e
};

/* The following table matching drums that are not supported in the DSP
** the first 11 entries are for drums 24->34, the last 3 are for 82->84
*/
const uint8 audfmt_smaf_hp_drum_range_map[14]= {
  75, 55, 69, 55, 49, 69, 80, 38, 37, 36, 55, 42, 51, 53
};

const uint8 audfmt_smaf_hp_drum_lookup[61][12] = {
  0x70, 0x01, 0x30, 0x07, 0xff, 0x0c, 0xa1, 0x31, 0xf8, 0xaf, 0x00, 0xa6, 
  0x78, 0x01, 0x00, 0x08, 0xf3, 0x00, 0xa0, 0x11, 0x4a, 0x95, 0x20, 0xa0, 
  0x78, 0x01, 0x4c, 0x68, 0x60, 0x14, 0xa0, 0x00, 0x48, 0x54, 0x5c, 0xa0, 
  0x78, 0x01, 0x00, 0x08, 0xf3, 0x00, 0xa0, 0x31, 0x4a, 0x95, 0x20, 0xa0, 
  0x78, 0x01, 0xdc, 0x68, 0x60, 0x14, 0xa0, 0x08, 0x48, 0x54, 0x48, 0xa0, 
  0x78, 0x01, 0xfc, 0x38, 0x60, 0x14, 0xa0, 0x08, 0x58, 0x56, 0x00, 0xa0, 
  0x70, 0x01, 0x70, 0x07, 0xff, 0x0c, 0xa1, 0x51, 0xf8, 0xaf, 0x00, 0xa6, 
  0x78, 0x01, 0x00, 0x0c, 0xf1, 0x24, 0xa2, 0x00, 0x77, 0xf3, 0x00, 0xa0, 
  0x58, 0x01, 0x20, 0xc9, 0xff, 0x01, 0xa0, 0x10, 0x8b, 0xfb, 0x00, 0xa0, 
  0x68, 0x01, 0x11, 0xd7, 0xa1, 0x34, 0xa5, 0x01, 0x50, 0x70, 0x00, 0xa0, 
  0x78, 0x01, 0x60, 0x08, 0xf3, 0x0c, 0xa2, 0x20, 0x6a, 0x93, 0x00, 0xa6, 
  0x68, 0x01, 0x10, 0x7f, 0xf7, 0x00, 0xa0, 0x01, 0x63, 0xf0, 0x00, 0xa0, 
  0x60, 0x01, 0x01, 0x78, 0xb7, 0x10, 0xa2, 0x11, 0x70, 0xf0, 0x00, 0xa0, 
  0x70, 0x01, 0x70, 0x07, 0xff, 0x00, 0xa1, 0x41, 0xf8, 0xaf, 0x00, 0xa6, 
  0x78, 0x01, 0x00, 0x0b, 0xa0, 0x34, 0xa2, 0x00, 0x78, 0xa3, 0x00, 0xa6, 
  0x68, 0x01, 0xf4, 0xf0, 0xf0, 0x00, 0xa0, 0xf1, 0x63, 0xf1, 0x10, 0xa7, 
  0x78, 0x01, 0x00, 0x0b, 0xa0, 0x30, 0xa2, 0x00, 0x78, 0xa3, 0x00, 0xa6, 
  0x58, 0x01, 0xc0, 0x6a, 0xff, 0x24, 0xa0, 0x50, 0x54, 0xf7, 0x18, 0xa0, 
  0x68, 0x01, 0x90, 0x88, 0xd0, 0x00, 0xa0, 0x11, 0x78, 0xa6, 0x00, 0xa2, 
  0x60, 0x01, 0xc0, 0x6a, 0xff, 0x3c, 0xa0, 0x60, 0x54, 0xf7, 0x00, 0xa0, 
  0x68, 0x01, 0xc0, 0x87, 0xf0, 0x00, 0xa0, 0x71, 0x78, 0x74, 0x04, 0xa2, 
  0x60, 0x01, 0xc0, 0x99, 0xff, 0x3c, 0xa0, 0x80, 0x64, 0xf7, 0x00, 0xa0, 
  0x68, 0x01, 0xc0, 0x26, 0xf0, 0x00, 0xa0, 0x21, 0x3b, 0xc4, 0x00, 0xa2, 
  0x60, 0x01, 0xc0, 0x5a, 0xff, 0x2c, 0xa0, 0xa0, 0x85, 0xf7, 0x00, 0xa0, 
  0x60, 0x01, 0xc0, 0x9a, 0xff, 0x3c, 0xa0, 0xc0, 0x65, 0xf7, 0x00, 0xa0, 
  0x38, 0x01, 0xe0, 0x16, 0xf0, 0x00, 0xa7, 0xf1, 0x2f, 0x90, 0x00, 0x06, 
  0x60, 0x01, 0xd0, 0x9a, 0xff, 0x3c, 0xa0, 0xf0, 0x65, 0xf7, 0x00, 0xa0, 
  0x68, 0x01, 0xf0, 0x07, 0xf4, 0x02, 0xa6, 0x70, 0x44, 0xfe, 0x50, 0xa6, 
  0x78, 0x01, 0xb0, 0x35, 0xf3, 0x00, 0xa5, 0x09, 0x20, 0x70, 0x28, 0x0e, 
  0x58, 0x01, 0xe0, 0x44, 0xee, 0x28, 0xa3, 0x70, 0x55, 0xfe, 0x00, 0xa1, 
  0x68, 0x01, 0xf4, 0x06, 0xf0, 0x00, 0xa2, 0x21, 0x4b, 0xc4, 0x00, 0xa3, 
  0x38, 0x01, 0xe0, 0x01, 0xf0, 0x44, 0xa0, 0xf1, 0x31, 0x90, 0x14, 0x06, 
  0x78, 0x01, 0x11, 0x79, 0xf4, 0x00, 0xa2, 0x01, 0x53, 0xf0, 0x10, 0xa0, 
  0x78, 0x01, 0xe0, 0x06, 0xf0, 0x06, 0xa0, 0xf9, 0x21, 0x9e, 0x04, 0x0e, 
  0x78, 0x01, 0x10, 0x10, 0xf0, 0x08, 0xa5, 0x11, 0x4f, 0xf2, 0x00, 0xa0, 
  0x68, 0x01, 0xf0, 0x07, 0xf4, 0x02, 0xa6, 0x70, 0x44, 0xfe, 0x54, 0xa6, 
  0x58, 0x01, 0x60, 0xc5, 0xf0, 0x00, 0xa0, 0x20, 0x85, 0xf0, 0x00, 0xa0, 
  0x59, 0x01, 0x10, 0x4a, 0xfb, 0x00, 0xa0, 0x20, 0x68, 0xc9, 0x00, 0xa0, 
  0x58, 0x01, 0x10, 0xb9, 0xf8, 0x0d, 0xa0, 0x10, 0x7b, 0xfb, 0x00, 0xa6, 
  0x58, 0x01, 0x10, 0xda, 0xf8, 0x35, 0xa0, 0x20, 0x88, 0xfb, 0x00, 0xa0, 
  0x58, 0x01, 0x10, 0xf6, 0xf8, 0x68, 0xa1, 0x20, 0x36, 0xfc, 0x00, 0x00, 
  0x78, 0x01, 0x10, 0xa9, 0xf0, 0x00, 0xa3, 0x00, 0x69, 0xf0, 0x00, 0xa0, 
  0x78, 0x01, 0x00, 0x89, 0xf8, 0x2e, 0xa1, 0x00, 0x64, 0xfa, 0x00, 0xa0, 
  0x68, 0x01, 0x70, 0x6c, 0xe2, 0x54, 0xa0, 0x20, 0x68, 0xf1, 0x00, 0xa0, 
  0x68, 0x01, 0x70, 0x6c, 0xe2, 0x54, 0xa0, 0x20, 0x68, 0xf1, 0x00, 0xa0, 
  0x78, 0x01, 0xf0, 0xf5, 0xa4, 0x00, 0xa0, 0xf0, 0x74, 0x80, 0x10, 0xa2, 
  0x78, 0x01, 0xa0, 0x86, 0xe4, 0x00, 0xa2, 0x30, 0x68, 0x84, 0x00, 0xa1, 
  0xf8, 0x01, 0x87, 0x02, 0x62, 0x5c, 0x03, 0x84, 0x7c, 0x53, 0x08, 0x00, 
  0xf8, 0x01, 0x87, 0x02, 0x62, 0x5c, 0x03, 0x84, 0x7c, 0x53, 0x08, 0x00, 
  0x78, 0x01, 0x10, 0x76, 0xf0, 0x00, 0xab, 0x41, 0x87, 0x63, 0x00, 0xa2, 
  0x78, 0x01, 0x00, 0x41, 0xf0, 0x10, 0x02, 0xf1, 0x63, 0x41, 0x24, 0x03, 
  0x41, 0x01, 0x60, 0x40, 0x1f, 0xfc, 0xa0, 0x51, 0x57, 0xff, 0x00, 0xa0, 
  0x40, 0x01, 0x60, 0x40, 0x0f, 0xfc, 0xa3, 0x21, 0x57, 0xff, 0x00, 0xa0, 
  0x41, 0x01, 0x60, 0x40, 0x1f, 0xfc, 0xa0, 0x21, 0x57, 0xff, 0x00, 0xa0, 
  0x40, 0x01, 0x10, 0x75, 0x5e, 0x6a, 0xa0, 0x20, 0x55, 0x60, 0x00, 0xa0, 
  0x40, 0x01, 0x18, 0x88, 0xf4, 0x25, 0xa0, 0x28, 0x50, 0x80, 0x20, 0xa0, 
  0x60, 0x01, 0xa0, 0x00, 0xef, 0x6d, 0xa3, 0xe1, 0x5f, 0xf0, 0x01, 0xa0, 
  0x60, 0x01, 0xb0, 0x00, 0xef, 0x6d, 0xa3, 0xe1, 0x3f, 0xf0, 0x49, 0xa0, 
  0x78, 0x01, 0xf0, 0x62, 0xf5, 0x00, 0xa4, 0xf0, 0x88, 0x96, 0x00, 0xa7, 
  0xf0, 0x01, 0xf0, 0x04, 0xf0, 0x0e, 0x03, 0xf1, 0x47, 0x83, 0x00, 0x0c, 
  0x58, 0x01, 0xe0, 0x54, 0x3e, 0x00, 0xa3, 0x90, 0x36, 0x40, 0x19, 0xa1
};

const uint8 audfmt_smaf_fm_4op_dls_lookup[128][29] = {
  0x43, 0x02, 0x67, 0xff, 0x9d, 0x00, 0x10, 0x40,
  0x23, 0x33, 0xe2, 0x73, 0x00, 0x50, 0x00, 0x12,
  0x41, 0xd3, 0x58, 0x01, 0x10, 0x00, 0x23, 0x63,
  0xd4, 0x02, 0x01, 0x10, 0x00,
  0x43, 0x23, 0x22, 0xf5, 0x9e, 0x00, 0x10, 0x00,
  0x22, 0x32, 0xff, 0x72, 0x00, 0x50, 0x00, 0x23,
  0x22, 0xfd, 0x66, 0x01, 0x10, 0x00, 0x13, 0x52,
  0xf4, 0x28, 0x01, 0x10, 0x00,
  0x45, 0x23, 0x42, 0xd6, 0x51, 0x00, 0x40, 0x06,
  0x13, 0x61, 0xd7, 0x22, 0x01, 0x10, 0x00, 0x33,
  0x43, 0xde, 0x2d, 0x00, 0x20, 0x00, 0x13, 0x51,
  0xdf, 0x22, 0x01, 0x20, 0x00,
  0x45, 0x23, 0x51, 0xfe, 0x68, 0x45, 0x13, 0x06,
  0x23, 0xa3, 0xd3, 0x0a, 0x43, 0x27, 0x00, 0x22,
  0x51, 0xc3, 0x5c, 0x10, 0x17, 0x05, 0x33, 0xa3,
  0xd3, 0x0a, 0x44, 0x23, 0x00,
  0x83, 0x23, 0xa3, 0xb3, 0x6d, 0x30, 0x30, 0x01,
  0x22, 0x92, 0xb4, 0x6f, 0x04, 0x30, 0x00, 0x13,
  0x44, 0xa1, 0x4d, 0x04, 0x20, 0x01, 0x13, 0x71,
  0xa8, 0x14, 0x31, 0x10, 0x00,
  0x45, 0x53, 0xc4, 0xfb, 0x8c, 0x44, 0x70, 0x95,
  0x12, 0x82, 0xff, 0x12, 0x41, 0x10, 0x00, 0x13,
  0xb0, 0xf1, 0x49, 0x44, 0x10, 0x02, 0x13, 0x72,
  0xff, 0x10, 0x41, 0x10, 0x00,
  0x46, 0x23, 0x52, 0xe0, 0x01, 0x00, 0x10, 0x24,
  0x03, 0x52, 0xf3, 0x53, 0x00, 0x60, 0x18, 0x03,
  0x13, 0xf6, 0x72, 0x00, 0x70, 0x20, 0x23, 0x72,
  0xef, 0x10, 0x03, 0x10, 0x28,
  0x43, 0x12, 0x61, 0xff, 0x60, 0x44, 0x10, 0x2d,
  0x12, 0x51, 0xf0, 0x74, 0x44, 0x10, 0x00, 0x33,
  0x73, 0xf2, 0x6d, 0x44, 0x70, 0x20, 0x23, 0x92,
  0xf2, 0x20, 0x41, 0x30, 0x00,
  0x45, 0x63, 0x56, 0xef, 0x56, 0x00, 0x90, 0x12,
  0x42, 0x44, 0xde, 0x18, 0x01, 0x10, 0x00, 0x63,
  0x66, 0xec, 0x5b, 0x00, 0xb0, 0x28, 0x42, 0x44,
  0xee, 0x18, 0x01, 0x10, 0x00,
  0x47, 0x33, 0x49, 0xf4, 0x24, 0x44, 0x70, 0x00,
  0x23, 0x3b, 0xfb, 0x3c, 0x44, 0x40, 0x00, 0x23,
  0x43, 0xf4, 0x49, 0x44, 0x20, 0x01, 0x32, 0x44,
  0xfe, 0x10, 0x44, 0x10, 0x00,
  0x45, 0x23, 0x25, 0x50, 0x82, 0x00, 0x20, 0x08,
  0x33, 0x24, 0xf0, 0x05, 0x01, 0x13, 0x00, 0x23,
  0x25, 0xa0, 0x70, 0x00, 0x90, 0x08, 0x13, 0x12,
  0xf0, 0x18, 0x01, 0x17, 0x00,
  0x45, 0x22, 0x44, 0xc2, 0x5c, 0x41, 0x70, 0x00,
  0x23, 0x59, 0xd6, 0x1e, 0x51, 0x40, 0x00, 0x23,
  0x34, 0xc2, 0x78, 0x51, 0x80, 0x00, 0x32, 0x42,
  0xdf, 0x1c, 0x31, 0x10, 0x00,
  0x45, 0x43, 0x47, 0xaf, 0xa1, 0x00, 0xc0, 0x07,
  0x42, 0x54, 0xbf, 0x14, 0x01, 0x10, 0x00, 0x63,
  0x47, 0xbf, 0x84, 0x00, 0x60, 0x00, 0x53, 0x54,
  0xdf, 0x14, 0x01, 0x10, 0x00,
  0x45, 0x62, 0x69, 0xfd, 0x62, 0x00, 0x50, 0x02,
  0x52, 0x77, 0xfd, 0x0c, 0x03, 0x10, 0x00, 0x63,
  0x66, 0xfa, 0x76, 0x00, 0x50, 0x00, 0x62, 0x76,
  0xfe, 0x02, 0x03, 0x10, 0x00,
  0x45, 0x32, 0x34, 0xf5, 0x41, 0x30, 0xa0, 0x80,
  0x22, 0x33, 0xf2, 0x14, 0x03, 0x10, 0x00, 0x32,
  0x34, 0xf5, 0x41, 0x30, 0x73, 0x47, 0x22, 0x33,
  0xf2, 0x16, 0x03, 0x27, 0x00,
  0x46, 0x42, 0x4a, 0xec, 0x1a, 0x00, 0x20, 0x0b,
  0x32, 0x33, 0xb5, 0x51, 0x00, 0x30, 0x08, 0x32,
  0x33, 0xd0, 0x28, 0x00, 0x10, 0x00, 0x42, 0x44,
  0xc6, 0x18, 0x03, 0x10, 0x00,
  0xc2, 0x02, 0xc4, 0xf0, 0x02, 0x30, 0x00, 0x00,
  0x02, 0xc5, 0xf0, 0x01, 0x00, 0x10, 0x20, 0x02,
  0xc5, 0xd1, 0x1e, 0x30, 0x31, 0x48, 0x02, 0xc1,
  0xf0, 0x1e, 0x30, 0x20, 0x20,
  0x47, 0x02, 0xa5, 0xe1, 0x0e, 0x50, 0x02, 0x04,
  0x02, 0x08, 0xd5, 0x74, 0x00, 0x20, 0x00, 0x02,
  0xa5, 0xe1, 0x06, 0x13, 0x13, 0x00, 0x02, 0xa6,
  0xe0, 0x06, 0x03, 0x27, 0x00,
  0x87, 0x02, 0xdf, 0xf0, 0x25, 0x70, 0x13, 0x04,
  0x02, 0xaf, 0xb1, 0x15, 0x50, 0x10, 0xa8, 0x02,
  0xef, 0xf0, 0x24, 0x13, 0x26, 0x00, 0x02, 0xef,
  0xf0, 0x24, 0x33, 0x07, 0x88,
  0x47, 0x02, 0x5f, 0x90, 0x4e, 0x00, 0x30, 0x00,
  0x02, 0x27, 0xb2, 0x74, 0x00, 0x70, 0x00, 0x02,
  0x5f, 0x80, 0x12, 0x00, 0x10, 0x00, 0x02, 0x57,
  0x80, 0x12, 0x00, 0x00, 0x28,
  0x85, 0x03, 0x58, 0x71, 0x62, 0x00, 0x20, 0x83,
  0x03, 0x6f, 0x50, 0x02, 0x01, 0x10, 0x00, 0x03,
  0x5c, 0x63, 0x29, 0x00, 0x10, 0x28, 0x03, 0x7f,
  0x50, 0x01, 0x01, 0x20, 0x00,
  0x85, 0x02, 0x02, 0x81, 0x54, 0x00, 0x36, 0x8a,
  0x02, 0xa2, 0x72, 0x09, 0x01, 0x17, 0x00, 0x02,
  0x0f, 0x61, 0x48, 0x00, 0x12, 0x88, 0x02, 0xaf,
  0x70, 0x1e, 0x01, 0x23, 0x00,
  0x84, 0x02, 0x9f, 0xf0, 0xb3, 0x07, 0xe0, 0x00,
  0x02, 0x8f, 0xf0, 0xa4, 0x11, 0xa0, 0x00, 0x02,
  0x8f, 0xf0, 0x90, 0x03, 0x10, 0x00, 0x02, 0x8f,
  0x60, 0x0e, 0x01, 0x20, 0x00,
  0x05, 0x02, 0x0c, 0x70, 0x3c, 0x30, 0x21, 0x64,
  0x02, 0xa2, 0x70, 0x2a, 0x11, 0x21, 0x00, 0x02,
  0x0f, 0x70, 0x50, 0x10, 0x10, 0x2b, 0x02, 0xaf,
  0x70, 0x28, 0x31, 0x10, 0x80,
  0x85, 0x13, 0x41, 0xe8, 0x55, 0x00, 0x10, 0x06,
  0x32, 0x73, 0xff, 0x00, 0x01, 0x10, 0x00, 0x53,
  0x55, 0xb4, 0x38, 0x00, 0x30, 0x08, 0x42, 0x94,
  0xdf, 0x36, 0x03, 0x10, 0x00,
  0x44, 0x12, 0x47, 0xf2, 0x6a, 0x44, 0x90, 0x2c,
  0x13, 0x83, 0xf5, 0xb6, 0x45, 0xd0, 0x00, 0x13,
  0x42, 0xf1, 0x5e, 0x45, 0x10, 0x06, 0x22, 0x83,
  0xdf, 0x10, 0x25, 0x10, 0x00,
  0x43, 0x13, 0x77, 0xf3, 0x45, 0x45, 0x10, 0x00,
  0x12, 0x45, 0xf2, 0x4b, 0x45, 0x50, 0x00, 0x02,
  0x72, 0xff, 0x7d, 0x45, 0x30, 0x00, 0x02, 0x82,
  0xcf, 0x10, 0x45, 0x10, 0x00,
  0xc5, 0x22, 0x2a, 0xf1, 0x3d, 0x00, 0x10, 0x08,
  0x22, 0x92, 0xff, 0x0e, 0x03, 0x10, 0x00, 0x23,
  0x32, 0xf6, 0x41, 0x00, 0x30, 0x20, 0x42, 0x84,
  0xe6, 0x0e, 0x03, 0x10, 0x00,
  0x85, 0x63, 0x78, 0xd7, 0x44, 0x00, 0x00, 0x07,
  0x32, 0x99, 0xe7, 0x00, 0x03, 0x00, 0x00, 0x32,
  0x83, 0xe9, 0x13, 0x00, 0x10, 0x00, 0x33, 0xa4,
  0xb3, 0x00, 0x03, 0x10, 0x00,
  0x44, 0x03, 0x28, 0xff, 0x4c, 0x44, 0x07, 0x62,
  0x02, 0x11, 0xc1, 0x3d, 0x44, 0x23, 0x00, 0x02,
  0xa2, 0xb1, 0x3c, 0x44, 0x10, 0x00, 0x12, 0xa1,
  0xb1, 0x28, 0x44, 0x20, 0x00,
  0x84, 0x02, 0x2c, 0xb0, 0x21, 0x00, 0x20, 0x1c,
  0x02, 0xa5, 0xc1, 0x74, 0x00, 0x10, 0x00, 0x02,
  0xa5, 0xc1, 0x5c, 0x00, 0x20, 0x40, 0x02, 0xa1,
  0xc5, 0x3c, 0x03, 0x10, 0x30,
  0x85, 0x82, 0x72, 0xf0, 0x59, 0x00, 0x00, 0x45,
  0x32, 0x93, 0xdf, 0x18, 0x01, 0x20, 0x40, 0x82,
  0x72, 0xb0, 0x44, 0x00, 0x00, 0x05, 0x72, 0x77,
  0xaf, 0x34, 0x05, 0x20, 0x30,
  0x45, 0x33, 0x83, 0xba, 0x39, 0x00, 0x10, 0x03,
  0x32, 0x83, 0xcb, 0x00, 0x05, 0x10, 0x00, 0x32,
  0x13, 0x91, 0x1f, 0x00, 0x10, 0x00, 0x32, 0x83,
  0xca, 0x14, 0x05, 0x10, 0x00,
  0x43, 0x13, 0x32, 0xa1, 0x72, 0x44, 0x10, 0x06,
  0x33, 0x64, 0x94, 0xe8, 0x44, 0xc0, 0x00, 0x23,
  0x33, 0xb2, 0x5a, 0x44, 0x10, 0x00, 0x13, 0x81,
  0xb2, 0x00, 0x44, 0x20, 0x00,
  0x43, 0x23, 0x37, 0xf1, 0x4e, 0x44, 0x10, 0x05,
  0x43, 0x6b, 0xc7, 0x54, 0x44, 0x70, 0x00, 0x23,
  0x69, 0xf2, 0x5e, 0x44, 0x20, 0x00, 0x63, 0x82,
  0xb6, 0x00, 0x44, 0x10, 0x00,
  0x43, 0x23, 0x33, 0xc1, 0x76, 0x45, 0x12, 0x04,
  0x33, 0x63, 0xa3, 0x66, 0x45, 0x11, 0x00, 0x23,
  0x63, 0x91, 0x66, 0x45, 0x10, 0x00, 0x23, 0x81,
  0xb2, 0x00, 0x45, 0x20, 0x00,
  0x43, 0x23, 0x37, 0xf2, 0x3a, 0x44, 0x10, 0x03,
  0x63, 0x66, 0xf4, 0x54, 0x44, 0x90, 0x00, 0x23,
  0x69, 0xc2, 0x62, 0x44, 0x10, 0x00, 0xf3, 0x82,
  0xff, 0x0c, 0x44, 0x10, 0x00,
  0x43, 0x23, 0x37, 0xf1, 0x3a, 0x44, 0x10, 0x02,
  0x62, 0x75, 0xb2, 0x48, 0x44, 0xd0, 0x00, 0x23,
  0x69, 0x92, 0x7a, 0x44, 0x10, 0x00, 0x63, 0x82,
  0xf6, 0x18, 0x44, 0x10, 0x00,
  0x83, 0x22, 0x86, 0xe5, 0x38, 0x00, 0x10, 0x05,
  0x12, 0x84, 0xe6, 0x9d, 0x00, 0x20, 0x40, 0x12,
  0x82, 0xe6, 0x8c, 0x01, 0x10, 0x00, 0x22, 0x82,
  0xe9, 0x00, 0x07, 0x10, 0x00,
  0x45, 0x72, 0x85, 0xf6, 0x50, 0x44, 0x20, 0x06,
  0x72, 0x81, 0xfc, 0x01, 0x44, 0x20, 0x00, 0x72,
  0x73, 0xf6, 0x50, 0x44, 0x10, 0x06, 0x72, 0x82,
  0xfc, 0x01, 0x44, 0x10, 0x00,
  0x85, 0x03, 0x30, 0x60, 0x4a, 0x10, 0x10, 0x62,
  0x02, 0x74, 0x62, 0x0c, 0x01, 0x15, 0x00, 0x73,
  0xa5, 0xe0, 0x19, 0x00, 0x40, 0x32, 0x72, 0x77,
  0x6f, 0x0e, 0x03, 0x11, 0x00,
  0x85, 0x03, 0x30, 0x60, 0x26, 0x00, 0x10, 0x0a,
  0x02, 0x76, 0x61, 0x0c, 0x13, 0x10, 0x00, 0x73,
  0x76, 0xe0, 0x21, 0x10, 0x10, 0x32, 0x72, 0x77,
  0x6f, 0x0e, 0x03, 0x10, 0x00,
  0x83, 0x02, 0x66, 0xf0, 0x40, 0x03, 0x10, 0x0c,
  0xf2, 0xe5, 0xff, 0x50, 0x43, 0x50, 0x00, 0x02,
  0x75, 0xf2, 0xb4, 0x13, 0x10, 0x00, 0x02, 0x73,
  0x61, 0x06, 0x13, 0x30, 0x00,
  0x83, 0x00, 0x26, 0xf0, 0x64, 0x01, 0x10, 0x06,
  0xf0, 0xe6, 0xff, 0x54, 0x43, 0x50, 0x88, 0x00,
  0x46, 0xf3, 0x6c, 0x13, 0x30, 0x00, 0x00, 0x73,
  0x61, 0x02, 0x01, 0x20, 0x00,
  0xc5, 0x03, 0x32, 0x71, 0x5a, 0x10, 0x12, 0xa3,
  0x02, 0x63, 0x61, 0x08, 0x33, 0x20, 0x00, 0x03,
  0x43, 0x70, 0x59, 0x00, 0x14, 0x63, 0x02, 0x63,
  0x61, 0x08, 0x53, 0x10, 0x00,
  0x85, 0x52, 0xbb, 0xf9, 0x50, 0x00, 0x10, 0x07,
  0x82, 0x77, 0xe2, 0x00, 0x07, 0x10, 0x00, 0x62,
  0x57, 0xff, 0x45, 0x00, 0x10, 0x40, 0x52, 0x66,
  0xcf, 0x00, 0x07, 0x10, 0x00,
  0x43, 0x52, 0x88, 0xf4, 0xa4, 0x00, 0x20, 0x06,
  0x72, 0x98, 0xb4, 0x84, 0x00, 0x50, 0x00, 0x32,
  0x27, 0xb4, 0x86, 0x00, 0x10, 0x05, 0x23, 0x24,
  0xf1, 0x10, 0x44, 0x10, 0x00,
  0x43, 0x43, 0x38, 0xf3, 0x11, 0x44, 0x10, 0x03,
  0x23, 0x22, 0xff, 0x84, 0x44, 0x07, 0x00, 0x33,
  0x37, 0xf0, 0x72, 0x44, 0x10, 0x00, 0x33, 0x34,
  0xff, 0x00, 0x44, 0x00, 0x40,
  0x87, 0x00, 0x6a, 0x50, 0x2e, 0x30, 0x11, 0x3a,
  0x00, 0x66, 0xc1, 0x60, 0x00, 0x20, 0x20, 0x00,
  0x66, 0x61, 0x1e, 0x03, 0x15, 0x00, 0x00, 0x65,
  0x61, 0x14, 0x33, 0x23, 0x60,
  0x87, 0x02, 0x5a, 0x60, 0x00, 0x10, 0x13, 0x4b,
  0x02, 0x56, 0xc1, 0x5c, 0x30, 0x10, 0x10, 0x02,
  0x66, 0x50, 0x02, 0x05, 0x15, 0x08, 0x02, 0x65,
  0x51, 0x1c, 0x03, 0x17, 0xd8,
  0x85, 0x02, 0x28, 0x91, 0x6c, 0x10, 0x16, 0x00,
  0x02, 0x5f, 0x70, 0x1c, 0x03, 0x13, 0x00, 0x03,
  0x2b, 0x90, 0x50, 0x10, 0x13, 0x20, 0x02, 0x4f,
  0x60, 0x01, 0x13, 0x17, 0x00,
  0x85, 0x02, 0x28, 0x91, 0x4c, 0x10, 0x10, 0x4d,
  0x02, 0x56, 0x60, 0x18, 0x03, 0x10, 0x00, 0x02,
  0x28, 0x80, 0x28, 0x10, 0x10, 0xd8, 0x02, 0x47,
  0x53, 0x01, 0x13, 0x10, 0x00,
  0x45, 0x02, 0x00, 0xcf, 0x4c, 0x00, 0x60, 0x3d,
  0x02, 0x53, 0x66, 0x5e, 0x03, 0x40, 0x00, 0x02,
  0x3f, 0x70, 0x79, 0x00, 0x10, 0x40, 0x02, 0x5f,
  0x50, 0x00, 0x03, 0x20, 0x00,
  0x85, 0x03, 0x00, 0xcf, 0x50, 0x30, 0x50, 0x3f,
  0x02, 0x53, 0x76, 0x52, 0x03, 0x40, 0x00, 0x02,
  0x47, 0x73, 0x6a, 0x10, 0x10, 0x50, 0x02, 0x51,
  0x90, 0x00, 0x05, 0x10, 0x00,
  0x85, 0x02, 0x40, 0xaf, 0x58, 0x00, 0x10, 0x00,
  0x02, 0x5f, 0x71, 0x22, 0x03, 0x10, 0x00, 0x02,
  0x4f, 0x90, 0x69, 0x10, 0x10, 0x48, 0x02, 0x5f,
  0x70, 0x02, 0x13, 0x30, 0x00,
  0x87, 0x43, 0x64, 0xf6, 0x00, 0x30, 0x40, 0x45,
  0x32, 0x35, 0xc1, 0x00, 0x00, 0x10, 0x30, 0x62,
  0x67, 0xc0, 0x00, 0x03, 0x10, 0x30, 0x72, 0x67,
  0xb0, 0x00, 0x03, 0x00, 0x30,
  0x43, 0x02, 0x58, 0x81, 0x50, 0x01, 0x10, 0x0e,
  0x02, 0x58, 0xa4, 0x5e, 0x01, 0x30, 0x00, 0x02,
  0x67, 0x71, 0x6a, 0x00, 0x10, 0x00, 0x02, 0x8f,
  0x90, 0x18, 0x11, 0x10, 0x00,
  0x43, 0x02, 0x76, 0x71, 0x70, 0x01, 0x10, 0x47,
  0x02, 0x58, 0x94, 0x3e, 0x01, 0x10, 0x00, 0x02,
  0x77, 0x61, 0x6a, 0x01, 0x10, 0x08, 0x02, 0x8f,
  0x80, 0x14, 0x03, 0x10, 0x00,
  0x43, 0x02, 0x65, 0x62, 0x88, 0x01, 0x10, 0x47,
  0x02, 0xb8, 0xc4, 0x62, 0x01, 0x10, 0x08, 0x02,
  0x97, 0x93, 0x46, 0x01, 0x10, 0x08, 0x02, 0x8f,
  0x70, 0x00, 0x03, 0x20, 0x00,
  0x45, 0x02, 0x70, 0x75, 0x68, 0x10, 0x30, 0x50,
  0x02, 0x9d, 0x94, 0x00, 0x01, 0x00, 0x10, 0x02,
  0x69, 0x71, 0x4c, 0x11, 0x50, 0x88, 0x02, 0x97,
  0x82, 0x00, 0x01, 0x10, 0x10,
  0x45, 0x02, 0x09, 0x70, 0x43, 0x00, 0x12, 0x40,
  0x02, 0x7e, 0x90, 0x06, 0x01, 0x17, 0x00, 0x02,
  0x29, 0x61, 0x5a, 0x30, 0x16, 0x04, 0x02, 0x7e,
  0xa0, 0x06, 0x01, 0x13, 0x40,
  0x45, 0x02, 0x26, 0x81, 0x58, 0x44, 0x17, 0x06,
  0x02, 0x8f, 0x90, 0x20, 0x40, 0x17, 0x00, 0x02,
  0x57, 0x71, 0x58, 0x41, 0x10, 0x66, 0x02, 0x88,
  0x90, 0x1c, 0x41, 0x10, 0x00,
  0x45, 0x02, 0x86, 0x72, 0x40, 0x44, 0x17, 0x06,
  0x02, 0xaf, 0x90, 0x28, 0x44, 0x17, 0x00, 0x02,
  0x86, 0x72, 0x40, 0x41, 0x10, 0x06, 0x02, 0xa8,
  0x90, 0x28, 0x41, 0x10, 0x00,
  0x43, 0x02, 0x43, 0x61, 0x70, 0x01, 0x10, 0x06,
  0x02, 0x57, 0x97, 0x9c, 0x01, 0x60, 0x08, 0x32,
  0x35, 0x7b, 0x8c, 0x01, 0x10, 0x40, 0x02, 0x7f,
  0xf0, 0x10, 0x03, 0x10, 0x00,
  0x43, 0x63, 0x69, 0xf3, 0x74, 0x10, 0x30, 0x00,
  0x02, 0x62, 0x80, 0x68, 0x00, 0x10, 0x00, 0x02,
  0x05, 0x80, 0x31, 0x10, 0x10, 0x08, 0x02, 0x86,
  0x81, 0x0c, 0x03, 0x10, 0x00,
  0x45, 0x07, 0x03, 0x90, 0x2a, 0x44, 0x10, 0x0c,
  0x06, 0x92, 0x80, 0x24, 0x41, 0x10, 0x00, 0x07,
  0x03, 0x90, 0x36, 0x44, 0x10, 0x4c, 0x06, 0x92,
  0x90, 0x54, 0x41, 0x10, 0x08,
  0x45, 0x07, 0x03, 0x70, 0x16, 0x44, 0x10, 0x0b,
  0x06, 0x92, 0x70, 0x3c, 0x43, 0x10, 0x40, 0x07,
  0x03, 0x70, 0x22, 0x44, 0x10, 0x4b, 0x06, 0x92,
  0x70, 0x36, 0x43, 0x10, 0x00,
  0x45, 0x03, 0x53, 0x70, 0x4a, 0x00, 0x10, 0x06,
  0x03, 0x82, 0x72, 0x18, 0x03, 0x20, 0x00, 0x03,
  0x15, 0x70, 0x39, 0x10, 0x20, 0x10, 0x03, 0x84,
  0x71, 0x14, 0x05, 0x10, 0x00,
  0x85, 0x02, 0x40, 0xa0, 0x7a, 0x03, 0x10, 0x28,
  0x12, 0x91, 0x90, 0x24, 0x03, 0x30, 0x00, 0x02,
  0x40, 0xb2, 0x60, 0x23, 0x10, 0x00, 0x02, 0xa0,
  0xa0, 0x38, 0x03, 0x20, 0x00,
  0x45, 0x02, 0x40, 0xa0, 0x8a, 0x03, 0x10, 0x28,
  0x12, 0x91, 0x91, 0x2c, 0x03, 0x30, 0x00, 0x02,
  0x40, 0xb2, 0x60, 0x23, 0x10, 0x00, 0x12, 0xa0,
  0xa1, 0x2c, 0x03, 0x20, 0x00,
  0x85, 0x07, 0x07, 0xc1, 0x62, 0x00, 0x10, 0x08,
  0x07, 0x81, 0x71, 0x02, 0x11, 0x30, 0x48, 0x07,
  0x07, 0xc1, 0x62, 0x00, 0x10, 0x08, 0x07, 0x81,
  0x71, 0x0c, 0x11, 0x31, 0x48,
  0x45, 0x03, 0x12, 0x71, 0x96, 0x11, 0x20, 0x07,
  0x02, 0x82, 0x81, 0x0c, 0x11, 0x10, 0x00, 0x03,
  0x12, 0x51, 0x69, 0x11, 0x40, 0x00, 0x02, 0x82,
  0x71, 0x0c, 0x00, 0x10, 0x00,
  0x85, 0x02, 0x7c, 0xa1, 0x30, 0x00, 0x50, 0x17,
  0x02, 0x87, 0x9f, 0x9c, 0x13, 0x10, 0x00, 0x02,
  0x75, 0x81, 0x78, 0x10, 0x10, 0x08, 0x02, 0xa5,
  0x80, 0x18, 0x01, 0x10, 0x00,
  0x45, 0x02, 0x1a, 0xd1, 0x1c, 0x10, 0x30, 0x07,
  0x02, 0xb8, 0x73, 0x94, 0x01, 0x30, 0x00, 0x02,
  0x98, 0xe0, 0x9c, 0x31, 0x10, 0x07, 0x02, 0xa5,
  0x60, 0x04, 0x11, 0x10, 0x80,
  0x85, 0x72, 0xa6, 0x90, 0xe8, 0x35, 0x20, 0x1f,
  0x02, 0xa5, 0x80, 0x10, 0x15, 0x10, 0x40, 0x62,
  0x69, 0xa9, 0x3c, 0x01, 0x70, 0xc0, 0x02, 0xa5,
  0x80, 0x90, 0x35, 0x10, 0x00,
  0x85, 0x02, 0x60, 0xa0, 0x01, 0x20, 0xd0, 0x07,
  0x12, 0xaa, 0xb0, 0x8c, 0x21, 0xa0, 0x18, 0x02,
  0x4f, 0x80, 0xb0, 0x15, 0x20, 0x00, 0x02, 0x90,
  0x80, 0x14, 0x25, 0x10, 0x00,
  0x45, 0x03, 0x7c, 0xc1, 0x30, 0x00, 0x50, 0x57,
  0x02, 0x97, 0x76, 0x6c, 0x01, 0x10, 0x00, 0x02,
  0x88, 0x73, 0x2f, 0x30, 0x20, 0x40, 0x02, 0x85,
  0x70, 0x04, 0x05, 0x10, 0x00,
  0x45, 0x02, 0x5c, 0xa1, 0x18, 0x00, 0x50, 0x17,
  0x02, 0x97, 0x65, 0x5c, 0x31, 0x10, 0x40, 0x02,
  0x38, 0xa3, 0x09, 0x50, 0x00, 0x90, 0x02, 0x95,
  0x60, 0x04, 0x05, 0x10, 0x00,
  0x42, 0x02, 0x6a, 0x60, 0x14, 0x15, 0x10, 0x00,
  0x02, 0x78, 0x80, 0x14, 0x55, 0x10, 0x00, 0x92,
  0x7a, 0x60, 0xb0, 0x55, 0x10, 0x88, 0x02, 0x78,
  0x60, 0x14, 0x55, 0x17, 0x40,
  0x85, 0x72, 0x86, 0x80, 0xf1, 0x15, 0x20, 0x1f,
  0x02, 0x95, 0x80, 0x00, 0x05, 0x10, 0x00, 0x62,
  0x69, 0xa9, 0x3c, 0x01, 0x10, 0xc0, 0x02, 0xa5,
  0x70, 0x68, 0x15, 0x10, 0x40,
  0x85, 0x02, 0x7f, 0xf4, 0xbb, 0x00, 0x10, 0x40,
  0x02, 0xaf, 0xa0, 0x0e, 0x03, 0x10, 0x40, 0x02,
  0x2f, 0xf3, 0x9b, 0x10, 0x20, 0x41, 0x02, 0xaf,
  0xa0, 0x0e, 0x01, 0x10, 0x40,
  0x45, 0x02, 0x70, 0xf0, 0x68, 0x41, 0x17, 0x07,
  0x02, 0x7f, 0xd0, 0x28, 0x41, 0x17, 0x40, 0x02,
  0x3f, 0xf0, 0x51, 0x41, 0x10, 0xa7, 0x02, 0x8f,
  0xe0, 0x28, 0x41, 0x10, 0x40,
  0x85, 0x02, 0x74, 0xc8, 0x00, 0x30, 0x40, 0x47,
  0x02, 0x66, 0xc7, 0x51, 0x54, 0x40, 0x40, 0x02,
  0x56, 0x85, 0x0d, 0x01, 0x20, 0x00, 0x02, 0x84,
  0x61, 0x08, 0x30, 0x10, 0x80,
  0x45, 0x02, 0x27, 0x76, 0x11, 0x15, 0x14, 0x40,
  0x02, 0x86, 0xf0, 0x2c, 0x15, 0x10, 0x40, 0x02,
  0x27, 0x76, 0x0d, 0x15, 0x11, 0x40, 0x02, 0x86,
  0xf0, 0x2c, 0x15, 0x12, 0x40,
  0x45, 0x02, 0x62, 0x92, 0x22, 0x45, 0x14, 0x08,
  0x02, 0x81, 0x92, 0x38, 0x15, 0x20, 0x60, 0x02,
  0x62, 0x92, 0x2a, 0x45, 0x11, 0x08, 0x02, 0x81,
  0x92, 0x38, 0x15, 0x21, 0x60,
  0x45, 0x02, 0x00, 0x4f, 0x34, 0x00, 0x70, 0x30,
  0x02, 0x83, 0x76, 0x4c, 0x01, 0x20, 0x00, 0x02,
  0x9f, 0x70, 0x71, 0x30, 0x10, 0x40, 0x02, 0x8f,
  0x70, 0x08, 0x03, 0x10, 0x00,
  0x47, 0x12, 0x81, 0xc1, 0x54, 0x45, 0x10, 0xc0,
  0x12, 0x61, 0xc2, 0x3c, 0x45, 0x11, 0x00, 0x02,
  0x80, 0xc0, 0x68, 0x45, 0x24, 0x00, 0x12, 0x81,
  0xc1, 0x18, 0x45, 0x31, 0x80,
  0x45, 0x02, 0x32, 0xb0, 0x58, 0x00, 0x10, 0x08,
  0x02, 0x92, 0xa0, 0x44, 0x03, 0x10, 0x80, 0x42,
  0x43, 0xc5, 0x2e, 0x00, 0x10, 0x05, 0x02, 0x93,
  0xd6, 0x30, 0x00, 0x10, 0x00,
  0x45, 0x31, 0x3f, 0xf0, 0x98, 0x44, 0x70, 0x0d,
  0x40, 0x47, 0xf0, 0x2e, 0x44, 0x50, 0x00, 0x00,
  0x11, 0x60, 0x62, 0x03, 0x17, 0x0e, 0x00, 0x51,
  0x81, 0x02, 0x03, 0x10, 0x00,
  0x45, 0x02, 0x50, 0xa0, 0xa0, 0x01, 0x11, 0x07,
  0x02, 0x40, 0x30, 0x08, 0x05, 0x10, 0x00, 0x02,
  0x30, 0xa0, 0xbc, 0x01, 0x13, 0x07, 0x02, 0x40,
  0x30, 0x08, 0x05, 0x12, 0x00,
  0x45, 0x00, 0x45, 0x61, 0x88, 0x05, 0x17, 0x18,
  0x00, 0x65, 0xa1, 0x0a, 0x05, 0x26, 0x00, 0x00,
  0x33, 0x61, 0x88, 0x05, 0x10, 0xdc, 0x00, 0x53,
  0x90, 0x02, 0x05, 0x12, 0x40,
  0x45, 0x00, 0x00, 0xaf, 0x84, 0x45, 0x17, 0x42,
  0x00, 0x73, 0x43, 0x5a, 0x05, 0x80, 0x00, 0x00,
  0x33, 0x70, 0x85, 0x05, 0x10, 0x06, 0x00, 0x5f,
  0x60, 0x00, 0x05, 0x23, 0x00,
  0x45, 0x02, 0x31, 0x24, 0xaa, 0x15, 0x70, 0x04,
  0x02, 0x52, 0x63, 0x00, 0x11, 0x10, 0x00, 0x02,
  0x31, 0x24, 0xaa, 0x15, 0x71, 0x04, 0x02, 0x52,
  0x63, 0x00, 0x11, 0x12, 0x00,
  0x45, 0x03, 0x32, 0xf0, 0x5e, 0x00, 0x10, 0x56,
  0x02, 0x46, 0x50, 0x0e, 0x01, 0x15, 0x00, 0x03,
  0x36, 0xf3, 0x1d, 0x00, 0x10, 0x02, 0x02, 0x47,
  0x50, 0x11, 0x03, 0x11, 0x00,
  0x45, 0x00, 0x31, 0x41, 0x94, 0x44, 0x15, 0x06,
  0x00, 0x52, 0x60, 0x02, 0x01, 0x17, 0x00, 0x00,
  0x45, 0xc1, 0x78, 0x44, 0x13, 0x05, 0x00, 0x52,
  0x80, 0x02, 0x03, 0x10, 0x00,
  0x45, 0x02, 0x38, 0x40, 0x78, 0x10, 0x15, 0x00,
  0x02, 0x48, 0x30, 0x02, 0x03, 0x13, 0x00, 0x02,
  0x41, 0x30, 0x8a, 0x00, 0x22, 0x00, 0x02, 0x52,
  0x70, 0x02, 0x03, 0x17, 0x00,
  0x45, 0x03, 0x68, 0xf8, 0x0b, 0x55, 0xa0, 0x01,
  0x13, 0x25, 0x80, 0x00, 0x75, 0x10, 0x80, 0x03,
  0x68, 0xf8, 0x0b, 0x45, 0xa2, 0x01, 0x13, 0x25,
  0x80, 0x00, 0x55, 0x13, 0x80,
  0x85, 0x02, 0x33, 0x63, 0x4a, 0x10, 0x30, 0x03,
  0x02, 0x42, 0x40, 0x25, 0x01, 0x30, 0x00, 0x02,
  0x33, 0x63, 0x40, 0x00, 0x10, 0x85, 0x02, 0x41,
  0x41, 0x0e, 0x03, 0x10, 0x00,
  0x45, 0x12, 0x58, 0xf4, 0x51, 0x45, 0x60, 0x05,
  0x22, 0x42, 0xc7, 0x24, 0x44, 0x10, 0x00, 0x12,
  0x58, 0xf4, 0x51, 0x45, 0xe2, 0x05, 0x22, 0x42,
  0xc7, 0x24, 0x44, 0x12, 0x00,
  0x85, 0x22, 0x43, 0x6f, 0x55, 0x00, 0x10, 0xa4,
  0x02, 0x43, 0xf3, 0x00, 0x03, 0x10, 0x00, 0x02,
  0x43, 0xcf, 0x41, 0x00, 0x23, 0xa4, 0x52, 0x46,
  0x93, 0x30, 0x03, 0x27, 0x00,
  0x42, 0x12, 0x41, 0xf5, 0x1e, 0x15, 0x13, 0x58,
  0x52, 0x42, 0xff, 0x1c, 0x15, 0x13, 0x48, 0x12,
  0x41, 0xf5, 0x1e, 0x15, 0x11, 0x58, 0x52, 0x42,
  0xff, 0x1c, 0x15, 0x11, 0x48,
  0x45, 0x00, 0x11, 0x12, 0x4a, 0x47, 0x37, 0x6c,
  0x00, 0x41, 0x21, 0x26, 0x10, 0x30, 0x00, 0x00,
  0x21, 0x20, 0x50, 0x45, 0x10, 0x04, 0x00, 0x31,
  0x30, 0x0e, 0x45, 0x14, 0x00,
  0x85, 0x02, 0x03, 0x45, 0x88, 0x00, 0x20, 0x00,
  0x02, 0xc2, 0xa0, 0x3a, 0x03, 0x10, 0x00, 0x02,
  0x23, 0x32, 0x8c, 0x30, 0x10, 0x80, 0x02, 0x3f,
  0xa0, 0x00, 0x03, 0x10, 0x00,
  0x45, 0x02, 0x33, 0x58, 0x65, 0x11, 0x20, 0x0a,
  0x12, 0x41, 0x62, 0x18, 0x11, 0x17, 0x20, 0x02,
  0x33, 0x58, 0x65, 0x11, 0x23, 0x0a, 0x12, 0x41,
  0x62, 0x18, 0x11, 0x13, 0x20,
  0x45, 0x32, 0x22, 0xd5, 0x2a, 0x45, 0x20, 0x03,
  0x62, 0x42, 0xff, 0x20, 0x45, 0x70, 0x48, 0x32,
  0x22, 0xd5, 0x0e, 0x47, 0x11, 0x44, 0x62, 0x42,
  0xff, 0x20, 0x45, 0x41, 0x88,
  0x85, 0x23, 0x13, 0xd1, 0x28, 0x00, 0x10, 0x20,
  0x33, 0x53, 0xde, 0x02, 0x03, 0x30, 0x00, 0x42,
  0x27, 0xf1, 0x3c, 0x01, 0x60, 0x08, 0x82, 0x87,
  0xfe, 0x02, 0x03, 0x10, 0x08,
  0x43, 0x10, 0x31, 0xf2, 0x68, 0x44, 0x10, 0x44,
  0x50, 0x6a, 0xf7, 0x50, 0x45, 0x30, 0x00, 0x30,
  0x38, 0xf3, 0x60, 0x44, 0x50, 0x80, 0x41, 0x44,
  0xff, 0x04, 0x45, 0x30, 0x40,
  0x43, 0x50, 0x57, 0xf2, 0x52, 0x00, 0x30, 0x46,
  0x80, 0x89, 0xf4, 0x56, 0x00, 0x50, 0x80, 0x20,
  0x32, 0xff, 0xa8, 0x00, 0x10, 0x05, 0x21, 0x22,
  0xff, 0x0c, 0x44, 0x10, 0x00,
  0x45, 0x52, 0x6a, 0xfa, 0x21, 0x45, 0x43, 0x26,
  0x32, 0x52, 0xc0, 0x20, 0x45, 0x10, 0x00, 0x52,
  0x68, 0xfa, 0x39, 0x45, 0x52, 0x06, 0x43, 0x42,
  0xc0, 0x20, 0x45, 0x14, 0x00,
  0x45, 0x00, 0xc9, 0xb1, 0x42, 0x44, 0x10, 0x49,
  0x00, 0xdf, 0x70, 0x2c, 0x44, 0x31, 0x40, 0x00,
  0x99, 0xa3, 0x00, 0x44, 0x10, 0x00, 0x00, 0xd6,
  0x80, 0x28, 0x44, 0x40, 0x08,
  0x85, 0x03, 0x39, 0x81, 0x1e, 0x00, 0x10, 0x0a,
  0x02, 0x76, 0x62, 0x00, 0x41, 0x10, 0x00, 0x73,
  0x76, 0xc0, 0x25, 0x00, 0x20, 0x20, 0x72, 0x83,
  0x9e, 0x66, 0x05, 0x10, 0x00,
  0x45, 0x02, 0x40, 0xa0, 0x42, 0x03, 0x10, 0x28,
  0x12, 0x91, 0x90, 0x34, 0x03, 0x60, 0x00, 0x02,
  0x40, 0xb2, 0x60, 0x23, 0x10, 0x00, 0x02, 0xa0,
  0xa0, 0x3c, 0x03, 0x20, 0x00,
  0x45, 0x30, 0x46, 0xf5, 0x41, 0x44, 0xe0, 0x03,
  0x70, 0x66, 0xce, 0x2e, 0x44, 0x20, 0x00, 0x20,
  0x26, 0xc5, 0x78, 0x44, 0x77, 0x08, 0x40, 0x55,
  0xfd, 0x04, 0x54, 0x60, 0x00,
  0x45, 0x40, 0x4a, 0xe2, 0x5c, 0x44, 0x70, 0x01,
  0x60, 0x67, 0xf1, 0x20, 0x44, 0x50, 0x00, 0x60,
  0x49, 0xe2, 0x84, 0x44, 0xa7, 0x00, 0x60, 0x67,
  0xf7, 0x10, 0x44, 0x20, 0x00,
  0x47, 0x43, 0x54, 0x42, 0x00, 0x45, 0x20, 0x80,
  0x32, 0x46, 0x60, 0x58, 0x45, 0x20, 0x00, 0x43,
  0x44, 0xe2, 0x00, 0x45, 0x20, 0x05, 0x43, 0x44,
  0xf2, 0x00, 0x00, 0x03, 0x10,
  0x45, 0x91, 0x9a, 0xf2, 0x86, 0x44, 0x51, 0x05,
  0x71, 0x7a, 0xf2, 0x00, 0x44, 0x23, 0x00, 0xa0,
  0x8a, 0xf2, 0x70, 0x44, 0xa3, 0x00, 0x70, 0x7a,
  0xf2, 0x00, 0x44, 0x20, 0x00,
  0x44, 0x42, 0x3b, 0xf5, 0x60, 0x00, 0x00, 0x27,
  0xa2, 0x3c, 0xf1, 0x4c, 0x00, 0x40, 0x00, 0x42,
  0x3c, 0xf4, 0x2c, 0x01, 0x30, 0x00, 0x52, 0x5e,
  0xf0, 0x00, 0x05, 0x00, 0x00,
  0x45, 0x82, 0x89, 0xf5, 0x3a, 0x44, 0x10, 0x27,
  0x52, 0x53, 0xfe, 0x00, 0x44, 0x00, 0x00, 0x43,
  0x43, 0xf5, 0x32, 0x44, 0x00, 0x08, 0x43, 0x44,
  0xfe, 0x00, 0x44, 0x00, 0x00,
  0xc5, 0x81, 0xab, 0xe2, 0x00, 0x04, 0x07, 0x62,
  0x01, 0x43, 0xcf, 0x00, 0x14, 0x00, 0x00, 0x00,
  0x44, 0xf7, 0x00, 0x44, 0x00, 0xc0, 0x00, 0x77,
  0xff, 0x00, 0x45, 0x00, 0xb0,
  0x45, 0x02, 0x0f, 0x40, 0x00, 0x45, 0xe0, 0x07,
  0xf2, 0xff, 0x2f, 0x18, 0x25, 0x90, 0x40, 0x02,
  0x0f, 0x40, 0x00, 0x45, 0xe0, 0x07, 0xf2, 0xff,
  0x2f, 0x18, 0x45, 0xe0, 0x18,
  0x45, 0x42, 0x68, 0xf2, 0x02, 0x45, 0x60, 0x47,
  0x42, 0xa6, 0x88, 0x3e, 0x45, 0x30, 0x10, 0x22,
  0x66, 0xf2, 0x00, 0x45, 0x60, 0x51, 0x82, 0xa8,
  0x88, 0x02, 0x45, 0x60, 0x10,
  0x45, 0x02, 0x5c, 0xa1, 0x38, 0x00, 0x50, 0x17,
  0x82, 0x98, 0x7f, 0x14, 0x11, 0x10, 0x00, 0x02,
  0x5c, 0xa1, 0x24, 0x00, 0x80, 0x07, 0x72, 0x98,
  0x8f, 0x30, 0x11, 0x10, 0x00,
  0x03, 0x02, 0x0f, 0xf0, 0x10, 0x00, 0x30, 0x07,
  0x22, 0x22, 0xff, 0x54, 0x10, 0x00, 0x20, 0x42,
  0x40, 0xff, 0x30, 0x00, 0x00, 0x08, 0x42, 0x42,
  0x1f, 0x00, 0x00, 0x00, 0x00,
  0x85, 0x33, 0x37, 0x3a, 0x56, 0x55, 0x50, 0x00,
  0x73, 0x76, 0x53, 0x01, 0x55, 0xa0, 0x00, 0x33,
  0x37, 0x3a, 0x56, 0x55, 0x50, 0x00, 0x43, 0x46,
  0x53, 0x01, 0x75, 0xa7, 0x00,
  0x45, 0x03, 0x02, 0xb6, 0x71, 0x45, 0x50, 0x15,
  0x33, 0x34, 0xf1, 0x0c, 0x45, 0x40, 0x00, 0x03,
  0x02, 0xb6, 0x71, 0x45, 0x50, 0x15, 0x33, 0x34,
  0xf1, 0x3c, 0x45, 0x47, 0x00,
  0xc5, 0x02, 0x06, 0xf0, 0x04, 0x55, 0xf0, 0x75,
  0x02, 0x50, 0x20, 0x10, 0x55, 0x00, 0x00, 0x02,
  0x00, 0xf0, 0x60, 0x44, 0x00, 0x70, 0x02, 0x50,
  0x20, 0x2c, 0x44, 0x00, 0x00,
  0x45, 0x02, 0x1c, 0xf0, 0x00, 0x00, 0x00, 0xc7,
  0x02, 0x52, 0x40, 0x20, 0x00, 0x00, 0x00, 0x02,
  0x10, 0x6f, 0x00, 0x30, 0x90, 0xc7, 0x72, 0x70,
  0x3f, 0x1c, 0x00, 0x30, 0x00,
  0x45, 0x02, 0x03, 0xff, 0x2c, 0x01, 0x50, 0x07,
  0x82, 0x86, 0xfb, 0x10, 0x11, 0xf0, 0x30, 0x02,
  0x02, 0xff, 0x08, 0x11, 0x50, 0x0f, 0x82, 0x86,
  0xfb, 0x68, 0x11, 0x50, 0x30
};

const uint8 audfmt_smaf_fm_2op_dls_lookup[129][15] = {
  0x40, 0x27, 0x34, 0xf2, 0x42, 0x40, 0x10, 0x2b,
  0x17, 0x62, 0xf3, 0x02, 0x41, 0x10, 0x00,
  0x40, 0x27, 0x34, 0xf3, 0x41, 0x40, 0x10, 0x6c,
  0x17, 0x62, 0xf3, 0x10, 0x41, 0x10, 0x00,
  0x40, 0x17, 0x51, 0xf3, 0x64, 0x44, 0x10, 0x06,
  0x27, 0x61, 0xf6, 0x20, 0x41, 0x10, 0x00,
  0xc0, 0x26, 0x53, 0xf4, 0x41, 0x31, 0x17, 0x21,
  0x37, 0x73, 0xf5, 0x00, 0x10, 0x13, 0x00,
  0x40, 0x06, 0x41, 0xe5, 0x4d, 0x20, 0x10, 0x05,
  0x26, 0x82, 0xd6, 0x00, 0x21, 0x10, 0x00,
  0x40, 0x36, 0x4d, 0xf7, 0x1d, 0x40, 0x43, 0x15,
  0x36, 0x83, 0xbf, 0x08, 0x21, 0x11, 0x00,
  0x40, 0x17, 0xf3, 0xc1, 0x51, 0x44, 0x30, 0x52,
  0x36, 0x73, 0xdf, 0x1c, 0x41, 0x10, 0xa8,
  0x40, 0x27, 0x82, 0xf1, 0x80, 0x42, 0x30, 0x2b,
  0x27, 0xa2, 0xf2, 0x1c, 0x41, 0x10, 0x20,
  0xc0, 0x27, 0x15, 0xfc, 0xa9, 0x06, 0xc0, 0x00,
  0x46, 0x55, 0xff, 0x08, 0x01, 0x10, 0x00,
  0x41, 0x27, 0x55, 0xf6, 0x08, 0x44, 0x70, 0x00,
  0x27, 0x24, 0xf1, 0x01, 0x41, 0x10, 0x00,
  0x40, 0x27, 0x4a, 0x95, 0xa0, 0x44, 0x70, 0x41,
  0x26, 0x43, 0xff, 0x02, 0x41, 0x10, 0x00,
  0x40, 0x27, 0x35, 0xf1, 0x59, 0x01, 0x70, 0x01,
  0x26, 0x62, 0xff, 0x02, 0x30, 0x10, 0x00,
  0x40, 0x57, 0x5a, 0xf4, 0x58, 0x44, 0x50, 0x02,
  0x27, 0x43, 0xed, 0x0c, 0x44, 0x10, 0x00,
  0x40, 0x73, 0x0b, 0xb5, 0x48, 0x44, 0x70, 0x34,
  0x46, 0x76, 0xfe, 0x00, 0x44, 0x10, 0x00,
  0x40, 0x17, 0x1b, 0xf0, 0x3a, 0x44, 0x71, 0x86,
  0x36, 0x39, 0xf0, 0x30, 0x14, 0x10, 0x40,
  0x40, 0x36, 0x54, 0xd5, 0x3e, 0x44, 0x37, 0xa3,
  0x36, 0x32, 0xf2, 0x1e, 0x41, 0x10, 0x00,
  0x41, 0x06, 0xa0, 0xd0, 0x38, 0x41, 0x00, 0x21,
  0x06, 0x91, 0xf1, 0x1c, 0x01, 0x20, 0x48,
  0x40, 0x02, 0x99, 0xc5, 0x7c, 0x01, 0x40, 0x00,
  0x06, 0x80, 0xa0, 0x00, 0x01, 0x00, 0x00,
  0x80, 0x06, 0xab, 0xf1, 0x68, 0x10, 0x00, 0x8b,
  0x06, 0xba, 0xf0, 0x14, 0x11, 0x20, 0x00,
  0x40, 0x06, 0x37, 0xa2, 0x3c, 0x41, 0x50, 0x8a,
  0x07, 0x45, 0x41, 0x28, 0x43, 0x10, 0x00,
  0x40, 0x06, 0x47, 0x70, 0x40, 0x45, 0x10, 0x08,
  0x07, 0x84, 0x40, 0x16, 0x44, 0x10, 0x00,
  0x40, 0x06, 0xa8, 0x90, 0x68, 0x44, 0x17, 0x21,
  0x07, 0xa0, 0x51, 0x1c, 0x01, 0x23, 0x80,
  0x40, 0x06, 0x31, 0x90, 0x70, 0x44, 0x10, 0x2c,
  0x06, 0x91, 0x60, 0x20, 0x41, 0x30, 0x00,
  0x41, 0x06, 0x81, 0x63, 0x14, 0x12, 0x23, 0x62,
  0x06, 0x81, 0x61, 0x34, 0x11, 0x17, 0xc8,
  0x40, 0x27, 0x33, 0xf2, 0x94, 0x46, 0x20, 0x47,
  0x17, 0x83, 0xf2, 0x08, 0x41, 0x10, 0x40,
  0x40, 0x17, 0x44, 0xf7, 0x90, 0x41, 0x30, 0xaf,
  0x17, 0x62, 0xf5, 0x08, 0x41, 0x10, 0x40,
  0x40, 0x17, 0x66, 0xb2, 0x69, 0x41, 0x30, 0x07,
  0x26, 0x92, 0xcf, 0x00, 0x45, 0x10, 0x00,
  0x40, 0x36, 0x37, 0xf2, 0x1e, 0x01, 0x32, 0x8b,
  0x36, 0x93, 0xf4, 0x0e, 0x01, 0x10, 0x00,
  0x40, 0x26, 0x79, 0xa3, 0x46, 0x42, 0x00, 0x56,
  0x27, 0x82, 0xd5, 0x18, 0x41, 0x20, 0x00,
  0x80, 0x06, 0x98, 0xd2, 0x25, 0x40, 0x10, 0x23,
  0x16, 0x94, 0xd1, 0x10, 0x41, 0x20, 0x20,
  0x40, 0x06, 0x28, 0x73, 0x24, 0x42, 0x10, 0x0d,
  0x06, 0x91, 0xd3, 0x1e, 0x44, 0x20, 0x30,
  0x40, 0x82, 0x72, 0x90, 0x45, 0x41, 0x00, 0x03,
  0x56, 0x93, 0xbf, 0x20, 0x41, 0x20, 0x40,
  0x40, 0x16, 0x24, 0xb1, 0x13, 0x40, 0x10, 0x02,
  0x36, 0x94, 0xd6, 0x00, 0x41, 0x10, 0x00,
  0x80, 0x17, 0x89, 0xf1, 0x4a, 0x41, 0x10, 0x45,
  0x27, 0x84, 0x75, 0x00, 0x45, 0x20, 0x00,
  0x80, 0x17, 0x89, 0xa2, 0x3a, 0x41, 0x10, 0x45,
  0x27, 0x84, 0xf4, 0x00, 0x45, 0x40, 0x00,
  0x80, 0x17, 0x82, 0xb1, 0x60, 0x45, 0x11, 0x05,
  0x27, 0x82, 0xbf, 0x08, 0x41, 0x40, 0x00,
  0x80, 0x26, 0xa8, 0xf3, 0x30, 0x45, 0x30, 0x43,
  0x36, 0xf5, 0xf4, 0x10, 0x45, 0x10, 0x40,
  0x80, 0x26, 0xa8, 0xf3, 0x38, 0x41, 0x40, 0x03,
  0x36, 0xf4, 0xf4, 0x12, 0x41, 0x10, 0x40,
  0x40, 0x17, 0x23, 0xf2, 0x2e, 0x41, 0x10, 0x05,
  0x17, 0x71, 0xf4, 0x08, 0x40, 0x10, 0x00,
  0x40, 0x06, 0x46, 0xf2, 0x32, 0x41, 0x10, 0x29,
  0x06, 0x93, 0xd4, 0x00, 0x41, 0x11, 0x40,
  0x80, 0x06, 0x35, 0xd3, 0x44, 0x40, 0x10, 0xa3,
  0x06, 0x76, 0x50, 0x10, 0x01, 0x10, 0x00,
  0x80, 0x06, 0x15, 0xa1, 0x40, 0x00, 0x10, 0x0c,
  0x06, 0x75, 0x60, 0x18, 0x01, 0x20, 0x00,
  0x80, 0x06, 0x44, 0x61, 0x70, 0x01, 0x10, 0x2d,
  0x06, 0x61, 0x60, 0x00, 0x03, 0x10, 0x80,
  0x80, 0x06, 0x51, 0x91, 0x49, 0x40, 0x10, 0xaa,
  0x06, 0x73, 0x90, 0x00, 0x41, 0x10, 0x40,
  0xc0, 0x07, 0x11, 0xf1, 0x1d, 0x44, 0x10, 0xac,
  0x06, 0x6f, 0x61, 0x10, 0x34, 0x10, 0x00,
  0x41, 0x57, 0x56, 0x72, 0x00, 0x44, 0x10, 0x63,
  0x76, 0x75, 0xd2, 0x00, 0x44, 0x10, 0x20,
  0x40, 0x35, 0x25, 0xf3, 0x88, 0x40, 0x20, 0x0e,
  0x35, 0x22, 0xff, 0x00, 0x41, 0x10, 0x00,
  0x40, 0x54, 0x4b, 0xf0, 0x50, 0x44, 0x20, 0x86,
  0x44, 0x49, 0xd0, 0x00, 0x14, 0x00, 0x40,
  0x80, 0x06, 0x11, 0xb1, 0x6e, 0x04, 0x10, 0x40,
  0x06, 0x62, 0x70, 0x10, 0x11, 0x20, 0x80,
  0x80, 0x06, 0x11, 0xb1, 0x5a, 0x04, 0x10, 0x40,
  0x06, 0x62, 0x60, 0x10, 0x11, 0x20, 0x80,
  0x40, 0x06, 0x11, 0xb1, 0x7e, 0x04, 0x15, 0x41,
  0x06, 0x52, 0x60, 0x10, 0x11, 0x20, 0xa0,
  0x80, 0x06, 0x21, 0x82, 0x80, 0x44, 0x10, 0x6e,
  0x06, 0x4f, 0x60, 0x02, 0x11, 0x10, 0x40,
  0x41, 0x06, 0x55, 0x60, 0x18, 0x23, 0x10, 0x44,
  0x06, 0x5a, 0x50, 0x24, 0x01, 0x20, 0x80,
  0x80, 0x06, 0x16, 0x50, 0x94, 0x41, 0x10, 0x00,
  0x06, 0x65, 0x62, 0x00, 0x43, 0x20, 0x00,
  0x80, 0x16, 0x22, 0x81, 0x52, 0x03, 0x10, 0x05,
  0x16, 0x53, 0x61, 0x10, 0x01, 0x40, 0x00,
  0x40, 0x45, 0x44, 0x56, 0x00, 0x54, 0x10, 0x04,
  0x64, 0x65, 0xa4, 0x18, 0x41, 0x20, 0x30,
  0x80, 0x06, 0x05, 0x91, 0x42, 0x41, 0x10, 0x06,
  0x06, 0x9f, 0xe0, 0x18, 0x40, 0x10, 0x00,
  0x40, 0x06, 0x05, 0x81, 0x4e, 0x40, 0x10, 0x06,
  0x16, 0x90, 0x70, 0x00, 0x43, 0x10, 0x00,
  0x80, 0x06, 0x06, 0xa1, 0x3a, 0x41, 0x10, 0x05,
  0x06, 0x72, 0x63, 0x00, 0x47, 0x10, 0x00,
  0x40, 0x06, 0x08, 0x71, 0x5e, 0x41, 0x50, 0x15,
  0x06, 0xa3, 0x80, 0x20, 0x43, 0x10, 0x28,
  0x80, 0x06, 0x02, 0x63, 0x56, 0x02, 0x10, 0x04,
  0x06, 0x72, 0xd0, 0x00, 0x21, 0x10, 0x00,
  0x80, 0x06, 0x05, 0x71, 0x2a, 0x41, 0x10, 0x45,
  0x06, 0x8c, 0xb0, 0x14, 0x40, 0x10, 0x00,
  0x80, 0x06, 0x04, 0x83, 0x50, 0x41, 0x11, 0x0e,
  0x06, 0x90, 0xa0, 0x14, 0x40, 0x10, 0x40,
  0x80, 0x06, 0x04, 0x62, 0x5a, 0x40, 0x10, 0x46,
  0x06, 0x63, 0xa2, 0x00, 0x41, 0x10, 0x00,
  0x40, 0x07, 0x03, 0x90, 0x56, 0x44, 0x10, 0x4d,
  0x06, 0xb2, 0x80, 0x14, 0x41, 0x10, 0x00,
  0x40, 0x07, 0x03, 0x90, 0x36, 0x44, 0x10, 0x4c,
  0x06, 0x92, 0x80, 0x18, 0x41, 0x10, 0x00,
  0x40, 0x07, 0x03, 0x70, 0x16, 0x44, 0x10, 0x0b,
  0x06, 0x92, 0x70, 0x1c, 0x43, 0x10, 0x00,
  0x40, 0x07, 0x03, 0x90, 0x2a, 0x44, 0x10, 0x43,
  0x06, 0xf2, 0x80, 0x0c, 0x13, 0x10, 0x00,
  0x80, 0x06, 0x01, 0xd2, 0x82, 0x14, 0x10, 0x40,
  0x06, 0x82, 0x80, 0x0c, 0x41, 0x30, 0x08,
  0x80, 0x07, 0x0e, 0x93, 0x62, 0x00, 0x10, 0x43,
  0x06, 0x8b, 0xa0, 0x00, 0x01, 0x10, 0x28,
  0x80, 0x07, 0x07, 0xc1, 0x5e, 0x00, 0x10, 0x08,
  0x07, 0x81, 0x71, 0x00, 0x11, 0x30, 0x48,
  0x80, 0x06, 0x00, 0x90, 0x84, 0x44, 0x20, 0x02,
  0x06, 0x80, 0x80, 0x00, 0x21, 0x10, 0x00,
  0x81, 0x06, 0xb9, 0x8f, 0x0c, 0x35, 0x10, 0x93,
  0x06, 0xc3, 0x81, 0x00, 0x30, 0x10, 0x00,
  0x80, 0x06, 0x58, 0x85, 0x1d, 0x01, 0x10, 0x88,
  0x06, 0x75, 0x60, 0x00, 0x33, 0x10, 0x00,
  0x80, 0x76, 0xa6, 0x90, 0xcc, 0x10, 0x20, 0x18,
  0x06, 0xa5, 0x80, 0x10, 0x13, 0x10, 0x40,
  0x40, 0x06, 0x06, 0xc4, 0x0f, 0x13, 0x20, 0x16,
  0x06, 0x88, 0x71, 0x0c, 0x11, 0x10, 0x40,
  0x40, 0x06, 0x67, 0x6a, 0x74, 0x11, 0x10, 0x1f,
  0x06, 0x70, 0xb0, 0x00, 0x12, 0x10, 0x00,
  0x40, 0x47, 0x65, 0xf8, 0x24, 0x50, 0x10, 0x27,
  0x06, 0x90, 0x60, 0x04, 0x11, 0x10, 0x00,
  0x41, 0x06, 0x87, 0xf0, 0x70, 0x11, 0x10, 0xa5,
  0x06, 0x78, 0x80, 0x00, 0x11, 0x10, 0x00,
  0x41, 0x16, 0x7a, 0x8a, 0x79, 0x11, 0x30, 0x03,
  0x06, 0x96, 0x80, 0x00, 0x10, 0x10, 0x00,
  0x41, 0x06, 0x9f, 0xd0, 0x35, 0x43, 0x14, 0x30,
  0x06, 0x9f, 0xd0, 0x04, 0x41, 0x10, 0x40,
  0x41, 0x06, 0x80, 0xf0, 0x28, 0x41, 0x17, 0xc0,
  0x06, 0x80, 0xf0, 0x5c, 0x45, 0x10, 0xd8,
  0x40, 0x06, 0x57, 0xca, 0x2c, 0x40, 0x25, 0x18,
  0x06, 0x80, 0x90, 0x00, 0x31, 0x00, 0x90,
  0x40, 0x06, 0x27, 0x76, 0x21, 0x10, 0x14, 0x40,
  0x06, 0x86, 0xf0, 0x0c, 0x11, 0x10, 0x40,
  0x40, 0x06, 0xb2, 0x91, 0x22, 0x40, 0x14, 0x08,
  0x06, 0xa1, 0x92, 0x24, 0x01, 0x20, 0x00,
  0x80, 0x06, 0x51, 0x61, 0x23, 0x00, 0x35, 0x81,
  0x06, 0x7f, 0xd0, 0x18, 0x11, 0x10, 0x00,
  0x41, 0x06, 0x7f, 0xa0, 0x0d, 0x41, 0x30, 0x25,
  0x06, 0x7f, 0xb0, 0x08, 0x43, 0x10, 0x20,
  0x40, 0x06, 0x84, 0xd4, 0x29, 0x04, 0x14, 0x68,
  0x06, 0x84, 0xf0, 0x18, 0x01, 0x10, 0x80,
  0x41, 0x07, 0x42, 0xf6, 0x0a, 0x40, 0x41, 0x50,
  0x06, 0x40, 0x60, 0x00, 0x11, 0x10, 0xa0,
  0x41, 0x06, 0x50, 0x50, 0x00, 0x45, 0x15, 0xa0,
  0x07, 0x30, 0x50, 0x02, 0x71, 0x10, 0x80,
  0x40, 0x04, 0x41, 0xd3, 0x41, 0x00, 0x15, 0x46,
  0x04, 0x65, 0xc1, 0x0c, 0x11, 0x10, 0x00,
  0x40, 0x07, 0x13, 0x40, 0x88, 0x10, 0x10, 0x07,
  0x07, 0x3a, 0x70, 0x06, 0x01, 0x20, 0x40,
  0x40, 0x06, 0x33, 0x36, 0xce, 0x10, 0x70, 0x46,
  0x06, 0x52, 0x63, 0x00, 0x03, 0x10, 0x48,
  0x41, 0x84, 0x52, 0x5f, 0x00, 0x13, 0x15, 0xa3,
  0x06, 0x51, 0x50, 0x00, 0x01, 0x10, 0x80,
  0x41, 0x06, 0x58, 0x63, 0x00, 0x33, 0x15, 0x62,
  0x06, 0x52, 0x72, 0x00, 0x31, 0x10, 0xa0,
  0x40, 0x06, 0x11, 0x14, 0x68, 0x15, 0x10, 0x08,
  0x04, 0x4f, 0x40, 0x0c, 0x15, 0x10, 0x40,
  0x40, 0x07, 0x68, 0xf8, 0x0b, 0x44, 0xa0, 0x01,
  0x17, 0x25, 0x80, 0x00, 0x11, 0x10, 0x80,
  0x41, 0x06, 0x43, 0x59, 0x40, 0x05, 0x31, 0x60,
  0x06, 0x30, 0x50, 0x10, 0x15, 0x11, 0x48,
  0x40, 0x16, 0x58, 0xf4, 0x31, 0x44, 0x60, 0x05,
  0x44, 0x44, 0xfe, 0x14, 0x41, 0x10, 0x00,
  0x00, 0x34, 0x38, 0xf5, 0x6a, 0x04, 0x30, 0x46,
  0x16, 0x43, 0xf5, 0x08, 0x11, 0x10, 0x40,
  0x41, 0x16, 0x44, 0xf5, 0x00, 0x15, 0x10, 0x58,
  0x54, 0x42, 0xff, 0x00, 0x11, 0x12, 0x48,
  0xc0, 0x06, 0x31, 0x15, 0x46, 0x10, 0x10, 0x43,
  0x06, 0x31, 0x21, 0x06, 0x01, 0x10, 0x00,
  0x40, 0x06, 0x11, 0x36, 0x5e, 0x00, 0x10, 0x02,
  0x06, 0x3d, 0xf0, 0x02, 0x11, 0x10, 0x40,
  0x40, 0xa4, 0x34, 0x5f, 0x65, 0x01, 0x20, 0x0a,
  0x06, 0x41, 0x62, 0x0c, 0x13, 0x10, 0x20,
  0x40, 0x34, 0x32, 0xd5, 0x22, 0x44, 0x20, 0x03,
  0x26, 0x52, 0xff, 0x1c, 0x41, 0x70, 0x08,
  0x40, 0x15, 0x13, 0xd1, 0x30, 0x44, 0x10, 0x08,
  0x47, 0x53, 0xde, 0x02, 0x41, 0x30, 0x00,
  0x40, 0x44, 0x47, 0xf7, 0x40, 0x44, 0x10, 0x6d,
  0x46, 0x65, 0xfe, 0x00, 0x44, 0x10, 0x10,
  0x40, 0x44, 0x45, 0xd3, 0x46, 0x00, 0x30, 0x04,
  0x44, 0x44, 0xe1, 0x08, 0x01, 0x10, 0x08,
  0x40, 0x74, 0x7a, 0xf5, 0x11, 0x44, 0x40, 0x06,
  0x35, 0x31, 0xc0, 0x14, 0x44, 0x10, 0x00,
  0x40, 0x06, 0x2c, 0x72, 0x58, 0x44, 0x10, 0x05,
  0x06, 0x7f, 0x60, 0x2c, 0x11, 0x40, 0x40,
  0x80, 0x07, 0x0d, 0xd3, 0x16, 0x44, 0x10, 0x06,
  0x06, 0x86, 0x51, 0x14, 0x41, 0x10, 0x00,
  0x40, 0x06, 0x0a, 0xd0, 0x10, 0x44, 0x00, 0x10,
  0x06, 0xbf, 0xb0, 0x26, 0x41, 0x10, 0x00,
  0x41, 0x46, 0x54, 0xf4, 0x01, 0x15, 0x35, 0x65,
  0x46, 0x55, 0xf4, 0x00, 0x11, 0x10, 0xa8,
  0xc0, 0x64, 0x6c, 0xe2, 0x54, 0x44, 0x70, 0x45,
  0x64, 0x68, 0xf1, 0x0c, 0x41, 0x20, 0x40,
  0x40, 0x54, 0x57, 0x83, 0x76, 0x44, 0x50, 0x44,
  0x56, 0x6f, 0xf0, 0x00, 0x50, 0x10, 0x00,
  0x40, 0x91, 0x9a, 0xf2, 0x4a, 0x40, 0x82, 0x06,
  0x81, 0x87, 0xff, 0x08, 0x40, 0x21, 0x00,
  0x40, 0x44, 0x40, 0xf0, 0x0a, 0x01, 0x00, 0x00,
  0x34, 0x3d, 0xf0, 0x00, 0x45, 0x00, 0x00,
  0x40, 0x54, 0x58, 0x94, 0x05, 0x24, 0x17, 0x53,
  0x64, 0x69, 0xf0, 0x00, 0x04, 0x03, 0x80,
  0x40, 0x74, 0x71, 0xf0, 0x2c, 0x44, 0x10, 0x27,
  0x45, 0x43, 0xf0, 0x00, 0x44, 0x00, 0x00,
  0xc0, 0x04, 0x0f, 0x50, 0x00, 0x64, 0x11, 0x17,
  0xf4, 0xff, 0x2f, 0x0c, 0x13, 0x43, 0xc0,
  0x40, 0x42, 0x68, 0xf2, 0x02, 0x45, 0x60, 0x47,
  0x42, 0xa6, 0x88, 0x02, 0x45, 0x30, 0x10,
  0x40, 0x72, 0xa6, 0x98, 0x2c, 0x15, 0x20, 0x1f,
  0x02, 0xa9, 0x8f, 0x1c, 0x15, 0x10, 0x40,
  0x00, 0x00, 0x06, 0xf0, 0x00, 0x01, 0xe0, 0x97,
  0x30, 0x3f, 0x10, 0x00, 0x01, 0x00, 0x18,
  0x40, 0x35, 0x37, 0x3a, 0x56, 0x75, 0x50, 0x00,
  0x25, 0x76, 0x53, 0x00, 0x75, 0xa0, 0x00,
  0x40, 0x01, 0x02, 0xb6, 0x71, 0x44, 0x50, 0x15,
  0x31, 0x34, 0xf1, 0x00, 0x44, 0x40, 0x00,
  0xc0, 0x00, 0x06, 0xf0, 0x00, 0x55, 0xf0, 0x77,
  0x06, 0x50, 0x20, 0x00, 0x55, 0x00, 0x00,
  0x00, 0x02, 0x1f, 0xf0, 0x00, 0x51, 0x60, 0xc7,
  0x06, 0x52, 0x51, 0x10, 0x17, 0x40, 0x00,
  0x40, 0x00, 0x04, 0x6f, 0x0c, 0x01, 0x50, 0x17,
  0x84, 0x86, 0xfb, 0x00, 0x11, 0xf0, 0x30,
  0x40, 0x00, 0x36, 0xF3, 0x49, 0x43, 0xA0, 0x14,
  0x40, 0x44, 0xE6, 0x02, 0x25, 0x20, 0x00
};

const uint8 audfmt_smaf_fm_4op_dls_lookup2[3][29] = {
  0x45, 0x03, 0x03, 0x90, 0x2a, 0x44, 0x10, 0x0c,
  0x02, 0x92, 0x80, 0x24, 0x41, 0x10, 0x00, 0x03,
  0x03, 0x90, 0x36, 0x44, 0x10, 0x4c, 0x02, 0x92,
  0x90, 0x54, 0x41, 0x10, 0x08,
  0x45, 0x03, 0x03, 0x70, 0x16, 0x44, 0x10, 0x0b,
  0x02, 0x92, 0x70, 0x3c, 0x43, 0x10, 0x40, 0x03,
  0x03, 0x70, 0x22, 0x44, 0x10, 0x4b, 0x02, 0x92,
  0x70, 0x36, 0x43, 0x10, 0x00,
  0x85, 0x03, 0x07, 0xc1, 0x62, 0x00, 0x10, 0x08,
  0x03, 0x81, 0x71, 0x02, 0x11, 0x30, 0x48, 0x03,
  0x07, 0xc1, 0x62, 0x00, 0x10, 0x08, 0x03, 0x81,
  0x71, 0x0c, 0x11, 0x31, 0x48
};

const uint8 audfmt_smaf_fm_2op_dls_lookup2[128][15] = {
  0x40, 0x23, 0x34, 0xf2, 0x42, 0x40, 0x10, 0x2b,
  0x13, 0x62, 0xf3, 0x02, 0x41, 0x10, 0x00,
  0x40, 0x23, 0x34, 0xf3, 0x41, 0x40, 0x10, 0x6c,
  0x13, 0x62, 0xf3, 0x10, 0x41, 0x10, 0x00,
  0x40, 0x13, 0x51, 0xf3, 0x64, 0x44, 0x10, 0x06,
  0x23, 0x61, 0xf6, 0x20, 0x41, 0x10, 0x00,
  0xc0, 0x22, 0x53, 0xf4, 0x41, 0x31, 0x17, 0x21,
  0x33, 0x73, 0xf5, 0x00, 0x10, 0x13, 0x00,
  0x40, 0x02, 0x41, 0xe5, 0x4d, 0x20, 0x10, 0x05,
  0x22, 0x82, 0xd6, 0x00, 0x21, 0x10, 0x00,
  0x40, 0x32, 0x4d, 0xf7, 0x1d, 0x40, 0x43, 0x15,
  0x32, 0x83, 0xbf, 0x08, 0x21, 0x11, 0x00,
  0x40, 0x13, 0xf3, 0xc1, 0x51, 0x44, 0x30, 0x52,
  0x32, 0x73, 0xdf, 0x1c, 0x41, 0x10, 0xa8,
  0x40, 0x23, 0x82, 0xf1, 0x80, 0x42, 0x30, 0x2b,
  0x23, 0xa2, 0xf2, 0x1c, 0x41, 0x10, 0x20,
  0xc0, 0x23, 0x15, 0xfc, 0xa9, 0x06, 0xc0, 0x00,
  0x42, 0x55, 0xff, 0x08, 0x01, 0x10, 0x00,
  0x41, 0x23, 0x55, 0xf6, 0x08, 0x44, 0x70, 0x00,
  0x23, 0x24, 0xf1, 0x01, 0x41, 0x10, 0x00,
  0x40, 0x23, 0x4a, 0x95, 0xa0, 0x44, 0x70, 0x41,
  0x22, 0x43, 0xff, 0x02, 0x41, 0x10, 0x00,
  0x40, 0x23, 0x35, 0xf1, 0x59, 0x01, 0x70, 0x01,
  0x22, 0x62, 0xff, 0x02, 0x30, 0x10, 0x00,
  0x40, 0x53, 0x5a, 0xf4, 0x58, 0x44, 0x50, 0x02,
  0x23, 0x43, 0xed, 0x0c, 0x44, 0x10, 0x00,
  0x40, 0x73, 0x0b, 0xb5, 0x48, 0x44, 0x70, 0x34,
  0x42, 0x76, 0xfe, 0x00, 0x44, 0x10, 0x00,
  0x40, 0x13, 0x1b, 0xf0, 0x3a, 0x44, 0x71, 0x86,
  0x32, 0x39, 0xf0, 0x30, 0x14, 0x10, 0x40,
  0x40, 0x32, 0x54, 0xd5, 0x3e, 0x44, 0x37, 0xa3,
  0x32, 0x32, 0xf2, 0x1e, 0x41, 0x10, 0x00,
  0x41, 0x02, 0xa0, 0xd0, 0x38, 0x41, 0x00, 0x21,
  0x02, 0x91, 0xf1, 0x1c, 0x01, 0x20, 0x48,
  0x40, 0x02, 0x99, 0xc5, 0x7c, 0x01, 0x40, 0x00,
  0x02, 0x80, 0xa0, 0x00, 0x01, 0x00, 0x00,
  0x80, 0x02, 0xab, 0xf1, 0x68, 0x10, 0x00, 0x8b,
  0x02, 0xba, 0xf0, 0x14, 0x11, 0x20, 0x00,
  0x40, 0x02, 0x37, 0xa2, 0x3c, 0x41, 0x50, 0x8a,
  0x03, 0x45, 0x41, 0x28, 0x43, 0x10, 0x00,
  0x40, 0x02, 0x47, 0x70, 0x40, 0x45, 0x10, 0x08,
  0x03, 0x84, 0x40, 0x16, 0x44, 0x10, 0x00,
  0x40, 0x02, 0xa8, 0x90, 0x68, 0x44, 0x17, 0x21,
  0x03, 0xa0, 0x51, 0x1c, 0x01, 0x23, 0x80,
  0x40, 0x02, 0x31, 0x90, 0x70, 0x44, 0x10, 0x2c,
  0x02, 0x91, 0x60, 0x20, 0x41, 0x30, 0x00,
  0x41, 0x02, 0x81, 0x63, 0x14, 0x12, 0x23, 0x62,
  0x02, 0x81, 0x61, 0x34, 0x11, 0x17, 0xc8,
  0x40, 0x23, 0x33, 0xf2, 0x94, 0x46, 0x20, 0x47,
  0x13, 0x83, 0xf2, 0x08, 0x41, 0x10, 0x40,
  0x40, 0x13, 0x44, 0xf7, 0x90, 0x41, 0x30, 0xaf,
  0x13, 0x62, 0xf5, 0x08, 0x41, 0x10, 0x40,
  0x40, 0x13, 0x66, 0xb2, 0x69, 0x41, 0x30, 0x07,
  0x22, 0x92, 0xcf, 0x00, 0x45, 0x10, 0x00,
  0x40, 0x32, 0x37, 0xf2, 0x1e, 0x01, 0x32, 0x8b,
  0x32, 0x93, 0xf4, 0x0e, 0x01, 0x10, 0x00,
  0x40, 0x22, 0x79, 0xa3, 0x46, 0x42, 0x00, 0x56,
  0x23, 0x82, 0xd5, 0x18, 0x41, 0x20, 0x00,
  0x80, 0x02, 0x98, 0xd2, 0x25, 0x40, 0x10, 0x23,
  0x12, 0x94, 0xd1, 0x10, 0x41, 0x20, 0x20,
  0x40, 0x02, 0x28, 0x73, 0x24, 0x42, 0x10, 0x0d,
  0x02, 0x91, 0xd3, 0x1e, 0x44, 0x20, 0x30,
  0x40, 0x82, 0x72, 0x90, 0x45, 0x41, 0x00, 0x03,
  0x52, 0x93, 0xbf, 0x20, 0x41, 0x20, 0x40,
  0x40, 0x12, 0x24, 0xb1, 0x13, 0x40, 0x10, 0x02,
  0x32, 0x94, 0xd6, 0x00, 0x41, 0x10, 0x00,
  0x80, 0x13, 0x89, 0xf1, 0x4a, 0x41, 0x10, 0x45,
  0x23, 0x84, 0x75, 0x00, 0x45, 0x20, 0x00,
  0x80, 0x13, 0x89, 0xa2, 0x3a, 0x41, 0x10, 0x45,
  0x23, 0x84, 0xf4, 0x00, 0x45, 0x40, 0x00,
  0x80, 0x13, 0x82, 0xb1, 0x60, 0x45, 0x11, 0x05,
  0x23, 0x82, 0xbf, 0x08, 0x41, 0x40, 0x00,
  0x80, 0x22, 0xa8, 0xf3, 0x30, 0x45, 0x30, 0x43,
  0x32, 0xf5, 0xf4, 0x10, 0x45, 0x10, 0x40,
  0x80, 0x22, 0xa8, 0xf3, 0x38, 0x41, 0x40, 0x03,
  0x32, 0xf4, 0xf4, 0x12, 0x41, 0x10, 0x40,
  0x40, 0x13, 0x23, 0xf2, 0x2e, 0x41, 0x10, 0x05,
  0x13, 0x71, 0xf4, 0x08, 0x40, 0x10, 0x00,
  0x40, 0x02, 0x46, 0xf2, 0x32, 0x41, 0x10, 0x29,
  0x02, 0x93, 0xd4, 0x00, 0x41, 0x11, 0x40,
  0x80, 0x02, 0x35, 0xd3, 0x44, 0x40, 0x10, 0xa3,
  0x02, 0x76, 0x50, 0x10, 0x01, 0x10, 0x00,
  0x80, 0x02, 0x15, 0xa1, 0x40, 0x00, 0x10, 0x0c,
  0x02, 0x75, 0x60, 0x18, 0x01, 0x20, 0x00,
  0x80, 0x02, 0x44, 0x61, 0x70, 0x01, 0x10, 0x2d,
  0x02, 0x61, 0x60, 0x00, 0x03, 0x10, 0x80,
  0x80, 0x02, 0x51, 0x91, 0x49, 0x40, 0x10, 0xaa,
  0x02, 0x73, 0x90, 0x00, 0x41, 0x10, 0x40,
  0xc0, 0x03, 0x11, 0xf1, 0x1d, 0x44, 0x10, 0xac,
  0x02, 0x6f, 0x61, 0x10, 0x34, 0x10, 0x00,
  0x41, 0x53, 0x56, 0x72, 0x00, 0x44, 0x10, 0x63,
  0x72, 0x75, 0xd2, 0x00, 0x44, 0x10, 0x20,
  0x40, 0x31, 0x25, 0xf3, 0x88, 0x40, 0x20, 0x0e,
  0x31, 0x22, 0xff, 0x00, 0x41, 0x10, 0x00,
  0x40, 0x50, 0x4b, 0xf0, 0x50, 0x44, 0x20, 0x86,
  0x40, 0x49, 0xd0, 0x00, 0x14, 0x00, 0x40,
  0x80, 0x02, 0x11, 0xb1, 0x6e, 0x04, 0x10, 0x40,
  0x02, 0x62, 0x70, 0x10, 0x11, 0x20, 0x80,
  0x80, 0x02, 0x11, 0xb1, 0x5a, 0x04, 0x10, 0x40,
  0x02, 0x62, 0x60, 0x10, 0x11, 0x20, 0x80,
  0x40, 0x02, 0x11, 0xb1, 0x7e, 0x04, 0x15, 0x41,
  0x02, 0x52, 0x60, 0x10, 0x11, 0x20, 0xa0,
  0x80, 0x02, 0x21, 0x82, 0x80, 0x44, 0x10, 0x6e,
  0x02, 0x4f, 0x60, 0x02, 0x11, 0x10, 0x40,
  0x41, 0x02, 0x55, 0x60, 0x18, 0x23, 0x10, 0x44,
  0x02, 0x5a, 0x50, 0x24, 0x01, 0x20, 0x80,
  0x80, 0x02, 0x16, 0x50, 0x94, 0x41, 0x10, 0x00,
  0x02, 0x65, 0x62, 0x00, 0x43, 0x20, 0x00,
  0x80, 0x12, 0x22, 0x81, 0x52, 0x03, 0x10, 0x05,
  0x12, 0x53, 0x61, 0x10, 0x01, 0x40, 0x00,
  0x40, 0x41, 0x44, 0x56, 0x00, 0x54, 0x10, 0x04,
  0x60, 0x65, 0xa4, 0x18, 0x41, 0x20, 0x30,
  0x80, 0x02, 0x05, 0x91, 0x42, 0x41, 0x10, 0x06,
  0x02, 0x9f, 0xe0, 0x18, 0x40, 0x10, 0x00,
  0x40, 0x02, 0x05, 0x81, 0x4e, 0x40, 0x10, 0x06,
  0x12, 0x90, 0x70, 0x00, 0x43, 0x10, 0x00,
  0x80, 0x02, 0x06, 0xa1, 0x3a, 0x41, 0x10, 0x05,
  0x02, 0x72, 0x63, 0x00, 0x47, 0x10, 0x00,
  0x40, 0x02, 0x08, 0x71, 0x5e, 0x41, 0x50, 0x15,
  0x02, 0xa3, 0x80, 0x20, 0x43, 0x10, 0x28,
  0x80, 0x02, 0x02, 0x63, 0x56, 0x02, 0x10, 0x04,
  0x02, 0x72, 0xd0, 0x00, 0x21, 0x10, 0x00,
  0x80, 0x02, 0x05, 0x71, 0x2a, 0x41, 0x10, 0x45,
  0x02, 0x8c, 0xb0, 0x14, 0x40, 0x10, 0x00,
  0x80, 0x02, 0x04, 0x83, 0x50, 0x41, 0x11, 0x0e,
  0x02, 0x90, 0xa0, 0x14, 0x40, 0x10, 0x40,
  0x80, 0x02, 0x04, 0x62, 0x5a, 0x40, 0x10, 0x46,
  0x02, 0x63, 0xa2, 0x00, 0x41, 0x10, 0x00,
  0x40, 0x03, 0x03, 0x90, 0x56, 0x44, 0x10, 0x4d,
  0x02, 0xb2, 0x80, 0x14, 0x41, 0x10, 0x00,
  0x40, 0x03, 0x03, 0x90, 0x36, 0x44, 0x10, 0x4c,
  0x02, 0x92, 0x80, 0x18, 0x41, 0x10, 0x00,
  0x40, 0x03, 0x03, 0x70, 0x16, 0x44, 0x10, 0x0b,
  0x02, 0x92, 0x70, 0x1c, 0x43, 0x10, 0x00,
  0x40, 0x03, 0x03, 0x90, 0x2a, 0x44, 0x10, 0x43,
  0x02, 0xf2, 0x80, 0x0c, 0x13, 0x10, 0x00,
  0x80, 0x02, 0x01, 0xd2, 0x82, 0x14, 0x10, 0x40,
  0x02, 0x82, 0x80, 0x0c, 0x41, 0x30, 0x08,
  0x80, 0x03, 0x0e, 0x93, 0x62, 0x00, 0x10, 0x43,
  0x02, 0x8b, 0xa0, 0x00, 0x01, 0x10, 0x28,
  0x80, 0x03, 0x07, 0xc1, 0x5e, 0x00, 0x10, 0x08,
  0x03, 0x81, 0x71, 0x00, 0x11, 0x30, 0x48,
  0x80, 0x02, 0x00, 0x90, 0x84, 0x44, 0x20, 0x02,
  0x02, 0x80, 0x80, 0x00, 0x21, 0x10, 0x00,
  0x81, 0x02, 0xb9, 0x8f, 0x0c, 0x35, 0x10, 0x93,
  0x02, 0xc3, 0x81, 0x00, 0x30, 0x10, 0x00,
  0x80, 0x02, 0x58, 0x85, 0x1d, 0x01, 0x10, 0x88,
  0x02, 0x75, 0x60, 0x00, 0x33, 0x10, 0x00,
  0x80, 0x72, 0xa6, 0x90, 0xcc, 0x10, 0x20, 0x18,
  0x02, 0xa5, 0x80, 0x10, 0x13, 0x10, 0x40,
  0x40, 0x02, 0x06, 0xc4, 0x0f, 0x13, 0x20, 0x16,
  0x02, 0x88, 0x71, 0x0c, 0x11, 0x10, 0x40,
  0x40, 0x02, 0x67, 0x6a, 0x74, 0x11, 0x10, 0x1f,
  0x02, 0x70, 0xb0, 0x00, 0x12, 0x10, 0x00,
  0x40, 0x43, 0x65, 0xf8, 0x24, 0x50, 0x10, 0x27,
  0x02, 0x90, 0x60, 0x04, 0x11, 0x10, 0x00,
  0x41, 0x02, 0x87, 0xf0, 0x70, 0x11, 0x10, 0xa5,
  0x02, 0x78, 0x80, 0x00, 0x11, 0x10, 0x00,
  0x41, 0x12, 0x7a, 0x8a, 0x79, 0x11, 0x30, 0x03,
  0x02, 0x96, 0x80, 0x00, 0x10, 0x10, 0x00,
  0x41, 0x02, 0x9f, 0xd0, 0x35, 0x43, 0x14, 0x30,
  0x02, 0x9f, 0xd0, 0x04, 0x41, 0x10, 0x40,
  0x41, 0x02, 0x80, 0xf0, 0x28, 0x41, 0x17, 0xc0,
  0x02, 0x80, 0xf0, 0x5c, 0x45, 0x10, 0xd8,
  0x40, 0x02, 0x57, 0xca, 0x2c, 0x40, 0x25, 0x18,
  0x02, 0x80, 0x90, 0x00, 0x31, 0x00, 0x90,
  0x40, 0x02, 0x27, 0x76, 0x21, 0x10, 0x14, 0x40,
  0x02, 0x86, 0xf0, 0x0c, 0x11, 0x10, 0x40,
  0x40, 0x02, 0xb2, 0x91, 0x22, 0x40, 0x14, 0x08,
  0x02, 0xa1, 0x92, 0x24, 0x01, 0x20, 0x00,
  0x80, 0x02, 0x51, 0x61, 0x23, 0x00, 0x35, 0x81,
  0x02, 0x7f, 0xd0, 0x18, 0x11, 0x10, 0x00,
  0x41, 0x02, 0x7f, 0xa0, 0x0d, 0x41, 0x30, 0x25,
  0x02, 0x7f, 0xb0, 0x08, 0x43, 0x10, 0x20,
  0x40, 0x02, 0x84, 0xd4, 0x29, 0x04, 0x14, 0x68,
  0x02, 0x84, 0xf0, 0x18, 0x01, 0x10, 0x80,
  0x41, 0x03, 0x42, 0xf6, 0x0a, 0x40, 0x41, 0x50,
  0x02, 0x40, 0x60, 0x00, 0x11, 0x10, 0xa0,
  0x41, 0x02, 0x50, 0x50, 0x00, 0x45, 0x15, 0xa0,
  0x03, 0x30, 0x50, 0x02, 0x71, 0x10, 0x80,
  0x40, 0x00, 0x41, 0xd3, 0x41, 0x00, 0x15, 0x46,
  0x00, 0x65, 0xc1, 0x0c, 0x11, 0x10, 0x00,
  0x40, 0x03, 0x13, 0x40, 0x88, 0x10, 0x10, 0x07,
  0x03, 0x3a, 0x70, 0x06, 0x01, 0x20, 0x40,
  0x40, 0x02, 0x33, 0x36, 0xce, 0x10, 0x70, 0x46,
  0x02, 0x52, 0x63, 0x00, 0x03, 0x10, 0x48,
  0x41, 0x80, 0x52, 0x5f, 0x00, 0x13, 0x15, 0xa3,
  0x02, 0x51, 0x50, 0x00, 0x01, 0x10, 0x80,
  0x41, 0x02, 0x58, 0x63, 0x00, 0x33, 0x15, 0x62,
  0x02, 0x52, 0x72, 0x00, 0x31, 0x10, 0xa0,
  0x40, 0x02, 0x11, 0x14, 0x68, 0x15, 0x10, 0x08,
  0x00, 0x4f, 0x40, 0x0c, 0x15, 0x10, 0x40,
  0x40, 0x03, 0x68, 0xf8, 0x0b, 0x44, 0xa0, 0x01,
  0x13, 0x25, 0x80, 0x00, 0x11, 0x10, 0x80,
  0x41, 0x02, 0x43, 0x59, 0x40, 0x05, 0x31, 0x60,
  0x02, 0x30, 0x50, 0x10, 0x15, 0x11, 0x48,
  0x40, 0x12, 0x58, 0xf4, 0x31, 0x44, 0x60, 0x05,
  0x40, 0x44, 0xfe, 0x14, 0x41, 0x10, 0x00,
  0x00, 0x30, 0x38, 0xf5, 0x6a, 0x04, 0x30, 0x46,
  0x12, 0x43, 0xf5, 0x08, 0x11, 0x10, 0x40,
  0x41, 0x12, 0x44, 0xf5, 0x00, 0x15, 0x10, 0x58,
  0x50, 0x42, 0xff, 0x00, 0x11, 0x12, 0x48,
  0xc0, 0x02, 0x31, 0x15, 0x46, 0x10, 0x10, 0x43,
  0x02, 0x31, 0x21, 0x06, 0x01, 0x10, 0x00,
  0x40, 0x02, 0x11, 0x36, 0x5e, 0x00, 0x10, 0x02,
  0x02, 0x3d, 0xf0, 0x02, 0x11, 0x10, 0x40,
  0x40, 0xa0, 0x34, 0x5f, 0x65, 0x01, 0x20, 0x0a,
  0x02, 0x41, 0x62, 0x0c, 0x13, 0x10, 0x20,
  0x40, 0x30, 0x32, 0xd5, 0x22, 0x44, 0x20, 0x03,
  0x22, 0x52, 0xff, 0x1c, 0x41, 0x70, 0x08,
  0x40, 0x11, 0x13, 0xd1, 0x30, 0x44, 0x10, 0x08,
  0x43, 0x53, 0xde, 0x02, 0x41, 0x30, 0x00,
  0x40, 0x40, 0x47, 0xf7, 0x40, 0x44, 0x10, 0x6d,
  0x42, 0x65, 0xfe, 0x00, 0x44, 0x10, 0x10,
  0x40, 0x40, 0x45, 0xd3, 0x46, 0x00, 0x30, 0x04,
  0x40, 0x44, 0xe1, 0x08, 0x01, 0x10, 0x08,
  0x40, 0x70, 0x7a, 0xf5, 0x11, 0x44, 0x40, 0x06,
  0x31, 0x31, 0xc0, 0x14, 0x44, 0x10, 0x00,
  0x40, 0x02, 0x2c, 0x72, 0x58, 0x44, 0x10, 0x05,
  0x02, 0x7f, 0x60, 0x2c, 0x11, 0x40, 0x40,
  0x80, 0x03, 0x0d, 0xd3, 0x16, 0x44, 0x10, 0x06,
  0x02, 0x86, 0x51, 0x14, 0x41, 0x10, 0x00,
  0x40, 0x02, 0x0a, 0xd0, 0x10, 0x44, 0x00, 0x10,
  0x02, 0xbf, 0xb0, 0x26, 0x41, 0x10, 0x00,
  0x41, 0x42, 0x54, 0xf4, 0x01, 0x15, 0x35, 0x65,
  0x42, 0x55, 0xf4, 0x00, 0x11, 0x10, 0xa8,
  0xc0, 0x60, 0x6c, 0xe2, 0x54, 0x44, 0x70, 0x45,
  0x60, 0x68, 0xf1, 0x0c, 0x41, 0x20, 0x40,
  0x40, 0x50, 0x57, 0x83, 0x76, 0x44, 0x50, 0x44,
  0x52, 0x6f, 0xf0, 0x00, 0x50, 0x10, 0x00,
  0x40, 0x91, 0x9a, 0xf2, 0x4a, 0x40, 0x82, 0x06,
  0x81, 0x87, 0xff, 0x08, 0x40, 0x21, 0x00,
  0x40, 0x40, 0x40, 0xf0, 0x0a, 0x01, 0x00, 0x00,
  0x30, 0x3d, 0xf0, 0x00, 0x45, 0x00, 0x00,
  0x40, 0x50, 0x58, 0x94, 0x05, 0x24, 0x17, 0x53,
  0x60, 0x69, 0xf0, 0x00, 0x04, 0x03, 0x80,
  0x40, 0x70, 0x71, 0xf0, 0x2c, 0x44, 0x10, 0x27,
  0x41, 0x43, 0xf0, 0x00, 0x44, 0x00, 0x00,
  0xc0, 0x00, 0x0f, 0x50, 0x00, 0x64, 0x11, 0x17,
  0xf0, 0xff, 0x2f, 0x0c, 0x13, 0x43, 0xc0,
  0x40, 0x42, 0x68, 0xf2, 0x02, 0x45, 0x60, 0x47,
  0x42, 0xa6, 0x88, 0x02, 0x45, 0x30, 0x10,
  0x40, 0x72, 0xa6, 0x98, 0x2c, 0x15, 0x20, 0x1f,
  0x02, 0xa9, 0x8f, 0x1c, 0x15, 0x10, 0x40,
  0x00, 0x00, 0x06, 0xf0, 0x00, 0x01, 0xe0, 0x97,
  0x30, 0x3f, 0x10, 0x00, 0x01, 0x00, 0x18,
  0x40, 0x31, 0x37, 0x3a, 0x56, 0x75, 0x50, 0x00,
  0x21, 0x76, 0x53, 0x00, 0x75, 0xa0, 0x00,
  0x40, 0x01, 0x02, 0xb6, 0x71, 0x44, 0x50, 0x15,
  0x31, 0x34, 0xf1, 0x00, 0x44, 0x40, 0x00,
  0xc0, 0x00, 0x06, 0xf0, 0x00, 0x55, 0xf0, 0x77,
  0x02, 0x50, 0x20, 0x00, 0x55, 0x00, 0x00,
  0x00, 0x02, 0x1f, 0xf0, 0x00, 0x51, 0x60, 0xc7,
  0x02, 0x52, 0x51, 0x10, 0x17, 0x40, 0x00,
  0x40, 0x00, 0x04, 0x6f, 0x0c, 0x01, 0x50, 0x17,
  0x80, 0x86, 0xfb, 0x00, 0x11, 0xf0, 0x30
};

/* Please note that the first two drum mappings should be to 0x1f and
** 0x21. However, since these are outside of the support MIDI drum range,
** these two have been mapped to 0x28 and 0x23, respectively.
*/
const uint8 audfmt_smaf_pcm_drum_dls_map[24] = {
  0x28, 0x23, 0x23, 0x24, 0x26, 0x28, 0x29, 0x2a,
  0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32,
  0x33, 0x34, 0x37, 0x39, 0x3b, 0x2a, 0x2a, 0x31  
};

#ifndef FEATURE_AUDIO_CMX_5P0
#ifdef FEATURE_SMAF_ROM_DLS
#error code not present
#endif /* FEATURE_SMAF_ROM_DLS */
#endif /* not FEATURE_AUDIO_CMX_5P0 */

const uint8 audfmt_smaf_pcm_drum_dls_lookup[24][16] = {
  0x3e, 0x80, 0x79, 0x00, 0x08, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x0b, 0x9b, 0x0b, 0x9b, 0x81,
  0x23, 0x28, 0x79, 0x00, 0x08, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x03, 0xa9, 0x03, 0xa9, 0x80,
  0x27, 0x10, 0x79, 0x00, 0x08, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x03, 0xa9, 0x03, 0xa9, 0x80,
  0x2e, 0xe0, 0x79, 0x00, 0x08, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x03, 0xa9, 0x03, 0xa9, 0x80,
  0x4a, 0x38, 0x79, 0x00, 0x08, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x0b, 0x9b, 0x0b, 0x9b, 0x81,
  0x59, 0xd8, 0x79, 0x00, 0x08, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x0b, 0x9b, 0x0b, 0x9b, 0x81,
  0x1f, 0x40, 0x41, 0x00, 0x58, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x0a, 0xd4, 0x0d, 0xc5, 0x82,
  0x3a, 0x98, 0xa9, 0x00, 0x08, 0xf0, 0xf0, 0x40,
  0x00, 0x00, 0x00, 0x04, 0xd7, 0x04, 0xd7, 0x83,
  0x27, 0x10, 0x59, 0x00, 0x58, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x0a, 0xd4, 0x0d, 0xc5, 0x82,
  0x36, 0xb0, 0xa9, 0x00, 0x08, 0xf0, 0xf0, 0x40,
  0x00, 0x00, 0x00, 0x04, 0xd7, 0x04, 0xd7, 0x83,
  0x30, 0xd4, 0x71, 0x00, 0x58, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x0a, 0xd4, 0x0d, 0xc5, 0x82,
  0x3c, 0x8c, 0xa9, 0x00, 0x08, 0xf0, 0xf0, 0x30,
  0x00, 0x00, 0x00, 0x0c, 0xfb, 0x0c, 0xfb, 0x84,
  0x3a, 0x98, 0x91, 0x00, 0x58, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x0a, 0xd4, 0x0d, 0xc5, 0x82,
  0x46, 0x50, 0xa9, 0x00, 0x58, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x0a, 0xd4, 0x0d, 0xc5, 0x82,
  0x33, 0x90, 0xa9, 0x00, 0x58, 0xf0, 0xf0, 0x00,
  0x00, 0x00, 0x00, 0x0d, 0xf9, 0x15, 0xdb, 0x86,
  0x52, 0x08, 0xc1, 0x00, 0x58, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x0a, 0xd4, 0x0d, 0xc5, 0x82,
  0x3e, 0x80, 0x59, 0x00, 0x58, 0xf0, 0xf0, 0x30,
  0x00, 0x00, 0x00, 0x06, 0x3a, 0x12, 0xc0, 0x85,
  0x23, 0x28, 0x59, 0x00, 0x58, 0xf0, 0xf0, 0x10,
  0x00, 0x00, 0x00, 0x0d, 0xf9, 0x15, 0xdb, 0x86,
  0x52, 0x08, 0x69, 0x00, 0x58, 0xf0, 0xf0, 0x28,
  0x00, 0x00, 0x00, 0x0d, 0xf9, 0x15, 0xdb, 0x86,
  0x36, 0xb0, 0x59, 0x00, 0x58, 0xf0, 0xf0, 0x00,
  0x00, 0x00, 0x00, 0x0d, 0xf9, 0x15, 0xdb, 0x86,
  0x46, 0x50, 0x59, 0x00, 0x58, 0xf0, 0xf0, 0x30,
  0x00, 0x00, 0x00, 0x06, 0x3a, 0x12, 0xc0, 0x85,
  0x9c, 0x40, 0x59, 0x00, 0x78, 0xf0, 0xf0, 0x00,
  0x00, 0x00, 0x00, 0x0d, 0xf9, 0x15, 0xdb, 0x86,
  0x3a, 0x98, 0xa9, 0x00, 0x08, 0xf0, 0xf0, 0x18,
  0x00, 0x00, 0x00, 0x04, 0xd6, 0x04, 0xd6, 0x83,
  0x33, 0x90, 0x59, 0x00, 0x58, 0xf0, 0xf0, 0x00,
  0x00, 0x00, 0x00, 0x0d, 0xf8, 0x15, 0xda, 0x86
};

const uint8 audfmt_smaf_fm_drum_dls_map[40] = {
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x20,
  0x22, 0x25, 0x27, 0x35, 0x36, 0x38, 0x3a, 0x3c,
  0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44,
  0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c,
  0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54
};

const uint8 audfmt_smaf_fm_4op_drum_dls_lookup[40][31] = {
  0x48, 0x79, 0x65, 0xf8, 0xf0, 0xf0, 0xe0, 0x44, 
  0x60, 0x26, 0xf9, 0xf8, 0xfc, 0x00, 0x44, 0x50, 
  0x00, 0xf8, 0xfb, 0xfb, 0x9c, 0x44, 0xa0, 0x30, 
  0xf9, 0xfb, 0xef, 0x04, 0x44, 0x50, 0x00, 
  0x41, 0x79, 0x65, 0x08, 0x08, 0xf3, 0x00, 0x44, 
  0x50, 0x07, 0x88, 0xb8, 0x9b, 0x02, 0x44, 0x00, 
  0x00, 0x68, 0xe8, 0xf3, 0x90, 0x44, 0xc0, 0x30, 
  0x88, 0x8d, 0xc0, 0xb2, 0x44, 0x10, 0x30, 
  0x2c, 0x79, 0x65, 0x00, 0x65, 0xf0, 0x00, 0x42, 
  0x00, 0x97, 0x02, 0xa8, 0x96, 0x00, 0x66, 0x00, 
  0x19, 0x00, 0xf0, 0xff, 0x00, 0x00, 0x00, 0x87, 
  0x30, 0xf6, 0x36, 0x54, 0x64, 0x10, 0x00, 
  0x45, 0x79, 0x65, 0x08, 0x00, 0xf0, 0x00, 0x44, 
  0x40, 0x07, 0x68, 0xc8, 0xc9, 0x00, 0x44, 0xe0, 
  0x68, 0x48, 0x7a, 0xbd, 0x24, 0x44, 0x00, 0x38, 
  0xb8, 0xb8, 0x8d, 0x02, 0x44, 0x00, 0x00, 
  0x31, 0x79, 0x65, 0x00, 0x35, 0xf0, 0x00, 0x42, 
  0x55, 0x97, 0x02, 0xa6, 0x96, 0x00, 0x66, 0x30, 
  0x19, 0x00, 0xf0, 0xff, 0x00, 0x00, 0x90, 0x87, 
  0x30, 0xf6, 0x36, 0x40, 0x64, 0x30, 0xa0, 
  0x12, 0x79, 0x25, 0x00, 0x20, 0x70, 0x34, 0x24, 
  0x83, 0x17, 0x00, 0x95, 0xf3, 0x00, 0x03, 0x06, 
  0xd0, 0x80, 0x4a, 0xf3, 0x20, 0x44, 0x30, 0x00, 
  0x70, 0x76, 0xe5, 0x00, 0x44, 0x00, 0x00, 
  0x57, 0x41, 0x65, 0x58, 0x97, 0xff, 0x08, 0x44, 
  0x70, 0x0e, 0x59, 0xf8, 0xaf, 0x00, 0x44, 0x50, 
  0x30, 0x68, 0x55, 0xf0, 0x9c, 0x44, 0x20, 0x28, 
  0x98, 0x9a, 0xca, 0x50, 0x44, 0x50, 0x00, 
  0x3b, 0x79, 0x66, 0xc8, 0x89, 0xf9, 0x01, 0x44, 
  0xf0, 0xa7, 0x28, 0x8a, 0xdb, 0x00, 0x44, 0xa0, 
  0x18, 0x28, 0x58, 0xd5, 0x55, 0x44, 0xb0, 0x18, 
  0x98, 0x7b, 0xcb, 0x00, 0x44, 0xd0, 0x40, 
  0x12, 0x79, 0x65, 0x59, 0x50, 0xf0, 0x14, 0x44, 
  0xc0, 0x17, 0x79, 0x77, 0xf7, 0x00, 0x44, 0xb0, 
  0x00, 0x68, 0x6a, 0xf8, 0x00, 0x44, 0xc0, 0x00, 
  0x78, 0x77, 0xf7, 0x00, 0x44, 0x70, 0x10, 
  0x34, 0x79, 0x65, 0x08, 0x82, 0xf5, 0x20, 0x44, 
  0xa0, 0x0e, 0x39, 0x39, 0xdf, 0x00, 0x44, 0x70, 
  0x30, 0x08, 0x90, 0xbd, 0x00, 0x44, 0x90, 0x97, 
  0xb8, 0x99, 0xdb, 0x00, 0x44, 0x00, 0x90, 
  0x19, 0x69, 0x65, 0x48, 0x64, 0xf0, 0x08, 0x44, 
  0x00, 0x07, 0x98, 0x46, 0xf1, 0x00, 0x44, 0x00, 
  0x28, 0x08, 0xf8, 0xf1, 0x02, 0x44, 0xd1, 0xda, 
  0x98, 0x8a, 0xf5, 0x02, 0x44, 0xf0, 0x10, 
  0x4d, 0x59, 0x25, 0x08, 0x07, 0xf4, 0x5a, 0x44, 
  0xf0, 0x9f, 0x48, 0x45, 0xfe, 0x00, 0x44, 0xf0, 
  0x60, 0x08, 0x00, 0xf4, 0x32, 0x44, 0xc0, 0x9f, 
  0x28, 0x55, 0xfd, 0x6c, 0x44, 0xf0, 0x58, 
  0x6d, 0x91, 0x65, 0x48, 0x17, 0x82, 0x28, 0x44, 
  0xb0, 0x06, 0xc8, 0xb7, 0xe5, 0x00, 0x44, 0x50, 
  0x68, 0x58, 0x27, 0x82, 0x00, 0x44, 0xb0, 0x40, 
  0xc8, 0x77, 0xd6, 0x00, 0x44, 0xf0, 0x40, 
  0x54, 0xa9, 0x65, 0x89, 0x59, 0xf3, 0x02, 0x44, 
  0x30, 0x04, 0x59, 0x37, 0xa3, 0x00, 0x44, 0x00, 
  0x00, 0xb9, 0x9f, 0xfa, 0x0c, 0x44, 0x40, 0x10, 
  0x59, 0x5c, 0xf3, 0x80, 0x44, 0x10, 0x00, 
  0x22, 0x39, 0x25, 0x28, 0x61, 0xd0, 0x00, 0x24, 
  0x12, 0xef, 0x58, 0x96, 0xf2, 0x00, 0x02, 0x06, 
  0x40, 0x48, 0x45, 0xf0, 0x74, 0x44, 0x60, 0xef, 
  0x58, 0x46, 0xf0, 0x00, 0x44, 0x00, 0xf0, 
  0x24, 0xc1, 0x65, 0xc8, 0xc5, 0xf0, 0x00, 0x44, 
  0x60, 0x03, 0x88, 0x75, 0xf0, 0x00, 0x44, 0xb0, 
  0x00, 0xc8, 0xc5, 0xf0, 0x08, 0x44, 0xf0, 0x00, 
  0x78, 0x34, 0xf0, 0x00, 0x44, 0xf0, 0x00, 
  0x20, 0xc1, 0x65, 0xc8, 0xc5, 0xf0, 0x00, 0x44, 
  0x60, 0x03, 0x88, 0x75, 0xf0, 0x1c, 0x44, 0xb0, 
  0x00, 0xc8, 0xc5, 0xf0, 0x18, 0x44, 0xf0, 0x00, 
  0x88, 0x35, 0xf0, 0x00, 0x44, 0xf0, 0x00, 
  0x4c, 0x49, 0xe5, 0xa8, 0xae, 0xe1, 0x18, 0x46, 
  0x02, 0x04, 0x98, 0xa8, 0xf1, 0x00, 0x44, 0x10, 
  0x40, 0x08, 0x00, 0xf0, 0xb0, 0x44, 0x00, 0x00, 
  0x98, 0xad, 0xf1, 0x1c, 0x44, 0x00, 0x00, 
  0x13, 0x49, 0x65, 0x58, 0x8a, 0xac, 0x98, 0x44, 
  0xa0, 0x1f, 0x78, 0x96, 0xc4, 0x00, 0x44, 0xc0, 
  0x00, 0x68, 0x8a, 0xf8, 0x00, 0x44, 0xc0, 0x00, 
  0x78, 0x97, 0xf7, 0x00, 0x44, 0x20, 0x10, 
  0x0e, 0x59, 0x65, 0x58, 0x8a, 0xac, 0x7c, 0x44, 
  0xa0, 0x1f, 0x68, 0x95, 0xc4, 0x00, 0x44, 0xc0, 
  0x00, 0x68, 0x8a, 0xf8, 0x00, 0x44, 0xc0, 0x00, 
  0x78, 0x97, 0xf7, 0x00, 0x44, 0x20, 0x50, 
  0x33, 0xa9, 0x63, 0x08, 0x87, 0xc0, 0x58, 0x44, 
  0x20, 0x06, 0x68, 0x89, 0xf0, 0xc4, 0x44, 0x70, 
  0x00, 0xa8, 0x66, 0xd1, 0x68, 0x44, 0x90, 0x20, 
  0x68, 0x89, 0xf0, 0x18, 0x44, 0x90, 0x30, 
  0x1c, 0xa9, 0x63, 0x08, 0x87, 0xc0, 0x58, 0x44, 
  0x20, 0x06, 0x68, 0x89, 0xf0, 0xc4, 0x44, 0x70, 
  0x00, 0xa8, 0x66, 0xd1, 0x5c, 0x44, 0xc0, 0x20, 
  0x68, 0x89, 0xf0, 0x10, 0x44, 0x90, 0x30, 
  0x4d, 0x39, 0x65, 0x68, 0x6c, 0xe2, 0x54, 0x44, 
  0x70, 0x05, 0x68, 0x68, 0xf1, 0x28, 0x44, 0x20, 
  0x00, 0x68, 0x5c, 0xe2, 0x68, 0x44, 0x70, 0x00, 
  0x68, 0x68, 0xf1, 0x28, 0x44, 0x20, 0x00, 
  0x48, 0x39, 0x65, 0x68, 0x6c, 0xe2, 0x54, 0x44, 
  0x70, 0x05, 0x68, 0x68, 0xf1, 0x28, 0x44, 0x20, 
  0x00, 0x58, 0x5c, 0xe2, 0x34, 0x44, 0x70, 0x00, 
  0x68, 0x68, 0xf1, 0x74, 0x44, 0x20, 0x00, 
  0x49, 0x39, 0x63, 0x38, 0x50, 0xf6, 0x00, 0x44, 
  0x70, 0x07, 0x49, 0x48, 0xea, 0x60, 0x44, 0xf0, 
  0x00, 0x28, 0x64, 0xe4, 0x00, 0x44, 0xf0, 0x00, 
  0x98, 0x4b, 0x80, 0x08, 0x44, 0xf0, 0x30, 
  0x74, 0x31, 0x64, 0x38, 0x50, 0xf6, 0x10, 0x44, 
  0xf0, 0x07, 0x49, 0x48, 0x86, 0x0c, 0x44, 0xf0, 
  0x00, 0x38, 0x39, 0xcf, 0x00, 0x44, 0xf0, 0x30, 
  0xa8, 0x3a, 0x78, 0x00, 0x44, 0xf0, 0x60, 
  0x32, 0xc1, 0xe5, 0x00, 0x70, 0xf0, 0x28, 0x20, 
  0x00, 0x0d, 0x02, 0xc1, 0xc1, 0xac, 0x21, 0xe0, 
  0x00, 0x02, 0x70, 0xf0, 0x5c, 0x47, 0x00, 0x02, 
  0x02, 0xc1, 0x80, 0x18, 0x20, 0xf0, 0x00, 
  0x30, 0xc1, 0xe5, 0x00, 0x70, 0xf0, 0x28, 0x20, 
  0x00, 0x0d, 0x02, 0xc1, 0xc1, 0xac, 0x21, 0xe0, 
  0x00, 0x02, 0x70, 0xf0, 0x5c, 0x47, 0x00, 0x02, 
  0x02, 0xc1, 0x80, 0x18, 0x20, 0xf0, 0x00, 
  0x35, 0xe1, 0x63, 0x68, 0x02, 0xc0, 0x30, 0x00, 
  0x10, 0x0f, 0x68, 0xa3, 0xc0, 0x00, 0x00, 0x30, 
  0x20, 0x08, 0x61, 0xd0, 0x80, 0x00, 0xe5, 0x5f, 
  0xc9, 0xf7, 0xf6, 0x00, 0x00, 0x01, 0x38, 
  0x2b, 0xe1, 0x63, 0x68, 0x02, 0xc0, 0x38, 0x00, 
  0x10, 0x0f, 0x68, 0xa3, 0xc0, 0x00, 0x00, 0x30, 
  0x20, 0x08, 0x60, 0xf0, 0x80, 0x00, 0xd0, 0x5f, 
  0xe8, 0xc6, 0x96, 0x14, 0x00, 0x02, 0x38, 
  0x32, 0xa9, 0x64, 0x68, 0x67, 0xda, 0x2c, 0x44, 
  0x70, 0x00, 0xb8, 0x3e, 0xfa, 0x34, 0x44, 0x70, 
  0x00, 0x89, 0x0b, 0xfc, 0x00, 0x44, 0x00, 0x00, 
  0x89, 0x83, 0xf0, 0x00, 0x44, 0xf0, 0x40, 
  0x42, 0xc1, 0x64, 0x68, 0x66, 0xea, 0xfc, 0x44, 
  0xf0, 0x00, 0x68, 0x56, 0xfb, 0x00, 0x44, 0x60, 
  0x00, 0x09, 0xac, 0xdc, 0x00, 0x44, 0x70, 0x10, 
  0xc9, 0x77, 0xf4, 0x00, 0x44, 0x30, 0x40, 
  0x3c, 0xc1, 0x64, 0x68, 0x66, 0xea, 0xfc, 0x44, 
  0xf0, 0x00, 0x68, 0x56, 0xfb, 0x00, 0x44, 0x60, 
  0x00, 0x09, 0xaa, 0xfc, 0x40, 0x44, 0xa0, 0x10, 
  0xc9, 0xc7, 0xc4, 0x00, 0x44, 0x30, 0x80, 
  0x30, 0x59, 0x65, 0xc8, 0xda, 0xdb, 0x3e, 0x44, 
  0x00, 0x00, 0x68, 0x50, 0x60, 0x18, 0x44, 0x60, 
  0x00, 0x88, 0xc8, 0xf0, 0x38, 0x44, 0x00, 0x00, 
  0x78, 0x70, 0x60, 0x08, 0x44, 0x60, 0x00, 
  0x10, 0x59, 0xe5, 0xf8, 0x88, 0x9f, 0x30, 0x00, 
  0x00, 0x81, 0x88, 0x86, 0x74, 0x00, 0x00, 0xe0, 
  0x40, 0x08, 0x07, 0xf0, 0x90, 0x44, 0x20, 0x00, 
  0xb8, 0xb7, 0xdc, 0x48, 0x44, 0xb0, 0x00, 
  0x6e, 0x31, 0x67, 0x88, 0x4c, 0xf0, 0x14, 0x44, 
  0x90, 0xb0, 0x98, 0x55, 0xb6, 0x00, 0x44, 0xf0, 
  0x20, 0xc8, 0x7b, 0xf3, 0x1c, 0x44, 0xe0, 0x30, 
  0x88, 0xbd, 0xef, 0x18, 0x44, 0x60, 0x00, 
  0x6e, 0x31, 0x67, 0x58, 0x45, 0xf0, 0x0c, 0x44, 
  0x90, 0xb0, 0x98, 0x55, 0xb6, 0x00, 0x44, 0xf0, 
  0x20, 0x98, 0x79, 0xf3, 0x20, 0x44, 0xe0, 0x30, 
  0x58, 0xb5, 0xef, 0x0c, 0x44, 0x60, 0x00, 
  0x27, 0xb9, 0x64, 0x38, 0x50, 0x96, 0x00, 0x44, 
  0x70, 0x07, 0x49, 0x46, 0x96, 0x10, 0x44, 0x30, 
  0x00, 0x28, 0x64, 0xe4, 0x04, 0x44, 0xc0, 0x00, 
  0x98, 0x4a, 0x98, 0x08, 0x44, 0xf0, 0x30, 
  0x5c, 0xc1, 0x65, 0x08, 0x06, 0x70, 0x50, 0x44, 
  0x60, 0x10, 0x58, 0x5b, 0x64, 0x00, 0x44, 0x20, 
  0x18, 0x0b, 0x20, 0x30, 0x1c, 0x54, 0x60, 0x10, 
  0x2a, 0x65, 0x7f, 0x00, 0x44, 0x90, 0x10, 
  0x64, 0xd1, 0x65, 0x28, 0x04, 0xc0, 0x60, 0x46, 
  0x30, 0x9f, 0x58, 0x53, 0x44, 0x00, 0x66, 0x20, 
  0x10, 0x0b, 0x20, 0x30, 0x1c, 0x77, 0x70, 0x10, 
  0x2a, 0x64, 0x56, 0x08, 0x66, 0x30, 0x68
};

const uint8 audfmt_smaf_fm_2op_drum_dls_lookup[40][17] = {
  0x58, 0x79, 0x61, 0x98, 0xf0, 0xf0, 0x08, 0x24, 0x26, 
  0x82, 0x98, 0xfa, 0xf1, 0x00, 0x42, 0x22, 0x00, 
  0x1d, 0x79, 0x60, 0x58, 0x96, 0xd0, 0x00, 0x44, 0x00, 
  0xc7, 0x88, 0x98, 0x82, 0x10, 0x24, 0x00, 0x00, 
  0x3c, 0x79, 0x20, 0x00, 0x32, 0xa0, 0x00, 0x20, 0x15, 
  0xc7, 0x00, 0x75, 0x63, 0x28, 0x14, 0x00, 0x03, 
  0x24, 0x79, 0x60, 0x38, 0x4a, 0xd0, 0x00, 0x04, 0x06, 
  0x87, 0x88, 0x68, 0xd2, 0x08, 0x44, 0x02, 0x00, 
  0x18, 0x79, 0x60, 0x00, 0x58, 0xc2, 0x00, 0x02, 0x40, 
  0xc7, 0x00, 0x79, 0xb4, 0x04, 0x24, 0x26, 0x19, 
  0x12, 0x79, 0x20, 0x00, 0x20, 0x70, 0x34, 0x24, 0x83, 
  0x17, 0x00, 0x95, 0xf3, 0x00, 0x03, 0x06, 0xd0, 
  0x57, 0x41, 0x60, 0x58, 0x97, 0xff, 0x0c, 0x44, 0x70, 
  0x0e, 0x59, 0xf8, 0xaf, 0x00, 0x44, 0x50, 0x30, 
  0x5e, 0x79, 0x60, 0x98, 0xc9, 0xff, 0x01, 0x44, 0x40, 
  0x4b, 0x98, 0x7a, 0xfb, 0x00, 0x44, 0x20, 0xb0, 
  0x27, 0x79, 0xe0, 0x38, 0xb3, 0xf0, 0x30, 0x47, 0x20, 
  0xd7, 0x88, 0x97, 0xf3, 0x00, 0x44, 0x00, 0x30, 
  0x4a, 0x79, 0x60, 0x08, 0x07, 0xff, 0x00, 0x44, 0x70, 
  0x0e, 0xf9, 0xf8, 0xaf, 0x00, 0x44, 0x20, 0x30, 
  0x63, 0x69, 0x60, 0x08, 0xf0, 0xf0, 0x00, 0x44, 0xf0, 
  0x05, 0x69, 0x63, 0x61, 0x00, 0x44, 0xf0, 0x38, 
  0x6f, 0x59, 0xa0, 0x28, 0x5c, 0xf4, 0x00, 0x46, 0x63, 
  0x38, 0x68, 0x57, 0xf2, 0x00, 0x44, 0x76, 0x80, 
  0x6d, 0x91, 0x60, 0x48, 0x17, 0x82, 0x4c, 0x44, 0xa0, 
  0x66, 0x78, 0x67, 0xe5, 0x00, 0x44, 0xf0, 0x48, 
  0x54, 0xa9, 0x60, 0x79, 0x79, 0xf4, 0x00, 0x44, 0x10, 
  0x1f, 0x59, 0x53, 0xf0, 0x08, 0x44, 0x00, 0x00, 
  0x1a, 0x39, 0x60, 0x18, 0x10, 0xf0, 0x08, 0x44, 0x10, 
  0x2f, 0x49, 0x95, 0xf2, 0x18, 0x44, 0x10, 0x00, 
  0x2b, 0xc1, 0x60, 0xc8, 0xc5, 0xf0, 0x00, 0x44, 0x60, 
  0x03, 0x88, 0x75, 0xf0, 0x00, 0x44, 0xb0, 0x00, 
  0x21, 0xc1, 0x60, 0xc8, 0xc5, 0xf0, 0x00, 0x44, 0x60, 
  0x03, 0x88, 0x75, 0xf0, 0x00, 0x44, 0xb0, 0x00, 
  0x4c, 0x49, 0xe0, 0xa8, 0xae, 0xe1, 0x18, 0x46, 0x02, 
  0x04, 0x98, 0xa8, 0xf1, 0x00, 0x44, 0x10, 0x40, 
  0x13, 0x49, 0x60, 0x58, 0x8a, 0xac, 0x58, 0x44, 0xa0, 
  0x1f, 0x78, 0x96, 0xc4, 0x00, 0x44, 0xc0, 0x00, 
  0x39, 0x59, 0x60, 0x49, 0xf9, 0xf4, 0x1c, 0x44, 0x10, 
  0x00, 0x58, 0x36, 0xfb, 0x00, 0x44, 0x00, 0x00, 
  0x33, 0xa9, 0xe0, 0x68, 0xea, 0xf6, 0x38, 0x33, 0x83, 
  0x36, 0x58, 0x56, 0xf2, 0x00, 0x15, 0x16, 0x60, 
  0x2e, 0xa9, 0xa0, 0x68, 0xea, 0xf6, 0x24, 0x23, 0x63, 
  0x34, 0x58, 0x56, 0xf2, 0x04, 0x25, 0x16, 0x20, 
  0x4d, 0x39, 0x60, 0x68, 0x6c, 0xe2, 0x4c, 0x44, 0x77, 
  0x05, 0x68, 0x68, 0xf1, 0x10, 0x44, 0x23, 0x00, 
  0x48, 0x39, 0x60, 0x68, 0x6c, 0xe2, 0x4c, 0x44, 0x73, 
  0x07, 0x68, 0x68, 0xf1, 0x24, 0x44, 0x27, 0x00, 
  0x64, 0x39, 0x60, 0x68, 0xf5, 0xa4, 0x3c, 0x44, 0xf0, 
  0xa7, 0xa8, 0xb7, 0x80, 0x00, 0x44, 0xf0, 0x18, 
  0x6d, 0x31, 0x60, 0x68, 0xf5, 0xa4, 0x3c, 0x44, 0xf0, 
  0xa7, 0x98, 0xb7, 0x80, 0x00, 0x44, 0xf0, 0x18, 
  0x32, 0xc1, 0xe0, 0x02, 0x70, 0xf0, 0x5c, 0x47, 0x00, 
  0x02, 0x02, 0xc1, 0x80, 0x24, 0x20, 0xf0, 0x00, 
  0x30, 0xc1, 0xe0, 0x02, 0x70, 0xf0, 0x5c, 0x47, 0x00, 
  0x02, 0x02, 0xc1, 0x80, 0x18, 0x20, 0xf0, 0x00, 
  0x31, 0xb9, 0x60, 0x08, 0xb9, 0x80, 0x4c, 0x00, 0x00, 
  0x17, 0x89, 0x87, 0x84, 0x00, 0x00, 0xf0, 0x90, 
  0x30, 0xb9, 0x60, 0x08, 0xb9, 0x80, 0x4c, 0x00, 0x00, 
  0x17, 0x89, 0x84, 0x84, 0x00, 0x00, 0xf0, 0x90, 
  0x45, 0xa9, 0x61, 0xa8, 0xfc, 0xff, 0x58, 0x44, 0x60, 
  0x00, 0x89, 0x87, 0xff, 0x00, 0x44, 0x50, 0x00, 
  0x49, 0xc9, 0x60, 0x99, 0x9a, 0xf2, 0x4a, 0x40, 0x82, 
  0x06, 0x89, 0x87, 0xff, 0x00, 0x40, 0x21, 0x00, 
  0x44, 0xc9, 0x60, 0x99, 0x9a, 0xf2, 0x4a, 0x40, 0x82, 
  0x06, 0x89, 0x87, 0xff, 0x00, 0x40, 0x21, 0x00, 
  0x50, 0x59, 0xe1, 0xe8, 0xe6, 0x6d, 0x3c, 0x45, 0x17, 
  0x05, 0xe8, 0xe6, 0x74, 0x0c, 0x54, 0x13, 0x00, 
  0x10, 0x59, 0xe0, 0xf8, 0x88, 0x9f, 0x30, 0x00, 0x00, 
  0x81, 0x88, 0x86, 0x74, 0x00, 0x00, 0xe0, 0x40, 
  0x58, 0x31, 0x60, 0x78, 0x77, 0xf1, 0x32, 0x42, 0x54, 
  0x00, 0x98, 0x97, 0xdf, 0x00, 0x22, 0xa1, 0x00, 
  0x58, 0x31, 0x60, 0x08, 0x0f, 0xf0, 0x32, 0x42, 0x54, 
  0x00, 0x58, 0x55, 0xcf, 0x0c, 0x24, 0xa1, 0x00, 
  0x5e, 0xb9, 0x60, 0x38, 0x13, 0xb2, 0x02, 0x42, 0xa2, 
  0x07, 0x98, 0x6d, 0x74, 0x18, 0x44, 0xf4, 0x00, 
  0x4c, 0xc1, 0xe1, 0x68, 0x65, 0xc2, 0x02, 0x07, 0xf4, 
  0xd4, 0x58, 0xdb, 0xb1, 0x00, 0x31, 0xf2, 0x98, 
  0x3e, 0xd1, 0x60, 0x58, 0x54, 0x3e, 0x00, 0x44, 0xf0, 
  0x1b, 0x48, 0x37, 0x40, 0x1a, 0x44, 0x90, 0x08
};

/* The following two lookup tables are used to determine which intrument to
** use for SMAF Phrase files.
*/

const uint8 audfmt_smaf_phrase_instr_dls_lookup_4op[128][22] = {
  0x65, 0x01, 0x15, 0xe1, 0xb5, 0x3e, 0xa0, 0x11, 
  0xf1, 0xb8, 0x02, 0xa0, 0x15, 0x41, 0xab, 0x64, 
  0xa0, 0x15, 0x71, 0xad, 0x00, 0xa0,  
  0x65, 0x01, 0x11, 0x21, 0xce, 0x1d, 0xa0, 0x11, 
  0x62, 0xef, 0x10, 0xa0, 0x11, 0x51, 0xcd, 0x50, 
  0xa0, 0x15, 0x62, 0xce, 0x10, 0xa0,  
  0x75, 0x01, 0x31, 0x43, 0xfd, 0x4d, 0xa0, 0x11, 
  0x51, 0xbe, 0x00, 0xa0, 0x11, 0x41, 0xbe, 0x1d, 
  0xa0, 0x11, 0x51, 0xbf, 0x00, 0x88,  
  0x66, 0x01, 0x01, 0x41, 0xfe, 0x00, 0xa2, 0x48, 
  0x43, 0xce, 0x37, 0xa0, 0x18, 0x51, 0xc3, 0x61, 
  0x08, 0x29, 0x51, 0xcd, 0x00, 0xa0,  
  0x65, 0x01, 0x31, 0x72, 0xfe, 0x6e, 0xa0, 0x18, 
  0x82, 0xff, 0x02, 0xa0, 0x11, 0x51, 0xbe, 0x43,  
  0xa0, 0x11, 0x72, 0xff, 0x00, 0xa0,  
  0x65, 0x01, 0xe9, 0x72, 0xfd, 0x62, 0x00, 0x10, 
  0x83, 0xff, 0x03, 0xa0, 0x19, 0x51, 0xbf, 0x32, 
  0x00, 0x11, 0x71, 0xff, 0x00, 0xa0,  
  0x5e, 0x01, 0x15, 0x62, 0xff, 0x01, 0xa5, 0x45, 
  0x52, 0xf3, 0x53, 0xa3, 0x45, 0x13, 0xf6, 0x76, 
  0xa4, 0x11, 0x72, 0xff, 0x10, 0xa4,  
  0x6e, 0x01, 0x01, 0x72, 0xff, 0x03, 0xa2, 0x15, 
  0x51, 0xef, 0x53, 0xa2, 0x15, 0x41, 0xee, 0x6a, 
  0xa4, 0x11, 0x91, 0xfe, 0x00, 0xa4,  
  0x7d, 0x01, 0x20, 0x6c, 0xfe, 0x18, 0xa0, 0x10, 
  0x54, 0x9e, 0x10, 0xa0, 0x41, 0x66, 0xfc, 0x2b, 
  0xa2, 0x10, 0x44, 0xfe, 0x20, 0xa0,  
  0x6d, 0x01, 0x50, 0x33, 0xfe, 0x8c, 0xa2, 0x10, 
  0x33, 0xce, 0x0e, 0xa0, 0x90, 0x33, 0xfc, 0x72, 
  0xa1, 0x10, 0x43, 0xfe, 0x2a, 0xa0,  
  0x45, 0x01, 0x71, 0x35, 0x50, 0x62, 0xa0, 0x10, 
  0x34, 0xf0, 0x01, 0xa0, 0xd1, 0x25, 0xa0, 0x90, 
  0xa0, 0x10, 0x35, 0xf0, 0x14, 0xa0,  
  0x4d, 0x01, 0x4c, 0x6a, 0xf9, 0x7c, 0xad, 0x48, 
  0x54, 0x9f, 0x16, 0xa8, 0x69, 0x63, 0xfe, 0x8b, 
  0xa9, 0x18, 0x52, 0xaf, 0x00, 0xa8,  
  0x7d, 0x01, 0xc1, 0x47, 0xaf, 0xa1, 0xa0, 0x14, 
  0x55, 0x8f, 0x00, 0xa0, 0x71, 0x67, 0xcf, 0x84, 
  0xa0, 0x14, 0x65, 0xaf, 0x00, 0xa0,  
  0x55, 0x01, 0x90, 0x69, 0xfd, 0x5a, 0xa0, 0x10, 
  0x57, 0xfd, 0x02, 0xa8, 0x21, 0x66, 0xfa, 0x2e, 
  0xa0, 0x10, 0x66, 0xfe, 0x02, 0xa0,  
  0x55, 0x01, 0x48, 0x34, 0xdb, 0x42, 0x00, 0x10, 
  0x42, 0xcb, 0x14, 0xa0, 0xa0, 0x23, 0xd8, 0x65, 
  0xa0, 0x20, 0x33, 0xae, 0x02, 0xa0,  
  0x77, 0x01, 0x10, 0x44, 0xff, 0x02, 0xa1, 0x30, 
  0x43, 0x75, 0x21, 0xa1, 0x10, 0x43, 0xc0, 0x00, 
  0xa0, 0x10, 0x55, 0xbf, 0x00, 0xa0,  
  0x5f, 0x01, 0x04, 0x74, 0xa0, 0x02, 0x08, 0x24, 
  0x05, 0xf0, 0x56, 0x00, 0x14, 0x75, 0xd1, 0x00, 
  0x08, 0x24, 0x71, 0xc0, 0x01, 0x0c,  
  0x67, 0x01, 0x14, 0x75, 0xf1, 0x02, 0x08, 0x14, 
  0x08, 0xc1, 0x50, 0x00, 0x14, 0x75, 0xc2, 0x00, 
  0x08, 0x04, 0x76, 0xf0, 0x00, 0x00,  
  0x6f, 0x01, 0x6c, 0xaf, 0xf3, 0x02, 0x08, 0x1c, 
  0xaf, 0xa0, 0x19, 0x00, 0x1c, 0xaf, 0xa0, 0x02, 
  0x08, 0x0c, 0xaf, 0xa0, 0x02, 0x0c,  
  0x67, 0x01, 0x24, 0x4f, 0x90, 0x10, 0x00, 0x15, 
  0x2f, 0xf0, 0x50, 0x00, 0x15, 0x4f, 0x80, 0x28, 
  0x00, 0x45, 0x41, 0x60, 0x20, 0x05,  
  0x75, 0x01, 0x34, 0x58, 0xc1, 0x5a, 0x00, 0x15, 
  0x6f, 0x50, 0x1c, 0x08, 0x15, 0x5f, 0xc0, 0x5d, 
  0x00, 0x15, 0x7f, 0x60, 0x06, 0x00,  
  0x55, 0x01, 0x34, 0x02, 0x71, 0x5c, 0x04, 0x14, 
  0xa2, 0x62, 0x02, 0x00, 0x14, 0x0f, 0x61, 0x64, 
  0x04, 0x24, 0xaf, 0x60, 0x14, 0x08,  
  0x44, 0x01, 0x24, 0x6f, 0xf0, 0x53, 0xa0, 0x14, 
  0x5f, 0xf0, 0x90, 0xa0, 0x14, 0x5f, 0xf0, 0x64, 
  0x8a, 0x24, 0x9f, 0x60, 0x02, 0x88,  
  0x75, 0x01, 0x34, 0xa2, 0x61, 0x50, 0xa1, 0x14, 
  0xa2, 0x72, 0x26, 0xa0, 0x34, 0xa2, 0x61, 0x78, 
  0xa4, 0x24, 0xa2, 0x72, 0x36, 0xa0,  
  0x75, 0x01, 0x20, 0x43, 0xf7, 0x62, 0xa0, 0x18, 
  0x83, 0xff, 0x00, 0x60, 0x10, 0x43, 0xd7, 0x65, 
  0xa1, 0x10, 0x83, 0xcf, 0x01, 0x98,  
  0x55, 0x01, 0x55, 0x53, 0xff, 0x65, 0xa0, 0x15, 
  0x42, 0xff, 0x02, 0xa4, 0x54, 0x65, 0xff, 0xb5, 
  0xa0, 0x11, 0x53, 0xff, 0x02, 0xa3,  
  0x75, 0x01, 0x44, 0xa2, 0xff, 0x61, 0xa0, 0x14, 
  0xa2, 0xff, 0x01, 0xa0, 0x34, 0xa2, 0xff, 0x65, 
  0xa0, 0x14, 0x82, 0xff, 0x00, 0xa0,  
  0x64, 0x01, 0x50, 0x2a, 0xf2, 0x31, 0xa4, 0x20, 
  0x33, 0xff, 0x6c, 0xa5, 0x40, 0x32, 0xcf, 0x66, 
  0xa0, 0x11, 0x71, 0xcf, 0x14, 0xa0,  
  0x7d, 0x01, 0x21, 0xf9, 0xca, 0x3c, 0xa0, 0x10, 
  0x85, 0x9f, 0x00, 0xa0, 0x10, 0x83, 0xc9, 0x2b, 
  0xa0, 0x21, 0x83, 0xcf, 0x10, 0xa0,  
  0x7c, 0x01, 0x25, 0x2c, 0xff, 0x00, 0xa0, 0x24, 
  0x11, 0x91, 0x5d, 0xa0, 0x04, 0xa2, 0x94, 0x24, 
  0xa2, 0x24, 0xa1, 0x92, 0x00, 0xa4,  
  0x64, 0x01, 0x24, 0x27, 0xf4, 0x15, 0xa2, 0x04, 
  0xa1, 0xbf, 0x68, 0xa2, 0x14, 0xa1, 0xb2, 0x48, 
  0xa0, 0x14, 0xa2, 0xa1, 0x00, 0xa4,  
  0x5d, 0x01, 0x20, 0xf0, 0xf4, 0x03, 0xa1, 0x41, 
  0x83, 0xaf, 0x75, 0xa0, 0x20, 0x84, 0xff, 0x30, 
  0xa2, 0x80, 0x82, 0x6f, 0x20, 0xa0,  
  0x5d, 0x01, 0x00, 0x57, 0xf1, 0x76, 0xa2, 0x10, 
  0x83, 0xac, 0x00, 0xa0, 0x10, 0x11, 0x73, 0x1f, 
  0xa0, 0x10, 0x83, 0x7c, 0x00, 0xa0,  
  0x6d, 0x01, 0x11, 0x31, 0xaf, 0x33, 0xa0, 0x15, 
  0x62, 0xf6, 0x00, 0xa0, 0x41, 0x31, 0xaf, 0x66, 
  0xa0, 0x11, 0x62, 0xf6, 0x20, 0xa0,  
  0x75, 0x01, 0x15, 0x32, 0xfc, 0x5a, 0xa0, 0x25, 
  0x83, 0xf2, 0x00, 0xa0, 0x10, 0x71, 0xfd, 0x43, 
  0xa0, 0x10, 0x81, 0xff, 0x00, 0xa0,  
  0x75, 0x01, 0x15, 0x81, 0x52, 0x59, 0xa0, 0x25, 
  0x81, 0x74, 0x00, 0xa0, 0x15, 0x81, 0x74, 0x50, 
  0xa0, 0x15, 0x81, 0x74, 0x00, 0xa0,  
  0x65, 0x01, 0x10, 0xa1, 0xd9, 0x1e, 0xa0, 0x14, 
  0xf2, 0xcd, 0x01, 0xa0, 0x40, 0xa9, 0xfd, 0x00, 
  0xa0, 0x10, 0xf7, 0x9d, 0x02, 0xa0,  
  0x6d, 0x01, 0x10, 0xa2, 0xfe, 0x21, 0xa0, 0x14, 
  0xa2, 0xfd, 0x02, 0xa0, 0x20, 0xa9, 0xfd, 0x0e, 
  0xa2, 0x10, 0xf3, 0xed, 0x00, 0xa0,  
  0x74, 0x01, 0x04, 0x86, 0xfc, 0x44, 0xa2, 0x14, 
  0x84, 0xf0, 0x32, 0xa0, 0x14, 0x83, 0xf6, 0x78, 
  0xa0, 0x10, 0x82, 0xfc, 0x00, 0xa0,  
  0x75, 0x01, 0x14, 0x85, 0xfc, 0x36, 0xa1, 0x10, 
  0x81, 0xfc, 0x00, 0xa0, 0x10, 0x23, 0x54, 0x3e, 
  0xa0, 0x10, 0x82, 0xfc, 0x00, 0xa0,  
  0x95, 0x01, 0x15, 0x39, 0x81, 0x16, 0x01, 0x1c, 
  0x76, 0x52, 0x00, 0x20, 0x21, 0x76, 0xc0, 0x25, 
  0x04, 0x18, 0x73, 0x6e, 0x32, 0x80,  
  0x95, 0x01, 0x15, 0x3d, 0xf1, 0x34, 0xa1, 0x1c, 
  0x76, 0x50, 0x00, 0x20, 0x11, 0x32, 0xaf, 0x20, 
  0xa1, 0x24, 0x6f, 0x50, 0x00, 0x20,  
  0xb5, 0x01, 0x15, 0x3f, 0x80, 0x70, 0xa5, 0x1c, 
  0x84, 0x52, 0x00, 0xa0, 0x10, 0x51, 0x7a, 0x03, 
  0xa1, 0x1c, 0x86, 0x50, 0x00, 0x60,  
  0xbd, 0x01, 0x3d, 0x38, 0xa6, 0x61, 0x62, 0x0c, 
  0x74, 0x71, 0x00, 0x62, 0x14, 0x7f, 0xb0, 0x55, 
  0xa0, 0x14, 0x7f, 0x60, 0x00, 0xa1,  
  0xa5, 0x01, 0x1c, 0x52, 0x81, 0x5c, 0xac, 0x14, 
  0x7f, 0x60, 0x00, 0xa8, 0x3c, 0x63, 0xb0, 0x33, 
  0xa8, 0x14, 0x85, 0x81, 0x02, 0xa8,  
  0x7c, 0x01, 0x14, 0x46, 0xf8, 0x89, 0xa0, 0x11, 
  0x54, 0x87, 0x44, 0xa0, 0x10, 0x24, 0x93, 0xa5, 
  0xa0, 0x11, 0x55, 0x83, 0x00, 0xa0,  
  0x5e, 0x01, 0x10, 0x22, 0xc7, 0x01, 0xa0, 0x21, 
  0x29, 0x87, 0xbf, 0xa0, 0x44, 0x22, 0x47, 0xa5, 
  0xa0, 0x11, 0x24, 0x93, 0x00, 0xa0,  
  0x7d, 0x01, 0x01, 0x25, 0xff, 0x51, 0xa5, 0x11, 
  0x22, 0xfc, 0x00, 0xa0, 0x11, 0x37, 0xf0, 0x29, 
  0xa0, 0x11, 0x44, 0xff, 0x02, 0xa0,  
  0xbd, 0x01, 0x15, 0x31, 0x60, 0x52, 0x9a, 0x14, 
  0x74, 0x61, 0x24, 0xa2, 0x14, 0x75, 0x61, 0x3a, 
  0xa1, 0x1c, 0x75, 0x61, 0x00, 0x60,  
  0xb5, 0x01, 0x15, 0x31, 0xf2, 0x59, 0x00, 0x14, 
  0x70, 0x5f, 0x00, 0x01, 0x14, 0x3f, 0xa3, 0x31, 
  0x02, 0x1c, 0x5f, 0x80, 0x00, 0x41,  
  0xad, 0x01, 0x14, 0x28, 0x81, 0x8c, 0x01, 0x1c, 
  0x5f, 0x50, 0x00, 0x41, 0x04, 0x2b, 0x90, 0x1c, 
  0x03, 0x1c, 0x5f, 0x60, 0x01, 0x00,  
  0x9d, 0x01, 0x14, 0x21, 0x7f, 0x31, 0x05, 0x14, 
  0x5e, 0x50, 0x01, 0x02, 0x14, 0x21, 0x44, 0x03, 
  0x01, 0x1c, 0x5f, 0x50, 0x02, 0x80,  
  0x7d, 0x01, 0x60, 0x00, 0xaf, 0x18, 0xa6, 0x44, 
  0x83, 0x73, 0x60, 0x00, 0x14, 0x7f, 0x70, 0x51, 
  0x00, 0x14, 0x7f, 0x50, 0x00, 0x00,  
  0x7d, 0x01, 0x20, 0x40, 0x51, 0x40, 0xa0, 0x44, 
  0x57, 0x50, 0x8c, 0xa0, 0x14, 0x34, 0x51, 0x42, 
  0xa0, 0x14, 0x51, 0x42, 0x00, 0xa0,  
  0x95, 0x01, 0x30, 0x40, 0xaf, 0x6e, 0x88, 0x2c, 
  0x7f, 0x71, 0x10, 0x08, 0x14, 0x7f, 0x70, 0x4d, 
  0x80, 0x1c, 0x7f, 0x60, 0x00, 0x08,  
  0x6f, 0x01, 0x00, 0x65, 0xa6, 0x00, 0xa4, 0x10, 
  0x35, 0xc1, 0x00, 0xa6, 0x10, 0x68, 0xc0, 0x00, 
  0xa6, 0x20, 0x74, 0xa0, 0x00, 0xa4,  
  0x7d, 0x01, 0x14, 0x68, 0x91, 0x52, 0x00, 0x14, 
  0x8f, 0x80, 0x00, 0x00, 0x14, 0x04, 0x81, 0x54, 
  0x00, 0x1c, 0x8f, 0xa0, 0x20, 0x00,  
  0x75, 0x01, 0x14, 0x75, 0x81, 0x52, 0xa0, 0x14, 
  0x9f, 0x70, 0x00, 0xa0, 0x14, 0x04, 0x81, 0x48, 
  0xa0, 0x1c, 0x7f, 0xa0, 0x1c, 0x00,  
  0x7d, 0x01, 0x14, 0x26, 0x81, 0x5d, 0xa0, 0x1c, 
  0x72, 0x83, 0x00, 0x00, 0x15, 0x21, 0x72, 0x5e, 
  0x08, 0x1d, 0x61, 0x62, 0x00, 0x00,  
  0x55, 0x01, 0x75, 0x71, 0x72, 0x11, 0x01, 0x1c, 
  0x91, 0x84, 0x02, 0x01, 0x95, 0x67, 0x82, 0x50, 
  0x09, 0x14, 0x97, 0x72, 0x00, 0x04,  
  0x45, 0x01, 0x14, 0x07, 0x61, 0x1f, 0x01, 0x14, 
  0x7f, 0x80, 0x8c, 0x00, 0x14, 0x29, 0x62, 0x3e, 
  0x00, 0x14, 0x7f, 0x80, 0x00, 0x00,  
  0x75, 0x01, 0x14, 0x21, 0x72, 0x5c, 0xa0, 0x14, 
  0x8f, 0x70, 0x00, 0xa0, 0x14, 0x5a, 0x73, 0x78, 
  0xa0, 0x14, 0x8a, 0x73, 0x00, 0xa0,  
  0x7d, 0x01, 0x14, 0x2f, 0xc0, 0x68, 0x00, 0x14, 
  0x8f, 0xc0, 0x00, 0x00, 0x14, 0x55, 0x72, 0x24, 
  0x01, 0x14, 0x8f, 0x70, 0x00, 0x00,  
  0x6d, 0x01, 0x14, 0x22, 0xf1, 0x4a, 0x08, 0x1c, 
  0x7f, 0xc1, 0x00, 0x00, 0x14, 0x55, 0x41, 0x32, 
  0xa1, 0x14, 0x7f, 0x81, 0x00, 0xa0,  
  0xad, 0x01, 0x15, 0x03, 0xa0, 0x4e, 0x89, 0x1c, 
  0x82, 0x70, 0x00, 0x20, 0x14, 0x05, 0x80, 0x49, 
  0xa2, 0x1d, 0x64, 0x62, 0x00, 0x01,  
  0x4d, 0x01, 0x15, 0x23, 0x90, 0x60, 0xa4, 0x14, 
  0x82, 0x60, 0x00, 0x00, 0x44, 0x25, 0xc0, 0x5d, 
  0xa1, 0x1d, 0x84, 0x52, 0x00, 0x01,  
  0x6d, 0x01, 0x15, 0x63, 0x90, 0x29, 0x01, 0x1d, 
  0x82, 0x60, 0x00, 0x00, 0x14, 0x25, 0x80, 0x59, 
  0xa2, 0x15, 0x84, 0x72, 0x00, 0x02,  
  0x5d, 0x01, 0x15, 0x43, 0xa0, 0x2c, 0xa1, 0x25, 
  0xa2, 0xb0, 0x00, 0x00, 0x24, 0x45, 0x90, 0x5c, 
  0xa4, 0x15, 0x74, 0xc1, 0x00, 0x08,  
  0xac, 0x01, 0x1c, 0x29, 0xa2, 0xa4, 0x01, 0x24, 
  0x28, 0xa3, 0x6d, 0xa2, 0x14, 0x4a, 0xa1, 0x88, 
  0x08, 0x4c, 0x7f, 0x80, 0x02, 0x00,  
  0x8c, 0x01, 0x14, 0x27, 0x72, 0x50, 0xa0, 0x3d, 
  0x2a, 0x72, 0x81, 0x08, 0x24, 0x2a, 0x71, 0x4c, 
  0x80, 0x1c, 0x8a, 0x71, 0x00, 0x20,  
  0x6d, 0x01, 0x14, 0x85, 0xa1, 0x53, 0xa5, 0x25, 
  0x81, 0x61, 0x00, 0xa0, 0x15, 0x25, 0x71, 0x98, 
  0xa4, 0x35, 0x71, 0x61, 0x20, 0xa0,  
  0x6d, 0x01, 0x25, 0x1b, 0x92, 0x42, 0xa0, 0x14, 
  0x72, 0x61, 0x00, 0xa0, 0x35, 0x1b, 0x92, 0x7a, 
  0xa1, 0x14, 0x82, 0x71, 0x00, 0xa0,  
  0x7d, 0x01, 0x54, 0x1c, 0xd1, 0x00, 0xa0, 0x3c, 
  0x87, 0xbf, 0x68, 0x08, 0x1c, 0x75, 0x85, 0x78, 
  0x08, 0x1c, 0xa5, 0x81, 0x00, 0x08,  
  0x75, 0x01, 0x54, 0x1a, 0xd1, 0x00, 0xa0, 0x3c, 
  0x86, 0xa6, 0xac, 0x08, 0x1c, 0xf8, 0x85, 0x19, 
  0x08, 0x1c, 0xa5, 0x61, 0x00, 0x08,  
  0xbd, 0x01, 0x44, 0x99, 0xa2, 0x71, 0x08, 0x14, 
  0x8f, 0x60, 0x02, 0xa0, 0x24, 0x56, 0x91, 0xaa, 
  0x08, 0x14, 0x7f, 0x60, 0x00, 0x08,  
  0x7d, 0x01, 0x64, 0x15, 0xff, 0x01, 0xa0, 0x14, 
  0x86, 0xa6, 0x6c, 0xa0, 0x24, 0x88, 0x85, 0x32, 
  0xa0, 0x14, 0xa5, 0x61, 0x00, 0x08,  
  0x7e, 0x01, 0x14, 0x66, 0x7c, 0x34, 0x08, 0x64, 
  0x67, 0x88, 0x75, 0xa0, 0x24, 0x66, 0x65, 0x59, 
  0xa0, 0x14, 0x65, 0x62, 0x00, 0x08,  
  0x7c, 0x01, 0x60, 0x1f, 0x90, 0x00, 0xa0, 0x48, 
  0x56, 0x74, 0xb4, 0x00, 0x2c, 0x66, 0x54, 0x25, 
  0x00, 0x14, 0x65, 0x43, 0x00, 0x08,  
  0x75, 0x01, 0x1c, 0x47, 0x61, 0x9d, 0x09, 0x1c, 
  0x74, 0x51, 0x00, 0x28, 0x14, 0x47, 0x61, 0xb5, 
  0xa1, 0x14, 0x64, 0x41, 0x00, 0xa0,  
  0x45, 0x01, 0x14, 0x1f, 0xd0, 0x9c, 0x00, 0x14, 
  0x86, 0x71, 0x00, 0x00, 0x14, 0x65, 0x93, 0x43, 
  0x28, 0x14, 0x76, 0x70, 0x00, 0x00,  
  0x5d, 0x01, 0x14, 0x2f, 0xf0, 0x55, 0xa0, 0x00, 
  0x71, 0xff, 0x00, 0xa2, 0x20, 0x2f, 0xf3, 0x32, 
  0x0a, 0x1c, 0x8f, 0xf0, 0x00, 0x40,  
  0x64, 0x01, 0x14, 0x2f, 0xf0, 0x64, 0xa5, 0x14, 
  0x2f, 0xf0, 0x5d, 0xa0, 0x24, 0x2f, 0xf0, 0x65, 
  0x8d, 0x1c, 0x7f, 0xc0, 0x00, 0x20,  
  0x7d, 0x01, 0x44, 0x74, 0xc8, 0x00, 0x08, 0x44, 
  0x66, 0xc7, 0x5d, 0xa0, 0x2c, 0x56, 0x85, 0x19, 
  0x00, 0x14, 0x84, 0x61, 0x02, 0x08,  
  0x6d, 0x01, 0x14, 0xa3, 0xa4, 0x1d, 0xa1, 0x10, 
  0xa4, 0x8f, 0x00, 0xa0, 0x14, 0x26, 0x61, 0x52, 
  0xa0, 0x14, 0x66, 0x90, 0x00, 0xa0,  
  0x6c, 0x01, 0x21, 0x29, 0xbf, 0x34, 0xa4, 0x10, 
  0x11, 0x91, 0x68, 0xa0, 0x14, 0xa2, 0x92, 0x66, 
  0xa1, 0x24, 0xa1, 0x92, 0x00, 0xa0,  
  0xad, 0x01, 0x2c, 0x5d, 0x71, 0x5e, 0x00, 0x25, 
  0x52, 0x71, 0x05, 0x00, 0x34, 0x5f, 0x50, 0x65, 
  0x08, 0x14, 0x7f, 0x70, 0x00, 0x00,  
  0x65, 0x01, 0x3c, 0x7f, 0xc0, 0x50, 0xa1, 0x3c, 
  0x7f, 0xc0, 0x30, 0xa0, 0x10, 0x63, 0xff, 0x3c, 
  0xa0, 0x1c, 0x7f, 0x80, 0x00, 0xa1,  
  0x75, 0x01, 0x10, 0x93, 0xff, 0x64, 0xa0, 0x14, 
  0x92, 0xf0, 0x0c, 0xa0, 0x10, 0x93, 0xff, 0x8c, 
  0xa0, 0x14, 0x93, 0xa0, 0x00, 0xa0,  
  0x4d, 0x01, 0x71, 0x3f, 0x50, 0xa8, 0xa1, 0x80, 
  0x47, 0xf0, 0x4c, 0xa0, 0x14, 0x21, 0x37, 0x46, 
  0x00, 0x14, 0x51, 0x61, 0x00, 0x00,  
  0x6d, 0x01, 0x3c, 0x18, 0xa1, 0x59, 0x20, 0x15, 
  0x35, 0x20, 0x02, 0x88, 0x10, 0x11, 0xa3, 0x3a, 
  0x8c, 0x1d, 0x35, 0x20, 0x02, 0x20,  
  0x75, 0x01, 0x14, 0x42, 0xad, 0x6c, 0x00, 0x14, 
  0x65, 0xa7, 0x00, 0x00, 0x14, 0x33, 0xc4, 0x6c, 
  0x04, 0x14, 0x53, 0xc4, 0x00, 0x00,  
  0x45, 0x01, 0x64, 0x14, 0x52, 0x42, 0xa0, 0x44, 
  0x3a, 0x61, 0x72, 0xa0, 0x15, 0x14, 0x50, 0x35, 
  0xa1, 0x2d, 0x3a, 0x60, 0x00, 0x00,  
  0x6d, 0x01, 0x14, 0x31, 0x23, 0x3d, 0xa0, 0x24, 
  0x52, 0x44, 0x02, 0xa0, 0xa4, 0x21, 0x2d, 0xca, 
  0x08, 0x14, 0x52, 0x3d, 0x00, 0xa0,  
  0x65, 0x01, 0x14, 0x31, 0x63, 0x22, 0xa0, 0x1c, 
  0x52, 0x43, 0x00, 0x01, 0x34, 0x71, 0xa7, 0x68, 
  0x0d, 0x14, 0x7f, 0x70, 0x00, 0xa1,  
  0x6d, 0x01, 0x14, 0x31, 0x43, 0x54, 0xa0, 0x1c, 
  0x52, 0x33, 0x00, 0x00, 0x54, 0x41, 0x2e, 0x94, 
  0xa0, 0x1c, 0x52, 0x4d, 0x0c, 0x00,  
  0x45, 0x01, 0x14, 0x71, 0x24, 0x24, 0xa1, 0x14, 
  0x7f, 0x40, 0x00, 0xa0, 0x14, 0x41, 0x1e, 0x12, 
  0xa0, 0x1c, 0x52, 0x5d, 0x00, 0x00,  
  0x4d, 0x01, 0xa5, 0x68, 0xff, 0x0d, 0xa0, 0x19, 
  0x23, 0x80, 0x00, 0x38, 0x20, 0x63, 0xc7, 0x5c, 
  0xa0, 0x20, 0x46, 0xb3, 0x04, 0xa0,  
  0x5d, 0x01, 0x34, 0x21, 0x33, 0x32, 0xa2, 0x34, 
  0x32, 0x41, 0x62, 0xa0, 0x14, 0x31, 0x30, 0x38, 
  0xa1, 0x14, 0x31, 0x41, 0x00, 0xa0,  
  0x55, 0x01, 0x10, 0x33, 0x79, 0x31, 0xa0, 0x40, 
  0x32, 0xcb, 0x02, 0xa0, 0x60, 0x43, 0x79, 0x52, 
  0xa0, 0x10, 0x24, 0xa7, 0x00, 0xa0,  
  0xad, 0x01, 0x24, 0x42, 0xdf, 0x48, 0xa0, 0x18, 
  0x45, 0xff, 0x00, 0x00, 0x10, 0x43, 0x6f, 0x74, 
  0xa2, 0x14, 0x5f, 0xf0, 0x00, 0x18,  
  0x7d, 0x01, 0x44, 0x83, 0xc8, 0x51, 0xa6, 0x24,  
  0x73, 0xa8, 0x01, 0xa5, 0x20, 0x11, 0xf5, 0x31, 
  0xa0, 0x18, 0x52, 0xff, 0x00, 0x60,  
  0x5d, 0x01, 0x1c, 0x11, 0x15, 0xb6, 0xc1, 0x34, 
  0x31, 0x21, 0x00, 0x0d, 0x64, 0x31, 0x21, 0x64, 
  0xa0, 0x44, 0x31, 0x21, 0x3c, 0xa0,  
  0xad, 0x01, 0x20, 0x02, 0x19, 0x3d, 0x01, 0x24, 
  0x22, 0x71, 0x00, 0x00, 0x14, 0x4f, 0x90, 0x59, 
  0x00, 0x14, 0x2f, 0x90, 0x00, 0x02,  
  0x74, 0x01, 0x81, 0x49, 0x82, 0x5e, 0xa0, 0x15, 
  0x42, 0x62, 0x56, 0xa0, 0x34, 0x41, 0x61, 0x7e, 
  0xa0, 0x1c, 0x31, 0x82, 0x00, 0xa0,  
  0x55, 0x01, 0x10, 0x33, 0xf5, 0x18, 0xa0, 0x10, 
  0x61, 0xff, 0x02, 0xa1, 0x10, 0x32, 0xf5, 0x0d, 
  0xa0, 0x80, 0x52, 0xff, 0x00, 0xa1,  
  0x45, 0x01, 0x11, 0x13, 0xa1, 0x30, 0xa1, 0x31, 
  0x53, 0xae, 0x02, 0xa0, 0x10, 0x25, 0xa1, 0x3c, 
  0xa1, 0x50, 0x56, 0xae, 0x02, 0xa0,  
  0x6d, 0x01, 0x10, 0x47, 0xf7, 0x08, 0xa1, 0x10, 
  0x65, 0xfe, 0x00, 0xa2, 0x10, 0x4a, 0xf7, 0x68, 
  0xa0, 0x30, 0x68, 0xfe, 0x30, 0xa2,  
  0x65, 0x01, 0x30, 0x45, 0xd3, 0x46, 0x00, 0x10, 
  0x44, 0xe1, 0x00, 0x01, 0x50, 0x55, 0xd3, 0x46, 
  0x00, 0x10, 0x55, 0xe1, 0x00, 0xa1,  
  0x65, 0x01, 0x40, 0x6a, 0xf5, 0x1d, 0xa0, 0x10, 
  0x52, 0xc0, 0x00, 0xa0, 0x60, 0x5a, 0xf5, 0x15, 
  0xa0, 0x10, 0x52, 0xf0, 0x30, 0xa0,  
  0x65, 0x01, 0x14, 0x0c, 0x72, 0x21, 0xa1, 0x24, 
  0xcf, 0x60, 0x00, 0xa1, 0x14, 0x0c, 0x72, 0x20, 
  0xa0, 0x24, 0xcf, 0x60, 0x0c, 0xa0,  
  0x65, 0x01, 0x15, 0x3e, 0xd3, 0x14, 0x09, 0x14, 
  0x66, 0x51, 0x00, 0xa0, 0x20, 0x3f, 0xf0, 0x46, 
  0x09, 0x14, 0x65, 0x50, 0x00, 0x01,  
  0x7c, 0x01, 0x14, 0x39, 0x82, 0x4a, 0x00, 0x44, 
  0x48, 0xd0, 0x65, 0x02, 0x34, 0x5a, 0xd0, 0x45, 
  0xa0, 0x14, 0xbf, 0xa0, 0x00, 0xa0,  
  0x6d, 0x01, 0x50, 0x51, 0xfe, 0x41, 0xa0, 0x20, 
  0x73, 0xce, 0x02, 0xa0, 0x10, 0x26, 0xc5, 0x78, 
  0xa1, 0x30, 0x44, 0xfd, 0x00, 0xa8,  
  0x6d, 0x01, 0x70, 0x6c, 0xe2, 0x54, 0xa0, 0x20, 
  0x68, 0xf1, 0x00, 0xa0, 0xd0, 0x6c, 0xe2, 0x54, 
  0xa0, 0x20, 0x68, 0xf1, 0x00, 0xa0,  
  0x65, 0x01, 0x50, 0x57, 0x63, 0x76, 0xa0, 0x10, 
  0x5f, 0xd0, 0x00, 0xa0, 0x70, 0x57, 0x63, 0x46, 
  0xa0, 0x10, 0x5f, 0xd0, 0x00, 0xa0,  
  0x6d, 0x01, 0x81, 0x8a, 0xf2, 0x5a, 0xa0, 0x21, 
  0x78, 0xfe, 0x00, 0xa0, 0xa0, 0x8b, 0xf2, 0xbc, 
  0xa0, 0x20, 0x78, 0xff, 0x00, 0xa0,  
  0x65, 0x01, 0x11, 0x78, 0xa0, 0x34, 0xa0, 0x00, 
  0x3a, 0xf0, 0x00, 0xa1, 0x00, 0x72, 0xf0, 0x30, 
  0xa0, 0x00, 0x32, 0xf8, 0x00, 0xa0,  
  0x7d, 0x01, 0x10, 0x89, 0xf5, 0x4e, 0xa4, 0x00, 
  0x53, 0xfe, 0x18, 0xa0, 0x01, 0x43, 0xf5, 0x4a, 
  0xa1, 0x01, 0x44, 0xfe, 0x00, 0xa0,  
  0x7d, 0x01, 0x10, 0x5a, 0xf5, 0x10, 0xa4, 0x10, 
  0x58, 0xfe, 0x00, 0xa0, 0x40, 0x55, 0xf5, 0x62, 
  0xa1, 0x10, 0x56, 0xfe, 0x00, 0xa0,  
  0x7d, 0x01, 0xf0, 0x0f, 0x10, 0x34, 0xa0, 0x08, 
  0xbf, 0x1f, 0x01, 0x0b, 0x70, 0x0f, 0x10, 0xb3, 
  0xa7, 0x08, 0xcf, 0x1f, 0x90, 0x0b,  
  0x7d, 0x01, 0xb0, 0x48, 0xf2, 0x1e, 0xa0, 0x30, 
  0x46, 0x58, 0x0e, 0xa2, 0xb0, 0x48, 0xd2, 0x8c, 
  0xa7, 0xf0, 0x46, 0x58, 0x4c, 0xa3,  
  0x7d, 0x01, 0x30, 0x6f, 0xf0, 0x00, 0xa0, 0x30, 
  0x94, 0x50, 0x02, 0xa5, 0x00, 0xaf, 0xff, 0xa4, 
  0xa0, 0x30, 0xd8, 0x6f, 0x40, 0xa0,  
  0x3d, 0x01, 0x64, 0x1f, 0xf0, 0x00, 0xa0, 0x48, 
  0x42, 0x18, 0x00, 0x09, 0x10, 0x00, 0x0f, 0x75, 
  0xa0, 0x10, 0x40, 0x1f, 0xfc, 0xa0,  
  0x5f, 0x01, 0x88, 0x88, 0x6f, 0x00, 0xa0, 0x10, 
  0x10, 0x0f, 0xfc, 0xa0, 0x78, 0x87, 0x6f, 0x00, 
  0xa0, 0xc8, 0x88, 0x4f, 0x00, 0xa0,  
  0x6d, 0x01, 0x55, 0x13, 0xc7, 0x65, 0x02, 0x41, 
  0x24, 0xf1, 0x00, 0xa0, 0x11, 0x1e, 0xf1, 0x51, 
  0xa5, 0x74, 0x31, 0xf1, 0x03, 0x08,  
  0xfc, 0x01, 0xf8, 0x16, 0xf0, 0x00, 0xa8, 0xdd, 
  0x5f, 0x4d, 0x00, 0xa9, 0xfd, 0x1e, 0x20, 0x00, 
  0xaa, 0x0c, 0x61, 0x21, 0x00, 0xaa,  
  0x7d, 0x01, 0xf4, 0x1c, 0xf0, 0x00, 0xa0, 0x0c, 
  0x52, 0x41, 0x00, 0x0f, 0xf0, 0x10, 0x6f, 0x00, 
  0xaf, 0xf0, 0x70, 0x3f, 0x00, 0xa7,  
  0x7d, 0x01, 0x5c, 0x03, 0xff, 0x10, 0x00, 0xf8, 
  0x86, 0xfb, 0x00, 0x0e, 0xf0, 0xa6, 0xfb, 0x00, 
  0xa2, 0xf0, 0xb6, 0xff, 0x00, 0xa2
};

/* The following table matching drums that are not supported in the DSP
** the first 11 entries are for drums 24->34, the last 3 are for 82->84
*/
const uint8 audfmt_smaf_phrase_drum_4op_range_map[14]= {
  54, 38, 55, 69, 69, 58, 75, 38, 37, 36, 40, 67, 81, 81
};

const uint8 audfmt_smaf_phrase_drum_dls_lookup_4op[61][22] = {
  0x75, 0x01, 0x00, 0xa0, 0xc0, 0x00, 0xa4, 0x50, 0xf9,
  0x6b, 0x00, 0xa0, 0x00, 0xff, 0xfb, 0x00, 0xa0, 0x60,
  0x6a, 0xf8, 0x00, 0xa0,
  0x7d, 0x01, 0x50, 0x08, 0xf3, 0x00, 0xa0, 0x00, 0x6a,
  0x95, 0x02, 0xa0, 0x00, 0x00, 0x00, 0xb4, 0xa0, 0x00,
  0x84, 0x80, 0x32, 0xa0,
  0x7d, 0x01, 0xd4, 0x30, 0x6f, 0x00, 0xa0, 0x02, 0x46, 
  0x39, 0x3c, 0xa2, 0x00, 0x00, 0x00, 0x00, 0xa7, 0x02, 
  0x3a, 0x8e, 0x4c, 0xa0,  
  0x7d, 0x01, 0x40, 0x00, 0xf0, 0x00, 0xa0, 0xe0, 0x6b, 
  0x85, 0x00, 0xa0, 0x00, 0x00, 0x00, 0xb4, 0xa4, 0x00, 
  0x84, 0x80, 0x3a, 0xa0,  
  0x7d, 0x01, 0xec, 0x32, 0x6f, 0x00, 0xa2, 0x1a, 0x46, 
  0x39, 0x3c, 0xab, 0x20, 0x0a, 0x7c, 0x00, 0xa7, 0x10, 
  0x7a, 0x84, 0x38, 0xa1,  
  0x7e, 0x01, 0x00, 0x45, 0x79, 0x00, 0xa4, 0x0d, 0x02, 
  0xf7, 0x00, 0xac, 0x06, 0x78, 0xc0, 0x00, 0xa2, 0xda, 
  0xf4, 0x3b, 0x01, 0xaa,  
  0x6d, 0x01, 0x70, 0x77, 0xff, 0x10, 0xa1, 0x41, 0x88, 
  0xaf, 0x04, 0xa6, 0x60, 0x00, 0x70, 0x00, 0xa0, 0xf0, 
  0xb7, 0x78, 0x23, 0xa7,  
  0x7d, 0x01, 0x00, 0x0c, 0xf1, 0x24, 0xa2, 0x00, 0x77, 
  0xf3, 0x00, 0xa0, 0x00, 0x0c, 0xf1, 0x44, 0xa2, 0x00, 
  0x77, 0xf3, 0x00, 0xa0,  
  0x5d, 0x01, 0x20, 0xc9, 0xff, 0x01, 0xa0, 0x10, 0x8b, 
  0xfb, 0x00, 0xa0, 0x60, 0xc9, 0x5f, 0x01, 0xa0, 0x40, 
  0x8b, 0xfc, 0x00, 0xa0,  
  0x6d, 0x01, 0x11, 0xd7, 0xa1, 0x34, 0xa5, 0x01, 0x50, 
  0x70, 0x00, 0xa0, 0x10, 0xd7, 0xa1, 0x54, 0xa5, 0x00, 
  0x50, 0x70, 0x00, 0xa0,  
  0x7d, 0x01, 0xc1, 0x50, 0xf0, 0x08, 0xa2, 0xa1, 0x77, 
  0xf7, 0x0c, 0xa0, 0xc0, 0x6c, 0xf8, 0x00, 0xa0, 0x10, 
  0x77, 0xf7, 0x00, 0xa2,  
  0x6d, 0x01, 0x10, 0x7f, 0xf7, 0x18, 0xa0, 0x01, 0x63, 
  0xf0, 0x00, 0xa0, 0x30, 0x7f, 0xf7, 0x00, 0xa0, 0x00, 
  0x64, 0xf0, 0x00, 0xa0,  
  0x75, 0x01, 0xd0, 0x7d, 0xff, 0x00, 0xa5, 0x30, 0x67, 
  0xf7, 0x04, 0xa0, 0x00, 0x77, 0xf7, 0x28, 0xa5, 0x30, 
  0x86, 0xf7, 0x00, 0xa0, 
  0x55, 0x01, 0x00, 0x07, 0x8f, 0x03, 0xa1, 0x41, 0xb8, 
  0xaf, 0x00, 0xa6, 0x00, 0x09, 0x57, 0x00, 0xa0, 0x00, 
  0x68, 0x99, 0x00, 0xa7,  
  0x7d, 0x01, 0x81, 0x50, 0xf0, 0x08, 0xa2, 0x81, 0x77, 
  0xf7, 0x08, 0xa0, 0xc0, 0x6c, 0xf8, 0x00, 0xa0, 0x10, 
  0x79, 0xf7, 0x00, 0xa2,  
  0x6d, 0x01, 0xf4, 0xf0, 0xf0, 0x00, 0xa0, 0xf1, 0x63, 
  0xf1, 0x00, 0xa7, 0xf4, 0xf3, 0xf1, 0x00, 0xa0, 0xf0, 
  0x63, 0xf1, 0xc4, 0xa1,  
  0x7d, 0x01, 0xc1, 0x50, 0xf0, 0x08, 0xa2, 0xa1, 0x77, 
  0xf7, 0x0c, 0xa0, 0xc0, 0x6c, 0xf8, 0x00, 0xa0, 0x10, 
  0x77, 0xf7, 0x00, 0xa2,  
  0x65, 0x01, 0xc0, 0x6a, 0xff, 0x3c, 0xa0, 0x50, 0x53, 
  0xf7, 0x18, 0xa0, 0x10, 0x53, 0xff, 0x74, 0xa1, 0x50, 
  0x53, 0xf7, 0x00, 0xa0,  
  0x7d, 0x01, 0xf0, 0x99, 0x90, 0x00, 0xa0, 0xf1, 0x67, 
  0xf6, 0x10, 0xa4, 0x80, 0x86, 0x78, 0x00, 0xa0, 0xf0, 
  0x7a, 0x7a, 0x2c, 0xa0,  
  0x65, 0x01, 0xc0, 0x6a, 0xff, 0x3c, 0xa0, 0x60, 0x54, 
  0xf7, 0x00, 0xa0, 0x10, 0x57, 0xff, 0x54, 0xa1, 0x60, 
  0x64, 0xf7, 0x08, 0xa0,  
  0x7d, 0x01, 0xf0, 0x99, 0x90, 0x00, 0xa0, 0xf1, 0x67, 
  0xf6, 0x00, 0xa1, 0x30, 0x88, 0x88, 0x00, 0xa0, 0x10, 
  0x7a, 0x99, 0x00, 0xa0,  
  0x65, 0x01, 0xc0, 0x99, 0xff, 0x3c, 0xa0, 0x80, 0x63, 
  0xf7, 0x00, 0xa0, 0x10, 0x67, 0xff, 0x54, 0xa1, 0x80, 
  0x75, 0xf7, 0x14, 0xa0,  
  0x7d, 0x01, 0x20, 0x37, 0x80, 0x00, 0xa2, 0xa1, 0x42, 
  0xb7, 0x20, 0xa1, 0xf0, 0x85, 0x79, 0x00, 0xa7, 0x51, 
  0x55, 0x48, 0x00, 0xa3, 
  0x65, 0x01, 0xc0, 0x5a, 0xff, 0x2c, 0xa0, 0xa0, 0x64, 
  0xf7, 0x00, 0xa0, 0x10, 0x85, 0xff, 0x54, 0xa1, 0xa0, 
  0x97, 0xf7, 0x00, 0xa0,  
  0x65, 0x01, 0xc0, 0x9a, 0xff, 0x3c, 0xa0, 0xc0, 0x65, 
  0xf7, 0x00, 0xa0, 0x10, 0x67, 0xff, 0x54, 0xa1, 0xc0, 
  0x75, 0xf7, 0x14, 0xa0,  
  0x3d, 0x01, 0xe0, 0x06, 0xf0, 0x04, 0xa7, 0xf1, 0x2f, 
  0x90, 0x00, 0x06, 0xf0, 0x06, 0xf0, 0x04, 0xa7, 0xf0, 
  0x4f, 0x90, 0x00, 0xa2, 
  0x65, 0x01, 0xd0, 0x9a, 0xff, 0x3c, 0xa0, 0xf0, 0x65, 
  0xf7, 0x00, 0xa0, 0x20, 0x67, 0xff, 0x58, 0xa1, 0xf0, 
  0x75, 0xf7, 0x14, 0xa0, 
  0x6d, 0x01, 0xf0, 0x07, 0xf4, 0x02, 0xa6, 0x70, 0x44, 
  0xfe, 0x54, 0xa6, 0xf0, 0x07, 0xf4, 0x82, 0xa6, 0xb0, 
  0x44, 0xfe, 0x54, 0xa6,  
  0x7d, 0x01, 0xb0, 0x35, 0xf3, 0x00, 0xa5, 0x09, 0x20, 
  0x70, 0x28, 0x0e, 0xb0, 0x35, 0xf3, 0x00, 0xa5, 0x00, 
  0x40, 0x70, 0x44, 0xa6,  
  0x5d, 0x01, 0xe0, 0x44, 0xee, 0x28, 0xa3, 0x70, 0x55, 
  0xfe, 0x00, 0xa1, 0xe0, 0x44, 0xee, 0x68, 0xa3, 0x70, 
  0x55, 0xfe, 0x00, 0xa1,  
  0x6d, 0x01, 0xf4, 0x06, 0xf0, 0x0c, 0xa2, 0x21, 0x4a, 
  0xc4, 0x00, 0xa3, 0xf0, 0x86, 0xf8, 0x00, 0xa7, 0x21, 
  0x3b, 0x74, 0x00, 0xa2, 
  0x7d, 0x01, 0xf0, 0x36, 0x70, 0x00, 0x07, 0x91, 0x12, 
  0x96, 0x08, 0x02, 0xd0, 0x30, 0x50, 0x00, 0x07, 0x31, 
  0x56, 0x47, 0x04, 0x09, 
  0x65, 0x01, 0x01, 0x79, 0xf4, 0x02, 0xa2, 0x01, 0x53, 
  0xf0, 0x20, 0xa0, 0x11, 0x5a, 0x74, 0x00, 0xa1, 0x01, 
  0x53, 0xa0, 0x20, 0xa0, 
  0x7d, 0x01, 0xe0, 0x06, 0xf0, 0x06, 0xa0, 0xf9, 0x2f, 
  0x90, 0x00, 0x0e, 0xe0, 0x29, 0xf0, 0x06, 0xa5, 0xf0, 
  0x4f, 0x90, 0x00, 0xa2,  
  0x7d, 0x01, 0x10, 0x10, 0xa0, 0x00, 0xa4, 0x00, 0x4f, 
  0xf2, 0x14, 0xa0, 0x50, 0x86, 0x88, 0x00, 0xa4, 0xfd, 
  0x67, 0x79, 0x24, 0xa0,  
  0x6d, 0x01, 0xf0, 0x07, 0xf4, 0x02, 0xa6, 0x70, 0x44, 
  0xfe, 0x54, 0xa6, 0xf0, 0x07, 0xf4, 0x82, 0xa6, 0xb0, 
  0x44, 0xfe, 0x54, 0xa6, 
  0x5d, 0x01, 0x60, 0xc5, 0xf0, 0x00, 0xa0, 0x20, 0x85, 
  0xf0, 0x28, 0xa0, 0x60, 0xc5, 0xf0, 0x00, 0xa0, 0x20, 
  0x85, 0xf0, 0x18, 0xa0, 
  0x5d, 0x01, 0x10, 0x5a, 0xfb, 0x30, 0xa0, 0x20, 0x68, 
  0xc9, 0x00, 0xa0, 0x10, 0x57, 0xfb, 0x80, 0xa0, 0x20, 
  0x59, 0xc9, 0x00, 0xa0,  
  0x7d, 0x01, 0x70, 0xc5, 0xa0, 0x00, 0xa0, 0x10, 0x85, 
  0xf0, 0x00, 0xa0, 0x70, 0xf8, 0x88, 0x5c, 0xa0, 0x10, 
  0x98, 0x92, 0x38, 0xa4,  
  0x65, 0x01, 0x90, 0x48, 0xaf, 0x8c, 0xa3, 0xe0, 0x76, 
  0xcf, 0x08, 0xa0, 0xc0, 0x6b, 0xf8, 0x00, 0xa0, 0x10, 
  0x78, 0xf7, 0x10, 0xa2,  
  0x7d, 0x01, 0xa0, 0x5a, 0xaf, 0xa0, 0xa3, 0xd0, 0x75, 
  0xcf, 0x00, 0xa0, 0xc0, 0x6a, 0xf8, 0x00, 0xa0, 0x10, 
  0x77, 0xf7, 0x44, 0xa2,  
  0x75, 0x01, 0xf0, 0xb5, 0x80, 0x00, 0xa0, 0x60, 0x74, 
  0x80, 0x00, 0xa7, 0x70, 0xc0, 0xf0, 0x00, 0xa0, 0x00, 
  0x55, 0xf0, 0x00, 0xa0,  
  0x7d, 0x01, 0x00, 0x88, 0xf8, 0x32, 0xa1, 0x00, 0x64, 
  0xfa, 0x00, 0xa0, 0x00, 0x88, 0xf8, 0x32, 0xa1, 0x00, 
  0x64, 0xfa, 0x00, 0xa0,  
  0x6d, 0x01, 0x70, 0x6c, 0xe2, 0x54, 0xa0, 0x20, 0x68, 
  0xf1, 0x00, 0xa0, 0x70, 0x5c, 0xe2, 0x34, 0xa0, 0x20, 
  0x68, 0xf1, 0x00, 0xa0,  
  0x6d, 0x01, 0x70, 0x6c, 0xe2, 0x54, 0xa0, 0x20, 0x68, 
  0xf1, 0x00, 0xa0, 0x70, 0x5c, 0xe2, 0x34, 0xa0, 0x20, 
  0x68, 0xf1, 0x4c, 0xa0,  
  0x7c, 0x01, 0x70, 0x28, 0xf6, 0x00, 0xa0, 0xf1, 0x24, 
  0xe8, 0x00, 0xa0, 0xf0, 0x24, 0xf8, 0x00, 0xa2, 0xf0, 
  0x88, 0x77, 0x10, 0xa7, 
  0x7d, 0x01, 0xa0, 0x87, 0xe4, 0x00, 0xa1, 0x30, 0x8b, 
  0x84, 0x00, 0xa1, 0xa0, 0x86, 0xe4, 0x68, 0xa2, 0x30, 
  0x88, 0x84, 0x90, 0xa4,  
  0xfd, 0x01, 0xf7, 0x02, 0x62, 0x5c, 0x03, 0x84, 0x7c, 
  0x53, 0x00, 0x00, 0xe7, 0x02, 0x62, 0x00, 0xa3, 0xe4, 
  0x7c, 0x53, 0x84, 0xa0,  
  0xfd, 0x01, 0xb7, 0x02, 0x62, 0x5c, 0x03, 0x84, 0x7c, 
  0x53, 0x00, 0x00, 0xe7, 0x02, 0x62, 0x00, 0xa3, 0xe4, 
  0x7c, 0x53, 0x84, 0xa0,  
  0x7d, 0x01, 0x10, 0x76, 0xf0, 0x00, 0xab, 0x41, 0x87, 
  0x63, 0x00, 0xa2, 0x10, 0x76, 0xf0, 0x00, 0xa2, 0x11, 
  0x87, 0x63, 0x00, 0xa2,  
  0x7d, 0x01, 0x00, 0x41, 0xf0, 0x10, 0x02, 0xf1, 0x63, 
  0x41, 0x38, 0x04, 0x00, 0x41, 0xf0, 0x00, 0xa4, 0xf1,
  0x63, 0x41, 0x40, 0xa3,  
  0x44, 0x01, 0xf0, 0x67, 0xda, 0x00, 0xa0, 0x50, 0x67, 
  0xfa, 0x00, 0xa0, 0x61, 0x0b, 0xdc, 0x64, 0xa2, 0x51, 
  0x57, 0xfa, 0x00, 0xa0,  
  0x44, 0x01, 0xf0, 0x66, 0xea, 0x00, 0xa0, 0x60, 0x66, 
  0xfa, 0x00, 0xa0, 0x81, 0x0b, 0xec, 0x00, 0xa2, 0x31, 
  0x67, 0xaf, 0x10, 0xa0, 
  0x44, 0x01, 0xf0, 0x66, 0xea, 0x00, 0xa0, 0x60, 0x67, 
  0xfa, 0x00, 0xa0, 0x81, 0x0b, 0xec, 0x00, 0xa2, 0x21, 
  0x67, 0xaf, 0x10, 0xa0,  
  0x45, 0x01, 0x10, 0x75, 0x5e, 0x6a, 0xa0, 0x20, 0x55, 
  0x60, 0x00, 0xa0, 0x00, 0x80, 0x40, 0x00, 0xa0, 0x00, 
  0x70, 0xa0, 0x00, 0xa0,  
  0x45, 0x01, 0x10, 0x88, 0xf4, 0x25, 0xa0, 0x20, 0x50, 
  0x90, 0x1c, 0xa0, 0x00, 0x80, 0x40, 0x00, 0xa0, 0x00, 
  0x70, 0xa0, 0x00, 0xa0,  
  0x7f, 0x01, 0xa0, 0x80, 0xf0, 0xfc, 0xa6, 0x00, 0x80, 
  0xf0, 0xfc, 0xa4, 0xf0, 0x99, 0xff, 0x28, 0xa0, 0xf0, 
  0x99, 0xaf, 0x24, 0xa6,  
  0x7f, 0x01, 0xc0, 0x65, 0xf0, 0x68, 0xa6, 0x00, 0x80, 
  0xf0, 0xfc, 0xa4, 0xf0, 0x55, 0xff, 0x00, 0xa0, 0xf0, 
  0x55, 0xaf, 0x64, 0xa6,  
  0x7d, 0x01, 0xf0, 0x62, 0xf5, 0x00, 0xa4, 0xf0, 0x88, 
  0x96, 0x00, 0xa7, 0xf0, 0x62, 0xf5, 0x00, 0xa4, 0xf0, 
  0x88, 0x96, 0x00, 0xa7,  
  0x45, 0x01, 0xf0, 0x06, 0x60, 0x00, 0xa2, 0x20, 0x5b, 
  0xc4, 0x00, 0xa3, 0x4f, 0x20, 0x30, 0x70, 0xa8, 0x46, 
  0x85, 0x7f, 0x00, 0xa2,  
  0x5d, 0x01, 0xe0, 0x54, 0x3e, 0x00, 0xa3, 0x98, 0x37, 
  0x40, 0x01, 0xa1, 0xea, 0x54, 0x3e, 0x00, 0xab, 0x5f, 
  0x37, 0x4f, 0x06, 0xa2
};

/* The following table matching drums that are not supported in the DSP
** the first 11 entries are for drums 24->34, the last 3 are for 82->84
*/
const uint8 audfmt_l1_ma3_2op_drum_range_map[14]= {
  75, 55, 69, 69, 38, 38, 80, 38, 80, 50, 38, 70, 81, 81
};

const uint8 audfmt_l1_ma3_2op_drum_dls_lookup[61][15] = {
  0x61, 0x90, 0xf0, 0xf0, 0x08, 0x24, 0x26, 0x82,
  0x90, 0xfa, 0xf1, 0x00, 0x42, 0x22, 0x00, 
  0x60, 0x50, 0x96, 0xd0, 0x00, 0x44, 0x00, 0xc7,
  0x80, 0x98, 0x82, 0x10, 0x24, 0x00, 0x00, 
  0x20, 0x00, 0x32, 0xa0, 0x00, 0x20, 0x15, 0xc7,
  0x00, 0x75, 0x63, 0x28, 0x14, 0x00, 0x03, 
  0x60, 0x30, 0x4a, 0xd0, 0x00, 0x04, 0x06, 0x87,
  0x80, 0x68, 0xd2, 0x08, 0x44, 0x02, 0x00, 
  0x60, 0x00, 0x58, 0xc2, 0x00, 0x02, 0x40, 0xc7,
  0x00, 0x79, 0xb4, 0x04, 0x24, 0x26, 0x19, 
  0x20, 0x00, 0x20, 0x70, 0x34, 0x24, 0x83, 0x17,
  0x00, 0x95, 0xf3, 0x00, 0x03, 0x06, 0xd0, 
  0x60, 0x50, 0x97, 0xff, 0x0c, 0x44, 0x70, 0x0e,
  0x51, 0xf8, 0xaf, 0x00, 0x44, 0x50, 0x30, 
  0x60, 0x10, 0x12, 0xb9, 0x00, 0x44, 0x10, 0xd6,
  0x70, 0x77, 0xa1, 0x0c, 0x44, 0x10, 0x00, 
  0x60, 0x90, 0xc9, 0xff, 0x01, 0x44, 0x40, 0x4b,
  0x90, 0x7a, 0xfb, 0x00, 0x44, 0x20, 0xb0, 
  0x60, 0x40, 0x42, 0xb0, 0x04, 0x44, 0x00, 0x20,
  0x81, 0x82, 0xe0, 0x00, 0x44, 0x00, 0x00, 
  0xe0, 0x30, 0xb3, 0xf0, 0x30, 0x47, 0x20, 0xd7,
  0x80, 0x97, 0xf3, 0x00, 0x44, 0x00, 0x30, 
  0x60, 0x21, 0x27, 0xc5, 0x00, 0x44, 0x00, 0x09,
  0x50, 0x77, 0xff, 0x00, 0x44, 0x00, 0x40, 
  0x60, 0x40, 0x46, 0xd9, 0x00, 0x44, 0x01, 0x29,
  0x70, 0x75, 0xf0, 0x00, 0x44, 0x04, 0x00, 
  0x60, 0x00, 0x07, 0xff, 0x00, 0x44, 0x70, 0x0e,
  0xf1, 0xf8, 0xaf, 0x00, 0x44, 0x20, 0x30, 
  0x60, 0x10, 0x12, 0xf0, 0x00, 0x44, 0x00, 0x87,
  0x60, 0x77, 0xd5, 0x00, 0x44, 0x32, 0x00, 
  0x60, 0x00, 0xf0, 0xf0, 0x00, 0x44, 0xf0, 0x05,
  0x61, 0x63, 0x61, 0x00, 0x44, 0xf0, 0x38, 
  0x60, 0x40, 0x05, 0xf9, 0x00, 0x44, 0x20, 0x07,
  0x70, 0x7f, 0xf0, 0x00, 0x44, 0x10, 0x30, 
  0x60, 0x40, 0x49, 0xc4, 0x02, 0x44, 0x01, 0x01,
  0x40, 0x45, 0xf1, 0x00, 0x44, 0x00, 0x20, 
  0xe0, 0x20, 0x22, 0xf0, 0x01, 0x45, 0x50, 0x17,
  0x80, 0x89, 0xd3, 0x14, 0x44, 0xf3, 0x00, 
  0x60, 0x40, 0x49, 0xc4, 0x02, 0x44, 0x01, 0x01,
  0x40, 0x45, 0xf1, 0x00, 0x44, 0x00, 0x20, 
  0x60, 0x00, 0x03, 0xc1, 0x14, 0x44, 0xb0, 0x27,
  0x61, 0x67, 0x96, 0x10, 0x44, 0x00, 0x00, 
  0x60, 0x50, 0x5a, 0xd4, 0x02, 0x44, 0x01, 0x82,
  0x40, 0x44, 0xd1, 0x00, 0x44, 0x00, 0x20, 
  0x60, 0x00, 0x36, 0xf1, 0x00, 0x44, 0xe0, 0x27,
  0x40, 0x75, 0x72, 0x00, 0x45, 0xd0, 0xd0, 
  0x60, 0x60, 0x6b, 0xd4, 0x02, 0x44, 0x03, 0x01,
  0x50, 0x66, 0xd0, 0x00, 0x44, 0x00, 0x20, 
  0x60, 0x60, 0x6b, 0xd4, 0x02, 0x44, 0x03, 0x01,
  0x50, 0x66, 0xd0, 0x00, 0x44, 0x00, 0x20, 
  0x20, 0x01, 0x06, 0xc0, 0x00, 0x44, 0xe0, 0x87,
  0x40, 0x45, 0xb2, 0x10, 0x00, 0xf0, 0x00, 
  0x60, 0x50, 0x58, 0xc6, 0x00, 0x44, 0x00, 0x0a,
  0x50, 0x56, 0xc2, 0x00, 0x44, 0x10, 0x80, 
  0x60, 0x60, 0x00, 0xbb, 0xa0, 0x44, 0x90, 0x25,
  0x40, 0x4a, 0xe3, 0x00, 0x44, 0x40, 0x68, 
  0xa0, 0x30, 0x43, 0xe7, 0x00, 0x20, 0xa4, 0x9f,
  0x50, 0x57, 0x92, 0x00, 0x03, 0x02, 0x00, 
  0xa0, 0x20, 0x5c, 0xf4, 0x00, 0x46, 0x63, 0x38,
  0x60, 0x57, 0xf2, 0x00, 0x44, 0x76, 0x80, 
  0x60, 0x40, 0x17, 0x82, 0x4c, 0x44, 0xa0, 0x66,
  0x70, 0x67, 0xe5, 0x00, 0x44, 0xf0, 0x48, 
  0x20, 0x01, 0x86, 0xd0, 0x00, 0x44, 0xc4, 0x3f,
  0x60, 0x68, 0x92, 0x14, 0x00, 0xf0, 0x30, 
  0x60, 0x71, 0x79, 0xf4, 0x00, 0x44, 0x10, 0x1f,
  0x51, 0x53, 0xf0, 0x08, 0x44, 0x00, 0x00, 
  0x20, 0x01, 0x12, 0xd0, 0x28, 0x44, 0xf4, 0x06,
  0x40, 0x48, 0xc2, 0x10, 0x00, 0x90, 0x30, 
  0x60, 0x10, 0x10, 0xf0, 0x08, 0x44, 0x10, 0x2f,
  0x41, 0x95, 0xf2, 0x18, 0x44, 0x10, 0x00, 
  0x60, 0x60, 0x00, 0xbb, 0xa0, 0x44, 0x90, 0x67,
  0x40, 0x4a, 0xe3, 0x00, 0x44, 0x40, 0x68, 
  0x60, 0xc0, 0xc5, 0xf0, 0x00, 0x44, 0x60, 0x03,
  0x80, 0x75, 0xf0, 0x00, 0x44, 0xb0, 0x00, 
  0x60, 0xc0, 0xc5, 0xf0, 0x00, 0x44, 0x60, 0x03,
  0x80, 0x75, 0xf0, 0x00, 0x44, 0xb0, 0x00, 
  0xe0, 0xa0, 0xae, 0xe1, 0x18, 0x46, 0x02, 0x04,
  0x90, 0xa8, 0xf1, 0x00, 0x44, 0x10, 0x40, 
  0x60, 0x50, 0x8a, 0xac, 0x58, 0x44, 0xa0, 0x1f,
  0x70, 0x96, 0xc4, 0x00, 0x44, 0xc0, 0x00, 
  0x60, 0x41, 0xf9, 0xf4, 0x1c, 0x44, 0x10, 0x00,
  0x50, 0x36, 0xfb, 0x00, 0x44, 0x00, 0x00, 
  0xe0, 0x60, 0xea, 0xf6, 0x38, 0x33, 0x83, 0x36,
  0x50, 0x56, 0xf2, 0x00, 0x15, 0x16, 0x60, 
  0xa0, 0x60, 0xea, 0xf6, 0x24, 0x23, 0x63, 0x34,
  0x50, 0x56, 0xf2, 0x04, 0x25, 0x16, 0x20, 
  0x60, 0x60, 0x6c, 0xe2, 0x4c, 0x44, 0x77, 0x05,
  0x60, 0x68, 0xf1, 0x10, 0x44, 0x23, 0x00, 
  0x60, 0x60, 0x6c, 0xe2, 0x4c, 0x44, 0x73, 0x07,
  0x60, 0x68, 0xf1, 0x24, 0x44, 0x27, 0x00, 
  0x60, 0x60, 0xf5, 0xa4, 0x3c, 0x44, 0xf0, 0xa7,
  0xa0, 0xb7, 0x80, 0x00, 0x44, 0xf0, 0x18, 
  0x60, 0x60, 0xf5, 0xa4, 0x3c, 0x44, 0xf0, 0xa7,
  0x90, 0xb7, 0x80, 0x00, 0x44, 0xf0, 0x18, 
  0xe0, 0x02, 0x70, 0xf0, 0x5c, 0x47, 0x00, 0x02,
  0x02, 0xc1, 0x80, 0x24, 0x20, 0xf0, 0x00, 
  0xe0, 0x02, 0x70, 0xf0, 0x5c, 0x47, 0x00, 0x02,
  0x02, 0xc1, 0x80, 0x18, 0x20, 0xf0, 0x00, 
  0x60, 0x00, 0xb9, 0x80, 0x4c, 0x00, 0x00, 0x17,
  0x81, 0x87, 0x84, 0x00, 0x00, 0xf0, 0x90, 
  0x60, 0x00, 0xb9, 0x80, 0x4c, 0x00, 0x00, 0x17,
  0x81, 0x84, 0x84, 0x00, 0x00, 0xf0, 0x90, 
  0x61, 0xa0, 0xfc, 0xff, 0x58, 0x44, 0x60, 0x00,
  0x81, 0x87, 0xff, 0x00, 0x44, 0x50, 0x00, 
  0x60, 0x91, 0x9a, 0xf2, 0x4a, 0x40, 0x82, 0x06,
  0x81, 0x87, 0xff, 0x00, 0x40, 0x21, 0x00, 
  0x60, 0x91, 0x9a, 0xf2, 0x4a, 0x40, 0x82, 0x06,
  0x81, 0x87, 0xff, 0x00, 0x40, 0x21, 0x00, 
  0xe1, 0xe0, 0xe6, 0x6d, 0x3c, 0x45, 0x17, 0x05,
  0xe0, 0xe6, 0x74, 0x0c, 0x54, 0x13, 0x00, 
  0xe0, 0xf0, 0x88, 0x9f, 0x30, 0x00, 0x00, 0x81,
  0x80, 0x86, 0x74, 0x00, 0x00, 0xe0, 0x40, 
  0x60, 0x70, 0x77, 0xf1, 0x32, 0x42, 0x54, 0x00,
  0x90, 0x97, 0xdf, 0x00, 0x22, 0xa1, 0x00, 
  0x60, 0x00, 0x0f, 0xf0, 0x32, 0x42, 0x54, 0x00,
  0x50, 0x55, 0xcf, 0x0c, 0x24, 0xa1, 0x00, 
  0x60, 0x30, 0x13, 0xb2, 0x02, 0x42, 0xa2, 0x07,
  0x90, 0x6d, 0x74, 0x18, 0x44, 0xf4, 0x00, 
  0xe1, 0x60, 0x65, 0xc2, 0x02, 0x07, 0xf4, 0xd4,
  0x50, 0xdb, 0xb1, 0x00, 0x31, 0xf2, 0x98, 
  0x60, 0x50, 0x54, 0x3e, 0x00, 0x44, 0xf0, 0x1b,
  0x40, 0x37, 0x40, 0x1A, 0x44, 0x90, 0x08
};

/* The following table matching drums that are not supported in the DSP
** the first 11 entries are for drums 24->34, the last 3 are for 82->84
*/
const uint8 audfmt_l1_ma3_4op_drum_range_map[14]= {
  75, 40, 38, 40, 69, 38, 80, 38, 80, 41, 48, 70, 81, 57
};

const uint8 audfmt_l1_ma3_4op_drum_dls_lookup[61][29] = {
  0x65, 0xf0, 0xf0, 0xf0, 0xe0, 0x44, 0x60, 0x26,
  0xf1, 0xf8, 0xfc, 0x00, 0x44, 0x50, 0x00, 0xf0,
  0xfb, 0xfb, 0x9c, 0x44, 0xa0, 0x30, 0xf1, 0xfb,
  0xef, 0x04, 0x44, 0x50, 0x00, 
  0x65, 0x00, 0x08, 0xf3, 0x00, 0x44, 0x50, 0x07,
  0x80, 0xb8, 0x9b, 0x02, 0x44, 0x00, 0x00, 0x60,
  0xe8, 0xf3, 0x90, 0x44, 0xc0, 0x30, 0x80, 0x8d,
  0xc0, 0xb2, 0x44, 0x10, 0x30, 
  0x65, 0x00, 0x65, 0xf0, 0x00, 0x42, 0x00, 0x97,
  0x02, 0xa8, 0x96, 0x00, 0x66, 0x00, 0x19, 0x00,
  0xf0, 0xff, 0x00, 0x00, 0x00, 0x87, 0x30, 0xf6,
  0x36, 0x54, 0x64, 0x10, 0x00, 
  0x65, 0x00, 0x00, 0xf0, 0x00, 0x44, 0x40, 0x07,
  0x60, 0xc8, 0xc9, 0x00, 0x44, 0xe0, 0x68, 0x40,
  0x7a, 0xbd, 0x24, 0x44, 0x00, 0x38, 0xb0, 0xb8,
  0x8d, 0x02, 0x44, 0x00, 0x00, 
  0x65, 0x00, 0x35, 0xf0, 0x00, 0x42, 0x55, 0x97,
  0x02, 0xa6, 0x96, 0x00, 0x66, 0x30, 0x19, 0x00,
  0xf0, 0xff, 0x00, 0x00, 0x90, 0x87, 0x30, 0xf6,
  0x36, 0x40, 0x64, 0x30, 0xa0, 
  0x25, 0x00, 0x20, 0x70, 0x34, 0x24, 0x83, 0x17,
  0x00, 0x95, 0xf3, 0x00, 0x03, 0x06, 0xd0, 0x80,
  0x4a, 0xf3, 0x20, 0x44, 0x30, 0x00, 0x70, 0x76,
  0xe5, 0x00, 0x44, 0x00, 0x00, 
  0x65, 0x50, 0x97, 0xff, 0x08, 0x44, 0x70, 0x0e,
  0x51, 0xf8, 0xaf, 0x00, 0x44, 0x50, 0x30, 0x60,
  0x55, 0xf0, 0x9c, 0x44, 0x20, 0x28, 0x90, 0x9a,
  0xca, 0x50, 0x44, 0x50, 0x00, 
  0x65, 0x00, 0x0c, 0xf1, 0x24, 0x44, 0x00, 0xc7,
  0x70, 0x77, 0xf3, 0x00, 0x44, 0x00, 0x00, 0x70,
  0x5b, 0xef, 0xc0, 0x44, 0x70, 0x06, 0x60, 0x0a,
  0xf0, 0x70, 0x44, 0x00, 0x00, 
  0x66, 0xc0, 0x89, 0xf9, 0x01, 0x44, 0xf0, 0xa7,
  0x20, 0x8a, 0xdb, 0x00, 0x44, 0xa0, 0x18, 0x20,
  0x58, 0xd5, 0x55, 0x44, 0xb0, 0x18, 0x90, 0x7b,
  0xcb, 0x00, 0x44, 0xd0, 0x40, 
  0x65, 0xb0, 0x6a, 0xff, 0x00, 0x44, 0x20, 0x21,
  0x60, 0x56, 0xf7, 0x40, 0x44, 0x20, 0x00, 0x70,
  0x49, 0xb7, 0x34, 0x44, 0x50, 0x08, 0x80, 0x57,
  0xf7, 0x00, 0x44, 0x10, 0x60, 
  0x65, 0x51, 0x50, 0xf0, 0x14, 0x44, 0xc0, 0x17,
  0x71, 0x77, 0xf7, 0x00, 0x44, 0xb0, 0x00, 0x60,
  0x6a, 0xf8, 0x00, 0x44, 0xc0, 0x00, 0x70, 0x77,
  0xf7, 0x00, 0x44, 0x70, 0x10, 
  0x65, 0x70, 0x7d, 0xff, 0x00, 0x44, 0xd0, 0x36,
  0x70, 0x47, 0xf7, 0x00, 0x44, 0x20, 0x00, 0x70,
  0x69, 0xf7, 0x4c, 0x44, 0x30, 0x50, 0xa0, 0x68,
  0xfb, 0x00, 0x44, 0x20, 0x60, 
  0x65, 0x70, 0x6d, 0xff, 0x38, 0x44, 0xd0, 0x36,
  0x70, 0x77, 0xf7, 0x30, 0x44, 0x20, 0x00, 0x70,
  0x49, 0xf7, 0x24, 0x44, 0x20, 0x10, 0x80, 0x67,
  0xf7, 0x00, 0x44, 0x10, 0x60, 
  0x65, 0x00, 0x82, 0xf5, 0x20, 0x44, 0xa0, 0x0e,
  0x31, 0x39, 0xdf, 0x00, 0x44, 0x70, 0x30, 0x00,
  0x90, 0xbd, 0x00, 0x44, 0x90, 0x97, 0xb0, 0x99,
  0xdb, 0x00, 0x44, 0x00, 0x90, 
  0x65, 0x51, 0x70, 0xf0, 0x1c, 0x44, 0xc0, 0x6f,
  0xa1, 0x97, 0xf7, 0x00, 0x44, 0x80, 0x00, 0x70,
  0x85, 0xf6, 0x00, 0x44, 0xc0, 0x06, 0x70, 0x67,
  0xf7, 0x04, 0x44, 0x70, 0x10, 
  0x65, 0x40, 0x64, 0xf0, 0x08, 0x44, 0x00, 0x07,
  0x90, 0x46, 0xf1, 0x00, 0x44, 0x00, 0x28, 0x00,
  0xf8, 0xf1, 0x02, 0x44, 0xd1, 0xda, 0x90, 0x8a,
  0xf5, 0x02, 0x44, 0xf0, 0x10, 
  0x65, 0x00, 0x71, 0xf0, 0x28, 0x44, 0xc0, 0x17,
  0xb0, 0x77, 0xfd, 0x00, 0x00, 0xb0, 0x00, 0x60,
  0x5a, 0xf8, 0x24, 0x44, 0x90, 0xe7, 0xa0, 0x68,
  0xf1, 0x00, 0x44, 0x70, 0x90, 
  0x65, 0x60, 0x6a, 0xdf, 0x24, 0x44, 0xc0, 0x34,
  0x70, 0xba, 0xf7, 0x00, 0x44, 0x50, 0x00, 0x50,
  0x53, 0xff, 0x54, 0x44, 0x00, 0x08, 0x50, 0x66,
  0xf7, 0x00, 0x44, 0x50, 0x00, 
  0x65, 0x00, 0x00, 0xf3, 0x08, 0x44, 0xf0, 0x07,
  0xc0, 0x88, 0xbb, 0x02, 0x44, 0x00, 0x00, 0x30,
  0xd3, 0xf3, 0x30, 0x44, 0x00, 0x77, 0x80, 0xba,
  0xb0, 0x02, 0x44, 0xd0, 0x58, 
  0x65, 0x60, 0x65, 0xcf, 0x00, 0x44, 0xc0, 0x30,
  0x70, 0xb9, 0xf7, 0x00, 0x44, 0xb0, 0x98, 0x40,
  0x54, 0xff, 0x68, 0x44, 0x00, 0x08, 0x40, 0x66,
  0xf5, 0x00, 0x44, 0x50, 0x00, 
  0x65, 0x00, 0x00, 0xf3, 0x00, 0x44, 0xc0, 0x07,
  0x80, 0x88, 0x7b, 0x3e, 0x44, 0x00, 0x00, 0x50,
  0xe3, 0xf3, 0x20, 0x44, 0x60, 0xa7, 0x80, 0x83,
  0x80, 0x02, 0x44, 0x50, 0x58, 
  0x65, 0x60, 0x65, 0xcf, 0x04, 0x44, 0xa0, 0x30,
  0x70, 0x98, 0xf7, 0x00, 0x44, 0x10, 0x90, 0x50,
  0x53, 0xff, 0x44, 0x44, 0x00, 0x08, 0x50, 0x56,
  0xf7, 0x00, 0x44, 0x50, 0x00, 
  0x65, 0x00, 0x00, 0xf3, 0x08, 0x44, 0xf0, 0x07,
  0x60, 0x87, 0xbb, 0x06, 0x44, 0x00, 0x00, 0x30,
  0xd3, 0xf3, 0x30, 0x44, 0x00, 0x77, 0x50, 0xb9,
  0xb0, 0x26, 0x44, 0xd0, 0x58, 
  0x65, 0x60, 0x65, 0xcf, 0x00, 0x44, 0xa0, 0x30,
  0x70, 0xa9, 0xf6, 0x00, 0x44, 0x80, 0x90, 0x20,
  0x54, 0xff, 0x70, 0x44, 0x00, 0x00, 0x50, 0x76,
  0xf7, 0x08, 0x44, 0x40, 0x00, 
  0x65, 0x60, 0x54, 0xcf, 0x24, 0x44, 0xa0, 0x30,
  0x70, 0x79, 0xf7, 0x00, 0x44, 0x10, 0x98, 0x50,
  0x54, 0xff, 0x9c, 0x44, 0x10, 0x08, 0x60, 0x76,
  0xf7, 0x0c, 0x44, 0x40, 0x00, 
  0x25, 0x00, 0x69, 0xf0, 0x38, 0x44, 0xd0, 0x83,
  0x40, 0x53, 0x92, 0x0c, 0x00, 0xb0, 0x90, 0x00,
  0x43, 0xb0, 0x00, 0x44, 0xf0, 0x07, 0x50, 0x54,
  0xc7, 0x34, 0x44, 0xf0, 0x30, 
  0x65, 0x60, 0x74, 0xcf, 0x10, 0x44, 0x80, 0x30,
  0x70, 0x8a, 0xf7, 0x00, 0x44, 0x60, 0x98, 0x50,
  0x65, 0xff, 0xc8, 0x44, 0x10, 0x28, 0x50, 0x66,
  0xc7, 0x00, 0x44, 0x50, 0x00, 
  0x25, 0x00, 0x07, 0xf4, 0x26, 0x44, 0xf0, 0x07,
  0x40, 0x44, 0xee, 0x0c, 0x44, 0x90, 0x70, 0x00,
  0x00, 0xf4, 0x22, 0x44, 0xe0, 0x07, 0x20, 0x54,
  0xfd, 0x40, 0x44, 0xe0, 0x58, 
  0x65, 0x70, 0x23, 0xf1, 0x7c, 0x44, 0x00, 0x07,
  0x51, 0x23, 0xe0, 0x00, 0x11, 0x50, 0xf0, 0x80,
  0x31, 0xf6, 0x3c, 0x44, 0x20, 0x07, 0x50, 0x32,
  0x90, 0x24, 0x44, 0x00, 0x30, 
  0x25, 0x00, 0x07, 0xf4, 0x5a, 0x44, 0xf0, 0x9f,
  0x40, 0x45, 0xfe, 0x00, 0x44, 0xf0, 0x60, 0x00,
  0x00, 0xf4, 0x32, 0x44, 0xc0, 0x9f, 0x20, 0x55,
  0xfd, 0x6c, 0x44, 0xf0, 0x58, 
  0x65, 0x40, 0x17, 0x82, 0x28, 0x44, 0xb0, 0x06,
  0xc0, 0xb7, 0xe5, 0x00, 0x44, 0x50, 0x68, 0x50,
  0x27, 0x82, 0x00, 0x44, 0xb0, 0x40, 0xc0, 0x77,
  0xd6, 0x00, 0x44, 0xf0, 0x40, 
  0x65, 0x30, 0x36, 0xc0, 0x00, 0x00, 0x80, 0xcf,
  0x41, 0x33, 0x9c, 0x00, 0x00, 0xd0, 0x18, 0x30,
  0x30, 0x50, 0x04, 0x00, 0x50, 0x6e, 0x51, 0x83,
  0xc6, 0x00, 0x10, 0xf0, 0x98, 
  0x65, 0x81, 0x59, 0xf3, 0x02, 0x44, 0x30, 0x04,
  0x51, 0x37, 0xa3, 0x00, 0x44, 0x00, 0x00, 0xb1,
  0x9f, 0xfa, 0x0c, 0x44, 0x40, 0x10, 0x51, 0x5c,
  0xf3, 0x80, 0x44, 0x10, 0x00, 
  0x65, 0x00, 0x06, 0xf0, 0x02, 0x44, 0x90, 0x07,
  0x21, 0x24, 0x90, 0x00, 0x11, 0x00, 0x00, 0x00,
  0x03, 0xf0, 0x02, 0x44, 0xf0, 0x07, 0x30, 0x46,
  0x93, 0x20, 0x44, 0x74, 0x10, 
  0x25, 0x20, 0x61, 0xd0, 0x00, 0x24, 0x12, 0xef,
  0x50, 0x96, 0xf2, 0x00, 0x02, 0x06, 0x40, 0x40,
  0x45, 0xf0, 0x74, 0x44, 0x60, 0xef, 0x50, 0x46,
  0xf0, 0x00, 0x44, 0x00, 0xf0, 
  0x65, 0x00, 0x60, 0xf0, 0x14, 0x66, 0xe0, 0x3f,
  0x50, 0x45, 0xeb, 0x40, 0x45, 0xf0, 0x70, 0x00,
  0x00, 0xf4, 0x1e, 0x44, 0x90, 0x37, 0x30, 0x55,
  0xfe, 0x00, 0x44, 0x90, 0x58, 
  0x65, 0xc0, 0xc5, 0xf0, 0x00, 0x44, 0x60, 0x03,
  0x80, 0x75, 0xf0, 0x00, 0x44, 0xb0, 0x00, 0xc0,
  0xc5, 0xf0, 0x08, 0x44, 0xf0, 0x00, 0x70, 0x34,
  0xf0, 0x00, 0x44, 0xf0, 0x00, 
  0x65, 0xc0, 0xc5, 0xf0, 0x00, 0x44, 0x60, 0x03,
  0x80, 0x75, 0xf0, 0x1c, 0x44, 0xb0, 0x00, 0xc0,
  0xc5, 0xf0, 0x18, 0x44, 0xf0, 0x00, 0x80, 0x35,
  0xf0, 0x00, 0x44, 0xf0, 0x00, 
  0xe5, 0xa0, 0xae, 0xe1, 0x18, 0x46, 0x02, 0x04,
  0x90, 0xa8, 0xf1, 0x00, 0x44, 0x10, 0x40, 0x00,
  0x00, 0xf0, 0xb0, 0x44, 0x00, 0x00, 0x90, 0xad,
  0xf1, 0x1c, 0x44, 0x00, 0x00, 
  0x65, 0x50, 0x8a, 0xac, 0x98, 0x44, 0xa0, 0x1f,
  0x70, 0x96, 0xc4, 0x00, 0x44, 0xc0, 0x00, 0x60,
  0x8a, 0xf8, 0x00, 0x44, 0xc0, 0x00, 0x70, 0x97,
  0xf7, 0x00, 0x44, 0x20, 0x10, 
  0x65, 0x50, 0x8a, 0xac, 0x7c, 0x44, 0xa0, 0x1f,
  0x60, 0x95, 0xc4, 0x00, 0x44, 0xc0, 0x00, 0x60,
  0x8a, 0xf8, 0x00, 0x44, 0xc0, 0x00, 0x70, 0x97,
  0xf7, 0x00, 0x44, 0x20, 0x50, 
  0x63, 0x00, 0x87, 0xc0, 0x58, 0x44, 0x20, 0x06,
  0x60, 0x89, 0xf0, 0xc4, 0x44, 0x70, 0x00, 0xa0,
  0x66, 0xd1, 0x68, 0x44, 0x90, 0x20, 0x60, 0x89,
  0xf0, 0x18, 0x44, 0x90, 0x30, 
  0x63, 0x00, 0x87, 0xc0, 0x58, 0x44, 0x20, 0x06,
  0x60, 0x89, 0xf0, 0xc4, 0x44, 0x70, 0x00, 0xa0,
  0x66, 0xd1, 0x5c, 0x44, 0xc0, 0x20, 0x60, 0x89,
  0xf0, 0x10, 0x44, 0x90, 0x30, 
  0x65, 0x60, 0x6c, 0xe2, 0x54, 0x44, 0x70, 0x05,
  0x60, 0x68, 0xf1, 0x28, 0x44, 0x20, 0x00, 0x50,
  0x5c, 0xe2, 0x34, 0x44, 0x70, 0x00, 0x60, 0x68,
  0xf1, 0x74, 0x44, 0x20, 0x00, 
  0x65, 0x60, 0x6c, 0xe2, 0x54, 0x44, 0x70, 0x05,
  0x60, 0x68, 0xf1, 0x28, 0x44, 0x20, 0x00, 0x50,
  0x5c, 0xe2, 0x34, 0x44, 0x70, 0x00, 0x60, 0x68,
  0xf1, 0x74, 0x44, 0x20, 0x00, 
  0x63, 0x30, 0x50, 0xf6, 0x00, 0x44, 0x70, 0x07,
  0x41, 0x48, 0xea, 0x60, 0x44, 0xf0, 0x00, 0x20,
  0x64, 0xe4, 0x00, 0x44, 0xf0, 0x00, 0x90, 0x4b,
  0x80, 0x08, 0x44, 0xf0, 0x30, 
  0x64, 0x30, 0x50, 0xf6, 0x10, 0x44, 0xf0, 0x07,
  0x41, 0x48, 0x86, 0x0c, 0x44, 0xf0, 0x00, 0x30,
  0x39, 0xcf, 0x00, 0x44, 0xf0, 0x30, 0xa0, 0x3a,
  0x78, 0x00, 0x44, 0xf0, 0x60, 
  0xe5, 0x00, 0x70, 0xf0, 0x28, 0x20, 0x00, 0x0d,
  0x02, 0xc1, 0xc1, 0xac, 0x21, 0xe0, 0x00, 0x02,
  0x70, 0xf0, 0x5c, 0x47, 0x00, 0x02, 0x02, 0xc1,
  0x80, 0x18, 0x20, 0xf0, 0x00, 
  0xe5, 0x00, 0x70, 0xf0, 0x28, 0x20, 0x00, 0x0d,
  0x02, 0xc1, 0xc1, 0xac, 0x21, 0xe0, 0x00, 0x02,
  0x70, 0xf0, 0x5c, 0x47, 0x00, 0x02, 0x02, 0xc1,
  0x80, 0x18, 0x20, 0xf0, 0x00, 
  0x63, 0x60, 0x02, 0xc0, 0x30, 0x00, 0x10, 0x0f,
  0x60, 0xa3, 0xc0, 0x00, 0x00, 0x30, 0x20, 0x00,
  0x61, 0xd0, 0x80, 0x00, 0xe5, 0x5f, 0xc1, 0xf7,
  0xf6, 0x00, 0x00, 0x01, 0x38, 
  0x63, 0x60, 0x02, 0xc0, 0x38, 0x00, 0x10, 0x0f,
  0x60, 0xa3, 0xc0, 0x00, 0x00, 0x30, 0x20, 0x00,
  0x60, 0xf0, 0x80, 0x00, 0xd0, 0x5f, 0xe0, 0xc6,
  0x96, 0x14, 0x00, 0x02, 0x38, 
  0x64, 0x60, 0x67, 0xda, 0x2c, 0x44, 0x70, 0x00,
  0xb0, 0x3e, 0xfa, 0x34, 0x44, 0x70, 0x00, 0x81,
  0x0b, 0xfc, 0x00, 0x44, 0x00, 0x00, 0x81, 0x83,
  0xf0, 0x00, 0x44, 0xf0, 0x40, 
  0x64, 0x60, 0x66, 0xea, 0xfc, 0x44, 0xf0, 0x00,
  0x60, 0x56, 0xfb, 0x00, 0x44, 0x60, 0x00, 0x01,
  0xac, 0xdc, 0x00, 0x44, 0x70, 0x10, 0xc1, 0x77,
  0xf4, 0x00, 0x44, 0x30, 0x40, 
  0x64, 0x60, 0x66, 0xea, 0xfc, 0x44, 0xf0, 0x00,
  0x60, 0x56, 0xfb, 0x00, 0x44, 0x60, 0x00, 0x01,
  0xaa, 0xfc, 0x40, 0x44, 0xa0, 0x10, 0xc1, 0xc7,
  0xc4, 0x00, 0x44, 0x30, 0x80, 
  0x65, 0xc0, 0xda, 0xdb, 0x3e, 0x44, 0x00, 0x00,
  0x60, 0x50, 0x60, 0x18, 0x44, 0x60, 0x00, 0x80,
  0xc8, 0xf0, 0x38, 0x44, 0x00, 0x00, 0x70, 0x70,
  0x60, 0x08, 0x44, 0x60, 0x00, 
  0xe5, 0xf0, 0x88, 0x9f, 0x30, 0x00, 0x00, 0x81,
  0x80, 0x86, 0x74, 0x00, 0x00, 0xe0, 0x40, 0x00,
  0x07, 0xf0, 0x90, 0x44, 0x20, 0x00, 0xb0, 0xb7,
  0xdc, 0x48, 0x44, 0xb0, 0x00, 
  0x67, 0x80, 0x4c, 0xf0, 0x14, 0x44, 0x90, 0xb0,
  0x90, 0x55, 0xb6, 0x00, 0x44, 0xf0, 0x20, 0xc0,
  0x7b, 0xf3, 0x1c, 0x44, 0xe0, 0x30, 0x80, 0xbd,
  0xef, 0x18, 0x44, 0x60, 0x00, 
  0x67, 0x50, 0x45, 0xf0, 0x0c, 0x44, 0x90, 0xb0,
  0x90, 0x55, 0xb6, 0x00, 0x44, 0xf0, 0x20, 0x90,
  0x79, 0xf3, 0x20, 0x44, 0xe0, 0x30, 0x50, 0xb5,
  0xef, 0x0c, 0x44, 0x60, 0x00, 
  0x64, 0x30, 0x50, 0x96, 0x00, 0x44, 0x70, 0x07,
  0x41, 0x46, 0x96, 0x10, 0x44, 0x30, 0x00, 0x20,
  0x64, 0xe4, 0x04, 0x44, 0xc0, 0x00, 0x90, 0x4a,
  0x98, 0x08, 0x44, 0xf0, 0x30, 
  0x65, 0x00, 0x06, 0x70, 0x50, 0x44, 0x60, 0x10,
  0x50, 0x5b, 0x64, 0x00, 0x44, 0x20, 0x18, 0x03,
  0x20, 0x30, 0x1c, 0x54, 0x60, 0x10, 0x22, 0x65,
  0x7f, 0x00, 0x44, 0x90, 0x10, 
  0x65, 0x20, 0x04, 0xc0, 0x60, 0x46, 0x30, 0x9f,
  0x50, 0x53, 0x44, 0x00, 0x66, 0x20, 0x10, 0x03,
  0x20, 0x30, 0x1c, 0x77, 0x70, 0x10, 0x22, 0x64,
  0x56, 0x08, 0x66, 0x30, 0x68
};
#endif /* FEATURE_SMAF */

/* Download able sound
*/
audfmt_dls_struct_type audfmt_dls_struct;
#endif /* FEATURE_WEBAUDIO */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_DLS)
const audfmt_prog_def_in_use_type audfmt_prog_def_seq[] = {
  AUDFMT_PROG_DEF_RESERVED_SEQ1,
  AUDFMT_PROG_DEF_RESERVED_SEQ2,
  AUDFMT_PROG_DEF_RESERVED_SEQ3,
  AUDFMT_PROG_DEF_RESERVED_SEQ4,
  AUDFMT_PROG_DEF_RESERVED_ALL
};

const audfmt_prog_def_in_use_type audfmt_prog_def_in_use_seq[] = {
  AUDFMT_PROG_DEF_IN_USE_SEQ1,
  AUDFMT_PROG_DEF_IN_USE_SEQ2,
  AUDFMT_PROG_DEF_IN_USE_SEQ3,
  AUDFMT_PROG_DEF_IN_USE_SEQ4,
  AUDFMT_PROG_DEF_IN_USE_ALL
};
#endif /* FEATURE_WEBAUDIO || FEATURE_DLS */

#ifdef FEATURE_DLS
/***********************************************************************
**
** The following defines and structures are used in calculating various
** parameter settings for DLS.
**
************************************************************************/


#ifdef FEATURE_AUDIO_CMX_5P0
/* This structure definition is used as the default settings for DLS
** program definitions.
*/
const audfmt_hq_prog_def_struct_type audfmt_prog_def_default = {
  0x0000, 0x007F, NULL,   0xFCACAE9B, 0xE0DB5F61, 0xFCACAE9B, 0xE0DB5F61,
  0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x03E80000, 0x80000000,
  0xE39953DC, 0x00000000, 0x00000000, 0x00000000, 0x80000000, 0x80000000,
  0x80000000, 0x80000000, 0x03E80000, 0x80000000, 0x00000000, 0x00000000,
  0x00000000, 0x32000000, 0x19000000, 0x7FFFFFFF, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0xFC400000, 0xFC400000, 0xFC400000, 0x00000000,
  0x32000000, 0x32000000, 0x00640000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x01F40000,
  0x03E80000, 0x00000000, 0x03E80000, 0x00000000
};

#else

#define Q14SHIFT 14
#define Q15SHIFT 15
#define Q16SHIFT 16
#define Q32SHIFT 32

#define ONE_BY_63_Q20 16644
#define ONEQ20        1048576

#define LOG2_440Q14         143873
#define HALFQ14             8192
#define LOG2_ONE_BY_TEN_Q14 -54426
#define CONST_L32_Q15       544428

#define FRAMESPERSEC 400
#define HALFQ9       256
#define Q9SHIFT      9

#define NOOFTERMS 6

#define ONE_BY_190_Q18 1378
#define ONE_OVER_ONE_TWENTY_Q16 546 

/*
** values unrelated with sampling frequency
*/
#define CENTTABLE_MIN  (-100)
#define CENTTABLE_MAX  (100)
#define CENTTABLE_MID  (100)
#define CENTTABLE_RESO (1)

/*
** pow(2.0, -100/1200.0) -- pow(2.0, 100/1200.0)
*/
static const sint31 table_cent2ratioL30[201] = {
  0x3c686fce, 0x3c715f36, 0x3c7a4ff1, 0x3c8341ff, 
  0x3c8c355f, 0x3c952a12, 0x3c9e2018, 0x3ca71772, 
  0x3cb0101f, 0x3cb90a1f, 0x3cc20574, 0x3ccb021d, 
  0x3cd4001a, 0x3cdcff6b, 0x3ce60012, 0x3cef020d, 
  0x3cf8055d, 0x3d010a03, 0x3d0a0ffe, 0x3d13174e, 
  0x3d1c1ff5, 0x3d2529f2, 0x3d2e3544, 0x3d3741ee, 
  0x3d404fee, 0x3d495f45, 0x3d526ff3, 0x3d5b81f8, 
  0x3d649555, 0x3d6daa09, 0x3d76c015, 0x3d7fd77a, 
  0x3d88f036, 0x3d920a4b, 0x3d9b25b9, 0x3da4427f, 
  0x3dad609f, 0x3db68017, 0x3dbfa0ea, 0x3dc8c315, 
  0x3dd1e69b, 0x3ddb0b7b, 0x3de431b5, 0x3ded5949, 
  0x3df68239, 0x3dffac83, 0x3e08d828, 0x3e120528, 
  0x3e1b3383, 0x3e24633b, 0x3e2d944e, 0x3e36c6bd, 
  0x3e3ffa89, 0x3e492fb1, 0x3e526635, 0x3e5b9e17, 
  0x3e64d756, 0x3e6e11f1, 0x3e774deb, 0x3e808b42, 
  0x3e89c9f6, 0x3e930a09, 0x3e9c4b7a, 0x3ea58e4a, 
  0x3eaed278, 0x3eb81805, 0x3ec15ef2, 0x3ecaa73d, 
  0x3ed3f0e8, 0x3edd3bf3, 0x3ee6885e, 0x3eefd628, 
  0x3ef92553, 0x3f0275df, 0x3f0bc7cb, 0x3f151b18, 
  0x3f1e6fc7, 0x3f27c5d6, 0x3f311d47, 0x3f3a761a, 
  0x3f43d04f, 0x3f4d2be6, 0x3f5688df, 0x3f5fe73b, 
  0x3f6946f9, 0x3f72a81b, 0x3f7c0a9f, 0x3f856e87, 
  0x3f8ed3d3, 0x3f983a82, 0x3fa1a296, 0x3fab0c0d, 
  0x3fb476e9, 0x3fbde32a, 0x3fc750cf, 0x3fd0bfd9, 
  0x3fda3049, 0x3fe3a21e, 0x3fed1559, 0x3ff689f9, 
  0x40000000, 0x4009776c, 0x4012f03f, 0x401c6a79, 
  0x4025e61a, 0x402f6322, 0x4038e191, 0x40426168, 
  0x404be2a6, 0x4055654c, 0x405ee95b, 0x40686ed1, 
  0x4071f5b1, 0x407b7df8, 0x408507a9, 0x408e92c3, 
  0x40981f47, 0x40a1ad34, 0x40ab3c8b, 0x40b4cd4c, 
  0x40be5f77, 0x40c7f30c, 0x40d1880c, 0x40db1e77, 
  0x40e4b64d, 0x40ee4f8e, 0x40f7ea3b, 0x41018653, 
  0x410b23d7, 0x4114c2c7, 0x411e6324, 0x412804ed, 
  0x4131a823, 0x413b4cc6, 0x4144f2d6, 0x414e9a53, 
  0x4158433e, 0x4161ed96, 0x416b995d, 0x41754692, 
  0x417ef535, 0x4188a547, 0x419256c7, 0x419c09b7, 
  0x41a5be16, 0x41af73e5, 0x41b92b23, 0x41c2e3d1, 
  0x41cc9def, 0x41d6597d, 0x41e0167d, 0x41e9d4ec, 
  0x41f394cd, 0x41fd561f, 0x420718e2, 0x4210dd17, 
  0x421aa2be, 0x422469d7, 0x422e3262, 0x4237fc5f, 
  0x4241c7cf, 0x424b94b2, 0x42556308, 0x425f32d2, 
  0x4269040f, 0x4272d6bf, 0x427caae4, 0x4286807d, 
  0x4290578a, 0x429a300c, 0x42a40a02, 0x42ade56e, 
  0x42b7c24f, 0x42c1a0a5, 0x42cb8071, 0x42d561b3, 
  0x42df446b, 0x42e9289a, 0x42f30e3f, 0x42fcf55b, 
  0x4306dded, 0x4310c7f7, 0x431ab378, 0x4324a071, 
  0x432e8ee2, 0x43387ecb, 0x4342702c, 0x434c6306, 
  0x43565758, 0x43604d23, 0x436a4468, 0x43743d25, 
  0x437e375d, 0x4388330e, 0x43923039, 0x439c2ede, 
  0x43a62efe, 0x43b03099, 0x43ba33ae, 0x43c4383f, 
  0x43ce3e4b
};

/* This structure definition is used as the default settings for DLS
** program definitions.
*/
const audfmt_hq_prog_def_struct_type audfmt_prog_def_default = {
  0x7f, 0x00, 0x00, 0x00, 0x6c, 0x00, 0x50, 0x20, 0x5e, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x5e, 0x00, 0x00, 0x00, 0x02, 0x00, 0x7f, 0x01, 0x3c, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xc1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x7f
};
#endif /* FEATURE_AUDIO_CMX_5P0 */

static const sint31 exp2TableQ14[12]= {
  16384, 17358, 18390, 19484, 20643, 21870,
  23170, 24548, 26008, 27554, 29193, 30929
};

/* These structures are used to parse through a DLS file. Only one 
** active DLS load function is allowed per sequence.
*/
#define               AUDFMT_DLS_NUM_STATE_STRUCT  5
audfmt_dls_state_type audfmt_dls_state[AUDFMT_DLS_NUM_STATE_STRUCT];

/* This table contains parsed DLS information formatted in proper structures.
*/
audfmt_hq_dls_struct_type audfmt_dls_table;

/* The following scratch buffer is used in DLS processing
*/
uint8 audfmt_dls_scratch[sizeof(audfmt_hq_prog_def_struct_type)];
#endif /* FEATURE_DLS */

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

audmain_audfmt_poly_mode_type audfmt_poly_mode = AUDMAIN_AUDFMT_POLY_MODE_0;

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_enable_mfi_processing

DESCRIPTION
  This function enables MFi processing and must be called to enable MFi
  parser code every time an MFi file is to be processed.
  This function also sets the 4-byte MFi extension ID to be used. Please
  note that this ID can only be set once, the mfi_id parameter will be
  ignored after the first call to this function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_enable_mfi_processing (
  uint8 *mfi_id
) {
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
} /* end audfmt_enable_mfi_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of the current song.

DEPENDENCIES
  None

RETURN VALUE
  sample rate of the file

SIDE EFFECTS
  None

===========================================================================*/
snd_sample_rate_type audfmt_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
  /*lint -esym(715,parse_ctl) Tell Lint we know parse_ctl parameter
  ** is not used
  */
) {
  /* Sampling rate for MIDI-based playback is fixed according to Synth
  ** capability
  */
#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_GASYNTH_INTERFACE
#ifdef FEATURE_AUDIO_QSYNTH_48K_SUPPORT
  return(SND_SAMPLE_RATE_48000);
#else 
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  return(SND_SAMPLE_RATE_44000);
#else
  return(SND_SAMPLE_RATE_44100);
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
#endif /* FEATURE_AUDIO_QSYNTH_48K_SUPPORT */
#else
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  snd_sample_rate_type ret_val = SND_SAMPLE_RATE_44000;
#else
  snd_sample_rate_type ret_val = SND_SAMPLE_RATE_44100;
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
  audfmt_parse_type    *midi_ctl;
  uint32               sequence;

  if ((parse_ctl != NULL) && (parse_ctl->tracks != NULL)) {
    midi_ctl = (audfmt_parse_type *) parse_ctl->tracks;
    sequence = AUDFMT_NUM_SEQUENCES;
    while(sequence != 0) {
      if(midi_ctl->midi_status & AUDFMT_MASK_ENABLED) {
        if((midi_ctl->hybrid_mode == CMID_POLY_MAX_HP) || 
           (midi_ctl->midi_status & AUDFMT_MASK_MULTISEQ_MODE)) {
          /* Only can get 72 voice with 32kHz sample rate */
          ret_val = SND_SAMPLE_RATE_32000;
        }
        break;
      }
      sequence--;
      midi_ctl++;
    }
  }
  return(ret_val);
#endif /* FEATURE_GASYNTH_INTERFACE */
#else
  return(SND_SAMPLE_RATE_32000);
#endif /* FEATURE_WEBAUDIO */
} /* end audfmt_get_sample_rate */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION cmid_vib_activate

DESCRIPTION
  This function will activate any pending VIBE commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmid_vib_activate (
  audfmt_parse_type *midi_ctl
) {
  if(midi_ctl->vib_ctl != NULL) {
    if(midi_ctl->midi_status2 & AUDFMT_MASK_VIB_EN_PENDING) {
      midi_ctl->vib_ctl->vib_ctl = 
                               audvib_get_pattern(midi_ctl->vib_ctl->vib_pend);
      midi_ctl->vib_ctl->index   = 0;
      midi_ctl->vib_ctl->time_ms = midi_ctl->vib_ctl->vib_ctl->time_ms;
    } else if(midi_ctl->midi_status2 & AUDFMT_MASK_VIB_DIS_PENDING) {
      midi_ctl->vib_ctl->vib_ctl = NULL;
      midi_ctl->vib_ctl->index   = 0;
      midi_ctl->vib_ctl->time_ms = 0;
    }
    midi_ctl->midi_status2 &= ~(AUDFMT_MASK_VIB_EN_PENDING |
                                AUDFMT_MASK_VIB_DIS_PENDING);
  }
} /* end cmid_vib_activate */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_cb_store_reset

DESCRIPTION
  This function resets the parameters in an audfmt_cb_store_type structure
  to initial values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_cb_store_reset (
  audfmt_cb_store_type *cb_store
) {
  if(cb_store != NULL) {
    cb_store->used             = FALSE;
    cb_store->pclear           = SND_CMX_AF_INVALID;
    cb_store->pnew             = SND_CMX_AF_INVALID;
    cb_store->pdisable         = SND_CMX_AF_INVALID;
    cb_store->text_new.buf_rtn = NULL;
    cb_store->text_app_num     = 0;
    cb_store->tclear           = SND_CMX_AF_INVALID;
    cb_store->tdisable         = SND_CMX_AF_INVALID;
    cb_store->trew             = 0;
    cb_store->tadv             = 0;
    cb_store->led_cmd          = SND_CMX_AF_INVALID;
    cb_store->saf_clear        = SND_CMX_AF_INVALID;
    cb_store->saf_disable      = SND_CMX_AF_INVALID;
    cb_store->saf_cmd          = SND_CMX_AF_INVALID;
    cb_store->svg_cmd          = SND_CMX_AF_INVALID;

    memset((uint8 *)(&cb_store->pic_store), 0, 
           sizeof(audfmt_ext_store_type) * CMID_EXT_MAX_BUFFER);
    memset((uint8 *)(&cb_store->saf_store), 0, 
           sizeof(audfmt_ext_store_type) * CMID_EXT_MAX_BUFFER);

#ifdef FEATURE_SMAF
    cb_store->smaf_cb_num      = 0;
#endif /* FEATURE_SMAF */

  }
} /* end audfmt_cb_store_reset */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_do_wa_stored_cb

DESCRIPTION
  This function calls the stored callbacks specific to WebAudio.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_do_wa_stored_cb (
  audfmt_parse_type *midi_ctl
) {
  audfmt_cb_store_type *cb_store = midi_ctl->cb_store;
  snd_af_access_type   *handle   = midi_ctl->handle;
  uint32               counter;
#ifdef FEATURE_SMAF
  audfmt_smaf_cb_store_type *smaf_cb;
#endif /* FEATURE_SMAF */

  snd_cmx_af_pic_speed_enum_type af_speed = SND_CMX_AF_PIC_SPEED_NORMAL;
  snd_cmx_af_buf_rtn_type        buf_rtn = {NULL, 0, NULL, 0, 0};

  if((handle != NULL) && (handle->data_func != NULL)) {
    if(cb_store != NULL) {
      if(cb_store->tdisable != SND_CMX_AF_INVALID) {
        handle->data_func(cb_store->tdisable, handle->audfmt_ptr, 0, NULL);
        cb_store->tdisable = SND_CMX_AF_INVALID;
      }
      if(cb_store->pdisable != SND_CMX_AF_INVALID) {
        handle->data_func(cb_store->pdisable, handle->audfmt_ptr, 0, &af_speed);
        cb_store->pdisable = SND_CMX_AF_INVALID;
      }
      if(cb_store->tclear == SND_CMX_AF_TEXT_CLEAR) {
        handle->data_func(SND_CMX_AF_TEXT_CLEAR, handle->audfmt_ptr, 0, NULL);
        cb_store->tclear = SND_CMX_AF_INVALID;
      }

      if(cb_store->text_new.buf_rtn != NULL) {
        handle->data_func(SND_CMX_AF_TEXT_NEW, handle->audfmt_ptr,
                          cb_store->text_new_num, &cb_store->text_new);
        cb_store->text_new.buf_rtn = NULL;
      }
      for(counter = 0; counter < cb_store->text_app_num; counter++) {
        if(cb_store->text_app[counter].af_text.buf_rtn == NULL) {
          buf_rtn.remain_bytes = cb_store->text_app[counter].length;
          cb_store->text_app[counter].af_text.buf_rtn = &buf_rtn;
        }
        handle->data_func(SND_CMX_AF_TEXT_APPEND, handle->audfmt_ptr,
                          cb_store->text_app[counter].num, 
                          &cb_store->text_app[counter].af_text);
      }
      cb_store->text_app_num = 0;

      if(cb_store->trew > cb_store->tadv) {
        handle->data_func(SND_CMX_AF_TEXT_BACK, handle->audfmt_ptr,
                          cb_store->trew - cb_store->tadv, NULL);
      } else if(cb_store->trew < cb_store->tadv) {
        handle->data_func(SND_CMX_AF_TEXT_ADVANCE, handle->audfmt_ptr,
                          cb_store->tadv - cb_store->trew, NULL);
      }
      cb_store->trew = 0;
      cb_store->tadv = 0;

      if(cb_store->pclear == SND_CMX_AF_PIC_CLEAR) {
        handle->data_func(SND_CMX_AF_PIC_CLEAR, handle->audfmt_ptr, 0, 
                          &af_speed);
      } 
      cb_store->pclear = SND_CMX_AF_INVALID;

      if(cb_store->pnew == SND_CMX_AF_PIC_NEW) {
        handle->data_func(SND_CMX_AF_PIC_NEW, handle->audfmt_ptr,
                          cb_store->pic_num, &(cb_store->pic_status));
      }
      cb_store->pnew   = SND_CMX_AF_INVALID;

      if(cb_store->saf_disable != SND_CMX_AF_INVALID) {
        handle->data_func(cb_store->saf_disable, handle->audfmt_ptr, 0, NULL);
        cb_store->saf_disable = SND_CMX_AF_INVALID;
      }

      if(cb_store->saf_clear == SND_CMX_AF_ANIM_CLEAR) {
        handle->data_func(SND_CMX_AF_ANIM_CLEAR, handle->audfmt_ptr, 0, NULL);
      } 
      cb_store->saf_clear = SND_CMX_AF_INVALID;

      if(cb_store->saf_cmd != SND_CMX_AF_INVALID) {
        if(cb_store->saf_cmd == SND_CMX_AF_SAF) {
          midi_ctl->midi_status |= AUDFMT_MASK_SAF_OBJECTS_DONE;
        }
        handle->data_func(cb_store->saf_cmd, handle->audfmt_ptr,
                          cb_store->saf_num, &(cb_store->saf_status));
      }
      cb_store->saf_cmd = SND_CMX_AF_INVALID;

      if(cb_store->svg_cmd != SND_CMX_AF_INVALID) {
        handle->data_func(cb_store->svg_cmd, handle->audfmt_ptr,
                          cb_store->svg_num, &(cb_store->svg_status));
      }
      cb_store->svg_cmd = SND_CMX_AF_INVALID;

      if(cb_store->led_cmd == SND_CMX_AF_LED_CONTROL) {
        handle->data_func(SND_CMX_AF_LED_CONTROL, handle->audfmt_ptr, 0, 
                          &cb_store->led_ctl);
        cb_store->led_cmd = SND_CMX_AF_INVALID;
      }

#ifdef FEATURE_SMAF
      for (counter = 0; counter < cb_store->smaf_cb_num; counter++) {
        smaf_cb = &cb_store->smaf_cb[counter];
        handle->data_func(smaf_cb->smaf_cmd, handle->audfmt_ptr, 0, 
                          &smaf_cb->smaf_ctl);
      }
      cb_store->smaf_cb_num = 0;
#endif /* FEATURE_SMAF */
    }
  } else {
    audfmt_cb_store_reset(cb_store);
  }
} /* end audfmt_do_wa_stored_cb */

/* <EJECT> */
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_activate_adec_channels

DESCRIPTION
  This function performs audio decoder related functions.

DEPENDENCIES
  None

RETURN VALUE
  AUDFMT_STATUS_SUCCESS
  AUDFMT_STATUS_FAILURE

SIDE EFFECTS
  audfmt_adec_mask

===========================================================================*/
static audfmt_status_type audfmt_activate_adec_channels (
  audfmt_parse_type *midi_ctl
) {
  audfmt_status_type      ret_val = AUDFMT_STATUS_SUCCESS;

  audfmt_adec_status_type *adec_status;
  uint32                  counter;
  boolean                 activate_chan;

  adec_status = midi_ctl->adec_status;
  for(counter = 0; counter < AUDFMT_NUM_ADEC_CHANNELS; counter++) {
    if(adec_status->reset) {
      adec_status->reset   = FALSE;
      adec_status->enable  = FALSE;
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
      audfmt_adec_mask    &= ~(1 << counter);
      audfadec_wa_reset(counter, FALSE);
    } else if(adec_status->enable) {
      /* activate channel unless an error occurs in setting pending vol or pan */
      activate_chan = TRUE;
      if(adec_status->volume) {
        if(audmain_audfmt_adec_chan_vol(counter,
                          midi_ctl->wav_chan_vol[adec_status->channel]) ==
                          AUDMAIN_STATUS_SUCCESS) {
          adec_status->volume = FALSE;
        } else {
          /* DSP not ready to accept chan vol cmd so wait to activate the channel */
          ret_val = AUDFMT_STATUS_FAILURE;
          activate_chan = FALSE;
        }
      }
      if(adec_status->panning) {
        if(audmain_audfmt_adec_chan_pan(counter,
                          midi_ctl->wav_chan_pan[adec_status->channel]) ==
                          AUDMAIN_STATUS_SUCCESS) {
          adec_status->panning = FALSE;
        } else {
          /* DSP not ready to accept chan pan cmd so wait to activate the channel */
          ret_val = AUDFMT_STATUS_FAILURE;
          activate_chan = FALSE;
        }
      }
      if (activate_chan)
	  {
        /* activate adec chan only after chan vol and pan are both set */
        audfadec_wa_activate(counter);
      }
	}
    adec_status++;
  }
  return(ret_val);
} /* end audfmt_activate_adec_channels */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_do_stored_cb

DESCRIPTION
  This function calls the stored callbacks.

DEPENDENCIES
  None

RETURN VALUE
  AUDFMT_STATUS_SUCCESS
  AUDFMT_STATUS_FAILURE

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_do_stored_cb (
  audfmt_parse_type *midi_ctl
) {
  audfmt_status_type      ret_val = AUDFMT_STATUS_SUCCESS;

  if((midi_ctl->handle != NULL) && (midi_ctl->handle->data_func != NULL)) {
    switch(midi_ctl->type) {
      case AUDFMT_MIDI_CMID_TYPE:
      case AUDFMT_MIDI_MELO_TYPE:
        if(!(midi_ctl->midi_status & (AUDFMT_MASK_SEEK_ACTIVE |
                                      AUDFMT_MASK_RESUME_PENDING))) {
          audfmt_do_wa_stored_cb(midi_ctl);

          /* Start adec playback */
          if (audfmt_activate_adec_channels(midi_ctl) != AUDFMT_STATUS_SUCCESS) {
			  ret_val = AUDFMT_STATUS_FAILURE;
		  }

          /* Start VIBE commands */
          if(midi_ctl->midi_status2 & (AUDFMT_MASK_VIB_EN_PENDING |
                                       AUDFMT_MASK_VIB_DIS_PENDING)) {
            cmid_vib_activate(midi_ctl);
          }
        }
        break;

#ifdef FEATURE_SMAF
      case AUDFMT_MIDI_SMAF_TYPE:
        if(!(midi_ctl->midi_status & (AUDFMT_MASK_SEEK_ACTIVE |
                                      AUDFMT_MASK_RESUME_PENDING))) {
          audfmt_do_wa_stored_cb(midi_ctl);

		  /* Start adec playback */
		  if (audfmt_activate_adec_channels(midi_ctl) != AUDFMT_STATUS_SUCCESS) {
			  ret_val = AUDFMT_STATUS_FAILURE;
		  }
        }
        break;
#endif /* FEATURE_SMAF */
    }
  } else {
    audfmt_cb_store_reset(midi_ctl->cb_store);
  }
  return(ret_val);
} /* end audfmt_do_stored_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_buffer_cb

DESCRIPTION
  This function buffers the callback for later use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_buffer_cb (
  audfmt_parse_type     *midi_ctl,
  snd_af_data_cmd_enum  command,
  uint32                num,
  void                  *data
) {
  audfmt_status_type     ret_val = AUDFMT_STATUS_SUCCESS;
  audfmt_text_store_type *text_store;
  snd_cmx_af_text_type   *text_type;
  audfmt_cb_store_type   *cb_store = midi_ctl->cb_store;
#ifdef FEATURE_SMAF
  audfmt_smaf_cb_store_type *smaf_cb;
#endif /* FEATURE_SMAF */

  if(cb_store != NULL) {
    switch (command) {
      case SND_CMX_AF_TEXT_CLEAR:
        cb_store->tclear           = SND_CMX_AF_TEXT_CLEAR;
        cb_store->trew             = 0;
        cb_store->tadv             = 0;
        cb_store->text_new.buf_rtn = NULL;
        cb_store->text_app_num     = 0;
        break;

      case SND_CMX_AF_TEXT_NEW:
        text_type                     = (snd_cmx_af_text_type *) data;
        cb_store->text_new.xpos_type  = text_type->xpos_type;
        cb_store->text_new.ypos_type  = text_type->ypos_type;
        cb_store->text_new_num        = num;
        cb_store->text_new_buf        = *(text_type->buf_rtn);

        cb_store->text_new.buf_rtn = &(cb_store->text_new_buf);
        cb_store->text_app_num     = 0;
        cb_store->tclear           = SND_CMX_AF_INVALID;
        cb_store->trew             = 0;
        cb_store->tadv             = 0;
        break;

      case SND_CMX_AF_TEXT_APPEND:
        if(cb_store->text_app_num < AUDFMT_TEXT_STORE_NUM) {
          text_type  = (snd_cmx_af_text_type *) data;
          text_store = &cb_store->text_app[cb_store->text_app_num];
  
          text_store->af_text.xpos_type  = text_type->xpos_type;
          text_store->af_text.ypos_type  = text_type->ypos_type;
          text_store->num                = num;
          cb_store->text_app_buf         = *(text_type->buf_rtn);
          text_store->af_text.buf_rtn    = &(cb_store->text_app_buf);
  
          cb_store->text_app_num++;
        } else {
          ret_val = AUDFMT_STATUS_STORE_ERR;
        }
        break;
  
      case SND_CMX_AF_TEXT_BACK:
        cb_store->trew += num;
        break;
  
      case SND_CMX_AF_TEXT_ADVANCE:
        cb_store->tadv += num;
        break;
  
      case SND_CMX_AF_TEXT_DISABLE:
      case SND_CMX_AF_TEXT_ENABLE:
        cb_store->tdisable = command;
        break;
  
      case SND_CMX_AF_PIC_NEW:
        cb_store->pnew       = SND_CMX_AF_PIC_NEW;
        cb_store->pic_status = *((snd_cmx_af_pic_new_type *) data);
        cb_store->pic_num    = num;
        cb_store->pic_buf    = *(cb_store->pic_status.buf_rtn);
  
        cb_store->pic_status.buf_rtn = &(cb_store->pic_buf);
        break;
  
      case SND_CMX_AF_PIC_CLEAR:
        cb_store->pclear = SND_CMX_AF_PIC_CLEAR;
        cb_store->pnew   = SND_CMX_AF_INVALID;
        break;
  
      case SND_CMX_AF_PIC_ENABLE:
      case SND_CMX_AF_PIC_DISABLE:
        cb_store->pdisable = command;
        break;
  
      case SND_CMX_AF_ANIM_CLEAR:
        cb_store->saf_clear = SND_CMX_AF_ANIM_CLEAR;
        if((cb_store->saf_cmd == SND_CMX_AF_SAF_FRAME_ID) ||
           (cb_store->saf_cmd == SND_CMX_AF_SAF_FRAME)) {
          cb_store->saf_cmd = SND_CMX_AF_INVALID;
        }
        cb_store->svg_cmd = SND_CMX_AF_INVALID;
        break;
  
      case SND_CMX_AF_ANIM_ENABLE:
      case SND_CMX_AF_ANIM_DISABLE:
        cb_store->saf_disable = command;
        break;
  
      case SND_CMX_AF_SAF:
        cb_store->saf_cmd    = SND_CMX_AF_SAF;
        cb_store->saf_status = *((snd_cmx_af_saf_type *) data);
        cb_store->saf_num    = num;
        cb_store->saf_buf    = *(cb_store->saf_status.buf_rtn);
  
        cb_store->saf_status.buf_rtn = &(cb_store->saf_buf);
        break;
  
      case SND_CMX_AF_SAF_FRAME:
        if(cb_store->saf_cmd != SND_CMX_AF_SAF) {
          cb_store->saf_cmd    = SND_CMX_AF_SAF_FRAME;
          cb_store->saf_status = *((snd_cmx_af_saf_type *) data);
          cb_store->saf_num    = num;
          cb_store->saf_buf    = *(cb_store->saf_status.buf_rtn);
          cb_store->saf_status.buf_rtn = &(cb_store->saf_buf);
        }
        break;
  
      case SND_CMX_AF_SAF_FRAME_ID:
        if(cb_store->saf_cmd != SND_CMX_AF_SAF) {
          cb_store->saf_cmd    = SND_CMX_AF_SAF_FRAME_ID;
          cb_store->saf_status = *((snd_cmx_af_saf_type *) data);
          cb_store->saf_num    = num;
        }
        break;

      case SND_CMX_AF_SVG_FRAME_ID:
        cb_store->svg_cmd    = SND_CMX_AF_SVG_FRAME_ID;
        cb_store->svg_status = *((snd_cmx_af_svg_frame_type *) data);
        cb_store->svg_num    = num;
        break;

      case SND_CMX_AF_LED_CONTROL:
        cb_store->led_cmd = command;
        cb_store->led_ctl = *((snd_cmx_af_led_ctl_type *) data);
        break;

#if defined(FEATURE_SMAF)
      case SND_CMX_SMAF_LED_CONTROL:
      case SND_CMX_SMAF_VIB_CONTROL:
        if(cb_store->smaf_cb_num < AUDFMT_SMAF_CB_STORE_NUM) {
          smaf_cb = &cb_store->smaf_cb[cb_store->smaf_cb_num];
          smaf_cb->smaf_cmd = command;
          smaf_cb->smaf_ctl = *((snd_cmx_smaf_led_vib_ctl_type *) data);
          cb_store->smaf_cb_num++;
        } else {
          ret_val = AUDFMT_STATUS_STORE_ERR;
        }
        break;
#endif

      default:
        if((midi_ctl->handle != NULL) && (midi_ctl->handle->data_func != NULL))
        {
          midi_ctl->handle->data_func(command, midi_ctl->handle->audfmt_ptr,
                                      num, data);
        }
        break;
    }
  }

  return(ret_val);
} /* end audfmt_buffer_cb */
#endif /* FEATURE_WEBAUDIO */
  
/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_do_play_cb

DESCRIPTION
  This function performs a playback callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_do_play_cb (
  snd_af_access_type    *handle,
  snd_af_data_cmd_enum  command,
  uint32                num,
  void                  *data
) {
  if((handle != NULL) && (handle->data_func != NULL)) {
    handle->data_func(command, handle->audfmt_ptr, num, data);
  }
} /* end audfmt_do_play_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_play_cb

DESCRIPTION
  This function performs a playback callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_play_cb (
  audmain_parse_ctl_type *parse_ctl,
  snd_af_data_cmd_enum   command,
  uint32                 num,
  void                   *data
) {
  audfmt_parse_type *midi_ctl;
  uint32            sequence;

  switch(command) {
    case SND_CMX_AF_TIME_TICK:
      if(parse_ctl != NULL) {
        midi_ctl = (audfmt_parse_type *) parse_ctl->tracks;
        sequence = AUDFMT_NUM_SEQUENCES;
        while(sequence != 0) {
          if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
             !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                        AUDFMT_MASK_GET_SPEC))) {
            audfmt_do_play_cb(midi_ctl->handle, command, num, data);
          }
          sequence--;
          midi_ctl++;
        }
      }
      break;

    default:
      break;
  }
} /* end audfmt_play_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_restore_track

DESCRIPTION
  This function rewinds the data pointer in the track by the number of
  bytes indicated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_restore_track (
  audfmt_track_type *track,
  uint32            num
) {
  if(track->index >= num) {
    track->index            -= num;
    track->current_location -= num;
    return(AUDFMT_STATUS_SUCCESS);
  }
  return(AUDFMT_STATUS_ERROR);
} /* end audfmt_restore_track */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_peek_next_track_byte

DESCRIPTION
  This function peeks at the next track byte.

DEPENDENCIES
  None

RETURN VALUE
  Next data byte, if available.

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_peek_next_track_byte (
  audfmt_track_type  *track,
  uint8              *next_byte
) {
  if(track->index < track->buf_size) {
    *next_byte = track->track_buf[track->index];
  } else if((track->index < (track->buf_size + track->buf_reserve_size)) &&
            (track->track_buf_reserve != NULL)) {
    *next_byte = track->track_buf_reserve[track->index - track->buf_size];
  } else if(track->current_location >= track->track_end) {
    return(AUDFMT_STATUS_ERROR);
  } else {
    /* Data is not available, return buffer error */
    return(AUDFMT_STATUS_BUFF_ERR);
  }

  return(AUDFMT_STATUS_SUCCESS);
} /* end audfmt_peek_next_track_byte */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_next_track_byte

DESCRIPTION
  This function gets the next byte from the track.

DEPENDENCIES
  None

RETURN VALUE
  Next data byte, if available.

SIDE EFFECTS
  Data position in track will be updated.
  counter will be incremented if get was successful.

===========================================================================*/
static audfmt_status_type audfmt_get_next_track_byte (
  audfmt_track_type  *track,
  uint8              *next_byte,
  uint32             *counter
) {

  if(track->index < track->buf_size) {
    *next_byte = track->track_buf[track->index++];
    track->current_location++;
  } else if((track->index < (track->buf_size + track->buf_reserve_size)) &&
            (track->track_buf_reserve != NULL)) {
    *next_byte = track->track_buf_reserve[track->index - track->buf_size];
    track->index++;
    track->current_location++;
  } else if(track->current_location >= track->track_end) {
    return(AUDFMT_STATUS_ERROR);
  } else {
    /* Data is not available, return buffer error */
    return(AUDFMT_STATUS_BUFF_ERR);
  }

  if(counter != NULL) {
    *counter = *counter + 1;
  }

  return(AUDFMT_STATUS_SUCCESS);
} /* end audfmt_get_next_track_byte */

/* <EJECT> */
/*===========================================================================

FUNCTION midi_read_var_len

DESCRIPTION
  This function reads a midi "variable length block".

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the read is lacking data, the track will be returned to its original
  state.

===========================================================================*/
static audfmt_status_type midi_read_var_len (
  audfmt_track_type *track, 
  uint32            *num,
  uint32            *counter
) {
  audfmt_status_type ret_val;
  uint32             num_read, bytes_read = 0;
  uint8              btemp;

  ret_val  = audfmt_get_next_track_byte(track, &btemp, &bytes_read);
  num_read = MIDI_VAR_LENGTH_BITS(btemp);
  while ((ret_val == AUDFMT_STATUS_SUCCESS) && 
          MIDI_VAR_LENGTH_CONT(btemp) && (bytes_read < MIDI_VAR_LENGTH_MAX)) {
    ret_val  = audfmt_get_next_track_byte(track, &btemp, &bytes_read);
    num_read = MIDI_VAR_LENGTH_CALC(num_read, btemp);
  }
    
  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    if(MIDI_VAR_LENGTH_CONT(btemp)) {
      ret_val = AUDFMT_STATUS_DATA_ERR;
    } else {
      if(counter != NULL) {
        *counter = *counter + bytes_read;
      }
      *num = num_read;
    }
  } else if(ret_val != AUDFMT_STATUS_ERROR) {
    if(audfmt_restore_track(track, bytes_read) != AUDFMT_STATUS_SUCCESS) {
      ret_val = AUDFMT_STATUS_ERROR;
    }
  }
  return(ret_val);
} /* end midi_read_var_len */

/* <EJECT> */
#ifdef FEATURE_SMAF
/*===========================================================================

FUNCTION audfmt_get_next_track_bit

DESCRIPTION
  This function gets the next bit from the track.

DEPENDENCIES
  None

RETURN VALUE
  Next data bit, if available.

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_get_next_track_bit (
  audfmt_track_type  *track,
  uint8              *next_bit,
  uint32             *counter
) {
  audfmt_status_type ret_val;

  if(track->num_compressed_bits == 0) {
    ret_val = audfmt_get_next_track_byte(track, &track->compressed_bits,
                                         counter);
    if(ret_val != AUDFMT_STATUS_SUCCESS) {
      return(ret_val);
    }
    track->num_compressed_bits = 8;
  }

  if((track->compressed_bits & 0x80) == 0x80) {
    *next_bit = 1;
  } else {
    *next_bit = 0;
  }

  track->compressed_bits <<= 1;
  track->num_compressed_bits--;

  return(AUDFMT_STATUS_SUCCESS);
} /* end audfmt_get_next_track_bit */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_next_compressed_track_byte

DESCRIPTION
  This function decompresses the next byte from a compressed track.

DEPENDENCIES
  None

RETURN VALUE
  Next data byte, if available.

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_get_next_compressed_track_byte (
  audfmt_parse_type  *midi_ctl,
  audfmt_track_type  *track,
  uint8              *next_byte,
  uint32             *counter
) {
  audfmt_status_type ret_val;
  audfmt_huffman_entry_type *huffman_table_ptr = midi_ctl->smaf_huffman_table;
  uint32 current_bit = 0x80000000, code_length = 0;
  uint32 bit_value;
  uint8  value;

  do {
    code_length++;
    ret_val = audfmt_get_next_track_bit(track, &value, counter);
    if(ret_val != AUDFMT_STATUS_SUCCESS) {
      break;
    }
    if(value != 0) {
      bit_value = current_bit;
    } else {
      bit_value = 0;
    }
    while((huffman_table_ptr->codeword & current_bit) != bit_value) {
      huffman_table_ptr++;
    }
    current_bit >>= 1;
  } while(code_length != huffman_table_ptr->code_length);

  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    *next_byte = huffman_table_ptr->value;
  }
  return(ret_val);
} /* end audfmt_get_next_compressed_track_byte */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_next_track_byte_smaf

DESCRIPTION
  This function gets the next byte from a mobile format SMAF track.

DEPENDENCIES
  None

RETURN VALUE
  Next data byte, if available.

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_get_next_track_byte_smaf (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track,
  uint8             *next_byte,
  uint32            *counter
) {
  track->current_offset++;
  if(midi_ctl->smaf_type == AUDFMT_SMAF_COMPRESSED_TYPE) {
    return(audfmt_get_next_compressed_track_byte(midi_ctl, track, next_byte,
                                                 counter));
  } else {
    return(audfmt_get_next_track_byte(track, next_byte, counter));
  }
} /* end audfmt_get_next_track_byte_smaf */

/* <EJECT> */
/*===========================================================================

FUNCTION midi_read_var_len_smaf

DESCRIPTION
  This function reads a SMAF mobile "variable length block".

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the read is lacking data, the track will be returned to its original
  state.

===========================================================================*/
static audfmt_status_type midi_read_var_len_smaf (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track,
  uint32            *num,
  uint32            *counter
) {
  audfmt_status_type ret_val;
  uint32             num_read, bytes_read = 0;
  uint8              btemp;

  uint8  num_bits        = track->num_compressed_bits;
  uint8  compressed_bits = track->compressed_bits;
  uint32 offset          = track->current_offset;

  ret_val  = audfmt_get_next_track_byte_smaf(midi_ctl, track, &btemp,
                                             &bytes_read);
  num_read = MIDI_VAR_LENGTH_BITS(btemp);
  while ((ret_val == AUDFMT_STATUS_SUCCESS) &&
          MIDI_VAR_LENGTH_CONT(btemp) && (bytes_read < MIDI_VAR_LENGTH_MAX)) {
    ret_val  = audfmt_get_next_track_byte_smaf(midi_ctl, track, &btemp,
                                               &bytes_read);
    num_read = MIDI_VAR_LENGTH_CALC(num_read, btemp);
  }

  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    if(MIDI_VAR_LENGTH_CONT(btemp)) {
      ret_val = AUDFMT_STATUS_DATA_ERR;
    } else {
      if(counter != NULL) {
        *counter = *counter + bytes_read;
      }
      *num = num_read;
    }
  } else if(ret_val != AUDFMT_STATUS_ERROR) {
    if(audfmt_restore_track(track, bytes_read) != AUDFMT_STATUS_SUCCESS) {
      ret_val = AUDFMT_STATUS_ERROR;
    }
    track->num_compressed_bits = num_bits;
    track->compressed_bits     = compressed_bits;
    track->current_offset      = offset;
  }
  return(ret_val);
} /* end midi_read_var_len_smaf */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_led_buffer_cb

DESCRIPTION
  This function buffer up a smaf led status command on the callback array.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void smaf_led_buffer_cb (
  audfmt_parse_type    *midi_ctl,
  snd_cmx_smaf_led_ctl_status_enum_type smaf_led_ctl,
  snd_af_data_cmd_enum control
) {
  snd_cmx_smaf_led_vib_ctl_type  smaf_ctl;
  audfmt_cb_store_type *cb_store = midi_ctl->cb_store;
  boolean cb_required = TRUE;
  uint32 counter;

  /* If there is already CMD buffered, buffured CMD should be opposite of the
     current CMD, if not then there is problem with the synchronization logic.
     Clear the buffured CMD
  */
  if(cb_store->smaf_cb_num != 0) {
    for(counter = 0; counter < cb_store->smaf_cb_num; counter++) {
      if(cb_store->smaf_cb[counter].smaf_cmd == SND_CMX_SMAF_LED_CONTROL) {
        while(counter < (cb_store->smaf_cb_num - 1)) {
          memcpy(&(cb_store->smaf_cb[counter]), &(cb_store->smaf_cb[counter + 1]),
                 sizeof(audfmt_smaf_cb_store_type));
          counter++;
        }
        cb_store->smaf_cb_num = cb_store->smaf_cb_num - 1;
        cb_required = FALSE;
        break;
      }
    }
  }
  /* Send only if no LED CMD was buffred */
  if(cb_required == TRUE) {
    smaf_ctl.smaf_led_ctl = smaf_led_ctl;
    audfmt_buffer_cb(midi_ctl, control, 0, &smaf_ctl);
  }
} /* end smaf_led_buffer_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_vib_buffer_cb

DESCRIPTION
  This function buffer up a smaf vib status command on the callback array.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void smaf_vib_buffer_cb (
  audfmt_parse_type    *midi_ctl,
  snd_cmx_smaf_vib_ctl_status_enum_type smaf_vib_ctl,
  snd_af_data_cmd_enum control
) {
  snd_cmx_smaf_led_vib_ctl_type  smaf_ctl;
  audfmt_cb_store_type *cb_store = midi_ctl->cb_store;
  boolean cb_required = TRUE;
  uint32 counter;

  /* If there is already CMD buffered, buffured CMD should be opposite of the
     current CMD, if not then there is problem with the synchronization logic.
     Clear the buffured CMD
  */
  if(cb_store->smaf_cb_num != 0) {
    for(counter = 0; counter < cb_store->smaf_cb_num; counter++) {
      if(cb_store->smaf_cb[counter].smaf_cmd == SND_CMX_SMAF_VIB_CONTROL) {
        while(counter < (cb_store->smaf_cb_num - 1)) {
          memcpy(&(cb_store->smaf_cb[counter]), &(cb_store->smaf_cb[counter + 1]),
                 sizeof(audfmt_smaf_cb_store_type));
          counter++;
        }
        cb_store->smaf_cb_num = cb_store->smaf_cb_num - 1;
        cb_required = FALSE;
        break;
      }
    }
  }
  /* Send only if no VIB CMD was buffered */
  if(cb_required == TRUE) {              
    smaf_ctl.smaf_vib_ctl = smaf_vib_ctl;
    audfmt_buffer_cb(midi_ctl, control, 0, &smaf_ctl);
  }
} /* end smaf_vib_buffer_cb */
#endif /* FEATURE_SMAF */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_check_buffer_done

DESCRIPTION
  This function checks the data buffers of the track to see if one has
  expired. If so, this function will free the buffer and request the next
  data buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_check_buffer_done (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track
) {
  audfmt_status_type ret_val = AUDFMT_STATUS_SUCCESS;

  if(track->index >= track->buf_size) {
    /* If data pointer is beyond end of first buffer */
    if(track->track_buf_reserve != NULL) {
      /* If second buffer is available, then free first buffer and swap */
      audfmt_do_play_cb(track->audfmt_handle, SND_CMX_AF_FREE_BUF, 0, 
                        (void *) track->track_buf);

      track->track_buf         = track->track_buf_reserve;
      track->index             = track->index - track->buf_size;
      track->buf_size          = track->buf_reserve_size;
      track->track_buf_reserve = NULL;
      track->buf_reserve_size  = 0;
    }
  }

// should this be removed???
  /* This is put in here to take care of case where an empty buffer was
  ** returned from the client.
  */
  if((track->track_buf_reserve != NULL) && (track->buf_reserve_size == 0)) {
    audfmt_do_play_cb(track->audfmt_handle, SND_CMX_AF_FREE_BUF, 0, 
                      (void *) track->track_buf_reserve);
    track->track_buf_reserve = NULL;
  }
    
  if((track->track_buf_reserve == NULL) &&
     ((track->track_end == AUDFMT_MAX_32BIT) ||
      (track->file_location < track->track_end)) &&
     !(track->track_status & AUDFMT_TRACK_MASK_PENDING_BUF)) {
    midi_ctl->num_read++;
    track->track_status |= AUDFMT_TRACK_MASK_PENDING_BUF;
    if(audmain_get_async_data(track->audfmt_handle, track->file_location,
                              &(track->track_buf_reserve)) !=
       AUDMAIN_STATUS_SUCCESS) {
      ret_val = AUDFMT_STATUS_ERROR;
    }

    /* This check is necessary to ensure that we do not queue too many
    ** read commands to the client, esp for multisequencer situation.
    */
    if(midi_ctl->num_read >= 8) {
      ret_val = AUDFMT_STATUS_BUFF_ERR;
    }
  }
  return(ret_val);
} /* end audfmt_check_buffer_done */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_track_skip_ahead

DESCRIPTION
  This function skips the indicated number of bytes ahead. The appropriate
  buffer management is taken care of in this fucntion.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_track_skip_ahead (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track,
  uint32            num_bytes
) {
  audfmt_status_type ret_val = AUDFMT_STATUS_SUCCESS;

  track->current_location += num_bytes;

  if(num_bytes != 0) {
    if((track->index + num_bytes) < track->buf_size) {
      /* The jump destination is within our current data buffer */
      track->index += num_bytes;
    } else if((track->track_buf_reserve != NULL) &&
              ((track->index + num_bytes) <
               (track->buf_size + track->buf_reserve_size))) {
      /* Jump destination is in reserve buffer */
      /* Free current buffer and swap with reserve */
      audfmt_do_play_cb(track->audfmt_handle, SND_CMX_AF_FREE_BUF, 0, 
                        (void *) track->track_buf);

      track->track_buf         = track->track_buf_reserve;
      track->index             = track->index + num_bytes - track->buf_size;
      track->buf_size          = track->buf_reserve_size;
      track->track_buf_reserve = NULL;
      track->buf_reserve_size  = 0;
  
      if((track->track_end == AUDFMT_MAX_32BIT) ||
         (track->file_location < track->track_end)) {
        midi_ctl->num_read++;
        track->track_status  |= AUDFMT_TRACK_MASK_PENDING_BUF;

        if(audmain_get_async_data(track->audfmt_handle, track->file_location,
                                  &(track->track_buf_reserve)) !=
           AUDMAIN_STATUS_SUCCESS) {
          ret_val = AUDFMT_STATUS_ERROR;
        }
      }
    } else {
      /* Location beyond the data buffers we currently have */
      if(track->track_buf_reserve != NULL) {
        /* If reserve buffer is current, free it */
        audfmt_do_play_cb(track->audfmt_handle, SND_CMX_AF_FREE_BUF, 0, 
                          (void *) track->track_buf_reserve);
        track->track_buf_reserve = NULL;
        track->buf_reserve_size  = 0;
      } else {
        /* Reserve buffer has already been requested, therefore we should
        ** skip the next buffer that is returned
        */
        track->skip_buf_count++;
      }
      track->index          = track->buf_size;
      track->file_location  = track->current_location;
      track->track_status  |= AUDFMT_TRACK_MASK_DO_TIME |
                              AUDFMT_TRACK_MASK_PENDING_BUF;

      midi_ctl->num_read++;
      (void) audmain_get_async_data(track->audfmt_handle,
                                    track->file_location, 
                                    &(track->track_buf_reserve));
      return(AUDFMT_STATUS_BUFF_ERR);
    }
  }

  return(ret_val);
} /* end audfmt_track_skip_ahead */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_add_global_command

DESCRIPTION
  This function adds a command to the global command buffer if there is room.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_add_global_command (
  audfmt_parse_type *midi_ctl,
  uint8             *command,
  uint16            num_bytes
) {
  audfmt_status_type ret_val = AUDFMT_STATUS_SUCCESS;
  uint16             counter;

  if(midi_ctl != NULL) {
    if((audfmt_global_cmd.num_words + num_bytes) < AUDFMT_CMD_BUF_MAX_WORDS) {
      /* Add command to queue with proper sequence number */
      audfmt_global_cmd.command_buf[audfmt_global_cmd.num_words++] =
                                              (*command | midi_ctl->sequence);
      for(counter = 1; counter < num_bytes; counter++) {
        audfmt_global_cmd.command_buf[audfmt_global_cmd.num_words++] =
                                              command[counter];
      }
      audfmt_global_cmd.num_commands++;
    } else {
      ret_val = AUDFMT_STATUS_CMD_BUFF_ERR;
    }
  }
  return(ret_val);
} /* end audfmt_add_global_command */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_add_command

DESCRIPTION
  This function adds a command to the command buffer if there is room.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_add_command (
  audfmt_parse_type *midi_ctl,
  uint8             *command,
  uint16            num_bytes
) {
  audfmt_status_type  ret_val = AUDFMT_STATUS_SUCCESS;
  audfmt_cmd_buf_type *cmd_buf;
  uint16              counter;

  if(midi_ctl != NULL) {
    cmd_buf = &midi_ctl->cmd_buf;
    if((cmd_buf->num_words + num_bytes) < AUDFMT_CMD_BUF_MAX_WORDS) {
      /* Add command to queue with proper sequence number */
      cmd_buf->command_buf[cmd_buf->num_words++] = (*command |
                                                    midi_ctl->sequence);
      for(counter = 1; counter < num_bytes; counter++) {
        cmd_buf->command_buf[cmd_buf->num_words++] = command[counter];
      }
      cmd_buf->num_commands++;
    } else {
      ret_val = AUDFMT_STATUS_CMD_BUFF_ERR;
    }
  }
  return(ret_val);
} /* end audfmt_add_command */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_check_no_command

DESCRIPTION
  This function checks for a note on command that matches the indicated 
  note off command in the current command buffer, if found it will be removed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_check_no_command (
  audfmt_cmd_buf_type  *cmd_buf,
  uint8                *command
) {
  audfmt_status_type ret_val = AUDFMT_STATUS_FAILURE;
  uint32             counter;

  if(cmd_buf != NULL) {
    counter = 0;
    while((counter < cmd_buf->num_words) && (ret_val != AUDFMT_STATUS_SUCCESS))
    {
      switch(cmd_buf->command_buf[counter] & 0xf0) {
        case MIDI_NOTE_ON:
          if((cmd_buf->command_buf[counter+1] == command[1]) &&
             (cmd_buf->command_buf[counter+2] == command[2])) {
            cmd_buf->num_commands -= 1;
            cmd_buf->num_words    -= AUDFMT_COMMAND_SIZE_4;
            for(; counter < cmd_buf->num_words; counter++) {
              cmd_buf->command_buf[counter] = 
                          cmd_buf->command_buf[counter + AUDFMT_COMMAND_SIZE_4];
            }
            ret_val = AUDFMT_STATUS_SUCCESS;
            break;
          }
          /* Fall thru */

        case MIDI_NOTE_OFF:
        case MIDI_AFTER_TOUCH:
        case MIDI_CONTROL_CHANGE:
        case MIDI_PITCH_WHEEL:
          counter += AUDFMT_COMMAND_SIZE_4;
          break;

        case MIDI_PROGRAM_CHANGE:
        case MIDI_CHANNEL_PRESSURE:
          counter += AUDFMT_COMMAND_SIZE_3;
          break;

        case 0xf0:
          if(cmd_buf->command_buf[counter] == MIDI_RESET_COMMAND) {
            counter += 1;
          } else {
            while(cmd_buf->command_buf[counter++] != 0xf7);
          }
          break;
 
        default:
          ERR_FATAL("Unrecognized command: %d", 
                     cmd_buf->command_buf[counter], 0, 0);
      }
    }
  }
  return(ret_val);
} /* end audfmt_check_no_command */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION cmid_note_off_add

DESCRIPTION
  This function adds a note off event to the compact midi note off buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type cmid_note_off_add (
  audfmt_parse_type *midi_ctl,
  uint32            gate,
  uint8             chan,
  uint8             note,
  uint8             device_ctl
) {
  audfmt_note_off_buffer_type *no_buffer;
  audfmt_status_type          ret_val = AUDFMT_STATUS_CMD_BUFF_ERR;
  uint32 index, index2, index3, counter;

  no_buffer = &midi_ctl->note_off_buffer;
  if(no_buffer->num < CMID_NOTE_OFF_MAX_COMMANDS) {
    index   = no_buffer->index;
    counter = 0;
    /* Find position to insert this note off command */
    while((gate >= no_buffer->commands[index].gate) && 
          (counter < no_buffer->num)) {
      gate  -= no_buffer->commands[index].gate;
      index  = CMID_NOTE_OFF_NEXT_COMMAND(index);
      counter++;
    }
    if(counter != no_buffer->num) {
      /* If not at the end of note off list, move rest of note off commands
      ** down the list
      */
      no_buffer->commands[index].gate -= gate;

      index2 = index + (no_buffer->num - counter);
      index2 = CMID_NOTE_OFF_INDEX_CORR(index2);
  
      while (index2 != index) {
        index3                      = index2;
        index2                      = CMID_NOTE_OFF_PREV_COMMAND(index2);
        no_buffer->commands[index3] = no_buffer->commands[index2];
      }
    }
  
    /* Insert note off command */
    no_buffer->commands[index].gate        = gate;
    no_buffer->commands[index].channel     = (chan & MIDI_CHANNEL_NIBBLE);
    no_buffer->commands[index].note        = note;
    no_buffer->commands[index].device_mask = device_ctl;
    no_buffer->num++;

    ret_val = AUDFMT_STATUS_SUCCESS;
  }
  return(ret_val);
} /* end cmid_note_off_add */

/* <EJECT> */
/*===========================================================================

FUNCTION cmid_note_off_remove

DESCRIPTION
  This function removes a note off event from the note off buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type cmid_note_off_remove (
  audfmt_parse_type *midi_ctl,
  uint8             chan,
  uint8             note
) {
  audfmt_note_off_buffer_type *no_buffer;
  uint32 index, index2, index3;
  uint32 commands = 0;

  no_buffer = &midi_ctl->note_off_buffer;
  index     = no_buffer->index;
  chan      = chan & MIDI_CHANNEL_NIBBLE;

  /* Scan list for matching note off comamnd */
  while(commands < no_buffer->num) {
    if((no_buffer->commands[index].channel == chan) &&
       (no_buffer->commands[index].note == note)) {
      commands = no_buffer->num - commands;
      /* If note off command is not at the end of the list then move
      ** the rest of the commands up
      */
      if(commands > 1) {
        index2 = no_buffer->index + no_buffer->num;

        index2 = CMID_NOTE_OFF_INDEX_CORR(index2);
        index3 = CMID_NOTE_OFF_NEXT_COMMAND(index);

        no_buffer->commands[index3].gate += no_buffer->commands[index].gate;

        while (index3 != index2) {
          no_buffer->commands[index] = no_buffer->commands[index3];
          index  = index3;
          index3 = CMID_NOTE_OFF_NEXT_COMMAND(index3);
        }
      }
      no_buffer->num--;
      return(AUDFMT_STATUS_SUCCESS);
    }

    index = CMID_NOTE_OFF_NEXT_COMMAND(index);
    commands ++;
  }
  return(AUDFMT_STATUS_FAILURE);
} /* end cmid_note_off_remove */

/* <EJECT> */
/*===========================================================================

FUNCTION cmid_note_off_check_overlap

DESCRIPTION
  This function checks the cmid note off queue for a match to the indicated
  note on command, if found, the comamnd will be reinserted in the queue at
  the appropriate time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type cmid_note_off_check_overlap (
  audfmt_parse_type *midi_ctl,
  uint32            gate,
  uint8             chan,
  uint8             note
) {
  audfmt_note_off_buffer_type *no_buffer;
  uint32 index, index2, index3;
  uint32 commands = 0;
  uint32 ticks    = 0;
  uint8  device_ctl;

  no_buffer = &midi_ctl->note_off_buffer;
  index     = no_buffer->index;
  chan      = chan & MIDI_CHANNEL_NIBBLE;

  /* Scan list for matching note off command */
  while(commands < no_buffer->num) {
    ticks += no_buffer->commands[index].gate;
    if((no_buffer->commands[index].channel == chan) &&
       (no_buffer->commands[index].note == note)) {

      device_ctl = no_buffer->commands[index].device_mask;
      if(ticks < gate) {
        /* If new gate time is greater than old one, reposition note off
        ** command
        */
        commands = no_buffer->num - commands;
        if(commands > 1) {
          /* If note off is in middle of list, first remove and then
          ** reinsert at correct position
          */
          index2 = no_buffer->index + no_buffer->num;

          index2 = CMID_NOTE_OFF_INDEX_CORR(index2);
          index3 = CMID_NOTE_OFF_NEXT_COMMAND(index);

          no_buffer->commands[index3].gate += no_buffer->commands[index].gate;

          while (index3 != index2) {
            no_buffer->commands[index] = no_buffer->commands[index3];
            index  = index3;
            index3 = CMID_NOTE_OFF_NEXT_COMMAND(index3);
          }
          no_buffer->num--;
          cmid_note_off_add(midi_ctl, gate, chan, note, device_ctl);
        } else {
          no_buffer->commands[index].gate = gate;
        }
      }
      return(AUDFMT_STATUS_SUCCESS);
    }

    index = CMID_NOTE_OFF_NEXT_COMMAND(index);
    commands ++;
  }
  return(AUDFMT_STATUS_FAILURE);
} /* end cmid_note_off_check_overlap */

/* <EJECT> */
/*===========================================================================

FUNCTION cmid_note_off_delta_time

DESCRIPTION
  This function gets the time left before the next compact midi note off event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type cmid_note_off_delta_time (
  audfmt_parse_type *midi_ctl,
  uint32            *delta_time
) {
  audfmt_note_off_buffer_type *no_buffer;

  no_buffer = &midi_ctl->note_off_buffer;
  if(no_buffer->num != 0) {
    *delta_time = no_buffer->commands[no_buffer->index].gate;
    return(AUDFMT_STATUS_SUCCESS);
  } else {
    return(AUDFMT_STATUS_FAILURE);
  }
} /* end cmid_note_off_delta_time */

/* <EJECT> */
/*===========================================================================

FUNCTION cmid_note_off_age

DESCRIPTION
  This function ages the compact midi note off buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmid_note_off_age (
  audfmt_parse_type *midi_ctl,
  uint32            elapsed_tick
) {
  audfmt_note_off_buffer_type *no_buffer;
  uint32 index, number;

  no_buffer = &midi_ctl->note_off_buffer;
  number    = no_buffer->num;
  index     = no_buffer->index;

  while((elapsed_tick != 0) && (number != 0)) {
    if(elapsed_tick <= no_buffer->commands[index].gate) {
      no_buffer->commands[index].gate -= elapsed_tick;
      elapsed_tick                     = 0;
    } else {
      elapsed_tick                    -= no_buffer->commands[index].gate;
      no_buffer->commands[index].gate  = 0;
      index                            = CMID_NOTE_OFF_NEXT_COMMAND(index);
      number--;
    }
  }
} /* end cmid_note_off_age */

/* <EJECT> */
/*===========================================================================

FUNCTION cmid_note_off_verify

DESCRIPTION
  This function checks for any compact midi note offs that have expired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type cmid_note_off_verify (
  audfmt_parse_type *midi_ctl
) {
  audfmt_note_off_buffer_type *no_buffer;
  audfmt_status_type          ret_val = AUDFMT_STATUS_SUCCESS;
  uint32                      index;
  uint8                       buf[AUDFMT_COMMAND_SIZE_4];
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

  no_buffer = &midi_ctl->note_off_buffer;
  index     = no_buffer->index;

  /* Scan list for expired note off commands */
  while((no_buffer->commands[index].gate == 0) && (no_buffer->num > 0)) {
    if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
      buf[0] = MIDI_NOTE_OFF;
      buf[1] = no_buffer->commands[index].channel;
      buf[2] = no_buffer->commands[index].note;
      buf[3] = 0;

#ifdef FEATURE_SMAF
// need to verify SMAF adec off commands are correct
      if((midi_ctl->type == AUDFMT_MIDI_SMAF_TYPE) &&
         AUDFMT_IS_ADEC_NO(buf[2]) &&
         midi_ctl->adec_status[AUDFMT_NO_GET_ADEC_CHAN(buf[2])].enable) {
        midi_ctl->adec_status[AUDFMT_NO_GET_ADEC_CHAN(buf[2])].reset = TRUE;
      } else
#endif /* FEATURE_SMAF */
      if(audfmt_check_no_command(&midi_ctl->cmd_buf, buf) !=
                AUDFMT_STATUS_SUCCESS) {
        ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
        if(ret_val != AUDFMT_STATUS_SUCCESS) break;
	  }
#ifdef FEATURE_SMAF
      if(!(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE) &&
         (midi_ctl->cb_store != NULL)) {
        if (midi_ctl->smaf_led_ctl & (1<<buf[1])) {
          if(midi_ctl->smaf_led_count == 1) {
            smaf_led_buffer_cb(midi_ctl, SND_SMAF_LED_CTL_STATUS_OFF, SND_CMX_SMAF_LED_CONTROL);
          }
          if(midi_ctl->smaf_led_count > 0) {
            midi_ctl->smaf_led_count = midi_ctl->smaf_led_count - 1;
          }
        }
        if (midi_ctl->smaf_vib_ctl & (1<<buf[1])) {
          if(midi_ctl->smaf_vib_count == 1) {
            smaf_vib_buffer_cb(midi_ctl, SND_SMAF_VIB_CTL_STATUS_OFF, SND_CMX_SMAF_VIB_CONTROL);
          }
          if(midi_ctl->smaf_vib_count > 0) {
            midi_ctl->smaf_vib_count = midi_ctl->smaf_vib_count - 1;
          }
        }
      }
#endif /* FEATURE_SMAF */

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
    }

    index            = CMID_NOTE_OFF_NEXT_COMMAND(index);
    no_buffer->index = index;
    no_buffer->num--;
  }
  return(ret_val);
} /* end cmid_note_off_verify */

/* <EJECT> */
/*===========================================================================

FUNCTION cmid_note_off_tick_remain

DESCRIPTION
  This function calculates the ticks in all remaining note off commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static uint32 cmid_note_off_tick_remain (
  audfmt_parse_type *midi_ctl
) {
  audfmt_note_off_buffer_type *no_buffer;
  uint32 index, counter, intermediate_ticks = 0, ticks = 0;

  no_buffer = &midi_ctl->note_off_buffer;
  index     = no_buffer->index;
  counter   = 0;

  while(counter < no_buffer->num) {
    intermediate_ticks += no_buffer->commands[index].gate;

    if(no_buffer->commands[index].note < 0x80) {
      ticks += intermediate_ticks;
      intermediate_ticks = 0;
    }

    index  = CMID_NOTE_OFF_NEXT_COMMAND(index);
    counter++;
  }
  return(ticks);
} /* end cmid_note_off_tick_remain */

/* <EJECT> */
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

/* <EJECT> */
/*===========================================================================

FUNCTION cmid_vib_next_index

DESCRIPTION
  This function determines the next state in the vibrator control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type cmid_vib_next_index (
  audfmt_vib_type *vib_ctl
) {
  audfmt_vib_ctl_type *vib_ptr;

  vib_ctl->index++;
  vib_ptr          = &(vib_ctl->vib_ctl[vib_ctl->index]);
  vib_ctl->time_ms = vib_ptr->time_ms;

  if(vib_ptr->vib_ctl == AUDFMT_VIB_RPT) {
    vib_ctl->index   = 0;
    vib_ctl->time_ms = vib_ctl->vib_ctl->time_ms;
  } else if((vib_ptr->vib_ctl == AUDFMT_VIB_STOP) ||
            ((vib_ptr->vib_ctl != AUDFMT_VIB_ON) && 
             (vib_ptr->vib_ctl != AUDFMT_VIB_OFF))) {
    vib_ctl->index   = 0;
    vib_ctl->time_ms = 0;
    vib_ctl->vib_ctl = NULL;
    return(AUDFMT_STATUS_DONE);
  }
  return(AUDFMT_STATUS_SUCCESS);
} /* end cmid_vib_next_index */

/* <EJECT> */
/*===========================================================================

FUNCTION cmid_vib_age

DESCRIPTION
  This function ages the cmid vibrator control by the indicated number of
  milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmid_vib_age (
  audfmt_vib_type *vib_ctl,
  uint32          time_ms
) {
  audfmt_status_type status = AUDFMT_STATUS_SUCCESS;

  if(vib_ctl->vib_ctl != NULL) {
    while ((time_ms != 0) && (status == AUDFMT_STATUS_SUCCESS)) {
      if(time_ms < vib_ctl->time_ms) {
        vib_ctl->time_ms -= time_ms;
        time_ms           = 0;
      } else {
        time_ms -= vib_ctl->time_ms;
        status   = cmid_vib_next_index(vib_ctl);
      }
    }
  }
} /* end cmid_vib_age */

/* <EJECT> */
/*===========================================================================

FUNCTION cmid_check_vib

DESCRIPTION
  This function checks to see if the vibrator is currently enabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static boolean cmid_check_vib (
  audfmt_vib_type *vib_ctl
) {
  return((vib_ctl->vib_ctl != NULL) && 
          (vib_ctl->vib_ctl[vib_ctl->index].vib_ctl == AUDFMT_VIB_ON)); 
} /* end cmid_check_vib */

/* <EJECT> */
/*===========================================================================

FUNCTION cmid_vib_time_ms

DESCRIPTION
  This function returns the time remaining until the next vibrator event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type cmid_vib_time_ms (
  audfmt_vib_type *vib_ctl,
  uint32          *delta_time_ms
) {
  if(vib_ctl->vib_ctl != NULL) {
    *delta_time_ms = vib_ctl->time_ms;
    return(AUDFMT_STATUS_SUCCESS);
  } else {
    return(AUDFMT_STATUS_FAILURE);
  }
} /* end cmid_vib_time_ms */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_reset_adec_channels

DESCRIPTION
  This function resets all ADEC channels for the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  audfmt_adec_mask

===========================================================================*/
static void audfmt_reset_adec_channels (
  audfmt_parse_type *midi_ctl,
  boolean           end_seq
) {
  audfmt_adec_status_type *adec_status;
  uint32                  counter;

  adec_status = midi_ctl->adec_status;

  for(counter = 0; counter < AUDFMT_NUM_ADEC_CHANNELS; counter++) {
    if(adec_status->enable) {
      audfmt_adec_mask &= ~(1<< counter);
      audfadec_wa_reset(counter, end_seq);
    }
    adec_status->enable = FALSE;
    adec_status->reset  = FALSE;
    adec_status++;
  }
} /* end audfmt_reset_adec_channels */
  
/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_adec_channel

DESCRIPTION
  This function finds an ADEC channel to use.

DEPENDENCIES
  audfmt_adec_mask

RETURN VALUE
  None

SIDE EFFECTS
  audfmt_adec_mask

===========================================================================*/
static audfmt_status_type audfmt_get_adec_channel (
  audfmt_parse_type *midi_ctl,
  uint32            *channel
) {
  audfmt_status_type ret_val = AUDFMT_STATUS_FAILURE;
  uint32             counter = 0;

  /* First see if there are any ADEC channels free */
  while((counter < AUDFMT_NUM_ADEC_CHANNELS) &&
        (audfmt_adec_mask & (1 << counter))) {
    counter++;
  }

  if(counter < AUDFMT_NUM_ADEC_CHANNELS) {
    audfmt_adec_mask |= (1 << counter);
    *channel          = counter;
    ret_val           = AUDFMT_STATUS_SUCCESS;
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
  } else {
    /* If no ADEC channels free, then take a channel that is currently
    ** being used by current sequence, if any.
    */
    for(counter = 0; counter < AUDFMT_NUM_ADEC_CHANNELS; counter++) {
      if(midi_ctl->adec_status[counter].enable) {
        *channel = counter;
        ret_val  = AUDFMT_STATUS_SUCCESS;
        break;
      }
    }
  }
  return(ret_val);
} /* end audfmt_get_adec_channel */
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_read_time

DESCRIPTION
  This function reads the delta time from the given track according to the
  indicated type.

DEPENDENCIES
  None

RETURN VALUE
  audfmt_status_type
  delta_time - time read from track

SIDE EFFECTS
  If the time read fails due to lack of data, the track will be returned to
  its original state.

===========================================================================*/
static audfmt_status_type audfmt_read_time (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track,
  uint32            *delta_time
) {
  audfmt_status_type ret_val;
#ifdef FEATURE_WEBAUDIO
  uint8              tbyte;
#ifdef FEATURE_SMAF
  uint16             tword;
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */

  switch(midi_ctl->type) {
    case AUDFMT_MIDI_MIDI_TYPE:
      ret_val = midi_read_var_len(track, delta_time, NULL);
      break;

#ifdef FEATURE_WEBAUDIO
    case AUDFMT_MIDI_CMID_TYPE:
    case AUDFMT_MIDI_MELO_TYPE:
    case AUDFMT_MIDI_WASF_TYPE:
      ret_val = audfmt_get_next_track_byte(track, &tbyte, NULL);
      if(ret_val != AUDFMT_STATUS_BUFF_ERR) {
        *delta_time = tbyte;
      }
      break;

#ifdef FEATURE_SMAF
    case AUDFMT_MIDI_SMAF_TYPE:
      switch(midi_ctl->smaf_type) {
        case AUDFMT_SMAF_HP_TYPE:
        case AUDFMT_SMAF_PHRASE_TYPE:
          ret_val = audfmt_get_next_track_byte(track, &tbyte, NULL);
          if(ret_val == AUDFMT_STATUS_SUCCESS) {
            if(tbyte & 0x80) {
              tword   = ((tbyte & 0x7f) << 7);
              ret_val = audfmt_get_next_track_byte(track, &tbyte, NULL);
              if(ret_val == AUDFMT_STATUS_SUCCESS) {
                *delta_time = (tword | (tbyte & 0x7f)) + 128;
              } else if(ret_val != AUDFMT_STATUS_ERROR) {
                if(audfmt_restore_track(track, 1) != AUDFMT_STATUS_SUCCESS) {
                  ret_val = AUDFMT_STATUS_ERROR;
                }
              }
            } else {
              *delta_time = tbyte;
            }
          }
          break;

      case AUDFMT_SMAF_MOBILE_TYPE:
        case AUDFMT_SMAF_COMPRESSED_TYPE:
          ret_val = midi_read_var_len_smaf(midi_ctl, track, delta_time, NULL);
          break;

        //case AUDFMT_SMAF_MOBILE_TYPE:
          //ret_val = midi_read_var_len(track, delta_time, NULL);
          //break;

        default:
          ret_val = AUDFMT_STATUS_ERROR;
          break;
      }
      break;
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */

    case AUDFMT_MIDI_INVALID_TYPE:
    default:
      ret_val = AUDFMT_STATUS_ERROR;
      break;
  }

  return(ret_val);
} /* end audfmt_read_time */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_calc_routine

DESCRIPTION
  This function calculates the formula mul1 * mul2 / div1 and returns a 
  32 bit result. 64bit math is done if necessary.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Calculated 32-bit value.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 audfmt_calc_routine (
  uint32 mul1,
  uint32 mul2,
  uint32 div1,
  uint32 overflow_factor
) {
  qword  qtemp1, qtemp2;
  uint32 ret_val;
  uint16 tshort;

  /* overflow_factor is a function of mul2, if mul1 > overflow_factor then
  ** we must use 64bit math.
  */

   /* If div1 is 0 then it leads to div1 byzero crash */
   /* set the value of div1 to 1                      */
   if(div1 ==0)
   {
     div1++;
   }
  if(overflow_factor < mul1) {
    qw_set(qtemp1, 0, mul1);
    qw_mul(qtemp2, qtemp1, mul2);
    /* If div1 is larger than 16bit value, then the division must be done in 
    ** two stages.
    */
    if(div1 > AUDFMT_MAX_24BIT) {
      tshort = (uint16) (div1 >> AUDFMT_24_OVERFLOW_FACTOR);
      (void) qw_div(qtemp1, qtemp2, tshort);
      (void) qw_div_by_power_of_2(qtemp2, qtemp1, AUDFMT_24_OVERFLOW_FACTOR);
    } else if(div1 > AUDFMT_MAX_16BIT) {
      tshort = (uint16) (div1 >> AUDFMT_16_OVERFLOW_FACTOR);
      (void) qw_div(qtemp1, qtemp2, tshort);
      (void) qw_div_by_power_of_2(qtemp2, qtemp1, AUDFMT_16_OVERFLOW_FACTOR);
    } else {
      (void) qw_div(qtemp1, qtemp2, (uint16) div1);
      qw_equ(qtemp2, qtemp1);
    }

    if(qw_hi(qtemp2) != 0) {
      ERR("Overflow cap at AUDFMT_MAX_32BIT!!!",0,0,0);
      ret_val = AUDFMT_MAX_32BIT;
    } else {
      ret_val = qw_lo(qtemp2);
    }

  } else {
    ret_val = (mul1 * mul2) / div1;
  }

  return(ret_val);
} /* end audfmt_calc_routine */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_calc_ms

DESCRIPTION
  This function calculates the delta time, in milliseconds, for the 
  indicated type.

DEPENDENCIES
  audfmt_ppqn_1000

RETURN VALUE
  time_ms - Calculated time, in ms.

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_calc_ms (
  audfmt_parse_type *midi_ctl,
  uint32            delta_time,
  uint32            *time_ms
) {

  switch(midi_ctl->type) {
    case AUDFMT_MIDI_MIDI_TYPE:
      *time_ms = audfmt_calc_routine ( delta_time, midi_ctl->tempo, 
                                       midi_ctl->calc_factor,
                                       midi_ctl->calc_ms_of );
      break;

#ifdef FEATURE_WEBAUDIO
    case AUDFMT_MIDI_CMID_TYPE:
    case AUDFMT_MIDI_MELO_TYPE:
    case AUDFMT_MIDI_WASF_TYPE:
      *time_ms = audfmt_calc_routine ( delta_time, midi_ctl->calc_factor, 
                                       midi_ctl->tempo, midi_ctl->calc_ms_of );
      break;

#ifdef FEATURE_SMAF
    case AUDFMT_MIDI_SMAF_TYPE:
      /* Calc factor of 0 indicates that user tempo is 100% */
      if(midi_ctl->calc_factor == 0) {
        *time_ms = delta_time * midi_ctl->smaf_timebase_d;
      } else {
        *time_ms = (delta_time * midi_ctl->calc_factor) /
                                               midi_ctl->tempo_factor;
      }
      break;
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */

    case AUDFMT_MIDI_INVALID_TYPE:
    default:
      *time_ms = 0;
      break;
  }
} /* end audfmt_calc_ms */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_calc_tick

DESCRIPTION
  This function calculates the delta time, in ticks, for the 
  indicated type.

DEPENDENCIES
  None

RETURN VALUE
  time - Calculated time, in ticks.

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_calc_tick (
  audfmt_parse_type *midi_ctl,
  uint32            time_ms,
  uint32            *tick
) {

  switch(midi_ctl->type) {
    case AUDFMT_MIDI_MIDI_TYPE:
      *tick = audfmt_calc_routine ( time_ms, midi_ctl->calc_factor, 
                                    midi_ctl->tempo, midi_ctl->calc_tick_of);
      break;

#ifdef FEATURE_WEBAUDIO
    case AUDFMT_MIDI_CMID_TYPE:
    case AUDFMT_MIDI_MELO_TYPE:
    case AUDFMT_MIDI_WASF_TYPE:
      *tick = audfmt_calc_routine ( time_ms, midi_ctl->tempo,
                                    midi_ctl->calc_factor, 
                                    midi_ctl->calc_tick_of );
      break;

#ifdef FEATURE_SMAF
    case AUDFMT_MIDI_SMAF_TYPE:
      /* Calc factor of 0 indicates that user tempo is 100% */
      if(midi_ctl->calc_factor == 0) {
        *tick = time_ms / midi_ctl->smaf_timebase_d;
      } else {
        *tick = (time_ms * midi_ctl->tempo_factor) / midi_ctl->calc_factor;
      }
      break;
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */

    case AUDFMT_MIDI_INVALID_TYPE:
    default:
      *tick = 0;
      break;
  }
} /* end audfmt_calc_tick */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_do_pause_cmd

DESCRIPTION
  This function pauses the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_do_pause_cmd (
  audfmt_parse_type *midi_ctl,
  boolean           pause_flag
) {
#ifdef FEATURE_WEBAUDIO
  uint32 channel;
#endif /* FEATURE_WEBAUDIO */

  if(midi_ctl != NULL) {
    if(audmain_audfmt_pause_seq(midi_ctl->sequence, pause_flag) == 
       AUDMAIN_STATUS_SUCCESS) {
      midi_ctl->midi_status2 &= ~AUDFMT_MASK_PAUSE_CMD_PENDING;
#ifdef FEATURE_WEBAUDIO
      for(channel = 0; channel < AUDFMT_NUM_ADEC_CHANNELS; channel++) {
        if(midi_ctl->adec_status[channel].enable) {
          audmain_audfmt_adec_chan_pause(channel, pause_flag);
        }
      }
#endif /* FEATURE_WEBAUDIO */
    } else if(pause_flag) {
      midi_ctl->midi_status2 |= AUDFMT_MASK_PAUSE_CMD_PENDING;
    } else {
      midi_ctl->midi_status2 &= ~AUDFMT_MASK_PAUSE_CMD_PENDING;
    }
  }
} /* end audfmt_do_pause_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_final_cmd

DESCRIPTION
  This function returns the final commands in the global command buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_get_final_cmd (
  audmain_cmd_buf_type *cmd_buf
) {
  audmain_midi_cmd_buf_type *midi_cmd;

  if(audfmt_global_cmd.num_commands != 0) {
    midi_cmd = &(cmd_buf->midi_cmd[0]);
    if(
#ifdef FEATURE_ANIMATED_AAC
       (audmain_get_adec_mode() != AUDMAIN_ADEC_MODE_AAC) && 
#endif /* FEATURE_ANIMATED_AAC */
       ((midi_cmd->num_words + audfmt_global_cmd.num_words) <=
        AUDMAIN_CMD_BUF_MAX_WORDS)) {
      /* Issue commands */
      memcpy( (uint8 *) &midi_cmd->command_buf[midi_cmd->num_words],
              (uint8 *) audfmt_global_cmd.command_buf,
              audfmt_global_cmd.num_words * 2
            );
      midi_cmd->num_words    += audfmt_global_cmd.num_words;
      midi_cmd->num_commands += audfmt_global_cmd.num_commands;
    }

    audfmt_global_cmd.num_words    = 0;
    audfmt_global_cmd.num_commands = 0;
  }
} /* end audfmt_get_final_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_tune_cmd

DESCRIPTION
  This function enters a master tune command into the command buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_set_tune_cmd (
  uint8             tune_factor,
  audfmt_parse_type *midi_ctl
) {
  audfmt_status_type ret_val;
  uint8              buf[AUDFMT_COMMAND_SIZE_4];

  buf[0]  = MIDI_CONTROL_CHANGE;
  buf[1]  = MIDI_ALL_CHANNELS;
  buf[2]  = MIDI_RP_FINE_COMMAND;
  buf[3]  = MIDI_RP_TUNE_PARM1;
  ret_val = audfmt_add_global_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    buf[2]  = MIDI_RP_COARSE_COMMAND;
    buf[3]  = MIDI_RP_TUNE_PARM2;
    ret_val = audfmt_add_global_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

    if(ret_val == AUDFMT_STATUS_SUCCESS) {
      buf[2]  = MIDI_RP_TUNE_PARM3;
      buf[3]  = tune_factor;
      ret_val = audfmt_add_global_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

      if(ret_val == AUDFMT_STATUS_SUCCESS) {
        buf[2]  = MIDI_RP_FINE_COMMAND;
        buf[3]  = MIDI_RP_RESET_PARM;
        ret_val = audfmt_add_global_command(midi_ctl, buf,
                                            AUDFMT_COMMAND_SIZE_4);
      }
    }
  }

  return(ret_val);
} /* end audfmt_set_tune_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_pan_cmd

DESCRIPTION
  This function enters a master pan command into the command buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_set_pan_cmd (
  uint8             pan_factor,
  audfmt_parse_type *midi_ctl
) {
  uint8 buf[8];

  buf[0] = 0xf0;
  buf[1] = 0x7f;
  buf[2] = 0x7f;
  buf[3] = 0x04;
  buf[4] = 0x02;
  buf[5] = 0x00;
  buf[6] = pan_factor;
  buf[7] = 0xf7;

  return(audfmt_add_global_command(midi_ctl, buf, 8));
} /* end audfmt_set_pan_cmd */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION audfmt_reset_hq_bank

DESCRIPTION
  This function resets banks to HQ for midi and smaf files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_reset_hq_bank (
  audfmt_parse_type *midi_ctl
) {
  uint8 buf[AUDFMT_COMMAND_SIZE_4];

  buf[0] = MIDI_CONTROL_CHANGE;
  buf[1] = MIDI_ALL_CHANNELS;
  buf[2] = MIDI_BANK_SEL_COMMAND;
  buf[3] = 0;
  audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

  if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_MA2) {
    buf[0] = MIDI_CONTROL_CHANGE;
    buf[1] = 9;
    buf[2] = MIDI_BANK_SEL_COMMAND;
    buf[3] = CMID_BANK_NORMAL;
    audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_force_hq_bank

DESCRIPTION
  This function forces all banks to HQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_force_hq_bank (
  audfmt_parse_type *midi_ctl
) {
  uint8 buf[AUDFMT_COMMAND_SIZE_4];

  buf[0] = MIDI_CONTROL_CHANGE;
  buf[1] = MIDI_ALL_CHANNELS;
  buf[2] = MIDI_BANK_SEL_COMMAND;
  buf[3] = CMID_BANK_NORMAL_HQ;
  audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

  buf[0] = MIDI_CONTROL_CHANGE;
  buf[1] = 9;
  buf[2] = MIDI_BANK_SEL_COMMAND;
  buf[3] = CMID_BANK_DRUM_HQ;
  audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_reset_cmid_bank

DESCRIPTION
  This function resets banks to legacy for c-midi files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_reset_cmid_bank (
  audfmt_parse_type *midi_ctl
) {
  uint8 buf[AUDFMT_COMMAND_SIZE_4];

  buf[0] = MIDI_CONTROL_CHANGE;
  buf[1] = MIDI_ALL_CHANNELS;
  buf[2] = MIDI_BANK_SEL_COMMAND;
  buf[3] = CMID_BANK_0;
  audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
} /* end audfmt_reset_cmid_bank */
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_tune

DESCRIPTION
  This function sets the playback tune factor for all sequences.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_set_tune (
  uint8 *tune_factor
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  audfmt_parse_type   *midi_ctl;
  uint32              index = AUDFMT_NUM_SEQUENCES;

  if(*tune_factor < AUDFMT_MIN_TUNE_FACTOR) {
    *tune_factor = AUDFMT_MIN_TUNE_FACTOR;
  } else if(*tune_factor > AUDFMT_MAX_TUNE_FACTOR) {
    *tune_factor = AUDFMT_MAX_TUNE_FACTOR;
  }

  midi_ctl = audfmt_parse_ctl;

  while(index != 0) {
    if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
       !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                  AUDFMT_MASK_GET_SPEC))) {
      if(audfmt_set_tune_cmd(*tune_factor, midi_ctl) == AUDFMT_STATUS_SUCCESS)
      {
        midi_ctl->tune_factor = *tune_factor;
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_TUNE, *tune_factor,
                          NULL);
        ret_val = AUDMAIN_STATUS_SUCCESS;
      }
    }
    index--;
    midi_ctl++;
  }
  return(ret_val);
} /* end audfmt_set_tune */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_pan

DESCRIPTION
  This function sets the playback pan factor for all sequences.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_set_pan (
  uint8 *pan_factor
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  audfmt_parse_type   *midi_ctl;
  uint32              index = AUDFMT_NUM_SEQUENCES;

  if(*pan_factor > AUDFMT_MAX_PAN_FACTOR) {
    *pan_factor = AUDFMT_MAX_PAN_FACTOR;
  }

  midi_ctl = audfmt_parse_ctl;

  while(index != 0) {
    if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
       !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                  AUDFMT_MASK_GET_SPEC))) {
      if(audfmt_set_pan_cmd(*pan_factor, midi_ctl) == AUDFMT_STATUS_SUCCESS)
      {
        midi_ctl->pan_factor = *pan_factor;
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_PAN, *pan_factor,
                          NULL);
        ret_val = AUDMAIN_STATUS_SUCCESS;
      }
    }
    index--;
    midi_ctl++;
  }
  return(ret_val);
} /* end audfmt_set_pan */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_tempo_adjust_up

DESCRIPTION
  This function adjusts the time by the user-controlled tempo factor
  (factored up). 

  Note: time_ms must be less than AUDFMT_MAX_32BIT/AUDFMT_MAX_TEMPO_MULT 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
static uint32 audfmt_tempo_adjust_up (
  uint32 tempo_factor,
  uint32 time_ms
) {
  if(tempo_factor == TEMPO_MULT_FACTOR) {
    return(time_ms);
  } else {
    return( time_ms * tempo_factor / TEMPO_MULT_FACTOR );
  }
} /* end audfmt_tempo_adjust_up */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_tempo_adjust_down

DESCRIPTION
  This function adjusts the time by the user-controlled tempo factor 
  (factored down).

  Note: time_ms must be less than AUDFMT_MAX_32BIT/TEMPO_MULT_FACTOR 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
static uint32 audfmt_tempo_adjust_down (
  uint32 tempo_factor,
  uint32 time_ms
) {
  if(tempo_factor == TEMPO_MULT_FACTOR) {
    return(time_ms);
  } else {
    return( time_ms * TEMPO_MULT_FACTOR / tempo_factor );
  }
} /* end audfmt_tempo_adjust_down */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_data_cb_req

DESCRIPTION
  This function responds accordingly to an incoming data buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audfmt_get_data_cb_req (
  uint8 **data, 
  uint8 *buf, 
  uint32 length, 
  uint32 pos 
  /*lint -esym(715,pos) Tell Lint we know pos parameter is not used */
) {
  audfmt_track_type *track_ptr;
  audfmt_parse_type *midi_ctl;
  boolean           ret_val = FALSE;
  uint32            index, counter;

  midi_ctl = audfmt_parse_ctl;
  index    = AUDFMT_NUM_SEQUENCES;

  while((index--) && (!ret_val)) {
    track_ptr = midi_ctl->tracks;

    /* Scan tracks to find match */
    counter = AUDFMT_MAX_MIDI_TRACKS;
    while((counter != 0) && ((data != &track_ptr->track_buf) &&
                             (data != &track_ptr->track_buf_reserve))) {
      track_ptr++;
      counter--;
    }

    if(counter != 0) {
      /* Found buffer that this request was from */
      ret_val = TRUE;
      if(midi_ctl->midi_status & AUDFMT_MASK_ENABLED) {
        midi_ctl->num_read--;
        
        /* if buffer is NULL */
        if (buf == NULL) {
          MSG_ERROR("ERROR:Buffer recieved is NULL(%d) ABORT operation", buf,0, 0);
          midi_ctl->midi_status |= AUDFMT_MASK_ERROR;
          midi_ctl->error_status = SND_ABORT_DATA_ACCESS_ERR;
          if(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME) {
            track_ptr->track_status &= ~AUDFMT_TRACK_MASK_BUFF_DELAY;
            midi_ctl->midi_status   &= ~(AUDFMT_MASK_PENDING_BUF);
            midi_ctl->midi_status2  &= ~(AUDFMT_MASK_NEXT_BUFFER);
            if(!preparse_sent) {
              preparse_sent = TRUE;
              audmain_do_preparse(midi_ctl->handle);
            }
          } else {
            return(TRUE);
          }
        }

        if(track_ptr->skip_buf_count != 0) {
          /* Free buffer because we are supposed to skip */
          track_ptr->skip_buf_count--;
          audfmt_do_play_cb(track_ptr->audfmt_handle, SND_CMX_AF_FREE_BUF, 0, 
                            (void *) buf);
        } else if((pos == track_ptr->file_location) &&
                  (track_ptr->audfmt_handle != NULL)) {
            /* Sanity check for file position */
            if(data == &track_ptr->track_buf) {
              track_ptr->buf_size           = length;
              track_ptr->track_buf          = buf;
            } else {
              track_ptr->buf_reserve_size   = length;
              track_ptr->track_buf_reserve  = buf;
            }
            track_ptr->file_location += length;
            track_ptr->track_status  &= ~AUDFMT_TRACK_MASK_PENDING_BUF;

            if((track_ptr->track_status & AUDFMT_TRACK_MASK_BUFF_DELAY) &&
               (length == 0)) {
              /* If an empty buffer is returned, set flags to indicate next
              ** buffer should fire a preparse command
              */
              track_ptr->track_status &= ~AUDFMT_TRACK_MASK_BUFF_DELAY;
              midi_ctl->midi_status2  |= AUDFMT_MASK_NEXT_BUFFER;
            } else if((track_ptr->track_status & AUDFMT_TRACK_MASK_BUFF_DELAY)
                      || (midi_ctl->midi_status2 & AUDFMT_MASK_NEXT_BUFFER)) {
              /* Playback is waiting for this data buffer, send
              ** preparse command 
              */
              track_ptr->track_status &= ~AUDFMT_TRACK_MASK_BUFF_DELAY;
              midi_ctl->midi_status   &= ~(AUDFMT_MASK_PENDING_BUF);
              midi_ctl->midi_status2  &= ~(AUDFMT_MASK_NEXT_BUFFER);
              if(!preparse_sent) {
                preparse_sent = TRUE;
                audmain_do_preparse(midi_ctl->handle);
              }
            }
          } else {
            audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0, 
                            (void *) buf);
        }
      }
    }
    midi_ctl++;
  }

  return(FALSE);
} /* end audfmt_get_data_cb_req */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_check_seek_resume

DESCRIPTION
  This function checks conditions for resuming playback after seek.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_check_seek_resume (
  audfmt_parse_type *midi_ctl
) {
  audfmt_parse_type   *sequence;
  uint32              index = AUDFMT_NUM_SEQUENCES;
  boolean             do_resume = TRUE;

  if(midi_ctl->midi_status & AUDFMT_MASK_WAIT_RESUME) {
    /* If waiting for a synchronous resume then check if all sequences
    ** are ready to resume
    */
    if(!(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE)) {
      /* If done seeking, set resume pending flag in this sequence */
      midi_ctl->midi_status |= AUDFMT_MASK_RESUME_PENDING;
    }

    sequence = audfmt_parse_ctl;

    /* Verify that all sequences that are waiting to resume are all
    ** done seeking
    */
    while(index != 0) {
      if((sequence->midi_status & AUDFMT_MASK_ENABLED) &&
         (sequence->midi_status & AUDFMT_MASK_WAIT_RESUME)) {
        if(!((sequence->midi_status & AUDFMT_MASK_RESUME_PENDING) ||
             (sequence->midi_status & AUDFMT_MASK_ERROR) ||
             (sequence->midi_status & AUDFMT_MASK_DONE_PENDING)) ||
           (sequence->midi_status & AUDFMT_MASK_PARSING_HEADER)) {
          do_resume = FALSE;
          break;
        }
      }
      index--;
      sequence++;
    }

    if(do_resume) {
      index    = AUDFMT_NUM_SEQUENCES;
      sequence = audfmt_parse_ctl;
      while(index != 0) {
        if((sequence->midi_status & AUDFMT_MASK_ENABLED) &&
           (sequence->midi_status & AUDFMT_MASK_RESUME_PENDING)) {
          sequence->midi_status &= ~(AUDFMT_MASK_WAIT_RESUME |
                                     AUDFMT_MASK_RESUME_PENDING);
          if(!(sequence->midi_status & (AUDFMT_MASK_PAUSED       |
                                        AUDFMT_MASK_SEEK_PAUSED  |
                                        AUDFMT_MASK_ERROR)) &&
             (!(sequence->midi_status & AUDFMT_MASK_DONE_PENDING) ||
              (sequence->delta_tick != 0))) {
            audfmt_do_pause_cmd(sequence, FALSE);
            sequence->midi_status |= AUDFMT_MASK_INIT_DONE;
            audfmt_do_play_cb(sequence->handle, SND_CMX_AF_RESUME,
                              sequence->play_time_ms, NULL);
          }
        }
        index--;
        sequence++;
      }
    }
  } else if(!(midi_ctl->midi_status & (AUDFMT_MASK_PAUSED |
                                       AUDFMT_MASK_SEEK_PAUSED))) {
    audfmt_do_pause_cmd(midi_ctl, FALSE);
    /* If sequence is not paused, then send resume status to client */
    audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_RESUME,
                      midi_ctl->play_time_ms, NULL);
  }
} /* end audfmt_check_seek_resume */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_fforward_single

DESCRIPTION
  This function moves the playback of a single sequence forward ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_fforward_single (
  audfmt_parse_type *midi_ctl,
  uint32            ms_forward,
  boolean           do_fforward
) {
  audfmt_status_type ret_val = AUDFMT_STATUS_FAILURE;
  uint32             tmp_seek_ms;

  if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
     !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {
    if(do_fforward) {
      midi_ctl->midi_status &= ~AUDFMT_MASK_SEEK_PAUSED;

      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FFORWARD,
                        midi_ctl->play_time_ms, NULL);
    }

    tmp_seek_ms = AUDFMT_MAX_32BIT - midi_ctl->seek_ms;

    if(ms_forward > tmp_seek_ms) {
      MSG_HIGH("Seek time overflow", 0, 0, 0);
      midi_ctl->seek_ms = AUDFMT_MAX_32BIT;
    } else {
      midi_ctl->seek_ms += ms_forward;
    }

    if(!(midi_ctl->midi_status & AUDFMT_MASK_PARSING_HEADER)) {
      if(!(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE)) {
        /* Update the playtime up to this point with old tempo factor */
        midi_ctl->play_time_ms += audfmt_tempo_adjust_up(
                                                midi_ctl->tempo_factor,
                                                midi_ctl->accumulated_ms);
        midi_ctl->accumulated_ms = 0;

        switch(midi_ctl->type) {
          case AUDFMT_MIDI_MIDI_TYPE:
            midi_ctl->calc_factor  = midi_ctl->ppqn_10 * TEMPO_MULT_FACTOR;
            midi_ctl->calc_tick_of = AUDFMT_MAX_32BIT / midi_ctl->calc_factor;
            break;

#ifdef FEATURE_WEBAUDIO
          case AUDFMT_MIDI_CMID_TYPE:
          case AUDFMT_MIDI_MELO_TYPE:
            audfmt_reset_adec_channels(midi_ctl, FALSE);
            midi_ctl->calc_factor  = AUDFMT_MS_PER_MINUTE;
            midi_ctl->calc_ms_of   = AUDFMT_MAX_32BIT / AUDFMT_MS_PER_MINUTE;
            break;

#ifdef FEATURE_SMAF
          case AUDFMT_MIDI_SMAF_TYPE:
            audfmt_reset_adec_channels(midi_ctl, FALSE);
            midi_ctl->calc_factor  = 0;
            break;
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */
        }

        midi_ctl->midi_status |= AUDFMT_MASK_SEEK_ACTIVE;
        midi_ctl->midi_status &= ~AUDFMT_MASK_RESUME_PENDING;

        audfmt_do_pause_cmd(midi_ctl, TRUE);
      }

      audfmt_calc_tick(midi_ctl, midi_ctl->seek_ms, &midi_ctl->seek_tick);

      if(midi_ctl->midi_status & AUDFMT_MASK_PARSE_DONE) {
        midi_ctl->midi_status &= ~AUDFMT_MASK_PARSE_DONE;
        midi_ctl->midi_status |= AUDFMT_MASK_PARSE_PENDING;
        if(!preparse_sent) {
          preparse_sent = TRUE;
          audmain_do_preparse(midi_ctl->handle);
        }
      }
    }

    ret_val = AUDFMT_STATUS_SUCCESS;
  }
  return(ret_val);
} /* end audfmt_fforward_single */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_header_do_failure

DESCRIPTION
  This function performs failure processing for header parsing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_header_do_failure (
  audfmt_parse_type *midi_ctl
) {
  audfmt_track_type *track_ptr;
  boolean           check_reset;
  uint32            counter;

  audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FAILURE, 0, NULL);

  track_ptr = midi_ctl->tracks;
  for(counter = 0; counter < AUDFMT_MAX_MIDI_TRACKS; counter++) {
    if(track_ptr->track_buf != NULL) {
      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0, 
                        (void *) track_ptr->track_buf);
      track_ptr->track_buf = NULL;
    }
    if(track_ptr->track_buf_reserve != NULL) {
      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0, 
                        (void *) track_ptr->track_buf_reserve);
      track_ptr->track_buf_reserve = NULL;
    }
    track_ptr++;
  }

  memset((uint8 *) (midi_ctl->tracks), 0,
         sizeof(audfmt_track_type) * AUDFMT_MAX_MIDI_TRACKS);
  memset((uint8 *)(&midi_ctl->cmd_buf), 0, sizeof(audfmt_cmd_buf_type));

  check_reset = ((midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                           AUDFMT_MASK_GET_SPEC)) != 0);

  midi_ctl->midi_status  = 0;
  midi_ctl->handle       = NULL;
#ifdef FEATURE_WEBAUDIO
  audfmt_cb_store_reset(midi_ctl->cb_store);
#endif /* FEATURE_WEBAUDIO */

  if(check_reset) {
    audmain_audfmt_check_reset();
  }
} /* end audfmt_header_do_failure */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_send_spec

DESCRIPTION
  This function sends back specifications of this file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_send_spec (
  audfmt_parse_type *midi_ctl
) {
  snd_cmx_af_spec_type audfmt_spec;

  if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
#ifdef FEATURE_WEBAUDIO
    if(midi_ctl->type == AUDFMT_MIDI_MELO_TYPE) {
      audfmt_spec.file_type          = SND_CMX_AF_FILE_MFI;
      audfmt_spec.midi_spec.supported_ops = SND_SUPPORTED_OPS_MASK_SEEK |
                                            SND_SUPPORTED_OPS_MASK_GETTIME;
    } else if(midi_ctl->type == AUDFMT_MIDI_CMID_TYPE) {
      audfmt_spec.file_type          = SND_CMX_AF_FILE_CMID;
      audfmt_spec.cmid_spec.contents = midi_ctl->contents;
      audfmt_spec.cmid_spec.supported_ops = SND_SUPPORTED_OPS_MASK_SEEK |
                                            SND_SUPPORTED_OPS_MASK_GETTIME;
      switch(midi_ctl->code) {
        case CMID_CODE_ANSI:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_ANSI;
          break;
        case CMID_CODE_ISO8859_1:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_ISO8859_1;
          break;
        case CMID_CODE_ISO8859_2:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_ISO8859_2;
          break;
        case CMID_CODE_ISO8859_3:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_ISO8859_3;
          break;
        case CMID_CODE_ISO8859_4:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_ISO8859_4;
          break;
        case CMID_CODE_ISO8859_5:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_ISO8859_5;
          break;
        case CMID_CODE_ISO8859_6:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_ISO8859_6;
          break;
        case CMID_CODE_ISO8859_7:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_ISO8859_7;
          break;
        case CMID_CODE_ISO8859_8:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_ISO8859_8;
          break;
        case CMID_CODE_ISO8859_9:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_ISO8859_9;
          break;
        case CMID_CODE_ISO8859_10:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_ISO8859_10;
          break;
        case CMID_CODE_SHIFTJIS:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_SHIFTJIS;
          break;
        case CMID_CODE_HANGUL:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_HANGUL;
          break;
        case CMID_CODE_CHINESE_SIMP:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_CHINESE_SIMP;
          break;
        case CMID_CODE_CHINESE_TRAD:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_CHINESE_TRAD;
          break;
        case CMID_CODE_HINDI:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_HINDI;
          break;
        default:
          audfmt_spec.cmid_spec.char_code = SND_CMX_AF_CS_UNKNOWN;
          break;
      }
    } else
#ifdef FEATURE_SMAF
    if(midi_ctl->type == AUDFMT_MIDI_SMAF_TYPE) {
      audfmt_spec.file_type = SND_CMX_AF_FILE_SMAF;
      audfmt_spec.smaf_spec.supported_ops = SND_SUPPORTED_OPS_MASK_SEEK |
                                            SND_SUPPORTED_OPS_MASK_GETTIME;
      audfmt_spec.smaf_spec.code_type   = midi_ctl->smaf_audio_spec.code_type;
      memcpy( audfmt_spec.smaf_spec.artist, midi_ctl->smaf_audio_spec.artist, 30);
      memcpy( audfmt_spec.smaf_spec.title, midi_ctl->smaf_audio_spec.title, 30);
      memcpy( audfmt_spec.smaf_spec.copyright, midi_ctl->smaf_audio_spec.copyright, 30);

      /* Need to add more */
    } else
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */
    {
      audfmt_spec.file_type = SND_CMX_AF_FILE_MIDI;
      audfmt_spec.midi_spec.supported_ops = SND_SUPPORTED_OPS_MASK_SEEK |
                                            SND_SUPPORTED_OPS_MASK_GETTIME;
    }
    audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_AUDIO_SPEC, 0, &audfmt_spec);
  }
} /* end audfmt_send_spec */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_finish_header

DESCRIPTION
  This function ends header parsing and kicks off playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_finish_header (
  audfmt_parse_type *midi_ctl
) {
#ifdef FEATURE_WEBAUDIO
  audfmt_parse_type       *midi_ptr;
  uint32                  counter;
  audmain_dls_config_type dls_config;
#ifdef FEATURE_SMAF
  audfmt_track_type       *track_ptr;
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */
  boolean           initial_play;

  if(midi_ctl->midi_status & AUDFMT_MASK_GET_SPEC) {
    audfmt_send_spec(midi_ctl);
    midi_ctl->midi_status = 0;
  } else {
    initial_play = ((midi_ctl->midi_status & AUDFMT_MASK_RESTARTING) == 0);
    midi_ctl->midi_status &= ~(AUDFMT_MASK_PARSING_HEADER |
                               AUDFMT_MASK_RESTARTING);
    midi_ctl->midi_status |= AUDFMT_MASK_PARSE_PENDING;

    if(midi_ctl->type == AUDFMT_MIDI_MIDI_TYPE) {
      midi_ctl->midi_status |= AUDFMT_MASK_SILENCE_SKIP;
    }
#ifdef FEATURE_SMAF
    else if(midi_ctl->type == AUDFMT_MIDI_SMAF_TYPE) {
      if(midi_ctl->midi_status & AUDFMT_MASK_PRO_FOUND) {
        if(midi_ctl->smaf_start_tick != 0) {
          midi_ctl->midi_status |= AUDFMT_MASK_INITIAL_SKIP;
        }
      } else if(midi_ctl->midi_status & AUDFMT_MASK_MSPI_FOUND) {
        track_ptr              = midi_ctl->tracks;
        midi_ctl->midi_status |= AUDFMT_MASK_OFFSET_SKIP;
        for(counter = 0; counter < midi_ctl->num_tracks; counter++) {
          if(track_ptr->audfmt_handle != NULL) {
            if(track_ptr->start_offset != 0) {
              track_ptr->track_status |= AUDFMT_TRACK_MASK_OFFSET_SKIP;
            } else {
              midi_ctl->midi_status &= ~AUDFMT_MASK_OFFSET_SKIP;
            }
          }
          track_ptr++;
        }
      } else {
        midi_ctl->midi_status |= AUDFMT_MASK_SILENCE_SKIP;
      }
    }
#endif /* FEATURE_SMAF */

    if(initial_play) {
      audfmt_send_spec(midi_ctl);
      if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_AUDIO_CMX_5P0
        dls_config.hp_inst_def  = (uint32) &audfmt_dls_struct.prog_rec;
        dls_config.hp_drum_def  = (uint32) &audfmt_dls_struct.prog_rec;
#else
        dls_config.hp_inst_def  = (uint32) &audfmt_dls_struct.prog_def;
        dls_config.hp_drum_def  = (uint32) &audfmt_dls_struct.prog_def;
#endif /* FEATURE_AUDIO_CMX_5P0 */
        dls_config.hp_wave_info = (uint32) &audfmt_dls_struct.wave_info;
#ifdef FEATURE_DLS
#ifdef FEATURE_AUDIO_CMX_5P0
		dls_config.hq_inst_def  = (uint32) &audfmt_dls_table.inst_rec;
		dls_config.hq_drum_def  = (uint32) &audfmt_dls_table.drum_rec;
#else
        dls_config.hq_inst_def  = (uint32) &audfmt_dls_table.inst_addr;
        dls_config.hq_drum_def  = (uint32) &audfmt_dls_table.drum_addr;
#endif /* FEATURE_AUDIO_CMX_5P0 */
        dls_config.hq_wave_info = (uint32) &audfmt_dls_table.wave_info;
        dls_config.hq_inst_num  = 128;
        dls_config.hq_drum_num  = 128;
#else
        dls_config.hq_inst_def  = 0;
        dls_config.hq_drum_def  = 0;
        dls_config.hq_wave_info = 0;
        dls_config.hq_inst_num  = 0;
        dls_config.hq_drum_num  = 0;
#endif /* FEATURE_DLS */
        audmain_set_dls_config(&dls_config);

        if(midi_ctl->midi_status & AUDFMT_MASK_MULTISEQ_MODE) {
#ifdef FEATURE_AUDIO_CMX_5P0
          audmain_set_hybrid_mode(AUDMAIN_AUDFMT_POLY_MODE_7);
#else
          audmain_set_hybrid_mode(audfmt_poly_mode);
#endif /* FEATURE_AUDIO_CMX_5P0 */
        } else
#endif /* FEATURE_WEBAUDIO */
		{
#ifdef FEATURE_AUDIO_CMX_5P0
          audmain_set_hybrid_mode(AUDMAIN_AUDFMT_POLY_MODE_7);
#else
          audmain_set_hybrid_mode(midi_ctl->hybrid_mode);
#endif /* FEATURE_AUDIO_CMX_5P0 */
		}
      }
    }

    if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
      /* Process any seek commands now since all necessary parameters
      ** for this sequence are known.
      */
      if(midi_ctl->seek_ms != 0) {
        audfmt_fforward_single (midi_ctl, 0, FALSE);
      }
    }
  
#ifdef FEATURE_WEBAUDIO
    if(audfmt_cb_store.used) {
      midi_ptr = audfmt_parse_ctl;
      for(counter = 0; counter < AUDFMT_NUM_SEQUENCES; counter++) {
        if(midi_ptr->midi_status & AUDFMT_MASK_ENABLED) {
          midi_ptr->midi_status |= AUDFMT_MASK_CB_STORE_DONE;
        }
        midi_ptr++;
      }
    }
#endif /* FEATURE_WEBAUDIO */

    if(midi_ctl->midi_status & AUDFMT_MASK_WAIT_RESUME) {
      audfmt_check_seek_resume(midi_ctl);
    }

    if(!preparse_sent) {
      preparse_sent = TRUE;
      audmain_do_preparse(midi_ctl->handle);
    }
  }
} /* end audfmt_finish_header */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_DLS
/*===========================================================================

FUNCTION cmid_dls_callback

DESCRIPTION
  This function is used as the callback function for DLS loading operation
  for c-MIDI files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmid_dls_callback (
  snd_af_data_cmd_enum  command,
  void                  *audfmt_file,
  uint32                num,
  void                  *data
) {
  audfmt_parse_type  *midi_ctl = (audfmt_parse_type *) audfmt_file;
  snd_af_access_type *handle   = midi_ctl->handle;

  if((command == SND_CMX_AF_DLS_DONE) || (command == SND_CMX_AF_DLS_FAILURE)) {
    /* Get next buffer of data */
    audmain_get_header_data(midi_ctl->handle,
                            midi_ctl->tracks[0].current_location,
                            &(midi_ctl->tracks[0].track_buf));
  } else {
    handle->data_func(command, handle->audfmt_ptr, num, data);
  }
} /* end cmid_dls_callback */
#endif /* FEATURE_DLS */

/* <EJECT> */
/*===========================================================================

FUNCTION do_cmid_chunk

DESCRIPTION
  This function parses the chunks in c-midi file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void do_cmid_chunk (
  audfmt_parse_type *midi_ctl
) {
  audfmt_track_type *track_ptr;
  uint32            chunk_length, tlong;
  uint8             *buffer;
  boolean           track_found = FALSE;

  snd_cmx_af_svg_data_type svg_data;
  snd_cmx_af_saf_type      saf_data;
  snd_cmx_af_buf_rtn_type  buf_rtn = {NULL, 0, NULL, 0, 0};

  track_ptr = midi_ctl->tracks;

  /* Make sure there is enough data to figure out chunk ID and size */
  while((!track_found) && (track_ptr->current_location < midi_ctl->file_end) &&
        (track_ptr->index < track_ptr->buf_size) &&
        ((track_ptr->buf_size - track_ptr->index) >= 8)) {

    buffer       = &(track_ptr->track_buf[track_ptr->index]);
    chunk_length = buffer[4]<<24 | buffer[5]<<16 | buffer[6]<<8 | buffer[7];

    if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                  AUDFMT_MASK_GET_SPEC))) {
#ifdef FEATURE_DLS
      if(memcmp(buffer, "DLS ", 4) == 0) {
        tlong = track_ptr->current_location + 8;
        track_ptr->current_location += chunk_length + 8;

        if(track_ptr->current_location < midi_ctl->file_end) {
          audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0, 
                            (void *) track_ptr->track_buf);
          track_ptr->track_buf = NULL;

          midi_ctl->dls_handle.audfmt_ptr  = midi_ctl;
          midi_ctl->dls_handle.data_func   = cmid_dls_callback;
          midi_ctl->midi_status           |= AUDFMT_MASK_DLS_ENABLED;

          audfmt_load_offset_dls(&midi_ctl->dls_handle, tlong,
                                                     midi_ctl->sequence);
          return;
        }
      }
#endif /* FEATURE_DLS */
      if((memcmp(buffer, "ANIM", 4) == 0) && (midi_ctl->cb_store != NULL) &&
         (midi_ctl->anim_format_type == 0xff)) {
        if((track_ptr->index + 9) <= (track_ptr->buf_size)) {
          switch(buffer[8] & 0x7) {
            case CMID_EXT_SAF_FORMAT_SAF:
              midi_ctl->anim_format_type = CMID_EXT_SAF_FORMAT_SAF;
              midi_ctl->anim_current_id  = (buffer[8] >> 3) & 0x1f;

              buf_rtn.remain_bytes = chunk_length - 1;
              saf_data.buf_rtn     = &buf_rtn;
              saf_data.objbuf_size = midi_ctl->saf_objbuf_size;
              saf_data.cont        = SND_CMX_AF_SAF_NOT_CONT;
              audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_SAF,
                                track_ptr->current_location + 9, &saf_data);

              midi_ctl->midi_status |= AUDFMT_MASK_SAF_OBJECTS_DONE;
              break;

            case CMID_EXT_SAF_FORMAT_SVG:
              midi_ctl->anim_format_type = CMID_EXT_SAF_FORMAT_SVG;
              midi_ctl->anim_current_id  = (buffer[8] >> 3) & 0x1f;
              svg_data.size              = chunk_length - 1;

              audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_SVG,
                                track_ptr->current_location + 9, &svg_data);
              break;

          }
        } else {
          track_ptr->index = track_ptr->buf_size;
          break;
        }
      }
    }

    if(memcmp(buffer, "trac", 4) == 0) {
      track_found = TRUE;
    } else {
      track_ptr->current_location += chunk_length + 8;
      track_ptr->index            += chunk_length + 8;
    }
  }

  if(track_ptr->current_location < midi_ctl->file_end) {
    /* Must have come to end of data buffer, so free buffer */
    audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0, 
                      (void *) track_ptr->track_buf);
    track_ptr->track_buf = NULL;

    if(track_found) {
      /* If done parsing chunks, then kick off track parsing */
      if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_GET_SPEC))) {
        if(midi_ctl->cmid_wave_type == AUDFMT_CMID_WAVE_ADPCM) {
          (void) audmain_set_adec_mode(AUDMAIN_ADEC_MODE_ADPCM);
#ifdef FEATURE_ANIMATED_AAC
        } else if(midi_ctl->cmid_wave_type == AUDFMT_CMID_WAVE_AAC) {
          (void) audmain_set_adec_mode(AUDMAIN_ADEC_MODE_AAC);
#endif /* FEATURE_ANIMATED_AAC */
        } else {
          (void) audmain_set_adec_mode(AUDMAIN_ADEC_MODE_QCP);
        }
      }
      midi_ctl->midi_status2 &= ~AUDFMT_MASK_PARSING_CHUNKS;
      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_SET_TRACKS,
                        midi_ctl->num_tracks, (void *) &tlong);
    }

    /* Get next buffer of data */
    audmain_get_header_data(midi_ctl->handle, track_ptr->current_location,
                            &(track_ptr->track_buf));
  } else {
    /* Error occurred while parsing c-midi header */
    audfmt_header_do_failure(midi_ctl);
  }
} /* end do_cmid_chunk */

/* <EJECT> */
/*===========================================================================

FUNCTION do_cmid_cnts

DESCRIPTION
  This function parses the CNTS chunk in c-midi file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void do_cmid_cnts (
  audfmt_parse_type *midi_ctl,
  uint8             *buffer,
  uint32            chunk_length
) {
  while(chunk_length != 0) {
    chunk_length--;
    switch(*buffer++) {
      case 'S':
        if(memcmp(buffer, "ONG;", MIN(chunk_length, 4)) == 0) {
          midi_ctl->contents |= SND_CMID_CONTENTS_SONG;
        } else {
          midi_ctl->contents |= SND_CMID_CONTENTS_UNKNOWN;
        }
        break;

      case 'W':
        if(memcmp(buffer, "AVE;", MIN(chunk_length, 4)) == 0) {
          midi_ctl->contents |= SND_CMID_CONTENTS_WAVE;
        } else {
          midi_ctl->contents |= SND_CMID_CONTENTS_UNKNOWN;
        }
        break;

      case 'T':
        if(memcmp(buffer, "EXT;", MIN(chunk_length, 4)) == 0) {
          midi_ctl->contents |= SND_CMID_CONTENTS_TEXT;
        } else {
          midi_ctl->contents |= SND_CMID_CONTENTS_UNKNOWN;
        }
        break;

      case 'P':
        if(memcmp(buffer, "ICT;", MIN(chunk_length, 4)) == 0) {
          midi_ctl->contents |= SND_CMID_CONTENTS_PIC;
        } else {
          midi_ctl->contents |= SND_CMID_CONTENTS_UNKNOWN;
        }
        break;

      case 'A':
        if(memcmp(buffer, "NIM;", MIN(chunk_length, 4)) == 0) {
          midi_ctl->contents |= SND_CMID_CONTENTS_ANIM;
        } else {
          midi_ctl->contents |= SND_CMID_CONTENTS_UNKNOWN;
        }
        break;

      case 'L':
        if(memcmp(buffer, "ED;", MIN(chunk_length, 3)) == 0) {
          midi_ctl->contents |= SND_CMID_CONTENTS_LED;
        } else {
          midi_ctl->contents |= SND_CMID_CONTENTS_UNKNOWN;
        }
        break;

      case 'V':
        if(memcmp(buffer, "IB;", MIN(chunk_length, 3)) == 0) {
          midi_ctl->contents |= SND_CMID_CONTENTS_VIB;
        } else {
          midi_ctl->contents |= SND_CMID_CONTENTS_UNKNOWN;
        }
        break;

      case ';':
        break;

      default:
        midi_ctl->contents |= SND_CMID_CONTENTS_UNKNOWN;
        break;
    }
    while((chunk_length != 0) && (*buffer != ';')) {
      chunk_length--;
      buffer++;
    }
    while((chunk_length != 0) && (*buffer == ';')) {
      chunk_length--;
      buffer++;
    }
  }
} /* end do_cmid_cnts */

/* <EJECT> */
/*===========================================================================

FUNCTION do_cmid_subchunk

DESCRIPTION
  This function parses the header subchunks in c-midi file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void do_cmid_subchunk (
  audfmt_parse_type *midi_ctl
) {
  audfmt_track_type       *track_ptr;
  snd_cmx_af_buf_rtn_type buf_rtn;
  uint32                  chunk_length, tlong;
  uint16                  *exs_ptr;
  uint8                   *buffer;
  boolean                 data_available, chunk_parsed;

  track_ptr = midi_ctl->tracks;

  /* Make sure there is enough data to figure out chunk ID and size */
  while((track_ptr->current_location < track_ptr->track_end) &&
        (track_ptr->index < track_ptr->buf_size) &&
        ((track_ptr->buf_size - track_ptr->index) > 6)) {

    buffer         = &(track_ptr->track_buf[track_ptr->index]);
    chunk_length   = buffer[4]<<8 | buffer[5];
    data_available = (chunk_length < 
                      (track_ptr->buf_size - track_ptr->index - 6));
    chunk_parsed   = TRUE;

    switch(*buffer++) {
      case 'c':
        if(midi_ctl->type == AUDFMT_MIDI_CMID_TYPE) {
          if(memcmp(buffer, "nts", 3) == 0) {
            if(!data_available) {
              chunk_parsed = FALSE;
            } else {
              do_cmid_cnts(midi_ctl, buffer + 5, chunk_length);
              if((midi_ctl->contents & (SND_CMID_CONTENTS_TEXT |
                                        SND_CMID_CONTENTS_PIC  |
                                        SND_CMID_CONTENTS_ANIM |
                                        SND_CMID_CONTENTS_LED)) &&
                 !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                            AUDFMT_MASK_GET_SPEC)) &&
                 !audfmt_cb_store.used) {
                /* If this c-midi file has graphics/text/LED commands,
                ** then assign it as main c-midi file
                */
                audfmt_cb_store_reset(&audfmt_cb_store);
                audfmt_cb_store.used = TRUE;
                midi_ctl->cb_store   = &audfmt_cb_store;
              }
              if((midi_ctl->contents & SND_CMID_CONTENTS_VIB) &&
                 !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                            AUDFMT_MASK_GET_SPEC)) &&
                 !audfmt_cmid_vib_ctl.used) {
                /* If this c-midi file has VIB commands, then add a
                ** VIB controller
                */
                audfmt_cmid_vib_ctl.vib_ctl   = NULL;
                audfmt_cmid_vib_ctl.index     = 0;
                audfmt_cmid_vib_ctl.time_ms   = 0;
                audfmt_cmid_vib_ctl.vib_pend  = 0;
                audfmt_cmid_vib_ctl.used      = TRUE;
                midi_ctl->vib_ctl             = &audfmt_cmid_vib_ctl;
              }
            }
          } else if(memcmp(buffer, "ode", 3) == 0) {
            if(chunk_length != CMID_CODE_BYTES) {
              midi_ctl->midi_status |= AUDFMT_MASK_ERROR;
            } else if(!data_available) {
              chunk_parsed = FALSE;
            } else {
              midi_ctl->code = buffer[5];
            }
          } else if(memcmp(buffer, "rpt", 3) == 0) {
            chunk_length <<= 1;
          } else if(memcmp(buffer, "uep", 3) == 0) {
            if(midi_ctl->midi_status & AUDFMT_MASK_CUEP_PLAY) {
              if(!data_available) {
                chunk_parsed = FALSE;
              } else {
                if (chunk_length == midi_ctl->num_tracks * CMID_CUEP_BYTES) {
                  buffer += 5;
                  for(tlong = 0; tlong < midi_ctl->num_tracks; tlong ++) {
                    midi_ctl->cuep_offset[tlong] =
                                           buffer[0] << 24 | buffer[1] << 16 |
                                           buffer[2] << 8  | buffer[3];
                    buffer += 4;
                    if(midi_ctl->cuep_offset[tlong] != AUDFMT_MAX_32BIT) {
                      midi_ctl->cuep_offset[tlong]++;
                    }
                    midi_ctl->cuep_do_tick[tlong] = FALSE;
                    midi_ctl->cuep_tick[tlong]    = 0;
                  }
                }
              }
            }
          }
        }
        break;

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

      case 'e':
        exs_ptr = NULL;
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
        {
          if((memcmp(buffer, "xsa", 3) == 0) ||
             (memcmp(buffer, "xst", 3) == 0)) {
            exs_ptr = &midi_ctl->exsa_size;
          } else if(memcmp(buffer, "xsb", 3) == 0) {
            exs_ptr = &midi_ctl->exsb_size;
          } else if(memcmp(buffer, "xsc", 3) == 0) {
            exs_ptr = &midi_ctl->exsc_size;
          } else if(memcmp(buffer, "xsn", 3) == 0) {
            exs_ptr = &midi_ctl->exsn_size;
          }
        }

        if(exs_ptr != NULL) {
          if(chunk_length != CMID_EXST_BYTES) {
            midi_ctl->midi_status |= AUDFMT_MASK_ERROR;
          } else if(!data_available) {
            chunk_parsed = FALSE;
          } else {
            *exs_ptr = buffer[5] << 8 | buffer[6];
            if(midi_ctl->type == AUDFMT_MIDI_MELO_TYPE) {
              *exs_ptr += 1;
            }
          }
        }
        break;

      case 'h':
        if(midi_ctl->type == AUDFMT_MIDI_CMID_TYPE) {
          if(memcmp(buffer, "ist", 3) == 0) {
            if(chunk_length != CMID_HIST_BYTES) {
              midi_ctl->midi_status |= AUDFMT_MASK_ERROR;
            }
          }
        }
        break;

      case 'n':
        if(memcmp(buffer, "ote", 3) == 0) {
          if(chunk_length != CMID_NOTE_BYTES) {
            midi_ctl->midi_status |= AUDFMT_MASK_ERROR;
          } else if(!data_available) {
            chunk_parsed = FALSE;
          } else {
            midi_ctl->note_size = buffer[5] << 8 | buffer[6];
          }
        }
        break;

      case 'p':
        if(midi_ctl->type == AUDFMT_MIDI_CMID_TYPE) {
          if(memcmp(buffer, "cpi", 3) == 0) {
            if(chunk_length != CMID_PCPI_BYTES) {
              midi_ctl->midi_status |= AUDFMT_MASK_ERROR;
            } else if(!data_available) {
              chunk_parsed = FALSE;
            } else {
              if(buffer[5] == 1) {
                midi_ctl->midi_status2 |= AUDFMT_MASK_PCPI_ENABLED;
              }
            }
          } else if(memcmp(buffer, "oly", 3) == 0) {
            if(chunk_length != CMID_POLY_BYTES) {
              midi_ctl->midi_status |= AUDFMT_MASK_ERROR;
            } else if(!data_available) {
              chunk_parsed = FALSE;
            } else {
              if(midi_ctl->version <= 500) {
                midi_ctl->hybrid_mode = CMID_POLY_MAX_HP;
              } else if(buffer[5] <= CMID_POLY_MAX) {
                midi_ctl->hybrid_mode = buffer[5];
              }
            }
          }
        }
        break;

      case 's':
        if(memcmp(buffer, "orc", 3) == 0) {
          if(((midi_ctl->type == AUDFMT_MIDI_MELO_TYPE) &&
              (chunk_length != CMID_SORC_BYTES1)) ||
             ((chunk_length != CMID_SORC_BYTES1) &&
              (chunk_length != CMID_SORC_BYTES2))) {
            midi_ctl->midi_status |= AUDFMT_MASK_ERROR;
          }
        } else if(midi_ctl->type == AUDFMT_MIDI_CMID_TYPE) {
          if(memcmp(buffer, "afo", 3) == 0) {
            if(chunk_length != CMID_SAFO_BYTES) {
              midi_ctl->midi_status |= AUDFMT_MASK_ERROR;
            } else {
              if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                            AUDFMT_MASK_GET_SPEC))) {
                midi_ctl->saf_objbuf_size = ((buffer[5] << 24) | 
                                             (buffer[6] << 16) | 
                                             (buffer[7] << 8)  | 
                                             (buffer[8]));
              }
            }
          }
        }
        break;

      case 't':
        if(memcmp(buffer, "itl", 3) == 0) {
          if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                        AUDFMT_MASK_GET_SPEC))) {
            if(data_available) {
              buf_rtn.length1      = chunk_length;
              buf_rtn.buf1         = &buffer[5];
              buf_rtn.remain_bytes = 0;
            } else {
              buf_rtn.length1      = 0;
              buf_rtn.buf1         = NULL;
              buf_rtn.remain_bytes = chunk_length;
            }
            buf_rtn.length2        = 0;
            buf_rtn.buf2           = NULL;

            audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_TITLE,
                              track_ptr->current_location + 6, &buf_rtn);
          }
        }
        break;

      case 'v':
        if(memcmp(buffer, "ers", 3) == 0) {
          if(chunk_length != CMID_VERS_BYTES) {
            midi_ctl->midi_status |= AUDFMT_MASK_ERROR;
          } else if(!data_available) {
            chunk_parsed = FALSE;
          } else {
            midi_ctl->version = ((buffer[5] - '0') * 1000) + 
                                ((buffer[6] - '0') * 100) +
                                ((buffer[7] - '0') * 10) + 
                                (buffer[8] - '0');
            if((midi_ctl->type == AUDFMT_MIDI_MELO_TYPE) &&
               (midi_ctl->version > 399)) {
              midi_ctl->midi_status |= AUDFMT_MASK_ERROR;
            }
          }
        }
        break;

      case 'w':
        if(midi_ctl->type == AUDFMT_MIDI_CMID_TYPE) {
          if(memcmp(buffer, "ave", 3) == 0) {
            if(chunk_length != CMID_WAVE_BYTES) {
              midi_ctl->midi_status |= AUDFMT_MASK_ERROR;
            } else if(!data_available) {
              chunk_parsed = FALSE;
            } else {
              if(buffer[5] == CMID_WAVE_MODE_ADPCM) {
                midi_ctl->cmid_wave_type = AUDFMT_CMID_WAVE_ADPCM;
              }
              if(buffer[5] == CMID_WAVE_MODE_AAC) {
                midi_ctl->cmid_wave_type = AUDFMT_CMID_WAVE_AAC;
              }
            }
          }
        }
        break;

    }

    if((midi_ctl->midi_status & AUDFMT_MASK_ERROR) || !chunk_parsed) {
      break;
    } else {
      track_ptr->current_location += chunk_length + 6;
      track_ptr->index            += chunk_length + 6;
    }
  }

  if(!(midi_ctl->midi_status & AUDFMT_MASK_ERROR) &&
     (track_ptr->current_location <= track_ptr->track_end)) {
    if(track_ptr->current_location == track_ptr->track_end) {
      midi_ctl->midi_status2 &= ~AUDFMT_MASK_PARSING_SUBCHUNKS;
      midi_ctl->midi_status2 |= AUDFMT_MASK_PARSING_CHUNKS;
      do_cmid_chunk(midi_ctl);
    } else {
      /* Must have come to end of data buffer, so free buffer */
      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0, 
                        (void *) track_ptr->track_buf);
      track_ptr->track_buf = NULL;

      /* Get next buffer of data */
      audmain_get_header_data(midi_ctl->handle, track_ptr->current_location,
                              &(track_ptr->track_buf));
    }
  } else {
    /* Error occurred while parsing c-midi header */
    audfmt_header_do_failure(midi_ctl);
  }
} /* end do_cmid_subchunk */
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_check_next_track

DESCRIPTION
  This function finishes processing for current track. It will either
  parse the next track or start playback if all tracks are done.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_check_next_track (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track_ptr
) {
  uint32 num_tracks_done;

#ifdef FEATURE_SMAF
  if((midi_ctl->type == AUDFMT_MIDI_SMAF_TYPE) &&
     !(midi_ctl->midi_status & AUDFMT_MASK_RESTARTING)) {
    /* Save starting position of track for restarting SMAF file */
    midi_ctl->track_start[midi_ctl->current_header_track] =
                                             track_ptr->current_location;

    /* Save initial compressed bit information for restarting SMAF file */
    midi_ctl->smaf_start_comp_bits     = track_ptr->compressed_bits;
    midi_ctl->smaf_start_num_comp_bits = track_ptr->num_compressed_bits;
  }
#endif /* FEATURE_SMAF */

  num_tracks_done = midi_ctl->current_header_track + 1;

  /* If more data for this track, get next block of data */
  if(track_ptr->file_location < track_ptr->track_end) {
    audmain_get_header_data( midi_ctl->handle,
                             track_ptr->file_location,
                             &(track_ptr->track_buf_reserve) );
  } else {
    track_ptr->track_buf_reserve = NULL;
    track_ptr->buf_reserve_size  = 0;
    midi_ctl->current_header_track++;
  }

  /* If not the last track, setup next track */
  if(num_tracks_done < midi_ctl->num_tracks) {
#ifdef FEATURE_SMAF
    if(midi_ctl->type == AUDFMT_MIDI_SMAF_TYPE) {
      if(midi_ctl->midi_status & AUDFMT_MASK_RESTARTING) {
        /* If restarting SMAF, used information saved from original
        ** parsing of SMAF header
        */
        audmain_get_header_data( midi_ctl->handle,
                                 midi_ctl->track_start[num_tracks_done],
                                 &((track_ptr + 1)->track_buf));
      } else {
        /* If first time parsing this SMAF header, start of next track
        ** is in current_location parameter of next track
        */
        audmain_get_header_data( midi_ctl->handle,
                                 (track_ptr + 1)->current_location,
                                 &((track_ptr + 1)->track_buf));
      }
    } else
#endif /* FEATURE_SMAF */
    {
      /* Start of next track is at the end of current track */
      audmain_get_header_data( midi_ctl->handle, track_ptr->track_end,
                               &((track_ptr + 1)->track_buf));
    }
  } else if(midi_ctl->current_header_track == midi_ctl->num_tracks) {
    /* Done parsing header, start playback */
    audfmt_finish_header(midi_ctl);
  }
} /* end audfmt_check_next_track */

/* <EJECT> */
#ifdef FEATURE_SMAF
/*===========================================================================

FUNCTION smaf_get_next_8_bits

DESCRIPTION
  This function is used to generate huffman table; it returns the next
  8 bits in the data stream.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_get_next_8_bits (
  audfmt_track_type *track_ptr,
  uint8             *next_byte,
  uint32            *counter
) {
  audfmt_status_type ret_val;
  uint8              value;

  ret_val = audfmt_get_next_track_byte(track_ptr, &value, counter);
  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    *next_byte = track_ptr->compressed_bits |
                                (value >> track_ptr->num_compressed_bits);

    track_ptr->compressed_bits = value << (8 - track_ptr->num_compressed_bits);
  }

  return(ret_val);
} /* end smaf_get_next_8_bits */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_get_next_bit

DESCRIPTION
  This function is used to generate huffman table; it returns the next
  bit in the data stream.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_get_next_bit (
  audfmt_track_type *track_ptr,
  uint8             *next_bit,
  uint32            *counter
) {
  audfmt_status_type ret_val;
  uint8              tbyte = 0;

  if(track_ptr->num_compressed_bits == 0) {
    ret_val = audfmt_get_next_track_byte(track_ptr, &track_ptr->compressed_bits,
                                         counter);
    if(ret_val != AUDFMT_STATUS_SUCCESS) {
      return(ret_val);
    }
    track_ptr->num_compressed_bits = 8;
  }

  if((track_ptr->compressed_bits & 0x80) == 0x80) {
    tbyte = 1;
  }

  track_ptr->compressed_bits <<= 1;
  track_ptr->num_compressed_bits--;

  *next_bit = tbyte;

  return(AUDFMT_STATUS_SUCCESS);
} /* end smaf_get_next_bit */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_generate_huffman_table

DESCRIPTION
  This function is used to generate huffman table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_generate_huffman_table (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track_ptr
) {
  uint32 bytes_read      = 0;
  uint32 current_code    = midi_ctl->smaf_current_code;
  uint32 code_length     = midi_ctl->smaf_code_length;
  uint8  num_bits        = track_ptr->num_compressed_bits;
  uint8  compressed_bits = track_ptr->compressed_bits;
  uint8  tbyte;

  audfmt_status_type        ret_val;
  audfmt_huffman_entry_type *huffman_table_entry =
                                         midi_ctl->smaf_huffman_table_entry;

  do {
    while((ret_val = smaf_get_next_bit(track_ptr, &tbyte, &bytes_read)) ==
          AUDFMT_STATUS_SUCCESS) {
      if((tbyte == 1) && (code_length <= 32)) {
        code_length++;
        current_code <<= 1;
      } else {
        break;
      }
    }

    if(code_length > 32) {
      return(AUDFMT_STATUS_FAILURE);
    }

    if(ret_val == AUDFMT_STATUS_SUCCESS) {
      if((ret_val = smaf_get_next_8_bits(track_ptr, &tbyte, &bytes_read)) ==
         AUDFMT_STATUS_SUCCESS) {
        /* The codeword is shifted up to be left justified in order to
        ** facilitate the lookup procedure.
        */
        huffman_table_entry->value       = tbyte;
        huffman_table_entry->codeword    = current_code << (32 - code_length);
        huffman_table_entry->code_length = code_length;

        while((code_length > 0) && ((current_code & 0x1) == 0x1)) {
          current_code >>= 1;
          code_length--;
        }

        current_code |= 0x1;
        huffman_table_entry++;
        midi_ctl->smaf_current_code        = current_code;
        midi_ctl->smaf_code_length         = code_length;
        midi_ctl->smaf_huffman_table_entry = huffman_table_entry;
        bytes_read                         = 0;
        num_bits                           = track_ptr->num_compressed_bits;
        compressed_bits                    = track_ptr->compressed_bits;
      }
    }
  } while((code_length != 0) && (ret_val == AUDFMT_STATUS_SUCCESS) &&
          (huffman_table_entry < &midi_ctl->smaf_huffman_table[256]));

  if((huffman_table_entry == &midi_ctl->smaf_huffman_table[256]) ||
     (ret_val == AUDFMT_STATUS_ERROR)) {
    return(AUDFMT_STATUS_FAILURE);
  } else if(code_length == 0) {
    /* Huffman table is generated, free any used up buffers. We know
    ** that if track_buf_reserve != NULL, then track_buf must contain
    ** an exhausted data buffer. Therefore free and swap.
    */
    if(track_ptr->track_buf_reserve != NULL) {
      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                        (void *) track_ptr->track_buf);
      track_ptr->index             -= track_ptr->buf_size;
      track_ptr->track_buf          = track_ptr->track_buf_reserve;
      track_ptr->buf_size           = track_ptr->buf_reserve_size;
      track_ptr->track_buf_reserve  = NULL;
      track_ptr->buf_reserve_size   = 0;
    }

    return(AUDFMT_STATUS_DONE);
  } else {
    audfmt_restore_track (track_ptr, bytes_read);
    track_ptr->num_compressed_bits = num_bits;
    track_ptr->compressed_bits     = compressed_bits;

    /* In generating huffman table, two data buffers are used to facilitate
    ** codes that span two data buffers. The algorithm will exhaust all data
    ** until there is not enough data to complete the next code. Then
    ** the first buffer is freed and the next one is requested. Even though
    ** the data request is to the track_buf, the upper functions know to 
    ** transfer it to track_buf_reserve when generating Huffman table.
    */
    if(track_ptr->track_buf_reserve != NULL) {
      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                        (void *) track_ptr->track_buf);
      track_ptr->index             -= track_ptr->buf_size;
      track_ptr->track_buf          = track_ptr->track_buf_reserve;
      track_ptr->buf_size           = track_ptr->buf_reserve_size;
      track_ptr->track_buf_reserve  = NULL;
    }

    audmain_get_header_data(midi_ctl->handle, track_ptr->file_location,
                            &(track_ptr->track_buf));
    
    return(AUDFMT_STATUS_BUFF_ERR);
  }
} /* end smaf_generate_huffman_table */

/* <EJECT> */

#ifdef FEATURE_AUDIO_CMX_5P0
/*===========================================================================

FUNCTION audfmt_dls_reset_prog_def

DESCRIPTION
  This function initializes the program definition structure to
  default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_reset_prog_def (
  audfmt_hq_prog_def_struct_type *prog_ptr
) {
  if(prog_ptr != NULL) {
    memcpy(prog_ptr, &audfmt_prog_def_default,
                                  sizeof(audfmt_hq_prog_def_struct_type));
  }
} /* end audfmt_dls_reset_prog_def */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_dls_inst_def

DESCRIPTION
  This function sets up the DLS intrument definition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void smaf_dls_inst_def (
  audfmt_param_def_type          *param_ptr,
  audfmt_hq_prog_def_struct_type *prog_ptr,
  uint8                          *buf
) {
  uint8  btemp;
  uint8  sustainLevel;
  uint8  decayRate, sustainRate;

  if((param_ptr != NULL) && (prog_ptr != NULL) && (buf != NULL)) {
    param_ptr->wave_id       = buf[15];
    param_ptr->sampling_freq = (buf[0] << 8) | buf[1];
    param_ptr->loop_start    = (buf[11] << 8) | buf[12];
    param_ptr->loop_end      = (buf[13] << 8) | buf[14];
    param_ptr->wave_type     = buf[3] & 0x0f;
    param_ptr->total_level   = buf[7] >> 2;

    audfmt_dls_reset_prog_def(prog_ptr);

    btemp = buf[6] >> 4;
    prog_ptr->aeg_attack_time   = smaf_aeg_attack[btemp];

    btemp = buf[5] >> 4;
    prog_ptr->aeg_release_time  = smaf_aeg_release[btemp];

    decayRate    = buf[5] & 0xf;
    sustainLevel = buf[6] & 0xf;
    sustainRate  = buf[4] >> 4;

    if(sustainLevel == 0){
 	   decayRate = sustainRate;
       sustainLevel = 15;
    }else if(sustainLevel > 6 || 
          decayRate == 0 || sustainRate == 0){
          decayRate = decayRate;
	}else if(sustainLevel <= 2 && decayRate > 5){
          decayRate = sustainRate;
          sustainLevel = 15;
	}else if(decayRate <= 2){
          decayRate = decayRate;
          sustainLevel = 15;
	}else if(decayRate > sustainRate){
          decayRate = (2*sustainRate + decayRate)/3;
	}else{
        decayRate = (2*decayRate + sustainRate)/3;
    }
 
    prog_ptr->aeg_decay_time    = smaf_aeg_decay[decayRate];
    prog_ptr->aeg_sustain_level = (15-sustainLevel) * 8;
  }
} /* end smaf_dls_inst_def */

#else /* FEATURE_AUDIO_CMX_5P0 */
/*===========================================================================

FUNCTION audfmt_smaf_get_dls_drum_tune

DESCRIPTION
  This function calculates the tune parameter for DLS drums.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static uint32 audfmt_smaf_get_dls_drum_tune (
  audfmt_param_def_type *param_ptr
) {
  uint32 tlong1, tlong2;
  uint64 ret_val;

  if(param_ptr->wave_id == 0) {
    ret_val = 129;
  } else if(param_ptr->wave_id > 0x80) {
    ret_val = 1;
  } else {
    ret_val = 129 - param_ptr->wave_id;
  }

  tlong1 = ret_val / 12;
  tlong2 = ret_val - (tlong1 * 12);

  ret_val = (1 << tlong1) * audfmt_smaf_dls_drum_exp2TableQ20[tlong2];
  ret_val = ((ret_val * (param_ptr->sampling_freq)) / 44100) ;

  return((uint32)ret_val);
} /* end audfmt_smaf_get_dls_drum_tune */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_dls_inst_def

DESCRIPTION
  This function sets up the DLS intrument definition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void smaf_dls_inst_def (
  audfmt_param_def_type       *param_ptr,
  audfmt_prog_def_struct_type *prog_ptr,
  uint8                       *buf
) {
  uint8  btemp;
  uint8  sustainLevel;
  uint8  decayRate, sustainRate;

  if((param_ptr != NULL) && (prog_ptr != NULL) && (buf != NULL)) {
    param_ptr->wave_id       = buf[15];
    param_ptr->sampling_freq = (buf[0] << 8) | buf[1];
    param_ptr->loop_start    = (buf[11] << 8) | buf[12];
    param_ptr->loop_end      = (buf[13] << 8) | buf[14];
    param_ptr->wave_type     = buf[3] & 0x0f;

    btemp = buf[6] >> 4;
    prog_ptr->aeg_attack_time   = smaf_aeg_attack[btemp];

    btemp = buf[5] >> 4;
    prog_ptr->aeg_release_time  = smaf_aeg_decay[btemp];;

    prog_ptr->aeg_depth  = buf[7];
    prog_ptr->aeg_depth  = audfmt_smaf_map_total_level[prog_ptr->aeg_depth];

    decayRate    = buf[5] & 0xf;
    sustainLevel = buf[6] & 0xf;
    sustainRate  = buf[4] >> 4;

    if(sustainLevel == 0){
 	   decayRate = sustainRate;
       sustainLevel = 15;
    }else if(sustainLevel > 6 || 
          decayRate == 0 || sustainRate == 0){
          decayRate = decayRate;
	}else if(sustainLevel <= 2 && decayRate > 5){
          decayRate = sustainRate;
          sustainLevel = 15;
	}else if(decayRate <= 2){
          decayRate = decayRate;
          sustainLevel = 15;
	}else if(decayRate > sustainRate){
          decayRate = (2*sustainRate + decayRate)/3;
	}else{
        decayRate = (2*decayRate + sustainRate)/3;
    }
 
    prog_ptr->aeg_decay_time    = smaf_aeg_decay[decayRate];
    prog_ptr->aeg_sustain_level = (15-sustainLevel) * 8;

   if(param_ptr->bank_msb == 125) {
      prog_ptr->flw_key       = 0;
    } else {
      prog_ptr->flw_key       = 0x3c;
    }
    prog_ptr->max_key           = 0x7f;
    prog_ptr->course            = 0;
    prog_ptr->type              = 1;
    prog_ptr->tvf_fc            = 0x7f;
    prog_ptr->tvf_resonance     = 0;
    prog_ptr->feg_attack_time   = 0;
    prog_ptr->feg_decay_time    = 0;
    prog_ptr->feg_release_time  = 0;
    prog_ptr->feg_initial_level = 0;
    prog_ptr->feg_sustain_level = 0;
    prog_ptr->feg_release_level = 0;
    prog_ptr->feg_depth         = 0;
    prog_ptr->peg_attack_time   = 0;
    prog_ptr->peg_decay_time    = 0;
    prog_ptr->peg_release_time  = 0;
    prog_ptr->peg_initial_level = 0;
    prog_ptr->peg_sustain_level = 0;
    prog_ptr->peg_release_level = 0;
    prog_ptr->peg_depth         = 0;
    prog_ptr->lfo_freq          = 0;
    prog_ptr->lfo_peg_depth     = 0;
    prog_ptr->lfo_feg_depth     = 0;
    prog_ptr->lfo_aeg_depth     = 0;
    prog_ptr->lfo_delay         = 0;

    prog_ptr->aeg_initial_level = 0;
  }
} /* end smaf_dls_inst_def */
#endif /* FEATURE_AUDIO_CMX_5P0 */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_dec_msbit

DESCRIPTION
  This function decodes the escaped MSBit in SMAF buffers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void smaf_dec_msbit (
  uint8  *dest,
  uint8  *src,
  uint32 length
) {
  uint32 counter;
  uint8  bit_mask;

  while(length > 7) {
    bit_mask = *src++;
    for(counter = 0; counter < 7; counter++) {
      bit_mask <<= 1;
      *dest++ = (*src++ | (bit_mask & 0x80));
    }
    length -= 8;
  }

  if(length != 0) {
    bit_mask = *src++;
    length--;
    for(counter = 0; counter < length; counter++) {
      bit_mask <<= 1;
      *dest++ = (*src++ | (bit_mask & 0x80));
    }
  }
} /* end smaf_dec_msbit */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_get_fm_dls_param_diff

DESCRIPTION
  This function finds the difference between the paramter values for the FM
  instrument DLS and the parameters of instruments DLS in the lookup.
  It takes the bit offset and the mask byte as argument to extract the 
  parameter value from the bytes num1 and num2 and returns
  the absolute difference.

DEPENDENCIES
  None

RETURN VALUE
  uint32 : dls error value

SIDE EFFECTS
  None

===========================================================================*/
uint32 smaf_get_fm_dls_param_diff (
  uint8       num1, 
  const uint8 num2,
  uint8       bitoffset,
  uint8       mask
) {
  uint32 dls_param, lookup_param;

  dls_param    = (uint32) ((num1 >> bitoffset) & mask);
  lookup_param = (uint32) ((num2 >> bitoffset) & mask); 

  if(dls_param > lookup_param) {
    return (dls_param - lookup_param);
  } else {
    return (lookup_param - dls_param);
  }
} /* end smaf_get_fm_dls_param_diff */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_get_fm_dls_error

DESCRIPTION
  This function finds the error between FM DLS instrument paramters and
  FM instrument parameters stored for each GM Instrument in a lookup table.

DEPENDENCIES
  None

RETURN VALUE
  uint32 : dls error value

SIDE EFFECTS
  None

===========================================================================*/
uint32 smaf_get_fm_dls_error (
  uint8       *scratch_buf,
  const uint8 *lookup_buf,
  int         bufsize
) {
  uint32 dls_error;
  int    pos;

  dls_error = smaf_get_fm_dls_param_diff (
                     *scratch_buf, *lookup_buf, 6, 0x03 ) * 333;        /* LFO */

  if(bufsize > 15) {
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf++, *lookup_buf++, 0, 0x07 ) * 143;  /* ALG */
  } else {
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf++, *lookup_buf++, 0, 0x01 ) * 1;  /* ALG */
  }

  for(pos = 1; pos < bufsize; pos += 7) {
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf, *lookup_buf, 4, 0x0f ) * 67;      /* SR  */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf, *lookup_buf, 3, 0x01 ) * 1;      /* XOF */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf, *lookup_buf, 1, 0x01 ) * 1;      /* SUS */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf++, *lookup_buf++, 0, 0x01 ) * 1;  /* KSR */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf, *lookup_buf, 0, 0x0f ) * 54;      /* DR  */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf++, *lookup_buf++, 4, 0x0f ) * 67;  /* RR  */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf, *lookup_buf, 4, 0x0f ) * 120;      /* AR  */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf++, *lookup_buf++, 0, 0x0f ) * 67;  /* SL  */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf, *lookup_buf, 2, 0x3f ) * 16;      /* TL  */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf++, *lookup_buf++, 0, 0x03 ) * 33;  /* KSL */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf, *lookup_buf, 5, 0x03 ) * 33;      /* DAM */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf, *lookup_buf, 1, 0x03 ) * 33;      /* DVB */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf, *lookup_buf, 4, 0x01 ) * 1;      /* EAM */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf++, *lookup_buf++, 0, 0x01 ) * 1;  /* EVB */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf, *lookup_buf, 4, 0x0f ) * 60;    /* MULTI */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf++, *lookup_buf++, 0, 0x07 ) * 143; /* DT  */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf, *lookup_buf, 0, 0x07 ) * 143;      /* FB  */
    dls_error += smaf_get_fm_dls_param_diff (
                        *scratch_buf++, *lookup_buf++, 3, 0x03 ) * 1300;   /* WS  */
  }
  return(dls_error);
} /* end smaf_get_fm_dls_error */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_get_fm_dls_drum_error

DESCRIPTION
  This function finds the error between FM DLS drum paramters 
  the FM drum parameters stored for GM drums in a lookup table.

DEPENDENCIES
  None

RETURN VALUE
  uint32 : dls error value

SIDE EFFECTS
  None

===========================================================================*/
uint32 smaf_get_fm_dls_drum_error (
  uint8       *scratch_buf,
  const uint8 *lookup_buf,
  int         bufsize
) {
  /* error in drum key */ 
  uint32 dls_error;

  dls_error  = smaf_get_fm_dls_param_diff(
                 *scratch_buf, *lookup_buf, 0, 0xff)*10;
  dls_error += smaf_get_fm_dls_error(scratch_buf+2, lookup_buf+2, bufsize-2);

  return(dls_error);
} /* end smaf_get_fm_dls_drum_error */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_map_fm_dls_drum

DESCRIPTION
  This function maps the SMAF drums that are out-of-range to a valid GM drum.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint8 smaf_map_fm_dls_drum (
  uint8 drum,
  const uint8 *drum_map
) {
  if(drum_map != NULL) {
    if((drum >= 24) && (drum <= 34)) {
      drum = drum_map[drum - 24];
    } else if((drum >= 82) && (drum <= 84)) {
      drum = drum_map[drum - 71];
    }
  }
  return(drum);
}

/* <EJECT> */
#ifndef FEATURE_AUDIO_CMX_5P0
#ifdef FEATURE_SMAF_ROM_DLS
#error code not present
#endif /* FEATURE_SMAF_ROM_DLS */
#endif /* not FEATURE_AUDIO_CMX_5P0 */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_do_mtsu

DESCRIPTION
  This function processes the Mtsu (track setup data) SMAF chunk. This chunk
  contains DLS data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_do_mtsu (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track
) {

#ifdef FEATURE_AUDIO_CMX_5P0
  audfmt_hq_prog_def_struct_type       *prog_ptr =  NULL;
#else
  audfmt_prog_def_struct_type          *prog_ptr =  NULL;
#endif /* FEATURE_AUDIO_CMX_5P0 */
  audfmt_param_def_type                *param_ptr = NULL;
  audfmt_waveform_param_type           *wave_param_ptr;
  audfmt_smaf_dls_entry_type           *fm_dls;
  uint32  index, length, bytes_read, counter, fm_dls_inst;
  uint32  dls_error, next_error;
  boolean match;
  uint8   *buf,tempbuf[10];

#ifndef FEATURE_AUDIO_CMX_5P0
#ifdef  FEATURE_SMAF_ROM_DLS
#error code not present
#endif /* FEATURE_SMAF_ROM_DLS */
#endif /* not FEATURE_AUDIO_CMX_5P0*/
 
  /* Process until at end of Mtsu chunk (delta_tick holds the location of
  ** end of Mtsu chunk) or until not enough data to process next system
  ** exclusive message
  */
  while((track->current_location < track->delta_tick) &&
        (track->buf_size > (track->index + 10))) {
    buf   = &track->track_buf[track->index];
    match = FALSE;
    /* Check for sys exclusive message type */
    if(*buf == 0xf0) {
      track->index++;
      track->current_location++;
      bytes_read = 1;
	  memcpy((uint8 *)tempbuf,(uint8 *)buf,(10*sizeof(uint8)));

      /* Get length of sys exclusive message */
      if(midi_read_var_len(track, &length, &bytes_read) ==
         AUDFMT_STATUS_SUCCESS) {
        buf = &track->track_buf[track->index];
		
        if(length ==0x07){

		  if((tempbuf[2] == 0x43) && (tempbuf[3] == 0x79) &&
             ((tempbuf[4] == 0x07) || (tempbuf[4] == 0x06)) &&
             (tempbuf[5] == 0x7f) && (tempbuf[6] == 0x07) &&
			 ((tempbuf[7] == 0x01) || (tempbuf[7] == 0x02)) &&	
			 (tempbuf[8] == 0xF7)){
			 
                if(tempbuf[7]&0x2){
			     midi_ctl->midi_status3|=AUDFMT_MASK_SMAF_ADPCM_PAIR;
			    }				 
		   }     
	    }
        if(((length == 0x1e) && (memcmp(smaf_inst_id, buf, 5) == 0)) ||
           ((length == 0x1b) && (memcmp(smaf_inst_id2, buf, 5) == 0))) {
          /* This sys exclusive message is an instrument parameter
          ** definition for DLS waveform
          */
          /* Check if we have enough data for this instrument definition */
          if((track->index + length) < track->buf_size) {
            if(length == 0x1e) {
              memcpy(audfmt_smaf_scratch_buf, buf, 10);
              smaf_dec_msbit(&audfmt_smaf_scratch_buf[10], &buf[10], 0x13);
            } else {
              memcpy(audfmt_smaf_scratch_buf, buf, length);
            }
#ifndef FEATURE_AUDIO_CMX_5P0
#ifdef FEATURE_SMAF_ROM_DLS
#error code not present
#endif
#endif
            if((audfmt_smaf_scratch_buf[5] == 0x7d) &&
               (audfmt_smaf_scratch_buf[9] == 0x01) &&
               (audfmt_smaf_scratch_buf[25] >= 0x80)) {
              if(midi_ctl->smaf_fm_drum_index < AUDFMT_SMAF_DLS_ENTRY_MAX) {
                for(counter = 0; counter < 24; counter++) {
#ifndef FEATURE_AUDIO_CMX_5P0
#ifdef  FEATURE_SMAF_ROM_DLS
                  if((
                     (memcmp(audfmt_smaf_scratch_buf+10,
                             &audfmt_smaf_pcm_drum_dls_lookup[counter],
                             2)==0) && 
                     (memcmp(audfmt_smaf_scratch_buf+13,
                             &audfmt_smaf_pcm_drum_dls_lookup[counter][3],
                             4)==0) &&
                     (memcmp(audfmt_smaf_scratch_buf + 18,
                             &audfmt_smaf_pcm_drum_dls_lookup[counter][8],
                             8)==0))) {
                    is_rom_dls_drum    = TRUE;
                    rom_dls_drum_index = counter;
                    total_level        = audfmt_smaf_scratch_buf[17];
                    break;
                  }
#else
                  if(memcmp(audfmt_smaf_scratch_buf + 10,
                            &audfmt_smaf_pcm_drum_dls_lookup[counter],
                            16) == 0) {
                    fm_dls = &midi_ctl->smaf_fm_drum
                                             [midi_ctl->smaf_fm_drum_index++];
                    fm_dls->bank_msb   = audfmt_smaf_scratch_buf[5];
                    fm_dls->bank_lsb   = audfmt_smaf_scratch_buf[6];
                    fm_dls->program    = audfmt_smaf_scratch_buf[8];
                    fm_dls->instrument = audfmt_smaf_pcm_drum_dls_map[counter];
                    match              = TRUE;
                    break;
                }
#endif /* FEATURE SMAF_ROM_DLS */
#else
                  if(memcmp(audfmt_smaf_scratch_buf + 10,
                            &audfmt_smaf_pcm_drum_dls_lookup[counter],
                            16) == 0) {
                    fm_dls = &midi_ctl->smaf_fm_drum
                                             [midi_ctl->smaf_fm_drum_index++];
                    fm_dls->bank_msb   = audfmt_smaf_scratch_buf[5];
                    fm_dls->bank_lsb   = audfmt_smaf_scratch_buf[6];
                    fm_dls->program    = audfmt_smaf_scratch_buf[8];
                    fm_dls->instrument = audfmt_smaf_pcm_drum_dls_map[counter];
                    match              = TRUE;
                    break;
                  }
#endif /* FEATURE_AUDIO_CMX_5P0 */
                }
              }
            }

            if(!match) {
              param_ptr = audfmt_dls_struct.param_def;
              /* Find a free instrument parameter definition structure */
              for(counter = 0; counter < 128; counter++) {
                if(param_ptr->in_use_flag == AUDFMT_PROG_DEF_FREE) {
                  break;
                }
                param_ptr++;
              }

              if(counter < 128) {
                /* Mark this instrument definition structure as reserved */
                param_ptr->in_use_flag = audfmt_prog_def_seq
                                                         [midi_ctl->sequence];
                prog_ptr               = &audfmt_dls_struct.prog_def[counter];

                param_ptr->bank_msb    = buf[5];
                param_ptr->bank_lsb    = buf[6];
                if(param_ptr->bank_msb == 124) {
                  param_ptr->program = buf[7];
                } else {
                  param_ptr->program = buf[8];
                }

                if(buf[2] == 6) {
                  smaf_dec_msbit(audfmt_smaf_scratch_buf, &buf[10], 19);
                  smaf_dls_inst_def(param_ptr, prog_ptr,
                                    audfmt_smaf_scratch_buf);
                } else {
                  smaf_dls_inst_def(param_ptr, prog_ptr, &buf[10]);
                }
              }
            }

            /* Update data pointer position */
            track->index            += length;
            track->current_location += length;

#ifndef FEATURE_AUDIO_CMX_5P0
#ifdef FEATURE_SMAF_ROM_DLS
#error code not present
#endif /* FEATURE_SMAF_ROM_DLS */
#endif /* not FEATURE_AUDIO_CMX_5P0 */
          } else {
            /* Rewind data pointer and get next data buffer */
            track->index            -= bytes_read;
            track->current_location -= bytes_read;
            break;
          }
        } else if(((length == 0x2f) && (memcmp(smaf_inst_id, buf, 5) == 0)) ||
                  ((length == 0x2a) && (memcmp(smaf_inst_id2, buf, 5) == 0))) {
          /* This sys exclusive message is an instrument parameter
          ** definition for DLS waveform
          */
          /* Check if we have enough data for this instrument definition */
          if((track->index + length) < track->buf_size) {
            if(length == 0x2f) {
              memcpy(audfmt_smaf_scratch_buf, buf, 10);
              smaf_dec_msbit(&audfmt_smaf_scratch_buf[10], &buf[10], 0x24);
            } else {
              memcpy(audfmt_smaf_scratch_buf, buf, length);
            }
            if(audfmt_smaf_scratch_buf[5] == 0x7c) {
              if(midi_ctl->smaf_fm_dls_index < AUDFMT_SMAF_DLS_ENTRY_MAX) {
                if((audfmt_smaf_scratch_buf[10] == 0x00) &&
                   (audfmt_smaf_scratch_buf[11] == 0x79)) {
                  for(counter = 0; counter < 128; counter++) {
                    if(memcmp(audfmt_smaf_scratch_buf + 12,
                              &audfmt_smaf_fm_4op_dls_lookup[counter],
                              29) == 0) {
                      fm_dls = &midi_ctl->smaf_fm_dls
                                               [midi_ctl->smaf_fm_dls_index++];
                      fm_dls->bank_msb   = audfmt_smaf_scratch_buf[5];
                      fm_dls->bank_lsb   = audfmt_smaf_scratch_buf[6];
                      fm_dls->program    = audfmt_smaf_scratch_buf[7];
                      fm_dls->instrument = counter;

                      match = TRUE;
                      break;
                    }
                  }
                  if(counter == 128) {
                    for(counter = 0; counter < 3; counter++) {
                      if(memcmp(audfmt_smaf_scratch_buf + 12,
                                &audfmt_smaf_fm_4op_dls_lookup2[counter],
                                29) == 0) {
                        fm_dls = &midi_ctl->smaf_fm_dls
                                               [midi_ctl->smaf_fm_dls_index++];
                        fm_dls->bank_msb   = audfmt_smaf_scratch_buf[5];
                        fm_dls->bank_lsb   = audfmt_smaf_scratch_buf[6];
                        fm_dls->program    = audfmt_smaf_scratch_buf[7];
                        if(counter == 0) {
                          fm_dls->instrument = 0x41;
                        } else if(counter == 1) {
                          fm_dls->instrument = 0x42;
                        } else if(counter == 2) {
                          fm_dls->instrument = 0x46;
                        }

                        match = TRUE;
                        break;
                      }
                    }
                  }
                }
                /* fm dls to GM instrument mapping  */
                if(!match) {
                  fm_dls_inst = ((audfmt_smaf_scratch_buf[7] > 0) && 
                                 (audfmt_smaf_scratch_buf[7] < 128)) ?
                                audfmt_smaf_scratch_buf[7] : 0;

                  dls_error   = smaf_get_fm_dls_error (
                                   audfmt_smaf_scratch_buf + 12,
                                   audfmt_smaf_fm_4op_dls_lookup[fm_dls_inst],
                                   29 );

                  for(counter = 0; counter < 128; counter++) {
                    next_error = smaf_get_fm_dls_error (
                                     audfmt_smaf_scratch_buf + 12,
                                     audfmt_smaf_fm_4op_dls_lookup[counter],
                                     29 );
                    if(next_error == 0) {
                      fm_dls_inst = counter;
                      break;
                    } else if(next_error < dls_error) {
                      fm_dls_inst = counter;
                      dls_error   = next_error;
                    }
                  }
                  fm_dls = &midi_ctl->smaf_fm_dls
                                           [midi_ctl->smaf_fm_dls_index++];
                  fm_dls->bank_msb   = audfmt_smaf_scratch_buf[5];
                  fm_dls->bank_lsb   = audfmt_smaf_scratch_buf[6];
                  fm_dls->program    = audfmt_smaf_scratch_buf[7];
                  fm_dls->instrument = fm_dls_inst;
                }
              }
            } else if(audfmt_smaf_scratch_buf[5] == 0x7d) {
              if(midi_ctl->smaf_fm_drum_index < AUDFMT_SMAF_DLS_ENTRY_MAX) {
                for(counter = 0; counter < 40; counter++) {
                  if(memcmp(audfmt_smaf_scratch_buf + 10,
                            &audfmt_smaf_fm_4op_drum_dls_lookup[counter],
                            31) == 0) {
                    fm_dls = &midi_ctl->smaf_fm_drum
                                             [midi_ctl->smaf_fm_drum_index++];
                    fm_dls->bank_msb   = audfmt_smaf_scratch_buf[5];
                    fm_dls->bank_lsb   = audfmt_smaf_scratch_buf[6];
                    fm_dls->program    = audfmt_smaf_scratch_buf[8];
                    fm_dls->instrument = smaf_map_fm_dls_drum(
                                          audfmt_smaf_fm_drum_dls_map[counter],
                                          audfmt_l1_ma3_4op_drum_range_map);

                    match = TRUE;
                    break;
                  }
                }

                /* fm dls to GM instrument mapping  */
                if(!match) {
                  fm_dls_inst = 0;
                  dls_error   = smaf_get_fm_dls_drum_error (
                                    audfmt_smaf_scratch_buf + 10,
                                    audfmt_smaf_fm_4op_drum_dls_lookup[0], 31 );

                  for(counter = 1; counter < 40; counter++) {
                    next_error = smaf_get_fm_dls_drum_error (
                                   audfmt_smaf_scratch_buf + 10,
                                   audfmt_smaf_fm_4op_drum_dls_lookup[counter],
                                   31 );
                    if(next_error == 0) {
                      fm_dls_inst = counter;
                      break;
                    } else if(next_error < dls_error) {
                      fm_dls_inst = counter;
                      dls_error   = next_error;
                    }
                  }
                  if(dls_error < 5000) {
                    fm_dls = &midi_ctl->smaf_fm_drum
                                          [midi_ctl->smaf_fm_drum_index++];
                    fm_dls->bank_msb   = audfmt_smaf_scratch_buf[5];
                    fm_dls->bank_lsb   = audfmt_smaf_scratch_buf[6];
                    fm_dls->program    = audfmt_smaf_scratch_buf[8];
                    fm_dls->instrument = smaf_map_fm_dls_drum(
                                            audfmt_smaf_fm_drum_dls_map
                                                               [fm_dls_inst],
                                            audfmt_l1_ma3_4op_drum_range_map);
                  }
                }
              }
            }
            /* Update data pointer position */
            track->index            += length;
            track->current_location += length;
          } else {
            /* Rewind data pointer and get next data buffer */
            track->index            -= bytes_read;
            track->current_location -= bytes_read;
            break;
          }
        } else if(((length == 0x1f) && (memcmp(smaf_inst_id, buf, 5) == 0)) ||
                  ((length == 0x1c) && (memcmp(smaf_inst_id2, buf, 5) == 0))) {
          /* This sys exclusive message is an instrument parameter
          ** definition for DLS waveform
          */
          /* Check if we have enough data for this instrument definition */
          if((track->index + length) < track->buf_size) {
            if(length == 0x1f) {
              memcpy(audfmt_smaf_scratch_buf, buf, 10);
              smaf_dec_msbit(&audfmt_smaf_scratch_buf[10], &buf[10], 0x14);
            } else {
              memcpy(audfmt_smaf_scratch_buf, buf, length);
            }
            if(audfmt_smaf_scratch_buf[5] == 0x7c) {
              if(midi_ctl->smaf_fm_dls_index < AUDFMT_SMAF_DLS_ENTRY_MAX) {
                if(audfmt_smaf_scratch_buf[11] == 0x79) {
                  /* Check against special DLS instrument */
                  if(memcmp(audfmt_smaf_scratch_buf + 12,
                            &audfmt_smaf_fm_2op_dls_lookup[128], 15) == 0) {
                    fm_dls = &midi_ctl->smaf_fm_dls
                                            [midi_ctl->smaf_fm_dls_index++];
                    fm_dls->bank_msb   = audfmt_smaf_scratch_buf[5];
                    fm_dls->bank_lsb   = audfmt_smaf_scratch_buf[6];
                    fm_dls->program    = audfmt_smaf_scratch_buf[7];
                    fm_dls->instrument = 12;
                    match = TRUE;
                  } else {
                    for(counter = 0; counter < 128; counter++) {
                      if((memcmp(audfmt_smaf_scratch_buf + 12,
                                 &audfmt_smaf_fm_2op_dls_lookup[counter],
                                 15) == 0) ||
                         (memcmp(audfmt_smaf_scratch_buf + 12,
                                 &audfmt_smaf_fm_2op_dls_lookup2[counter],
                                 15) == 0)) {
                        fm_dls = &midi_ctl->smaf_fm_dls
                                               [midi_ctl->smaf_fm_dls_index++];
                        fm_dls->bank_msb   = audfmt_smaf_scratch_buf[5];
                        fm_dls->bank_lsb   = audfmt_smaf_scratch_buf[6];
                        fm_dls->program    = audfmt_smaf_scratch_buf[7];
                        fm_dls->instrument = counter;

                        match = TRUE;
                        break;
                      }
                    }
                  }
                  /* fm dls to GM instrument mapping  */
                  if(!match) {
                    fm_dls_inst = ((audfmt_smaf_scratch_buf[7] > 0) && 
                                   (audfmt_smaf_scratch_buf[7] < 128)) ?
                                  audfmt_smaf_scratch_buf[7] : 0;
                    dls_error   = smaf_get_fm_dls_error (
                                    audfmt_smaf_scratch_buf + 12,
                                    audfmt_smaf_fm_2op_dls_lookup[fm_dls_inst],
                                    15 );

                    for(counter = 0; counter < 129; counter++) {
                      next_error = smaf_get_fm_dls_error (
                                      audfmt_smaf_scratch_buf + 12,
                                      audfmt_smaf_fm_2op_dls_lookup[counter],
                                      15 );

                      if(next_error == 0) {
                        fm_dls_inst = counter;
                        break;
                      } else if(next_error < dls_error) {
                        fm_dls_inst = counter;
                        dls_error   = next_error;
                      }
                    }
                    fm_dls = &midi_ctl->smaf_fm_dls
                                        [midi_ctl->smaf_fm_dls_index++];
                    fm_dls->bank_msb   = audfmt_smaf_scratch_buf[5];
                    fm_dls->bank_lsb   = audfmt_smaf_scratch_buf[6];
                    fm_dls->program    = audfmt_smaf_scratch_buf[7];

                    /* Remap special DLS instrument to 12 */
                    if (fm_dls_inst == 128) {
                      fm_dls_inst = 12;
                    }
                    fm_dls->instrument = fm_dls_inst;
                  }
                }
              }
            } else if(audfmt_smaf_scratch_buf[5] == 0x7d) {
              if(midi_ctl->smaf_fm_drum_index < AUDFMT_SMAF_DLS_ENTRY_MAX) {
                for(counter = 0; counter < 40; counter++) {
                  if(memcmp(audfmt_smaf_scratch_buf + 10,
                            &audfmt_smaf_fm_2op_drum_dls_lookup[counter],
                            17) == 0) {
                    fm_dls = &midi_ctl->smaf_fm_drum
                                             [midi_ctl->smaf_fm_drum_index++];
                    fm_dls->bank_msb   = audfmt_smaf_scratch_buf[5];
                    fm_dls->bank_lsb   = audfmt_smaf_scratch_buf[6];
                    fm_dls->program    = audfmt_smaf_scratch_buf[8];
                    fm_dls->instrument = smaf_map_fm_dls_drum(
                                              audfmt_smaf_fm_drum_dls_map
                                                                   [counter],
                                              audfmt_l1_ma3_2op_drum_range_map);

                    match = TRUE;
                    break;
                  }
                }

                /* fm dls to GM instrument mapping  */
                if(!match) {
                  fm_dls_inst = 0;
                  dls_error   = smaf_get_fm_dls_drum_error (
                                    audfmt_smaf_scratch_buf + 10,
                                    audfmt_smaf_fm_2op_drum_dls_lookup[0], 17 );

                  for(counter = 1; counter < 40; counter++) {
                    next_error = smaf_get_fm_dls_drum_error (
                                    audfmt_smaf_scratch_buf + 10,
                                    audfmt_smaf_fm_2op_drum_dls_lookup[counter],
                                    17 );

                    if(next_error == 0) {
                      fm_dls_inst = counter;
                      break;
                    } else if(next_error < dls_error) {
                      fm_dls_inst = counter;
                      dls_error   = next_error;
                    }
                  }
                  if(dls_error < 2800) {
                    fm_dls = &midi_ctl->smaf_fm_drum
                                            [midi_ctl->smaf_fm_drum_index++];
                    fm_dls->bank_msb   = audfmt_smaf_scratch_buf[5];
                    fm_dls->bank_lsb   = audfmt_smaf_scratch_buf[6];
                    fm_dls->program    = audfmt_smaf_scratch_buf[8];
                    fm_dls->instrument = smaf_map_fm_dls_drum(
                                              audfmt_smaf_fm_drum_dls_map
                                                               [fm_dls_inst],
                                              audfmt_l1_ma3_2op_drum_range_map);
                  }
                }
              }
            }
            /* Update data pointer position */
            track->index            += length;
            track->current_location += length;
          } else {
            /* Rewind data pointer and get next data buffer */
            track->index            -= bytes_read;
            track->current_location -= bytes_read;
            break;
          }
        } else if((memcmp(smaf_wave_id, buf, 5) == 0) ||
                  (memcmp(smaf_wave_id2, buf, 5) == 0)) {
          /* This system exclusive message contains a DLS waveform */
          /* Check to make sure we have enough data to process this waveform */
          if((track->index + 7) < track->buf_size) {

            /* Find instrument parameter defintion that matches this
            ** waveform ID
            */
            param_ptr = audfmt_dls_struct.param_def;
            for(counter = 0; counter < 128; counter++) {
              if((param_ptr->in_use_flag ==
                               audfmt_prog_def_seq[midi_ctl->sequence]) &&
                 (param_ptr->wave_id == buf[5])) {
                break;
              }
              param_ptr++;
            }

            if(counter < 128) {
              /* Find a free waveform structure */
              wave_param_ptr = audfmt_dls_struct.wave_param;
              for(index = 0; index < 128; index++) {
                if(wave_param_ptr->in_use_flag == AUDFMT_PROG_DEF_FREE) {
                  break;
                }
                wave_param_ptr++;
              }

              if(index < 128) {
                /* Mark this waveform structure as reserved */
                wave_param_ptr->in_use_flag =
                                    audfmt_prog_def_seq[midi_ctl->sequence];

                wave_param_ptr->wave_entry   = index;
                wave_param_ptr->inst_entry   = counter;
                wave_param_ptr->wave_id      = param_ptr->wave_id;
                wave_param_ptr->wave_encoded = (buf[2] == 6);
                wave_param_ptr->location     = track->current_location + 7;
                wave_param_ptr->size         = length - 8;
                wave_param_ptr->drum_tune    = (param_ptr->bank_msb == 125);

                wave_param_ptr->encoded_waveform = NULL;
                wave_param_ptr->decoded_waveform = NULL;

                param_ptr->wave_id = counter;
/* mapping the flw_key value to the wave_id in case pcm dls drum*/
#ifndef FEATURE_AUDIO_CMX_5P0
                if(param_ptr->bank_msb == 125)
                {
                  audfmt_dls_struct.prog_def[counter].flw_key = param_ptr->wave_id;
                }
#endif /* FEATURE_AUDIO_CMX_5P0 */

                /* Do callback to retrieve waveform data */
                audmain_get_dls(midi_ctl->handle, wave_param_ptr->location,
                                wave_param_ptr->size, wave_param_ptr);

                track->track_status |= AUDFMT_TRACK_MASK_DO_MTSU_CONT;
                 /*
                  The idea is PCM DLS instrument
                  can share the Wav data.They can sahre only if all paramters
                  for instrument definitons are identical.
                  in this imeplementation if instrument defitions are diff
                  like sampling rate loop etc then rewind location in file 
                  and make file to see same message.
				 */
                 {
                  boolean match_found =  FALSE;

                  if(counter < 128) {
                    audfmt_param_def_type *seek_ptr = param_ptr + 1;

                    for(counter++; counter < 128; counter++) {
                      if((seek_ptr->in_use_flag ==
                                   audfmt_prog_def_seq[midi_ctl->sequence]) &&
                         (seek_ptr->wave_id == buf[5])) {

                        if((param_ptr->bank_msb == 125) ||
                           (param_ptr->loop_start != seek_ptr->loop_start) ||
                           (param_ptr->loop_end != seek_ptr->loop_end) ||
                           (param_ptr->bank_msb != seek_ptr->bank_msb) ||
                           (param_ptr->sampling_freq !=
                                               seek_ptr->sampling_freq)) {
                          match_found = TRUE;
                          break;
                        }
                      }
                      seek_ptr++;
                    }
                  }

                  if(match_found) {
                    /* Update data pointer position */
                    track->index            -= bytes_read;
                    track->current_location -= bytes_read;
                  } else {
                    /* Update data pointer position */
                    track->index            += length;
                    track->current_location += length;
                  }
                }

                /* Return for now to process waveform data callback.
                ** The rest of Mtsu chunk will be processed after waveform
                ** data is processed.
                */
                return(AUDFMT_STATUS_SUCCESS);
              }
            }

            /* Update data pointer to end of this system exclusive message */
            track->index            += length;
            track->current_location += length;
          } else {
            /* Rewind data pointer to get next buffer at start of this
            ** sys exclusive message
            */
            track->index            -= bytes_read;
            track->current_location -= bytes_read;
            break;
          }
        } else {
          /* Unsupported system exclusive command, update data pointer
          ** to next message
          */
          track->index            += length;
          track->current_location += length;
        }
      } else {
        /* End of instrument System Exclusive Message */
        /* Error encountered, skip to end of Mtsu */
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                          (void *) track->track_buf);
        track->track_buf = NULL;
        track->buf_size  = 0;

        track->current_location = track->delta_tick;
        audmain_get_header_data(midi_ctl->handle, track->current_location,
                                &(track->track_buf));
        return(AUDFMT_STATUS_SUCCESS);
      }
    } else {
      /* Error encountered, skip to end of Mtsu */
      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                        (void *) track->track_buf);
      track->track_buf = NULL;
      track->buf_size  = 0;

      track->current_location = track->delta_tick;
      audmain_get_header_data(midi_ctl->handle, track->current_location,
                              &(track->track_buf));
      return(AUDFMT_STATUS_SUCCESS);
    }
  }

  if(track->current_location == track->delta_tick) {
    /* Reached end of Mtsu chunk (delta_tick contains location of end of
    ** Mtsu chunk). Free up instrument parameter definition structures
    ** that have been reserved but without waveforms.
    */
    param_ptr = audfmt_dls_struct.param_def;
    for(counter = 0; counter < 128; counter++) {
      if(param_ptr->in_use_flag == audfmt_prog_def_seq[midi_ctl->sequence]) {
        param_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
      }
      param_ptr++;
    }

    /* Clear delta_tick parameter so that it can be used for its intended use */
    track->delta_tick = 0;
    return(AUDFMT_STATUS_DONE);
  } else {
    /* Not at end of Mtsu chunk, free current buffer and get next buffer */
    audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                      (void *) track->track_buf);
    track->track_buf = NULL;
    track->buf_size  = 0;

    audmain_get_header_data(midi_ctl->handle, track->current_location,
                            &(track->track_buf));
    return(AUDFMT_STATUS_SUCCESS);
  }
} /* end smaf_do_mtsu */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_do_hp_mtsu

DESCRIPTION
  This function processes the Mtsu (track setup data) SMAF chunk for HP format.
  This chunk contains DLS data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_do_hp_mtsu (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track
) {
  audfmt_smaf_dls_entry_type *fm_dls;
  uint32 length, bytes_read, counter;
  uint8  *buf;

  /* Process until at end of Mtsu chunk (delta_tick holds the location of
  ** end of Mtsu chunk) or until not enough data to process next system
  ** exclusive message
  */
  while((track->current_location < track->delta_tick) &&
        (track->buf_size > (track->index + 5))) {
    buf = &track->track_buf[track->index];
    /* Check for sys exclusive message type */
    if((buf[0] == 0xff) && (buf[1] == 0xf0)) {
      length                   = buf[2];
      track->index            += 3;
      track->current_location += 3;
      bytes_read               = 3;

      if((length == 0x12) && (buf[3] == 0x43) && (buf[4] == 0x3)) {
        /* This sys exclusive message is an instrument parameter
        ** definition for DLS waveform
        */
        /* Check if we have enough data for this instrument definition */
        if((track->index + length) < track->buf_size) {
          if(buf[6] == 0) {
            if(midi_ctl->smaf_fm_dls_index < AUDFMT_SMAF_DLS_ENTRY_MAX) {
              for(counter = 0; counter < 128; counter++) {
                if(memcmp(&buf[8], &audfmt_smaf_hp_dls_lookup[counter],
                          12) == 0) {
                  fm_dls = &midi_ctl->smaf_fm_dls
                                           [midi_ctl->smaf_fm_dls_index++];
                  fm_dls->program    = buf[7];
                  fm_dls->instrument = counter;
                  break;
                }
              }
            }
          } else if(buf[6] == 0x80) {
            if(midi_ctl->smaf_fm_drum_index < AUDFMT_SMAF_DLS_ENTRY_MAX) {
              for(counter = 0; counter < 61; counter++) {
                if(memcmp(&buf[8], &audfmt_smaf_hp_drum_lookup[counter],
                          12) == 0) {
                  fm_dls = &midi_ctl->smaf_fm_drum
                                           [midi_ctl->smaf_fm_drum_index++];
                  fm_dls->program    = buf[7];
                  fm_dls->instrument = smaf_map_fm_dls_drum(counter + 24,
                                                 audfmt_smaf_hp_drum_range_map);
                  break;
                }
              }
            }
          }
          /* Update data pointer position */
          track->index            += length;
          track->current_location += length;
        } else {
          /* Rewind data pointer and get next data buffer */
          track->index            -= bytes_read;
          track->current_location -= bytes_read;
          break;
        }
      } else {
        /* Unsupported system exclusive command, update data pointer
        ** to next message
        */
        track->index            += length;
        track->current_location += length;
      }
    } else {
      /* End of instrument System Exclusive Message */
      /* Error encountered, skip to end of Mtsu */
      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                        (void *) track->track_buf);
      track->track_buf = NULL;
      track->buf_size  = 0;

      track->current_location = track->delta_tick;
      audmain_get_header_data(midi_ctl->handle, track->current_location,
                              &(track->track_buf));
      return(AUDFMT_STATUS_SUCCESS);
    }
  }

  if(track->current_location == track->delta_tick) {
    /* Reached end of Mtsu chunk (delta_tick contains location of end of
    ** Mtsu chunk).
    ** Clear delta_tick parameter so that it can be used for its intended use.
    */
    track->delta_tick = 0;
    return(AUDFMT_STATUS_DONE);
  } else {
    /* Not at end of Mtsu chunk, free current buffer and get next buffer */
    audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                      (void *) track->track_buf);
    track->track_buf = NULL;
    track->buf_size  = 0;

    audmain_get_header_data(midi_ctl->handle, track->current_location,
                            &(track->track_buf));
    return(AUDFMT_STATUS_SUCCESS);
  }
} /* end smaf_do_hp_mtsu */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_do_mspi

DESCRIPTION
  This function processes the MspI SMAF chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_do_mspi (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track
) {
  uint32 length;

  /* Process until at end of MspI chunk (delta_tick holds the location of
  ** end of MspI chunk)
  */
  while((track->current_location < track->delta_tick) &&
        (track->index < track->buf_size)) {
    switch(track->running_status) {
      case 0:
        /* Look for tag */
        if(track->buf_size >= (track->index + 3)) {
          if(memcmp(&track->track_buf[track->index], "st:", 3) == 0) {
            if(track->buf_size >= (track->index + 8)) {
              if(track->track_buf[track->index + 7] == ',') {
                length = track->track_buf[track->index + 3];
                length = (length << 8) | track->track_buf[track->index + 4];
                length = (length << 8) | track->track_buf[track->index + 5];
                length = (length << 8) | track->track_buf[track->index + 6];

                track->start_offset = length;

                track->index            += 8;
                track->current_location += 8;
              } else {
                track->index            += 3;
                track->current_location += 3;
                track->running_status    = 1;
              }
            } else {
              track->index = track->buf_size;
            }
          } else if(memcmp(&track->track_buf[track->index], "sp:", 3) == 0) {
            if(track->buf_size >= (track->index + 8)) {
              if(track->track_buf[track->index + 7] == ',') {
                length = track->track_buf[track->index + 3];
                length = (length << 8) | track->track_buf[track->index + 4];
                length = (length << 8) | track->track_buf[track->index + 5];
                length = (length << 8) | track->track_buf[track->index + 6];

                track->stop_offset = length;

                track->index            += 8;
                track->current_location += 8;
              } else {
                track->index            += 3;
                track->current_location += 3;
                track->running_status    = 1;
              }
            } else {
              track->index = track->buf_size;
            }
          } else {
            track->index            += 3;
            track->current_location += 3;
            track->running_status    = 1;
          }
        } else {
          track->index = track->buf_size;
        }
        break;

      case 1:
        /* Look for comma */
        while((track->current_location < track->delta_tick) &&
              (track->index < track->buf_size)) {
          if(track->track_buf[track->index] == ',') {
            track->running_status = 0;
          }
          track->index++;
          track->current_location++;
        }
        break;
    }
  }

  if(track->current_location == track->delta_tick) {
    /* Reached end of MspI chunk (delta_tick contains location of end of
    ** MspI chunk).
    ** Clear delta_tick parameter so that it can be used for its intended use.
    */
    track->delta_tick     = 0;
    track->running_status = 0;
    return(AUDFMT_STATUS_DONE);
  } else {
    /* Not at end of MspI chunk, free current buffer and get next buffer */
    audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                      (void *) track->track_buf);
    track->track_buf = NULL;
    track->buf_size  = 0;

    audmain_get_header_data(midi_ctl->handle, track->current_location,
                            &(track->track_buf));
    return(AUDFMT_STATUS_SUCCESS);
  }
} /* end smaf_do_mspi */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_phrase_get_fm_dls_error

DESCRIPTION
  This function finds the error between FM DLS instrument paramters for SMAF
  phrase files and FM instrument parameters stored for each GM Instrument 
  in a lookup table.

INPUTS
  uint8 : *scratch_buf
  uint8 : *lookup_buf
  int : bufsize

DEPENDENCIES
  None

RETURN VALUE
  uint32 : dls error value

SIDE EFFECTS
  None

===========================================================================*/
uint32 smaf_phrase_get_fm_dls_error (
  uint8       *scratch_buf,
  const uint8 *lookup_buf,
  int         bufsize
) {
  uint32 dls_error;
  uint32 pos;   

  /* Evaluate error for LFO, FB, ALG */
  dls_error = smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 6, 0x03) * 333;     /* LFO  */
 
  /* Check bufsize, 2 op_byte_length = 10, 4 op_byte_length = 20 */
  if (bufsize > 12) {
    /* This means we have more than 2 ops, i.e. 4 ops */
    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 3, 0x07) * 286;     /* FB   */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf++, *lookup_buf++, 0, 0x07) * 143; /* ALG  */         
  } else {
    /* This means we have 2 ops */
    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 3, 0x07) * 143;     /* FB   */
    
    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf++, *lookup_buf++, 0, 0x01) * 1;   /* ALG  */
  }

  /* advance both buffers by one byte - throw away byte contains no info */
  scratch_buf++;
  lookup_buf++;   
   
  /* Check per operator parameter difference errors */
  /* each op requires 5 bytes worth of parameter info */
  for(pos = 2; pos < bufsize; pos += 5) {
    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 4, 0x0F) * 60;      /* MULT */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 3, 0x01) * 1;       /* VIB  */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 2, 0x01) * 1;       /* EGT  */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 1, 0x01) * 1;       /* SUS  */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf++, *lookup_buf++, 0, 0x01) * 1;   /* KSR  */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 4, 0x0F) * 67;      /* RR   */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf++, *lookup_buf++, 0, 0x0F) * 54;  /* DR   */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 4, 0x0F) * 120;     /* AR   */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf++, *lookup_buf++, 0, 0x0F) * 67;  /* SL   */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 2, 0x3F) * 16;      /* TL   */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf++, *lookup_buf++, 0, 0x03) * 33;  /* KSL  */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 6, 0x03) * 33;      /* DVB  */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 4, 0x03) * 33;      /* DAM  */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf, *lookup_buf, 3, 0x01) * 33;      /* AM   */

    dls_error += smaf_get_fm_dls_param_diff(
                       *scratch_buf++, *lookup_buf++, 0, 0x07) * 1300;/* WS   */
  }
  return(dls_error);
}

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_phrase_closest_instrument

DESCRIPTION
  This function determines the closest matching instrument to the
  indicated FM DLS parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint8 smaf_phrase_closest_instrument (
  smaf_fm_dls_error_fn_type err_fn,
  uint8                     *inst,
  uint32                    num_bytes,
  const uint8               *table,
  uint32                    num_entries,
  uint32                    *error
) {
  uint8  ret_val = 0;
  uint32 min_error = 0, counter, tlong;

  if((err_fn != NULL) && (inst != NULL) && (table != NULL) &&
     (num_entries > 0)) {
    min_error = err_fn(inst, table, num_bytes);
    if(min_error != 0) {
      for(counter = 1; counter < num_entries; counter++) {
        table += num_bytes;
        tlong  = err_fn(inst, table, num_bytes);
        if(tlong == 0) {
          ret_val   = counter;
          min_error = 0;
          break;
        } else if(tlong < min_error) {
          ret_val   = counter;
          min_error = tlong;
        }
      }
    }
  }

  if(error != NULL) {
    *error = min_error;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_process_waveform

DESCRIPTION
  This function processes the waveform data for DLS waveforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static uint32 smaf_process_waveform (
  uint8   *data,
  uint16  *buf,
  uint32  length,
  uint8   wave_type,
  boolean encoded
) {
  uint32 sample_count = 0, bit_mask, counter, value, value_nibble;
  sint31 signal = 0, step = 0x7f;

  if(encoded) {
    if(wave_type == 3) {
      while(length > 7) {
        bit_mask = *data++;
        for(counter = 0; counter < 7; counter++) {
          bit_mask <<= 1;
          *buf++ = (*data++ | (bit_mask & 0x80))<<8;
        }
        sample_count += 7;
        length       -= 8;
      }
      if(length != 0) {
        bit_mask = *data++;
        length--;
        for(counter = 0; counter < length; counter++) {
          bit_mask <<= 1;
          *buf++ = (*data++ | (bit_mask & 0x80))<<8;
          sample_count++;
        }
      }
    } else if(wave_type == 0) {
      while(length > 7) {
        bit_mask = *data++;
        for(counter = 0; counter < 7; counter++) {
          bit_mask <<= 1;
          value = *data++ | (bit_mask & 0x80);
          value_nibble = value & 0xf;

          signal += (step * audfmt_smaf_diff_lookup[value_nibble])/8;
          if(signal < -32768) {
            signal = -32768;
          } else if(signal > 32767) {
            signal = 32767;
          }
          step = (step * audfmt_smaf_index_scale[value_nibble]) >> 8;
          if(step < 0x7f) {
            step = 0x7f;
          } else if(step > 0x6000) {
            step = 0x6000;
          }
          *buf++ = ((signal >> 8) & 0xff) << 8;

          value_nibble = (value >> 4) & 0xf;

          signal += (step * audfmt_smaf_diff_lookup[value_nibble])/8;
          if(signal < -32768) {
            signal = -32768;
          } else if(signal > 32767) {
            signal = 32767;
          }
          step = (step * audfmt_smaf_index_scale[value_nibble]) >> 8;
          if(step < 0x7f) {
            step = 0x7f;
          } else if(step > 0x6000) {
            step = 0x6000;
          }
          *buf++ = ((signal >> 8) & 0xff) << 8;
        }
        sample_count += 14;
        length       -= 8;
      }
      if(length != 0) {
        bit_mask = *data++;
        length--;
        for(counter = 0; counter < length; counter++) {
          bit_mask <<= 1;
          value = *data++ | (bit_mask & 0x80);
          value_nibble = value & 0xf;

          signal += (step * audfmt_smaf_diff_lookup[value_nibble])/8;
          if(signal < -32768) {
            signal = -32768;
          } else if(signal > 32767) {
            signal = 32767;
          }
          step = (step * audfmt_smaf_index_scale[value_nibble]) >> 8;
          if(step < 0x7f) {
            step = 0x7f;
          } else if(step > 0x6000) {
            step = 0x6000;
          }
          *buf++ = ((signal >> 8) & 0xff) << 8;

          value_nibble = (value >> 4) & 0xf;

          signal += (step * audfmt_smaf_diff_lookup[value_nibble])/8;
          if(signal < -32768) {
            signal = -32768;
          } else if(signal > 32767) {
            signal = 32767;
          }
          step = (step * audfmt_smaf_index_scale[value_nibble]) >> 8;
          if(step < 0x7f) {
            step = 0x7f;
          } else if(step > 0x6000) {
            step = 0x6000;
          }
          *buf++ = ((signal >> 8) & 0xff) <<8 ;
          sample_count += 2;
        }
      }
    } else if(wave_type == 2) {
      while(length > 7) {
        bit_mask = *data++;
        for(counter = 0; counter < 7; counter++) {
          bit_mask <<= 1;
          *buf++ = ((*data++ | (bit_mask & 0x80)) - 0x80) << 8;
        }
        sample_count += 7;
        length       -= 8;
      }
      if(length != 0) {
        bit_mask = *data++;
        length--;
        for(counter = 0; counter < length; counter++) {
          bit_mask <<= 1;
          *buf++ = ((*data++ | (bit_mask & 0x80)) - 0x80) << 8;
          sample_count++;
        }
      }
    }
  } else {
    if(wave_type == 3) {
      sample_count = length;
      while(length != 0) {
        *buf++ = (*data++) << 8;
        length--;
      }
    } else if(wave_type == 0) {
      while(length != 0) {
        value = *data++;
        value_nibble = value & 0xf;

        signal += (step * audfmt_smaf_diff_lookup[value_nibble])/8;
        if(signal < -32768) {
          signal = -32768;
        } else if(signal > 32767) {
          signal = 32767;
        }
        step = (step * audfmt_smaf_index_scale[value_nibble]) >> 8;
        if(step < 0x7f) {
          step = 0x7f;
        } else if(step > 0x6000) {
          step = 0x6000;
        }
        *buf++ = ((signal >> 8) & 0xff) << 8;

        value_nibble = (value >> 4) & 0xf;

        signal += (step * audfmt_smaf_diff_lookup[value_nibble])/8;
        if(signal < -32768) {
          signal = -32768;
        } else if(signal > 32767) {
          signal = 32767;
        }
        step = (step * audfmt_smaf_index_scale[value_nibble]) >> 8;
        if(step < 0x7f) {
          step = 0x7f;
        } else if(step > 0x6000) {
          step = 0x6000;
        }
        *buf++ = ((signal >> 8) & 0xff) <<8 ;
        sample_count += 2;
        length--;
      }
    } else if(wave_type == 2) {
      sample_count = length;
      while(length != 0) {
        *buf++ = ((*data++) - 0x80) << 8;
        length--;
      }
    }
  }

  return(sample_count);
} /* end smaf_process_waveform */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_process_packed_waveform

DESCRIPTION
  This function processes the waveform data for DLS waveforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static uint32 smaf_process_packed_waveform (
  uint8   *data,
  uint8   *buf,
  uint32  length,
  uint8   wave_type,
  boolean encoded
) {
  uint32 sample_count = 0, bit_mask, counter, value, value_nibble;
  sint31 signal = 0, step = 0x7f;

  if(encoded) {
    if(wave_type == 3) {
      while(length > 7) {
        bit_mask = *data++;
        for(counter = 0; counter < 7; counter++) {
          bit_mask <<= 1;
          *buf++ = (*data++ | (bit_mask & 0x80));
        }
        sample_count += 7;
        length       -= 8;
      }
      if(length != 0) {
        bit_mask = *data++;
        length--;
        for(counter = 0; counter < length; counter++) {
          bit_mask <<= 1;
          *buf++ = (*data++ | (bit_mask & 0x80));
          sample_count++;
        }
      }
    } else if(wave_type == 0) {
      while(length > 7) {
        bit_mask = *data++;
        for(counter = 0; counter < 7; counter++) {
          bit_mask <<= 1;
          value = *data++ | (bit_mask & 0x80);
          value_nibble = value & 0xf;

          signal += (step * audfmt_smaf_diff_lookup[value_nibble])/8;
          if(signal < -32768) {
            signal = -32768;
          } else if(signal > 32767) {
            signal = 32767;
          }
          step = (step * audfmt_smaf_index_scale[value_nibble]) >> 8;
          if(step < 0x7f) {
            step = 0x7f;
          } else if(step > 0x6000) {
            step = 0x6000;
          }
          *buf++ = ((signal >> 8) & 0xff) ;

          value_nibble = (value >> 4) & 0xf;

          signal += (step * audfmt_smaf_diff_lookup[value_nibble])/8;
          if(signal < -32768) {
            signal = -32768;
          } else if(signal > 32767) {
            signal = 32767;
          }
          step = (step * audfmt_smaf_index_scale[value_nibble]) >> 8;
          if(step < 0x7f) {
            step = 0x7f;
          } else if(step > 0x6000) {
            step = 0x6000;
          }
          *buf++ = ((signal >> 8) & 0xff) ;
        }
        sample_count += 14;
        length       -= 8;
      }
      if(length != 0) {
        bit_mask = *data++;
        length--;
        for(counter = 0; counter < length; counter++) {
          bit_mask <<= 1;
          value = *data++ | (bit_mask & 0x80);
          value_nibble = value & 0xf;

          signal += (step * audfmt_smaf_diff_lookup[value_nibble])/8;
          if(signal < -32768) {
            signal = -32768;
          } else if(signal > 32767) {
            signal = 32767;
          }
          step = (step * audfmt_smaf_index_scale[value_nibble]) >> 8;
          if(step < 0x7f) {
            step = 0x7f;
          } else if(step > 0x6000) {
            step = 0x6000;
          }
          *buf++ = ((signal >> 8) & 0xff) ;

          value_nibble = (value >> 4) & 0xf;

          signal += (step * audfmt_smaf_diff_lookup[value_nibble])/8;
          if(signal < -32768) {
            signal = -32768;
          } else if(signal > 32767) {
            signal = 32767;
          }
          step = (step * audfmt_smaf_index_scale[value_nibble]) >> 8;
          if(step < 0x7f) {
            step = 0x7f;
          } else if(step > 0x6000) {
            step = 0x6000;
          }
          *buf++ = ((signal >> 8) & 0xff) ;
          sample_count += 2;
        }
      }
    } else if(wave_type == 2) {
      while(length > 7) {
        bit_mask = *data++;
        for(counter = 0; counter < 7; counter++) {
          bit_mask <<= 1;
          *buf++ = ((*data++ | (bit_mask & 0x80)) - 0x80) ;
        }
        sample_count += 7;
        length       -= 8;
      }
      if(length != 0) {
        bit_mask = *data++;
        length--;
        for(counter = 0; counter < length; counter++) {
          bit_mask <<= 1;
          *buf++ = ((*data++ | (bit_mask & 0x80)) - 0x80) ;
          sample_count++;
        }
      }
    }
  } else {
    if(wave_type == 3) {
      sample_count = length;
      while(length != 0) {
        *buf++ = (*data++) ;
        length--;
      }
    } else if(wave_type == 0) {
      while(length != 0) {
        value = *data++;
        value_nibble = value & 0xf;

        signal += (step * audfmt_smaf_diff_lookup[value_nibble])/8;
        if(signal < -32768) {
          signal = -32768;
        } else if(signal > 32767) {
          signal = 32767;
        }
        step = (step * audfmt_smaf_index_scale[value_nibble]) >> 8;
        if(step < 0x7f) {
          step = 0x7f;
        } else if(step > 0x6000) {
          step = 0x6000;
        }
        *buf++ = ((signal >> 8) & 0xff) ;

        value_nibble = (value >> 4) & 0xf;

        signal += (step * audfmt_smaf_diff_lookup[value_nibble])/8;
        if(signal < -32768) {
          signal = -32768;
        } else if(signal > 32767) {
          signal = 32767;
        }
        step = (step * audfmt_smaf_index_scale[value_nibble]) >> 8;
        if(step < 0x7f) {
          step = 0x7f;
        } else if(step > 0x6000) {
          step = 0x6000;
        }
        *buf++ = ((signal >> 8) & 0xff) ;
        sample_count += 2;
        length--;
      }
    } else if(wave_type == 2) {
      sample_count = length;
      while(length != 0) {
        *buf++ = ((*data++) - 0x80) ;
        length--;
      }
    }
  }

  return(sample_count);
} /* end smaf_process_packed_waveform */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_finalize_pcm_dls

DESCRIPTION
  This function finalizes PCM DLS instrument and waveform definitions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_finalize_pcm_dls (
  audfmt_parse_type          *midi_ctl,
  audfmt_param_def_type      *param_ptr,
  audfmt_waveform_param_type *wave_param_ptr
) {
#ifdef FEATURE_AUDIO_CMX_5P0
  audfmt_hq_waveform_info_type   *wave_ptr;
  audfmt_hq_prog_def_struct_type *prog_ptr;
#else
  audfmt_waveform_info_type      *wave_ptr;
#endif /* FEATURE_AUDIO_CMX_5P0 */
  audfmt_status_type             ret_val = AUDFMT_STATUS_FAILURE;

  uint8  *buf;
  uint16 *p_word;
  uint32 index, max_index;
  #ifndef FEATURE_AUDIO_CMX_5P0
  boolean isdrum = FALSE;
  #endif
  if((midi_ctl != NULL) && (param_ptr != NULL) && (wave_param_ptr != NULL)) {
    if((wave_param_ptr->decoded_waveform != NULL) &&
       (wave_param_ptr->encoded_waveform != NULL)) {
  #ifndef FEATURE_AUDIO_CMX_5P0       
      if(param_ptr->bank_msb == 125){
	  	 isdrum = TRUE;
	  } 
 #endif	  
      p_word = (uint16 *) wave_param_ptr->decoded_waveform;
      buf    = wave_param_ptr->decoded_waveform;
      if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_PACKED_DLS_WF) {
        index     = smaf_process_packed_waveform(
                              wave_param_ptr->encoded_waveform, buf,
                              wave_param_ptr->size, param_ptr->wave_type,
                              wave_param_ptr->wave_encoded);
        max_index = 2 * 8190;
      } else {
        index     = smaf_process_waveform(
                              wave_param_ptr->encoded_waveform, p_word,
                              wave_param_ptr->size, param_ptr->wave_type,
                              wave_param_ptr->wave_encoded);
        max_index = 8190;
      }

      if(index >= max_index) {
        /* Maximum of samples in 16bit words that DSP can support */
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_DLS, 0,
                          (void *) wave_param_ptr->encoded_waveform);
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_DLS, 0,
                          (void *) wave_param_ptr->decoded_waveform);
        wave_param_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
        param_ptr->in_use_flag      = AUDFMT_PROG_DEF_FREE;
      } else {
        wave_param_ptr->in_use_flag = audfmt_prog_def_in_use_seq
                                                         [midi_ctl->sequence];

        wave_param_ptr->sampling_freq = param_ptr->sampling_freq;
        wave_param_ptr->loop_start    = param_ptr->loop_start;
        wave_param_ptr->loop_end      = param_ptr->loop_end;

        wave_ptr  = &audfmt_dls_struct.wave_info[wave_param_ptr->wave_entry];
		wave_ptr->wave_length    = index;
		wave_ptr->loop_start     = param_ptr->loop_start;
		if(wave_ptr->loop_start & 0x8000) {
		  wave_ptr->loop_start = 0;
		}
		if(param_ptr->loop_end < wave_ptr->loop_start) {
		  wave_ptr->loop_start = param_ptr->loop_end;
		}
		wave_ptr->loop_length    = param_ptr->loop_end -
								   wave_ptr->loop_start;

#ifdef FEATURE_AUDIO_CMX_5P0
        {
          uint32 counter;
          audfmt_hq_prog_def_struct_type *prog_seek_ptr;
          audfmt_param_def_type       *seek_ptr;
          audfmt_prog_def_in_use_type next_status;

          next_status   =
                   audfmt_prog_def_in_use_seq[midi_ctl->sequence];
          prog_seek_ptr =
                   &audfmt_dls_struct.prog_def[param_ptr->wave_id + 1];
          seek_ptr      =
                   &audfmt_dls_struct.param_def[param_ptr->wave_id + 1];

          for(counter = (param_ptr->wave_id + 1); counter < 128;
              counter++) {
            if((seek_ptr->in_use_flag == param_ptr->in_use_flag) &&
               (seek_ptr->wave_id == wave_param_ptr->wave_id) &&
               (seek_ptr->bank_msb != 125) &&
               (seek_ptr->loop_start == param_ptr->loop_start) &&
               (seek_ptr->loop_end == param_ptr->loop_end) &&
               (seek_ptr->bank_msb == param_ptr->bank_msb) &&
               (seek_ptr->sampling_freq == param_ptr->sampling_freq)) {
              seek_ptr->in_use_flag      = next_status;
              prog_seek_ptr->wf_info_ptr = wave_ptr;

              audfmt_dls_struct.prog_rec[counter].num_regions = 1;
              audfmt_dls_struct.prog_rec[counter].addr = prog_seek_ptr;
            }
            prog_seek_ptr++;
            seek_ptr++;
          }
        }

        param_ptr->in_use_flag      = audfmt_prog_def_in_use_seq
                                                 [midi_ctl->sequence];

		prog_ptr  = &audfmt_dls_struct.prog_def[param_ptr->wave_id];

		audfmt_dls_struct.prog_rec[param_ptr->wave_id].num_regions = 1;
		audfmt_dls_struct.prog_rec[param_ptr->wave_id].addr        = prog_ptr;

		prog_ptr->wf_info_ptr    = wave_ptr;

		wave_ptr->gain           = audfmt_smaf_map_total_level[param_ptr->total_level];
		wave_ptr->fine_tune      = 0;
		wave_ptr->sampling_rate  = param_ptr->sampling_freq;
		if(wave_ptr->loop_start >= wave_ptr->wave_length) {
		  wave_ptr->loop_start   = wave_ptr->wave_length;
		} else if((wave_ptr->loop_start + wave_ptr->loop_length) >=
				  wave_ptr->wave_length) {
		  wave_ptr->loop_length = wave_ptr->wave_length - wave_ptr->loop_start;
		}

		if(wave_param_ptr->drum_tune) {
		  wave_ptr->unity_note = param_ptr->program;
		} else {
		  wave_ptr->unity_note = 60;
		}

		/*wave_ptr->loop_type_wave_depth = 0xF0;*/
		wave_ptr->wave_addr            = buf;
       {
	     uint8*  wave_start_addr,*loop_start_addr,*loop_end_addr;

	   	 wave_start_addr = wave_param_ptr->decoded_waveform;
	  	 loop_start_addr = wave_param_ptr->decoded_waveform+wave_ptr->loop_start;
	     loop_end_addr = wave_param_ptr->decoded_waveform+param_ptr->loop_end;
	     /*
		   Loop type variable was earlier set to hardcoded to 0xF
		   which should not be case. For one shot it should be 0x2
		   loop_and_end =0x0 and for loop_and_release = = 0x1.
		   lower 4 bits are used for wave depth so should not be modified.
		   purpose of +1 is since it includes data @ loop_end_addr in
	          calculating length.
		   */

	       if(wave_ptr->loop_length==0){
			  wave_ptr->loop_type_wave_depth = 0x20;
			}else if((wave_ptr->loop_length > 0) &&
		            (wave_ptr->wave_length==((loop_end_addr-wave_start_addr)+1))){
			  wave_ptr->loop_type_wave_depth = 0x00;		   		 
		    }else if((wave_ptr->loop_length > 0) &&
	               (wave_ptr->wave_length>((loop_end_addr-wave_start_addr)+1))){
	 	    wave_ptr->loop_type_wave_depth = 0x10;
	    }  
    }		

#else /* FEATURE_AUDIO_CMX_5P0 */
        {
          uint32 counter;
          audfmt_prog_def_struct_type *prog_seek_ptr;
          audfmt_param_def_type       *seek_ptr;
          audfmt_prog_def_in_use_type next_status;
                  next_status  = audfmt_prog_def_in_use_seq[midi_ctl->sequence];

          prog_seek_ptr = &audfmt_dls_struct.prog_def[param_ptr->wave_id + 1];
          seek_ptr      = &audfmt_dls_struct.param_def[param_ptr->wave_id + 1];

          for(counter = (param_ptr->wave_id + 1); counter < 128; counter++) {
            if((seek_ptr->in_use_flag == param_ptr->in_use_flag) &&
               (seek_ptr->wave_id == wave_param_ptr->wave_id) &&
               (seek_ptr->bank_msb != 125) &&
               (seek_ptr->loop_start == param_ptr->loop_start) &&
               (seek_ptr->loop_end == param_ptr->loop_end) &&
               (seek_ptr->bank_msb == param_ptr->bank_msb) &&
               (seek_ptr->sampling_freq == param_ptr->sampling_freq)) {
              seek_ptr->in_use_flag      = next_status;
              prog_seek_ptr->waveform_id = wave_param_ptr->wave_entry;
            }
            prog_seek_ptr++;
            seek_ptr++;
          }
        }

        param_ptr->in_use_flag      = audfmt_prog_def_in_use_seq
                                                         [midi_ctl->sequence];
        audfmt_dls_struct.prog_def[param_ptr->wave_id].waveform_id =
                                                   wave_param_ptr->wave_entry;
        wave_ptr->wave_loc       = 4;
        wave_ptr->reserved       = 0;
        wave_ptr->wave_type      = 0;

        if(wave_param_ptr->drum_tune) {
          index = audfmt_smaf_get_dls_drum_tune(param_ptr);
        } else {
          index = ((uint64) ((uint64) param_ptr->sampling_freq *
                             (uint64) 81895)) >> 6;
        }
        wave_ptr->wave_tune_msw  = (index >> 16) & 0xffff;
        wave_ptr->wave_tune_lsw  = index & 0xffff;

        wave_ptr->wave_addr_lsw  = ((uint32) buf) & 0xffff;
        wave_ptr->wave_addr_msw  = ((uint32) buf >> 16) & 0xffff;

        if(wave_ptr->loop_start >= wave_ptr->wave_length) {
          wave_ptr->loop_length = 2;
          if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_PACKED_DLS_WF) {
            buf[wave_ptr->wave_length]    = 0;
            buf[wave_ptr->wave_length+1]  = 0;
          } else {
            p_word[wave_ptr->wave_length]    = 0;
            p_word[wave_ptr->wave_length+1]  = 0;
          }
          wave_ptr->loop_start   = wave_ptr->wave_length;
          wave_ptr->wave_length += 2;
        } else if((wave_ptr->loop_start + wave_ptr->loop_length) >=
                  wave_ptr->wave_length) {
          wave_ptr->loop_length = wave_ptr->wave_length - wave_ptr->loop_start;
        } else if(wave_ptr->loop_length == 0) {
          wave_ptr->loop_length = 2;
          if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_PACKED_DLS_WF) {
            buf[wave_ptr->loop_start]   = 0;
            buf[wave_ptr->loop_start+1] = 0;
          } else {
            p_word[wave_ptr->loop_start]   = 0;
            p_word[wave_ptr->loop_start+1] = 0;
          }
        }

         if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_PACKED_DLS_WF) {
          if(wave_ptr->loop_length == 1) {
            buf[wave_ptr->loop_start+1] = buf[wave_ptr->loop_start];
            buf[wave_ptr->loop_start+2] = buf[wave_ptr->loop_start];
            buf[wave_ptr->loop_start+3] = buf[wave_ptr->loop_start];
	       if(isdrum) {
                buf[wave_ptr->loop_start+4] = 0;
                buf[wave_ptr->loop_start+5] = 0;
				buf[wave_ptr->loop_start+6] = 0;
				buf[wave_ptr->loop_start+7] = 0;
				buf[wave_ptr->loop_start+8] = 0;				
            }

          } else if(wave_ptr->loop_length == 2) {
            buf[wave_ptr->loop_start+2] = buf[wave_ptr->loop_start];
            buf[wave_ptr->loop_start+3] = buf[wave_ptr->loop_start+1];
            buf[wave_ptr->loop_start+4] = buf[wave_ptr->loop_start];
			if(isdrum) {
                buf[wave_ptr->loop_start+5] = 0;
                buf[wave_ptr->loop_start+6] = 0;
				buf[wave_ptr->loop_start+7] = 0;
                buf[wave_ptr->loop_start+8] = 0;
				buf[wave_ptr->loop_start+9] = 0;				
            }
          } else {
            index = wave_ptr->loop_start + wave_ptr->loop_length;
            buf[index++] = buf[wave_ptr->loop_start];
            buf[index++] = buf[wave_ptr->loop_start+1];
            buf[index++]   = buf[wave_ptr->loop_start+2];
			 if(isdrum) {
                buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
                buf[index] = 0;
            }

          }
        } else {
          if(wave_ptr->loop_length == 1) {
            p_word[wave_ptr->loop_start+1] = p_word[wave_ptr->loop_start];
            p_word[wave_ptr->loop_start+2] = p_word[wave_ptr->loop_start];
            p_word[wave_ptr->loop_start+3] = p_word[wave_ptr->loop_start];
			 if(isdrum) {
                p_word[wave_ptr->loop_start+4] = 0;
                p_word[wave_ptr->loop_start+5] = 0;
				p_word[wave_ptr->loop_start+6] = 0;
				p_word[wave_ptr->loop_start+7] = 0;
				p_word[wave_ptr->loop_start+8] = 0;
            }

          } else if(wave_ptr->loop_length == 2) {
            p_word[wave_ptr->loop_start+2] = p_word[wave_ptr->loop_start];
            p_word[wave_ptr->loop_start+3] = p_word[wave_ptr->loop_start+1];
            p_word[wave_ptr->loop_start+4] = p_word[wave_ptr->loop_start];
			 if(isdrum) {
                p_word[wave_ptr->loop_start+5] = 0;
                p_word[wave_ptr->loop_start+6] = 0;
				p_word[wave_ptr->loop_start+7] = 0;
				p_word[wave_ptr->loop_start+8] = 0;
				p_word[wave_ptr->loop_start+9] = 0;
            }

          } else {
            index = wave_ptr->loop_start + wave_ptr->loop_length;
            p_word[index++] = p_word[wave_ptr->loop_start];
            p_word[index++] = p_word[wave_ptr->loop_start+1];
            p_word[index++]   = p_word[wave_ptr->loop_start+2];
			  if(isdrum) {
                p_word[index++] = 0;
				p_word[index++] = 0;
				p_word[index++] = 0;
	            p_word[index++] = 0;
                p_word[index] = 0;
            }

          }
        }

		if(isdrum){
		  wave_ptr->wave_length += 8;
		}else{
          wave_ptr->wave_length += 3;
		}
	
#endif /* FEATURE_AUDIO_CMX_5P0 */

        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_DLS, 0,
                          (void *) wave_param_ptr->encoded_waveform);
#ifndef FEATURE_AUDIO_CMX_5P0
        if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_FM16) {
            uint8 buf[AUDFMT_COMMAND_SIZE_4];
            midi_ctl->hybrid_mode   = 6;

            buf[0] = MIDI_CONTROL_CHANGE;
            buf[1] = MIDI_ALL_CHANNELS;
            buf[2] = MIDI_BANK_SEL_COMMAND;
            buf[3] = CMID_BANK_NORMAL_HQ;
            audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

            /* with hybrid mode changed to 6,drum can no longer default to HQ*/
            buf[0] = MIDI_CONTROL_CHANGE;
            buf[1] = 9;
            buf[2] = MIDI_BANK_SEL_COMMAND;
            buf[3] = CMID_BANK_DRUM;
            audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
          }
#endif
        ret_val = AUDFMT_STATUS_SUCCESS;
      }
    } else {
      if(wave_param_ptr->encoded_waveform != NULL) {
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_DLS, 0,
                          (void *) wave_param_ptr->encoded_waveform);
      }
      if(wave_param_ptr->decoded_waveform != NULL) {
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_DLS, 0,
                          (void *) wave_param_ptr->decoded_waveform);
      }
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_do_voic

DESCRIPTION
  This function processes the VOIC (voice setup data) SMAF chunk. This chunk
  contains program information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_do_voic (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track
) {
  audfmt_smaf_dls_entry_type     *fm_dls;
  audfmt_waveform_param_type     *wave_param_ptr, *in_use_wave, *res_wave;
  audfmt_param_def_type          *param_ptr;
#ifdef FEATURE_AUDIO_CMX_5P0
  audfmt_hq_prog_def_struct_type *prog_ptr;
#else /* FEATURE_AUDIO_CMX_5P0 */
  audfmt_prog_def_struct_type    *prog_ptr;
#endif /* FEATURE_AUDIO_CMX_5P0 */
  audfmt_status_type             status;

  boolean skip_chunk;
  uint32  chunk_length, inst_error, drum_error, index;
  uint16  sampling_rate, loop_start, loop_end;
  uint8   *buf, inst, drum, bank;

  /* Process until at end of VOIC chunk (delta_tick holds the location of
  ** end of VOIC chunk) or until not enough data to process next system
  ** exclusive message
  */
  while((track->current_location < track->delta_tick) &&
        (track->buf_size > (track->index + 8))) {
    buf = &track->track_buf[track->index];
    chunk_length = buf[4];
    chunk_length = chunk_length<<8 | buf[5];
    chunk_length = chunk_length<<8 | buf[6];
    chunk_length = chunk_length<<8 | buf[7];

    if(strncmp("EXVO", (char *) buf, 4) == 0) {
      switch(midi_ctl->smaf_phrase_type) {
        case AUDFMT_SMAF_PHRASE_L1:
          if(midi_ctl->smaf_fm_dls_index < 4) {
            fm_dls = &midi_ctl->smaf_fm_dls[midi_ctl->smaf_fm_dls_index];
            if(chunk_length == 0x15) {
              if(track->buf_size > (track->index + 8 + 0x15)) {
                if((buf[8] == 0xff)  && (buf[9] == 0xf0) &&
                   (buf[10] == 0x12) && (buf[11] == 0x43) &&
                   (buf[12] == 0x03)) {
                  inst = smaf_phrase_closest_instrument(
                                    smaf_phrase_get_fm_dls_error, &buf[16],
                                    12, audfmt_smaf_hp_dls_lookup[0], 128,
                                    &inst_error);

                  if(inst_error == 0) {
                    fm_dls->bank_msb   = 0;
                    fm_dls->program    = midi_ctl->smaf_fm_dls_index;
                    fm_dls->instrument = inst;
                    midi_ctl->smaf_fm_dls_index++;
                  } else {
                    drum = smaf_phrase_closest_instrument(
                                    smaf_phrase_get_fm_dls_error, &buf[16],
                                    12, audfmt_smaf_hp_drum_lookup[0], 61,
                                    &drum_error);

                    if((drum_error == 0) ||
                       !SMAF_USE_INSTR_2OP(drum_error, inst_error)) {
                      fm_dls->bank_msb   = 0x80;
                      fm_dls->program    = midi_ctl->smaf_fm_dls_index;
                      fm_dls->instrument = smaf_map_fm_dls_drum(drum + 24,
                                                 audfmt_smaf_hp_drum_range_map);
                      midi_ctl->smaf_fm_dls_index++;
                    } else {
                      fm_dls->bank_msb   = 0;
                      fm_dls->program    = midi_ctl->smaf_fm_dls_index;
                      fm_dls->instrument = inst;
                      midi_ctl->smaf_fm_dls_index++;
                    }
                  }

                  track->index            += 0x15 + 8;
                  track->current_location += 0x15 + 8;
                  break;
                }
              } else {
                break;
              }
            } else if(chunk_length == 0x1f) {
              if(track->buf_size > (track->index + 8 + 0x1f)) {
                if((buf[8] == 0xff)  && (buf[9] == 0xf0) &&
                   (buf[10] == 0x1c) && (buf[11] == 0x43) &&
                   (buf[12] == 0x03)) {
                  inst = smaf_phrase_closest_instrument(
                                    smaf_phrase_get_fm_dls_error, &buf[16], 22,
                                    audfmt_smaf_phrase_instr_dls_lookup_4op[0],
                                    128, &inst_error);

                  if(inst_error == 0) {
                    fm_dls->bank_msb   = 0;
                    fm_dls->program    = midi_ctl->smaf_fm_dls_index;
                    fm_dls->instrument = inst;
                    midi_ctl->smaf_fm_dls_index++;
                  } else {
                    drum = smaf_phrase_closest_instrument(
                                    smaf_phrase_get_fm_dls_error, &buf[16], 22,
                                    audfmt_smaf_phrase_drum_dls_lookup_4op[0],
                                    61, &drum_error);

                    if((drum_error == 0) ||
                       !SMAF_USE_INSTR_4OP(drum_error, inst_error)) {
                      fm_dls->bank_msb   = 0x80;
                      fm_dls->program    = midi_ctl->smaf_fm_dls_index;
                      fm_dls->instrument = smaf_map_fm_dls_drum(drum + 24,
                                        audfmt_smaf_phrase_drum_4op_range_map);
                      midi_ctl->smaf_fm_dls_index++;
                    } else {
                      fm_dls->bank_msb   = 0;
                      fm_dls->program    = midi_ctl->smaf_fm_dls_index;
                      fm_dls->instrument = inst;
                      midi_ctl->smaf_fm_dls_index++;
                    }
                  }

                  track->index            += 0x1f + 8;
                  track->current_location += 0x1f + 8;
                  break;
                }
              } else {
                break;
              }
            } else if(chunk_length == 0x17) {
              if(track->buf_size > (track->index + 8 + 0x17)) {
                if((buf[8] == 0xff)  && (buf[9] == 0xf0) &&
                   (buf[10] == 0x14) && (buf[11] == 0x43) &&
                   (buf[12] == 0x04)) {
                  bank = 0;
                  inst = smaf_phrase_closest_instrument(
                                    smaf_get_fm_dls_error, &buf[15], 15,
                                    audfmt_smaf_fm_2op_dls_lookup[0],
                                    128, &inst_error);

                  if(inst_error != 0) {
                    drum = smaf_phrase_closest_instrument(
                                    smaf_get_fm_dls_error, &buf[15], 15,
                                    audfmt_smaf_fm_2op_dls_lookup2[0],
                                    128, &drum_error);

                    if(drum_error < inst_error) {
                      inst       = drum;
                      inst_error = drum_error;
                    }
                  }

                  if(inst_error != 0) {
                    for(index = 0; index < 61; index++) {
                      if(memcmp(&buf[15],
                                &audfmt_l1_ma3_2op_drum_dls_lookup[index],
                                15) == 0) {
                        inst = smaf_map_fm_dls_drum(index + 24,
                                             audfmt_l1_ma3_2op_drum_range_map);
                        bank = 0x80;
                        break;
                      }
                    }
                  }
                  fm_dls->bank_msb   = bank;
                  fm_dls->program    = midi_ctl->smaf_fm_dls_index;
                  fm_dls->instrument = inst;
                  midi_ctl->smaf_fm_dls_index++;

                  track->index            += 0x17 + 8;
                  track->current_location += 0x17 + 8;
                  break;
                }
              } else {
                break;
              }
            } else if(chunk_length == 0x25) {
              if(track->buf_size > (track->index + 8 + 0x25)) {
                if((buf[8] == 0xff)  && (buf[9] == 0xf0) &&
                   (buf[10] == 0x22) && (buf[11] == 0x43) &&
                   (buf[12] == 0x04)) {
                  bank = 0;
                  inst = smaf_phrase_closest_instrument(
                                    smaf_get_fm_dls_error, &buf[15], 29,
                                    audfmt_smaf_fm_4op_dls_lookup[0],
                                    128, &inst_error);

                  if(inst_error != 0) {
                    drum = smaf_phrase_closest_instrument(
                                    smaf_get_fm_dls_error, &buf[15], 29,
                                    audfmt_smaf_fm_4op_dls_lookup2[0],
                                    3, &drum_error);

                    if(drum_error < inst_error) {
                      inst_error = drum_error;
                      if(drum == 0) {
                        inst = 0x41;
                      } else if(drum == 1) {
                        inst = 0x42;
                      } else if(drum == 2) {
                        inst = 0x46;
                      }
                    }
                  }

                  if(inst_error != 0) {
                    for(index = 0; index < 61; index++) {
                      if(memcmp(&buf[15],
                                &audfmt_l1_ma3_4op_drum_dls_lookup[index],
                                29) == 0) {
                        inst = smaf_map_fm_dls_drum(index + 24,
                                             audfmt_l1_ma3_4op_drum_range_map);
                        bank = 0x80;
                        break;
                      }
                    }
                  }
                  fm_dls->bank_msb   = bank;
                  fm_dls->program    = midi_ctl->smaf_fm_dls_index;
                  fm_dls->instrument = inst;
                  midi_ctl->smaf_fm_dls_index++;

                  track->index            += 0x25 + 8;
                  track->current_location += 0x25 + 8;
                  break;
                }
              } else {
                break;
              }
            }

            fm_dls->bank_msb   = 0;
            fm_dls->program    = midi_ctl->smaf_fm_dls_index;
            fm_dls->instrument = midi_ctl->smaf_fm_dls_index;
            midi_ctl->smaf_fm_dls_index++;
          }
          track->index            += chunk_length + 8;
          track->current_location += chunk_length + 8;
          break;

        case AUDFMT_SMAF_PHRASE_L2:
          if(midi_ctl->smaf_fm_dls_index < 16) {
            fm_dls = &midi_ctl->smaf_fm_dls[midi_ctl->smaf_fm_dls_index];
            if(chunk_length == 0x1a) {
              if(track->buf_size > (track->index + 8 + 0x1a)) {
                if((buf[8] == 0xff)  && (buf[9] == 0xf0) &&
                   (buf[10] == 0x17) && (buf[11] == 0x43) &&
                   (buf[12] == 0x05)) {
                  if(buf[14] >= 0x80) {
                    inst = smaf_phrase_closest_instrument(
                                    smaf_get_fm_dls_drum_error, &buf[16], 17,
                                    audfmt_smaf_fm_2op_drum_dls_lookup[0],
                                    40, &inst_error);

                    inst = audfmt_smaf_fm_drum_dls_map[inst];
                    inst = smaf_map_fm_dls_drum(inst,
                                              audfmt_l1_ma3_2op_drum_range_map);
                  } else {
                    inst = smaf_phrase_closest_instrument(
                                    smaf_get_fm_dls_error, &buf[18], 15,
                                    audfmt_smaf_fm_2op_dls_lookup[0],
                                    128, &inst_error);

                    if(inst_error != 0) {
                      drum = smaf_phrase_closest_instrument(
                                      smaf_get_fm_dls_error, &buf[18], 15,
                                      audfmt_smaf_fm_2op_dls_lookup2[0],
                                      128, &drum_error);

                      if(drum_error < inst_error) {
                        inst = drum;
                      }
                    }
                  }

                  fm_dls->bank_msb   = buf[14];
                  fm_dls->bank_lsb   = 0;
                  fm_dls->program    = buf[15];
                  fm_dls->instrument = inst;
                  midi_ctl->smaf_fm_dls_index++;
                }
              } else {
                break;
              }
            } else if(chunk_length == 0x28) {
              if(track->buf_size > (track->index + 8 + 0x28)) {
                if((buf[8] == 0xff)  && (buf[9] == 0xf0) &&
                   (buf[10] == 0x25) && (buf[11] == 0x43) &&
                   (buf[12] == 0x05)) {
                  if(buf[14] >= 0x80) {
                    inst = smaf_phrase_closest_instrument(
                                    smaf_get_fm_dls_drum_error, &buf[16], 31,
                                    audfmt_smaf_fm_4op_drum_dls_lookup[0],
                                    40, &inst_error);

                    inst = audfmt_smaf_fm_drum_dls_map[inst];
                    inst = smaf_map_fm_dls_drum(inst,
                                              audfmt_l1_ma3_4op_drum_range_map);
                  } else {
                    inst = smaf_phrase_closest_instrument(
                                    smaf_get_fm_dls_error, &buf[18], 29,
                                    audfmt_smaf_fm_4op_dls_lookup[0],
                                    128, &inst_error);

                    if(inst_error != 0) {
                      drum = smaf_phrase_closest_instrument(
                                      smaf_get_fm_dls_error, &buf[18], 29,
                                      audfmt_smaf_fm_4op_dls_lookup2[0],
                                      3, &drum_error);

                      if(drum_error < inst_error) {
                        if(drum == 0) {
                          inst = 0x41;
                        } else if(drum == 1) {
                          inst = 0x42;
                        } else if(drum == 2) {
                          inst = 0x46;
                        }
                      }
                    }
                  }

                  fm_dls->bank_msb   = buf[14];
                  fm_dls->bank_lsb   = 0;
                  fm_dls->program    = buf[15];
                  fm_dls->instrument = inst;
                  midi_ctl->smaf_fm_dls_index++;
                }
              } else {
                break;
              }
            } else if(chunk_length == 0x19) {
              if(track->buf_size > (track->index + 8 + 0x19)) {
                if((buf[8] == 0xff)  && (buf[9] == 0xf0) &&
                   (buf[10] == 0x16) && (buf[11] == 0x43) &&
                   (buf[12] == 0x05)) {
                  if(buf[31] >= 0x80) {
                    for(inst = 0; inst < 21; inst++) {
                      if(memcmp(&buf[16],
                                &audfmt_smaf_pcm_drum_dls_lookup[inst],
                                16) == 0) {
                        drum = audfmt_smaf_pcm_drum_dls_map[inst];

                        fm_dls->bank_msb   = buf[14];
                        fm_dls->bank_lsb   = 0;
                        fm_dls->program    = buf[15];
                        fm_dls->instrument = drum;
                        midi_ctl->smaf_fm_dls_index++;
                        break;
                      }
                    }

                    if(inst == 21) {
                      fm_dls->bank_msb   = buf[14];
                      fm_dls->bank_lsb   = 0;
                      fm_dls->program    = buf[15];
                      fm_dls->instrument = buf[15] + SMAF_HP_NOTE_OFFSET;
                      midi_ctl->smaf_fm_dls_index++;
                    }
                  } else {
                    sampling_rate = (buf[16] << 8) | buf[17];
                    loop_start    = (buf[27] << 8) | buf[28];
                    loop_end      = (buf[29] << 8) | buf[30];

                    wave_param_ptr = audfmt_dls_struct.wave_param;
                    in_use_wave    = NULL;
                    res_wave       = NULL;
                    for(index = 0; index < 128; index++) {
                      if(wave_param_ptr->wave_id == buf[31]) {
                        if(wave_param_ptr->in_use_flag ==
                                     audfmt_prog_def_seq[midi_ctl->sequence]) {
                          res_wave = wave_param_ptr;
                          break;
                        } else if(wave_param_ptr->in_use_flag ==
                              audfmt_prog_def_in_use_seq[midi_ctl->sequence]) {
                          if(in_use_wave != NULL) {
                            if((wave_param_ptr->sampling_freq ==
                                sampling_rate) &&
                               (wave_param_ptr->loop_start == loop_start) &&
                               (wave_param_ptr->loop_end == loop_end)) {
                              in_use_wave = wave_param_ptr;
                            }
                          } else {
                            in_use_wave = wave_param_ptr;
                          }
                        }
                      }
                      wave_param_ptr++;
                    }

                    if((res_wave != NULL) ||
                       ((in_use_wave != NULL) &&
                        (wave_param_ptr->sampling_freq == sampling_rate) &&
                        (wave_param_ptr->loop_start == loop_start) &&
                        (wave_param_ptr->loop_end == loop_end))) {
                      param_ptr = audfmt_dls_struct.param_def;
                      /* Find a free inst parameter definition structure */
                      for(index = 0; index < 128; index++) {
                        if(param_ptr->in_use_flag == AUDFMT_PROG_DEF_FREE) {
                          break;
                        }
                        param_ptr++;
                      }
                      if(index < 128) {
                        /* Mark this instrument definition structure as */
                        /* reserved                                     */
                        param_ptr->in_use_flag = audfmt_prog_def_seq
                                                         [midi_ctl->sequence];

                        prog_ptr = &audfmt_dls_struct.prog_def[index];

                        smaf_dls_inst_def(param_ptr, prog_ptr, &buf[16]);

                        param_ptr->wave_id = index;

                        if(res_wave != NULL) {
                          status = smaf_finalize_pcm_dls(midi_ctl, param_ptr,
                                                         res_wave);
                        } else {
                          param_ptr->in_use_flag = audfmt_prog_def_in_use_seq
                                                          [midi_ctl->sequence];

#ifdef FEATURE_AUDIO_CMX_5P0
						  prog_ptr->wf_info_ptr  = 
                              &audfmt_dls_struct.wave_info[in_use_wave->wave_entry];
#else
                          prog_ptr->waveform_id  = in_use_wave->wave_entry;
#endif /* FEATURE_AUDIO_CMX_5P0 */
                          status = AUDFMT_STATUS_SUCCESS;
                        }

                        if(status == AUDFMT_STATUS_SUCCESS) {
                          fm_dls->bank_msb   = buf[14];
                          fm_dls->bank_lsb   = 0x80;
                          fm_dls->program    = buf[15];
                          fm_dls->instrument = index;
                          midi_ctl->smaf_fm_dls_index++;
                        }
                      }
                    } else if(in_use_wave != NULL) {
                      /* Find a free waveform structure */
                      wave_param_ptr = audfmt_dls_struct.wave_param;
                      for(index = 0; index < 128; index++) {
                        if(wave_param_ptr->in_use_flag == AUDFMT_PROG_DEF_FREE)
                        {
                          break;
                        }
                        wave_param_ptr++;
                      }

                      if(index < 128) {
                        /* Mark this waveform structure as reserved */
                        wave_param_ptr->in_use_flag =
                                audfmt_prog_def_seq[midi_ctl->sequence];

                        wave_param_ptr->wave_entry   = index;
                        wave_param_ptr->inst_entry   = 0xff;
                        wave_param_ptr->wave_id      = in_use_wave->wave_id;
                        wave_param_ptr->wave_encoded = FALSE;
                        wave_param_ptr->drum_tune    = FALSE;
                        wave_param_ptr->location     = in_use_wave->location;
                        wave_param_ptr->size         = in_use_wave->size;
                        wave_param_ptr->encoded_waveform = NULL;
                        wave_param_ptr->decoded_waveform = NULL;

                        /* Do callback to retrieve waveform data */
                        audmain_get_dls(midi_ctl->handle,
                                        wave_param_ptr->location,
                                        wave_param_ptr->size, wave_param_ptr);

                        track->track_status |= AUDFMT_TRACK_MASK_DO_VOIC_CONT;

                        /* Return for now to process waveform data callback.
                        ** The rest of VOIC chunk will be processed after
                        ** waveform data is processed.
                        */
                        return(AUDFMT_STATUS_SUCCESS);
                      }
                    }
                  }
                }
              } else {
                break;
              }
            }
          }
          track->index            += chunk_length + 8;
          track->current_location += chunk_length + 8;
          break;

        default:
          track->index            += chunk_length + 8;
          track->current_location += chunk_length + 8;
          break;
      }
    } else if(strncmp("EXWV", (char *) buf, 4) == 0) {
      /* Find a free waveform structure */
      wave_param_ptr = audfmt_dls_struct.wave_param;
      for(index = 0; index < 128; index++) {
        if(wave_param_ptr->in_use_flag == AUDFMT_PROG_DEF_FREE) {
          break;
        }
        wave_param_ptr++;
      }

      if(index < 128) {
        inst_error = chunk_length - 4;
        if(track->buf_size > (track->index + 8 + 8)) {
          if((buf[8] == 0xff) && (buf[9] == 0xf1) &&
             (buf[10] == (inst_error & 0xff)) &&
             (buf[11] == ((inst_error >> 8) & 0xff)) &&
             (buf[12] == 0x43) && (buf[13] == 0x05)) {
            /* Mark this waveform structure as reserved */
            wave_param_ptr->in_use_flag =
                            audfmt_prog_def_seq[midi_ctl->sequence];

            wave_param_ptr->wave_entry   = index;
            wave_param_ptr->inst_entry   = 0xff;
            wave_param_ptr->wave_id      = buf[15];
            wave_param_ptr->wave_encoded = FALSE;
            wave_param_ptr->drum_tune    = FALSE;
            wave_param_ptr->location     = track->current_location + 16;
            wave_param_ptr->size         = chunk_length - 8;
            wave_param_ptr->encoded_waveform = NULL;
            wave_param_ptr->decoded_waveform = NULL;

            /* Do callback to retrieve waveform data */
            audmain_get_dls(midi_ctl->handle, wave_param_ptr->location,
                            wave_param_ptr->size, wave_param_ptr);

            track->track_status |= AUDFMT_TRACK_MASK_DO_VOIC_CONT;

            /* Update data pointer position */
            track->index            += chunk_length + 8;
            track->current_location += chunk_length + 8;

            /* Return for now to process waveform data callback.
            ** The rest of Mtsu chunk will be processed after waveform
            ** data is processed.
            */
            return(AUDFMT_STATUS_SUCCESS);
          }
        }
      }
      track->index            += chunk_length + 8;
      track->current_location += chunk_length + 8;
    } else if(strncmp("DEVO", (char *) buf, 4) == 0) {
      skip_chunk = TRUE;
      if(midi_ctl->smaf_phrase_type == AUDFMT_SMAF_PHRASE_L1) {
        if(midi_ctl->smaf_fm_dls_index < 4) {
          fm_dls = &midi_ctl->smaf_fm_dls[midi_ctl->smaf_fm_dls_index];

          if(chunk_length == 0x01) {
            if(track->buf_size > (track->index + 8 + 1)) {
              fm_dls->bank_msb   = 0;
              fm_dls->program    = midi_ctl->smaf_fm_dls_index;
              fm_dls->instrument = buf[8];
              midi_ctl->smaf_fm_dls_index++;
            } else {
              skip_chunk = FALSE;
            }
          } else {
            fm_dls->bank_msb   = 0;
            fm_dls->program    = midi_ctl->smaf_fm_dls_index;
            fm_dls->instrument = midi_ctl->smaf_fm_dls_index;
            midi_ctl->smaf_fm_dls_index++;
          }
        }
      }
      if(skip_chunk) {
        track->index            += chunk_length + 8;
        track->current_location += chunk_length + 8;
      }
    } else {
      track->index            += chunk_length + 8;
      track->current_location += chunk_length + 8;

      if(track->current_location > track->delta_tick) {
        /* Error encountered, skip to end of VOIC */
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                          (void *) track->track_buf);
        track->track_buf = NULL;
        track->buf_size  = 0;
        track->current_location = track->delta_tick;
        audmain_get_header_data(midi_ctl->handle, track->current_location,
                                &(track->track_buf));
        return(AUDFMT_STATUS_SUCCESS);
      }
    }
  }

  if(track->current_location == track->delta_tick) {
    /* Reached end of VOIC chunk (delta_tick contains location of end of
    ** VOIC chunk). Clear delta_tick parameter so that it can be used for
    ** its intended use.
    */
    wave_param_ptr = audfmt_dls_struct.wave_param;
    for(index = 0; index < 128; index++) {
      if(wave_param_ptr->in_use_flag ==
         audfmt_prog_def_seq[midi_ctl->sequence]) {
        wave_param_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
        if(wave_param_ptr->encoded_waveform != NULL) {
          audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_DLS, 0,
                            (void *) wave_param_ptr->encoded_waveform);
        }
        if(wave_param_ptr->decoded_waveform != NULL) {
          audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_DLS, 0,
                            (void *) wave_param_ptr->decoded_waveform);
        }
      }
      wave_param_ptr++;
    }

    track->delta_tick = 0;
    return(AUDFMT_STATUS_DONE);
  } else {
    /* Not at end of VOIC chunk, free current buffer and get next buffer */
    audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                      (void *) track->track_buf);
    track->track_buf = NULL;
    track->buf_size  = 0;

    audmain_get_header_data(midi_ctl->handle, track->current_location,
                            &(track->track_buf));
    return(AUDFMT_STATUS_SUCCESS);
  }
} /* end smaf_do_voic */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_do_mtsp

DESCRIPTION
  This function processes the Mtsp (searching for Mwa* chunks) and 
  ATR* (searching for Awa* chunks) SMAF chunks. These chunks contain the
  PCM data chunks for sound effects.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_do_mtsp (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track_ptr,
  uint32            position
) {
  uint32 index = 0, chunk_length;
  uint8  *buf = track_ptr->track_buf_reserve;

  /* Data for locating the Awa* and Mwa* chunks is contained in the 
  ** reserve buffer space. The regular track_buf buffer currently still
  ** contains data required to continue parsing header after these chunks
  ** are located.
  */
  /* Parse until at end of Mtsp/ATR* chunk or until there is insufficient
  ** data to process Mwa* and Awa* chunks
  */
  while((index < track_ptr->buf_reserve_size) && 
        ((index + position + 8) < track_ptr->track_end)) {
    if(track_ptr->track_status & AUDFMT_TRACK_MASK_PCM_TRACK) {
      /* This is ATR* chunk (PCM track for HP format) */
      if(strncmp("Awa", (char *) buf, 3) == 0) {
        chunk_length   = buf[4];
        chunk_length   = chunk_length<<8 | buf[5];
        chunk_length   = chunk_length<<8 | buf[6];
        chunk_length   = chunk_length<<8 | buf[7];

        if((buf[3] != 0) && (buf[3] < 0x3f)) {
          index += 8;
          midi_ctl->wav_store[buf[3]].num_bytes = chunk_length;
          midi_ctl->wav_store[buf[3]].location  = position + index;
          index += chunk_length;
          buf   += chunk_length + 8;
        } else {
          return(AUDFMT_STATUS_FAILURE);
        }
      } else {
        /* Unrecognized chunk, end search for Awa* chunks */
		/*
		  Track buffer contains MTSQ events and reserve buffer holds
		  MWA data.Track buffer should not be freed.
		*/
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                          (void *) track_ptr->track_buf_reserve);
        track_ptr->buf_reserve_size   = 0;
        track_ptr->track_buf_reserve  = NULL;

        return(AUDFMT_STATUS_DONE);
      }
    } else {
      /* Mtsp chunk (PCM data chunk for mobile format) */
      if(strncmp("Mtsp", (char *) buf, 4) == 0) {
        index += 8;
        buf   += 8;
      } else if(strncmp("Mwa", (char *) buf, 3) == 0) {
        chunk_length   = buf[4];
        chunk_length   = chunk_length<<8 | buf[5];
        chunk_length   = chunk_length<<8 | buf[6];
        chunk_length   = chunk_length<<8 | buf[7];
        if((buf[3] != 0) && (buf[3] < 0x3f)) {
          index += 8;
          midi_ctl->wav_store[buf[3] - 1].num_bytes    = chunk_length - 3;
          midi_ctl->wav_store[buf[3] - 1].location     = position + index + 3;
          midi_ctl->wav_spec[buf[3] - 1].format        = buf[8];
          midi_ctl->wav_spec[buf[3] - 1].sampling_rate = buf[9] << 8 | buf[10];
          midi_ctl->wav_spec[buf[3] - 1].mode_3d       = FALSE;
          index += chunk_length;
          buf   += chunk_length + 8;
        } else {
          return(AUDFMT_STATUS_FAILURE);
        }
      } else {
        /* Unrecognized chunk, end search for Mwa* chunks */
		/*
		  Track buffer contains MTSQ events and reserve buffer holds
		  MWA data.
		*/ 
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                          (void *) track_ptr->track_buf_reserve);
        track_ptr->buf_reserve_size   = 0;
        track_ptr->track_buf_reserve  = NULL;

        return(AUDFMT_STATUS_DONE);
      }
    }
  }

  /* If dropped out, then free current buffer */
  audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                    (void *) track_ptr->track_buf_reserve);
  track_ptr->track_buf_reserve  = NULL;
  track_ptr->buf_reserve_size   = 0;

  /* If not at end of Mtsp/ATR* chunk, get next data buffer */
  if((index + position + 8) < track_ptr->track_end) {
    audmain_get_header_data(midi_ctl->handle, position + index,
                            &(track_ptr->track_buf));
    return(AUDFMT_STATUS_SUCCESS);
  } else {
    return(AUDFMT_STATUS_DONE);
  }
} /* end smaf_do_mtsp */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_do_track

DESCRIPTION
  This function processes the MTR* and ATR* track chunks to locate the main
  sequence chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void smaf_do_track (
  audfmt_parse_type *midi_ctl,
  uint8             *buf,
  uint32            length,
  uint32            pos
) {
  audfmt_status_type status;
  audfmt_track_type  *track_ptr;
  uint32             chunk_length;
  uint8              *buffer, timebase_d, timebase_g;
  boolean            pcm_track;
  uint16             counter, index;

  track_ptr = &midi_ctl->tracks[midi_ctl->current_header_track];

  if(!(track_ptr->track_status & AUDFMT_TRACK_MASK_INIT_DONE)) {
    /* Check that the chunk ID matches what is expected and that we 
    ** have enough data to process the beginning of this chunk
    */
    if(((strncmp(AUDFMT_MTR_ID, (char *) buf, 3) == 0) ||
        (strncmp("ATR", (char *) buf, 3) == 0)) && (length >= 28)) {
      pcm_track = (strncmp("ATR", (char *) buf, 3) == 0);

      if(pcm_track) {
        timebase_d = buf[12];
        timebase_g = buf[13];

        midi_ctl->wav_spec[0].format = (((buf[11] >> 4) & 0xf) |
                                        (buf[10] & 0xf0));
        switch(buf[10] & 0xf) {
          case 0:
            midi_ctl->wav_spec[0].sampling_rate = 4000;
            break;
          case 1:
            midi_ctl->wav_spec[0].sampling_rate = 8000;
            break;
          case 2:
            midi_ctl->wav_spec[0].sampling_rate = 11025;
            break;
          case 3:
            midi_ctl->wav_spec[0].sampling_rate = 22050;
            break;
          case 4:
            midi_ctl->wav_spec[0].sampling_rate = 44100;
            break;
          default:
            audfmt_header_do_failure(midi_ctl);
            return;
            break;
        }
      } else {
        timebase_d = buf[10];
        timebase_g = buf[11];
      }

      /* Get chunk length */
      chunk_length   = buf[4];
      chunk_length   = chunk_length<<8 | buf[5];
      chunk_length   = chunk_length<<8 | buf[6];
      chunk_length   = chunk_length<<8 | buf[7];

      if(midi_ctl->smaf_type == AUDFMT_SMAF_INVALID_TYPE) {
        /* Get track information from track header */
        switch(buf[8]) {
          case 0:
            midi_ctl->smaf_type = AUDFMT_SMAF_HP_TYPE;
            break;
          case 1:
            midi_ctl->smaf_type = AUDFMT_SMAF_COMPRESSED_TYPE;
            break;
          case 2:
            midi_ctl->smaf_type = AUDFMT_SMAF_MOBILE_TYPE;
            break;
          default:
            audfmt_header_do_failure(midi_ctl);
            return;
            break;
        }
        switch (timebase_d) {
          case 0:
            midi_ctl->smaf_timebase_d = 1;
            break;
          case 1:
            midi_ctl->smaf_timebase_d = 2;
            break;
          case 2:
            midi_ctl->smaf_timebase_d = 4;
            break;
          case 3:
            midi_ctl->smaf_timebase_d = 5;
            break;
          case 0x10:
            midi_ctl->smaf_timebase_d = 10;
            break;
          case 0x11:
            midi_ctl->smaf_timebase_d = 20;
            break;
          case 0x12:
            midi_ctl->smaf_timebase_d = 40;
            break;
          case 0x13:
            midi_ctl->smaf_timebase_d = 50;
            break;
          default:
            audfmt_header_do_failure(midi_ctl);
            return;
            break;
        }
        midi_ctl->calc_ms_of   = 0x100000;
        midi_ctl->calc_tick_of = 0x100000 * midi_ctl->smaf_timebase_d;
        switch (timebase_g) {
          case 0:
            midi_ctl->smaf_timebase_g = 1;
            break;
          case 1:
            midi_ctl->smaf_timebase_g = 2;
            break;
          case 2:
            midi_ctl->smaf_timebase_g = 4;
            break;
          case 3:
            midi_ctl->smaf_timebase_g = 5;
            break;
          case 0x10:
            midi_ctl->smaf_timebase_g = 10;
            break;
          case 0x11:
            midi_ctl->smaf_timebase_g = 20;
            break;
          case 0x12:
            midi_ctl->smaf_timebase_g = 40;
            break;
          case 0x13:
            midi_ctl->smaf_timebase_g = 50;
            break;
          default:
            audfmt_header_do_failure(midi_ctl);
            return;
            break;
        }
      } else if(midi_ctl->smaf_type == AUDFMT_SMAF_HP_TYPE) {
        /* HP format is only one that can have multiple tracks. All tracks
        ** still have to have common features like timebase
        */
// check for invalid status
      } else {
        audfmt_header_do_failure(midi_ctl);
        return;
      }

      if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_GET_SPEC))) {
        /* Led and Vibrator controls */
        if(midi_ctl->smaf_type == AUDFMT_SMAF_HP_TYPE) {
          if(midi_ctl->current_header_track < 4) {
            counter = (1 << (midi_ctl->current_header_track << 2));

            if((buf[12] & SMAF_HP_VIB_MASK1) == SMAF_HP_VIB_MASK1) {
              midi_ctl->smaf_vib_ctl |= counter;
            }
            counter <<= 1;

            if((buf[12] & SMAF_HP_VIB_MASK2) == SMAF_HP_VIB_MASK2) {
              midi_ctl->smaf_vib_ctl |= counter;
            }
            counter <<= 1;

            if((buf[13] & SMAF_HP_VIB_MASK1) == SMAF_HP_VIB_MASK1) {
              midi_ctl->smaf_vib_ctl |= counter;
            }
            counter <<= 1;

            if((buf[13] & SMAF_HP_VIB_MASK2) == SMAF_HP_VIB_MASK2) {
              midi_ctl->smaf_vib_ctl |= counter;
            }
          }
          /* no led control for AUDFMT_SMAF_HP_TYPE */
        } else {
          index  = 1;
          buffer = &buf[12];
          for(counter = 0; counter < AUDFMT_MAX_CHANNELS; counter++) {
            if((*buffer & SMAF_MOBILE_VIB_MASK) == SMAF_MOBILE_VIB_MASK) {
              midi_ctl->smaf_vib_ctl |= index;
            }
            if((*buffer & SMAF_MOBILE_LED_MASK) == SMAF_MOBILE_LED_MASK) {
              midi_ctl->smaf_led_ctl |= index;
            }
            index <<= 1;
            buffer++;
          }
        }

        /* If this SMAF or HP file has graphics/text/LED commands,
        */
        if (((midi_ctl->smaf_led_ctl) || (midi_ctl->smaf_vib_ctl)) && 
            !audfmt_cb_store.used) {
          audfmt_cb_store_reset(&audfmt_cb_store);
          audfmt_cb_store.used = TRUE;
          midi_ctl->cb_store   = &audfmt_cb_store;
        }
      }

      track_ptr->audfmt_handle       = midi_ctl->handle;
      track_ptr->buf_size            = length;
      track_ptr->file_location       = pos + length;
      track_ptr->running_status      = 0;
      track_ptr->skip_buf_count      = 0;
      track_ptr->delta_tick          = 0;
      track_ptr->num_compressed_bits = 0;
      track_ptr->track_status        = AUDFMT_TRACK_MASK_DO_TIME |
                                       AUDFMT_TRACK_MASK_INIT_DONE;
      track_ptr->track_buf           = buf;
      track_ptr->track_buf_reserve   = NULL;
      track_ptr->buf_reserve_size    = 0;
      track_ptr->track_end           = pos + 8 + chunk_length;
      track_ptr->start_offset        = 0;
      track_ptr->stop_offset         = AUDFMT_MAX_32BIT;
      track_ptr->current_offset      = 0;

      /* Set data position to end of track header */
      if(midi_ctl->smaf_type == AUDFMT_SMAF_HP_TYPE) {
        track_ptr->current_location  = pos + 14;
        track_ptr->index             = 14;
      } else {
        track_ptr->current_location  = pos + 28;
        track_ptr->index             = 28;
      }

      if(pcm_track) {
        track_ptr->track_status |= AUDFMT_TRACK_MASK_PCM_TRACK;
      } else {
        /* Initialize huffman table variables */
        midi_ctl->smaf_huffman_table_entry  = midi_ctl->smaf_huffman_table;
        midi_ctl->smaf_current_code         = 0;
        midi_ctl->smaf_code_length          = 0;
      }
    } else if((strncmp("MMMG", (char *) buf, 4) == 0) && (length >= 10)) {
      /* Get chunk length */
      chunk_length   = buf[4];
      chunk_length   = chunk_length<<8 | buf[5];
      chunk_length   = chunk_length<<8 | buf[6];
      chunk_length   = chunk_length<<8 | buf[7];

      midi_ctl->smaf_type = AUDFMT_SMAF_PHRASE_TYPE;

      switch(buf[8]) {
        case 1:
          midi_ctl->smaf_phrase_type = AUDFMT_SMAF_PHRASE_L1;
          break;
        case 2:
          midi_ctl->smaf_phrase_type = AUDFMT_SMAF_PHRASE_L2;
          break;
        default:
          midi_ctl->smaf_phrase_type = AUDFMT_SMAF_PHRASE_UNKNOWN;
          break;
      }

      midi_ctl->smaf_timebase_d = buf[9];

      midi_ctl->calc_ms_of   = 0x100000;
      midi_ctl->calc_tick_of = 0x100000 * midi_ctl->smaf_timebase_d;

      for(index = 0; index < AUDFMT_MAX_CHANNELS; index++) {
        midi_ctl->smaf_ma2_pitch[index].note    = 0x80;
        midi_ctl->smaf_ma2_pitch[index].c_value = 0x80;
        midi_ctl->smaf_ma2_pitch[index].b_value = 0x100;
      }

      track_ptr->audfmt_handle       = midi_ctl->handle;
      track_ptr->buf_size            = length;
      track_ptr->file_location       = pos + length;
      track_ptr->running_status      = 0;
      track_ptr->skip_buf_count      = 0;
      track_ptr->delta_tick          = 0;
      track_ptr->num_compressed_bits = 0;
      track_ptr->track_status        = AUDFMT_TRACK_MASK_DO_TIME |
                                       AUDFMT_TRACK_MASK_INIT_DONE;
      track_ptr->track_buf           = buf;
      track_ptr->track_buf_reserve   = NULL;
      track_ptr->buf_reserve_size    = 0;
      track_ptr->track_end           = pos + 8 + chunk_length;
      track_ptr->start_offset        = 0;
      track_ptr->stop_offset         = AUDFMT_MAX_32BIT;
      track_ptr->current_offset      = 0;

      /* Set data position to end of track header */
      track_ptr->current_location  = pos + 10;
      track_ptr->index             = 10;
    } else {
      audfmt_header_do_failure(midi_ctl);
      return;
    }
  } else if(track_ptr->track_status & AUDFMT_TRACK_MASK_DO_MTSU_CONT) {
    /* This status indicates that we have just returned from processing
    ** waveform data and we should continue parsing through the Mtsu
    ** chunk.
    */
    track_ptr->track_status     &= ~AUDFMT_TRACK_MASK_DO_MTSU_CONT;
    status = smaf_do_mtsu(midi_ctl, track_ptr);

    if(status == AUDFMT_STATUS_FAILURE) {
      audfmt_header_do_failure(midi_ctl);
      return;
    } else if(status == AUDFMT_STATUS_DONE) {
      track_ptr->track_status &= ~AUDFMT_TRACK_MASK_DO_MTSU;
    } else {
      return;
    }
  } else if(track_ptr->track_status & AUDFMT_TRACK_MASK_DO_MTSU) {
    /* This status indicates that we have just received a buffer for
    ** Mtsu chunk processing.
    */
    track_ptr->track_buf     = buf;
    track_ptr->buf_size      = length;
    track_ptr->file_location = pos + length;
    track_ptr->index         = 0;

    if(midi_ctl->smaf_type == AUDFMT_SMAF_HP_TYPE) {
      status = smaf_do_hp_mtsu(midi_ctl, track_ptr);
    } else {
      status = smaf_do_mtsu(midi_ctl, track_ptr);
    }

    if(status == AUDFMT_STATUS_FAILURE) {
      audfmt_header_do_failure(midi_ctl);
      return;
    } else if(status == AUDFMT_STATUS_DONE) {
      track_ptr->track_status &= ~AUDFMT_TRACK_MASK_DO_MTSU;
    } else {
      return;
    }
  } else if(track_ptr->track_status & AUDFMT_TRACK_MASK_DO_MSPI) {
    /* This status indicates that we have just received a buffer for
    ** MspI chunk processing.
    */
    track_ptr->track_buf     = buf;
    track_ptr->buf_size      = length;
    track_ptr->file_location = pos + length;
    track_ptr->index         = 0;

    status = smaf_do_mspi(midi_ctl, track_ptr);

    if(status == AUDFMT_STATUS_FAILURE) {
      audfmt_header_do_failure(midi_ctl);
      return;
    } else if(status == AUDFMT_STATUS_DONE) {
      track_ptr->track_status &= ~AUDFMT_TRACK_MASK_DO_MSPI;
    } else {
      return;
    }
  } else if(track_ptr->track_status & AUDFMT_TRACK_MASK_DO_VOIC_CONT) {
    track_ptr->track_status &= ~AUDFMT_TRACK_MASK_DO_VOIC_CONT;
    status = smaf_do_voic(midi_ctl, track_ptr);

    if(status == AUDFMT_STATUS_FAILURE) {
      audfmt_header_do_failure(midi_ctl);
      return;
    } else if(status == AUDFMT_STATUS_DONE) {
      track_ptr->track_status &= ~AUDFMT_TRACK_MASK_DO_VOIC;
    } else {
      return;
    }
  } else if(track_ptr->track_status & AUDFMT_TRACK_MASK_DO_VOIC) {
    /* This status indicates that we have just received a buffer for
    ** VOIC chunk processing.
    */
    track_ptr->track_buf     = buf;
    track_ptr->buf_size      = length;
    track_ptr->file_location = pos + length;
    track_ptr->index         = 0;
    status = smaf_do_voic(midi_ctl, track_ptr);

    if(status == AUDFMT_STATUS_FAILURE) {
      audfmt_header_do_failure(midi_ctl);
      return;
    } else if(status == AUDFMT_STATUS_DONE) {
      track_ptr->track_status &= ~AUDFMT_TRACK_MASK_DO_VOIC;
    } else {
      return;
    }
  } else if(track_ptr->track_status & AUDFMT_TRACK_MASK_DO_MTSP) {
    /* This status indicates that we have just received a buffer for
    ** Mtsp/ATR* chunk processing. Data buffers for this processing are
    ** kept in reserved buffer; current buffers contain data that is
    ** required to continue parsing SMAF header after Mtsp/ATR* processing.
    */
    track_ptr->track_buf_reserve = buf;
    track_ptr->buf_reserve_size  = length;
    status = smaf_do_mtsp (midi_ctl, track_ptr, pos);
    
    switch(status) {
      case AUDFMT_STATUS_DONE:
        /* Done searching for Mwa* and Awa* chunks; should continue with
        ** regular track parsing now.
        */
        track_ptr->track_status &= ~AUDFMT_TRACK_MASK_DO_MTSP;
        track_ptr->track_status |= AUDFMT_TRACK_MASK_MTSP_DONE;
        break;

      case AUDFMT_STATUS_FAILURE:
        audfmt_header_do_failure(midi_ctl);
        /* Fall thru */

      default:
        return;
    }
  } else if(track_ptr->track_status & AUDFMT_TRACK_MASK_DO_HUFFMAN) {
    /* This status indicates that we are currently generating huffman
    ** table. Two data buffers are used for this processing; new data
    ** buffer will be put in reserved buffer area.
    */
    track_ptr->track_buf_reserve = buf;
    track_ptr->buf_reserve_size  = length;
    track_ptr->file_location     = pos + length;

    status = smaf_generate_huffman_table (midi_ctl, track_ptr);

    if(status == AUDFMT_STATUS_FAILURE) {
      audfmt_header_do_failure(midi_ctl);
    } else if(status == AUDFMT_STATUS_DONE) {
      track_ptr->track_status &= ~AUDFMT_TRACK_MASK_DO_HUFFMAN;
      audfmt_check_next_track (midi_ctl, track_ptr);
    }
    return;
  } else {
    track_ptr->track_buf     = buf;
    track_ptr->buf_size      = length;
    track_ptr->file_location = pos + length;
    track_ptr->index         = 0;
  }

  /* Parse through chunks within this track chunk */
  while((track_ptr->current_location < (track_ptr->track_end - 8)) &&
        (track_ptr->index < track_ptr->buf_size) &&
        ((track_ptr->buf_size - track_ptr->index) > 8)) {

    buffer         = &(track_ptr->track_buf[track_ptr->index]);
    chunk_length   = buffer[4];
    chunk_length   = chunk_length<<8 | buffer[5];
    chunk_length   = chunk_length<<8 | buffer[6];
    chunk_length   = chunk_length<<8 | buffer[7];

    if((strncmp("Mtsu", (char *) buffer, AUDFMT_CHUNK_ID_BYTES) == 0) &&
       !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                  AUDFMT_MASK_GET_SPEC))) {
      track_ptr->current_location += 8;
      track_ptr->index            += 8;
      track_ptr->delta_tick        = track_ptr->current_location + chunk_length;
      track_ptr->track_status     |= AUDFMT_TRACK_MASK_DO_MTSU;

      if(midi_ctl->smaf_type == AUDFMT_SMAF_HP_TYPE) {
        status = smaf_do_hp_mtsu(midi_ctl, track_ptr);
      } else {
        status = smaf_do_mtsu(midi_ctl, track_ptr);
      }

      if(status == AUDFMT_STATUS_FAILURE) {
        audfmt_header_do_failure(midi_ctl);
        return;
      } else if(status == AUDFMT_STATUS_DONE) {
        track_ptr->track_status   &= ~AUDFMT_TRACK_MASK_DO_MTSU;
      } else {
        /* This status indicates that another buffer has been requested
        ** and Mtsu chunk will continue to be parsed when the new buffer is
        ** received
        */
        return;
      }
    } else if(((strncmp("MspI", (char *) buffer, AUDFMT_CHUNK_ID_BYTES) == 0)
            || (strncmp("AspI", (char *) buffer, AUDFMT_CHUNK_ID_BYTES) == 0))
            && !(midi_ctl->midi_status & (AUDFMT_MASK_GET_SPEC |
                                          AUDFMT_MASK_PRO_FOUND))) {
      midi_ctl->midi_status |= AUDFMT_MASK_MSPI_FOUND;

      track_ptr->current_location += 8;
      track_ptr->index            += 8;
      track_ptr->running_status    = 0;
      track_ptr->delta_tick        = track_ptr->current_location + chunk_length;
      track_ptr->track_status     |= AUDFMT_TRACK_MASK_DO_MSPI;

      status = smaf_do_mspi(midi_ctl, track_ptr);

      if(status == AUDFMT_STATUS_FAILURE) {
        audfmt_header_do_failure(midi_ctl);
        return;
      } else if(status == AUDFMT_STATUS_DONE) {
        track_ptr->track_status   &= ~AUDFMT_TRACK_MASK_DO_MSPI;
      } else {
        /* This status indicates that another buffer has been requested
        ** and MspI chunk will continue to be parsed when the new buffer is
        ** received
        */
        return;
      }
    } else if ((strncmp("VOIC", (char *) buffer, AUDFMT_CHUNK_ID_BYTES) == 0) &&
               (midi_ctl->smaf_type == AUDFMT_SMAF_PHRASE_TYPE) &&
               ((midi_ctl->smaf_phrase_type == AUDFMT_SMAF_PHRASE_L1) ||
                (midi_ctl->smaf_phrase_type == AUDFMT_SMAF_PHRASE_L2)) &&
               !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                          AUDFMT_MASK_GET_SPEC))) {
      track_ptr->current_location += 8;
      track_ptr->index            += 8;
      track_ptr->delta_tick        = track_ptr->current_location + chunk_length;
      track_ptr->track_status     |= AUDFMT_TRACK_MASK_DO_VOIC;

      status = smaf_do_voic(midi_ctl, track_ptr);

      if(status == AUDFMT_STATUS_FAILURE) {
        audfmt_header_do_failure(midi_ctl);
        return;
      } else if(status == AUDFMT_STATUS_DONE) {
        track_ptr->track_status   &= ~AUDFMT_TRACK_MASK_DO_VOIC;
      } else {
        /* This status indicates that another buffer has been requested
        ** and VOIC chunk will continue to be parsed when the new buffer is
        ** received
        */
        return;
      }
    } else if((!(track_ptr->track_status & AUDFMT_TRACK_MASK_PCM_TRACK) &&
               (strncmp(AUDFMT_MTSQ_ID, (char *) buffer,
                        AUDFMT_CHUNK_ID_BYTES) == 0)) ||
              ((track_ptr->track_status & AUDFMT_TRACK_MASK_PCM_TRACK) &&
               (strncmp("Atsq", (char *) buffer, AUDFMT_CHUNK_ID_BYTES) == 0))||
              ((midi_ctl->smaf_type == AUDFMT_SMAF_PHRASE_TYPE) &&
               (strncmp("SEQU", (char *) buffer, AUDFMT_CHUNK_ID_BYTES) == 0)))
    {
      /* Found the main sequence chunks Mtsq/Atsq within the track chunk */
      if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_GET_SPEC)) &&
         !(track_ptr->track_status & AUDFMT_TRACK_MASK_MTSP_DONE) &&
         ((chunk_length + track_ptr->current_location + 16) <
          track_ptr->track_end)) {
        /* If there is still data at end of track, then check if it is
        ** PCM data
        */
        track_ptr->track_status |= AUDFMT_TRACK_MASK_DO_MTSP;
        audmain_get_header_data(midi_ctl->handle,
                                chunk_length + track_ptr->current_location + 8,
                                &(track_ptr->track_buf));
      } else {
        /* Located main sequence data for this track */
        track_ptr->current_location += 8;
        track_ptr->index            += 8;
        track_ptr->track_end         = track_ptr->current_location +
                                       chunk_length;

        if(midi_ctl->smaf_type == AUDFMT_SMAF_COMPRESSED_TYPE) {
          /* If compressed type, generate huffman table */
          track_ptr->current_location += 4;
          track_ptr->index            += 4;
          status = smaf_generate_huffman_table (midi_ctl, track_ptr);

          if(status == AUDFMT_STATUS_FAILURE) {
            audfmt_header_do_failure(midi_ctl);
          } else if(status == AUDFMT_STATUS_BUFF_ERR) {
            track_ptr->track_status |= AUDFMT_TRACK_MASK_DO_HUFFMAN;
          } else {
            audfmt_check_next_track (midi_ctl, track_ptr);
          }
        } else {
          audfmt_check_next_track (midi_ctl, track_ptr);
        }
      }
      return;
    } else {
      if(chunk_length < track_ptr->track_end) {
        /* Skip over unrecognized chunks */
        track_ptr->current_location += chunk_length + 8;
        track_ptr->index            += chunk_length + 8;
      } else {
        midi_ctl->midi_status       |= AUDFMT_MASK_ERROR;
        track_ptr->current_location  = track_ptr->track_end;
      }
    }
  }

  /* If we've dropped out, then free data buffer and get next buffer */
  audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0,
                    (void *) track_ptr->track_buf);
  track_ptr->track_buf = NULL;

  if(!(midi_ctl->midi_status & AUDFMT_MASK_ERROR) &&
     (track_ptr->current_location <= track_ptr->track_end)) {
    audmain_get_header_data(midi_ctl->handle, track_ptr->current_location,
                            &(track_ptr->track_buf));
  } else {
    audfmt_header_do_failure(midi_ctl);
  }
} /* end smaf_do_track */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_do_opda

DESCRIPTION
  This function parses through the OPDA chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_do_opda (
  audfmt_parse_type *midi_ctl
) {
  audfmt_status_type ret_val = AUDFMT_STATUS_SUCCESS;
  audfmt_track_type  *track_ptr;
  uint32             chunk_length, tlong, chunk_header;
  uint8              *buffer;

  track_ptr = &midi_ctl->tracks[midi_ctl->num_tracks];

  /* Parse through until the end of the file is reached; make sure
  ** there is sufficient data to parse chunk
  */
  while((track_ptr->current_location <= (track_ptr->buf_reserve_size - 8)) &&
        (track_ptr->index < track_ptr->buf_size) &&
        ((track_ptr->buf_size - track_ptr->index) > 8)) {

    buffer         = &(track_ptr->track_buf[track_ptr->index]);
    chunk_length   = buffer[4];
    chunk_length   = chunk_length<<8 | buffer[5];
    chunk_length   = chunk_length<<8 | buffer[6];
    chunk_length   = chunk_length<<8 | buffer[7];
    chunk_header   = 8;

    if((midi_ctl->midi_status2 & AUDFMT_MASK_PARSING_DCH) != 0) {
      /* Process each tag unit */

      chunk_header = 4;
      chunk_length = buffer[2];
      chunk_length = chunk_length<<8 | buffer[3];

      if((chunk_length + 4) > midi_ctl->smaf_dch_size) {
        chunk_length = midi_ctl->smaf_dch_size - 4;
        midi_ctl->midi_status2 &= ~AUDFMT_MASK_PARSING_DCH;
      } else if((track_ptr->index + 4 + chunk_length) <= track_ptr->buf_size) {
        tlong = (chunk_length < 29) ? chunk_length : 29;

        if((strncmp( (char*)buffer, "AN", 2) == 0) &&
           ((midi_ctl->smaf_audio_spec.flag != 0x1) ||
            SMAF_GET_SPEC_FROM_OPDA(midi_ctl->smaf_content_type))) {
          memcpy(midi_ctl->smaf_audio_spec.artist, (char *) (buffer + 4),
                 tlong);
          midi_ctl->smaf_audio_spec.flag |= 0x01;
        } else if((strncmp( (char*)buffer, "ST", 2) == 0) &&
                  ((midi_ctl->smaf_audio_spec.flag != 0x2) ||
                   SMAF_GET_SPEC_FROM_OPDA(midi_ctl->smaf_content_type))) {
          memcpy(midi_ctl->smaf_audio_spec.title, (char *) (buffer + 4),
                 tlong);
          midi_ctl->smaf_audio_spec.flag |= 0x02;
        } else if((strncmp( (char*)buffer, "CR", 2) == 0) &&
                  ((midi_ctl->smaf_audio_spec.flag != 0x4) ||
                   SMAF_GET_SPEC_FROM_OPDA(midi_ctl->smaf_content_type))) {
          memcpy(midi_ctl->smaf_audio_spec.copyright, (char *) (buffer + 4),
                 tlong);
          midi_ctl->smaf_audio_spec.flag |= 0x04;
        }

        midi_ctl->smaf_dch_size -= chunk_length + 4;
        if(midi_ctl->smaf_audio_spec.flag == 0x7) {
          midi_ctl->midi_status2      &= ~AUDFMT_MASK_PARSING_DCH;
          track_ptr->index            += midi_ctl->smaf_dch_size;
          track_ptr->current_location += midi_ctl->smaf_dch_size;
        }
      } else{
        break;
      }
    } else if((strncmp("Dch", (char *)buffer, 3) == 0) && (chunk_length >= 4) &&
              (midi_ctl->midi_status2 & AUDFMT_MASK_CNTI_DONE) &&
              !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                         AUDFMT_MASK_GET_SPEC)) &&
              ((midi_ctl->smaf_audio_spec.flag != 0x7) ||
               SMAF_GET_SPEC_FROM_OPDA(midi_ctl->smaf_content_type))
             ) {

      midi_ctl->midi_status2 |= AUDFMT_MASK_PARSING_DCH;
      midi_ctl->smaf_dch_size = chunk_length;
      chunk_length = 0;
    } else if((strncmp("Pro", (char *)buffer, 3) == 0) &&
              (chunk_length == 12) &&
              !(midi_ctl->midi_status & AUDFMT_MASK_PRO_FOUND)) {
      if((track_ptr->index + 20) <= track_ptr->buf_size) {
        tlong = buffer[12];
        tlong = tlong<<8 | buffer[13];
        tlong = tlong<<8 | buffer[14];
        tlong = tlong<<8 | buffer[15];
        midi_ctl->smaf_start_tick = tlong;

        tlong = buffer[16];
        tlong = tlong<<8 | buffer[17];
        tlong = tlong<<8 | buffer[18];
        tlong = tlong<<8 | buffer[19];
        midi_ctl->smaf_end_tick = tlong;

        if(midi_ctl->smaf_end_tick > midi_ctl->smaf_start_tick) {
          midi_ctl->midi_status |= AUDFMT_MASK_PRO_FOUND;
        }
      } else {
        break;
      }
    }

    track_ptr->current_location += chunk_length + chunk_header;
    track_ptr->index            += chunk_length + chunk_header;
  }

  if(track_ptr->current_location < (track_ptr->buf_reserve_size - 8)) {
    /* If exhausted data, free buffer and get more */
    audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0, 
                      (void *) track_ptr->track_buf);
    track_ptr->track_buf = NULL;

    audmain_get_header_data(midi_ctl->handle, track_ptr->current_location,
                            &(track_ptr->track_buf));
  } else if(track_ptr->current_location > track_ptr->buf_reserve_size) {
    audfmt_header_do_failure(midi_ctl);
  } else {
    track_ptr->index            += track_ptr->buf_reserve_size -
                                           track_ptr->current_location;
    track_ptr->current_location  = track_ptr->buf_reserve_size;

    midi_ctl->midi_status2 &= ~AUDFMT_MASK_PARSING_OPDA;
    midi_ctl->midi_status2 |= AUDFMT_MASK_OPDA_DONE;
    ret_val = AUDFMT_STATUS_DONE;
  }

  return(ret_val);
} /* end smaf_do_opda */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_find_chunk

DESCRIPTION
  This function finds all the tracks (MTR/ATR chunks) in a SMAF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void smaf_find_chunk (
  audfmt_parse_type *midi_ctl
) {
  snd_cmx_smaf_graph_info_type graph_info;
  audfmt_track_type            *track_ptr, *old_track_ptr;
  uint32                       chunk_length, tlong;
  uint8                        *buffer;

  track_ptr = &midi_ctl->tracks[midi_ctl->num_tracks];

  /* Parse through until the end of the file is reached; make sure
  ** there is sufficient data to parse chunk
  */
  while((track_ptr->current_location < (track_ptr->track_end - 8)) &&
        (track_ptr->index < track_ptr->buf_size) &&
        ((track_ptr->buf_size - track_ptr->index) > 8)) {

    buffer         = &(track_ptr->track_buf[track_ptr->index]);
    chunk_length   = buffer[4];
    chunk_length   = chunk_length<<8 | buffer[5];
    chunk_length   = chunk_length<<8 | buffer[6];
    chunk_length   = chunk_length<<8 | buffer[7];

    if( (strncmp(AUDFMT_CNTI_ID, (char *) buffer, 3) == 0) &&
        !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                   AUDFMT_MASK_GET_SPEC))
        ){
      /* Parse CNTI chunk */
      if( chunk_length < 5){
        audfmt_header_do_failure( midi_ctl);
        return;
      }
      
      track_ptr->buf_reserve_size = track_ptr->current_location + 8 +
                                       chunk_length;
      if(track_ptr->buf_reserve_size < track_ptr->track_end) {
        track_ptr->current_location += 8;
        track_ptr->index            += 8;

        if(smaf_do_cnti(midi_ctl) != AUDFMT_STATUS_DONE) {
          return;
        }
      } else {
        audfmt_header_do_failure(midi_ctl);
        return;
      }
    } else if((strncmp(AUDFMT_MTR_ID, (char *) buffer, 3) == 0) ||
              (strncmp("ATR", (char *) buffer, 3) == 0) ||
              (strncmp("MMMG", (char *) buffer, 4) == 0)) {
      /* If a track ID is found (MTR/ATR), then advance track pointer.
      ** When the end of the file is reached we should have a count of
      ** all the tracks in this file and the start of each track should
      ** be in their respective current_location parameters.
      */
      old_track_ptr = track_ptr;
      track_ptr++;
      track_ptr->current_location = old_track_ptr->current_location;
      track_ptr->index            = old_track_ptr->index;
      track_ptr->buf_size         = old_track_ptr->buf_size;
      track_ptr->track_end        = old_track_ptr->track_end;
      track_ptr->track_buf        = old_track_ptr->track_buf;
      old_track_ptr->track_buf    = NULL;
      old_track_ptr->track_status = 0;
      midi_ctl->num_tracks++;
      midi_ctl->current_header_track++;
      track_ptr->current_location += chunk_length + 8;
      track_ptr->index            += chunk_length + 8;
    } else if ((strncmp("OPDA", (char *)buffer, 4) == 0) &&
               !(midi_ctl->midi_status2 & AUDFMT_MASK_OPDA_DONE)) {
      track_ptr->buf_reserve_size = track_ptr->current_location + 8 +
                                    chunk_length;
      if(track_ptr->buf_reserve_size < track_ptr->track_end) {
        midi_ctl->midi_status2 |= AUDFMT_MASK_PARSING_OPDA;
        track_ptr->current_location += 8;
        track_ptr->index            += 8;

        if(smaf_do_opda(midi_ctl) != AUDFMT_STATUS_DONE) {
          return;
        }
      } else {
        audfmt_header_do_failure(midi_ctl);
        return;
      }
    } else if (strncmp("GTR", (char *)buffer, 3) == 0) {
      if (!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                     AUDFMT_MASK_GET_SPEC))) {
        /* If a Graphics Track ID is found, and not in calc time
        ** then send information to client through callback.
        */
        graph_info.num  = buffer[3] & 0xf;
        graph_info.pos  = track_ptr->current_location + 8;
        graph_info.size = chunk_length;
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_SMAF_GRAPH_INFO, 0,
                          &graph_info);

        midi_ctl->midi_status2 |= AUDFMT_MASK_GRAPH_CHUNK_ENABLED;
      }
      track_ptr->current_location += chunk_length + 8;
      track_ptr->index            += chunk_length + 8;
    } else {
      track_ptr->current_location += chunk_length + 8;
      track_ptr->index            += chunk_length + 8;
    }
  }

  /* If exhausted data, free buffer and get more */
  audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0, 
                    (void *) track_ptr->track_buf);
  track_ptr->track_buf = NULL;

  if(!(midi_ctl->midi_status & AUDFMT_MASK_ERROR) &&
     (track_ptr->current_location <= track_ptr->track_end)) {
    if(track_ptr->current_location >= (track_ptr->track_end - 8)) {
      /* Reached end of SMAF file; start parsing tracks */
      midi_ctl->midi_status2 &= ~AUDFMT_MASK_PARSING_CHUNKS;
      midi_ctl->current_header_track = 0;
      track_ptr = midi_ctl->tracks;
      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_SET_TRACKS,
                        midi_ctl->num_tracks, (void *) &tlong);
    }
    audmain_get_header_data(midi_ctl->handle, track_ptr->current_location,
                            &(track_ptr->track_buf));
  } else {
    audfmt_header_do_failure(midi_ctl);
  }
} /* end smaf_find_chunk */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_do_cnti

DESCRIPTION
  This function processes CNTI chunk in a SMAF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates audio spec

===========================================================================*/
static audfmt_status_type smaf_do_cnti (
  audfmt_parse_type *midi_ctl
) {
  audfmt_status_type ret_val = AUDFMT_STATUS_SUCCESS;
  audfmt_track_type  *track_ptr;
  uint8              *buffer, *tag_end;
  uint32             chunk_length, tlong, tag_len;

  track_ptr = &midi_ctl->tracks[midi_ctl->num_tracks];
  buffer    = &(track_ptr->track_buf[track_ptr->index]);

  if((midi_ctl->midi_status2 & AUDFMT_MASK_PARSING_CNTI) == 0) {
    midi_ctl->smaf_content_type         = buffer[9];
    midi_ctl->smaf_audio_spec.code_type = buffer[10];

    track_ptr->current_location    += 5;
    track_ptr->index               += 5;
    midi_ctl->midi_status2         |= AUDFMT_MASK_PARSING_CNTI;
    midi_ctl->smaf_audio_spec.flag  = 0;
  }
  tlong = track_ptr->buf_reserve_size - track_ptr->current_location;

  while((tlong != 0) && ((track_ptr->buf_size - track_ptr->index) > 4)) {
    buffer = &(track_ptr->track_buf[track_ptr->index]);

    if(*(buffer+2) == ':') {
      tag_end = buffer + 3;
      for(chunk_length = 0;
          ((chunk_length + track_ptr->index) < track_ptr->buf_size) &&
          ((chunk_length + track_ptr->index) < track_ptr->buf_reserve_size); 
          chunk_length++, tag_end++) {
        if(*tag_end == ',') {
          break;
        }
      }

      if((*tag_end == ',') && ((chunk_length + 4) <= tlong)) {
        /* Copy meta data */

        tag_len = (chunk_length < 29) ? chunk_length : 29;

        if(strncmp((char *) buffer, "AN", 2) == 0) {
          memcpy(midi_ctl->smaf_audio_spec.artist, (char *) (buffer + 3),
                 tag_len);
          midi_ctl->smaf_audio_spec.flag |= 0x1;
        } else if(strncmp((char *) buffer, "ST", 2) == 0) {
          memcpy(midi_ctl->smaf_audio_spec.title, (char *) (buffer + 3),
                 tag_len);
          midi_ctl->smaf_audio_spec.flag |= 0x2;
        } else if(strncmp((char *) buffer, "CR", 2) == 0) {
          memcpy(midi_ctl->smaf_audio_spec.copyright, (char *) (buffer + 3),
                 tag_len);
          midi_ctl->smaf_audio_spec.flag |= 0x4;
        }

        if((midi_ctl->smaf_audio_spec.flag & 0x07) == 0x07) {
          track_ptr->index += track_ptr->buf_reserve_size -
                                                track_ptr->current_location;
          track_ptr->current_location = track_ptr->buf_reserve_size;
          break;
        }
      } else if(tlong < (track_ptr->buf_size - track_ptr->index)) {
        chunk_length = tlong - 4;
      } else {
        chunk_length = 0;
        break;
      }
    } else {
      /* Error stop processing CNTI */
      chunk_length = tlong - 4;
    }

    track_ptr->current_location += chunk_length + 4;
    track_ptr->index            += chunk_length + 4;
    tlong                       -= chunk_length + 4;
  }

  if(track_ptr->current_location < track_ptr->buf_reserve_size) {
    /* If exhausted data, free buffer and get more */
    audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0, 
    (void *) track_ptr->track_buf);
    track_ptr->track_buf = NULL;
  
    audmain_get_header_data(midi_ctl->handle, track_ptr->current_location,
                            &(track_ptr->track_buf));
    return(ret_val);
  } else if(track_ptr->current_location > track_ptr->buf_reserve_size) {
    audfmt_header_do_failure(midi_ctl);
    return(ret_val);
  } else {
    midi_ctl->midi_status2 &= ~AUDFMT_MASK_PARSING_CNTI;
    midi_ctl->midi_status2 |= AUDFMT_MASK_CNTI_DONE;
    ret_val = AUDFMT_STATUS_DONE;
  }
  
  return(ret_val);
}
#endif /* FEATURE_SMAF */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_data_cb_req

DESCRIPTION
  This function is the callback function for DLS waveform data requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_data_cb_req (
  uint8  *data,
  uint8  *buf,
  uint32 length,
  void   *client_data
) {
#ifdef FEATURE_SMAF
  audfmt_waveform_param_type *wave_param_ptr;
  audfmt_param_def_type      *param_ptr;
  audfmt_parse_type          *midi_ctl;

  if(client_data != NULL) {
    wave_param_ptr = (audfmt_waveform_param_type *) client_data;

    switch(wave_param_ptr->in_use_flag) {
      case AUDFMT_PROG_DEF_RESERVED_SEQ1:
        midi_ctl = &audfmt_parse_ctl[0];
        break;
      case AUDFMT_PROG_DEF_RESERVED_SEQ2:
        midi_ctl = &audfmt_parse_ctl[1];
        break;
      case AUDFMT_PROG_DEF_RESERVED_SEQ3:
        midi_ctl = &audfmt_parse_ctl[2];
        break;
      case AUDFMT_PROG_DEF_RESERVED_SEQ4:
        midi_ctl = &audfmt_parse_ctl[3];
        break;
      default:
        return;
    }

    wave_param_ptr->encoded_waveform = data;
    wave_param_ptr->decoded_waveform = buf;

    if(wave_param_ptr->inst_entry != 0xff) {
      param_ptr = &audfmt_dls_struct.param_def[wave_param_ptr->inst_entry];
      smaf_finalize_pcm_dls(midi_ctl, param_ptr, wave_param_ptr);
    }

    smaf_do_track(midi_ctl, NULL, 0, 0);
  }
#endif /* FEATURE_SMAF */
}

/* <EJECT> */
/*===========================================================================

FUNCTION do_track_chunk

DESCRIPTION
  This function verifies a c-midi/midi track chunk and returns its size.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static boolean do_track_chunk (
  audfmt_parse_type *midi_ctl,
  uint8             *buf,
  uint32            *track_length
) {
#ifdef FEATURE_WEBAUDIO
  if((midi_ctl->type == AUDFMT_MIDI_CMID_TYPE) ||
     (midi_ctl->type == AUDFMT_MIDI_MELO_TYPE)) {
    if(strncmp(AUDFMT_TRAC_ID, (char *) buf, AUDFMT_CHUNK_ID_BYTES)) {
      return(FALSE);
    }
  } else 
#endif /* FEATURE_WEBAUDIO */
  if(strncmp(AUDFMT_MTRK_ID, (char *) buf, AUDFMT_CHUNK_ID_BYTES)) {
    return(FALSE);
  }

  buf += AUDFMT_CHUNK_ID_BYTES;
  *track_length = *buf++;
  *track_length = (*track_length)<<8 | *buf++;
  *track_length = (*track_length)<<8 | *buf++;
  *track_length = (*track_length)<<8 | *buf;

  return(TRUE);
} /* end do_track_chunk */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_header_data_cb_req

DESCRIPTION
  This function is the callback function for data read commands while
  parsing file headers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_header_data_cb_req (
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 pos
) {
  audfmt_track_type *track_ptr;
  audfmt_parse_type *midi_ctl;
  uint32            track_size;
  uint32            counter;

  midi_ctl = audfmt_parse_ctl;

  for(counter = 0; counter < AUDFMT_NUM_SEQUENCES; counter++) {
    if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
       (midi_ctl->midi_status & AUDFMT_MASK_PARSING_HEADER)) {
      track_ptr       = &midi_ctl->tracks[midi_ctl->current_header_track];

      if(data == &track_ptr->track_buf) {
        if((length == 0) || (buf == NULL)) {
          audfmt_header_do_failure(midi_ctl);
          break;
        }
#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_SMAF
        if(midi_ctl->type == AUDFMT_MIDI_SMAF_TYPE) {
          if(midi_ctl->midi_status2 & AUDFMT_MASK_PARSING_CHUNKS) {
            /* The AUDFMT_MASK_PARSING_CHUNKS status for SMAF files means
            ** the number of tracks and their locations have not yet been
            ** all located
            */
            /* Sanity check for position */
            if(track_ptr->current_location == pos) {
              track_ptr->track_buf = buf;
              track_ptr->index     = 0;
              track_ptr->buf_size  = length;
              if( (midi_ctl->midi_status2 & AUDFMT_MASK_PARSING_CNTI) != 0){
                if(smaf_do_cnti(midi_ctl) == AUDFMT_STATUS_DONE) {
                  smaf_find_chunk(midi_ctl);
                }
              } else if(midi_ctl->midi_status2 & AUDFMT_MASK_PARSING_OPDA) {
                if(smaf_do_opda(midi_ctl) == AUDFMT_STATUS_DONE) {
                  smaf_find_chunk(midi_ctl);
                }
              } else {
                smaf_find_chunk(midi_ctl);
              }
            } else {
              audfmt_header_do_failure(midi_ctl);
            }
          } else if(midi_ctl->midi_status & AUDFMT_MASK_RESTARTING) {
            /* If restarting SMAF file there is no need to process
            ** and find tracks.
            */
            track_ptr->audfmt_handle    = midi_ctl->handle;
            track_ptr->buf_size         = length;
            track_ptr->current_location = pos;
            track_ptr->file_location    = pos + length;
            track_ptr->index            = 0;
            track_ptr->running_status   = 0;
            track_ptr->skip_buf_count   = 0;
            track_ptr->delta_tick       = 0;
            track_ptr->current_offset   = 0;
            if(track_ptr->track_status & AUDFMT_TRACK_MASK_PCM_TRACK) {
              track_ptr->track_status   = AUDFMT_TRACK_MASK_DO_TIME |
                                          AUDFMT_TRACK_MASK_PCM_TRACK;
            } else {
              track_ptr->track_status   = AUDFMT_TRACK_MASK_DO_TIME;
            }
            track_ptr->track_buf        = buf;

            audfmt_check_next_track(midi_ctl, track_ptr);
          } else {
            /* Find all tracks in this SMAF file */
            smaf_do_track(midi_ctl, buf, length, pos);
          }
          break;
        }
#endif /* FEATURE_SMAF */
        if((midi_ctl->type == AUDFMT_MIDI_CMID_TYPE) ||
           (midi_ctl->type == AUDFMT_MIDI_MELO_TYPE)) {
          if(midi_ctl->midi_status2 & AUDFMT_MASK_PARSING_SUBCHUNKS) {
            /* Parse all header chunks in c-midi file */
            /* Sanity check for position */
            if(track_ptr->current_location == pos) {
              track_ptr->track_buf = buf;
              track_ptr->index     = 0;
              track_ptr->buf_size  = length;
              do_cmid_subchunk(midi_ctl);
            } else {
              audfmt_header_do_failure(midi_ctl);
            }
            break;
          } else if(midi_ctl->midi_status2 & AUDFMT_MASK_PARSING_CHUNKS) {
            /* Parse all chunks in c-midi file */
            /* Sanity check for position and size of buffer */
            if((track_ptr->current_location == pos) && (length > 8)) {
              track_ptr->track_buf = buf;
              track_ptr->index     = 0;
              track_ptr->buf_size  = length;
              do_cmid_chunk(midi_ctl);
            } else {
              audfmt_header_do_failure(midi_ctl);
            }
            break;
          } else if((midi_ctl->cuep_offset[0] != AUDFMT_MAX_32BIT) &&
                    !(midi_ctl->midi_status & AUDFMT_MASK_FIND_CUEP_END)) {
            /* Cuepoint playback */
            if(!(track_ptr->track_status & AUDFMT_TRACK_MASK_CUEP_DONE)) {
              /* Need to find size of all track chunks for cuepoint playback */
              if(do_track_chunk(midi_ctl, buf, &track_size)) {
                audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_BUF, 0, 
                                  (void *) buf);

                if(midi_ctl->cuep_offset[midi_ctl->current_header_track] !=
                   AUDFMT_MAX_32BIT) {
                  /* If the current track has cuepoint position, then start */
                  pos += 8;
                  midi_ctl->track_start[midi_ctl->current_header_track] = pos;
                  if(track_size != AUDFMT_MAX_32BIT) {
                    track_ptr->track_end   = pos + track_size;
                  } else {
                    track_ptr->track_end   = AUDFMT_MAX_32BIT;
                  }
                  track_ptr->track_status |= AUDFMT_TRACK_MASK_CUEP_DONE;
                  pos += midi_ctl->cuep_offset[midi_ctl->current_header_track];
                  if(pos >= track_ptr->track_end) {
                    audfmt_header_do_failure(midi_ctl);
                  } else {
                    /* Get data buffer at cuepoint position */
                    audmain_get_header_data( midi_ctl->handle, pos,
                                             &(track_ptr->track_buf) );
                  }
                } else {
                  track_ptr->audfmt_handle     = NULL;
                  track_ptr->track_status      = 0;
                  track_ptr->track_buf         = NULL;
                  track_ptr->buf_size          = 0;
                  track_ptr->track_buf_reserve = NULL;
                  track_ptr->buf_reserve_size  = 0;

                  pos += 8;
                  midi_ctl->track_start[midi_ctl->current_header_track] = pos;

                  /* This track does not have cuepoint position, so skip */
                  midi_ctl->current_header_track++;
                  if(midi_ctl->current_header_track != midi_ctl->num_tracks) {
                    audmain_get_header_data(midi_ctl->handle, pos + track_size,
                                            &((track_ptr + 1)->track_buf));
                  } else {
                    audfmt_finish_header(midi_ctl);
                  }
                }
              } else {
                audfmt_header_do_failure(midi_ctl);
              }
            } else {
              /* Data buffer at cuepoint position */
              track_ptr->audfmt_handle    = midi_ctl->handle;
              track_ptr->buf_size         = length;
              track_ptr->current_location = pos;
              track_ptr->file_location    = pos + length;
              track_ptr->index            = 0;
              track_ptr->running_status   = 0;
              track_ptr->skip_buf_count   = 0;
              track_ptr->delta_tick       = midi_ctl->cuep_tick[
                                                midi_ctl->current_header_track];
              if(midi_ctl->cuep_do_tick[midi_ctl->current_header_track]) {
                track_ptr->track_status   = AUDFMT_TRACK_MASK_DO_TIME;
              } else {
                track_ptr->track_status   = 0;
              }
              track_ptr->track_buf        = buf;

              audfmt_check_next_track(midi_ctl, track_ptr);
            }
            break;
          }
        }
#endif /* FEATURE_WEBAUDIO */
        /* Normal playback for MIDI/c-MIDI tracks */
        if(do_track_chunk(midi_ctl, buf, &track_size)) {
          track_ptr->audfmt_handle    = midi_ctl->handle;
          track_ptr->buf_size         = length;
          track_ptr->current_location = pos + 8;
          track_ptr->file_location    = pos + length;
          track_ptr->index            = 8;
          track_ptr->running_status   = 0;
          track_ptr->skip_buf_count   = 0;
          track_ptr->delta_tick       = 0;
          track_ptr->track_status     = AUDFMT_TRACK_MASK_DO_TIME |
                                        AUDFMT_TRACK_MASK_CMD_PEND;
          track_ptr->track_buf        = buf;
          if(track_size != AUDFMT_MAX_32BIT) {
            track_ptr->track_end      = pos + 8 + track_size;
          } else {
            track_ptr->track_end      = AUDFMT_MAX_32BIT;
          }

#ifdef FEATURE_WEBAUDIO
          if((midi_ctl->type == AUDFMT_MIDI_CMID_TYPE) ||
             (midi_ctl->type == AUDFMT_MIDI_MELO_TYPE)) {
            /* Save track start locations for c-MIDI files for restarting */
            midi_ctl->track_start[midi_ctl->current_header_track] = pos + 8;
            if(midi_ctl->midi_status & AUDFMT_MASK_FIND_CUEP_END) {
              midi_ctl->cuep_offset[midi_ctl->current_header_track]  = 0;
              midi_ctl->cuep_do_tick[midi_ctl->current_header_track] = TRUE;
              midi_ctl->cuep_tick[midi_ctl->current_header_track]    = 0;
            }
          }
#endif /* FEATURE_WEBAUDIO */
          audfmt_check_next_track(midi_ctl, track_ptr);
        } else {
          audfmt_header_do_failure(midi_ctl);
        }
        break;
      } else if(data == &track_ptr->track_buf_reserve) {
        /* Sanity check: check that file position is consistent */
        if((track_ptr->file_location == pos) && (length != 0) && (buf != NULL))
        {
          track_ptr->buf_reserve_size   = length;
          track_ptr->file_location     += length;
          track_ptr->track_buf_reserve  = buf;

          midi_ctl->current_header_track++;
          if(midi_ctl->current_header_track == midi_ctl->num_tracks) {
            audfmt_finish_header(midi_ctl);
          }
        } else {
          audfmt_header_do_failure(midi_ctl);
        }
        break;
      }
    }

    /* Check next MIDI parse */
    midi_ctl++;
  }
} /* end audfmt_header_data_cb_req */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION parse_cmid_head

DESCRIPTION
  This function parses a compact midi file to get relevant information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type parse_cmid_head (
  audfmt_parse_type       *midi_ctl,
  snd_af_access_type      *handle,
  audmain_play_param_type *param,
  uint8                   *buffer,
  uint32                  buffer_size
) {
  uint32 header_size, tlong;

  if((buffer == NULL) || (buffer_size < 13)) {
    return(AUDFMT_STATUS_FAILURE);
  }

  tlong = buffer[4];
  tlong = (tlong << 8) | buffer[5];
  tlong = (tlong << 8) | buffer[6];
  tlong = (tlong << 8) | buffer[7];

  if(tlong == AUDFMT_MAX_32BIT) {
    midi_ctl->file_end = AUDFMT_MAX_32BIT;
  } else {
    midi_ctl->file_end = tlong + 8;
  }

  header_size = buffer[8];
  header_size = (header_size << 8) | buffer[9];

  if(header_size > tlong) {
    return(AUDFMT_STATUS_FAILURE);
  }

  midi_ctl->num_tracks = buffer[12];

  if ((midi_ctl->num_tracks < AUDFMT_MIN_CMID_TRACKS) ||
      (midi_ctl->num_tracks > AUDFMT_MAX_CMID_TRACKS)) {
    return(AUDFMT_STATUS_FAILURE);
  }

  midi_ctl->cmid_tempo           = 125;
  midi_ctl->cmid_timebase        = 3;
  midi_ctl->cmid_master_vol      = 100;
  midi_ctl->tempo                = AUDFMT_CMID_DEFAULT_TEMPO;
  midi_ctl->tempo_pending        = AUDFMT_CMID_DEFAULT_TEMPO;
  midi_ctl->tempo_factor         = TEMPO_MULT_FACTOR;
  midi_ctl->tempo_factor_pending = TEMPO_MULT_FACTOR;
  midi_ctl->calc_factor          = AUDFMT_MS_PER_MINUTE;
  midi_ctl->calc_ms_of           = AUDFMT_MAX_32BIT / AUDFMT_MS_PER_MINUTE;
  midi_ctl->calc_tick_of         = AUDFMT_MAX_32BIT / AUDFMT_CMID_DEFAULT_TEMPO;

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
  {
    midi_ctl->type = AUDFMT_MIDI_CMID_TYPE;
  }

  midi_ctl->handle               = handle;
  midi_ctl->midi_status          = AUDFMT_MASK_ENABLED |
                                   AUDFMT_MASK_PARSING_HEADER;
  midi_ctl->midi_status2         = AUDFMT_MASK_PARSING_SUBCHUNKS;

  if(param->get_specs) {
    midi_ctl->midi_status        |= AUDFMT_MASK_GET_SPEC;
  } else if(param->calc_time) {
    midi_ctl->midi_status        |= AUDFMT_MASK_CALC_TIME;
  } else {
    if(param->start_position != SND_AUDFMT_START_NORMAL) {
      midi_ctl->midi_status      |= AUDFMT_MASK_CUEP_PLAY;
      midi_ctl->cuep_repeat_orig  = (uint32) (param->start_position -
                                               SND_AUDFMT_START_CUE_POINT);
      midi_ctl->cuep_repeat       = midi_ctl->cuep_repeat_orig;
    } else if(param->cuep_enable) {
      midi_ctl->midi_status      |= AUDFMT_MASK_CUEP_PLAY;
      midi_ctl->cuep_repeat_orig  = param->repeat_cnt;
      midi_ctl->cuep_repeat       = midi_ctl->cuep_repeat_orig;
    } else if(param->repeat_enable) {
      midi_ctl->midi_status      |= AUDFMT_MASK_REPEAT_ENABLE;
    }
    if(param->multiseq_flag) {
      midi_ctl->midi_status      |= AUDFMT_MASK_MULTISEQ_MODE;
    }
  }

  midi_ctl->failed_cmd_cnt         = 0;
  midi_ctl->current_header_track   = 0;
  midi_ctl->file_offset            = param->byte_offset;
  midi_ctl->num_read               = 0;
  midi_ctl->elapsed_ms             = 0;
  midi_ctl->elapsed_tick           = 0;
  midi_ctl->accumulated_ms         = 0;
  midi_ctl->play_time_ms           = 0;
  midi_ctl->play_tick              = 0;
  midi_ctl->delta_tick             = 0;
  midi_ctl->delta_time             = 0;
  midi_ctl->repeat_ms              = 0;
  midi_ctl->interval_ms            = param->repeat_ms;
  midi_ctl->repeat_orig            = param->repeat_cnt;
  midi_ctl->repeat_count           = param->repeat_cnt;
  midi_ctl->seek_ms                = 0;
  midi_ctl->seek_tick              = 0;
  midi_ctl->seek_elapsed_tick      = 0;
  midi_ctl->cmd_buf.num_words      = 0;
  midi_ctl->cmd_buf.num_commands   = 0;

  if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {
    audfmt_add_global_command(midi_ctl, (uint8 *) audfmt_seq_reset,
                              sizeof(audfmt_seq_reset));
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
    {
      audfmt_reset_cmid_bank(midi_ctl);
    }

    /* Set all channels to be enabled by default */
    memset((uint8 *)(midi_ctl->channel_active), 1,
                                     sizeof(boolean) * AUDFMT_MAX_CHANNELS);
    memset((uint8 *)(midi_ctl->channel_index_active), 1,
                                     sizeof(boolean) * AUDFMT_MAX_CHANNELS);
  }

  midi_ctl->note_size              = 0;
  midi_ctl->exsb_size              = 0;
  midi_ctl->exsc_size              = 0;
  midi_ctl->exsn_size              = 0;
  midi_ctl->contents               = 0;
  midi_ctl->code                   = 0;
  midi_ctl->hybrid_mode            = CMID_POLY_DEFAULT;
  midi_ctl->tune_factor            = AUDFMT_TUNE_DEFAULT;
  midi_ctl->pan_factor             = AUDFMT_PAN_DEFAULT;
  midi_ctl->note_off_buffer.num    = 0;
  midi_ctl->note_off_buffer.index  = 0;
  midi_ctl->cb_store               = NULL;
  midi_ctl->vib_ctl                = NULL;
  midi_ctl->saf_objbuf_size        = CMID_SAF_OBJBUF_SIZE;
  midi_ctl->anim_format_type       = 0xff;

  for(tlong = 0; tlong < AUDFMT_MAX_CHANNELS; tlong++) {
    midi_ctl->channels[tlong]    = tlong;
  }
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
  {
    midi_ctl->exsa_size      = 0;
    midi_ctl->version        = 0;
    midi_ctl->cmid_wave_type = AUDFMT_CMID_WAVE_QCELP;
    for(tlong = 0; tlong < AUDFMT_MAX_CHANNELS; tlong++) {
      midi_ctl->bank_status[tlong] = AUDFMT_BANK_LEGACY;
    }
  }
  memset(midi_ctl->bank_msb, 0, sizeof(uint8) * AUDFMT_MAX_CHANNELS);
  memset(midi_ctl->program, 0, sizeof(uint8) * AUDFMT_MAX_CHANNELS);

  memset(midi_ctl->chan_vol, 63, sizeof(uint8) * AUDFMT_MAX_CHANNELS);

  memset((uint8 *)(midi_ctl->cuep_offset), 0xFF, 
                                   AUDFMT_MAX_CMID_TRACKS * sizeof(uint32));
  memset((uint8 *)(midi_ctl->jump_info), 0, 
                    AUDFMT_MAX_CMID_JUMP * sizeof(audfmt_jump_struct_type));
  memset((uint8 *)(midi_ctl->adec_status), 0, 
                                 sizeof(audfmt_adec_status_type) * 4);
  memset((uint8 *)(midi_ctl->wav_store), 0, 
         sizeof(audfmt_ext_store_type) * CMID_EXT_MAX_BUFFER);
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
  {
    for(tlong = 0; tlong < CMID_MAX_WAV_CHAN; tlong++) {
      midi_ctl->adec_chan_vol[tlong] = CMID_VOL_TO_DSP(CMID_DEFAULT_WAV_VOLUME);
      midi_ctl->wav_chan_vol[tlong] = ADEC_CONTENT_VOL(
                              (uint16) CMID_VOL_TO_DSP(CMID_DEFAULT_WAV_VOLUME),
                              midi_ctl->adec_chan_vol[tlong]);
      midi_ctl->wav_chan_pan[tlong] = CMID_PAN_TO_DSP(CMID_DEFAULT_WAV_PANPOT);
    }
  }

  midi_ctl->tracks->index            = 13;
  midi_ctl->tracks->track_buf        = buffer;
  midi_ctl->tracks->buf_size         = buffer_size;
  midi_ctl->tracks->current_location = midi_ctl->file_offset + 13;
  midi_ctl->tracks->track_end        = midi_ctl->file_offset + header_size + 10;

  do_cmid_subchunk(midi_ctl);

  return(AUDFMT_STATUS_SUCCESS);
} /* end parse_cmid_head */

/* <EJECT> */
#ifdef FEATURE_SMAF
/*===========================================================================

FUNCTION parse_smaf_head

DESCRIPTION
  This function parses a SMAF file to get relevant information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type parse_smaf_head (
  audfmt_parse_type       *midi_ctl,
  snd_af_access_type      *handle,
  audmain_play_param_type *param,
  uint8                   *buffer,
  uint32                  buffer_size
) {
  uint32 file_size;
  uint16 tlong;
  audmain_qsynth_vol_mode_enum_type vol_mode = AUDMAIN_QSYNTH_SMAF_VOL_LVL_MA2;
 
  if((buffer == NULL) || (buffer_size < 18) ||
     (strncmp(AUDFMT_CNTI_ID, (char *) &buffer[8], AUDFMT_CHUNK_ID_BYTES) != 0))
  {
    return(AUDFMT_STATUS_FAILURE);
  }

  pan_seq_counter =1;
  file_size = buffer[4];
  file_size = (file_size << 8) | buffer[5];
  file_size = (file_size << 8) | buffer[6];
  file_size = (file_size << 8) | buffer[7];

  midi_ctl->num_tracks = 0;
  midi_ctl->midi_status3 = 0;
  smaf_clean_heap((audfmt_parse_type*) midi_ctl);   

  midi_ctl->tempo_factor         = TEMPO_MULT_FACTOR;
  midi_ctl->tempo_factor_pending = TEMPO_MULT_FACTOR;
  midi_ctl->cmid_master_vol      = 127;

  midi_ctl->type                 = AUDFMT_MIDI_SMAF_TYPE;
  midi_ctl->smaf_type            = AUDFMT_SMAF_INVALID_TYPE;
  midi_ctl->handle               = handle;
  midi_ctl->midi_status          = AUDFMT_MASK_ENABLED |
                                   AUDFMT_MASK_PARSING_HEADER;
  midi_ctl->midi_status2         = AUDFMT_MASK_PARSING_CHUNKS;

  memset( (void*)&midi_ctl->smaf_audio_spec, 0, sizeof( midi_ctl->smaf_audio_spec));

  if((buffer[17] == 0) || (buffer[17] == 0x1)) {
    midi_ctl->hybrid_mode        = 7;
  } else if((buffer[17] >= 0x30) && (buffer[17] <= 0x55)) {
    tlong = buffer[17] & 0xf;
    if((tlong <= 5) && (tlong != 3)) {
      midi_ctl->hybrid_mode      = 7;
      midi_ctl->midi_status2    |= AUDFMT_MASK_SMAF_FM16;
    } else {
      midi_ctl->hybrid_mode      = 1;
    }
   if((buffer[17] & 0x0F) > 3){
      vol_mode=AUDMAIN_QSYNTH_SMAF_VOL_LVL_MA5;        
      midi_ctl->midi_status3 |=  AUDFMT_MASK_SMAF_MA5;
    }else{
      vol_mode=AUDMAIN_QSYNTH_SMAF_VOL_LVL_MA3;
      midi_ctl->midi_status3 |=  AUDFMT_MASK_SMAF_MA3;
    }
  } else {
    midi_ctl->hybrid_mode        = 1;
  }
 #ifdef FEATURE_AUDIO_CMX_5P0
   audmain_set_qsynth_vol_mode(AUDMAIN_QSYNTH_SMAF_VOL_LVL);
 #else
   audmain_set_qsynth_vol_mode((audmain_qsynth_vol_mode_enum_type)vol_mode);
 #endif
  if(param->get_specs) {
    midi_ctl->midi_status       |= AUDFMT_MASK_GET_SPEC;
  } else if(param->calc_time) {
    midi_ctl->midi_status       |= AUDFMT_MASK_CALC_TIME;
  } else {
    if(param->repeat_enable) {
      midi_ctl->midi_status     |= AUDFMT_MASK_REPEAT_ENABLE;
    }
    if(param->multiseq_flag) {
      midi_ctl->midi_status     |= AUDFMT_MASK_MULTISEQ_MODE;
    }
  }

  if(param->packed_dls_wf_mode) {
    midi_ctl->midi_status2 |= AUDFMT_MASK_SMAF_PACKED_DLS_WF;
  }

  midi_ctl->failed_cmd_cnt         = 0;
  midi_ctl->current_header_track   = 0;
  midi_ctl->file_offset            = param->byte_offset;
  midi_ctl->num_read               = 0;
  midi_ctl->elapsed_ms             = 0;
  midi_ctl->elapsed_tick           = 0;
  midi_ctl->accumulated_ms         = 0;
  midi_ctl->play_time_ms           = 0;
  midi_ctl->play_tick              = 0;
  midi_ctl->calc_factor            = 0;
  midi_ctl->delta_tick             = 0;
  midi_ctl->delta_time             = 0;
  midi_ctl->repeat_ms              = 0;
  midi_ctl->interval_ms            = param->repeat_ms;
  midi_ctl->repeat_orig            = param->repeat_cnt;
  midi_ctl->repeat_count           = param->repeat_cnt;
  midi_ctl->seek_ms                = 0;
  midi_ctl->seek_tick              = 0;
  midi_ctl->seek_elapsed_tick      = 0;
  midi_ctl->cmd_buf.num_words      = 0;
  midi_ctl->cmd_buf.num_commands   = 0;

  if(buffer[17] < 0x30) {
    midi_ctl->midi_status2 |= AUDFMT_MASK_SMAF_MA2; 
  }

  if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {
    audfmt_add_global_command(midi_ctl, (uint8 *) audfmt_seq_reset,
                              sizeof(audfmt_seq_reset));

    audfmt_reset_hq_bank(midi_ctl);
  }

  midi_ctl->note_off_buffer.num    = 0;
  midi_ctl->note_off_buffer.index  = 0;

  memset((uint8 *)(midi_ctl->bank_msb), 0, sizeof(uint8) * AUDFMT_MAX_CHANNELS);
  memset((uint8 *)(midi_ctl->bank_lsb), 0, sizeof(uint8) * AUDFMT_MAX_CHANNELS);
  for(tlong = 0; tlong < AUDFMT_MAX_CHANNELS; tlong++) {
    midi_ctl->bank_status[tlong] = AUDFMT_BANK_NORMAL;
  }

  if(!(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_MA2)) {
    midi_ctl->bank_status[9] = AUDFMT_BANK_DRUM;
    midi_ctl->bank_msb[9]    = 125;
  } else {
    /* Initialize MA2 pitch bend command structures to invalid values */
    for(tlong = 0; tlong < AUDFMT_MAX_CHANNELS; tlong++) {
      midi_ctl->smaf_ma2_pitch[tlong].note    = 0x80;
      midi_ctl->smaf_ma2_pitch[tlong].c_value = 0x80;
      midi_ctl->smaf_ma2_pitch[tlong].b_value = 0x100;
    }
  }
  memset(midi_ctl->chan_vol, 64, sizeof(uint8) * AUDFMT_MAX_CHANNELS);
  memset(midi_ctl->smaf_octave_shift, 0, sizeof(uint8) * AUDFMT_MAX_CHANNELS); 
  memset((uint8 *)(midi_ctl->smaf_program_change), 0x80,
                                          sizeof(uint8) * AUDFMT_MAX_CHANNELS);
  memset((uint8 *)(midi_ctl->smaf_velocity), SMAF_HP_NOTE_DEFAULT_VEL,
                                          sizeof(uint8) * AUDFMT_MAX_CHANNELS);
  memset((uint8 *)(midi_ctl->adec_status), 0, 
                                 sizeof(audfmt_adec_status_type) * 4);
  memset((uint8 *)(midi_ctl->wav_store), 0, 
         sizeof(audfmt_ext_store_type) * CMID_EXT_MAX_BUFFER);

  midi_ctl->tune_factor             = AUDFMT_TUNE_DEFAULT;
  midi_ctl->pan_factor              = AUDFMT_PAN_DEFAULT;
  midi_ctl->smaf_led_ctl            = 0;
  midi_ctl->smaf_vib_ctl            = 0;
  midi_ctl->smaf_fm_dls_index       = 0;
  midi_ctl->smaf_fm_drum_index      = 0;

  for(tlong = 0; tlong < CMID_MAX_WAV_CHAN; tlong++) {
    midi_ctl->adec_chan_vol[tlong] = CMID_VOL_TO_DSP(CMID_DEFAULT_WAV_VOLUME);
    midi_ctl->wav_chan_vol[tlong]  = ADEC_CONTENT_VOL( 
                                     CMID_VOL_TO_DSP(CMID_DEFAULT_WAV_VOLUME),
                                     midi_ctl->adec_chan_vol[tlong]);
    midi_ctl->wav_chan_pan[tlong]  = CMID_PAN_TO_DSP(CMID_DEFAULT_WAV_PANPOT);
  }


  midi_ctl->tracks->index            = 8;
  midi_ctl->tracks->track_buf        = buffer;
  midi_ctl->tracks->buf_size         = buffer_size;
  midi_ctl->tracks->current_location = midi_ctl->file_offset + 8;
  midi_ctl->tracks->track_end        = midi_ctl->file_offset + file_size + 8;

  smaf_find_chunk(midi_ctl);

  return(AUDFMT_STATUS_SUCCESS);
} /* end parse_smaf_head */
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
/*===========================================================================

FUNCTION do_midi_head

DESCRIPTION
  This function parses the midi head.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static boolean do_midi_head (
  audfmt_parse_type *midi_ctl,
  uint8             *buffer
) {
  uint32 tlong;
  uint32 index;

  if(buffer == NULL) {
    MSG_FATAL("Null buffer for header.",0,0,0);
    return(FALSE);
  }

  /* Verify header size */
  index = AUDFMT_CHUNK_ID_BYTES;
  tlong = buffer[index++];
  tlong = tlong<<8 | buffer[index++];
  tlong = tlong<<8 | buffer[index++];
  tlong = tlong<<8 | buffer[index++];

  if(tlong != MIDI_HEADER_LENGTH) {
    MSG_ERROR("Unrecognized midi file!",0,0,0);
    return(FALSE);
  }

  /* Verify MIDI type */
  tlong = buffer[index++];
  tlong = (tlong)<<8 | buffer[index++];

  if((tlong != MIDI_TYPE_0) && (tlong !=  MIDI_TYPE_1)) {
    MSG_ERROR("Unsupported Midi type: %d", tlong,0,0);
    return(FALSE);
  }

  /* Determine number of tracks */
  tlong = buffer[index++];
  tlong = (tlong)<<8 | buffer[index++];

  if(tlong > AUDFMT_MAX_MIDI_TRACKS) {
    MSG_HIGH("Only first %d tracks are supported: %d", AUDFMT_MAX_MIDI_TRACKS,
                                                       tlong, 0);
    tlong = AUDFMT_MAX_MIDI_TRACKS;
  }

  midi_ctl->num_tracks = tlong;

  /* Determine PPQN */
  tlong = buffer[index++];
  tlong = (tlong<<8 | buffer[index++]) * 10;

  if(tlong == 0) {
    MSG_ERROR("Illegal PPQN value of 0", 0,0,0);
    return(FALSE);
  }

  midi_ctl->ppqn_10 = tlong;

  /* Determine tempo and overflow values */
  midi_ctl->tempo                = AUDFMT_MIDI_DEFAULT_TEMPO;
  midi_ctl->tempo_pending        = AUDFMT_MIDI_DEFAULT_TEMPO;
  midi_ctl->tempo_factor         = TEMPO_MULT_FACTOR;
  midi_ctl->tempo_factor_pending = TEMPO_MULT_FACTOR;
  midi_ctl->calc_factor          = midi_ctl->ppqn_10 * TEMPO_MULT_FACTOR;
  midi_ctl->calc_ms_of           = AUDFMT_MAX_32BIT / AUDFMT_MIDI_DEFAULT_TEMPO;
  midi_ctl->calc_tick_of         = AUDFMT_MAX_32BIT / midi_ctl->calc_factor;

  return(TRUE);
} /* end do_midi_head */

/* <EJECT> */
/*===========================================================================

FUNCTION parse_midi_head

DESCRIPTION
  This function parses the midi head for relevant infomation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type parse_midi_head (
  audfmt_parse_type       *midi_ctl,
  snd_af_access_type      *handle,
  audmain_play_param_type *param,
  uint8                   *buffer
) {
  uint32 tlong;

  if(!do_midi_head(midi_ctl, buffer)) {
    return(AUDFMT_STATUS_FAILURE);
  }

  audfmt_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, (void *) buffer);


  audfmt_do_play_cb(handle, SND_CMX_AF_SET_TRACKS, midi_ctl->num_tracks,
                    (void *) &tlong);

  midi_ctl->type        = AUDFMT_MIDI_MIDI_TYPE;
  midi_ctl->handle      = handle;
  midi_ctl->midi_status = AUDFMT_MASK_ENABLED | AUDFMT_MASK_PARSING_HEADER;

  if(param->get_specs) {
    midi_ctl->midi_status   |= AUDFMT_MASK_GET_SPEC;
  } else if(param->calc_time) {
    midi_ctl->midi_status   |= AUDFMT_MASK_CALC_TIME;
  } else {
    if(param->repeat_enable) {
      midi_ctl->midi_status |= AUDFMT_MASK_REPEAT_ENABLE;
    }
    if(param->multiseq_flag) {
      midi_ctl->midi_status |= AUDFMT_MASK_MULTISEQ_MODE;
    }
  }

  midi_ctl->midi_status2           = 0;
  midi_ctl->failed_cmd_cnt         = 0;
  midi_ctl->current_header_track   = 0;
  midi_ctl->file_offset            = param->byte_offset;
  midi_ctl->num_read               = 0;
  midi_ctl->elapsed_ms             = 0;
  midi_ctl->elapsed_tick           = 0;
  midi_ctl->accumulated_ms         = 0;
  midi_ctl->play_time_ms           = 0;
  midi_ctl->play_tick              = 0;
  midi_ctl->delta_tick             = 0;
  midi_ctl->delta_time             = 0;
  midi_ctl->repeat_ms              = 0;
  midi_ctl->interval_ms            = param->repeat_ms;
  midi_ctl->repeat_orig            = param->repeat_cnt;
  midi_ctl->repeat_count           = param->repeat_cnt;
  midi_ctl->seek_ms                = 0;
  midi_ctl->seek_tick              = 0;
  midi_ctl->seek_elapsed_tick      = 0;
  midi_ctl->tune_factor            = AUDFMT_TUNE_DEFAULT;
  midi_ctl->pan_factor             = AUDFMT_PAN_DEFAULT;

  midi_ctl->cmd_buf.num_words      = 0;
  midi_ctl->cmd_buf.num_commands   = 0;

#ifdef FEATURE_WEBAUDIO
  midi_ctl->note_off_buffer.num    = 0;
  midi_ctl->note_off_buffer.index  = 0;
#endif /* FEATURE_WEBAUDIO */

  if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {
    audfmt_add_global_command(midi_ctl, (uint8 *) audfmt_seq_reset,
                              sizeof(audfmt_seq_reset));

    /* Set all channels to be enabled by default */
    memset((uint8 *)(midi_ctl->channel_active), 1,
                                     sizeof(boolean) * AUDFMT_MAX_CHANNELS);
  }

  memset((uint8 *)(midi_ctl->bank_msb), 0, sizeof(uint8) * AUDFMT_MAX_CHANNELS);
  memset((uint8 *)(midi_ctl->bank_lsb), 0, sizeof(uint8) * AUDFMT_MAX_CHANNELS);

  #ifdef FEATURE_AUDIO_CMX_5P0
  memset((uint8 *)(midi_ctl->midi_pitch_is_changed), FALSE,
         sizeof(boolean) * AUDFMT_MAX_CHANNELS);
  memset((uint8 *)(midi_ctl->midi_ch_pitch_param), 0,
         sizeof(uint16) * AUDFMT_MAX_CHANNELS);

  memset((uint8 *)(midi_ctl->midi_vol_is_changed), FALSE,
         sizeof(boolean) * AUDFMT_MAX_CHANNELS);
  memset((uint8 *)(midi_ctl->midi_ch_vol_param), 0,
         sizeof(uint16) * AUDFMT_MAX_CHANNELS);

  memset((uint8 *)(midi_ctl->midi_pan_is_changed), FALSE,
         sizeof(boolean) * AUDFMT_MAX_CHANNELS);
  memset((uint8 *)(midi_ctl->midi_ch_pan_param), 0,
         sizeof(uint16) * AUDFMT_MAX_CHANNELS);
  #endif /* FEATURE_AUDIO_CMX_5P0 */

  for(tlong = 0; tlong < AUDFMT_MAX_CHANNELS; tlong++) {
    midi_ctl->bank_status[tlong] = AUDFMT_BANK_NORMAL;
  }
  midi_ctl->bank_status[9] = AUDFMT_BANK_DRUM;
  midi_ctl->drum_program   = 0;

#ifdef FEATURE_WEBAUDIO
  midi_ctl->cb_store               = NULL;
  midi_ctl->vib_ctl                = NULL;
  /* Set hybrid mode to HQ by default */
  midi_ctl->hybrid_mode            = CMID_POLY_MAX_HQ;

  if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {
    audfmt_reset_hq_bank(midi_ctl);
  }
#else
  midi_ctl->hybrid_mode            = 0;
#endif /* FEATURE_WEBAUDIO */

  audmain_get_header_data(handle, midi_ctl->file_offset + MIDI_HEADER_BYTES,
                          &(midi_ctl->tracks[0].track_buf));

  return(AUDFMT_STATUS_SUCCESS);
} /* end parse_midi_head */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_parse_head

DESCRIPTION
  This function parses the head of a file to determine what type of file it
  is. It then calls the appropriate setup function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_parse_head (
  uint8                   *buffer,
  snd_af_access_type      *handle,
  audmain_play_param_type *param,
  audmain_parse_ctl_type  *parse_ctl
) {
  audfmt_midi_type     af_type = AUDFMT_MIDI_INVALID_TYPE;
  audmain_status_type  ret_val = AUDMAIN_STATUS_FAILURE;
  audfmt_parse_type    *midi_ctl;
  uint32               counter;

  if(buffer == NULL) {
    MSG_FATAL("Null buffer",0,0,0);
  } else if
      (strncmp(AUDFMT_MTHD_ID, (char *) buffer, AUDFMT_CHUNK_ID_BYTES) == 0) {
    af_type = AUDFMT_MIDI_MIDI_TYPE;
#ifdef FEATURE_WEBAUDIO
  } else if
      (strncmp(AUDFMT_CMID_ID, (char *) buffer, AUDFMT_CHUNK_ID_BYTES) == 0) {
    af_type = AUDFMT_MIDI_CMID_TYPE;
  } else if
      (strncmp(AUDFMT_WASF_ID, (char *) buffer, AUDFMT_CHUNK_ID_BYTES) == 0) {
    af_type = AUDFMT_MIDI_WASF_TYPE;
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
#ifdef FEATURE_SMAF
  } else if
      (strncmp(AUDFMT_MMMD_ID, (char *) buffer, AUDFMT_CHUNK_ID_BYTES) == 0) {
    af_type = AUDFMT_MIDI_SMAF_TYPE;
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */
  } else {
    return(AUDMAIN_STATUS_ERROR);
  }

  /* Find available sequence
  */
  midi_ctl = audfmt_parse_ctl;
  for(counter = 0; counter < AUDFMT_NUM_SEQUENCES; counter++) {
    if(!(midi_ctl[counter].midi_status & AUDFMT_MASK_ENABLED)) {
      break;
    }
  }

  if(counter != AUDFMT_NUM_SEQUENCES) {
  	midi_ctl += counter;
    /* Clear the global data */
    memset(midi_ctl, 0, sizeof(audfmt_parse_type));

    midi_ctl->sequence = counter;
    midi_ctl->error_status = SND_SUCCESS;

    switch(af_type) {
      case AUDFMT_MIDI_MIDI_TYPE:
        if(parse_midi_head(midi_ctl, handle, param, buffer) == 
           AUDFMT_STATUS_SUCCESS) {
          ret_val = AUDMAIN_STATUS_SUCCESS;
          if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                        AUDFMT_MASK_GET_SPEC))) {
            (void) audmain_set_adec_mode(AUDMAIN_ADEC_MODE_ADPCM);
            audmain_set_qsynth_vol_mode(AUDMAIN_QSYNTH_DEFAULT_VOL_LVL);
          }
        }
        break;

#ifdef FEATURE_WEBAUDIO
      case AUDFMT_MIDI_CMID_TYPE:
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
      case AUDFMT_MIDI_WASF_TYPE:
        if(parse_cmid_head(midi_ctl, handle, param, buffer, 
                           parse_ctl->buffer_size) == AUDFMT_STATUS_SUCCESS) {
          ret_val = AUDMAIN_STATUS_SUCCESS;
          main_audfmt_parse_ctl = parse_ctl;
		  if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                        AUDFMT_MASK_GET_SPEC))) {
            audmain_set_qsynth_vol_mode(AUDMAIN_QSYNTH_DEFAULT_VOL_LVL);
          }
        }
        break;

#ifdef FEATURE_SMAF
      case AUDFMT_MIDI_SMAF_TYPE:
        if(parse_smaf_head(midi_ctl, handle, param, buffer,
                           parse_ctl->buffer_size) == AUDFMT_STATUS_SUCCESS) {
          ret_val = AUDMAIN_STATUS_SUCCESS;
          main_audfmt_parse_ctl = parse_ctl;
          if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                        AUDFMT_MASK_GET_SPEC))) {
            (void) audmain_set_adec_mode(AUDMAIN_ADEC_MODE_YADPCM);
          }
        }
        break;
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */

      case AUDFMT_MIDI_INVALID_TYPE:
      default:
        ret_val = AUDMAIN_STATUS_FAILURE;
        break;
    }

    if(ret_val == AUDMAIN_STATUS_SUCCESS) {
      parse_ctl->tracks = audfmt_parse_ctl;
    }
  }

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

  return(ret_val);
} /* end audfmt_parse_head */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_adjust_time

DESCRIPTION
  This function reduces time_ms and tick by equivalent durations according to
   the current tempo.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_adjust_time (
  audfmt_parse_type *midi_ctl,
  uint32            *time_ms,
  uint32            *tick
) {
  uint32 tlong;

  /* Calculate number of milliseconds contained by tick. */
  audfmt_calc_ms(midi_ctl, *tick, &tlong);

  /* Subtract time from time_ms */
  if(tlong <= *time_ms) {
    *time_ms -= tlong;
  } else {
    *time_ms  = 0;
  }

  /* Clear delta_time in tick */
  *tick    = 0;
} /* end audfmt_adjust_time */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_adjust_tempo_factor

DESCRIPTION
  This function updates the tempo_factor to tempo_factor_pending and
  recalculates any factors, if necessary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_adjust_tempo_factor (
  audfmt_parse_type *midi_ctl
) {
  /* Update tempo factor */
  midi_ctl->tempo_factor = midi_ctl->tempo_factor_pending;

  /* Recalculate overflow factors based on new tempo factor */
  switch(midi_ctl->type) {
    case AUDFMT_MIDI_MIDI_TYPE:
      midi_ctl->calc_factor  = midi_ctl->ppqn_10 * midi_ctl->tempo_factor;
      midi_ctl->calc_tick_of = AUDFMT_MAX_32BIT / midi_ctl->calc_factor;
      break;

#ifdef FEATURE_WEBAUDIO
    case AUDFMT_MIDI_CMID_TYPE:
    case AUDFMT_MIDI_MELO_TYPE:
      midi_ctl->calc_factor  = audfmt_tempo_adjust_down(midi_ctl->tempo_factor,
                                                        AUDFMT_MS_PER_MINUTE);
      midi_ctl->calc_ms_of   = AUDFMT_MAX_32BIT / midi_ctl->calc_factor;
      break;

#ifdef FEATURE_SMAF
    case AUDFMT_MIDI_SMAF_TYPE:
      /* For SMAF, if tempo factor is 100%, then set calc_factor to 0 to 
      ** indicate that no adjusting needs to be done in calculating time.
      */
      if(midi_ctl->tempo_factor == 100) {
        midi_ctl->calc_factor = 0;
      } else {
        midi_ctl->calc_factor = midi_ctl->smaf_timebase_d * 100;
      }
      break;
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */
  }
} /* end audfmt_adjust_tempo_factor */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_calc_time_remain

DESCRIPTION
  This function calculates the time remaining until the next event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_calc_time_remain (
  audfmt_parse_type *midi_ctl
) {
  uint32 tlong;

  if(midi_ctl != NULL) {
    audfmt_calc_ms(midi_ctl, midi_ctl->elapsed_tick + midi_ctl->delta_tick,
                   &tlong);

    if(tlong > midi_ctl->elapsed_ms) {
      midi_ctl->delta_time = tlong - midi_ctl->elapsed_ms;
    } else {
      midi_ctl->delta_time = 1;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_age

DESCRIPTION
  This function ages the file by the indicated number of milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
) {
  audfmt_parse_type *midi_ctl;
  uint32            eticks, elapsed_ms, counter;
  boolean           age_progressing = FALSE;

  if(parse_ctl != NULL) {
    midi_ctl = (audfmt_parse_type *) parse_ctl->tracks;

    for(counter = 0; counter < AUDFMT_NUM_SEQUENCES; counter++) {
      /* Check if we should be aging this sequence */
      if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
         !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME      |
                                    AUDFMT_MASK_PAUSED         |
                                    AUDFMT_MASK_SEEK_PAUSED    |
                                    AUDFMT_MASK_SEEK_ACTIVE    |
                                    AUDFMT_MASK_RESUME_PENDING |
                                    AUDFMT_MASK_PARSING_HEADER))) {

        if(midi_ctl->repeat_ms != 0) {
          if(midi_ctl->tempo_factor != midi_ctl->tempo_factor_pending) {
            audfmt_adjust_tempo_factor(midi_ctl);
          }
          /* Age repeat timer */
          age_progressing = TRUE;
          elapsed_ms      = audfmt_tempo_adjust_up(midi_ctl->tempo_factor,
                                                   *elapsed_time);
          if(elapsed_ms <= midi_ctl->repeat_ms) {
            midi_ctl->repeat_ms    -= elapsed_ms;
            midi_ctl->play_time_ms += elapsed_ms;
            elapsed_ms              = 0;
          } else {
            elapsed_ms             -= midi_ctl->repeat_ms;
            midi_ctl->play_time_ms += midi_ctl->repeat_ms;
            midi_ctl->repeat_ms     = 0;
            elapsed_ms              = audfmt_tempo_adjust_down(
                                                    midi_ctl->tempo_factor,
                                                    elapsed_ms);
          }
        } else {
          elapsed_ms = *elapsed_time;
        }
        if((midi_ctl->repeat_ms == 0) &&
           (midi_ctl->midi_status & AUDFMT_MASK_INIT_DONE)) {
          if(!(midi_ctl->midi_status & AUDFMT_MASK_STARTED_AGE)) {
            midi_ctl->midi_status |= AUDFMT_MASK_STARTED_AGE;
          } else {
            age_progressing = TRUE;

            if(midi_ctl->elapsed_ms >= midi_ctl->calc_tick_of) {
              /* Need to adjust elapsed time to avoid overflow of variables */
              audfmt_adjust_time(midi_ctl, &midi_ctl->elapsed_ms,
                                 &midi_ctl->elapsed_tick);
            }

            midi_ctl->elapsed_ms     += elapsed_ms;
            midi_ctl->accumulated_ms += elapsed_ms;

#ifdef FEATURE_WEBAUDIO
            if((midi_ctl->vib_ctl != NULL) &&
               (midi_ctl->vib_ctl->vib_ctl != NULL)) {
              /* Age vibrator sequence */
              elapsed_ms = audfmt_tempo_adjust_up(midi_ctl->tempo_factor,
                                                  *elapsed_time);
              cmid_vib_age(midi_ctl->vib_ctl, elapsed_ms);
// need to take care of aging vib during seeking
            }
#endif /* FEATURE_WEBAUDIO */

            if(midi_ctl->midi_status & AUDFMT_MASK_PARSE_DONE) {
              /* If next events of this sequence are available */
              audfmt_calc_tick(midi_ctl, midi_ctl->elapsed_ms, &eticks);
              eticks = eticks - midi_ctl->elapsed_tick;

              if(eticks >= midi_ctl->delta_tick) {
                if(midi_ctl->midi_status & AUDFMT_MASK_DONE_PENDING) {
                  /* If finished playing file, update total playtime */
                  midi_ctl->play_time_ms += audfmt_tempo_adjust_up (
                                                      midi_ctl->tempo_factor,
                                                      midi_ctl->accumulated_ms);
                  midi_ctl->accumulated_ms = 0;
                }

                /* Update elapsed tick accumulator */
                midi_ctl->elapsed_tick += midi_ctl->delta_tick;

                /* Set time for this event to zero so that parser will execute
                ** these events.
                */
                midi_ctl->delta_tick    = 0;
                midi_ctl->delta_time    = 0;

                /* Account for tempo change command */
                if(midi_ctl->tempo_pending != midi_ctl->tempo) {
                  audfmt_adjust_time(midi_ctl, &midi_ctl->elapsed_ms,
                                     &midi_ctl->elapsed_tick);
                  midi_ctl->tempo = midi_ctl->tempo_pending;

                  /* Recalculate overflow factors based on new tempo */
                  if(midi_ctl->tempo != 0) {
                    switch(midi_ctl->type) {
                      case AUDFMT_MIDI_MIDI_TYPE:
                        midi_ctl->calc_ms_of   = AUDFMT_MAX_32BIT /  
                                                              midi_ctl->tempo;
                        break;

#ifdef FEATURE_WEBAUDIO
                      case AUDFMT_MIDI_CMID_TYPE:
                      case AUDFMT_MIDI_MELO_TYPE:
                        midi_ctl->calc_tick_of = AUDFMT_MAX_32BIT / 
                                                               midi_ctl->tempo;
                        break;
#endif /* FEATURE_WEBAUDIO */
                    }
                  }
                }
              } else {
                midi_ctl->delta_tick   -= eticks;
                midi_ctl->elapsed_tick += eticks;

                audfmt_calc_time_remain(midi_ctl);
              }
            }
          }
        }

        /* Account for tempo factor change */
        if(midi_ctl->tempo_factor != midi_ctl->tempo_factor_pending) {
          /* Adjust elapsed ticks/time based on old tempo factor */ 
          audfmt_adjust_time(midi_ctl, &midi_ctl->elapsed_ms,
                             &midi_ctl->elapsed_tick);

          /* Update the playtime up to this point with old tempo factor */
          midi_ctl->play_time_ms += audfmt_tempo_adjust_up(
                                                      midi_ctl->tempo_factor, 
                                                      midi_ctl->accumulated_ms);
          midi_ctl->accumulated_ms = 0;

          audfmt_adjust_tempo_factor(midi_ctl);

          if(midi_ctl->midi_status & AUDFMT_MASK_PARSE_DONE) {
            if(midi_ctl->delta_tick != 0) {
              audfmt_calc_time_remain(midi_ctl);
            }
          }
        }
      }
      midi_ctl++;
    }
    if(age_progressing) {
      *adjusted_time = *elapsed_time;
    } else {
      *elapsed_time  = 0;
      *adjusted_time = 0;
    }
  }
} /* end audfmt_age */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION audfmt_set_pitchrange_cmd

DESCRIPTION
  This function enters a pitch range command into the command buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_set_pitchrange_cmd (
  audfmt_parse_type   *midi_ctl,
  uint8               channel,
  uint8               pitch_factor
) {
  audfmt_status_type ret_val;
  uint8              buf[AUDFMT_COMMAND_SIZE_4];

  buf[0]  = MIDI_CONTROL_CHANGE;
  buf[1]  = channel;
  buf[2]  = MIDI_RP_COARSE_COMMAND;
  buf[3]  = MIDI_RP_PITCH_PARM1;
  ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    buf[2]  = MIDI_RP_FINE_COMMAND;
    buf[3]  = MIDI_RP_PITCH_PARM2;
    ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

    if(ret_val == AUDFMT_STATUS_SUCCESS) {
      buf[2]  = MIDI_RP_PITCH_PARM3;
      buf[3]  = pitch_factor;
      ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

      if(ret_val == AUDFMT_STATUS_SUCCESS) {
        buf[2]  = MIDI_RP_PITCH_PARM4;
        buf[3]  = MIDI_RP_PITCH_PARM5;
        ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

        if(ret_val == AUDFMT_STATUS_SUCCESS) {
          buf[2]  = MIDI_RP_FINE_COMMAND;
          buf[3]  = MIDI_RP_RESET_PARM;
          ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
        }
      }
    }
  }

  return(ret_val);
} /* end audfmt_set_pitchrange_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION do_cmid_pitch_bend

DESCRIPTION
  This function processes a cmid extension status A pitch bend command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type do_cmid_pitch_bend (
  uint8               command,
  audfmt_parse_type   *midi_ctl,
  uint8               track_num
) {
  audfmt_status_type ret_val;
  uint32             bytes_read = 2;
  uint8              buf[AUDFMT_COMMAND_SIZE_4];
  uint8              tbyte;

  ret_val = audfmt_get_next_track_byte(&midi_ctl->tracks[track_num], &tbyte,
                                       &bytes_read);
  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    buf[0]  = MIDI_PITCH_WHEEL;
    buf[1]  = CMID_CHANNEL(((command >> 5) & 0x3), track_num,
                           midi_ctl->num_tracks, midi_ctl->channels);
    buf[2]  = CMID_FINE_PITCH_BEND_LSB(tbyte);
    buf[3]  = CMID_FINE_PITCH_BEND_MSB(command, tbyte);
    ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
  }

  if((ret_val != AUDFMT_STATUS_SUCCESS) && (ret_val != AUDFMT_STATUS_ERROR)) {
    /* If this command parse was not successful, restore the track to
    ** original condition.
    */
    if(audfmt_restore_track(&midi_ctl->tracks[track_num], bytes_read) !=
       AUDFMT_STATUS_SUCCESS) {
      ret_val = AUDFMT_STATUS_ERROR;
    }
  }
  return (ret_val);
} /* end do_cmid_pitch_bend */

/* <EJECT> */
/*===========================================================================

FUNCTION get_cmid_bank

DESCRIPTION
  This function returns the correct bank value for the given channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint8 get_cmid_bank (
  audfmt_parse_type *midi_ctl,
  uint8             channel
) {
  uint8 ret_val;

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
  {
    if(midi_ctl->version <= 200) {
      if(channel == AUDFMT_DRUM_CHANNEL) {
        ret_val = CMID_BANK_DRUM;
      } else {
        ret_val = CMID_BANK_NORMAL;
      }
    } else if(midi_ctl->version <= 400) {
      if(channel == AUDFMT_DRUM_CHANNEL) {
        ret_val = CMID_BANK_DRUM;
      } else {
        if((midi_ctl->bank_msb[channel] >> 1) == 0) {
          ret_val = CMID_BANK_0;
        } else {
          ret_val = CMID_BANK_NORMAL;
        }
      }
    } else {
      if((midi_ctl->bank_msb[channel] >> 1) == 0) {
        ret_val = CMID_BANK_0;
      } else if((midi_ctl->bank_msb[channel] == 0x3f) ||
                (midi_ctl->bank_msb[channel] == 0x3d)) {
        ret_val = CMID_BANK_DRUM;
      } else if(midi_ctl->bank_msb[channel] == 0x3e) {
        ret_val = CMID_BANK_DRUM_HQ;
      } else if((midi_ctl->bank_msb[channel] == 0x4) ||
                (midi_ctl->bank_msb[channel] == 0x5)) {
        ret_val = CMID_BANK_NORMAL_HQ;
      } else {
        ret_val = CMID_BANK_NORMAL;
      }
    }
  }

  return(ret_val);
} /* end get_cmid_bank */

/* <EJECT> */
/*===========================================================================

FUNCTION get_cmid_bank_status

DESCRIPTION
  This function returns the bank status for the given channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_bank_status_type get_cmid_bank_status (
  audfmt_parse_type *midi_ctl,
  uint8             channel
) {
  audfmt_bank_status_type ret_val;

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
  if(((midi_ctl->version <= 400) && (channel == AUDFMT_DRUM_CHANNEL)) ||
     ((midi_ctl->version > 400) && ((midi_ctl->bank_msb[channel] == 0x3f) ||
                                    (midi_ctl->bank_msb[channel] == 0x3d) ||
                                    (midi_ctl->bank_msb[channel] == 0x3e)))) {
    ret_val = AUDFMT_BANK_DRUM;
  } else if((midi_ctl->version > 200) &&
            ((midi_ctl->bank_msb[channel] >> 1) == 0)) {
    ret_val = AUDFMT_BANK_LEGACY;
  } else {
    ret_val = AUDFMT_BANK_NORMAL;
  }

  return(ret_val);
} /* end get_cmid_bank_status */

/* <EJECT> */
/*===========================================================================

FUNCTION check_cmid_drum_bank_status

DESCRIPTION
  This function checks if the drum bank is set correctly for the current
  channel. If not, a command will be sent to DSP to configure to the correct
  bank.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void check_cmid_drum_bank_status (
  audfmt_parse_type *midi_ctl,
  uint8             channel
) {
  uint8 buf[AUDFMT_COMMAND_SIZE_4];
  uint8 bank;

  bank = get_cmid_bank(midi_ctl, channel);

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
  if(midi_ctl->bank_status[channel] != AUDFMT_BANK_DRUM) {
    buf[0] = MIDI_CONTROL_CHANGE;
    buf[1] = channel;
    buf[2] = MIDI_BANK_SEL_COMMAND;
    buf[3] = bank;

    if(audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4) ==
       AUDFMT_STATUS_SUCCESS) {
      midi_ctl->bank_status[channel] = AUDFMT_BANK_DRUM;
    }
  }
} /* end check_cmid_drum_bank_status */

/* <EJECT> */
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

/* <EJECT> */
/*===========================================================================

FUNCTION do_cmid_extension

DESCRIPTION
  This function processes a cmid extension event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type do_cmid_extension (
  uint8               command,
  audfmt_parse_type   *midi_ctl,
  uint8               track_num
) {
  audfmt_parse_type         *midi_ptr;
  snd_cmx_af_buf_rtn_type   buf_rtn = {NULL, 0, NULL, 0, 0};
  snd_cmx_af_pic_new_type   pic_state;
  snd_cmx_af_text_type      text_state;
  snd_cmx_af_saf_type       saf_state;
  snd_cmx_af_svg_frame_type svg_frame;
  snd_af_data_cmd_enum      af_cmd = SND_CMX_AF_INVALID;
  audfmt_status_type        ret_val;
  audfadec_wa_info_type     wa_info;
  audfmt_track_type         *track;
  uint32                    bytes_read = 2;

  uint32 length, channel;
  uint8  tbyte[8], counter;

  uint32 frame_id;

  track   = &midi_ctl->tracks[track_num];
  ret_val = audfmt_get_next_track_byte(track, &(tbyte[0]), &bytes_read);
  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    ret_val = audfmt_get_next_track_byte(track, &(tbyte[1]), &bytes_read);
  }

  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    length = tbyte[0];
    length = length<<8 | tbyte[1];

    if((midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                 AUDFMT_MASK_FIND_CUEP_END)) ||
       (midi_ctl->type == AUDFMT_MIDI_MELO_TYPE)) {
      return(audfmt_track_skip_ahead(midi_ctl, track, length));
    }

    switch(command) {
      case CMID_EXT_TEXT:
        if(midi_ctl->cb_store == NULL) {
          if(!audfmt_cb_store.used &&
             ((midi_ctl->midi_status & AUDFMT_MASK_CB_STORE_DONE) == 0)) {
            /* If this c-midi file has graphics/text/LED commands,
            ** then assign it as main c-midi file
            */
            audfmt_cb_store_reset(&audfmt_cb_store);
            audfmt_cb_store.used = TRUE;
            midi_ctl->cb_store   = &audfmt_cb_store;

            midi_ptr = audfmt_parse_ctl;
            for(counter = 0; counter < AUDFMT_NUM_SEQUENCES; counter++) {
              if(midi_ptr->midi_status & AUDFMT_MASK_ENABLED) {
                midi_ptr->midi_status |= AUDFMT_MASK_CB_STORE_DONE;
              }
              midi_ptr++;
            }
          } 
        }
        /* Check if current sequence has control of display output */
        if(midi_ctl->cb_store != NULL) {
          ret_val = audfmt_get_next_track_byte(track, &(tbyte[0]), &bytes_read);
          if(ret_val != AUDFMT_STATUS_SUCCESS) break;
          length--;
          if((tbyte[0] & CMID_EXT_TEXT_CMD_MASK) == CMID_EXT_TEXT_APPEND) {
            af_cmd = SND_CMX_AF_TEXT_APPEND;
          } else {
            af_cmd = SND_CMX_AF_TEXT_NEW;
          }

          switch(CMID_EXT_TEXT_X_POS(tbyte[0])) {
            case CMID_EXT_TEXT_X_POS_CENTER:
              text_state.xpos_type = SND_CMX_AF_POS_CENTER;
              break;

            case CMID_EXT_TEXT_X_POS_RIGHT:
              text_state.xpos_type = SND_CMX_AF_POS_RB;
              break;

            case CMID_EXT_TEXT_X_POS_LEFT:
            default:
              text_state.xpos_type = SND_CMX_AF_POS_LT;
              break;
          }

          switch(CMID_EXT_TEXT_Y_POS(tbyte[0])) {
            case CMID_EXT_TEXT_Y_POS_CENTER:
              text_state.ypos_type = SND_CMX_AF_POS_CENTER;
              break;

            case CMID_EXT_TEXT_Y_POS_BOTTOM:
              text_state.ypos_type = SND_CMX_AF_POS_RB;
              break;

            case CMID_EXT_TEXT_Y_POS_TOP:
            default:
              text_state.ypos_type = SND_CMX_AF_POS_LT;
              break;
          }

          buf_rtn.remain_bytes = length;
          text_state.buf_rtn   = &buf_rtn;

          ret_val = audfmt_buffer_cb(midi_ctl, af_cmd, track->current_location,
                                     &text_state);
        }

        if(ret_val == AUDFMT_STATUS_SUCCESS) {
          return(audfmt_track_skip_ahead(midi_ctl, track, length));
        }
        break;

      case CMID_EXT_PICTURE:
        /* Check if current sequence has control of display output */
        if(midi_ctl->cb_store == NULL) {
          if(!audfmt_cb_store.used &&
             ((midi_ctl->midi_status & AUDFMT_MASK_CB_STORE_DONE) == 0)) {
            /* If this c-midi file has graphics/text/LED commands,
            ** then assign it as main c-midi file
            */
            audfmt_cb_store_reset(&audfmt_cb_store);
            audfmt_cb_store.used = TRUE;
            midi_ctl->cb_store   = &audfmt_cb_store;

            midi_ptr = audfmt_parse_ctl;
            for(counter = 0; counter < AUDFMT_NUM_SEQUENCES; counter++) {
              if(midi_ptr->midi_status & AUDFMT_MASK_ENABLED) {
                midi_ptr->midi_status |= AUDFMT_MASK_CB_STORE_DONE;
              }
              midi_ptr++;
            }
          } else {
            return(audfmt_track_skip_ahead(midi_ctl, track, length));
          }
        }

        /* Read picture attributes */
        for(counter = 0; counter < 5; counter++) {
          ret_val = audfmt_get_next_track_byte(track, &(tbyte[counter]),
                                               &bytes_read);
          if(ret_val != AUDFMT_STATUS_SUCCESS) break;
        }
        if(ret_val == AUDFMT_STATUS_SUCCESS) {
          length -= 5;
          switch(CMID_EXT_MODE(tbyte[1])) {
            case CMID_EXT_MODE_STORE:
              counter = CMID_EXT_ID(tbyte[0]);
              midi_ctl->cb_store->pic_store[counter].location  =
                                                       track->current_location;
              midi_ctl->cb_store->pic_store[counter].num_bytes = length;
              return(audfmt_track_skip_ahead(midi_ctl, track, length));
              break;

            case CMID_EXT_MODE_SET:
              counter = CMID_EXT_ID(tbyte[0]);
              midi_ctl->cb_store->pic_store[counter].location  =
                                                       track->current_location;
              midi_ctl->cb_store->pic_store[counter].num_bytes = length;
              /* Fall thru */

            case CMID_EXT_MODE_RECYCLE:
              counter = CMID_EXT_ID(tbyte[0]);
              if(midi_ctl->cb_store->pic_store[counter].num_bytes != 0) {
                counter = CMID_EXT_PIC_FORMAT(tbyte[1]);
                if(counter == CMID_EXT_PIC_FORMAT_BMP) {
                  pic_state.pic_type = SND_CMX_AF_PIC_TYPE_BMP;
                } else if(counter == CMID_EXT_PIC_FORMAT_PNG) {
                  pic_state.pic_type = SND_CMX_AF_PIC_TYPE_PNG;
                } else if(counter == CMID_EXT_PIC_FORMAT_JPEG) {
                  pic_state.pic_type = SND_CMX_AF_PIC_TYPE_JPEG;
                } else {
                  pic_state.pic_type = SND_CMX_AF_PIC_TYPE_UNKNOWN;
                }

                if( tbyte[2] == CMID_EXT_PIC_DRAW_SLOW ) {
                  pic_state.draw_speed = SND_CMX_AF_PIC_SPEED_SLOW;
                } else if( tbyte[2] == CMID_EXT_PIC_DRAW_MIDDLE ) {
                  pic_state.draw_speed = SND_CMX_AF_PIC_SPEED_MIDDLE;
                } else if( tbyte[2] == CMID_EXT_PIC_DRAW_FAST ) {
                  pic_state.draw_speed = SND_CMX_AF_PIC_SPEED_FAST;
                } else {
                  pic_state.draw_speed = SND_CMX_AF_PIC_SPEED_NORMAL;
                }

                if(midi_ctl->midi_status2 & AUDFMT_MASK_PCPI_ENABLED) {
                  pic_state.xpos_type    = SND_CMX_AF_POS_PIXEL;
                  pic_state.xpos_percent = tbyte[3];
                  pic_state.ypos_type    = SND_CMX_AF_POS_PIXEL;
                  pic_state.ypos_percent = tbyte[4];
                } else {
                  if(CMID_EXT_PIC_OFFSET_PC(tbyte[3])) {
                    pic_state.xpos_type    = SND_CMX_AF_POS_PERCENT;
                    pic_state.xpos_percent = tbyte[3];
                  } else if(tbyte[3] == CMID_EXT_PIC_OFFSET_LT) {
                    pic_state.xpos_type    = SND_CMX_AF_POS_LT;
                  } else if(tbyte[3] == CMID_EXT_PIC_OFFSET_RB) {
                    pic_state.xpos_type    = SND_CMX_AF_POS_RB;
                  } else {
                    pic_state.xpos_type    = SND_CMX_AF_POS_CENTER;
                  }

                  if(CMID_EXT_PIC_OFFSET_PC(tbyte[4])) {
                    pic_state.ypos_type    = SND_CMX_AF_POS_PERCENT;
                    pic_state.ypos_percent = tbyte[4];
                  } else if(tbyte[4] == CMID_EXT_PIC_OFFSET_LT) {
                    pic_state.ypos_type    = SND_CMX_AF_POS_LT;
                  } else if(tbyte[4] == CMID_EXT_PIC_OFFSET_RB) {
                    pic_state.ypos_type    = SND_CMX_AF_POS_RB;
                  } else {
                    pic_state.ypos_type    = SND_CMX_AF_POS_CENTER;
                  }
                }

                counter = CMID_EXT_ID(tbyte[0]);
                buf_rtn.remain_bytes =
                              midi_ctl->cb_store->pic_store[counter].num_bytes;
                pic_state.buf_rtn    = &buf_rtn;

                audfmt_buffer_cb(midi_ctl, SND_CMX_AF_PIC_NEW,
                               midi_ctl->cb_store->pic_store[counter].location,
                               &pic_state);
              }
              /* Fall thru */

            default:
              return(audfmt_track_skip_ahead(midi_ctl, track, length));
              break;
          }
        }
        break;

      case CMID_EXT_SAF:
        /* Check if current sequence has control of display output */
        if(midi_ctl->cb_store == NULL) {
          if(!audfmt_cb_store.used &&
             ((midi_ctl->midi_status & AUDFMT_MASK_CB_STORE_DONE) == 0)) {
            /* If this c-midi file has graphics/text/LED commands,
            ** then assign it as main c-midi file
            */
            audfmt_cb_store_reset(&audfmt_cb_store);
            audfmt_cb_store.used = TRUE;
            midi_ctl->cb_store   = &audfmt_cb_store;

            midi_ptr = audfmt_parse_ctl;
            for(counter = 0; counter < AUDFMT_NUM_SEQUENCES; counter++) {
              if(midi_ptr->midi_status & AUDFMT_MASK_ENABLED) {
                midi_ptr->midi_status |= AUDFMT_MASK_CB_STORE_DONE;
              }
              midi_ptr++;
            }
          } else {
            return(audfmt_track_skip_ahead(midi_ctl, track, length));
          }
        }

        /* Read animation attributes */
        for(counter = 0; counter < 6; counter++) {
          ret_val = audfmt_get_next_track_byte(track, &(tbyte[counter]),
                                               &bytes_read);
          if(ret_val != AUDFMT_STATUS_SUCCESS) break;
        }

        if(ret_val == AUDFMT_STATUS_SUCCESS) {
          if(midi_ctl->version >= 500) {
            ret_val = audfmt_get_next_track_byte(track, &(tbyte[6]),
                                                 &bytes_read);
            if(ret_val != AUDFMT_STATUS_SUCCESS) break;

            ret_val = audfmt_get_next_track_byte(track, &(tbyte[7]),
                                                 &bytes_read);
            if(ret_val != AUDFMT_STATUS_SUCCESS) break;

            if(midi_ctl->midi_status2 & AUDFMT_MASK_PCPI_ENABLED) {
              saf_state.xpos_type   = SND_CMX_AF_POS_PIXEL;
              saf_state.xpos_num    = tbyte[6];
              saf_state.ypos_type   = SND_CMX_AF_POS_PIXEL;
              saf_state.ypos_num    = tbyte[7];
            } else {
              if(CMID_EXT_PIC_OFFSET_PC(tbyte[6])) {
                saf_state.xpos_type = SND_CMX_AF_POS_PERCENT;
                saf_state.xpos_num  = tbyte[6];
              } else if(tbyte[6] == CMID_EXT_PIC_OFFSET_LT) {
                saf_state.xpos_type = SND_CMX_AF_POS_LT;
              } else if(tbyte[6] == CMID_EXT_PIC_OFFSET_RB) {
                saf_state.xpos_type = SND_CMX_AF_POS_RB;
              } else {
                saf_state.xpos_type = SND_CMX_AF_POS_CENTER;
              }

              if(CMID_EXT_PIC_OFFSET_PC(tbyte[7])) {
                saf_state.ypos_type = SND_CMX_AF_POS_PERCENT;
                saf_state.ypos_num  = tbyte[7];
              } else if(tbyte[7] == CMID_EXT_PIC_OFFSET_LT) {
                saf_state.ypos_type = SND_CMX_AF_POS_LT;
              } else if(tbyte[7] == CMID_EXT_PIC_OFFSET_RB) {
                saf_state.ypos_type = SND_CMX_AF_POS_RB;
              } else {
                saf_state.ypos_type = SND_CMX_AF_POS_CENTER;
              }
            }
          } else {
            saf_state.xpos_type = SND_CMX_AF_POS_CENTER;
            saf_state.ypos_type = SND_CMX_AF_POS_CENTER;
          }
        }

        if(ret_val == AUDFMT_STATUS_SUCCESS) {
          if(length == 0) {
            length = tbyte[0] << 24 | tbyte[1] << 16 |
                     tbyte[2] << 8  | tbyte[3];
          } else {
            if(CMID_EXT_SAF_CONT(tbyte[3])) {
              saf_state.cont = SND_CMX_AF_SAF_CONT;
            } else {
              saf_state.cont = SND_CMX_AF_SAF_NOT_CONT;
            }
          }
          length -= 6;

          if(midi_ctl->version >= 500) {
            length -= 2;
          }

          switch(CMID_EXT_MODE(tbyte[4])) {
            case CMID_EXT_MODE_STORE:
              counter = CMID_EXT_ID(tbyte[4]);
              midi_ctl->cb_store->saf_store[counter].location  =
                                                       track->current_location;
              midi_ctl->cb_store->saf_store[counter].num_bytes = length;
              return(audfmt_track_skip_ahead(midi_ctl, track, length));
              break;

            case CMID_EXT_MODE_SET:
              counter = CMID_EXT_ID(tbyte[4]);
              midi_ctl->cb_store->saf_store[counter].location  =
                                                       track->current_location;
              midi_ctl->cb_store->saf_store[counter].num_bytes = length;
              /* Fall thru */

            case CMID_EXT_MODE_RECYCLE:
              counter = CMID_EXT_ID(tbyte[4]);
              if(midi_ctl->cb_store->saf_store[counter].num_bytes != 0) {
                counter = CMID_EXT_SAF_CMD(tbyte[5]);
                if((counter == CMID_EXT_SAF_CMD_FRAME_ID) &&
                   (CMID_EXT_SAF_FORMAT(tbyte[5]) == CMID_EXT_SAF_FORMAT_SAF) &&
                   (midi_ctl->midi_status & AUDFMT_MASK_SAF_OBJECTS_DONE)) {
                   ret_val = audfmt_get_next_track_byte(track, &(tbyte[0]),
                                                        &bytes_read);
                   if(ret_val != AUDFMT_STATUS_SUCCESS) break;
                   ret_val = audfmt_get_next_track_byte(track, &(tbyte[1]),
                                                        &bytes_read);
                  if(ret_val != AUDFMT_STATUS_SUCCESS) break;
                  length -= 2;

                  frame_id = tbyte[0] << 8 | tbyte[1];
                  audfmt_buffer_cb(midi_ctl, 
                                   SND_CMX_AF_SAF_FRAME_ID, frame_id,
                                   &saf_state);

                } else if((counter == CMID_EXT_SAF_CMD_FRAME) &&
                   (CMID_EXT_SAF_FORMAT(tbyte[5]) == CMID_EXT_SAF_FORMAT_SAF) &&
                   (midi_ctl->midi_status & AUDFMT_MASK_SAF_OBJECTS_DONE)) {
                  counter              = CMID_EXT_ID(tbyte[4]);
                  buf_rtn.remain_bytes =
                              midi_ctl->cb_store->saf_store[counter].num_bytes;
                  saf_state.buf_rtn    = &buf_rtn;

                  audfmt_buffer_cb(midi_ctl, SND_CMX_AF_SAF_FRAME,
                                midi_ctl->cb_store->saf_store[counter].location,
                                &saf_state);
                } else if((counter == CMID_EXT_SAF_CMD_DATA) &&
                   (CMID_EXT_SAF_FORMAT(tbyte[5]) == CMID_EXT_SAF_FORMAT_SAF) &&
                   !(midi_ctl->midi_status & AUDFMT_MASK_SAF_OBJECTS_DONE) &&
                   (midi_ctl->anim_format_type == 0xff)) {
                  counter              = CMID_EXT_ID(tbyte[4]);
                  buf_rtn.remain_bytes =
                              midi_ctl->cb_store->saf_store[counter].num_bytes;
                  saf_state.buf_rtn    = &buf_rtn;
                  saf_state.objbuf_size = midi_ctl->saf_objbuf_size;

                  audfmt_buffer_cb(midi_ctl, SND_CMX_AF_SAF,
                                midi_ctl->cb_store->saf_store[counter].location,
                                &saf_state);
                } else if((counter == CMID_EXT_ANIM_CHUNK_CMD_FRAME) &&
                          (midi_ctl->anim_format_type != 0xff)) {
                  if(CMID_EXT_SAF_FORMAT(tbyte[5]) ==
                                              midi_ctl->anim_format_type) {
                    for(counter = 0; counter < 5; counter++) {
                      ret_val = audfmt_get_next_track_byte(track,
                                                           &(tbyte[counter]),
                                                           &bytes_read);
                      if(ret_val != AUDFMT_STATUS_SUCCESS) break;
                    }

                    if(ret_val == AUDFMT_STATUS_SUCCESS) {
                      length -= 5;
                      if((tbyte[0] & 0x1f) == midi_ctl->anim_current_id) {
                        frame_id = tbyte[1] << 24 | tbyte[2] << 16 |
                                   tbyte[3] << 8  | tbyte[4];

                        if(midi_ctl->anim_format_type ==
                                                 CMID_EXT_SAF_FORMAT_SAF) {
                          audfmt_buffer_cb(midi_ctl,
                                           SND_CMX_AF_SAF_FRAME_ID, frame_id,
                                           &saf_state);
                        } else if(midi_ctl->anim_format_type ==
                                                 CMID_EXT_SAF_FORMAT_SVG) {
                          svg_frame.xpos_type = saf_state.xpos_type;
                          svg_frame.ypos_type = saf_state.ypos_type;
                          svg_frame.xpos_num  = saf_state.xpos_num;
                          svg_frame.ypos_num  = saf_state.ypos_num;
                          audfmt_buffer_cb(midi_ctl,
                                           SND_CMX_AF_SVG_FRAME_ID, frame_id,
                                           &svg_frame);
                        } 
                      }
                    } else {
                      break;
                    }
                  }
                }
              }
              /* Fall Thru */

            default:
              return(audfmt_track_skip_ahead(midi_ctl, track, length));
              break;
          }
        }
        break;

      case CMID_EXT_WAVE:
        /* Read wave attributes */
        for(counter = 0; counter < 7; counter++) {
          ret_val = audfmt_get_next_track_byte(track, &(tbyte[counter]),
                                               &bytes_read);
          if(ret_val != AUDFMT_STATUS_SUCCESS) break;
        }

        if(ret_val == AUDFMT_STATUS_SUCCESS) {
          length -= 7;

          /* If previous flag is set and this WAV object was already started
          ** or if format not supported, then skip.
          */
          if((CMID_EXT_WAVE_FORMAT(tbyte[1]) != CMID_EXT_WAVE_QCP_FORMAT) &&
             (CMID_EXT_WAVE_FORMAT(tbyte[1]) != CMID_EXT_WAVE_AAC_FORMAT) &&
             (CMID_EXT_WAVE_FORMAT(tbyte[1]) != CMID_EXT_WAVE_ADPCM_FORMAT)) {
            return(audfmt_track_skip_ahead(midi_ctl, track, length));
          }

          if(CMID_EXT_WAVE_PREV(tbyte[6])) {
            channel = CMID_CHANNEL_INDEX(tbyte[0]);
            for(counter = 0; counter < AUDFMT_NUM_ADEC_CHANNELS; counter++) {
              if((midi_ctl->adec_status[counter].enable) &&
                 (midi_ctl->adec_status[counter].channel == channel)) {
                return(audfmt_track_skip_ahead(midi_ctl, track, length));
              }
            }             
          }

          switch(CMID_EXT_MODE(tbyte[1])) {
            case CMID_EXT_MODE_STORE:
              if(!CMID_EXT_WAVE_PREV(tbyte[6])) {
                counter = CMID_EXT_ID(tbyte[0]);
                midi_ctl->wav_store[counter].location  =
                                                 track->current_location - 5;
                midi_ctl->wav_store[counter].num_bytes = length + 5;
              }
              return(audfmt_track_skip_ahead(midi_ctl, track, length));
              break;

            case CMID_EXT_MODE_RECYCLE:
              if(!(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE)) {
                counter = CMID_CHANNEL_INDEX(tbyte[0]);
                for(channel = 0; channel < AUDFMT_NUM_ADEC_CHANNELS; channel++)
                {
                  if((midi_ctl->adec_status[channel].enable) &&
                     (midi_ctl->adec_status[channel].channel == counter)) {
                    break;
                  }
                }

                if((channel < AUDFMT_NUM_ADEC_CHANNELS) ||
                   (audfmt_get_adec_channel(midi_ctl, &channel) ==
                    AUDFMT_STATUS_SUCCESS)) {
                  counter = CMID_EXT_ID(tbyte[0]);
                  if(midi_ctl->wav_store[counter].num_bytes != 0) {
                    wa_info.block_start = 
                                   midi_ctl->wav_store[counter].location - 6;
                    wa_info.location   = midi_ctl->wav_store[counter].location;
                    wa_info.length     = midi_ctl->wav_store[counter].num_bytes;
                    wa_info.next_block = 0;

                    wa_info.buf1        = NULL;
                    wa_info.length_buf1 = 0;
                    wa_info.buf2        = NULL;
                    wa_info.length_buf2 = 0;
                    wa_info.adec_mode   = (audmain_adec_mode_enum_type)
                                                      CMID_ADEC_MODE(tbyte[1]);
                    wa_info.mfi_flag    = FALSE;

                    midi_ctl->adec_status[channel].enable  = TRUE;
                    midi_ctl->adec_status[channel].volume  = TRUE;
                    midi_ctl->adec_status[channel].panning = TRUE;
                    midi_ctl->adec_status[channel].channel =
                                                 CMID_CHANNEL_INDEX(tbyte[0]);

                    audfadec_wa_out_qcp(track->audfmt_handle, channel, &wa_info,
                                        main_audfmt_parse_ctl, TRUE);
                  }
                }
              }
              return(audfmt_track_skip_ahead(midi_ctl, track, length));
              break;

            case CMID_EXT_MODE_SET:
              counter = CMID_EXT_ID(tbyte[0]);
              if(!CMID_EXT_WAVE_PREV(tbyte[6])) {
                midi_ctl->wav_store[counter].location  =
                                               track->current_location - 5;
                midi_ctl->wav_store[counter].num_bytes = length + 5;
              }

              if(!(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE)) {
                counter = CMID_CHANNEL_INDEX(tbyte[0]);
                for(channel = 0; channel < AUDFMT_NUM_ADEC_CHANNELS; channel++)
                {
                  if((midi_ctl->adec_status[channel].enable) &&
                     (midi_ctl->adec_status[channel].channel == counter)) {
                    break;
                  }
                }

                if((channel < AUDFMT_NUM_ADEC_CHANNELS) ||
                   (audfmt_get_adec_channel(midi_ctl, &channel) ==
                    AUDFMT_STATUS_SUCCESS)) {
                  wa_info.block_start = track->current_location - 11;
                  wa_info.location    = track->current_location;
                  wa_info.length      = length;
                  wa_info.buf1        = NULL;
                  wa_info.length_buf1 = 0;
                  wa_info.buf2        = NULL;
                  wa_info.length_buf2 = 0;
                  wa_info.next_block  = tbyte[2] << 24 | tbyte[3] << 16 |
                                        tbyte[4] << 8  | tbyte[5];
                  wa_info.adec_mode   = (audmain_adec_mode_enum_type)
                                                   CMID_ADEC_MODE(tbyte[1]);
                  wa_info.mfi_flag    = FALSE;

                  if(wa_info.next_block != 0) {
                    wa_info.next_block  += wa_info.block_start;
                  }

                  counter = CMID_CHANNEL_INDEX(tbyte[0]);
#ifdef FEATURE_ANIMATED_AAC
                  if(wa_info.adec_mode == AUDMAIN_ADEC_MODE_AAC) {
                    if(counter != 0) {
                      /* Should I just ignore it or return an error? */
                      return(audfmt_track_skip_ahead(midi_ctl, track, length));
                      break;
                    }
                  }
#endif /* FEATURE_ANIMATED_AAC */
                  midi_ctl->adec_status[channel].enable  = TRUE;
                  midi_ctl->adec_status[channel].volume  = TRUE;
                  midi_ctl->adec_status[channel].panning = TRUE;
                  midi_ctl->adec_status[channel].channel = counter;

                  audfadec_wa_out_qcp(track->audfmt_handle, channel, &wa_info,
                                      main_audfmt_parse_ctl, FALSE);
                }
              }
              /* Fall thru */

            default:
              return(audfmt_track_skip_ahead(midi_ctl, track, length));
              break;
          }
        }
        break;

      case CMID_EXT_WAVETABLE:
      default:
        return(audfmt_track_skip_ahead(midi_ctl, track, length));
        break;
    }
  }

  if((ret_val != AUDFMT_STATUS_SUCCESS) && (ret_val != AUDFMT_STATUS_ERROR)) {
    /* If this command parse was not successful, restore the track to
    ** original condition.
    */
    if(audfmt_restore_track(track, bytes_read) != AUDFMT_STATUS_SUCCESS) {
      ret_val = AUDFMT_STATUS_ERROR;
    }
  }

  return(ret_val);
} /* end do_cmid_extension */

/* <EJECT> */
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

/* <EJECT> */
/*===========================================================================

FUNCTION do_cmid_end_cuep

DESCRIPTION
  This function processes a cmid cuepoint end event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type do_cmid_end_cuep (
  audfmt_parse_type *midi_ctl
) {
  audfmt_track_type  *track = midi_ctl->tracks;
  uint32             tlong;

  if(midi_ctl->midi_status & AUDFMT_MASK_FIND_CUEP_END) {
    midi_ctl->midi_status &= ~AUDFMT_MASK_FIND_CUEP_END;
  } else {
    if(midi_ctl->cuep_repeat != 0) {
      midi_ctl->cuep_repeat--;
      if(midi_ctl->cuep_repeat == 0) {
        track++;
        for(tlong = 1; tlong < midi_ctl->num_tracks; tlong ++) {
          if(track->audfmt_handle != NULL) {
            audfmt_do_play_cb(track->audfmt_handle, SND_CMX_AF_FREE_BUF, 0,
                              (void *) track->track_buf);
            audfmt_do_play_cb(track->audfmt_handle, SND_CMX_AF_FREE_BUF, 0,
                              (void *) track->track_buf_reserve);
            track->audfmt_handle = NULL;
          }
          track++;
        }
        midi_ctl->current_header_track = 1;
        return(AUDFMT_STATUS_DONE);
      }
    }
    midi_ctl->midi_status2       |= AUDFMT_MASK_CUEP_PENDING;
  }

  midi_ctl->tracks->track_status |= AUDFMT_TRACK_MASK_BUFF_DELAY;
  midi_ctl->midi_status          |= AUDFMT_MASK_PENDING_BUF;

  midi_ctl->current_header_track = 0;
  for(tlong = 0; tlong < midi_ctl->num_tracks; tlong ++) {
    if(midi_ctl->cuep_offset[tlong] != AUDFMT_MAX_32BIT) {
      track->audfmt_handle     = midi_ctl->handle;
      audfmt_do_play_cb(track->audfmt_handle, SND_CMX_AF_FREE_BUF,
                        0, (void *) track->track_buf);
      audfmt_do_play_cb(track->audfmt_handle, SND_CMX_AF_FREE_BUF,
                        0, (void *) track->track_buf_reserve);
      track->track_buf         = NULL;
      track->buf_size          = 0;
      track->track_buf_reserve = NULL;
      track->buf_reserve_size  = 0;
      track->index             = 0;
      track->file_location     = midi_ctl->track_start[tlong] +
                                 midi_ctl->cuep_offset[tlong];
      track->current_location  = track->file_location;

      if(midi_ctl->cuep_do_tick[tlong]) {
        track->delta_tick    = 0;
        track->track_status |= AUDFMT_TRACK_MASK_DO_TIME;
      } else {
        track->delta_tick    = midi_ctl->cuep_tick[tlong];
      }

      if(track->track_status & AUDFMT_TRACK_MASK_PENDING_BUF) {
        track->skip_buf_count++;
      }
      track->track_status     |= AUDFMT_TRACK_MASK_PENDING_BUF;
      if(tlong != 0) {
        track->track_status   &= ~AUDFMT_TRACK_MASK_BUFF_DELAY;
      }

      midi_ctl->current_header_track++;
      midi_ctl->num_read++;
      audmain_get_async_data(track->audfmt_handle, track->file_location,
                             &(track->track_buf));
    }
    track++;
  }
  return(AUDFMT_STATUS_JUMP);
} /* end do_cmid_end_cuep */

/* <EJECT> */
/*===========================================================================

FUNCTION do_cmid_statusB

DESCRIPTION
  This function processes a cmid status B event.

DEPENDENCIES
  cmid_timebase_table

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type do_cmid_statusB (
  uint8               command,
  audfmt_parse_type   *midi_ctl,
  uint8               track_num
) {
  audfmt_status_type             ret_val;
  audfmt_cmd_buf_type            *cmd_buf;
  audfmt_track_type              *track;
  audfmt_jump_struct_type        *jump_ptr;
  snd_cmx_af_pic_speed_enum_type af_speed = SND_CMX_AF_PIC_SPEED_NORMAL;
  snd_af_data_cmd_enum           af_cmd   = SND_CMX_AF_INVALID;
  snd_cmx_af_led_ctl_type        led_ctl;
  uint32                         bytes_read = 2, tlong = 0;
  uint8                          data, channel, buf[8];

#ifdef FEATURE_DLS
  audfmt_inst_def_type           *inst_def;
#endif /* FEATURE_DLS */

  track   = &midi_ctl->tracks[track_num];
  cmd_buf = &midi_ctl->cmd_buf;

  ret_val = audfmt_get_next_track_byte(track, &data, &bytes_read);
  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    if(CMID_STATUSB_TEMPO(command)) {
      if(data < AUDFMT_CMID_MIN_TEMPO) {
        data = AUDFMT_CMID_MIN_TEMPO;
      }
      midi_ctl->cmid_tempo    = data;
      midi_ctl->cmid_timebase = command & CMID_TIMEBASE_NIBBLE;
      midi_ctl->tempo_pending = data * cmid_timebase_table
                                            [command & CMID_TIMEBASE_NIBBLE];
      if(!(midi_ctl->midi_status & (AUDFMT_MASK_STARTED_AGE |
                                    AUDFMT_MASK_SEEK_ACTIVE)) &&
         (midi_ctl->delta_tick == 0)) {
        midi_ctl->tempo = midi_ctl->tempo_pending;
        if(midi_ctl->tempo != 0) {
          midi_ctl->calc_tick_of = AUDFMT_MAX_32BIT / midi_ctl->tempo;
        }
      }
    } else if(command == CMID_END_OF_TRACK) {
      if((midi_ctl->cuep_offset[0] != AUDFMT_MAX_32BIT) && (track_num == 0)) {
        ret_val = do_cmid_end_cuep(midi_ctl);
      } else {
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
        return(AUDFMT_STATUS_DONE);
      }
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
    } else if(((midi_ctl->type == AUDFMT_MIDI_MELO_TYPE) &&
               (command == CMID_MELO_JUMP)) ||
              ((midi_ctl->type == AUDFMT_MIDI_CMID_TYPE) &&
               (command == CMID_JUMP))) {
      if((track_num == 0) &&
         !(midi_ctl->midi_status & (AUDFMT_MASK_FIND_CUEP_END |
                                    AUDFMT_MASK_FIND_POLY_CMD))) {
        if(midi_ctl->type == AUDFMT_MIDI_MELO_TYPE) {
          if(MELO_INVALID_JUMP_POINT(data)) {
            return(ret_val);
          }
          buf[0] = MELO_JUMP_ID(data);
          buf[1] = MELO_JUMP_POINT(data);
          buf[2] = MELO_JUMP_COUNT(data);
        } else {
          buf[0] = CMID_JUMP_ID(data);
          buf[1] = CMID_JUMP_POINT(data);
          buf[2] = CMID_JUMP_COUNT(data);
        }

        jump_ptr = &midi_ctl->jump_info[buf[0]];
        if(buf[1]) {
          /* Jump point command */
          if(jump_ptr->active == FALSE) {
            /* There is no jump destination saved, ignore this command */
            return(ret_val);
          }

          if(jump_ptr->same_time) {
            ret_val = AUDFMT_STATUS_STOP;
          } else {
            if(CMID_JUMP_INIT(jump_ptr->repeat_num)) {
              /* If first time at this jump point, initialize repeat count */
              jump_ptr->repeat_num = buf[2];
              if((midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME) &&
                 (((midi_ctl->type == AUDFMT_MIDI_MELO_TYPE) &&
                   MELO_JUMP_INDEFINITE(jump_ptr->repeat_num)) ||
                  ((midi_ctl->type == AUDFMT_MIDI_CMID_TYPE) &&
                   CMID_JUMP_INDEFINITE(jump_ptr->repeat_num)))) {
                /* If infinite repeat and doing time calculation, ignore
                ** command 
                */
                jump_ptr->active = FALSE;
                return(ret_val);
              }
            }

            if((midi_ctl->type != AUDFMT_MIDI_MELO_TYPE) &&
               (jump_ptr->repeat_num == 0)) {
              /* If repeat count is zero, finished jumping */
              jump_ptr->active = FALSE;
              return(ret_val);
            }

            if((jump_ptr->repeat_num > 0) && 
               ((midi_ctl->type == AUDFMT_MIDI_MELO_TYPE) ||
                !CMID_JUMP_INDEFINITE(jump_ptr->repeat_num))) {
              /* Decrement repeat count */
              jump_ptr->repeat_num--;
              if(jump_ptr->repeat_num == 0) {
                jump_ptr->active = FALSE;
              }
            }

            midi_ctl->current_header_track = 0;

            midi_ctl->tracks->track_status |= AUDFMT_TRACK_MASK_BUFF_DELAY |
                                              AUDFMT_TRACK_MASK_DO_TIME;
            midi_ctl->midi_status          |= AUDFMT_MASK_PENDING_BUF;
            midi_ctl->midi_status2         |= AUDFMT_MASK_JUMP_PENDING;

            /* Reload data buffers with jump destination data */
            track = midi_ctl->tracks;
            for(tlong = 0; tlong < midi_ctl->num_tracks; tlong ++) {
              if(jump_ptr->location[tlong] != 0) {
                track->audfmt_handle = midi_ctl->handle;
                audfmt_do_play_cb(track->audfmt_handle, SND_CMX_AF_FREE_BUF,
                                  0, (void *) track->track_buf);
                audfmt_do_play_cb(track->audfmt_handle, SND_CMX_AF_FREE_BUF,
                                  0, (void *) track->track_buf_reserve);
                track->track_buf         = NULL;
                track->buf_size          = 0;
                track->track_buf_reserve = NULL;
                track->buf_reserve_size  = 0;
                track->index             = 0;
                track->delta_tick        = jump_ptr->delta_tick[tlong];
                track->file_location     = jump_ptr->location[tlong];
                track->current_location  = track->file_location;

                if(track->track_status & AUDFMT_TRACK_MASK_PENDING_BUF) {
                  track->skip_buf_count++;
                }
                track->track_status     |= AUDFMT_TRACK_MASK_PENDING_BUF;
                if(tlong != 0) {
                  track->track_status   &= ~AUDFMT_TRACK_MASK_BUFF_DELAY;
                  if(jump_ptr->do_delta_tick[tlong]) {
                    track->track_status |= AUDFMT_TRACK_MASK_DO_TIME;
                  } else {
                    track->track_status &= ~AUDFMT_TRACK_MASK_DO_TIME;
                  }
                }

                midi_ctl->current_header_track++;
                midi_ctl->num_read++;
                if(audmain_get_async_data(track->audfmt_handle,
                                          track->file_location,
                                          &(track->track_buf)) !=
                   AUDMAIN_STATUS_SUCCESS) {
                  ret_val = AUDFMT_STATUS_ERROR;
                }
              }
              track++;
            }
            ret_val = AUDFMT_STATUS_JUMP;
          }
        } else {

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

          /* Jump destination command */
          jump_ptr->same_time        = TRUE;
          jump_ptr->location[0]      = track->current_location;
          jump_ptr->delta_tick[0]    = 0;
          jump_ptr->do_delta_tick[0] = TRUE;
          jump_ptr->repeat_num       = CMID_JUMP_INIT_VAL;
          jump_ptr->active           = TRUE;

          for(tlong = 1; tlong < midi_ctl->num_tracks; tlong ++) {
            track = &midi_ctl->tracks[tlong];
            if(track->audfmt_handle != NULL) {
              jump_ptr->location[tlong]      = track->current_location;
              if(track->track_status & AUDFMT_TRACK_MASK_DO_TIME) {
                jump_ptr->delta_tick[tlong]    = 0;
                jump_ptr->do_delta_tick[tlong] = TRUE;
              } else {
                jump_ptr->delta_tick[tlong]    = track->delta_tick;
                jump_ptr->do_delta_tick[tlong] = FALSE;
              }
            } else {
              jump_ptr->location[tlong]      = 0;
              jump_ptr->delta_tick[tlong]    = 0;
              jump_ptr->do_delta_tick[tlong] = FALSE;
            }
          }
        }
      }
    } else if(command == CMID_CUE_POINT) {
      if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME) &&
         (midi_ctl->cuep_offset[0] != AUDFMT_MAX_32BIT) && (track_num == 0)) {
        if(data == CMID_CUE_POINT_START) {
          if(midi_ctl->midi_status & AUDFMT_MASK_FIND_CUEP_END) {
            track = midi_ctl->tracks;
            midi_ctl->cuep_offset[0]  = track->current_location -
                                           midi_ctl->track_start[0] - 3;
            midi_ctl->cuep_do_tick[0] = FALSE;
            midi_ctl->cuep_tick[0]    = 0;

            track++;
            for(tlong = 1; tlong < midi_ctl->num_tracks; tlong ++) {
              if(track->audfmt_handle != NULL) {
                midi_ctl->cuep_offset[tlong] = track->current_location -
                                               midi_ctl->track_start[tlong];

                if(track->track_status & AUDFMT_TRACK_MASK_DO_TIME) {
                  midi_ctl->cuep_do_tick[tlong] = TRUE;
                  midi_ctl->cuep_tick[tlong]    = 0;
                } else {
                  midi_ctl->cuep_do_tick[tlong] = FALSE;
                  midi_ctl->cuep_tick[tlong]    = track->delta_tick;
                }
              } else {
                midi_ctl->cuep_offset[tlong] = AUDFMT_MAX_32BIT;
              }
              track++;
            }
          }
        } else if(data == CMID_CUE_POINT_END) {
          ret_val = do_cmid_end_cuep(midi_ctl);
        }
      }
    } else if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                         AUDFMT_MASK_FIND_CUEP_END |
                                         AUDFMT_MASK_FIND_POLY_CMD))) {
      channel = CMID_CHANNEL(CMID_CHANNEL_INDEX(data), track_num,
                             midi_ctl->num_tracks, midi_ctl->channels);
      switch(command) {
        case CMID_MASTER_BALANCE:
        case CMID_MASTER_MUTE:
        case CMID_AUDIO_EFFECT:
        case CMID_VISUAL_EFFECT:
        case CMID_PLAY_MODE:
        case CMID_PLAY_PART:
        case CMID_PLAY_MEDIA:
        case CMID_PART_CONFIG:
          break;

        case CMID_MASTER_VOLUME:
          buf[0] = 0xf0;
          buf[1] = 0x7f;
          buf[2] = 0x7f;
          buf[3] = 0x04;
          buf[4] = 0x01;
          buf[5] = 0;
          buf[6] = data & CMID_MASTER_BIT_MASK;
          buf[7] = 0xf7;
          ret_val = audfmt_add_command(midi_ctl, buf, 8);
          midi_ctl->cmid_master_vol = buf[6];
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
          break;

        case CMID_MASTER_TUNE:
          if(midi_ctl->type != AUDFMT_MIDI_MELO_TYPE) {
            buf[0] = 0xf0;
            buf[1] = 0x7f;
            buf[2] = 0x7f;
            buf[3] = 0x04;
            buf[4] = 0x04;
            buf[5] = 0x00;
            buf[6] = data & CMID_MASTER_BIT_MASK; 
            buf[7] = 0xf7;
            ret_val = audfmt_add_command(midi_ctl, buf, 8);
          }
          break;

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

        case CMID_PAUSE: /* This is relative master volume command for MELO */
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
          if(!(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE)) {
            midi_ctl->midi_status2 |= AUDFMT_MASK_PAUSE_PENDING;
          }
          break;

        case CMID_STOP:
          /* Use an all notes off command instead of a stop command because
          ** stop is not supported by QSynth image.
          */
          buf[0]  = MIDI_CONTROL_CHANGE;
          buf[1]  = MIDI_ALL_CHANNELS;
          buf[2]  = MIDI_ALL_NOTE_OFF_COMMAND;
          buf[3]  = 0;
          ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
          break;

        case CMID_RESET:
          buf[0]  = MIDI_CONTROL_CHANGE;
          buf[1]  = MIDI_ALL_CHANNELS;
          buf[2]  = MIDI_ALL_CONTROLLERS_RESET;
          buf[3]  = 0;
          ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
          break;

        case CMID_PROGRAM_CHANGE:
          data = data & CMID_PAYLOAD_MASK;
          if((midi_ctl->bank_msb[channel] & 0x1) != 0) {
            data |= CMID_BANK_SHIFT;
          }
          midi_ctl->program[channel] = data;
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

#ifdef FEATURE_DLS
          if(get_cmid_bank_status(midi_ctl, channel) == AUDFMT_BANK_NORMAL) {
            inst_def = audfmt_dls_table.inst_def;
            for(tlong = 0; tlong < AUDFMT_NUM_INST_DEF; tlong++) {
              if(((inst_def->in_use_flag ==
                   audfmt_prog_def_in_use_seq[midi_ctl->sequence]) ||
                  (inst_def->in_use_flag == AUDFMT_PROG_DEF_IN_USE_ALL)) &&
                 ((inst_def->bank_msb == midi_ctl->bank_msb[channel]) ||
                  (inst_def->bank_msb == 0)) &&
                 (inst_def->program == data)) {
                break;
              }
              inst_def++;
            }

            if(tlong < AUDFMT_NUM_INST_DEF) {
              data = tlong;
              if(midi_ctl->bank_status[channel] != AUDFMT_BANK_NORMAL_DLS) {
                buf[0]  = MIDI_CONTROL_CHANGE;
                buf[1]  = channel;
                buf[2]  = MIDI_BANK_SEL_COMMAND;
                buf[3]  = 0x73;
                ret_val = audfmt_add_command(midi_ctl, buf,
                                                   AUDFMT_COMMAND_SIZE_4);
                if(ret_val == AUDFMT_STATUS_SUCCESS) {
                  midi_ctl->bank_status[channel] = AUDFMT_BANK_NORMAL_DLS;
                }
              }
            } else {
              if(midi_ctl->bank_status[channel] != AUDFMT_BANK_NORMAL) {
                buf[0]  = MIDI_CONTROL_CHANGE;
                buf[1]  = channel;
                buf[2]  = MIDI_BANK_SEL_COMMAND;
                buf[3]  = get_cmid_bank(midi_ctl, channel);
                ret_val = audfmt_add_command(midi_ctl, buf,
                                                   AUDFMT_COMMAND_SIZE_4);
                if(ret_val == AUDFMT_STATUS_SUCCESS) {
                  midi_ctl->bank_status[channel] = AUDFMT_BANK_NORMAL;
                }
              }
            }
          }
#endif /* FEATURE_DLS */

          buf[0] = MIDI_PROGRAM_CHANGE;
          buf[1] = channel;
          buf[2] = data;
          ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_3);
          break;

        case CMID_BANK_CHANGE:
          midi_ctl->bank_msb[channel] = data & CMID_PAYLOAD_MASK;

          buf[0]  = MIDI_CONTROL_CHANGE;
          buf[1]  = channel;
          buf[2]  = MIDI_BANK_SEL_COMMAND;
          buf[3]  = get_cmid_bank(midi_ctl, channel);

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

          ret_val = audfmt_add_command(midi_ctl, buf,
                                             AUDFMT_COMMAND_SIZE_4);

          midi_ctl->bank_status[channel] = get_cmid_bank_status(midi_ctl,
                                                                buf[1]);
          break;

        case CMID_CHANNEL_VOLUME:
          midi_ctl->chan_vol[channel] = (data & CMID_PAYLOAD_MASK);
          buf[0]  = MIDI_CONTROL_CHANGE;
          buf[1]  = channel;
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
          {
            buf[2]  = MIDI_VOLUME_COMMAND;
          }
          buf[3]  = midi_ctl->chan_vol[channel] << 1;
          ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
          break;

        case CMID_CHANNEL_PANPOT:
          buf[0]  = MIDI_CONTROL_CHANGE;
          buf[1]  = channel;
          buf[2]  = MIDI_PANPOT_COMMAND;
          buf[3]  = (data & CMID_PAYLOAD_MASK) << 1;
          ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
          break;

        case CMID_CHANNEL_PITCH_BEND:
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
          {
            if(midi_ctl->version <= 300) {
              buf[0]  = MIDI_PITCH_WHEEL;
              buf[1]  = channel;
              buf[2]  = 0;
              buf[3]  = (uint8) ((data & CMID_PAYLOAD_MASK) * 2);
              ret_val = audfmt_add_command(midi_ctl, buf,
                                           AUDFMT_COMMAND_SIZE_4);
            }
          }
          break;

        case CMID_CHANNEL_PB_RANGE:
          data  = CMID_PB_RANGE(data);
          if(data > CMID_PB_RANGE_MAX) {
            data = CMID_PB_RANGE_MAX;
          }
          ret_val = audfmt_set_pitchrange_cmd(midi_ctl, channel, data);
          break;

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

        case CMID_CHANNEL_ASSIGN:
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
          if(midi_ctl->num_tracks == 1) {
            midi_ctl->channels[CMID_CHANNEL_INDEX(data)] =
                                                   data & CMID_PROGRAM_MASK;
          } else {
            MSG_ERROR("Channel assign with more than 1 track!",0,0,0);
          }
          break;

        case CMID_WAVE_CHANNEL_VOLUME:
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
          {
            channel = CMID_CHANNEL_INDEX(data);
            midi_ctl->wav_chan_vol[channel] = (uint16) ADEC_CONTENT_VOL(
                    (CMID_VOL_TO_DSP((uint16) (data & CMID_PAYLOAD_MASK))),
                    midi_ctl->adec_chan_vol[channel]);

            if(midi_ctl->wav_chan_vol[channel] > CMID_MAX_WAV_DSP_VOLUME) {
              midi_ctl->wav_chan_vol[channel] = CMID_MAX_WAV_DSP_VOLUME;
            }

            for(tlong = 0; tlong < AUDFMT_NUM_ADEC_CHANNELS; tlong++) {
              if((midi_ctl->adec_status[tlong].enable) &&
                 (midi_ctl->adec_status[tlong].channel == channel)) {
                midi_ctl->adec_status[tlong].volume = TRUE;
              }
            }
          }
          break;

        case CMID_WAVE_CHANNEL_BALANCE:
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
          {
            channel = CMID_CHANNEL_INDEX(data);
            midi_ctl->wav_chan_pan[channel] = 
                           CMID_PAN_TO_DSP((sint15) (data & CMID_PAYLOAD_MASK));

            for(tlong = 0; tlong < AUDFMT_NUM_ADEC_CHANNELS; tlong++) {
              if((midi_ctl->adec_status[tlong].enable) &&
                 (midi_ctl->adec_status[tlong].channel == channel)) {
                midi_ctl->adec_status[tlong].panning = TRUE;
              }
            }
          }
          break;

        case CMID_WAVE_DETUNE:
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
          break;

        case CMID_TEXT_CONTROL:
          if(midi_ctl->cb_store != NULL) {
            if(data == CMID_TEXT_ENABLE) {
              af_cmd = SND_CMX_AF_TEXT_ENABLE;
            } else if(data == CMID_TEXT_DISABLE) {
              af_cmd = SND_CMX_AF_TEXT_DISABLE;
            } else if(data == CMID_TEXT_CLEAR) {
              af_cmd = SND_CMX_AF_TEXT_CLEAR;
            } else if(data == CMID_TEXT_ADVANCE_ONE) {
              af_cmd = SND_CMX_AF_TEXT_ADVANCE;
              tlong  = 1;
            } else if(data == CMID_TEXT_ADVANCE_TWO) {
              af_cmd = SND_CMX_AF_TEXT_ADVANCE;
              tlong  = 2;
            }
            if(af_cmd != SND_CMX_AF_INVALID) {
              audfmt_buffer_cb(midi_ctl, af_cmd, tlong, NULL);
            }
          }
          break;

        case CMID_PICTURE_CONTROL:
          if(midi_ctl->cb_store != NULL) {
            if(data == CMID_PICTURE_ENABLE) {
              af_cmd   = SND_CMX_AF_PIC_ENABLE;
            } else if(data == CMID_PICTURE_DISABLE) {
              af_cmd   = SND_CMX_AF_PIC_DISABLE;
            } else if(data == CMID_PICTURE_CLEAR_NORMAL) {
              af_cmd   = SND_CMX_AF_PIC_CLEAR;
              af_speed = SND_CMX_AF_PIC_SPEED_NORMAL;
            }

            if(af_cmd != SND_CMX_AF_INVALID) {
              audfmt_buffer_cb(midi_ctl, af_cmd, 0, &af_speed);
            }
          }
          break;

        case CMID_LED_CONTROL:
          if(midi_ctl->cb_store != NULL) {
            if(CMID_LED_ENABLE(data)) {
              switch (CMID_LED_CTL(data)) {
                case CMID_LED_DARK_BLACK:
                  led_ctl.led_ctl = SND_CMX_AF_LED_DARK_BLACK;
                  break;
                case CMID_LED_DARK_BLUE:
                  led_ctl.led_ctl = SND_CMX_AF_LED_DARK_BLUE;
                  break;
                case CMID_LED_DARK_GREEN:
                  led_ctl.led_ctl = SND_CMX_AF_LED_DARK_GREEN;
                  break;
                case CMID_LED_DARK_CYAN:
                  led_ctl.led_ctl = SND_CMX_AF_LED_DARK_CYAN;
                  break;
                case CMID_LED_DARK_RED:
                  led_ctl.led_ctl = SND_CMX_AF_LED_DARK_RED;
                  break;
                case CMID_LED_DARK_PURPLE:
                  led_ctl.led_ctl = SND_CMX_AF_LED_DARK_PURPLE;
                  break;
                case CMID_LED_DARK_YELLOW:
                  led_ctl.led_ctl = SND_CMX_AF_LED_DARK_YELLOW;
                  break;
                case CMID_LED_DARK_WHITE:
                  led_ctl.led_ctl = SND_CMX_AF_LED_DARK_WHITE;
                  break;
                case CMID_LED_BRIGHT_BLACK:
                  led_ctl.led_ctl = SND_CMX_AF_LED_BRIGHT_BLACK;
                  break;
                case CMID_LED_BRIGHT_BLUE:
                  led_ctl.led_ctl = SND_CMX_AF_LED_BRIGHT_BLUE;
                  break;
                case CMID_LED_BRIGHT_GREEN:
                  led_ctl.led_ctl = SND_CMX_AF_LED_BRIGHT_GREEN;
                  break;
                case CMID_LED_BRIGHT_CYAN:
                  led_ctl.led_ctl = SND_CMX_AF_LED_BRIGHT_CYAN;
                  break;
                case CMID_LED_BRIGHT_RED:
                  led_ctl.led_ctl = SND_CMX_AF_LED_BRIGHT_RED;
                  break;
                case CMID_LED_BRIGHT_PURPLE:
                  led_ctl.led_ctl = SND_CMX_AF_LED_BRIGHT_PURPLE;
                  break;
                case CMID_LED_BRIGHT_YELLOW:
                  led_ctl.led_ctl = SND_CMX_AF_LED_BRIGHT_YELLOW;
                  break;
                case CMID_LED_BRIGHT_WHITE:
                  led_ctl.led_ctl = SND_CMX_AF_LED_BRIGHT_WHITE;
                  break;
                default:
                  return(ret_val);
              }
              led_ctl.led_enable = TRUE;
            } else {
              led_ctl.led_enable = FALSE;
            }
            audfmt_buffer_cb(midi_ctl, SND_CMX_AF_LED_CONTROL, 0, &led_ctl);
          }
          break;

        case CMID_VIB_CONTROL:
          if(midi_ctl->vib_ctl != NULL) {
            if(CMID_VIB_ENABLE(data)) {
              midi_ctl->midi_status2      &= ~AUDFMT_MASK_VIB_DIS_PENDING;
              midi_ctl->midi_status2      |= AUDFMT_MASK_VIB_EN_PENDING;
              midi_ctl->vib_ctl->vib_pend  = CMID_VIB_CTL(data);
            } else {
              midi_ctl->midi_status2      &= ~AUDFMT_MASK_VIB_EN_PENDING;
              midi_ctl->midi_status2      |= AUDFMT_MASK_VIB_DIS_PENDING;
            }
          }
          break;

        case CMID_ANIM_CONTROL:
          if(midi_ctl->cb_store != NULL) {
            if(data == CMID_ANIM_ENABLE) {
              af_cmd = SND_CMX_AF_ANIM_ENABLE;
            } else if(data == CMID_ANIM_DISABLE) {
              af_cmd = SND_CMX_AF_ANIM_DISABLE;
            } else if(data == CMID_ANIM_CLEAR) {
              af_cmd = SND_CMX_AF_ANIM_CLEAR;
            }

            if(af_cmd != SND_CMX_AF_INVALID) {
              audfmt_buffer_cb(midi_ctl, af_cmd, 0, NULL);
            }
          }
          break;

        default:
          break;
      }
    }
  }

  if((ret_val != AUDFMT_STATUS_SUCCESS) && (ret_val != AUDFMT_STATUS_ERROR) &&
     (ret_val != AUDFMT_STATUS_DONE) && (ret_val != AUDFMT_STATUS_JUMP) &&
     (ret_val != AUDFMT_STATUS_STOP)) {
    /* If this command parse was not successful, restore the track to
    ** original condition.
    */
    if(audfmt_restore_track(track, bytes_read) != AUDFMT_STATUS_SUCCESS) {
      ret_val = AUDFMT_STATUS_ERROR;
    }
  }

  return(ret_val);
} /* end do_cmid_statusB */

/* <EJECT> */
/*===========================================================================

FUNCTION do_cmid_note

DESCRIPTION
  This function parses one compact midi note command and enters it 
  into cmd_buf.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type do_cmid_note (
  uint8               command,
  audfmt_parse_type   *midi_ctl,
  uint8               track_num
) {
  audfmt_status_type      ret_val;
  audfmt_track_type       *track;
  audfmt_bank_status_type bank_status;
  uint32                  bytes_read = 1;
  uint8                   buf[AUDFMT_COMMAND_SIZE_4], index, chan, note, gate;
  uint8                   vel = CMID_DEFAULT_VELOCITY, device_mask = 0;

#ifdef FEATURE_DLS
  audfmt_inst_def_type *inst_def;
  uint32               counter;
#endif /* FEATURE_DLS */

  chan  = CMID_CHANNEL_INDEX(command);
  index = CMID_GET_CHANNEL_INDEX(chan, track_num, midi_ctl->num_tracks);
  chan  = CMID_CHANNEL(chan, track_num, midi_ctl->num_tracks,
                       midi_ctl->channels);
  note  = CMID_NOTE(command);

  track   = &midi_ctl->tracks[track_num];
  ret_val = audfmt_get_next_track_byte(track, &gate, &bytes_read);
  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    if(midi_ctl->note_size != 0) {
      ret_val = audfmt_get_next_track_byte(track, &vel, &bytes_read);
      if(ret_val == AUDFMT_STATUS_SUCCESS) {
        switch(CMID_OCTAVE_SHIFT(vel)) {
          case CMID_OCTAVE_MINUS1:
            note -= CMID_ONE_OCTAVE;
            break;
  
          case CMID_OCTAVE_MINUS2:
            note -= CMID_TWO_OCTAVE;
            break;

          case CMID_OCTAVE_PLUS1:
            note += CMID_ONE_OCTAVE;
            break;

          default:
            break;
        }
        vel = CMID_VELOCITY(vel);
      }
    }
 
    if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                  AUDFMT_MASK_FIND_CUEP_END |
                                  AUDFMT_MASK_FIND_POLY_CMD))) {
      if(ret_val == AUDFMT_STATUS_SUCCESS) {
        bank_status = get_cmid_bank_status(midi_ctl, chan);
        /* Drum and SFX banks are treated as drum for device control. */
        if((bank_status == AUDFMT_BANK_DRUM) ||
           (bank_status == AUDFMT_BANK_SFX)) {
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
#ifdef FEATURE_DLS
          /* Check if DLS drum */
          inst_def = audfmt_dls_table.drum_def;
          for(counter = 0; counter < AUDFMT_NUM_INST_DEF; counter++) {
            if(((inst_def->in_use_flag ==
                 audfmt_prog_def_in_use_seq[midi_ctl->sequence]) ||
                (inst_def->in_use_flag == AUDFMT_PROG_DEF_IN_USE_ALL)) &&
               ((inst_def->bank_msb == midi_ctl->bank_msb[chan]) ||
                (inst_def->bank_msb == 0)) &&
               (inst_def->drum_note == note)) {
              break;
            }
            inst_def++;
          }

          if(counter < AUDFMT_NUM_INST_DEF) {
            note = counter;
            if(midi_ctl->bank_status[chan] != AUDFMT_BANK_DRUM_DLS) {
              buf[0] = MIDI_CONTROL_CHANGE;
              buf[1] = chan;
              buf[2] = MIDI_BANK_SEL_COMMAND;
              buf[3] = 0x72;

              ret_val = audfmt_add_command(midi_ctl, buf,
                                           AUDFMT_COMMAND_SIZE_4);
              if(ret_val == AUDFMT_STATUS_SUCCESS) {
                midi_ctl->bank_status[chan] = AUDFMT_BANK_DRUM_DLS;
              }
            }
          } else
#endif /* FEATURE_DLS */
          {
            check_cmid_drum_bank_status(midi_ctl, chan);
          }
        } else {
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
        }

        buf[0] = MIDI_NOTE_ON;
        buf[1] = chan;
        buf[2] = note;
        buf[3] = vel;

        if(midi_ctl->channel_active[chan] &&
           midi_ctl->channel_index_active[index]) {
          chan |= MIDI_NOTE_ON;

          if(cmid_note_off_check_overlap(midi_ctl, gate, chan, note) !=
             AUDFMT_STATUS_SUCCESS) {
            ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
            if(ret_val == AUDFMT_STATUS_SUCCESS ) {
              if(cmid_note_off_add(midi_ctl, gate, chan, note, device_mask) !=
                 AUDFMT_STATUS_SUCCESS) {
                /* Could not add note off command; we're better off not issuing
                ** note on.
                */
                midi_ctl->cmd_buf.num_words    -= AUDFMT_COMMAND_SIZE_4;
                midi_ctl->cmd_buf.num_commands--;
              } else {
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
              }
            }
          }
        }
      }
    }

    if(ret_val == AUDFMT_STATUS_SUCCESS) {
      if(midi_ctl->note_size > 1) {
        return(audfmt_track_skip_ahead(midi_ctl, track,
                                       midi_ctl->note_size - 1));
      }
    }
  }

  if((ret_val != AUDFMT_STATUS_SUCCESS) && (ret_val != AUDFMT_STATUS_ERROR)) {
    /* If this command parse was not successful, restore the track to
    ** original condition.
    */
    if(audfmt_restore_track(track, bytes_read) != AUDFMT_STATUS_SUCCESS) {
      ret_val = AUDFMT_STATUS_ERROR;
    }
  }

  return (ret_val);
} /* end do_cmid_note */

/* <EJECT> */
/*===========================================================================

FUNCTION parse_cmid_cmd

DESCRIPTION
  This function parses one compact midi event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type parse_cmid_cmd (
  audfmt_parse_type    *midi_ctl,
  uint8                track_num
) {
  audfmt_status_type ret_val;
  audfmt_track_type  *track;
  uint8              tbyte1, command;

  track   = &midi_ctl->tracks[track_num];
  ret_val = audfmt_get_next_track_byte(track, &tbyte1, NULL);

  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    if(CMID_EXTENSION_BYTE(tbyte1)) {
      ret_val = audfmt_get_next_track_byte(track, &command, NULL);
      if(ret_val == AUDFMT_STATUS_SUCCESS) {
        if(CMID_STATUS_A(command)) {
          if((midi_ctl->version <= 300) ||
             (midi_ctl->type == AUDFMT_MIDI_MELO_TYPE)) {
            MSG_MED("Status A: %d", command, 0, 0);
            ret_val = audfmt_track_skip_ahead(midi_ctl, track,
                                              midi_ctl->exsa_size);
          } else {
            switch(tbyte1) {
              case CMID_EXT_CLASS_A:
                ret_val = audfmt_track_skip_ahead(midi_ctl, track,
                                                  midi_ctl->exsa_size);
                break;

              case CMID_EXT_CLASS_B:
                ret_val = audfmt_track_skip_ahead(midi_ctl, track,
                                                  midi_ctl->exsb_size);
                break;

              case CMID_EXT_CLASS_C:
                ret_val = audfmt_track_skip_ahead(midi_ctl, track,
                                                  midi_ctl->exsc_size);
                break;

              case CMID_EXT_NORMAL:
                if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                              AUDFMT_MASK_FIND_CUEP_END |
                                              AUDFMT_MASK_FIND_POLY_CMD)) &&
                   (midi_ctl->exsn_size == 1)) {
                  ret_val = do_cmid_pitch_bend(command, midi_ctl, track_num);
                } else {
                  ret_val = audfmt_track_skip_ahead(midi_ctl, track,
                                                    midi_ctl->exsn_size);
                }
                break;
            }
          }
        } else if(CMID_STATUS_B(command)) {
          MSG_MED("Status B: %d", command, 0, 0);
          ret_val = do_cmid_statusB(command, midi_ctl, track_num);
        } else {
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
          {
            ret_val = do_cmid_extension(command, midi_ctl, track_num);
          }
        }
      } else if(ret_val != AUDFMT_STATUS_ERROR) {
        if(audfmt_restore_track(track, 1) != AUDFMT_STATUS_SUCCESS) {
          ret_val = AUDFMT_STATUS_ERROR;
        }
      }
    } else {
      ret_val = do_cmid_note(tbyte1, midi_ctl, track_num);
    }

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

  }

  return (ret_val);
} /* end parse_cmid_cmd */

/* <EJECT> */
#ifdef FEATURE_SMAF
/*===========================================================================

FUNCTION smaf_get_pitch_offset

DESCRIPTION
  This function calculates the note offset and pitch values based
  on Cx and Bx values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_get_pitch_offset (
  uint8  c_value,
  uint8  b_value,
  uint8  *note,
  uint16 *pitch
) {
  audfmt_status_type ret_val = AUDFMT_STATUS_FAILURE;
  uint32 diff1, diff2;
  uint8  *lo = NULL, *hi = NULL, index = 0;

  if(c_value & 0x1) {
    if(b_value >= AUDFMT_SMAF_PITCH_LO_B_OFFSET) {
      b_value -= AUDFMT_SMAF_PITCH_LO_B_OFFSET;
      lo       = (uint8 *) &audfmt_smaf_pitch_lo_b_values[0];
      hi       = (uint8 *) &audfmt_smaf_pitch_lo_b_values[1];
      ret_val  = AUDFMT_STATUS_SUCCESS;
    }
  } else {
    if(b_value <= AUDFMT_SMAF_PITCH_HI_B_MAX) {
      b_value += AUDFMT_SMAF_PITCH_HI_B_OFFSET;
      lo       = (uint8 *) &audfmt_smaf_pitch_hi_b_values[0];
      hi       = (uint8 *) &audfmt_smaf_pitch_hi_b_values[1];
      ret_val  = AUDFMT_STATUS_SUCCESS;
    }
  }

  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    while(*hi < b_value) {
      lo++;
      hi++;
      index++;
    }

    if(index == 0) {
      diff1 = (*hi - b_value) * 0x1000;
      diff1 = diff1 / *hi;

      *pitch = 0x2000 - diff1;
      index  = 0;
    } else if(index == 6) {
      diff1 = (b_value - *lo) * 0x1000;
      diff1 = diff1 / (*hi - *lo);

      *pitch = 0x2000 + diff1;
      index  = 5;
    } else {
      diff1 = *hi - b_value;
      diff2 = b_value - *lo;

      if(diff1 < diff2) {
        diff1 = diff1 * 0x1000;
        diff1 = diff1 / (*hi - *lo);

        *pitch = 0x2000 - diff1;
      } else {
        diff2 = diff2 * 0x1000;
        diff2 = diff2 / (*hi - *lo);

        *pitch = 0x2000 + diff2;
        index--;
      }
    }
    *note += index;
  }

  return(ret_val);
} /* end smaf_get_pitch_offset */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_get_note_and_pitch

DESCRIPTION
  This function calculates the note and pitch values based on Cx and Bx values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_get_note_and_pitch (
  uint8  c_value,
  uint8  b_value,
  uint8  *note,
  uint16 *pitch
) {
  audfmt_status_type ret_val = AUDFMT_STATUS_FAILURE;

  c_value &= ~0x20;
  if(c_value <= 0x1f) {
    if(c_value == 0) {
      *note = 1;
    } else if(c_value == 0x1f) {
      *note = 109;
    } else {
      *note = ((c_value / 4) * 12) + 13;
    }
    if(!(c_value & 0x1)) {
      *note += 6;
    }
    ret_val = smaf_get_pitch_offset(c_value, b_value, note, pitch);
  }

  return(ret_val);
} /* end smaf_get_note_and_pitch */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_smaf_pitchrange

DESCRIPTION
  This function adds RPN pitch range commands to the command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_set_smaf_pitchrange (
  audfmt_parse_type *midi_ctl,
  uint8             channel,
  uint8             range
) {
  audfmt_status_type ret_val;
  uint8              buf[AUDFMT_COMMAND_SIZE_4];

  buf[0]  = MIDI_CONTROL_CHANGE;
  buf[1]  = channel;
  buf[2]  = MIDI_RP_COARSE_COMMAND;
  buf[3]  = MIDI_RP_PITCH_PARM1;
  ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    buf[2]  = MIDI_RP_FINE_COMMAND;
    buf[3]  = MIDI_RP_PITCH_PARM2;
    ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

    if(ret_val == AUDFMT_STATUS_SUCCESS) {
      buf[2]  = MIDI_RP_PITCH_PARM3;
      buf[3]  = range;
      ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
    }
  }

  return(ret_val);
} /* end audfmt_set_smaf_pitchrange */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_smaf_pitch

DESCRIPTION
  This function adds a pitch bend command to the command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_set_smaf_pitch (
  audfmt_parse_type *midi_ctl,
  uint8             channel,
  uint16            pitch
) {
  audfmt_status_type ret_val;
  uint8              buf[AUDFMT_COMMAND_SIZE_4];

  if(pitch == 0x4000) {
    pitch--;
  }

  buf[0]  = MIDI_PITCH_WHEEL;
  buf[1]  = channel;
  buf[2]  = pitch & 0x7f;
  buf[3]  = (pitch >> 7) & 0x7f;
  ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

  return (ret_val);
} /* end audfmt_set_smaf_pitch */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_smaf_get_pitch

DESCRIPTION
  This function issues commands to achieve the indicated pitch in reference
  to the current settings on the selected channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_smaf_get_pitch (
  audfmt_parse_type *midi_ctl,
  uint8             channel,
  uint8             note,
  uint16            pitch
) {
  audfmt_smaf_ma2_pitch_type *pitch_cmd;
  audfmt_status_type ret_val = AUDFMT_STATUS_SUCCESS;
  uint32 tlong;
  uint8  range;

  pitch_cmd = &midi_ctl->smaf_ma2_pitch[channel];
  if(note < pitch_cmd->note) {
    note = pitch_cmd->note - note;
    if(pitch < 0x2000) {
      range = note + 1;
    } else {
      range = note;
    }
    if(range < 2) {
      range = 2;
    }
    if(range != pitch_cmd->range) {
      ret_val = audfmt_set_smaf_pitchrange(midi_ctl, channel, range);
    }
    if(ret_val == AUDFMT_STATUS_SUCCESS) {
      pitch_cmd->range = range;
      tlong = 0x2000 - ((0x2000 * note) / range);
    }
  } else {
    note = note - pitch_cmd->note;
    if(pitch > 0x2000) {
      range = note + 1;
    } else {
      range = note;
    }
    if(range < 2) {
      range = 2;
    }
    if(range != pitch_cmd->range) {
      ret_val = audfmt_set_smaf_pitchrange(midi_ctl, channel, range);
    }
    if(ret_val == AUDFMT_STATUS_SUCCESS) {
      pitch_cmd->range = range;
      tlong = 0x2000 + ((0x2000 * note) / range);
    }
  }

  if(ret_val == AUDFMT_STATUS_SUCCESS) {
    if(pitch < 0x2000) {
      pitch = 0x2000 - pitch;
      if(pitch_cmd->range != 2) {
        pitch = (pitch * 2) / pitch_cmd->range;
      }
      pitch = tlong - pitch;
    } else {
      pitch = pitch - 0x2000;
      if(pitch_cmd->range != 2) {
        pitch = (pitch * 2) / pitch_cmd->range;
      }
      pitch = tlong + pitch;
    }
    ret_val = audfmt_set_smaf_pitch(midi_ctl, channel, pitch);
  }

  return(ret_val);
} /* end audfmt_smaf_get_pitch */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_ma2_pitch_cmd

DESCRIPTION
  This function processes a MA2 SysEx pitch bend command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_ma2_pitch_cmd (
  audfmt_parse_type *midi_ctl,
  uint8             channel
) {
  audfmt_smaf_ma2_pitch_type *pitch_cmd;
  audfmt_status_type         ret_val = AUDFMT_STATUS_SUCCESS;
  uint8                      note, buf[AUDFMT_COMMAND_SIZE_4];
  uint16                     pitch;

  pitch_cmd = &midi_ctl->smaf_ma2_pitch[channel];
  if((pitch_cmd->c_value != 0x80) && (pitch_cmd->b_value != 0x100)) {
    if(pitch_cmd->c_value & 0x20) {
      if(smaf_get_note_and_pitch(pitch_cmd->c_value, pitch_cmd->b_value,
                                 &note, &pitch) == AUDFMT_STATUS_SUCCESS) {
        if(pitch_cmd->note == 0x80) {
          buf[0] = MIDI_NOTE_ON;
          buf[1] = channel;
          buf[2] = note;
          buf[3] = SMAF_HP_NOTE_DEFAULT_VEL;

          ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

          if(ret_val == AUDFMT_STATUS_SUCCESS) {
            pitch_cmd->note  = note;
            pitch_cmd->range = 2;
            ret_val = audfmt_set_smaf_pitch(midi_ctl, channel, pitch);
          }
        } else {
          ret_val = audfmt_smaf_get_pitch(midi_ctl, channel, note, pitch);
        }
      }
    } else {
      buf[0] = MIDI_NOTE_OFF;
      buf[1] = channel;
      buf[2] = pitch_cmd->note;
      buf[3] = 0;

      ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
      if(ret_val == AUDFMT_STATUS_SUCCESS) {
        ret_val = audfmt_set_smaf_pitch(midi_ctl, channel, 0x2000);
        if(ret_val == AUDFMT_STATUS_SUCCESS) {
          ret_val = audfmt_set_smaf_pitchrange(midi_ctl, channel, 2);
          if(ret_val == AUDFMT_STATUS_SUCCESS) {
            pitch_cmd->note    = 0x80;
            pitch_cmd->c_value = 0x80;
            pitch_cmd->b_value = 0x100;
          }
        }
      }
    }
  }
  return(ret_val);
} /* end smaf_ma2_pitch_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION parse_smaf_hp_cmd

DESCRIPTION
  This function parses one smaf handyphone command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type parse_smaf_hp_cmd (
  audfmt_parse_type    *midi_ctl,
  uint8                track_num
) {
  audfmt_status_type         ret_val = AUDFMT_STATUS_SUCCESS;
  audfmt_track_type          *track;
  audfadec_wa_info_type      wa_info;
  audfmt_smaf_dls_entry_type *fm_dls;

  uint8  status, tbyte1, tbyte2, channel, buf[6];
  uint32 length, counter, adec_chan, bytes_read = 0;

  track   = &midi_ctl->tracks[track_num];
  ret_val = audfmt_get_next_track_byte(track, &status, &bytes_read);
  if(ret_val != AUDFMT_STATUS_SUCCESS) return(ret_val);

  switch(status) {
    case SMAF_HP_EXCLUSIVE_MESSAGE:
      ret_val = audfmt_get_next_track_byte(track, &tbyte1, &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;
      if(tbyte1 == SMAF_HP_EXCLUSIVE_CMD) {
        /* Get size of exclusive message */
        ret_val = audfmt_get_next_track_byte(track, &tbyte1, &bytes_read);
        if(ret_val != AUDFMT_STATUS_SUCCESS) break;

        if(((midi_ctl->smaf_type == AUDFMT_SMAF_PHRASE_TYPE) ||
            (midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_MA2)) &&
           (tbyte1 == 0x6)) {
          for(tbyte1 = 0; tbyte1 < 6; tbyte1++) {
            ret_val = audfmt_get_next_track_byte(track, &(buf[tbyte1]),
                                                 &bytes_read);
            if(ret_val != AUDFMT_STATUS_SUCCESS) break;
          }
          if(ret_val == AUDFMT_STATUS_SUCCESS) {
            if((buf[0] == 0x43) && (buf[1] == 0x03) && (buf[2] == 0x90)) {
              tbyte1 = buf[3] & 0xf;
              if((buf[3] & 0xf0) == 0xb0) {
                if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
                  midi_ctl->smaf_ma2_pitch[tbyte1].b_value = buf[4];
                  ret_val = smaf_ma2_pitch_cmd(midi_ctl, tbyte1);
                }
              } else if((buf[3] & 0xf0) == 0xc0) {
                if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
                  midi_ctl->smaf_ma2_pitch[tbyte1].c_value = buf[4];
                  ret_val = smaf_ma2_pitch_cmd(midi_ctl, tbyte1);
                }
                if((ret_val == AUDFMT_STATUS_SUCCESS) && (buf[4] & 0x20)) {
                  midi_ctl->midi_status &= ~AUDFMT_MASK_SILENCE_SKIP;
                }
              }
            }
          }
        } else {
          return(audfmt_track_skip_ahead(midi_ctl, track, tbyte1));
        }
      } else if(tbyte1 == SMAF_HP_NOP_CMD) {
        if(track->current_location == track->track_end) {
          ret_val = AUDFMT_STATUS_DONE;
        }
      }else if((0x10<=tbyte1)&&(tbyte1<=0x1F)&&
	           (!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME))) {
	       /*
     	         Added to skip phrase file with user events.
     	         CR 162680
	       */  
           ret_val = AUDFMT_STATUS_SUCCESS;
       }else {
        ret_val = AUDFMT_STATUS_DATA_ERR;
      }
      break;

    case SMAF_HP_CONTROL_MESSAGE:
      ret_val = audfmt_get_next_track_byte(track, &tbyte1, &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;
      /* Verify end of sequence command (i.e) at this point it is 2 consequent
       0x00 (or) current location has exceeded track end*/
      if((tbyte1 == SMAF_HP_TRACK_END_CMD) ||
         (track->current_location >= track->track_end)) 
      {
        /* Verify end of sequence command i.e 0x00 (or) current location 
           has exceeded track end*/
        if(track->current_location < track->track_end)
        {
          /*Verify end of sequence command (i.e) at this point it is a 0x00 */
          ret_val = audfmt_get_next_track_byte(track, &tbyte1, &bytes_read);
          if(ret_val != AUDFMT_STATUS_SUCCESS) break;
        }
        if((tbyte1 == SMAF_HP_TRACK_END_CMD) || 
           (track->current_location >= track->track_end)) {
          if(midi_ctl->current_header_track == 1) {
            track->delta_tick = cmid_note_off_tick_remain(midi_ctl);
            if(track->delta_tick != 0) {
              ret_val = AUDFMT_STATUS_PENDING;
            } else {
              ret_val = AUDFMT_STATUS_DONE;
            }
          } else {
            ret_val = AUDFMT_STATUS_DONE;
          }
        } else {
          ret_val = AUDFMT_STATUS_DATA_ERR;
        }
      } else {
        /* Get channel number for this command */
        channel = SMAF_HP_CHANNEL_NUM(tbyte1, track_num);
        if(SMAF_HP_NORMAL_CMD(tbyte1)) {
          /* Get value for this command */
          ret_val = audfmt_get_next_track_byte(track, &tbyte2, &bytes_read);
          if(ret_val != AUDFMT_STATUS_SUCCESS) break;

          if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
            switch(SMAF_HP_NORMAL_CMD_TYPE(tbyte1)) {
              case SMAF_HP_PROGRAM_CHANGE_CMD:
                if(midi_ctl->smaf_type == AUDFMT_SMAF_PHRASE_TYPE) {
                  switch(midi_ctl->smaf_phrase_type) {
                    case AUDFMT_SMAF_PHRASE_L1:
                      tbyte2 = tbyte2 & 0x3;
                      midi_ctl->smaf_program_change[channel] = tbyte2;

                      buf[0]  = MIDI_CONTROL_CHANGE;
                      buf[1]  = channel;
                      buf[2]  = MIDI_BANK_SEL_COMMAND;
                      if(tbyte2 < midi_ctl->smaf_fm_dls_index) {
                        fm_dls = &midi_ctl->smaf_fm_dls[tbyte2];

                        if(fm_dls->bank_msb == 0x80) {
                          if(midi_ctl->midi_status2 &
                                            AUDFMT_MASK_SMAF_DEFAULT_HQ) {
                            buf[3]  = CMID_BANK_DRUM_HQ;
                          } else {
                            buf[3]  = CMID_BANK_DRUM;
                          }
                        } else {
                          if(midi_ctl->midi_status2 &
                                            AUDFMT_MASK_SMAF_DEFAULT_HQ) {
                            buf[3]  = CMID_BANK_NORMAL_HQ;
                          } else {
                            buf[3]  = CMID_BANK_NORMAL;
                          }
                        }
                        ret_val = audfmt_add_command(midi_ctl, buf,
                                                     AUDFMT_COMMAND_SIZE_4);
                        buf[2]  = fm_dls->instrument;
                      } else {
                        if(midi_ctl->midi_status2 &
                                          AUDFMT_MASK_SMAF_DEFAULT_HQ) {
                          buf[3]  = CMID_BANK_NORMAL_HQ;
                        } else {
                          buf[3]  = CMID_BANK_NORMAL;
                        }
                        ret_val = audfmt_add_command(midi_ctl, buf,
                                                     AUDFMT_COMMAND_SIZE_4);

                        buf[2] = tbyte2;
                      }
                      break;

                    case AUDFMT_SMAF_PHRASE_L2:
                      buf[2] = tbyte2;
                      fm_dls = midi_ctl->smaf_fm_dls;
                      for(counter = 0; counter < midi_ctl->smaf_fm_dls_index;
                          counter++) {
                        if((fm_dls->bank_msb == midi_ctl->bank_msb[channel]) &&
                           (fm_dls->program == tbyte2)) {
                          buf[0]  = MIDI_CONTROL_CHANGE;
                          buf[1]  = channel;
                          buf[2]  = MIDI_BANK_SEL_COMMAND;
                          if(fm_dls->bank_lsb == 0x80) {
                            buf[3]  = CMID_BANK_NORMAL_DLS;
                          } else {
                            if(fm_dls->bank_msb >= 0x80) {
                              if(midi_ctl->midi_status2 &
                                                AUDFMT_MASK_SMAF_DEFAULT_HQ) {
                                buf[3]  = CMID_BANK_DRUM_HQ;
                              } else {
                                buf[3]  = CMID_BANK_DRUM;
                              }
                            } else {
                              if(midi_ctl->midi_status2 &
                                                AUDFMT_MASK_SMAF_DEFAULT_HQ) {
                                buf[3]  = CMID_BANK_NORMAL_HQ;
                              } else {
                                buf[3]  = CMID_BANK_NORMAL;
                              }
                            }
                          }
                          ret_val = audfmt_add_command(midi_ctl, buf,
                                                       AUDFMT_COMMAND_SIZE_4);
                          buf[2] = fm_dls->instrument;
                          break;
                        }
                        fm_dls++;
                      }
                      break;

                    default:
                      buf[2] = tbyte2;
                      break;
                  }
                } else {
                  if(midi_ctl->bank_status[channel] == AUDFMT_BANK_DRUM) {
                    midi_ctl->smaf_program_change[channel] = tbyte2;
                  }

                  fm_dls = midi_ctl->smaf_fm_dls;
                  for(counter = 0; counter < midi_ctl->smaf_fm_dls_index;
                      counter++) {
                    if(fm_dls->program == tbyte2) {
                      tbyte2 = fm_dls->instrument;
                      break;
                    }
                    fm_dls++;
                  }

                  buf[2]  = tbyte2;
                }

                if(ret_val == AUDFMT_STATUS_SUCCESS) {
                  buf[0]  = MIDI_PROGRAM_CHANGE;
                  buf[1]  = channel;
                  if(!(track->track_status & AUDFMT_TRACK_MASK_PCM_TRACK)) {
                    ret_val = audfmt_add_command(midi_ctl, buf,
                                                 AUDFMT_COMMAND_SIZE_3);
                  }
                }
                break;

              case SMAF_HP_BANK_SELECT_CMD:
                if((midi_ctl->smaf_type != AUDFMT_SMAF_PHRASE_TYPE) ||
                   (midi_ctl->smaf_phrase_type != AUDFMT_SMAF_PHRASE_L1)) {
                  midi_ctl->bank_msb[channel] = tbyte2;

                  buf[0] = MIDI_CONTROL_CHANGE;
                  buf[1] = channel;
                  buf[2] = MIDI_BANK_SEL_COMMAND;

                  if(tbyte2 == 0) {
                    buf[3] = 0;
                    if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_MA2) {
                      /* For MA2, channel 9 is not defaulted to drum bank */
                      midi_ctl->bank_status[channel] = AUDFMT_BANK_NORMAL;
                      buf[3] = CMID_BANK_NORMAL;
                    } else if(channel == 9) {
                      midi_ctl->bank_status[channel] = AUDFMT_BANK_DRUM;
                      if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_DEFAULT_HQ) {
                        buf[3]  = CMID_BANK_DRUM_HQ;
                      }
                    } else {
                      midi_ctl->bank_status[channel] = AUDFMT_BANK_NORMAL;
                      if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_DEFAULT_HQ) {
                        buf[3]  = CMID_BANK_NORMAL_HQ;
                      }
                    }
                  } else if(tbyte2 < 0x80) {
                    if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_DEFAULT_HQ) {
                      buf[3]  = CMID_BANK_NORMAL_HQ;
                    } else {
                      buf[3]  = CMID_BANK_NORMAL;
                    }
                    midi_ctl->bank_status[channel] = AUDFMT_BANK_NORMAL;
                  } else {
                    if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_DEFAULT_HQ) {
                      buf[3]  = CMID_BANK_DRUM_HQ;
                    } else {
                      buf[3]  = CMID_BANK_DRUM;
                    }
                    midi_ctl->bank_status[channel] = AUDFMT_BANK_DRUM;
                  }

                  if(!(track->track_status & AUDFMT_TRACK_MASK_PCM_TRACK)) {
                    ret_val = audfmt_add_command(midi_ctl, buf,
                                                 AUDFMT_COMMAND_SIZE_4);
                  }
                }
                break;

              case SMAF_HP_OCTAVE_SHIFT_CMD:
                if(!(track->track_status & AUDFMT_TRACK_MASK_PCM_TRACK)) {
                  if(tbyte2 <= 4) {
                    midi_ctl->smaf_octave_shift[channel] = tbyte2;
                  } else {
                    switch(tbyte2) {
                      case 0x81:
                        midi_ctl->smaf_octave_shift[channel] = -1;
                        break;

                      case 0x82:
                        midi_ctl->smaf_octave_shift[channel] = -2;
                        break;

                      case 0x83:
                        midi_ctl->smaf_octave_shift[channel] = -3;
                        break;

                      case 0x84:
                        midi_ctl->smaf_octave_shift[channel] = -4;
                        break;

                      default:
                        break;
                    }
                  }
                }
                break;

              case SMAF_HP_MODULATION_CMD:
                buf[0]  = MIDI_CONTROL_CHANGE;
                buf[1]  = channel;
                buf[2]  = MIDI_MODULATION_COMMAND;
                buf[3]  = tbyte2;
                if(!(track->track_status & AUDFMT_TRACK_MASK_PCM_TRACK)) {
                  ret_val = audfmt_add_command(midi_ctl, buf,
                                               AUDFMT_COMMAND_SIZE_4);
                }
                break;

              case SMAF_HP_PITCH_BEND_CMD:
                buf[0]  = MIDI_PITCH_WHEEL;
                buf[1]  = channel;
                buf[2]  = 0;
                buf[3]  = tbyte2;
                if(!(track->track_status & AUDFMT_TRACK_MASK_PCM_TRACK)) {
                  ret_val = audfmt_add_command(midi_ctl, buf,
                                               AUDFMT_COMMAND_SIZE_4);
                }
                break;

              case SMAF_HP_VOLUME_CMD:
                buf[0]  = MIDI_CONTROL_CHANGE;
                buf[1]  = channel;
                buf[2]  = MIDI_VOLUME_COMMAND;
                buf[3]  = tbyte2;
                if(!(track->track_status & AUDFMT_TRACK_MASK_PCM_TRACK)) {
                  ret_val = audfmt_add_command(midi_ctl, buf,
                                               AUDFMT_COMMAND_SIZE_4);
                }
                break;

              case SMAF_HP_PAN_CMD:
                buf[0]  = MIDI_CONTROL_CHANGE;
                buf[1]  = channel;
                buf[2]  = MIDI_PANPOT_COMMAND;
                buf[3]  = tbyte2;
                if(!(track->track_status & AUDFMT_TRACK_MASK_PCM_TRACK)) {
                  ret_val = audfmt_add_command(midi_ctl, buf,
                                               AUDFMT_COMMAND_SIZE_4);
                }
                break;

              case SMAF_HP_EXPRESSION_CMD:
                buf[0]  = MIDI_CONTROL_CHANGE;
                buf[1]  = channel;
                buf[2]  = MIDI_EXPRESSION_COMMAND;
                buf[3]  = tbyte2;
                if(!(track->track_status & AUDFMT_TRACK_MASK_PCM_TRACK)) {
                  ret_val = audfmt_add_command(midi_ctl, buf,
                                               AUDFMT_COMMAND_SIZE_4);
                }
                break;

              case SMAF_HP_VELOCITY_CMD:
                midi_ctl->smaf_velocity[channel] = MIN(tbyte2, 0x7f);
                break;

              default:
                break;
            }
          }
        } else if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
          /* Get value for short message type */
          tbyte2  = SMAF_HP_SHORT_CMD_VALUE(tbyte1);
          switch(SMAF_HP_SHORT_CMD_TYPE(tbyte1)) {
            case SMAF_HP_SHORT_EXPRESSION_CMD:
              buf[0]  = MIDI_CONTROL_CHANGE;
              buf[1]  = channel;
              buf[2]  = MIDI_EXPRESSION_COMMAND;
              if(tbyte2 == 1) {
                buf[3]  = 0;
              } else {
                buf[3]  = ((tbyte2 - 2) * 8) + 0x1f;
              }
              if(!(track->track_status & AUDFMT_TRACK_MASK_PCM_TRACK)) {
                ret_val = audfmt_add_command(midi_ctl, buf,
                                             AUDFMT_COMMAND_SIZE_4);
              }
              break;

            case SMAF_HP_SHORT_PITCH_BEND_CMD:
              buf[0]  = MIDI_PITCH_WHEEL;
              buf[1]  = channel;
              buf[2]  = 0;
              buf[3]  = tbyte2 << 3;
              if(!(track->track_status & AUDFMT_TRACK_MASK_PCM_TRACK)) {
                ret_val = audfmt_add_command(midi_ctl, buf,
                                             AUDFMT_COMMAND_SIZE_4);
              }
              break;

	    case SMAF_HP_SHORT_MODULATION_CMD:
              if((tbyte2 >= 1) && (tbyte2 <= 14)) {
              buf[0]  = MIDI_CONTROL_CHANGE;
              buf[1]  = channel;
              buf[2]  = MIDI_MODULATION_COMMAND;
              buf[3]  = smaf_short_mod_values[tbyte2 - 1];
              if(!(track->track_status & AUDFMT_TRACK_MASK_PCM_TRACK)) {
                ret_val = audfmt_add_command(midi_ctl, buf,
                                             AUDFMT_COMMAND_SIZE_4);
	        }
              }
              break;

            default:
              break;
          }
        }
      }
      break;

    default:
      /* Note message */
      /* Get gate time for note message */
      ret_val = audfmt_get_next_track_byte(track, &tbyte1, &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;
      if(tbyte1 & 0x80) {
        length  = ((tbyte1 & 0x7f) << 7);
        ret_val = audfmt_get_next_track_byte(track, &tbyte1, &bytes_read);
        if(ret_val != AUDFMT_STATUS_SUCCESS) break;
        length = (length | (tbyte1 & 0x7f)) + 128;
      } else {
        length = tbyte1;
      }

      if(!(midi_ctl->midi_status & AUDFMT_MASK_INITIAL_SKIP) &&
         !(track->track_status & AUDFMT_TRACK_MASK_OFFSET_SKIP)) {
        channel = SMAF_HP_NOTE_CHANNEL(status);
        if(!(track->track_status & AUDFMT_TRACK_MASK_PCM_TRACK)) {
          channel += (track_num * 4);
          tbyte1   = SMAF_HP_NOTE_OCTAVE(status);
          tbyte2   = SMAF_HP_NOTE_NUMBER(status);

          buf[0] = MIDI_NOTE_ON;
          buf[1] = channel;
          if(midi_ctl->smaf_type == AUDFMT_SMAF_PHRASE_TYPE) {
            switch(midi_ctl->smaf_phrase_type) {
              case AUDFMT_SMAF_PHRASE_L1:
                if(midi_ctl->smaf_program_change[channel] <
                   midi_ctl->smaf_fm_dls_index) {
                  fm_dls = &midi_ctl->smaf_fm_dls[
                                      midi_ctl->smaf_program_change[channel]];
                  if(fm_dls->bank_msb == 0x80) {
                    buf[2] = fm_dls->instrument;
                  } else {
                    buf[2] = ((tbyte1 + midi_ctl->smaf_octave_shift[channel]) *
                              12) + tbyte2 + SMAF_HP_NOTE_OFFSET;
                  }
                } else {
                  buf[2] = ((tbyte1 + midi_ctl->smaf_octave_shift[channel]) *
                            12) + tbyte2 + SMAF_HP_NOTE_OFFSET;
                }
                break;

              case AUDFMT_SMAF_PHRASE_L2:
                buf[2] = ((tbyte1 + midi_ctl->smaf_octave_shift[channel]) *
                          12) + tbyte2 + SMAF_HP_NOTE_OFFSET;

                if(midi_ctl->bank_msb[channel] >= 0x80) {
                  fm_dls = midi_ctl->smaf_fm_dls;
                  for(counter = 0; counter < midi_ctl->smaf_fm_dls_index;
                      counter++) {
                    if((fm_dls->bank_msb == midi_ctl->bank_msb[channel]) &&
                       ((fm_dls->program + SMAF_HP_NOTE_OFFSET) == buf[2])) {
                      buf[2] = fm_dls->instrument;
                      break;
                    }
                    fm_dls++;
                  }
                  if(counter == midi_ctl->smaf_fm_dls_index) {
                    return(ret_val);
                  }
                }
                break;

              default:
                buf[2] = ((tbyte1 + midi_ctl->smaf_octave_shift[channel]) *
                          12) + tbyte2 + SMAF_HP_NOTE_OFFSET;
                break;
            }
          } else {
            if((midi_ctl->bank_status[channel] == AUDFMT_BANK_DRUM) &&
               (midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_MA2) &&
               (midi_ctl->smaf_program_change[channel] != 0x80)) {
              /* For MA2 files, drums notes are the program number setting */
              buf[2] = midi_ctl->smaf_program_change[channel];
            } else {
              buf[2] = ((tbyte1 + midi_ctl->smaf_octave_shift[channel]) * 12) +
                       tbyte2 + SMAF_HP_NOTE_OFFSET;

              if(midi_ctl->bank_status[channel] == AUDFMT_BANK_DRUM) {
                fm_dls = midi_ctl->smaf_fm_drum;
                for(counter = 0; counter < midi_ctl->smaf_fm_drum_index;
                    counter++) {
                  if(fm_dls->program == buf[2]) {
                    buf[2] = fm_dls->instrument;
                    break;
                  }
                  fm_dls++;
                }
              }
            }
          }
          buf[3] = midi_ctl->smaf_velocity[buf[1]];

          if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
            ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
          }
          if(ret_val == AUDFMT_STATUS_SUCCESS) {
            cmid_note_off_add(midi_ctl, length, buf[1], buf[2], 0);
            if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                          AUDFMT_MASK_SEEK_ACTIVE)) &&
               (midi_ctl->cb_store != NULL)) {
              if((midi_ctl->smaf_vib_ctl & (1<<buf[1])) != 0) {
                if(midi_ctl->smaf_vib_count == 0) {
                  smaf_vib_buffer_cb(midi_ctl, SND_SMAF_VIB_CTL_STATUS_ON, SND_CMX_SMAF_VIB_CONTROL);
                }
                midi_ctl->smaf_vib_count = midi_ctl->smaf_vib_count + 1;
              }
            }
          }
        } else if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                             AUDFMT_MASK_SEEK_ACTIVE))) {

          tbyte2 = SMAF_HP_WAVE_NUMBER(status);

          switch(SMAF_WAVE_FORMAT(midi_ctl->wav_spec[0].format)) {
            case SMAF_HP_WAVE_FORMAT_PCM:
              wa_info.yadpcm_mode = AUDMAIN_ADPCM_SIGNED_PCM_MODE;
              break;

            case SMAF_HP_WAVE_FORMAT_YADPCM:
              wa_info.yadpcm_mode = AUDMAIN_ADPCM_YADPCM_MODE;
              break;

            default:
              return(ret_val);
          }

          switch(SMAF_WAVE_BASE_BIT(midi_ctl->wav_spec[0].format)) {
            case SMAF_WAVE_BASE_BIT_4:
              wa_info.bits_per_sample = 4;
              break;

            case SMAF_WAVE_BASE_BIT_8:
              wa_info.bits_per_sample = 8;
              break;

            case SMAF_WAVE_BASE_BIT_12:
            case SMAF_WAVE_BASE_BIT_16:
            default:
              return(ret_val);
          }

          wa_info.channels       = SMAF_WAVE_CHANNELS(
                                               midi_ctl->wav_spec[0].format);
          wa_info.sample_rate_hz = midi_ctl->wav_spec[0].sampling_rate;

          wa_info.block_start = 0;
          wa_info.location    = midi_ctl->wav_store[tbyte2].location;
          wa_info.length      = midi_ctl->wav_store[tbyte2].num_bytes;
          wa_info.next_block  = 0;
          wa_info.buf1        = NULL;
          wa_info.length_buf1 = 0;
          wa_info.buf2        = NULL;
          wa_info.length_buf2 = 0;
          wa_info.adec_mode   = AUDMAIN_ADEC_MODE_YADPCM;
          wa_info.mfi_flag    = FALSE;

          /* Get ADEC channel */
          for(adec_chan = 0; adec_chan < AUDFMT_NUM_ADEC_CHANNELS; adec_chan++)
          {
            if((midi_ctl->adec_status[adec_chan].enable) &&
               (midi_ctl->adec_status[adec_chan].channel == channel)) {
              break;
            }
          }

          if((adec_chan < AUDFMT_NUM_ADEC_CHANNELS) ||
             (audfmt_get_adec_channel(midi_ctl, &adec_chan) ==
              AUDFMT_STATUS_SUCCESS)) {
            midi_ctl->adec_status[adec_chan].enable  = TRUE;
            midi_ctl->adec_status[adec_chan].reset   = FALSE;
            midi_ctl->adec_status[adec_chan].channel = channel;
            audfadec_wa_out_qcp(midi_ctl->handle, adec_chan, &wa_info,
                                      main_audfmt_parse_ctl, FALSE);

// need to update use of channel 9
            if(length != 0) {
              if(cmid_note_off_check_overlap(midi_ctl, length, 0x9,
                                             AUDFMT_NO_ADEC_CHAN(adec_chan)) !=
                 AUDFMT_STATUS_SUCCESS) {
                cmid_note_off_add(midi_ctl, length, 0x9,
                                  AUDFMT_NO_ADEC_CHAN(adec_chan), 0);
              }
            } else {
              cmid_note_off_remove(midi_ctl, 0x9,
                                   AUDFMT_NO_ADEC_CHAN(adec_chan));
            }
          }
        }
      }

      if(ret_val == AUDFMT_STATUS_SUCCESS) {
        midi_ctl->midi_status &= ~AUDFMT_MASK_SILENCE_SKIP;
      }
      break;
  }

  if((ret_val != AUDFMT_STATUS_SUCCESS) && (ret_val != AUDFMT_STATUS_ERROR) &&
     (ret_val != AUDFMT_STATUS_DONE) && (bytes_read != 0)) {
    /* If this command parse was not successful, restore the track to
    ** original condition.
    */
    if(audfmt_restore_track(track, bytes_read) != AUDFMT_STATUS_SUCCESS) {
      ret_val = AUDFMT_STATUS_ERROR;
    }
  }

  return(ret_val);
} /* end parse_smaf_hp_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_do_mobile_note

DESCRIPTION
  This function parses one smaf mobile note command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type smaf_do_mobile_note (
  audfmt_parse_type *midi_ctl,
  uint8             *buf,
  uint32            gate
) {
  audfmt_status_type         ret_val = AUDFMT_STATUS_SUCCESS;
  audfmt_smaf_dls_entry_type *fm_dls;
  audfmt_param_def_type      *param_ptr;
  audfadec_wa_info_type      wa_info;
  uint32                     adec_chan, counter;
  uint8                      bank_cmd[4];
  uint8 noteno;
  uint8 loop;
  uint8 limit;
  audfmt_smaf_channel_pair *temp_ptr,*note_tmp;
  boolean note_pair = FALSE;
  sint15 pan_val=0;
  temp_ptr = midi_ctl->channel_pair_ptr;

  if((((buf[1] == 0x9) && (midi_ctl->bank_msb[buf[1]] == 0))|| 
      SMAF_MOBILE_DRUM_BANK(midi_ctl->bank_msb[buf[1]])) &&
     (buf[2] <= 0x3e) && (midi_ctl->wav_store[buf[2]].location != 0)) {
     noteno = buf[2];
     limit=1;
	 loop=0;
    	 do{
	 	loop++;	
		while((temp_ptr!=NULL)&&(midi_ctl->midi_status3&AUDFMT_MASK_SMAF_ADPCM_PAIR)
			&&loop==1){

		   if((buf[2]==temp_ptr->wavid2||buf[2]==temp_ptr->wavid1)&& 
		   	   temp_ptr->pair_status == TRUE){
		   	   note_pair = TRUE;
               temp_ptr->pair_status = FALSE;
			   break;
			}else{
			 	temp_ptr = temp_ptr->next_channel_pair;	
		   }
		}
		
		if(!note_pair && (midi_ctl->midi_status3&AUDFMT_MASK_SMAF_ADPCM_PAIR)
			&&loop==1){
		   note_tmp = midi_ctl->channel_pair_ptr;
		   while(note_tmp!=NULL){
		    if((note_tmp->wavid1 == buf[2])||(note_tmp->wavid2==buf[2])){
               temp_ptr = note_tmp;
			 }
		  	note_tmp = note_tmp->next_channel_pair;			  	
		  }
		}
		 if((temp_ptr!=NULL)&&(midi_ctl->midi_status3&AUDFMT_MASK_SMAF_ADPCM_PAIR)&&
		    (midi_ctl->wav_store[temp_ptr->wavid1].location!=0)&&
			(midi_ctl->wav_store[temp_ptr->wavid2].location!=0)&&
			(buf[2]==temp_ptr->wavid2||buf[2]==temp_ptr->wavid1)){

           if(loop==1){
 			 noteno = temp_ptr->wavid1;
 			}else{
 		     noteno = temp_ptr->wavid2;
			}
			limit=2;
		 }
		    
	    if(!(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE)) {
			
	      switch(SMAF_WAVE_FORMAT(midi_ctl->wav_spec[noteno].format)) {
	        case SMAF_MOBILE_WAVE_FORMAT_PCM:
	          wa_info.yadpcm_mode = AUDMAIN_ADPCM_SIGNED_PCM_MODE;
	          break;

	        case SMAF_MOBILE_WAVE_FORMAT_YADPCM:
	          wa_info.yadpcm_mode = AUDMAIN_ADPCM_YADPCM_MODE;
	          break;

		/* Offset Binary PCM */
	    case SMAF_MOBILE_WAVE_FORMAT_OBPCM:
		  wa_info.yadpcm_mode=AUDMAIN_ADPCM_UNSIGNED_PCM_MODE;
		  break; 

	        default:
	          return(ret_val);
	      }

	      switch(SMAF_WAVE_BASE_BIT(midi_ctl->wav_spec[noteno].format)) {
	        case SMAF_WAVE_BASE_BIT_4:
	          wa_info.bits_per_sample = 4;
	          break;

	        case SMAF_WAVE_BASE_BIT_8:
	          wa_info.bits_per_sample = 8;
	          break;

	        case SMAF_WAVE_BASE_BIT_12:
	        case SMAF_WAVE_BASE_BIT_16:
	        default:
	          return(ret_val);
	      }

	      wa_info.channels = SMAF_WAVE_CHANNELS(midi_ctl->wav_spec[noteno].format);
	      wa_info.sample_rate_hz = midi_ctl->wav_spec[noteno].sampling_rate;

	      wa_info.block_start = 0;
	      wa_info.location    = midi_ctl->wav_store[noteno].location;
	      wa_info.length      = midi_ctl->wav_store[noteno].num_bytes;
	      wa_info.next_block  = 0;
	      wa_info.buf1        = NULL;
	      wa_info.length_buf1 = 0;
	      wa_info.buf2        = NULL;
	      wa_info.length_buf2 = 0;
	      wa_info.adec_mode   = AUDMAIN_ADEC_MODE_YADPCM;
	      wa_info.mfi_flag    = FALSE;

	      if(audfmt_get_adec_channel(midi_ctl, &adec_chan) ==
	         AUDFMT_STATUS_SUCCESS) {

			/*Decide Pan here*/
			pan_val = (sint15)audfmt_get_smaf_chn_pan(midi_ctl,noteno,buf[1]);
   	        midi_ctl->wav_chan_pan[adec_chan] = (sint15)pan_val;
			midi_ctl->adec_status[adec_chan].panning = TRUE;	 

	        midi_ctl->adec_status[adec_chan].enable  = TRUE;
	        midi_ctl->adec_status[adec_chan].wav_id  = noteno;
	        midi_ctl->adec_status[adec_chan].reset   = FALSE;
	        midi_ctl->adec_status[adec_chan].channel = adec_chan;

			counter = midi_ctl->cmid_master_vol + 1;
			
                midi_ctl->adec_chan_vol[adec_chan] = buf[3];
	        if(midi_ctl->wav_spec[noteno].mode_3d) {
	          midi_ctl->wav_chan_vol[adec_chan] = ((counter * counter) >> 1) << 1;
	        } else {
	          midi_ctl->wav_chan_vol[adec_chan] = audfmt_set_smaf_chn_volume(midi_ctl, buf[3]);
	        }
	        midi_ctl->adec_status[adec_chan].volume  = TRUE;
	        audfadec_wa_out_qcp(midi_ctl->handle, adec_chan, &wa_info,
	                            main_audfmt_parse_ctl, FALSE);

	    if(gate != 0) {
          gate++;
          if(cmid_note_off_check_overlap(midi_ctl, gate, buf[1],
                                         AUDFMT_NO_ADEC_CHAN(adec_chan)) !=
             AUDFMT_STATUS_SUCCESS) {
            cmid_note_off_add(midi_ctl, gate, buf[1],
                              AUDFMT_NO_ADEC_CHAN(adec_chan), 0);
            /* Do LED & Vib note */
            if(!(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE) &&
               (midi_ctl->cb_store != NULL)) {
              if (midi_ctl->smaf_led_ctl & (1<<buf[1])) {
                if(midi_ctl->smaf_led_count == 0) {
                  smaf_led_buffer_cb(midi_ctl, SND_SMAF_LED_CTL_STATUS_ON, SND_CMX_SMAF_LED_CONTROL);
                }
                midi_ctl->smaf_led_count = midi_ctl->smaf_led_count + 1;
              }
              if (midi_ctl->smaf_vib_ctl & (1<<buf[1])) {
                if(midi_ctl->smaf_vib_count == 0) {
                  smaf_vib_buffer_cb(midi_ctl, SND_SMAF_VIB_CTL_STATUS_ON, SND_CMX_SMAF_VIB_CONTROL);
                }
                midi_ctl->smaf_vib_count = midi_ctl->smaf_vib_count + 1;
			  }
		    }
          }
        } else {
	          cmid_note_off_remove(midi_ctl, buf[1],
	                               AUDFMT_NO_ADEC_CHAN(adec_chan));
	        }
	      }
	    }
	}while(loop<limit);
	
  } else {
    if(((buf[1] == 0x9) && (midi_ctl->bank_msb[buf[1]] == 0)) ||
       SMAF_MOBILE_DRUM_BANK(midi_ctl->bank_msb[buf[1]])) {
      /* Check if DLS drum */
      param_ptr = audfmt_dls_struct.param_def;
      for(counter = 0; counter < 128; counter++) {
        if((param_ptr->in_use_flag ==
            audfmt_prog_def_in_use_seq[midi_ctl->sequence]) &&
           (param_ptr->bank_msb == midi_ctl->bank_msb[buf[1]]) &&
           (param_ptr->bank_lsb == midi_ctl->bank_lsb[buf[1]]) &&
           (param_ptr->program == buf[2])) {
          break;
        }
        param_ptr++;
      }

      if(counter < 128) {
        buf[2] = counter;
        if(midi_ctl->bank_status[buf[1]] != AUDFMT_BANK_DRUM_DLS) {
          bank_cmd[0] = MIDI_CONTROL_CHANGE;
          bank_cmd[1] = buf[1];
          bank_cmd[2] = MIDI_BANK_SEL_COMMAND;
          bank_cmd[3] = CMID_BANK_DRUM_DLS;

          ret_val = audfmt_add_command(midi_ctl, bank_cmd,
                                       AUDFMT_COMMAND_SIZE_4);
          if(ret_val == AUDFMT_STATUS_SUCCESS) {
            midi_ctl->bank_status[buf[1]] = AUDFMT_BANK_DRUM_DLS;
          }
        }
      } else {
        if(midi_ctl->bank_status[buf[1]] != AUDFMT_BANK_DRUM) {
          bank_cmd[0] = MIDI_CONTROL_CHANGE;
          bank_cmd[1] = buf[1];
          bank_cmd[2] = MIDI_BANK_SEL_COMMAND;
          if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_DEFAULT_HQ) {
            bank_cmd[3]  = CMID_BANK_DRUM_HQ;
          } else {
            bank_cmd[3]  = CMID_BANK_DRUM;
          }

          ret_val = audfmt_add_command(midi_ctl, bank_cmd,
                                       AUDFMT_COMMAND_SIZE_4);
          if(ret_val == AUDFMT_STATUS_SUCCESS) {
            midi_ctl->bank_status[buf[1]] = AUDFMT_BANK_DRUM;
          }
        }

        fm_dls = midi_ctl->smaf_fm_drum;
        for(counter = 0; counter < midi_ctl->smaf_fm_drum_index; counter++) {
          if((fm_dls->bank_msb == midi_ctl->bank_msb[buf[1]]) &&
             (fm_dls->bank_lsb == midi_ctl->bank_lsb[buf[1]]) &&
             (fm_dls->program == buf[2])) {
            buf[2] = fm_dls->instrument;
            break;
          }
          fm_dls++;
        }
      }
    }

    if(ret_val == AUDFMT_STATUS_SUCCESS) {
      ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
      if(ret_val == AUDFMT_STATUS_SUCCESS) {
        if(cmid_note_off_add(midi_ctl, gate, buf[1], buf[2], 0) !=
           AUDFMT_STATUS_SUCCESS) {
          /* Could not add note off command; we're better off not issuing
          ** note on.
          */
          midi_ctl->cmd_buf.num_words -= AUDFMT_COMMAND_SIZE_4;
          midi_ctl->cmd_buf.num_commands--;
        } else {
          /* Do LED & Vib note */
          if(!(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE) &&
             (midi_ctl->cb_store != NULL)) {
            if (midi_ctl->smaf_led_ctl & (1<<buf[1])) {
              if(midi_ctl->smaf_led_count == 0) {
                smaf_led_buffer_cb(midi_ctl, SND_SMAF_LED_CTL_STATUS_ON, SND_CMX_SMAF_LED_CONTROL);
              }
              midi_ctl->smaf_led_count = midi_ctl->smaf_led_count + 1;
            }
            if (midi_ctl->smaf_vib_ctl & (1<<buf[1])) {
              if(midi_ctl->smaf_vib_count == 0) {
                smaf_vib_buffer_cb(midi_ctl, SND_SMAF_VIB_CTL_STATUS_ON, SND_CMX_SMAF_VIB_CONTROL);
              }
              midi_ctl->smaf_vib_count = midi_ctl->smaf_vib_count + 1;
            }
          }
        }
      }
    }
  }
  return(ret_val);
} /* end smaf_do_mobile_note */

/* <EJECT> */
/*===========================================================================

FUNCTION parse_smaf_mobile_cmd

DESCRIPTION
  This function parses one smaf mobile command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type parse_smaf_mobile_cmd (
  audfmt_parse_type    *midi_ctl,
  uint8                track_num
) {
  audfmt_status_type         ret_val = AUDFMT_STATUS_SUCCESS;
  audfmt_track_type          *track;
  audfmt_param_def_type      *param_ptr;
  audfmt_smaf_dls_entry_type *fm_dls;

  uint8  status, tbyte1, tbyte2, buf[9];
  uint8  num_bits;
  uint8  compressed_bits;
  uint32 tlong, length, bytes_read = 0, counter, offset;
  audfmt_smaf_channel_pair *temp_ptr,*temp_ptr1;
  audfmt_smaf_channel_pan *pantmp,*pantmp1;
  boolean skip = FALSE;

  track = &midi_ctl->tracks[track_num];

  /* Save compressed bit information */
  num_bits        = track->num_compressed_bits;
  compressed_bits = track->compressed_bits;
  offset          = track->current_offset;

  ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track, &status,
                                            &bytes_read);
  if(ret_val != AUDFMT_STATUS_SUCCESS) {
    if(bytes_read != 0) {
      /* If this command parse was not successful, restore the track to
      ** original condition.
      */
      if(audfmt_restore_track(track, bytes_read) != AUDFMT_STATUS_SUCCESS) {
        ret_val = AUDFMT_STATUS_ERROR;
      }
    }
    track->num_compressed_bits = num_bits;
    track->compressed_bits     = compressed_bits;
    track->current_offset      = offset;
    return(ret_val);
  }

  switch(status & MIDI_STATUS_NIBBLE) {
    case SMAF_MOBILE_SHORT_NOTE_CMD:
      ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track, &tbyte1,
                                                &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;
      ret_val = midi_read_var_len_smaf(midi_ctl, track, &length, &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;

      if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_INITIAL_SKIP)) &&
         !(track->track_status & AUDFMT_TRACK_MASK_OFFSET_SKIP)) {
        buf[0] = MIDI_NOTE_ON;
        buf[1] = status & MIDI_CHANNEL_NIBBLE;
        buf[2] = tbyte1;
        buf[3] = midi_ctl->chan_vol[buf[1]];

        ret_val = smaf_do_mobile_note(midi_ctl, buf, length);
      }

      if(ret_val == AUDFMT_STATUS_SUCCESS) {
        midi_ctl->midi_status &= ~AUDFMT_MASK_SILENCE_SKIP;
      }
      break;

    case MIDI_NOTE_ON:
      ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track, &tbyte1,
                                                &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;
      ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track, &tbyte2,
                                                &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;
      ret_val = midi_read_var_len_smaf(midi_ctl, track, &length, &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;

      buf[1] = status & MIDI_CHANNEL_NIBBLE;
      midi_ctl->chan_vol[buf[1]] = tbyte2;

      if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_INITIAL_SKIP)) &&
         !(track->track_status & AUDFMT_TRACK_MASK_OFFSET_SKIP)) {
        buf[0] = MIDI_NOTE_ON;
        buf[2] = tbyte1;
        buf[3] = tbyte2;

        ret_val = smaf_do_mobile_note(midi_ctl, buf, length);
      }

      if(ret_val == AUDFMT_STATUS_SUCCESS) {
        midi_ctl->midi_status &= ~AUDFMT_MASK_SILENCE_SKIP;
      }
      break;

    case MIDI_CONTROL_CHANGE:
      ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track, &tbyte1,
                                                &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;
      ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track, &tbyte2,
                                                &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;

      if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
        buf[0] = MIDI_CONTROL_CHANGE;
        buf[1] = status & MIDI_CHANNEL_NIBBLE;
        buf[2] = tbyte1;
        buf[3] = tbyte2;

        if(buf[2] == MIDI_BANK_SEL_COMMAND) {
          midi_ctl->bank_msb[buf[1]] = buf[3];
        } else if(buf[2] == MIDI_FINE_BANK_SEL_COMMAND) {
          midi_ctl->bank_lsb[buf[1]] = buf[3];
        } else if(buf[2] == SMAF_MA5_PITCH_BEND_CMD){
         /*2681
            when Ma5 Pitch Bend command is found in the File
            convert it to Ma3 format since Ma5 format for
            Pitch bend is not a standard MIDI command format.
            Discard fine volume for Ma5.
            Changed the seq as per 163002          
            new mapping-0xBn 0x64 0x0 0xBn 0x65 0x0 0xBn 0x6 <value>
		*/  
          buf[0] = MIDI_CONTROL_CHANGE;
          buf[1] = status & MIDI_CHANNEL_NIBBLE;
          buf[2] = 0x64;
          buf[3] = 0x00;
		 ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
		 
		 if(ret_val==AUDFMT_STATUS_SUCCESS) {
	      buf[0] = MIDI_CONTROL_CHANGE;
          buf[1] = status & MIDI_CHANNEL_NIBBLE;
          buf[2] = 0x65;
          buf[3] = 0x00;
		  ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);		 
		 }
		  if(ret_val==AUDFMT_STATUS_SUCCESS) {
		  buf[0] = MIDI_CONTROL_CHANGE;
          buf[1] = status & MIDI_CHANNEL_NIBBLE;
          buf[2] = 0x06;
          buf[3] = tbyte2;			
  	      ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
		  }
		  /*
         if(ret_val==AUDFMT_STATUS_SUCCESS) {
		  buf[0] = MIDI_CONTROL_CHANGE;
          buf[1] = status & MIDI_CHANNEL_NIBBLE;
          buf[2] = 0x26;
          buf[3] = 0x00;			
  	      ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
		  }
		  */
       }else if (buf[2] == MIDI_PANPOT_COMMAND) {
		 {
		  /*
		      panning changes start-CC10
		      See if any NOTE is ON with Midi Channel in control message.
		  */
		  uint32 index=0;
		  audfmt_note_off_buffer_type *note_buffer;
		  boolean skip;
		  audfmt_smaf_channel_cc10_pan *cc10_pantmp,*cc10_pantmp1;
		  skip = FALSE;
		  note_buffer = &midi_ctl->note_off_buffer;
		  /*
			Panning in Control Message Format
			0xBn, 0x0A, <val>, where n is the MIDI channel 
			 and <val> is the panning value.
		  */
		   while(index < CMID_NOTE_OFF_MAX_COMMANDS) {
             if((buf[1] & 0x0F) == note_buffer->commands[index].channel) {
               /* 
                 note off channel matches CC channel 
               */
               for(counter = 0; counter < AUDFMT_NUM_ADEC_CHANNELS; counter++) {
                 if( midi_ctl->adec_status[counter].enable &&
					   (midi_ctl->adec_status[counter].channel == 
						  AUDFMT_NO_GET_ADEC_CHAN(note_buffer->commands[index].note))) {

                   /*
                     This is an ADEC channel so set adec panning fields 
                   */
                   midi_ctl->wav_chan_pan[counter] = (sint15)(buf[3]-0x40);
                   midi_ctl->adec_status[counter].panning = TRUE;
                   index = CMID_NOTE_OFF_MAX_COMMANDS; 
                   break; /* out of for loop*/
                 }
               }
             } 
             index++;
           } /* end note off loop*/

           ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);

	  /*
	    Save the data
	    whether Pan hits before/after NOTE Message
	  */

	  if(midi_ctl->cc10_channel_pan_ptr==NULL){
		  midi_ctl->cc10_channel_pan_ptr = (audfmt_smaf_channel_cc10_pan*) 
			   							malloc(sizeof(audfmt_smaf_channel_cc10_pan));
		  midi_ctl->cc10_channel_pan_ptr->midi_channel= buf[1];
		  midi_ctl->cc10_channel_pan_ptr->pan_val = (sint15)(buf[3]-0x40);
  		  midi_ctl->cc10_channel_pan_ptr->seq = (pan_seq_counter++);
	      midi_ctl->cc10_channel_pan_ptr->next_cc10_channel_pan= NULL;

      }else{
         cc10_pantmp1= midi_ctl->cc10_channel_pan_ptr;
		 
	     while(((cc10_pantmp1->next_cc10_channel_pan)!=NULL) && skip==FALSE){

		  if(cc10_pantmp1->midi_channel==(buf[1])){
		    skip = TRUE;
		    cc10_pantmp1->pan_val= (sint15)(buf[3]-0x40);
			cc10_pantmp1->seq= (pan_seq_counter++);
	      }else{
	       cc10_pantmp1 = cc10_pantmp1->next_cc10_channel_pan;
	      }
	     }	
		if(!skip){
		   cc10_pantmp= (audfmt_smaf_channel_cc10_pan*) 
		     			malloc(sizeof(audfmt_smaf_channel_cc10_pan));
		   cc10_pantmp->midi_channel= buf[1];					
	       cc10_pantmp->pan_val= (sint15)(buf[3]-0x40);
	  	   cc10_pantmp->next_cc10_channel_pan= NULL;
		   cc10_pantmp->seq = (pan_seq_counter++);
		   cc10_pantmp1->next_cc10_channel_pan= cc10_pantmp;	
		}		   			   
	  }  
		 
  	}/*End of local block*/
   /* panning changes end*/
   }else {
          ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
        }
      }
      break;

    case MIDI_PITCH_WHEEL:
      ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track, &tbyte1,
                                                &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;
      ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track, &tbyte2,
                                                &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;

      if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
        buf[0] = MIDI_PITCH_WHEEL;
        buf[1] = status & MIDI_CHANNEL_NIBBLE;
        buf[2] = tbyte1;
        buf[3] = tbyte2;

        ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_4);
      }
      break;

    case MIDI_PROGRAM_CHANGE:
      ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track, &tbyte1,
                                                &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;

      if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
        tbyte2 = status & MIDI_CHANNEL_NIBBLE;
        if(!(((tbyte2 == 0x9) && (midi_ctl->bank_msb[tbyte2] == 0)) ||
             SMAF_MOBILE_DRUM_BANK(midi_ctl->bank_msb[tbyte2]))) {
          param_ptr = audfmt_dls_struct.param_def;
          for(counter = 0; counter < 128; counter++) {
            if((param_ptr->in_use_flag ==
                audfmt_prog_def_in_use_seq[midi_ctl->sequence]) &&
               (param_ptr->bank_msb == midi_ctl->bank_msb[tbyte2]) &&
               (param_ptr->bank_lsb == midi_ctl->bank_lsb[tbyte2]) &&
               (param_ptr->program == tbyte1)) {
              break;
            }
            param_ptr++;
          }

          if(counter < 128) {
            tbyte1 = counter;
            if(midi_ctl->bank_status[tbyte2] != AUDFMT_BANK_NORMAL_DLS) {
              buf[0]  = MIDI_CONTROL_CHANGE;
              buf[1]  = tbyte2;
              buf[2]  = MIDI_BANK_SEL_COMMAND;
              buf[3]  = CMID_BANK_NORMAL_DLS;
              ret_val = audfmt_add_command(midi_ctl, buf,
                                                 AUDFMT_COMMAND_SIZE_4);
              if(ret_val == AUDFMT_STATUS_SUCCESS) {
                midi_ctl->bank_status[tbyte2] = AUDFMT_BANK_NORMAL_DLS;
              }
            }
          } else {
            fm_dls = midi_ctl->smaf_fm_dls;
            for(counter = 0; counter < midi_ctl->smaf_fm_dls_index; counter++) {
              if((fm_dls->bank_msb == midi_ctl->bank_msb[tbyte2]) &&
                 (fm_dls->bank_lsb == midi_ctl->bank_lsb[tbyte2]) &&
                 (fm_dls->program == tbyte1)) {
                tbyte1 = fm_dls->instrument;
                break;
              }
              fm_dls++;
            }

            if(midi_ctl->bank_status[tbyte2] != AUDFMT_BANK_NORMAL) {
              buf[0]  = MIDI_CONTROL_CHANGE;
              buf[1]  = tbyte2;
              buf[2]  = MIDI_BANK_SEL_COMMAND;
              if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_DEFAULT_HQ) {
                buf[3]  = CMID_BANK_NORMAL_HQ;
              } else {
                buf[3]  = CMID_BANK_NORMAL;
              }
              ret_val = audfmt_add_command(midi_ctl, buf,
                                                 AUDFMT_COMMAND_SIZE_4);
              if(ret_val == AUDFMT_STATUS_SUCCESS) {
                midi_ctl->bank_status[tbyte2] = AUDFMT_BANK_NORMAL;
              }
            }
          }

          if(ret_val == AUDFMT_STATUS_SUCCESS) {
            buf[0] = MIDI_PROGRAM_CHANGE;
            buf[1] = tbyte2;
            buf[2] = tbyte1;

            ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_3);
          }
        }
      }
      break;

    case MIDI_AFTER_TOUCH:
      ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track, &tbyte1,
                                                &bytes_read);
      if(ret_val != AUDFMT_STATUS_SUCCESS) break;

    case MIDI_CHANNEL_PRESSURE:
      ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track, &tbyte1,
                                                &bytes_read);
      break;

    case MIDI_SYSTEM_MESSAGE:
    if(status == MIDI_META_EVENT) {
        ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track, &tbyte1,
                                                  &bytes_read);
        if(ret_val == AUDFMT_STATUS_SUCCESS) {
          if(tbyte1 == MIDI_END_OF_TRACK) {
            ret_val = AUDFMT_STATUS_DONE;
          }else if((tbyte1 == MIDI_NOP_EVENT)&& 
           (track->current_location == track->track_end)){
              /*
                combination of NOP/EOS/0x00NOP also mean END of track
              */
            ret_val = AUDFMT_STATUS_DONE;
	      }
        }
      } else if(status == MIDI_SYS_EXCLUSIVE) {
        ret_val = midi_read_var_len_smaf(midi_ctl, track, &length, &bytes_read);
        if(ret_val == AUDFMT_STATUS_SUCCESS ) {
          if(length == 7) {
            for(counter = 0; counter < length; counter++) {
              ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track,
                                                        &buf[counter],
                                                        &bytes_read);
              if(ret_val != AUDFMT_STATUS_SUCCESS) break;
            }
            if((ret_val == AUDFMT_STATUS_SUCCESS) &&
               !(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
         					   
              if((buf[0] == 0x43) && (buf[1] == 0x79) &&
                ((buf[2] == 0x07)|| (buf[2] == 0x06)) &&
                 (buf[3] == 0x7f) && (buf[4] == 0x00)) {
                buf[6] = buf[5];
                buf[0] = 0xf0;
                buf[1] = 0x7f;
                buf[2] = 0x7f;
                buf[3] = 0x04;
                buf[4] = 0x01;
                buf[5] = 0x00;
                buf[7] = 0xf7;
                ret_val = audfmt_add_command(midi_ctl, buf, 8);
                if(ret_val == AUDFMT_STATUS_SUCCESS) {
                  midi_ctl->cmid_master_vol = buf[6];
                  tlong = midi_ctl->cmid_master_vol + 1;
                  tlong = (tlong * tlong) >> 1;
                  for(counter = 0; counter < AUDFMT_NUM_ADEC_CHANNELS;
                      counter++) {
                    if(midi_ctl->adec_status[counter].enable){
                      if(midi_ctl->wav_spec[
                              midi_ctl->adec_status[counter].wav_id].mode_3d) {
                        midi_ctl->wav_chan_vol[counter] = tlong << 1;
                      } else {
	                midi_ctl->wav_chan_vol[counter] =
                             audfmt_set_smaf_chn_volume(midi_ctl,
                                         midi_ctl->adec_chan_vol[counter]);
                      }		
                  
					  midi_ctl->adec_status[counter].volume  = TRUE;
		             }
                   }
				   MSG_ERROR("Master volume: %d", buf[6], 0, 0);
                  }				  	  
                }
             }
          }else if(length == 9) {
            for(counter = 0; counter < length; counter++) {
              ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track,
                                                        &buf[counter],
                                                        &bytes_read);
              if(ret_val != AUDFMT_STATUS_SUCCESS) break;
            }
		 if(ret_val == AUDFMT_STATUS_SUCCESS){

			 if((buf[0] == 0x43) && (buf[1] == 0x79) &&
	            (buf[2] == 0x06)  && (buf[3] == 0x7f) && 
	            (buf[4] == 0x08) && (buf[8]==0xF7)&&
	            (midi_ctl->midi_status3 & AUDFMT_MASK_SMAF_MA3)&&
		        (midi_ctl->midi_status3 & AUDFMT_MASK_SMAF_ADPCM_PAIR)){
		          if(midi_ctl->channel_pair_ptr==NULL){
					midi_ctl->channel_pair_ptr = (audfmt_smaf_channel_pair*) 
						   							malloc(sizeof(audfmt_smaf_channel_pair));
					midi_ctl->channel_pair_ptr->wavid1 = (buf[6]-1);
					midi_ctl->channel_pair_ptr->wavid2 = (buf[7]-1);
					midi_ctl->channel_pair_ptr->pair_status = TRUE;
					midi_ctl->channel_pair_ptr->next_channel_pair = NULL;
	             }else{
	               temp_ptr1 = midi_ctl->channel_pair_ptr;
				   while((temp_ptr1->next_channel_pair)!=NULL){
				    temp_ptr1 = temp_ptr1->next_channel_pair;
				   }				   
				   temp_ptr = (audfmt_smaf_channel_pair*) 
				     			malloc(sizeof(audfmt_smaf_channel_pair));
				   temp_ptr->wavid1 = (buf[6]-1);
				   temp_ptr->wavid2 = (buf[7]-1);
				   temp_ptr->next_channel_pair = NULL;
				   temp_ptr->pair_status = TRUE;
				   temp_ptr1->next_channel_pair = temp_ptr;				 
				 } 
		      }
		   }
			
	    if(ret_val == AUDFMT_STATUS_SUCCESS){

			   if((buf[0] == 0x43) && (buf[1] == 0x79) &&
		          ((buf[2] == 0x06)||(buf[2] == 0x07)) &&
		          (buf[3] == 0x7f) && (buf[4] == 0x0B) && (buf[8]==0xF7)){

				 {
				 	/*
				 	 If SYS EX comes while note on is
					 Active
					 */
				   uint32 counter=0;
				   for(counter=0;counter<=3;counter++){
					 if((midi_ctl->adec_status[counter].enable==TRUE) && 
					 	(midi_ctl->adec_status[counter].wav_id == (buf[5]-1))){

						midi_ctl->wav_chan_pan[counter] = (sint15)(buf[7]-0x40);
						midi_ctl->adec_status[counter].panning = TRUE;
					 }
				   }	
				 }
		      if(midi_ctl->channel_pan_ptr==NULL){
					  midi_ctl->channel_pan_ptr = (audfmt_smaf_channel_pan*) 
						   							malloc(sizeof(audfmt_smaf_channel_pan));
					  midi_ctl->channel_pan_ptr->wavid = (buf[5]-1);
					  midi_ctl->channel_pan_ptr->pan_val= (sint15)(buf[7]-0x40);
					  midi_ctl->channel_pan_ptr->seq = (pan_seq_counter++);
				      midi_ctl->channel_pan_ptr->next_channel_pan= NULL;
	             }else{
	               pantmp1= midi_ctl->channel_pan_ptr;
				   
				   while(((pantmp1->next_channel_pan)!=NULL) && skip==FALSE){

					if(pantmp1->wavid ==(buf[5]-1)){
					   skip = TRUE;
					   pantmp1->pan_val= (sint15)(buf[7]-0x40);		
					   pantmp1->seq = (pan_seq_counter++);
				    }else{
				      pantmp1 = pantmp1->next_channel_pan;
				     }
				   }	
					if(!skip){
					   pantmp= (audfmt_smaf_channel_pan*) 
					     			malloc(sizeof(audfmt_smaf_channel_pan));
					   pantmp->wavid= (buf[5]-1);					
 			           pantmp->pan_val= (sint15)(buf[7]-0x40);
					   pantmp->seq = (pan_seq_counter++);
				  	   pantmp->next_channel_pan = NULL;
					   pantmp1->next_channel_pan= pantmp;	
					}		   			   
				 }  					
			  }			
	       	}

            if(ret_val == AUDFMT_STATUS_SUCCESS) {
              if((buf[0] == 0x43) && (buf[1] == 0x79) && (buf[2] == 0x07) &&
                 (buf[3] == 0x7f) && (buf[4] == 0x0b) && (buf[5] >= 1)) {
                midi_ctl->wav_spec[buf[5] - 1].mode_3d = (buf[6] == 2);

                tlong = midi_ctl->cmid_master_vol + 1;
                tlong = (tlong * tlong) >> 1;
                if(buf[6] == 2) {
                  tlong = tlong << 1;
                }
                for(counter = 0; counter < AUDFMT_NUM_ADEC_CHANNELS;
                    counter++) {
                  if((midi_ctl->adec_status[counter].enable) &&
                     (midi_ctl->adec_status[counter].wav_id == (buf[5] - 1))) {
                   if(buf[6] == 2) {
                      midi_ctl->wav_chan_vol[counter]        = tlong;
                    } else {
                      midi_ctl->wav_chan_vol[counter] =
                             audfmt_set_smaf_chn_volume(midi_ctl,
                                         midi_ctl->adec_chan_vol[counter]);
                    }
                    midi_ctl->adec_status[counter].volume  = TRUE;
                  }
                }
              }
            }
          } else {
            for(;length > 0; length--) {
              ret_val = audfmt_get_next_track_byte_smaf(midi_ctl, track,
                                                        &tbyte1, &bytes_read);
              if(ret_val != AUDFMT_STATUS_SUCCESS) break;
            }
          }
        }
      } else {
        ret_val = AUDFMT_STATUS_DATA_ERR;
      }
      break;

    default:
      break;
  }

  if((ret_val != AUDFMT_STATUS_SUCCESS) && (ret_val != AUDFMT_STATUS_ERROR) &&
     (ret_val != AUDFMT_STATUS_DATA_ERR) && (ret_val != AUDFMT_STATUS_DONE)) {
    if(bytes_read != 0) {
      /* If this command parse was not successful, restore the track to
      ** original condition.
      */
      if(audfmt_restore_track(track, bytes_read) != AUDFMT_STATUS_SUCCESS) {
        ret_val = AUDFMT_STATUS_ERROR;
      }
    }
    track->num_compressed_bits = num_bits;
    track->compressed_bits     = compressed_bits;
    track->current_offset      = offset;
  }
  return(ret_val);
} /* end parse_smaf_mobile_cmd */
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
/*===========================================================================

FUNCTION parse_midi_cmd

DESCRIPTION
  This function parses one midi command.

DEPENDENCIES
  None

RETURN VALUE
  cmd_buf - the parsed command is entered into this buffer.

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type parse_midi_cmd (
  audfmt_parse_type    *midi_ctl,
  uint8                track_num
) {
  audfmt_status_type ret_val;
  audfmt_track_type  *track;

  uint8  command, tbyte1, tbyte2, buf[8], max_voices = AUDFMT_MAX_MIDI_VOICES;
  uint32 length, bytes_read = 0;

#ifdef FEATURE_DLS
  audfmt_inst_def_type *inst_def;
  uint32               counter;
#endif /* FEATURE_DLS */

  track   = &midi_ctl->tracks[track_num];
  ret_val = audfmt_peek_next_track_byte(track, &command);
  if(ret_val != AUDFMT_STATUS_SUCCESS) return(ret_val);

  if(MIDI_STATUS_BYTE(command)) {
    (void) audfmt_get_next_track_byte(track, &command, &bytes_read);
  } else {
    command = track->running_status;
  }

  if(!MIDI_STATUS_BYTE(command)) {
    MSG_ERROR("Invalid running status: %x", command, 0, 0);
    return(AUDFMT_STATUS_DATA_ERR);
  }

  switch(command & MIDI_STATUS_NIBBLE) {
    case MIDI_NOTE_OFF:
    case MIDI_NOTE_ON:
    case MIDI_AFTER_TOUCH:
    case MIDI_CONTROL_CHANGE:
    case MIDI_PITCH_WHEEL:
      if(track->delta_tick != 0) {
        ret_val = AUDFMT_STATUS_CMD_FOUND;
      } else {
        track->running_status = command;
        ret_val = audfmt_get_next_track_byte(track, &tbyte1, &bytes_read);
        if(ret_val != AUDFMT_STATUS_SUCCESS) break;
        ret_val = audfmt_get_next_track_byte(track, &tbyte2, &bytes_read);
        if(ret_val != AUDFMT_STATUS_SUCCESS) break;

        if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
          buf[0] = command & MIDI_STATUS_NIBBLE;
          buf[1] = command & MIDI_CHANNEL_NIBBLE;
          buf[2] = MIN(tbyte1, 0x7f);
          buf[3] = MIN(tbyte2, 0x7f);

          if(midi_ctl->channel_active[buf[1]]) {
            if((buf[0] == MIDI_NOTE_ON) && (buf[3] == 0)) {
              buf[0] = MIDI_NOTE_OFF;
            }

            if(buf[0] == MIDI_NOTE_OFF) {
              if((midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE) &&
                 (audfmt_check_no_command(&midi_ctl->cmd_buf, buf) ==
                                               AUDFMT_STATUS_SUCCESS)) {
                break;
              }
            }

            if(buf[0] == MIDI_CONTROL_CHANGE) {
              if(buf[2] == MIDI_BANK_SEL_COMMAND) {
                midi_ctl->bank_msb[buf[1]] = buf[3];
                break;
              } else if(buf[2] == 0x20) {
                midi_ctl->bank_lsb[buf[1]] = buf[3];
                break;
              }
            }

#ifdef FEATURE_AUDIO_CMX_5P0
            /* Not to send redundant pitch, vol, pan during ffw for midi.
             * buffer them is parser and send before unpausing dsp 
             */
            if( ( buf[0] == MIDI_PITCH_WHEEL ) &&
                ( midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE ) )
            {
              midi_ctl->midi_pitch_is_changed[buf[1]] = TRUE;
              midi_ctl->midi_ch_pitch_param[buf[1]] = buf[2]<<8 | buf[3];
            }
            else if( ( (command & MIDI_STATUS_NIBBLE) == MIDI_CONTROL_CHANGE)&&
                     ( !( buf[2] - MIDI_VOLUME_COMMAND ) ) &&
                     ( midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE ) )
            {
			  midi_ctl->midi_vol_is_changed[buf[1]] = TRUE;
			  midi_ctl->midi_ch_vol_param[buf[1]] = buf[3];
			}
            else if( ( (command & MIDI_STATUS_NIBBLE) == MIDI_CONTROL_CHANGE)&&
                     ( !( buf[2] - MIDI_PANPOT_COMMAND ) ) &&
                     ( midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE ) )
			{
			  midi_ctl->midi_pan_is_changed[buf[1]] = TRUE;
			  midi_ctl->midi_ch_pan_param[buf[1]] = buf[3];
			}
            else
#endif /* FEATURE_AUDIO_CMX_5P0 */
            ret_val = audfmt_add_command(midi_ctl, buf,
                                         AUDFMT_COMMAND_SIZE_4);
          }
        }

        if(((command & MIDI_STATUS_NIBBLE) == MIDI_NOTE_ON) &&
           (ret_val == AUDFMT_STATUS_SUCCESS)) {
          midi_ctl->midi_status &= ~AUDFMT_MASK_SILENCE_SKIP;
        }
      }
      break;

    case MIDI_PROGRAM_CHANGE:
    case MIDI_CHANNEL_PRESSURE:
      if(track->delta_tick != 0) {
        ret_val = AUDFMT_STATUS_CMD_FOUND;
      } else {
        track->running_status = command;
        ret_val = audfmt_get_next_track_byte(track, &tbyte1, &bytes_read);
        if(ret_val != AUDFMT_STATUS_SUCCESS) break;

        if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
          tbyte2 = command & MIDI_CHANNEL_NIBBLE;
#ifdef FEATURE_DLS
          if((command & MIDI_STATUS_NIBBLE) == MIDI_PROGRAM_CHANGE) {
              inst_def = audfmt_dls_table.inst_def;
              for(counter = 0; counter < AUDFMT_NUM_INST_DEF; counter++) {
                if(((inst_def->in_use_flag ==
                     audfmt_prog_def_in_use_seq[midi_ctl->sequence]) ||
                    (inst_def->in_use_flag == AUDFMT_PROG_DEF_IN_USE_ALL)) &&
                   ((inst_def->bank_msb == midi_ctl->bank_msb[tbyte2]) ||
                    (inst_def->bank_msb == 0)) &&
                   ((inst_def->bank_lsb == midi_ctl->bank_lsb[tbyte2]) ||
                    (inst_def->bank_lsb == 0)) &&
                   (inst_def->program == tbyte1)) {
                  break;
                }
                inst_def++;
              }

              if(counter < AUDFMT_NUM_INST_DEF) {
                tbyte1 = counter;
                if(midi_ctl->bank_status[tbyte2] != AUDFMT_BANK_NORMAL_DLS) {
                  buf[0]  = MIDI_CONTROL_CHANGE;
                  buf[1]  = tbyte2;
                  buf[2]  = MIDI_BANK_SEL_COMMAND;
                  buf[3]  = 0x73;
                  ret_val = audfmt_add_command(midi_ctl, buf,
                                                     AUDFMT_COMMAND_SIZE_4);
                  if(ret_val == AUDFMT_STATUS_SUCCESS) {
                    midi_ctl->bank_status[tbyte2] = AUDFMT_BANK_NORMAL_DLS;
                  }
                }
              } else {
                if(((tbyte2 != 0x09) && (midi_ctl->bank_status[tbyte2] != AUDFMT_BANK_NORMAL) ||
                     (tbyte2 == 0x09) && (midi_ctl->bank_status[tbyte2] != AUDFMT_BANK_DRUM)) 
                  ) {
                  buf[0]  = MIDI_CONTROL_CHANGE;
                  buf[1]  = tbyte2;
                  buf[2]  = MIDI_BANK_SEL_COMMAND;
                  buf[3]  = 0;
                  ret_val = audfmt_add_command(midi_ctl, buf,
                                                     AUDFMT_COMMAND_SIZE_4);
                  if(ret_val == AUDFMT_STATUS_SUCCESS) {
                    midi_ctl->bank_status[tbyte2] = 
                                    (tbyte2 == 0x09)? AUDFMT_BANK_DRUM :AUDFMT_BANK_NORMAL;
                  }
                }
              }
          }
#endif /* FEATURE_DLS */

          buf[0] = command & MIDI_STATUS_NIBBLE;
          buf[1] = tbyte2;
          buf[2] = MIN(tbyte1, 0x7f);

          if(midi_ctl->channel_active[tbyte2]) {
            ret_val = audfmt_add_command(midi_ctl, buf, AUDFMT_COMMAND_SIZE_3);
          }
        }
      }
      break;

    case MIDI_SYSTEM_MESSAGE:
      if(command == MIDI_META_EVENT) {
        ret_val = audfmt_get_next_track_byte(track, &tbyte1, &bytes_read);
        if(ret_val != AUDFMT_STATUS_SUCCESS) break;

        if(tbyte1 == MIDI_END_OF_TRACK) {
          return(AUDFMT_STATUS_DONE);
        } else {
          ret_val = midi_read_var_len(track, &length, &bytes_read);
          if(ret_val != AUDFMT_STATUS_SUCCESS) break;

          if(tbyte1 == MIDI_TEMPO_EVENT) {
            if(length != MIDI_TEMPO_LENGTH) {
              MSG_ERROR("Invalid length for tempo message! %ld", length, 0, 0);
              ret_val = AUDFMT_STATUS_DATA_ERR;
              break;
            }

            if(track->delta_tick != 0) {
              ret_val = AUDFMT_STATUS_CMD_FOUND;
            } else {
              ret_val = audfmt_get_next_track_byte(track, &tbyte2, &bytes_read);
              if(ret_val != AUDFMT_STATUS_SUCCESS) break;
              length = tbyte2;

              ret_val = audfmt_get_next_track_byte(track, &tbyte2, &bytes_read);
              if(ret_val != AUDFMT_STATUS_SUCCESS) break;
              length = length<<8 | tbyte2;

              ret_val = audfmt_get_next_track_byte(track, &tbyte2, &bytes_read);
              if(ret_val != AUDFMT_STATUS_SUCCESS) break;
              length = length<<8 | tbyte2;

              midi_ctl->tempo_pending = length;
              if(!(midi_ctl->midi_status & (AUDFMT_MASK_STARTED_AGE |
                                            AUDFMT_MASK_SEEK_ACTIVE)) &&
                 (midi_ctl->delta_tick == 0)) {
                midi_ctl->tempo = midi_ctl->tempo_pending;
                if(midi_ctl->tempo != 0) {
                  midi_ctl->calc_ms_of = AUDFMT_MAX_32BIT / midi_ctl->tempo;
                }
              }
            }
            break;
          }
        }
      } else if(command == MIDI_SYS_EXCLUSIVE) {
        track->running_status = 0;

        ret_val = midi_read_var_len(track, &length, &bytes_read);
        if(ret_val != AUDFMT_STATUS_SUCCESS) break;

        if(length >= 7) {
          ret_val = audfmt_get_next_track_byte(track, &buf[0], &bytes_read);
          if(ret_val != AUDFMT_STATUS_SUCCESS) break;
          ret_val = audfmt_get_next_track_byte(track, &buf[1], &bytes_read);
          if(ret_val != AUDFMT_STATUS_SUCCESS) break;
          ret_val = audfmt_get_next_track_byte(track, &buf[2], &bytes_read);
          if(ret_val != AUDFMT_STATUS_SUCCESS) break;
          ret_val = audfmt_get_next_track_byte(track, &buf[3], &bytes_read);
          if(ret_val != AUDFMT_STATUS_SUCCESS) break;

          length -= 4;

          if(memcmp(midi_mvol_cmd, buf, 4) == 0) {
            if(track->delta_tick != 0) {
              ret_val = AUDFMT_STATUS_CMD_FOUND;
            } else if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME)) {
              ret_val = audfmt_get_next_track_byte(track, &tbyte1, &bytes_read);
              if(ret_val != AUDFMT_STATUS_SUCCESS) break;
              ret_val = audfmt_get_next_track_byte(track, &tbyte2, &bytes_read);
              if(ret_val != AUDFMT_STATUS_SUCCESS) break;
              length -= 2;

              buf[0] = 0xf0;
              buf[1] = 0x7f;
              buf[2] = 0x7f;
              buf[3] = 0x04;
              buf[4] = 0x01;
              buf[5] = tbyte1;
              buf[6] = tbyte2;
              buf[7] = 0xf7;
// SYS EX command is not work for Standard MIDI. need feature.
// in old way, it change the mvol to the DSP register.
              ret_val = audfmt_add_command(midi_ctl, buf, 8);
            }
          } else if(memcmp(midi_mip_cmd, buf, 4) == 0) {
            memset((uint8 *)(midi_ctl->channel_active), 0,
                                   sizeof(boolean) * AUDFMT_MAX_CHANNELS);

#ifdef FEATURE_WEBAUDIO
            if(!(midi_ctl->midi_status & AUDFMT_MASK_MULTISEQ_MODE)) {
              if(midi_ctl->hybrid_mode == CMID_POLY_MAX_HP) {
                max_voices = AUDFMT_MAX_HP_VOICES;
              } else {
                max_voices = AUDFMT_MAX_HQ_VOICES;
              }
            }
#endif /* FEATURE_WEBAUDIO */

            while(length > 1) {
              ret_val = audfmt_get_next_track_byte(track, &tbyte1, &bytes_read);
              if(ret_val != AUDFMT_STATUS_SUCCESS) break;
              ret_val = audfmt_get_next_track_byte(track, &tbyte2, &bytes_read);
              if(ret_val != AUDFMT_STATUS_SUCCESS) break;
              length -= 2;

              if(tbyte2 <= max_voices) {
                if(tbyte1 < AUDFMT_MAX_CHANNELS) {
                  midi_ctl->channel_active[tbyte1] = TRUE;
                }
              } else {
                break;
              }
            }
          }
        }
      } else {
        if(command <= MIDI_END_SYS_EXCLUSIVE) {
          /* Reset running status */
          track->running_status = 0;
        }

        /* Skip this system message command */
        ret_val = midi_read_var_len(track, &length, &bytes_read);
      }

      if(ret_val == AUDFMT_STATUS_SUCCESS) {
        return(audfmt_track_skip_ahead(midi_ctl, track, length));
      }
      break;

    default:
      break;
  }

  if((ret_val != AUDFMT_STATUS_SUCCESS) && 
     (ret_val != AUDFMT_STATUS_ERROR) &&
     (ret_val != AUDFMT_STATUS_DATA_ERR) && 
     (bytes_read != 0)) {
    /* If this command parse was not successful, restore the track to 
    ** original condition.
    */
    if(audfmt_restore_track(track, bytes_read) != AUDFMT_STATUS_SUCCESS) {
      ret_val = AUDFMT_STATUS_ERROR;
    }
  }

  if(ret_val == AUDFMT_STATUS_CMD_FOUND) {
    track->track_status &= ~AUDFMT_TRACK_MASK_CMD_PEND;
  }
  return(ret_val);
} /* end parse_midi_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_parse_cmd

DESCRIPTION
  This function calls the appropriate command parser.

DEPENDENCIES
  None

RETURN VALUE
  audfmt_status_type

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_parse_cmd (
  audfmt_parse_type *midi_ctl,
  uint32            track_num
) {
  audfmt_status_type ret_val;

  switch(midi_ctl->type) {
    case AUDFMT_MIDI_MIDI_TYPE:
      ret_val = parse_midi_cmd(midi_ctl, track_num);
      break;

#ifdef FEATURE_WEBAUDIO
    case AUDFMT_MIDI_CMID_TYPE:
    case AUDFMT_MIDI_MELO_TYPE:
    case AUDFMT_MIDI_WASF_TYPE:
      ret_val = parse_cmid_cmd(midi_ctl, track_num);
      break;

#ifdef FEATURE_SMAF
    case AUDFMT_MIDI_SMAF_TYPE:
      switch(midi_ctl->smaf_type) {
        case AUDFMT_SMAF_HP_TYPE:
        case AUDFMT_SMAF_PHRASE_TYPE:
          ret_val = parse_smaf_hp_cmd(midi_ctl, track_num);
          break;

        case AUDFMT_SMAF_COMPRESSED_TYPE:
        case AUDFMT_SMAF_MOBILE_TYPE:
          ret_val = parse_smaf_mobile_cmd(midi_ctl, track_num);
          break;

        default:
          ret_val = AUDFMT_STATUS_ERROR;
          break;
      }
      break;
#endif /* FEATURE_SMAF */
#endif

    case AUDFMT_MIDI_INVALID_TYPE:
    default:
      ret_val = AUDFMT_STATUS_ERROR;
      break;
  }

  return(ret_val);
} /* end audfmt_parse_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_restart

DESCRIPTION
  This function restarts the indicated sequence from the beginning.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_restart (
  audfmt_parse_type *midi_ctl,
  boolean           clear_seek
) {
  audfmt_track_type *track;
  uint32            tlong;

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

  /* Save status that may persist through a restart */
  tlong = midi_ctl->midi_status & ( AUDFMT_MASK_SAF_OBJECTS_DONE |
                                    AUDFMT_MASK_WAIT_RESUME      |
                                    AUDFMT_MASK_REPEAT_ENABLE    |
                                    AUDFMT_MASK_PAUSED           |
                                    AUDFMT_MASK_DLS_ENABLED      |
                                    AUDFMT_MASK_FIND_CUEP_END    |
                                    AUDFMT_MASK_FIND_POLY_CMD    |
                                    AUDFMT_MASK_MULTISEQ_MODE    |
                                    AUDFMT_MASK_PRO_FOUND        |
                                    AUDFMT_MASK_MSPI_FOUND       |
                                    AUDFMT_MASK_CUEP_PLAY        |
                                    AUDFMT_MASK_CB_STORE_DONE    );

  if(!clear_seek) {
    tlong |= (midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE);
    if(midi_ctl->accumulated_ms != 0) {
      midi_ctl->play_time_ms += audfmt_tempo_adjust_up(midi_ctl->tempo_factor,
                                                      midi_ctl->accumulated_ms);
    }
  } else {
    midi_ctl->play_time_ms         = 0;
    midi_ctl->repeat_ms            = 0;
    midi_ctl->repeat_count         = midi_ctl->repeat_orig;
  }

  midi_ctl->midi_status = tlong | ( AUDFMT_MASK_ENABLED        |
                                    AUDFMT_MASK_PARSING_HEADER |
                                    AUDFMT_MASK_RESTARTING     );
                          
  midi_ctl->midi_status2        &= (AUDFMT_MASK_PAUSE_CMD_PENDING   |
                                    AUDFMT_MASK_VOL_CMD_PENDING     |
                                    AUDFMT_MASK_GRAPH_CHUNK_ENABLED |
                                    AUDFMT_MASK_SMAF_MA2            |
                                    AUDFMT_MASK_SMAF_FM16           |
                                    AUDFMT_MASK_SMAF_DEFAULT_HQ     |
                                    AUDFMT_MASK_SMAF_PACKED_DLS_WF  |
                                    AUDFMT_MASK_PCPI_ENABLED);

  midi_ctl->elapsed_ms           = 0;
  midi_ctl->elapsed_tick         = 0;
  midi_ctl->accumulated_ms       = 0;
  midi_ctl->play_tick            = 0;
  midi_ctl->delta_tick           = 0;
  midi_ctl->delta_time           = 0;
  midi_ctl->seek_elapsed_tick    = 0;
  midi_ctl->current_header_track = 0;
  midi_ctl->tempo_factor         = TEMPO_MULT_FACTOR;

  midi_ctl->cmd_buf.num_words      = 0;
  midi_ctl->cmd_buf.num_commands   = 0;
  audfmt_add_global_command(midi_ctl, (uint8 *) audfmt_seq_reset,
                            sizeof(audfmt_seq_reset));

  if(midi_ctl->tune_factor != AUDFMT_TUNE_DEFAULT) {
    audfmt_set_tune_cmd(midi_ctl->tune_factor, midi_ctl);
  }
  if(midi_ctl->pan_factor != AUDFMT_PAN_DEFAULT)
  {
    audfmt_set_pan_cmd(midi_ctl->pan_factor, midi_ctl);
  }

  /* Free all allocated buffers */
  track = midi_ctl->tracks;
  for(tlong = 0; tlong < midi_ctl->num_tracks; tlong ++) {
    audfmt_do_play_cb(track->audfmt_handle, SND_CMX_AF_FREE_BUF,
                      0, (void *) track->track_buf);
    audfmt_do_play_cb(track->audfmt_handle, SND_CMX_AF_FREE_BUF,
                      0, (void *) track->track_buf_reserve);
    if(track->track_status & AUDFMT_TRACK_MASK_PENDING_BUF) {
      track->skip_buf_count++;
    }
    track++;
  }

  switch(midi_ctl->type) {
    case AUDFMT_MIDI_MIDI_TYPE:
      midi_ctl->tempo            = AUDFMT_MIDI_DEFAULT_TEMPO;
      midi_ctl->tempo_pending    = AUDFMT_MIDI_DEFAULT_TEMPO;
      midi_ctl->calc_ms_of       = AUDFMT_MAX_32BIT / AUDFMT_MIDI_DEFAULT_TEMPO;

      memset((uint8 *)(midi_ctl->bank_msb), 0,
                                      sizeof(uint8) * AUDFMT_MAX_CHANNELS);
      memset((uint8 *)(midi_ctl->bank_lsb), 0,
                                      sizeof(uint8) * AUDFMT_MAX_CHANNELS);
      for(tlong = 0; tlong < AUDFMT_MAX_CHANNELS; tlong++) {
        midi_ctl->bank_status[tlong] = AUDFMT_BANK_NORMAL;
      }
      midi_ctl->bank_status[9] = AUDFMT_BANK_DRUM;
      midi_ctl->drum_program   = 0;
#ifdef FEATURE_WEBAUDIO
      audfmt_reset_hq_bank(midi_ctl);
#endif /* FEATURE_WEBAUDIO */
      audmain_get_header_data(midi_ctl->handle,
                              midi_ctl->file_offset + MIDI_HEADER_BYTES,
                              &(midi_ctl->tracks[0].track_buf));
      break;

#ifdef FEATURE_WEBAUDIO
    case AUDFMT_MIDI_CMID_TYPE:
    case AUDFMT_MIDI_MELO_TYPE:
      audfmt_reset_adec_channels(midi_ctl, FALSE);
      if(midi_ctl->cb_store != NULL) {
        audfmt_cb_store_reset(midi_ctl->cb_store);
        midi_ctl->cb_store->used = TRUE;
      }

      if(midi_ctl->vib_ctl != NULL) {
        audfmt_cmid_vib_ctl.vib_ctl   = NULL;
        audfmt_cmid_vib_ctl.index     = 0;
        audfmt_cmid_vib_ctl.time_ms   = 0;
        audfmt_cmid_vib_ctl.vib_pend  = 0;
      }

      midi_ctl->cmid_tempo       = 125;
      midi_ctl->cmid_timebase    = 3;
      midi_ctl->cmid_master_vol  = 100;
      midi_ctl->tempo            = AUDFMT_CMID_DEFAULT_TEMPO;
      midi_ctl->tempo_pending    = AUDFMT_CMID_DEFAULT_TEMPO;
      midi_ctl->calc_tick_of     = AUDFMT_MAX_32BIT / AUDFMT_CMID_DEFAULT_TEMPO;

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
      {
        audfmt_reset_cmid_bank(midi_ctl);
        for(tlong = 0; tlong < AUDFMT_MAX_CHANNELS; tlong++) {
          midi_ctl->bank_status[tlong] = AUDFMT_BANK_LEGACY;
        }
      }
      midi_ctl->note_off_buffer.num = 0;
      for(tlong = 0; tlong < AUDFMT_MAX_CHANNELS; tlong++) {
        midi_ctl->channels[tlong]    = tlong;
      }
      memset(midi_ctl->bank_msb, 0, sizeof(uint8) * AUDFMT_MAX_CHANNELS);
      memset(midi_ctl->program, 0, sizeof(uint8) * AUDFMT_MAX_CHANNELS);
      memset(midi_ctl->chan_vol, 63, sizeof(uint8) * AUDFMT_MAX_CHANNELS);
      memset((uint8 *)(midi_ctl->jump_info), 0,
                        AUDFMT_MAX_CMID_JUMP * sizeof(audfmt_jump_struct_type));
      midi_ctl->cuep_repeat = midi_ctl->cuep_repeat_orig;
      memset((uint8 *)(midi_ctl->adec_status), 0, 
                                     sizeof(audfmt_adec_status_type) * 4);
      memset((uint8 *)(midi_ctl->wav_store), 0,
             sizeof(audfmt_ext_store_type) * CMID_EXT_MAX_BUFFER);
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
      {
        for(tlong = 0; tlong < CMID_MAX_WAV_CHAN; tlong++) {
          midi_ctl->wav_chan_vol[tlong] = ADEC_CONTENT_VOL(
                                      CMID_VOL_TO_DSP(CMID_DEFAULT_WAV_VOLUME),
                                      midi_ctl->adec_chan_vol[tlong]);
          midi_ctl->wav_chan_pan[tlong] =
                                      CMID_PAN_TO_DSP(CMID_DEFAULT_WAV_PANPOT);
        }
      }

      /* Set all channels to be enabled by default */
      memset((uint8 *)(midi_ctl->channel_active), 1,
                                       sizeof(boolean) * AUDFMT_MAX_CHANNELS);
      memset((uint8 *)(midi_ctl->channel_index_active), 1,
                                       sizeof(boolean) * AUDFMT_MAX_CHANNELS);

      audmain_get_header_data(midi_ctl->handle,
                              midi_ctl->track_start[0] - 8,
                              &(midi_ctl->tracks[0].track_buf));
      break;

#ifdef FEATURE_SMAF
    case AUDFMT_MIDI_SMAF_TYPE:
      audfmt_reset_adec_channels(midi_ctl, FALSE);

      /* Clear remains stored callbacks */
      if(midi_ctl->cb_store != NULL) {
        audfmt_cb_store_reset(midi_ctl->cb_store);
        midi_ctl->cb_store->used = TRUE;
      }

      midi_ctl->cmid_master_vol     = 127;
      midi_ctl->note_off_buffer.num = 0;
      memset((uint8 *)(midi_ctl->bank_msb), 0,
                                    sizeof(uint8) * AUDFMT_MAX_CHANNELS);
      memset((uint8 *)(midi_ctl->bank_lsb), 0,
                                    sizeof(uint8) * AUDFMT_MAX_CHANNELS);
      for(tlong = 0; tlong < AUDFMT_MAX_CHANNELS; tlong++) {
        midi_ctl->bank_status[tlong] = AUDFMT_BANK_NORMAL;
      }
      if(!(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_MA2)) {
        midi_ctl->bank_status[9] = AUDFMT_BANK_DRUM;
        midi_ctl->bank_msb[9]    = 125;
      } else {
        memset(midi_ctl->smaf_program_change, 0x80,
                                      sizeof(uint8) * AUDFMT_MAX_CHANNELS);
      }

      for(tlong = 0; tlong < CMID_EXT_MAX_BUFFER; tlong++) {
        midi_ctl->wav_spec[tlong].mode_3d = FALSE;
      }

      for(tlong = 0; tlong < AUDFMT_MAX_CHANNELS; tlong++) {
        midi_ctl->smaf_ma2_pitch[tlong].note    = 0x80;
        midi_ctl->smaf_ma2_pitch[tlong].c_value = 0x80;
        midi_ctl->smaf_ma2_pitch[tlong].b_value = 0x100;
      }
      memset((uint8 *)(midi_ctl->smaf_velocity), SMAF_HP_NOTE_DEFAULT_VEL,
                                          sizeof(uint8) * AUDFMT_MAX_CHANNELS);
      memset((uint8 *)(midi_ctl->adec_status), 0, 
                                     sizeof(audfmt_adec_status_type) * 4);
      memset(midi_ctl->chan_vol, 64, sizeof(uint8) * AUDFMT_MAX_CHANNELS);
      memset(midi_ctl->smaf_octave_shift, 0,
                                      sizeof(uint8) * AUDFMT_MAX_CHANNELS);
      midi_ctl->tracks[0].compressed_bits     = midi_ctl->smaf_start_comp_bits;
      midi_ctl->tracks[0].num_compressed_bits =
                                           midi_ctl->smaf_start_num_comp_bits;

      if(midi_ctl->midi_status2 & AUDFMT_MASK_SMAF_DEFAULT_HQ) {
        audfmt_force_hq_bank(midi_ctl);
      } else {
        audfmt_reset_hq_bank(midi_ctl);
      }

      audmain_get_header_data(midi_ctl->handle,
                              midi_ctl->track_start[0],
                              &(midi_ctl->tracks[0].track_buf));
      break;
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */
  }

  if(clear_seek) {
    midi_ctl->seek_ms   = 0;
    midi_ctl->seek_tick = 0;
  } else {
    if(midi_ctl->seek_ms != 0) {
      audfmt_calc_tick(midi_ctl, midi_ctl->seek_ms, &midi_ctl->seek_tick);
    } else {
      midi_ctl->seek_tick = 0;
    }
  }
} /* end audfmt_restart */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_end_sequence

DESCRIPTION
  This function ends the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void audfmt_end_sequence (
  audfmt_parse_type    *midi_ctl,
  snd_af_data_cmd_enum command
) {
  uint8 buf[4];
#ifdef FEATURE_WEBAUDIO
  audfmt_prog_def_in_use_type prog_def_reserve, prog_def_in_use;
  audfmt_waveform_param_type  *wave_param_ptr;
  audfmt_param_def_type       *param_ptr;
  uint32                      counter;
#ifdef FEATURE_AUDIO_CMX_5P0
  uint8                       *address;
#else
  uint32                      address;
#endif /* FEATURE_AUDIO_CMX_5P0 */
#endif /* FEATURE_WEBAUDIO */

  /* Verify that other sequences are not waiting for this sequence to
  ** finish seeking; otherwise kick off other sequences.
  */
  if(midi_ctl->midi_status & AUDFMT_MASK_WAIT_RESUME) {
    midi_ctl->midi_status |= AUDFMT_MASK_DONE_PENDING;
    midi_ctl->midi_status &= ~AUDFMT_MASK_PARSING_HEADER;
    audfmt_check_seek_resume(midi_ctl);
  }

  if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {
    buf[0] = MIDI_CONTROL_CHANGE;
    buf[1] = MIDI_ALL_CHANNELS;
    buf[2] = MIDI_ALL_NOTE_OFF_COMMAND;
    buf[3] = 0;
    audfmt_add_global_command(midi_ctl, buf, 4);

#ifdef FEATURE_WEBAUDIO
    audfmt_reset_adec_channels(midi_ctl, TRUE);
    if(midi_ctl->cb_store != NULL) {
      midi_ctl->cb_store->used = FALSE;
    }
    if(midi_ctl->vib_ctl != NULL) {
      midi_ctl->vib_ctl->used = FALSE;
    }

    prog_def_reserve = audfmt_prog_def_seq[midi_ctl->sequence];
    prog_def_in_use  = audfmt_prog_def_in_use_seq[midi_ctl->sequence];
    param_ptr        = audfmt_dls_struct.param_def;
    wave_param_ptr   = audfmt_dls_struct.wave_param;
    for(counter = 0; counter < 128; counter++) {
      if((param_ptr->in_use_flag == prog_def_reserve) ||
         (param_ptr->in_use_flag == prog_def_in_use)) {
        param_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
      }
      if((wave_param_ptr->in_use_flag == prog_def_reserve) ||
         (wave_param_ptr->in_use_flag == prog_def_in_use)) {
        wave_param_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
#ifdef FEATURE_AUDIO_CMX_5P0
		address = audfmt_dls_struct.wave_info[counter].wave_addr;
		audfmt_dls_struct.wave_info[counter].wave_addr = NULL;
		audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_DLS,
						  0, address);
#else
        address = (audfmt_dls_struct.wave_info[counter].wave_addr_msw<<16) |
                  audfmt_dls_struct.wave_info[counter].wave_addr_lsw;
        audfmt_dls_struct.wave_info[counter].wave_addr_msw = 0;
        audfmt_dls_struct.wave_info[counter].wave_addr_lsw = 0;
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FREE_DLS,
                          0, (void *) address);
#endif /* FEATURE_AUDIO_CMX_5P0 */
      }
      param_ptr++;
      wave_param_ptr++;
    }

#ifdef FEATURE_DLS
    if(midi_ctl->midi_status & AUDFMT_MASK_DLS_ENABLED) {
      audfmt_unload_offset_dls(midi_ctl->handle, midi_ctl->sequence);
    }
#endif /* FEATURE_DLS */
#endif /* FEATURE_WEBAUDIO */

    audfmt_do_pause_cmd(midi_ctl, FALSE);
    audmain_audfmt_set_vol_seq(midi_ctl->sequence, AUDFMT_VOL_DEFAULT);
  }

  midi_ctl->midi_status = 0;

  if(command == SND_CMX_AF_ABORT){
    if(midi_ctl->error_status == SND_SUCCESS){
      MSG_ERROR("Abort without error status being set", 0,0,0);
      midi_ctl->error_status = SND_ABORT;
    }
    audfmt_do_play_cb(midi_ctl->handle, command, 0, &midi_ctl->error_status);
  } else {
  audfmt_do_play_cb(midi_ctl->handle, command, 0, NULL);
  }
#ifdef FEATURE_SMAF
 smaf_clean_heap((audfmt_parse_type*) midi_ctl);   	
#endif
} /* end audfmt_end_sequence */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_check_reset_calc_time

DESCRIPTION
  This function sends a reset command if there are no active sequences.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_check_reset_calc_time ( void )
{
  audfmt_parse_type *midi_ctl;
  uint32            sequence = AUDFMT_NUM_SEQUENCES;

  midi_ctl = audfmt_parse_ctl;
  while(sequence != 0) {
    if(midi_ctl->midi_status & AUDFMT_MASK_ENABLED) {
      break;
    }
    sequence--;
    midi_ctl++;
  }

  if(sequence == 0) {
    audmain_audfmt_check_reset();
  }
} /* end audfmt_check_reset_calc_time */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_parse

DESCRIPTION
  This function parses one time event of a the specified file type.

DEPENDENCIES
  None

RETURN VALUE
  audmain_status_type
  time_remain - time, in ms, until next event.

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_parse (
  audmain_parse_ctl_type   *parse_ctl,
  audmain_cmd_buf_type     *cmd_buf,
  uint32                   *time_remain
) {
  audmain_status_type       audfmt_status = AUDMAIN_STATUS_DONE;
  audmain_midi_cmd_buf_type *midi_cmd;
  audfmt_parse_type         *midi_ctl;
  audfmt_parse_type         *preparse_ctl = NULL;
  uint32                    counter, adjusted_repeat;
#ifdef FEATURE_WEBAUDIO
  uint32                    tlong;
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
#endif /* FEATURE_WEBAUDIO */

  if(parse_ctl != NULL) {
    midi_ctl     = (audfmt_parse_type *) parse_ctl->tracks;
    *time_remain = AUDFMT_MAX_32BIT;
#ifdef FEATURE_WEBAUDIO
    cmd_buf->vib = FALSE;
#endif /* FEATURE_WEBAUDIO */

    midi_cmd = &(cmd_buf->midi_cmd[0]);
    if((audfmt_global_cmd.num_commands != 0) &&
#ifdef FEATURE_ANIMATED_AAC
       (audmain_get_adec_mode() != AUDMAIN_ADEC_MODE_AAC) &&
#endif /* FEATURE_ANIMATED_AAC */
       ((midi_cmd->num_words + audfmt_global_cmd.num_words) <=
        AUDMAIN_CMD_BUF_MAX_WORDS)) {
      /* Issue commands */
      memcpy( (uint8 *) &midi_cmd->command_buf[midi_cmd->num_words],
              (uint8 *) audfmt_global_cmd.command_buf,
              audfmt_global_cmd.num_words * 2
            );
      midi_cmd->num_words    += audfmt_global_cmd.num_words;
      midi_cmd->num_commands += audfmt_global_cmd.num_commands;

      audfmt_global_cmd.num_words    = 0;
      audfmt_global_cmd.num_commands = 0;
    }

    /* Parse through all sequences */
    for(counter = 0; counter < AUDFMT_NUM_SEQUENCES; counter++) {
      if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
         !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_GET_SPEC))) {
        if(midi_ctl->midi_status & AUDFMT_MASK_ERROR) {
          audfmt_end_sequence(midi_ctl, SND_CMX_AF_ABORT);
        } else if((midi_ctl->midi_status & AUDFMT_MASK_PARSING_HEADER) ||
                  ((midi_ctl->midi_status & (AUDFMT_MASK_PAUSED |
                                             AUDFMT_MASK_SEEK_PAUSED)) &&
                   !(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE))) {
          /* If currently parsing header or paused and not seeking, set
          ** return value to not return DONE status
          */
          audfmt_status = AUDMAIN_STATUS_SUCCESS;
        } else {
          if((cmd_buf->start_aging) && 
             !(midi_ctl->midi_status & AUDFMT_MASK_INIT_DONE)){
            midi_ctl->midi_status |= AUDFMT_MASK_INIT_DONE;
            if(!(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE)){
              /* donot send SND_CMX_AF_STARTED during seek operation*/ 
              audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_STARTED,
                                                              0, NULL);
            }
          }
          /* If pause command is pending, execute */
          if(midi_ctl->midi_status2 & AUDFMT_MASK_PAUSE_CMD_PENDING) {
            audfmt_do_pause_cmd(midi_ctl, TRUE);
          }
          if(midi_ctl->midi_status2 & AUDFMT_MASK_VOL_CMD_PENDING) {
            if(audmain_audfmt_set_vol_seq(midi_ctl->sequence,
                                          midi_ctl->vol_factor) ==
               AUDMAIN_STATUS_SUCCESS) {
              midi_ctl->midi_status2 &= ~AUDFMT_MASK_VOL_CMD_PENDING;
            }
          }
          if(midi_ctl->repeat_ms != 0) {
            adjusted_repeat = audfmt_tempo_adjust_down(midi_ctl->tempo_factor,
                                                       midi_ctl->repeat_ms);
            if(adjusted_repeat < *time_remain) {
              *time_remain = adjusted_repeat;
            }
          } else if(midi_ctl->midi_status & AUDFMT_MASK_PARSE_DONE) {
            /* If current sequence is done with parsing current event and
            ** detal time has expired, then issue commands.
            */
            if(midi_ctl->delta_time == 0) {
              midi_cmd = &(cmd_buf->midi_cmd[counter]);
              if((midi_cmd->num_words + midi_ctl->cmd_buf.num_words) <=
                 AUDMAIN_CMD_BUF_MAX_WORDS) {
                midi_ctl->failed_cmd_cnt = 0;
#ifdef FEATURE_WEBAUDIO
                /* If jump command is pending, execute */
                if(midi_ctl->midi_status2 & AUDFMT_MASK_JUMP_PENDING) {
                  audfmt_reset_adec_channels(midi_ctl, FALSE);
                  midi_ctl->midi_status2 &= ~AUDFMT_MASK_JUMP_PENDING;
                  audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_JUMP_POINT_END,
                                    0, NULL);
                }
                /* If cue point command is pending, execute */
                if(midi_ctl->midi_status2 & AUDFMT_MASK_CUEP_PENDING) {
                  audfmt_reset_adec_channels(midi_ctl, FALSE);
                  midi_ctl->midi_status2 &= ~AUDFMT_MASK_CUEP_PENDING;
                  audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_CUE_POINT_END,
                                    0, NULL);
                }
                /* If pause command is pending, execute */
                if(midi_ctl->midi_status2 & AUDFMT_MASK_PAUSE_PENDING) {
                  midi_ctl->midi_status2 &= ~AUDFMT_MASK_PAUSE_PENDING;
                  midi_ctl->midi_status |= AUDFMT_MASK_PAUSED;
                  audfmt_do_pause_cmd(midi_ctl, TRUE);

                  tlong = midi_ctl->play_time_ms +
                          audfmt_tempo_adjust_up(midi_ctl->tempo_factor,
                                                 midi_ctl->accumulated_ms);

                  audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_PAUSE,
                                    tlong, NULL);
                }
                /* Execute any pending callbacks for this time event */
                if (audfmt_do_stored_cb(midi_ctl) != AUDFMT_STATUS_SUCCESS) {
				    /* failed because DSP image was not ready */
					MSG_ERROR("audfmt_do_stored_cb() Failed",0,0,0);
					/* ensure that pending callbacks are tried again asap */
					midi_ctl->midi_status &= ~AUDFMT_MASK_INIT_DONE;
					*time_remain = 0;
				}

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
#endif /* FEATURE_WEBAUDIO */

#ifdef FEATURE_ANIMATED_AAC
                if( audmain_get_adec_mode() != AUDMAIN_ADEC_MODE_AAC)
#endif /* FEATURE_ANIMATED_AAC */
                {
                  /* Issue commands */
                  memcpy( (uint8 *) &midi_cmd->command_buf[midi_cmd->num_words],
                          (uint8 *) midi_ctl->cmd_buf.command_buf,
                          midi_ctl->cmd_buf.num_words * 2
                        );
                  midi_cmd->num_words    += midi_ctl->cmd_buf.num_words;
                  midi_cmd->num_commands += midi_ctl->cmd_buf.num_commands;
                }

                midi_ctl->cmd_buf.num_words    = 0;
                midi_ctl->cmd_buf.num_commands = 0;

                if(midi_ctl->midi_status & AUDFMT_MASK_DONE_PENDING) {
                  audfmt_end_sequence(midi_ctl, SND_CMX_AF_PLAY_DONE);
                } else if(midi_ctl->midi_status & AUDFMT_MASK_REPEAT_PENDING) {
                  midi_ctl->repeat_ms = midi_ctl->interval_ms;
                  audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_REPEAT,
                                    0, NULL);
                  audfmt_restart(midi_ctl, FALSE);
                } else if(midi_ctl->midi_status & (AUDFMT_MASK_INIT_DONE |
                                                   AUDFMT_MASK_SEEK_ACTIVE)) {
                  /* Set parse pending status to parse through next events */
                  midi_ctl->midi_status &= ~AUDFMT_MASK_PARSE_DONE;
                  midi_ctl->midi_status |= AUDFMT_MASK_PARSE_PENDING;
                  preparse_ctl = midi_ctl;
                } else {
                  *time_remain = 20;
                }
              } else {
                if(midi_ctl->failed_cmd_cnt >= 1000) {
                  midi_ctl->error_status = SND_ABORT_RESOURCE_ERR;
                  audfmt_end_sequence(midi_ctl, SND_CMX_AF_ABORT);
                } else {
                  midi_ctl->failed_cmd_cnt++;
                }

                /* Command queue is full, set time to minimum in order to
                ** come back ASAP and process pending events.
                */
                *time_remain = 0;
                if(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE) {
                   if(midi_ctl->midi_status & AUDFMT_MASK_INIT_DONE) {
                  /* Set parse pending status to parse through next events */
                  midi_ctl->midi_status &= ~AUDFMT_MASK_PARSE_DONE;
                  midi_ctl->midi_status |= AUDFMT_MASK_PARSE_PENDING;
                  preparse_ctl = midi_ctl;
                 } else {
                    *time_remain = 50 ; 
                 } 
                }
              }
            } else if(midi_ctl->delta_time < *time_remain) {
              *time_remain = midi_ctl->delta_time;
            }
          }
#ifdef FEATURE_WEBAUDIO
          /* Process VIB command */
          if(!(midi_ctl->midi_status & (AUDFMT_MASK_SEEK_ACTIVE |
                                        AUDFMT_MASK_RESUME_PENDING)) &&
             (midi_ctl->vib_ctl != NULL)) {
            cmd_buf->vib = cmid_check_vib(midi_ctl->vib_ctl);
            if((cmid_vib_time_ms(midi_ctl->vib_ctl, &tlong) ==
               AUDFMT_STATUS_SUCCESS) && (tlong < *time_remain)) {
              *time_remain = tlong;
            }
          }
#endif /* FEATURE_WEBAUDIO */
          if(midi_ctl->midi_status & AUDFMT_MASK_ENABLED) {
            audfmt_status = AUDMAIN_STATUS_SUCCESS;
          }
        }
      }
      midi_ctl++;
    }

    /* If a preparse command needs to be issued, issue command */
    if((preparse_ctl != NULL) && !preparse_sent) {
      preparse_sent = TRUE;
      audmain_do_preparse(preparse_ctl->handle);
    }
  }
  return(audfmt_status);
} /* end audfmt_parse */

/* <EJECT> */
#ifdef FEATURE_SMAF
/*===========================================================================

FUNCTION audfmt_get_track_offset

DESCRIPTION
  This function gets the current byte offset into the indicated track.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static uint32 audfmt_get_track_offset (
  audfmt_parse_type *midi_ctl,
  audfmt_track_type *track_ptr,
  uint32            track_num
) {
  uint32 ret_val = 0;

  switch(midi_ctl->type) {
    case AUDFMT_MIDI_MIDI_TYPE:
    case AUDFMT_MIDI_CMID_TYPE:
    case AUDFMT_MIDI_MELO_TYPE:
    case AUDFMT_MIDI_WASF_TYPE:
    default:
      break;

    case AUDFMT_MIDI_SMAF_TYPE:
      switch(midi_ctl->smaf_type) {
        case AUDFMT_SMAF_HP_TYPE:
        case AUDFMT_SMAF_PHRASE_TYPE:
        default:
          if(track_num < (AUDFMT_MAX_CMID_TRACKS + 1)) {
            ret_val = track_ptr->current_location -
                                     midi_ctl->track_start[track_num];
          }
          break;

        case AUDFMT_SMAF_COMPRESSED_TYPE:
        case AUDFMT_SMAF_MOBILE_TYPE:
          ret_val = track_ptr->current_offset;
          break;
      }
      break;
  }

  return(ret_val);
}
#endif /* FEATURE_SMAF */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_parse_single

DESCRIPTION
  This function parses one time event of the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static boolean audfmt_parse_single (
  audfmt_parse_type *midi_ctl
) {
  audfmt_status_type status;
  audfmt_track_type  *track_ptr;
  uint32             min_tick, counter;
  boolean            ret_val = FALSE, seek_done = FALSE;

#ifdef FEATURE_WEBAUDIO
  audfmt_jump_struct_type  *jump_ptr;
#endif /* FEATURE_WEBAUDIO */

  if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) && 
     (midi_ctl->midi_status & AUDFMT_MASK_PARSE_PENDING) &&
     !(midi_ctl->midi_status & AUDFMT_MASK_PENDING_BUF)) {
    ret_val = TRUE;
    do {
      /* First process any time in repeat_ms if seeking */
      if((midi_ctl->repeat_ms != 0) && 
         (midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE)) {
        if(midi_ctl->repeat_ms < midi_ctl->seek_ms) {
          midi_ctl->seek_ms      -= midi_ctl->repeat_ms;
          midi_ctl->play_time_ms += midi_ctl->repeat_ms;
          midi_ctl->repeat_ms     = 0;
          audfmt_calc_tick(midi_ctl, midi_ctl->seek_ms, &midi_ctl->seek_tick);
        } else {
          midi_ctl->play_time_ms += midi_ctl->seek_ms;
          midi_ctl->repeat_ms    -= midi_ctl->seek_ms;
          midi_ctl->seek_ms       = 0;
          midi_ctl->seek_tick     = 0;
        }
      }

      /* If this sequence was not previously delayed for some reason, then
      ** determine smallest delta time from all its tracks.
      */
      if(!(midi_ctl->midi_status & AUDFMT_MASK_PARSE_DELAYED) &&
         (midi_ctl->delta_time == 0)) {
        /* Find track with lowest tick value */
        min_tick  = AUDFMT_MAX_32BIT;
        track_ptr = midi_ctl->tracks;
        counter   = midi_ctl->num_tracks;
        while(counter-- != 0) {
          if(track_ptr->audfmt_handle != NULL) {
            min_tick = MIN(min_tick, track_ptr->delta_tick);
            if(min_tick == 0) {
              break;
            }
          }
          track_ptr++;
        }

#ifdef FEATURE_WEBAUDIO
        /* Need to also take into account note off buffer when finding 
        ** smallest tick value.
        */
        if(min_tick != 0) {
          if(cmid_note_off_delta_time(midi_ctl, &counter) ==
             AUDFMT_STATUS_SUCCESS) {
            min_tick = MIN(min_tick, counter);
          }
        }
#endif /* FEATURE_WEBAUDIO */

        /* If seek is active, need to determine if this time event will
        ** expire the seek time.
        */
        if((midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE) &&
           (!(midi_ctl->midi_status & (AUDFMT_MASK_SILENCE_SKIP |
                                       AUDFMT_MASK_INITIAL_SKIP |
                                       AUDFMT_MASK_OFFSET_SKIP  |
                                       AUDFMT_MASK_FIND_CUEP_END)) ||
            (midi_ctl->seek_ms == 0))) {
#ifdef FEATURE_AUDIO_CMX_5P0
          uint8 counter = 0, buf[4] = {0};
#endif /* FEATURE_AUDIO_CMX_5P0 */
          /* Check for seek done status */
          if(midi_ctl->seek_tick <= (min_tick + midi_ctl->seek_elapsed_tick)) {
            if(midi_ctl->seek_tick > midi_ctl->seek_elapsed_tick) {
              min_tick = (midi_ctl->seek_tick - midi_ctl->seek_elapsed_tick);
            } else {
              min_tick = 0;
            }
            midi_ctl->play_time_ms      += midi_ctl->seek_ms;
            midi_ctl->seek_ms            = 0;
            midi_ctl->seek_tick          = 0;
            midi_ctl->seek_elapsed_tick  = 0;
            midi_ctl->elapsed_tick       = 0;
            midi_ctl->elapsed_ms         = 0;
            midi_ctl->midi_status       &= ~(AUDFMT_MASK_SEEK_ACTIVE |
                                             AUDFMT_MASK_PARSE_PENDING |
                                             AUDFMT_MASK_STARTED_AGE);
            midi_ctl->midi_status       |= AUDFMT_MASK_PARSE_DONE;
#ifdef FEATURE_AUDIO_CMX_5P0
            if(midi_ctl->type == AUDFMT_MIDI_MIDI_TYPE) {
              /* send the last valid pitch, vol, pan cmd for each channel
               * if pending to be sent
               */
              for( counter = 0; counter < AUDFMT_MAX_CHANNELS; counter++ )
              {
                if( midi_ctl->midi_pitch_is_changed[counter] == TRUE )
                {
                  buf[0] = MIDI_PITCH_WHEEL;
                  buf[1] = counter;
                  buf[2] = midi_ctl->midi_ch_pitch_param[counter]>>8;
                  buf[3] = midi_ctl->midi_ch_pitch_param[counter] & 0x00FF;

                  audfmt_add_command(midi_ctl, buf,
                                     AUDFMT_COMMAND_SIZE_4);
                  midi_ctl->midi_pitch_is_changed[counter] = FALSE;
                }

                if( midi_ctl->midi_vol_is_changed[counter] == TRUE )
                {
                  buf[0] = MIDI_CONTROL_CHANGE;
                  buf[1] = counter;
                  buf[2] = MIDI_VOLUME_COMMAND;
                  buf[3] = midi_ctl->midi_ch_vol_param[counter];

                  audfmt_add_command(midi_ctl, buf,
                                     AUDFMT_COMMAND_SIZE_4);
                  midi_ctl->midi_vol_is_changed[counter] = FALSE;
                }

                if( midi_ctl->midi_pan_is_changed[counter] == TRUE )
                {
                  buf[0] = MIDI_CONTROL_CHANGE;
                  buf[1] = counter;
                  buf[2] = MIDI_PANPOT_COMMAND;
                  buf[3] = midi_ctl->midi_ch_pan_param[counter];

                  audfmt_add_command(midi_ctl, buf,
                                     AUDFMT_COMMAND_SIZE_4);
                  midi_ctl->midi_pan_is_changed[counter] = FALSE;
                }
              }
            }  
#endif /* FEATURE_AUDIO_CMX_5P0 */
            audfmt_adjust_tempo_factor(midi_ctl);

            audfmt_check_seek_resume(midi_ctl);
            seek_done = TRUE;
          }
        }

        /* If no minimum time was found within all tracks, then it must mean
        ** file is done playing.
        */
        if(min_tick == AUDFMT_MAX_32BIT) {
          midi_ctl->midi_status &= ~AUDFMT_MASK_PARSE_PENDING;
          midi_ctl->midi_status |= AUDFMT_MASK_PARSE_DONE |
                                                AUDFMT_MASK_DONE_PENDING;
          return(TRUE);
        } else if(min_tick > 0) {
#ifdef FEATURE_SMAF
          if(midi_ctl->midi_status & AUDFMT_MASK_INITIAL_SKIP) {
            if(midi_ctl->play_tick <= midi_ctl->smaf_start_tick) {
              counter = midi_ctl->smaf_start_tick - midi_ctl->play_tick;
              if(counter <= min_tick) {
                min_tick = counter;
              }
            } else {
              min_tick = 0;
            }
          } else if(midi_ctl->midi_status & AUDFMT_MASK_PRO_FOUND) {
            if(midi_ctl->play_tick <= midi_ctl->smaf_end_tick) {
              min_tick = MIN(midi_ctl->smaf_end_tick - midi_ctl->play_tick,
                             min_tick);
            }
          }
          midi_ctl->play_tick += min_tick;
#endif /* FEATURE_SMAF */

          /* Adjust each track by min_tick */
          track_ptr = midi_ctl->tracks;
          counter   = midi_ctl->num_tracks;
          while(counter-- != 0) {
            if(track_ptr->audfmt_handle != NULL) {
              track_ptr->delta_tick -= min_tick;
              if(track_ptr->delta_tick == 0) {
                track_ptr->track_status |= AUDFMT_TRACK_MASK_CMD_PEND;
              }
            }
            track_ptr++;
          }
#ifdef FEATURE_WEBAUDIO
          cmid_note_off_age(midi_ctl, min_tick);
#endif /* FEATURE_WEBAUDIO */
        }

        if(seek_done) {
          return(TRUE);
        }

        /* Update time until next event for this file */
        if(midi_ctl->midi_status & (AUDFMT_MASK_SILENCE_SKIP |
                                    AUDFMT_MASK_INITIAL_SKIP |
                                    AUDFMT_MASK_OFFSET_SKIP  |
                                    AUDFMT_MASK_FIND_CUEP_END)) {
          midi_ctl->delta_tick = 0;
        } else {
          midi_ctl->delta_tick = min_tick;
        }

        if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                      AUDFMT_MASK_SEEK_ACTIVE))) {
          if(midi_ctl->delta_tick != 0) {
            audfmt_calc_time_remain(midi_ctl);
          } else {
            midi_ctl->delta_time = 0;
          }
        }
      } else {
        /* Clear parse delay flag */
        midi_ctl->midi_status &= ~AUDFMT_MASK_PARSE_DELAYED;
      }

      /* If seek is active, accumulate time and check if seek time has
      ** expired.
      */ 
      if(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE) {
        if(midi_ctl->delta_tick != 0) {
          /* Check for seek done status; this situation should only occur if
          ** the seek time is smaller than the delta tick calculated from
          ** before a buffer error.
          */
          if(midi_ctl->seek_tick <= (midi_ctl->delta_tick +
                                     midi_ctl->seek_elapsed_tick)) {
            if(midi_ctl->seek_tick > midi_ctl->seek_elapsed_tick) {
              midi_ctl->delta_tick        -= (midi_ctl->seek_tick -
                                              midi_ctl->seek_elapsed_tick);
            }
            midi_ctl->play_time_ms      += midi_ctl->seek_ms;
            midi_ctl->seek_ms            = 0;
            midi_ctl->seek_tick          = 0;
            midi_ctl->seek_elapsed_tick  = 0;
            midi_ctl->elapsed_tick       = 0;
            midi_ctl->elapsed_ms         = 0;
            midi_ctl->midi_status       &= ~(AUDFMT_MASK_SEEK_ACTIVE |
                                             AUDFMT_MASK_STARTED_AGE);

            audfmt_adjust_tempo_factor(midi_ctl);

            audfmt_check_seek_resume(midi_ctl);

            if(midi_ctl->delta_tick != 0) {
              audfmt_calc_time_remain(midi_ctl);
            } else {
              midi_ctl->delta_time = 0;
            }
          } else {
            /* Accumulate elapsed time during seeking */
            midi_ctl->seek_elapsed_tick  += midi_ctl->delta_tick;
            midi_ctl->delta_tick          = 0;
            midi_ctl->delta_time          = 0;
            /* Adjust seek times to avoid overflow of tick/tme accumulation */
            while(midi_ctl->calc_ms_of <= midi_ctl->seek_elapsed_tick) {
              if(midi_ctl->calc_tick_of < midi_ctl->seek_ms) {
                midi_ctl->seek_ms           -= midi_ctl->calc_tick_of;
                midi_ctl->play_time_ms      += midi_ctl->calc_tick_of;
                midi_ctl->seek_elapsed_tick -= midi_ctl->calc_ms_of;
              } else {
                audfmt_calc_ms(midi_ctl, midi_ctl->seek_elapsed_tick,
                               &min_tick);
                midi_ctl->play_time_ms      += min_tick;
                midi_ctl->seek_ms            = 0;
                midi_ctl->seek_elapsed_tick  = 0;
              }

              if(midi_ctl->seek_ms != 0) {
                audfmt_calc_tick(midi_ctl, midi_ctl->seek_ms,
                                 &midi_ctl->seek_tick);
              } else {
                midi_ctl->seek_tick = 0;
              }
            }
            if(midi_ctl->seek_elapsed_tick > midi_ctl->seek_tick) {
              MSG_ERROR("Should never happen",0,0,0);
            }
          }
        }
      }

#ifdef FEATURE_WEBAUDIO
      /* Add note off commands to command queue and return delayed status
      ** if command queue overflows.
      */
      if(cmid_note_off_verify(midi_ctl) != AUDFMT_STATUS_SUCCESS) {
        midi_ctl->midi_status &= ~AUDFMT_MASK_PARSE_PENDING;
        midi_ctl->midi_status |= AUDFMT_MASK_PARSE_DONE |
                                 AUDFMT_MASK_PARSE_DELAYED;
        return(ret_val);
      }
#endif /* FEATURE_WEBAUDIO */

#ifdef FEATURE_SMAF
      if((midi_ctl->midi_status & AUDFMT_MASK_INITIAL_SKIP) &&
         (midi_ctl->play_tick >= midi_ctl->smaf_start_tick)) {
        midi_ctl->midi_status &= ~AUDFMT_MASK_INITIAL_SKIP;
      }

      if((midi_ctl->midi_status & AUDFMT_MASK_PRO_FOUND) &&
         (midi_ctl->play_tick >= midi_ctl->smaf_end_tick)) {

        for(counter = 0; counter < midi_ctl->num_tracks; counter++) {
          track_ptr = &midi_ctl->tracks[counter];
          if(track_ptr->audfmt_handle != NULL) {
            audfmt_do_play_cb(track_ptr->audfmt_handle, SND_CMX_AF_FREE_BUF,
                              0, (void *) track_ptr->track_buf);
            audfmt_do_play_cb(track_ptr->audfmt_handle, SND_CMX_AF_FREE_BUF,
                              0, (void *) track_ptr->track_buf_reserve);
            track_ptr->audfmt_handle = NULL;
          }
        }

        if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME) &&
           (midi_ctl->midi_status & AUDFMT_MASK_REPEAT_ENABLE)) {
          if(!(midi_ctl->midi_status & AUDFMT_MASK_REPEAT_PENDING)) {
            if(midi_ctl->repeat_count != 0) {
              midi_ctl->repeat_count--;
              if(midi_ctl->repeat_count == 0) {
                midi_ctl->midi_status |= AUDFMT_MASK_DONE_PENDING;
                break;
              }
            }
            midi_ctl->midi_status |= AUDFMT_MASK_REPEAT_PENDING;
          }
        } else {
          midi_ctl->midi_status |= AUDFMT_MASK_DONE_PENDING;
        }
      }
#endif /* FEATURE_SMAF */

      /* Parse next event for all tracks */
      for(counter = 0; counter < midi_ctl->num_tracks; counter++) {
        track_ptr = &midi_ctl->tracks[counter];
        if(track_ptr->audfmt_handle != NULL) {
          status = AUDFMT_STATUS_SUCCESS;
          while(((track_ptr->delta_tick == 0) || 
                 ((midi_ctl->type == AUDFMT_MIDI_MIDI_TYPE) &&
                  (track_ptr->track_status & AUDFMT_TRACK_MASK_CMD_PEND))) &&
                (status == AUDFMT_STATUS_SUCCESS)) {
            /* If DO_TIME flag is set, then parse tick event first before
            ** parsing for commands.
            */
            if(!(track_ptr->track_status & AUDFMT_TRACK_MASK_DO_TIME)) {
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
              (void) audfmt_check_buffer_done(midi_ctl, track_ptr);
              status = audfmt_parse_cmd(midi_ctl, counter);
            }

            switch(status) {
              case AUDFMT_STATUS_DONE:
                /* If this track is done, free buffers */
                audfmt_do_play_cb(track_ptr->audfmt_handle, SND_CMX_AF_FREE_BUF,
                                  0, (void *) track_ptr->track_buf);
                audfmt_do_play_cb(track_ptr->audfmt_handle, SND_CMX_AF_FREE_BUF,
                                  0, (void *) track_ptr->track_buf_reserve);
                track_ptr->audfmt_handle = NULL;

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

                /* If this is the last active track, the end of playback has
                ** been reached.
                */
                if(--midi_ctl->current_header_track == 0) {
                  if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME) &&
                     (midi_ctl->midi_status & AUDFMT_MASK_REPEAT_ENABLE)) {
                    if(midi_ctl->repeat_count != 0) {
                      midi_ctl->repeat_count--;
                      if(midi_ctl->repeat_count == 0) {
                        midi_ctl->midi_status |= AUDFMT_MASK_DONE_PENDING;
                        break;
                      }
                    }
                    midi_ctl->midi_status |= AUDFMT_MASK_REPEAT_PENDING;
                  } else {
                    midi_ctl->midi_status |= AUDFMT_MASK_DONE_PENDING;
                  }
                }
                break;

              case AUDFMT_STATUS_SUCCESS:
                /* Read next tick value from track */
                track_ptr->track_status |= AUDFMT_TRACK_MASK_DO_TIME;
                status = audfmt_check_buffer_done(midi_ctl, track_ptr);
#ifdef FEATURE_SMAF
                if(status == AUDFMT_STATUS_SUCCESS) {
                  if(track_ptr->track_status & AUDFMT_TRACK_MASK_OFFSET_SKIP) {
                    min_tick = audfmt_get_track_offset(midi_ctl, track_ptr,
                                                       counter);
                    if(track_ptr->start_offset == min_tick) {
                      track_ptr->track_status &= ~AUDFMT_TRACK_MASK_OFFSET_SKIP;
                      midi_ctl->midi_status   &= ~AUDFMT_MASK_OFFSET_SKIP;
                    } else if(track_ptr->start_offset < min_tick) {
                      status = AUDFMT_STATUS_ERROR;
                    }
                  } else if(midi_ctl->midi_status & AUDFMT_MASK_MSPI_FOUND) {
                    min_tick = audfmt_get_track_offset(midi_ctl, track_ptr,
                                                       counter);
                    if(track_ptr->stop_offset == min_tick) {
                      status = AUDFMT_STATUS_DONE;
                      /* If this track is done, free buffers */
                      audfmt_do_play_cb(track_ptr->audfmt_handle,
                                       SND_CMX_AF_FREE_BUF, 0,
                                       (void *) track_ptr->track_buf);
                      audfmt_do_play_cb(track_ptr->audfmt_handle,
                                       SND_CMX_AF_FREE_BUF, 0,
                                       (void *) track_ptr->track_buf_reserve);
                      track_ptr->audfmt_handle = NULL;

                      /* If this is the last active track, the end of
                      ** playback has been reached.
                      */
                      if(--midi_ctl->current_header_track == 0) {
                        if(!(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME) &&
                           (midi_ctl->midi_status &
                                                AUDFMT_MASK_REPEAT_ENABLE)) {
                          if(midi_ctl->repeat_count != 0) {
                            midi_ctl->repeat_count--;
                            if(midi_ctl->repeat_count == 0) {
                              midi_ctl->midi_status |=
                                                  AUDFMT_MASK_DONE_PENDING;
                              break;
                            }
                          }
                          midi_ctl->midi_status |= AUDFMT_MASK_REPEAT_PENDING;
                        } else {
                          midi_ctl->midi_status |= AUDFMT_MASK_DONE_PENDING;
                        }
                      }
                    } else if(track_ptr->stop_offset < min_tick) {
                      status = AUDFMT_STATUS_ERROR;
                    }
                  }
                }
#endif /* FEATURE_SMAF */

                if(status == AUDFMT_STATUS_SUCCESS) {
                  status = audfmt_read_time(midi_ctl, track_ptr, &min_tick);
                  if(status == AUDFMT_STATUS_SUCCESS) {
                    track_ptr->delta_tick += min_tick;
#ifdef FEATURE_WEBAUDIO
                    if((counter == 0) && (track_ptr->delta_tick != 0)) {
                      jump_ptr = midi_ctl->jump_info;
                      for(min_tick = 0; min_tick < AUDFMT_MAX_CMID_JUMP;
                          min_tick++) {
                        jump_ptr->same_time = FALSE;
                        jump_ptr++;
                      }
                    }
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
#endif /* FEATURE_WEBAUDIO */
                  }
                }
                break;

              case AUDFMT_STATUS_JUMP:
                /* If jump status, then process like a delay event if not
                ** doing calc time.
                */
                if(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME) {
                  midi_ctl->midi_status |= AUDFMT_MASK_PARSE_DELAYED;
                  break;
                }
              case AUDFMT_STATUS_CMD_BUFF_ERR:
              case AUDFMT_STATUS_STORE_ERR:
                midi_ctl->midi_status &= ~AUDFMT_MASK_PARSE_PENDING;
                midi_ctl->midi_status |= AUDFMT_MASK_PARSE_DONE |
                                         AUDFMT_MASK_PARSE_DELAYED;
                break;
            }

            switch(status) {
              case AUDFMT_STATUS_SUCCESS:
                /* Clear DO_TIME flag and continue parsing track if
                ** data available
                */
                track_ptr->track_status &= ~AUDFMT_TRACK_MASK_DO_TIME;
                if((status = audfmt_check_buffer_done(midi_ctl, track_ptr)) !=
                   AUDFMT_STATUS_SUCCESS) {
                  midi_ctl->midi_status   |= (AUDFMT_MASK_PARSE_DELAYED |
                                              AUDFMT_MASK_PENDING_BUF);
                  track_ptr->track_status |= AUDFMT_TRACK_MASK_BUFF_DELAY;
                  ret_val = FALSE;
                }
                break;

              case AUDFMT_STATUS_STOP:
                midi_ctl->midi_status &= ~AUDFMT_MASK_PARSE_PENDING;
                midi_ctl->midi_status |= AUDFMT_MASK_DONE_PENDING |
                                         AUDFMT_MASK_PARSE_DONE;
                break;

              case AUDFMT_STATUS_ERROR:
              case AUDFMT_STATUS_DATA_ERR:
                midi_ctl->midi_status &= ~AUDFMT_MASK_PARSE_PENDING;
                midi_ctl->midi_status |= AUDFMT_MASK_ERROR |
                                         AUDFMT_MASK_PARSE_DONE;
                if(status == AUDFMT_STATUS_ERROR){
                  midi_ctl->error_status = SND_ABORT;
                } else {
                  midi_ctl->error_status = SND_ABORT_DATA_ERR;
                }
                break;

              case AUDFMT_STATUS_BUFF_ERR:
                /* Set flags for delay status and flags to receive data
                ** buffers.
                */
                midi_ctl->midi_status     |= AUDFMT_MASK_PARSE_DELAYED;
                if(track_ptr->track_status & AUDFMT_TRACK_MASK_PENDING_BUF) {
                  midi_ctl->midi_status   |= AUDFMT_MASK_PENDING_BUF;
                  track_ptr->track_status |= AUDFMT_TRACK_MASK_BUFF_DELAY;
                  ret_val = FALSE;
                } else {
                  midi_ctl->midi_status   &= ~AUDFMT_MASK_PARSE_PENDING;
                  midi_ctl->midi_status   |= AUDFMT_MASK_PARSE_DONE;
                }
                break;
            }
          }
// should done be added to this check?
          if(midi_ctl->midi_status & (AUDFMT_MASK_ERROR |
                                      AUDFMT_MASK_PARSE_DELAYED)) {
            break;
          }
        }
      }

      /* Process events for seeking */
      if(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE) {
        /* Check if error or reached end of file during seek */
        if(midi_ctl->midi_status & (AUDFMT_MASK_ERROR |
                                    AUDFMT_MASK_DONE_PENDING)) {
          audfmt_calc_ms(midi_ctl, midi_ctl->seek_elapsed_tick, &min_tick);
          midi_ctl->play_time_ms      += min_tick;
          midi_ctl->seek_ms            = 0;
          midi_ctl->seek_tick          = 0;
          midi_ctl->seek_elapsed_tick  = 0;

          midi_ctl->midi_status &= ~AUDFMT_MASK_SEEK_ACTIVE;
          if(midi_ctl->midi_status & AUDFMT_MASK_WAIT_RESUME) {
            audfmt_check_seek_resume(midi_ctl);
          }
        } else {
          /* Adjust seek time if restarting file or tempo changed */
          if((midi_ctl->midi_status & AUDFMT_MASK_REPEAT_PENDING) ||
             (midi_ctl->tempo_pending != midi_ctl->tempo)) {
            audfmt_calc_ms(midi_ctl, midi_ctl->seek_elapsed_tick, &min_tick);
            if(min_tick < midi_ctl->seek_ms) {
              midi_ctl->seek_ms         -= min_tick;
              midi_ctl->play_time_ms    += min_tick;
            } else {
              midi_ctl->play_time_ms    += midi_ctl->seek_ms;
              midi_ctl->seek_ms          = 0;
            }
            midi_ctl->seek_elapsed_tick  = 0;

            /* Adjust factors for any tempo commands */
            if(midi_ctl->tempo_pending != midi_ctl->tempo) {
              midi_ctl->tempo = midi_ctl->tempo_pending;
              if(midi_ctl->tempo != 0) {
                switch(midi_ctl->type) {
                  case AUDFMT_MIDI_MIDI_TYPE:
                    midi_ctl->calc_ms_of   = AUDFMT_MAX_32BIT / midi_ctl->tempo;
                    break;
#ifdef FEATURE_WEBAUDIO
                  case AUDFMT_MIDI_CMID_TYPE:
                  case AUDFMT_MIDI_MELO_TYPE:
                    midi_ctl->calc_tick_of = AUDFMT_MAX_32BIT / midi_ctl->tempo;
                    break;
#endif /* FEATURE_WEBAUDIO */
                }
              }
            }

            /* Recalculate seek tick */
            if(midi_ctl->seek_ms != 0) {
              audfmt_calc_tick(midi_ctl, midi_ctl->seek_ms,
                               &midi_ctl->seek_tick);
            } else {
              midi_ctl->seek_tick = 0;
            }
          }
        }
      }

      /* Process events for calc time command */
      if(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME) {
        if(midi_ctl->midi_status & AUDFMT_MASK_ERROR) {
          audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_FAILURE, 0, NULL);
          midi_ctl->midi_status = 0;
          audfmt_check_reset_calc_time();
          return(FALSE);
        } else if(!(midi_ctl->midi_status & AUDFMT_MASK_PARSE_DELAYED)) {
          /* Add tick amount to elapsed tick */
          midi_ctl->elapsed_tick += midi_ctl->delta_tick;
          midi_ctl->delta_tick    = 0;

          /* Check for overflow of tick counter and update accordingly */
          if(midi_ctl->elapsed_tick >= midi_ctl->calc_ms_of) {
            midi_ctl->play_time_ms += midi_ctl->calc_tick_of;
            midi_ctl->elapsed_tick -= midi_ctl->calc_ms_of;
          }

          /* Adjust time for tempo changes */
          if(midi_ctl->tempo_pending != midi_ctl->tempo) {
            audfmt_calc_ms(midi_ctl, midi_ctl->elapsed_tick, &min_tick);
            midi_ctl->play_time_ms += min_tick;
            midi_ctl->elapsed_tick  = 0;

            midi_ctl->tempo = midi_ctl->tempo_pending;
            if(midi_ctl->tempo != 0) {
              switch(midi_ctl->type) {
                case AUDFMT_MIDI_MIDI_TYPE:
                  midi_ctl->calc_ms_of   = AUDFMT_MAX_32BIT / midi_ctl->tempo;
                  break;

#ifdef FEATURE_WEBAUDIO
                case AUDFMT_MIDI_CMID_TYPE:
                case AUDFMT_MIDI_MELO_TYPE:
                  midi_ctl->calc_tick_of = AUDFMT_MAX_32BIT / midi_ctl->tempo;
                  break;
#endif /* FEATURE_WEBAUDIO */
              }
            }
          }

          /* If done, calculate remaining time */
          if(midi_ctl->midi_status & AUDFMT_MASK_DONE_PENDING) {
            audfmt_calc_ms(midi_ctl, midi_ctl->elapsed_tick, &min_tick);
            midi_ctl->play_time_ms += min_tick;
            midi_ctl->midi_status   = 0;
            audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_CALC_TIME_RESULT,
                              midi_ctl->play_time_ms, NULL);
            audfmt_check_reset_calc_time();
            return(FALSE);
          }
        }
      }

      if(midi_ctl->midi_status & (AUDFMT_MASK_SILENCE_SKIP |
                                  AUDFMT_MASK_INITIAL_SKIP |
                                  AUDFMT_MASK_OFFSET_SKIP  |
                                  AUDFMT_MASK_FIND_CUEP_END)) {
        /* Adjust time for tempo changes */
        if(midi_ctl->tempo_pending != midi_ctl->tempo) {
          midi_ctl->tempo = midi_ctl->tempo_pending;
          if(midi_ctl->tempo != 0) {
            switch(midi_ctl->type) {
              case AUDFMT_MIDI_MIDI_TYPE:
                midi_ctl->calc_ms_of   = AUDFMT_MAX_32BIT / midi_ctl->tempo;
                break;

#ifdef FEATURE_WEBAUDIO
              case AUDFMT_MIDI_CMID_TYPE:
              case AUDFMT_MIDI_MELO_TYPE:
                midi_ctl->calc_tick_of = AUDFMT_MAX_32BIT / midi_ctl->tempo;
                break;
#endif /* FEATURE_WEBAUDIO */
            }
          }
        }
      }
    } while ((midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME    |
                                       AUDFMT_MASK_SEEK_ACTIVE  |
                                       AUDFMT_MASK_SILENCE_SKIP |
                                       AUDFMT_MASK_INITIAL_SKIP |
                                       AUDFMT_MASK_OFFSET_SKIP  |
                                       AUDFMT_MASK_FIND_CUEP_END)) &&
             !(midi_ctl->midi_status & (AUDFMT_MASK_PARSE_DELAYED  |
                                        AUDFMT_MASK_REPEAT_PENDING |
                                        AUDFMT_MASK_ERROR)));

    /* Set status of sequence to parse done if it has finished parsing
    ** this time event
    */
    if(!(midi_ctl->midi_status & AUDFMT_MASK_PARSE_DELAYED)) {
      midi_ctl->midi_status &= ~AUDFMT_MASK_PARSE_PENDING;
      midi_ctl->midi_status |= AUDFMT_MASK_PARSE_DONE;
    }
  }
  return (ret_val);
} /* end audfmt_parse_single */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_preparse

DESCRIPTION
  This function preparses one time event of a the specified file type.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If a time event of any sequence has been successfully parsed.
  FALSE - If no sequence has been parsed successfully.

SIDE EFFECTS
  None

===========================================================================*/
boolean audfmt_preparse (
  audmain_parse_ctl_type   *parse_ctl
) {
  audfmt_parse_type *midi_ctl;
  boolean           ret_val = TRUE;

  if(parse_ctl != NULL) {
    ret_val       = FALSE;
    preparse_sent = FALSE;
    midi_ctl      = (audfmt_parse_type *) parse_ctl->tracks;

    ret_val = audfmt_parse_single(midi_ctl) || ret_val;
#ifdef FEATURE_AUDFMT_MULTISEQUENCER
    ret_val = audfmt_parse_single(&midi_ctl[1]) || ret_val;
    ret_val = audfmt_parse_single(&midi_ctl[2]) || ret_val;
    ret_val = audfmt_parse_single(&midi_ctl[3]) || ret_val;
#endif /* FEATURE_AUDFMT_MULTISEQUENCER */
  }
  return(ret_val);
} /* end audfmt_preparse */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_reset

DESCRIPTION
  This function resets all active sequences.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audfmt_reset ( void )
{
  audfmt_parse_type   *midi_ctl;
  uint32              sequence = AUDFMT_NUM_SEQUENCES;
  boolean             ret_val  = TRUE;

  midi_ctl = audfmt_parse_ctl;
  while(sequence != 0) {
    if(midi_ctl->midi_status & AUDFMT_MASK_ENABLED) {
      if(!(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_GET_SPEC))) {
        audfmt_end_sequence(midi_ctl, SND_CMX_AF_PLAY_DONE);
      } else {
        ret_val = FALSE;
      }
    }
    sequence--;
    midi_ctl++;
  }
  preparse_sent = FALSE;
  return(ret_val);
} /* end audfmt_reset */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_reset_calc_time

DESCRIPTION
  This function stops all sequences that are doing a CALC_TIME.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audfmt_reset_calc_time ( void )
{
  audfmt_parse_type   *midi_ctl;
  uint32              sequence = AUDFMT_NUM_SEQUENCES;
  boolean             ret_val  = TRUE;

  midi_ctl = audfmt_parse_ctl;
  while(sequence != 0) {
    if(midi_ctl->midi_status & AUDFMT_MASK_ENABLED) {
      if(midi_ctl->midi_status & AUDFMT_MASK_CALC_TIME) {
        audfmt_end_sequence(midi_ctl, SND_CMX_AF_CALC_TIME_ABORT);
      } else {
        ret_val = FALSE;
      }
    }
    sequence--;
    midi_ctl++;
  }

  return(ret_val);
} /* end audfmt_reset_calc_time */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_check_reset

DESCRIPTION
  This function checks if there are any active sequences.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audfmt_check_reset ( void )
{
  audfmt_parse_type   *midi_ctl;
  uint32              sequence = AUDFMT_NUM_SEQUENCES;
  boolean             ret_val  = TRUE;

  midi_ctl = audfmt_parse_ctl;
  while(sequence != 0) {
    if(midi_ctl->midi_status & AUDFMT_MASK_ENABLED) {
      ret_val = FALSE;
      break;
    }
    sequence--;
    midi_ctl++;
  }

  return(ret_val);
} /* end audfmt_check_reset */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_pause_flag

DESCRIPTION
  This function gets pause status of MIDI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audfmt_get_pause_flag (
  audmain_parse_ctl_type *parse_ctl
) {
  audfmt_parse_type *midi_ctl;
  boolean           ret_val  = FALSE;
  uint32            sequence = AUDFMT_NUM_SEQUENCES;

  if(parse_ctl != NULL) {
    midi_ctl = (audfmt_parse_type *) parse_ctl->tracks;
    while((sequence != 0) && (ret_val == FALSE)) {
      if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
         !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_GET_SPEC))) {
        ret_val = (midi_ctl->midi_status & (AUDFMT_MASK_PAUSED |
                                            AUDFMT_MASK_SEEK_PAUSED));
      }
      sequence--;
      midi_ctl++;
    }
  }

  return(ret_val);
} /* end audfmt_get_pause_flag */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_pause

DESCRIPTION
  This function gets pause status of MIDI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audfmt_get_pause (
  audmain_parse_ctl_type *parse_ctl
) {
  audfmt_parse_type *midi_ctl;
  boolean           ret_val  = TRUE;
  uint32            sequence = AUDFMT_NUM_SEQUENCES;
  uint8             num_seq  = 0;

  if(parse_ctl != NULL) {
    midi_ctl = (audfmt_parse_type *) parse_ctl->tracks;
    while((sequence != 0) && (ret_val == TRUE)) {
      if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
         !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_GET_SPEC))) {
         if(((midi_ctl->midi_status & 
                 (AUDFMT_MASK_PAUSED | AUDFMT_MASK_SEEK_PAUSED |
                  AUDFMT_MASK_SEEK_ACTIVE)) == 0) ||
            ((midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE) &&
             !(midi_ctl->midi_status & AUDFMT_MASK_INIT_DONE))) {                                 
           ret_val = FALSE;
         }
         num_seq++;
      }
      sequence--;
      midi_ctl++;
    }
  }

  ret_val = (num_seq > 0) ? ret_val: FALSE;
  return(ret_val);
} /* end audfmt_get_pause */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_valid_time

DESCRIPTION
  This function gets valid_time status of MIDI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audfmt_get_valid_time (
  audmain_parse_ctl_type *parse_ctl
) {
  audfmt_parse_type *midi_ctl;
  boolean           ret_val  = FALSE;
  uint32            sequence = AUDFMT_NUM_SEQUENCES;

  if(parse_ctl != NULL) {
    midi_ctl = (audfmt_parse_type *) parse_ctl->tracks;
    while((sequence != 0) && (ret_val == FALSE)) {
      if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
         !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_GET_SPEC))) {
        ret_val = ((midi_ctl->midi_status & AUDFMT_MASK_PARSE_DONE) != 0);
      }
      sequence--;
      midi_ctl++;
    }
  }

  return(ret_val);
} /* end audfmt_get_valid_time */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_pause

DESCRIPTION
  This function pauses playback of all sequences.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_pause (
  audmain_parse_ctl_type *parse_ctl,
  boolean                pause_flag
) {
  audfmt_parse_type   *midi_ctl;
  uint32              sequence = AUDFMT_NUM_SEQUENCES;
  uint32              tlong;

  if(parse_ctl != NULL) {
    midi_ctl = (audfmt_parse_type *) parse_ctl->tracks;
    while(sequence != 0) {
      if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
         !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_GET_SPEC))) {
        if(pause_flag) {
          audfmt_do_pause_cmd(midi_ctl, TRUE);
          midi_ctl->midi_status |= AUDFMT_MASK_PAUSED;

          tlong = midi_ctl->play_time_ms + midi_ctl->seek_ms +
                  audfmt_tempo_adjust_up(midi_ctl->tempo_factor,
                                         midi_ctl->accumulated_ms);

          audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_PAUSE, tlong, NULL);
        } else {
          midi_ctl->midi_status &= ~(AUDFMT_MASK_PAUSED |
                                     AUDFMT_MASK_STARTED_AGE);
          if(!(midi_ctl->midi_status & (AUDFMT_MASK_SEEK_ACTIVE |
                                        AUDFMT_MASK_SEEK_PAUSED |
                                        AUDFMT_MASK_RESUME_PENDING))) {
            audfmt_do_pause_cmd(midi_ctl, FALSE);
            midi_ctl->midi_status |= AUDFMT_MASK_INIT_DONE;
            tlong = midi_ctl->play_time_ms +
                    audfmt_tempo_adjust_up(midi_ctl->tempo_factor,
                                           midi_ctl->accumulated_ms);
            audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_RESUME, tlong, NULL);
          }
        }
      }
      sequence--;
      midi_ctl++;
    }
  }
} /* end audfmt_pause */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_seek_pause

DESCRIPTION
  This function pauses all files being played in preparation for seek command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_seek_pause (
  audmain_parse_ctl_type *parse_ctl
) {
  audfmt_parse_type *midi_ctl;
  uint32            sequence = AUDFMT_NUM_SEQUENCES;

  if(parse_ctl != NULL) {
    midi_ctl = (audfmt_parse_type *) parse_ctl->tracks;
    while(sequence != 0) {
      if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
         !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_GET_SPEC))) {
        audfmt_do_pause_cmd(midi_ctl, TRUE);
        midi_ctl->midi_status |= AUDFMT_MASK_SEEK_PAUSED;
      }
      sequence--;
      midi_ctl++;
    }
  }
} /* end audfmt_seek_pause */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_sequence_num

DESCRIPTION
  This function gets the sequence num of given sequence

DEPENDENCIES
  None

RETURN VALUE
  seq num

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_get_sequence_num (
  void     *sequence,
  uint16 * seq_num
) {
  audfmt_parse_type   *midi_ctl;
  uint32              index   = AUDFMT_NUM_SEQUENCES;

  midi_ctl = audfmt_parse_ctl;
  while((index != 0) && (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle->audfmt_ptr != sequence)) {
    index--;
  }

if(index != 0)
{
   midi_ctl = midi_ctl+AUDFMT_NUM_SEQUENCES-index;
   
  if((midi_ctl->handle->audfmt_ptr == sequence) &&
     (midi_ctl->midi_status & AUDFMT_MASK_ENABLED)) {
    *seq_num = midi_ctl->sequence;
    return AUDMAIN_STATUS_SUCCESS;
  }
}

  return AUDMAIN_STATUS_FAILURE;
} /* end audfmt_get_sequence_num  */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_rewind_single

DESCRIPTION
  This function moves the playback of a single sequence back ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static audfmt_status_type audfmt_rewind_single (
  audfmt_parse_type *midi_ctl,
  uint32            ms_rewind
) {
  audfmt_status_type ret_val = AUDFMT_STATUS_ERROR;
  uint32             tlong;

  if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
     !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {

    midi_ctl->midi_status &= ~AUDFMT_MASK_SEEK_PAUSED;

    audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_REWIND,
                      midi_ctl->play_time_ms, NULL);

    if(midi_ctl->midi_status & AUDFMT_MASK_PARSING_HEADER) {
      if(ms_rewind < midi_ctl->seek_ms) {
        midi_ctl->seek_ms -= ms_rewind;
      } else {
        midi_ctl->seek_ms  = 0;

        if(!(midi_ctl->midi_status & AUDFMT_MASK_PAUSED)) {
          audfmt_check_seek_resume(midi_ctl);
        }
      }
    } else {
      if(midi_ctl->midi_status & AUDFMT_MASK_SEEK_ACTIVE) {
        audfmt_calc_ms(midi_ctl, midi_ctl->seek_elapsed_tick, &tlong);
        if(tlong < midi_ctl->seek_ms) {
          midi_ctl->seek_ms         -= tlong;
        } else {
          midi_ctl->seek_ms          = 0;
        }
        midi_ctl->play_time_ms      += tlong;
        midi_ctl->seek_elapsed_tick  = 0;

        if(ms_rewind <= midi_ctl->seek_ms) {
          midi_ctl->seek_ms -= ms_rewind;
          ms_rewind          = 0;
          audfmt_calc_tick(midi_ctl, midi_ctl->seek_ms, &midi_ctl->seek_tick);
        } else {
          ms_rewind         -= midi_ctl->seek_ms;
        }
      } else {
        midi_ctl->play_time_ms  += audfmt_tempo_adjust_up(
                                                   midi_ctl->tempo_factor,
                                                   midi_ctl->accumulated_ms);
        midi_ctl->accumulated_ms = 0;
      }

      if(ms_rewind != 0) {
        tlong = midi_ctl->play_time_ms;

        audfmt_restart(midi_ctl, TRUE);

        if(ms_rewind < tlong) {
          audfmt_fforward_single (midi_ctl, tlong - ms_rewind, FALSE);
        }
      }

      if(!(midi_ctl->midi_status & (AUDFMT_MASK_PAUSED |
                                    AUDFMT_MASK_SEEK_ACTIVE))) {
        audfmt_check_seek_resume(midi_ctl);
      }
    }

    ret_val = AUDFMT_STATUS_SUCCESS;
  }
  return(ret_val);
} /* end audfmt_rewind_single */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_fforward

DESCRIPTION
  This function moves the playback of all sequences forward ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_fforward (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 ms_forward,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
  /*lint -esym(715,elapsed_ms) Tell Lint we know elapsed_ms parameter
  ** is not used
  */
  /*lint -esym(715,adjusted_ms) Tell Lint we know adjusted_ms parameter
  ** is not used
  */
) {
  audfmt_parse_type   *midi_ctl;
  audmain_status_type ret_val  = AUDMAIN_STATUS_FAILURE;
  uint32              sequence = AUDFMT_NUM_SEQUENCES;

  if(parse_ctl != NULL) {
    midi_ctl = (audfmt_parse_type *) parse_ctl->tracks;
    while(sequence != 0) {
      if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
         !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_GET_SPEC))) {
        midi_ctl->midi_status |= AUDFMT_MASK_WAIT_RESUME;
        audfmt_fforward_single(midi_ctl, ms_forward, TRUE);
      }
      sequence--;
      midi_ctl++;
    }
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
} /* end audfmt_fforward */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_rewind

DESCRIPTION
  This function moves the playback of all sequences back ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_rewind (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 ms_rewind,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
) {
  audfmt_parse_type   *midi_ctl;
  audmain_status_type ret_val  = AUDMAIN_STATUS_FAILURE;
  uint32              sequence = AUDFMT_NUM_SEQUENCES;

  if(parse_ctl != NULL) {
    midi_ctl = (audfmt_parse_type *) parse_ctl->tracks;

    while(sequence != 0) {
      if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
         !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                    AUDFMT_MASK_GET_SPEC))) {
        midi_ctl->midi_status |= AUDFMT_MASK_WAIT_RESUME;
      }
      sequence--;
      midi_ctl++;
    }

    sequence = AUDFMT_NUM_SEQUENCES;
    midi_ctl = (audfmt_parse_type *) parse_ctl->tracks;
    while(sequence != 0) {
      audfmt_rewind_single(midi_ctl, ms_rewind);
      sequence--;
      midi_ctl++;
    }

    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
} /* end audfmt_rewind */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_tempo

DESCRIPTION
  This function sets the playback tempo factor of all sequences.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
audmain_status_type audfmt_set_tempo (
  uint32  *tempo_factor
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  audfmt_parse_type   *midi_ctl;
  uint32              index = AUDFMT_NUM_SEQUENCES;

  if(*tempo_factor < MIN_TEMPO_MULT_FACTOR) {
    *tempo_factor = MIN_TEMPO_MULT_FACTOR;
  } else if(*tempo_factor > MAX_TEMPO_MULT_FACTOR) {
    *tempo_factor = MAX_TEMPO_MULT_FACTOR;
  }

  midi_ctl = audfmt_parse_ctl;

  while(index != 0) {
    if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
       !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                  AUDFMT_MASK_GET_SPEC))) {
      midi_ctl->tempo_factor_pending = *tempo_factor;
      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_TEMPO, *tempo_factor,
                        NULL);
      ret_val = AUDMAIN_STATUS_SUCCESS;
    }
    index--;
    midi_ctl++;
  }
  return(ret_val);
} /* end audfmt_set_tempo */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_reset_sequence

DESCRIPTION
  This function resets the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_reset_sequence (
  void *sequence
) {
  audfmt_parse_type   *midi_ctl;
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  uint32              index   = AUDFMT_NUM_SEQUENCES;

  midi_ctl = audfmt_parse_ctl;
  while((index != 0) && 
  	(!(midi_ctl[AUDFMT_NUM_SEQUENCES-index].midi_status & AUDFMT_MASK_ENABLED)|| 
	(midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle == NULL) ||
	(midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle->audfmt_ptr != sequence))) {
    index--;
  }
  if(index != 0)
	midi_ctl = midi_ctl + AUDFMT_NUM_SEQUENCES-index;

  if((index != 0) && (midi_ctl->handle != NULL) &&
     (midi_ctl->handle->audfmt_ptr == sequence) &&
     (midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
     !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME |
                                AUDFMT_MASK_GET_SPEC))) {
    audfmt_end_sequence(midi_ctl, SND_CMX_AF_PLAY_DONE);
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
} /* end audfmt_reset_sequence */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_pause_sequence

DESCRIPTION
  This function pauses the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_pause_sequence (
  void     *sequence,
  boolean  pause_flag
) {
  audfmt_parse_type   *midi_ctl;
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  uint32              index   = AUDFMT_NUM_SEQUENCES;

  midi_ctl = audfmt_parse_ctl;
  while((index != 0) && (!(midi_ctl[AUDFMT_NUM_SEQUENCES-index].midi_status & AUDFMT_MASK_ENABLED)|| 
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle == NULL) ||
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle->audfmt_ptr != sequence))) {
    index--;
  }

  if(index != 0)
midi_ctl = midi_ctl +AUDFMT_NUM_SEQUENCES-index;
  if((index != 0) && (midi_ctl->handle != NULL) &&
     (midi_ctl->handle->audfmt_ptr == sequence) &&
     (midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
     !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {
    if(pause_flag) {
      audfmt_do_pause_cmd(midi_ctl, TRUE);
      midi_ctl->midi_status |= AUDFMT_MASK_PAUSED;

      index = midi_ctl->play_time_ms + midi_ctl->seek_ms +
              audfmt_tempo_adjust_up(midi_ctl->tempo_factor,
                                     midi_ctl->accumulated_ms);

      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_PAUSE, index, NULL);
    } else {
      midi_ctl->midi_status &= ~(AUDFMT_MASK_PAUSED | AUDFMT_MASK_STARTED_AGE);
      if(!(midi_ctl->midi_status & (AUDFMT_MASK_SEEK_ACTIVE |
                                    AUDFMT_MASK_SEEK_PAUSED |
                                    AUDFMT_MASK_RESUME_PENDING))) {
        audfmt_do_pause_cmd(midi_ctl, FALSE);
        index = midi_ctl->play_time_ms +
                audfmt_tempo_adjust_up(midi_ctl->tempo_factor,
                                       midi_ctl->accumulated_ms);
        audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_RESUME, index, NULL);
      }
    }
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
} /* end audfmt_pause_sequence */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_fforward_sequence

DESCRIPTION
  This function fast-forwards the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_fforward_sequence (
  void     *sequence,
  uint32   ms_forward
) {
  audfmt_parse_type   *midi_ctl;
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  uint32              index   = AUDFMT_NUM_SEQUENCES;

  midi_ctl = audfmt_parse_ctl;
  while((index != 0) && (!(midi_ctl[AUDFMT_NUM_SEQUENCES-index].midi_status & AUDFMT_MASK_ENABLED)|| 
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle == NULL) ||
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle->audfmt_ptr != sequence))) {
    index--;
  }

  if(index != 0)
  	midi_ctl = midi_ctl + AUDFMT_NUM_SEQUENCES-index;

  if((index != 0) && (midi_ctl->handle != NULL) &&
     (midi_ctl->handle->audfmt_ptr == sequence) &&
     (midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
     !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {
    if(audfmt_fforward_single(midi_ctl, ms_forward, TRUE) ==
       AUDFMT_STATUS_SUCCESS) {
      ret_val = AUDMAIN_STATUS_SUCCESS;
    }
  }

  return(ret_val);
} /* end audfmt_fforward_sequence */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_rewind_sequence

DESCRIPTION
  This function rewinds the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_rewind_sequence (
  void     *sequence,
  uint32   ms_rewind
) {
  audfmt_parse_type   *midi_ctl;
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  uint32              index   = AUDFMT_NUM_SEQUENCES;

  midi_ctl = audfmt_parse_ctl;
  while((index != 0) && (!(midi_ctl[AUDFMT_NUM_SEQUENCES-index].midi_status & AUDFMT_MASK_ENABLED)|| 
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle == NULL) ||
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle->audfmt_ptr != sequence))) {
    index--;
  }

    if(index != 0)
  	midi_ctl = midi_ctl + AUDFMT_NUM_SEQUENCES-index;

  if((index != 0) && (midi_ctl->handle != NULL) &&
     (midi_ctl->handle->audfmt_ptr == sequence) &&
     (midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
     !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {
    if(audfmt_rewind_single(midi_ctl, ms_rewind) == AUDFMT_STATUS_SUCCESS) {
      ret_val = AUDMAIN_STATUS_SUCCESS;
    }
  }

  return(ret_val);
} /* end audfmt_rewind_sequence */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_tempo_sequence

DESCRIPTION
  This function sets the playback tempo factor of the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_set_tempo_sequence (
  void     *sequence,
  uint32   tempo_factor
) {
  audfmt_parse_type   *midi_ctl;
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  uint32              index   = AUDFMT_NUM_SEQUENCES;

  midi_ctl = audfmt_parse_ctl;
  while((index != 0) && (!(midi_ctl[AUDFMT_NUM_SEQUENCES-index].midi_status & AUDFMT_MASK_ENABLED)|| 
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle == NULL) ||
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle->audfmt_ptr != sequence))) {
    index--;
  }

    if(index != 0)
  	midi_ctl = midi_ctl + AUDFMT_NUM_SEQUENCES-index;

  if((index != 0) && (midi_ctl->handle != NULL) &&
     (midi_ctl->handle->audfmt_ptr == sequence) &&
     (midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
     !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {
    if(tempo_factor < MIN_TEMPO_MULT_FACTOR) {
      tempo_factor = MIN_TEMPO_MULT_FACTOR;
    } else if(tempo_factor > MAX_TEMPO_MULT_FACTOR) {
      tempo_factor = MAX_TEMPO_MULT_FACTOR;
    }

    midi_ctl->tempo_factor_pending = tempo_factor;
    audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_TEMPO, tempo_factor,
                      NULL);

    ret_val = AUDMAIN_STATUS_SUCCESS;
  }
  return(ret_val);
} /* end audfmt_set_tempo_sequence */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_tune_sequence

DESCRIPTION
  This function sets the playback tune factor of the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_set_tune_sequence (
  void     *sequence,
  uint32   tune_factor
) {
  audfmt_parse_type *midi_ctl;
  uint32            index = AUDFMT_NUM_SEQUENCES;
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

  midi_ctl = audfmt_parse_ctl;

  while((index != 0) && (!(midi_ctl[AUDFMT_NUM_SEQUENCES-index].midi_status & AUDFMT_MASK_ENABLED)|| 
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle == NULL) ||
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle->audfmt_ptr != sequence))) {
    index--;
  }

      if(index != 0)
  	midi_ctl = midi_ctl + AUDFMT_NUM_SEQUENCES-index;

  if((index != 0) && (midi_ctl->handle != NULL) &&
     (midi_ctl->handle->audfmt_ptr == sequence) &&
     (midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
     !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {

    if(tune_factor < AUDFMT_MIN_TUNE_FACTOR) {
      tune_factor = AUDFMT_MIN_TUNE_FACTOR;
    } else if(tune_factor > AUDFMT_MAX_TUNE_FACTOR) {
      tune_factor = AUDFMT_MAX_TUNE_FACTOR;
    }

    if(audfmt_set_tune_cmd(tune_factor, midi_ctl) == AUDFMT_STATUS_SUCCESS) {
      midi_ctl->tune_factor = tune_factor;
      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_TUNE, tune_factor, NULL);
      ret_val = AUDMAIN_STATUS_SUCCESS;
    }
  }

  return(ret_val);
} /* end audfmt_set_tune_sequence */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_pan_sequence

DESCRIPTION
  This function sets the playback pan factor of the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_set_pan_sequence (
  void     *sequence,
  uint16   pan_factor
) {

  audfmt_parse_type *midi_ctl;
  uint16 index;
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

  midi_ctl = audfmt_parse_ctl;
  index = AUDFMT_NUM_SEQUENCES;

  while((index != 0) && (!(midi_ctl[AUDFMT_NUM_SEQUENCES-index].midi_status & AUDFMT_MASK_ENABLED)|| 
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle == NULL) ||
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle->audfmt_ptr != sequence))) {
    index--;
  }

        if(index != 0)
  	midi_ctl = midi_ctl + AUDFMT_NUM_SEQUENCES-index;

  if((index != 0) && (midi_ctl->handle != NULL) &&
     (midi_ctl->handle->audfmt_ptr == sequence) &&
     (midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
     !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {

    if(pan_factor > AUDFMT_MAX_PAN_FACTOR) {
      pan_factor = AUDFMT_MAX_PAN_FACTOR;
    }

    if(audfmt_set_pan_cmd(pan_factor, midi_ctl) == AUDFMT_STATUS_SUCCESS) {
      midi_ctl->pan_factor = pan_factor;
      audfmt_do_play_cb(midi_ctl->handle, SND_CMX_AF_PAN, pan_factor, NULL);
      ret_val = AUDMAIN_STATUS_SUCCESS;
    }
  }

  return(ret_val);
} /* end audfmt_set_pan_sequence */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_vol_sequence

DESCRIPTION
  This function sets the volume of the indicated sequence if it has Adec data.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS
  AUDMAIN_STATUS_FAILURE : sequence not active

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_set_vol_sequence (
  void     *sequence,
  uint16   vol_factor
) {
  audfmt_parse_type *midi_ctl;
  uint16 index;
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

  midi_ctl = audfmt_parse_ctl;
  index = AUDFMT_NUM_SEQUENCES;

  while((index != 0) && (!(midi_ctl[AUDFMT_NUM_SEQUENCES-index].midi_status & AUDFMT_MASK_ENABLED)|| 
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle == NULL) ||
        (midi_ctl[AUDFMT_NUM_SEQUENCES-index].handle->audfmt_ptr != sequence))) {
    index--;
  }
      if(index != 0)
  	midi_ctl = midi_ctl + AUDFMT_NUM_SEQUENCES-index;

  if((index != 0) && (midi_ctl->handle != NULL) &&
     (midi_ctl->handle->audfmt_ptr == sequence) &&
     (midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
     !(midi_ctl->midi_status & (AUDFMT_MASK_CALC_TIME | AUDFMT_MASK_GET_SPEC)))
  {

    midi_ctl->vol_factor = vol_factor;
    if(audmain_audfmt_set_vol_seq(midi_ctl->sequence, vol_factor) !=
       AUDMAIN_STATUS_SUCCESS) {
      midi_ctl->midi_status2 |= AUDFMT_MASK_VOL_CMD_PENDING;
    }
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return ret_val;
} /* end audfmt_set_vol_sequence */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION audfmt_set_poly_mode

DESCRIPTION
  This function sets the playback mode for MIDI files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_set_poly_mode (
  audfmt_midi_poly_mode_type mode
) {
  audfmt_parse_type *midi_ctl;
  uint32            counter, hybrid_mode;

  if(mode == AUDFMT_MIDI_POLY_MODE_HP) {
    hybrid_mode = CMID_POLY_MAX_HP;
  } else {
    hybrid_mode = CMID_POLY_MAX_HQ;
  }

  midi_ctl = audfmt_parse_ctl;
  for(counter = 0; counter < AUDFMT_NUM_SEQUENCES; counter++) {
    if((midi_ctl->midi_status & AUDFMT_MASK_ENABLED) &&
       (midi_ctl->type == AUDFMT_MIDI_MIDI_TYPE)) {
      midi_ctl->hybrid_mode = hybrid_mode;
    }
    midi_ctl++;
  }
} /* end audfmt_set_poly_mode */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_adec_channel_done

DESCRIPTION
  This function resets the flags for the ADEC channel indicated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_adec_channel_done (
  uint32 channel
) {
  audfmt_parse_type *midi_ctl;
  uint32            counter;

  if(audfmt_adec_mask & (1 << channel)) {
    midi_ctl = audfmt_parse_ctl;
    for(counter = 0; counter < AUDFMT_NUM_SEQUENCES; counter++) {
      if(midi_ctl->adec_status[channel].enable) {
        midi_ctl->adec_status[channel].enable = FALSE;
#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */
        break;
      }
      midi_ctl++;
    }

    audfmt_adec_mask &= ~(1<< channel);
  }
} /* end audfmt_adec_channel_done */
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#ifdef FEATURE_DLS

#ifndef FEATURE_AUDIO_CMX_5P0
/* 32 bit x 32 bit multiplication */
sint31 MUL32x32s30 (
  sint31 a,
  sint31 b
) {
  return ((sint31) ((((int64) a) * ((int64) b)) >> 30));
}

/*****************************************************************/
/* Function: getlfofreqindex                                     */
/*****************************************************************/
/*  Inputs: absolute pitch specified in the DLS file (32-bits)   */
/*  Outputs: the index to the table that contains the freq of the*/
/*           LFO in our C-code                                   */
/*****************************************************************/
/* Description: Used to get the index of the LFOs from the 32-bit*/
/*              value specified by the absolute pitch of the inst*/
/*              The table has freq values given by               */
/*              table[i] = 0.5*40^(i/127)                        */
/*              so we equate 0.1*200^(i/127) =                   */
/*                           pow(2,(abspitch>>16-6900)/1200)*440 */
/*              i = (127/log2(200))*(((ap>>16-6900)/1200)-       */
/*                                           log2(0.1) +log2(440)*/
/*              ap is absolute pitch                             */
/*              i=max(0,i); this is to restrict to [0.1,20] freq */
/*              range                                            */
/*****************************************************************/
uint8 getLfoFreqIndex (
  sint31 abspitch
) {
  int64  frq;
  sint31 freq;
  sint15 index;
  uint8  ret_val;

  if(abspitch == (sint31) 0x80000000) {
    return(0);
  }

  freq = ((abspitch >> 16) - 6900); /* values for fmax = -5351.3179474
                                    ** value for fmin = -14523.94537
                                    ** hence freq should be in Q14
                                    */

  /* parameters are in Q 14 from now on..*/
  freq = (freq << Q14SHIFT) / 1200;

  /* freq = (ap>>16 - 6900)/1200 - log2(0.1) */
  freq = freq - LOG2_ONE_BY_TEN_Q14 + LOG2_440Q14; 

  /* frq = (127/log2(200)) * (ap>>16 - 6900)/1200 - log2(0.1) */
  frq = ((int64) freq * (int64) CONST_L32_Q15);
  frq = (frq >> Q15SHIFT);

  index = (sint15) ((frq + HALFQ14) >> Q14SHIFT); /* index =(int)(freq+0.5); */

  ret_val = (index > 127) ? 127:((index < 0) ? 0:index);
  return(ret_val);
}

/************************************************************/
/* Function: CentToRatioL15                                 */
/************************************************************/
/* Description: Calculates and returns                      */
/*              pow(2.0, (((double)cent)/1200.0)) in Q 15   */
/************************************************************/
sint31 CentToRatioL15 (
  sint31 cent
) {
  /* return (long)pow(2.0, (((double)cent)/1200.0 + 15.0)); */
  sint31 nCent = cent * CENTTABLE_RESO;
  sint31 ratio = 1 << 16;

  if(nCent > 0) {
    while(nCent > (CENTTABLE_MAX * CENTTABLE_RESO)) {
      ratio  = MUL32x32s30(ratio, table_cent2ratioL30[
                                            (CENTTABLE_MID + CENTTABLE_MAX) *
                                            CENTTABLE_RESO] );
      nCent -= CENTTABLE_MAX * CENTTABLE_RESO;
    }
  } else {
    while(nCent < (CENTTABLE_MIN * CENTTABLE_RESO)) {
      ratio  = MUL32x32s30( ratio, table_cent2ratioL30[0] );
      nCent -= CENTTABLE_MIN*CENTTABLE_RESO;
    }
  }
  ratio = MUL32x32s30(ratio, table_cent2ratioL30
                                    [nCent + (CENTTABLE_MID * CENTTABLE_RESO)]);

  return(ratio >> 1);
}

sint31 DescaleAbsoluteTimeL9 (
  sint31 x
) {
  sint31 r;

  if((uint32)x == 0x80000000) {
    return 0;
  }
  r = CentToRatioL15(x >> Q16SHIFT) >> 6;

  return((r != 0) ? r : 1);  /* [1/2^10 second] */
}

/*****************************************************************/
/* Function: getTimeIndex                                        */
/*****************************************************************/
/*  Inputs: absolute time specified in the DLS file (32-bits)    */
/*  Outputs: the index to the table that contains the no. of     */
/*           frames in our C-code                                */
/*****************************************************************/
/* Description: Used to get the index of LFO Delay,aegAttack Time*/
/*              aegDecayTime, aegReleaseTime pegAttackTime,      */
/*              pegDecayTime, pegReleaseTime                     */
/*****************************************************************/
uint8 getTimeIndex (
  sint31 abstime
) {
  sint15 index;
  sint31 frames;
  sint31 fxtimeL9;

  if(abstime == (sint31) 0x80000000) {
    return(0);
  }

  fxtimeL9 = (sint15) DescaleAbsoluteTimeL9(abstime);
  fxtimeL9 = fxtimeL9 * FRAMESPERSEC;
  frames   = (sint31) (fxtimeL9 + HALFQ9) >> Q9SHIFT;

  /* After calculation of no. of frames, we need to find out */
  /* index that would give us the required frames. The whole */
  /* table is divided into four regions, each is approximated*/
  /* with a linear equation. The following are the equations */
  /* and the regions.                                        */

  /* index = (frames+3)/3, if 1<=index<=10           */
  /* index = (frames+42)/7, if 11<=index<=56         */
  /* index = (frames+3563)/66, if 57<=index<=106     */
  /* index = (frames+56219)/552, if 107<=index<=128  */
  /* return (index-1)                                */

  /* index = (frames+4)/4, if 1 <= index <= 10 */
  index = (frames + 4) / 4;
  if((index >= 1) && (index <= 10)) {
    /* When frames!=0 and index =1, we return 0. This means that   */
    /* attacktime =0. But frames!=0 indicates that attacktime!=0   */
    /* It is observed that if attacktime becomes 0, when it is not */
    /* the output is not good. So we force attacktime to take a +ve*/
    /* if frames!=0                                                */

    if((index == 1) && (frames != 0)) {
      index++;
    }
    return(index - 1);
  } else {    /* index = (frames+49)/8, 11 <= index <= 56 */
    index = (frames + 49) / 8;
    if(index <= 56) {
      return(index-1);
    } else {    /* 57 <= index <= 106 */
      index = (frames + 4200) / 77;
      if(index <= 106) {
        return(index-1);
      } else {
        index = (frames + (sint31) 69427) / 675;
        if(index <= 128) {
          return(index-1);
        } else {
          return(127);
        }
      }
    }
  }
}

/****************************************************************/
/* Function: sqrtQ0                                             */
/* Input : 32-bit non-negative number,Q0                        */
/* output: 16-bit non-negative number whose square is close     */
/*         to the input value,Q0                                */
/****************************************************************/
/* Description: Calculates the integer whos square is closer    */
/*              to the input. Differs from the C- sqrt in the   */
/*              sense returns the closer integer than the floor */
/*              value                                           */
/****************************************************************/
sint15 sqrtQ0 (
  sint31 input
) {
  sint31 prod;
  sint31 temprod;
  sint15 tempres;
  sint15 res = 0;
  sint15 i = 14;

  /* 15 bits to be determined. see if bit i is 1 or 0 */
  while(i >= 0) {
    tempres = res + (1 << i);
    prod    = tempres * tempres;
    if(input >= prod) {
      res = tempres;
    }
    i--;
  }
  tempres = res + 1;
  temprod = tempres * tempres;
  prod    = res * res;
  if((temprod - input) < (input - prod)) {
    res++;
  }

  return(res);
}

/************************************************************/
/* Function: getPegSustainLevelIndex                        */
/* Input:  Sustain level (32-bit) from DLS file, % cents    */
/************************************************************/
/* Description: Calculates the Sustain level index for PEG  */  
/*              using sustain level from DLS as input       */
/************************************************************/
uint8 getPegSustainLevelIndex (
  sint31 Slevel
) {
  sint31 sratio;
  sint15 index;

  /* sustainlevelindex is restricted to -63->63           */
  /* convert the ratio into centum.                       */
  /* get the equivalent ratio from the tblf2eg table.     */
  /* the values in our table are 2^t, t=-2:2, 257 values. */
  /* values are in Q 13 format.                           */
  /* this ratio has to be converted into exp form for our */
  /* purpose, but we can still use the linear value to    */
  /* get the index.                                       */
  /* x(i) = -2 + i/64                                     */
  /* therefore, i = (x+2)*64                              */
  /* levelindex = i/2 (integer portion) and -63.          */

  /* sustain level can be any ration between 0,1 in log   */
  /* domain. our values are also in log domain.           */
  /* 0-1 correspond to -128->0 in the table. but the      */
  /* restriction on sustainlevel index is from -63->63    */
  /* 63 corresponding to +2                               */

  sratio = Slevel / 4000;  /* centum in Q14 */
  sratio = -63*(0x4000-sratio);
  index  = sratio>>14;
  index  = (index < -63) ? -63:((index > 63) ? 63:index);

  return(index);
}

/************************************************************/
/* Function: dB2ratio                                       */
/************************************************************/
/* Description: Calculates the pow(10,-96*x/20),x is in Q14 */
/*   Uses Taylor's series expansion                         */
/*   exp(x) = 1+x+x^2 /2 +x^3/6+x^4/24+x^5/120+x^6/720      */
/*   x = -4.8*log10*inp                                     */
/*   range of x is -11.06<x<0,hence we cannot neglect higher*/
/*   order terms of the taylor series expansion             */
/*   so x is divided by 12, and the result is raised to 12  */
/*                                                          */
/* outputs: pow(10,-96*x/20)                                */
/************************************************************/
sint31 dB2ratio (
  sint31 inp
) {
  /* 1/2, 1/6, 1/24, 1/120, 1/720, in Q15 */
  sint31 invfcts[] = {0x4000, 0x1555, 0x555, 0x111, 0x2d};

  int64  temp1;
  int64  result;
  sint31 x;
  sint15 i;
  
  temp1 = (-30180) * inp;                       /* y'=-4.8*ln10/12; Q15*Q14 */
  x = (sint31) ((temp1 + 0x8000) >> Q15SHIFT);  /* x in Q11 for expansion   */

  result = 0x4000 + (int64)x;                   /* 1 in Q14;                */
  temp1  = ((int64) x * (int64) x) >> 14;       /* x^2                      */
  for(i = 0; i < (NOOFTERMS - 1); i++) {
    result = result + ((temp1 * (int64) invfcts[i]) >> Q15SHIFT); 
    temp1  = (temp1 * (int64) x) >> Q14SHIFT;
  }
  temp1  = (result * result) >> Q14SHIFT;       /* res^2                    */
  temp1  = (temp1 * temp1) >> Q14SHIFT;         /* res^4                    */
  result = (temp1 * temp1) >> Q14SHIFT;         /* res^8                    */
  result = (temp1 * result) >> Q14SHIFT;        /* res^12                   */

  return ((sint31) result);
}

/************************************************************/
/* Function: getAegSustainLevelIndex                        */
/* Input:  Sustain level (32-bit) from DLS file,units: %dB  */
/************************************************************/
/* Description: Calculates the ratio for the AEG at which   */
/*              note would be sustained                     */
/************************************************************/
uint8 getAegSustainLevelIndex (
  sint31 Slevel
) {
  sint31 cent,tempcent;
  sint15 index;
  uint8  ret_val;

  tempcent = Slevel / 4000;  /* centum is in Q14 */
  if (tempcent == 0) {
    return(0);
  }

  /* Used the Faith's way of conversion, They divide the */
  /* input by 4000 and use it as a Q14 number            */
  /* Next they subtract it from 1 and calculate the ratio*/
  /* This ratio is the one used as sustain level         */
  /* We have a table that closely follows 2*i^2 for the  */
  /* sustain level. They are in Q15. Hence we need to    */
  /* find that i, that gives closest value of the sratio */
  /* Hence, i^2= sratio (both side are now in Q14        */

  cent = (1 << Q14SHIFT) - tempcent;  /* Q14 */
  
  tempcent = dB2ratio(cent);
  index    = sqrtQ0(tempcent);
  ret_val  = (index > 127) ? 127:((index < 0) ? 0:index);
  return(ret_val);
}

/************************************************************/
/* Function: getAegDepthIndex                               */
/* Input:  lGain expressed in relative gain units from the  */
/*         DLS file, associated with Wave Sample chunk,     */
/*         specified in the waveform                        */
/************************************************************/
/* Description: Calculates the AEG depth index to the table */
/************************************************************/
uint8 getAegDepthIndex (
  sint31 atten
) {
  /* fixed point */
  sint31 fxratio;
  sint15 index;
  uint8  ret_val;
  
  if(atten == (sint31) 0x80000000) {  /* if abs=0, return 0 */
    return(0);
  }

  /* fxratio = (atten>>4)/(96*10)             */
  /* We have gain = 200*log10(v)*2^16         */
  /* so v = pow(10,gain>>16/200)              */
  /* dB2ratio outputs pow(10,-96*x/20)        */
  /* for our purpose, to compensate for the   */
  /* factors involved, we need to divide gain */
  /* by 96*10, and the result is shifted >> 2 */
  /* to make the result Q14                   */

  fxratio = (atten / 960) >> 2;
  fxratio = -fxratio;
  fxratio = dB2ratio(fxratio);
  index   = sqrtQ0(fxratio);

  ret_val = (index > 127) ? 127:((index < 0) ? 0:index);
  return(ret_val);
}

/*****************************************************************/
/* Function: getAegAttackLevelIndex                              */
/* Inputs: attackTimeIndex, decayTimeIndex,sustainLevelIndex     */
/*****************************************************************/
/* Description: Calculates the InitialLevelIndex taking          */
/*              attacktime, decaytime and sustainlevel as inputs */
/*              aegAttack1Level =(attackTime==0)?((decayTime==0)?*/
/*                                       sustainLevel:127):127;  */
/* NOTES: Used by both PEG & AEG                                 */
/*****************************************************************/
uint8 getAegAttackLevelIndex (
  uint8 attackTimeIndex,
  uint8 decayTimeIndex,
  uint8 sustainLevelIndex
) {
  uint8 index;

  if((attackTimeIndex == 0) && (decayTimeIndex == 0)) {
    index = sustainLevelIndex;
  } else {
    index = 0x7f;
  }

  return(index);
}

/*****************************************************************/
/* Function: getPegAttackLevelIndex                              */
/* Inputs: attackTimeIndex, decayTimeIndex,sustainLevelIndex     */
/*****************************************************************/
/* Description: Calculates the InitialLevelIndex taking          */
/*              attacktime, decaytime and sustainlevel as inputs */
/*              aegAttack1Level =(attackTime==0)?((decayTime==0)?*/
/*                                       sustainLevel:127):127;  */
/* NOTES: Used by both PEG & AEG                                 */
/*****************************************************************/
uint8 getPegAttackLevelIndex (
  uint8 attackTimeIndex,
  uint8 decayTimeIndex,
  uint8 sustainLevelIndex,
  uint8 attacklevel
) {
  uint8 index;

  if((attackTimeIndex == 0) && (decayTimeIndex == 0)) {
    index = sustainLevelIndex;
  } else {
    index = attacklevel;
  }

  return(index);
}

/***************************************************************/
/* Function: getAegInitialLevelIndex, getPegInitialLevelIndex  */
/* Inputs: attackTimeIndex, decayTimeIndex,sustainLevelIndex   */
/***************************************************************/
/* Description: Calculates the InitialLevelIndex taking        */
/*    attacktime, decaytime and sustainlevel as inputs         */
/*    aegInitialLevel =(attackTime==0)?((decayTime==0)?        */
/*    sustainLevel:1):0;                                       */
/* NOTES: Used by both PEG & AEG                               */
/***************************************************************/
uint8 getAegInitialLevelIndex (
  uint8 attackTimeIndex,
  uint8 decayTimeIndex,
  uint8 sustainLevelIndex
) {
  uint8 index;

  if(attackTimeIndex == 0) {
    if(decayTimeIndex == 0) {
      index = sustainLevelIndex;
    } else {
      index = 0x7f;
    }
  } else {
    index = 0;
  }

  return(index);
}

uint8 getPegInitialLevelIndex (
  uint8 attackTimeIndex,
  uint8 decayTimeIndex,
  uint8 sustainLevelIndex,
  uint8 attackLevelIndex,
  uint8 initialLevelIndex
) {
  uint8 index;

  if(attackTimeIndex == 0) {
    if(decayTimeIndex == 0) {
      index = sustainLevelIndex;
    } else {
      index = attackLevelIndex;
    }
  } else {
    index = 0;
  }

  return(index);
}

/************************************************************/
/* Function: getLfoAegDepthIndex                            */
/* Inputs: modLfo2Gain                                      */
/************************************************************/
/* Description: the table that has lfoAegDepths in DSP      */
/* corresponds to 12*(0:127)/127 dB depths.                 */
/* Therefore, given modLfoToGain in relative dB,need to find*/
/* the index, i = 127*modLfoToGain/(120*(2^16))             */
/************************************************************/
uint8 getLfoAegDepthIndex (
  sint31 modLfo2Gain
) {
  sint31 temp;
  sint15 index;
  uint8  ret_val;

  temp  = modLfo2Gain >> 16;
  temp  = temp * 127;
  temp  = temp * ONE_OVER_ONE_TWENTY_Q16;
  index = (sint15) (((temp >> 15) + 1) >> 1);
  ret_val = (index < 0) ? 0:((index > 127) ? 127:index);
  return(ret_val);
}

/****************************************************************************/
/* Fuction: getPegDepthIndex                                                */
/* Inputs:  lScale value from the EG2 pitch connection.                     */
/* Description:                                                             */
/*    Calculates ratio= pow(2,lscale/1200), in Q15, gets index = ratio>>9.  */ 
/****************************************************************************/
uint8 getPegDepthIndex (
  sint31 lScale,
  uint8  *slevelindex,
  uint8  *attacklevel
) {
  sint31 sratio, nCents;
  sint15 index;
  sint7  tbyte;
  int64  mulres;

  if(lScale == 0) {
    return(0);
  }

  tbyte  = (sint7) *slevelindex;
  index  = tbyte;
  sratio = index * ONE_BY_63_Q20;
  sratio = (sint31) (ONEQ20 + sratio) >> 4;     /* Q16           */
  mulres = (int64) sratio * (int64) lScale;     /* Q16*Q16 = Q32 */

  nCents = (sint31) (mulres >> Q32SHIFT);
  /* nCents will be in the range of -1200 to 1200. */
  /* these correspond to -64 to +64 in our code.   */
  index  = (nCents * 64) / 1200;
  tbyte  = (index < -64) ? -64:((index > 64) ? 64:index);

  *slevelindex = (uint8) tbyte;

  nCents = lScale >> Q16SHIFT;
  index  = (nCents * 64) / 1200;
  tbyte  = (index < -64) ? -64:((index > 64) ? 64:index);

  *attacklevel = (uint8) tbyte;

  return(32);
}

/****************************************************************************/
/* Fuction: getTune                                                         */
/* Inputs:                                                                  */
/*    noteNum: root node of the waveform.                                   */
/*    samplingFreq: sampling frequency at which the waveform is stored      */
/* Description:                                                             */
/*   Gets the tune value for the given note number                          */
/*   Calculates: pow(2,(129-notenum) / 12) *fs1/48000, and convert into q20 */ 
/****************************************************************************/
sint31 getTune (
  boolean drum,
  uint32  mapped_note,
  uint8   orig_key,
  sint15  noteNum,
  sint15  fine_tune,
  sint31  samplingFreq
) {
  sint31 x,y;
  sint31 result;
  sint31 tune;

  if(noteNum > 128) {
    noteNum = 128;
  } else if(noteNum < 1) {
    noteNum = 1;
  }

  if(drum) {
    noteNum -= (orig_key-mapped_note);

    if(noteNum > 128) {
      noteNum = 128;
    }
  }

  noteNum = 129 - noteNum;
  x = noteNum / 12;
  y = noteNum - (x * 12);

  result  = 1 << x;
  result *= exp2TableQ14[y];                        /* Q 14.              */
  tune    = CentToRatioL15(fine_tune);              /* tune=contribution from fine_tune, Q15 */
  result  = ((int64)result* (int64) tune )>>15;     /* multiply tune, result in  Q14         */     
  result  = ((result * 64) / 48000) * samplingFreq; /* end result converted into Q20         */

  return(result);
}

uint8 getAttackDecayScale (
  sint31 centTime
) {
  return (uint8) ((centTime >> 24) & 0xFF);
}

/****************************************************************************/
/* Fuction: getLfoPegDepthIndex                                             */
/* Inputs:                                                                  */
/*    lScale value from the instrument defnition indicating the connection  */
/*    value.                                                                */
/* Description:                                                             */
/*    Calculates the lfo->pegdepth index. The new table that was included   */
/*    has values in Q13 from 0->1 divided into 128 intervals. Method to     */
/*    to convert is as follows:                                             */
/*      lScale = abs(lScale)                                                */
/*      temp   = CentToRatioL15(lScale>>Q16SHIFT) >> 2                      */
/*      temp   = temp - 0x2000;                                             */
/*      index  = temp >> 6                                                  */
/****************************************************************************/
sint15 getLfoPegDepthIndex (
  sint31 lScale
) {
  sint31 temp;
  sint15 index;
  uint8  ret_val;

  if(lScale < 0) {
    lScale = -lScale;   /* get the absolute value */
  }

  temp  = CentToRatioL15(lScale >> Q16SHIFT) >> 2; /* gets the ratio in Q13 */
  temp  = temp - 0x2000;                           /* temp = x-1.           */
  index = (sint15) (temp >> 6);

  ret_val = (index < 0) ? 0:((index > 127) ? 127:index);
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_reset_prog_def

DESCRIPTION
  This function initializes the program definition structure to
  default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_reset_prog_def (
  audfmt_hq_prog_def_struct_type *prog_ptr
) {
  if(prog_ptr != NULL) {
    memcpy(prog_ptr, &audfmt_prog_def_default,
                                  sizeof(audfmt_hq_prog_def_struct_type));
  }
} /* end audfmt_dls_reset_prog_def */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_prog_final_init

DESCRIPTION
  This function does the final initialization of program definition after
  all articulation commands have been applied.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_prog_final_init (
  audfmt_hq_prog_def_struct_type *prog_ptr
) {
  if(prog_ptr != NULL) {
    prog_ptr->aeg_attack1_level = getAegAttackLevelIndex(
                                          prog_ptr->aeg_attack1_time,
                                          prog_ptr->aeg_decay2_time,
                                          prog_ptr->aeg_sustain_level);

    prog_ptr->peg_attack1_level = getPegAttackLevelIndex(
                                          prog_ptr->peg_attack1_time,
                                          prog_ptr->peg_decay2_time,
                                          prog_ptr->peg_sustain_level,
                                          prog_ptr->peg_attack1_level);

    prog_ptr->aeg_initial_level = getAegInitialLevelIndex(
                                          prog_ptr->aeg_attack1_time,
                                          prog_ptr->aeg_decay2_time,
                                          prog_ptr->aeg_sustain_level);

    prog_ptr->peg_initial_level = getPegInitialLevelIndex(
                                          prog_ptr->peg_attack1_time,
                                          prog_ptr->peg_decay2_time,
                                          prog_ptr->peg_sustain_level,
                                          prog_ptr->peg_attack1_level,
                                          prog_ptr->peg_initial_level);
  }
} /* end audfmt_dls_prog_final_init */

#endif /* not FEATURE_AUDIO_CMX_5P0 */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_read_length

DESCRIPTION
  This function decodes a length field from a DLS file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 audfmt_dls_read_length (
  uint8 *buffer
) {
  uint32 ret_val;

  ret_val  = (buffer[3] << 24) | (buffer[2] << 16) |
             (buffer[1] << 8)  | (buffer[0]);

  return(ret_val);
} /* end audfmt_dls_read_length */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_get_byte

DESCRIPTION
  This function gets the next byte from DLS buffers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint8 audfmt_dls_get_byte (
  audfmt_dls_state_type *state,
  uint32                *index
) {
  uint8 ret_val;

  if(*index < state->length_buf1) {
    ret_val  = state->buf1[*index];
  } else {
    ret_val  = state->buf2[*index - state->length_buf1];
  }

  *index += 1;
  return(ret_val);
} /* end audfmt_dls_get_byte */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_get_word

DESCRIPTION
  This function gets the next word from DLS buffers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint16 audfmt_dls_get_word (
  audfmt_dls_state_type *state,
  uint32                *index
) {
  uint16 ret_val;

  ret_val  = audfmt_dls_get_byte(state, index);
  ret_val |= (audfmt_dls_get_byte(state, index) << 8);

  return(ret_val);
} /* end audfmt_dls_get_word */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_get_int

DESCRIPTION
  This function gets the next integer from DLS buffers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 audfmt_dls_get_int (
  audfmt_dls_state_type *state,
  uint32                *index
) {
  uint32 ret_val;

  ret_val  = audfmt_dls_get_byte(state, index);
  ret_val |= (audfmt_dls_get_byte(state, index) << 8);
  ret_val |= (audfmt_dls_get_byte(state, index) << 16);
  ret_val |= (audfmt_dls_get_byte(state, index) << 24);

  return(ret_val);
} /* end audfmt_dls_get_int */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_reset_dls

DESCRIPTION
  This function initializes the indicated DLS state structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_reset_dls (
  audfmt_dls_state_type *state
) {
  if(state != NULL) {
    state->state              = AUDFMT_DLS_STATE_INIT;
    state->prev_state         = AUDFMT_DLS_STATE_INVALID;
    state->data_req           = AUDFMT_DLS_CHUNK_UNRECOGNIZED;
    state->handle             = NULL;
    state->riff_length        = 0;
    state->lins_length        = 0;
    state->ins_length         = 0;
    state->lrgn_length        = 0;
    state->lart_length        = 0;
    state->rgn_length         = 0;
    state->wave_length        = 0;

    state->wvpl_position      = 0;
    state->ptbl_position      = 0;

    state->file_position    = 0;
    state->current_position = 0;
    state->index            = 0;
    state->buf1             = NULL;
    state->length_buf1      = 0;
    state->buf2             = NULL;
    state->length_buf2      = 0;
  }
} /* end audfmt_reset_dls */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_xfer_prog_def

DESCRIPTION
  This function copies entire program definition except for max_key,
  waveform_id and aeg_depth.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_xfer_prog_def (
  audfmt_hq_prog_def_struct_type *dest_ptr,
  audfmt_hq_prog_def_struct_type *src_ptr
) {
  if((dest_ptr != NULL) && (src_ptr != NULL)) {
#ifdef FEATURE_AUDIO_CMX_5P0
    dest_ptr->mod_lfo_freq                 = src_ptr->mod_lfo_freq;
    dest_ptr->mod_lfo_delay                = src_ptr->mod_lfo_delay;
    dest_ptr->vib_lfo_freq                 = src_ptr->vib_lfo_freq;
    dest_ptr->vib_lfo_delay                = src_ptr->vib_lfo_delay;
    dest_ptr->aeg_delay_time               = src_ptr->aeg_delay_time;
    dest_ptr->aeg_attack_time              = src_ptr->aeg_attack_time;
    dest_ptr->aeg_hold_time                = src_ptr->aeg_hold_time;
    dest_ptr->aeg_decay_time               = src_ptr->aeg_decay_time;
    dest_ptr->aeg_sustain_level            = src_ptr->aeg_sustain_level;
    dest_ptr->aeg_release_time             = src_ptr->aeg_release_time;
    dest_ptr->aeg_shutdown_time            = src_ptr->aeg_shutdown_time;
    dest_ptr->aeg_velocity_to_attack       = src_ptr->aeg_velocity_to_attack;
    dest_ptr->aeg_key_to_decay             = src_ptr->aeg_key_to_decay;
    dest_ptr->aeg_key_to_hold              = src_ptr->aeg_key_to_hold;
    dest_ptr->peg_delay_time               = src_ptr->peg_delay_time;
    dest_ptr->peg_attack_time              = src_ptr->peg_attack_time;
    dest_ptr->peg_hold_time                = src_ptr->peg_hold_time;
    dest_ptr->peg_decay_time               = src_ptr->peg_decay_time;
    dest_ptr->peg_sustain_level            = src_ptr->peg_sustain_level;
    dest_ptr->peg_release_time             = src_ptr->peg_release_time;
    dest_ptr->peg_velocity_to_attack       = src_ptr->peg_velocity_to_attack;
    dest_ptr->peg_key_to_decay             = src_ptr->peg_key_to_decay;
    dest_ptr->peg_key_to_hold              = src_ptr->peg_key_to_hold;
    dest_ptr->note_to_key                  = src_ptr->note_to_key;
    dest_ptr->rpn2_to_key                  = src_ptr->rpn2_to_key;
    dest_ptr->tvf_fc                       = src_ptr->tvf_fc;
    dest_ptr->tvf_resonance                = src_ptr->tvf_resonance;
    dest_ptr->mod_lfo_to_fc                = src_ptr->mod_lfo_to_fc;
    dest_ptr->mod_lfo_cc1_to_fc            = src_ptr->mod_lfo_cc1_to_fc;
    dest_ptr->mod_lfo_ch_pressure_to_fc    = src_ptr->mod_lfo_ch_pressure_to_fc;
    dest_ptr->peg_to_fc                    = src_ptr->peg_to_fc;
    dest_ptr->velocity_to_fc               = src_ptr->velocity_to_fc;
    dest_ptr->key_to_fc                    = src_ptr->key_to_fc;
    dest_ptr->mod_lfo_to_gain              = src_ptr->mod_lfo_to_gain;
    dest_ptr->mod_lfo_cc1_to_gain          = src_ptr->mod_lfo_cc1_to_gain;
    dest_ptr->mod_lfo_ch_pressure_to_gain  = src_ptr->mod_lfo_ch_pressure_to_gain;
    dest_ptr->velocity_to_gain             = src_ptr->velocity_to_gain;
    dest_ptr->cc7_to_gain                  = src_ptr->cc7_to_gain;
    dest_ptr->cc11_to_gain                 = src_ptr->cc11_to_gain;
    dest_ptr->fine_tune                    = src_ptr->fine_tune;
    dest_ptr->pitch_wheel_rpn0_to_pitch    = src_ptr->pitch_wheel_rpn0_to_pitch;
    dest_ptr->key_to_pitch                 = src_ptr->key_to_pitch;
    dest_ptr->rpn1_to_pitch                = src_ptr->rpn1_to_pitch;
    dest_ptr->vib_lfo_to_pitch             = src_ptr->vib_lfo_to_pitch;
    dest_ptr->vib_lfo_cc1_to_pitch         = src_ptr->vib_lfo_cc1_to_pitch;
    dest_ptr->vib_lfo_ch_pressure_to_pitch = src_ptr->vib_lfo_ch_pressure_to_pitch;
    dest_ptr->mod_lfo_to_pitch             = src_ptr->mod_lfo_to_pitch;
    dest_ptr->mod_lfo_cc1_to_pitch         = src_ptr->mod_lfo_cc1_to_pitch;
    dest_ptr->mod_lfo_ch_pressure_to_pitch = src_ptr->mod_lfo_ch_pressure_to_pitch;
    dest_ptr->peg_to_pitch                 = src_ptr->peg_to_pitch;
    dest_ptr->pan                          = src_ptr->pan;
    dest_ptr->cc10_to_pan                  = src_ptr->cc10_to_pan;
    dest_ptr->cc91_to_reverb_send          = src_ptr->cc91_to_reverb_send;
    dest_ptr->reverb_send                  = src_ptr->reverb_send;
    dest_ptr->cc93_to_chorus_send          = src_ptr->cc93_to_chorus_send;
    dest_ptr->chorus_send                  = src_ptr->chorus_send;
#else
    dest_ptr->detune            = src_ptr->detune;
    dest_ptr->course            = src_ptr->course;
    dest_ptr->flw_key           = src_ptr->flw_key;
    dest_ptr->vel_offset        = src_ptr->vel_offset;
    dest_ptr->lfo_mod_sw        = src_ptr->lfo_mod_sw;
    dest_ptr->key_flw_type      = src_ptr->key_flw_type;
    dest_ptr->lfo1_freq         = src_ptr->lfo1_freq;
    dest_ptr->lfo1_type         = src_ptr->lfo1_type;
    dest_ptr->lfo1_feg_depth    = src_ptr->lfo1_feg_depth;
    dest_ptr->lfo1_peg_depth    = src_ptr->lfo1_peg_depth;
    dest_ptr->lfo1_delay        = src_ptr->lfo1_delay;
    dest_ptr->lfo1_aeg_depth    = src_ptr->lfo1_aeg_depth;
    dest_ptr->lfo2_freq         = src_ptr->lfo2_freq;
    dest_ptr->lfo2_type         = src_ptr->lfo2_type;
    dest_ptr->lfo2_feg_depth    = src_ptr->lfo2_feg_depth;
    dest_ptr->lfo2_peg_depth    = src_ptr->lfo2_peg_depth;
    dest_ptr->lfo2_delay        = src_ptr->lfo2_delay;
    dest_ptr->lfo2_aeg_depth    = src_ptr->lfo2_aeg_depth;
    dest_ptr->tvf_fc            = src_ptr->tvf_fc;
    dest_ptr->tvf_type          = src_ptr->tvf_type;
    dest_ptr->tvf_flw_key       = src_ptr->tvf_flw_key;
    dest_ptr->tvf_resonance     = src_ptr->tvf_resonance;
    dest_ptr->peg_attack1_time  = src_ptr->peg_attack1_time;
    dest_ptr->tvf_key_flw_type  = src_ptr->tvf_key_flw_type;
    dest_ptr->peg_release_time  = src_ptr->peg_release_time;
    dest_ptr->peg_decay2_time   = src_ptr->peg_decay2_time;
    dest_ptr->peg_attack1_level = src_ptr->peg_attack1_level;
    dest_ptr->peg_initial_level = src_ptr->peg_initial_level;
    dest_ptr->peg_release_level = src_ptr->peg_release_level;
    dest_ptr->peg_sustain_level = src_ptr->peg_sustain_level;
    dest_ptr->feg_attack1_time  = src_ptr->feg_attack1_time;
    dest_ptr->peg_depth         = src_ptr->peg_depth;
    dest_ptr->feg_release_time  = src_ptr->feg_release_time;
    dest_ptr->feg_decay2_time   = src_ptr->feg_decay2_time;
    dest_ptr->feg_attack1_level = src_ptr->feg_attack1_level;
    dest_ptr->feg_initial_level = src_ptr->feg_initial_level;
    dest_ptr->feg_release_level = src_ptr->feg_release_level;
    dest_ptr->feg_sustain_level = src_ptr->feg_sustain_level;
    dest_ptr->aeg_attack1_time  = src_ptr->aeg_attack1_time;
    dest_ptr->feg_depth         = src_ptr->feg_depth;
    dest_ptr->aeg_release_time  = src_ptr->aeg_release_time;
    dest_ptr->aeg_decay2_time   = src_ptr->aeg_decay2_time;
    dest_ptr->aeg_attack1_level = src_ptr->aeg_attack1_level;
    dest_ptr->aeg_initial_level = src_ptr->aeg_initial_level;
    dest_ptr->aeg_sustain_level = src_ptr->aeg_sustain_level;
#endif /* FEATURE_AUDIO_CMX_5P0 */
  }
} /* end audfmt_dls_xfer_prog_def */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_prog_resort

DESCRIPTION
  This function resorts program definitions by increasing order of max_key.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_prog_resort (
  audfmt_dls_state_type *state
) {
  audfmt_inst_def_type           *inst_ptr;
  audfmt_prog_def_ctl_type       *prog_ctl, *current_ctl;
  audfmt_hq_prog_def_struct_type *prog_ptr, *current_prog;
  uint32 index;

  if(state != NULL) {
    inst_ptr = state->inst_def_ptr;
    if((inst_ptr->current_region > 0) && (!inst_ptr->drum)) {
      current_prog = &inst_ptr->prog_def[inst_ptr->current_region];
      prog_ptr     = current_prog - 1;
      index        = inst_ptr->current_region;
      while((index > 0) && (prog_ptr->max_key > current_prog->max_key)) {
        prog_ptr--;
        index--;
      }
      prog_ptr++;

      if(prog_ptr != current_prog) {
        memcpy(audfmt_dls_scratch, current_prog,
                                     sizeof(audfmt_hq_prog_def_struct_type));
        do {
          memcpy(current_prog, current_prog-1,
                                       sizeof(audfmt_hq_prog_def_struct_type));
          current_prog--;
        } while(current_prog != prog_ptr);

        memcpy(current_prog, audfmt_dls_scratch,
                             sizeof(audfmt_hq_prog_def_struct_type));

        current_ctl = &inst_ptr->prog_ctl[inst_ptr->current_region];
        prog_ctl    = &inst_ptr->prog_ctl[index];

        memcpy(audfmt_dls_scratch, current_ctl,
                                     sizeof(audfmt_prog_def_ctl_type));
        do {
          memcpy(current_ctl, current_ctl-1,
                                       sizeof(audfmt_prog_def_ctl_type));
          current_ctl--;
        } while(current_ctl != prog_ctl);

        memcpy(current_ctl, audfmt_dls_scratch,
                             sizeof(audfmt_prog_def_ctl_type));
      }
    }
  }
} /* end audfmt_dls_prog_resort */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_ins_final

DESCRIPTION
  This function verifies and finalizes the current instrument settings.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_ins_final (
  audfmt_dls_state_type *state
) {
  audfmt_inst_def_type           *inst_ptr;
  audfmt_prog_def_ctl_type       *prog_ctl;
  audfmt_hq_prog_def_struct_type *prog_ptr;
  boolean drum_active = FALSE;
  uint32  counter;
  uint8   *buffer;

  if(state != NULL) {
    inst_ptr = state->start_inst;

    if(inst_ptr->buffer == NULL) {
      inst_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
    } else if(inst_ptr->drum) {
      buffer = inst_ptr->buffer;
      while(inst_ptr != NULL) {
        prog_ptr = inst_ptr->prog_def;
        prog_ctl = inst_ptr->prog_ctl;
        if(!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WLNK_DONE_FLAG) ||
           !(prog_ctl->prog_mask & AUDFMT_PROG_DEF_RGNH_DONE_FLAG)) {
          inst_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
        } else {
          drum_active         = TRUE;
          inst_ptr->drum_note = prog_ptr->max_key;
          if(!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_ART_DONE_FLAG)) {
            audfmt_dls_xfer_prog_def(prog_ptr, &state->inst_prog_def);
          }
        }

        inst_ptr = (audfmt_inst_def_type *) inst_ptr->next_inst_def;
      }

      if(!drum_active) {
        state->handle->data_func(SND_CMX_AF_FREE_VAR_BUF,
                                 state->handle->audfmt_ptr, 0, buffer);
      }
    } else if(inst_ptr->num_regions != inst_ptr->current_region) {
      inst_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
      
      state->handle->data_func(SND_CMX_AF_FREE_VAR_BUF,
                               state->handle->audfmt_ptr, 0,
                               inst_ptr->buffer);
    } else {
      prog_ptr = inst_ptr->prog_def;
      prog_ctl = inst_ptr->prog_ctl;
      counter  = inst_ptr->num_regions;
      while(counter--) {
        if(!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WLNK_DONE_FLAG) ||
           !(prog_ctl->prog_mask & AUDFMT_PROG_DEF_RGNH_DONE_FLAG)) {
          inst_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
      
          state->handle->data_func(SND_CMX_AF_FREE_VAR_BUF,
                                   state->handle->audfmt_ptr, 0,
                                   inst_ptr->buffer);
          break;
        } else if(!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_ART_DONE_FLAG)) {
          audfmt_dls_xfer_prog_def(prog_ptr, &state->inst_prog_def);
        }
        prog_ptr++;
        prog_ctl++;
      }

      inst_ptr->prog_def[inst_ptr->num_regions - 1].max_key = 127;
    }
  }
} /* end audfmt_dls_ins_final */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_free_state_struct

DESCRIPTION
  This function frees the indicated DLS state structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_free_state_struct (
  audfmt_dls_state_type *state
) {
  uint32 counter;

  if(state != NULL) {
    for(counter = 0; counter < AUDFMT_DLS_NUM_STATE_STRUCT; counter++) {
      if(state == &audfmt_dls_state[counter]) {
        state->state  = AUDFMT_DLS_STATE_INVALID;
        state->handle = NULL;
        break;
      }
    }
  }
} /* end audfmt_dls_free_state_struct */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_get_state_struct

DESCRIPTION
  This function gets a DLS state structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_dls_state_type *audfmt_dls_get_state_struct ()
{
  audfmt_dls_state_type *ret_val = NULL;
  uint32                counter;

  for(counter = 0; counter < AUDFMT_DLS_NUM_STATE_STRUCT; counter++) {
    if(audfmt_dls_state[counter].handle == NULL) {
      ret_val = &audfmt_dls_state[counter];
      audfmt_reset_dls(ret_val);
      break;
    }
  }

  return(ret_val);
} /* end audfmt_dls_get_state_struct */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_release_sequence

DESCRIPTION
  This function frees all DLS resources that have been allocated for
  the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_release_sequence (
  snd_af_access_type *handle,
  uint32             sequence
) {
  audfmt_prog_def_in_use_type   res, in_use;
  audfmt_hq_waveform_info_type  *wave_info;
  audfmt_wave_ctl_type          *wave_ctl;
  audfmt_inst_def_type          *inst_ptr, *drum_ptr;
  uint32                        counter, counter2;

  res      = audfmt_prog_def_seq[sequence];
  in_use   = audfmt_prog_def_in_use_seq[sequence];
  inst_ptr = audfmt_dls_table.inst_def;
  counter  = AUDFMT_NUM_INST_DEF;
  while(counter--) {
    if((inst_ptr->in_use_flag == res) || (inst_ptr->in_use_flag == in_use)) {
      if(inst_ptr->buffer != NULL) {
        handle->data_func(SND_CMX_AF_FREE_VAR_BUF, handle->audfmt_ptr,
                          0, inst_ptr->buffer);
        inst_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
      }
    }
    inst_ptr++;
  }

  drum_ptr = audfmt_dls_table.drum_def;
  for(counter = 0; counter < AUDFMT_NUM_INST_DEF; counter++) {
    if((drum_ptr->in_use_flag == res) || (drum_ptr->in_use_flag == in_use)) {
      if(drum_ptr->buffer != NULL) {
        handle->data_func(SND_CMX_AF_FREE_VAR_BUF, handle->audfmt_ptr,
                          0, drum_ptr->buffer);
        drum_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
        inst_ptr = drum_ptr + 1;
        for(counter2 = (counter + 1); counter2 < AUDFMT_NUM_INST_DEF;
            counter2++) {
          if((inst_ptr->in_use_flag == res) ||
             (inst_ptr->in_use_flag == in_use)) {
            if(inst_ptr->buffer == drum_ptr->buffer) {
              inst_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
            }
          }
          inst_ptr++;
        }
      }
    }
    drum_ptr++;
  }

  wave_ctl = audfmt_dls_table.wave_ctl;
  for(counter = 0; counter < AUDFMT_NUM_WAVE_DEF; counter++) {
    if(wave_ctl->in_use_flag == in_use) {
      wave_info = &audfmt_dls_table.wave_info[counter];
#ifdef FEATURE_AUDIO_CMX_5P0
      counter2  = (uint32) wave_info->wave_addr;
#else
      counter2  = (wave_info->wave_addr_msw << 16) | wave_info->wave_addr_lsw;
#endif /* FEATURE_AUDIO_CMX_5P0 */
      handle->data_func(SND_CMX_AF_FREE_VAR_BUF, handle->audfmt_ptr,
                        0, (void *) counter2);

      wave_ctl->in_use_flag = AUDFMT_PROG_DEF_FREE;
    } else if(wave_ctl->in_use_flag == res) {
      wave_ctl->in_use_flag = AUDFMT_PROG_DEF_FREE;
    }
    wave_ctl++;
  }
} /* end audfmt_dls_release_sequence */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_release_reserved

DESCRIPTION
  This function frees all DLS resources that have been reserved for
  the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_release_reserved (
  snd_af_access_type *handle,
  uint32             sequence
) {
  audfmt_prog_def_in_use_type   res, in_use;
  audfmt_wave_ctl_type          *wave_ctl;
  audfmt_inst_def_type          *inst_ptr, *drum_ptr;
  uint32                        counter, counter2;
  boolean                       release;

  res      = audfmt_prog_def_seq[sequence];
  in_use   = audfmt_prog_def_in_use_seq[sequence];
  inst_ptr = audfmt_dls_table.inst_def;
  counter  = AUDFMT_NUM_INST_DEF;
  while(counter--) {
    if(inst_ptr->in_use_flag == res) {
      if(inst_ptr->buffer != NULL) {
        handle->data_func(SND_CMX_AF_FREE_VAR_BUF, handle->audfmt_ptr,
                          0, inst_ptr->buffer);
        inst_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
      }
    }
    inst_ptr++;
  }

  drum_ptr = audfmt_dls_table.drum_def;
  counter  = AUDFMT_NUM_INST_DEF;
  while(counter--) {
    if(drum_ptr->in_use_flag == res) {
      if(drum_ptr->buffer != NULL) {
        drum_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
        release  = TRUE;
        inst_ptr = audfmt_dls_table.drum_def;
        counter2 = AUDFMT_NUM_INST_DEF;
        while(counter2--) {
          if(inst_ptr->buffer == drum_ptr->buffer) {
            if(inst_ptr->in_use_flag == in_use) {
              release  = FALSE;
            } else if(inst_ptr->in_use_flag == res) {
              inst_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
            }
          }
          inst_ptr++;
        }
        if(release) {
          handle->data_func(SND_CMX_AF_FREE_VAR_BUF, handle->audfmt_ptr,
                            0, drum_ptr->buffer);
        }
      }
    }
    drum_ptr++;
  }

  wave_ctl = audfmt_dls_table.wave_ctl;
  counter  = AUDFMT_NUM_WAVE_DEF;
  while(counter--) {
    if(wave_ctl->in_use_flag == res) {
      wave_ctl->in_use_flag = AUDFMT_PROG_DEF_FREE;
    }
    wave_ctl++;
  }
} /* end audfmt_dls_release_reserved */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_get_inst_def

DESCRIPTION
  This function retrieves a free instrument definition entry.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_inst_def_type *audfmt_dls_get_inst_def ()
{
  audfmt_inst_def_type *ret_val = NULL, *inst_def;
  uint32               counter;

  inst_def = audfmt_dls_table.inst_def;
  counter  = AUDFMT_NUM_INST_DEF;
  while(counter--) {
    if(inst_def->in_use_flag == AUDFMT_PROG_DEF_FREE) {
      inst_def->buffer        = NULL;
      inst_def->prog_ctl      = NULL;
      inst_def->prog_def      = NULL;
      inst_def->next_inst_def = NULL;
      ret_val                 = inst_def;
      break;
    }
    inst_def++;
  }

  return(ret_val);
} /* end audfmt_dls_get_inst_def */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_get_drum_def

DESCRIPTION
  This function retrieves a free drum definition entry.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_inst_def_type *audfmt_dls_get_drum_def ()
{
  audfmt_inst_def_type *ret_val = NULL, *inst_def;
  uint32               counter;

  inst_def = audfmt_dls_table.drum_def;
  counter  = AUDFMT_NUM_INST_DEF;
  while(counter--) {
    if(inst_def->in_use_flag == AUDFMT_PROG_DEF_FREE) {
      inst_def->buffer        = NULL;
      inst_def->prog_ctl      = NULL;
      inst_def->prog_def      = NULL;
      inst_def->next_inst_def = NULL;
      ret_val                 = inst_def;
      break;
    }
    inst_def++;
  }

  return(ret_val);
} /* end audfmt_dls_get_drum_def */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_get_wave_ctl

DESCRIPTION
  This function retrieves a free wave control entry.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_wave_ctl_type *audfmt_dls_get_wave_ctl (
  uint32 *index
) {
  audfmt_wave_ctl_type *ret_val = NULL, *wave_ctl;
  uint32               counter;

  wave_ctl = audfmt_dls_table.wave_ctl;
  for(counter = 0; counter < AUDFMT_NUM_WAVE_DEF; counter++) {
    if(wave_ctl->in_use_flag == AUDFMT_PROG_DEF_FREE) {
      ret_val = wave_ctl;
      *index  = counter;
      break;
    }
    wave_ctl++;
  }

  return(ret_val);
} /* end audfmt_dls_get_wave_ctl */

/* <EJECT> */
#ifdef FEATURE_AUDIO_CMX_5P0
/*===========================================================================

FUNCTION audfmt_dls_waveform_to_signed

DESCRIPTION
  This function converts an 8bit unsigned waveform to signed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_waveform_to_signed (
  uint8  *buffer,
  uint32 size
) {
  uint8 tbyte, *buffer_1 = buffer + 1;

  while(size > 1) {
    tbyte     = *buffer - 0x80;
    *buffer   = *buffer_1 - 0x80;
    *buffer_1 = tbyte;
    buffer   += 2;
    buffer_1 += 2;
    size     -= 2;
  }

  if(size != 0) {
    *buffer_1 = *buffer - 0x80;
    *buffer   = 0;
  }
  return(AUDFMT_STATUS_SUCCESS);
} /* end audfmt_dls_waveform_to_signed */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_waveform_compress

DESCRIPTION
  This function converts a 16bit waveform to 8bit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_waveform_compress (
  uint8  *buffer,
  uint32 size
) {
  uint8 *src_1, *src_2;

  src_1  = buffer + 1;
  src_2  = buffer + 3;
  while(size > 1) {
    *buffer++ = *src_2;
    *buffer++ = *src_1;
    src_1  += 4;
    src_2  += 4;
    size   -= 2;
  }

  if(size != 0) {
    *(buffer+1) = *src_1;
    *(buffer)   = 0;
  }
  return(AUDFMT_STATUS_SUCCESS);
} /* end audfmt_dls_waveform_compress */

/* <EJECT> */

#else /* FEATURE_AUDIO_CMX_5P0 */
/*===========================================================================

FUNCTION audfmt_dls_waveform_to_signed

DESCRIPTION
  This function converts an 8bit unsigned waveform to signed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_waveform_to_signed (
  uint8  *buffer,
  uint32 *length,
  uint16 *loop_start,
  uint16 *loop_length
) {
  uint32 index, size = *length;
  uint8 tbyte, *buffer_1 = buffer + 1;

  if((*loop_start >= size) || (*loop_length == 0)) {
    *loop_start     = size;
    *loop_length    = 2;
    buffer[size]    = 0x80;
    buffer[size+1]  = 0x80;
    size           += 2;
  } else if((*loop_start + *loop_length) >= size) {
    *loop_length = size - *loop_start;
  }

  if(*loop_length == 1) {
    buffer[*loop_start+1] = buffer[*loop_start];
    buffer[*loop_start+2] = buffer[*loop_start];
    buffer[*loop_start+3] = buffer[*loop_start];
  } else if(*loop_length == 2) {
    buffer[*loop_start+2] = buffer[*loop_start];
    buffer[*loop_start+3] = buffer[*loop_start+1];
    buffer[*loop_start+4] = buffer[*loop_start];
  } else {
    index = *loop_start + *loop_length;
    buffer[index++] = buffer[*loop_start];
    buffer[index++] = buffer[*loop_start+1];
    buffer[index]   = buffer[*loop_start+2];
  }
  size    += 3;
  *length  = size;

  while(size > 1) {
    tbyte     = *buffer - 0x80;
    *buffer   = *buffer_1 - 0x80;
    *buffer_1 = tbyte;
    buffer   += 2;
    buffer_1 += 2;
    size     -= 2;
  }

  if(size != 0) {
    *buffer_1 = *buffer - 0x80;
    *buffer   = 0;
  }
  return(AUDFMT_STATUS_SUCCESS);
} /* end audfmt_dls_waveform_to_signed */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_waveform_compress

DESCRIPTION
  This function converts a 16bit waveform to 8bit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_waveform_compress (
  uint8  *buffer,
  uint32 *length,
  uint16 *loop_start,
  uint16 *loop_length
) {
  uint32 index, index2, size = *length;
  uint8 *src_1, *src_2;

  if((*loop_start >= size) || (*loop_length == 0)) {
    *loop_start     = size;
    *loop_length    = 2;
    index           = size << 1;
    buffer[index++] = 0;
    buffer[index++] = 0;
    buffer[index++] = 0;
    buffer[index]   = 0;
    size           += 2;
  } else if((*loop_start + *loop_length) >= size) {
    *loop_length = size - *loop_start;
  }

  if(*loop_length == 1) {
    index  = (*loop_start << 1) + 2;
    index2 = *loop_start << 1;
    buffer[index++] = buffer[index2];
    buffer[index++] = buffer[index2+1];
    buffer[index++] = buffer[index2];
    buffer[index++] = buffer[index2+1];
    buffer[index++] = buffer[index2];
    buffer[index]   = buffer[index2+1];
  } else if(*loop_length == 2) {
    index  = (*loop_start << 1) + 4;
    index2 = *loop_start << 1;
    buffer[index++] = buffer[index2];
    buffer[index++] = buffer[index2+1];
    buffer[index++] = buffer[index2+2];
    buffer[index++] = buffer[index2+3];
    buffer[index++] = buffer[index2];
    buffer[index]   = buffer[index2+1];
  } else {
    index  = (*loop_start + *loop_length) << 1;
    index2 = *loop_start << 1;
    buffer[index++] = buffer[index2++];
    buffer[index++] = buffer[index2++];
    buffer[index++] = buffer[index2++];
    buffer[index++] = buffer[index2++];
    buffer[index++] = buffer[index2++];
    buffer[index]   = buffer[index2];
  }
  size    += 3;
  *length  = size;

  src_1  = buffer + 1;
  src_2  = buffer + 3;
  while(size > 1) {
    *buffer++ = *src_2;
    *buffer++ = *src_1;
    src_1  += 4;
    src_2  += 4;
    size   -= 2;
  }

  if(size != 0) {
    *(buffer+1) = *src_1;
    *(buffer)   = 0;
  }
  return(AUDFMT_STATUS_SUCCESS);
} /* end audfmt_dls_waveform_compress */

#endif /* FEATURE_AUDIO_CMX_5P0 */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_get_data_cb_req

DESCRIPTION
  This function handles data callbacks for the DLS processing unit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_get_data_cb_req (
  void      *client_data,
  uint8     *buf,
  uint32    length,
  uint32    position
) {
  audfmt_dls_state_type *state = (audfmt_dls_state_type *) client_data;

  if((state == &audfmt_dls_state[0]) || (state == &audfmt_dls_state[1]) ||
     (state == &audfmt_dls_state[2]) || (state == &audfmt_dls_state[3]) ||
     (state == &audfmt_dls_state[4])) {
    if(state->state == AUDFMT_DLS_STATE_RELEASE) {
      if(state->handle != NULL) {
        if((state->prev_state == AUDFMT_DLS_STATE_LRGN_INIT) ||
           (state->prev_state == AUDFMT_DLS_STATE_PTBL) ||
           (state->prev_state == AUDFMT_DLS_STATE_WAVE_DATA)) {
          state->handle->data_func(SND_CMX_AF_FREE_VAR_BUF,
                                   state->handle->audfmt_ptr, 0, buf);
        } else {
          state->handle->data_func(SND_CMX_AF_FREE_BUF,
                                   state->handle->audfmt_ptr, 0, buf);
        }
        if(state->buf1 != NULL) {
          state->handle->data_func(SND_CMX_AF_FREE_BUF,
                                   state->handle->audfmt_ptr, 0, state->buf1);
        }

        if(state->buf2 != NULL) {
          state->handle->data_func(SND_CMX_AF_FREE_BUF,
                                   state->handle->audfmt_ptr, 0, state->buf2);
        }      
        state->handle->data_func(SND_CMX_AF_DLS_DONE, state->handle->audfmt_ptr,
                                 0, NULL);
      }
      audfmt_dls_free_state_struct(state);
    } else {
      if(state->state == AUDFMT_DLS_STATE_LRGN_INIT) {
        state->inst_def_ptr->buffer = buf;
      } else if(state->buf1 == NULL) {
        state->length_buf1   = length;
        state->buf1          = buf;
        state->file_position = position + length;
        state->index         = 0;

        if(((length == 0) || (buf == NULL)) &&
           (state->state != AUDFMT_DLS_STATE_PTBL) &&
           (state->state != AUDFMT_DLS_STATE_WAVE_INIT) &&
           (state->state != AUDFMT_DLS_STATE_WAVE_DATA)) {
          state->state = AUDFMT_DLS_STATE_ERROR;
        }
      } else if(state->buf2 == NULL) {
        state->length_buf2   = length;
        state->buf2          = buf;
        state->file_position = position + length;

        if((length == 0) || (buf == NULL)) {
          state->state = AUDFMT_DLS_STATE_ERROR;
        }
      }
      audfmt_process_dls(state);
    }
  }
} /* end audfmt_dls_get_data_cb_req */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_check_buffer

DESCRIPTION
  This function checks the DLS buffers to see if more data is required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_check_buffer (
  audfmt_dls_state_type *state
) {
  snd_af_read_cb_type read_cb;
  audfmt_status_type  ret_val = AUDFMT_STATUS_SUCCESS;

  if(state->current_position > state->file_position) {
    if(state->buf1 != NULL) {
      state->handle->data_func(SND_CMX_AF_FREE_BUF,
                               state->handle->audfmt_ptr, 0, state->buf1);
      state->buf1        = NULL;
      state->length_buf1 = 0;
    }

    if(state->buf2 != NULL) {
      state->handle->data_func(SND_CMX_AF_FREE_BUF,
                               state->handle->audfmt_ptr, 0, state->buf2);
      state->buf2        = NULL;
      state->length_buf2 = 0;
    }

    read_cb.client_data = state;
    audmain_dls_get_data(SND_CMX_AF_READ, state->handle,
                         state->current_position, &read_cb);

    ret_val = AUDFMT_STATUS_BUFF_ERR;
  } else if(state->data_req == AUDFMT_DLS_INSUFFICIENT_DATA) {
    state->data_req = AUDFMT_DLS_CHUNK_UNRECOGNIZED;

    read_cb.client_data = state;
    audmain_dls_get_data(SND_CMX_AF_READ, state->handle,
                         state->file_position, &read_cb);

    ret_val = AUDFMT_STATUS_BUFF_ERR;
  } else if(state->index > state->length_buf1) {
    state->handle->data_func(SND_CMX_AF_FREE_BUF,
                             state->handle->audfmt_ptr, 0, state->buf1);
    state->index       -= state->length_buf1;
    state->buf1         = state->buf2;
    state->length_buf1  = state->length_buf2;
    state->buf2         = NULL;
    state->length_buf2  = 0;
  }

  return(ret_val);
} /* end audfmt_dls_check_buffer */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_get_next_chunk

DESCRIPTION
  This function returns the chunk ID and length of the next chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_dls_chunk_enum_type audfmt_dls_get_next_chunk (
  audfmt_dls_state_type *state,
  uint32                *chunk_length,
  uint32                *total_length
) {
  audfmt_dls_chunk_enum_type ret_val;
  uint32                     length, buf1_len;
  uint8                      buffer[12];
  
  length   = state->length_buf1 + state->length_buf2 - state->index;

  if(length >= 8) {
    buf1_len = ((state->length_buf1 - state->index) < 12) ?
                    (state->length_buf1 - state->index):12;
    memcpy(buffer, &state->buf1[state->index], buf1_len);
    if((buf1_len < 12) && (state->buf2 != NULL)) {
      memcpy(&buffer[buf1_len], state->buf2, 12-buf1_len);
    }

    if(memcmp(buffer, "LIST", 4) == 0) {
      if(length >= 12) {
        *chunk_length = audfmt_dls_read_length(buffer + 4);

        if(*chunk_length & 0x1) {
          *total_length = *chunk_length + 1;
        } else {
          *total_length = *chunk_length;
        }

        if(memcmp(buffer + 8, "lins", 4) == 0) {
          ret_val = AUDFMT_DLS_LIST_LINS;
        } else if(memcmp(buffer + 8, "wvpl", 4) == 0) {
          ret_val = AUDFMT_DLS_LIST_WVPL;
        } else if(memcmp(buffer + 8, "ins ", 4) == 0) {
          ret_val = AUDFMT_DLS_LIST_INS;
        } else if(memcmp(buffer + 8, "lrgn", 4) == 0) {
          ret_val = AUDFMT_DLS_LIST_LRGN;
        } else if(memcmp(buffer + 8, "lart", 4) == 0) {
          ret_val = AUDFMT_DLS_LIST_LART;
        } else if(memcmp(buffer + 8, "lar2", 4) == 0) {
          ret_val = AUDFMT_DLS_LIST_LART;
        } else if(memcmp(buffer + 8, "rgn ", 4) == 0) {
          ret_val = AUDFMT_DLS_LIST_RGN;
        } else if(memcmp(buffer + 8, "rgn2", 4) == 0) {
          ret_val = AUDFMT_DLS_LIST_RGN;
        } else {
          ret_val = AUDFMT_DLS_LIST_UNRECOGNIZED;
        }
      } else {
        ret_val = AUDFMT_DLS_INSUFFICIENT_DATA;
      }
    } else {
      if(memcmp(buffer, "ptbl", 4) == 0) {
        ret_val = AUDFMT_DLS_CHUNK_PTBL;
      } else if(memcmp(buffer, "insh", 4) == 0) {
        ret_val = AUDFMT_DLS_CHUNK_INSH;
      } else if(memcmp(buffer, "rgnh", 4) == 0) {
        ret_val = AUDFMT_DLS_CHUNK_RGNH;
      } else if(memcmp(buffer, "wsmp", 4) == 0) {
        ret_val = AUDFMT_DLS_CHUNK_WSMP;
      } else if(memcmp(buffer, "wlnk", 4) == 0) {
        ret_val = AUDFMT_DLS_CHUNK_WLNK;
      } else if(memcmp(buffer, "art1", 4) == 0) {
        ret_val = AUDFMT_DLS_CHUNK_ART1;
      } else if(memcmp(buffer, "art2", 4) == 0) {
        ret_val = AUDFMT_DLS_CHUNK_ART2;
      } else if(memcmp(buffer, "fmt ", 4) == 0) {
        ret_val = AUDFMT_DLS_CHUNK_FMT;
      } else if(memcmp(buffer, "data", 4) == 0) {
        ret_val = AUDFMT_DLS_CHUNK_DATA;
      } else {
        ret_val = AUDFMT_DLS_CHUNK_UNRECOGNIZED;
      }
      *chunk_length = audfmt_dls_read_length(buffer + 4);

      if(*chunk_length & 0x1) {
        *total_length = *chunk_length + 1;
      } else {
        *total_length = *chunk_length;
      }
    }
  } else {
    ret_val = AUDFMT_DLS_INSUFFICIENT_DATA;
  }

  return(ret_val);
} /* end audfmt_dls_get_next_chunk */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_check_next_ins

DESCRIPTION
  This function allocates a new drum def for each drum region, if necessary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_check_next_ins (
  audfmt_dls_state_type *state
) {
  audfmt_status_type   ret_val = AUDFMT_STATUS_FAILURE;
  audfmt_inst_def_type *current_inst, *next_inst;

  current_inst = state->inst_def_ptr;
  if((!current_inst->drum) || (current_inst->current_region == 0)) {
    ret_val = AUDFMT_STATUS_SUCCESS;
  } else {
    next_inst = audfmt_dls_get_drum_def();
    if(next_inst != NULL) {
      next_inst->in_use_flag      = audfmt_prog_def_seq[state->sequence];
      next_inst->drum             = TRUE;
      next_inst->bank_msb         = current_inst->bank_msb;
      next_inst->bank_lsb         = current_inst->bank_lsb;
      next_inst->program          = current_inst->program;
      next_inst->num_regions      = current_inst->num_regions;
      next_inst->current_region   = current_inst->current_region;
      next_inst->buffer           = current_inst->buffer;
      next_inst->prog_ctl         = current_inst->prog_ctl + 1;
      next_inst->prog_def         = current_inst->prog_def + 1;

      current_inst->next_inst_def = next_inst;
      state->inst_def_ptr         = next_inst;

      ret_val = AUDFMT_STATUS_SUCCESS;
    }
  }

  return(ret_val);
} /* end audfmt_dls_check_next_ins */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_drum_find

DESCRIPTION
  This function searches for a drum definition that is using the indicated
  buffer; if buffer is not in use then the buffer will be freed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_drum_find (
  audfmt_dls_state_type *state,
  uint8                 *buffer
) {
  audfmt_inst_def_type *drum_ptr;
  boolean found = FALSE;
  uint32  counter;

  drum_ptr = audfmt_dls_table.drum_def;
  counter  = AUDFMT_NUM_INST_DEF;
  while(counter--) {
    if((drum_ptr->in_use_flag == audfmt_prog_def_seq[state->sequence]) &&
       (drum_ptr->buffer == buffer)) {
      found = TRUE;
      break;
    }
    drum_ptr++;
  }

  if(!found) {
    state->handle->data_func(SND_CMX_AF_FREE_VAR_BUF,
                             state->handle->audfmt_ptr, 0, buffer);
  }
} /* end audfmt_dls_drum_find */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_process_lrgn

DESCRIPTION
  This function processes the lrgn chunk; a buffer is allocated that will
  be sufficient for all regions for this instrument.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_process_lrgn (
  audfmt_dls_state_type *state,
  uint32                chunk_length
) {
  audfmt_status_type  ret_val = AUDFMT_STATUS_FAILURE;
  snd_af_read_cb_type read_cb;
  uint32              size;

  if((state->inst_def_ptr != NULL) && (state->inst_def_ptr->num_regions > 0)) {
    size = state->inst_def_ptr->num_regions *
                        (sizeof(audfmt_hq_prog_def_struct_type) +
                                      sizeof(audfmt_prog_def_ctl_type));

    read_cb.client_data = state;
    read_cb.buffer_size = size;
    read_cb.offset      = size;
    audmain_dls_get_data(SND_CMX_AF_READ_VAR_DLS, state->handle, 0, &read_cb);

    ret_val = AUDFMT_STATUS_SUCCESS;
  }

  return(ret_val);
} /* end audfmt_dls_process_lrgn */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_process_lrgn_init

DESCRIPTION
  This function initializes the current instrument in preparation to
  process lrgn chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_process_lrgn_init (
  audfmt_dls_state_type *state
) {
  audfmt_status_type             ret_val = AUDFMT_STATUS_FAILURE;
  audfmt_inst_def_type           *inst_ptr;
  audfmt_prog_def_ctl_type       *prog_ctl;
  audfmt_hq_prog_def_struct_type *prog_ptr;
  uint32                         counter;

  if((state != NULL) && (state->inst_def_ptr != NULL) &&
     (state->inst_def_ptr->buffer != NULL)) {
    inst_ptr = state->inst_def_ptr;

    inst_ptr->current_region = 0;
    inst_ptr->prog_ctl       = (audfmt_prog_def_ctl_type *) inst_ptr->buffer;
    inst_ptr->prog_def       = (audfmt_hq_prog_def_struct_type *)
                                  (inst_ptr->prog_ctl + inst_ptr->num_regions);

    prog_ctl = inst_ptr->prog_ctl;
    prog_ptr = inst_ptr->prog_def;
    counter  = inst_ptr->num_regions;
    while(counter--) {
      prog_ctl->prog_mask   = 0;
      prog_ctl->loop_start  = 0;
      prog_ctl->loop_length = 0;
      prog_ctl++;

      audfmt_dls_reset_prog_def(prog_ptr++);
    }

    ret_val = AUDFMT_STATUS_SUCCESS;
  }

  return(ret_val);
} /* end audfmt_dls_process_lrgn_init */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_process_insh

DESCRIPTION
  This function processes the insh chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_process_insh (
  audfmt_dls_state_type *state,
  uint32                chunk_length
) {
  audfmt_inst_def_type *inst_ptr;
  audfmt_status_type   ret_val = AUDFMT_STATUS_BUFF_ERR;
  uint32               buf_size, index, tlong;

  buf_size = state->length_buf1 + state->length_buf2 - state->index;

  if(buf_size >= (chunk_length + 8)) {
    index    = state->index + 8;
    buf_size = audfmt_dls_get_int(state, &index);
    tlong    = audfmt_dls_get_int(state, &index);

    inst_ptr = audfmt_dls_get_inst_def();

    if(inst_ptr != NULL) {
      inst_ptr->in_use_flag = audfmt_prog_def_seq[state->sequence];
      inst_ptr->num_regions = (uint8) buf_size;
      inst_ptr->bank_msb    = (uint8) ((tlong >> 8) & 0x7f);
      inst_ptr->bank_lsb    = (uint8) (tlong & 0x7f);
      inst_ptr->drum        = 0;

      tlong = audfmt_dls_get_int(state, &index);
      inst_ptr->program  = (uint8) (tlong & 0x7f);

      state->inst_def_ptr = inst_ptr;
      state->start_inst   = inst_ptr;
    }
    ret_val = AUDFMT_STATUS_SUCCESS;
  }
  return(ret_val);
} /* end audfmt_dls_process_insh */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_process_rgnh

DESCRIPTION
  This function processes the rgnh chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_process_rgnh (
  audfmt_dls_state_type *state,
  uint32                chunk_length
) {
  audfmt_inst_def_type           *inst_ptr;
  audfmt_hq_prog_def_struct_type *prog_ptr;
  audfmt_prog_def_ctl_type       *prog_ctl;
  uint32                         buf_size, index, tlong;

  buf_size = state->length_buf1 + state->length_buf2 - state->index;

  if(buf_size >= (chunk_length + 8)) {
    index = state->index + 8;
    tlong = audfmt_dls_get_int(state, &index);

    inst_ptr = state->inst_def_ptr;
    if(inst_ptr->drum) {
      prog_ptr = inst_ptr->prog_def;
      prog_ctl = inst_ptr->prog_ctl;
    } else {
      prog_ptr = &(inst_ptr->prog_def[inst_ptr->current_region]);
      prog_ctl = &(inst_ptr->prog_ctl[inst_ptr->current_region]);
    }

#ifdef FEATURE_AUDIO_CMX_5P0
    prog_ptr->min_key    = (uint16) (tlong & 0xffff);
    prog_ptr->max_key    = (uint16) ((tlong >> 16) & 0xffff);
#else
    prog_ptr->max_key    = (uint8) ((tlong >> 16) & 0xffff);
#endif /* FEATURE_AUDIO_CMX_5P0 */
    prog_ctl->prog_mask |= AUDFMT_PROG_DEF_RGNH_DONE_FLAG;

    return(AUDFMT_STATUS_SUCCESS);
  } else {
    return(AUDFMT_STATUS_BUFF_ERR);
  }
} /* audfmt_dls_process_rgnh */

/*===========================================================================

FUNCTION audfmt_dls_process_wlnk

DESCRIPTION
  This function processes the wlnk chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_process_wlnk (
  audfmt_dls_state_type *state,
  uint32                chunk_length
) {
  audfmt_inst_def_type        *inst_ptr;
  audfmt_prog_def_ctl_type    *prog_ctl;
  uint32                      buf_size, index, tlong;

  buf_size = state->length_buf1 + state->length_buf2 - state->index;

  if(buf_size >= (chunk_length + 8)) {
    index = state->index + 8;
    tlong = audfmt_dls_get_int(state, &index);
    tlong = audfmt_dls_get_int(state, &index);
    tlong = audfmt_dls_get_int(state, &index);

    inst_ptr = state->inst_def_ptr;
    if(inst_ptr->drum) {
      prog_ctl = inst_ptr->prog_ctl;
    } else {
      prog_ctl = &(inst_ptr->prog_ctl[inst_ptr->current_region]);
    }

    prog_ctl->wave_id    = tlong;
    prog_ctl->prog_mask |= AUDFMT_PROG_DEF_WLNK_DONE_FLAG;

    return(AUDFMT_STATUS_SUCCESS);
  } else {
    return(AUDFMT_STATUS_BUFF_ERR);
  }
} /* end audfmt_dls_process_wlnk */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_process_wsmp

DESCRIPTION
  This function processes the wsmp chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_process_wsmp (
  audfmt_dls_state_type *state,
  uint32                chunk_length
) {
  audfmt_inst_def_type           *inst_ptr;
  audfmt_prog_def_ctl_type       *prog_ctl = NULL;
  audfmt_wave_ctl_type           *wave_ctl = NULL;
#ifndef FEATURE_AUDIO_CMX_5P0
  audfmt_hq_prog_def_struct_type *prog_ptr = NULL;
  uint8  aeg_depth;
#else
  uint32 gain;
#endif /* not FEATURE_AUDIO_CMX_5P0 */
  uint32 buf_size, index, tlong, loop_length;
  uint32 loop_start;
  uint16 fine_tune, unity_note;

  buf_size = state->length_buf1 + state->length_buf2 - state->index;

  if(buf_size >= (chunk_length + 8)) {
    if(state->state == AUDFMT_DLS_STATE_LIST_WAVE) {
      wave_ctl  = &audfmt_dls_table.wave_ctl[state->wave_index];
    } else {
      inst_ptr = state->inst_def_ptr;
      if(inst_ptr->drum) {
#ifndef FEATURE_AUDIO_CMX_5P0
        prog_ptr = inst_ptr->prog_def;
#endif /* not FEATURE_AUDIO_CMX_5P0 */
        prog_ctl = inst_ptr->prog_ctl;
      } else {
#ifndef FEATURE_AUDIO_CMX_5P0
        prog_ptr = &(inst_ptr->prog_def[inst_ptr->current_region]);
#endif /* not FEATURE_AUDIO_CMX_5P0 */
        prog_ctl = &(inst_ptr->prog_ctl[inst_ptr->current_region]);
      }
    }

    index      = state->index + 8;
    buf_size   = audfmt_dls_get_int(state, &index) - 20;
    unity_note = audfmt_dls_get_word(state, &index);
    fine_tune  = audfmt_dls_get_word(state, &index);
#ifdef FEATURE_AUDIO_CMX_5P0
	gain       = audfmt_dls_get_int(state, &index);
#else
    aeg_depth  = getAegDepthIndex(audfmt_dls_get_int(state, &index));
#endif /* FEATURE_AUDIO_CMX_5P0 */

#ifndef FEATURE_AUDIO_CMX_5P0
    if(prog_ptr != NULL) {
      prog_ptr->aeg_depth   = aeg_depth;
	}
#endif /* not FEATURE_AUDIO_CMX_5P0 */

	if(prog_ctl != NULL) {
#ifdef FEATURE_AUDIO_CMX_5P0
      prog_ctl->gain        = gain;
#endif /* FEATURE_AUDIO_CMX_5P0 */
      prog_ctl->prog_mask  |= AUDFMT_PROG_DEF_WSMP_DONE_FLAG;
      prog_ctl->unity_note  = unity_note;
      prog_ctl->fine_tune   = fine_tune;
    }

    if(wave_ctl != NULL) {
#ifdef FEATURE_AUDIO_CMX_5P0
	  wave_ctl->gain        = gain;
#else
      wave_ctl->aeg_depth   = aeg_depth;
#endif /* not FEATURE_AUDIO_CMX_5P0 */
      wave_ctl->wave_mask  |= AUDFMT_WAVE_CTL_WSMP_DONE_FLAG;
      wave_ctl->unity_note  = unity_note;
      wave_ctl->fine_tune   = fine_tune;
    }

    tlong = audfmt_dls_get_int(state, &index);
    while(buf_size) {
      (void) audfmt_dls_get_byte(state, &index);
      buf_size--;
    }
    tlong = audfmt_dls_get_int(state, &index);

    if(tlong == 1) {
      buf_size = audfmt_dls_get_int(state, &index);
      if(buf_size > 16) {
        buf_size -= 16;
      } else {
        buf_size = 0;
      }
      tlong       = audfmt_dls_get_int(state, &index);
      loop_start  = audfmt_dls_get_int(state, &index);
      loop_length = audfmt_dls_get_int(state, &index);

      if(prog_ctl != NULL) {
#ifdef FEATURE_AUDIO_CMX_5P0
        prog_ctl->loop_type    = tlong;
#endif /* FEATURE_AUDIO_CMX_5P0 */
        prog_ctl->loop_start   = loop_start;
        prog_ctl->loop_length  = loop_length;
      }

      if(wave_ctl != NULL) {
#ifdef FEATURE_AUDIO_CMX_5P0
        wave_ctl->loop_type   = tlong;
#endif /* FEATURE_AUDIO_CMX_5P0 */
        wave_ctl->loop_start  = loop_start;
        wave_ctl->loop_length = loop_length;
      }

      /* Not necessary if array is always size 1 */
      while(buf_size) {
        (void) audfmt_dls_get_byte(state, &index);
        buf_size--;
      }
    }

    return(AUDFMT_STATUS_SUCCESS);
  } else {
    return(AUDFMT_STATUS_BUFF_ERR);
  }
} /* end audfmt_dls_process_wsmp */

/* <EJECT> */

#ifdef FEATURE_AUDIO_CMX_5P0
/*===========================================================================

FUNCTION audfmt_dls_process_art

DESCRIPTION
  This function processes the art1 and art2 chunks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_process_art (
  audfmt_dls_chunk_enum_type  chunk_id,
  audfmt_dls_state_type       *state,
  uint32                      chunk_length
) {
  audfmt_inst_def_type           *inst_ptr;
  audfmt_hq_prog_def_struct_type *prog_ptr;
  audfmt_prog_def_ctl_type       *prog_ctl;

  uint32  buf_size, index, tlong;
  uint16  src, cont, dest, trans;

  buf_size = state->length_buf1 + state->length_buf2 - state->index;

  if(buf_size >= (chunk_length + 8)) {
    if(state->prev_state == AUDFMT_DLS_STATE_LIST_INS) {
      prog_ptr            = &state->inst_prog_def;
    } else {
      inst_ptr            = state->inst_def_ptr;
      if(inst_ptr->drum) {
        prog_ptr = inst_ptr->prog_def;
        prog_ctl = inst_ptr->prog_ctl;
      } else {
        prog_ptr = &(inst_ptr->prog_def[inst_ptr->current_region]);
        prog_ctl = &(inst_ptr->prog_ctl[inst_ptr->current_region]);
      }

      prog_ctl->prog_mask |= AUDFMT_PROG_DEF_ART_DONE_FLAG;
    }

    index = state->index + 8;
    buf_size = audfmt_dls_get_int(state, &index) - 8;
    while(buf_size) {
      (void) audfmt_dls_get_byte(state, &index);
      buf_size--;
    }
    buf_size = audfmt_dls_get_int(state, &index);

    while(buf_size--) {
      src   = audfmt_dls_get_word(state, &index);
      cont  = audfmt_dls_get_word(state, &index);
      dest  = audfmt_dls_get_word(state, &index);
      trans = audfmt_dls_get_word(state, &index);
      tlong = audfmt_dls_get_int(state, &index);
      if(trans == 0) {
        if((src == 0) && (cont == 0)) {
          switch(dest) {
            case 0x03:
              prog_ptr->fine_tune         = tlong;
              break;

            case 0x04:
              prog_ptr->pan               = tlong;
              break;

            case 0x80:
              prog_ptr->chorus_send       = tlong;
              break;

            case 0x81:
              prog_ptr->reverb_send       = tlong;
              break;

            case 0x104:
              prog_ptr->mod_lfo_freq      = tlong;
              break;

            case 0x105:
              prog_ptr->mod_lfo_delay     = tlong;
              break;

            case 0x114:
              prog_ptr->vib_lfo_freq      = tlong;
              break;

            case 0x115:
              prog_ptr->vib_lfo_delay     = tlong;
              break;

            case 0x206:
              prog_ptr->aeg_attack_time   = tlong;
              break;

            case 0x207:
              prog_ptr->aeg_decay_time    = tlong;
              break;

            case 0x209:
              prog_ptr->aeg_release_time  = tlong;
              break;

            case 0x20a:
              prog_ptr->aeg_sustain_level = tlong;
              break;

            case 0x20b:
              prog_ptr->aeg_delay_time    = tlong;
              break;

            case 0x20c:
              prog_ptr->aeg_hold_time     = tlong;
              break;

            case 0x20d:
              prog_ptr->aeg_shutdown_time = tlong;
              break;

            case 0x30a:
              prog_ptr->peg_attack_time   = tlong;
              break;

            case 0x30b:
              prog_ptr->peg_decay_time    = tlong;
              break;

            case 0x30d:
              prog_ptr->peg_release_time  = tlong;
              break;

            case 0x30e:
              prog_ptr->peg_sustain_level = tlong;
              break;

            case 0x30f:
              prog_ptr->peg_delay_time    = tlong;
              break;

            case 0x310:
              prog_ptr->peg_hold_time     = tlong;
              break;

            case 0x500:
              prog_ptr->tvf_fc            = tlong;
              break;

            case 0x501:
              prog_ptr->tvf_resonance     = tlong;
              break;

            default:
              break;
          }
        } else if((src == 0x02) && (cont == 0)) {
          if(dest == 0x206) {
            prog_ptr->aeg_velocity_to_attack = tlong;
          } else if(dest == 0x30a) {
            prog_ptr->peg_velocity_to_attack = tlong;
          } else if(dest == 0x500) {
            prog_ptr->velocity_to_fc         = tlong;
          }
        } else if((src == 0x03) && (cont == 0)) {
          if (dest == 0x03) {
            prog_ptr->key_to_pitch     = tlong;
          } else if(dest == 0x207) {
            prog_ptr->aeg_key_to_decay = tlong;
          } else if(dest == 0x20c) {
            prog_ptr->aeg_key_to_hold  = tlong;
          } else if(dest == 0x30b) {
            prog_ptr->peg_key_to_decay = tlong;
          } else if(dest == 0x310) {
            prog_ptr->peg_key_to_hold  = tlong;
          } else if(dest == 0x500) {
            prog_ptr->key_to_fc        = tlong;
          }
        } else if((src == 0x05) && (cont == 0) && (dest == 0x500)) {
          prog_ptr->peg_to_fc = tlong;
        } else if(chunk_id == AUDFMT_DLS_CHUNK_ART1) {
          if(src == 0x01) {
            if(dest == 0x01) {
              if(cont == 0) {
                prog_ptr->mod_lfo_to_gain      = tlong;
              } else if(cont == 0x81) {
                prog_ptr->mod_lfo_cc1_to_gain  = tlong;
              }
            } else if(dest == 0x03) {
              if(cont == 0) {
                prog_ptr->mod_lfo_to_pitch     = tlong;
              } else if(cont == 0x81) {
                prog_ptr->mod_lfo_cc1_to_pitch = tlong;
              }
            }
          } else if((src == 0x05) && (cont == 0) && (dest == 0x03)) {
            prog_ptr->peg_to_pitch              = tlong;
          } else if((src == 0x06) && (cont == 0x100) && (dest == 0x03)) {
            prog_ptr->pitch_wheel_rpn0_to_pitch = tlong;
          } else if((src == 0x8a) && (cont == 0) && (dest == 0x04)) {
            prog_ptr->cc10_to_pan               = tlong;
          } else if((src == 0x101) && (cont == 0) && (dest == 0x03)) {
            prog_ptr->rpn1_to_pitch             = tlong;
          }
        } else if((chunk_id == AUDFMT_DLS_CHUNK_ART2) && (cont == 0)) {
          if((src == 0xdb) && (dest == 0x81)) {
            prog_ptr->cc91_to_reverb_send = tlong;
          } else if((src == 0xdd) && (dest == 0x80)) {
            prog_ptr->cc93_to_chorus_send = tlong;
          }
        }
      } else if((trans == 0x01) && (cont == 0) && (dest == 0x01) && (chunk_id == AUDFMT_DLS_CHUNK_ART1)) {
        if (src == 0x02) {
          prog_ptr->velocity_to_gain = tlong;
        } else if(src == 0x87) {
          prog_ptr->cc7_to_gain      = tlong;
        } else if(src == 0x8b) {
          prog_ptr->cc11_to_gain     = tlong;
        }
      } else if(chunk_id == AUDFMT_DLS_CHUNK_ART2) {
        if(trans == 0x4000) {
          if(src == 0x01) {
            if(cont == 0) {
              if(dest == 0x01) {
                prog_ptr->mod_lfo_to_gain  = tlong;
              } else if(dest == 0x03) {
                prog_ptr->mod_lfo_to_pitch = tlong;
              } else if(dest == 0x500) {
                prog_ptr->mod_lfo_to_fc    = tlong;
              }
            } else if(cont == 0x08) {
              if(dest == 0x03) {
                prog_ptr->mod_lfo_ch_pressure_to_pitch = tlong;
              } else if(dest == 0x500) {
                prog_ptr->mod_lfo_ch_pressure_to_fc    = tlong;
              }
            } else if(cont == 0x81) {
              if(dest == 0x01) {
                prog_ptr->mod_lfo_cc1_to_gain  = tlong;
              } else if(dest == 0x03) {
                prog_ptr->mod_lfo_cc1_to_pitch = tlong;
              } else if(dest == 0x500) {
                prog_ptr->mod_lfo_cc1_to_fc    = tlong;
          }
        }
        } else if((src == 0x05) && (cont == 0) && (dest == 0x03)) {
            prog_ptr->peg_to_pitch              = tlong;
          } else if((src == 0x06) && (cont == 0x100) && (dest == 0x03)) {
            prog_ptr->pitch_wheel_rpn0_to_pitch = tlong;
          } else if((src == 0x09) && (dest == 0x03)) {
            if(cont == 0) {
              prog_ptr->vib_lfo_to_pitch             = tlong;
            } else if(cont == 0x08) {
              prog_ptr->vib_lfo_ch_pressure_to_pitch = tlong;
            } else if(cont == 0x81) {
              prog_ptr->vib_lfo_cc1_to_pitch         = tlong;
            }
          } else if((src == 0x8a) && (cont == 0) && (dest == 0x04)) {
            prog_ptr->cc10_to_pan   = tlong;
          } else if((src == 0x101) && (cont == 0) && (dest == 0x03)) {
            prog_ptr->rpn1_to_pitch = tlong;
          } else if((src == 0x102) && (cont == 0) && (dest == 0x05)) {
            prog_ptr->rpn2_to_key   = tlong;
          }
        } else if((trans == 0x8000) && (src == 0x01) && (cont == 0x08) && (dest == 0x01)) {
          prog_ptr->mod_lfo_ch_pressure_to_gain = tlong;
        } else if((trans == 0x8400) && (cont == 0) && (dest == 0x01)) {
          if (src == 0x02) {
            prog_ptr->velocity_to_gain = tlong;
          } else if(src == 0x87) {
            prog_ptr->cc7_to_gain      = tlong;
          } else if(src == 0x8b) {
            prog_ptr->cc11_to_gain     = tlong;
          }
        }
      }
    }

    return(AUDFMT_STATUS_SUCCESS);
  } else {
    return(AUDFMT_STATUS_BUFF_ERR);
  }
} /* end audfmt_dls_process_art */

#else /* FEATURE_AUDIO_CMX_5P0 */
/*===========================================================================

FUNCTION audfmt_dls_process_art

DESCRIPTION
  This function processes the art1 and art2 chunks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_process_art (
  audfmt_dls_chunk_enum_type  chunk_id,
  audfmt_dls_state_type       *state,
  uint32                      chunk_length
) {
  audfmt_inst_def_type           *inst_ptr;
  audfmt_hq_prog_def_struct_type *prog_ptr;
  audfmt_prog_def_ctl_type       *prog_ctl;

  boolean *peg_depth_art_flag;
  uint32  *peg_depth_art_value;
  uint32  buf_size, index, tlong;
  uint16  src, cont, dest, trans;

  buf_size = state->length_buf1 + state->length_buf2 - state->index;

  if(buf_size >= (chunk_length + 8)) {
    if(state->prev_state == AUDFMT_DLS_STATE_LIST_INS) {
      prog_ptr            = &state->inst_prog_def;
      peg_depth_art_flag  = &state->peg_depth_art_flag;
      peg_depth_art_value = &state->peg_depth_art_value;
    } else {
      inst_ptr            = state->inst_def_ptr;
      peg_depth_art_flag  = &inst_ptr->peg_depth_art_flag;
      peg_depth_art_value = &inst_ptr->peg_depth_art_value;
      if(inst_ptr->drum) {
        prog_ptr = inst_ptr->prog_def;
        prog_ctl = inst_ptr->prog_ctl;
      } else {
        prog_ptr = &(inst_ptr->prog_def[inst_ptr->current_region]);
        prog_ctl = &(inst_ptr->prog_ctl[inst_ptr->current_region]);
      }

      prog_ctl->prog_mask |= AUDFMT_PROG_DEF_ART_DONE_FLAG;
    }

    index = state->index + 8;
    buf_size = audfmt_dls_get_int(state, &index) - 8;
    while(buf_size) {
      (void) audfmt_dls_get_byte(state, &index);
      buf_size--;
    }
    buf_size = audfmt_dls_get_int(state, &index);

    while(buf_size--) {
      src   = audfmt_dls_get_word(state, &index);
      cont  = audfmt_dls_get_word(state, &index);
      dest  = audfmt_dls_get_word(state, &index);
      trans = audfmt_dls_get_word(state, &index);
      tlong = audfmt_dls_get_int(state, &index);
      if(trans == 0) {
        if((src == 0) && (cont == 0)) {
          switch(dest) {
            case 0x105:
              prog_ptr->lfo1_delay        = getTimeIndex(tlong);
              break;

            case 0x104:
              prog_ptr->lfo1_freq         = getLfoFreqIndex(tlong);
              break;

            case 0x115:
              prog_ptr->lfo2_delay        = getTimeIndex(tlong);
              break;

            case 0x114:
              prog_ptr->lfo2_freq         = getLfoFreqIndex(tlong);
              break;

            case 0x206:
              prog_ptr->aeg_attack1_time  = getTimeIndex(tlong);
              break;

            case 0x207:
              prog_ptr->aeg_decay2_time   = getTimeIndex(tlong);
              break;

            case 0x209:
              prog_ptr->aeg_release_time  = getTimeIndex(tlong);
              break;

            case 0x20a:
              prog_ptr->aeg_sustain_level = getAegSustainLevelIndex(tlong);
              break;

            case 0x30a:
              prog_ptr->peg_attack1_time  = getTimeIndex(tlong);
              break;

            case 0x30b:
              prog_ptr->peg_decay2_time   = getTimeIndex(tlong);
              break;

            case 0x30d:
              prog_ptr->peg_release_time  = getTimeIndex(tlong);
              break;

            case 0x30e:
              prog_ptr->peg_sustain_level = getPegSustainLevelIndex(tlong);
              break;
            default:
              break;
          }
        } else if((src == 0x02) && (cont == 0)) {
          if(dest == 0x206) {
            prog_ptr->feg_depth = getAttackDecayScale(tlong);
          } else if(dest == 0x30a) {
            prog_ptr->feg_release_level = getAttackDecayScale(tlong);
          }
        } else if((src == 0x03) && (cont == 0)) {
          if(dest == 0x207) {
            prog_ptr->feg_initial_level = getAttackDecayScale(tlong);
          } else if(dest == 0x30b) {
            prog_ptr->feg_attack1_level = getAttackDecayScale(tlong);
          }
        } else if((src == 0x05) && (cont == 0) && (dest == 0x03)) {
          *peg_depth_art_flag  = TRUE;
          *peg_depth_art_value = tlong;
        } else if(chunk_id == AUDFMT_DLS_CHUNK_ART1) {
          if((src == 0x01) && (cont == 0) && (dest == 0x01)) {
            prog_ptr->lfo1_aeg_depth = getLfoAegDepthIndex(tlong);
          } else if((src == 0x01) && (cont == 0) && (dest == 0x03)) {
            prog_ptr->lfo1_peg_depth = getLfoPegDepthIndex(tlong);
          }
        }
      } else if((trans == 0x4000) && (chunk_id == AUDFMT_DLS_CHUNK_ART2)) {
        if((src == 0x01) && (cont == 0) && (dest == 0x01)) {
          prog_ptr->lfo1_aeg_depth = getLfoAegDepthIndex(tlong);
        } else if((src == 0x01) && (cont == 0) && (dest == 0x03)) {
          prog_ptr->lfo1_peg_depth = getLfoPegDepthIndex(tlong);
        } else if((src == 0x05) && (cont == 0) && (dest == 0x03)) {
          *peg_depth_art_flag  = TRUE;
          *peg_depth_art_value = tlong;
        }
      }
    }

    return(AUDFMT_STATUS_SUCCESS);
  } else {
    return(AUDFMT_STATUS_BUFF_ERR);
  }
} /* end audfmt_dls_process_art */
#endif /* FEATURE_AUDIO_CMX_5P0 */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_setup_ptbl

DESCRIPTION
  This function sets up to read ptbl chunk into memory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_setup_ptbl (
  audfmt_dls_state_type *state
) {
  snd_af_read_cb_type read_cb;
  audfmt_status_type  ret_val = AUDFMT_STATUS_FAILURE;

  if(state->ptbl_position != 0) {
    if(state->buf1 != NULL) {
      state->handle->data_func(SND_CMX_AF_FREE_BUF,
                               state->handle->audfmt_ptr, 0, state->buf1);
      state->buf1        = NULL;
      state->length_buf1 = 0;
    }

    if(state->buf2 != NULL) {
      state->handle->data_func(SND_CMX_AF_FREE_BUF,
                               state->handle->audfmt_ptr, 0, state->buf2);
      state->buf2        = NULL;
      state->length_buf2 = 0;
    }

    read_cb.client_data = state;
    read_cb.buffer_size = state->ptbl_length;
    read_cb.offset      = 0;
    audmain_dls_get_data(SND_CMX_AF_READ_VAR_DLS, state->handle,
                         state->ptbl_position, &read_cb);

    ret_val = AUDFMT_STATUS_SUCCESS;
  }
  return(ret_val);
} /* end audfmt_dls_setup_ptbl */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_process_ptbl

DESCRIPTION
  This function looks up waveform offsets for each region in ptbl.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_process_ptbl (
  audfmt_dls_state_type *state
) {
  audfmt_inst_def_type     *inst_ptr, *drum_ptr;
  audfmt_prog_def_ctl_type *prog_ctl;
  audfmt_status_type       ret_val = AUDFMT_STATUS_FAILURE;
  uint32                   counter, counter2, tlong, size, cues;
  uint8                    *buf;

  if((state->buf1 == NULL) || (state->length_buf1 == 0)) {
    if(state->buf1 != NULL) {
      state->handle->data_func(SND_CMX_AF_FREE_VAR_BUF,
                               state->handle->audfmt_ptr, 0, state->buf1);
      state->buf1 = NULL;
    }
  } else {
    buf   = state->buf1;
    size  = *buf++;
    size |= (*buf++ << 8);
    size |= (*buf++ << 16);
    size |= (*buf++ << 24);

    buf += (size - 8);

    cues  = *buf++;
    cues |= (*buf++ << 8);
    cues |= (*buf++ << 16);
    cues |= (*buf << 24);

    if((cues << 2) <= state->length_buf1) {
      inst_ptr = audfmt_dls_table.inst_def;
      drum_ptr = audfmt_dls_table.drum_def;
      counter  = AUDFMT_NUM_INST_DEF;
      while(counter--) {
        if(inst_ptr->in_use_flag == audfmt_prog_def_seq[state->sequence]) {
          prog_ctl = inst_ptr->prog_ctl;
          counter2 = inst_ptr->num_regions;
          while(counter2--) {
            if(prog_ctl->wave_id <= cues) {
              tlong  = (prog_ctl->wave_id << 2) + size;
              buf    = &state->buf1[tlong];
              tlong  = *buf++;
              tlong |= (*buf++ << 8);
              tlong |= (*buf++ << 16);
              tlong |= (*buf << 24);

              prog_ctl->wave_id = tlong + state->wvpl_position;
            } else {
              inst_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
              state->handle->data_func(SND_CMX_AF_FREE_VAR_BUF,
                                       state->handle->audfmt_ptr, 0,
                                       inst_ptr->buffer);
              break;
            }
            prog_ctl++;
          }
        }
        if(drum_ptr->in_use_flag == audfmt_prog_def_seq[state->sequence]) {
          prog_ctl = drum_ptr->prog_ctl;
          if(prog_ctl->wave_id <= cues) {
            tlong  = (prog_ctl->wave_id << 2) + size;
            buf    = &state->buf1[tlong];
            tlong  = *buf++;
            tlong |= (*buf++ << 8);
            tlong |= (*buf++ << 16);
            tlong |= (*buf << 24);

            prog_ctl->wave_id = tlong + state->wvpl_position;
          } else {
            drum_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;
            audfmt_dls_drum_find(state, drum_ptr->buffer);
          }
        }
        inst_ptr++;
        drum_ptr++;
      }

      state->handle->data_func(SND_CMX_AF_FREE_VAR_BUF,
                               state->handle->audfmt_ptr, 0, state->buf1);
      state->buf1 = NULL;
      
      ret_val = AUDFMT_STATUS_SUCCESS;
    }
  }
  return(ret_val);
} /* end audfmt_dls_process_ptbl */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_wave_cleanup

DESCRIPTION
  This function frees data buffers that were allocated to process
  wave list chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_wave_cleanup (
  audfmt_dls_state_type *state
) {
  if(state->buf1 != NULL) {
    state->handle->data_func(SND_CMX_AF_FREE_BUF,
                             state->handle->audfmt_ptr, 0, state->buf1);

    state->buf1        = NULL;
    state->length_buf1 = 0;
  }

  if(state->buf2 != NULL) {
    state->handle->data_func(SND_CMX_AF_FREE_BUF,
                             state->handle->audfmt_ptr, 0, state->buf2);

    state->buf2        = NULL;
    state->length_buf2 = 0;
  }
} /* end audfmt_dls_wave_cleanup */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_wave_fail

DESCRIPTION
  This function frees resources for the current wave.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_wave_fail (
  audfmt_dls_state_type *state
) {
  audfmt_prog_def_in_use_type in_use;
  audfmt_prog_def_ctl_type    *prog_ctl;
  audfmt_inst_def_type        *inst_ptr, *drum_ptr;
  uint32                      counter, counter2;

  audfmt_dls_wave_cleanup(state);

  in_use   = audfmt_prog_def_seq[state->sequence];
  inst_ptr = audfmt_dls_table.inst_def;
  drum_ptr = audfmt_dls_table.drum_def;
  counter  = AUDFMT_NUM_INST_DEF;
  while(counter--) {
    if(inst_ptr->in_use_flag == in_use) {
      prog_ctl = inst_ptr->prog_ctl;
      counter2 = inst_ptr->num_regions;
      while(counter2--) {
        if(prog_ctl->wave_id == state->wave_position) {
          inst_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;

          state->handle->data_func(SND_CMX_AF_FREE_VAR_BUF,
                                   state->handle->audfmt_ptr, 0,
                                   inst_ptr->buffer);
          break;
        }
        prog_ctl++;
      }
    }
    if(drum_ptr->in_use_flag == in_use) {
      prog_ctl = drum_ptr->prog_ctl;
      if(prog_ctl->wave_id == state->wave_position) {
        drum_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;

        audfmt_dls_drum_find(state, drum_ptr->buffer);
      }
    }
    inst_ptr++;
    drum_ptr++;
  }

  if(audfmt_dls_table.wave_ctl[state->wave_index].in_use_flag == in_use) {
    audfmt_dls_table.wave_ctl[state->wave_index].in_use_flag = 
                                                         AUDFMT_PROG_DEF_FREE;
  }
} /* end audfmt_dls_wave_fail */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_wave_free_similar

DESCRIPTION
  This function frees all regions/instruments that are using the current
  waveform.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_wave_free_similar (
  audfmt_dls_state_type *state
) {
  audfmt_inst_def_type     *inst_ptr;
  audfmt_prog_def_ctl_type *prog_ctl;
  audfmt_wave_ctl_type     *wave_ctl;
  uint32                   counter, counter2;

  wave_ctl = &audfmt_dls_table.wave_ctl[state->wave_index];
  counter  = AUDFMT_NUM_INST_DEF;
  if(!(wave_ctl->wave_mask & AUDFMT_WAVE_CTL_DRUM_WAVEFORM)) {
    inst_ptr = audfmt_dls_table.inst_def;
    while(counter--) {
      if(inst_ptr->in_use_flag == audfmt_prog_def_seq[state->sequence]) {
        prog_ctl = inst_ptr->prog_ctl;
        counter2 = inst_ptr->num_regions;
        while(counter2--) {
          if((!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WAVE_DONE_FLAG)) &&
             (state->wave_position == prog_ctl->wave_id) &&
             (((!state->wave_param_enabled) &&
               (!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WSMP_DONE_FLAG))) ||
              (state->wave_param_enabled &&
#ifdef FEATURE_AUDIO_CMX_5P0
               (prog_ctl->gain == state->gain) &&
#endif /* FEATURE_AUDIO_CMX_5P0 */
               (prog_ctl->unity_note == state->unity_note) &&
               (prog_ctl->fine_tune == state->fine_tune)   &&
               (prog_ctl->loop_start == state->loop_start) &&
               (prog_ctl->loop_length == state->loop_length)))) {
            inst_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;

            state->handle->data_func(SND_CMX_AF_FREE_VAR_BUF,
                                     state->handle->audfmt_ptr, 0,
                                     inst_ptr->buffer);
            break;
          }
          prog_ctl++;
        }
      }
      inst_ptr++;
    }
  } else {
    inst_ptr = audfmt_dls_table.drum_def;
    while(counter--) {
      if(inst_ptr->in_use_flag == audfmt_prog_def_seq[state->sequence]) {
        prog_ctl = inst_ptr->prog_ctl;
        if((!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WAVE_DONE_FLAG)) &&
           (state->wave_position == prog_ctl->wave_id) &&
           (((!state->wave_param_enabled) &&
             (!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WSMP_DONE_FLAG))) ||
            (state->wave_param_enabled &&
#ifdef FEATURE_AUDIO_CMX_5P0
             (prog_ctl->gain == state->gain) &&
#endif /* FEATURE_AUDIO_CMX_5P0 */
             (prog_ctl->unity_note == state->unity_note) &&
             (prog_ctl->fine_tune == state->fine_tune)   &&
             (prog_ctl->loop_start == state->loop_start) &&
             (prog_ctl->loop_length == state->loop_length)))) {
          inst_ptr->in_use_flag = AUDFMT_PROG_DEF_FREE;

          audfmt_dls_drum_find(state, inst_ptr->buffer);
        }
      }
      inst_ptr++;
    }
  }
} /* end audfmt_dls_wave_free_similar */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_setup_wave

DESCRIPTION
  This function searches for the next waveform to retrieve and kicks off
  the process.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_setup_wave (
  audfmt_dls_state_type *state
) {
  audfmt_prog_def_in_use_type in_use;
  audfmt_inst_def_type        *inst_ptr;
  audfmt_prog_def_ctl_type    *prog_ctl;
  audfmt_wave_ctl_type        *wave_ctl;
  snd_af_read_cb_type         read_cb;
  audfmt_status_type          ret_val = AUDFMT_STATUS_FAILURE;
  uint32                      counter, counter2;

  in_use   = audfmt_prog_def_seq[state->sequence];
  inst_ptr = audfmt_dls_table.inst_def;
  for(counter = 0; counter < AUDFMT_NUM_INST_DEF; counter++) {
    if(inst_ptr->in_use_flag == in_use) {
      prog_ctl = inst_ptr->prog_ctl;
      for(counter2 = 0; counter2 < inst_ptr->num_regions; counter2++) {
        if(!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WAVE_DONE_FLAG)) {
          break;
        }
        prog_ctl++;
      }

      if(counter2 < inst_ptr->num_regions) {
        wave_ctl = audfmt_dls_get_wave_ctl(&state->wave_index);

        if(wave_ctl != NULL) {
          wave_ctl->in_use_flag   = in_use;
          wave_ctl->wave_mask     = 0;
          wave_ctl->loop_start    = 0;
          wave_ctl->loop_length   = 0;

          if(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WSMP_DONE_FLAG) {
            state->wave_param_enabled = TRUE;
#ifdef FEATURE_AUDIO_CMX_5P0
            state->gain               = prog_ctl->gain;
#endif /* FEATURE_AUDIO_CMX_5P0 */
            state->unity_note         = prog_ctl->unity_note;
            state->fine_tune          = prog_ctl->fine_tune;
            state->loop_start         = prog_ctl->loop_start;
            state->loop_length        = prog_ctl->loop_length;
          } else {
            state->wave_param_enabled = FALSE;
          }
          state->wave_position      = prog_ctl->wave_id;
          state->wave_data_position = 0;
          state->wave_data_length   = 0;

          state->current_position = state->wave_position;

          read_cb.client_data = state;
          audmain_dls_get_data(SND_CMX_AF_READ, state->handle,
                               state->current_position, &read_cb);

          ret_val = AUDFMT_STATUS_SUCCESS;
        }
        break;
      } else {
        inst_ptr->in_use_flag = audfmt_prog_def_in_use_seq[state->sequence];
#ifdef FEATURE_AUDIO_CMX_5P0
		audfmt_dls_table.inst_rec[counter].num_regions = inst_ptr->num_regions;
		audfmt_dls_table.inst_rec[counter].addr = inst_ptr->prog_def;
#else
        audfmt_dls_table.inst_addr[counter] =
                                     ((uint32) inst_ptr->prog_def) >> 1;
#endif /* FEATURE_AUDIO_CMX_5P0 */
      }
    }
    inst_ptr++;
  }

  if(ret_val == AUDFMT_STATUS_FAILURE) {
    inst_ptr = audfmt_dls_table.drum_def;
    for(counter = 0; counter < AUDFMT_NUM_INST_DEF; counter++) {
      if(inst_ptr->in_use_flag == in_use) {
        prog_ctl = inst_ptr->prog_ctl;
        if(!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WAVE_DONE_FLAG)) {
          wave_ctl = audfmt_dls_get_wave_ctl(&state->wave_index);

          if(wave_ctl != NULL) {
            wave_ctl->in_use_flag = in_use;
            wave_ctl->wave_mask   = AUDFMT_WAVE_CTL_DRUM_WAVEFORM;
            wave_ctl->loop_start  = 0;
            wave_ctl->loop_length = 0;

            if(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WSMP_DONE_FLAG) {
              state->wave_param_enabled = TRUE;
#ifdef FEATURE_AUDIO_CMX_5P0
              state->gain               = prog_ctl->gain;
#endif /* FEATURE_AUDIO_CMX_5P0 */
              state->unity_note         = prog_ctl->unity_note;
              state->fine_tune          = prog_ctl->fine_tune;
              state->loop_start         = prog_ctl->loop_start;
              state->loop_length        = prog_ctl->loop_length;
            } else {
              state->wave_param_enabled = FALSE;
            }
            state->wave_position = prog_ctl->wave_id;

            state->current_position = state->wave_position;

            read_cb.client_data = state;
            audmain_dls_get_data(SND_CMX_AF_READ, state->handle,
                                 state->current_position, &read_cb);

            ret_val = AUDFMT_STATUS_SUCCESS;
          }
          break;
        } else {
          inst_ptr->in_use_flag = audfmt_prog_def_in_use_seq[state->sequence];
#ifdef FEATURE_AUDIO_CMX_5P0
		  audfmt_dls_table.drum_rec[counter].num_regions = inst_ptr->num_regions;
		  audfmt_dls_table.drum_rec[counter].addr = inst_ptr->prog_def;
#else
          audfmt_dls_table.drum_addr[counter] =
                                     ((uint32) inst_ptr->prog_def) >> 1;
#endif /* FEATURE_AUDIO_CMX_5P0 */
        }
      }
      inst_ptr++;
    }
  }

  return(ret_val);
} /* end audfmt_dls_setup_wave */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_prepare_wave

DESCRIPTION
  This function verifies the wave list chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_prepare_wave (
  audfmt_dls_state_type *state
) {
  audfmt_status_type ret_val = AUDFMT_STATUS_FAILURE;
  uint8              *buffer;

  if((state->buf1 != NULL) && (state->length_buf1 != 0)) {
    buffer = state->buf1;
    if((memcmp(buffer, "LIST", 4) == 0) &&
       (memcmp(buffer + 8, "wave", 4) == 0)) {
      state->wave_length       = audfmt_dls_read_length(buffer + 4);
      state->index            += 12;
      state->current_position += 12;

      ret_val = AUDFMT_STATUS_SUCCESS;
    }
  }

  if(ret_val == AUDFMT_STATUS_FAILURE) {
    audfmt_dls_wave_fail(state);
  }

  return(ret_val);
} /* end audfmt_dls_prepare_wave */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_process_wave_fmt

DESCRIPTION
  This function processes the fmt chunk in a wave list chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_process_wave_fmt (
  audfmt_dls_state_type *state,
  uint32                chunk_length
) {
  audfmt_status_type   ret_val = AUDFMT_STATUS_FAILURE;
  audfmt_wave_ctl_type *wave_ctl;
  uint32 buf_size, index, tlong;
  uint16 tword, format;

  buf_size = state->length_buf1 + state->length_buf2 - state->index;

  if(buf_size >= (chunk_length + 8)) {
    index  = state->index + 8;
    format = audfmt_dls_get_word(state, &index);
    tword  = audfmt_dls_get_word(state, &index);

    if((format == 1) && (tword == 1)) {
      buf_size = audfmt_dls_get_int(state, &index);
      tlong    = audfmt_dls_get_int(state, &index);
      tword    = audfmt_dls_get_word(state, &index);
      tword    = audfmt_dls_get_word(state, &index);

      if((tword == 8) || (tword == 16)) {
        wave_ctl = &audfmt_dls_table.wave_ctl[state->wave_index];
        wave_ctl->wave_mask       |= AUDFMT_WAVE_CTL_FMT_DONE_FLAG;
        wave_ctl->sampling_rate    = buf_size;
        wave_ctl->bits_per_sample  = tword;
        ret_val                    = AUDFMT_STATUS_SUCCESS;
      }
    }
  } else {
    ret_val = AUDFMT_STATUS_BUFF_ERR;
  }

  return(ret_val);
} /* end audfmt_dls_process_wave_fmt */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_process_wave_data

DESCRIPTION
  This function processes the data chunk in a wave list chunk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_process_wave_data (
  audfmt_dls_state_type *state,
  uint32                chunk_length,
  uint32                position
) {
  audfmt_status_type   ret_val = AUDFMT_STATUS_FAILURE;
  audfmt_wave_ctl_type *wave_ctl;
  snd_af_read_cb_type  read_cb;
  uint32               samples;

  wave_ctl = &audfmt_dls_table.wave_ctl[state->wave_index];
  if(!(wave_ctl->wave_mask & AUDFMT_WAVE_CTL_FMT_DONE_FLAG)) {
    audfmt_dls_wave_fail(state);
  } else {
    if(wave_ctl->bits_per_sample == 8) {
      samples = chunk_length;
    } else {
      samples = chunk_length >> 1;
    }

    if(samples > 16384) {
      audfmt_dls_wave_fail(state);
    } else {
      /* If wsmp chunk to be used with this waveform has not yet been
      ** encountered, return pending status and process data chunk
      ** when wsmp chunk is found.
      */ 
      if((!state->wave_param_enabled) &&
         (!(wave_ctl->wave_mask & AUDFMT_WAVE_CTL_WSMP_DONE_FLAG))) {
        wave_ctl->wave_mask |= AUDFMT_WAVE_CTL_DATA_PEND_FLAG;

        state->wave_data_length   = chunk_length;
        state->wave_data_position = position;

        ret_val = AUDFMT_STATUS_PENDING;
      } else {
        audfmt_dls_wave_cleanup(state);

        read_cb.client_data = state;
        read_cb.buffer_size = chunk_length;
        read_cb.offset      = 0;
        audmain_dls_get_data(SND_CMX_AF_READ_VAR_DLS, state->handle,
                             position, &read_cb);

        ret_val = AUDFMT_STATUS_SUCCESS;
      }
    }
  }
  return(ret_val);
} /* end audfmt_dls_process_wave_data */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_process_pending_wave_data

DESCRIPTION
  This function checks if the data chunk in a wave list chunk is ready
  to be processed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audfmt_status_type audfmt_dls_process_pending_wave_data (
  audfmt_dls_state_type *state
) {
  audfmt_status_type   ret_val;
  audfmt_wave_ctl_type *wave_ctl;

  wave_ctl = &audfmt_dls_table.wave_ctl[state->wave_index];

  if(wave_ctl->wave_mask & AUDFMT_WAVE_CTL_DATA_PEND_FLAG) {
    ret_val = audfmt_dls_process_wave_data(state, state->wave_data_length,
                                           state->wave_data_position);

    if(ret_val != AUDFMT_STATUS_SUCCESS) {
      ret_val = AUDFMT_STATUS_FAILURE;
    }
  } else {
    ret_val = AUDFMT_STATUS_PENDING;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_prepare_wave_data

DESCRIPTION
  This function formats the waveform data to be used for DLS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_dls_prepare_wave_data (
  audfmt_dls_state_type *state
) {
  audfmt_inst_def_type           *inst_ptr;
  audfmt_hq_prog_def_struct_type *prog_ptr = NULL;
  audfmt_prog_def_ctl_type       *prog_ctl;
  audfmt_wave_ctl_type           *wave_ctl;
  audfmt_hq_waveform_info_type   *wave_info;
  audfmt_status_type             status = AUDFMT_STATUS_FAILURE;
  uint32                         counter, counter2;
  uint16                         loop_start, loop_length;
#ifdef FEATURE_AUDIO_CMX_5P0
  uint32                         gain , wave_length;
  uint16                         loop_type, unity_note;
  sint15                         fine_tune;
#else
  uint32                         tune, num_samples;
#endif /* FEATURE_AUDIO_CMX_5P0 */

  wave_ctl = &audfmt_dls_table.wave_ctl[state->wave_index];
  if((state->buf1 != NULL) && (state->length_buf1 != 0)) {
    wave_ctl->wave_mask |= AUDFMT_WAVE_CTL_DATA_DONE_FLAG;

    counter  = AUDFMT_NUM_INST_DEF;
    if(!(wave_ctl->wave_mask & AUDFMT_WAVE_CTL_DRUM_WAVEFORM)) {
      inst_ptr = audfmt_dls_table.inst_def;
      while(counter--) {
        if(inst_ptr->in_use_flag == audfmt_prog_def_seq[state->sequence]) {
          prog_ctl = inst_ptr->prog_ctl;
          for(counter2 = 0; counter2 < inst_ptr->num_regions; counter2++) {
            if((!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WAVE_DONE_FLAG)) &&
               (state->wave_position == prog_ctl->wave_id) &&
               (((!state->wave_param_enabled) &&
                 (!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WSMP_DONE_FLAG))) ||
                (state->wave_param_enabled &&
#ifdef FEATURE_AUDIO_CMX_5P0
				 (prog_ctl->gain == state->gain) &&
#endif /* FEATURE_AUDIO_CMX_5P0 */
                 (prog_ctl->unity_note == state->unity_note) &&
                 (prog_ctl->fine_tune == state->fine_tune)   &&
                 (prog_ctl->loop_start == state->loop_start) &&
                 (prog_ctl->loop_length == state->loop_length)))) {
              status = AUDFMT_STATUS_SUCCESS;
              prog_ctl->prog_mask |= AUDFMT_PROG_DEF_WAVE_DONE_FLAG;

              prog_ptr              = &inst_ptr->prog_def[counter2];
#ifdef FEATURE_AUDIO_CMX_5P0
			  wave_info             = &audfmt_dls_table.wave_info[state->wave_index];
			  prog_ptr->wf_info_ptr = wave_info;
#else
              prog_ptr->waveform_id = (uint8) state->wave_index;
              if(!state->wave_param_enabled) {
                prog_ptr->aeg_depth = wave_ctl->aeg_depth;
              }
#endif /* FEATURE_AUDIO_CMX_5P0 */
            }
            prog_ctl++;
          }
        }
        inst_ptr++;
      }
    } else {
      inst_ptr = audfmt_dls_table.drum_def;
      for(counter = 0; counter < AUDFMT_NUM_INST_DEF; counter++) {
        if(inst_ptr->in_use_flag == audfmt_prog_def_seq[state->sequence]) {
          prog_ctl = inst_ptr->prog_ctl;
          if((!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WAVE_DONE_FLAG)) &&
             (state->wave_position == prog_ctl->wave_id) &&
             (((!state->wave_param_enabled) &&
               (!(prog_ctl->prog_mask & AUDFMT_PROG_DEF_WSMP_DONE_FLAG))) ||
              (state->wave_param_enabled &&
#ifdef FEATURE_AUDIO_CMX_5P0
				 (prog_ctl->gain == state->gain) &&
#endif /* FEATURE_AUDIO_CMX_5P0 */
               (prog_ctl->unity_note == state->unity_note) &&
               (prog_ctl->fine_tune == state->fine_tune)   &&
               (prog_ctl->loop_start == state->loop_start) &&
               (prog_ctl->loop_length == state->loop_length)))) {
            status = AUDFMT_STATUS_SUCCESS;
            prog_ctl->prog_mask |= AUDFMT_PROG_DEF_WAVE_DONE_FLAG;

            prog_ptr              = inst_ptr->prog_def;
#ifdef FEATURE_AUDIO_CMX_5P0
			wave_info             = &audfmt_dls_table.wave_info[state->wave_index];
			prog_ptr->wf_info_ptr = wave_info;
#else
            prog_ptr->waveform_id = (uint8) state->wave_index;
            if(!state->wave_param_enabled) {
              prog_ptr->aeg_depth = wave_ctl->aeg_depth;
            }
#endif /* FEATURE_AUDIO_CMX_5P0 */
            break;
          }
        }
        inst_ptr++;
      }
    }
  }

  if(status == AUDFMT_STATUS_FAILURE) {
    audfmt_dls_wave_free_similar(state);
    wave_ctl->in_use_flag = AUDFMT_PROG_DEF_FREE;
    if(state->buf1 != NULL) {
      state->handle->data_func(SND_CMX_AF_FREE_VAR_BUF,
                               state->handle->audfmt_ptr, 0, state->buf1);
      state->buf1 = NULL;
    }
  } else {
    wave_ctl->in_use_flag = audfmt_prog_def_in_use_seq[state->sequence];
    wave_info = &audfmt_dls_table.wave_info[state->wave_index];
#ifdef FEATURE_AUDIO_CMX_5P0
	wave_info->wave_addr = state->buf1;

	if(state->wave_param_enabled) {
	  loop_start  = (uint16) state->loop_start;
	  loop_length = (uint16) state->loop_length;
	  loop_type   = (uint16) state->loop_type;
	  gain        = state->gain;
	  unity_note  = state->unity_note;
	  fine_tune   = state->fine_tune;
	} else {
	  loop_start  = (uint16) wave_ctl->loop_start;
	  loop_length = (uint16) wave_ctl->loop_length;
	  loop_type   = (uint16) wave_ctl->loop_type;
	  gain        = wave_ctl->gain;
	  unity_note  = wave_ctl->unity_note;
	  fine_tune   = wave_ctl->fine_tune;
	}

// set the loop_type_wave_depth = 0x20 if loop_start, looplength is 0; 
    if((loop_start ==0)||(loop_length ==0))
    {
       loop_type = 0x02;
    }
	if(wave_ctl->bits_per_sample == 8) {
       wave_length = state->length_buf1;
	  status = audfmt_dls_waveform_to_signed(state->buf1, state->length_buf1);
	} else {
     wave_length = (state->length_buf1) >> 1;
	  status = audfmt_dls_waveform_compress(state->buf1, (state->length_buf1) >> 1);
	}
#else
    wave_info->wave_addr_lsw = ((uint32) state->buf1) & 0xffff;
    wave_info->wave_addr_msw = (((uint32) state->buf1) >> 16) & 0xffff;

    if(state->wave_param_enabled) {
      loop_start  = state->loop_start;
      loop_length = state->loop_length;
      tune        = getTune(((wave_ctl->wave_mask &
                              AUDFMT_WAVE_CTL_DRUM_WAVEFORM) != 0),
                              counter, prog_ptr->max_key,
                              state->unity_note, state->fine_tune,
                              wave_ctl->sampling_rate);
    } else {
      loop_start  = wave_ctl->loop_start;
      loop_length = wave_ctl->loop_length;
      tune        = getTune(((wave_ctl->wave_mask &
                              AUDFMT_WAVE_CTL_DRUM_WAVEFORM) != 0),
                              counter, prog_ptr->max_key,
                              wave_ctl->unity_note, wave_ctl->fine_tune,
                              wave_ctl->sampling_rate);
    }

    if(wave_ctl->bits_per_sample == 8) {
      num_samples = state->length_buf1;
      status = audfmt_dls_waveform_to_signed(state->buf1, &num_samples,
                                             &loop_start, &loop_length);
    } else {
      num_samples = (state->length_buf1) >> 1;
      status = audfmt_dls_waveform_compress(state->buf1, &num_samples,
                                            &loop_start, &loop_length);
    }
#endif /* FEATURE_AUDIO_CMX_5P0 */

    if(status == AUDFMT_STATUS_FAILURE) {
      audfmt_dls_wave_free_similar(state);
      wave_ctl->in_use_flag = AUDFMT_PROG_DEF_FREE;
      if(state->buf1 != NULL) {
        state->handle->data_func(SND_CMX_AF_FREE_VAR_BUF,
                                 state->handle->audfmt_ptr, 0, state->buf1);
        state->buf1 = NULL;
      }
    } else {
#ifdef FEATURE_AUDIO_CMX_5P0
      wave_info->gain                 = gain;
      wave_info->fine_tune            = fine_tune;
      wave_info->loop_type_wave_depth = (uint8) (loop_type << 4);
      wave_info->unity_note           = unity_note;
      wave_info->loop_length          = loop_length;
      wave_info->loop_start           = loop_start;
      wave_info->wave_length          = wave_length;
      wave_info->sampling_rate        = wave_ctl->sampling_rate;
#else

      wave_info->loop_start1   = loop_start;
      wave_info->loop_start2   = loop_start;
      wave_info->loop_length1  = loop_length;
      wave_info->loop_length2  = loop_length;
      wave_info->wave_tune_msw = (tune >> 16) & 0xffff;
      wave_info->wave_tune_lsw = tune & 0xffff;
      wave_info->wave_length   = num_samples;
#endif /* FEATURE_AUDIO_CMX_5P0 */

      state->buf1 = NULL;
    }
  }
} /* end audfmt_dls_prepare_wave_data */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_process_dls

DESCRIPTION
  This function is the main processing state machine for DLS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_process_dls (
  audfmt_dls_state_type *state
) {
  audfmt_hq_prog_def_struct_type *prog_ptr;
  audfmt_inst_def_type           *inst_ptr;
  audfmt_status_type             status, ret_val;
  uint32                         length, total_length;

  do {
    switch(state->state) {
      case AUDFMT_DLS_STATE_INIT:
        if((state->length_buf1 > 12) &&
           (memcmp(state->buf1, "RIFF", 4) == 0) &&
           (memcmp(state->buf1 + 8, "DLS ", 4) == 0)) {
          state->riff_length = audfmt_dls_read_length(state->buf1 + 4) - 4;
          state->current_position += 12;
          state->index             = 12;
          state->state             = AUDFMT_DLS_STATE_RIFF;
        } else {
          state->state = AUDFMT_DLS_STATE_ERROR;
        }
        break;

      case AUDFMT_DLS_STATE_RIFF:
        if(state->riff_length > 8) {
          switch(audfmt_dls_get_next_chunk(state, &length, &total_length)) {
            case AUDFMT_DLS_LIST_LINS:
              state->lins_length       = total_length - 4;
              state->state             = AUDFMT_DLS_STATE_LIST_LINS;
              state->riff_length      -= (total_length + 8);
              state->current_position += 12;
              state->index            += 12;
              break;

            case AUDFMT_DLS_LIST_WVPL:
              state->wvpl_position     = state->current_position + 12;
              state->riff_length      -= (total_length + 8);
              state->current_position += (total_length + 8);
              state->index            += (total_length + 8);
              break;

            case AUDFMT_DLS_CHUNK_PTBL:
              state->ptbl_position     = state->current_position + 8;
              state->ptbl_length       = length;
              state->riff_length      -= (total_length + 8);
              state->current_position += (total_length + 8);
              state->index            += (total_length + 8);
              break;

            case AUDFMT_DLS_LIST_UNRECOGNIZED:
            case AUDFMT_DLS_CHUNK_UNRECOGNIZED:
              state->riff_length      -= (total_length + 8);
              state->current_position += (total_length + 8);
              state->index            += (total_length + 8);
              break;

            case AUDFMT_DLS_INSUFFICIENT_DATA:
              state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              break;
          }
        } else {
          state->riff_length = 0;
          if(audfmt_dls_setup_ptbl(state) == AUDFMT_STATUS_SUCCESS) {
            state->state = AUDFMT_DLS_STATE_PTBL;
          } else {
            state->state = AUDFMT_DLS_STATE_ERROR;
          }
        }
        break;

      case AUDFMT_DLS_STATE_LIST_LINS:
        if(state->lins_length >= 8) {
          switch(audfmt_dls_get_next_chunk(state, &length, &total_length)) {
            case AUDFMT_DLS_LIST_INS:
              audfmt_dls_reset_prog_def(&state->inst_prog_def);
#ifndef FEATURE_AUDIO_CMX_5P0
              state->peg_depth_art_flag  = FALSE;
#endif /* not FEATURE_AUDIO_CMX_5P0 */
              state->inst_def_ptr        = NULL;
              state->ins_length          = total_length - 4;
              state->state               = AUDFMT_DLS_STATE_LIST_INS;
              state->lins_length        -= (total_length + 8);
              state->current_position   += 12;
              state->index              += 12;
              break;

            case AUDFMT_DLS_LIST_UNRECOGNIZED:
            case AUDFMT_DLS_CHUNK_UNRECOGNIZED:
              state->lins_length      -= (total_length + 8);
              state->current_position += (total_length + 8);
              state->index            += (total_length + 8);
              break;

            case AUDFMT_DLS_INSUFFICIENT_DATA:
              state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              break;
          }
        } else if(state->lins_length == 0) {
          state->state = AUDFMT_DLS_STATE_RIFF;
        }
        break;

      case AUDFMT_DLS_STATE_LIST_INS:
        if(state->ins_length >= 8) {
          switch(audfmt_dls_get_next_chunk(state, &length, &total_length)) {
            case AUDFMT_DLS_LIST_LRGN:
              if(audfmt_dls_process_lrgn(state, length) ==
                 AUDFMT_STATUS_SUCCESS) {
                state->lrgn_length       = total_length - 4;
                state->state             = AUDFMT_DLS_STATE_LRGN_INIT;
                state->ins_length       -= (total_length + 8);
                state->current_position += 12;
                state->index            += 12;
              } else {
                state->ins_length       -= (total_length + 8);
                state->current_position += (total_length + 8);
                state->index            += (total_length + 8);
              }
              break;

            case AUDFMT_DLS_LIST_LART:
              state->lart_length       = total_length - 4;
              state->state             = AUDFMT_DLS_STATE_LIST_LART;
              state->ins_length       -= (total_length + 8);
              state->current_position += 12;
              state->index            += 12;
              state->prev_state        = AUDFMT_DLS_STATE_LIST_INS;
              break;

            case AUDFMT_DLS_CHUNK_INSH:
              if(audfmt_dls_process_insh(state, length) ==
                 AUDFMT_STATUS_SUCCESS) {
                state->ins_length       -= (total_length + 8);
                state->current_position += (total_length + 8);
                state->index            += (total_length + 8);
              } else {
                state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              }
              break;

            case AUDFMT_DLS_LIST_UNRECOGNIZED:
            case AUDFMT_DLS_CHUNK_UNRECOGNIZED:
              state->ins_length       -= (total_length + 8);
              state->current_position += (total_length + 8);
              state->index            += (total_length + 8);
              break;

            case AUDFMT_DLS_INSUFFICIENT_DATA:
              state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              break;
          }
        } else if(state->ins_length == 0) {
#ifndef FEATURE_AUDIO_CMX_5P0
          if(state->peg_depth_art_flag) {
            state->inst_prog_def.peg_depth = getPegDepthIndex(
                                       state->peg_depth_art_value,
                                       &state->inst_prog_def.peg_sustain_level,
                                       &state->inst_prog_def.peg_attack1_level);

          }
          audfmt_dls_prog_final_init(&state->inst_prog_def);
#endif /* not FEATURE_AUDIO_CMX_5P0 */
          audfmt_dls_ins_final(state);
          state->state = AUDFMT_DLS_STATE_LIST_LINS;
        }
        break;

      case AUDFMT_DLS_STATE_LRGN_INIT:
        if(audfmt_dls_process_lrgn_init(state) == AUDFMT_STATUS_SUCCESS) {
          state->state             = AUDFMT_DLS_STATE_LIST_LRGN;
        } else {
          state->state             = AUDFMT_DLS_STATE_LIST_INS;
          state->current_position += state->lrgn_length;
          state->index            += state->lrgn_length;
          state->lrgn_length       = 0;
        }
        break;

      case AUDFMT_DLS_STATE_LIST_LRGN:
        if(state->lrgn_length >= 8) {
          switch(audfmt_dls_get_next_chunk(state, &length, &total_length)) {
            case AUDFMT_DLS_LIST_RGN:
              inst_ptr = state->inst_def_ptr;
              if((inst_ptr->current_region < inst_ptr->num_regions) &&
                 (audfmt_dls_check_next_ins(state) == AUDFMT_STATUS_SUCCESS)) {
#ifndef FEATURE_AUDIO_CMX_5P0
                inst_ptr->peg_depth_art_flag   = FALSE;
#endif /* not FEATURE_AUDIO_CMX_5P0 */
                state->rgn_length              = total_length - 4;
                state->state                   = AUDFMT_DLS_STATE_LIST_RGN;
                state->lrgn_length            -= (total_length + 8);
                state->current_position       += 12;
                state->index                  += 12;
              } else {
                state->lrgn_length      -= (total_length + 8);
                state->current_position += (total_length + 8);
                state->index            += (total_length + 8);
              }
              break;

            case AUDFMT_DLS_LIST_UNRECOGNIZED:
            case AUDFMT_DLS_CHUNK_UNRECOGNIZED:
              state->lrgn_length      -= (total_length + 8);
              state->current_position += (total_length + 8);
              state->index            += (total_length + 8);
              break;

            case AUDFMT_DLS_INSUFFICIENT_DATA:
              state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              break;
          }
        } else if(state->lrgn_length == 0) {
          state->state = AUDFMT_DLS_STATE_LIST_INS;
        }
        break;

      case AUDFMT_DLS_STATE_LIST_LART:
        if(state->lart_length >= 8) {
          switch(audfmt_dls_get_next_chunk(state, &length, &total_length)) {
            case AUDFMT_DLS_CHUNK_ART1:
              if(audfmt_dls_process_art(AUDFMT_DLS_CHUNK_ART1, state, length)
                  == AUDFMT_STATUS_SUCCESS) {
                state->lart_length      -= (total_length + 8);
                state->current_position += (total_length + 8);
                state->index            += (total_length + 8);
              } else {
                state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              }
              break;

            case AUDFMT_DLS_CHUNK_ART2:
              if(audfmt_dls_process_art(AUDFMT_DLS_CHUNK_ART2, state, length)
                  == AUDFMT_STATUS_SUCCESS) {
                state->lart_length      -= (total_length + 8);
                state->current_position += (total_length + 8);
                state->index            += (total_length + 8);
              } else {
                state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              }
              break;

            case AUDFMT_DLS_LIST_UNRECOGNIZED:
            case AUDFMT_DLS_CHUNK_UNRECOGNIZED:
              state->lart_length      -= (total_length + 8);
              state->current_position += (total_length + 8);
              state->index            += (total_length + 8);
              break;

            case AUDFMT_DLS_INSUFFICIENT_DATA:
              state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              break;
          }
        } else if(state->lart_length == 0) {
          state->state = state->prev_state;
        }
        break;

      case AUDFMT_DLS_STATE_LIST_RGN:
        if(state->rgn_length >= 8) {
          switch(audfmt_dls_get_next_chunk(state, &length, &total_length)) {
            case AUDFMT_DLS_LIST_LART:
              state->lart_length       = total_length - 4;
              state->state             = AUDFMT_DLS_STATE_LIST_LART;
              state->rgn_length       -= (total_length + 8);
              state->current_position += 12;
              state->index            += 12;
              state->prev_state        = AUDFMT_DLS_STATE_LIST_RGN;
              break;

            case AUDFMT_DLS_CHUNK_RGNH:
              if(audfmt_dls_process_rgnh(state, length) ==
                 AUDFMT_STATUS_SUCCESS) {
                state->rgn_length       -= (total_length + 8);
                state->current_position += (total_length + 8);
                state->index            += (total_length + 8);
              } else {
                state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              }
              break;

            case AUDFMT_DLS_CHUNK_WSMP:
              if(audfmt_dls_process_wsmp(state, length) ==
                 AUDFMT_STATUS_SUCCESS) {
                state->rgn_length       -= (total_length + 8);
                state->current_position += (total_length + 8);
                state->index            += (total_length + 8);
              } else {
                state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              }
              break;

            case AUDFMT_DLS_CHUNK_WLNK:
              if(audfmt_dls_process_wlnk(state, length) ==
                 AUDFMT_STATUS_SUCCESS) {
                state->rgn_length       -= (total_length + 8);
                state->current_position += (total_length + 8);
                state->index            += (total_length + 8);
              } else {
                state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              }
              break;

            case AUDFMT_DLS_LIST_UNRECOGNIZED:
            case AUDFMT_DLS_CHUNK_UNRECOGNIZED:
              state->rgn_length       -= (total_length + 8);
              state->current_position += (total_length + 8);
              state->index            += (total_length + 8);
              break;

            case AUDFMT_DLS_INSUFFICIENT_DATA:
              state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              break;
          }
        } else if(state->rgn_length == 0) {
          state->state = AUDFMT_DLS_STATE_LIST_LRGN;

          inst_ptr = state->inst_def_ptr;
          if(inst_ptr->drum) {
            prog_ptr = inst_ptr->prog_def;
          } else {
            prog_ptr = &(inst_ptr->prog_def[inst_ptr->current_region]);
          }

#ifndef FEATURE_AUDIO_CMX_5P0
          if(inst_ptr->peg_depth_art_flag) {
            prog_ptr->peg_depth = getPegDepthIndex(
                                       inst_ptr->peg_depth_art_value,
                                       &prog_ptr->peg_sustain_level,
                                       &prog_ptr->peg_attack1_level);

          }

          audfmt_dls_prog_final_init(prog_ptr);
#endif /* not FEATURE_AUDIO_CMX_5P0 */
          audfmt_dls_prog_resort(state);
          inst_ptr->current_region++;
        }
        break;

      case AUDFMT_DLS_STATE_PTBL:
        if(audfmt_dls_process_ptbl(state) == AUDFMT_STATUS_SUCCESS) {
          if(audfmt_dls_setup_wave(state) == AUDFMT_STATUS_SUCCESS) {
            state->state = AUDFMT_DLS_STATE_WAVE_INIT;
          } else {
            state->state = AUDFMT_DLS_STATE_DONE;
          }
        } else {
          state->state = AUDFMT_DLS_STATE_ERROR;
        }
        break;

      case AUDFMT_DLS_STATE_WAVE_INIT:
        if(audfmt_dls_prepare_wave(state) == AUDFMT_STATUS_SUCCESS) {
          state->state = AUDFMT_DLS_STATE_LIST_WAVE;
        } else if(audfmt_dls_setup_wave(state) != AUDFMT_STATUS_SUCCESS) {
          state->state = AUDFMT_DLS_STATE_DONE;
        }
        break;

      case AUDFMT_DLS_STATE_LIST_WAVE:
        if(state->wave_length >= 8) {
          switch(audfmt_dls_get_next_chunk(state, &length, &total_length)) {
            case AUDFMT_DLS_CHUNK_FMT:
              ret_val = audfmt_dls_process_wave_fmt(state, length);
              switch(ret_val) {
                case AUDFMT_STATUS_SUCCESS:
                  state->wave_length      -= (total_length + 8);
                  state->current_position += (total_length + 8);
                  state->index            += (total_length + 8);
                  break;

                case AUDFMT_STATUS_BUFF_ERR:
                  state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
                  break;

                case AUDFMT_STATUS_FAILURE:
                default:
                  audfmt_dls_wave_fail(state);
                  if(audfmt_dls_setup_wave(state) == AUDFMT_STATUS_SUCCESS) {
                    state->state = AUDFMT_DLS_STATE_WAVE_INIT;
                  } else {
                    state->state = AUDFMT_DLS_STATE_DONE;
                  }
                  break;
              }
              break;

            case AUDFMT_DLS_CHUNK_DATA:
              ret_val = audfmt_dls_process_wave_data(state, length,
                                           state->current_position + 8);
              switch(ret_val) {
                case AUDFMT_STATUS_PENDING:
                  /* wsmp chunk not yet encountered; continue processing
                  ** wave list.
                  */
                  state->wave_length      -= (total_length + 8);
                  state->current_position += (total_length + 8);
                  state->index            += (total_length + 8);
                  break;

                case AUDFMT_STATUS_SUCCESS:
                  state->state = AUDFMT_DLS_STATE_WAVE_DATA;
                  break;

                case AUDFMT_STATUS_FAILURE:
                default:
                  if(audfmt_dls_setup_wave(state) == AUDFMT_STATUS_SUCCESS) {
                    state->state = AUDFMT_DLS_STATE_WAVE_INIT;
                  } else {
                    state->state = AUDFMT_DLS_STATE_DONE;
                  }
                  break;
              }
              break;

            case AUDFMT_DLS_CHUNK_WSMP:
              if(audfmt_dls_process_wsmp(state, length) ==
                 AUDFMT_STATUS_SUCCESS) {
                ret_val = audfmt_dls_process_pending_wave_data( state );
                switch(ret_val) {
                  case AUDFMT_STATUS_PENDING:
                    /* This status indicates that data chunk has not yet been
                    ** encountered; continue parsing wave list.
                    */
                    state->wave_length      -= (total_length + 8);
                    state->current_position += (total_length + 8);
                    state->index            += (total_length + 8);
                    break;

                  case AUDFMT_STATUS_SUCCESS:
                    state->state = AUDFMT_DLS_STATE_WAVE_DATA;
                    break;

                  case AUDFMT_STATUS_FAILURE:
                  default:
                    if(audfmt_dls_setup_wave(state) == AUDFMT_STATUS_SUCCESS) {
                      state->state = AUDFMT_DLS_STATE_WAVE_INIT;
                    } else {
                      state->state = AUDFMT_DLS_STATE_DONE;
                    }
                    break;
                }
              } else {
                state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              }
              break;

            case AUDFMT_DLS_LIST_UNRECOGNIZED:
            case AUDFMT_DLS_CHUNK_UNRECOGNIZED:
              state->wave_length      -= (total_length + 8);
              state->current_position += (total_length + 8);
              state->index            += (total_length + 8);
              break;

            case AUDFMT_DLS_INSUFFICIENT_DATA:
              state->data_req = AUDFMT_DLS_INSUFFICIENT_DATA;
              break;
          }
        } else {
          audfmt_dls_wave_cleanup(state);
          if(audfmt_dls_setup_wave(state) == AUDFMT_STATUS_SUCCESS) {
            state->state = AUDFMT_DLS_STATE_WAVE_INIT;
          } else {
            state->state = AUDFMT_DLS_STATE_DONE;
          }
        }
        break;

      case AUDFMT_DLS_STATE_WAVE_DATA:
        audfmt_dls_prepare_wave_data(state);
        if(audfmt_dls_setup_wave(state) == AUDFMT_STATUS_SUCCESS) {
          state->state = AUDFMT_DLS_STATE_WAVE_INIT;
        } else {
          state->state = AUDFMT_DLS_STATE_DONE;
        }
        break;

      case AUDFMT_DLS_STATE_DONE:
        audfmt_dls_release_reserved(state->handle, state->sequence);
        state->handle->data_func(SND_CMX_AF_DLS_DONE, state->handle->audfmt_ptr,
                                 0, NULL);
        audfmt_dls_free_state_struct(state);
        break;

      case AUDFMT_DLS_STATE_ERROR:
        audfmt_dls_release_reserved(state->handle, state->sequence);
        state->handle->data_func(SND_CMX_AF_DLS_FAILURE,
                                 state->handle->audfmt_ptr, 0, NULL);
        audfmt_dls_free_state_struct(state);
        break;
    }

    switch(state->state) {
      case AUDFMT_DLS_STATE_INIT:
      case AUDFMT_DLS_STATE_RIFF:
      case AUDFMT_DLS_STATE_LIST_LINS:
      case AUDFMT_DLS_STATE_LIST_INS:
      case AUDFMT_DLS_STATE_LIST_LRGN:
      case AUDFMT_DLS_STATE_LIST_LART:
      case AUDFMT_DLS_STATE_LIST_RGN:
        if(state->riff_length > 0) {
          status = audfmt_dls_check_buffer(state);
        } else {
          status = AUDFMT_STATUS_SUCCESS;
        }
        break;

      case AUDFMT_DLS_STATE_ERROR:
      case AUDFMT_DLS_STATE_DONE:
        status = AUDFMT_STATUS_SUCCESS;
        break;

      case AUDFMT_DLS_STATE_LIST_WAVE:
        if(state->wave_length > 0) {
          status = audfmt_dls_check_buffer(state);
        } else {
          status = AUDFMT_STATUS_SUCCESS;
        }
        break;

      case AUDFMT_DLS_STATE_LRGN_INIT:
      case AUDFMT_DLS_STATE_WAVE_INIT:
      case AUDFMT_DLS_STATE_WAVE_DATA:
      case AUDFMT_DLS_STATE_PTBL:
      case AUDFMT_DLS_STATE_INVALID:
      default:
        status = AUDFMT_STATUS_DONE;
        break;
    }

  } while(status == AUDFMT_STATUS_SUCCESS);
} /* end audfmt_process_dls */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_load_offset_dls

DESCRIPTION
  This function loads a DLS file from an offset position within the file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_load_offset_dls (
  snd_af_access_type *handle,
  uint32             start_position,
  uint32             sequence
) {
  audmain_status_type   ret_val = AUDMAIN_STATUS_FAILURE;
  audfmt_dls_state_type *dls_state;
  snd_af_read_cb_type   read_cb;

  if((handle != NULL) && (handle->data_func != NULL)) {
    if(sequence <= 4) {
      dls_state = audfmt_dls_get_state_struct();
      if(dls_state != NULL) {
        dls_state->handle           = handle;
        dls_state->sequence         = sequence;
        dls_state->current_position = start_position;

        read_cb.client_data = dls_state;
        audmain_dls_get_data(SND_CMX_AF_READ, handle, start_position, &read_cb);

        ret_val = AUDMAIN_STATUS_SUCCESS;
      } else {
        handle->data_func(SND_CMX_AF_DLS_FAILURE, handle->audfmt_ptr, 0, NULL);
      }
    }
  }

  return(ret_val);
} /* end audfmt_load_offset_dls */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_unload_offset_dls

DESCRIPTION
  This function unloads the DLS resources for the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_unload_offset_dls (
  snd_af_access_type *handle,
  uint32             sequence
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  uint32  counter;

  if((handle != NULL) && (handle->data_func != NULL)) {
    for(counter = 0; counter < AUDFMT_DLS_NUM_STATE_STRUCT; counter++) {
      if((audfmt_dls_state[counter].handle != NULL) &&
         (audfmt_dls_state[counter].sequence == sequence)) {
        audfmt_dls_state[counter].prev_state = audfmt_dls_state[counter].state;
        audfmt_dls_state[counter].state      = AUDFMT_DLS_STATE_RELEASE;
      }
    }

    audfmt_dls_release_sequence(handle, sequence);
    handle->data_func(SND_CMX_AF_DLS_DONE, handle->audfmt_ptr, 0, NULL);

    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
} /* end audfmt_unload_offset_dls */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_load_dls

DESCRIPTION
  This function loads a DLS file for all sequences to use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_load_dls (
  snd_af_access_type *handle,
  uint32             byte_offset
) {
  return(audfmt_load_offset_dls(handle, byte_offset, 4));
} /* end audfmt_load_dls */

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_unload_dls

DESCRIPTION
  This function unloads DLS resources reserved for all sequences to use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_unload_dls (
  snd_af_access_type *handle
) {
  return(audfmt_unload_offset_dls(handle, 4));
} /* end audfmt_unload_dls */

#endif /* FEATURE_DLS */

/*==========================================================================
 FUNCTION audfmt_multi_set_poly_mode

 DESCRPTION
   This function sets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   Status to indicate if the command is carried out correctly

 SIDE EFFECTS
   None
 ==========================================================================*/
audmain_status_type audfmt_multi_set_poly_mode (
  audmain_audfmt_poly_mode_type mode
) {
  uint8 seq;
  
  for (seq=0; seq<AUDFMT_NUM_SEQUENCES; seq++) {
    if ((audfmt_parse_ctl[seq].midi_status & AUDFMT_MASK_ENABLED) && 
        (audfmt_parse_ctl[seq].midi_status & AUDFMT_MASK_MULTISEQ_MODE)) {
      /* Do not allow the client to set hybrid mode until
         all multiseqeuncer playback are done */
      return(AUDMAIN_STATUS_FAILURE);
    }
  }

  if (mode < AUDMAIN_AUDFMT_POLY_MODE_0 ||
      mode > AUDMAIN_AUDFMT_POLY_MODE_7) {
    return(AUDMAIN_STATUS_FAILURE);
  }

  audfmt_poly_mode = mode;
  
  return(AUDMAIN_STATUS_SUCCESS);
}
#ifdef FEATURE_SMAF
/*===========================================================================

FUNCTION smaf_clean_heap

DESCRIPTION
  This function clears global variables.

INPUTS
audfmt_parse_type

RETURN VALUE
none

SIDE EFFECTS
  None

===========================================================================*/
void smaf_clean_heap(audfmt_parse_type* midi_ctl)
{
  audfmt_smaf_channel_pair *chan_pair_temp;
  audfmt_smaf_channel_pan *pan_tmp;
  audfmt_smaf_channel_cc10_pan *tmp_cc10;
  
   if(midi_ctl->channel_pair_ptr!=NULL){
    chan_pair_temp = midi_ctl->channel_pair_ptr;
   	while(chan_pair_temp!=NULL){
	   midi_ctl->channel_pair_ptr = midi_ctl->channel_pair_ptr->next_channel_pair;
	   free(chan_pair_temp);
	   chan_pair_temp= midi_ctl->channel_pair_ptr; 
	}
   }
 /*
  Spider 4002
 */
   if(midi_ctl->channel_pan_ptr!=NULL){
   	  pan_tmp = midi_ctl->channel_pan_ptr;
     while(pan_tmp!=NULL){
      midi_ctl->channel_pan_ptr = midi_ctl->channel_pan_ptr->next_channel_pan;
	  free(pan_tmp);
	  pan_tmp = midi_ctl->channel_pan_ptr;
	}
  }

   if(midi_ctl->cc10_channel_pan_ptr!=NULL){
   	  tmp_cc10 = midi_ctl->cc10_channel_pan_ptr;
     while(tmp_cc10!=NULL){
      midi_ctl->cc10_channel_pan_ptr = midi_ctl->cc10_channel_pan_ptr->next_cc10_channel_pan;
	  free(tmp_cc10);
	  tmp_cc10 = midi_ctl->cc10_channel_pan_ptr;
	}
  }
  pan_seq_counter =1;   
  midi_ctl->cc10_channel_pan_ptr = NULL;
  midi_ctl->channel_pair_ptr = NULL;   
  midi_ctl->channel_pan_ptr = NULL;  
}
/*===========================================================================

FUNCTION audfmt_get_smaf_chn_pan

DESCRIPTION
  This function returns the Pan value
INPUTS

RETURN VALUE
pan value for ADEC channel

SIDE EFFECTS
  None

===========================================================================*/
sint15 audfmt_get_smaf_chn_pan(audfmt_parse_type *midi_ctl,uint8 noteno,uint8 midi_channel)
{
/*
  when smaf_do_mobile_note is called
  buf[1] = status & MIDI_CHANNEL_NIBBLE;
  buf[2] = this will be the Wav id to be played.

*/

  audfmt_smaf_channel_pan *pantmp;
  audfmt_smaf_channel_cc10_pan *cc10_pan;
  sint15 PanValue = 0x00;//deafult it will be center
  sint15 note_pan =0x00,midi_pan=0x00;
  uint32 note_seq =0,midi_seq=0;
  

  pantmp = midi_ctl->channel_pan_ptr;
  cc10_pan = midi_ctl->cc10_channel_pan_ptr;
			 
   while((pantmp)!=NULL){
     if(pantmp->wavid ==noteno){
     	note_pan = pantmp->pan_val;	
		note_seq = pantmp->seq;
		break;
     }			  
     pantmp= pantmp->next_channel_pan;
   }				 

  while((cc10_pan)!=NULL){
     if(cc10_pan->midi_channel == midi_channel){
     	midi_pan = cc10_pan->pan_val;	
		midi_seq = cc10_pan->seq;
		break;
     }			  
     cc10_pan= cc10_pan->next_cc10_channel_pan;
   }						  


 if(note_seq > midi_seq){
  /*
   This means that SYS EX has come later.
  */
   PanValue = note_pan;
 }else if(midi_seq < note_seq){
  /*
   This means that MIDI channel Pan
   has come later to SYS EX Pan.
  */
   PanValue = midi_pan;
 } 
 return((sint15)PanValue);
}

/*===========================================================================

FUNCTION audfmt_calculate_smaf_adec_chn_volume

DESCRIPTION
  This function calculates the ADEC Channel volume for SMAF File.

INPUTS
  smafgaintable/ in file/key velocity

RETURN VALUE
	Adec Channel volume is returned.

SIDE EFFECTS
  None

===========================================================================*/
uint16 audfmt_set_smaf_chn_volume(audfmt_parse_type* midi_ctl, uint16 keyvelocity)
{

 uint32 adec_channel_volume;
 uint32 temp;
 adec_channel_volume=0;temp=0;


 temp = (SMAF_CHANNEL_VOL_EXP_CONSTANT*SMAF_CHANNEL_VOL_EXP_CONSTANT) << 1; //Q15

 if((midi_ctl->midi_status3 &  AUDFMT_MASK_SMAF_MA3)||
          (midi_ctl->midi_status2 &AUDFMT_MASK_SMAF_MA2)){
 	// sysEx master volume is concave
	adec_channel_volume = smafgaintable[midi_ctl->cmid_master_vol]; //Q15
 	// key velocity is linear
	adec_channel_volume = (adec_channel_volume * keyvelocity) >> 7; //Q15
 	// Enforce -12dB limit
    if (adec_channel_volume > 0x2000) {
        adec_channel_volume = 0x2000;
    }
    adec_channel_volume = (adec_channel_volume * temp) >> 17; //Q13
 }else if((midi_ctl->midi_status3 & AUDFMT_MASK_SMAF_MA5)){
     //concave through out
    adec_channel_volume = smafgaintable[midi_ctl->cmid_master_vol]; 
	adec_channel_volume = (adec_channel_volume * temp) >> 15; //Q15
	adec_channel_volume = (adec_channel_volume * smafgaintable[keyvelocity]) >> 17; //Q13
 }else{
 	/*
 	MA3 or Ma5 will be identified at the parsing of the 
	file look for 17th byte in the file and compare with 
	CNTI values for MA2,MA3,MA5
	*/
 }
 /*
  Channel Adec volume is modified if the key velocity is modified or
  if the master volume is modified can be at run time depending on
  following msg. NOTE_MSG = key velocity.SYSTEM_MSG = master volume.
 */
 return adec_channel_volume;
}
#endif
#endif /* FEATURE_MIDI */
