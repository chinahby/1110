 /* =========================================================================

DESCRIPTION
   API for file and directory operations. This is more or less the same as
   the ANSI C API. This sits on top of the DMSS EFS API.

Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.

============================================================================ */

/* =========================================================================

                             Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/fsa.c_v   1.2   13 Nov 2002 16:40:00   rajeevg  $
$Header: //source/qcom/qct/core/services/ansic/main/latest/src/fsa.c#2 $ $DateTime: 2008/01/31 15:46:52 $ $Author: cgross $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/28/08   cwg     Feature-ized use of fpos_t depending on version of ARMCC
09/26/02    gr     Added fflush() and rewind(). Added zrex_fsa_init() for
                   backward compatibility.
05/22/02    gr     Renamed from zrexfsa.c. Removed zrex prefix from various
                   names. Took out support for MSP_PLATFORM_THOR. Removed
                   inclusion of target.h.
07/02/01   kar     Included target.h and customer.h
06/11/00   kar     Created

============================================================================ */

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "customer.h"
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "rex.h"
#include "fs.h"
#include "assert.h"
#include "AEEstd.h"

/* These functions need to be defined only if we are running on target.
** Off target, the libraries we link to provide all these functions.
*/

#if defined (FEATURE_ANSIC_FILE_API)

#ifndef FEATURE_REX_DYNA_MEM
   #error FEATURE_REX_DYNA_MEM needs to be turned on
#endif

/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */
#define FSA_MAX_FILENAME FS_FILENAME_MAX_LENGTH
   /* Maximum number of characters in the fully qualified name of a file.
   */
#define FSA_MODESTRING_SIZE 3
   /* The size of the string field used to store mode information about
   ** a file.
   */
/* ------------------------------------------------------------------------
** Type Definitions
** ------------------------------------------------------------------------ */
typedef struct
{
   list_link_type link;
   FILE          *filehandle;
      /* File's handle (used by both ANSI and FS routines).
      */
   char           filename[ FSA_MAX_FILENAME ];
      /* The name of the file.
      */
   char           mode[ FSA_MODESTRING_SIZE ];
      /* The mode in which the file has been opened.
      */
} fsa_file_list_item_type;
   /* Type for items on the list of open files.
   */

typedef struct
{
   list_link_type        link;
   fs_enum_iterator_type iterator;
      /* Control structure used by fs_enum_next, fs_enum_prev, etc.
      */
} fsa_find_list_item_type;
   /* Type for items on the list used by find_first_file and
   ** find_next_file to return the next file.
   */

typedef fs_enum_data_type fsa_fileinfo_type;
   /* Type for structure that holds information about a file.
   */

typedef struct
{
   char name[ FSA_MAX_FILENAME ];
   char mode[ FSA_MODESTRING_SIZE ];
} fsa_srch_criteria_type;
   /* Structure whose fields correspond to various search criteria
   ** that can be used on files. This structure is not used at the
   ** moment. Fields such as creation/access time, whether open or
   ** closed, etc will be added later.
   */

/* ------------------------------------------------------------------------
** Static Variables
** ------------------------------------------------------------------------ */
static list_type fsa_file_list;
   /* The list of open files.
   */
static list_type fsa_find_list;
   /* The list of find operations currently active.
   */

/* ------------------------------------------------------------------------
** Extern Declarations
** ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
** Macros
** ------------------------------------------------------------------------ */
#define FILEHANDLE_TO_FSHANDLE( handle )\
   (fs_handle_type)((unsigned long) handle & 0xFFFF)

/* ==================================================================
FUNCTION  FSA_INIT
DESCRIPTION
   Initializes this module.
===================================================================== */
void
fsa_init( void )
{
   list_init( &fsa_file_list );
   list_init( &fsa_find_list );
} /* END fsa_init */


/* ==================================================================
FUNCTION  ZREX_FSA_INIT
DESCRIPTION
   Just calls fsa_init. Here for backward compatibility.
===================================================================== */
void
zrex_fsa_init( void )
{
   fsa_init( );
} /* END zrex_fsa_init */


/* ==================================================================
FUNCTION  FSA_FILEHANDLE_COMPARE_FN
DESCRIPTION
   Function used to compare filehandles. Returns 1 if the filehandles
   match and 0 otherwise. Used as an argument to the
   list_linear_search function.
===================================================================== */
static int
fsa_filehandle_compare_fn(
   void *item_ptr,
   void *filehandle
)
{
   fsa_file_list_item_type *fl_item_ptr;
   FILE                    *fl_filehandle;

   fl_item_ptr   = (fsa_file_list_item_type *) item_ptr;
   fl_filehandle = (FILE *) filehandle;

   return ( fl_item_ptr->filehandle == fl_filehandle );
} /* fsa_filehandle_compare_fn */


/* ==================================================================
FUNCTION  FOPEN
DESCRIPTION
   Opens a named file and returns a handle.
   NOTE: Files are opened in binary mode.
===================================================================== */
FILE*
fopen(
   const char *filename,
   const char *mode
)
{
   fs_rsp_msg_type          rsp;
   FILE                    *filehandle;
   fsa_file_list_item_type *item_ptr;

   ASSERT( filename != NULL );
   ASSERT( mode != NULL );

   /* Check whether the file exists.
   */
   fs_test( filename, NULL, &rsp );
   if ( !rsp.nametest.name_found )
   {
      /* Create the file.
      */
      fs_open_xparms_type open_xparms;
      open_xparms.create.cleanup_option   = FS_OC_CLOSE;
      open_xparms.create.buffering_option = FS_OB_ALLOW;
      if ( ( strcmp(mode, "r") == 0 ) || ( strcmp(mode, "rb") == 0) )
      {
         open_xparms.create.attribute_mask = FS_FA_READONLY;
      }
      else
      {
         open_xparms.create.attribute_mask = FS_FA_UNRESTRICTED;
      }

      fs_open( filename, FS_OA_CREATE, &open_xparms, NULL, &rsp );
      if ( rsp.open.status != FS_OKAY_S )
      {
         return NULL;
      }
   }
   else
   {
      /* Open existing file.
      */
      fs_access_option_type fs_mode;

      if ( ( strcmp(mode, "r") == 0 ) || ( strcmp(mode, "rb") == 0 ) )
      {
         fs_mode = FS_OA_READONLY;
      }
      else if ( ( strcmp(mode, "w") == 0 ) || ( strcmp(mode, "wb") == 0 ) )
      {
         /* FS does not seem to support write-only access.
         */
         fs_mode = FS_OA_READWRITE;
      }
      else
      {
         fs_mode = FS_OA_READWRITE;
      }

      fs_open( filename, fs_mode, NULL, NULL, &rsp );
      if ( rsp.open.status != FS_OKAY_S )
      {
         return NULL;
      }
   }

   filehandle = (FILE *) rsp.open.handle;

   /* Update the file list.
   */
   item_ptr = (fsa_file_list_item_type *)
                rex_malloc( sizeof(fsa_file_list_item_type) );
   if ( item_ptr != NULL )
   {
      item_ptr->filehandle = filehandle;
      std_strlcpy(
              item_ptr->filename,
              filename,
              FS_FILENAME_MAX_LENGTH
      );
      memcpy( item_ptr->mode, mode, 2 );
      list_push_back( &fsa_file_list, &item_ptr->link );
   }
   else  /* malloc failed */
   {
      fs_close( FILEHANDLE_TO_FSHANDLE( filehandle ), NULL, &rsp );
      return NULL;
   }

   return filehandle;
} /* END fopen */


/* ==================================================================
FUNCTION  FCLOSE
DESCRIPTION
   Closes a file specified using a filehandle.
===================================================================== */
int
fclose(
   FILE *stream
)
{
   fs_rsp_msg_type rsp;

   fs_close( FILEHANDLE_TO_FSHANDLE( stream ), NULL, &rsp );
   if ( rsp.close.status != FS_OKAY_S )
   {
      return EOF;
   }
   else
   {
      /* Remove the file from the open file list.
      */
      fsa_file_list_item_type *item_ptr;
      item_ptr = (fsa_file_list_item_type *)
                 list_linear_search(
                                    &fsa_file_list,
                                    fsa_filehandle_compare_fn,
                                    stream
                 );
      if ( item_ptr == NULL )
      {
         return EOF;
      }
      else
      {
         list_pop_item( &fsa_file_list, &item_ptr->link );
         rex_free( item_ptr );
      }
   }

   return 0;
} /* END fclose */


/* ==================================================================
FUNCTION  FREAD
DESCRIPTION
   Reads items from a file. Returns the number of full items actually
   read, which may be less than the specified number. If size or count
   is 0, fread returns immediately.

===================================================================== */
size_t
fread(
   void  *data_ptr,
   size_t size,
   size_t nmemb,
   FILE  *stream
)
{
   fs_rsp_msg_type rsp;
   size_t          num_bytes;

   num_bytes = size * nmemb;

   if ( num_bytes == 0 )
   {
      return 0;
   }

   fs_read(
           FILEHANDLE_TO_FSHANDLE( stream ),
           data_ptr,
           num_bytes,
           NULL,
           &rsp
   );

   return (size_t) ( rsp.read.count / size );
} /* END fread */


/* ==================================================================
FUNCTION  FWRITE
DESCRIPTION
   Writes items into a file. Returns the number of full items actually
   written, which may be less than the specified number. If size or
   count is 0, fread returns immediately.

===================================================================== */
size_t
fwrite(
   const void *data_ptr,
   size_t      size,
   size_t      nmemb,
   FILE       *stream
)
{
   fs_rsp_msg_type rsp;
   size_t          num_bytes;

   num_bytes = size * nmemb;

   if ( num_bytes == 0 )
   {
      return 0;
   }

   fs_write(
            FILEHANDLE_TO_FSHANDLE( stream ),
            data_ptr,
            num_bytes,
            NULL,
            &rsp
   );

   return (size_t) ( rsp.write.count / size );
} /* END fwrite */


/* ==================================================================
FUNCTION  FSEEK
DESCRIPTION
   Moves the file pointer for a specified file to a specified position.
===================================================================== */
int
fseek(
   FILE *stream,
   long  offset,
   int   whence
)
{
   fs_rsp_msg_type          rsp;
   fs_seek_origin_type      fs_origin;
   fsa_file_list_item_type *item_ptr;

   switch ( whence )
   {
      case SEEK_SET:
         fs_origin = FS_SEEK_SET;
         break;

      case SEEK_CUR:
         fs_origin = FS_SEEK_CURRENT;
         break;

      case SEEK_END:
         fs_origin = FS_SEEK_EOF;
         break;

      default:
         /* This is an error.
         */
         return -1;
   }

   if ( offset < 0 )
   {
      fs_origin = FS_SEEK_SET;
      if ( whence == SEEK_CUR )
      {
         offset += ftell( stream );
         if ( offset < 0 )
         {
            return -1;
         }
      }

      if ( whence == SEEK_END )
      {
         item_ptr = (fsa_file_list_item_type *)
                    list_linear_search(
                                       &fsa_file_list,
                                       fsa_filehandle_compare_fn,
                                       stream
                    );
         if ( item_ptr )
         {
            fs_file_size( item_ptr->filename, NULL, &rsp );

            if ( rsp.file_size.status == FS_OKAY_S )
            {
               offset += rsp.file_size.size;
            }
            else
            {
               return -1;
            }
         }
         else
         {
            return -1;
         }
      }
   }

   fs_seek(
           FILEHANDLE_TO_FSHANDLE( stream ),
           fs_origin,
           (fs_file_position_type) offset,
           NULL,
           &rsp
   );
 
   if ( rsp.seek.status == FS_OKAY_S )
   {
      return 0;
   }
   else
   {
      return -1;
   }
} /* END fseek */


/* ==================================================================
FUNCTION  FTELL
DESCRIPTION
   Moves the file pointer for a specified file to a specified position.
===================================================================== */
long
ftell(
   FILE *stream
)
{
   fs_rsp_msg_type rsp;

   fs_tell( FILEHANDLE_TO_FSHANDLE( stream ), NULL, &rsp );
 
   if ( rsp.tell.status == FS_OKAY_S )
   {
      return (long) rsp.tell.position;
   }
   else
   {
      return -1;
   }
} /* END ftell */


/* ==================================================================
FUNCTION  FGETPOS
DESCRIPTION
   Retrieves the file pointer and stores it in the variable pointed
   to by pos_ptr. Returns 0 on success and -1 on failure.
===================================================================== */
int
fgetpos(
   FILE   *stream,
   fpos_t *pos_ptr
)
{
   fs_rsp_msg_type rsp;

   fs_tell( FILEHANDLE_TO_FSHANDLE( stream ), NULL, &rsp );
 
   if ( rsp.tell.status == FS_OKAY_S )
   {

#if __ARMCC_VERSION >= 300000   
      pos_ptr->__pos = (unsigned long) rsp.tell.position;
#else
      pos_ptr->__lo = (unsigned long) rsp.tell.position;
#endif

      return 0;
   }
   else
   {
      return -1;
   }
} /* END fgetpos */


/* ==================================================================
FUNCTION  FSETPOS
DESCRIPTION
   Sets the file pointer to the value in the variable pointed to by
   pos_ptr. Returns 0 on success and -1 on failure.
===================================================================== */
int
fsetpos(
   FILE         *stream,
   const fpos_t *pos_ptr
)
{
   fs_rsp_msg_type rsp;

   fs_seek(
           FILEHANDLE_TO_FSHANDLE( stream ),
           FS_SEEK_SET,
#if __ARMCC_VERSION >= 300000
           (fs_file_position_type) pos_ptr->__pos,
#else           
           (fs_file_position_type) pos_ptr->__lo,
#endif
           NULL,
           &rsp
   );
 
   if ( rsp.seek.status == FS_OKAY_S )
   {
      return 0;
   }
   else
   {
      return -1;
   }
} /* END fsetpos */


/* ==================================================================
FUNCTION  FEOF
DESCRIPTION
   Returns 1 if the current file pointer is at the end of the file
   and 0 otherwise.
===================================================================== */
int
feof(
   FILE *stream
)
{
   fs_rsp_msg_type          rsp;
   fsa_file_list_item_type *item_ptr;
   char                    *filename;
   unsigned long            filesize;

   /* Get the size of the file, get the current position in the file,
   ** and compare the two. If the current position is greater than or
   ** equal to the size, then return 1, else return 0.
   */
   item_ptr = (fsa_file_list_item_type *)
              list_linear_search(
                                 &fsa_file_list,
                                 fsa_filehandle_compare_fn,
                                 stream
              );
   if ( item_ptr )
   {
      filename = item_ptr->filename;
      fs_file_size( filename, NULL, &rsp );

      if ( rsp.file_size.status == FS_OKAY_S )
      {
         filesize = rsp.file_size.size;
         fs_tell( FILEHANDLE_TO_FSHANDLE( stream ), NULL, &rsp );
         if ( rsp.file_size.status == FS_OKAY_S )
         {
             if ( rsp.tell.position >= filesize )
             {
                return 1;
             }
         }
      }
   }

   return 0;
} /* END feof */


/* ==================================================================
FUNCTION  RMDIR
DESCRIPTION
   Removes a directory.
===================================================================== */
int
rmdir(
   const char *dirname
)
{
   fs_rsp_msg_type rsp;
 
   fs_rmdir( dirname, NULL, &rsp );

   if ( rsp.rmdir.status == FS_OKAY_S )
   {
      return 0;
   }
   else
   {
      return -1;
   }
} /* END rmdir */


/* ==================================================================
FUNCTION  MKDIR
DESCRIPTION
   Creates a directory.
===================================================================== */
int
mkdir(
   const char *dirname
)
{
   fs_rsp_msg_type rsp;
 
   fs_mkdir( dirname, NULL, &rsp );

   if ( rsp.mkdir.status == FS_OKAY_S )
   {
      return 0;
   }
   else
   {
      return -1;
   }
} /* END mkdir */


/* ==================================================================
FUNCTION  CHDIR
DESCRIPTION
   Changes the current directory to a specified directory.
===================================================================== */
int
chdir(
   const char *dirname
)
{
   /* Fake out LINT */
	(void) dirname; 
 
   /* STUB */
   return 0;

} /* END chdir */


/* ==================================================================
FUNCTION  LIST_FILES
DESCRIPTION
   Returns the list of files in a specified directory. A pointer to
   an array of strings must be passed in. The user can malloc this
   array by using ( think this through... )
===================================================================== */
void
list_files(
   const char *dirname,
   char      **dir_list_ptr
)
{
   return;
} /* END list_files */


/* ==================================================================
FUNCTION  FIND_FIRST_FILE
DESCRIPTION
   Returns information about the first file in a directory that
   matches the specified file properties. Returns a NON-NULL handle
   on success and NULL on failure.

   NOTE: For now, the specified search criteria are ignored.
===================================================================== */
void *
find_first_file(
   const char                   *dirname,
      /* Name of directory in which to search.
      */
   const fsa_srch_criteria_type *find_info,
      /* Structure that can be used to pass in search criteria.
      */
   fsa_fileinfo_type            *fileinfo_ptr
      /* Pointer to structure to hold file information.
      */
)
{
   fs_rsp_msg_type          rsp;
   fsa_find_list_item_type *item_ptr;
   fs_enum_data_type       *file_data_ptr;

   ASSERT( fileinfo_ptr != NULL );

   item_ptr = (fsa_find_list_item_type * ) 
              rex_malloc( sizeof(fsa_find_list_item_type) );
   if ( item_ptr == NULL )
   {
      return NULL;
   }

   fs_enum_init(
                dirname,
                FS_ENUM_FILES,
                &item_ptr->iterator,
                NULL,
                &rsp
   );

   if ( rsp.enum_init.status != FS_OKAY_S )
   {
      rex_free( item_ptr ); 
      return NULL;
   }

   file_data_ptr = (fs_enum_data_type *) fileinfo_ptr;
   fs_enum_next(
                file_data_ptr,
                &item_ptr->iterator,
                NULL,
                &rsp
   );

   if ( rsp.enum_next.status != FS_OKAY_S )
   {
      fs_enum_finish(
                     &item_ptr->iterator,
                     NULL,
                     &rsp
      );
      rex_free( item_ptr ); 
      return NULL;
   }

   list_push_back( &fsa_find_list, &item_ptr->link );

   return (void *) item_ptr;
} /* END find_first_file */


/* ==================================================================
FUNCTION  FIND_NEXT_FILE
DESCRIPTION
   Returns information about the next file in a directory that
   matches the specified file properties. Returns 0 on success and
   -1 on failure.

   NOTE: For now, the specified search criteria are ignored.
===================================================================== */
int
find_next_file(
   void              *handle,
      /* Handle obtained using a call to find_first_file.
      */
   fsa_fileinfo_type *fileinfo_ptr
      /* Pointer to structure to hold file information.
      */
)
{
   fs_rsp_msg_type          rsp;
   fsa_find_list_item_type *item_ptr;
   fs_enum_data_type       *file_data_ptr;

   ASSERT( handle != NULL );
   ASSERT( fileinfo_ptr != NULL );

   item_ptr      = (fsa_find_list_item_type *) handle;
   file_data_ptr = (fs_enum_data_type *) fileinfo_ptr;

   fs_enum_next(
                file_data_ptr,
                &item_ptr->iterator,
                NULL,
                &rsp
   );

   if ( rsp.enum_next.status != FS_OKAY_S )
   {
      fs_enum_finish(
                     &item_ptr->iterator,
                     NULL,
                     &rsp
      );
      list_pop_item( &fsa_find_list, &item_ptr->link );
      rex_free( item_ptr ); 
      return -1;
   }

   return 0;
} /* END find_next_file */


/* ==================================================================
FUNCTION  FIND_PREV_FILE
DESCRIPTION
   Returns information about the previous file in a directory that
   matches the specified file properties. Returns 0 on success and
   -1 on failure.

   NOTE: For now, the specified search criteria are ignored.
===================================================================== */
int
find_prev_file(
   void              *handle,
      /* Handle obtained using a call to find_first_file.
      */
   fsa_fileinfo_type *fileinfo_ptr
      /* Pointer to structure to hold file information.
      */
)
{
   fs_rsp_msg_type          rsp;
   fsa_find_list_item_type *item_ptr;
   fs_enum_data_type       *file_data_ptr;

   ASSERT( handle != NULL );
   ASSERT( fileinfo_ptr != NULL );

   item_ptr      = (fsa_find_list_item_type *) handle;
   file_data_ptr = (fs_enum_data_type *) fileinfo_ptr;

   fs_enum_prev(
                file_data_ptr,
                &item_ptr->iterator,
                NULL,
                &rsp
   );

   if ( rsp.enum_prev.status != FS_OKAY_S )
   {
      fs_enum_finish(
                     &item_ptr->iterator,
                     NULL,
                     &rsp
      );
      list_pop_item( &fsa_find_list, &item_ptr->link );
      rex_free( item_ptr ); 
      return -1;
   }

   return 0;
} /* END find_prev_file */


/* ==================================================================
FUNCTION  FIND_CLOSE
DESCRIPTION
   Closes a file search handle.
===================================================================== */
void
find_close(
   void *handle
      /* Handle obtained using a call to find_first_file.
      */
)
{
   fs_rsp_msg_type          rsp;
   fsa_find_list_item_type *item_ptr;

   ASSERT( handle != NULL );

   item_ptr = (fsa_find_list_item_type *) handle;

   fs_enum_finish(
                  &item_ptr->iterator,
                  NULL,
                  &rsp
   );
   list_pop_item( &fsa_find_list, &item_ptr->link );
   rex_free( item_ptr );

   return;
} /* END find_close */


/* ==================================================================
FUNCTION  REWIND
DESCRIPTION
   Sets a file pointer to the beginning of the filestream.
===================================================================== */
void
rewind(
   FILE *stream
)
{
   (void) fseek( stream, 0, SEEK_SET );

   return;
} /* END rewind */


/* ==================================================================
FUNCTION  FFLUSH
DESCRIPTION
   Flushes a filestream. Since we buffer nothing, this function does
   nothing but return 0 indicating a successful flush.
===================================================================== */
int
fflush(
   FILE *stream
)
{
   /* Avoid compiler warning. */
   (void) stream;

   return 0;
} /* END fflush */

#endif /* FEATURE_ANSIC_FILE_API */

