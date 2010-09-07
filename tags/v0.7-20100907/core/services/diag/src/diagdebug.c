/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Saved F3 message retrieval support
                
GENERAL DESCRIPTION
  Contains handlers to support F3 message trace retrieval from the EFS.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2004-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagdebug.c#11 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/08   vg      Updated code to use PACK() vs. PACKED
04/24/08   tbg     Added method to verify logs being loaded from EFS match
                   the build currently loaded on the phone.
05/30/06   tbg     Added error check in diag_debug_write_to_efs()
10/31/05   as      Fixed lint errors.
10/04/05   tbg     Added ability to turn on F3 trace saving by default,
                   if FEATURE_SAVE_TRACE_ON_BY_DEFAULT is defined
08/15/05   eav     Removed calls to MSG_ERROR in run-time situations.
                   pop_oldest_record will return an error.  Upon error,
                   reinitialize the function
08/08/05   eav     Replaced ERR_FATAL with MSG_ERROR.  Do more checking of
                   buffer before saving to the EFS.
04/18/05   as      Fixed critical lint errors.
03/23/05   eav     Added FEATURE_LOG_FUNCTIONS.  Under this feature a NV item
                   will be read.  If this item is enabled, the function calls
                   will be logged in a RAM buffer.  This saving is handled in
                   rex.
02/25/04   as      Added featurization around msg_get_num_args()
02/18/04   as      Fixed critical lint errors.
11/30/04   as      Added FEATURE_DIAG_DEBUG_INVOKE_CRASH. This feature resets 
                   the phone automatically without user intervention if the 
                   phone encounters a critical failure 
10/08/04   eav     Cross reference table now dynamically allocated and stored
                   in efs file.
08/26/04   eav     Added optimizations for saving the timestamps.  Now saving
                   only the delta between timestamps, instead of the
                   absolute timestamp.
08/25/04   eav     Added support for multiple EFS file handling.
08/12/04   eav     Fixed compilation errors if FEATURE_ERR_EXTENDED_STORE is
                   not defined
08/03/04   tbg     Added call to err_data_is_valid() to see if any errors
                   have been logged.
07/13/04   eav     Cleaned up Lint errors
05/12/04   eav     Added event logging support.  Made function calls more
                   generic.  Added mechanism to combine the event trace
                   and F3 trace into one EFS file.  Added
                   diag_debug_trace_to_file to save all types into the
                   same RAM buffer.
04/01/04   eav     File created.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "AEEstd.h"
#include "diagtarget.h"
#include "diagdebug.h"
#include "diagdiag.h"
#include "err.h"
#include "nv_items.h"
#include "mobile.h"

#if defined FEATURE_ERR_EXTENDED_STORE

#include "diagcmd.h"
#include "diagdebug_nzi.h" 
#include "diagbuf.h"
#include "diagnv.h"
#include "memheap.h"
#include "tmc.h"
#include "ts.h"

#ifdef FEATURE_SAVE_TRACE_EXTENDED
  #include <stdlib.h>
  #include "nv.h"
  #include "stdio.h"
#endif

#ifdef FEATURE_EFS2
  #include "fs_fcntl.h"
  #include "fs_public.h"
#else
  #include "fs.h"
#endif

 
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#define DIAG_DEBUG_GET_TYPE(x)     (x >> 13)
#define DIAG_DEBUG_MSG_NUM_ARGS(x) ((x >> 8) & 0x1F)

#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
  #define DIAG_DEBUG_GET_DETAILS(x)  (x & 0xF)
#else
  #define DIAG_DEBUG_GET_DETAILS(x)  (x & 0x3)
#endif

/* Packet handlers */
PACK(void *) diag_debug_core_dump_handler (PACK(void *) , uint16);

#ifdef FEATURE_SAVE_TRACE_EXTENDED
PACK(void *) diag_debug_file_get_names_handler (PACK(void *), uint16);
PACK(void *) diag_debug_file_get_range_handler (PACK(void *), uint16);
PACK(void *) diag_debug_file_get_next_handler  (PACK(void *), uint16);
PACK(void *) diag_debug_file_rm_trace_handler  (PACK(void *), uint16);
PACK(void *) diag_debug_full_evt_get_next_handler  (PACK(void *), uint16,
                                                       diag_debug_hdr_type);
PACK(void *) diag_debug_full_msg_get_next_handler  (PACK(void *), uint16,
                                                       diag_debug_hdr_type);
#else /* FEATURE_SAVE_TRACE_EXTENDED */
PACK(void *) diag_debug_get_range_handler (PACK(void *), uint16);
PACK(void *) diag_debug_get_next_handler  (PACK(void *), uint16);
PACK(void *) diag_debug_rm_trace_handler  (PACK(void *), uint16);
PACK(void *) diag_debug_evt_get_next_handler  (PACK(void *), uint16,
                                                       diag_debug_hdr_type);
PACK(void *) diag_debug_msg_get_next_handler  (PACK(void *), uint16,
                                                       diag_debug_hdr_type);
#endif /* FEATURE_SAVE_TRACE_EXTENDED */

PACK(void *) diag_debug_invoke_crash_handler (PACK(void *), uint16);

static const diagpkt_user_table_entry_type diag_debug_tbl[] =
{
  {DIAG_DEBUG_CORE_DUMP, DIAG_DEBUG_CORE_DUMP, diag_debug_core_dump_handler},
#ifdef FEATURE_SAVE_TRACE_EXTENDED
  {DIAG_DEBUG_FILE_GET_NAMES, DIAG_DEBUG_FILE_GET_NAMES,
                                          diag_debug_file_get_names_handler},
  {DIAG_DEBUG_FILE_GET_RANGE, DIAG_DEBUG_FILE_GET_RANGE,
                                          diag_debug_file_get_range_handler},
  {DIAG_DEBUG_FILE_GET_NEXT, DIAG_DEBUG_FILE_GET_NEXT,
                                          diag_debug_file_get_next_handler},
  {DIAG_DEBUG_FILE_DEL_TRACE, DIAG_DEBUG_FILE_DEL_TRACE,
                                          diag_debug_file_rm_trace_handler},
#else
  {DIAG_DEBUG_DEL_TRACE, DIAG_DEBUG_DEL_TRACE, diag_debug_rm_trace_handler},
  {DIAG_DEBUG_GET_RANGE, DIAG_DEBUG_GET_RANGE, diag_debug_get_range_handler},
  {DIAG_DEBUG_GET_NEXT,  DIAG_DEBUG_GET_NEXT,  diag_debug_get_next_handler},
#endif
  {DIAG_DEBUG_INVOKE_CRASH, DIAG_DEBUG_INVOKE_CRASH, diag_debug_invoke_crash_handler}
};

typedef PACK(struct) {
  evt_length_type        evt_length;
  evt_id_type            id;
  diag_debug_tstamp_type tstamp;
  evt_payload_len_type   payload_length;
} evt_debug_store_type;

typedef struct {
  msg_const_type*        const_data_ptr;
  diag_debug_tstamp_type tstamp;
} msg_debug_store_type;

/* The dispatch table can be registered before runtime if this file is built
 * as a C++ file.
 */
#ifdef __cplusplus
  DIAGPKT_DISPATCH_AUTOREGISTER(DIAG_SUBSYS_DEBUG, diag_debug_tbl);
#endif

static void diag_debug_buffer_init( void );
static int diag_debug_write_to_efs(int, PACK(void*), unsigned int);
static __inline int  diag_debug_pop_oldest_record( void );
static __inline void diag_debug_trace_from_buffer(uint8*, unsigned int, int);

#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  static __inline void diag_debug_update_reference_ts( void );
#endif

#ifdef FEATURE_SAVE_TRACE_EXTENDED
#define FILENAME_BUF_SIZE 50
static char filename_g[FILENAME_BUF_SIZE];
static char cross_filename_g[FILENAME_BUF_SIZE];

static uint64 timestamps[MAX_NUM_TRACE_FILES];
static uint8 timestamps_index[MAX_NUM_TRACE_FILES];
#endif
static uint32 total_records = 0;
uint8 diag_debug_control_mask = 0;
uint8 diag_debug_detail_mask = 0;
#ifdef FEATURE_LOG_FUNCTIONS
uint8 diag_debug_call_trace_enable = 0x1;
#endif

#ifdef FEATURE_EFS2
typedef dword fs_file_position_type;

static int trace_fd                      = -1;
static int cross_fd                      = -1;
static int diag_debug_buffer_initialized = FALSE;

#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
static int    timestamp_table_set_up = FALSE;
static uint64 trc_tstamp[TRACE_MAX_NUM_RECORDS];
#endif

#define FS_SEEK_SET     SEEK_SET

#if defined(FEATURE_SAVE_TRACE_EXTENDED) && defined(FEATURE_EFS2)
static char debug_trace_file_format[] = "%s/%s_index%02d%s";
#endif

static void diag_debug_read_efs(int*, char*, PACK(void *), int);
static int  diag_debug_seek_efs(int*, char*, int, int);
static void evt_retrieve_prep( evt_debug_store_type*, int, char);
static void msg_retrieve_prep( msg_debug_store_type*, int, int );
static void msg_retrieve_args_prep( PACK(uint32 *), int, int, int );

#else  /* Not FEATURE_EFS2 */
static fs_handle_type msg_fd;
static fs_handle_type xtable_fd_g;
static fs_status_type
  diag_debug_read_efs(fs_handle_type*, char*, PACK(void *), int);
static fs_status_type
  diag_debug_seek_efs(fs_handle_type*, char*,
                                 fs_seek_origin_type, fs_file_position_type);
static fs_status_type
        msg_retrieve_prep( msg_debug_store_type*, int);
#endif /* Not FEATURE_EFS2 */
 

#ifdef FEATURE_DIAG_DEBUG_INVOKE_CRASH
/* sleep signals */
#define DIAG_DEBUG_INVOKE_CRASH_SLEEP_SIG     0x100000
#include "err.h"
extern void err_store_test(int);
#endif /* FEATURE_DIAG_DEBUG_INVOKE_CRASH */

/*===========================================================================

FUNCTION DIAG_DEBUG_TRACE_TO_BUFFER

DESCRIPTION
  Saves the buffer to the RAM buffer containing trace information.

DEPENDENCIES
  Interrupts must be locked.  FEATURE_ERR_EXTENDED_STORE must be defined.

RETURN VALUE
  If < 0, failure occurred.  Otherwise, success.

SIDE EFFECTS
  None
===========================================================================*/
int diag_debug_trace_to_buffer ( uint8* src, int length)
{

  int i, ret_val=0;

  if (!err_data_is_valid())
  {
    /* Check to see if the trace buffer needs to be initialized */
    if ( diag_debug_buffer_initialized == FALSE )
    {
      diag_debug_buffer_init();
    }
  
    /* Save to the circular buffer according to where the head points */
    for (i = 0; i < length; i++)
    {
      trace_buffer[trace_buffer_head] = src[i];

      trace_buffer_head++;

      if (trace_buffer_head >= TRACE_BUFFER_SIZE)
      {
        /* Wrap the head around to the beginning of the circular buffer */
        trace_buffer_head = 0;
      }

      /* Check the relation of the head to the tail.  If the head is at the
       * same index as the tail, you know the buffer is full.  Therefore,
       * you must pop the oldest record.
       */
      if (trace_buffer_head == trace_buffer_tail)
      {
        ret_val = diag_debug_pop_oldest_record();

        /* In case of error, reinitialize the trace buffer */
        if (ret_val == DIAGDEBUG_TRACE_BUFFER_CORRUPTED)
        {
          diag_debug_buffer_init();
          return ret_val;
        }
      }
    }
  }

  return DIAGDEBUG_SUCCESS;
}

/*===========================================================================

FUNCTION DIAG_DEBUG_INC_TRACE_RECORD_COUNT

DESCRIPTION
  Increase the trace record count

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void diag_debug_inc_trace_record_count ( void )
{
  if (!err_data_is_valid())
  {
    diag_debug_trace_record_count ++;
  }

  return;
}

/*===========================================================================

FUNCTION DIAG_DEBUG_TRACE_TIMESTAMP_TO_BUFFER

DESCRIPTION
  Specialized version of "diag_debug_trace_to_buffer" to handle timestamps.
  This function specifically deals with the ability to handle timestamps and
  the optimization of it.

DEPENDENCIES
  Interrupts must be locked

RETURN VALUE
  Same as diag_debug_trace_to_buffer return value
  
SIDE EFFECTS
  None
===========================================================================*/
int diag_debug_trace_timestamp_to_buffer ( qword timestamp )
{
  int ret_val = 0;

#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  diag_debug_tstamp_type       curr_ts  = 0;
  diag_debug_tstamp_delta_type delta_ts = 0;

  curr_ts = QW_CVT_Q2N(timestamp);

  delta_ts = curr_ts - prev_tstamp;

  /* Save the difference between the previous and current timestamp */
  ret_val = diag_debug_trace_to_buffer((uint8*) &delta_ts,
                                       sizeof(diag_debug_tstamp_delta_type));

  /* Update the previous timestamp */
  prev_tstamp = curr_ts;

#else
  diag_debug_tstamp_type shortened_ts;

  /* Get rid of the last two bytes of the timestamp */
  shortened_ts.hi = qw_hi(timestamp);
  shortened_ts.lo = qw_lo(timestamp) >> HALF_WORD_BITS;

  ret_val = diag_debug_trace_to_buffer((uint8*) &shortened_ts,
                                     sizeof(diag_debug_tstamp_type));

#endif /* FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS */

  return ret_val;
}


/*===========================================================================

FUNCTION DIAG_DEBUG_BUFFER_INIT

DESCRIPTION
  Initializes the circular buffer indices.

DEPENDENCIES
  Interrupts must be locked

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void
diag_debug_buffer_init( void )
{
#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  qword curr_time;

  /* Set up the reference timestamp */
  ts_get(curr_time);
  ref_tstamp = QW_CVT_Q2N(curr_time);
  prev_tstamp = ref_tstamp;
#endif

  diag_debug_trace_record_count = 0;

  /* Set up the buffer indices */
  trace_buffer_head = 0;
  trace_buffer_tail = 0;

  /* Initialize build ID associated with this run */
  /* (timestamp set when file is saved) */

  trace_buffer_id.timestamp=0;
  trace_buffer_id.buffer_ptr = (uint32)(&trace_buffer_id);
  memset(trace_buffer_id.build,0,DIAG_DEBUG_MAX_ID_LENGTH);
  std_strlcpy((char*)trace_buffer_id.build,(char*)mob_sw_rev,
              DIAG_DEBUG_MAX_ID_LENGTH);
  
  /* Once initialized to true, the trace records will be allowed to save */
  diag_debug_buffer_initialized = TRUE;
  return;
}

/*===========================================================================

FUNCTION DIAG_DEBUG_POP_OLDEST_RECORD

DESCRIPTION
  If the circular buffer fills up, the first record to leave the queue is
  the oldest record (FIFO).  This function assumes the tail is pointing to
  the oldest record already.  By incrementing the tail, you are allowing the
  previously indexed space to be overwritten.

DEPENDENCIES
  Interrupts must be locked

RETURN VALUE
  None

SIDE EFFECTS
  Decreases the diag_debug_trace_record_count by 1

===========================================================================*/
static __inline int
diag_debug_pop_oldest_record( void )
{
  diag_debug_hdr_type record_hdr = 0;
  evt_length_type evt_len = 0;
  unsigned int increment = 0;
  int type = 0;
  char details = 0;
#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
  int byte_sized = 0;
#endif

  /* Get the header of the oldest record stored in the RAM buffer */
  diag_debug_trace_from_buffer((uint8*) &record_hdr,
                         trace_buffer_tail, sizeof(diag_debug_hdr_type));

  type = DIAG_DEBUG_GET_TYPE(record_hdr);

  switch(type)
  {
    case DIAG_DEBUG_MSG_BIT:
      /* Increment past header and const type, at least */
      increment = sizeof(diag_debug_hdr_type)+ sizeof(msg_const_type *);

      details = DIAG_DEBUG_GET_DETAILS(record_hdr);

      if (details & DIAG_DEBUG_SAVETIME)
      {
#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
        increment += sizeof(diag_debug_tstamp_delta_type);
#else
        increment += sizeof(diag_debug_tstamp_type);
#endif
      }

      if (details & DIAG_DEBUG_SAVEVARS)
      {
#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
        byte_sized = DIAG_DEBUG_BYTE_SIZED_ARGS(details);
        
        increment += (DIAG_DEBUG_MSG_NUM_ARGS(record_hdr) *
                                (sizeof(msg_arg_type) - (uint32)byte_sized));
#else
        increment += (DIAG_DEBUG_MSG_NUM_ARGS(record_hdr) *
                                                       sizeof(msg_arg_type));
#endif
      }

      break;
    case DIAG_DEBUG_EVT_BIT:
      diag_debug_trace_from_buffer((uint8*) &evt_len,
            trace_buffer_tail+sizeof(diag_debug_hdr_type),
                              sizeof(evt_length_type));
      increment = sizeof(diag_debug_hdr_type) +
                  sizeof(evt_length_type) + evt_len;
      break;
    case DIAG_DEBUG_LOG_BIT:
    default:
      return DIAGDEBUG_TRACE_BUFFER_CORRUPTED;
  }

#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  /* Update the oldest timestamp, since oldest record is going to be
   * popped off
   */
  ret_val = diag_debug_update_reference_ts();
  if (ret_val != DIAGDEBUG_SUCCESS)
  {
    return ret_val;
  }
#endif

  /* Move the tail index to point to the next oldest record */
  trace_buffer_tail = (trace_buffer_tail + increment) % TRACE_BUFFER_SIZE;
 
  diag_debug_trace_record_count--;
 
  return (int)increment;
}


#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
/*===========================================================================

FUNCTION DIAG_DEBUG_UPDATE_REFERENCE_TS

DESCRIPTION
  Update the reference timestamp.

DEPENDENCIES
  Interrupts must be locked.  Trace_buffer_tail points to the oldest message.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static __inline int
diag_debug_update_reference_ts( void )
{
  int type = -1;

  diag_debug_hdr_type record_hdr        = 0;
  diag_debug_tstamp_delta_type delta_ts = 0;

  /* Get the header of the oldest record stored in the RAM buffer */
  diag_debug_trace_from_buffer((uint8*) &record_hdr,
                         trace_buffer_tail, sizeof(diag_debug_hdr_type));

  type = DIAG_DEBUG_GET_TYPE(record_hdr);
  switch(type)
  {
    case DIAG_DEBUG_MSG_BIT:
      if (DIAG_DEBUG_GET_DETAILS(record_hdr) & DIAG_DEBUG_SAVETIME)
      {
        /* Retrieve timestamp delta */
        diag_debug_trace_from_buffer((uint8*) &delta_ts,
                              trace_buffer_tail+sizeof(diag_debug_hdr_type)+
                              sizeof(msg_const_type*),
                              sizeof(diag_debug_tstamp_delta_type));
      }
      break;
    case DIAG_DEBUG_EVT_BIT:
      if (DIAG_DEBUG_GET_DETAILS(record_hdr) & DIAG_DEBUG_SAVETIME)
      {
        /* Retrieve timestamp delta */
        diag_debug_trace_from_buffer((uint8*) &delta_ts,
            trace_buffer_tail+sizeof(diag_debug_hdr_type)+
            sizeof(evt_length_type)+sizeof(evt_id_type),
                              sizeof(diag_debug_tstamp_delta_type));
      }
      break;
    case DIAG_DEBUG_LOG_BIT:
    default:
      return DIAGDEBUG_TRACE_BUFFER_CORRUPTED;
      break;
  }

  /* set the reference timestamp */
  ref_tstamp = ref_tstamp + delta_ts;

  return DIAGDEBUG_SUCCESS;
}

#endif /* FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS */

/*===========================================================================

FUNCTION DIAG_DEBUG_TRACE_FROM_BUFFER

DESCRIPTION
  Reads the circular buffer and places the bytes into the destination buffer

DEPENDENCIES
  Interrupts must be locked

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static __inline void
diag_debug_trace_from_buffer(uint8* dst, unsigned int start_index, int length)
{
  int i;
  
  for (i = 0; i < length; i++)
  {
    dst[i] = trace_buffer[(i + (int)start_index) % TRACE_BUFFER_SIZE];
  }
  
  return;
}

/*===========================================================================

FUNCTION DIAG_DEBUG_SAVE_TRACE_TO_EFS

DESCRIPTION
  Saves the RAM buffers containing the F3 messages, events, and logs to a
  single EFS file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void
diag_debug_save_trace_to_efs( int file_index )
{
#ifdef FEATURE_EFS2

  unsigned int num_bytes = 0;
  int fd = -1;
  int xtable_fd = -1;
  int *cross_ref_table = NULL;
  int byte_position = 0;
  int record_size = 0;
  uint32 temp_count = 0;
  int i = 0;
  qword ts;
#ifdef FEATURE_SAVE_TRACE_EXTENDED
  char ts_file[FILENAME_BUF_SIZE];
  char crs_file[FILENAME_BUF_SIZE];
  char f3_file[FILENAME_BUF_SIZE];

#endif /* FEATURE_SAVE_TRACE_EXTENDED */

  if ((trace_buffer_head >= TRACE_BUFFER_SIZE) ||
      (trace_buffer_tail >= TRACE_BUFFER_SIZE) || !err_data_is_valid())
  {
    /* Indices were never initialized.  Do not write to EFS */
    return;
  }

#ifdef FEATURE_SAVE_TRACE_EXTENDED
  /* -------------------------------------------------------------------- *
   *  Save trace buffer to EFS                                            *
   * -------------------------------------------------------------------- */

  /* Make the directory if it does not exist */
  (void) efs_mkdir(DEBUG_TRACE_DIR, 0666);

  /* create file names for each file, delete if they already exist */
  (void) std_strlprintf(ts_file, sizeof(ts_file), debug_trace_file_format,
    DEBUG_TRACE_DIR, DEBUG_TRACE_NAME, file_index, ID_FILENAME_EXT);
  (void) std_strlprintf(f3_file, sizeof(f3_file), debug_trace_file_format,
    DEBUG_TRACE_DIR, DEBUG_TRACE_NAME, file_index, TRACE_FILENAME_EXT);  
  (void) std_strlprintf(crs_file, sizeof(crs_file), debug_trace_file_format,
    DEBUG_TRACE_DIR, DEBUG_TRACE_NAME, file_index, CROSS_FILENAME_EXT);
  (void) efs_unlink(ts_file);
  (void) efs_unlink(f3_file);
  (void) efs_unlink(crs_file);

  /*****************************
   *  SAVE IDENTIFICATION FILE 
   *****************************/

  /* The identification file serves two purposes.
   * 1. Provide a timestamp which is needed by QXDM to identify the logs
   *    associated with this file index.
   * 2. Identify which build was running when the logs associated with this
   *    index were saved.  If it is not the same build running when the logs
   *    are retrieved, the logs will not be visible to QXDM.
   */

  /* Get the timestamp value */
  ts_get(ts);
  trace_buffer_id.timestamp = QW_CVT_Q2N(ts);  

  /* Open the EFS file */
  fd = efs_open(ts_file, O_RDWR | O_CREAT | O_TRUNC, 0666);  
  if (fd == -1)
  {
    MSG_ERROR("File open failed: %d", efs_errno,0,0);
    return;
  }

  /* Save the id structure to the trace file. */
  if (diag_debug_write_to_efs(fd, (void *) &trace_buffer_id,
        sizeof(diag_debug_id_type)) != DIAGDEBUG_SUCCESS)
  {
    MSG_ERROR ("File did not write properly", 0, 0, 0);
    return;
  }

  /* Close the EFS file */
  if (efs_close(fd) == -1)
      {
    MSG_ERROR ("File did not close properly", 0, 0, 0);
    return;
      }

  /************************
   *  SAVE F3 TRACE FILE
   ************************/

  /* Open the EFS file */
  fd = efs_open(f3_file, O_RDWR | O_CREAT | O_TRUNC, 0666);
#else /* FEATURE_SAVE_TRACE_EXTENDED */

  /* Open the EFS file */
  fd = efs_open(DEBUG_TRACE_FILE, O_RDWR | O_CREAT | O_TRUNC, 0666);
#endif /* FEATURE_SAVE_TRACE_EXTENDED */

  if (fd == -1)
  {
    MSG_ERROR("File open failed: %d", efs_errno,0,0);
    return;
  }    
  
  /* Save the trace record count to the trace file */
  if( diag_debug_write_to_efs(fd, (void *) &diag_debug_trace_record_count,
        sizeof(uint32)) != DIAGDEBUG_SUCCESS)
  {
    MSG_ERROR ("File did not write properly", 0, 0, 0);
    return;
  }    

#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  /* First save the reference timestamp to efs */
  if(diag_debug_write_to_efs(fd, (void *) &ref_tstamp,
       sizeof(diag_debug_tstamp_type)) != DIAGDEBUG_SUCCESS)
  {
    MSG_ERROR ("File did not write properly", 0, 0, 0);
    return;
  }     
#endif

  /* Save circular buffer to EFS.  Save starting from the tail and
   * end at the head.  Be aware if the head has wrapped around to behind
   * the tail.
   */
  if (trace_buffer_tail < trace_buffer_head)
  {
    /* ---------------------------------------------------------
     * |     |   Tail   |    |      |   Head   |       |       |
     * ---------------------------------------------------------
     */

    num_bytes = trace_buffer_head - trace_buffer_tail;

    if(diag_debug_write_to_efs(fd, (void *) (trace_buffer+trace_buffer_tail),
         num_bytes) != DIAGDEBUG_SUCCESS)
    {
      MSG_ERROR ("File did not write properly", 0, 0, 0);
      return;
    }
  }
  else if (trace_buffer_tail > trace_buffer_head)
  {
    /* ---------------------------------------------------------
     * |    |    |  Head  |    |      |  Tail  |               |
     * ---------------------------------------------------------
     */
    /* First copy from the tail to the end of the buffer */
    num_bytes = TRACE_BUFFER_SIZE - trace_buffer_tail;

    if(diag_debug_write_to_efs(fd, (void *) (trace_buffer+trace_buffer_tail),
         num_bytes) != DIAGDEBUG_SUCCESS)
    {
      MSG_ERROR ("File did not write properly", 0, 0, 0);
      return;
    }      

    /* Next, write from the beginning of the array to the head */
    num_bytes = trace_buffer_head;

    if(diag_debug_write_to_efs(fd, (void *) trace_buffer, num_bytes)
       != DIAGDEBUG_SUCCESS)
    {
      MSG_ERROR ("File did not write properly", 0, 0, 0);
      return;
    }
  }
  else
  {
    /* if head == tail, then nothing in buffer. */
  }

  /* Close the EFS file */
  if (efs_close(fd) == -1)
  {
    MSG_ERROR ("File did not close properly", 0, 0, 0);
    return;
  }

  /* -------------------------------------------------------------------- *
   * After saving the trace buffer, save the byte offsets of each record  *
   * and store into a new table.  The new table is then stored into       *
   * the EFS.                                                             *
   * -------------------------------------------------------------------- */

  /* Create and open trace cross reference table */
#ifdef FEATURE_SAVE_TRACE_EXTENDED
  
  
  /************************
   *  SAVE CRS FILE
   ************************/
  
  xtable_fd = efs_open(crs_file, O_RDWR | O_CREAT | O_TRUNC, 0666);
#else
  xtable_fd = efs_open(DEBUG_TRACE_CROSS_REF_FILE,
                                           O_RDWR | O_CREAT | O_TRUNC, 0666);
#endif

  /* Allocate memory for cross reference.  Take memory from tmc heap because
   * that is the only heap available at this point.
   */
  cross_ref_table = (int*) mem_malloc(&tmc_heap,
                             diag_debug_trace_record_count * sizeof(int));

  if (cross_ref_table == NULL)
  {
    /* Space not allocated.  Do not write to EFS */
    return;
  }

  /* Increment byte_position past any meta info before the records in the
   * EFS file.  In this case, it is going past the record count.
   */
  byte_position = sizeof(int);

#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  /* Go past reference timestamp */
  byte_position += sizeof(uint64);
#endif

  /* temp_count needed because the pop_oldest_record function will
   * change the value of diag_debug_trace_record_count.
   */
  temp_count = diag_debug_trace_record_count;

  for (i = 0; i < (int)temp_count; i++)
  {
    cross_ref_table[i] = byte_position;
    record_size = diag_debug_pop_oldest_record();

    if (record_size == DIAGDEBUG_TRACE_BUFFER_CORRUPTED)
    {
      MSG_ERROR("Error when parsing trace buffer",0,0,0);

      /* Contents of trace buffer invalid */
      mem_free(&tmc_heap, cross_ref_table);
      (void) efs_close(xtable_fd);
#ifdef FEATURE_SAVE_TRACE_EXTENDED
      (void) efs_unlink(f3_file);     /* Remove the original data file */
      (void) efs_unlink(crs_file); /* Remove the cross reference table */
      (void) efs_unlink(ts_file); /* Remove the cross reference table */      
#endif
      return;
    }

    byte_position += record_size;
  }

  /* Save the cross reference table */
  if(diag_debug_write_to_efs(xtable_fd, (void *) cross_ref_table,
       temp_count * sizeof(int)) != DIAGDEBUG_SUCCESS)
    {
      MSG_ERROR ("File did not write properly", 0, 0, 0);
      return;
    }

  /* Free the memory used by the cross reference table */
  mem_free (&tmc_heap, cross_ref_table);

  /* Close the EFS file */
  if (efs_close(xtable_fd) == -1)
  {
    MSG_ERROR ("File did not close properly", 0, 0, 0);
    return;
  }

#endif /* FEATURE_EFS2 */
  return;
}

/*===========================================================================

FUNCTION DIAG_DEBUG_WRITE_TO_EFS

DESCRIPTION
  Wrapper to write to the EFS

DEPENDENCIES
  None

RETURN VALUE
  DIAGDEBUG_TRACE_FAILURE if a failure in efs_write occurs
  DIAGDEBUG_SUCCESS if successful

SIDE EFFECTS
  None

===========================================================================*/
static int
diag_debug_write_to_efs(int fd, PACK(void*) src, unsigned int num_bytes)
{
#ifdef FEATURE_EFS2
  unsigned int total_bytes = 0;
  int bytes_written = 0;

  while (total_bytes < num_bytes)
  {
    bytes_written = efs_write(fd,
                              ((uint8*) src) + total_bytes,
                              num_bytes - total_bytes
                             );
    
    if (bytes_written > 0)
    {
      total_bytes += bytes_written;
    }
    else
    {
      return DIAGDEBUG_TRACE_FAILURE;
    }
  }
#endif
  return DIAGDEBUG_SUCCESS;
}

/**************************************************************************
 
 ***                   Retrieval functions follow                      ****

 **************************************************************************/
 
/*===========================================================================

FUNCTION DIAG_DEBUG_INIT

DESCRIPTION
  Initialization function for this module.  Registers the packet handlers.
  Reads the diag_debug NV Item to set the run time mask.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void diagdebug_init( void )
{
  nv_stat_enum_type ret_val;

#ifndef __cplusplus
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DEBUG, diag_debug_tbl);
#endif

  /* Read the diag_debug NV Items */
  ret_val = diag_nv_read(NV_DIAG_DEBUG_DETAIL_I,  &diag_debug_detail_mask);
  if (ret_val != NV_DONE_S)
  {
    /* NV Item has yet to be written, or was not read correctly. */    
    /* Set to default value. */
    diag_debug_detail_mask = DIAG_DEBUG_DETAIL_MASK_DEFAULT_VAL;
  }
    
  ret_val = diag_nv_read(NV_DIAG_DEBUG_CONTROL_I, &diag_debug_control_mask);
  
  if (ret_val != NV_DONE_S)
  {
    /* NV Item has yet to be written, or was not read correctly. */
    /* Set to default value. */
    diag_debug_control_mask = DIAG_DEBUG_CONTROL_MASK_DEFAULT_VAL;
  }  
#ifdef FEATURE_LOG_FUNCTIONS
  ret_val = 
   diag_nv_read(NV_DIAG_DEBUG_CALL_TRACE_I, &diag_debug_call_trace_enable);

  if (ret_val != NV_DONE_S)
  {
    diag_debug_call_trace_enable = 0x0;
  }
#endif

#ifdef FEATURE_SAVE_TRACE_EXTENDED
  /* Read NV Item to determine maximum number of trace files to save */
  ret_val = diag_nv_read(NV_DIAG_DEBUG_MAX_FILES_I, &diag_debug_max_files);
  if (ret_val != NV_DONE_S)
  {
    /* NV Item has yet to be written, or was not read correctly. */
    /* Set to default value. */
    diag_debug_max_files = DIAG_DEBUG_MAX_FILES_DEFAULT_VAL;
  }
#endif

  if (diag_debug_control_mask == 0)
  {
    /* Not saving anything, so init buffer to zero */
    diag_debug_buffer_init();
  }

  return;
} /* END diag_debug_init */

#ifndef FEATURE_SAVE_TRACE_EXTENDED 
/*===========================================================================

FUNCTION DIAG_DEBUG_GET_RANGE_HANDLER

DESCRIPTION
  Retrieves the number of records stored in the type-specific EFS file

DEPENDENCIES
  diag_debug_init must be called prior.

RETURN VALUE
  Pointer to the response packet to the argument req_ptr.

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *)
diag_debug_get_range_handler ( PACK(void *) req_ptr, uint16 pkt_len )
{
  diag_serv_range_rsp_type * rsp_ptr     = NULL;
  int rsp_len = 0;
#ifdef FEATURE_EFS2
  int fd = -1;
  int status;
#endif  
  if (pkt_len < sizeof(diag_serv_range_req_type))
  {
    return diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

#ifdef FEATURE_EFS2
  fd = efs_open(DEBUG_TRACE_FILE, O_RDONLY);

  if (fd == -1)
  {
    status = 0;
  }

  /* Read the total number of records */
  status = efs_read(fd, &total_records, sizeof(int));

  if (status >= 0)
  {
    status = 0;
  }

  /* Close the EFS file */
  efs_close(fd);

#endif /* ifdef FEATURE_EFS2 */

  /* Create the response and send it over to diag */
  rsp_len = sizeof(diag_serv_range_rsp_type);

  rsp_ptr = (diag_serv_range_rsp_type *) diagpkt_subsys_alloc(
                                               DIAG_SUBSYS_DEBUG,
                                               DIAG_DEBUG_GET_RANGE,
                                               rsp_len);
  if (rsp_ptr == NULL)
  {
    return NULL;
  }

  rsp_ptr->status    = status;
  rsp_ptr->num_types = total_records;

  return rsp_ptr;
}
 
/*===========================================================================

FUNCTION DIAG_DEBUG_GET_NEXT_HANDLER

DESCRIPTION
  Retrieves the next record in the EFS file.

DEPENDENCIES
  diag_debug_init and diag_debug_get_range handler must be called prior.

RETURN VALUE
  Pointer to the response packet of the req_ptr.

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *)
diag_debug_get_next_handler ( PACK(void *) req_ptr, uint16 pkt_len )
{
  diag_serv_fetch_req_type * request_ptr = NULL;
  PACK(void *)              rsp_ptr     = NULL;

  fs_file_position_type fs_seek_position = 0;
  diag_debug_hdr_type record_hdr = 0;
  uint8 type;
  
  if (pkt_len < sizeof(diag_serv_fetch_req_type))
  {
    return diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  request_ptr = (diag_serv_fetch_req_type *) req_ptr;

  if (request_ptr->index >= total_records)
  {
    return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
  }
#ifdef FEATURE_EFS2

  diag_debug_seek_efs(&cross_fd, DEBUG_TRACE_CROSS_REF_FILE, FS_SEEK_SET,
                                         request_ptr->index * sizeof(int));

  diag_debug_read_efs(&cross_fd, DEBUG_TRACE_CROSS_REF_FILE, &fs_seek_position,
                                           sizeof(fs_file_position_type));

  diag_debug_seek_efs(&trace_fd, DEBUG_TRACE_FILE,
                                           FS_SEEK_SET, fs_seek_position);

  /* Read EFS to get the type of packet */
  diag_debug_read_efs(&trace_fd, DEBUG_TRACE_FILE,
                         &record_hdr, sizeof(diag_debug_hdr_type));

  type = DIAG_DEBUG_GET_TYPE(record_hdr);

  switch (type)
  {
     case DIAG_DEBUG_MSG_BIT:
       rsp_ptr = diag_debug_msg_get_next_handler(req_ptr, pkt_len, record_hdr);
       break;
     case DIAG_DEBUG_EVT_BIT:
       rsp_ptr = diag_debug_evt_get_next_handler(req_ptr, pkt_len, record_hdr);
       break;
     case DIAG_DEBUG_LOG_BIT:
     default:
       break;
  }
#endif
  
  return rsp_ptr;
}
 
/*===========================================================================

FUNCTION DIAG_DEBUG_MSG_GET_NEXT_HANDLER

DESCRIPTION
  Retrieves the next record in the F3 message trace EFS file.

DEPENDENCIES
  diag_debug_init and diag_debug_get_range handler must be called prior.

RETURN VALUE
  Pointer to the response packet of the req_ptr.

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *)
diag_debug_msg_get_next_handler ( PACK(void *) req_ptr,
                                  uint16 pkt_len,
                                  diag_debug_hdr_type record_hdr)
{
  msg_debug_store_type msg;
  diag_serv_fetch_req_type * request_ptr = NULL;
  diag_serv_fetch_rsp_type * rsp_ptr = NULL;
  int byte_sized = 0;
  int rsp_len=0;
  int offset =0;
  int savevars = 0, savetime = 0, num_args = 0;
#ifndef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  qword tstamp;
#endif

#ifdef FEATURE_EFS2
  int status = 0;
#else
  fs_status_type status = FS_OKAY_S;
#endif

  if (pkt_len < sizeof(diag_serv_fetch_req_type))
  {
    return diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  request_ptr = (diag_serv_fetch_req_type *) req_ptr;

  if (request_ptr->index >= total_records)
  {
    return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
  }

  savetime = DIAG_DEBUG_GET_DETAILS(record_hdr) & DIAG_DEBUG_SAVETIME;
  savevars = DIAG_DEBUG_GET_DETAILS(record_hdr) & DIAG_DEBUG_SAVEVARS;
#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
  byte_sized = DIAG_DEBUG_BYTE_SIZED_ARGS(DIAG_DEBUG_GET_DETAILS(record_hdr));
#endif

  /* Get message from the EFS */
  msg_retrieve_prep( &msg, savetime, request_ptr->index );

#ifndef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  qw_set(tstamp, msg.tstamp.hi, msg.tstamp.lo << HALF_WORD_BITS);
#endif /* FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS */

#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  num_args = msg_get_num_args(msg.const_data_ptr);
#else
  num_args = 0;
#endif

  /* Now that payload is created, construct the rest of the packet */
  rsp_len = FPOS(diag_serv_fetch_rsp_type, diag_serv_fetch_type) +
            FPOS(diag_serv_msg_struct_type, args) +
            (FSIZ(diag_serv_msg_struct_type, args[0]) * num_args) +
            strlen(msg.const_data_ptr->fmt) + 1 + // Account for NULLs
            strlen(msg.const_data_ptr->fname) + 1;
  
  /* Reserve space for the packet */
  rsp_ptr = (diag_serv_fetch_rsp_type *)
       diagpkt_subsys_alloc(DIAG_SUBSYS_DEBUG, DIAG_DEBUG_GET_NEXT, rsp_len);

  if (rsp_ptr == NULL)
    return NULL;

  /* Start filling up the fields within rsp_ptr */
  rsp_ptr->type   = DIAG_DEBUG_MSG_BIT;
  rsp_ptr->index  = request_ptr->index;
  rsp_ptr->status = status;

#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  rsp_ptr->diag_serv_fetch_type.msg.ts       = msg.tstamp;
#else
  rsp_ptr->diag_serv_fetch_type.msg.ts       = QW_CVT_Q2N(tstamp);
#endif
  rsp_ptr->diag_serv_fetch_type.msg.num_args = num_args;
  rsp_ptr->diag_serv_fetch_type.msg.line     = msg.const_data_ptr->desc.line;

  /* Put the arguments in the response packet */ 
#ifdef FEATURE_SAVE_TRACE_EXTENDED
  msg_retrieve_args_prep (rsp_ptr->diag_serv_fetch_type.full_msg.args,
                                           num_args, savevars, byte_sized);
#else
  msg_retrieve_args_prep (rsp_ptr->diag_serv_fetch_type.msg.args,
                                           num_args, savevars, byte_sized);
#endif

  /* Compute offset of where to start copying format string */
  /* Remember to account for NULLs */
  offset = rsp_len - (strlen(msg.const_data_ptr->fmt)+1)
                   - (strlen(msg.const_data_ptr->fname)+1);

  /* Now copy format string into the packet including NULLs */
  memcpy(((uint8*)rsp_ptr) + offset, msg.const_data_ptr->fmt,
                              strlen(msg.const_data_ptr->fmt)+1);

  /* Compute offset of where to start copying filename string */
  offset += strlen(msg.const_data_ptr->fmt)+1;

  /* Now copy filename string into the packet including NULLs */
  memcpy(((uint8*)rsp_ptr) + offset, msg.const_data_ptr->fname,
                                        strlen(msg.const_data_ptr->fname)+1);

  /* Now that the rsp_ptr has been set up, ship out to the DM side */
  return rsp_ptr;
}
 
/*===========================================================================

FUNCTION DIAG_DEBUG_EVT_GET_NEXT_HANDLER

DESCRIPTION
  Retrieves the next record in the event trace EFS file.

DEPENDENCIES
  diag_debug_init and diag_debug_get_range handler must be called prior.

RETURN VALUE
  Pointer to the response packet of the req_ptr.

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *)
diag_debug_evt_get_next_handler ( PACK(void *) req_ptr,
                                  uint16 pkt_len,
                                  diag_debug_hdr_type record_hdr
                                )
{
  diag_serv_fetch_req_type * request_ptr = NULL;
  diag_serv_fetch_rsp_type * rsp_ptr = NULL;

  evt_debug_store_type evt;
  event_id_type        event_id;

  int status = 0;
  int offset = 0;

  int payload_length_flag = 0;
  int rsp_len = 0;
  uint32 temp_uint32 = 0;
  evt_payload_len_type payload_length = 0;
#ifndef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  qword tstamp;
#endif

  request_ptr = (diag_serv_fetch_req_type *) req_ptr;

  /* Retrieve the event from the EFS file */
  evt_retrieve_prep( &evt, request_ptr->index,
                  DIAG_DEBUG_GET_DETAILS(record_hdr) & DIAG_DEBUG_SAVETIME);

  temp_uint32 = (uint32) evt.id;
  memcpy(&event_id, &temp_uint32, sizeof(event_id_type));

#ifndef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  /* Set up the timestamp */
  qw_set(tstamp, evt.tstamp.hi, evt.tstamp.lo << HALF_WORD_BITS);
#endif

  payload_length_flag = event_id.payload_len;

  /* Now that payload is created, construct the rest of the packet */
  rsp_len = FPOS(diag_serv_fetch_rsp_type, diag_serv_fetch_type) +
            sizeof(diag_serv_evt_struct_type);

  switch (payload_length_flag)
  {
    case EVENT_PAY_NONE:  /* No payload data */
      break;
    case EVENT_PAY_ONE_BYTE:  /* byte of payload data, no PAYLOAD_LENGTH */
      rsp_len += sizeof(evt_payload_type);
      break;
    case EVENT_PAY_TWO_BYTE:  /* Two bytes of payload data, no PAYLOAD_LENGTH */
      rsp_len += (2 * sizeof(evt_payload_type));
      break;
    case EVENT_PAY_LENGTH:  /* One byte PAYLOAD_LENGTH, specifying length */
      rsp_len += sizeof (evt_payload_len_type); // payload length byte

      payload_length = evt.payload_length;

      rsp_len += payload_length; // actual payload length
      break;
    default:
      break;
  }
  
  /* Reserve space for the packet */
  rsp_ptr = (diag_serv_fetch_rsp_type *)
       diagpkt_subsys_alloc(DIAG_SUBSYS_DEBUG, DIAG_DEBUG_GET_NEXT, rsp_len);

  if (rsp_ptr == NULL)
    return NULL;

  /* Start filling up the fields within rsp_ptr */
  rsp_ptr->type   = DIAG_DEBUG_EVT_BIT;
  rsp_ptr->index  = request_ptr->index;
  rsp_ptr->status = status;

  rsp_ptr->diag_serv_fetch_type.evt.evt_id = evt.id;
#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  rsp_ptr->diag_serv_fetch_type.evt.ts     = evt.tstamp;
#else
  rsp_ptr->diag_serv_fetch_type.evt.ts     = QW_CVT_Q2N(tstamp);
#endif

  switch (payload_length_flag)
  {
    case EVENT_PAY_NONE:  /* No payload data */
      break;
    case EVENT_PAY_ONE_BYTE:  /* A byte of payload data, no PAYLOAD_LENGTH */
      offset = rsp_len - sizeof(evt_payload_type);

      /* Do a read from EFS and copy directly into response pointer */
      diag_debug_read_efs(&trace_fd, DEBUG_TRACE_FILE, ((uint8*)rsp_ptr)+offset,
                                                   sizeof(evt_payload_type));
      break;
    case EVENT_PAY_TWO_BYTE:  /* Two bytes of payload data, no PAYLOAD_LENGTH */
      offset = rsp_len - (2 * sizeof(evt_payload_type));

      /* Do a read from EFS and copy directly into response pointer */
      diag_debug_read_efs(&trace_fd, DEBUG_TRACE_FILE, ((uint8*)rsp_ptr)+offset,
                                              2 * sizeof(evt_payload_type));
      break;
    case EVENT_PAY_LENGTH:  /* One byte PAYLOAD_LENGTH, specifying length */
      offset = rsp_len - sizeof (evt_payload_len_type) - payload_length;
      memcpy( ((uint8*) rsp_ptr)+offset, &payload_length,
                                             sizeof(evt_payload_len_type) );
   
      /* Get the payload length from the event retrieved */
      offset += sizeof(evt_payload_len_type);
     
      /* Do a read from EFS and copy directly into response pointer */
      diag_debug_read_efs(&trace_fd, DEBUG_TRACE_FILE, ((uint8*)rsp_ptr)+offset,
                                                            payload_length);
      break;
    default:
      break;
  }

  /* Now that the rsp_ptr has been set up, ship out to the DM side */
  return rsp_ptr;
}
 
/*===========================================================================

FUNCTION DIAG_DEBUG_RM_TRACE_HANDLER

DESCRIPTION
  Removes the trace file from the EFS

DEPENDENCIES
  diag_debug_init must be called prior

RETURN VALUE
  Pointer to the response packet of the req_ptr

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *)
diag_debug_rm_trace_handler ( PACK(void *) req_ptr, uint16 pkt_len )
{
  diag_serv_reset_info_rsp_type * rsp_ptr = NULL;
  int rsp_len = 0;
#ifdef FEATURE_EFS2
  int status = -1;
#endif 

  if (pkt_len < sizeof(diag_serv_reset_info_req_type))
  {
    return diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

#ifdef FEATURE_EFS2
  /* Close the EFS file if it is open */
  efs_close(trace_fd);
  efs_close(cross_fd);

  /* Remove the EFS file */
  status = efs_unlink(DEBUG_TRACE_FILE);

  status = efs_unlink(DEBUG_TRACE_CROSS_REF_FILE);
#endif /* FEATURE_EFS2 */

  rsp_len = sizeof (diag_serv_reset_info_rsp_type);

  /* Create response packet */
  rsp_ptr = (diag_serv_reset_info_rsp_type *)
            diagpkt_subsys_alloc( DIAG_SUBSYS_DEBUG,
                                  DIAG_DEBUG_DEL_TRACE, rsp_len);

  if (rsp_ptr == NULL)
  {
    return NULL;
  }

  rsp_ptr->status = status;

  return rsp_ptr;
}

#endif /* ifndef FEATURE_SAVE_TRACE_EXTENDED */ 
/*===========================================================================

FUNCTION DIAG_DEBUG_CORE_DUMP_HANDLER

DESCRIPTION
  Enables a core dump to occur remotely by calling ERR_FATAL

DEPENDENCIES
  diag_debug_init must be called prior

RETURN VALUE
  Pointer to the response packet of the req_ptr

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *)
diag_debug_core_dump_handler (PACK(void *) req_ptr, uint16 pkt_len)
{
  (void) req_ptr; /* to supress compiler warning */
  (void) pkt_len; /* to supress compiler warning */
  ERR_FATAL("Forced core dump %d, %d, %d",1,2,3);
  return NULL;                                                 //lint !e527
}
 
#ifdef FEATURE_SAVE_TRACE_EXTENDED
/*===========================================================================

FUNCTION DIAG_DEBUG_FILE_GET_NAMES_HANDLER

DESCRIPTION
  Gets a list of trace files

DEPENDENCIES
  diag_debug_init must be called prior

RETURN VALUE
  Pointer to the response packet of the rsp_ptr

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *)
diag_debug_file_get_names_handler (PACK(void *) req_ptr, uint16 pkt_len)
{
  diag_serv_file_names_rsp_type * rsp_ptr = NULL;
  EFSDIR *fs_dirp;
  struct fs_dirent *dirent_ptr = NULL;
  uint8 num_files = 0;
  int stat_result;
  int fd = -1;
  diag_debug_id_type id;
  char filename[FILENAME_BUF_SIZE];
  char *d_ptr, index_str[3];
  int i, rsp_len = 0, index = 0, rbytes = 0, file_index;
  struct fs_stat sbuf;

  (void) req_ptr; /* to supress compiler warning */
  (void) pkt_len; /* to supress compiler warning */

  /* Open directory to be read */
  fs_dirp = efs_opendir(DEBUG_TRACE_DIR);

  if (fs_dirp == NULL)
  {
    /* Invalid directory name */

    return rsp_ptr;
  }

  /* To skip the "." and ".." directory entries */
  (void) efs_readdir (fs_dirp);
  (void) efs_readdir (fs_dirp);

  dirent_ptr = efs_readdir (fs_dirp);

  while ( dirent_ptr != NULL && num_files < MAX_NUM_TRACE_FILES)
  {
    /* Need to identify which files are timestamp files */
    /* First, compare initial str to see if it matches DEBUG_TRACE_FILENAME */
    d_ptr = (dirent_ptr->d_name + strlen(dirent_ptr->d_name) 
             - strlen(ID_FILENAME_EXT));
    if ( (strncmp(dirent_ptr->d_name,DEBUG_TRACE_NAME,
                  strlen(DEBUG_TRACE_NAME))   == 0) &&
         (strncmp(d_ptr,ID_FILENAME_EXT,
                  strlen(ID_FILENAME_EXT)) == 0))
    {
      /* We have an id file match */
      (void) std_strlcpy (filename, DEBUG_TRACE_DIR, sizeof(filename));
      (void) std_strlcat (filename, "/", sizeof(filename));
      (void) std_strlcat (filename, dirent_ptr->d_name, sizeof(filename));
      stat_result = efs_lstat (filename, &sbuf);

      if (stat_result != 0)
      {
        /* Some error in reading status */
        return rsp_ptr;
      }
        
      /* Found file! */
      fd = efs_open(filename, O_RDONLY);
      if (fd == -1)
      {
        return rsp_ptr;
      }
      
      rbytes = efs_read(fd,&id,sizeof(id));
      efs_close(fd);
      
      /* Check to see if ID matches this build
       * (and that id file was read successfully)
       */
      if((rbytes == sizeof(id)) &&
        ((uint32)(id.buffer_ptr) == (uint32)(&trace_buffer_id)) &&
        (strncmp((char*)mob_sw_rev,(char*)id.build,
          DIAG_DEBUG_MAX_ID_LENGTH) == 0))
      {
        /* We have a matching ID. */
        
        /* Store index associated with this id to make 
         * future references easier */
        d_ptr = filename + 
            ( strlen(DEBUG_TRACE_DIR) + 
              strlen(DEBUG_TRACE_NAME) + 
              7 ); /* This 7 is for the '/' and '_index' in the filemane */
        (void) std_strlcpy(index_str, d_ptr, sizeof(index_str));
        file_index = atoi(index_str);
        
        /* Now we have the timestamp and the index, and want to store then in
         * their respective arrays.  To make correlation between the host and
         * the actual trace files stored on EFS easier, we will sort the files
         * based on their index value.  The lowest index (i.e. 0) will
         * be first in the list, etc.  This appears to be the order that the
         * file system returns them by default, but we will not make that 
         * assumption (we will read starting from the end of the list to
         * speed things up if this is the case).
         */

        for(i=num_files; i>0; i--)
        {
          if((timestamps_index[i-1] > file_index))
          {
            /* Bump entry up one position in list */
            timestamps[i] = timestamps[i-1];
            timestamps_index[i] = timestamps_index[i-1];            
          }
          else
          {
            /* Drop out and save new entry at index i */
            break;
          }
        }
        
        timestamps_index[i] = file_index;
        timestamps[i] = id.timestamp;            
        num_files++;
      }
    }

    /* Get next file name */
    dirent_ptr = efs_readdir (fs_dirp);
  }

  (void) efs_closedir(fs_dirp);

  /* Create the response and send it over to diag */
  rsp_len = FPOS(diag_serv_file_names_rsp_type, timestamps) +
            (FSIZ(diag_serv_file_names_rsp_type, timestamps[0]) * num_files);

  rsp_ptr = (diag_serv_file_names_rsp_type *) diagpkt_subsys_alloc(
                                               DIAG_SUBSYS_DEBUG,
                                               DIAG_DEBUG_FILE_GET_NAMES,
                                               rsp_len);
  if (rsp_ptr == NULL)
  {
    return NULL;
  }

  rsp_ptr->status    = 0;
  rsp_ptr->num_files = num_files;

  for (i = 0; i < num_files; i++)
  {
    index = rsp_len - ((num_files-i) * sizeof(uint64));
    memcpy( ((uint8*)rsp_ptr)+index, &(timestamps[i]), sizeof(uint64));
  }

  return rsp_ptr;
}
 
/*===========================================================================

FUNCTION DIAG_DEBUG_FILE_GET_RANGE_HANDLER

DESCRIPTION
  Retrieves the number of records stored in the specified EFS file

DEPENDENCIES
  diag_debug_init must be called prior

RETURN VALUE
  Pointer to the response packet to the argument req_ptr.

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *)
diag_debug_file_get_range_handler ( PACK(void *) req_ptr, uint16 pkt_len )
{
  diag_serv_file_range_req_type * request     = NULL;
  diag_serv_range_rsp_type * rsp_ptr          = NULL;
  int rsp_len = 0;

#ifdef FEATURE_EFS2
  int    status = 0;
  uint8  file_index = 0;
  #ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
    diag_debug_tstamp_type trc_ref_tstamp = 0;
  #endif
#endif

  if (pkt_len < sizeof(diag_serv_range_req_type))
  {
    return NULL;
  }

  request = (diag_serv_file_range_req_type *) req_ptr;

#ifdef FEATURE_EFS2

  file_index = timestamps_index[request->file_index];

  /* Set the global "filename" variable */
  (void) std_strlprintf(filename_g, sizeof(filename_g),debug_trace_file_format,
    DEBUG_TRACE_DIR, DEBUG_TRACE_NAME, file_index, TRACE_FILENAME_EXT);   
  /* Set the global cross reference table corresponding to the filename */
  (void) std_strlprintf(cross_filename_g, sizeof(cross_filename_g),
    debug_trace_file_format, DEBUG_TRACE_DIR, DEBUG_TRACE_NAME,
    file_index, CROSS_FILENAME_EXT);   

  trace_fd = efs_open(filename_g, O_RDONLY);

  if (trace_fd == -1)
  {
    return NULL;
  }

  /* Read the total number of records */
  status = efs_read(trace_fd, &total_records, sizeof(int));

  if (status >= 0)
  {
    status = 0;
  }

  /* Close the EFS file */
  (void) efs_close(trace_fd);
  trace_fd = -1;


#endif /* ifdef FEATURE_EFS2 */

#ifdef FEATURE_SAVE_TRACE_RECORD_TIMESTAMPS
  /* Now that a file has been selected, make sure the timestamp table is
   * set up for the "get_next" handlers.
   */
  timestamp_table_set_up = FALSE;
#endif

  /* Create the response and send it over to diag */
  rsp_len = sizeof(diag_serv_range_rsp_type);

  rsp_ptr = (diag_serv_range_rsp_type *) diagpkt_subsys_alloc(
                                               DIAG_SUBSYS_DEBUG,
                                               DIAG_DEBUG_FILE_GET_RANGE,
                                               (uint32)rsp_len);
  if (rsp_ptr == NULL)
  {
    /* Error occurred... get out */
    return NULL;
  }

  rsp_ptr->status    = (uint32)status;
  rsp_ptr->num_types = total_records;
 
  return rsp_ptr;
}

/*===========================================================================

FUNCTION DIAG_DEBUG_FILE_GET_NEXT_HANDLER

DESCRIPTION
  Retrieves the next record in the EFS file.  The EFS file is set by calling
  diag_debug_file_get_range_handler before.

DEPENDENCIES
  diag_debug_init and diag_debug_file_get_range_handler must be called prior.

RETURN VALUE
  Pointer to the response packet of the req_ptr.

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *)
diag_debug_file_get_next_handler ( PACK(void *) req_ptr, uint16 pkt_len )
{
  diag_serv_fetch_req_type * request_ptr = NULL;
  PACK(void *)             rsp_ptr     = NULL;

  fs_file_position_type fs_seek_position = 0;
  diag_debug_hdr_type record_hdr = 0;
  uint8 type;
#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  int increment=0, count=0, details=0;
  int byte_sized = 0;
  diag_debug_tstamp_type trc_ref_tstamp;
  diag_debug_tstamp_delta_type delta_ts;
#endif

  if (pkt_len < sizeof(diag_serv_fetch_req_type))
  {
    return diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  request_ptr = (diag_serv_fetch_req_type *) req_ptr;

  if (request_ptr->index >= total_records)
  {
    return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
  }

#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  /* If this is the first time getting a record, then set up the timestamp
   * table.
   */
  if (!timestamp_table_set_up)
  {
    /* Move the file pointer past the record count */
    diag_debug_seek_efs(&trace_fd, filename_g, FS_SEEK_SET, sizeof(int));

    /* Get the reference timestamp */
    diag_debug_read_efs(&trace_fd, filename_g,
                         &trc_ref_tstamp, sizeof(uint64));

    for (count = 0; count < total_records; count++)
    {
      /* reset the increment */
      increment = 0;

      /* Just read the first two bytes right away */
      diag_debug_read_efs(&trace_fd, filename_g,
                         &record_hdr, sizeof(diag_debug_hdr_type));

      /* Get the type of the current record */
      switch(DIAG_DEBUG_GET_TYPE(record_hdr))
      {
        case DIAG_DEBUG_MSG_BIT:

          /* already incremented past header.  Figure out how
           * many bytes to get past the rest of the message packet */
          efs_lseek(trace_fd, sizeof(msg_const_type*), SEEK_CUR);

          details = DIAG_DEBUG_GET_DETAILS(record_hdr);

          if ( details & DIAG_DEBUG_SAVETIME )
          {
            /* Determine the timestamp from optimized timestamp */
            efs_read(trace_fd, &delta_ts, sizeof(diag_debug_tstamp_delta_type));

            if (count == 0)
            {
              trc_tstamp[count] = delta_ts + trc_ref_tstamp;
            }
            else
            {
              trc_tstamp[count] = delta_ts + trc_tstamp[count-1];
            }
          }

          if ( details & DIAG_DEBUG_SAVEVARS )
          {
#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
            byte_sized = DIAG_DEBUG_BYTE_SIZED_ARGS(details);
            
            increment += DIAG_DEBUG_MSG_NUM_ARGS(record_hdr) *
                                           (sizeof(msg_arg_type)-byte_sized);
#else
            increment += (DIAG_DEBUG_MSG_NUM_ARGS(record_hdr) *
                                                    sizeof(msg_arg_type));
#endif
          }

          break;
        case DIAG_DEBUG_EVT_BIT:
          /* Determine the increment saved in the event record */
          efs_read(trace_fd, &increment, sizeof(evt_length_type));
          
          /* Increment past the evt_id_type */
          efs_lseek(trace_fd, sizeof(evt_id_type), SEEK_CUR);
          increment -= sizeof(evt_id_type);

          details = DIAG_DEBUG_GET_DETAILS(record_hdr);

          if (details & DIAG_DEBUG_SAVETIME)
          {
            /* Determine timestamp from reference timestamp */
            efs_read(trace_fd, &delta_ts, sizeof(diag_debug_tstamp_delta_type));
            increment -= sizeof(diag_debug_tstamp_delta_type);

            if (count == 0)
            {
              trc_tstamp[count] = delta_ts + trc_ref_tstamp;
            }
            else
            {
              trc_tstamp[count] = delta_ts + trc_tstamp[count-1];
            }
          }
          break;
        case DIAG_DEBUG_LOG_BIT:
        default:
          return NULL;
          break;
      }

      /* Move the file pointer by an increment of "record_length" */
      efs_lseek(trace_fd, increment, SEEK_CUR);

      diag_kick_watchdog();

    }

    timestamp_table_set_up = TRUE;
  }
#endif /* FEATURE_SAVE_TRACE_RECORD_TIMESTAMPS */

#ifdef FEATURE_EFS2

  (void) diag_debug_seek_efs(&cross_fd, cross_filename_g, FS_SEEK_SET,
                                         (int)(request_ptr->index * sizeof(int)));

  diag_debug_read_efs(&cross_fd, cross_filename_g, &fs_seek_position,
                                             sizeof(fs_file_position_type));

  (void) diag_debug_seek_efs(&trace_fd, filename_g, FS_SEEK_SET, (int)fs_seek_position);

  /* Read EFS to get the type of packet */
  diag_debug_read_efs(&trace_fd, filename_g,
                         &record_hdr, sizeof(diag_debug_hdr_type));

  type = DIAG_DEBUG_GET_TYPE(record_hdr);

  switch (type)
  {
     case DIAG_DEBUG_MSG_BIT:
       rsp_ptr = diag_debug_full_msg_get_next_handler(req_ptr,
                                                       pkt_len, record_hdr);
       break;
     case DIAG_DEBUG_EVT_BIT:
       rsp_ptr = diag_debug_full_evt_get_next_handler(req_ptr,
                                                       pkt_len, record_hdr);
       break;
     case DIAG_DEBUG_LOG_BIT:
     default:
       MSG_ERROR("Record type not handled %d",type,0,0);
       return NULL;
  }

  if (request_ptr->index == total_records - 1)
  {
    (void) efs_close(trace_fd);  trace_fd = -1;
    (void) efs_close(cross_fd);  cross_fd = -1;
  }
#endif

  diag_kick_watchdog();

  return rsp_ptr;
}

/*===========================================================================

FUNCTION DIAG_DEBUG_FULL_MSG_GET_NEXT_HANDLER

DESCRIPTION
  Retrieves the next record in the F3 message trace EFS file.

DEPENDENCIES
  diag_debug_init and diag_debug_get_range handler must be called prior.

RETURN VALUE
  Pointer to the response packet of the req_ptr.

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *)
diag_debug_full_msg_get_next_handler ( PACK(void *) req_ptr,
                                       uint16 pkt_len,
                                       diag_debug_hdr_type record_hdr)
{
  msg_debug_store_type msg;
  diag_serv_fetch_req_type * request_ptr = NULL;
  diag_serv_fetch_rsp_type * rsp_ptr = NULL;
  dword rsp_len=0;
  int offset =0;
  int savevars = 0, savetime = 0;
  uint8 num_args = 0;
  int byte_sized = 0;
#ifndef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  qword tstamp;
#endif

#ifdef FEATURE_EFS2
  int status = 0;
#else
  fs_status_type status = FS_OKAY_S;
#endif

  if (pkt_len < sizeof(diag_serv_fetch_req_type))
  {
    return diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  request_ptr = (diag_serv_fetch_req_type *) req_ptr;

  savetime = DIAG_DEBUG_GET_DETAILS(record_hdr) & DIAG_DEBUG_SAVETIME;
  savevars = DIAG_DEBUG_GET_DETAILS(record_hdr) & DIAG_DEBUG_SAVEVARS;
#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
  byte_sized = DIAG_DEBUG_BYTE_SIZED_ARGS(DIAG_DEBUG_GET_DETAILS(record_hdr));
#endif

  /* Get message from the EFS */
  msg_retrieve_prep( &msg, savetime, (int)request_ptr->index );

#ifndef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  qw_set(tstamp, msg.tstamp.hi, msg.tstamp.lo << HALF_WORD_BITS);
#endif

#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  num_args = msg_get_num_args(msg.const_data_ptr);
#else
  num_args = 0;
#endif

  /* Now that payload is created, construct the rest of the packet */
  rsp_len = FPOS(diag_serv_fetch_rsp_type, diag_serv_fetch_type) +
            FPOS(diag_serv_full_msg_struct_type, args) +
            (FSIZ(diag_serv_full_msg_struct_type, args[0]) * num_args) +
            strlen(msg.const_data_ptr->fmt) + 1 + // Account for NULLs
            strlen(msg.const_data_ptr->fname) + 1;

  /* Reserve space for the packet */
  rsp_ptr = (diag_serv_fetch_rsp_type *)
    diagpkt_subsys_alloc(DIAG_SUBSYS_DEBUG, DIAG_DEBUG_FILE_GET_NEXT, rsp_len);

  if (rsp_ptr == NULL)
    return NULL;

  /* Start filling up the fields within rsp_ptr */
  rsp_ptr->type   = DIAG_DEBUG_MSG_BIT;
  rsp_ptr->index  = request_ptr->index;
  rsp_ptr->status = (uint32)status;

  rsp_ptr->diag_serv_fetch_type.full_msg.cmd_code = DIAG_EXT_MSG_F;
  rsp_ptr->diag_serv_fetch_type.full_msg.ts_type  = 0; // WCDMA format
  rsp_ptr->diag_serv_fetch_type.full_msg.num_args = num_args;
  rsp_ptr->diag_serv_fetch_type.full_msg.drop_cnt = 0; // No messages dropped
#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  rsp_ptr->diag_serv_fetch_type.full_msg.ts       = msg.tstamp;
#else
  rsp_ptr->diag_serv_fetch_type.full_msg.ts       = QW_CVT_Q2N(tstamp);
#endif
  rsp_ptr->diag_serv_fetch_type.full_msg.line  =
                                           msg.const_data_ptr->desc.line;
  rsp_ptr->diag_serv_fetch_type.full_msg.ss_id =
                                           msg.const_data_ptr->desc.ss_id;
  rsp_ptr->diag_serv_fetch_type.full_msg.ss_mask =
                                           msg.const_data_ptr->desc.ss_mask;

#ifdef FEATURE_SAVE_TRACE_EXTENDED
  /* Put the arguments in the response packet */
  msg_retrieve_args_prep (rsp_ptr->diag_serv_fetch_type.full_msg.args,
                                            num_args, savevars, byte_sized);
#else
  msg_retrieve_args_prep (rsp_ptr->diag_serv_fetch_type.msg.args,
                                            num_args, savevars, byte_sized);
#endif

  /* Compute offset of where to start copying format string */
  /* Remember to account for NULLs */
  offset = (int)(rsp_len - (strlen(msg.const_data_ptr->fmt)+1)
                   - (strlen(msg.const_data_ptr->fname)+1));

  /* Now copy format string into the packet including NULLs */
  memcpy(((uint8*)rsp_ptr) + offset, msg.const_data_ptr->fmt,
                              strlen(msg.const_data_ptr->fmt)+1);

  /* Compute offset of where to start copying filename string */
  offset += (int)strlen(msg.const_data_ptr->fmt)+1;

  /* Now copy filename string into the packet including NULLs */
  memcpy(((uint8*)rsp_ptr) + offset, msg.const_data_ptr->fname,
                                        strlen(msg.const_data_ptr->fname)+1);

  /* Now that the rsp_ptr has been set up, ship out to the DM side */
  return rsp_ptr;
}

/*===========================================================================

FUNCTION DIAG_DEBUG_FULL_EVT_GET_NEXT_HANDLER

DESCRIPTION
  Retrieves the next event record in the trace EFS file.

DEPENDENCIES
  diag_debug_init and diag_debug_get_range handler must be called prior.

RETURN VALUE
  Pointer to the response packet of the req_ptr.

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *)
diag_debug_full_evt_get_next_handler ( PACK(void *) req_ptr,
                                       uint16 pkt_len,
                                       diag_debug_hdr_type record_hdr
                                     )
{
  diag_serv_fetch_req_type * request_ptr = NULL;
  diag_serv_fetch_rsp_type * rsp_ptr = NULL;

  evt_debug_store_type evt;
  event_id_type        event_id;

  int status = 0;
  uint32 offset = 0;

  int payload_length_flag = 0;
  dword rsp_len = 0;
  uint32 temp_uint32 = 0;
  evt_payload_len_type payload_length = 0;

#ifndef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  qword tstamp;
#endif

  (void) pkt_len; /* to supress compiler warning */

  request_ptr = (diag_serv_fetch_req_type *) req_ptr;

  /* Retrieve the event from the EFS file */
  evt_retrieve_prep( &evt, (int)request_ptr->index,
                  DIAG_DEBUG_GET_DETAILS(record_hdr) & DIAG_DEBUG_SAVETIME);

  temp_uint32 = (uint32) evt.id;
  memcpy(&event_id, &temp_uint32, sizeof(event_id_type));

#ifndef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  /* Set up the timestamp */
  qw_set(tstamp, evt.tstamp.hi, evt.tstamp.lo << HALF_WORD_BITS);
#endif

  payload_length_flag = event_id.payload_len;

  /* Now that payload is created, construct the rest of the packet */
  rsp_len = FPOS(diag_serv_fetch_rsp_type, diag_serv_fetch_type) +
            sizeof(diag_serv_full_evt_struct_type);

  switch (payload_length_flag)
  {
    case EVENT_PAY_NONE:  /* No payload data */
      break;
    case EVENT_PAY_ONE_BYTE:  /* byte of payload data, no PAYLOAD_LENGTH */
      rsp_len += sizeof(evt_payload_type);
      break;
    case EVENT_PAY_TWO_BYTE:  /* Two bytes of payload data, no PAYLOAD_LENGTH */
      rsp_len += (2 * sizeof(evt_payload_type));
      break;
    case EVENT_PAY_LENGTH:  /* One byte PAYLOAD_LENGTH, specifying length */
      rsp_len += sizeof (evt_payload_len_type); // payload length byte
      payload_length = evt.payload_length;

      rsp_len += payload_length; // actual payload length
      break;
    default:
      break;
  }

  /* Reserve space for the packet */
  rsp_ptr = (diag_serv_fetch_rsp_type *)
       diagpkt_subsys_alloc(DIAG_SUBSYS_DEBUG,
                                    DIAG_DEBUG_FILE_GET_NEXT, rsp_len);

  if (rsp_ptr == NULL)
    return NULL;

  /* Start filling up the fields within rsp_ptr */
  rsp_ptr->type   = DIAG_DEBUG_EVT_BIT;
  rsp_ptr->index  = request_ptr->index;
  rsp_ptr->status = (uint32)status;

  rsp_ptr->diag_serv_fetch_type.full_evt.cmd_code     = DIAG_EVENT_REPORT_F;
  rsp_ptr->diag_serv_fetch_type.full_evt.event.evt_id = evt.id;
  rsp_ptr->diag_serv_fetch_type.full_evt.length       = evt.evt_length;
#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  rsp_ptr->diag_serv_fetch_type.full_evt.event.ts     = evt.tstamp;
#else
  rsp_ptr->diag_serv_fetch_type.full_evt.event.ts     = QW_CVT_Q2N(tstamp);
#endif

  switch (payload_length_flag)
  {
    case EVENT_PAY_NONE:  /* No payload data */
      break;
    case EVENT_PAY_ONE_BYTE:  /* A byte of payload data, no PAYLOAD_LENGTH */
      offset = rsp_len - sizeof(evt_payload_type);

      /* Do a read from EFS and copy directly into response pointer */
      diag_debug_read_efs(&trace_fd, filename_g, ((uint8*)rsp_ptr)+offset,
                                                   sizeof(evt_payload_type));
      break;
    case EVENT_PAY_TWO_BYTE:  /* Two bytes of payload data, no PAYLOAD_LENGTH */
      offset = rsp_len - (2 * sizeof(evt_payload_type));

      /* Do a read from EFS and copy directly into response pointer */
      diag_debug_read_efs(&trace_fd, filename_g, ((uint8*)rsp_ptr)+offset,
                                              2 * sizeof(evt_payload_type));
      break;
    case EVENT_PAY_LENGTH:  /* One byte PAYLOAD_LENGTH, specifying length */
      offset = rsp_len - sizeof (evt_payload_len_type) - payload_length;
      memcpy( ((uint8*) rsp_ptr)+offset, &payload_length,
                                             sizeof(evt_payload_len_type) );

      /* Get the payload length from the event retrieved */
      offset += sizeof(evt_payload_len_type);

      /* Do a read from EFS and copy directly into response pointer */
      diag_debug_read_efs(&trace_fd, filename_g, ((uint8*)rsp_ptr)+offset,
                                                            payload_length);
      break;
    default:
      MSG_ERROR("Invalid payload length flag %d",payload_length_flag,0,0);
      return NULL;
  }

  /* Now that the rsp_ptr has been set up, ship out to the DM side */
  return rsp_ptr;
}
 
/*===========================================================================

FUNCTION DIAG_DEBUG_FILE_RM_TRACE_HANDLER

DESCRIPTION
  Removes the trace file indicated by the EFS file

DEPENDENCIES
  diag_debug_init must be called prior

RETURN VALUE
  Pointer to the response packet of the req_ptr

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *)
diag_debug_file_rm_trace_handler ( PACK(void *) req_ptr, uint16 pkt_len )
{
  diag_serv_reset_info_rsp_type * rsp_ptr      = NULL;
  diag_serv_file_reset_info_req_type * request = NULL;
  int rsp_len = 0;
  char file[FILENAME_BUF_SIZE];
#ifdef FEATURE_EFS2
  int status = -1;
  uint8 file_index = 0;
#endif

  if (pkt_len < sizeof(diag_serv_reset_info_req_type))
  {
    return diagpkt_err_rsp(DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  request = (diag_serv_file_reset_info_req_type *) req_ptr;

#ifdef FEATURE_EFS2

  file_index = request->file_index;

  (void) std_strlprintf(file, sizeof(file), debug_trace_file_format,
    DEBUG_TRACE_DIR, DEBUG_TRACE_NAME, file_index, ID_FILENAME_EXT);

  /* Remove the Timestamp file */
  (void) efs_unlink(file);

  /* Remove the cross reference file */
  (void) std_strlprintf(file, sizeof(file), debug_trace_file_format,
    DEBUG_TRACE_DIR, DEBUG_TRACE_NAME, file_index, CROSS_FILENAME_EXT);  
  status = efs_unlink(file);

  if (status == -1)
  {
    /* If error, most likely due to the fact that it was still open */
    (void) efs_close(cross_fd);
    status = efs_unlink(file);
  }

  /* Remove the trace file */
  (void) std_strlprintf(file, sizeof(file), debug_trace_file_format,
    DEBUG_TRACE_DIR, DEBUG_TRACE_NAME, file_index, TRACE_FILENAME_EXT);   
  status = efs_unlink(file);
  if (status == -1)
  {
    /* If error, most likely due to the fact that it was still open */
    (void) efs_close(trace_fd);
    status = efs_unlink(file);
  }

  trace_fd = -1;
  cross_fd = -1;
#endif /* FEATURE_EFS2 */

  rsp_len = sizeof (diag_serv_reset_info_rsp_type);

  /* Create response packet */
  rsp_ptr = (diag_serv_reset_info_rsp_type *)
            diagpkt_subsys_alloc( DIAG_SUBSYS_DEBUG,
                                  DIAG_DEBUG_FILE_DEL_TRACE, (uint32)rsp_len);

  if (rsp_ptr == NULL)
  {
    return NULL;
  }

  if (status == -1)
  {
    rsp_ptr->status = (uint32)efs_errno;
  }
  else
  {
    rsp_ptr->status = (uint32)status;
  }

  return rsp_ptr;
}
 
#endif /* FEATURE_SAVE_TRACE_EXTENDED */

#ifdef FEATURE_EFS2
static void
diag_debug_read_efs(int* handle, char* filename, PACK(void*) dest, int num_bytes)
{
  int num_bytes_read = 0;
  int total_bytes_read = 0;

  do {
    /* Try initial read of bytes */
    num_bytes_read = efs_read(*handle, (void*) dest, (uint32)num_bytes);

    if (num_bytes_read < 0) /* Maybe reason it failed because it was closed */
    {
      *handle = efs_open(filename, O_RDONLY);

      if (*handle != -1)
      {
        /* File needed to be opened, so proceed reading */
        num_bytes_read = efs_read(*handle, (void*) dest, (uint32)num_bytes);
      }
      else
      {
        (void) efs_close(*handle);
        break;
      }
    }

    total_bytes_read += num_bytes_read;

  } while (total_bytes_read < num_bytes && num_bytes_read != -1);

  return;
}

/*===========================================================================

FUNCTION DIAG_DEBUG_SEEK_EFS

DESCRIPTION
  Wrapper to seek within the EFS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static int
diag_debug_seek_efs(int* handle, char* filename, int origin, int num_bytes)
{
  int seek_ret_val = 0;

  seek_ret_val = efs_lseek(*handle, num_bytes, origin);

  if (seek_ret_val == -1) /* error occurred */
  {
    /* Maybe the file was closed */
    *handle = efs_open(filename, O_RDONLY);
    seek_ret_val = efs_lseek(*handle, num_bytes, origin);

    if (seek_ret_val == -1)
    {
      /* There are still problems */
      (void) efs_close(*handle);
    }

  }

  return seek_ret_val == -1; /* if 0, successful */
}

#else /* Not FEATURE_EFS2 */
static fs_status_type
diag_debug_read_efs(fs_handle_type* handle, char* filename,
                         PACK(void*) dest, int num_bytes)
{
  fs_rsp_msg_type fs_rsp;
  fs_open_xparms_type fs_open_xparms;
  fs_status_type status;

  fs_read(*handle, dest, num_bytes, NULL, &fs_rsp);

  switch (fs_rsp.read.status)
  {
    case FS_BAD_FILE_HANDLE_S:
    case FS_FILE_NOT_OPEN_S:
      /* Open file */
      fs_open_xparms.create.cleanup_option = FS_OC_CLOSE;
      fs_open_xparms.create.buffering_option = FS_OB_PROHIBIT;
      fs_open_xparms.create.attribute_mask = FS_FA_UNRESTRICTED;
      fs_open(filename, FS_OA_READONLY, &fs_open_xparms, NULL, &fs_rsp);

      *handle = fs_rsp.open.handle;

      fs_read(*handle, dest, num_bytes, NULL, &fs_rsp);
      status = fs_rsp.read.status;
      break;
    case FS_OKAY_S:
      /* File read OK */
      status = fs_rsp.read.status;
      break;
    default:
      /* save the status before you close the file it */
      status = fs_rsp.read.status;

      /* Handle every other status */
      fs_close (*handle, NULL, &fs_rsp);
      break;
  }

  return status;
}

static fs_status_type
diag_debug_seek_efs(fs_handle_type* handle, char* filename,
                    fs_seek_origin_type origin, fs_file_position_type position)
{
  fs_rsp_msg_type fs_rsp;
  fs_open_xparms_type fs_open_xparms;

  fs_seek(*handle, origin, position, NULL, &fs_rsp);

  switch (fs_rsp.seek.status)
  {
    case FS_BAD_FILE_HANDLE_S:
    case FS_FILE_NOT_OPEN_S:
      /* Open file */
      fs_open_xparms.create.cleanup_option = FS_OC_CLOSE;
      fs_open_xparms.create.buffering_option = FS_OB_PROHIBIT;
      fs_open_xparms.create.attribute_mask = FS_FA_UNRESTRICTED;
      fs_open(filename, FS_OA_READONLY, &fs_open_xparms, NULL, &fs_rsp);

      *handle = fs_rsp.open.handle;

      /* Seek in file again */
      fs_seek(*handle, origin, position, NULL, &fs_rsp);
      break;
    case FS_EOF_S:
      /* Close file */
      fs_close(*handle, NULL, &fs_rsp);
      break;
    case FS_OKAY_S:
      /* File read OK */
      break;
    default:
      /* Handle every other status */
      MSG_ERROR("File seek failed: %d", fs_rsp.seek.status,0,0);
      fs_close (*handle, NULL, &fs_rsp);
      return fs_rsp.seek.status;
      break;
  }

  return fs_rsp.seek.status;
}

#endif /* FEATURE_EFS2 */
/*===========================================================================

FUNCTION EVT_RETRIEVE_PREP

DESCRIPTION
  Sets up the event represented by the event record in the circular buffer.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void
evt_retrieve_prep( evt_debug_store_type* evt, int index, char savetime )
{
  int payload_length_flag = 0;

  evt_id_type evt_id                      = 0;
  evt_payload_len_type evt_payload_length = 0;
  evt_length_type evt_length              = 0;
  diag_debug_tstamp_type evt_ts;

  event_id_type event_id;
  uint32 temp_uint32     = 0;

  /*------------------------------------------------------------------*
   * Assume the file pointer to the EFS is already at the correct     *
   * position.                                                        *
   *------------------------------------------------------------------*/

  (void) index; /* to supress compiler warning */

  /* Get the event length.  Not used for anything */
  diag_debug_read_efs(&trace_fd, DEBUG_TRACE_FILE, &evt_length,
                                                     sizeof(evt_length_type));

  /* Need to add bytes to the length.  The length was shortened to
   * account for the timestamp optimization.  However, for the packet
   * to be send out, the entire length will include the full 8 bytes
   * for the timestamp.
   */
  evt_length += OPTIMIZED_TSTAMP_DIFFERENCE;

  /* Get event id */
  diag_debug_read_efs(&trace_fd, DEBUG_TRACE_FILE, &evt_id,
                                                       sizeof(evt_id_type));
#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  if (savetime)
  {
    /* Get event timestamp */
    evt_ts = trc_tstamp[index];

    /* Need to increment the timestamp pointer */
    diag_debug_seek_efs(&trace_fd, DEBUG_TRACE_FILE, SEEK_CUR,
                                      sizeof(diag_debug_tstamp_delta_type));
  }
  else
  {
    /* Need to account for timestamp in event packet length even if not saved
     * in the buffer.
     */
    evt_length += sizeof(diag_debug_tstamp_type);
    evt_ts = 0;
  }
  
#else
  if (savetime)
  {
    /* Get event timestamp */
    diag_debug_read_efs(&trace_fd, DEBUG_TRACE_FILE, &evt_ts,
                                              sizeof(diag_debug_tstamp_type));
  }
  else
  {
    /* Need to account for timestamp in event packet length even if not saved
     * in the buffer.
     */
    evt_length += sizeof(diag_debug_tstamp_type);
    evt_ts.hi = 0;
    evt_ts.lo = 0;
  }
#endif

  temp_uint32 = (uint32) evt_id;
  memcpy(&event_id, &temp_uint32, sizeof(event_id_type));
  payload_length_flag = event_id.payload_len;

  /* Determine if there is a payload length byte */
  switch (payload_length_flag){
    case EVENT_PAY_NONE:
    case EVENT_PAY_ONE_BYTE:
    case EVENT_PAY_TWO_BYTE:
      break;
    case EVENT_PAY_LENGTH:
      diag_debug_read_efs(&trace_fd, DEBUG_TRACE_FILE,
                       &evt_payload_length, sizeof(evt_payload_len_type));
      break;
    default:
      MSG_ERROR("Invalid payload length %d", payload_length_flag,0,0);
      return;
  }

  evt->id = evt_id;
  evt->tstamp = evt_ts;
  evt->payload_length = evt_payload_length;
  evt->evt_length = evt_length;

  return;
}
/*===========================================================================

FUNCTION MSG_RETRIEVE_PREP

DESCRIPTION
  Sets up the msg represented by the msg record in the circular buffer.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void
msg_retrieve_prep( msg_debug_store_type* msg , int savetime, int index)
{
  (void) index; /* to supress compiler warning */
  /*---------------------------------------------------------------*
   * Assume that the file pointer has been moved to the correct    *
   * position in the EFS file.                                     *
   *---------------------------------------------------------------*/

  /* Get the const_data_ptr */
  diag_debug_read_efs(&trace_fd, DEBUG_TRACE_FILE,
                          &(msg->const_data_ptr), sizeof(msg_const_type *));

  if (msg->const_data_ptr == NULL)
  {
    MSG_ERROR("msg_const_type saved incorrectly",0,0,0);
    return;
  }

#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  /* check if you need to retrieve the timestamp from the EFS file */
  if (savetime)
  {
     msg->tstamp = trc_tstamp[index];

     /* Still need to seek past the timestamp */
     diag_debug_seek_efs(&trace_fd, DEBUG_TRACE_FILE, SEEK_CUR,
                                 sizeof(diag_debug_tstamp_delta_type));
  }
  else
  {
    /* zero out timestamp */
    msg->tstamp = 0;
  }
#else
  /* check if you need to retrieve the timestamp from the EFS file */
  if (savetime)
  {
     diag_debug_read_efs(&trace_fd, DEBUG_TRACE_FILE,
                              &(msg->tstamp), sizeof(diag_debug_tstamp_type));
  }
  else
  {
    /* zero out timestamp */
    msg->tstamp.hi = 0;
    msg->tstamp.lo = 0;
  }
#endif

  return;
}

/*===========================================================================

FUNCTION MSG_RETRIEVE_ARGS_PREP

DESCRIPTION
  Sets the arguments in the rsp_ptr according to the arguments saved in the
  circular buffer
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void
msg_retrieve_args_prep(PACK(uint32 *) arg_buffer, int num_args,
                                                 int savevars, int bytevars)
{
  int i = 0;

  /* Set the arguments to put into the message */
  for (i = 0; i < num_args; i++)
  {
    if (savevars == TRUE)
    {
      arg_buffer[i] = 0;
      diag_debug_read_efs(&trace_fd, DEBUG_TRACE_FILE,
                           &(arg_buffer[i]), (int)(sizeof(msg_arg_type) - (uint32)bytevars));
    }
    else
    {
      arg_buffer[i] = 0;
    }
  }

  return;
}

/*===========================================================================
FUNCTION DIAG_DEBUG_INVOKE_CRASH_HANDLER

DESCRIPTION: Diag command for creating different kinds of crash
scenarios. The command uses the err_store_test to cause the crash.
The arguments for the function are the failure mode and delay interval 
(in seconds) after which the crash should be triggered. The different 
failure modes are listed as follows

  1 - soft-reset (branch to zero)      
  2 - HW Watchdog expiry
  4 - Div by zero
  5 - SWI error
  6 - Error Fatal
  7 - Data abort
  8 - Prefetch abort
  9 - Undefined instruction

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  Crashes the system
===========================================================================*/
PACK(void *) diag_debug_invoke_crash_handler (
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{    
#if defined(FEATURE_DIAG_DEBUG_INVOKE_CRASH) && defined(EXTENDED_STORE_TEST)   
    rex_timer_type sleep_timer;
    diag_debug_invoke_crash_req_type *req;
#endif
    diag_debug_invoke_crash_rsp_type *rsp = NULL;
    const int rsp_len = sizeof( diag_debug_invoke_crash_rsp_type );

  (void) req_pkt; /* to supress compiler warning */
  (void) pkt_len; /* to supress compiler warning */

    /* send response immediately, so tool won't timeout */

    rsp = (diag_debug_invoke_crash_rsp_type *) diagpkt_subsys_alloc(
       DIAG_SUBSYS_DEBUG,                       
       DIAG_DEBUG_INVOKE_CRASH,
       rsp_len);

    diagpkt_commit(rsp);

    /* now set timer to cause crash after delay */

#if defined(FEATURE_DIAG_DEBUG_INVOKE_CRASH) && defined(EXTENDED_STORE_TEST)

    req = (diag_debug_invoke_crash_req_type *) req_pkt;

    /* Defines a timer */
    rex_def_timer (&sleep_timer, rex_self(), DIAG_DEBUG_INVOKE_CRASH_SLEEP_SIG);


    /* Clear the signals. The block on the current task is cleared */
    rex_clr_sigs(rex_self(), DIAG_DEBUG_INVOKE_CRASH_SLEEP_SIG);


    /* Wait for sleep_duration. This blocks the current task, and
       different task with which is ready to run starts executing */
    rex_timed_wait(DIAG_DEBUG_INVOKE_CRASH_SLEEP_SIG, &sleep_timer,
                   1000*(req->delay));

    /* Clear the signals. The block on the current task is cleared */
    rex_clr_sigs(rex_self(), DIAG_DEBUG_INVOKE_CRASH_SLEEP_SIG);

    /* call the error function to crash the phone according
    to the failure mode specified */
    err_store_test(req->failure_mode);

   
#endif /* FEATURE_DIAG_DEBUG_INVOKE_CRASH */


    return(NULL);

} /* diag_debug_invoke_crash_handler */


#endif /* FEATURE_ERR_EXTENDED_STORE */
