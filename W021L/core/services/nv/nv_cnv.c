/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N    C N V   T A S K

GENERAL DESCRIPTION
This task's main functionality is to back up the contents of NV into a CNV
format file and store in the file system. This file would later be used by NV
module to restore the contents of NV after a rebuild. The CNV file can also be
loaded as part of a CEFS image in the factory to load the NV items in an
efficient manner. After the restore, the file shall be unlinked.

LIMITATIONS
As the number of NV items increase, backup and restoration would take longer.
If a customer has added NV items beyond NV_MAX_I and used different tables
to get the offset into EFS files that are used for NV, then the backup logic
needs to be modified to include those NV items.

Copyright (c) 2005 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/nv1/nv_cnv.c#1 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/06/05   ck      Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "target.h"
#include "rex.h"
#include "task.h"
#include "tmc.h"
#include "dog.h"
#include "msg.h"
#include "err.h"
#include "queue.h"
#include "nv.h"
#include "nvim.h"
#include "nv_cnv.h"
#include "nv_cnvdiag.h"
#include "fs_public.h"
#include "crc.h"

#ifdef FEATURE_NV_CNV

/* <EJECT> */
/*===========================================================================
             DEFINITIONS AND DATA DECLARATIONS FOR MODULE

      This section contains local definitions for constants, macros, types,
      variables and other items needed by this module.
===========================================================================*/

/* Write buffer size */
#define CNV_BLK_SIZE 1024

/* Buffers to be placed on the auth_free_q */
#define CNV_NUM_CMD_BUFS 3

/* File to back up the data into */
extern const char   *cnv_file;

/* Timer definitions */
rex_timer_type cnv_rpt_timer;  /* Watchdog timer */

/* Command pool */
cnv_cmd_type cnv_cmd_pool[ CNV_NUM_CMD_BUFS ];

/* Queue to hold commands for cnv */
q_type       cnv_cmd_q;

/* Queue to hold free buffers to be placed on the cnv_cmd_q */
q_type       cnv_cmd_free_q;

/* Buffer to hold report to be returned to task issuing command */
cnv_rpt_type cnv_rpt_buf;

/* Buffer for command to NV task */
nv_cmd_type  cnv_nv_buf;

/* Buffer for data returned from NV task */
nv_item_type cnv_nv_data;

/* <EJECT> */
/*===========================================================================

FUNCTION CNV_WAIT

DESCRIPTION
  This function just performs a REX wait as specified by the
  caller, except that it automatically kicks the watchdog timer every
  so often while waiting. It also acknowledges the TASK_STOP_SIG and
  TASK_OFFLINE_SIG when they are set.

DEPENDENCIES
  Relies on cnv_RPT_TIMER_SIG being raised often enough to
  avoid watchdog alarms, so should not be called until cnv_init has
  run.

RETURN VALUE
  Returns the value returned by rex_wait.

SIDE EFFECTS
  The watchdog gets kicked periodically.

===========================================================================*/

rex_sigs_type 
cnv_wait (
  /* Mask of signals to wait for */
  word  sigs
)
{
  /* Return value from REX: bitmap of signals found raised */
  rex_sigs_type rex_signals_mask;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Keep waiting and kicking the watchdog as long as the return
     value has the kick timer signal set and none of the user's
     desired signals are set. Also process offline or stop procedure
     if TASK_OFFLINE_SIG or TASK_STOP_SIG is set*/

  do {
    /* Wait for the caller's conditions OR our kick timer */
    rex_signals_mask = rex_wait ( sigs | CNV_RPT_TIMER_SIG | TASK_STOP_SIG |
                                  TASK_OFFLINE_SIG );

    if ((rex_signals_mask & TASK_STOP_SIG) != 0) {
      MSG_MED( "TASK_STOP_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &cnv_tcb, TASK_STOP_SIG );
      /* Process task stop procedure. */
      task_stop();
    }
    else if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0) {
      MSG_MED( "TASK_OFFLINE_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &cnv_tcb, TASK_OFFLINE_SIG );
      /* Process task offline procedure. */
      task_offline();
    }
    else if ((rex_signals_mask & CNV_RPT_TIMER_SIG) != 0) {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      dog_report(DOG_CNV_RPT);
      (void) rex_set_timer( &cnv_rpt_timer, DOG_CNV_RPT_TIME );
    }
  } while ( (rex_signals_mask & sigs ) == 0x0000 );
  /* Keep looping as long as user's conditions are un-met */

  /* Return the rex_wait return value with user condition(s) set */
  return ( rex_signals_mask );

} /* cnv_wait */

/*===========================================================================

FUNCTION CNV_NV_READ

DESCRIPTION
  This procedure encapsulates an NV read operation for CNV task.

DEPENDENCIES
  Assumes that cnv_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.

===========================================================================*/
nv_stat_enum_type 
cnv_nv_read (
  nv_items_enum_type item              /* Which NV item to read */
)
{
  /* Fill in necessary fields of command block */
  cnv_nv_buf.done_q_ptr = NULL;
  cnv_nv_buf.tcb_ptr = &cnv_tcb;
  cnv_nv_buf.sigs = CNV_NV_CMD_SIG;

  /* Initialize the command fields */
  cnv_nv_buf.cmd        = NV_READ_F;     /* We will do reads        */
  cnv_nv_buf.data_ptr   = &cnv_nv_data;  /* Initialize data pointer */
  cnv_nv_buf.item       = item;

  /* Clear the signal */
  (void) rex_clr_sigs( &cnv_tcb, CNV_NV_CMD_SIG );

  /* Queue up the command */
  nv_cmd( &cnv_nv_buf );

  /* Wait for the response */
  (void) cnv_wait(CNV_NV_CMD_SIG );

  /* Clear the signal */
  (void) rex_clr_sigs( &cnv_tcb, CNV_NV_CMD_SIG );

  /* Return status */
  return ( cnv_nv_buf.status );
} /* cnv_nv_read */

/*======================================================================
FUNCTION CNV_BACKUP_NV

DESCRIPTION
  This function backs up the contents of NV into a CNV format file.

DEPENDENCIES
  None

Return
 TRUE/FALSE based on SUCESS/FAILURE of back up.
======================================================================*/
boolean 
cnv_backup_nv(void)
{
  cnv_header_type header;
  cnv_item_hdr_type item;

  static byte cnv_file_buf[CNV_BLK_SIZE];
  byte cnv_pad_buf[3] = {0xFF,0xFF,0xFF};

  nv_stat_enum_type status;

  int file_handle = NULL;
  fs_off_t offset;
  fs_ssize_t size;
  uint16 j = 0;
  uint16 array_size = 0;
  uint16 item_id = 0;
  uint16 num_bytes = 0;
  uint32 total_size   = 0;
  nvim_item_size_type item_size = 0;
  uint8  pad_size   = 0;

  word count;
  word size_left;
  word crc16;

  /* Memset the buffer to all zeroes */
  memset(cnv_file_buf, 0, sizeof(cnv_file_buf));

  /* Open the CNV file */
  file_handle = efs_open (cnv_file, O_RDWR|O_CREAT|O_TRUNC, DEFFILEMODE);
  if(file_handle <= 0) {
    ERR("Could not open the CNV file !!",0,0,0);
    return FALSE;
  }
  /* Fill in the header information */
  memcpy( header.name, "CNV*", sizeof(header.name));
  header.version = 0x0001;
  header.pad = 0xFFFF;

  /* Copy header information into temp buffer and increment num_bytes */
  memcpy(cnv_file_buf + num_bytes, &header, sizeof(header));
  num_bytes += sizeof(header);

  /* Iterate for all items */
  for (item_id = 0; item_id < NV_MAX_I; item_id++) {
    /* Some items need special handling */
    switch ((nv_items_enum_type)item_id) {
      /* Items that are listed separately require special handling */
      case NV_SMS_I:
        item_size = sizeof(nv_sms_type);
        array_size = NVI_MAX_SMS_ADDR;
        break;

      case NV_SMS_GW_I:
        item_size = sizeof(nv_sms_gw_type);
        array_size = NVI_MAX_SMS_ADDR;
        break;

      case NV_RENTAL_TIMER_I:
        item_size = sizeof(nv_rental_timer_type);
        array_size = 0;
        break;

      case NV_RENTAL_CNT_I:
        item_size = sizeof(dword);
        array_size = 0;
        break;

      case NV_VERNO_MAJ_I:
      case NV_VERNO_MIN_I:
        item_size = nvim_op_get_size((nv_items_enum_type)item_id);
        break;

      default:
        /* If the entry is not valid, then continue to the next item */
        if (!(nvim_op_get_presence(item_id))) {
          MSG_MED ("NV Item %d is not valid",item_id,0,0);
          continue;
        }

        /* get the array size of the item */
        array_size = nvim_op_get_array_size((nv_items_enum_type)item_id);

        /* get the size of the item */
        item_size = nvim_op_get_size((nv_items_enum_type)item_id);

        break;
      } /* end of switch */

    /* Update the structure */
    item.nv_item_size = item_size;
    item.nv_item_number = item_id;

    j = 0;

    do {
      /* Set command specific information */
      switch ((nv_items_enum_type)item_id) {
        case NV_SMS_I:
          cnv_nv_data.sms.address = j;
          break;

        case NV_SMS_GW_I:
          cnv_nv_data.sms_gw.address = j;
          cnv_nv_data.sms_gw.length = NV_SMS_GW_DATA_SIZ;
          break;

        default:
          /* Set the array index as the first byte of data */
          *((byte *)(&cnv_nv_buf)->data_ptr) = j;
          break;
      }
      
      status = cnv_nv_read((nv_items_enum_type)item_id);

      MSG_MED("Finished read of item %d",item_id, 0, 0);

      /* Proceed to back up only if the status is DONE */
      if (status != NV_DONE_S) {
        continue;
      }

      /* Find the number of pad bytes */
      if ((item_size%4) != 0) {
        pad_size = 4 - (item_size % 4);
      }
      else {
        pad_size = 0;
      }

      /* Check to see if there is space for item to be written in present blk.*/
      if( (num_bytes + item_size + sizeof(item) + pad_size) <= CNV_BLK_SIZE) {
        memcpy(cnv_file_buf + num_bytes, &item, sizeof(item));

        num_bytes += sizeof(item);

        /* Copy the contents of data_ptr to the cnv_file_buf */
        memcpy(cnv_file_buf + num_bytes, (void*) cnv_nv_buf.data_ptr,
               item_size);

        /* Increment the number of bytes */
        num_bytes += item_size;

        /* Pad the item up if the item size is not a multiple of 4 */
        if (pad_size != 0) {
          memcpy(cnv_file_buf + num_bytes, &cnv_pad_buf, pad_size);
          num_bytes += pad_size;
        }
      }
      else {
        /* Write the file to EFS */
        size = efs_write (file_handle, cnv_file_buf, num_bytes);
        if (size !=  num_bytes) {
          MSG_HIGH("Write to CNV file failed",0,0,0);
          return (FALSE);
        }

        /* Add to the total size */
        total_size += num_bytes;

        /* Memset the buffer to all zeroes */
        memset(cnv_file_buf, 0, sizeof(cnv_file_buf));

        /* Clear the offset */
        num_bytes = 0;

        memcpy(cnv_file_buf + num_bytes, &item, sizeof(item));

        num_bytes += sizeof(item);

        /* Copy the contents of data_ptr to the cnv_file_buf */
        memcpy(cnv_file_buf + num_bytes, (void*) cnv_nv_buf.data_ptr,
               item_size);

        /* Increment the offset */
        num_bytes += item_size;

        /* Pad the item up if the item size is not a multiple of 4 */
        if (pad_size != 0) {
          memcpy(cnv_file_buf + num_bytes, &cnv_pad_buf, pad_size);
          num_bytes += pad_size;
        }
      }

      if ((num_bytes % 4) != 0) {
        ERR_FATAL ("Invalid ISize %d PSize %d for item %d ", item_size,
                    pad_size, item_id);
      }
    } while (++j < array_size);
  } /* each item */

  /* Write the accumulated data in the buffer to the file */
  size = efs_write (file_handle, cnv_file_buf, num_bytes);
  if (size != num_bytes) {
    MSG_HIGH("Write to CNV file failed",0,0,0);
    return (FALSE);
  }

  /* Add to the total number of bytes written */
  total_size += num_bytes;

  /* Fill the last item with FF FF and 2 bytes for data which will the CRC */

  item.nv_item_size = 2;
  item.nv_item_number = 0xFFFF;

  /* Write the last item to the file */
  size = efs_write (file_handle, &item, sizeof(item));
  if (size != sizeof(item)) {
    MSG_HIGH("Write to CNV file failed",0,0,0);
    return (FALSE);
  }

  /* Add to the total size */
  total_size += sizeof(item);

  /* Rewind to the beginning of the file to do CRC */
  offset = efs_lseek (file_handle, 0, SEEK_SET);

  if (offset < 0) {
    MSG_HIGH ("Cannot seek to the beginning of the file",0,0,0);
    return (FALSE);
  }

  /* Clear the contents of the buffer  - not necessary */
  memset(cnv_file_buf, 0, sizeof(cnv_file_buf));

  /* Now cycle through the contents of the file to compute CRC */
  crc16 = (word) CRC_16_STEP_SEED;
  size_left = total_size;
  while (size_left > 0) {
    count = size_left;
    if (count > sizeof (cnv_file_buf))
      count = sizeof (cnv_file_buf);

    size = efs_read(file_handle, cnv_file_buf, count);
    if (size != count) {
      MSG_HIGH ("Unable to read CNV data", 0, 0, 0);
      return (FALSE);
    }

    crc16 = crc_16_step (crc16, cnv_file_buf, count);
    size_left -= count;
  }

  /* Seek to the end of the file to write the CRC */
  offset = efs_lseek (file_handle, 0, SEEK_END);

  if (offset < 0) {
    MSG_HIGH ("Cannot seek to the end of the file",0,0,0);
    return (FALSE);
  }

  /* Change the byte ordering for CRC */
  FLOPW(cnv_file_buf, crc16);

  /* Write CRC as last two bytes of the file */
  size = efs_write (file_handle, cnv_file_buf, sizeof(crc16));
  if (size != sizeof(crc16)) {
    MSG_HIGH("Write to CNV file failed",0,0,0);
    return (FALSE);
  }

  efs_close(file_handle);
  return TRUE;
} /* cnv_backup_nv */

/* <EJECT> */
/*===========================================================================

FUNCTION CNV_PROCESS_COMMAND

DESCRIPTION
  This procedure processes a command sent to the CNV task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void 
cnv_process_command (
  cnv_cmd_type *cmd_ptr
    /* Pointer to received command */
)
{
  boolean status = FALSE;

  switch(cmd_ptr->hdr.command) {
    case CNV_BACKUP_NV_F:

      MSG_HIGH( "Starting backup of NV", 0, 0, 0 );
      status = cnv_backup_nv();
      MSG_HIGH( "Ending backup of NV", 0, 0, 0 );

      /*  Report back the result */
      if (cmd_ptr->backup.hdr.rpt_function != NULL) {
        cnv_rpt_buf.rpt_type = CNV_BACKUP_NV_R;
        /* Copy the user data */
        cnv_rpt_buf.user_data = cmd_ptr->hdr.user_data;

        /* Set the status */
        cnv_rpt_buf.rpt.status = status;

        cnv_rpt_buf.rpt_hdr.task_ptr = NULL;
        /* Queue report for requesting task */
        (*(cmd_ptr->backup.hdr.rpt_function))( &cnv_rpt_buf );
        MSG_MED( "Reporting result of backup", 0, 0, 0 );
      }

    break;

    default:
      ERR("Invalid command received by CNV",0,0,0);
    break;
  }
} /* cnv_process_command */

/* <EJECT> */
/*===========================================================================

FUNCTION CNV_INIT

DESCRIPTION
  This procedure initializes the queues and timers for CNV Task.
  It should be called only once, at powerup time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cnv_init( void )
{
  byte i; /* Index into free buffer pool */

/*- - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------
  ** Initialize timers
  ** ----------------- */
  rex_def_timer( &cnv_rpt_timer, &cnv_tcb, CNV_RPT_TIMER_SIG );

  /* -----------------
  ** Initialize queues
  ** ----------------- */
  (void) q_init( &cnv_cmd_q );
  (void) q_init( &cnv_cmd_free_q );

  /* Fill cnv_free_q */
  for (i = 0; i < CNV_NUM_CMD_BUFS; i++) {
    cnv_cmd_pool[i].hdr.cmd_hdr.done_q_ptr = &cnv_cmd_free_q;
    q_put( &cnv_cmd_free_q, q_link( &cnv_cmd_pool[i],
           &cnv_cmd_pool[i].hdr.cmd_hdr.link ) );
  }

  /* Initialize the NV data pointer in the command buffer */
  cnv_nv_buf.data_ptr = &cnv_nv_data;

  /* Register the DIAG dispatch table */
  nv_cnvdiag_init();

} /* cnv_init() */

/*===========================================================================

FUNCTION CNV_TASK

DESCRIPTION
  This procedure is the entrance procedure for the CNV task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/

void 
cnv_task (
  dword dummy
    /* Parameter required for REX.  Tell lint to ignore it. */
    /*lint -esym(715,dummy) */
)
{
  rex_sigs_type rex_signals_mask;  /* Mask of signals returned by rex */
  cnv_cmd_type *cmd_ptr;            /* Pointer to received command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------
  ** Perform initialization
  ** ---------------------- */
  cnv_init();

  /* ------------------------------------------------
  ** Process task start procedure.
  ** ------------------------------------------------ */
  task_start(
    CNV_RPT_TIMER_SIG,             /* report timer signal for task */
    DOG_CNV_RPT,                   /* watchdog report */
    &cnv_rpt_timer                 /* pointer to report timer */
  );

  /* ------------------------------------------------------------------
  ** Initially kick watchdog and set timer for watchdog report interval
  ** ------------------------------------------------------------------ */

  CNV_DOG_RPT();

  /* Searcher guarantees implicitly that it will fill the buffer as soon
     as it acquires the paging channel. Hence we are guaranteed that
     cnv will get the command signal long before we enter the traffic
     channel */

  for (;;) {
    /* Never exit this loop... */

    /* If there is already a command on our command queue,  don't wait. */
    if (q_cnt( &cnv_cmd_q ) != 0) {
      rex_signals_mask = CNV_CMD_Q_SIG;
    }
    else {
      rex_signals_mask =
        rex_wait( CNV_RPT_TIMER_SIG | CNV_CMD_Q_SIG | TASK_STOP_SIG |
                  TASK_OFFLINE_SIG );
    }

    if ((rex_signals_mask & CNV_RPT_TIMER_SIG) != 0) {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      dog_report(DOG_CNV_RPT);
      (void) rex_set_timer( &cnv_rpt_timer, DOG_CNV_RPT_TIME );
    }

    /* -------------------------------------------------------
    ** Check if powerdown command signal was set.  If set then
    ** clear signal, and process task stop procedure.
    ** ------------------------------------------------------- */
    if ((rex_signals_mask & TASK_STOP_SIG) != 0) {
      MSG_MED( "TASK_STOP_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &cnv_tcb, TASK_STOP_SIG );
      /* Process task stop procedure. */
      task_stop();
    }

    /* -----------------------------------------------------
    ** Check if offline command signal was set.  If set then
    ** clear signal, and process task offline procedure.
    ** ----------------------------------------------------- */
    if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0) {
      MSG_MED( "TASK_OFFLINE_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &cnv_tcb, TASK_OFFLINE_SIG );
      /* Process task offline procedure. */
      task_offline();
    }

    /*-----------------------------
    ** We have a command to process
    ** ---------------------------- */
    if ((rex_signals_mask & CNV_CMD_Q_SIG) != 0) {
      (void) rex_clr_sigs( &cnv_tcb, CNV_CMD_Q_SIG );
      if ((cmd_ptr = (cnv_cmd_type*) q_get( &cnv_cmd_q )) != NULL) {
        cnv_process_command( cmd_ptr );

        /* return buffer to free queue */
        if( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL ) {
          /* place command on requested queue */
          q_put( cmd_ptr->hdr.cmd_hdr.done_q_ptr, &cmd_ptr->hdr.cmd_hdr.link );
        }
        else {
          ERR("done_q_ptr was NULL!", 0, 0, 0);
        }
      }
    } /* end if ((rex_signals_mask & CNV_CMD_Q_SIG) != 0) */
  } /* end for (;;) */
} /* end cnv_task */

/*===========================================================================

FUNCTION CNV_CMD

DESCRIPTION
  The cnv_cmd pointed to by the parameter passed in is queued up for cnv.

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void 
cnv_cmd (
  cnv_cmd_type *cmd_ptr                    /* the command to queue up */
)
{
  /* -------------------------------------------------------
  ** All cnv commands should return to the free queue.  Set
  ** this field again here to ensure no buffers get lost.
  ** ------------------------------------------------------- */
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = &cnv_cmd_free_q;

    /* init link */
  ( void )q_link( cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link );

  q_put( &cnv_cmd_q, &cmd_ptr->hdr.cmd_hdr.link ); /* and queue it */

  (void)rex_set_sigs( &cnv_tcb, CNV_CMD_Q_SIG );  /* signal the cnv task */
} /* cnv_cmd */

#endif /* FEATURE_NV_CNV */
