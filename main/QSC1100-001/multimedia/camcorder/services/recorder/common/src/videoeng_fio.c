/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Video Encode Engine File Object")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     V I D E O   E N C O D E   E N G I N E
                             F I L E   O B J E C T

GENERAL DESCRIPTION
  This module contains the implementation of the video encode engine file 
  object which supports file either on EFS or on memory.

REFERENCES

EXTERNALIZED FUNCTIONS
  video_eng_mem_file_fopen
    This function opens a file on memory.
  video_eng_efs_file_fopen
    This function corresponds to ANSI fopen and opens a file on EFS.
  video_eng_file_fclose
    This function corresponds to ANSI fclose.
  video_eng_file_fread
    This function corresponds to ANSI fread.
  video_eng_file_fseek           
    This function corresponds to ANSI fseek.
  video_eng_file_ftell
    This function corresponds to ANSI ftell.
  video_eng_file_fwrite          
    This function corresponds to ANSI fwrite.
  video_eng_file_fcopy
    This function copies data from one file to another.
  video_eng_mem_file
    This function checks if file is on memory.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2003 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/src/videoeng_fio.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/07   Kadhir  Merged fix in video_eng_file_fcopy() from 7K
05/02/07   gopi    File copy returns zero if operation fails.
04/10/06   Nagesh  Made code Lint error free
10/17/05   gm      Automated script to modify the MSG macro's
03/17/05   jk      Updated as per code walk. Made Lint free.
03/07/05   jk      Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdlib.h>             /* Dynamic memory allocation               */
#include "camcorder_msg.h" 
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Definitions for byte, word, etc.        */

#include "err.h"                /* Error logging/reporting services        */
#include <memory.h>             /* Memory manipulation routines            */

#if defined (FEATURE_EFS2) || defined (T_WINNT)

#include "videoeng_fio.h"

/* <EJECT> */
/*===========================================================================

                          DECLARATIONS FOR MODULE

===========================================================================*/

typedef struct video_eng_mem_object_struct {
    uint8     *buf_ptr;
    uint32    buf_size;
    uint32    length;
    uint32    pos;
    boolean   free_buf;
} video_eng_mem_object_type;


static video_eng_mem_object_type * video_eng_mem_file_create
(
    uint8   *buf_ptr, 
    uint32  buf_size
);

static int32 video_eng_mem_fclose (void * object);

static uint32  video_eng_mem_fread 
(
    void    *buffer, 
    uint32  size, 
    uint32  count, 
    void    *object
);

static int32  video_eng_mem_fseek 
( 
    void  *object, 
    int32 offset, 
    int32 origin 
);

static int32  video_eng_mem_ftell ( void * object );

static uint32  video_eng_mem_fwrite 
( 
    void  *buffer, 
    uint32  size, 
    uint32  count, 
    void    *object 
);

static int32 video_eng_efs_fclose (void * object);

static uint32  video_eng_efs_fread 
(
    void    *buffer, 
    uint32  size, 
    uint32  count, 
    void    *object
);

static int32  video_eng_efs_fseek 
( 
    void  *object, 
    int32 offset, 
    int32 origin 
);

static int32  video_eng_efs_ftell ( void * object );

static uint32  video_eng_efs_fwrite 
( 
    void    *buffer, 
    uint32  size, 
    uint32  count, 
    void    *object 
);

/* <EJECT> */
/*===========================================================================

                LOCAL DATA DECLARATIONS

===========================================================================*/

static video_eng_file_ops_type video_eng_efs_file_ops = 
{
    video_eng_efs_fclose,
    video_eng_efs_fread,
    video_eng_efs_fseek,
    video_eng_efs_ftell,
    video_eng_efs_fwrite
};

static video_eng_file_ops_type video_eng_mem_file_ops = 
{
    video_eng_mem_fclose,
    video_eng_mem_fread,
    video_eng_mem_fseek,
    video_eng_mem_ftell,
    video_eng_mem_fwrite
};


/* <EJECT> */
/*===========================================================================

                EXTERNAL FUNCTIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_mem_file_fopen

DESCRIPTION
  This function opens a file on memory

DEPENDENCIES
  None

PARAMETERS
  IN uint8 *buf_ptr
    This is the buffer pointer. If NULL, this function allocates memory of
    'buf_size'.

  IN uint32 buf_size
    This is the buffer size. It limits the maximum size of the opened file.

RETURN VALUE
  This function returns a pointer to video engine file object.

SIDE EFFECTS
  None

===========================================================================*/
VIDEO_ENG_FILE * video_eng_mem_file_fopen
(
  uint8   *buf_ptr, 
  uint32  buf_size
)
{
    VIDEO_ENG_FILE  *file;

    file = (VIDEO_ENG_FILE *) videoeng_malloc(sizeof(VIDEO_ENG_FILE));
    if (file)
    {
        file->object = (void *) video_eng_mem_file_create(buf_ptr, buf_size);
        file->ops = &video_eng_mem_file_ops;
        if (!file->object)
        {
            CAMCORDER_MSG_ERROR ("mem_file_fopen: Unable to create mem file object", 
                        0, 0, 0);
            videoeng_free(file);
            file = NULL;
        }
    }
    else
    {
        CAMCORDER_MSG_ERROR ("mem_file_fopen: Unable to allocate memory", 0, 0, 0);
    }
    return (file);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_efs_file_fopen

DESCRIPTION
  This function opens a file on EFS

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  This function returns a pointer to video engine file object.

SIDE EFFECTS
  None

===========================================================================*/
VIDEO_ENG_FILE * video_eng_efs_file_fopen
(
  const char *filename, 
  const char *mode
)
{
    VIDEO_ENG_FILE  *file;

    file = (VIDEO_ENG_FILE *) videoeng_malloc(sizeof(VIDEO_ENG_FILE));
    if (file)
    {
        file->object = efs_fopen(filename, mode);
        file->ops = &video_eng_efs_file_ops;
        if (!file->object)
        {
            CAMCORDER_MSG_ERROR ("efs_file_fopen: Unable to open efs file", 0, 0, 0);
            videoeng_free(file);
            file = NULL;
        }
    }
    else
    {
        CAMCORDER_MSG_ERROR ("efs_file_fopen: Unable to allocate memory", 0, 0, 0);
    }
    return (file);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_file_fclose

DESCRIPTION
  This function closes video engine file object.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
int32 video_eng_file_fclose (VIDEO_ENG_FILE * file)
{
    int32 result;

    if (!file) return -1;

    result = file->ops->fclose(file->object);
    videoeng_free(file);
    return result;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_file_fread

DESCRIPTION
  This function reads data from video engine file object.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  Number of items that has been read.

SIDE EFFECTS
  None

===========================================================================*/
uint32 video_eng_file_fread(
    void           *buffer,
    uint32          size,    
    uint32          count,
    const VIDEO_ENG_FILE  *file
)
{
    if (!file) return 0;

    return file->ops->fread(buffer, size, count, file->object);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_file_fseek

DESCRIPTION
  This function moves file offset of video engine file object.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
int32 video_eng_file_fseek(
    const VIDEO_ENG_FILE  *file,
    int32           offset,
    int32           origin
)
{
    if (!file) return -1;

    return file->ops->fseek(file->object, offset, origin);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_file_ftell

DESCRIPTION
  This function tells file offset of video engine file object.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
int32 video_eng_file_ftell( const VIDEO_ENG_FILE * file )
{
    if (!file) return -1;

    return file->ops->ftell(file->object);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_file_fwrite

DESCRIPTION
  This function writes data to video engine file object.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  Number of items that has been written.

SIDE EFFECTS
  None

===========================================================================*/
uint32 video_eng_file_fwrite(
    void            *buffer,
    uint32          size,    
    uint32          count,
    const VIDEO_ENG_FILE  *file
)
{
    if (!file) return 0;

    return file->ops->fwrite(buffer, size, count, file->object);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_file_fcopy

DESCRIPTION
  This function copies data from 'in' to 'out'.

DEPENDENCIES
  None

PARAMETERS
  IN VIDEO_ENG_FILE  *out,
    Pointer to video engine file object to which data will be copied.

  IN VIDEO_ENG_FILE  *in,
    Pointer to video engine file object from which data will be copied.

  IN uint32          size,
    Size of data to be copied.

  IN void            *buffer
    A buffer which will be used to copy data in case both 'in' and 'out'
    are all EFS files. If not given, this function allocates buffer
    internally and frees it before exit. If given, its size must not be
    less than 'size'.

RETURN VALUE
  Number of bytes that has been copied.

SIDE EFFECTS
  None

===========================================================================*/
uint32 video_eng_file_fcopy(
    const VIDEO_ENG_FILE  *out,
    const VIDEO_ENG_FILE  *in,
    uint32          size,
    void            *buffer
)
{
    video_eng_mem_object_type *in_obj;
    video_eng_mem_object_type *out_obj;
    uint32                    num_bytesread,num_bytes = 0;
    boolean                   free_buf = FALSE;

    if (!out || !in) return 0;

    if ( in->ops == &video_eng_efs_file_ops && 
        out->ops == &video_eng_efs_file_ops)
    {
        if (!buffer)
        {
            buffer = videoeng_malloc(size);
            free_buf = TRUE;
        }
        if (buffer)
        {
            num_bytesread = video_eng_file_fread(buffer, 1, size, in);
            num_bytes = video_eng_file_fwrite(buffer, 1, num_bytesread, out);
           //Check if we were able to write entire file into destination. 
            if (num_bytesread != num_bytes) 
            {
                /* A value of Zero will indicate to client to that
                   file copy failed.
                */
                CAMCORDER_MSG_ERROR ("File Copy failed in camcorder",0, 0, 0);
                num_bytes = 0;
            }

            if (free_buf)
            {
                videoeng_free(buffer);
                buffer = NULL;
            }
        }
        else
        {
            CAMCORDER_MSG_ERROR ("file_fcopy: Unable to allocate memory", 0, 0, 0);
        }
    }
    else if ( in->ops == &video_eng_efs_file_ops && 
             out->ops == &video_eng_mem_file_ops)
    {
        out_obj = (video_eng_mem_object_type *) out->object;
        num_bytes = MIN(size, (uint32)(out_obj->length - out_obj->pos));
        num_bytes = video_eng_file_fread(out_obj->buf_ptr + out_obj->pos, 1, 
                                            num_bytes, in);
        out_obj->pos += num_bytes;
        out_obj->length = MAX(out_obj->length, out_obj->pos);
    }
    else if ( in->ops == &video_eng_mem_file_ops && 
             out->ops == &video_eng_efs_file_ops)
    {
        in_obj  = (video_eng_mem_object_type *) in->object;
        num_bytes = MIN(size, (uint32)(in_obj->length - in_obj->pos));
        num_bytes = video_eng_file_fwrite(in_obj->buf_ptr + in_obj->pos, 1, 
                                              num_bytes, out);
        in_obj->pos += num_bytes;
    }
    else if ( in->ops == &video_eng_mem_file_ops && 
             out->ops == &video_eng_mem_file_ops)
    {
        in_obj  = (video_eng_mem_object_type *) in->object;
        out_obj = (video_eng_mem_object_type *) out->object;
        num_bytes = MIN(size, (uint32)(in_obj->length - in_obj->pos));
        num_bytes = MIN(num_bytes, (uint32)(out_obj->buf_size - out_obj->pos));
        memcpy(out_obj->buf_ptr + out_obj->pos, in_obj->buf_ptr + in_obj->pos, 
              num_bytes);
        in_obj->pos += num_bytes;
        out_obj->pos += num_bytes;
        /* If out_obj->pos <= out_obj->length, output file won't grow */
        out_obj->length = MAX(out_obj->length, out_obj->pos);
    }

    return num_bytes; /*lint !e429 'buffer' is freed if allocated here. */
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_mem_file

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  TRUE if 'file' points to a file on memory.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean video_eng_mem_file( const VIDEO_ENG_FILE  *file )
{
    if (!file) return FALSE;

    return (file->ops == &video_eng_mem_file_ops);
}


/* <EJECT> */
/*===========================================================================

                            LOCAL FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION video_eng_mem_file_create

DESCRIPTION
  This function creates a video engine memory file object.

DEPENDENCIES
  None

PARAMETERS
  IN uint8 *buf_ptr
    This is the buffer pointer. If NULL, this function allocates memory of
    'buf_size'.

  IN uint32 buf_size
    This is the buffer size. 

RETURN VALUE
  Pointer to video engine memory file object if successful.
  NULL otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static video_eng_mem_object_type * video_eng_mem_file_create
(
    uint8     *buf_ptr, 
    uint32    buf_size
)
{
    video_eng_mem_object_type *object;

    if (!buf_size)
    {
        return NULL;
    }
    object = (video_eng_mem_object_type *) 
                videoeng_malloc(sizeof(video_eng_mem_object_type));
    if (object)
    {
        if (buf_ptr)
        {
            object->buf_ptr = buf_ptr;
            object->free_buf = FALSE;    
        }
        else
        {
            object->buf_ptr = (uint8 *) videoeng_malloc(buf_size);
            object->free_buf = TRUE;    
        }

        if (object->buf_ptr)
        {
            object->buf_size = buf_size;
            object->length = 0;
            object->pos = 0;
        }
        else
        {
            CAMCORDER_MSG_ERROR ("mem_file_create: Unable to allocate memory", 0, 0, 0);
            videoeng_free(object);
            object = NULL;
        }
    }
    else
    {
        CAMCORDER_MSG_ERROR ("mem_file_create: Unable to allocate memory", 0, 0, 0);
    }
    return (object);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_mem_fclose

DESCRIPTION
  This function frees all memory resources and closes a file on memory.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static int32 video_eng_mem_fclose (void * object)
{
    video_eng_mem_object_type *memObj = (video_eng_mem_object_type *) object;

    if (memObj->free_buf)
    {
        videoeng_free(memObj->buf_ptr);
    }
    videoeng_free(memObj);
    return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_mem_fread

DESCRIPTION
  This function reads data from file on memory.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static uint32  video_eng_mem_fread 
(
    void    *buffer, 
    uint32  size, 
    uint32  count, 
    void    *object
)
{
    video_eng_mem_object_type *memObj = (video_eng_mem_object_type *) object;
    uint32                    bytes_read;
    uint32                    count_read = 0;

    if (size)
    {
        count_read = MIN( count, (memObj->length - memObj->pos) / size);
        bytes_read = count_read * size;
        memcpy(buffer, memObj->buf_ptr + memObj->pos, bytes_read);
        memObj->pos += bytes_read;
    }
    return (count_read);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_mem_fseek

DESCRIPTION
  This function moves file offset of file on memory

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static int32  video_eng_mem_fseek 
( 
    void  *object, 
    int32 offset, 
    int32 origin 
)
{
    video_eng_mem_object_type *memObj = (video_eng_mem_object_type *) object;
      int32                     new_pos;

      switch (origin) 
    {
      case SEEK_SET:
            new_pos = offset;
            break;
      case SEEK_END:
            new_pos = (int32) memObj->length - offset;
            break;
      case SEEK_CUR:
            new_pos = (int32) memObj->pos + offset;
            break;
      default:
            return -1;
      }
      if (new_pos < 0 || new_pos > ((int32) memObj->buf_size - 1)) {
            return -1;
      }
      memObj->pos = (uint32) new_pos;
      return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_mem_ftell

DESCRIPTION
  This function tells file offset of file on memory

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static int32 video_eng_mem_ftell ( void * object )
{
    video_eng_mem_object_type *memObj = (video_eng_mem_object_type *) object;

    return (int32) memObj->pos;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_mem_fwrite

DESCRIPTION
  This function writes data to file on memory.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static uint32  video_eng_mem_fwrite 
( 
    void  *buffer, 
    uint32  size, 
    uint32  count, 
    void    *object 
)
{
    video_eng_mem_object_type *memObj = (video_eng_mem_object_type *) object;
    uint32                    bytes_written;
    uint32                    count_written = 0;

    if (size)
    {
        count_written = MIN( count, (memObj->buf_size - memObj->pos) / size);
        bytes_written = count_written * size;
        memcpy(memObj->buf_ptr + memObj->pos, buffer, bytes_written);
        memObj->pos += bytes_written;
        memObj->length = MAX(memObj->length, memObj->pos);
    }
    return (count_written);
}/*lint !e818 "buffer is a non constant pointer in the calling function"*/

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_efs_fclose

DESCRIPTION
  This function closes file on EFS.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static int32 video_eng_efs_fclose (void *object)
{
    return efs_fclose((EFS_FILE *) object);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_efs_fread

DESCRIPTION
  This function reads data from file on EFS.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static uint32  video_eng_efs_fread 
(
    void    *buffer, 
    uint32  size, 
    uint32  count, 
    void    *object
)
{
    return efs_fread(buffer, size, count, (EFS_FILE *) object);  
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_efs_fseek

DESCRIPTION
  This function moves the file offset of file on EFS.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static int32  video_eng_efs_fseek 
( 
    void  *object, 
    int32 offset, 
    int32 origin 
)
{
      return efs_fseek((EFS_FILE *) object, offset, origin);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_efs_ftell

DESCRIPTION
  This function returns the file offset of file on EFS

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static int32 video_eng_efs_ftell ( void *object )
{
      return efs_ftell((EFS_FILE *) object);
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_efs_fwrite

DESCRIPTION
  This function writes data to file on EFS.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static uint32  video_eng_efs_fwrite 
( 
    void    *buffer, 
    uint32  size, 
    uint32  count, 
    void    *object 
)
{
    return efs_fwrite(buffer, size, count, (EFS_FILE *) object);
}

#endif /* defined (FEATURE_EFS2) || defined (T_WINNT) */
