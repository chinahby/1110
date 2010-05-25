/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	mp4_interface.h  
* User for : 	mp4 demux mux, 
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#ifndef MP4_INTERFACE_H
#define MP4_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

//#include "mp4_ip.h"
#include "mp4_basedef.h"
#include "mp4_private.h"
#include "mp4_user.h"
/* =========================== public interface ========================= // */

/* call this first to open the file and create all the objects */
mp4_infor_t* mp4_open(char *filename, uint32 rw_mode);
/* close the file and delete all the objects */
uint32 mp4_close(mp4_infor_t *file);

/* Read all the information about the file.  Requires a MOOV atom be present in the file. */
/* If no MOOV atom exists return 1 else return 0. */
uint32 mp4_read_info(mp4_infor_t *file);
uint32 mp4_write_info(mp4_infor_t *file);
uint32 mp4_write_table(mp4_infor_t *file);

/* return 1 if the file is a quicktime file */
uint32 mp4_check_sig(mp4_infor_t *file);

/* get audio parameter information */
uint32 mp4_audio_channels(mp4_infor_t *file, uint32  *channels);
uint32 mp4_audio_bits(mp4_infor_t *file, uint32  *bits);
uint32 mp4_audio_sample_rate(mp4_infor_t *file, uint32  *samplerate);
uint32 mp4_audio_time_scale(mp4_infor_t *file, uint32  *timescale);
uint32 mp4_audio_sample_duration(mp4_infor_t *file, uint32  sample, uint32*duration);
uint32 mp4_audio_compressor(mp4_infor_t *file, char**compressor);

uint32 mp4_audio_frame_size(mp4_infor_t *file, uint32  frame, uint32  *framesize);
uint32 mp4_audio_max_frame_size(mp4_infor_t *file, uint32  *framesize);
uint32 mp4_audio_stsz_frame_size(mp4_infor_t *file, uint32  *framesize);

/* get video parameter information */
uint32 mp4_video_width(mp4_infor_t *file, uint32  *width);
uint32 mp4_video_height(mp4_infor_t *file, uint32  *height);
uint32 mp4_video_depth(mp4_infor_t *file, uint32  *depth);
uint32 mp4_video_time_scale(mp4_infor_t *file, uint32  *timescale);
uint32 mp4_video_sample_duration(mp4_infor_t *file, uint32  sample, uint32  *duration);
uint32 mp4_video_frame_rate(mp4_infor_t *file, uint32  *framerate);
uint32 mp4_video_compressor(mp4_infor_t *file, char**compressor);

uint32 mp4_video_frame_size(mp4_infor_t *file, uint32  frame, uint32  *framesize);
uint32 mp4_video_max_frame_size(mp4_infor_t *file, uint32  *framesize);

uint32 mp4_is_syn_video_sample(mp4_infor_t *file, uint32  sample);

/* make the quicktime file streamable */
uint32 mp4_set_time_scale(mp4_infor_t *file, uint32  time_scale);
uint32  mp4_get_time_scale(mp4_infor_t *file, uint32  *timescale); 

int mp4_set_mp4v_decoder_config(mp4_infor_t *file, void* dec_config);
int mp4_set_mp4a_decoder_config(mp4_infor_t *file, void* dec_config);
int mp4_set_d263_decoder_config(mp4_infor_t *file, void* d263_config);
int mp4_set_damr_decoder_config(mp4_infor_t *file, void* damr_config);
int mp4_get_video_decoder_config(mp4_infor_t *file, void** decconfig, unsigned int * len);
int mp4_get_audio_decoder_config(mp4_infor_t *file, void** decconfig, unsigned int * len);

/* set up tracks in a new file after opening and before writing */
/* audio is stored two channels per quicktime track */
uint32 mp4_set_audio(mp4_infor_t *file, uint32 channels, uint32 bits, uint32  sample_rate,uint32  sample_size,  uint32  time_scale, uint32  sample_duration, char *compressor);
/* video is stored one layer per quicktime track */
uint32 mp4_set_video(mp4_infor_t *file, uint32 tracks, uint32 frame_w, uint32 frame_h, uint32  time_scale, uint32  sample_duration, char *compressor);

/* ========================== Access to raw data follows. */
/* set position of file descriptor relative to a track */
uint32  mp4_set_audio_position(mp4_infor_t *file, uint32  sample);
uint32  mp4_set_video_position(mp4_infor_t *file, uint32  frame);

uint32 mp4_read_audio_frame(mp4_infor_t *file, char *audio_buffer, uint32 maxBytes,  uint32 *readbytes);
uint32 mp4_read_video_frame(mp4_infor_t *file, char *video_buffer, uint32 maxBytes,  uint32 *readbytes);
uint32 mp4_write_audio_frame(mp4_infor_t *file, char *audio_buffer, uint32  bytes, uint32  duration);
uint32 mp4_write_video_frame(mp4_infor_t *file, char *video_buffer, uint32  bytes, uint32  isKeyFrame, uint32  duration);

uint32 mp4_read_audio_frames(mp4_infor_t *file, audio_buffer_t *audio_buffer);
uint32 mp4_read_video_frames(mp4_infor_t *file, video_buffer_t *video_buffer);
//uint32 mp4_write_audio_frames(mp4_infor_t *file, audio_buffer_t *audio_buffer);
uint32 mp4_write_audio_frames(mp4_infor_t *file, audio_buffer_t *audio_buffer,uint32 bufferindex);
#if VIDEORECORDERIN848
uint32 mp4_write_video_frames(mp4_infor_t *file, video_buffer_t *video_buffer,uint32 bufferindex);
#else
uint32 mp4_write_video_frames(mp4_infor_t *file, video_buffer_t *video_buffer);
#endif

uint32 mp4_read_key_video_frame(mp4_infor_t *file, char *video_buffer, uint32 maxBytes,  uint32 *readbytes);
uint32 mp4_read_key_video_frames(mp4_infor_t *file, video_buffer_t *video_buffer);

#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_read_video_frame_ver2(mp4_infor_t *file, video_buffer_t *video_buffer);
uint32 mp4_read_audio_frame_ver2(mp4_infor_t *file, audio_buffer_t *audio_buffer);
uint32 mp4_read_audio_frame_ver3(mp4_infor_t *file, audio_buffer_t *audio_buffer, uint32 reg, uint32 bit);
uint32 mp4_read_audio_frame_ver3_post(mp4_infor_t *file, audio_buffer_t *audio_buffer);
uint32 mp4_read_video_frame_ver3(mp4_infor_t *file, video_buffer_t *video_buffer, uint32 reg, uint32 bit);
uint32 mp4_read_video_frame_ver3_post(mp4_infor_t *file, video_buffer_t *video_buffer);
#endif
/* these four function add by Amanda deng, use for seek time user want */
uint32 mp4_get_syn_video_sample(mp4_infor_t *file, uint32  sample, uint32  *frame);
uint32 mp4_get_syn_audio_sample(mp4_infor_t *file, uint32  time, uint32  *frame );
uint32 mp4_get_time_of_video_frame(mp4_infor_t *file, uint32  frame, uint32  *start);
uint32 mp4_get_time_of_audio_frame(mp4_infor_t *file, uint32  frame, uint32  *start);
uint32 mp4_get_video_frame_of_time(mp4_infor_t *file, uint32  time, uint32  *frame);
uint32 mp4_get_audio_frame_of_time(mp4_infor_t *file, uint32  time, uint32  *frame);

uint32 mp4_random_access_time(mp4_infor_t *file, uint32  time, uint32  *seektime, uint32 *audioframe, uint32 *videoframe);
uint32 mp4_random_access_syntime(mp4_infor_t *file, uint32  time, uint32 *audioframe, uint32 *videoframe);
uint32 mp4_random_access_audiotime(mp4_infor_t *file, uint32  time, uint32 *audioframe);


#ifdef __cplusplus
}
#endif

#endif

