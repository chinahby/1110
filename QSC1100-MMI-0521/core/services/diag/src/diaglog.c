/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Logging Services

General Description
  Services to support data logging.

Initializing and Sequencing Requirements 
  'log_init()' must be called once during initialization prior to use.

Copyright (c) 1990-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History 

$Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diaglog.c#10 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/06   as      Fixed critical lint errors.
07/13/06   as      Added RPC support for mobileview
05/19/06   as      Added Multi processor modem only support
10/31/05   as      Fixed lint errors.
10/10/05   as      Changed implementation to process configuration files 
                   without sending responses.
09/29/05   as      Added check for NULL pointer after diagbuf allocation.
03/09/05   as      Fixed log_on_demand_register to avoid logcode to become 
                   double registered.
02/10/05   as      Added FEATURE_DEBUG_DIAG_HEAP to debug memory corruption.
03/02/04   as      Corrected the boundary in Log Set Mask Response.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
07/30/02   as      Updated log_get_pkt to support FEATRUE_DIAG_NON_STREAMING.
07/30/02   lad     Added default values for last items for each ID in case
                   they are not defined in diagtune.h.
03/18/02   gr      Modified log_set_timestamp to call log_set_timestamp_ex.
03/15/02   gr      Added code to stuff frame number bits into the timestamp
                   field of the log header for WCDMA.
01/18/02   as      Added implementation for Log On Demand 
01/17/02   lad     Fixed array size for log_mask_offset_tbl[] and
                   log_nice_offset_tbl[].
10/24/01   jal     Cleaned up some compiler warnings.
09/12/01   lad     Fixed log_process_config_pkt to correctly set log masks.
06/27/01   lad     Overhauled logging service to support multiple equipment 
                   IDs and general cleanup.
                   Added FEATURE_LOG_EQUIP_ID_XLATE to allow old tools to
                   set log codes with equip ID != 1 using temp equip ID 1
                   codes.
04/17/01   lad     Added use of log_code_type instead of uint16.
04/06/01   lad     Changed use of FEATURE_DIAG_MSP_BUILD to FEATURE_ZREX_TIME.
02/23/01   lad     Updated for diag re-architecture.
12/05/00   lad     Added new logging API.
11/10/00   jq      Added support for analog logging.
11/02/00   lad     Makeover + API enhancements.
08/31/00   lad     Optimized logging API funcs.
                   Added logmask processing and logmask diag pkt functions.
07/31/00   lad     Added MSM5000_IRAM_FWD for log_submit().
05/22/00   lad     Added valid mask support for extended logmask.
05/15/00   lad     Changed log_nice default to 0.
05/10/00   lad     Added load balancing support.  Updated diagbuf API.
04/17/00   lad     FEATURE_DIAG_V7_LOG_COMPATIBILITY to use this diag with
                   old DMSS targets.
04/11/00   lad     Moved log_status to log.h as INLINE function.
02/28/00   lad     Made changes to use diagbuf allocation manager.
11/05/98   lad     Made necessary changes for ARM port.
05/03/95   rdh     Added log_status function.
08/31/92   gb      DMSS ver.
03/05/91   rdb     Created module. 

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "diagbuf.h"
#include "diagcmd.h"
#include "diagi.h"
#include "diagpkt.h"
#include "diagdiag.h"
#include "diagtarget.h"
#include "diagtune.h"
#include "log.h"

#include "assert.h"
#include "msg.h"
#include "qw.h"
#include "rex.h"
#include "diagcomm.h"

#undef LOG_ZREX_TIME_FORMAT
#undef LOG_WCDMA_TIME_FORMAT
#undef LOG_CDMA_TIME_FORMAT
#if defined(FEATURE_ZREX_TIME)
#error code not present
#elif defined(FEATURE_WCDMA)
#error code not present
#elif defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || \
      defined(FEATURE_HDR)
  #define LOG_CDMA_TIME_FORMAT
#endif

#ifdef LOG_ZREX_TIME_FORMAT
#error code not present
#else
  #ifdef LOG_WCDMA_TIME_FORMAT
#error code not present
  #endif
  #include "ts.h"
#endif

#include <string.h>

/* -------------------------------------------------------------------------
 * Definitions and Declarations
 * ------------------------------------------------------------------------- */
#if defined(T_WINNT)
#error code not present
#endif
typedef PACKED struct
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} log_header_type;
#if defined(T_WINNT)
#error code not present
#endif

#ifndef EQUIP_ID_MAX
   #define EQUIP_ID_MAX 16
#endif

/* -------------------------------------------------------------------------
 * Definitions for last log code per equipment ID.
 * If it is undefined, it is defined to 0.  digatune.h need only to 
 * contain values for included equipment IDs.
 * ------------------------------------------------------------------------- */
#if !defined (LOG_EQUIP_ID_0_LAST_CODE)
#define LOG_EQUIP_ID_0_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_1_LAST_CODE)
#define LOG_EQUIP_ID_1_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_2_LAST_CODE)
#define LOG_EQUIP_ID_2_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_3_LAST_CODE)
#define LOG_EQUIP_ID_3_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_4_LAST_CODE)
#define LOG_EQUIP_ID_4_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_5_LAST_CODE)
#define LOG_EQUIP_ID_5_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_6_LAST_CODE)
#define LOG_EQUIP_ID_6_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_7_LAST_CODE)
#define LOG_EQUIP_ID_7_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_8_LAST_CODE)
#define LOG_EQUIP_ID_8_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_9_LAST_CODE)
#define LOG_EQUIP_ID_9_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_10_LAST_CODE)
#define LOG_EQUIP_ID_10_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_11_LAST_CODE)
#define LOG_EQUIP_ID_11_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_12_LAST_CODE)
#define LOG_EQUIP_ID_12_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_13_LAST_CODE)
#define LOG_EQUIP_ID_13_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_14_LAST_CODE)
#define LOG_EQUIP_ID_14_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_15_LAST_CODE)
#define LOG_EQUIP_ID_15_LAST_CODE 0
#endif


/* -------------------------------------------------------------------------
 * Logging mask implementation details.
 *
 * The logging mask stores a bit for every code within the range specified
 * in log_codes.h.   Each equipment ID has a mask that is represented
 * as an array of bytes.  All of this are listed in an array of bytes
 * of size 'LOG_MASK_SIZE'.  An offset into this array is used to determine
 * the start of the mask associated with a particular equipment ID.
 *
 * The range is inclusive, meaning the beginning (0) and end value
 * ('LOG_EQUIP_ID_xxx_LAST_ITEM') are included in the range.  Therefore, all 
 * equipment IDs have at least 1 byte (range 0-0).
 *
 * 'log_mask' is the mask of bits used to represent the configuration of all
 * log codes.  '1' denotes the code being enabled, '0' denotes disabled.
 *
 * 'log_last_item_tbl' is an array of offsets into log_mask indexed by
 * equipment ID.
 *
 * 'LOG_MASK_ARRAY_INDEX()' determine the index into the mask for a given
 * equipment ID.
 *
 * 'LOG_MASK_BIT_MASK()' gives the bit of the code within its byte in the
 * mask.
 *
 * 'LOG_GET_EQUIP_ID()' retuns the equipment ID of a given log code.
 * 
 * 'LOG_GET_ITEM_NUM()' returns the item number of a given log code.
 *
 * 'log_mask_enabled()' returns non-zero if a code is enabled.
 *
 * 'log_set_equip_id()' sets the equipment ID in a log code.
 *
 * ------------------------------------------------------------------------- */

#define LOG_MASK_ARRAY_INDEX(xx_item) ((xx_item) >> 3)

#define LOG_MASK_BIT_MASK(xx_item) (0x01 << ((xx_item) & 7))

#define LOG_GET_EQUIP_ID(xx_code) ((((log_code_type) (xx_code)) >> 12) & 0x000F)

#define LOG_GET_ITEM_NUM(xx_code) (((log_code_type) (xx_code)) & 0x0FFF)

/* This computes the number of bytes in the log mask array. */
#define LOG_MASK_SIZE (\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_0_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_1_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_2_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_3_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_4_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_5_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_6_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_7_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_8_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_9_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_10_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_11_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_12_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_13_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_14_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_15_LAST_CODE)))

/* This array contains the log masks for all equipment IDs. */
/*lint -save  -e572 */
byte log_mask[LOG_MASK_SIZE];
const byte log_valid_mask[LOG_MASK_SIZE];
/*lint -restore */

/* Lookup table for the number of bits in a given equip ID's mask. */
static const uint16 log_last_item_tbl[LOG_EQUIP_ID_MAX + 1] = {
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_0_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_1_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_2_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_3_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_4_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_5_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_6_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_7_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_8_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_9_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_10_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_11_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_12_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_13_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_14_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_15_LAST_CODE)
};

/* The content of this table are computed in log_init().  'uint16' is used
 * instead of 'int' to reduce data bus cycles to access an element of this
 * array.  The last entry is the offset to the byte following the array.  It
 * is used for range checking. */
uint16 log_mask_offset_tbl[LOG_EQUIP_ID_LAST+1];

/* Log listener support */
typedef struct
{
  diag_log_listener listener;
  void *param; /* User's parameter */
}
log_listener_entry_type;

/*lint -save -e785 */
static diag_searchlist_type log_listeners = {FALSE, 0, 0, 0, 0, 0, NULL};
/*lint -restore */

typedef enum
{
  LOG_INIT_STATE = 0,
  LOG_NORMAL_STATE,
  LOG_FLUSH_STATE, /* Pending flush operation. */
  LOG_PANIC_STATE /* Panic mode flush in progress */

} log_state_enum_type;

static log_state_enum_type log_state = LOG_INIT_STATE;
static void *log_commit_last = NULL; /* Many writers, 1 reader (DIAG) */
static void *log_flush_last = NULL; /* 1 writer, 1 reader (both DIAG) */

#define LOG_DIAGPKT_OFFSET FPOS(diag_log_rsp_type, log)

#ifndef LOG_ON_DEMAND_SIZE
 #define  LOG_ON_DEMAND_SIZE  64
#endif

#ifdef LOG_WCDMA_TIME_FORMAT
#error code not present
#endif

typedef struct{
  log_code_type log_code;           /* stores the log_code */
  log_on_demand log_on_demand_ptr;  /* stores the function pointer */
}log_on_demand_list_item_type;

/* The log_on_demand list holds the entries for log_code and 
 * function pointer. */
static log_on_demand_list_item_type log_on_demand_list[LOG_ON_DEMAND_SIZE];

#ifdef FEATURE_DEBUG_DIAG_HEAP
uint32 log_listener_match_and_free_cnt = 0;
uint32 diag_add_log_listener_malloc_cnt = 0;
uint32 diag_add_log_listener_free_cnt = 0;
#endif /* FEATURE_DEBUG_DIAG_HEAP */


/* -------------------------------------------------------------------------
 * Internal (aka private) function prototypes
 * ------------------------------------------------------------------------- */
static boolean log_status_mask (log_code_type code);
static boolean log_search_mask (unsigned int id, unsigned int item);
static boolean log_mask_enabled (const byte *xx_mask_ptr, unsigned int xx_id,
                                 unsigned int xx_item);
static log_code_type log_set_equip_id (unsigned int code, 
                                       unsigned int equip_id);
static boolean log_config_mask (int code, boolean  flag);
static boolean log_sleep_vote (void);
static void log_service_outbound (diag_send_desc_type *desc);


/* -------------------------------------------------------------------------
 * External (aka public) Function Implementations
 * ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION LOG_ON_DEMAND_LIST_INIT

DESCRIPTION
  Initialize log on demand list.

===========================================================================*/
static void log_on_demand_list_init (void)
{
  int i;
  for (i = 0; i < LOG_ON_DEMAND_SIZE; i++)
  {
    log_on_demand_list[i].log_code = 0;
    log_on_demand_list[i].log_on_demand_ptr = NULL;
  }
}



/*=========================================================================*/
void log_init (void)
{
  int id;

  ASSERT(sizeof(log_header_type) == sizeof(log_hdr_type));
  ASSERT(LOG_EQUIP_ID_LAST <= LOG_EQUIP_ID_MAX);
  
  log_mask_offset_tbl[0] = 0;

  for (id = 0; id < LOG_EQUIP_ID_LAST; id++) {
    log_mask_offset_tbl[id+1] = 1 + log_mask_offset_tbl[id] +
      LOG_MASK_ARRAY_INDEX(log_last_item_tbl[id]);
  }


#if !defined(T_WINNT)
  (void) log_config_mask (-1, FALSE);
  diag_sleep_vote (DIAG_SLEEP_LOG, TRUE);
#else
#error code not present
#endif

  log_on_demand_list_init();

  (void) diagbuf_register_outbound_pkt_handler (DIAG_LOG_F, log_service_outbound);

  /* Now that init is complete, enable normal operation */
  log_state = LOG_NORMAL_STATE;

} /* log_init */



/*===========================================================================

FUNCTION LOG_SET_TIMESTAMP_EX

DESCRIPTION
  This function captures the system time and stores it in the given log record.
===========================================================================*/
static void log_set_timestamp_ex (PACKED void *log_hdr_ptr,
                                  log_time_indicator_type log_time_type)
{
  qword time;
  int ts_set_local = FALSE;

  switch ( log_time_type )
  {
    case LOG_TIME_IND_MSP_E:
#ifdef LOG_ZREX_TIME_FORMAT
#error code not present
#else
      MSG_HIGH( "zREX time format not supported", 0, 0, 0 );
#endif
      break;

    case LOG_TIME_IND_WCDMA_E:
#ifdef LOG_WCDMA_TIME_FORMAT
#error code not present
#else
      MSG_HIGH( "WCDMA time format not supported", 0, 0, 0 );
#endif
      break;

    case LOG_TIME_IND_CDMA_E:
#ifdef LOG_CDMA_TIME_FORMAT
      ts_get(time);
      ts_set_local = TRUE;
#else
      MSG_HIGH( "CDMA time format not supported", 0, 0, 0 );
#endif
      break;

    default:
      /* Unknown time format.
      */
      MSG_HIGH( "Unknown time format %d", log_time_type, 0, 0 );
      break;
  }

  if ( ts_set_local )
  {
/*lint -save -e645 */
    qw_equ_misaligned(((log_header_type *) log_hdr_ptr)->ts, time);
/*lint -restore */
  }

} /* log_set_timestamp_ex */



/*=========================================================================*/
PACKED void * log_alloc ( 
  log_code_type code,
  unsigned int length 
)
{
  diag_log_rsp_type *log_pkt_ptr; /* Pointer to packet being created */
  log_header_type *hdr_ptr = NULL;
  void *return_ptr = NULL;

  if (length <= sizeof(log_header_type))
  {
    MSG_LOW("Alloc invalid length %d", length, 0, 0);
  }
  else if (log_status (code))
  {
    /*------------------------------------------------
     Allocate enough for entire LOG response packet, 
     not just the log given.
    ------------------------------------------------*/
    log_pkt_ptr = (diag_log_rsp_type *) 
      diagbuf_alloc(DIAG_LOG_F, length + LOG_DIAGPKT_OFFSET, 0);
  
    if (log_pkt_ptr != NULL)
    {
      /* Fill in header first. */
/*lint -save -e545 -e740 Suspicious use of & Unusual pointer cast*/
      hdr_ptr = (log_header_type *) &log_pkt_ptr->log;
#if defined(LOG_ZREX_TIME_FORMAT)
#error code not present
#elif defined(LOG_WCDMA_TIME_FORMAT)
#error code not present
#elif defined(LOG_CDMA_TIME_FORMAT)
      log_set_timestamp_ex(hdr_ptr, LOG_TIME_IND_CDMA_E);
#else
  #error "Missing compiler flag"
#endif
      hdr_ptr->len = (uint16)length;
      hdr_ptr->code = code;
    
      /* Fill in top of packet. */
      log_pkt_ptr->more = 0;  
      log_pkt_ptr->len  = (uint16)length;

      return_ptr = (void *) &log_pkt_ptr->log;
/*lint -restore */
    }
    else 
    {
      /* Dropped a log. */
      MSG_LOW("Dropped log 0x%x", code, 0, 0);
    }
  } /* if valid and enabled */

  return return_ptr;

} /* log_alloc */



/*=========================================================================*/
void log_shorten (
  PACKED void *ptr,
  unsigned int length 
)
{
  byte *diag_pkt = (byte *) ptr; /* use byte* for pointer arithmetic */
  diag_log_rsp_type *diag_log;
   
  if (ptr)
  {
    diag_pkt -= LOG_DIAGPKT_OFFSET;
    diag_log = (diag_log_rsp_type *) diag_pkt;

    if (length < diag_log->len)
    {
      diagbuf_shorten ((PACKED void *) diag_log, length + LOG_DIAGPKT_OFFSET);

      /* Set the log packet length to the new length */
      diag_log->len = (uint16)length;

      /* log_set_length takes the log itself, not the log packet */
      log_set_length (ptr, length);
    }
  }
} /* log_shorten */



/*=========================================================================*/
void log_commit (PACKED void *ptr)
{
  if (ptr)
  {
    log_commit_last = (void *) ptr;

    /* Set pointer to begining of diag pkt, not the log */
    ptr = ((byte *) ptr - LOG_DIAGPKT_OFFSET);

    diagbuf_commit (ptr);
  }

  return;
} /* log_commit */



/*=========================================================================*/
void log_free (PACKED void *ptr)
{
  byte *pkt_ptr = (byte *) ptr;

  pkt_ptr -= LOG_DIAGPKT_OFFSET;

  diagbuf_shorten(pkt_ptr, 0);

  diagbuf_commit(pkt_ptr);

} /* log_free */



#ifndef MSM5000_IRAM_FWD /* Flag to use internal RAM */
/*=========================================================================*/
boolean log_submit (PACKED void *ptr)
{
  diag_log_rsp_type *log_pkt_ptr;
  /* The header is common to all logs, and is always at the beginning of the
   * packet. */
  log_header_type *log_ptr = (log_header_type *) ptr;

  boolean bReturnVal = FALSE;

  /* Local vars to avoid use of misaligned variables */
  log_code_type code;
  unsigned int length;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure argument is valid */
  if (log_ptr != NULL) {
    code = log_ptr->code;
    length = log_ptr->len;

    if (length > sizeof(log_header_type)) {

      if (log_status (code))
      {
        log_pkt_ptr = (diag_log_rsp_type *) diagbuf_alloc (DIAG_LOG_F, 
          LOG_DIAGPKT_OFFSET + length, 0);

        if (log_pkt_ptr != NULL) {
          log_pkt_ptr->more = 0;
          log_pkt_ptr->len  = (uint16)length;
/*lint -save -e545 -e685 -e669 Suspicious use of & */
          memcpy (&log_pkt_ptr->log, (void *) ptr, length);
/*lint -restore */

          /* Save address of last comitted log in case flush is needed */
/*lint -save -e545 Suspicious use of & */
          log_commit_last = (void *) log_pkt_ptr->log;
/*lint -restore */

          diagbuf_commit (log_pkt_ptr);

          bReturnVal = TRUE;

        } else {
          /* Dropped a log */
          MSG_LOW("Dropped log 0x%x", code, 0, 0);
        }
      }
    }
  }

  return bReturnVal;

} /* log_submit */
#endif /* !MSM5000_IRAM_FWD */



/*=========================================================================*/
void log_set_length (PACKED void *ptr, unsigned int length)
{
  /* All log packets are required to start with 'log_header_type'. */
  ((log_header_type *) ptr)->len = (uint16) length;

} /* log_set_length */



/*=========================================================================*/
void log_set_code (PACKED void *ptr, log_code_type code)
{
  /* All log packets are required to start with 'log_header_type'. */
  ((log_header_type *) ptr)->code = code;

} /* log_set_code */



/*=========================================================================*/
void log_set_timestamp (PACKED void *log_hdr_ptr)
{
#if defined(LOG_ZREX_TIME_FORMAT)
#error code not present
#elif defined(LOG_WCDMA_TIME_FORMAT)
#error code not present
#elif defined(LOG_CDMA_TIME_FORMAT)
  log_set_timestamp_ex(log_hdr_ptr,LOG_TIME_IND_CDMA_E);
#endif
} /* log_set_timestamp */



/*=========================================================================*/
unsigned int log_get_length (PACKED void *ptr)
{
  log_header_type *log = (log_header_type *) ptr;
  unsigned int length = 0;

  if (log)
  {
    length = log->len;
  }

  return length;
}



/*=========================================================================*/
log_code_type log_get_code (PACKED void *ptr)
{
  log_header_type *log = (log_header_type *) ptr;
  log_code_type code = 0;

  if (log)
  {
    code = (log_code_type) log->code;
  }

  return code;
}

/*===========================================================================

FUNCTION LOG_CONFIG_MASK

DESCRIPTION
  This function selects which log codes are enabled for logging.

RETURN VALUE
  TRUE if the operations was succesful.
  FALSE if an invalid code was specified.

===========================================================================*/
static boolean log_config_mask (
  int code,
    /* Specifies which log code is being referenced. Should be one of
       the enumerated log codes defined in log_codes.h.  -1 means ALL. */

  boolean  flag
    /* Specifies whether to enable (TRUE) or disable (FALSE) logging
       for the specified log code. */
)
{
  boolean return_status = TRUE;

  unsigned int id, item;
  unsigned int index;
  byte mask;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (code == -1) {
  /* code = -1, set all. */

    mask = (flag) ? 0xFF : 0;

    /* -1 means to set the entire mask */
/*lint -save  -e572 */
    for (index = 0; index < LOG_MASK_SIZE; index++) {
/*lint -restore */
       log_mask[index] = mask;
    }
  } else {
    /* Get ID and item from log code */
    id = LOG_GET_EQUIP_ID(code);
    item = LOG_GET_ITEM_NUM(code);

    /* Check if code is valid */
    if ((item <= log_last_item_tbl[id]) && (id <= LOG_EQUIP_ID_LAST)) {
    
      index = log_mask_offset_tbl[id] + LOG_MASK_ARRAY_INDEX(item);
      mask = LOG_MASK_BIT_MASK(item);

      if (flag) { 
        log_mask[index] |= mask;
      } else {
        log_mask[index] &= ~mask;
      }

    } else  {
      return_status = FALSE;
    }
  }

  return return_status;

} /* log_config_mask */

/*=========================================================================*/
void 
log_flush (void)
{
  if (log_state == LOG_NORMAL_STATE)
  {
    /* log_commit_last has multiple writers and is written prior to signalling
       the DIAG task.  This routine, however, is called from DIAG task only.  
       Therefore, DIAG is the only writer for log_flush_last which is
       captured here to mark the last log to send before declaring a flush
       complete.  Once sent, it is sent to NULL by log_service_outbound,
       also by the DIAG task. */
    log_flush_last = log_commit_last;

    if (log_flush_last)
    {
      log_state = LOG_FLUSH_STATE;
    }
  }
}

/*=========================================================================*/
void 
log_panic (void)
{
  if (log_state != LOG_INIT_STATE)
  {
    /* log_commit_last has multiple writers and is written prior to signalling
       the DIAG task.  This routine, however, is called from DIAG task only.  
       Therefore, DIAG is the only writer for log_flush_last which is
       captured here to mark the last log to send before declaring a flush
       complete.  Once sent, it is sent to NULL by log_service_outbound,
       also by the DIAG task. */
    log_flush_last = log_commit_last;

    if (log_flush_last)
    {
      log_state = LOG_PANIC_STATE;
    }
    else
    {
      log_state = LOG_INIT_STATE; /* Disable */
    }
  }
}

/*=========================================================================*/
boolean
log_status (log_code_type code)
{
  boolean status = FALSE;
  
  if (log_state == LOG_NORMAL_STATE || log_state == LOG_FLUSH_STATE)
  {
    /* No payload means the routine just tests for existence of 'code'. */
    status = diag_searchlist_search_all (code, &log_listeners, NULL, NULL);

    if (!status && log_state == LOG_NORMAL_STATE)
    {
      status = log_status_mask (code);
    }
  }

  return status;
}

/*===========================================================================

FUNCTION LOG_STATUS_MASK

DESCRIPTION
  This routine is a wrapper for log_search_mask().  It is used to look up
  the given code in the log mask.
  
RETURN VALUE
  A boolean indicating if the specified log is enabled.
    
===========================================================================*/
static boolean
log_status_mask (log_code_type code)
{
  unsigned int id, item;
  boolean status = FALSE;
  
  id = LOG_GET_EQUIP_ID (code);
  item = LOG_GET_ITEM_NUM (code);

  status = log_search_mask (id, item);

  return status;
}

/*===========================================================================

FUNCTION LOG_LISTENER_NOTIFY

DESCRIPTION
  This routine is called by the searchlist when a matching listener node is
  found.  It is called once per entry.  
  
  This routine is a wrapper to call the registered listeners.
  
===========================================================================*/
static void
log_listener_notify (uint32 seq_num, void *log, diag_searchlist_node_type *node)
{
  log_listener_entry_type *entry;
  
  if (node)
  {
    entry = (log_listener_entry_type *) node->goo;

    if (entry->listener)
    {
      entry->listener (seq_num, log, log_get_length (log), entry->param);
    }
  }
}

/*===========================================================================

FUNCTION LOG_SERVICE_OUTBOUND

DESCRIPTION
  This routine is called by diagbuf when it starts a new DIAG_LOG_F packet.
  This routine handles log listeners as well as sending to the comm layer.
  
  This routine also handles various 'log_state's that are used for 
  preserving listener state while flushing logs to the comm layer.
  
===========================================================================*/
static void
log_service_outbound (diag_send_desc_type *desc)
{
  const diag_log_rsp_type *log;
  log_code_type code;
  diag_send_state_enum_type state;


  if (desc)
  {
    state = desc->state;
    
    if (state == DIAG_SEND_STATE_START)
    {
      if (log_state == LOG_INIT_STATE)
      {
        /* Skip immediately */
        state = DIAG_SEND_STATE_COMPLETE;
      }
      else
      {
        log = (diag_log_rsp_type *) desc->pkt;
        code = log_get_code ((void *) log->log);

        (void) diag_searchlist_search_all (code, 
                                    &log_listeners, 
                                    (void *) log->log, 
                                    log_listener_notify);

        /* Check if this is the last item for a pending flush operation. */
        if (!log_status_mask (code))
        {
          /* Mark complete - don't send to comm layer. */
          state = DIAG_SEND_STATE_COMPLETE;
        }

        /* log_commit_last has multiple writers.  log_commit_last is always 
           captured before committing to the queue.  Therefore, it is always
           ahead of log->log in the queue.  Therefore, one can assume that if 
           this match succeeds, log_commit_last is no longer pointing to a 
           valid item in the queue, and must be cleared. */
        if ((void *) log->log == log_commit_last)
        {
          /* In the off chance DIAG is preempted by a writer of 
             'log_commit_last' between the above check and writing NULL here,
         the side effect is only that the log may be ignored by a flush
             operation when it copies it to log_flush_last.  DIAG is the 
             only reader of this variable, so there is no risk of capturing 
             a corrupt value for log_flush_last when captured from this 
             variable.  This is a tolerable side effect.  The only loss is 
             that any log generated from a preemption here will eb lost if a 
             flush begins before that log is sent.  The tradeoff would be to 
             require INTLOCK around this 'if' block. */
          log_commit_last = NULL;
        }

        if (log_flush_last)
        {
          log_flush_last = NULL;

          /* Flush or panic mode will always disable comm bound logs.
             Panic moves to 'init' state to halt the svc when complete. */
          if (log_state == LOG_FLUSH_STATE)
          {
            (void) log_config_mask (-1, FALSE);
            log_state = LOG_NORMAL_STATE;
          }
          else if (log_state == LOG_PANIC_STATE)
          {
            (void) log_config_mask (-1, FALSE);
            log_state = LOG_INIT_STATE; /* disable logging */
          }
          else
          {
            MSG_HIGH ("flush_last set in wrong state", 0, 0, 0);
//XXX       MSG (MSG_SSID_DIAG, MSG_LEGACY_HIGH, 
//XXX              "flush_last set in wrong state");
          } 
        }
      }
    }

    if (state == DIAG_SEND_STATE_COMPLETE)
    {
      /* Skip this packet - no tool asked for it. */
      desc->pkt = (void *) (((uint8 *) desc->last) + 1);
      desc->state = state;
    }
    else
    {
      /* This routine will transition state even if there if no sending can be 
         done.  Therefore, this routine can assume that state == "start" will
         only happen once. */
      (void) diagbuf_send_pkt (desc);
    }
  }

  return;
}

/*===========================================================================

FUNCTION LOG_LISTENER_MATCH_AND_FREE

DESCRIPTION
  This routine is passed to and called by diag_searchlist_delete() for each 
  node of the search until this routine returns TRUE or the search is 
  exhausted.  This routine is responsible for freeing the 'entry' which 
  was allocated in diag_add_log_listener().
   
===========================================================================*/
static boolean
log_listener_match_and_free (diag_searchlist_node_type *match_node,
                             diag_searchlist_node_type *list_node)
{
  boolean found = FALSE;
  log_listener_entry_type *match = NULL;
  log_listener_entry_type *entry = NULL;

  if (match_node && list_node)
  {
    match = (log_listener_entry_type *) match_node->goo;
    entry = (log_listener_entry_type *) list_node->goo;

    if (match->listener == entry->listener &&
        match->param == entry->param)
    {
      found = TRUE;
      diag_free (entry);

#ifdef FEATURE_DEBUG_DIAG_HEAP
      log_listener_match_and_free_cnt++;
      MSG_FATAL("FREE log_listener_match_and_free_cnt = %d, diag heap used = %d ",
          log_listener_match_and_free_cnt, diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    }
  }

  return found;
}

/*=========================================================================*/
boolean
diag_add_log_listener (const unsigned int log_code,
                       const diag_log_listener listener, void *param)
{
  log_listener_entry_type *entry = NULL;
  boolean success = FALSE;
  
  entry = (log_listener_entry_type *) 
    diag_malloc (sizeof (log_listener_entry_type));

#if defined (DIAG_APPS_PROC)
  diag_add_log_listener_on_modem(log_code, listener, param);
#endif
  
  if (entry)
  {

#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_add_log_listener_malloc_cnt++;
    MSG_FATAL("MALLOC diag_add_log_listener_malloc_cnt = %d, diag heap used = %d ",
        diag_add_log_listener_malloc_cnt, diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    entry->listener = listener;
    entry->param = param;

    success = diag_searchlist_add (&log_listeners, log_code, entry);

    if (success && log_listeners.initialized)
    {
      rex_enter_crit_sect (&log_listeners.crit_sect);
      
      if (log_listeners.count > 0)
      {
        DIAG_SET_MASK (diag_cx_state, DIAG_CX_LOG_LISTEN_S);
      }
      
      rex_leave_crit_sect (&log_listeners.crit_sect);
    }
    else
    {
      /* unable to add, free 'entry' */
      diag_free ((void *) entry);

#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_add_log_listener_free_cnt++;
      MSG_FATAL("FREE diag_add_log_listener_free_cnt = %d, diag heap used = %d ",
        diag_add_log_listener_free_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
    }
  }

  return success;
}

/*=========================================================================*/
boolean
diag_remove_log_listener (const unsigned int log_code,
                          const diag_log_listener listener, void *param)
{
  log_listener_entry_type entry;
  diag_searchlist_node_type node;
  boolean success = FALSE;
  
  entry.listener = listener;
  entry.param = param;
  
  node.key = log_code;
  node.goo = (void *) &entry;

#if defined (DIAG_APPS_PROC)
  diag_remove_log_listener_on_modem(log_code, listener, param);
#endif
  
  success = diag_searchlist_delete (&log_listeners, node,
                                    log_listener_match_and_free);

  if (success && log_listeners.initialized)
  {
    rex_enter_crit_sect (&log_listeners.crit_sect);
    
    if (log_listeners.count == 0)
    {
      DIAG_CLR_MASK (diag_cx_state, DIAG_CX_LOG_LISTEN_S);
    }

    rex_leave_crit_sect (&log_listeners.crit_sect);
  }

  return success;
}

/* -------------------------------------------------------------------------
 * Internal to diag (aka protected) function prototypes
 * ------------------------------------------------------------------------- */
/* Begin packet processing routines */
/*===========================================================================

FUNCTION LOG_PROCESS_CONFIG_PKT

DESCRIPTION
  This procedure processes a request to set the log mask.

  Once again the log service config packet has evolved.  This time it has
  evolved to be able to handle multiple equipment IDs.  In the past, the MSM
  always assumed the equipment ID is 1.  Multi-mode products, different
  technologies can log with the same service using equipment ID to specify the
  technology.  This means we have 16 masks, one for each equipment ID.

  The extended logmask packet was designed using a very odd bit ordering scheme
  that created a lot of confusion.  To alleviate this confusion, the logmask is
  now a byte array, with each bit being ordered LSB to MSB.  Macros are defined
  to perform these operations.

RETURN VALUE
  Pointer to response packet.

  This command effects what type of entries are logged.  
===========================================================================*/
PACKED void * log_process_config_pkt (
  PACKED void *req_pkt_ptr, /* pointer to request packet */
  uint16 pkt_len            /* length of request packet  */
)
{
  DIAG_LOG_CONFIG_F_req_type *req_ptr = (DIAG_LOG_CONFIG_F_req_type *) req_pkt_ptr;
  DIAG_LOG_CONFIG_F_rsp_type *rsp_ptr;
  unsigned int rsp_len = FPOS(DIAG_LOG_CONFIG_F_rsp_type, op_data);

  /* Pointers to unions for operation specific data */
  log_config_op_req_type *req_op_ptr;
  log_config_op_rsp_type *rsp_op_ptr;

  log_config_status_enum_type status = LOG_CONFIG_SUCCESS_S;

  /* Local working variables */
  unsigned int operation = req_ptr->operation;
  unsigned int id = 0; /* equip ID */
  unsigned int last_item;
  byte *mask_rd_ptr = NULL;
  byte *mask_wr_ptr = NULL;
  unsigned int i;
  boolean send_rsp = TRUE;

  /* First, determine the length of the response.
   * Second, allocate the response.
   * Last, fill in the response. */

  req_op_ptr = (log_config_op_req_type *) req_ptr->op_data;

  /* This switch statement computes rsp_len for the operation */
  switch (operation) {
    case LOG_CONFIG_DISABLE_OP:
      /* No response op data */
      break;
      
    case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
      rsp_len += sizeof(log_config_ranges_rsp_type);
      break;

    case LOG_CONFIG_RETRIEVE_VALID_MASK_OP:
      id = req_op_ptr->valid_mask.equip_id;
      if (id >= EQUIP_ID_MAX) {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      rsp_len += FPOS(log_config_valid_mask_rsp_type, mask) +
        (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[id]) + 1) * sizeof(byte);
      break;

    case LOG_CONFIG_SET_MASK_OP:

      id = req_op_ptr->set_mask.code_range.equip_id;
      if (id >= EQUIP_ID_MAX) {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      rsp_len +=  FPOS(log_config_valid_mask_rsp_type, mask) +
        (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[id]) + 1) * sizeof(byte);
      break;

    case LOG_CONFIG_GET_LOGMASK_OP:
      id = req_op_ptr->valid_mask.equip_id;
      if (id >= EQUIP_ID_MAX) {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      rsp_len +=  FPOS(log_config_valid_mask_rsp_type, mask) +
         (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[id]) + 1) * sizeof(byte);
      break;

    default:
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
  } /* end switch */

  rsp_ptr = (DIAG_LOG_CONFIG_F_rsp_type *) diagpkt_alloc(DIAG_LOG_CONFIG_F, rsp_len);

  if(rsp_ptr == NULL)
  {
    return rsp_ptr;
  }

  rsp_ptr->pad[0] = 0;
  rsp_ptr->pad[1] = 0;
  rsp_ptr->pad[2] = 0;

  rsp_ptr->operation = operation;

  rsp_op_ptr = (log_config_op_rsp_type *) rsp_ptr->op_data;

  /* This switch statement actually performs the operation */
  switch (operation) {
    case LOG_CONFIG_DISABLE_OP:
      (void) log_config_mask(-1, FALSE);
      break;

    case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
      for (i = 0; i <= LOG_EQUIP_ID_MAX; i++) {
        rsp_op_ptr->ranges.last_item[i] = (uint32) log_last_item_tbl[i];
      }
      break;

    case LOG_CONFIG_RETRIEVE_VALID_MASK_OP:
      rsp_op_ptr->valid_mask.code_range.equip_id = id;
      rsp_op_ptr->valid_mask.code_range.last_item = log_last_item_tbl[id];

      if (id > LOG_EQUIP_ID_LAST_DEFAULT)
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }

      mask_rd_ptr = (byte *) &log_valid_mask[log_mask_offset_tbl[id]];
      mask_wr_ptr = (byte *) rsp_op_ptr->valid_mask.mask;

      for (i = 0; i <= (unsigned int)
          LOG_MASK_ARRAY_INDEX(log_last_item_tbl[id]); i++) {

        *mask_wr_ptr = *mask_rd_ptr;
        mask_wr_ptr++;
        mask_rd_ptr++;
      }
      break;

    case LOG_CONFIG_SET_MASK_OP:
      rsp_op_ptr->set_mask.code_range.equip_id = id;
      rsp_op_ptr->set_mask.code_range.last_item = log_last_item_tbl[id];

      if (id > LOG_EQUIP_ID_LAST_DEFAULT)
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }

      mask_rd_ptr = (byte *) req_op_ptr->set_mask.mask;
      mask_wr_ptr = &log_mask[log_mask_offset_tbl[id]];
      last_item = MIN(log_last_item_tbl[id], req_op_ptr->set_mask.code_range.last_item);

      for (i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++) {
        *mask_wr_ptr = *mask_rd_ptr;
   
        rsp_op_ptr->set_mask.mask[i] = *mask_rd_ptr;

        mask_wr_ptr++;
        mask_rd_ptr++;
      }
      /* Zero all bits that are after the last item in the set mask */
      mask_wr_ptr--;
        i--;

      /* (The next boundary of 8) - 1 - (last_item) gives the number of bits 
       * at the end of the last byte that are not specified. 
       * This line 0's out the bits that remain in the mask that are not
       * included in last_item. */
/*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
/*lint -save -e817 */
       *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);
       rsp_op_ptr->set_mask.mask[i] = *mask_wr_ptr;

      /* Zero out all items that were not spefcified in the request. */
      for (i = last_item + 1; i <= log_last_item_tbl[id]; i++) {
        (void) log_config_mask(log_set_equip_id(i, id), FALSE);
      }

      for( i = LOG_MASK_ARRAY_INDEX(last_item) + 1; i <= 
           LOG_MASK_ARRAY_INDEX(log_last_item_tbl[id]); i++)
      {
         rsp_op_ptr->set_mask.mask[i] = 0;
      }
      break;

  case LOG_CONFIG_GET_LOGMASK_OP:
      rsp_op_ptr->get_mask.code_range.equip_id = id;
      rsp_op_ptr->get_mask.code_range.last_item = log_last_item_tbl[id];

      if (id > LOG_EQUIP_ID_LAST_DEFAULT)
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      mask_rd_ptr = (byte *) &log_mask[log_mask_offset_tbl[id]];
      mask_wr_ptr = (byte *) rsp_op_ptr->valid_mask.mask;

      for (i = 0; i <= (unsigned int)
          LOG_MASK_ARRAY_INDEX(log_last_item_tbl[id]); i++) {

       *mask_wr_ptr = *mask_rd_ptr;
       mask_wr_ptr++;
       mask_rd_ptr++;
      }
      break;
  default:
      break;

  } /* end switch */

  /* Config may have changed.  Run sleep vote routine. */
  (void) log_sleep_vote ();
  
  rsp_ptr->status = status;

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
    send_rsp = FALSE;
  #endif

/* if diag is not connected, don't send response */
  if (!diagcomm_status())
  {
    send_rsp = FALSE;
  }

/* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp_ptr);
    return NULL;
  }
  else
  {
    return ((void *) rsp_ptr);
  }

} /* log_process_config_pkt */



/*===========================================================================

FUNCTION LOG_PROCESS_EXT_LOGMASK

DESCRIPTION
  This function sets the logmask from the extended logmask bit field specified 
  in the diag_ext_logmask_req_type packet.

  This command effects what type of entries are logged.  
===========================================================================*/
void log_process_ext_logmask (
  PACKED uint8 *logmask_ptr,
  unsigned int  num_bits
)
{
  unsigned int i;
  log_code_type code;
  boolean code_enabled; /* Used to determine if a code is enabled. */

  /*----------------------------------------------------------------------
    For each bit allowed in the logging mask, configure that log condition
    active if the bit is 1 and configure that log condition inactive if
    the bit is 0.
  ----------------------------------------------------------------------*/
  /* Tell lint it is OK to have a constant used as a boolean here */
  /*lint -e506 */
  for(i = 0; i < MIN(((uint32)log_last_item_tbl[1]) + 1, num_bits); i++) {
  /*lint +e506 */
    code_enabled = (logmask_ptr[i >> 3] & (0x80 >> (i & 7))) 
                   ? TRUE : FALSE;

    code = log_set_equip_id(i, 1);

    (void) log_config_mask(code, code_enabled);
  }

  /* Vote on sleep for the logging service. */
  (void) log_sleep_vote ();
  
  /*--------------------------------------------------------------------------
    If this DMSS supports more bits than are given, we should set the rest 
    to 0 to make sure they don't get unsuspected logs.
  --------------------------------------------------------------------------*/
  for (; i < (uint32) (log_last_item_tbl[1] + 1); i++) {
    code = log_set_equip_id(i, 1);

    (void) log_config_mask(code, FALSE);
  }
} /* log_process_ext_logmask */



/*===========================================================================

FUNCTION LOG_PROCESS_EXT_LOGMASK_PKT

DESCRIPTION
  This procedure processes a request to change the logmask.

RETURN VALUE
  Pointer to response packet.

===========================================================================*/
PACKED void * log_process_ext_logmask_pkt (
  PACKED void *req_pkt_ptr, /* pointer to request packet */
  uint16 pkt_len            /* length of request packet  */
)
{
  DIAG_EXT_LOGMASK_F_req_type *req_ptr = (DIAG_EXT_LOGMASK_F_req_type *) req_pkt_ptr;
  DIAG_EXT_LOGMASK_F_rsp_type *rsp_ptr;
  const unsigned int rsp_len = sizeof(DIAG_EXT_LOGMASK_F_rsp_type);

  uint32 i;
  const byte *valid_mask_ptr = NULL;
  boolean enabled;
  boolean send_rsp = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Quick sanity check */
  if (pkt_len != FPOS(DIAG_EXT_LOGMASK_F_req_type, mask) +  
                 (req_ptr->num_bits + 7) / 8) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  if (req_ptr->num_bits == 0) {
    /*------------------------------------------------------- 
      Report the number of valid bits as well as the mask of 
      valid bits for this build
    -------------------------------------------------------*/
    rsp_ptr = (DIAG_EXT_LOGMASK_F_rsp_type *) diagpkt_alloc(DIAG_EXT_LOGMASK_F, rsp_len);

    if(rsp_ptr == NULL)
    {
      MSG_HIGH ("Diagpkt_alloc returned NULL in log_process_ext_logmask_pkt.", 0, 0, 0);
      return(rsp_ptr);
    }

    rsp_ptr->num_valid_bits = log_last_item_tbl[1] + 1;
    
    valid_mask_ptr = &log_valid_mask[log_mask_offset_tbl[1]];

    for (i = 0; i < DIAG_EXT_LOGMASK_NUM_BYTES; i++) {
      rsp_ptr->valid_mask[i] = 0;
    }

    for (i = 0; i < DIAG_EXT_LOGMASK_NUM_BITS; i++) {
      enabled = (log_mask_enabled(log_valid_mask, 1, i)) ? TRUE : FALSE;

      if (enabled) {
        rsp_ptr->valid_mask[i >> 3] |= 0x80 >> (i & 7);
      }
    }
  } else {

   (void) valid_mask_ptr; /* to supress lint (Warning) 550 */

    /* Process the logmask */
    log_process_ext_logmask(&req_ptr->mask[0], req_ptr->num_bits);

    /* Only return a command code */
    rsp_ptr = (DIAG_EXT_LOGMASK_F_rsp_type *) diagpkt_alloc(DIAG_EXT_LOGMASK_F, 
      FPOS(DIAG_EXT_LOGMASK_F_rsp_type, num_valid_bits));
  }

   
  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
    send_rsp = FALSE;
  #endif

/* if diag is not connected, don't send response */
  if (!diagcomm_status())
  {
    send_rsp = FALSE;
  }

/* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp_ptr);
    return NULL;
  }
  else
  {
    return ((void *) rsp_ptr);
  }
} /* log_process_ext_logmask_pkt */



/*===========================================================================

FUNCTION LOG_PROCESS_LEGACY_LOGMASK

DESCRIPTION
  This procedure processes a request to change the logmask.
  Note: This is for the orignal 32-bit logmask that the DM used.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
 
===========================================================================*/
PACKED void * log_process_legacy_logmask (
  PACKED void *req_pkt_ptr,
  uint16 pkt_len
)
{
  uint32 i;                /* index to loop through logmask */
  boolean code_enabled; /* Used to determine if a code is enabled. */

  DIAG_LOGMASK_F_req_type *req_ptr = (DIAG_LOGMASK_F_req_type *) req_pkt_ptr;
  DIAG_LOGMASK_F_rsp_type *rsp_ptr;
  const unsigned int rsp_len = sizeof(DIAG_LOGMASK_F_rsp_type);
  unsigned int mask = req_ptr->mask;
  const uint32 numbits = log_last_item_tbl[1] + 1;

  (void) pkt_len; /* suppress compiler warning */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For each bit allowed in the logging mask, configure that log condition
   * active if the bit is 1 and configure that log condition inactive if
   * the bit is 0. */

  /* Tell lint it is OK to have a constant used as a boolean here */
  /*lint -e506 */
  for( i = 0; i < MIN(numbits, 32); i++ )
  {
  /*lint +e506 */

    code_enabled = (boolean) (mask & 0x01);

    /* Configure logmask */
    (void) log_config_mask(log_set_equip_id(i, 1), code_enabled);

    mask >>= 1;
  }

  /* Set any codes above 32 to zero. */
  for(; i < numbits; i++)
  {
    (void) log_config_mask (log_set_equip_id(i, 1), FALSE);
  }

  /* Vote on sleep for the logging service. */
  (void) log_sleep_vote ();

  rsp_ptr = (DIAG_LOGMASK_F_rsp_type *) diagpkt_alloc (DIAG_LOGMASK_F, rsp_len);
  
  return (rsp_ptr);

} /* log_process_legacy_logmask */



#if defined (FEATURE_DIAG_NON_STREAMING)
/*===========================================================================

FUNCTION LOG_GET_PKT

DESCRIPTION
  This procedure processes a data logging request.

===========================================================================*/
PACKED void * log_get_pkt (
  PACKED void *req,
  uint16 pkt_len
)
{
  diag_log_rsp_type *log = NULL;
  const int rsp_len = FPOS (diag_log_rsp_type, log);

  if (diagbuf_send_single_buffered_pkt () == FALSE)
  {
    /* No log was queued.  Send an empty log response */
    log = (diag_log_rsp_type *) diagpkt_alloc (DIAG_LOG_F, rsp_len);

    if (log)
    {
      log->len = 0;
      log->more = 0;
    }
  }

  return (log);
} /* log_get_pkt */
#endif /* defined (FEATURE_DIAG_NON_STREAMING) */



/* -------------------------------------------------------------------------
 * Internal (aka private) function prototypes
 * ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION LOG_SEARCH_MASK

DESCRIPTION
  This function returns a boolean indicating TRUE if the given ID and 'item'
  denotes a valid and enabled log code.

===========================================================================*/
static boolean log_search_mask (
  unsigned int id, 
  unsigned int item
)
{
  boolean return_val = FALSE;

  /* if valid code val */
  if (log_mask_enabled(log_mask, id, item)) {
    return_val = TRUE;
  }

  return return_val;

} /* log_search_mask */



/*===========================================================================

FUNCTION LOG_MASK_ENABLED
      
DESCRIPTION
  This function returns a boolean indicating if the specified code is enabled.

  The equipment ID and item code are passed in to avoid duplicating the
  calculation within this call.  It has to be done for most routines that call
  this anyways.
   
===========================================================================*/
static boolean log_mask_enabled (
  const byte *mask_ptr, 
  unsigned int id, 
  unsigned int item
)
{
  unsigned int offset, index, mask;
  boolean enabled = FALSE;

  if (id <= LOG_EQUIP_ID_LAST && item <= log_last_item_tbl[id]) {

    offset = log_mask_offset_tbl[id];

    index = LOG_MASK_ARRAY_INDEX(item);

    mask = LOG_MASK_BIT_MASK(item);

    if (mask_ptr[offset + index] & mask) {
      enabled = TRUE;
    }
  } else {
    /* It is expected that this will be compiled out in a target build */
    MSG_LOW("Invalid code given 0x%x", (id << 12) | item, 0, 0);
  }
  return enabled;
} /* log_mask_enabled */



/*===========================================================================

FUNCTION LOG_SET_EQUIP_ID
      
DESCRIPTION
  This function returns the equipment ID of the specified 'code' 

===========================================================================*/
static log_code_type log_set_equip_id (
  unsigned int code,
  unsigned int equip_id
)
{
  ASSERT(equip_id <= LOG_EQUIP_ID_MAX);

  code &= 0x00000FFF;

  equip_id &= 0x0000000F;
    
  equip_id <<= 12;

  return (log_code_type) code | (log_code_type)equip_id;

} /* log_set_equip_id */



/*===========================================================================

FUNCTION LOG_SLEEP_VOTE

DESCRIPTION
  This function checks the configuration of the logging service to determine 
  if the system is allowed to sleep.  All log items must be disabled to 
  allow sleep.

===========================================================================*/
static boolean log_sleep_vote (void)
{
  unsigned int id = 0;
  uint16 item = 0;
  boolean ok2sleep = TRUE;

  /* This is not very efficient, but it works.  It only gets called
     when a config packet is sent, not from any clients.  Therefore,
     runtime efficiency is not a huge issue. */

  for (id = 0; ok2sleep && id < LOG_EQUIP_ID_MAX; id++)
  {
    for (item = 0; ok2sleep && item <= log_last_item_tbl[id]; item++)
    {
      if (log_mask_enabled (log_mask, id, item))
      {
        ok2sleep = FALSE;
      }
    }
  }

  diag_sleep_vote (DIAG_SLEEP_LOG, ok2sleep);

  return ok2sleep;

}


/*===========================================================================

FUNCTION LOG_ON_DEMAND_REGISTER

DESCRIPTION
  This function registers a function pointer to be associated with a 
  log code for logging on demand.  If the external device sends a request
  to trigger this log code, the function will be called.  The logging 
  must be performed by the client of this service.  It will not be 
  performed by the logging service itself.

===========================================================================*/
boolean log_on_demand_register ( 
  log_code_type log_code,
  log_on_demand log_on_demand_ptr
)
{
  int8 i;
  boolean status = FALSE;
  int8 first_avail_idx = -1;
 
  INTLOCK();

  for( i=0; i<LOG_ON_DEMAND_SIZE; i++ )
  {
    if ((first_avail_idx == -1) && (log_on_demand_list[i].log_on_demand_ptr == NULL))
    {
      first_avail_idx = i;
    }
    if(log_on_demand_list[i].log_code == log_code)
    {
      status = TRUE;
      break;
    } 
  }

  if ((status == FALSE) && (first_avail_idx != -1))
  {
    log_on_demand_list[first_avail_idx].log_code = log_code;
    log_on_demand_list[first_avail_idx].log_on_demand_ptr = log_on_demand_ptr;
    status = TRUE;
  }
  
  INTFREE();

  return status; /* buffer full */

}  /* log_on_demand_register */



/*===========================================================================

FUNCTION LOG_ON_DEMAND_UNREGISTER

DESCRIPTION
  This function unregisters the log function 

===========================================================================*/
boolean log_on_demand_unregister ( 
  log_code_type log_code
)
{ 
  int i;
  boolean status = FALSE;
 
  INTLOCK();

  for( i=0; i<LOG_ON_DEMAND_SIZE; i++ )
  {
    if(log_on_demand_list[i].log_code == log_code)
    {
      log_on_demand_list[i].log_code = 0;
      log_on_demand_list[i].log_on_demand_ptr = NULL;
      status = TRUE;
      break;
    }
  }

  INTFREE();

  return status; /* not found */

}  /* log_on_demand_unregister */



/*===========================================================================

FUNCTION LOG_ON_DEMAND_PKT

DESCRIPTION
  This procedure processes a log on demand request. It scans in the 
  log_on_demand_list to match the log_code. If it find the log_code it
  processes the function pointer associated with it.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
 
===========================================================================*/

PACKED void * log_on_demand_pkt (
  PACKED void *req_pkt_ptr,
  uint16 pkt_len
)
{
  DIAG_LOG_ON_DEMAND_F_req_type *req_ptr = 
      (DIAG_LOG_ON_DEMAND_F_req_type *) req_pkt_ptr;
  DIAG_LOG_ON_DEMAND_F_rsp_type *rsp_ptr;
  int i;

  const unsigned int rsp_len = sizeof( DIAG_LOG_ON_DEMAND_F_rsp_type );

  (void) pkt_len; /* suppress compiler warning */
  
  rsp_ptr = (DIAG_LOG_ON_DEMAND_F_rsp_type *) diagpkt_alloc( DIAG_LOG_ON_DEMAND_F, rsp_len );
  
  if( rsp_ptr == NULL ) {
    /* If we can't allocate, diagpkt_err_rsp() can't either. */
    return NULL;
  }

  rsp_ptr->log_code = req_ptr->log_code;

  rsp_ptr->status = LOG_ON_DEMAND_NOT_SUPPORTED_S;

  for( i=0; i<LOG_ON_DEMAND_SIZE; i++ ) {
    if ( ( log_on_demand_list[i].log_on_demand_ptr != NULL )
        && (log_on_demand_list[i].log_code == req_ptr->log_code) ) {
        
      rsp_ptr->status=(*log_on_demand_list[i].log_on_demand_ptr)
            (log_on_demand_list[i].log_code);

    }
  }

  return rsp_ptr;

} /* log_on_demand_pkt */

