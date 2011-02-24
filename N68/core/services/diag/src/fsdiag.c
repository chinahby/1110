/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Packet Processing

General Description
  Diagnostic packet processing routines for file system access.

Copyright (c) 1990-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //depot/asic/msmshared/services/diag/fsdiag.c#22 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/05    as     Fixed lint errors.
04/04/05    as     Fixed RVCT compilation errors.
02/18/04    as     Fixed critical lint errors.
08/12/04    as     Removed support for Remote file link.
06/23/04    as     Fixes for RVCT 2.1
05/18/04    as     Added security check to fsdiag_cmd.
11/05/03    pg     Added support for fs_format function.
07/10/03    gr     Moved access validation code into its own file.
06/23/07   jkl     Featurized for EFS2.
04/07/03   jct     Added file access validation callbacks which invoke a
                   registered callback for any file system access via
                   DIAG - the callback returns a status indicating whether or
                   not the operation may proceed.  If no callback exists
                   the operation proceeds.
12/02/02   lad     Added retry mechanism to read operation.
08/20/01   jal     Updated for core VU
09/06/00   jal     Incorporated David Brown's change to stop EFS file
                   descriptor leakage
05/10/00   lad     Updated usage of diagpkt_shorten().
03/23/00   lad     Fixed iterative listings.
02/28/00   lad     Changed functions to use diagbuf allocation services.
01/20/99   lad     Removed unnecessary debug MSG_MED calls.
11/16/99   lad     Set attrib uses existing creation date if -1 is set.
                   fs_write overwrite fix.
07/23/99   lad     Added rev 2 of EFS support.
03/26/99   lad     Created for Embedded Filesystem (EFS) support.  Write/MKDIR
===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "comdef.h"
#include "customer.h"

#include "diagbuf.h"
#include "diagcmd.h"
#include "fsdiag.h"

#include "diag.h"
#include "msg.h"
#include <string.h>
#include "fs.h"

/* Tell lint that we are OK with the fact that pkt_len and req are not 
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req) */
  
/*--------------------------------------------------------
  Gloabal data for cleanup of incomplete file transfers
--------------------------------------------------------*/
typedef enum
{
  FSDIAG_NONE,
  FSDIAG_READ,
  FSDIAG_WRITE
} fsdiag_operation_enum_type;

LOCAL fsdiag_operation_enum_type fsdiag_op = FSDIAG_NONE;
LOCAL fs_handle_type fsdiag_handle = NULL;
LOCAL char fsdiag_filename[FSDIAG_MAX_FILENAME_LEN] = {'\0'};


/*===========================================================================

FUNCTION FSDIAG_CLEANUP
DESCRIPTION
  This function cleans up (closes, etc) files operations that are were not 
  completed.

============================================================================*/
static boolean fsdiag_cleanup ()
{
  boolean bRetVal = TRUE;
  fs_rsp_msg_type fs_rsp;  /* FS requires we specify a response message */

  if (fsdiag_handle != NULL || fsdiag_filename[0] != NULL) 
  {
    fs_close(fsdiag_handle, NULL, &fs_rsp);

    if (fsdiag_op == FSDIAG_WRITE)
    {
      /*--------------------------------------------------------
       Remove the stagnant file - if not same file, who cares.
      --------------------------------------------------------*/
      fs_remove(fsdiag_filename, NULL, &fs_rsp);

      if (fs_rsp.rmfile.status != FS_OKAY_S) 
      {
        bRetVal = FALSE;
      }
    }
  }
  
  /*---------------------------------------------
   Initialize file handle and saved filename
  ---------------------------------------------*/
  fsdiag_handle = NULL;
  fsdiag_filename[0] = NULL;
  fsdiag_op = FSDIAG_NONE;

  return (bRetVal);

} /* fsdiag_cleanup */



/*===========================================================================

FUNCTION FSDIAG_MKDIR
DESCRIPTION
  This function handles FS "Create Directory" commands in DIAG.
  
============================================================================*/
static PACKED void * fsdiag_mkdir (
  fsdiag_mkdir_req_type *req,
  word pkt_len
)
{              
  fs_rsp_msg_type fs_rsp;  /* FS requires we specify a response message */
  fsdiag_rsp_pkt_type *rsp;
  const dword rsp_len = FPOS(fsdiag_rsp_pkt_type, fs_rsp);

  /*--------------------------------
   Check for valid packet length.
  --------------------------------*/
  if (pkt_len != req->len + sizeof(req->len)) 
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len));
  }

  FS_DIAG_VALIDATE_ACCESS( MKDIR, (const char *)req->name );

  rsp = (fsdiag_rsp_pkt_type *)diagpkt_alloc(DIAG_FS_OP_F, rsp_len);
                                
  if(rsp)
  {
    /*--------------------------------
    Check for valid filename length.
    --------------------------------*/
    fs_mkdir((const char *)req->name, NULL, &fs_rsp);

    rsp->fs_status = fs_rsp.mkdir.status;
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_mkdir.", 0, 0, 0);
  }
  return (rsp);

} /* fsdiag_mkdir */
      
      

/*===========================================================================

FUNCTION FSDIAG_DISP_DIRS
DESCRIPTION
  This function handles FS "Display Directory List" commands in DIAG.
  
============================================================================*/
static PACKED void *fsdiag_disp_dirs (
  fsdiag_disp_dirs_req_type *req,
  word pkt_len
)
{
  fs_rsp_msg_type fs_rsp;  /* FS requires we specify a response message */
  fsdiag_rsp_pkt_type *rsp;
  dword rsp_len = FPOS(fsdiag_rsp_pkt_type, fs_rsp);

  /*--------------------------------
   Check for valid packet length.
  --------------------------------*/
  if (pkt_len != req->len + sizeof(req->len)) 
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len));
  }

  FS_DIAG_VALIDATE_ACCESS( DISP_DIRS, (const char *)req->name );

  /*----------------------------------------------------------------
    Unknown response size.  Therefore, we will allocate the max
    possible size for this packet and reduce length later.
  ----------------------------------------------------------------*/
  rsp = (fsdiag_rsp_pkt_type *)diagpkt_alloc(DIAG_FS_OP_F, rsp_len + FSIZ(fsdiag_rsp_type, disp_dirs));

  if(rsp)
  {
    do
    {  
      /*--------------------------------
       Get the number of directories.
      --------------------------------*/
      /* Old versions of FS do not always properly set this variable */ 
      fs_rsp.num_dirs.num_dirs = 0;
  
      fs_num_dirs((const char *)req->name, NULL, &fs_rsp);
  
      if (fs_rsp.num_dirs.status == FS_OKAY_S)
      {
        rsp->fs_rsp.disp_dirs.num_dirs = fs_rsp.num_dirs.num_dirs;
      }
      else
      {
        break;  
      }
  
      /*-------------------------------------
        Now get the actual directory list 
      --------------------------------------*/
      fs_list_dirs((const char *)req->name,
                   (void*) rsp->fs_rsp.disp_dirs.dir_list.data,
                   FSDIAG_MAX_FILE_BLOCK_SIZE,
                   NULL,
                   &fs_rsp);
    
      if (fs_rsp.list_dirs.status == FS_OKAY_S)
      {
        rsp->fs_rsp.disp_dirs.dir_list.len = 
          (word) fs_rsp.list_dirs.count;
  
        rsp_len += sizeof(rsp->fs_rsp.disp_dirs.num_dirs)     +
                   sizeof(rsp->fs_rsp.disp_dirs.dir_list.len) +
                   rsp->fs_rsp.disp_dirs.dir_list.len;
      }
    /*lint -save -e717 */
    } while (0);
   /*lint -restore */

    rsp->fs_status = fs_rsp.any.status;
  
    /*------------------------------------------
      If necessary, shorten the length of the 
      response to the actual size
    ------------------------------------------*/
    diagpkt_shorten(rsp, rsp_len);
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_disp_dirs.", 0, 0, 0);
  }
  return (rsp);

} /* fsdiag_disp_dirs */



/*===========================================================================

FUNCTION FSDIAG_DISP_FILES
DESCRIPTION
  This function handles FS "Display File List" commands in DIAG.

============================================================================*/
static PACKED void *fsdiag_disp_files (
  fsdiag_disp_files_req_type *req,
  word pkt_len
)
{
  fs_rsp_msg_type fs_rsp;  /* FS requires we specify a response message */
  fsdiag_rsp_pkt_type *rsp;
  dword rsp_len = FPOS(fsdiag_rsp_pkt_type, fs_rsp);

  /*--------------------------------
   Check for valid packet length.
  --------------------------------*/
  if (pkt_len != req->len + sizeof(req->len)) 
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len));
  }

  FS_DIAG_VALIDATE_ACCESS( DISP_FILES, (const char *)req->name );

  /*----------------------------------------------------------------
    Unknown response size.  Therefore, we will allocate the max
    possible size for this packet and reduce length later.
  ----------------------------------------------------------------*/
  rsp = (fsdiag_rsp_pkt_type *)diagpkt_alloc(DIAG_FS_OP_F, rsp_len + FSIZ(fsdiag_rsp_type, disp_files));

  if(rsp)
  {
    do
    {
      /*--------------------------------
       Get the number of files.
      --------------------------------*/
      /* Old versions of FS don't always set this variable. */
      fs_rsp.num_files.num_files = 0;
  
      fs_num_files((const char *)req->name, NULL, &fs_rsp);
  
      rsp->fs_status = fs_rsp.num_files.status;
  
      if (fs_rsp.num_files.status == FS_OKAY_S)
      {
        rsp->fs_rsp.disp_files.num_files = fs_rsp.num_files.num_files;
      }
      else
      {
        break;  
      }
  
      /*------------------------------
        Now get the actual file list 
      -------------------------------*/
      fs_list_files((const char *)req->name,
                    (void*) rsp->fs_rsp.disp_files.file_list.data,
                    FSDIAG_MAX_FILE_BLOCK_SIZE,
                    NULL,
                    &fs_rsp);
    
      if (fs_rsp.list_files.status == FS_OKAY_S)
      {
        rsp->fs_rsp.disp_files.file_list.len = 
          (word) fs_rsp.list_files.count;
  
        rsp_len += sizeof(rsp->fs_rsp.disp_files.num_files)     +
                   sizeof(rsp->fs_rsp.disp_files.file_list.len) +
                   rsp->fs_rsp.disp_files.file_list.len;
      }
    /*lint -save -e717 */
    } while (0);
    /*lint -restore */

    rsp->fs_status = fs_rsp.any.status;
  
    /*------------------------------------------
      If necessary, shorten the length of the 
      response to the actual size
    ------------------------------------------*/
    diagpkt_shorten(rsp, rsp_len);
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_disp_files.", 0, 0, 0);
  }

  return (rsp);

} /* fsdiag_disp_files */



/*===========================================================================

FUNCTION FSDIAG_RMDIR
DESCRIPTION
  This function handles FS "Remove Directory" commands in DIAG.

============================================================================*/
static PACKED void *fsdiag_rmdir (
  fsdiag_rmdir_req_type *req,
  word pkt_len
)
{              
  fs_rsp_msg_type fs_rsp;  /* FS requires we specify a response message */
  fsdiag_rsp_pkt_type *rsp;
  const dword rsp_len = FPOS(fsdiag_rsp_pkt_type, fs_rsp);

  /*--------------------------------
   Check for valid packet length.
  --------------------------------*/
  if (pkt_len != req->len + sizeof(req->len)) 
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len));
  }

  FS_DIAG_VALIDATE_ACCESS( RMDIR, (const char *)req->name );

  rsp = (fsdiag_rsp_pkt_type *)diagpkt_alloc(DIAG_FS_OP_F, rsp_len);

  if(rsp)
  {
    fs_rmdir((const char *)req->name, NULL, &fs_rsp);
    rsp->fs_status = fs_rsp.rmdir.status;
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_rmdir.", 0, 0, 0);
  }
  return (rsp);

} /* fsdiag_rmdir */



/*===========================================================================

FUNCTION FSDIAG_GET_ATTRIB
DESCRIPTION
  This function handles FS "Get File Attributes" commands in DIAG.

============================================================================*/
static PACKED void *fsdiag_get_attrib (
  fsdiag_get_attrib_req_type *req,
  word pkt_len
)
{              
  fs_rsp_msg_type fs_rsp;  /* FS requires we specify a response message */
  fsdiag_rsp_pkt_type *rsp;
  dword rsp_len = FPOS(fsdiag_rsp_pkt_type, fs_rsp);
                                                        
  /*--------------------------------
   Check for valid packet length.
  --------------------------------*/
  if (pkt_len != req->len + sizeof(req->len))
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len));
  }

  FS_DIAG_VALIDATE_ACCESS( GET_ATTRIB, (const char *)req->name );

  /*---------------------------------
   Check for valid filename length.
  ---------------------------------*/
  fs_get_file_attributes((const char *)req->name, NULL, &fs_rsp);

  if (fs_rsp.get_attribs.status == FS_OKAY_S)
  {
    rsp_len += FSIZ(fsdiag_rsp_type, get_attrib);
  }

  rsp = (fsdiag_rsp_pkt_type *)diagpkt_alloc(DIAG_FS_OP_F, rsp_len);

  if(rsp)
  {
    rsp->fs_status = fs_rsp.get_attribs.status;
    
    if (fs_rsp.get_attribs.status == FS_OKAY_S)
    {
      rsp->fs_rsp.get_attrib.attrib.attribute_mask = 
        fs_rsp.get_attribs.attributes;
    
      rsp->fs_rsp.get_attrib.attrib.buffering_option = 
        fs_rsp.get_attribs.buffering_option;
    
      rsp->fs_rsp.get_attrib.attrib.cleanup_option = 
        fs_rsp.get_attribs.cleanup_option;

      rsp->fs_rsp.get_attrib.creation_date = 
        fs_rsp.get_attribs.creation_date;
    
      /*------------------------------------------------ 
         Event though it is not a attribute in the EFS, 
         file size would be a handy thing to know.
      ------------------------------------------------*/
      fs_file_size((const char *)req->name, NULL, &fs_rsp);

      if (fs_rsp.file_size.status == FS_OKAY_S)
      {
        rsp->fs_rsp.get_attrib.size = fs_rsp.file_size.size;
      }
      else
      {
        rsp->fs_rsp.get_attrib.size = 0;
      }
    }
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_get_attrib.", 0, 0, 0);
  }

  return (rsp);

} /* fsdiag_get_attrib */


/*===========================================================================

FUNCTION FSDIAG_SET_ATTRIB
DESCRIPTION
  This function handles FS SET ATTRIB commands in DIAG.
  
============================================================================*/
static PACKED void *fsdiag_set_attrib (
  fsdiag_set_attrib_req_type *req,
  word pkt_len
)
{              
  fs_rsp_msg_type fs_rsp;  /* FS requires we specify a response message */
  fsdiag_rsp_pkt_type *rsp;
  const uint32 rsp_len = FPOS(fsdiag_rsp_pkt_type, fs_rsp);

  /*--------------------------------
   Check for valid packet length.
  --------------------------------*/
  if (pkt_len != req->filename_info.len         + 
                 sizeof(req->filename_info.len) +
                 sizeof(req->creation_date)     +
                 sizeof(req->attribs))
  {
      return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len));
  }

  FS_DIAG_VALIDATE_ACCESS( SET_ATTRIB, (const char *)req->filename_info.name );

  rsp = (fsdiag_rsp_pkt_type *) diagpkt_alloc (DIAG_FS_OP_F, rsp_len);

  if(rsp)
  {
    /*---------------------------------------------------------------
     Since not all attributes will be externally settable via DIAG, 
     we need to get the existing attributes to set them back.
    ---------------------------------------------------------------*/
    if (req->creation_date == 0xFFFFFFFFU) 
    {
      fs_get_attribs_rsp_type get_rsp;

      fs_get_file_attributes((const char *)req->filename_info.name,
                             NULL,
                             (fs_rsp_msg_type *) &get_rsp);

      if (get_rsp.status == FS_OKAY_S)
      {
        req->creation_date = get_rsp.creation_date;
      }
      else
      {
        rsp->fs_status = FS_FAIL_S;
        return (rsp);
      }
    }

    fs_set_file_attributes(
      (const char *)req->filename_info.name, /* Absolute dir name  */
      (fs_attribute_mask_type) req->attribs.attribute_mask,
      (fs_file_date_type) req->creation_date,
      (fs_buffering_option_type) req->attribs.buffering_option,
      (fs_cleanup_option_type) req->attribs.cleanup_option,
      NULL,
      &fs_rsp);
  
    rsp->fs_status = fs_rsp.set_attribs.status;
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_set_attrib.", 0, 0, 0);
  }
    
  return (rsp);

} /* fsdiag_set_attrib */
      
      

/*===========================================================================

FUNCTION FSDIAG_READ
DESCRIPTION
  This function handles FS read commands in DIAG.

============================================================================*/
static PACKED void *fsdiag_read (
  fsdiag_read_req_type *req,
  word pkt_len
)
{                                                                         
  fs_rsp_msg_type            fs_rsp;  /* Response from FS task             */
  fsdiag_data_block_type *data_ptr;  /* Pointer to data block in packet   */
  
  static byte      next_seq_num = 0;  /* Expected next seqeunce number.    */
  static fs_file_position_type last_pos = 0xFFFFFFFFU;
  word             expected_pkt_len;  /* Used to calculate packet length   */
  fsdiag_rsp_pkt_type *rsp;
  dword rsp_len = FPOS(fsdiag_rsp_pkt_type, fs_rsp);                      
  
  /* Keep progress stats
  */
  static dword           read_so_far;      
  static dword     total_file_length;

  fs_rsp.any.status = FS_FAIL_S; /* Init to be a generic failure. */

  /* Do all error checking first
  */
  if (req->seq_num == 0) 
  {

    FS_DIAG_VALIDATE_ACCESS( READ, (const char *)req->filename_info.name );

    /*--------------------------------
     Check for valid packet length.
    --------------------------------*/
    expected_pkt_len = 
      sizeof(req->seq_num)           + 
      sizeof(req->filename_info.len) + 
      req->filename_info.len;

    if (pkt_len != expected_pkt_len) 
    {
      return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len));
    }
  }
  else if (next_seq_num == req->seq_num)
  {
    /*--------------------------------
     Check for valid packet length.
    --------------------------------*/
    expected_pkt_len = sizeof(req->seq_num);

    if (pkt_len != expected_pkt_len) 
    {
      (void) fsdiag_cleanup();
      next_seq_num = 0;

      return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len));
    }
  }
  else /* Bad sequence number */
  {
    if (req->seq_num == (next_seq_num - 1) || 
        ((req->seq_num - next_seq_num) >= 254) ) // Trying a retry
    {
      // Set the next sequence number to the current seq number 
      next_seq_num = req->seq_num;

      /* Reset the file position to last successful. */
      fs_seek (fsdiag_handle, FS_SEEK_SET, last_pos, NULL, &fs_rsp);

      if (fs_rsp.seek.status != FS_OKAY_S)
      {
        (void) fsdiag_cleanup();
        next_seq_num = 0;

        return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req, pkt_len));
      }
    }
    else 
    {
      (void) fsdiag_cleanup();
      next_seq_num = 0;

      return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req, pkt_len));
    }
  } /* if seq num 0 */

    /*--------------------------------------
   Allocate max size, then shorten later.
  --------------------------------------*/
  rsp = (fsdiag_rsp_pkt_type *)
    diagpkt_alloc(DIAG_FS_OP_F, rsp_len + FSIZ(fsdiag_rsp_type, read));

  if(rsp == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_read.", 0, 0, 0);
    return(rsp);
  }

  rsp->fs_rsp.read.seq_num = req->seq_num;

  /* Initialize to success */
  fs_rsp.any.status = FS_OKAY_S;

  /*-------------------------------------------------
    Check if this is to start a new file 
    transfer, or continue an old one.
  -------------------------------------------------*/
do
{
  if (req->seq_num == 0) 
  {
    /*---------------------------------------------
     Initialize local vars.
    ---------------------------------------------*/
    (void) fsdiag_cleanup();

    total_file_length = 0;
    read_so_far       = 0;

    /*----------------------------
     Open file for writing.
    ----------------------------*/
    fs_open((const char *)req->filename_info.name, FS_OA_READONLY, NULL, NULL, &fs_rsp);
   
    /* Save handle for subsequent reads */
    if ((fsdiag_handle = fs_rsp.open.handle) == NULL || 
        fs_rsp.any.status != FS_OKAY_S) 
    {
      (void) fsdiag_cleanup();
      next_seq_num = 0;

      rsp->fs_status = fs_rsp.open.status;

      break;
    }
    else
    {
      last_pos = 0;
    }

    /*-----------------------------------------------
     Save file name info for possible cleanup later
    -----------------------------------------------*/
    memcpy (fsdiag_filename, 
            (void *) req->filename_info.name,
            req->filename_info.len);

    /*-------------------------------------------
     Store operation for open file for cleanup.
    -------------------------------------------*/
    fsdiag_op = FSDIAG_READ;

    /*--------------------
     Record total length
    --------------------*/
    fs_file_size(fsdiag_filename, NULL, &fs_rsp);

    if (fs_rsp.file_size.status != FS_OKAY_S) 
    {
      (void) fsdiag_cleanup();
      next_seq_num = 0;
      
      rsp->fs_status = fs_rsp.file_size.status;

      break;
    }
    
    rsp->fs_rsp.read.data.begin.total_length = 
      total_file_length = fs_rsp.file_size.size;

    /*----------------------------------------
      Update response length when successful
    ----------------------------------------*/
    rsp_len += sizeof(rsp->fs_rsp.read.data.begin.total_length);

    /*----------------------------------------
      Set pointer to beginning of data block
    ----------------------------------------*/
    data_ptr = (fsdiag_data_block_type*) 
               &rsp->fs_rsp.read.data.begin.block;

  }
  else /* Seqeunce num != 0 */
  {
    data_ptr = (fsdiag_data_block_type*) 
                 &rsp->fs_rsp.read.data.block;
  }
  
  /* Record position prior to this read. */
  fs_tell (fsdiag_handle, NULL, &fs_rsp);
  
  if (fs_rsp.tell.status == FS_OKAY_S)
  {
    last_pos = fs_rsp.tell.position;
    
    /* Go ahead and read. */
/*lint -save -e545 Suspicious use of & */
    fs_read(fsdiag_handle,
            (PACKED void*)&(data_ptr->data),
            FSDIAG_MAX_FILE_BLOCK_SIZE,
            NULL,
            &fs_rsp);
/*lint -restore */
  }
  
  rsp->fs_status = fs_rsp.any.status;
  
  if (fs_rsp.any.status != FS_OKAY_S) 
  {
    (void) fsdiag_cleanup();
    next_seq_num = 0;
  }
  else
  {
    data_ptr->len = (word) fs_rsp.read.count;

    /*-------------------------------------------------------------- 
      Calculate response length.  It already has the value for the 
      preliminary data such as status, so just add block length. 
    --------------------------------------------------------------*/
    rsp_len += FPOS(fsdiag_read_rsp_type, data) +
               data_ptr->len                    +
               sizeof(data_ptr->len);

    read_so_far += data_ptr->len;

    /*----------------------------------------------------------
      Check if we read an expected amount, and determine if we 
      need more blocks to complete the transfer. 
    ----------------------------------------------------------*/
    if (read_so_far < total_file_length)
    {
      rsp->fs_rsp.read.more = TRUE;
      
      /* Must handle sequence number rollover */
      next_seq_num = (next_seq_num == 0xFF) ? 1 : next_seq_num + 1;
    }
    else
    {
      rsp->fs_rsp.read.more = FALSE;
    }
  
    if (rsp->fs_rsp.read.more == FALSE)
    {
      (void) fsdiag_cleanup();
      next_seq_num = 0;
    }
  }
/*lint -save -e717 */
} while(0);
/*lint -restore */

  diagpkt_shorten(rsp, rsp_len);

  return (rsp);
} /* fsdiag_read */
      
      

/*===========================================================================

FUNCTION FSDIAG_WRITE
DESCRIPTION
  This function handles FS write commands in DIAG.
  
============================================================================*/
static PACKED void *fsdiag_write (
  fsdiag_write_req_type *req,
  word pkt_len
)
{
  fs_open_xparms_type fs_open_xparms; /* Open and truncate parms          */
  fs_rsp_msg_type     fs_rsp;         /* Response from FS task            */
  fsdiag_data_block_type *data_ptr;  /* Pointer to data block in packet  */
  static byte next_seq_num = 0;       /* Expected next seqeunce number.   */
  word expected_pkt_len;              /* Used to calculate packet length  */
  fsdiag_rsp_pkt_type *rsp;
  dword rsp_len = FPOS(fsdiag_rsp_pkt_type, fs_rsp) + 
                 sizeof(fsdiag_write_rsp_type);
                                                                           
  /*--------------------------------------------------- 
   Keep progress stats for multiple packet operations 
  ---------------------------------------------------*/
  static dword written_so_far;
  static dword total_file_length;

  /*------------------------------
   Do all error checking first
  ------------------------------*/
  if (req->seq_num == 0) 
  {
    FS_DIAG_VALIDATE_ACCESS( WRITE,
       (const char *)req->block.begin.var_len_buf.name_info.name );

    /*--------------------------------
     Check for valid packet length.
    --------------------------------*/
    data_ptr = (fsdiag_data_block_type *) 
      &(req->block.begin.var_len_buf.raw_data[ 
        req->block.begin.var_len_buf.name_info.len + 
        sizeof (req->block.begin.var_len_buf.name_info.len)]);

    expected_pkt_len = 
      sizeof(req->seq_num)                               + 
      sizeof(req->more)                                  +
      sizeof(req->block.begin.mode)                      + 
      sizeof(req->block.begin.total_length)              +
      sizeof(req->block.begin.attrib)                    + 
      sizeof(req->block.begin.var_len_buf.name_info.len) +
      req->block.begin.var_len_buf.name_info.len         + 
      sizeof(data_ptr->len)                                  +
      data_ptr->len;

    if (pkt_len != expected_pkt_len) 
    {
      next_seq_num = 0;
      return( diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len) );
    }
  } /* Sequence number == 0 */
  else if (next_seq_num == req->seq_num)
  {
    /*----------------------------------------
      Assign data_ptr to request data block 
    ----------------------------------------*/
    data_ptr = &req->block.subseq;
      
    /*--------------------------------
     Check for valid packet length.
    --------------------------------*/
    expected_pkt_len = sizeof(req->seq_num)          +
                       sizeof(req->more)             +
                       sizeof(req->block.subseq.len) + 
                       req->block.subseq.len;

    if (pkt_len != expected_pkt_len) 
    {
      next_seq_num = 0;
      return( diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len) );
    }
  }
  else /* Bad sequence number */
  {
    (void) fsdiag_cleanup();
    next_seq_num = 0;
    
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req, pkt_len) );
  }
                                      
  rsp = (fsdiag_rsp_pkt_type *)diagpkt_alloc(DIAG_FS_OP_F, rsp_len);

  if(rsp == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_write.", 0, 0, 0);
    return(rsp);
  }

  rsp->fs_rsp.write.seq_num = req->seq_num;

  /*------------------------------------------------------------------
    For starters, check if this is to start a new file transfer, or 
    continue an old one.  Also, verify packet length.
  ------------------------------------------------------------------*/
  do
  {
    if (req->seq_num == 0) 
    {
      /*--------------------------------------------------- 
        Make sure the length of the data block is valid.
      ---------------------------------------------------*/
      if (data_ptr->len > FSDIAG_MAX_FILE_BLOCK_SIZE)
      {
        fs_rsp.any.status = FS_PARAMETER_ERROR_S;
        break;
      }
  
      /*------------------------------------------------------
        We are starting a new file.  Clean up any old files.
      ------------------------------------------------------*/
      (void) fsdiag_cleanup();
        
      total_file_length = 0;
      written_so_far = 0;
      next_seq_num = 0;
      
      /*----------------------------------------------
       Set default attributes for new file.
       External device may change attributes later.
      ----------------------------------------------*/                 
      fs_open_xparms.create.attribute_mask   = 
        (fs_attribute_mask_type) req->block.begin.attrib.attribute_mask;
  
      fs_open_xparms.create.cleanup_option   = 
        (fs_cleanup_option_type) req->block.begin.attrib.cleanup_option;
  
      fs_open_xparms.create.buffering_option = 
        (fs_buffering_option_type) req->block.begin.attrib.buffering_option;
  
      /*----------------------------
       Open file for writing.
      ----------------------------*/
      fs_open((const char *)req->block.begin.var_len_buf.name_info.name, 
              FS_OA_CREATE, 
              &fs_open_xparms, 
              NULL, 
              &fs_rsp);
     
      /*----------------------------------------------
       If overwrite option is specified, recover by 
       removing file and re-opening.
      ----------------------------------------------*/
      if (fs_rsp.open.status == FS_FILE_ALREADY_EXISTS_S &&
          req->block.begin.mode == FSDIAG_OVERWRITE) 
      {
        fs_remove((const char *)req->block.begin.var_len_buf.name_info.name, 
                  NULL, 
                  &fs_rsp);
  
        if (fs_rsp.rmfile.status == FS_OKAY_S)
        {
          /* Now try again */
          fs_open((const char *)req->block.begin.var_len_buf.name_info.name, 
                  FS_OA_CREATE,
                  &fs_open_xparms, 
                  NULL, 
                  &fs_rsp);
        }
      }
    
      if (fs_rsp.any.status != FS_OKAY_S) 
      {
        (void) fsdiag_cleanup();
        next_seq_num = 0;
        
        break;
      }
      
      /*-----------------------------------------------
       Save file name info for possible cleanup later
      -----------------------------------------------*/
      memcpy (fsdiag_filename, 
              (void *) req->block.begin.var_len_buf.name_info.name,
              req->block.begin.var_len_buf.name_info.len);
  
      /*--------------------------------------
        Save current operation for cleanup 
      --------------------------------------*/
      fsdiag_op     = FSDIAG_WRITE;
      fsdiag_handle = fs_rsp.open.handle;
      
      /*--------------------
        Record total length
      --------------------*/
      total_file_length = req->block.begin.total_length;
      written_so_far    = 0;
  
    } /* Sequence number == 0 */
    else if (next_seq_num == req->seq_num)
    {
      /*--------------------------------------------------- 
        Make sure the length of the data block is valid.
      ---------------------------------------------------*/
      if (data_ptr->len > FSDIAG_MAX_FILE_BLOCK_SIZE)
      {
        fs_rsp.any.status = FS_PARAMETER_ERROR_S;
        break;
      }
  
    }
    
    /*------------------------------------------
      Go ahead and write.
      Already did range checking on block size 
    ------------------------------------------*/
    written_so_far += data_ptr->len;
    if (written_so_far <= total_file_length)
    {
/*lint -save -e545 Suspicious use of & */
      fs_write(fsdiag_handle,  
               (PACKED void*)&(data_ptr->data),
               data_ptr->len,
               NULL,
               &fs_rsp);
/*lint -restore */
        
    }
    else /* Attempt to write more than specified length for file */
    {
      fs_rsp.any.status = FS_ILLEGAL_OPERATION_S;
    }
  
    if (fs_rsp.any.status != FS_OKAY_S) 
    {
      (void) fsdiag_cleanup();
      next_seq_num = 0;
      
      break;
    }
    else
    {
      /* Must handle sequence number rollover */
      next_seq_num = (next_seq_num == 0xFF) ? 1 : next_seq_num + 1;
    }
    
    if (req->more == FALSE)
    {
      /*-------------------
       Now close the file
      -------------------*/
      fs_close(fsdiag_handle, NULL, &fs_rsp);
          
      fsdiag_handle      = NULL;
      fsdiag_op          = FSDIAG_NONE;
      fsdiag_filename[0] = NULL;
      next_seq_num        = 0;
    }
/*lint -save -e717 */
  } while ( 0 );
/*lint -restore */
  
  rsp->fs_status = fs_rsp.any.status;

  return (rsp);
  
} /* fsdiag_write */



/*===========================================================================

FUNCTION FSDIAG_RMFILE
DESCRIPTION
  This function handles FS "Remove File" commands in DIAG.
  
============================================================================*/
static PACKED void *fsdiag_rmfile (
  fsdiag_rmfile_req_type *req,
  word pkt_len
)
{              
  fs_rsp_msg_type fs_rsp;  /* FS requires we specify a response message */
  fsdiag_rsp_pkt_type *rsp;
  const dword rsp_len = FPOS(fsdiag_rsp_pkt_type, fs_rsp);

  /*--------------------------------
   Check for valid packet length.
  --------------------------------*/
  if (pkt_len != req->len + sizeof(req->len)) 
  {  
    return( diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len) );
  }

  FS_DIAG_VALIDATE_ACCESS( DELETE, (const char *)req->name );
                                      
  rsp = (fsdiag_rsp_pkt_type *)diagpkt_alloc(DIAG_FS_OP_F, rsp_len);

  if(rsp)
  {
    (void) fsdiag_cleanup();

    fs_remove((const char *)req->name, /* Absolute dir name  */
              NULL,
              &fs_rsp);

    rsp->fs_status = fs_rsp.rmfile.status;
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_rmfile.", 0, 0, 0);
  }

  return (rsp);

} /* fsdiag_rmfile */
      


/*===========================================================================

FUNCTION FSDIAG_ITER
DESCRIPTION
  This function handles FS "Iterate Directories" commands in DIAG.

============================================================================*/
static PACKED void *fsdiag_iter (
  fsdiag_iter_dirs_req_type *req,
  uint16 pkt_len,
  fsdiag_op_enum_type op_type
)
{
  fs_rsp_msg_type       fs_rsp;
  static fs_enum_iterator_type fs_enum;
  static boolean previously_opened = 0;
  fs_enum_data_type     fs_enum_data;
  fsdiag_rsp_pkt_type *rsp;

  dword rsp_len = FPOS(fsdiag_rsp_pkt_type, fs_rsp);
  fs_enum_data.fullname_length = 0;
  /*--------------------------------
   Check for valid packet length.
  --------------------------------*/
  if (pkt_len != FPOS(fsdiag_iter_dirs_req_type, dir_name.name) + 
                 req->dir_name.len) 
  {
    return( diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len) );
  }

  do
  {
    /* If seq number is 0, do the initialization */
    if(req->seq_num == 0)
    {

      FS_DIAG_VALIDATE_ACCESS( ITERATE, (const char *)req->dir_name.name );

      /*----------------------
        Determine operation
      ----------------------*/
      if (op_type == FSDIAG_ITER_FILES)
      {
        fs_enum.enum_kind = FS_ENUM_FILES;
      }
      else
      {
        fs_enum.enum_kind = FS_ENUM_DIRECTORIES;
      }

      /* Close the previous iterator. */
      if (previously_opened)
      {
        fs_enum_finish (&fs_enum, NULL, &fs_rsp);
      }
      
      /*-----------------------
        Initialize enumerator.
      -----------------------*/
      fs_enum_init((const char *)req->dir_name.name,
                   fs_enum.enum_kind,
                   &fs_enum,
                   NULL,
                   &fs_rsp);

      previously_opened = TRUE;
      
      if (fs_rsp.enum_init.status != FS_OKAY_S)
      {
        rsp = (fsdiag_rsp_pkt_type *)diagpkt_alloc(DIAG_FS_OP_F, rsp_len);
        if(rsp == NULL)
        {
          MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_iter.", 0, 0, 0);
          return(rsp);     
        }
        break;
      }
    } /* if first time */
          
    /*---------------------------------------------------------------------
      Request the directory name corresponding to the given sequence number
    -----------------------------------------------------------------------*/
    fs_enum_next(&fs_enum_data,
                 &fs_enum,
                 NULL,
                 &fs_rsp);
    
    if (fs_rsp.enum_next.status == FS_OKAY_S)
    {
      rsp_len += FPOS(fsdiag_iter_rsp_type, item_name.name) + 
                 fs_enum_data.fullname_length;
    }

    rsp = (fsdiag_rsp_pkt_type *)diagpkt_alloc(DIAG_FS_OP_F, rsp_len);

    if(rsp == NULL)
    {
      MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_iter.", 0, 0, 0);
      return(rsp);     
    }

    if (fs_rsp.enum_next.status == FS_OKAY_S)
    {
      rsp->fs_rsp.iter.seq_num = req->seq_num;
      
      /* Attributes */
      rsp->fs_rsp.iter.attrib.attribute_mask   = fs_enum_data.attributes;
      rsp->fs_rsp.iter.attrib.buffering_option = fs_enum_data.buffering_option;
      rsp->fs_rsp.iter.attrib.cleanup_option   = fs_enum_data.cleanup_option;
  
      /* File info */
      rsp->fs_rsp.iter.creation_date = fs_enum_data.creation_date;
      rsp->fs_rsp.iter.logical_size  = fs_enum_data.logical_size;
      rsp->fs_rsp.iter.physical_size = fs_enum_data.physical_size;
  
      /* Name info */
      rsp->fs_rsp.iter.dirname_length = fs_enum_data.dirname_length;
      rsp->fs_rsp.iter.item_name.len  = fs_enum_data.fullname_length;
      memcpy((void*) rsp->fs_rsp.iter.item_name.name, 
             fs_enum_data.fullname, 
             fs_enum_data.fullname_length);
    }
/*lint -save -e717 */
  }while( 0 );
/*lint -restore */

  rsp->fs_status = fs_rsp.any.status;


  return (rsp);
} /* fsdiag_iter */



/*===========================================================================

FUNCTION FSDIAG_SPACE_AVAIL
DESCRIPTION
  This function handles FS "Space Available" commands in DIAG.
  
============================================================================*/
static PACKED void *fsdiag_space_avail (void)
{
  fsdiag_rsp_pkt_type *rsp;
  const dword rsp_len = FPOS(fsdiag_rsp_pkt_type, fs_rsp) + 
                       sizeof(fsdiag_space_avail_rsp_type);

  rsp = (fsdiag_rsp_pkt_type *)diagpkt_alloc(DIAG_FS_OP_F, rsp_len);

  if(rsp)
  {
    rsp->fs_rsp.space_avail = fs_space_available();

    rsp->fs_status = FS_OKAY_S;
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_space_avail.", 0, 0, 0);
  }
  return (rsp);
} /* fsdiag_space_avail */



/*===========================================================================

FUNCTION FSDIAG_FORMAT
DESCRIPTION
  This function handles FS "Format Card" commands in DIAG.
  
============================================================================*/
static PACKED void * fsdiag_format (
  fsdiag_format_req_type *req,
  word pkt_len
)
{              
  fs_rsp_msg_type fs_rsp;  /* FS requires we specify a response message */
  fsdiag_rsp_pkt_type *rsp;
  const dword rsp_len = FPOS(fsdiag_rsp_pkt_type, fs_rsp);

  /*--------------------------------
   Check for valid packet length.
  --------------------------------*/
  if (pkt_len != req->len + sizeof(req->len)) 
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req, pkt_len));
  }

  FS_DIAG_VALIDATE_ACCESS( FORMAT, (const char* )req->name );

  rsp = (fsdiag_rsp_pkt_type *)diagpkt_alloc(DIAG_FS_OP_F, rsp_len);

  if(rsp)
  {
    /*--------------------------------
     Check for valid filename length.
    --------------------------------*/
    fs_format( (const char*)req->name, NULL, &fs_rsp);

    rsp->fs_status = fs_rsp.format.status;
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in fsdiag_format.", 0, 0, 0);
  }

  return (rsp);

} /* fsdiag_format */
      
      

/*===========================================================================

FUNCTION FSDIAG_CMD
DESCRIPTION
  This procedure processes a request to perform an Embedded File System
  (EFS) operation.  It is a variable length command to save on bandwidth, 
  so the buffer to write into should only copy the needed bytes.  This
  procedure will call the appropriate procedure in diag_fs.c to handle
  each operation.

============================================================================*/
PACKED void * fsdiag_cmd (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  fsdiag_req_pkt_type *req = (fsdiag_req_pkt_type *)req_pkt;
  fsdiag_rsp_pkt_type *rsp = NULL;
  /* # of bytes being stripped off of generic packet (cmd_code, etc) */
  uint16 rsp_len = (uint16)DIAG_FS_REQ_LEN_DIFF;


  /*-------------------------------------------------------------------------
    Check security, since this is a secure funciton
  --------------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len) );
  }

  /*-------------------------------------------------------------------------
   Determine requested file operation and call appropriate DIAG FS function
  -------------------------------------------------------------------------*/
  switch (req->file_op) {
    case FSDIAG_MK_DIR:
      rsp = (fsdiag_rsp_pkt_type *)fsdiag_mkdir(&req->fs_req.mkdir,
                                 pkt_len - rsp_len);
      break;

    case FSDIAG_RM_DIR:
      rsp = (fsdiag_rsp_pkt_type *)fsdiag_rmdir(&req->fs_req.rmdir,
                                 pkt_len - rsp_len);
      break;
   
    case FSDIAG_DISP_DIRS:
      rsp = (fsdiag_rsp_pkt_type *)fsdiag_disp_dirs(&req->fs_req.disp_dirs,
                                     pkt_len - rsp_len);
      break;

    case FSDIAG_DISP_FILES:
      rsp = (fsdiag_rsp_pkt_type *)fsdiag_disp_files(&req->fs_req.disp_files,
                                      pkt_len - rsp_len);
      break;

    case FSDIAG_READ_FILE:
      rsp = (fsdiag_rsp_pkt_type *)fsdiag_read(&req->fs_req.read,
                                pkt_len - rsp_len);
      break;

    case FSDIAG_WRITE_FILE:
      rsp = (fsdiag_rsp_pkt_type *)fsdiag_write(&req->fs_req.write,
                                 pkt_len - rsp_len);
      break;
  
    case FSDIAG_REMOVE_FILE:
      rsp = (fsdiag_rsp_pkt_type *)fsdiag_rmfile(&req->fs_req.rmfile,
                                  pkt_len - rsp_len);
      break;
  
    case FSDIAG_GET_ATTRIB:
      rsp = (fsdiag_rsp_pkt_type *)fsdiag_get_attrib(&req->fs_req.get_attrib,
                                      pkt_len - rsp_len);
  
      break;
  
    case FSDIAG_SET_ATTRIB:
      rsp = (fsdiag_rsp_pkt_type *)fsdiag_set_attrib(&req->fs_req.set_attrib,
                                      pkt_len - rsp_len);
  
      break;
  
    case FSDIAG_ITER_DIRS:
    case FSDIAG_ITER_FILES:
      rsp = (fsdiag_rsp_pkt_type *)fsdiag_iter(&req->fs_req.iter_dirs,
                                pkt_len - rsp_len,
                                (fsdiag_op_enum_type) req->file_op);
      break;
    
    case FSDIAG_SPACE_AVAIL:
      rsp = (fsdiag_rsp_pkt_type *)fsdiag_space_avail();
      break;
  
    case FSDIAG_FORMAT:
      rsp = (fsdiag_rsp_pkt_type *)fsdiag_format(&req->fs_req.format,
                                      pkt_len - rsp_len);
      break;

    default:
      return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }

  if(rsp)
  {
    /* Copy the file operation into the response */
    rsp->file_op = req->file_op;
  }
  else
  {
    MSG_HIGH ("Response pointer returned to fsdiag_cmd is NULL.", 0, 0, 0);    
  }
  return (rsp);

} /* fsdiag_cmd */

/* Restore error reporting for these symbols */
/*lint +esym(715,pkt_len,req) */

static const diagpkt_user_table_entry_type fsdiag_tbl[] =
{
  {DIAG_FS_OP_F, DIAG_FS_OP_F, fsdiag_cmd}
};

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, fsdiag_tbl); 

#else

  void fsdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, fsdiag_tbl);
  }
  
#endif

#ifdef __cplusplus
  }
#endif
