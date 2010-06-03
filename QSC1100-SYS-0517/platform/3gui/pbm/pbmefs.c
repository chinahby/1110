/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER EFS 

GENERAL DESCRIPTION
  This file contains the functions used to manage phonebook entries within the 
  EFS.

  Copyright (c) 2002 - 2007 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/services/pbm/pbmefs.c#30 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/08   clm     Do not free write cmd buffer if file write fails.  This 
                   will be freed when the file is closed.
01/16/07   cvs     Add event for phonebooks becoming ready
10/10/06   cvs     Move messages to PBM's diag msg ID
05/03/06   cvs     Lint cleanup.
04/12/06   cvs     better logging
03/24/06   cvs     %x -> 0x%x in log messages
02/23/06   cvs     rework memory allocation macros
01/09/06   cvs     Lint fixes
11/15/05   cvs     Lint fixes
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/05/05   cvs     Revert previous change
08/05/05   cvs     Finish initing EFS PBs
07/29/05   AT      Fixed race condition between PBM and FS.  When doing
                   a deletion, the fs_close_cb would free the write command
                   buffer.  We then would check a variable in this buffer,
                   this causes big problems when PBM is getting deletions and 
                   writes quicker than the PBM task gets the processor.
07/27/05   AT      With increased maximum size, dynamically allocating EFS 
                   buffer.
07/19/05   AT      Added a range change.
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
05/10/04   cvs     Add missing critical section in pbm_efs_garbage_recollect()
04/14/05   cvs     Fix bug where PB cache wouldn't go back to ready 
                   after garbage collection
04/07/05   cvs     Better error handling if UID field is wrong size at init
03/28/05   RSB,cvs Added Phone Unique ID functionality
03/08/05   PA      Support for PBM notifications.
01/20/05   PA      Support for read lock and reentrant complex field 
                   enumeration.
01/12/05   PA      PBM USIM Support
01/10/05   PA      Added more PBM kick dogs.
12/08/04   PA      Kick dog before efs_init to be safe
11/19/04   PA      Start unique id at 1.
11/18/04   PA      Support for complex fields and garbage collection with 
                   write lock.
09/29/04   PA      Initial Revision
===========================================================================*/
#include "customer.h"
#ifdef FEATURE_PBM_TASK

#include "pbm.h"
#include "pbmefs.h"
#include "pbmutils.h"
#include "pbmtask.h"

/*===========================================================================
    Declare static globals for EFS
===========================================================================*/
/* Buffer to handle async EFS writes */
static pbm_async_efs_buffer pbm_efs_buffers[PBM_ASYNC_BUFFERS];

/* Assuming that fs_write callbacks will be in the same order as fs_writes */
static uint8 pbm_efs_buf_first = 0;
static uint8 pbm_efs_buf_last = 0;

/* Boolean to differentiate between empty and full condition,
 * pbm_async_efs_buffer is much bigger than unsigned char */
static boolean pbm_efs_buf_empty = TRUE;

/* Flag to indicate whether the phone unique ID file got reset/corrupted. */
static boolean g_ph_uid_is_reset = FALSE;

/* This is the "phone unique ID", intended to remain unique for the life of the record */
static uint64 g_ph_unique_id = 0;

/* This unique ID is for tracking purposes only, and is subject to change. */
static uint16 g_unique_id = 1;

/* The master_filename is the current version of master file. */
static char *g_master_filename =     "pbm_master_v_20.dat";
static char *g_delete_filename =     "pbm_delete_file.dat";

/* A "new" master file is created from existing master file and delete file. */
static char *g_new_master_filename = "new_master_file.dat";

/* These file are backed up in the case we find an error in the master file. */
static char *g_crpt_master_filename ="cor_master_file.dat";

/* Outdated master file names. */
static char *g_v1_master_filename =  "pbm_master_file.dat";
static char *g_old_master_filename = "old_master_file.dat";

/* Phone unique ID file name */
static char *g_ph_uid_filename =     "pbm_phone_uid.dat";

static pbm_return_type pbm_efs_cache_init(boolean powerup);

static pbm_return_type pbm_convert_old_files(void);
static pbm_return_type pbm_efs_garbage_collect(boolean);
static pbm_return_type pbm_create_initial_ph_uid_file(uint64 starting_value, boolean uid_is_reset);
static pbm_return_type pbm_init_phone_unique_id(void);
static pbm_return_type pbm_renumber_with_unique_id(void);

/* <EJECT> */
/*==========================================================================*
 *                      PBM EFS UTILITIES                                   *
 *==========================================================================*/

/*===========================================================================
FUNCTION PBM_EFS_INIT

DESCRIPTION
  Initialize EFS based phonebooks.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_efs_init()
{
  pbm_return_type ret_val;

  pbm_kick_dog();

  ret_val = pbm_convert_old_files();
  if (ret_val != PBM_SUCCESS)
    return ret_val;

  ret_val = pbm_efs_garbage_collect(TRUE);
  if (ret_val != PBM_SUCCESS)
    return ret_val;  

  ret_val = pbm_efs_cache_init(TRUE);
  if (ret_val != PBM_SUCCESS)
    return ret_val;

  ret_val = pbm_init_phone_unique_id();
  if (ret_val != PBM_SUCCESS)
    return ret_val;

  /* Do not set any cache that uses master phonebook as ready
   * until after initializing the PH UID. */

  PBM_SET_PB_CACHE_READY(&efs_cache);
  PBM_SET_PB_CACHE_READY(&sdl_cache);
  PBM_SET_PB_CACHE_READY(&incoming_calls_cache);
  PBM_SET_PB_CACHE_READY(&outgoing_calls_cache);
  PBM_SET_PB_CACHE_READY(&missed_calls_cache);

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_EFS_SET_UNIQUE_ID

DESCRIPTION
Sets the Unique ID for a record stored in EFS.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static void pbm_efs_set_unique_id(uint16 unique_id)
{
  g_unique_id = unique_id;
  return;
}

/*===========================================================================
FUNCTION PBM_CLEANUP_CORRUPT_FILES

DESCRIPTION
  This function should *never* be called.  However, we have a lot of code
  that checks for corruption in the master file.  There should be no way
  for this to happen, but if it ever does, this function will allow someone
  to salvage the phone book, and allow the user to continue to use it.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static void pbm_cleanup_corrupt_files(char *filename)
{
  fs_rsp_msg_type fs_rsp_msg;

  pbm_kick_dog();
  /* Remove Old backup of a corrupt master file . */
  fs_remove(g_crpt_master_filename, NULL, &fs_rsp_msg);
  if ((fs_rsp_msg.rmfile.status != FS_OKAY_S) &&
      (fs_rsp_msg.rmfile.status != FS_NONEXISTENT_FILE_S))
  {
    PBM_MSG_ERR("Error deleting old Delete File", 0, 0, 0);
  }


  fs_rename(filename, g_crpt_master_filename, NULL, &fs_rsp_msg);
  if (fs_rsp_msg.rename.status != FS_OKAY_S)
  {
    PBM_MSG_ERR("Error renaming %s to %s", filename, g_crpt_master_filename, 0);
  }

  /* Remove Delete File*/
  fs_remove(g_delete_filename, NULL, &fs_rsp_msg);
  if ((fs_rsp_msg.rmfile.status != FS_OKAY_S) &&
      (fs_rsp_msg.rmfile.status != FS_NONEXISTENT_FILE_S))
  {
    PBM_MSG_ERR("Error deleting Delete File", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION PBM_EFS_READ_V1

DESCRIPTION
  This function gets the next (non-deleted) address record in the
  Master File.  If no record has yet been read, reads the first record.
  If at EOF, returns PBM_EOF. The calling function must allocate return_rec,
  but pbm_efs_read() mallocs fields and data that are pointed to by
  return_rec.(Other error conditions????)  This can be designed as a
  sequential read, since it is only used at startup, and no random-access
  reads are required.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_efs_read_v1(fs_handle_type file_handle, pbm_addr_cache_s_type *return_rec)
{
  char *buffer;
  fs_rsp_msg_type  read_rsp_msg, close_rsp_msg;
  uint16 rec_len;
  uint16 data_len;
  uint16 offset = 0;
  uint16 i, j;
  uint32 *data_sizes = NULL;
  boolean ret_val;

  PBM_CHECK_PTR_RET(return_rec, PBM_ERROR);

  PBM_ALLOC_MEM_W_ERR_RET(buffer, PBM_MAX_EFS_REC_LEN, NO_ACTION);

  /* Start by reading the record length*/
  /* Note: record length provided is the *total*, including the 2 bytes for*/
  /*       the length info. itself*/
  pbm_kick_dog();
  fs_read(file_handle, buffer, sizeof(rec_len), NULL, &read_rsp_msg);

  if ( (read_rsp_msg.read.count < sizeof(rec_len)) &&
       (read_rsp_msg.read.status == FS_OKAY_S) )
  {
    /* Assume that this is because EOF was reached*/
    read_rsp_msg.read.status = FS_EOF_S;
  }

  if ( (read_rsp_msg.read.status != FS_OKAY_S) ||
       (read_rsp_msg.read.count != sizeof(rec_len)) )
  {
    fs_close(file_handle, NULL, &close_rsp_msg);
    if (close_rsp_msg.close.status != FS_OKAY_S)
    {
      PBM_MSG_ERR("Error closing Master File", 0, 0, 0);
      PBM_MEM_FREEIF(buffer);
      return PBM_ERROR;
    }
    if (read_rsp_msg.read.status == FS_EOF_S)
    {
      /* If EOF, close master file and return PBM_EOF.*/
      PBM_MEM_FREEIF(buffer);
      return PBM_EOF;
    }
    PBM_MEM_FREEIF(buffer);
    PBM_MSG_ERR("Error reading Master File", 0, 0, 0);
    return PBM_ERROR;
  }

  memcpy(&rec_len, buffer, sizeof(rec_len));

  if (rec_len > PBM_MAX_EFS_REC_LEN)
  {
    fs_close(file_handle, NULL, &close_rsp_msg);
    if (close_rsp_msg.close.status != FS_OKAY_S)
      PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
    PBM_MSG_ERR("Record is longer than max. allowed record length.", 0, 0, 0);
    PBM_MEM_FREEIF(buffer);
    return PBM_ERROR;
  }

  /* Record Length is OK.  Now read the rest of the record*/
  fs_read(file_handle, buffer, rec_len - sizeof(rec_len), NULL, &read_rsp_msg);

  if ( (read_rsp_msg.read.count < (rec_len - sizeof(rec_len))) &&
       (read_rsp_msg.read.status == FS_OKAY_S) )
  {
    /* Assume that this is because EOF was reached*/
    read_rsp_msg.read.status = FS_EOF_S;
  }

  if ( (read_rsp_msg.read.status != FS_OKAY_S) ||
       (read_rsp_msg.read.count != (rec_len - sizeof(rec_len))) )
  {
    fs_close(file_handle, NULL, &close_rsp_msg);
    if (close_rsp_msg.close.status != FS_OKAY_S)
      PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
    PBM_MSG_ERR("Error reading Master File", 0, 0, 0);
    PBM_MEM_FREEIF(buffer);
    return PBM_ERROR;
  }

  offset = 0;

  memcpy(&(return_rec->unique_id), buffer + offset, sizeof(return_rec->unique_id));
  offset += sizeof(return_rec->unique_id);

  memcpy(&(return_rec->record_id), buffer + offset, sizeof(return_rec->record_id));
  offset += sizeof(return_rec->record_id);

  memcpy(&(return_rec->category),  buffer + offset, sizeof(return_rec->category));
  offset += sizeof(return_rec->category);

  /* Count the number of fields in the record*/
  for (i = 0; offset < (rec_len - sizeof(rec_len)); i++)
  {
    /* Advance over field ID and field type*/
    offset += sizeof(return_rec->fields[0].field_id) +
              sizeof(return_rec->fields[0].field_type);
    memcpy(&data_len, buffer + offset, sizeof(data_len));
    offset += sizeof(data_len);
    offset += data_len;  /* Advance over the data*/
  }

  if (offset != (rec_len - sizeof(rec_len)))
  {
    /* File is corrupt!*/
    fs_close(file_handle, NULL, &close_rsp_msg);
    if (close_rsp_msg.close.status != FS_OKAY_S)
      PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
    PBM_MSG_ERR("Master File is corrupt.", 0, 0, 0);
    PBM_MEM_FREEIF(buffer);
    return PBM_ERROR;
  }

  /* Now i is the number of fields in the record*/
  return_rec->num_fields = i;
  PBM_ALLOC_MEM_W_ERR_RET(data_sizes, i * sizeof(int), PBM_MEM_FREEIF(buffer););
  if (data_sizes != NULL)
  {

    /* Set offset to beginning of fields*/
    offset = sizeof(return_rec->unique_id) +
             sizeof(return_rec->record_id) +
             sizeof(return_rec->category);

    /* Determine the field data sizes and copy the data in.*/
    for (j = 0; j < return_rec->num_fields; j++)
    {
      uint16 temp_size;

      offset += sizeof(return_rec->fields[0].field_id) +
                sizeof(return_rec->fields[0].field_type);

      memcpy( &(temp_size), buffer + offset,
              sizeof(return_rec->fields[0].data_len) );
      data_sizes[j] = (int)temp_size;
      offset += sizeof(return_rec->fields[0].data_len);

      offset += (uint16) data_sizes[j];
    }

    ret_val = pbm_allocate_fields_internal( &(return_rec->fields),
                                   (uint32) return_rec->num_fields,
                                   data_sizes );
    PBM_MEM_FREEIF(data_sizes);
    if (!ret_val)
    {
      PBM_MEM_FREEIF(buffer);
      return PBM_ERROR;  /* Out of memory*/
    }

    /* Reset offset to beginning of fields*/
    offset = sizeof(return_rec->unique_id) +
             sizeof(return_rec->record_id) +
             sizeof(return_rec->category);

    /* Copy in the field info.*/
    for (j = 0; j < return_rec->num_fields; j++)
    {
      void *field_ptr;

      memcpy( &(return_rec->fields[j].field_id), buffer + offset,
              sizeof(return_rec->fields[0].field_id));

      offset += sizeof(return_rec->fields[0].field_id);

      memcpy( &(return_rec->fields[j].field_type), buffer + offset,
              sizeof(return_rec->fields[0].field_type) );

      offset += sizeof(return_rec->fields[0].field_type);

      /* Fix an issue where we could have written garbage to the
       * upper 16 bytes of the field_type.  Safely assume high
       * 16 bits of field_type should have been 0's when file was
       * written.
       */
      /*lint -e{655} # bit-wise operation uses (compatible) enum's */
      return_rec->fields[j].field_type &= (pbm_field_type_e_type)0x0000FFFF;

      memcpy( &(return_rec->fields[j].data_len), buffer + offset,
              sizeof(return_rec->fields[0].data_len) );
      offset += sizeof(return_rec->fields[0].data_len);

      field_ptr = pbm_get_field_data_ptr(return_rec->fields,j); 
      if (field_ptr == NULL)
      {
        PBM_MSG_ERR("field_ptr NULL 0x%x %d",return_rec->fields,j,0);

        PBM_MEM_FREEIF(buffer);
        return PBM_ERROR;
      }
      memcpy( field_ptr,
              buffer + offset,
              return_rec->fields[j].data_len );
      offset += return_rec->fields[j].data_len;
    }
  }

  PBM_MEM_FREEIF(buffer);
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_EFS_READ

DESCRIPTION
  This function gets the next (non-deleted) address record in the
  Master File.  If no record has yet been read, reads the first record.
  If at EOF, returns PBM_EOF. The calling function must allocate return_rec,
  but pbm_efs_read() mallocs fields and data that are pointed to by
  return_rec.(Other error conditions????)  This can be designed as a
  sequential read, since it is only used at startup, and no random-access
  reads are required.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_efs_read(fs_handle_type file_handle, pbm_addr_cache_s_type *return_rec)
{
  char *buffer;
  fs_rsp_msg_type read_rsp_msg;
  uint16 rec_len;
  uint16 data_len;
  uint16 offset = 0;
  uint16 i, j;
  uint32 *data_sizes = NULL;
  boolean ret_val;

  PBM_CHECK_PTR_RET(return_rec, PBM_ERROR);

  PBM_ALLOC_MEM_W_ERR_RET(buffer, PBM_MAX_EFS_REC_LEN, NO_ACTION);

  /* Start by reading the record length*/
  /* Note: record length provided is the *total*, including the 2 bytes for*/
  /*       the length info. itself*/
  pbm_kick_dog();
  fs_read(file_handle, buffer, sizeof(rec_len), NULL, &read_rsp_msg);

  if ( (read_rsp_msg.read.count < sizeof(rec_len)) &&
       (read_rsp_msg.read.status == FS_OKAY_S) )
  {
    /* Assume that this is because EOF was reached*/
    read_rsp_msg.read.status = FS_EOF_S;
  }

  if ( (read_rsp_msg.read.status != FS_OKAY_S) ||
       (read_rsp_msg.read.count != sizeof(rec_len)) )
  {
    if (read_rsp_msg.read.status == FS_EOF_S)
    {
      /* If EOF, close master file and return PBM_EOF.*/
      PBM_MEM_FREEIF(buffer);
      return PBM_EOF;
    }
    PBM_MSG_ERR("Error reading Master File", 0, 0, 0);
    PBM_MEM_FREEIF(buffer);
    return PBM_ERROR;
  }

  memcpy(&rec_len, buffer, sizeof(rec_len));

  if (rec_len > PBM_MAX_EFS_REC_LEN)
  {
    PBM_MSG_ERR("Record is longer than max. allowed record length.", 0, 0, 0);
    PBM_MEM_FREEIF(buffer);
    return PBM_ERROR;
  }

  /* Record Length is OK.  Now read the rest of the record*/
  fs_read(file_handle, buffer, rec_len - sizeof(rec_len), NULL, &read_rsp_msg);

  if ( (read_rsp_msg.read.count < (rec_len - sizeof(rec_len))) &&
       (read_rsp_msg.read.status == FS_OKAY_S) )
  {
    /* Assume that this is because EOF was reached*/
    read_rsp_msg.read.status = FS_EOF_S;
  }

  if ( (read_rsp_msg.read.status != FS_OKAY_S) ||
       (read_rsp_msg.read.count != (rec_len - sizeof(rec_len))) )
  {
    PBM_MSG_ERR("Error reading Master File", 0, 0, 0);
    PBM_MEM_FREEIF(buffer);
    return PBM_ERROR;
  }

  memcpy(&(return_rec->unique_id), buffer, sizeof(return_rec->unique_id));
  offset = sizeof(return_rec->unique_id);

  memcpy(&(return_rec->record_id), buffer + offset, sizeof(return_rec->record_id));
  offset += sizeof(return_rec->record_id);

  memcpy(&(return_rec->category), buffer + offset, sizeof(return_rec->category));
  offset += sizeof(return_rec->category);

  /* Count the number of fields in the record*/
  for (i = 0; offset < (rec_len - sizeof(rec_len)); i++)
  {
    /* Advance over field ID and field type*/
    offset += sizeof(return_rec->fields[0].field_id);
    offset += sizeof(uint32); /* return_rec->fields[0].field_type */

    /* Copy the data length. */
    memcpy(&data_len, buffer + offset, sizeof(data_len));
    offset += sizeof(data_len);

    offset += data_len;  /* Advance over the data*/
  }

  if (offset != (rec_len - sizeof(rec_len)))
  {
    /* File is corrupt!*/
    PBM_MSG_ERR("Master File is corrupt.", 0, 0, 0);
    PBM_MEM_FREEIF(buffer);
    return PBM_ERROR;
  }

  /* Now i is the number of fields in the record*/
  return_rec->num_fields = i;
  PBM_ALLOC_MEM_W_ERR_RET(data_sizes, i * sizeof(int),
                          PBM_MEM_FREEIF(buffer));
  if (data_sizes != NULL)
  {
    /* Set offset to beginning of fields*/
    offset = sizeof(return_rec->unique_id) +
             sizeof(return_rec->record_id) +
             sizeof(return_rec->category);

    /* Determine the field data sizes and copy the data in.*/
    for (j = 0; j < return_rec->num_fields; j++)
    {
      uint16 temp_size;

      offset += sizeof(return_rec->fields[0].field_id) +
                sizeof(uint32); /* return_rec->fields[0].field_type */

      memcpy( &(temp_size), buffer + offset,
              sizeof(return_rec->fields[0].data_len) );

      data_sizes[j] = (int)temp_size;
      offset += sizeof(return_rec->fields[0].data_len);

      offset += (uint16) data_sizes[j];
    }

    ret_val = pbm_allocate_fields_internal( &(return_rec->fields),
                                   (uint32) return_rec->num_fields,
                                   data_sizes );

    PBM_MEM_FREEIF(data_sizes);
    if (!ret_val)
    {
      PBM_MEM_FREEIF(buffer);
      return PBM_ERROR;  /* Out of memory*/
    }

    /* Reset offset to beginning of fields*/
    offset = sizeof(return_rec->unique_id) +
             sizeof(return_rec->record_id) +
             sizeof(return_rec->category);

    /* Copy in the field info.*/
    for (j = 0; j < return_rec->num_fields; j++)
    {
      uint32 temp_field_type;
      void *field_ptr;

      if (return_rec->fields == NULL)
      {
        PBM_MSG_ERR("return_rec->fields NULL",0,0,0);

        PBM_MEM_FREEIF(buffer);
        return PBM_ERROR;
      }

      memcpy( &(return_rec->fields[j].field_id), buffer + offset,
              sizeof(return_rec->fields[0].field_id));
      offset += sizeof(return_rec->fields[0].field_id);

      memcpy( &temp_field_type, buffer + offset, sizeof(uint32));
      return_rec->fields[j].field_type = (pbm_field_type_e_type)temp_field_type;
      offset += sizeof(uint32);

      memcpy( &(return_rec->fields[j].data_len), buffer + offset,
              sizeof(return_rec->fields[0].data_len) );
      offset += sizeof(return_rec->fields[0].data_len);

      field_ptr = pbm_get_field_data_ptr(return_rec->fields,j);
      if (field_ptr == NULL)
      {
        PBM_MSG_ERR("field_ptr NULL 0x%x %d",return_rec->fields,j,0);

        PBM_MEM_FREEIF(buffer);
        return PBM_ERROR;
      }
      memcpy( field_ptr,
              buffer + offset,
              return_rec->fields[j].data_len );
      offset += return_rec->fields[j].data_len;
    }
  }

  PBM_MEM_FREEIF(buffer);
  return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_EFS_GENERATE_UNIQUE_ID

DESCRIPTION
Generates a Unique ID for a record stored in EFS.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
uint16 pbm_efs_generate_unique_id(void)
{
  return (g_unique_id++);
}

/*===========================================================================
FUNCTION PBM_EFS_WRITE_CB

DESCRIPTION
  Asynchronous callback after PBM EFS write.

  In case of error, we call the client callback. In case of success we wait
  for the result of close operation.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
/*lint -e{818} suppress param could be const */
static void pbm_efs_write_cb(fs_rsp_msg_type *msg)
{
  pbm_async_efs_buffer *buf = &pbm_efs_buffers[pbm_efs_buf_first];

  PBM_CHECK_PTR_RET(msg, VOID);

  /* Response from FS with the results of the write. */
  /* The info we need is referenced by pbm_efs_buf_first. */
  if (msg->write.status != FS_OKAY_S ||
      msg->write.count != buf->count)
  {
    PBM_MSG_ERR("Error writing to Master File %d %d %d", 
                msg->write.status, msg->write.count, buf->count);

    pbm_list_recover(buf->write, buf->deleted);
    pbm_write_notify(buf->write, PBM_ERROR);

    buf->cb_called = TRUE;
  }
}

/*===========================================================================
FUNCTION PBM_EFS_CLOSE_CB

DESCRIPTION
  Asynchronous callback after PBM EFS close.  We depend on pbm_efs_buf_first
  pointing to the information about this callback. 

DEPENDENCIES
  We assume that fs sends callbacks in the same order in which the operations
  were performed.

SIDE EFFECTS
===========================================================================*/
/*lint -e{818} suppress param could be const */
static void pbm_efs_close_cb(fs_rsp_msg_type *msg)
{
  pbm_async_efs_buffer *buf = &pbm_efs_buffers[pbm_efs_buf_first];

  PBM_CHECK_PTR_RET(msg, VOID);

  if (!buf->cb_called) 
  {
    if (msg->close.status != FS_OKAY_S) 
    {
      PBM_MSG_ERR("Error closing Master File", 0, 0, 0);

      pbm_list_recover(buf->write, buf->deleted);
      pbm_write_notify(buf->write, PBM_ERROR);

    } 
    else 
    {
      pbm_list_update(buf->write);
      pbm_write_notify(buf->write, PBM_SUCCESS);
    }
  }

  pbm_write_cmd_free(buf->write);

  /* After the close, we are done with this callback, and can get
   * the next one. */
  TASKLOCK();
  pbm_efs_buf_first = (pbm_efs_buf_first+1)%PBM_ASYNC_BUFFERS;
  pbm_efs_buf_empty = (pbm_efs_buf_first == pbm_efs_buf_last);
  TASKFREE();
}

/*===========================================================================
FUNCTION PBM_EFS_ADD

DESCRIPTION
  Adds a record to the end of the Master File.

DEPENDENCIES
  Must be called from within a critical section.

SIDE EFFECTS
===========================================================================*/
static fs_handle_type pbm_efs_add_open(const char *filename)
{
  fs_rsp_msg_type open_rsp_msg;

  PBM_CHECK_PTR_RET(filename, FS_NULL_HANDLE);

  /* Open Master File for append*/
  pbm_kick_dog();
  fs_open(filename, FS_OA_APPEND, NULL, NULL, &open_rsp_msg);
  if (open_rsp_msg.open.status != FS_OKAY_S)
  {
    if (open_rsp_msg.open.status == FS_NONEXISTENT_FILE_S)
    {
      /* If file doesn't already exist, open for create*/
      fs_open_xparms_type attribs;
      attribs.create.cleanup_option = FS_OC_REVERT;
      attribs.create.buffering_option = FS_OB_PROHIBIT;
      attribs.create.attribute_mask = FS_FA_UNRESTRICTED;
      fs_open(filename, FS_OA_CREATE, &attribs, NULL, &open_rsp_msg);
      if (open_rsp_msg.open.status != FS_OKAY_S)
      {
        /* Still doesn't work*/
        PBM_MSG_ERR("Error creating Master File", 0, 0, 0);
        return FS_NULL_HANDLE;
      }
    }
    else
    {
      PBM_MSG_ERR("Error opening Master File", 0, 0, 0);
      return FS_NULL_HANDLE;
    }
  }
  return open_rsp_msg.open.handle;
}

/*===========================================================================
FUNCTION PBM_EFS_ADD

DESCRIPTION
  Adds a record to the end of the Master File.

DEPENDENCIES
  Must be called from within a critical section.

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_efs_add(fs_handle_type file_handle, const pbm_addr_cache_s_type *new_record, boolean async)
{
  char *buffer = pbm_efs_buffers[pbm_efs_buf_last].buffer;
  fs_rsp_msg_type write_rsp_msg, close_rsp_msg;
  uint16 rec_len;
  uint16 count = 0, offset = 0;
  boolean opened = FALSE;
  int i;

  PBM_CHECK_PTR_RET(new_record, PBM_ERROR);

  if (file_handle == FS_NULL_HANDLE)
  {
    file_handle = pbm_efs_add_open(g_master_filename);
    if (file_handle == FS_NULL_HANDLE)
      return PBM_ERROR;

    opened = TRUE;
  }

  /* Initialize the memory so there isn't any garbage in the file. */
  memset(buffer, 0, sizeof(pbm_efs_buffers[0].buffer));

  /* Calculate the record length*/
  count = sizeof(rec_len) +
          sizeof(new_record->unique_id) +
          sizeof(new_record->record_id) +
          sizeof(new_record->category);

  for (i = 0; i < new_record->num_fields; i++)
  {
    count+= sizeof(new_record->fields[i].field_id) +
            sizeof(uint32) + /* new_record->fields[i].field_type */
            sizeof(new_record->fields[i].data_len) +
            new_record->fields[i].data_len;
  }

  rec_len = count;

  if (rec_len > PBM_MAX_EFS_REC_LEN)
  {
    if (opened)
    {
      fs_close(file_handle, NULL, &close_rsp_msg);
      if (close_rsp_msg.close.status != FS_OKAY_S)
        PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
    }
    PBM_MSG_ERR("Record len %d is longer than max. allowed record length.", rec_len, 0, 0);
    return PBM_ERROR;
  }

  /* Prepare the buffer*/
  memcpy(buffer, &rec_len, sizeof(rec_len));
  offset += sizeof(rec_len);

  memcpy(buffer + offset, &(new_record->unique_id), sizeof(new_record->unique_id));
  offset += sizeof(new_record->unique_id);

  memcpy(buffer + offset, &(new_record->record_id), sizeof(new_record->record_id));
  offset += sizeof(new_record->record_id);

  memcpy(buffer + offset, &(new_record->category), sizeof(new_record->category));
  offset += sizeof(new_record->category);

  for (i = 0; i < new_record->num_fields; i++)
  {
    uint32 temp_field_type = 0;
    void *field_ptr;

    if (new_record->fields == NULL)
    {
      if (opened)
      {
        fs_close(file_handle, NULL, &close_rsp_msg);
        if (close_rsp_msg.close.status != FS_OKAY_S)
          PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
      }
      PBM_MSG_ERR("new_record->fields NULL",0,0,0);
      return PBM_ERROR;
    }

    memcpy( buffer + offset, &(new_record->fields[i].field_id),
            sizeof(new_record->fields[i].field_id));
    offset += sizeof(new_record->fields[i].field_id);

    temp_field_type = (uint32)new_record->fields[i].field_type;
    memcpy( buffer + offset, &temp_field_type, sizeof(uint32));
    offset += sizeof(uint32);

    memcpy( buffer + offset, &(new_record->fields[i].data_len),
            sizeof(new_record->fields[i].data_len));
    offset += sizeof(new_record->fields[i].data_len);

    field_ptr = pbm_get_field_data_ptr(new_record->fields,i);
    if (field_ptr == NULL)
    {
      if (opened)
      {
        fs_close(file_handle, NULL, &close_rsp_msg);
        if (close_rsp_msg.close.status != FS_OKAY_S)
          PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
      }
      PBM_MSG_ERR("field_ptr NULL 0x%x %d",new_record->fields,i,0);
      return PBM_ERROR;
    }

    memcpy(buffer + offset, field_ptr,
           new_record->fields[i].data_len);
    offset += new_record->fields[i].data_len;
  }

  /* Write the buffer to the file*/
  pbm_kick_dog();
  if (async) {
    fs_rsp_msg_type *write_msg = &(pbm_efs_buffers[pbm_efs_buf_last].write_msg);
    fs_rsp_msg_type *close_msg = &(pbm_efs_buffers[pbm_efs_buf_last].close_msg);

    pbm_efs_buffers[pbm_efs_buf_last].count = rec_len;
    pbm_efs_buf_last = (pbm_efs_buf_last + 1)%PBM_ASYNC_BUFFERS;
    pbm_efs_buf_empty = FALSE;
    fs_write(file_handle, buffer, rec_len, pbm_efs_write_cb, write_msg);

    fs_close(file_handle, pbm_efs_close_cb, close_msg);
  } else {
    fs_write(file_handle, buffer, rec_len, NULL, &write_rsp_msg);
    if ((write_rsp_msg.write.status != FS_OKAY_S) ||
        (write_rsp_msg.write.count != rec_len) )
    {
      if (opened)
      {
        fs_close(file_handle, NULL, &close_rsp_msg);
        if (close_rsp_msg.close.status != FS_OKAY_S)
          PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
      }
      PBM_MSG_ERR("Error writing to Master File", 0, 0, 0);
      return PBM_ERROR;
    }

    /* Close the file*/
    if (opened)
    {
      fs_close(file_handle, NULL, &close_rsp_msg);
      if (close_rsp_msg.close.status != FS_OKAY_S)
      {
        PBM_MSG_ERR("Error closing Master File", 0, 0, 0);
        return PBM_ERROR;
      }
    }
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_EFS_DELETE

DESCRIPTION
Adds a record's Unique ID to the Delete File.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_efs_delete(const pbm_addr_cache_s_type *record_to_delete,
                               boolean async)
{
  static fs_handle_type file_handle = 0;

  fs_rsp_msg_type open_rsp_msg, write_rsp_msg, close_rsp_msg;

  PBM_CHECK_PTR_RET(record_to_delete, PBM_ERROR);

  /* Open Delete File for append*/
  fs_open(g_delete_filename, FS_OA_APPEND, NULL, NULL, &open_rsp_msg);
  if (open_rsp_msg.open.status != FS_OKAY_S)
  {
    if (open_rsp_msg.open.status == FS_NONEXISTENT_FILE_S)
    {
      /* If file doesn't already exist, open for create*/
      fs_open_xparms_type attribs;
      attribs.create.cleanup_option = FS_OC_REVERT;
      attribs.create.buffering_option = FS_OB_PROHIBIT;
      attribs.create.attribute_mask = FS_FA_UNRESTRICTED;
      fs_open(g_delete_filename, FS_OA_CREATE, &attribs, NULL, &open_rsp_msg);
      if (open_rsp_msg.open.status != FS_OKAY_S)
      {
        /* Still doesn't work*/
        PBM_MSG_ERR("Error creating Delete File", 0, 0, 0);
        return PBM_ERROR;
      }
    }
    else
    {
      PBM_MSG_ERR("Error opening Delete File", 0, 0, 0);
      return PBM_ERROR;
    }
  }
  file_handle = open_rsp_msg.open.handle;

  /* Write the Unique ID to the file*/
  pbm_kick_dog();
  if (async) {
    char *buffer = pbm_efs_buffers[pbm_efs_buf_last].buffer;
    fs_rsp_msg_type *write_msg = &(pbm_efs_buffers[pbm_efs_buf_last].write_msg);
    fs_rsp_msg_type *close_msg = &(pbm_efs_buffers[pbm_efs_buf_last].close_msg);

    pbm_efs_buffers[pbm_efs_buf_last].count =
                                    sizeof(record_to_delete->unique_id);
    pbm_efs_buf_last = (pbm_efs_buf_last + 1)%PBM_ASYNC_BUFFERS;
    pbm_efs_buf_empty = FALSE;
    memcpy(buffer, &(record_to_delete->unique_id),
             sizeof(record_to_delete->unique_id));
    fs_write(file_handle, buffer, sizeof(record_to_delete->unique_id),
             pbm_efs_write_cb, write_msg);

    fs_close(file_handle, pbm_efs_close_cb, close_msg);
  } else {
    fs_write(file_handle, &(record_to_delete->unique_id),
             sizeof(record_to_delete->unique_id),
             NULL, &write_rsp_msg);
    if ((write_rsp_msg.write.status != FS_OKAY_S) ||
        (write_rsp_msg.write.count != sizeof(record_to_delete->unique_id)) )
    {
      fs_close(file_handle, NULL, &close_rsp_msg);
      if (close_rsp_msg.close.status != FS_OKAY_S)
        PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
      PBM_MSG_ERR("Error writing to Delete File", 0, 0, 0);
      return PBM_ERROR;
    }

    /* Close the file*/
    fs_close(file_handle, NULL, &close_rsp_msg);
    if (close_rsp_msg.close.status != FS_OKAY_S)
    {
      PBM_MSG_ERR("Error closing Delete File", 0, 0, 0);
      return PBM_ERROR;
    }
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_CONVERT_OLD_FILES

DESCRIPTION
  Looks at the EFS and makes sure the master_file is the latest
  version.  If its not, then it updates it and removes old files.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_convert_old_files(void)
{
  fs_rsp_msg_type rmfile_rsp_msg, check_new_rsp_msg, open_rsp_msg, close_rsp_msg;
  fs_open_xparms_type attribs;
  pbm_return_type ret_val = PBM_SUCCESS;
  fs_handle_type old_file_handle = 0;
  fs_handle_type new_file_handle = 0;
  char *buffer;
  pbm_addr_cache_s_type record;

  /* Remove the backup Master File if one exists */
  pbm_kick_dog();
  fs_remove(g_old_master_filename, NULL, &rmfile_rsp_msg);
  if ( (rmfile_rsp_msg.rmfile.status != FS_OKAY_S) &&
       (rmfile_rsp_msg.rmfile.status != FS_NONEXISTENT_FILE_S) )
  {
    PBM_MSG_ERR("Error deleting old backup Master File", 0, 0, 0);
  }

  /* Check if "Version 1" master file exists. */
  fs_nametest(g_v1_master_filename, FS_TEST_FILE, NULL, &check_new_rsp_msg);
  if (check_new_rsp_msg.nametest.status != FS_OKAY_S) {
    PBM_MSG_ERR("Error while checking if Version 1 Master File exists", 0, 0, 0);
    return PBM_ERROR;
  }
  if (check_new_rsp_msg.nametest.name_found)
  {
    dword delete_size = 0;  /* num of elements in delete list*/
    fs_rsp_msg_type check_del_rsp_msg;
    fs_rsp_msg_type file_size_rsp_msg;

    /* Check if delete file exists, if its too big, we'll just delete the
     * old stuff. */
    fs_nametest(g_delete_filename, FS_TEST_FILE, NULL, &check_del_rsp_msg);
    if (check_del_rsp_msg.nametest.status != FS_OKAY_S) {
      PBM_MSG_ERR("Error while checking if Delete File exists", 0, 0, 0);
      return PBM_ERROR;
    }
    /* If delete file is too big, skip garbage collection and return */
    if (check_del_rsp_msg.nametest.name_found)
    {
      /* Determine Delete List size*/
      fs_file_size(g_delete_filename, NULL, &file_size_rsp_msg);
      if (file_size_rsp_msg.file_size.status != FS_OKAY_S) {
        PBM_MSG_ERR("Error getting size of delete file", 0, 0, 0);
        return PBM_ERROR;
      }
      delete_size = file_size_rsp_msg.file_size.size/sizeof(unsigned short);
      if (delete_size > PBM_MAX_DELETE_SIZE)
      {
        fs_remove(g_v1_master_filename, NULL, &rmfile_rsp_msg);
        if ( (rmfile_rsp_msg.rmfile.status != FS_OKAY_S) &&
             (rmfile_rsp_msg.rmfile.status != FS_NONEXISTENT_FILE_S) )
        {
          PBM_MSG_ERR("Error deleting old V1 Master File", 0, 0, 0);
        }

        /* Remove Delete File*/
        fs_remove(g_delete_filename, NULL, &rmfile_rsp_msg);
        if (rmfile_rsp_msg.rmfile.status != FS_OKAY_S)
        {
          PBM_MSG_ERR("Error deleting Delete File", 0, 0, 0);
        }

        return PBM_SUCCESS;
      }
    }

    /* Open "V1" Master File*/
    pbm_kick_dog();
    fs_open(g_v1_master_filename, FS_OA_READONLY, NULL, NULL, &open_rsp_msg);
    if (open_rsp_msg.open.status != FS_OKAY_S)
    {
      PBM_MSG_ERR("Error opening old Master File", 0, 0, 0);
      return PBM_ERROR;
    }
    old_file_handle = open_rsp_msg.open.handle;

    /* Remove the partially build V2 Master File if one exists */
    fs_remove(g_master_filename, NULL, &rmfile_rsp_msg);
    if ( (rmfile_rsp_msg.rmfile.status != FS_OKAY_S) &&
         (rmfile_rsp_msg.rmfile.status != FS_NONEXISTENT_FILE_S) )
    {
      PBM_MSG_ERR("Error deleting old backup Master File", 0, 0, 0);
    }

    /* Open "V2" Master File*/
    attribs.create.cleanup_option = FS_OC_REVERT;
    attribs.create.buffering_option = FS_OB_PROHIBIT;
    attribs.create.attribute_mask = FS_FA_UNRESTRICTED;
    fs_open(g_master_filename, FS_OA_CREATE, &attribs, NULL, &open_rsp_msg);
    if (open_rsp_msg.open.status != FS_OKAY_S)
    {
      fs_close(old_file_handle, NULL, &close_rsp_msg);
      if (close_rsp_msg.close.status != FS_OKAY_S)
        PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
      PBM_MSG_ERR("Error opening new Master File", 0, 0, 0);
      return PBM_ERROR;
    }
    new_file_handle = open_rsp_msg.open.handle;

    /* Allocate the read buffer*/
    PBM_ALLOC_MEM_W_ERR_RET(buffer, PBM_MAX_EFS_REC_LEN * sizeof(char),
            fs_close(old_file_handle, NULL, &close_rsp_msg);
            if (close_rsp_msg.close.status != FS_OKAY_S)
              PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
            fs_close(new_file_handle, NULL, &close_rsp_msg);
            if (close_rsp_msg.close.status != FS_OKAY_S)
              PBM_MSG_ERR("Could not close file handle", 0, 0, 0););

    while (PBM_SUCCESS == (ret_val = pbm_efs_read_v1(old_file_handle, &record)))
    {
      ret_val = pbm_efs_add(new_file_handle, &record, FALSE);
      if (ret_val != PBM_SUCCESS)
      {
        PBM_MSG_ERR("Error 0x%x Writing to new file.", ret_val, 0, 0);
        break;
      }
      pbm_free_fields_internal(&record.fields, record.num_fields);
    }
    if ((ret_val == PBM_ERROR_NOT_FOUND) || (ret_val == PBM_EOF))
    {
      ret_val = PBM_SUCCESS;
    }

    PBM_MEM_FREEIF(buffer);

    pbm_kick_dog();
    fs_close(old_file_handle, NULL, &close_rsp_msg);
    if (close_rsp_msg.close.status != FS_OKAY_S)
      PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
    fs_close(new_file_handle, NULL, &close_rsp_msg);
    if (close_rsp_msg.close.status != FS_OKAY_S)
      PBM_MSG_ERR("Could not close file handle", 0, 0, 0);

    /* Remove the replaced V1 Master File */
    fs_remove(g_v1_master_filename, NULL, &rmfile_rsp_msg);
    if ( (rmfile_rsp_msg.rmfile.status != FS_OKAY_S) &&
         (rmfile_rsp_msg.rmfile.status != FS_NONEXISTENT_FILE_S) )
    {
      PBM_MSG_ERR("Error deleting old V1 Master File", 0, 0, 0);
    }

  }

  return ret_val;
}

/*===========================================================================
FUNCTION PBM_EFS_GARBAGE_COLLECT

DESCRIPTION
Copies the non-deleted records of the Master File to a new file and removes
the Delete File. The unique id is reset only if reset_uid is set to TRUE.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_efs_garbage_collect(boolean reset_uid)
{
  static fs_handle_type old_file_handle = 0;
  static fs_handle_type new_file_handle = 0;
  static fs_handle_type delete_file_handle = 0;
  static uint16 *delete_list = NULL;
  static dword delete_size = 0;  /* num of elements in delete list*/
  pbm_addr_cache_s_type record;
  char *buffer;

  fs_rsp_msg_type open_rsp_msg;
  fs_rsp_msg_type file_size_rsp_msg;
  fs_rsp_msg_type read_rsp_msg;
  fs_rsp_msg_type close_rsp_msg;
  fs_rsp_msg_type old_close_rsp_msg;
  fs_rsp_msg_type new_close_rsp_msg;
  fs_rsp_msg_type rename_rsp_msg;
  fs_rsp_msg_type rmfile_rsp_msg;
  fs_rsp_msg_type check_new_rsp_msg;
  fs_rsp_msg_type check_del_rsp_msg;
  fs_open_xparms_type attribs;

  uint16 i;
  boolean deleted_record = FALSE;
  uint32 num_deleted_records = 0;

  pbm_return_type ret_val;

  /* Check if delete file exists, result used later */
  pbm_kick_dog();
  fs_nametest(g_delete_filename, FS_TEST_FILE, NULL, &check_del_rsp_msg);
  if (check_del_rsp_msg.nametest.status != FS_OKAY_S) {
    PBM_MSG_ERR("Error while checking if Delete File exists", 0, 0, 0);
    return PBM_ERROR;
  }


  /* Check if "new" master file exists. */
  fs_nametest(g_new_master_filename, FS_TEST_FILE, NULL, &check_new_rsp_msg);
  if (check_new_rsp_msg.nametest.status != FS_OKAY_S) {
    PBM_MSG_ERR("Error while checking if New Master File exists", 0, 0, 0);
    return PBM_ERROR;
  }

  if (check_new_rsp_msg.nametest.name_found) {
    /* Garbage collection was not completed last time. */

    if (check_del_rsp_msg.nametest.name_found) {
      /* Delete file found. Remove the "new" file that was created and do
       * garbage collection again this time. */
      fs_remove(g_new_master_filename, NULL, &rmfile_rsp_msg);
      if (rmfile_rsp_msg.rmfile.status != FS_OKAY_S) {
        PBM_MSG_ERR("Error deleting New Master File", 0, 0, 0);
        return PBM_ERROR;
      }
    } else {
      /* Delete file not found. Previous attempt at garbage collection
       * was successful. Copy "new" masterfile to the actual master file. */
      fs_remove(g_master_filename, NULL, &rmfile_rsp_msg);
      if ((rmfile_rsp_msg.rmfile.status != FS_OKAY_S) &&
          (rmfile_rsp_msg.rmfile.status != FS_NONEXISTENT_FILE_S))
      {
        PBM_MSG_ERR("Error deleting Master File", 0, 0, 0);
        return PBM_ERROR;
      }
      fs_rename(g_new_master_filename, g_master_filename, NULL,
                                                  &rename_rsp_msg);
      if (rename_rsp_msg.rename.status != FS_OKAY_S) {
        PBM_MSG_ERR("Error renaming New Master File to Master File", 0, 0, 0);
        return PBM_ERROR;
      }
    }
  }

  /* Return Success if delete file not found */
  if (!check_del_rsp_msg.nametest.name_found)
    return PBM_SUCCESS;

  /* Determine Delete List size*/
  fs_file_size(g_delete_filename, NULL, &file_size_rsp_msg);
  if (file_size_rsp_msg.file_size.status != FS_OKAY_S) {
    PBM_MSG_ERR("Error getting size of delete file", 0, 0, 0);
    return PBM_ERROR;
  }

  delete_size = file_size_rsp_msg.file_size.size/sizeof(delete_list[0]);
  if ((delete_size * sizeof(delete_list[0])) != file_size_rsp_msg.file_size.size)
  {
    PBM_MSG_ERR("Delete file is not an even number of bytes.", 0, 0, 0);
    return PBM_ERROR;
  }

  if (delete_size == 0)
  {
    /* No delete list -- we're done.*/
    return PBM_SUCCESS;
  }

  if (delete_size <= PBM_MAX_DELETE_SIZE)
  {
    /* Open the Delete File*/
    pbm_kick_dog();
    fs_open(g_delete_filename, FS_OA_READONLY, NULL, NULL, &open_rsp_msg);
    if (open_rsp_msg.open.status != FS_OKAY_S)
    {
      PBM_MSG_ERR("Error opening Delete File", 0, 0, 0);
      return PBM_ERROR;
    }
    delete_file_handle = open_rsp_msg.open.handle;

    PBM_ALLOC_MEM_W_ERR_RET(delete_list, delete_size * sizeof(delete_list[0]),
                    NO_ACTION);

    /* Read in the delete list*/
    fs_read(delete_file_handle, delete_list, delete_size*sizeof(delete_list[0]),
            NULL, &read_rsp_msg);

    if ( (read_rsp_msg.read.count < delete_size * sizeof(delete_list[0])) &&
         (read_rsp_msg.read.status == FS_OKAY_S) )
    {
      read_rsp_msg.read.status = FS_EOF_S;
    }

    fs_close(delete_file_handle, NULL, &close_rsp_msg);
    if (close_rsp_msg.close.status != FS_OKAY_S)
    {
      PBM_MEM_FREEIF(delete_list);
      PBM_MSG_ERR("Error closing Delete File", 0, 0, 0);
      return PBM_ERROR;
    }
    if ( (read_rsp_msg.read.status != FS_OKAY_S) ||
         (read_rsp_msg.read.count != delete_size * sizeof(delete_list[0])) )
    {
      PBM_MEM_FREEIF(delete_list);
      PBM_MSG_ERR("Error reading Delete File", 0, 0, 0);
      return PBM_ERROR;
    }

    /* Open "old" Master File*/
    fs_open(g_master_filename, FS_OA_READONLY, NULL, NULL, &open_rsp_msg);
    if (open_rsp_msg.open.status != FS_OKAY_S)
    {
      PBM_MEM_FREEIF(delete_list);
      PBM_MSG_ERR("Error opening old Master File", 0, 0, 0);
      return PBM_ERROR;
    }
    old_file_handle = open_rsp_msg.open.handle;

    /* Open "new" Master File*/
    attribs.create.cleanup_option = FS_OC_REVERT;
    attribs.create.buffering_option = FS_OB_PROHIBIT;
    attribs.create.attribute_mask = FS_FA_UNRESTRICTED;
    fs_open(g_new_master_filename, FS_OA_CREATE, &attribs, NULL, &open_rsp_msg);
    if (open_rsp_msg.open.status != FS_OKAY_S)
    {
      PBM_MEM_FREEIF(delete_list);
      fs_close(old_file_handle, NULL, &old_close_rsp_msg);
      if (old_close_rsp_msg.close.status != FS_OKAY_S)
        PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
      PBM_MSG_ERR("Error opening new Master File", 0, 0, 0);
      return PBM_ERROR;
    }
    new_file_handle = open_rsp_msg.open.handle;

    /* Allocate the read buffer*/
    PBM_ALLOC_MEM_W_ERR_RET(buffer, PBM_MAX_EFS_REC_LEN * sizeof(char),
      fs_close(old_file_handle, NULL, &old_close_rsp_msg);
      if (old_close_rsp_msg.close.status != FS_OKAY_S)
        PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
      PBM_MEM_FREEIF(delete_list););

    if (reset_uid)
      pbm_efs_set_unique_id (1);
    while (PBM_SUCCESS == (ret_val = pbm_efs_read(old_file_handle, &record)))
    {
      /* Check if it's a deleted record.*/
      deleted_record = FALSE;
      for (i = 0; i < delete_size; i++)
      {
        if (delete_list[i] == record.unique_id)
        {
          /* Deleted record -- toss out*/
          deleted_record = TRUE;
          break;
        }
      }

      if (!deleted_record)
      {
        if (reset_uid)
          record.unique_id = pbm_efs_generate_unique_id();
        ret_val = pbm_efs_add(new_file_handle, &record, FALSE);
        if (ret_val != PBM_SUCCESS)
        {
          PBM_MSG_ERR("Error 0x%x Writing to new file.", ret_val, 0, 0);
          break;
        }
      }
      else
      {
        num_deleted_records++;
      }

      pbm_free_fields_internal(&record.fields, record.num_fields);
    }

    pbm_kick_dog();
    fs_close(old_file_handle, NULL, &old_close_rsp_msg);
    if (old_close_rsp_msg.close.status != FS_OKAY_S)
      PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
    fs_close(new_file_handle, NULL, &new_close_rsp_msg);
    if (new_close_rsp_msg.close.status != FS_OKAY_S)
      PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
    PBM_MEM_FREEIF(buffer);
    PBM_MEM_FREEIF(delete_list);
    if (num_deleted_records != delete_size)
    {
      PBM_MSG_ERR("Delete size %d records deleted %d", delete_size,
          num_deleted_records, 0);
    }
  }
  else
  {
    ret_val = PBM_ERROR;
  }

  if ((ret_val != PBM_ERROR_NOT_FOUND) && (ret_val != PBM_EOF))
  {
    pbm_cleanup_corrupt_files(g_master_filename);
    ret_val = PBM_SUCCESS;
  }
  else
  {
    ret_val = PBM_SUCCESS;

    /* Remove Delete File*/
    pbm_kick_dog();
    fs_remove(g_delete_filename, NULL, &rmfile_rsp_msg);
    if (rmfile_rsp_msg.rmfile.status != FS_OKAY_S)
    {
      PBM_MSG_ERR("Error deleting Delete File", 0, 0, 0);
      return PBM_ERROR;
    }

    /* Copy new master file to the actual master file */
    fs_remove(g_master_filename, NULL, &rmfile_rsp_msg);
    if (rmfile_rsp_msg.rmfile.status != FS_OKAY_S) {
      PBM_MSG_ERR("Error %d deleting Master File",
          rmfile_rsp_msg.rmfile.status, 0, 0);
      return PBM_ERROR;
    }
    fs_rename(g_new_master_filename, g_master_filename, NULL,
                                                  &rename_rsp_msg);
    if (rename_rsp_msg.rename.status != FS_OKAY_S)
    {
      PBM_MSG_ERR("Error renaming New Master File to Master File", 0, 0, 0);
      return PBM_ERROR;
    }
  }

  return ret_val;
}


/*===========================================================================
FUNCTION PBM_EFS_CACHE_STRUCTURE_INIT

DESCRIPTION
  Initializes a new cache to contain a given number of records.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_efs_cache_structure_init(PB_CACHE *cache, 
                                             pbm_device_type pb_id,
                                             int num_of_records,
                                             boolean circular)
{
  PBM_CHECK_PTR_RET(cache, PBM_ERROR);

  cache->pb_id          = pb_id;
  cache->status         = PBM_STATUS_NOT_INIT;
  cache->circular       = circular;
  cache->text_length    = 255;
  cache->record_length  = 65535;
  cache->file_length    = 0;
  cache->num_of_records = num_of_records;
  cache->records_in_use = 0;
  cache->start          = 1;
  cache->pb_cache_array = NULL;

  PBM_ALLOC_MEM_W_ERR_RET(cache->pb_cache_array,
                          (uint32) (cache->num_of_records+1) * 
                                    sizeof(pbm_addr_cache_s_type *),
                          NO_ACTION);

  /* Zero-out cache to start with*/
  if (cache->pb_cache_array)
    memset(cache->pb_cache_array, 0,
        (uint32) (cache->num_of_records+1) * sizeof(pbm_addr_cache_s_type *) );
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_EFS_CACHE_INIT

DESCRIPTION
Loads all records from EFS.
The following caches are initialized from this file:
  Address Book cache.
  Speed Dial List
  Call History Caches.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_efs_cache_init(boolean powerup)
{
  pbm_addr_cache_s_type *record;
  pbm_return_type ret_val = PBM_SUCCESS;
  fs_rsp_msg_type open_rsp_msg, close_rsp_msg;
  uint16          location;
  static fs_handle_type file_handle = 0;
  uint16 max_unique_id = 0;

  if (PBM_SUCCESS != pbm_efs_cache_structure_init(&efs_cache, PBM_EFS,
                                                  PBM_MAX_EFS_RECS, FALSE))
    return PBM_ERROR;
  if (PBM_SUCCESS != pbm_efs_cache_structure_init(&sdl_cache, PBM_SDL,
                                                  PBM_MAX_SPEED_DIALS, FALSE))
    return PBM_ERROR;
  if (PBM_SUCCESS != pbm_efs_cache_structure_init(&incoming_calls_cache, PBM_RCVD,
                                                  PBM_MAX_CALL_HISTORY, TRUE))
    return PBM_ERROR;
  if (PBM_SUCCESS != pbm_efs_cache_structure_init(&outgoing_calls_cache, PBM_DIALED,
                                                  PBM_MAX_CALL_HISTORY, TRUE))
    return PBM_ERROR;
  if (PBM_SUCCESS != pbm_efs_cache_structure_init(&missed_calls_cache, PBM_MISS,
                                                  PBM_MAX_CALL_HISTORY, TRUE))
    return PBM_ERROR;


  /* Open Master File*/
  fs_open(g_master_filename, FS_OA_READONLY, NULL, NULL, &open_rsp_msg);
  if ((open_rsp_msg.open.status != FS_OKAY_S) &&
      (open_rsp_msg.open.status != FS_NONEXISTENT_FILE_S))
  {
    PBM_MSG_ERR("Error opening Master File", 0, 0, 0);
    return PBM_ERROR;
  }
  if (open_rsp_msg.open.status == FS_OKAY_S)
  {
    file_handle = open_rsp_msg.open.handle;

    /* Load from EFS*/
    record = pbm_cache_record_new();
    PBM_CHECK_PTR_RET(record, PBM_ERROR);
    while (PBM_SUCCESS == (ret_val = pbm_efs_read(file_handle, record)))
    {
      PB_CACHE *cache_pointer;

      /*Act as if this is the last record, and set Unique ID generator*/
      if (record->unique_id > max_unique_id)
        max_unique_id = record->unique_id;
      else
      {
        PBM_MSG_ERR("Found unique id %d less than current max %d",
                     record->unique_id, max_unique_id, 0);
        pbm_free_fields_internal(&record->fields, record->num_fields);
        continue;
      }

      cache_pointer = pbm_pb_id_to_cache(
                          pbm_record_id_to_phonebook(record->record_id));

      if (cache_pointer == NULL)
      {
        pbm_free_fields_internal(&record->fields, record->num_fields);
        PBM_MSG_ERR("Add to cache failed for record ID %d.",
            record->record_id, 0, 0);
        continue;
      }

      location = pbm_record_id_to_location(record->record_id);
      if ((location > 0) &&
          (location <= cache_pointer->num_of_records))
      {
        if (cache_pointer->pb_cache_array[location] &&
            cache_pointer->pb_cache_array[location]->num_fields)
        {
          PBM_MSG_ERR("Duplicate record ID 0x%x found in master file",
              record->record_id, 0, 0);
          /* Make sure the old record is deleted. */
          (void) pbm_efs_delete(cache_pointer->pb_cache_array[location], FALSE);
        }
      }
      ret_val = pbm_cache_add(cache_pointer, record, FALSE);
      if ( ret_val != PBM_SUCCESS )
      {
        pbm_free_fields_internal(&record->fields, record->num_fields);
        PBM_MSG_ERR("Add to cache failed with error %d.", ret_val, 0, 0);
        continue;
      }
      /* Assuming latest entry will be later in file. */
      if (cache_pointer->circular) {
        cache_pointer->start = pbm_record_id_to_location(record->record_id);
      }

      /* Allocate new record */
      record = pbm_cache_record_new();
      if (!record) 
      {
        fs_close(file_handle, NULL, &close_rsp_msg);
        if (close_rsp_msg.close.status != FS_OKAY_S)
          PBM_MSG_ERR("Could not close file handle", 0, 0, 0);
        return PBM_ERROR_MEM_FULL;
      }
    }
    pbm_efs_set_unique_id (++max_unique_id);

    /* Free our memory, close the files. */
    PBM_MEM_FREEIF(record);
    fs_close(file_handle, NULL, &close_rsp_msg);
    if (close_rsp_msg.close.status != FS_OKAY_S)
    {
      PBM_MSG_ERR("Error closing Master File", 0, 0, 0);
    }

    /* Empty Master File -- normal exit condition*/
    if (ret_val != PBM_EOF)
    {
      PBM_MSG_ERR("Error initializing PBM cache.", 0, 0, 0);
      return ret_val;
    }
  }

  PBM_CIRC_CACHE_ADDSTART(&incoming_calls_cache);
  PBM_CIRC_CACHE_ADDSTART(&outgoing_calls_cache);
  PBM_CIRC_CACHE_ADDSTART(&missed_calls_cache);

  /* on powerup, we want to wait until after phone UID is intialized 
   * to say that the caches are ready.  Otherwise, they're ready now. */
  if (!powerup)
  {
    PBM_SET_PB_CACHE_READY(&efs_cache);
    PBM_SET_PB_CACHE_READY(&sdl_cache);
    PBM_SET_PB_CACHE_READY(&incoming_calls_cache);
    PBM_SET_PB_CACHE_READY(&outgoing_calls_cache);
    PBM_SET_PB_CACHE_READY(&missed_calls_cache);
  }

  {
    int verify_index;
    int cache_index;
    PB_CACHE *circ_cache[] = {&incoming_calls_cache,
                              &outgoing_calls_cache,
                              &missed_calls_cache};

    /* Check each of the circular caches to make sure the entries are
     * stored contiguously.  If the EFS file is corrupted, this may not
     * have been detected yet.  If we find an issue, clear out that
     * cache, but allow the phone to still use that call history, just
     * forget about the bad (old) contents.
     */
    for (cache_index = 0; cache_index < (int) ARR_SIZE(circ_cache); cache_index++)
    {
      for (verify_index = 1; verify_index <= (circ_cache[cache_index])->records_in_use; verify_index++)
      {
        if (!circ_cache[cache_index]->pb_cache_array[
            PBM_CIRC_CACHE_NTHLOC(circ_cache[cache_index], verify_index)])
        {
          PBM_MSG_ERR("Built cache %d wrong entry %d of %d is NULL. Clearing cache.",
              cache_index, verify_index, circ_cache[cache_index]->records_in_use);

          /* Delete all entries in the file. */
          for (verify_index = 1;
               verify_index <= circ_cache[cache_index]->num_of_records;
               verify_index++)
          {
            if (circ_cache[cache_index]->pb_cache_array[verify_index] &&
                circ_cache[cache_index]->pb_cache_array[verify_index]->num_fields)
              (void) pbm_efs_delete(circ_cache[cache_index]->pb_cache_array[verify_index], FALSE);
          }

          /* Protect the sanctity of the cache. */
          rex_enter_crit_sect(&pbm_crit_sect);
          pbm_clear_cache(circ_cache[cache_index]);
          if (PBM_SUCCESS == pbm_efs_cache_structure_init(circ_cache[cache_index],
                                                          circ_cache[cache_index]->pb_id,
                                                          PBM_MAX_CALL_HISTORY, TRUE))
          {
            PBM_SET_PB_CACHE_READY(circ_cache[cache_index]);
          }
          rex_leave_crit_sect(&pbm_crit_sect);
          break;
        }
      }
    }
  }

  return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_EFS_WRITE_LOCK_CB

DESCRIPTION
  This function is called after write lock command is processed.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -e{818} suppress param could be const */
static void pbm_efs_write_lock_cb(pbm_writecb_data_s_type *cb_data)
{
  pbm_return_type ret_val;

  PBM_CHECK_PTR_RET(cb_data, VOID);

  PBM_CHECK_ERR(cb_data->ret, "PBM EFS wrile lock callback error");

  /* Do garbage collection */
  pbm_efs_garbage_recollect(TRUE);

  ret_val = pbm_write_unlock();

  PBM_CHECK_ERR(ret_val, "PBM unlock failure");
}

/*===========================================================================
FUNCTION PBM_EFS_GARBAGE_RECOLLECT

DESCRIPTION
  Does garbage collection at any point after init stage. Actual
  garbage collection occurs only when the number of records deleted after
  the last reboot exceeds a certain value.

  If reset is TRUE, the cache is cleared and unique id is regenerated.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_efs_garbage_recollect(boolean reset_uid)
{
  int i;
  pbm_return_type retval;
  fs_rsp_msg_type file_size_rsp_msg;
  fs_rsp_msg_type check_del_rsp_msg;
  dword delete_size = 0;
  PB_CACHE *pb_caches[] = { &efs_cache,
                            &sdl_cache,
                            &incoming_calls_cache,
                            &outgoing_calls_cache,
                            &missed_calls_cache
                          };

  /* Does the delete file exist? */
  pbm_kick_dog();
  fs_nametest(g_delete_filename, FS_TEST_FILE, NULL, &check_del_rsp_msg);
  if (check_del_rsp_msg.nametest.status != FS_OKAY_S) {
    PBM_MSG_ERR("Error while checking if Delete File exists", 0, 0, 0);
    return;
  }

  if (!check_del_rsp_msg.nametest.name_found)
    return;

  /* Determine Delete List size*/
  fs_file_size(g_delete_filename, NULL, &file_size_rsp_msg);
  if (file_size_rsp_msg.file_size.status != FS_OKAY_S) {
    if (file_size_rsp_msg.file_size.status == FS_NONEXISTENT_FILE_S)
    {
      /* No delete file, we're OK.  */
      return;
    }
    PBM_MSG_ERR("Error getting size of delete file", 0, 0, 0);
  }

  delete_size = file_size_rsp_msg.file_size.size/sizeof(uint16);

  if (delete_size < PBM_MAX_EFS_DELETES)
    return;     /* Not enough garbage, don't clean it up. */

  if (!reset_uid)
  {
    /* Try to acquire write_lock and return immediately if successful. 
     * Garbage collection will be done after write lock is complete.
     * Otherwise do garbage collection without updating cache. */
    retval = pbm_write_lock(PBM_RW_LOCK_BLOCKING, NULL, pbm_efs_write_lock_cb);
    if (retval == PBM_SUCCESS)
      return;
    else
      PBM_MSG_HIGH("Unable to obtain write lock", 0, 0, 0);
  } 
  else
  {
    rex_enter_crit_sect(&pbm_crit_sect);
    for (i = 0; i < (int) ARR_SIZE(pb_caches); ++i)
      pbm_clear_cache(pb_caches[i]);
    rex_leave_crit_sect(&pbm_crit_sect);
  }
  
  (void) pbm_efs_garbage_collect(reset_uid);

  if (reset_uid)
  {
    retval = pbm_efs_cache_init(FALSE);
    if (retval != PBM_SUCCESS)
    {
      PBM_MSG_ERR("Cache Init returned a failure %d", retval, 0, 0);
    }
  }

  return;
}

/*===========================================================================
FUNCTION PBM_RECORD_WRITE_TO_EFS

DESCRIPTION
  Adds a record to EFS. This is an internal function and should be called only from
  within PBM task. 

DEPENDENCIES
  The caller should free write cmd structure in case of an error. 
  If successful, write cmd is freed by this function (synchronous operation) or
  within the write callbacks (asynchronous operation).

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_write_to_efs(pbm_write_record_s_type *write)
{
  pbm_return_type ret_val;
  boolean async = FALSE;
  boolean deletion_only;

  PBM_CHECK_PTR_RET(write, PBM_ERROR);

  if (write->proc_func)
  {
    async = TRUE;
  }
  deletion_only = (write->curr_rec && write->curr_rec->num_fields) ?
                  FALSE : TRUE;
  
  if (async) {

    if (!pbm_efs_buf_empty && (pbm_efs_buf_first == pbm_efs_buf_last))
    {
      PBM_MSG_ERR("Could not write to efs buffers.", pbm_efs_buf_empty,
                  pbm_efs_buf_first, pbm_efs_buf_last);
      pbm_list_recover(write, FALSE);
      return PBM_ERROR_MEM_FULL;
    }

    pbm_efs_buffers[pbm_efs_buf_last].write = write;
    pbm_efs_buffers[pbm_efs_buf_last].cb_called = FALSE;
    pbm_efs_buffers[pbm_efs_buf_last].deleted = FALSE;
  } 

  if (write->prev_rec && write->prev_rec->num_fields)
  {
    /* Delete record from file.
     * An async delete is initiated only if there no addition within this
     * operation. */
    ret_val = pbm_efs_delete(write->prev_rec, 
                             (boolean) (async && deletion_only));
    /* if async && deletion_only is TRUE, then write gets freed in the
     * callback pbm_efs_close_cb. Make sure it is no longer used in
     * that case */

    if (ret_val != PBM_SUCCESS)
    {
      pbm_list_recover(write, FALSE);
      PBM_MSG_ERR("PBM EFS delete failed with error %d.", ret_val, 0, 0);
      return ret_val;
    }
    pbm_efs_buffers[pbm_efs_buf_last].deleted = TRUE;
  }

  if (!deletion_only) 
  {
    /* Add record to file */
    ret_val = pbm_efs_add(FS_NULL_HANDLE, write->curr_rec, async);
    if (ret_val != PBM_SUCCESS)
    {
      PBM_MSG_ERR("EFS add failed with error %d.", ret_val, 0, 0);
      pbm_list_recover(write, TRUE);
      return ret_val;
    }
  }

  /* We are done with write command */
  if (!async)
  {
    pbm_list_update(write);
    pbm_write_notify(write, PBM_SUCCESS);
    pbm_write_cmd_free(write);
  }

  return PBM_SUCCESS; 
}

/*===========================================================================
FUNCTION PBM_INIT_PHONE_UNIQUE_ID

DESCRIPTION
  Initializes the phone unique ID.  If the phone unique ID file does not exist, but the master
  phonebook file does exists, then an error flag is raised indicating the UIDs may have changed.
  Note that this situation could also occur if the phonebook contains only non-EFS entries
  and the UID file never initialized.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/

static pbm_return_type pbm_init_phone_unique_id(void)
{
  pbm_return_type nRet = PBM_SUCCESS;
  fs_handle_type file_handle = 0;
  fs_rsp_msg_type rsp_msg;
  uint64 file_ph_uid;
  boolean file_ph_uid_is_reset;
  uint64 rec_ph_uid;
  boolean doRenumber = FALSE;
  int i;

  do
  {
    fs_open(g_ph_uid_filename, FS_OA_READONLY, NULL, NULL, &rsp_msg);
    if (rsp_msg.open.status != FS_OKAY_S)
    {
      if (rsp_msg.open.status != FS_NONEXISTENT_FILE_S) 
      {
        /* Error opening PH UID file, initialize failed. */
        PBM_MSG_ERR("Error opening PH UID file, fs_open %d", rsp_msg.open.status, 0, 0);
        nRet = PBM_ERROR;
        break;
      }

      doRenumber = TRUE;
      break; /* Done with initialization. */
    }

    file_handle = rsp_msg.open.handle;
      
    fs_read(file_handle, (void *)&file_ph_uid, sizeof(file_ph_uid),
         NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_SIZED_RETCODE_BREAK(rsp_msg.read, sizeof(file_ph_uid), "Error reading PH UID file", doRenumber, TRUE);

    fs_read(file_handle, (void *)&file_ph_uid_is_reset, sizeof(file_ph_uid_is_reset),
         NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_SIZED_RETCODE_BREAK(rsp_msg.read, sizeof(file_ph_uid_is_reset), "Error reading PH UID file", doRenumber, TRUE);

    /* Check if any EFS records have PH UID fields greater than the PH UID on file.
     * If so, then renumber the phonebook. */
    for (i = 0; i < efs_cache.num_of_records; i++) 
    {
      /* Skip empty records. */
      if ( (efs_cache.pb_cache_array[i] == NULL) ||
         (efs_cache.pb_cache_array[i]->fields == NULL) ||
         (efs_cache.pb_cache_array[i]->num_fields == 0) ) 
      {
         continue;
      }

      /* If any record doesn't have a unique ID, renumber the phonebook */
      if (pbm_get_phone_unique_id_from_record(efs_cache.pb_cache_array[i],
        &rec_ph_uid) != PBM_SUCCESS) 
      {
        doRenumber = TRUE;
        break;
      }

      /* if any record has a UID bigger than the counter file, renumber the phonebook */
      if (rec_ph_uid > file_ph_uid) 
      {
        doRenumber = TRUE;
        break;
      }
    }

    if (doRenumber) 
    {
      break;
    }

    g_ph_unique_id = file_ph_uid;
    g_ph_uid_is_reset = file_ph_uid_is_reset;
  }
  while (FALSE); /*lint !e717 */

  if (file_handle) 
  {
    fs_close(file_handle, NULL, &rsp_msg);
    if (rsp_msg.close.status != FS_OKAY_S) 
    {
      PBM_MSG_ERR("Error closing file, fs_close %d", rsp_msg.close.status, 0, 0);
    }
    file_handle = 0;
  }

  if (doRenumber == TRUE) 
  {
    nRet = pbm_renumber_with_unique_id(); /* This will re-create PH UID file and set global flags. */
  }

  return nRet;
}

/*===========================================================================
FUNCTION PBM_RECORD_WRITE_TO_EFS

DESCRIPTION
  Gets the next phone unique ID.  Increments the counter in EFS.

DEPENDENCIES
  Must be called from within pbm_crit_sect critical section.

SIDE EFFECTS
===========================================================================*/

pbm_return_type pbm_get_next_phone_unique_id(uint64 *ph_unique_id)
{
  pbm_return_type nRet = PBM_SUCCESS;
  fs_handle_type file_handle = 0;
  fs_rsp_msg_type rsp_msg;
  uint64 new_ph_uid;

  PBM_CHECK_PTR_RET(ph_unique_id, PBM_ERROR);

  do 
  {
    fs_open(g_ph_uid_filename, FS_OA_READWRITE, NULL, NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_RETCODE_BREAK(rsp_msg.open, "Error opening PH UID file", nRet, PBM_ERROR);

    file_handle = rsp_msg.open.handle;

    new_ph_uid = g_ph_unique_id + 1;

    fs_write(file_handle, (void *)&new_ph_uid, sizeof(new_ph_uid),
         NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_SIZED_RETCODE_BREAK(rsp_msg.write, sizeof(new_ph_uid), "Error writing PH UID file", nRet, PBM_ERROR);

    *ph_unique_id = g_ph_unique_id = new_ph_uid;
  }
  while (FALSE); /*lint !e717 */

  if (file_handle) 
  {
    fs_close(file_handle, NULL, &rsp_msg);
    if (rsp_msg.close.status != FS_OKAY_S) 
    {
      PBM_MSG_ERR("Could not close PH UID file handle, fs_close %d",
       rsp_msg.close.status, 0, 0);
    }
    file_handle = 0;
  }

  return nRet;
}

/*===========================================================================
FUNCTION PBM_CREATE_INITIAL_PH_UID_FILE

DESCRIPTION
  Helper function to create an initial PH UID file. Initialized with the uint64 value 0.
  
DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO

===========================================================================*/

static pbm_return_type pbm_create_initial_ph_uid_file(uint64 starting_value, boolean uid_is_reset)
{
  pbm_return_type nRet = PBM_SUCCESS;
  fs_handle_type file_handle = 0;
  fs_open_xparms_type attribs;
  fs_rsp_msg_type rsp_msg;

  /* Check if file exists. */
  fs_nametest(g_ph_uid_filename, FS_TEST_FILE, NULL, &rsp_msg);
  PBM_CHECK_FILE_STATUS_RET(rsp_msg.nametest, "Error while checking if PH UID File exists", PBM_ERROR);
  if (rsp_msg.nametest.name_found)
  {
    /* Remove the file. */
    fs_remove(g_ph_uid_filename, NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_RET(rsp_msg.rmfile, "Error deleting PH UID File", PBM_ERROR);
  }

  /* Create the file. */
  attribs.create.cleanup_option = FS_OC_REVERT;
  attribs.create.buffering_option = FS_OB_PROHIBIT;
  attribs.create.attribute_mask = FS_FA_UNRESTRICTED;

  fs_open(g_ph_uid_filename, FS_OA_CREATE, &attribs, NULL, &rsp_msg);
  PBM_CHECK_FILE_STATUS_RET(rsp_msg.open, "Error creating PH UID File", PBM_ERROR);

  file_handle = rsp_msg.open.handle;

  fs_write(file_handle, (void *) &starting_value, sizeof(starting_value),
      NULL, &rsp_msg);
  PBM_CHECK_FILE_STATUS_SIZED_RETCODE(rsp_msg.write, sizeof(starting_value), "Error writing PH UID file", nRet, PBM_ERROR);

  fs_write(file_handle, (void *) &uid_is_reset, sizeof(uid_is_reset),
      NULL, &rsp_msg);
  PBM_CHECK_FILE_STATUS_SIZED_RETCODE(rsp_msg.write, sizeof(uid_is_reset), "Error writing PH UID file", nRet, PBM_ERROR);

  fs_close(file_handle, NULL, &rsp_msg);
  if (rsp_msg.close.status != FS_OKAY_S) 
  {
    PBM_MSG_ERR("Error closing file, fs_close %d", rsp_msg.close.status, 0, 0);
  }
  file_handle = 0;

  return nRet;
}

/*===========================================================================
FUNCTION PBM_GET_PHONE_UNIQUE_ID_FROM_RECORD

DESCRIPTION
  Given a cache record, retrieves the PH UID value and places it in the
  ph_unique_id parameter.

  Returns PBM_ERROR if not found or the record has an invalid entry.

DEPENDENCIES

SIDE EFFECTS

COMMENTS

SEE ALSO
===========================================================================*/

pbm_return_type pbm_get_phone_unique_id_from_record(const pbm_addr_cache_s_type *record,
   uint64 *ph_unique_id)
{
  int index;
  void *field_ptr;

  PBM_CHECK_PTR_RET(record, PBM_ERROR);
  PBM_CHECK_PTR_RET(ph_unique_id, PBM_ERROR);

  if (pbm_find_phone_uid_field(record->fields, record->num_fields, &index) == FALSE) 
  {
    return PBM_ERROR;
  }

  if (record->fields[index].data_len != PBM_DDWORD_SIZE) 
  {
    PBM_MSG_ERR("Found invalid PH UID field size at record %d", record->record_id, 0, 0);
    return PBM_ERROR;
  }

  field_ptr = pbm_get_field_data_ptr(record->fields,index);
  PBM_CHECK_PTR_RET(field_ptr, PBM_ERROR);

  memcpy((void *)ph_unique_id, field_ptr, PBM_DDWORD_SIZE);

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_RENUMBER_WITH_UNIQUE_ID

DESCRIPTION
   Renumbers all EFS phonebook records with new PH_UID fields. Creates a new
   PH_UID file with the 'is_reset' flag set to TRUE.

DEPENDENCIES

SIDE EFFECTS
   If successful, g_ph_unique_id is set with the last new unique ID after renumbering.
   g_ph_uid_is_reset flag is set to TRUE.  The PH UID file is re-created with the last 
   new unique ID.

COMMENTS

SEE ALSO
===========================================================================*/

static pbm_return_type pbm_renumber_with_unique_id(void)
{
  pbm_return_type nRet = PBM_SUCCESS;
  int i;
  int index;
  pbm_addr_cache_s_type *record;
  pbm_addr_cache_s_type old_rec;
  uint16 location;
  fs_handle_type new_file_handle = 0;
  fs_handle_type old_file_handle = 0;
  fs_open_xparms_type attribs;
  fs_rsp_msg_type rsp_msg;
  uint64 ph_unique_id = 0;

  /* If there are no records in PBM_EFS then just write out the PH UID file. */

  if (!efs_cache.records_in_use) 
  {
    nRet = pbm_create_initial_ph_uid_file(ph_unique_id, TRUE);
    g_ph_unique_id = 0;
    g_ph_uid_is_reset = TRUE;
    return nRet;
  }

  /* Renumber the EFS cache first. */

  for (i = 0; i < efs_cache.num_of_records; i++) 
  {
    /* Skip empty records. */
    if ( (efs_cache.pb_cache_array[i] == NULL) ||
      (efs_cache.pb_cache_array[i]->fields == NULL) ||
      (efs_cache.pb_cache_array[i]->num_fields == 0) ) 
    {
      continue;
    }

    record = efs_cache.pb_cache_array[i];

    if (pbm_find_phone_uid_field(record->fields,
      record->num_fields, &index) == FALSE) 
    {
      /* The given record does not have the PH UID field. Tag it. */

      ph_unique_id++;
         
      nRet = pbm_append_phone_uid_field(ph_unique_id,
         &record->fields, &record->num_fields);
      if (nRet != PBM_SUCCESS) 
      {
         break;
      }
    }
    else 
    {
      if ( (record->fields[index].data_len != PBM_DDWORD_SIZE) ) 
      {
        /* We have a record with a PH_UID field that has an invalid
         * length. For now, just set that field type to PBM_FIELD_NONE,
         * and add a good UID field. This shouldn't happen, unless the fields
         * get renumbered. */

        PBM_MSG_ERR("Found invalid PH UID field size at record %d",
          record->record_id, 0, 0);

        record->fields[index].field_id = PBM_FIELD_NONE;

        ph_unique_id++;

        nRet = pbm_append_phone_uid_field(ph_unique_id,
          &record->fields, &record->num_fields);

        if (nRet != PBM_SUCCESS)

        {
          break;
        }
      }
      else  /* size is correct */
      {
        void *data_ptr;

        ph_unique_id++;

        data_ptr = pbm_get_field_data_ptr(record->fields,index);

        PBM_CHECK_PTR_BREAK(data_ptr, nRet, PBM_ERROR);
        memcpy(data_ptr, (void *)&ph_unique_id, sizeof(ph_unique_id));
      }
    }
  }

  /* Bail out if we encountered an error during cache renumbering. */
  if (nRet != PBM_SUCCESS) 
  {
    return nRet;
  }

  /* Write the EFS cache out to the filesystem. */
  do 
  {
    /* Check if "new" master file exists, remove it if so. */
    pbm_kick_dog();
    fs_nametest(g_new_master_filename, FS_TEST_FILE, NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_RETCODE_BREAK(rsp_msg.nametest, "Error checking if New Master File exists", nRet, PBM_ERROR);

    if (rsp_msg.nametest.name_found) 
    {
      /* Should not occur since garbage collection already complete during initialization. */

      fs_remove(g_new_master_filename, NULL, &rsp_msg);
      PBM_CHECK_FILE_STATUS_RETCODE_BREAK(rsp_msg.rmfile, "Error deleting New Master File", nRet, PBM_ERROR);
    }

    /* Open "old" Master File*/
    fs_open(g_master_filename, FS_OA_READONLY, NULL, NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_RETCODE_BREAK(rsp_msg.open, "Error opening old Master File", nRet, PBM_ERROR);

    old_file_handle = rsp_msg.open.handle;

    /* Create "new" Master File*/
    attribs.create.cleanup_option = FS_OC_REVERT;
    attribs.create.buffering_option = FS_OB_PROHIBIT;
    attribs.create.attribute_mask = FS_FA_UNRESTRICTED;
    fs_open(g_new_master_filename, FS_OA_CREATE, &attribs, NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_RETCODE_BREAK(rsp_msg.open, "Error creating new Master File", nRet, PBM_ERROR);

    new_file_handle = rsp_msg.open.handle;

    while (PBM_SUCCESS == (nRet = pbm_efs_read(old_file_handle, &old_rec)))
    {
      /* Simply copy over any records that do not fall into the PBM_EFS phonebook. */
      if (pbm_record_id_to_phonebook(old_rec.record_id) != PBM_EFS) 
      {
        nRet = pbm_efs_add(new_file_handle, &old_rec, FALSE);
        if (nRet != PBM_SUCCESS) 
        {
          PBM_MSG_ERR("Error 0x%x Writing to new file.", nRet, 0, 0);
          pbm_free_fields_internal(&old_rec.fields, old_rec.num_fields);
          break;
        }
      }
      else
      {
        /* Use the updated efs_cache version of any records that are in PBM_EFS. */

        location = pbm_record_id_to_location(old_rec.record_id);
        if ((location > 0) &&
            (location <= efs_cache.num_of_records)) 
        {
          record = efs_cache.pb_cache_array[location];

          nRet = pbm_efs_add(new_file_handle, record, FALSE);
          if (nRet != PBM_SUCCESS) {
            PBM_MSG_ERR("Error 0x%x Writing to new file.", nRet, 0, 0);
            pbm_free_fields_internal(&old_rec.fields, old_rec.num_fields);
            break;
          }
        }
      }
         
      pbm_free_fields_internal(&old_rec.fields, old_rec.num_fields);
    }

    if (nRet != PBM_EOF) 
    {
      break;
    }

    fs_close(new_file_handle, NULL, &rsp_msg);
    if (rsp_msg.close.status != FS_OKAY_S) 
    {
      PBM_MSG_ERR("Error closing new master file, fs_close %d",
        rsp_msg.close.status, 0, 0);
    }
    new_file_handle = 0;

    fs_close(old_file_handle, NULL, &rsp_msg);
    if (rsp_msg.close.status != FS_OKAY_S) 
    {
      PBM_MSG_ERR("Error closing oldmaster file, fs_close %d",
         rsp_msg.close.status, 0, 0);
    }
    old_file_handle = 0;

    /* Copy new master file to the actual master file */
    fs_remove(g_master_filename, NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_RETCODE_BREAK(rsp_msg.rmfile, "Error deleting master file", nRet, PBM_ERROR);

    fs_rename(g_new_master_filename, g_master_filename, NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_RETCODE_BREAK(rsp_msg.rmfile, "Error renaming new master file to curr", nRet, PBM_ERROR);

    /* Create the PH UID file with is_reset flag set. */
    nRet = pbm_create_initial_ph_uid_file(ph_unique_id, TRUE);
    if (nRet != PBM_SUCCESS) 
    {
      break;
    }

    /* Initialization successful. Set the memory values. */
    g_ph_unique_id = ph_unique_id;
    g_ph_uid_is_reset = TRUE;
  }
  while (FALSE); /*lint !e717 */

  if (new_file_handle) 
  {
    fs_close(new_file_handle, NULL, &rsp_msg);
    if (rsp_msg.close.status != FS_OKAY_S) 
    {
      PBM_MSG_ERR("Error closing file, fs_close %d", rsp_msg.close.status, 0, 0);
    }
    new_file_handle = 0;
  }

  if (old_file_handle) {
    fs_close(old_file_handle, NULL, &rsp_msg);
    if (rsp_msg.close.status != FS_OKAY_S) 
    {
      PBM_MSG_ERR("Error closing file, fs_close %d", rsp_msg.close.status, 0, 0);
    }
    old_file_handle = 0;
  }

  return nRet;
}

/*===========================================================================
FUNCTION PBM_HAVE_UIDS_CHANGED

DESCRIPTION
  This function is used to see if the phone Unique ID's have changed.

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  pbm_validate_uid_changes
===========================================================================*/
boolean pbm_have_uids_changed()
{
  return g_ph_uid_is_reset;
}

/*===========================================================================
FUNCTION PBM_VALIDATE_UID_CHANGES

DESCRIPTION
  This function is used to acknowledge that the unique IDs have changed.

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  pbm_have_uids_changed
===========================================================================*/
pbm_return_type pbm_validate_uid_changes()
{
  pbm_return_type nRet = PBM_SUCCESS;
  fs_handle_type file_handle = 0;
  fs_rsp_msg_type rsp_msg;

  rex_enter_crit_sect(&pbm_crit_sect);

  if (!g_ph_uid_is_reset) 
  {
    rex_leave_crit_sect(&pbm_crit_sect);
    return PBM_SUCCESS;
  }

  do 
  {
    boolean val = FALSE;

    fs_open(g_ph_uid_filename, FS_OA_READWRITE, NULL, NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_RETCODE_BREAK(rsp_msg.open,"Cant open PH UID file",nRet, PBM_ERROR);

    file_handle = rsp_msg.open.handle;

    fs_seek(file_handle, FS_SEEK_SET, sizeof(uint64), NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_RETCODE_BREAK(rsp_msg.seek,"Cant seek PH UID file",nRet, PBM_ERROR);

    fs_write(file_handle, (void *)&val, sizeof(val), NULL, &rsp_msg);
    PBM_CHECK_FILE_STATUS_SIZED_RETCODE_BREAK(rsp_msg.write, sizeof(val), "Error reading PH UID file", nRet, PBM_ERROR);
  }
  while (FALSE); /*lint !e717 */

  if (file_handle)
  {
    fs_close(file_handle, NULL, &rsp_msg);
    if (rsp_msg.close.status != FS_OKAY_S) 
    {
      PBM_MSG_ERR("Could not close PH UID file, fs_close %d",
         rsp_msg.close.status, 0, 0);
    }
    file_handle = 0;
  }

  if (nRet == PBM_SUCCESS) 
  {
    g_ph_uid_is_reset = FALSE;
  }
   
  rex_leave_crit_sect(&pbm_crit_sect);

  /* send notification */
  if (nRet == PBM_SUCCESS) 
  {
    pbm_notify_data_s_type data;

    data.event = PBM_EVENT_PH_UNIQUE_IDS_VALIDATED;
    pbm_notify(&data);
  }

  return nRet;
}

#endif /* FEATURE_PBM_TASK */
