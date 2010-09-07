#ifndef VIDEOENG_FIO_H
#define VIDEOENG_FIO_H
/*===========================================================================

                     V I D E O   E N C O D E   E N G I N E
                             F I L E   O B J E C T
                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the video encode engine file object.

Copyright(c) 2005 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/06   Nagesh  Made code Lint error free
03/17/05   jk      Had videoeng_free(x) set x to NULL.
03/07/05   jk      Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "camcorder_util.h"

#if defined (FEATURE_EFS2) || defined (T_WINNT)
#include "fs_public.h"          /* File system typedefs and prototypes     */
#include "fs_stdio.h"           /* File system buffered I/O header         */
#endif /* defined (FEATURE_EFS2) || defined (T_WINNT) */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#ifdef T_ARM
#define FILE_MODE_RO     "r"
#define FILE_MODE_RW     "w+"
#else
#define FILE_MODE_RO     "rb"
#define FILE_MODE_RW     "wb+"
#endif


#ifdef FEATURE_CAMCORDER_MALLOC
  extern camcorderutil_malloc_context_type *videoeng_malloc_context;

  #define videoeng_malloc(x) \
    camcorderutil_malloc(videoeng_malloc_context,x,__func__,__LINE__)
  #define videoeng_free(x) { static msg_const_type m;\
                             camcorderutil_free(videoeng_malloc_context,\
                                              __LINE__,__func__,&m,x);\
                           }
  #define videoeng_realloc(x,y) \
    camcorderutil_realloc(videoeng_malloc_context,\
                           __LINE__,__func__,x,y)
#else
  #define videoeng_malloc(x)    malloc(x)
  #define videoeng_free(x)      { free(x); x=NULL; }
  #define videoeng_realloc(x,y) realloc(x,y)
#endif

/* <EJECT> */
/*---------------------------------------------------------------------------
** VIDEOENG_FIO Public Function Prototypes
**---------------------------------------------------------------------------
*/

#if defined (FEATURE_EFS2) || defined (T_WINNT)

/* <EJECT> */
/*===========================================================================

             V I D E O   E N G I N E   F I L E   O B J E C T

===========================================================================*/

typedef struct video_eng_file_struct  VIDEO_ENG_FILE;


typedef int32 (* video_eng_fclose_func_ptr_type)
                    (void * object); 

typedef uint32 (* video_eng_fread_func_ptr_type)
            (void * buffer, uint32 size, uint32 count, void * object); 

typedef int32 (* video_eng_fseek_func_ptr_type)
            ( void * object , int32 offset , int32 origin ); 

typedef int32 (* video_eng_ftell_func_ptr_type)
            ( void * object ); 

typedef uint32 (* video_eng_fwrite_func_ptr_type)
        (void * buffer, uint32 size, uint32 count, void * object );


typedef struct {
    video_eng_fclose_func_ptr_type    fclose;
    video_eng_fread_func_ptr_type     fread;
    video_eng_fseek_func_ptr_type     fseek;
    video_eng_ftell_func_ptr_type     ftell;
    video_eng_fwrite_func_ptr_type    fwrite;
} video_eng_file_ops_type;


struct video_eng_file_struct {
    void                      *object;
    video_eng_file_ops_type   *ops;
} ;


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
);

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
);

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
int32 video_eng_file_fclose (VIDEO_ENG_FILE * file);

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
    void            *buffer,
    uint32          size,    
    uint32          count,
    const VIDEO_ENG_FILE  *file
);

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
);

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
int32 video_eng_file_ftell( const VIDEO_ENG_FILE * file );

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
);

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
);

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
boolean video_eng_mem_file( const VIDEO_ENG_FILE  *file );

#endif /* defined (FEATURE_EFS2) || defined (T_WINNT) */
#endif /* VIDEOENG_FIO_H */
