#ifndef DIAGDEBUG_H

#define DIAGDEBUG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      Diag debug subsystem dispatch handling
                
GENERAL DESCRIPTION
  Packet processing routines for FEATURE_SAVE_DEBUG_TRACE and
  FEATURE_SAVE_DEBUG_TRACE.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2004 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagdebug.h#7 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/03/08   vg      Updated code to use PACK() vs. PACKED
04/24/08   tbg     Created diag_debug_id_type to help id source build for logs
12/19/06   as      Reverted FEATURE_ERR_SMALL_BUFFER change.
12/18/06   as      Added support to decrease buffer size under 
                   FEATURE_ERR_SMALL_BUFFER.
04/06/06   tbg     Allow optional external definitions of default CONTROL and
                   DETAIL values. Only applicable if the feature
                   FEATURE_SAVE_TRACE_ON_BY_DEFAULT is defined.
11/11/05   tbg     Changed naming conventions of diagdebug files.
10/04/05   tbg     Added ability to turn on F3 trace saving by default,
                   if FEATURE_SAVE_TRACE_ON_BY_DEFAULT is defined
08/11/05   eav     Added DIAGDEBUG_TRACE_BUFFER_CORRUPTED
11/30/04   as      Added FEATURE_AUTO_RESET. This feature resets the phone 
                   automatically without user intervention if the phone 
                   encounters a critical failure 
08/26/04   eav     Added support  for timestamp-saving optimizations
05/20/04   eav     Added supp rt to event-tracing.  Added support to put
                   event trace and F3 trace into a single trace buffer.
05/07/04   eav     File created.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "diagpkt.h"
#include "qw.h"

#if defined FEATURE_ERR_EXTENDED_STORE
 
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
extern uint8 diag_debug_control_mask;
extern uint8 diag_debug_detail_mask;

void diag_debug_save_trace_to_efs    ( int );
void diag_debug_save_trace_to_buffer ( uint8*, int );

/* Unified trace buffer constants */
#ifdef FEATURE_SAVE_TRACE_EXTENDED

#ifndef TRACE_BUFFER_SIZE
#define TRACE_BUFFER_SIZE      131072
#endif

#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#else
  #define DEBUG_TRACE_DIR        "err"
  #define DEBUG_TRACE_NAME       "err_f3"      
#endif /* FEATURE_MULTIPROCESSOR */

  #define DEBUG_TRACE_FILE       filename_g
  #define TRACE_FILENAME_EXT     ".f3"
  #define CROSS_FILENAME_EXT     ".crs"
  #define ID_FILENAME_EXT        ".id"
#else
  #define TRACE_BUFFER_SIZE      98304
  #define DEBUG_TRACE_FILE                 "err/dbg_trace.txt"
  #define DEBUG_TRACE_CROSS_REF_FILE       "err/dbg_trace.crs"
#endif

#define DIAGDEBUG_TRACE_BUFFER_CORRUPTED -2
#define DIAGDEBUG_TRACE_FAILURE          -1
#define DIAGDEBUG_SUCCESS                 0

#ifdef FEATURE_SAVE_TRACE_ON_BY_DEFAULT
/* Allow targets to define these values in other header files if needed.
 * Values will not be redefined if they already exist. */
#ifndef DIAG_DEBUG_CONTROL_MASK_DEFAULT_VAL
#define DIAG_DEBUG_CONTROL_MASK_DEFAULT_VAL 0x5
#endif

#ifndef DIAG_DEBUG_DETAIL_MASK_DEFAULT_VAL
#define DIAG_DEBUG_DETAIL_MASK_DEFAULT_VAL  0xFF
#endif

#else
/* Undefine values if FEATURE_SAVE_TRACE_ON_BY_DEFAULT is not enabled
 * and redefine to 0x0. */
#ifdef DIAG_DEBUG_CONTROL_MASK_DEFAULT_VAL
#undef DIAG_DEBUG_CONTROL_MASK_DEFAULT_VAL
#endif
#define DIAG_DEBUG_CONTROL_MASK_DEFAULT_VAL 0x0

#ifdef DIAG_DEBUG_DETAIL_MASK_DEFAULT_VAL
#undef DIAG_DEBUG_DETAIL_MASK_DEFAULT_VAL
#endif
#define DIAG_DEBUG_DETAIL_MASK_DEFAULT_VAL  0x0

#endif

#define MAX_NUM_TRACE_FILES                 50
#define DIAG_DEBUG_MAX_FILES_DEFAULT_VAL    5

#define DIAG_DEBUG_MAX_MSG_RECORD_SIZE 48

#define TRACE_MAX_NUM_RECORDS  TRACE_BUFFER_SIZE/6

#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
  #define MAX_8BIT_VALUE                      255
  #define MAX_16BIT_VALUE                     65535
  #define MAX_24BIT_VALUE                     16777215
  #define DIAG_DEBUG_BYTE_SIZED_ARGS(x) ((x & 0xC) >> 2)
#endif
#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
  #define OPTIMIZED_TSTAMP_DIFFERENCE \
        (sizeof(diag_debug_tstamp_type)-sizeof(diag_debug_tstamp_delta_type))
#else
  #define OPTIMIZED_TSTAMP_DIFFERENCE 2
#endif
#define DIAG_DEBUG_SAVETIME 0x2
#define DIAG_DEBUG_SAVEVARS 0x1

typedef uint16 diag_debug_hdr_type;
#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
typedef uint64 diag_debug_tstamp_type;
typedef int    diag_debug_tstamp_delta_type;
#else
  typedef PACK(struct) {
    uint16 lo;        /* Most significant of the lower 32 bits */
    uint32 hi;        /* Most significant bytes of a qword timestamp */
  } diag_debug_tstamp_type;
#endif

#define DIAG_DEBUG_MAX_ID_LENGTH 40

typedef struct {
  uint64 timestamp;
  uint32 buffer_ptr;
  uint8  build[DIAG_DEBUG_MAX_ID_LENGTH];
} diag_debug_id_type;

typedef uint32 msg_arg_type;

typedef uint16 evt_id_type;
typedef uint16 evt_length_type;
typedef uint8  evt_payload_type;
typedef uint8  evt_payload_len_type;

/* 0, 1 were part of a deprecated F3 trace.  Use 4, 5 instead */
#ifdef FEATURE_SAVE_TRACE_EXTENDED
  enum
  {
    DIAG_DEBUG_DEL_TRACE = 2,   /* Delete type trace file          */
    DIAG_DEBUG_CORE_DUMP,       /* Delete type trace file          */
    DIAG_DEBUG_GET_RANGE,       /* Get number of type saved        */
    DIAG_DEBUG_GET_NEXT,        /* Get next type saved             */
    DIAG_DEBUG_FILE_GET_NAMES,  /* Get names of all trace files       */
    DIAG_DEBUG_FILE_GET_RANGE,  /* Get number of type saved from file */
    DIAG_DEBUG_FILE_GET_NEXT,   /* Get next type saved from file      */
    DIAG_DEBUG_FILE_DEL_TRACE,   /* Delete specified trace file        */
    DIAG_DEBUG_INVOKE_CRASH     /* Invoke crash - for testing auto-reset */
  };
#else
  #define DIAG_DEBUG_DEL_TRACE         2 /* Delete type trace file          */
  #define DIAG_DEBUG_CORE_DUMP         3 /* Delete type trace file          */
  #define DIAG_DEBUG_GET_RANGE         4 /* Get number of type saved        */
  #define DIAG_DEBUG_GET_NEXT          5 /* Get next type saved             */
  #define DIAG_DEBUG_INVOKE_CRASH      10 /* Invoke crash - for testing auto-reset */  
#endif

#define DIAG_DEBUG_MSG_BIT 0x1
#define DIAG_DEBUG_LOG_BIT 0x2
#define DIAG_DEBUG_EVT_BIT 0x4

/* Return the number of events in the stored trace.
 */
typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
} diag_serv_range_req_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
  uint32 status;     /* 0 if no error; otherwise an error code */
  uint32 num_types;   /* Number of events */
} diag_serv_range_rsp_type;

/* Retrieve a range of types.
 */
typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
  uint32 index;      /* Index of event to retrieve     */
} diag_serv_fetch_req_type;

/* Holds all information the DM needs to display an event
 */
typedef PACK(struct) {
  uint16 evt_id;              /* ID of the event sent over       */
  uint64 ts;                  /* FULL timestamp (CDMA format)    */
  /* length and payload follow if included in this event_id      */
} diag_serv_evt_struct_type;

/* Holds all information the DM needs to display a message
 */
typedef PACK(struct) {
  uint64 ts;                  /* Message timestamp (CDMA format) */
  uint8  num_args;            /* Number of arguments             */
  uint16 line;                /* Line number                     */
  uint8  pad1;                /* Pad to align next field         */
  uint32 args[1];             /* Zero or more arguments          */
/* This field is followed by NULL-terminated format and filename
 * strings.
 */
} diag_serv_msg_struct_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
  uint8 failure_mode;
  uint8 delay;
} diag_debug_invoke_crash_req_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
} diag_debug_invoke_crash_rsp_type;

#ifdef FEATURE_SAVE_TRACE_EXTENDED

/* Holds all information the DM needs to display a message.  Conforms
 * to the Extended Message packet format described in the DMSS-SICD,
 * section 3.4.121.
 */
typedef PACK(struct) {
  uint8  cmd_code;            /* 121 (0x79)                      */
  uint8  ts_type;             /* indicates format of the TIME field */
  uint8  num_args;            /* Number of arguments             */
  uint8  drop_cnt;            /* Number of messages dropped      */
  uint64 ts;                  /* Message timestamp (CDMA format) */
  uint16 line;                /* Line number                     */
  uint16 ss_id;
  uint32 ss_mask;
  uint32 args[1];             /* Zero or more arguments          */
/* This field is followed by NULL-terminated format and filename
 * strings.
 */
} diag_serv_full_msg_struct_type;

/* Holds all information the DM needs to display an event.  Conforms
 * to the event packet format described in the DMSS-ICD, section 3.4.95.
 */
typedef PACK(struct) {
  uint8  cmd_code;                       /* 96 (0x60)            */
  uint16 length;                         /* Length of packet     */
  diag_serv_evt_struct_type event;
} diag_serv_full_evt_struct_type;

/* Return a list of all F3 filenames
 */
typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
  uint32 status;
  uint8  num_files;
  uint64 timestamps[1];
} diag_serv_file_names_rsp_type;

/* Request the number of records stored in filename.  All subsequent calls
 * of get_next will be retrieved from this filename.  The response to this
 * packet will be diag_serv_range_rsp_type.
 */
typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
  uint8 file_index;                    /* unique file index */
} diag_serv_file_range_req_type;

/* Delete specified trace file by filename index.  Response packet will be
 * the diag_serv_reset_info_rsp_type.
 */
typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
  uint8 file_index;                    /* unique file index */
} diag_serv_file_reset_info_req_type;

#endif /* FEATURE_SAVE_TRACE_EXTENDED */

typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
  uint32 index;      /* Index of retreived event                */
  uint8  type;
  uint32 status;     /* 0 if no error; otherwise an error code  */

  PACK(union) {
    diag_serv_evt_struct_type evt;  /* Retrieved event          */
    diag_serv_msg_struct_type msg;  /* Retrieved message        */
#ifdef FEATURE_SAVE_TRACE_EXTENDED
    diag_serv_full_msg_struct_type full_msg;  /* Retrieved full F3 message */
    diag_serv_full_evt_struct_type full_evt;  /* Retrieved full event      */
#endif
  } diag_serv_fetch_type;

} diag_serv_fetch_rsp_type;

/* Delete all trace information by deleting the trace file.
 */
typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
} diag_serv_reset_info_req_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
  uint32  status;    /* 0 if no error; otherwise an error code  */
} diag_serv_reset_info_rsp_type;

/*===========================================================================

FUNCTION DIAG_DEBUG_TRACE_TO_BUFFER

DESCRIPTION
  Saves the buffer to the RAM buffer containing trace information.

DEPENDENCIES
  None

RETURN VALUE
  If < 0, failure occurred.  Otherwise, success.

SIDE EFFECTS
  None
===========================================================================*/
int diag_debug_trace_to_buffer(uint8* src, int length);

/*===========================================================================

FUNCTION DIAG_DEBUG_TRACE_TIMESTAMP_TO_BUFFER

DESCRIPTION
  Saves the timestamp to the RAM buffer containing trace information.

DEPENDENCIES
  None

RETURN VALUE
  Same as diag_debug_trace_to_buffer return value

SIDE EFFECTS
  None
===========================================================================*/
int diag_debug_trace_timestamp_to_buffer(qword tstamp);

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
void diag_debug_inc_trace_record_count( void );
#endif /* #if defined FEATURE_ERR_EXTENDED_STORE */

#endif /* DIAGDEBUG_H */

