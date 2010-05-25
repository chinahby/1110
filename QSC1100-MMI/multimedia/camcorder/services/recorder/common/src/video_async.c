/**********************************************************************
 * video_async.c
 * Asynchronous file I/O task that does file writes on behalf of the video
 * task.
 *
 * Copyright (C) 2004-2007, Qualcomm, Inc.
 */

/*==========================================================================
 *
 *                          EDIT HISTORY FOR MODULE
 *
 *  This section contains comments describing changes made to the module.
 *  Notice that changes are listed in reverse chronological order.
 *
 *  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/src/video_async.c#4 $ $DateTime: 2008/08/12 05:55:17 $ $Author: psisodia $
 *
 * when        who   what, where, why
 * ---------   ---   -------------------------------------------------------
 * 08/09/08    ps     Dynamically allocated video_async_stack for ULC memory Optimization   
 * 06/30/08    shiven Fixed Klocwork error
 * 06/10/08    jdas   Removed VIDEO_ASYNC_INIT signal
 * 07/24/07    jdas   Added support of video_aysnc_request_complete()
 * 06/12/07    jdas   Made async_init to async_task and will do init after 
 *                    previous instance cleanup
 * 04/20/07    gopi   Made Async Task variables initialization synchronous.
 * 12/29/06    dhaval Releasing temp files after recording. 
 * 12/21/06    dhaval Removed lint error.   
 * 12/13/06    jdas  Made filename null terminated in video_async_file_unlink 
 * 11/21/06    dw    Temporary increase in stack size for hfat recording. 
 * 2006-18-10  Gopi  Moved file close and unlink operations to Async Task. 
 * 2003-02-09  gr    Created.
 * 
 *
 ==========================================================================*/

#include "comdef.h"
#include "fs_stdio.h"
#include "rex.h"
#include "task.h"
#include "video_async.h"
#include "assert.h"
#define VIDEO_ASYNC_STACK_SIZE 4096
#define MAX_FILE_CLOSE_SIZE    20
#define MAX_FILE_NAME_SIZE     80

rex_tcb_type video_async_tcb;
static int init_complete = 0;

static rex_tcb_type * video_async_client_tcb = NULL;
static uint32 video_async_client_sig = 0;

typedef struct
{
   char    filename [MAX_FILE_NAME_SIZE];
   boolean used;
}
unlinkfilename;

unlinkfilename  filenames[MAX_FILE_CLOSE_SIZE+1];

VIDEO_ENG_FILE  *fs_handle_close[MAX_FILE_CLOSE_SIZE+1];

static unsigned char * video_async_stack = NULL;

static void
video_async_task (dword parm)
{
  rex_sigs_type sigs;
  uint8 usedfilehandle;

  (void) parm;
  /*lint -e{716} infinite loop requires here */
  while (1) 
  {
    /* XXX: Replace 0xffffffff with the actual signal mask once we know
     * what signal to use to process the async writes. 0xffffffff will work
     * though.
     */
    sigs = rex_wait (0xffffffff);
    (void)rex_clr_sigs (rex_self (), 0xffffffff);

    if ((sigs & TASK_OFFLINE_SIG) != 0)
      task_offline ();

    if ((sigs & TASK_STOP_SIG) != 0)
      task_stop ();

    if ((VIDEO_ASYNC_SIG & sigs) != 0) 
    {
       efs_process_async_writes ();
    }

    if( ((sigs & VIDEO_ASYNC_SIG_FILECLOSE)  != 0)||
        ((sigs & VIDEO_ASYNC_SIG_FILEUNLINK) != 0)
      )
      {
        for(usedfilehandle = 0;usedfilehandle<MAX_FILE_CLOSE_SIZE;usedfilehandle++)
        {
          if(fs_handle_close[usedfilehandle])
          {
            (void)video_eng_file_fclose (fs_handle_close[usedfilehandle]);
            fs_handle_close[usedfilehandle] = NULL;
          }
        }
        for(usedfilehandle = 0;usedfilehandle<MAX_FILE_CLOSE_SIZE;usedfilehandle++)
        {
          if(filenames [usedfilehandle].used == 1)
          {
            efs_unlink (filenames [usedfilehandle].filename);
            filenames [usedfilehandle].used = 0;
          }
        }      
      }
     /* this should be before INIT if block in this loop
      ** Here we assume that all the previous call to the efs 
      ** are synchronous
      */
      if (sigs & VIDEO_ASYNC_SIG_REQ_COMPLETE)
      {
          (void)rex_set_sigs(video_async_client_tcb,
                             video_async_client_sig
                            );
          video_async_client_tcb = NULL;
      }
  }
}

void
video_async_init (void)
{
  if (!init_complete) {
    video_async_stack = (unsigned char *)malloc(VIDEO_ASYNC_STACK_SIZE * (sizeof(unsigned char)));
    if(video_async_stack == NULL)
    {
      return;
    } 
    /* Start up the video_async task */
    rex_def_task_ext (&video_async_tcb,
                      video_async_stack,
                      VIDEO_ASYNC_STACK_SIZE,
                      VIDEO_ASYNC_PRI,
                      video_async_task,
                      0,
                      "Video Async Task",
                      FALSE);
    memset (filenames,0,sizeof (filenames));
    memset (fs_handle_close,0,sizeof (fs_handle_close));
    init_complete = 1;
  }
} /* END video_async_init */

void video_aysnc_request_complete(rex_tcb_type * tcb, uint32 sig)
{
    if (tcb == NULL)
    {
        return;
    }
    if (init_complete == 1)
    {
        video_async_client_tcb = tcb;
        video_async_client_sig = sig;
        (void) rex_set_sigs(&video_async_tcb,
                            VIDEO_ASYNC_SIG_REQ_COMPLETE
                           );
    }
    else
    {
        (void)rex_set_sigs(tcb,sig);
    }
}
void video_async_file_close (VIDEO_ENG_FILE *fs_handle)
{
    uint8 FreeFilehandle;
    boolean FileHandleFound = FALSE;

    /*Check if Async Task is initialized*/
    if(init_complete == 1)
    {
        for(FreeFilehandle=0;FreeFilehandle<MAX_FILE_CLOSE_SIZE;FreeFilehandle++)
        {
            if(fs_handle_close [FreeFilehandle] == NULL)
            {
                FileHandleFound = TRUE;
                break;
            }            
        }

        ASSERT (FileHandleFound);

        fs_handle_close [FreeFilehandle] = fs_handle;

        (void)rex_set_sigs ( &video_async_tcb,         /* tcb for which the sigs will be set */
                             VIDEO_ASYNC_SIG_FILECLOSE /* the sigs to set                    */
                            );
    }
    /*If Async Task has not been initialized close file handle immediately*/
    else
    {
      video_eng_file_fclose (fs_handle);   
    }

}

void video_async_file_unlink (char *filename,uint8 filelength)
{
    uint8 FreeFilehandle;
    boolean FileHandleFound = FALSE;

    /*Check if Async Task is runing*/
    if(init_complete == 1)
    {
        for(FreeFilehandle=0;FreeFilehandle<MAX_FILE_CLOSE_SIZE;FreeFilehandle++)
        {
            if(filenames [FreeFilehandle].used == 0)
            {
                FileHandleFound = TRUE;
                break;
            }            
        }

        ASSERT (FileHandleFound);
        ASSERT (filelength < MAX_FILE_NAME_SIZE);

        filenames [FreeFilehandle].used = 1;
        memcpy (filenames [FreeFilehandle].filename,filename,filelength);
        filenames [FreeFilehandle].filename[filelength] = '\0';

       (void)rex_set_sigs ( &video_async_tcb,          /* tcb for which the sigs will be set */
                             VIDEO_ASYNC_SIG_FILEUNLINK /* the sigs to set                    */
                            );

    }
    /*If Async Task has not been initialized delete file immediately*/
    else
    {
      efs_unlink (filename);    
    }
}
