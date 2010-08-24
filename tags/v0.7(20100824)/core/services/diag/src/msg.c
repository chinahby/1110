
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Mobile Diagnostic Message Services

General Description
  All the declarations, definitions, and routines necessary to support
  reporting messages for errors and debugging.

Initialzation and Sequencing Requirements
  msg_init() must be called prior to msg_put().

Copyright (c) 1992-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/msg.c#19 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/08   vg      Updated code to use PACK() vs. PACKED
12/15/06   as      Fixed critical lint errors.
08/29/06   as      Added DSP Diag support.
10/31/05   as      Fixed lint errors.
08/15/05   eav     Checked to see if trace buffer contents have been corrupted 
02/18/04   as      Fixed critical lint errors.
11/04/04   as      Added code to set ts_type filed in WCDMA Phones
09/20/04   eav     Rearranged code so that interrupts are locked for a shorter
                   amount of time when saving a trace record to buffer
07/14/04   eav     Fixed LINT errors 
06/09/04   eav     Added support for "savevars" and "savetime" of the 
                   msg_const_type.
05/14/04   eav     Renamed functions and variables to conform to naming
                   convention.
03/23/04   eav     The function msg_save_trace() will save the
                   msg_ext_store_type sent to the msg_send_3 to the EFS.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
02/03/02   as      Added msg_sprintf function to support strings
04/19/02   igt     Added new msg_send() and support functions for enhanced
                   message services
04/30/01   lad     Added msg_format_filename() under T_WINNT for use in off
                   target testing.
04/06/01   lad     Changed use of FEATURE_DIAG_MSP_BUILD to FEATURE_ZREX_TIME.
02/23/01   lad     Made use of diagpkt API and performed cleanup for
10/16/00   jal     Turned on logging for off-target debug targets
06/09/00   lad     Added flags for MSM5000_IRAM_FWD
05/15/00   lad     Changed msg_nice default to 0.
05/10/00   lad     Added load balancing support.
04/11/00   lad     Removed LOCAL from msg_dropped for externalization.
02/28/00   lad     Added support for streaming diag services.
01/26/00   lad     Removed INTLOCK from uint32 ops.  uint32 is atomic in ARM.
03/31/99   lad     Changed dropped msg handling to be correctly noted.
03/25/99    ms     Incorporated optimized MSG routines.
02/14/97   rdh     Used Pascal directive on msg_put to save ROM.
05/19/92   jah     Changed to allow dynamic message filtering
03/03/92   jah     created file, drawn from brassboard err services

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "AEEstd.h"

#include <string.h>     /* Prototype for memcpy() */
#include <stdarg.h>     /* support for variable argument lists */

#ifndef T_MSM5500
  #define MSG_TBL_GEN
#endif

#include "msg.h"        /* Message typedefs and prototypes */

#include "diagcmd.h"        /* diag command codes */
#include "assert.h"
#include "diagbuf.h"        /* diag outpout buffer allocation
                   manager. */
#include "diagi.h"      /* Needed for sleep vote. */
#include "diagpkt.h"
#include "err.h"
#include "msg_pkt_defs.h"
#include "qw.h"         /* definition of qword */
#include "ts.h"         /* Timestamp typedefs and prototypes */
#include <stdio.h>
#include <stdarg.h>  /* Definitions va_start, va_end, etc */
#include "diagcomm.h"

#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
#ifdef FEATURE_EFS2
#include "fs_public.h"
#else
#include "fs.h"
#endif

#include "diagdebug.h"
#include "diagtarget.h"

#endif /* (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE) */

#ifdef FEATURE_WINCE
#error code not present
#else
  #ifdef FEATURE_QUARTZ_135
    #include "quartz/KxMutex.h"
  #else
    #include <KxMutex.h>
  #endif
#endif

#include <stdlib.h> 

#ifdef FEATURE_WINCE
#error code not present
#endif

/* Error (Warning) 628: no argument information provided for function 
  '__va_start() and __va_arg() */
/*lint -save -e{628} */

/* --------------------------------------------------------------------------
** Definitions and Declarations
** ----------------------------------------------------------------------- */

#if defined(IMAGE_MODEM_PROC)
  #if defined(FEATURE_DSPDIAG)
     extern void dspdiag_125_callback (PACK(void *) req_pkt, word req_len);
  #endif
#endif

typedef struct{
  uint32 args[10];
}msg_large_args;

#define MSG_LARGE_ARGS(X) (((msg_large_args *)X)->args)

typedef struct
{
  struct
  {
    uint16 ssid_first;      /* First Subsystem ID for range */
    uint16 ssid_last;       /* Last Subsystem ID for range */
  }
  ssid_ranges[MSG_MASK_TBL_CNT];
}
msg_large_ssid_ranges_type;

#define MSG_LARGE_SSID_RANGE(X) \
   (((msg_large_ssid_ranges_type *) X)->ssid_ranges)


typedef enum
{
  MSG_MASK_BT,          /* Build-time mask */
  MSG_MASK_RT           /* Run-time mask */
} msg_mask_type;

/* Message operating mode.  If a legacy message request packet is used, the message service will build legacy packet formats for MSG_SSID_LEGACY and ignore all other SSIDs. */

static enum
{
  MSG_MODE_STD,         /* Standard operating mode */
  MSG_MODE_TERSE,       /* Terse mode - send the store packet as is
                   (let the tool expand the pointers). */
  MSG_MODE_LEGACY       /* Legacy packet mode */
} msg_mode;

/* Statistics */
static uint32 msg_total;    /* total number of messages logged */
static uint32 msg_dropped;  /* number of dropped messages */
static uint32 msg_drop_delta;   /* number of dropped messages */

#if defined(FEATURE_DIAG_NON_STREAMING)
/* In legacy protocol mode, only legacy (SSID 0) messages are supported */
typedef struct
{
  q_link_type link;     /* Command queue link */
  uint32 drop_cnt;      /* Number of dropped messages so far */
  const msg_const_type *const_blk;  /* Pointer to printf style format string */
  uint32 code1;         /* parameters to go with the format string */
  uint32 code2;
  uint32 code3;
  qword time;           /* Time at which error occurred */
}
msg_q_item_type;

  /* Number of message buffers (of msg_q_item_type) */
#define MSG_BUFS 32

  /* Pool of message buffers */
static msg_q_item_type msg_bufs[MSG_BUFS];

static q_type msg_q;        /* Queue of active messages */
static q_type msg_free_q;   /* Queue of free message buffers */
#endif /* defined(FEATURE_DIAG_NON_STREAMING) */

/* Legacy values for level are used to index this array. Legacy values were:
   #define MSG_LVL_LOW     0
   #define MSG_LVL_MED     1
   #define MSG_LVL_HIGH    2
   #define MSG_LVL_ERROR   3
   #define MSG_LVL_FATAL   4
   #define MSG_LVL_NONE    255 */

static const uint32 msg_legacy_masks[] = {
  MSG_LEGACY_LOW, MSG_LEGACY_MED, MSG_LEGACY_HIGH, MSG_LEGACY_ERROR,
  MSG_LEGACY_FATAL
};

/*lint -save -e{570} */
static const uint32 msg_legacy_rt_masks[] = {
  MSG_LVL_LOW, MSG_LVL_MED, MSG_LVL_HIGH, MSG_LVL_ERROR, MSG_LVL_FATAL
};
/*lint -restore */

unsigned int msg_ssid_count (void);
static boolean msg_get_ssid_masks (msg_mask_type mask_type, uint16 ssid_start,
                   uint16 ssid_end, uint32 * mask);
static boolean msg_set_rt_mask (uint16 ssid, uint32 mask);
void msg_set_all_rt_masks (uint32 mask);
static boolean msg_sleep_vote (void);

#ifndef FEATURE_DIAG_NON_STREAMING
static void msg_expand_and_send_pkt (diag_send_desc_type *send);

static void msg_expand_and_send_legacy_pkt (diag_send_desc_type *desc);
#endif

#define MSG_WCDMA_TIME_FORMAT 2
#define MSG_CDMA_TIME_FORMAT  3

/* Define the symbol that tells this module which time format to use.
*/
#if defined(FEATURE_WCDMA)
#error code not present
#else
#define MSG_TIME_FORMAT MSG_CDMA_TIME_FORMAT
#endif

#if (MSG_TIME_FORMAT == MSG_WCDMA_TIME_FORMAT)
#error code not present
#endif

#if defined(T_WINNT)
#error code not present
#endif

#ifndef MSG_FMT_STR_ARG_SIZE
#define MSG_FMT_STR_ARG_SIZE 100 /* 100 is guess */
#endif


static void msg_get_time ( qword *ts );

#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
static void msg_save_trace (const msg_const_type *,
                            msg_arg_type *,
                            uint8,  
                            msg_ext_store_type *);
#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
static diag_debug_hdr_type msg_set_up_header (int, char, char, char);
#else
static diag_debug_hdr_type msg_set_up_header (int, char, char);
#endif

/*===========================================================================

FUNCTION MSG_GET_NUM_ARGS

DESCRIPTION
  Determines the number of arguments in a given constant data pointer.  It
  scans the fmt field and determines the number of %'s.  The printf conversion
  of "%%" is not meant to be an argument.  Therefore, must do a check so it
  doesn't increase the count of arguments.

DEPENDENCIES
  None

RETURN VALUE
  Number of arguments required in a format string

SIDE EFFECTS
  None

===========================================================================*/
uint8
msg_get_num_args(const msg_const_type * const_data_ptr)
{
  uint8 nargs = 0;
  char* temp;

  if (const_data_ptr)
  {
    temp = (char *) const_data_ptr->fmt;

    while (*temp != NULL)
    {
      if (*temp == '%')
      {
        /* In case the format string contains a %02x */
        while ( *(temp+1) >= '0' && *(temp+1) <= '9') {temp++;}

        switch( *(temp+1) )
        {
          case 'd':
          case 'x':
          case 'X':
          case 'u':
          case 'l':
            nargs++;
            break;
          default:
            break;
        }
      }

      temp++;
    }
  }
  return nargs;
}

/*===========================================================================

FUNCTION MSG_SAVE_TRACE
 
DESCRIPTION
  Extracts arguments based on the constant_data_ptr type and saves it to the
  circular buffer.  If timestamp-saving option is checked, it will be saved
  to the circular buffer as well.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes copied to the buffer.

SIDE EFFECTS
  None

===========================================================================*/
static void
msg_save_trace
(
  const msg_const_type * constant_data_ptr,
  msg_arg_type * args,
  uint8 num_args,
  msg_ext_store_type * msg
)
{
  if (diag_debug_control_mask & DIAG_DEBUG_MSG_BIT) /* Check if msg saving
                                                        should occur */
  {
    int saving_ret_val=0;
    int i = 0, record_offset = 0, num_bytes_of_args = 0;
    uint8 record_buf[DIAG_DEBUG_MAX_MSG_RECORD_SIZE];
    diag_debug_hdr_type record_hdr;
    char savetime = 0, savevars = 0;
    uint8 nargs = 0;  /* The actual number of arguments in the message */
    qword tstamp;     /* Time-stamp to be used with this message       */
#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
    int byte_sized = 3;
    int bytes_to_copy = 0;
#endif

    if (constant_data_ptr == NULL)
    {
      return;
    }

    if (msg)
    {
      /* Get the timestamp from the created F3 message */
      qw_equ(tstamp, msg->hdr.ts);
    }
    else
    {
      /* Message was dropped and not sent to diag.  Generate own ts */
      msg_get_time(&tstamp);                              //lint !e545
    }

    savetime = diag_debug_detail_mask & DIAG_DEBUG_SAVETIME;
    savevars = diag_debug_detail_mask & DIAG_DEBUG_SAVEVARS;

    if (savevars)
    {
      nargs = msg_get_num_args( constant_data_ptr );
      if(nargs > num_args)
      {
        /* Reject message if number of arguments in format string is more
         * than the actual number of arguments passed in.
         */
        return;
      }

#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
      /* See if all the arguments are eligible to store as a single byte */
      for (i = 0; i < nargs; i++)
      {
        /* All arguments must be small in order to be optimized */
        if (args[i] > MAX_8BIT_VALUE)
        {
          byte_sized = 2;  // Save only 2 bytes of the argument
          break;
        }
      }

      if (byte_sized == 2)
      {
        for (i = 0; i < nargs; i++)
        {
          if (args[i] > MAX_16BIT_VALUE)
          {
            byte_sized = 1;
            break;
          }
        }
      }

      if (byte_sized == 1)
      {
        for (i = 0; i < nargs; i++)
        {
          if (args[i] > MAX_24BIT_VALUE)
          {
            byte_sized = 0;
            break;
          }
        }
      }
#endif
    }

    /* Save the number of arguments in this F3 record */
#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
    record_hdr = msg_set_up_header(nargs, savetime, savevars, (char)byte_sized);
#else
    record_hdr = msg_set_up_header(nargs, savetime, savevars);
#endif
    memcpy(record_buf, &record_hdr, sizeof(diag_debug_hdr_type));
    record_offset += (int)sizeof(diag_debug_hdr_type);

    /* Save address of the msg_const_type */
    memcpy(record_buf+record_offset,
                            &constant_data_ptr, sizeof(msg_const_type*));
    record_offset += (int)sizeof(msg_const_type*);
   
#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
    /* Save arguments to the buffer */
    for (i = 0; i < nargs; i++)
    {
      bytes_to_copy = (int)(sizeof(msg_arg_type)-(uint32)byte_sized);
      memcpy(record_buf+record_offset+(i*bytes_to_copy),
                                            &args[i], (uint32)bytes_to_copy);
    }
    num_bytes_of_args += nargs * bytes_to_copy;
#else
    /* Save arguments to the buffer */
    for (i = 0; i < nargs; i++)
    {
      memcpy(record_buf+record_offset+(i*sizeof(msg_arg_type)),
                          &args[i], sizeof(msg_arg_type));
    }

    num_bytes_of_args += nargs*sizeof(msg_arg_type);
#endif

  /**
   *  Save the record to the global buffer atomically
   */
#ifdef FEATURE_WINCE
#error code not present
#else
  INTLOCK();                                                //lint !e525
#endif
  /* Save the beginning part of the record header */
    saving_ret_val = diag_debug_trace_to_buffer((uint8*) record_buf, record_offset);

  /* Save timestamp to buffer */
  if (savetime)
  {
     if (saving_ret_val == DIAGDEBUG_SUCCESS)
     {
       saving_ret_val = diag_debug_trace_timestamp_to_buffer(tstamp);
     }
  }

  /* Save the arguments after the timestamp */
  if (saving_ret_val == DIAGDEBUG_SUCCESS)
  {
    saving_ret_val = diag_debug_trace_to_buffer((uint8*)record_buf+record_offset,
                                                      num_bytes_of_args);
  }

  if (saving_ret_val == DIAGDEBUG_SUCCESS)
  {
    diag_debug_inc_trace_record_count();
  }

#ifdef FEATURE_WINCE
#error code not present
#else
  INTFREE();                                                   //lint !e525
#endif
  }

  return;
}

/*===========================================================================

FUNCTION MSG_SET_UP_HEADER
 
DESCRIPTION
  Creates the header for a F3 message record in the trace buffer.  The
  header structure is as follows:
  Total = 16 bits
    * 3 bits = Record type
    * 5 bits = number of arguments
    * 6 bits = reserved
    * 1 bit  = savetime
    * 1 bit  = savevars

DEPENDENCIES
  None

RETURN VALUE
  Header for the F3 record

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
static diag_debug_hdr_type
msg_set_up_header( int nargs, char savevars, char savetime, char byte_sized)
#else
static diag_debug_hdr_type
msg_set_up_header( int nargs, char savevars, char savetime )
#endif
{
  diag_debug_hdr_type hdr = 0;

  hdr |= (DIAG_DEBUG_MSG_BIT << 13);
  hdr |= (nargs & 0x1F) << 8;
#ifdef FEATURE_OPTIMIZE_TRACE_RECORDS
/*lint -save -e{734} */
  byte_sized = byte_sized << 2;
/*lint -restore */
  hdr |= (byte_sized | savevars | savetime);
#else
  hdr |= (savevars | savetime);
#endif

  return hdr;
}

/*===========================================================================

FUNCTION MSG_SAVE_3
 
DESCRIPTION
  Saves the F3 message to the RAM buffer.  Also called from the ERR_FATAL
  macro.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void msg_save_3(const msg_const_type* const_blk,
                uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3,
                msg_ext_store_type* msg)
{
    msg_arg_type f3_args[3];

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    f3_args[2] = xx_arg3;

    msg_save_trace(const_blk, f3_args, 3, msg);
    return;
}
#endif /* (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE) */

/*===========================================================================

FUNCTION MSG_INIT

DESCRIPTION
  Initialize the message reporting services.

DEPENDENCIES
  diagbuf_init() must be called prior to this call.

RETURN VALUE
  None

SIDE EFFECTS
  Sets the counts to 0, and throws away any buffered messages.

===========================================================================*/
void
msg_init (void)
{
#if defined(FEATURE_DIAG_NON_STREAMING)
  int i;

  /* Initialize the message queues */
  (void) q_init (&msg_q);
  (void) q_init (&msg_free_q);

  /* Place items on msg_free_q */
  for (i = 0; i < MSG_BUFS; i++)
  {
    q_put (&msg_free_q,
       q_link ((q_link_type *) & msg_bufs[i],
           (q_link_type *) & msg_bufs[i].link));
  }

#else

  (void) diagbuf_register_outbound_pkt_handler (DIAG_EXT_MSG_TERSE_F,
                     msg_expand_and_send_pkt);

#endif /* FEATURE_DIAG_NON_STREAMING */

  msg_total = 0;        /* total number of errors reported */
  msg_dropped = 0;      /* total number dropped */
  msg_drop_delta = 0;       /* number dropped since last success */
  msg_mode = MSG_MODE_STD;

  msg_set_all_rt_masks (0);

#ifdef PLATFORM_LTK
#error code not present
#endif

#ifdef FEATURE_WINCE
#error code not present
#endif

  (void) msg_sleep_vote ();
}               /* end of msg_init */

#if !defined (FEATURE_DIAG_NON_STREAMING)
#ifndef MSM5000_IRAM_FWD
static void
msg_get_time(qword * ts)
{

  /* Get timestamp for this message ASAP */
  ts_get (*ts);

  return;
}

/*===========================================================================

FUNCTION MSG_SEND_PREP

DESCRIPTION
  This message performs the common operations used by all messages.
  It allocates, fills in all data except arguments, and returns a pointer
  to the allocated message buffer.  It also handles message statisitics.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
static msg_ext_store_type *
msg_send_prep (const msg_const_type * const_blk, unsigned int num_args)
{
  uint32 rt_mask = 0;
  boolean valid_ssid = FALSE;

  msg_ext_store_type *msg = NULL;
  unsigned int rsp_len =
    FPOS (msg_ext_store_type, args) + num_args * FSIZ (msg_ext_store_type,
                               args[0]);

  /* Check the runtime mask */
  valid_ssid =
    msg_get_ssid_masks (MSG_MASK_RT, const_blk->desc.ss_id,
            const_blk->desc.ss_id, &rt_mask);

  if (valid_ssid && (const_blk->desc.ss_mask & rt_mask))
  {
    /*-----------------------------------------------------------------------
      Get a pointer to a buffer.  If it's a NULL pointer, there aren't any
      free buffers.  No effort is made to save higher priority messages in
      favor of lower priority messages, as message flooding hides enough
      details to make the usable margin on such a feature too small for
      implementation.
    -----------------------------------------------------------------------*/
    msg = (void *) diagbuf_alloc (DIAG_EXT_MSG_TERSE_F, rsp_len, 0);

    if (msg)
    {
/*lint -save -e{545} Suspicious use of & */
      msg_get_time(&(msg->hdr.ts));
/*lint -restore */

#if (MSG_TIME_FORMAT == MSG_WCDMA_TIME_FORMAT)
#error code not present
#else
      msg->hdr.ts_type = MSG_TS_TYPE_CDMA_FULL;
#endif
      
      msg->hdr.num_args = (uint8)num_args;

      msg->hdr.drop_cnt = (unsigned char) ((msg_drop_delta > 255) ? 255 : msg_drop_delta);

      msg_drop_delta = 0;   /* Reset delta drop count */

      /*-----------------------------------------------------------------------
        Set the pointer to the constant blk, to be expanded in DIAG context.
      -----------------------------------------------------------------------*/
      msg->const_data_ptr = const_blk;
    }
    else
    {
      msg_drop_delta++;
      msg_dropped++;        /* count up one more ignored message */
    }
  }

  return msg;
}               /* msg_send_3() */
#endif /* MSM5000_IRAM_FWD */
#endif /* !FEATURE_DIAG_NON_STREAMING */

#ifndef FEATURE_DIAG_NON_STREAMING
/*===========================================================================

FUNCTION MSG_SEND

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send (const msg_const_type * const_blk)
{
  msg_ext_store_type *msg = NULL;
  const unsigned int num_args = 0;  /* # of message arguments */

  msg = msg_send_prep (const_blk, num_args);

  if (msg)
  {
    diagbuf_commit (msg);
  }
#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  if (const_blk->do_save &&
      (diag_debug_detail_mask>>3 & const_blk->desc.ss_mask)
     )
  {
    msg_save_trace(const_blk, NULL, 0, msg);
  }
#endif
}               /* msg_send() */
#else
/* Stub out for non streaming. */
void
msg_send (const msg_const_type * const_blk)
{
  return;
}
#endif /* !FEATURE_DIAG_NON_STREAMING */

#ifndef FEATURE_DIAG_NON_STREAMING
/*===========================================================================

FUNCTION MSG_SEND_1

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 1 argument.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_1 (const msg_const_type * const_blk, uint32 xx_arg1)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 1;  /* # of message arguments */
#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[1];
#endif

  msg = msg_send_prep (const_blk, num_args);

  if (msg)
  {
    /*-----------------------------------------------------------------------
      Store the arguments in the buffer.
    -----------------------------------------------------------------------*/
    args = msg->args;

    args[0] = xx_arg1;

    diagbuf_commit (msg);
  }
#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  if (const_blk->do_save &&
      (diag_debug_detail_mask>>3 & const_blk->desc.ss_mask)
     )
  {
    f3_args[0] = xx_arg1;
    msg_save_trace(const_blk, f3_args, 1, msg);
  }
#endif
}               /* void msg_send_1() */
#else
/* Stub out for non streaming. */
void
msg_send_1 (const msg_const_type * const_blk, uint32 xx_arg1)
{
  return;
}
#endif /* !FEATURE_DIAG_NON_STREAMING */

#ifndef FEATURE_DIAG_NON_STREAMING
/*===========================================================================

FUNCTION MSG_SEND_2

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 2 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_2 (const msg_const_type * const_blk, uint32 xx_arg1, uint32 xx_arg2)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 2;  /* # of message arguments */
#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[2];
#endif

  msg = msg_send_prep (const_blk, num_args);

  if (msg)
  {
    /*-----------------------------------------------------------------------
      Store the arguments in the buffer.
    -----------------------------------------------------------------------*/
    args = MSG_LARGE_ARGS(msg->args); 

    args[0] = xx_arg1;
    args[1] = xx_arg2;

    diagbuf_commit (msg);
  }
#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  if (const_blk->do_save &&
      (diag_debug_detail_mask>>3 & const_blk->desc.ss_mask)
     )
  {
    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;

    msg_save_trace(const_blk, f3_args, 2, msg);
  }
#endif
}               /* msg_send_2() */
#else
/* Stub out for non streaming. */
void
msg_send_2 (const msg_const_type * const_blk, uint32 xx_arg1, uint32 xx_arg2)
{
  return;
}
#endif /* !FEATURE_DIAG_NON_STREAMING */

#ifndef MSM5000_IRAM_FWD
#ifndef FEATURE_DIAG_NON_STREAMING
/*===========================================================================

FUNCTION MSG_SEND_3

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 3 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_3 (const msg_const_type * const_blk, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 3;  /* # of message arguments */

  msg = msg_send_prep (const_blk, num_args);

  if (msg)
  {
    /*-----------------------------------------------------------------------
      Store the arguments in the buffer.
    -----------------------------------------------------------------------*/
    args = MSG_LARGE_ARGS(msg->args); 

    args[0] = xx_arg1;
    args[1] = xx_arg2;
    args[2] = xx_arg3;

    diagbuf_commit (msg);
  }
#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  if (const_blk->do_save &&
      (diag_debug_detail_mask>>3 & const_blk->desc.ss_mask)
     )
  {
    msg_save_3(const_blk, xx_arg1, xx_arg2, xx_arg3, msg);
  }
#endif
}    /* msg_send_3() */
#else /* !defined (FEATURE_DIAG_NON_STREAMING) */
void
msg_send_3 (const msg_const_type * const_blk, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  msg_q_item_type *q_item;

  msg_total++;

  //TODO: Legacy messages will not always be SSID_LEGACY in the future...
  if (msg_mode != MSG_MODE_LEGACY || !const_blk)
  {
    msg_dropped++;
  }
  else
  {
    /* If available, grab a free Q item and place it on the msg Q. */
    q_item = (msg_q_item_type *) q_get (&msg_free_q);

    if (q_item == NULL)
    {
      msg_dropped++;
    }
    else
    {
      ts_get (q_item->time);

      q_item->drop_cnt = msg_dropped;
      q_item->const_blk = const_blk;
      q_item->code1 = xx_arg1;
      q_item->code2 = xx_arg2;
      q_item->code3 = xx_arg3;

      /* Place message on msg Q */
      q_put (&msg_q, &q_item->link);
    }
  }
}
#endif /* !defined (FEATURE_DIAG_NON_STREAMING) */
#endif /* MSM5000_IRAM_FWD */

#ifndef FEATURE_DIAG_NON_STREAMING
/*===========================================================================

FUNCTION MSG_SEND_VAR

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with a variable number of
    arguments, specified by 'num_args'.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_var (const msg_const_type * const_blk, uint32 num_args, ...)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  va_list arg_list;     /* ptr to the variable argument list */
  unsigned int i;
#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[9];  // Assuming at most 9 arguments
#endif

  msg = msg_send_prep (const_blk, num_args);

  if (msg)
  {
    /*-----------------------------------------------------------------------
      Store the arguments in the buffer.
    -----------------------------------------------------------------------*/
    args = MSG_LARGE_ARGS(msg->args);

/*lint -save -e{64,10,78,718,746,628,732,530} */
    /* Initialize variable arguments */
    va_start (arg_list, num_args);
/*lint -restore */

    /* Store arguments from variable list. */
/*lint -save -e{64,10,78,718,746,628,732,530} */
    for (i = 0; i < num_args; i++)
    {
      args[i] = va_arg (arg_list, uint32);
    }               /* for */
/*lint -restore */
    /* Reset variable arguments */
    va_end (arg_list);

    diagbuf_commit (msg);
  }
#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  if (const_blk->do_save &&
      (diag_debug_detail_mask>>3 & const_blk->desc.ss_mask)
     )
  {
/*lint -save -e{64,10,718,746,628} */
    /* Initialize variable arguments */
    va_start (arg_list, num_args);

    /* Store arguments from variable list. */
    for (i = 0; i < num_args; i++)
    {
      f3_args[i] = va_arg (arg_list, uint32);
    }               /* for */

    /* Reset variable arguments */
    va_end (arg_list);
/*lint -restore */

    msg_save_trace(const_blk, f3_args, num_args, msg);
  }
#endif
}               /* msg_send_var() */
#else
/* Stub out for non streaming. */
void
msg_send_var (const msg_const_type * const_blk, uint8 num_args, ...)
{
  return;
}
#endif /* !FEATURE_DIAG_NON_STREAMING */

#ifndef FEATURE_DIAG_NON_STREAMING
/*===========================================================================

FUNCTION MSG_EXPAND_AND_SEND_PKT

DESCRIPTION
  This routine expands the intermediate storage format into the full
  packet format, copying data from the constant block in 'stored_msg'.

DEPENDENCIES
  msg_init() must have been called previously.

RETURN VALUE

===========================================================================*/
typedef enum
{
  MSG_EXPAND_STATE_HDR = 0,
  MSG_EXPAND_STATE_DESC,
  MSG_EXPAND_STATE_ARGS,
  MSG_EXPAND_STATE_FMT,
  MSG_EXPAND_STATE_FMT_NULL,
  MSG_EXPAND_STATE_FNAME,
  MSG_EXPAND_STATE_FNAME_NULL,
  MSG_EXPAND_STATE_COMPLETE
}
msg_expand_enum_type;

static void
msg_expand_and_send_pkt (diag_send_desc_type *desc)
{
  msg_ext_store_type *msg_store = NULL;
  diagbuf_status_enum_type status = DIAGBUF_READY_S;
  
  static diag_send_desc_type send = {NULL, NULL, DIAG_SEND_STATE_START, FALSE};
  static msg_expand_enum_type state_sav = MSG_EXPAND_STATE_HDR; /* Used to store state of process. */

  /* Local copies of static variables for optimized access.  
     This makes this routine persistent but not re-entrant. */
  msg_expand_enum_type state = state_sav;

  int send_pkt_len;
  
  //TODO: Move legacy mode support to diagbuf registration, not here.
  if (msg_mode == MSG_MODE_LEGACY)
  {
     msg_expand_and_send_legacy_pkt (desc);
  }
  else if (desc)
  {
    /* 'decs' is not moved until the entire message is complete.  Therefore,
       it always points to the stored message, in terse format. */
    msg_store = (msg_ext_store_type *) desc->pkt;

    if (desc->state == DIAG_SEND_STATE_START)
    {
      /* Increment state to indicate the packet is now in progress. */
      desc->state++;

      /* Keep stats of total messages */
      msg_total++;

      if (msg_store)
      {
        /* Add dropped messages to the total. */
        msg_total += msg_store->hdr.drop_cnt;
      }
      
      if (state != MSG_EXPAND_STATE_HDR)
      {
        /* This should never happen.  However, if we start over, just kill the
           last message in progress */
        ERR ("New message while in progress", 0, 0, 0);

        desc->pkt = (void *) ((byte *) desc->last + 1);
        desc->state = DIAG_SEND_STATE_COMPLETE;
        state = MSG_EXPAND_STATE_HDR;   /* reset state */
      }
    }

    /* The packet is expanded in fragments.  'state' marks each stage of
       the process. */
    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_HDR)
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set the command code since it was stored as MSG_EXT_TERSE_F. */
        msg_store->hdr.cmd_code = DIAG_EXT_MSG_F;

        /* Set up the send descriptor for this fragment. */
        send.pkt = (void *) &msg_store->hdr;
        send.last = (void *) ((byte *) send.pkt + sizeof (msg_store->hdr) - 1);
        send.state = DIAG_SEND_STATE_START;
        send.terminate = FALSE;
      }

      (void) diagbuf_send_pkt (&send);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
      }
    }

    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_DESC)
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set up the send descriptor for this fragment. */
        send.pkt = (void *) &msg_store->const_data_ptr->desc;
        send.last = (void *) ((byte *) send.pkt + 
          sizeof (msg_store->const_data_ptr->desc) - 1);
        
        send.state = DIAG_SEND_STATE_BUSY;
      }

      (void) diagbuf_send_pkt (&send);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
      }
    }

    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_ARGS)
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set up the send descriptor for this fragment. */
        send.pkt = (void *) msg_store->args;
        send.last = (void *) ((byte *) send.pkt + 
          sizeof (msg_store->args[0]) * msg_store->hdr.num_args - 1);
        
        send.state = DIAG_SEND_STATE_BUSY;
      }

      (void) diagbuf_send_pkt (&send);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
      }
    }

    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_FMT)
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set up the send descriptor for this fragment. */
        send.pkt = (void *) msg_store->const_data_ptr->fmt;

        send_pkt_len = (int)strlen ((char *) send.pkt);

        /* Do not sub 1 to include NULL char */
        send.last = (void *) ((byte *) send.pkt + 
          MIN (MSG_MAX_STRLEN, send_pkt_len));
        
        send.state = DIAG_SEND_STATE_BUSY;
      }

      (void) diagbuf_send_pkt (&send);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
        
        if (*((byte *) send.last) == 0)
        {
          state++; /* skip the NULL term state */
        }
      }
    }

    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_FMT_NULL)
    {
      /* There was a string length overrun in the FNAME state, so terminate
         with a NULL char.  Use the 'terminate' (which is otherwise ignored)
         field in the desc terminator as the NULL and set it to 0. */
      send.last = send.pkt = &desc->terminate;
      desc->terminate = 0;
      
      send.state = DIAG_SEND_STATE_BUSY;
  
      (void) diagbuf_send_pkt (&send);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
      }
    }

    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_FNAME)
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set up the send descriptor for this fragment. */
#if !defined (T_WINNT)
        send.pkt = (void *) msg_store->const_data_ptr->fname;
#else
#error code not present
#endif
        send_pkt_len = (int)strlen ((char *) send.pkt);
        /* Do not sub 1 to include NULL char */
        send.last = (void *) ((byte *) send.pkt + 
          MIN (MSG_MAX_STRLEN, send_pkt_len));
        
        send.state = DIAG_SEND_STATE_BUSY;
        
        if (*((byte *) send.last) == 0)
        {
          send.terminate = TRUE;
        }
      }

      (void) diagbuf_send_pkt (&send);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
        
        if (send.terminate == TRUE)
        {
          state++; /* skip the NULL term state */
        }
      }
    }

    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_FNAME_NULL)
    {
      /* There was a string length overrun in the FNAME state, so terminate
         with a NULL char.  Use the 'terminate' field in the desc tyerminator
         as the NULL and set it to 0. */
      send.last = send.pkt = &desc->terminate;
      desc->terminate = 0;
      
      send.state = DIAG_SEND_STATE_BUSY;
      send.terminate = TRUE;

      (void) diagbuf_send_pkt (&send);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state = MSG_EXPAND_STATE_HDR;
        desc->pkt = (void *) ((byte *) desc->last + 1);
        send.pkt = NULL;
      }
    }

    if (state == MSG_EXPAND_STATE_COMPLETE)
    {
      state = MSG_EXPAND_STATE_HDR;
      desc->pkt = (void *) ((byte *) desc->last + 1);
      desc->state = DIAG_SEND_STATE_COMPLETE;
      send.pkt = NULL;
    }
  }

  /* Commit local copies of persistent data back to static variables. */
  state = state_sav;
  
  return;
}
#endif /* !FEATURE_DIAG_NON_STREAMING */

#ifndef FEATURE_DIAG_NON_STREAMING
/*===========================================================================

FUNCTION MSG_EXPAND_AND_SEND_LEGACY_PKT

DESCRIPTION
  This routine expands the intermediate storage format into the legacy
  message format.

DEPENDENCIES
  msg_init() must have been called previously.

RETURN VALUE

===========================================================================*/
static void
msg_expand_and_send_legacy_pkt (diag_send_desc_type *desc)
{
  const msg_ext_store_type *msg_store;
  const uint32 *args = NULL;
  byte num_args = 0;
  unsigned int i;
  
  static msg_legacy_rsp_type msg;   /* Intermediate storage buffer */
  static diag_send_desc_type send = 
  {
    (void *) &msg,
    (void *) ((byte *) &msg + sizeof (msg) - 1),
    DIAG_SEND_STATE_START,
    TRUE
  };

  if (desc)
  {
    msg_store = (const msg_ext_store_type *) desc->pkt;

    if (desc->state == DIAG_SEND_STATE_START && msg_store)
    {
      /* Increment state to indicate the packet is now in progress. */
      desc->state++;

      /* Keep stats of total messages */
      msg_total++;

      /* Add dropped messages to the total. */
      msg_total += msg_store->hdr.drop_cnt;

      
      /* Set up the send descriptor. */
      send.pkt = (void *) &msg;
      send.state = DIAG_SEND_STATE_START;

      /* Fill in the legacy descriptor. */
      msg.cmd_code = DIAG_MSG_F;
      msg.qty = 1;

      msg.drop_cnt = msg_dropped;
      msg.total_msgs = msg_total;

      /* init to low message */
      msg.level = 0;

      for (i = 0; i < sizeof (msg_legacy_masks) / sizeof (msg_legacy_masks[0]);
           i++)
      {
        if (msg_legacy_masks[i] & msg_store->const_data_ptr->desc.ss_mask)
        {
          msg.level = (byte)i;
          break;
        }
      }

  #if !defined(T_WINNT)
      /* Copy file name and null terminate it. */

      (void) std_strlcpy ((char *) msg.file, msg_store->const_data_ptr->fname,
                      MSG_FILE_LEN);
  #else
#error code not present
  #endif

      /* Copy source file line number */
      msg.line = msg_store->const_data_ptr->desc.line;

      /* Copy format string and null terminate it. */

      (void) std_strlcpy ((char *) msg.fmt, msg_store->const_data_ptr->fmt,
                      MSG_FMT_LEN);

      /* Copy parameters for format string */
      args = MSG_LARGE_ARGS(msg_store->args);

      num_args = msg_store->hdr.num_args;

      /* Copy up to 3 args, depending on what is available */
      msg.code1 = (num_args >= 1) ? args[0] : 0;
      msg.code2 = (num_args >= 2) ? args[1] : 0;
      msg.code3 = (num_args >= 3) ? args[2] : 0;

      /* Get timestamp for this message ASAP */
      qw_equ (msg.time, (unsigned long *) msg_store->hdr.ts);
    }

    (void) diagbuf_send_pkt (&send);

    if (send.state == DIAG_SEND_STATE_COMPLETE)
    {
      send.pkt = NULL;
      desc->pkt = (void *) ((byte *) desc->last + 1);
      desc->state = DIAG_SEND_STATE_COMPLETE;
    }
  }

  return;
}
#endif /* !FEATURE_DIAG_NON_STREAMING */

/*===========================================================================

FUNCTION MSG_GET_SSID_MASKS

DESCRIPTION
  This function will get an SSID as input and return its corresponding
  build time mask or real time mask by accessing the appropriate data
  structure. It returns True/False based on whether the SSID is valid or not.

DEPENDENCIES

RETURN VALUE
  True/False

SIDE EFFECTS
  None.
===========================================================================*/
static boolean
msg_get_ssid_masks (msg_mask_type mask_type, uint16 ssid_start,
            uint16 ssid_end, uint32 * mask)
{
  unsigned int i;
  boolean success = FALSE;

  const msg_mask_tbl_type *tbl = msg_mask_tbl;
  const msg_mask_tbl_type *tbl_last = &msg_mask_tbl[MSG_MASK_TBL_CNT - 1];
  const uint32 *mask_array = NULL;  /* First pass forces a search */

/*-------------------------------------------------------------------------*/

  if (mask && ssid_start <= ssid_end)
  {
    /* Initialize as successful.  If invalid entry found, set to FALSE. */
    success = TRUE;

    for (i = ssid_start; i <= ssid_end; i++)
    {
      /* If not currently matched to a correct table, reset table search. */
      if (tbl > tbl_last || i < tbl->ssid_first || i > tbl->ssid_last)
      {
        tbl = msg_mask_tbl;
        mask_array = NULL;
      }

      /* Table search: if mask_array is not set */
      while (!mask_array && tbl <= tbl_last)
      {
        if (i >= tbl->ssid_first && i <= tbl->ssid_last)
        {
          if (mask_type == MSG_MASK_BT)
          {
            mask_array = tbl->bt_mask_array;
          }
          else
          {
            mask_array = tbl->rt_mask_array;
          }  
        }
        else
        {
          tbl++;        /* Look at next table entry */
        }
      }

      if (mask_array)
      {
        /* Valid SSID found.  Write mask to caller's mask. */
        mask[i - ssid_start] = mask_array[i - tbl->ssid_first];
      }
      else
      {
        /* Invalid SSID.  Indicate failure and continue. */
        mask[i - ssid_start] = 0;
        success = FALSE;    /* At least 1 entry is invalid */
      }
    }
  }

  return (success);
}               /* msg_get_ssid_masks() */

/*===========================================================================

FUNCTION MSG_SET_RT_MASK

DESCRIPTION
  This function will get an SSID and a real time mask as input and set the
  real time mask that corresponds to that ssid to the requested value.
  It returns True/False based on whether the SSID is valid or not.

DEPENDENCIES

RETURN VALUE
  True/False

SIDE EFFECTS
  None.
===========================================================================*/
static boolean
msg_set_rt_mask (uint16 ssid, uint32 mask)
{
  boolean found = FALSE;

  uint32 i;

  for (i = 0; !found && i < MSG_MASK_TBL_CNT; i++)
  {
    if (ssid >= msg_mask_tbl[i].ssid_first
    && ssid <= msg_mask_tbl[i].ssid_last)
    {
      found = TRUE;
      msg_mask_tbl[i].rt_mask_array[ssid - msg_mask_tbl[i].ssid_first] = mask;
    }
  }

  return (found);
}               /* msg_set_rt_mask() */

/*===========================================================================

FUNCTION MSG_SET_ALL_RT_MASKS

DESCRIPTION
  This function will get a real time mask as input and set ALL
  real time masks to the requested value.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void
msg_set_all_rt_masks (uint32 mask)
{
  uint32 i, j;

  for (i = 0; i < MSG_MASK_TBL_CNT; i++)
  {
    for (j = 0;
     j < (uint32)(msg_mask_tbl[i].ssid_last - msg_mask_tbl[i].ssid_first + 1);
     j++)
    {
      msg_mask_tbl[i].rt_mask_array[j] = mask;
    }
  }

  /* Vote for sleep */
  if (mask == 0)
  {
    diag_sleep_vote (DIAG_SLEEP_MSG, TRUE);
  }
  else
  {
    diag_sleep_vote (DIAG_SLEEP_MSG, FALSE);
  }


}               /* msg_set_all_rt_mask() */

/*===========================================================================

FUNCTION MSG_SSID_COUNT

DESCRIPTION
  This function will calculate the number of valid SubSystem IDs for this
  target and return it.

DEPENDENCIES

RETURN VALUE
  The number of valid SSIDs.

SIDE EFFECTS
  None.
===========================================================================*/
unsigned int
msg_ssid_count (void)
{
  unsigned int i, ssid_num = 0;

/*-------------------------------------------------------------------------*/

  for (i = 0; i < MSG_MASK_TBL_CNT; i++)
  {
    ssid_num += msg_mask_tbl[i].ssid_last - msg_mask_tbl[i].ssid_last + 1;
  }

  return (ssid_num);

}               /* msg_ssid_count() */

/*===========================================================================

             Debug Message Service Packet Handler Routines

===========================================================================*/

/* Extended Message Config Request */

/*===========================================================================

FUNCTION MSG_PKT_GET_SSID_RANGES

DESCRIPTION
  This function will return the valid SSID range.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
static void *
msg_pkt_get_ssid_ranges (void *req_pkt, word req_len)
{
  msg_get_ssid_ranges_req_type *req =
    (msg_get_ssid_ranges_req_type *) req_pkt;
  msg_get_ssid_ranges_rsp_type *rsp = NULL;
  unsigned int i = 0, rsp_len = 0;

/*-------------------------------------------------------------------------*/

  if (req && req->sub_cmd == (uint8) MSG_EXT_SUBCMD_GET_RANGES)
  {
    /*-------------------------------------------------------------------------
      calculate number of valid ssids and the length of the responce packet.
    -------------------------------------------------------------------------*/
    rsp_len = FPOS (msg_get_ssid_ranges_rsp_type, ssid_ranges) +
      MSG_MASK_TBL_CNT * FSIZ (msg_get_ssid_ranges_rsp_type, ssid_ranges);

    /*-------------------------------------------------------------------------
      allocate space for the responce packet
    -------------------------------------------------------------------------*/
    rsp = (msg_get_ssid_ranges_rsp_type *)
      diagpkt_alloc (DIAG_EXT_MSG_CONFIG_F, rsp_len);

    if (rsp)
    {
      rsp->sub_cmd = req->sub_cmd;
      rsp->status = TRUE;
      rsp->range_cnt = MSG_MASK_TBL_CNT;
      rsp->rsvd = 0;

      /*-------------------------------------------------------------------------
        make aux_access_ptr point to the end of the msg_get_ssid_ranges_rsp_type
        which is where the SSIDd should start.
      -------------------------------------------------------------------------*/

      for (i = 0; i < MSG_MASK_TBL_CNT; i++)
      {
        MSG_LARGE_SSID_RANGE(rsp->ssid_ranges)[i].ssid_first = msg_mask_tbl[i].ssid_first;
        MSG_LARGE_SSID_RANGE(rsp->ssid_ranges)[i].ssid_last = msg_mask_tbl[i].ssid_last;
      }
    }
  }                

  else
  {
    rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);
}               /* msg_pkt_get_ssid_ranges () */

/*===========================================================================

FUNCTION MSG_PKT_GET_MASKS

DESCRIPTION
  This function will retrieve an array of masks of specified type for the
  range of SSIDs provided.

DEPENDENCIES
  req is not NULL and points to a valid  request packet

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
static void *
msg_pkt_get_masks (void *req_pkt, word req_len, msg_mask_type mask_type)
{
  msg_get_mask_req_type *req = (msg_get_mask_req_type *) req_pkt;
  msg_get_mask_rsp_type *rsp = NULL;
  unsigned int rsp_len = 0;

/*-------------------------------------------------------------------------*/

  if (req && req->ssid_start <= req->ssid_end &&
      (req->sub_cmd == (uint8) MSG_EXT_SUBCMD_GET_BUILD_MASK ||
       req->sub_cmd == (uint8) MSG_EXT_SUBCMD_GET_RT_MASK))
  {
    rsp_len = FPOS (msg_get_mask_rsp_type, bld_mask) +
      ((req->ssid_end - req->ssid_start + 1) *
       FSIZ (msg_get_mask_rsp_type, bld_mask));

    rsp = (msg_get_mask_rsp_type *)
      diagpkt_alloc (DIAG_EXT_MSG_CONFIG_F, rsp_len);

    if (rsp)
    {
      rsp->sub_cmd = req->sub_cmd;
      rsp->ssid_start = req->ssid_start;
      rsp->ssid_end = req->ssid_end;
      rsp->pad = 0;

      rsp->status = msg_get_ssid_masks (mask_type, req->ssid_start,
                    req->ssid_end, rsp->bld_mask);
    }
  }
  else
  {
    rsp = (msg_get_mask_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);

}               /* msg_pkt_get_build_mask() */

/*===========================================================================

FUNCTION  MSG_PKT_SET_RT_MASKS

DESCRIPTION
  This function will SET the  value of the "tool" mask for the
  specific SubSystem ID and prepare a responce packet. If the
  SSID is not valid it will return Status = false.

DEPENDENCIES
  req is not NULL and points to a valid  request packet

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
static void *
msg_pkt_set_rt_masks (void *req_pkt, word req_len)
{
  msg_set_rt_mask_req_type *req = (msg_set_rt_mask_req_type *) req_pkt;
  msg_set_rt_mask_rsp_type *rsp = NULL;
  uint32 *mask = NULL;
  unsigned int rsp_len = req_len;   /* Same as request */
  int i;
  boolean success = TRUE;

/*-------------------------------------------------------------------------*/

  if (req && req->sub_cmd == MSG_EXT_SUBCMD_SET_RT_MASK &&
      req->ssid_start <= req->ssid_end)
  {
    rsp = (msg_set_rt_mask_rsp_type *)
      diagpkt_alloc (DIAG_EXT_MSG_CONFIG_F, rsp_len);

    if (rsp)
    {
      memcpy ((void *) rsp, (void *) req, rsp_len);

      rsp->status = TRUE;   /* Initialize */

      mask = req->rt_mask;

      for (i = req->ssid_start; i <= req->ssid_end; i++)
      {
        success = msg_set_rt_mask ((uint16) i, *mask);

        if (success == FALSE)
        {
          rsp->status = FALSE;
        }

        mask++;         /* Advance to next entry in array */
      }

      (void) msg_sleep_vote ();
    }
  }
  else
  {
    rsp = (msg_set_rt_mask_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);

}               /* msg_pkt_set_rt_masks() */

/*===========================================================================

FUNCTION   MSG_PKT_SET_ALL_MASKS

DESCRIPTION
  This function will SET the  values of the "tool" masks for all
  the valid SubSystem IDs and prepare a responce packet.

DEPENDENCIES
  req is not NULL and points to a valid  request packet

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
static void *
msg_pkt_set_all_masks (void *req_pkt, word req_len)
{
  msg_set_all_masks_req_type *req = (msg_set_all_masks_req_type *) req_pkt;
  msg_set_all_masks_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof (msg_set_all_masks_rsp_type);

/*-------------------------------------------------------------------------*/

  if (req && req->sub_cmd == MSG_EXT_SUBCMD_SET_ALL_RT_MASKS)
  {
    rsp = (msg_set_all_masks_rsp_type *)
      diagpkt_alloc (DIAG_EXT_MSG_CONFIG_F, rsp_len);

    /*-------------------------------------------------------------------------
      fill in the fields of the responce packet
    -------------------------------------------------------------------------*/
    rsp->sub_cmd = req->sub_cmd;

    rsp->status = TRUE;
    rsp->rsvd = 0;

    rsp->rt_mask = req->rt_mask;

    /*-------------------------------------------------------------------------
      set all the real time masks to the new requested value.
    -------------------------------------------------------------------------*/
    msg_set_all_rt_masks (req->rt_mask);
  }
  else
  {
    rsp = (msg_set_all_masks_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);

}               /* msg_pkt_set_all_masks() */

/*===========================================================================

FUNCTION MSG_PKT_PROCESS_CONFIG

DESCRIPTION
  This function will figure out the type of extended message request
  by examining the message command and then call the appropriate
  function to handle the request.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACK(void *)
msg_pkt_process_config (PACK(void *) req_pkt, word req_len)
{
  msg_get_ssid_ranges_req_type *req =
    (msg_get_ssid_ranges_req_type *) req_pkt;
  void *rsp = NULL;
  boolean valid_cmd = TRUE;
  boolean send_rsp = TRUE;

/*-------------------------------------------------------------------------*/

  if (req && req->cmd_code == DIAG_EXT_MSG_CONFIG_F)
  {
    /* dispatch acording to subcommand */
    switch (req->sub_cmd)
    {
    case MSG_EXT_SUBCMD_GET_RANGES:

    /*---------------------------------------------------------------------
          This will prepare a packet with all valid SubSystem IDs.
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_ssid_ranges (req, req_len);
      break;

    case MSG_EXT_SUBCMD_GET_BUILD_MASK:

    /*---------------------------------------------------------------------
          Will return a packet with the "build" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_masks (req, req_len, MSG_MASK_BT);
      break;

    case MSG_EXT_SUBCMD_GET_RT_MASK:

    /*---------------------------------------------------------------------
          Returns a packet with the "tool" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_masks (req, req_len, MSG_MASK_RT);

      break;

    case MSG_EXT_SUBCMD_SET_RT_MASK:

    /*---------------------------------------------------------------------
          forces mobile to set the "tool" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_set_rt_masks (req, req_len);
      break;

    case MSG_EXT_SUBCMD_SET_ALL_RT_MASKS:

    /*---------------------------------------------------------------------
          set "tool" masks for all valid SSIDs to a given value.
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_set_all_masks (req, req_len);
      break;

    default:            /* nothing to do */
      rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, req_len);
      valid_cmd = TRUE;
      break;
    }

    if (valid_cmd)
    {
      msg_mode = MSG_MODE_STD;
    }
  }
  else
  {
    rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, req_len);
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


#if defined(IMAGE_MODEM_PROC)
  #if defined(FEATURE_DSPDIAG)
    dspdiag_125_callback (req_pkt, req_len);
  #endif
#endif

/* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp);
    return NULL;
  }
  else
  {
    return ((void *) rsp);
  }
}               /* FUNC msg_pkt_process_config () */

/*===========================================================================

FUNCTION MSG_SLEEP_VOTE

DESCRIPTION
  This function checks the runtime masks for all SSIDs to determine if the
  message service is "off".  If "off", it will vote to the diagnostic task
  that it is OK to sleep.

===========================================================================*/
static boolean
msg_sleep_vote (void)
{
  uint32 i, j;
  boolean ok2sleep = TRUE;

  /* Check status us MSG service and vote for sleep. */
  for (i = 0; ok2sleep && i < MSG_MASK_TBL_CNT; i++)
  {
    for (j = 0; ok2sleep &&
     j < (uint32)(msg_mask_tbl[i].ssid_last - msg_mask_tbl[i].ssid_first + 1);
     j++)
    {
      /* If even 1 SSID RT mask is non-zero, vote no for sleep. */
      if (msg_mask_tbl[i].rt_mask_array[j] != 0)
      {
        ok2sleep = FALSE;
      }
    }
  }

  if (ok2sleep == TRUE)
  {
    diag_sleep_vote (DIAG_SLEEP_MSG, TRUE);
  }
  else
  {
    diag_sleep_vote (DIAG_SLEEP_MSG, FALSE);
  }

  return ok2sleep;
}

/* Legacy MSG service packet handler */

/*===========================================================================

FUNCTION MSG_PKT_LEGACY_REQ

DESCRIPTION
  This procedure processes a request to retreive the accumulated msgs in
  the mobile's msg buffer.

===========================================================================*/
PACK(void *)
msg_pkt_legacy_req (PACK(void *) req_ptr, word pkt_len)
{
#if defined(FEATURE_DIAG_NON_STREAMING)
  msg_q_item_type *q_item = NULL;
#endif
  msg_legacy_req_type *req = (msg_legacy_req_type *) req_ptr;
  msg_legacy_rsp_type *rsp;
  const unsigned int rsp_len = sizeof (msg_legacy_rsp_type);
  boolean send_rsp = TRUE;

  (void) pkt_len; /* suppress compiler warning */

#if defined (MSG_LEVEL) && (MSG_LEVEL != MSG_LVL_NONE)
  /* Suppress msg_file compiler warning. ** This compiler optimizes this to
     nothing, ** but msg_file is now referenced. */
#endif

  /* Legacy values for level are used to index this array. Legacy values were:
     #define MSG_LVL_LOW     0 #define MSG_LVL_MED     1 #define MSG_LVL_HIGH 2
     #define MSG_LVL_ERROR   3 #define MSG_LVL_FATAL   4 #define MSG_LVL_NONE
     255 */
  if (req->msg_level <
      sizeof (msg_legacy_rt_masks) / sizeof (msg_legacy_rt_masks[0]))
  {
    msg_set_all_rt_masks (msg_legacy_rt_masks[req->msg_level]);
  }
  else
  {
    msg_set_all_rt_masks (0);
  }

  (void) msg_sleep_vote ();

  /* Set mode to legacy mode. */
  msg_mode = MSG_MODE_LEGACY;

  /*----------------------------------------------------------------------
    Format the response packet.
  ----------------------------------------------------------------------*/
  rsp = (msg_legacy_rsp_type *) diagpkt_alloc (DIAG_MSG_F, rsp_len);

  if (rsp)
  {
    rsp->total_msgs = msg_total;

    /*----------------------------------------------------
      Since we are streaming, this response is an ACK.
      Set qty to 0.
    ----------------------------------------------------*/
    rsp->qty = 0;

    rsp->drop_cnt = msg_dropped;

#if defined(FEATURE_DIAG_NON_STREAMING)

    q_item = (msg_q_item_type *) q_get (&msg_q);

    if (q_item)
    {
      int idx =
    sizeof (msg_legacy_rt_masks) / sizeof (msg_legacy_rt_masks[0]);

      /* The tools use the qty field this to determine if the message is valid
         or just an ack. */
      rsp->qty = 1;

      rsp->level = 255;     /* default value - should never happen */

      /* attempt to find the msg level */
      while (idx > 0)
      {
    /* idx is initialized to count of array.  idx must be decremented since
       it is 0-based indexing. */
    idx--;

    if (msg_legacy_masks[idx] & q_item->const_blk->desc.ss_mask)
    {
      rsp->level = idx;
      idx = 0;      /* exit loop */
    }
      }

      /* Copy format string and null terminate it. */
      (void) memcpy ((void *) rsp->fmt,
             (void *) q_item->const_blk->fmt, MSG_FMT_LEN - 1);

      rsp->fmt[MSG_FMT_LEN - 1] = '\0';

      /* Copy source file line number */
      rsp->line = q_item->const_blk->desc.line;

      /* Copy file name and null terminate it. */
      (void) memcpy ((void *) rsp->file,
             (void *) q_item->const_blk->fname, MSG_FILE_LEN - 1);
      rsp->file[MSG_FILE_LEN - 1] = '\0';

      /* Copy parameters for format string */
      rsp->code1 = q_item->code1;
      rsp->code2 = q_item->code2;
      rsp->code3 = q_item->code3;

      /* Copy the message's timestamp */
      qw_equ_misaligned (rsp->time, q_item->time);

      /* We're finished copying it, place the item on the free queue */
      q_put (&msg_free_q, &q_item->link);

    }

#endif /* FEATURE_DIAG_NON_STREAMING */

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
    diagpkt_free(rsp);
    return NULL;
  }
  else
  {
    return ((void *) rsp);
  }

}               /* msg_pkt_legacy_req */

#if defined (FEATURE_MSG_IFACE_VIOLATION)
#if !defined (FEATURE_DIAG_NON_STREAMING)
/*===========================================================================

FUNCTION MSG_PUT

DESCRIPTION
  *** Do not call directly; use macro MSG_* defined in msg.h ***

  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_put (const msg_store_type * fmt_ptr,    /* Message store pointer */
     uint32 code1,      /* 1st parameter to go with format string */
     uint32 code2,      /* 2nd parameter to go with format string */
     uint32 code3       /* 3rd parameter to go with format string */
  )
{
  uint32 rt_mask = 0;
  boolean valid_ssid = FALSE;
  uint32 i = 0;
  msg_legacy_rsp_type *msg_ptr;
  const unsigned int length = sizeof (msg_legacy_rsp_type);

  valid_ssid =
    msg_get_ssid_masks (MSG_MASK_RT, MSG_SSID_DFLT,
            MSG_SSID_DFLT, &rt_mask);

  if (valid_ssid && fmt_ptr && (fmt_ptr->level & rt_mask))
  {
    /* Get pointer to a buffer.  If it's a NULL pointer, there aren't any free
       buffers.  No effort is made to save higher priority messages in favor
       of lower priority messages, as message flooding hides enough details
       to make the usable margin on such a feature too small for
       implementation. */
    
    msg_ptr = (msg_legacy_rsp_type *) diagbuf_alloc (DIAG_MSG_F, length, 0);
  
    if (msg_ptr != NULL)
    {
      /* Get timestamp for this message ASAP */
/*lint -save -e{545,740} Suspicious use of & Unusual pointer cast*/
      ts_get ((unsigned long *) &msg_ptr->time);
/*lint -restore */
  
      /* Copy the number of dropped messages at the time of this message */
      msg_ptr->drop_cnt = msg_dropped;
  
      msg_ptr->total_msgs = ++msg_total;
  
      /* Convert to legacy message level values. */
      msg_ptr->level = 0;
  
      for (i = 0; i < sizeof (msg_legacy_masks) / sizeof (msg_legacy_masks[0]);
           i++)
      {
        if (msg_legacy_masks[i] & fmt_ptr->level)
        {
          msg_ptr->level = (byte)i;
          break;
        }
      }
  
#if !defined(T_WINNT)
      /* Copy file name and null terminate it. */
      (void) memcpy ((void *) msg_ptr->file,
           (void *) fmt_ptr->file_ptr, MSG_FILE_LEN - 1);
      msg_ptr->file[MSG_FILE_LEN - 1] = '\0';
#else
#error code not present
#endif /* !T_WINNT */
  
      /* Copy source file line number */
      msg_ptr->line = fmt_ptr->line;
  
      /* Copy format string and null terminate it. */
      (void) memcpy ((void *) msg_ptr->fmt,
           (void *) fmt_ptr->xx_fmt, MSG_FMT_LEN - 1);
  
      msg_ptr->fmt[MSG_FMT_LEN - 1] = '\0';
  
      /* Copy parameters for format string */
      msg_ptr->code1 = code1;
      msg_ptr->code2 = code2;
      msg_ptr->code3 = code3;
  
      /* Fill in unchanging and defaulted data */
      msg_ptr->qty = 1;
  
      /* Enqueue the message */
      diagbuf_commit ((void *) msg_ptr);
  
    }
    else
    {
      msg_dropped++;        /* count up one more ignored message */
    }
  }

}               /* end of msg_put */
#else
void
msg_put (const msg_store_type * fmt_ptr,    /* Message store pointer */
     uint32 code1,      /* 1st parameter to go with format string */
     uint32 code2,      /* 2nd parameter to go with format string */
     uint32 code3       /* 3rd parameter to go with format string */
  )
{
  /* stub */
  return;
}

#endif /* !FEATURE_DIAG_NON_STREAMING */
#endif /* FEATURE_MSG_IFACE_VIOLATION */


#if defined(T_WINNT)
#error code not present
#endif /* T_WINNT */


#if !defined (FEATURE_DIAG_NON_STREAMING)
/*===========================================================================

FUNCTION MSG_SPRINTF

DESCRIPTION

  This will queue a debug message in Extended Message Format.  The message 
  will be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This will build a message sprintf diagnostic Message with var #  
  of parameters.

  Do not call directly; use macro MSG_SPRINTF_* defined in msg.h 
  
  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_sprintf (const msg_const_type * const_blk, ...)
{
  /* Declared union for future use */
  typedef union 
  {
    msg_ext_type ext;
  } msg_sprintf_desc_type;
  
  msg_sprintf_desc_type *msg = NULL;

  char *str = NULL;   /* Used to copy the file name and fmt string to the msg */
  va_list arg_list;   /* ptr to the variable argument list */

  unsigned int int_cnt = 0;           /* Calculate the # args, to allocate buffer */
  unsigned int fname_length = 0;      /* Stores the file name along with '\0'     */
  unsigned int fmt_length = 0;        /* Stores the fmt length,'\0' and arg size  */
  unsigned int total_allocated = 0;   /* Total buffer allocated                   */
  unsigned int fmt_len_available = 0; /* Remaining buffer for format string       */

  fname_length = strlen (const_blk->fname) + 1;

  /* Copy the length of the fmt string along with MSG_LENGTH_OF_ARGS */
  fmt_length = strlen (const_blk->fmt) + 1 + MSG_FMT_STR_ARG_SIZE;
 
  /* Calculate # of arguments to ensure enough space is allocated. */
  int_cnt = sizeof (msg_desc_type) - FSIZ (msg_ext_store_type, const_data_ptr) + 
            fmt_length + fname_length;

  /* Calculates number of uint32 required    */
  int_cnt = (int_cnt + sizeof (uint32) - 1) / sizeof (uint32);

  /*  Allocates the buffer required, fills in the header  */
  msg = (msg_sprintf_desc_type *) msg_send_prep (const_blk, int_cnt);
   
  if (msg)
  {
    /* Queue a debug message in Extended Message Format. */
    msg->ext.hdr.cmd_code = DIAG_EXT_MSG_F;

    /* This function embedds the argument in the string itself. 
    ** Hence the num_args is assigned 0 */
    msg->ext.hdr.num_args = 0;

    /* Copy 'desc'. */
    msg->ext.desc = const_blk->desc;

    /* Copy the format string where the argument list would start.
       Since there are no arguments, the format string starts in the 'args'
       field. */
    str = (char *) msg->ext.args;

    /* Calculate the buffer allocated */
    total_allocated = FPOS (msg_ext_store_type, args) + int_cnt * 
                          FSIZ (msg_ext_store_type, args[0]);

    /* Calculate the buffer left to copy the format string */
    fmt_len_available = total_allocated - (FPOS (msg_ext_type, args) + fname_length);

    if( fmt_len_available < fmt_length)
    {
       fmt_length = fmt_len_available;
    }

    /* Initialize variable argument list */
/*lint -save -e{64,718,746,516} */
    va_start(arg_list, const_blk);
/*lint -restore */

    /* Copy the format string with arguments */
#if defined __GNUC__
    (void) vsnprintf (str, fmt_length, const_blk->fmt, arg_list);
     str[fmt_length-1] = '\0';
#else
    (void) _vsnprintf (str, fmt_length, const_blk->fmt, arg_list);
    str[fmt_length-1] = '\0';
#endif

	/* Reset variable arguments */
    va_end(arg_list);

    /* Move the str pass the format string, strlen excludes the terminal 
    ** NULL hence 1 is added to include NULL. */
    str += strlen((const char *)str) + 1;

    /* Copy the filename */
    std_strlprintf(str, fname_length, (char *)const_blk->fname);

    /* Move the str pass the filename, strlen excludes the terminal NULL
    ** hence 1 is added to include NULL. */
    str += strlen((const char *)str) + 1;

    /* str is now pointing to the byte after the last valid byte. str - msg
     * gives the total length required. Use diagbuf_shorten to release the buffer */
    diagbuf_shorten (msg, (uint32)(str - (char *)msg) );

    diagbuf_commit (msg);
  }

  return;
}  /* msg_sprintf */
#else
/* Stub this out for non-streaming mode */
void msg_sprintf (const msg_const_type * const_blk, ...)
{
  return;
}
#endif /* !defined (FEATURE_DIAG_NON_STREAMING) */
