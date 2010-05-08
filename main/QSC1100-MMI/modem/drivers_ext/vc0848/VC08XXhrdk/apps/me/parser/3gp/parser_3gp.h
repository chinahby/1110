#ifndef _ME_PARSER_3GP_
#define _ME_PARSER_3GP_

#ifdef __cplusplus
extern "C" {
#endif

#include "..\..\me_comhead.h"

ME_U32	parser_3gp_init(void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor);
ME_U32	parser_3gp_release(void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor);
ME_U32	parser_3gp_check_filetype(void *mediafile, fs_callbacks *fs_filefunc);
ME_U32	parser_3gp_get_audio_infor(void *fileinfor, me_audio_infor *audio_infor);
ME_U32	parser_3gp_get_audio_streambuffersize(void *fileinfor, ME_U32 *framesize, void* audio_info);
ME_U32	parser_3gp_get_audio_duration(void *fileinfor, ME_U32 frame, ME_U32 * duration);
ME_U32	parser_3gp_read_audio(void *fileinfor, char *audio_buffer, ME_U32 *readbytes,ME_U32 bufferlength);

ME_U32	parser_3gp_get_video_infor(void *fileinfor, me_video_infor *video_infor);
ME_U32	parser_3gp_get_video_streambuffersize(void *fileinfor, ME_U32 *framesize);
ME_U32	parser_3gp_get_video_duration(void *fileinfor, ME_U32 frame, ME_U32 * duration);
ME_U32	parser_3gp_read_video(void *fileinfor, char *video_buffer, ME_U32 *readbytes,ME_U32 bufferlength);
ME_U32	parser_3gp_read_key_video(void *fileinfor, char *video_buffer, ME_U32 *readbytes,ME_U32 bufferlength);
ME_U32	parser_3gp_read_key_videoframes(void *fileinfor, video_stream_infor *video_buffer);

ME_U32	parser_3gp_seek(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *videoframe);
ME_U32	parser_3gp_seek_audio_syntime(void *fileinfor, ME_U32 pos);

ME_U32	parser_3gp_read_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer);
ME_U32	parser_3gp_read_video_frames(void *fileinfor, video_stream_infor *video_buffer);
#if (ME_VERSION2_FOURHANDLE)	
ME_U32	parser_3gp_read_audio_frames_reg(void *fileinfor, audio_stream_infor *audio_buffer, ME_U32 reg, ME_U32 bit);
ME_U32	parser_3gp_read_audio_frames_post(void *fileinfor, audio_stream_infor *audio_buffer);
ME_U32	parser_3gp_read_video_frames_reg(void *fileinfor, video_stream_infor *video_buffer, ME_U32 reg, ME_U32 bit);
ME_U32	parser_3gp_read_video_frames_post(void *fileinfor, video_stream_infor *video_buffer);
#endif
ME_U32	parser_3gp_register(void);

#ifdef __cplusplus
}
#endif

#endif 

