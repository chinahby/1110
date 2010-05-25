#ifndef _ME_MUXER_3GP_
#define _ME_MUXER_3GP_


#ifdef __cplusplus
extern "C" {
#endif

#include "..\..\me_comhead.h"

ME_U32	muxer_3gp_init(void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor);
ME_U32	muxer_3gp_release(void *fileinfor);
ME_U32	muxer_3gp_set_audio_infor(void *fileinfor, me_audio_infor *audio_infor);
ME_U32	muxer_3gp_write_audio(void *fileinfor,char *video_buffer, ME_U32 bytes, ME_U32 duration);
ME_U32	muxer_3gp_write_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer);
ME_U32	muxer_3gp_set_video_infor(void *fileinfor, me_video_infor *video_infor);
ME_U32	muxer_3gp_write_video(void *fileinfor, char *video_buffer, ME_U32 bytes, ME_U32 duration,ME_U32 is_key_frame);
ME_U32	muxer_3gp_write_video_frames(void *fileinfor, video_stream_infor *video_buffer);


ME_U32	muxer_3gp_register(void);

#ifdef __cplusplus
}
#endif

#endif 

