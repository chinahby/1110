/**********************************************************************
 * video_async.h
 * Asynchronous file I/O for the video task.
 *
 * Copyright (C) 2004 - 2007, Qualcomm, Inc.
 */

/*==========================================================================
 *
 *                          EDIT HISTORY FOR MODULE
 *
 *  This section contains comments describing changes made to the module.
 *  Notice that changes are listed in reverse chronological order.
 *
 *  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/video_async.h#1 $ $DateTime: 2008/05/01 11:10:49 $ $Author: karenp $
 *
 * when        who  what, where, why
 * ---------   ---  -------------------------------------------------------
 * 2004-02-10   gr   Created.
 * 2006-18-10   Gopi Moved file close and unlink operations to Async Task.
 ==========================================================================*/

#ifndef __VIDEO_ASYNC_H__
#define __VIDEO_ASYNC_H__

#include "rex.h"
#include "videoeng_fio.h"

#define VIDEO_ASYNC_SIG 0x01
#define VIDEO_ASYNC_SIG_FILECLOSE  0x02
#define VIDEO_ASYNC_SIG_FILEUNLINK 0x04
#define VIDEO_ASYNC_INIT 0x08
#define VIDEO_ASYNC_SIG_WRITE_DONE 0x10
#define VIDEO_ASYNC_SIG_REQ_COMPLETE 0x20

extern rex_tcb_type video_async_tcb;

void video_async_init (void);
void video_async_file_close (VIDEO_ENG_FILE *fs_handle);
void video_async_file_unlink (char *filename,uint8 filelength);
void video_aysnc_request_complete(rex_tcb_type * tcb, uint32 sig);
#endif /* not __VIDEO_ASYNC_H__ */

