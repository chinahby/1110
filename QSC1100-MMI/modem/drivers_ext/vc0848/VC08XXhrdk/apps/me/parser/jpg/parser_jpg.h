#ifndef _ME_PARSER_JPG_
#define _ME_PARSER_JPG_

#ifdef __cplusplus
extern "C" {
#endif

#include "..\..\me_comhead.h"
ME_U32	parser_jpg_register(void);
ME_U32	parser_jpg_init(void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor);
ME_U32  parser_jpg_release(void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor);
ME_U32	parser_jpg_check_filetype(void *mediafile, fs_callbacks *fs_filefunc);
ME_U32	parser_jpg_get_video_infor(void *fileinfor, me_video_infor *video_infor);
ME_U32	parser_jpg_read_image(void *fileinfor, char *audio_buffer, ME_U32 *readbytes,ME_U32 bufferlength);
ME_U32	parser_jpg_get_audio_infor(void *fileinfor, me_audio_infor *audio_infor);
ME_U32	parser_jpg_get_video_srcbuffersize(void *fileinfor, ME_U32 *framesize);
ME_U32	parser_jpg_read_image_frames(void *fileinfor, video_stream_infor *video_buffer);

#ifdef __cplusplus
}
#endif

#endif 
