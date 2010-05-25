/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   M D S P   S E R V I C E S   M O D U L E
                         L O G    F U N C T I O N S
                     
GENERAL DESCRIPTION
  This module implements the logging control for the Modem DSP (MDSP)
  embedded Digital Signal Processor.

EXTERNALIZED FUNCTIONS
  mdsp_log_init
  mdsp_log_send_processing
  mdsp_log_save_processing
  mdsp_log_set_mode_processing
  mdsp_log_process_app_state_change
  mdsp_log_dsp_start
  mdsp_log_dsp_failure
  mdsp_memory_save_processing


INITIALIZATION AND SEQUENCING REQUIREMENTS
  mdsp_log_init must be called before any other log functions


Copyright(c) 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/services/mdspsvc/mdsplog.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/08   ah      Branched from MSMSHARED_MDSP_SVCS.00.01.25
06/19/06   gfr     Handle modules that have 0 sized log buffers.
06/19/06   ah      Fixed Lint errors.
03/16/06   gfr     Do not use MDSP_FIRMWARE_VERSION as it is not defined on
                   all firmware targets.
02/09/06   gfr     Initial revision.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"           /* Feature definitions header             */

#ifdef FEATURE_MDSPSVC_LOG

#include "comdef.h"             /* Definitions for byte, word, etc.       */
#include "msg.h"                /* Message logging/reporting services     */
#include "log.h"                /* Log packet generation services         */
#include "mdspext.h"            /* MDSP firmware images and access        */
#include "mdspi.h"              /* Internal MDSP Services definitions     */
#include "rex.h"                /* REX definitions for timers             */
#include "fs_public.h"          /* File system definitions                */
#include "task.h"               /* Task definitions and prototypes        */
#include "ts.h"                 /* Timestamp functions                    */
#include "nv.h"                 /* NV functions                           */
#include "clk.h"                /* Clock functions (clk_busy_wait)        */



/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Define this flag to display profiling information - how many chunks
   are read each timer expiry, how long it takes, and how many packets
   were dropped - lots of messages, but useful for fine-tuning the period
   for a new application. */
/* #define MDSP_LOG_PROFILING */

/* The size (in words) of the chunk header - length + sequence number */
#define MDSP_LOG_CHUNK_HDR_SIZE               2

/* The number of chunks to store in the circular buffer */
#define MDSP_LOG_NUM_CHUNKS_TO_SAVE          50

/* The number of chunks to send to diag for each request */
#define MDSP_LOG_NUM_CHUNKS_TO_SEND          10

/* Default period (in ms) to read chunks when active */
#define MDSP_LOG_DEFAULT_PERIOD              10

/* Maximum possible period - used to facilitate calculating the minimum */
#define MDSP_LOG_MAX_PERIOD              0xFFFF

/* Version of the saved log file - for future backwards compatibility */
#define MDSP_LOG_FILE_VERSION                 1

/* The filename to save the chunks to when requested */
#define MDSP_LOG_FILE_NAME           "mdsp_log"

/* Version of the saved memory file - for future backwards compatibility */
#define MDSP_MEMORY_FILE_VERSION              1

/* The filename to save the memory to when requested */
#define MDSP_MEMORY_FILE_NAME     "mdsp_memory"

/* Some firmware versions do not export these definitions, so use defaults */
#ifndef MDSP_MEMA_SIZE16
#define MDSP_MEMA_SIZE16 16384
#define MDSP_MEMB_SIZE16 16384
#define MDSP_MEMC_SIZE32 12288
#define MDSP_TEXT_SIZE32 39936
#endif

/* A fw bug means that MDSP_TEXT_SIZE32 is actually in 16-bit words.  We
   define a different value to fix this and maintain convention. */
#define MDSP_MEMI_SIZE32 (MDSP_TEXT_SIZE32 / 2)

/* This is the value programmed to timerPeriod - if non-zero then firmware
   will generate periodic timer log packets with MIPS usage. */
#define MDSP_LOG_DEFAULT_MDSP_TIMER_PERIOD  0x10000


/* Structure containing one chunk of mDSP log packets - max chunk size is
   exported by firmware and is normally 64 words.  Must be packed to allow
   inclusion in the log packet below. */
typedef PACKED struct
{
   uint16  length;
   uint16  sequence_number;
   uint16  packets[MDSP_LOG_MAX_CHUNK_SIZE - MDSP_LOG_CHUNK_HDR_SIZE];
} mdsp_log_chunk_type;


/* Pack up a set of chunks into a log packet */
LOG_RECORD_DEFINE(LOG_MDSP_LOG_CHUNKS_C)
   uint16              num_chunks;
   mdsp_log_chunk_type chunks[MDSP_LOG_NUM_CHUNKS_TO_SEND];
LOG_RECORD_END


/* Structure containing all local parameters */
typedef struct
{
   /* Whether or not we are actively polling for chunks */
   boolean             active;

   /* Current overall logging mode */
   mdsp_log_mode_type  mode;

   /* The default log mode - may be enabled through NV - TODO */
   mdsp_log_mode_type  default_mode;

   /* Array of saved chunks and index to next write location */
   mdsp_log_chunk_type chunks[MDSP_LOG_NUM_CHUNKS_TO_SAVE];
   uint16              chunks_index;

   /* Count of new chunks since last log packet was sent, avoids sending
      duplicate chunks if requests arrive close together. */
   uint32              new_chunks;

   /* Timer used to poll the log buffer for new chunks */
   rex_timer_type      timer;

   /* If we stop the timer because the active app goes to sleep we save the
      time left and start the timer at that value when we wake up.  This is to
      ensure that in short awake cycles we are processing the timer at the
      expected period. */
   uint16              time_remaining;

   /* The log period to use in milliseconds */
   uint16              period;

   /* Flag used to trigger printing an error if packets are dropped due to
      too large a period.  Generally used during parameter tuning only. */
   boolean             count_dropped_packets;

   /* Expected next packet sequence number, used for counting dropped pkts */
   uint16              next_sequence_number;

   /* Whether or not to save the log and memory when an mDSP error occurs */
   boolean             save_on_error;

   /* Flag set if we are attempting to recovery from a DSP failure */
   boolean             recovery_in_progress;

} mdsp_log_params_type;


/* All local parameters */
static mdsp_log_params_type mdsp_log;


/* Header in the stored to file log */
typedef PACKED struct
{
   uint16    version;        /* Version of the file - must be first! */
   uint16    chunk_size;     /* Chunk size from the mDSP */
   uint16    num_chunks;     /* How many chunks are in the log */
   uint16    index;          /* Current write index */
   uint16    app_id;         /* App that caused the log to be saved */
   uint32    tag;            /* Optional tag passed by the app */
   time_type timestamp;      /* Timestamp when log was written to disk */
} mdsp_log_file_hdr_type;

static mdsp_log_file_hdr_type mdsp_log_file_hdr = 
{
   MDSP_LOG_FILE_VERSION,
   MDSP_LOG_MAX_CHUNK_SIZE,
   MDSP_LOG_NUM_CHUNKS_TO_SAVE,
   0,
   0,
   0,
   {0, 0}
};


/* Header in the memory file stored to disk */
typedef PACKED struct
{
   uint16    version;        /* Version of the file - must be first! */
   uint16    fw_version;     /* Firmware version */
   uint32    mem_addr[4];    /* ARM addresses of the A/B/C/I memory segments */
   uint16    mem_size[4];    /* Sizes in words of the memory segments */
   uint16    app_id;         /* App that caused the memory to be saved */
   uint32    tag;            /* Optional tag passed by the app */
   time_type timestamp;      /* Timestamp when memory was written to disk */
} mdsp_memory_file_hdr_type;

static mdsp_memory_file_hdr_type mdsp_memory_file_hdr = 
{
   MDSP_MEMORY_FILE_VERSION,
   0,           /* fw_version */
   0, 0, 0, 0,  /* mem_addr */
   MDSP_MEMA_SIZE16,
   MDSP_MEMB_SIZE16,
   MDSP_MEMC_SIZE32,
   MDSP_MEMI_SIZE32,
   0,           /* app_id */
   0,           /* tag */
   0, 0         /* timestamp */
};




/*===========================================================================

                             EXTERNAL REFERENCES

===========================================================================*/

extern uint16 mdsp_clock_mask;



/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/

static void   mdsp_log_determine_mode (void);
static void   mdsp_log_send_chunks (uint32 num_chunks);
static uint32 mdsp_log_read_chunks (void);



/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION mdsp_send_log_event

DESCRIPTION
  Sends a MDSP_EV_COMMAND event to the specified application.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mdsp_send_log_event
(
   mdsp_app_type        app_id,  /* Application ID */
   mdsp_log_status_type status   /* Log save status */
)
{
   mdsp_ev_log_type log_event;  /* event to send to app */
  
   /*---------------------------------------------------------------*/

   if ((mdsp_app_table[app_id].cb_ptr != NULL) &&
       ((mdsp_app_table[app_id].event_mask & (uint16)MDSP_EV_LOG) != 0))
   {
      log_event.event      = MDSP_EV_LOG;
      log_event.app_id     = app_id;
      log_event.log_status = status;

      (mdsp_app_table[app_id].cb_ptr)((mdsp_event_data_type*)&log_event);
   }

} /* mdsp_send_log_event */



/*===========================================================================

FUNCTION mdsp_log_set_mode_processing

DESCRIPTION
  Sets the logging mode for the given application.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_set_mode_processing
(
   mdsp_packet_type *packet  /* command packet to process */
)
{
   mdsp_log_mode_type log_mode;  /* shortcut to log mode */
   mdsp_app_type      app_id;    /* shortcut to app id */
   
   /*---------------------------------------------------------------*/
   
   /* Get the desired new log mode */
   log_mode = packet->log_set_mode_cmd.log_mode;
   app_id   = packet->log_set_mode_cmd.app_id;
   
   /* Validate parameters */
   if (log_mode >= MDSP_LOG_MAX)
   {
      MSG_ERROR("Invalid mDSP log mode: %d", log_mode, 0, 0);
      mdsp_send_cmd_event(app_id, MDSP_CMD_FAIL); 
      return;
   }

   /* Set the log mode if it changed */
   if (mdsp_app_table[app_id].log_mode != log_mode)
   {
      MSG_HIGH("mDSP log mode: app=%d, %d->%d", app_id,
               mdsp_app_table[app_id].log_mode, log_mode);
      mdsp_app_table[app_id].log_mode   = log_mode;
      mdsp_app_table[app_id].log_period = packet->log_set_mode_cmd.log_period;

      /* Redetermine the log mode */
      mdsp_log_determine_mode();
   }

   /* Send response */
   mdsp_send_cmd_event(packet->log_set_mode_cmd.app_id, MDSP_CMD_ACCEPT); 

} /* mdsp_log_set_mode_processing */



/*===========================================================================

FUNCTION mdsp_log_send_processing

DESCRIPTION
  Requests a log packet be sent to diag.  If logging is not enabled then
  this will do nothing.  Will first read out any pending chunks to ensure
  that the log packet contains the most recent information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_send_processing
(
   mdsp_packet_type *packet  /* command packet to process */
)
{
   mdsp_app_type app_id;  /* shortcut to app id */
   
   /*---------------------------------------------------------------*/

   app_id = packet->log_send_cmd.app_id;

   /* If this app is enabled we can flush any pending chunks to make sure
      the log packet is up-to-date */
   if (mdsp_app_table[app_id].state == MDSP_APP_STATE_ENABLED)
   {
      (void) mdsp_log_read_chunks();
   }

   /* Fire off the log packet now */
   mdsp_log_send_chunks(MDSP_LOG_NUM_CHUNKS_TO_SEND);

   /* Send response */
   mdsp_send_cmd_event(app_id, MDSP_CMD_ACCEPT);

} /* mdsp_log_send_processing */



/*===========================================================================

FUNCTION mdsp_log_save_processing

DESCRIPTION
  Requests that the current set of chunks be saved to EFS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_save_processing
(
   mdsp_packet_type *packet  /* command packet to process */
)
{
   mdsp_app_type app_id;    /* application id */
   int           fd;        /* file descriptor */
   uint32        tag;       /* tag shortcut */
   time_type     ts;        /* timestamp */

   /*---------------------------------------------------------------*/

   app_id = packet->log_save_cmd.app_id;
   tag    = packet->log_save_cmd.tag;
   
   /* Send command accept before starting the save */
   mdsp_send_cmd_event(app_id, MDSP_CMD_ACCEPT);

   /* If this app is enabled we can flush any pending chunks to make sure
      the log is up-to-date */
   if (app_id == MDSP_APP_SELF ||
       mdsp_app_table[app_id].state == MDSP_APP_STATE_ENABLED)
   {
      (void) mdsp_log_read_chunks();
   }
 
   /* Open the file */
   fd = efs_open(MDSP_LOG_FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0755);
   if (fd < 0)
   {
      MSG_ERROR("Unable to open log file", 0, 0, 0);
      mdsp_send_log_event(app_id, MDSP_LOG_SAVE_FAILED);
      return;
   }

   /* Fill in and write the header */
   mdsp_log_file_hdr.index  = mdsp_log.chunks_index;
   mdsp_log_file_hdr.app_id = (uint16)app_id;
   mdsp_log_file_hdr.tag    = tag;
   ts_get(ts);
   memcpy((void *)mdsp_log_file_hdr.timestamp, ts, sizeof(time_type));
   (void) efs_write(fd, (void *)&mdsp_log_file_hdr, sizeof(mdsp_log_file_hdr));

   /* Write the chunks buffer - note that EFS writes at most 8K for each
      call to efs_write.  This allows a chunks log of up to ~120 chunks
      before having to start looping on this call. */
   (void) efs_write(fd, (void *)&mdsp_log.chunks[0], sizeof(mdsp_log.chunks));

   /* Close the file */
   (void) efs_close(fd);

   /* Send response */
   mdsp_send_log_event(app_id, MDSP_LOG_SAVE_COMPLETE);

   MSG_HIGH("Saved mDSP chunks: app=%d, tag=%d", app_id, tag, 0);

} /* mdsp_log_save_processing */



/*===========================================================================

FUNCTION mdsp_log_activate

DESCRIPTION
  Activates logging by setting state and starting the log timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mdsp_log_activate (void)
{
   /* Nothing to do if we are already active */
   if (mdsp_log.active)
   {
      return;
   }
   
   /* Set the state */
   mdsp_log.active = TRUE;

   /* Start the timer to begin reading chunks */
   (void) rex_set_timer(&mdsp_log.timer, mdsp_log.time_remaining);
   
} /* mdsp_log_activate */



/*===========================================================================

FUNCTION mdsp_log_inactivate

DESCRIPTION
  Deactivates logging by setting state and stopping the log timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mdsp_log_inactivate (void)
{
   /* Nothing to do if we are already inactive */
   if (!mdsp_log.active)
   {
      return;
   }
   
   /* Set the state */
   mdsp_log.active = FALSE;

   /* Clear out the timer - if we get 0 back it means the timer must have
      already expired, so clear the signals from our TCB as well */
   mdsp_log.time_remaining = (uint16)rex_clr_timer(&mdsp_log.timer);
   if (mdsp_log.time_remaining == 0)
   {
      (void) rex_clr_sigs(&mdsp_tcb, MDSP_LOG_TIMER_SIG);
   }

} /* mdsp_log_inactivate */



/*===========================================================================

FUNCTION mdsp_log_process_app_state_change

DESCRIPTION
  Called when an app changes state to or from ENABLED state or awake/sleep
  to to handle activating or deactivating logging if necessary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_process_app_state_change (mdsp_app_type app_id)
{
   /* If this app does not require logging our state will not change */
   if (mdsp_app_table[app_id].log_mode == MDSP_LOG_DISABLED)
   {
      return;
   }

   /* Otherwise we re-determine the logging mode */
   mdsp_log_determine_mode();

} /* mdsp_log_process_app_state_change */



/*===========================================================================

FUNCTION mdsp_log_determine_mode

DESCRIPTION
  Determines the current logging mode based on which applications are active
  and what their logging modes are.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mdsp_log_determine_mode (void)
{
   mdsp_log_mode_type new_log_mode;  /* desired log mode */
   mdsp_app_type      app_id;        /* application id */
   
   /*---------------------------------------------------------------*/

   /* If the clock is off, we disable logging */
   if (mdsp_clock_mask == 0)
   {
      new_log_mode = MDSP_LOG_DISABLED;
   }
   /* Otherwise go through all apps and find highest requested logging type */
   else
   {
      new_log_mode = mdsp_log.default_mode;
      mdsp_log.period = MDSP_LOG_MAX_PERIOD;
      for (app_id = (mdsp_app_type)0; app_id < MDSP_APP_MAX; app_id++)
      {
         /* Only count apps that are enabled and awake */
         if ((mdsp_app_table[app_id].state == MDSP_APP_STATE_ENABLED) &&
             (mdsp_clock_mask & MDSP_APP_CLK_MASK((uint16)app_id)) &&
             (mdsp_app_table[app_id].log_mode != MDSP_LOG_DISABLED))
         {
            /* Find highest requested log mode */
            if (mdsp_app_table[app_id].log_mode > new_log_mode)
            {
               new_log_mode = mdsp_app_table[app_id].log_mode;
            }
            /* Find shortest requested period */
            if (mdsp_app_table[app_id].log_period < mdsp_log.period)
            {
               mdsp_log.period = mdsp_app_table[app_id].log_period;
            }
         }
      }

      /* If the period did not get set, use the default. */
      if (mdsp_log.period == MDSP_LOG_MAX_PERIOD)
      {
         mdsp_log.period = MDSP_LOG_DEFAULT_PERIOD;
      }
   }

   /* Check if the mode actually changed */
   if (mdsp_log.mode == new_log_mode)
   {
      return;
   }
   
   /* Set the log mode */
   MSG_LOW("mDSP log mode: %d->%d (period=%d)", mdsp_log.mode, new_log_mode, mdsp_log.period);
   mdsp_log.mode = new_log_mode;
   
   /* Check if we need to change states */
   if (!mdsp_log.active && mdsp_log.mode != MDSP_LOG_DISABLED)
   {
      mdsp_log_activate();
   }
   else if (mdsp_log.active && mdsp_log.mode == MDSP_LOG_DISABLED)
   {
      mdsp_log_inactivate();
   }
}



/*===========================================================================

FUNCTION mdsp_log_send_chunks

DESCRIPTION
  Send the given number of chunks out as diag packets.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mdsp_log_send_chunks
(
   uint32 num_chunks           /* How many chunks to send */
)
{
   uint32 chunk_index;                   /* loop index into chunk buffer */
   uint16 i;                             /* loop index */
   uint32 size;                          /* size of log packet */
   LOG_MDSP_LOG_CHUNKS_C_type *log_ptr;  /* allocated log packet */

   /*---------------------------------------------------------------*/

   /* Check that this log item is enabled and we have something to log */
   if (!log_status(LOG_MDSP_LOG_CHUNKS_C) || mdsp_log.new_chunks == 0)
   {
      return;
   }

   /* First do not send more chunks than are new or are available - note
      that mdsp_log_new_chunks may be larger than MDSP_LOG_NUM_CHUNKS_TO_SAVE
      so the order of the below checks is important. */
   if (num_chunks > mdsp_log.new_chunks)
   {
      num_chunks = mdsp_log.new_chunks;
   }
   if (num_chunks > MDSP_LOG_NUM_CHUNKS_TO_SAVE)
   {
      num_chunks = MDSP_LOG_NUM_CHUNKS_TO_SAVE;
   }

   /* Determine where in the chunk buffer to start */
   chunk_index = mdsp_log.chunks_index + MDSP_LOG_NUM_CHUNKS_TO_SAVE - num_chunks;
   if (chunk_index >= MDSP_LOG_NUM_CHUNKS_TO_SAVE)
   {
      chunk_index -= MDSP_LOG_NUM_CHUNKS_TO_SAVE;
   }

   while (num_chunks > 0)
   {
      /* Attempt to allocate a log packet */
      size = sizeof(LOG_MDSP_LOG_CHUNKS_C_type);
      if (num_chunks < MDSP_LOG_NUM_CHUNKS_TO_SEND)
      {
         size -= (MDSP_LOG_NUM_CHUNKS_TO_SEND - num_chunks) * sizeof(mdsp_log_chunk_type);
      }
      log_ptr = (LOG_MDSP_LOG_CHUNKS_C_type *)log_alloc(LOG_MDSP_LOG_CHUNKS_C, size);
      if (log_ptr == NULL)
      {
         break;
      }

      /* Spin through the buffer and copy chunks over to the log packets */
      for (i = 0; i < MDSP_LOG_NUM_CHUNKS_TO_SEND && num_chunks > 0; i++)
      {
         memcpy((void *)&log_ptr->chunks[i],
                (void *)&mdsp_log.chunks[chunk_index],
                sizeof(mdsp_log_chunk_type));

         num_chunks--;
         chunk_index++;
         if (chunk_index >= MDSP_LOG_NUM_CHUNKS_TO_SAVE)
         {
            chunk_index = 0;
         }
      }

      log_ptr->num_chunks = i;

      /* Send the packet */
      log_commit((PACKED void *)log_ptr);
   }

   /* Reset count of new chunks - note that if there are more new chunks 
      than were requested to log we drop the older ones.  This is by design. */
   mdsp_log.new_chunks = 0;

} /* mdsp_log_send_chunks */




/*===========================================================================

FUNCTION mdsp_log_read_chunks

DESCRIPTION
  Reads out any new chunks from mDSP memory into the internal buffer.

DEPENDENCIES
  Should not be called unless at least one application is enabled since
  results will be undefined if an image is in the process of being loaded.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static uint32 mdsp_log_read_chunks (void)
{
   uint16 logReadOffset;        /* read offset from shared memory */
   uint16 logWriteOffset;       /* write offset from shared memory */
   uint16 logBufferSize;        /* buffer size from shared memory */
   uint16 length;               /* length of chunk */
   uint16 index;                /* index into packet */
   uint32 new_chunks;           /* How many chunks were read */
   mdsp_log_chunk_type *chunk;  /* shortcut to current chunk */
   boolean clock_was_on;        /* Flag if SELF already had clock enabled */

   /*---------------------------------------------------------------*/

   /* We need the clock on while we read out the buffer - this function may
      be called during a recovery attempt in which case MDSP_APP_SELF will
      already have flagged the clock in use and we do not want to lose that */
   clock_was_on = (mdsp_clock_mask & MDSP_APP_CLK_MASK((uint16)MDSP_APP_SELF));
   mdsp_clocks_enable(MDSP_APP_SELF);

   /* Set auto update to zero to drop new packets - this is a semaphore
      on access to logReadOffset.  We wait 2 us after setting this to avoid
      any race conditions with firmware. */
   mdsp_write(MDSP_logAutoUpdate, 0);
   clk_busy_wait(2);

   /* Latch log offsets from firmware */
   logReadOffset  = mdsp_read(MDSP_logReadOffset);
   logWriteOffset = mdsp_read(MDSP_logWriteOffset);
   logBufferSize  = mdsp_read(MDSP_logBufferSize);
   new_chunks     = 0;

   /* If the buffer size is 0, this module must not support logging.  This
      can happen when the VI module is loaded using the GSM app. */
   if (logBufferSize == 0)
   {
      logReadOffset = logWriteOffset = 0;
   }
   /* Sanity check the values.  Read and write offsets must be multiples
      of the chunk size, and offsets must fall in the buffer. */
   else if ((logReadOffset % MDSP_LOG_MAX_CHUNK_SIZE) != 0 ||
            (logWriteOffset % MDSP_LOG_MAX_CHUNK_SIZE) != 0 ||
            logReadOffset >= logBufferSize ||
            logWriteOffset >= logBufferSize)
   {
      MSG_FATAL("Invalid read/write offset: r=%04x, w=%04x, s=%d",
                logReadOffset, logWriteOffset, logBufferSize);

      /* If we are not already in the recovery process, trigger one.  Note
         that this function will set the MDSP_APP_SELF bit in the clock
         mask, so we need to avoid clearing it.  Thus just return. */
      if (!mdsp_log.recovery_in_progress)
      {
         (void)mdsp_recover_from_dsp_failure();
      }
      return 0;
   }

   /* Read out chunks until we match - this is guaranteed by the above check */
   while (logReadOffset != logWriteOffset)
   {
      /* Read the length of the chunk */
      length = mdsp_read_offset(MDSP_logBuffer, logReadOffset);

      /* Sanity check that we are still inside the buffer */
      if (logReadOffset + length > logBufferSize || length < MDSP_LOG_CHUNK_HDR_SIZE)
      {
         MSG_FATAL("Bad mDSP log length (read=%d, length=%d, size=%d)",
                   logReadOffset, length, logBufferSize);

         /* If we are not already in the recovery process, trigger one.
            See comments above as well. */
         if (!mdsp_log.recovery_in_progress)
         {
            (void)mdsp_recover_from_dsp_failure();
         }
         return 0;
      }

      /* Get the location to store the chunk */
      chunk = &mdsp_log.chunks[mdsp_log.chunks_index];

      /* Read in the chunk from the mDSP buffer */
      chunk->length = length;
      chunk->sequence_number = mdsp_read_offset(MDSP_logBuffer, (logReadOffset + 1));
      mdsp_block_read
       (
         MDSP_logBuffer,
         logReadOffset + MDSP_LOG_CHUNK_HDR_SIZE,
         (uint16 *)chunk->packets,
         length - MDSP_LOG_CHUNK_HDR_SIZE,
         FALSE
       );

      /* Run dropped packet checking if enabled */
      if (mdsp_log.count_dropped_packets)
      {
         /* Check if the sequence number matches what we expect - note we
            have no initial condition test so expect spurious msgs */
         if (mdsp_log.next_sequence_number != chunk->sequence_number)
         {
            MSG_HIGH("Dropped %d packets (%d to %d)",
                     chunk->sequence_number - mdsp_log.next_sequence_number,
                     mdsp_log.next_sequence_number, chunk->sequence_number - 1);
         }

         /* Calculate the next expected sequence number by counting packets */
         mdsp_log.next_sequence_number = chunk->sequence_number;
         index = 0;
         while (index < length - MDSP_LOG_CHUNK_HDR_SIZE)
         {
            index += chunk->packets[index] & 0xFF;
            mdsp_log.next_sequence_number++;
         }
      }

      /* Move chunk index and counter */
      new_chunks++;
      mdsp_log.chunks_index++;
      if (mdsp_log.chunks_index >= MDSP_LOG_NUM_CHUNKS_TO_SAVE)
      {
         mdsp_log.chunks_index = 0;
      }

      /* Jump to the next chunk */
      logReadOffset += MDSP_LOG_MAX_CHUNK_SIZE;
      if (logReadOffset >= logBufferSize)
      {
         logReadOffset = 0;
      }
   }

   /* Update read pointer */
   mdsp_write(MDSP_logReadOffset, logReadOffset);
   mdsp_log.new_chunks += new_chunks;

   /* Re-enable auto-update (dropping of old packets) */
   mdsp_write(MDSP_logAutoUpdate, 0xFFFF);

   /* Done with the clock */
   if (!clock_was_on)
   {
      mdsp_clocks_disable(MDSP_APP_SELF);
   }

   return new_chunks;

} /* mdsp_log_read_chunks */



/*===========================================================================

FUNCTION mdsp_log_process_timer

DESCRIPTION
  MDSP Services log code timer expiry processing.  Reads out any new
  chunks from mDSP memory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_process_timer (void)
{
   uint32        new_chunks;  /* how many chunks were read */
   #ifdef MDSP_LOG_PROFILING
   timetick_type start;       /* time when chunk reading started */
   timetick_type mid;         /* time when reading was done */
   timetick_type end;         /* time when logging was done */
   #endif

   /*---------------------------------------------------------------*/

   /* Make sure we are active and the clock is on */
   if (!mdsp_log.active || mdsp_clock_mask == 0)
   {
      MSG_ERROR("Unexpected mDSP log timer expiry: active=%d, clock=%d",
                mdsp_log.active, mdsp_clock_mask, 0);
      return;
   }

   #ifdef MDSP_LOG_PROFILING
   start = timetick_get();
   #endif

   /* Read available chunks into the buffer */
   new_chunks = mdsp_log_read_chunks();

   #ifdef MDSP_LOG_PROFILING
   mid = timetick_get();
   #endif

   /* Send the log packets if we are in continuous mode */
   if (mdsp_log.mode == MDSP_LOG_CONTINUOUS)
   {
      mdsp_log_send_chunks(mdsp_log.new_chunks);
   }

   #ifdef MDSP_LOG_PROFILING
   end = timetick_get();
   MSG_HIGH("mDSP log: chunks=%d, t_read=%d, t_log=%d", new_chunks,
            timetick_diff(start, mid, T_SCLK),
            timetick_diff(mid, end, T_SCLK));
   #endif

   /* Restart timer - note it is possible that an interrupt happened
      during the above processing that turned off the clock and inactivated
      logging, so we double-check before installing the timer again. */
   if (mdsp_log.active)
   {
      (void) rex_set_timer(&mdsp_log.timer, mdsp_log.period);
   }

  /*lint -esym(550,new_chunks) Tell Lint we know new_chunks symbol is not accessed*/
} /* mdsp_log_process_timer */



/*===========================================================================

FUNCTION mdsp_log_dsp_failure

DESCRIPTION
  Called if an mDSP failure was detected and recovery is about to begin.
  Will cause log and memory to be saved to EFS if the appropriate flag is
  set.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_dsp_failure (void)
{
   /* If the save on error flag is set then save the log and memory, using
      SELF as the id */
   if (mdsp_log.save_on_error)
   {
      mdsp_log_save(MDSP_APP_SELF, 0);
      mdsp_memory_save(MDSP_APP_SELF, 0);
   }

   /* Flag that we are in the middle of a recovery attempt */
   mdsp_log.recovery_in_progress = TRUE;
}



/*===========================================================================

FUNCTION mdsp_log_dsp_start

DESCRIPTION
  This function is called after the firmware image has been loaded but
  before it starts running.  It sets up any shared variables that need
  to be set before the kernel starts.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_dsp_start (void)
{
   /* Set the timer period - note this variable is only read by firmware
      when it first comes out of reset */
   mdsp_write(MDSP_timerPeriod, MDSP_LOG_DEFAULT_MDSP_TIMER_PERIOD);

   /* Set auto update to drop old packets by default */
   mdsp_write(MDSP_logAutoUpdate, 0xFFFF);

   /* Everything is good now */
   mdsp_log.recovery_in_progress = FALSE;

   /* Store the firmware version */
   mdsp_memory_file_hdr.fw_version = mdsp_read(MDSP_fwVersion);
}



/*===========================================================================

FUNCTION mdsp_log_load_nv

DESCRIPTION
  MDSP Services log code NV loading function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Calls rex_wait while NV is read.

===========================================================================*/
static void mdsp_log_load_nv (void)
{
   nv_cmd_type    cmd;   /* NV command structure */
   nv_item_type   item;  /* NV item to be read */
   
   /*---------------------------------------------------------------*/

   /* Prepare the NV read command buffer. */
   cmd.item       = NV_MDSP_MEM_DUMP_ENABLED_I;
   cmd.cmd        = NV_READ_F;
   cmd.data_ptr   = &item;
   cmd.tcb_ptr    = &mdsp_tcb;
   cmd.sigs       = MDSP_LOG_NV_SIG;
   cmd.done_q_ptr = NULL;
  
   /* Read NV */
   nv_cmd(&cmd);
   (void)rex_wait(MDSP_LOG_NV_SIG);
   (void) rex_clr_sigs(&mdsp_tcb, MDSP_LOG_NV_SIG);

   /* Process the results */
   if (cmd.status == NV_DONE_S)
   {
      mdsp_log.save_on_error = item.mdsp_mem_dump_enabled;
   }
   else
   {
      mdsp_log.save_on_error = FALSE;
   }

} /* mdsp_log_load_nv */



/*===========================================================================

FUNCTION mdsp_log_init

DESCRIPTION
  MDSP Services log code initialization function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_init (void)
{
   /* Initialize timer */
   rex_def_timer(&mdsp_log.timer, &mdsp_tcb, MDSP_LOG_TIMER_SIG);

   /* Load NV */
   mdsp_log_load_nv();

   /* Set our state */
   mdsp_log.active          = FALSE;
   mdsp_log.mode            = MDSP_LOG_DISABLED;
   mdsp_log.default_mode    = MDSP_LOG_DISABLED;
   mdsp_log.period          = MDSP_LOG_DEFAULT_PERIOD;
   mdsp_log.time_remaining  = MDSP_LOG_DEFAULT_PERIOD;

   #ifdef MDSP_LOG_PROFILING
   /* Set this flag to print a message when packets are dropped */
   mdsp_log.count_dropped_packets = TRUE;
   #endif

   /* Fill in the static values in the memory file header */
   mdsp_memory_file_hdr.mem_addr[0] = (uint32)mdsp_bank_address(MDSP_BANK_RAMA, 0);
   mdsp_memory_file_hdr.mem_addr[1] = (uint32)mdsp_bank_address(MDSP_BANK_RAMB, 0);
   mdsp_memory_file_hdr.mem_addr[2] = (uint32)mdsp_bank_address(MDSP_BANK_RAMC, 0);
   mdsp_memory_file_hdr.mem_addr[3] = (uint32)mdsp_bank_address(MDSP_BANK_RAMI, 0);

} /* mdsp_log_init */



/*===========================================================================

FUNCTION mdsp_efs_write

DESCRIPTION
  Writes a chunk of data from mDSP memory to the given file.  Makes multiple
  calls to efs_write until all data is written.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static boolean mdsp_efs_write
(
   int            fd,          /* file descriptor */
   const uint16  *mdsp_addr,   /* ARM address of mDSP location to save */
   uint32         num_words,   /* number of words */
   uint32         word_size    /* word size in bytes */
)
{
   int         offset;          /* offset into given address for next write */
   int         bytes_written;   /* bytes written by efs_write */
   const char *address;         /* byte-wise address */
   fs_size_t   nbytes;          /* number of bytes left to write */
   boolean     ret_val;         /* return value */

   /*---------------------------------------------------------------*/

   /* EFS write works on bytes, so convert address and size accordingly */
   address = (const char *)mdsp_addr;
   nbytes = num_words * word_size;

   offset = 0;
   ret_val = TRUE;

   /* Loop until all bytes are written.  This is required because EFS
      will only write up to 8192 bytes at a time. */
   while (nbytes > 0)
   {
      bytes_written = efs_write(fd, address + offset, nbytes);
      if (bytes_written <= 0)
      {
         ret_val = FALSE;
         break;
      }

      nbytes -= (uint32)bytes_written;
      offset += bytes_written;
   }

   return ret_val;
}



/*===========================================================================

FUNCTION mdsp_memory_save_processing

DESCRIPTION
  Requests that the mDSP memory be saved to disk.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_memory_save_processing
(
   mdsp_packet_type *packet  /* command packet to process */
)
{
   mdsp_app_type        app_id;       /* application id */
   int                  fd;           /* file descriptor */
   uint32               tag;          /* tag shortcut */
   mdsp_log_status_type log_rsp;      /* log response */
   boolean              clock_was_on; /* Flag if SELF already had clock enabled */
   time_type            ts;           /* timestamp */

   /*---------------------------------------------------------------*/

   app_id  = packet->memory_save_cmd.app_id;
   tag     = packet->memory_save_cmd.tag;
   log_rsp = MDSP_LOG_SAVE_COMPLETE;

   /* Accept command before starting the save */
   mdsp_send_cmd_event(app_id, MDSP_CMD_ACCEPT);

   MSG_HIGH("Saving mDSP memory: app=%d, tag=%d", app_id, tag, 0);

   /* Open the file */
   fd = efs_open(MDSP_MEMORY_FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0755);
   if (fd < 0)
   {
      MSG_ERROR("Unable to open log file", 0, 0, 0);
      mdsp_send_log_event(app_id, MDSP_LOG_SAVE_FAILED);
      return;
   }

   /* Fill in and write the header */
   mdsp_memory_file_hdr.app_id = (uint16)app_id;
   mdsp_memory_file_hdr.tag    = tag;
   ts_get(ts);
   memcpy((void *)mdsp_memory_file_hdr.timestamp, ts, sizeof(time_type));
   if (efs_write(fd, (void *)&mdsp_memory_file_hdr, sizeof(mdsp_memory_file_hdr)) <= 0)
   {
      (void) efs_close(fd);
      MSG_ERROR("mDSP memory hdr save failed", 0, 0, 0);
      mdsp_send_log_event(app_id, MDSP_LOG_SAVE_FAILED);
      return;
   }

   /* Make sure the clock stays on while we are reading */
   clock_was_on = (mdsp_clock_mask & MDSP_APP_CLK_MASK((uint16)MDSP_APP_SELF));
   mdsp_clocks_enable(MDSP_APP_SELF);

   /* Write the four mdsp memory segments to the file */
   if (!mdsp_efs_write(fd, mdsp_bank_address(MDSP_BANK_RAMA, 0), MDSP_MEMA_SIZE16, 2) ||
       !mdsp_efs_write(fd, mdsp_bank_address(MDSP_BANK_RAMB, 0), MDSP_MEMB_SIZE16, 2) ||
       !mdsp_efs_write(fd, mdsp_bank_address(MDSP_BANK_RAMC, 0), MDSP_MEMC_SIZE32, 4) ||
       !mdsp_efs_write(fd, mdsp_bank_address(MDSP_BANK_RAMI, 0), MDSP_MEMI_SIZE32, 4))
   {
      MSG_ERROR("mDSP memory save failed", 0, 0, 0);
      log_rsp = MDSP_LOG_SAVE_FAILED;
   }

   /* No need for clock anymore */
   if (!clock_was_on)
   {
      mdsp_clocks_disable(MDSP_APP_SELF);
   }

   /* Close the file */
   (void) efs_close(fd);

   /* Send response */
   mdsp_send_log_event(app_id, log_rsp);

   MSG_HIGH("Saved mDSP memory: app=%d, tag=%d", app_id, tag, 0);

} /* mdsp_memory_save_processing */


#endif /* FEATURE_MDSPSVC_LOG */
