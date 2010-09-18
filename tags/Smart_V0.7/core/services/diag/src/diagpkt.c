/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Packet Processing

General Description
  Diagnostic packet processing core routines.

Copyright (c) 1990-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagpkt.c#11 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/11/08   sj      Removed calls to gsmdiag_init and umtsdiag_init for DIAG VU Decoupling
12/15/06   as      Fixed critical lint errors.
04/26/06   as      Moved vocdiag_init() under #ifndef FEATURE_WCDMA_PLT and 
                   GSM PLT flags.
10/31/05   as      Fixed lint errors.
05/19/05   as      Fixed subsystem V2 delayed response length.
04/25/05   as      Fixed comparison in function diagpkt_shorten.
03/23/05   as      Added check to prevent de-referencing NULL pointer in the 
                   diagpkt subsystem allocate functions.
03/09/05   as      Delayed Rsp ID is prevented from wrap-around to 1.
03/14/05   as      Moved diagpkt_init_manual_dispatch_table() call to diag.c
02/10/05   as      Corrected featurization.
02/10/05   as      Added FEATURE_DEBUG_DIAG_HEAP to debug memory corruption.
02/08/05   as      Added FEATURE_WPLT around gsmdiag_init().
12/14/04   sl      Added support for FEATURE_DIAG_MULTI_PORTS.
12/23/04   ec      Added function diagpkt_free()
11/30/04   as      Added FEATURE_AUTO_RESET. This feature resets the phone 
                   automatically without user intervention if the phone 
                   encounters a critical failure 
04/27/04   eav     Added FEATURE_SAVE_DEBUG_TRACE to diagdebug_init()
03/15/04   as      Implemented functions to access fields in delayed response 
                   header
03/05/04   as      Added diag_kick_watchdog() call, to fix diag watchdog timeout
01/21/04   as      Added prldiag_init()
12/16/03   as      Added support for version 2 subsystem packet. This 
                   mechanism will allow the user to request delayed response.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
01/08/03   ma      featurization for GSM PLT
01/07/03   lad     CRC calc for TX moved to diagbuf_send_pkt().
12/09/02   djm     do not hsdiag_init() for WPLT build
11/21/02   lad     Changed response packet queuing implementation to use
                   DIAG heap and a queue in lieu of diagbuf_alloc().
01/21/02   as      Changed diagbuf_blocking_alloc() to diagpkt_alloc().
05/21/01   lad     Moved dispatching routines to diagpkt_dispatch.c.
                   Moved header manipulation from diagbuf.c.
                   Improved opaque header support.
04/30/01   lad     Added implementation of diagpkt_subsys_alloc().
04/06/01   lad     Made diagpkt_dispatch() from diagpkt_process_request() for
                   reuse.
                   Introduced use of diagpkt_cmd_code_type.
02/23/01   lad     Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "diag.h"
#include "diagi.h"
#include "diagbuf.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "diagdiag.h"
#include "diagtarget.h"
#include "msg.h"
#include "dog.h"

#include "assert.h"
#include "rex.h"

#include <stdio.h>
#include <string.h>     /* Prototype for memcpy() */

/* Local structure definitions */
typedef struct
{
  uint8 command_code;
}
diagpkt_hdr_type;

typedef struct
{
  uint8 command_code;
  uint8 subsys_id;
  uint16 subsys_cmd_code;
}
diagpkt_subsys_hdr_type;

typedef struct
{
  uint8 command_code;
  uint8 subsys_id;
  uint16 subsys_cmd_code;
  uint32 status;  
  uint16 delayed_rsp_id;
  uint16 rsp_cnt; /* 0, means one response and 1, means two responses */
}
diagpkt_subsys_hdr_type_v2;

#define DIAGPKT_HDR_PATTERN (0xDEADD00DU)
#define DIAGPKT_OVERRUN_PATTERN (0xDEADU)

typedef struct
{
  q_link_type qlink;

  diag_cmd_rsp rsp_func; /* If !NULL, this is called in lieu of comm layer */
  void *rsp_func_param;

  diagpkt_rsp_type rsp; /* see diagi.h */
}
diagpkt_q_type;

static q_type diagpkt_rsp_q;

static struct
{
   boolean pending;

   diag_cmd_rsp rsp_func;
   void *rsp_func_param;
}
diagpkt_rsp_func_store = {FALSE, NULL, NULL};

/* This macro calculates the corresponding queue item pointer from a packet
   pointer. */
#define DIAGPKT_PKT2QITEM(p) \
  /*lint -save -e{545} */ ((diagpkt_q_type *) (((byte *) p) - FPOS (diagpkt_q_type, rsp.pkt))) /*lint -restore */
#define DIAGPKT_PKT2RSPTYPE(p) (&(DIAGPKT_PKT2QITEM (p)->rsp))
#if !defined (DIAGPKT_MSTR_TBL_SIZE)
#define DIAGPKT_MSTR_TBL_SIZE 128
#endif

/* This is the master dispatch table. */
static const diagpkt_master_table_type
  * diagpkt_master_table[DIAGPKT_MSTR_TBL_SIZE];

#define DIAGPKT_MAX_DELAYED_RSP 0xFFFF

/* delayed_rsp_id 0 represents no delay in the response. Any other number 
   means that the diag packet has a delayed response. */
static uint16 delayed_rsp_id = 1;

/* This macro gets the next delayed respose id. Once it reaches 
   DIAGPKT_MAX_DELAYED_RSP, it stays at DIAGPKT_MAX_DELAYED_RSP */

#define DIAGPKT_NEXT_DELAYED_RSP_ID(x)     \
 ((x < DIAGPKT_MAX_DELAYED_RSP) ? x++ : DIAGPKT_MAX_DELAYED_RSP)

#ifdef FEATURE_DEBUG_DIAG_HEAP
uint32 diagpkt_alloc_cnt = 0;
uint32 diagpkt_subsys_alloc_cnt = 0;
uint32 diagpkt_subsys_alloc_v2_cnt = 0;
uint32 diagpkt_commit_free_cnt = 0;
uint32 diagpkt_rsp_send_free_cnt = 0;
uint32 diagpkt_process_request_cmd_req_queued = 0;
uint32 diagpkt_process_req_fs_close_queued = 0;
uint32 diagpkt_subsys_alloc_fs_close = 0;

uint32 get_rsp_q_cnt(void)
{
  return q_cnt(&diagpkt_rsp_q);
}

extern uint32 diag_heap_used_bytes(void);
extern uint32 get_event_q_cnt(void);

#endif /* FEATURE_DEBUG_DIAG_HEAP */

/* -------------------------------------------------------------------------
 * Internal to diag functions
 * ------------------------------------------------------------------------- */

/*===========================================================================

FUNCTION DIAGPKT_WITH_DELAY

DESCRIPTION
  This procedure checks if the diagnostic packet has been registered with or
  without delay.
Note: Return value is 0 if Diag packet has no delayed response and 1 if Diag 
      packet has delayed response

===========================================================================*/
uint32 diagpkt_with_delay (diagpkt_subsys_id_type id,
                           diagpkt_subsys_cmd_code_type code)
{
  uint16 packet_id = code;      /* Command code for std or subsystem */
  uint8 subsys_id = id;
  const diagpkt_master_table_type *mstr_tbl_entry = NULL;
  const diagpkt_user_table_entry_type *tbl_entry = NULL;
  int tbl_entry_count = 0;
  int i, j;
  boolean found = FALSE;
  uint32 delay_flag = 0;

  /* Search the dispatch table for a matching subsystem ID.  If the
     subsystem ID matches, search that table for an entry for the given
     command code. */
  for (i = 0; !found && i < DIAGPKT_MSTR_TBL_SIZE; i++)
  {
    mstr_tbl_entry = diagpkt_master_table[i];

    if (mstr_tbl_entry != NULL && mstr_tbl_entry->subsysid == subsys_id)
    {
      tbl_entry = mstr_tbl_entry->user_table;

      delay_flag = mstr_tbl_entry->delay_flag;

      tbl_entry_count = (tbl_entry) ? mstr_tbl_entry->count : 0;

      for (j = 0; (tbl_entry!=NULL) && !found && j < tbl_entry_count; j++)
      {
        if (packet_id >= tbl_entry->cmd_code_lo && 
            packet_id <= tbl_entry->cmd_code_hi)
        {
          /* If the entry has no func, ignore it. */
          found = TRUE;
        }
        tbl_entry++;
      }
    }
  }

  return delay_flag;
}               /* diagpkt_with_delay */

/*===========================================================================

FUNCTION DIAGPKT_DELAY_ALLOC

DESCRIPTION
  This function allocates the specified amount of space in the diag output 
  buffer. 

===========================================================================*/
PACKED void * diagpkt_delay_alloc(
  diagpkt_cmd_code_type code,
  unsigned int length)
{
  PACKED void *ptr; /* Pointer to return. */

  unsigned int size = length;

  ptr = diagbuf_alloc (code, size, 0);
   
  return ptr;
} /* diagpkt_delay_alloc */

/* -------------------------------------------------------------------------
** Externalized functions
** ------------------------------------------------------------------------- */

/*===========================================================================

FUNCTION DIAGPKT_GET_CMD_CODE

DESCRIPTION
  This function returns the command code in the specified diag packet.

===========================================================================*/
diagpkt_cmd_code_type
diagpkt_get_cmd_code (PACKED void *ptr)
{
  /* Diag command codes are the first byte */
  return *((diagpkt_cmd_code_type *) ptr);
}               /* diag_get_cmd_code */

/*===========================================================================

FUNCTION DIAGPKT_SET_CMD_CODE

DESCRIPTION
  This function sets the command code in the specified diag packet.

===========================================================================*/
void
diagpkt_set_cmd_code (PACKED void *ptr, diagpkt_cmd_code_type cmd_code)
{
  *((diagpkt_cmd_code_type *) ptr) = cmd_code;
}               /* diagpkt_set_cmd_code */

/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_GET_ID

DESCRIPTION
  This function returns the subsystem ID in the specified diag packet.

  If the packet is not a DIAG_SUBSYS_CMD_F or DIAG_SUBSYS_CMD_VER_2_F packet,
  0xFFFF is returned.

===========================================================================*/
diagpkt_subsys_id_type
diagpkt_subsys_get_id (PACKED void *ptr)
{
  diagpkt_subsys_id_type id;
  diagpkt_subsys_hdr_type *pkt_ptr = (void *) ptr;

  if ((pkt_ptr->command_code == DIAG_SUBSYS_CMD_F) || (pkt_ptr->command_code
      == DIAG_SUBSYS_CMD_VER_2_F)) 
  {
    id = (diagpkt_subsys_id_type) pkt_ptr->subsys_id;
  } 
  else 
  {
    id = 0xFF;
  }
  return id;
}               /* diagpkt_subsys_get_id */

/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_GET_CMD_CODE

DESCRIPTION
  This function returns the subsystem command code in the specified
  diag packet.

  If the packet is not a DIAG_SUBSYS_CMD_F or DIAG_SUBSYS_CMD_VER_2_F packet,
  0xFFFF is returned.

===========================================================================*/
diagpkt_subsys_cmd_code_type
diagpkt_subsys_get_cmd_code (PACKED void *ptr)
{
  diagpkt_subsys_cmd_code_type code;
  diagpkt_subsys_hdr_type *pkt_ptr = (void *) ptr;

  if ((pkt_ptr->command_code == DIAG_SUBSYS_CMD_F) || (pkt_ptr->command_code
      == DIAG_SUBSYS_CMD_VER_2_F)) 
  {
    code = pkt_ptr->subsys_cmd_code;
  } 
  else 
  {
    code = 0xFFFF;
  }

  return code;
}               /* diagpkt_subsys_get_cmd_code */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_GET_STATUS 

DESCRIPTION
  This function gets the status field in the DIAG_SUBSYS_CMD_VER_2_F packet

  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

===========================================================================*/
diagpkt_subsys_status_type 
diagpkt_subsys_get_status (PACKED void *ptr) 
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

  ASSERT (pkt_ptr != NULL);
  ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);
  
  return pkt_ptr->status;
}               /* diagpkt_subsys_get_status */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_SET_STATUS 

DESCRIPTION
  This function sets the status field in the DIAG_SUBSYS_CMD_VER_2_F packet.

  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

===========================================================================*/
void
diagpkt_subsys_set_status (PACKED void *ptr, diagpkt_subsys_status_type status)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

  ASSERT (pkt_ptr != NULL);
  ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);
  
  pkt_ptr->status = status;
}               /* diagpkt_subsys_set_status */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_GET_DELAYED_RSP_ID

DESCRIPTION
  This function gets the delayed response ID field in the 
  DIAG_SUBSYS_CMD_VER_2_F packet.
  
  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

===========================================================================*/
diagpkt_subsys_delayed_rsp_id_type 
diagpkt_subsys_get_delayed_rsp_id (PACKED void *ptr)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

  ASSERT (pkt_ptr != NULL);
  ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);
  
  return pkt_ptr->delayed_rsp_id;
}               /* diagpkt_subsys_get_delayed_rsp_id */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_RESET_DELAYED_RSP_ID

DESCRIPTION
  This function sets the delayed response ID to zero in the 
  DIAG_SUBSYS_CMD_VER_2_F packet. 
  
  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

===========================================================================*/
void 
diagpkt_subsys_reset_delayed_rsp_id (PACKED void *ptr)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;
  
  ASSERT (pkt_ptr != NULL);
  ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);

  pkt_ptr->delayed_rsp_id = 0;
}               /* diagpkt_subsys_reset_delayed_rsp_id */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_SET_RSP_CNT

DESCRIPTION
  This function sets the response count in the DIAG_SUBSYS_CMD_VER_2_F packet.

  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

===========================================================================*/
void 
diagpkt_subsys_set_rsp_cnt (PACKED void *ptr, diagpkt_subsys_rsp_cnt rsp_cnt)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

  ASSERT (pkt_ptr != NULL);
  ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);
  
  pkt_ptr->rsp_cnt = rsp_cnt;
}               /* diagpkt_subsys_set_rsp_cnt */


/*==========================================================================*/
/* This is found in diag.c */
extern void *diag_blocking_malloc (unsigned int num_bytes);

PACKED void *
diagpkt_alloc (diagpkt_cmd_code_type code, unsigned int length)
{
  diagpkt_q_type *item = NULL;
  diagpkt_hdr_type *pkt = NULL;
  PACKED uint16 *pattern = NULL;    /* Overrun pattern. */
  unsigned int size = FPOS (diagpkt_q_type, rsp.pkt) + length + sizeof (uint16);

  /* The status of diagbuf_drain() if blocking is required. */
  diagbuf_status_enum_type status = DIAGBUF_READY_S;

  if (rex_self () == &diag_tcb)
  {
    #ifndef FEATURE_DIAG_NON_STREAMING
    /*-----------------------------------------------
      Try to allocate a buffer.  Size of buffer must
      include space for overhead and CRC at the end.
    -----------------------------------------------*/
    while ((item = (diagpkt_q_type *) diag_blocking_malloc (size)) == NULL)
    {
      /* Alloc not successful.  Block and retry until successful. */

      /*-----------------------------------------------------------
        It is possible that we could be unable to drain due to a
        lower priority task that has not committed its buffer.

        "Not ready" can also mean that flow control was
        asserterd.  Wait for flow control to allow more TX.

        If the last attempt to drain resulted in data not ready,
        then we should wait until another task commits a buffer or
        flow control is
      -----------------------------------------------------------*/
      if (status == DIAGBUF_NOT_READY_S)
      {
        (void) diag_wait (DIAG_TX_SIG);
        (void) rex_clr_sigs (&diag_tcb, DIAG_TX_SIG);
      }

      #ifdef FEATURE_DEBUG_DIAG_HEAP
      if( (get_event_q_cnt() == 0) && (get_rsp_q_cnt() == 0) && 
          (diag_heap_used_bytes() !=0) )
      {
         ERR_FATAL("Diag Heap Corruption Used = %d",diag_heap_used_bytes(),0,0);
      }
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      /* Kick the dog */ 
      diag_kick_watchdog ();

      /* Send more packets until enough space exists to alloc this response. */
      /* Send a mask of all '1's so it will return after 1 pass. */
      status = diagbuf_drain (0xFFFFFFFFU);

    }               /* while not getting space */

#else
    (void) status;      /* supppress compiler warning */

    item = (diagpkt_q_type *) diag_malloc (size);

#endif /* !FEATURE_DIAG_NON_STREAMING */
  }

  if (item)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diagpkt_alloc_cnt++;
    MSG_FATAL("BLOCKING MALLOC diagpkt_alloc = %d heapUsed = %d CmdCode = %d",
        diagpkt_alloc_cnt, diag_heap_used_bytes(),code);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    /* This pattern is written to verify pointers elsewhere in this
       service  are valid. */
    item->rsp.pattern = DIAGPKT_HDR_PATTERN;    /* Sanity check pattern */

    /* This should always be pending.  Just in case, check it. */
    if (diagpkt_rsp_func_store.pending)
    {
      /* Clear the cmd connection mask */
      DIAG_CLR_MASK (diag_cx_state, DIAG_CX_CMD_S);

      item->rsp_func = diagpkt_rsp_func_store.rsp_func;
      item->rsp_func_param = diagpkt_rsp_func_store.rsp_func_param;
    }
    else
    {
      item->rsp_func = NULL;
      item->rsp_func_param = NULL;
    }

    /* In either case, go ahead and clear the stored rsp_func info. */
    diagpkt_rsp_func_store.pending = FALSE;
    diagpkt_rsp_func_store.rsp_func = NULL;
    diagpkt_rsp_func_store.rsp_func_param = NULL;
    
    /* length ==  size unless packet is resized later */
    item->rsp.size = length;
    item->rsp.length = length;

    pattern = (PACKED uint16 *) & item->rsp.pkt[length];

    *pattern = DIAGPKT_OVERRUN_PATTERN;
/*lint -save -e{545,740} Suspicious use of & Unusual pointer cast*/
    pkt = (diagpkt_hdr_type *) & item->rsp.pkt;
/*lint -restore */
  }

  if (pkt)
  {
    pkt->command_code = code;
  }
  return (PACKED void *) pkt;

}               /* diagpkt_alloc */


/*===========================================================================

FUNCTION DIAGPKT_DELAY_COMMIT

DESCRIPTION
  This function is a wrapper of diagbuf_commit

===========================================================================*/
void diagpkt_delay_commit (PACKED void *pkt)
{
  diagbuf_commit (pkt);
  return;
}               /* diagpkt_delay_commit */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_ALLOC

DESCRIPTION
  This function returns the command code in the specified diag packet.

===========================================================================*/
PACKED void *
diagpkt_subsys_alloc (diagpkt_subsys_id_type id,
              diagpkt_subsys_cmd_code_type code, unsigned int length)
{
  diagpkt_subsys_hdr_type *hdr = NULL;

  ASSERT (rex_self () == &diag_tcb);

  hdr = (diagpkt_subsys_hdr_type *) diagpkt_alloc (DIAG_SUBSYS_CMD_F, length);

  if( hdr != NULL )
  {
    hdr->subsys_id = id;
    hdr->subsys_cmd_code = code;

#ifdef FEATURE_DEBUG_DIAG_HEAP
    diagpkt_subsys_alloc_cnt++;
    MSG_FATAL("BLOCKING MALLOC subsys_alloc_cnt = %d SSID = %d SS_CODE = %d",
        diagpkt_subsys_alloc_cnt, id,code);
    if( (id == 19) && (code == 3))
    {
      diagpkt_subsys_alloc_fs_close++;
      MSG_FATAL("Total fs_close req queued=%d",diagpkt_subsys_alloc_fs_close,0,0);
    }
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  }

  return (PACKED void *) hdr;

}               /* diagpkt_subsys_alloc */


/*===========================================================================
FUNCTION DIAGPKT_SUBSYS_ALLOC_V2

DESCRIPTION
  This function allocates the specified amount of space in the diag output 
  buffer.  If space is unavailable, access to the allocation buffer will be
  blocked using a semaphore until space is available.

DEPENDENCIES
  This may only be called by the diag task for responses to explicit request
  packets!  This function is *not* re-entrant.  If the OS ever supports
  semaphores other than INTLOCK, this function can be made re-entrant.  
  
  diagpkt_commit() must be called to commit the response packet to be sent.
  Not calling diagpkt_commit() will result in a memory leak.
===========================================================================*/
PACKED void *
diagpkt_subsys_alloc_v2 (diagpkt_subsys_id_type id,
              diagpkt_subsys_cmd_code_type code, unsigned int length)
{
  diagpkt_subsys_hdr_type_v2 *hdr = NULL;

  ASSERT (rex_self () == &diag_tcb);

  hdr = (diagpkt_subsys_hdr_type_v2 *) diagpkt_alloc (DIAG_SUBSYS_CMD_VER_2_F, length);

  if (hdr != NULL)
  {
    hdr->subsys_id = id;
    hdr->subsys_cmd_code = code;
    hdr->status = 0;

#ifdef FEATURE_DEBUG_DIAG_HEAP
    diagpkt_subsys_alloc_v2_cnt++;
    MSG_FATAL("BLOCKING MALLOC subsys_alloc_v2 = %d subsys_id = %d subsys_code = %d",
      diagpkt_subsys_alloc_v2_cnt, id,code);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    if( diagpkt_with_delay (id,code) )
    {
      hdr->delayed_rsp_id = DIAGPKT_NEXT_DELAYED_RSP_ID(delayed_rsp_id);
    }else
    {
      hdr->delayed_rsp_id = 0;
    }
    hdr->rsp_cnt = 0;
  }

  return (PACKED void *) hdr;

}               /* diagpkt_subsys_alloc_v2 */

/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_ALLOC_V2_DELAY

DESCRIPTION
  This function allocates the specified amount of space in the diag output 
  buffer. This function is used to send a delayed response.This response has
  same priority as F3 messages and logs.

DEPENDENCIES
  diagpkt_delay_commit() must be called to commit the response packet to be 
  sent. Not calling diagpkt_delay_commit() will result in a memory leak and 
  response packet will not be sent.

  Note:User is required to provide delayed response id as an argument.
       This helps tools to match the delayed response with the original 
       request response pair.

===========================================================================*/
PACKED void *
diagpkt_subsys_alloc_v2_delay (diagpkt_subsys_id_type id,
              diagpkt_subsys_cmd_code_type code,
              diagpkt_subsys_delayed_rsp_id_type delayed_rsp_id_arg,              
              unsigned int length)
{
  diagpkt_subsys_hdr_type_v2 *hdr = NULL;

  hdr = (diagpkt_subsys_hdr_type_v2 *) diagpkt_delay_alloc(
                 DIAG_SUBSYS_CMD_VER_2_F, 
                 length);

  if(hdr != NULL)
  {
    hdr->subsys_id = id;
    hdr->subsys_cmd_code = code;
    hdr->status = 0;
    hdr->delayed_rsp_id = delayed_rsp_id_arg;
    hdr->rsp_cnt = 1;
  }

  return (PACKED void *) hdr;

}               /* diagpkt_subsys_alloc_v2_delay */

/*==========================================================================*/
void
diagpkt_shorten (PACKED void *pkt, unsigned int new_length)
{

  diagpkt_q_type *item = NULL;
  PACKED uint16 *pattern = NULL;

  if (pkt)
  {
    /* Do pointer arithmetic in bytes, then case to q_type; */
    item = DIAGPKT_PKT2QITEM (pkt);

    if (new_length < item->rsp.size)
    {
      item->rsp.length = new_length;

      /* Write the new buffer overrun detection pattern */
      pattern = (PACKED uint16 *) & item->rsp.pkt[new_length];

      *pattern = DIAGPKT_OVERRUN_PATTERN;
    }
    else
    {
      /* handle failure case here */
    }
  }

  return;
}               /* diagpkt_shorten */


/*=========================================================================

FUNCTION DIAGPKT_FREE

DESCRIPTION
  This function free the packet allocated by diagpkt_alloc(), which doesn't
  need to 'commit' for sending as a response if it is merely a temporary
  processing packet.

===========================================================================*/
void
diagpkt_free(PACKED void *pkt)
{
  if (pkt)
  {
    diagpkt_q_type *item = DIAGPKT_PKT2QITEM(pkt);
    diag_free ((void *)item);
  }

  return;
}


/*==========================================================================*/
void
diagpkt_commit (PACKED void *pkt)
{
  diagpkt_q_type *item = NULL;
  PACKED uint16 *pattern;

  if (pkt)
  {
    item = DIAGPKT_PKT2QITEM (pkt);

    /* If pattern has been inverted, this is a double-commit.  Ignore. */
    if (item->rsp.pattern != ~DIAGPKT_HDR_PATTERN)
    {
      if (item->rsp.pattern == DIAGPKT_HDR_PATTERN)
      {
        /* Invert the header pattern to mark that the packet has been 
        processed.  This handles the incorrect usage of committing twice. */
        item->rsp.pattern = ~DIAGPKT_HDR_PATTERN;

        /* The 2 byte CRC field at the end of the allocated buffer is written
         * with a mask to detect buffer overrun.  Check to make sure that value
         * wasn't corrupted.
         */
         pattern = (PACKED uint16 *) & item->rsp.pkt[item->rsp.length];

         if (*pattern == DIAGPKT_OVERRUN_PATTERN)
         {
           #ifndef FEATURE_DIAG_NON_STREAMING
           if (item->rsp.length > 0)
           {
             /* The pattern check was successful, go ahead and queue the packet. */
             q_put (&diagpkt_rsp_q, q_link (item, &item->qlink));

             #ifdef FEATURE_DEBUG_DIAG_HEAP
             MSG_FATAL("Q_PUT Diagpkt_Commit diagpkt_rsp_q_cnt = %d  heap used = %d",
             q_cnt (&diagpkt_rsp_q),  diag_heap_used_bytes(),0); 
             #endif /* FEATURE_DEBUG_DIAG_HEAP */
            
             /* Notify the diag task so the queue can be serviced. */
             diag_tx_notify ();
           }
           else
           {
             diag_free (item);

             #ifdef FEATURE_DEBUG_DIAG_HEAP
             diagpkt_commit_free_cnt++;
             MSG_FATAL("FREE diagpkt_commit_free_cnt = %d, diag heap used = %d ",
             diagpkt_commit_free_cnt,diag_heap_used_bytes(),0);
             #endif /* FEATURE_DEBUG_DIAG_HEAP */
           }
           #else
           if (item->rsp.length > 0)
           {
             diagbuf_encode_and_send_pkt ((void *) item->rsp.pkt.buf, 
                                          item->rsp.length);
           }

           diag_free (item);

           #ifdef FEATURE_DEBUG_DIAG_HEAP
           diagpkt_commit_free_cnt++;
           MSG_FATAL("FREE diagpkt_commit_free_cnt = %d, diag heap used = %d ",
           diagpkt_commit_free_cnt,diag_heap_used_bytes(),0);
           #endif /* FEATURE_DEBUG_DIAG_HEAP */

           #endif
         }
         else {
           ERR_FATAL ("Buffer overrun on packet at 0x%x", (uint32) pkt, 0, 0);
         }
       }
       else
      {
    ERR_FATAL ("Invalid pointer used for diag buffering", 0, 0, 0);
      }
    }
  }

  return;
}               /* diagpkt_commit */

/*==========================================================================*/
boolean
diagpkt_rsp_send (void)
{
  static diagpkt_q_type *item_sav = NULL;
  static diag_send_desc_type send =
    { NULL, NULL, DIAG_SEND_STATE_COMPLETE, TRUE };

  /* Local copies of static variables for optimized access.  
     This makes this routine persistent but not re-entrant. */
  diagpkt_q_type *item = item_sav;

  /* Local variables */
  diagbuf_status_enum_type status = DIAGBUF_READY_S;
  boolean finished = FALSE;

  while (!finished && status == DIAGBUF_READY_S) {
    if (diag_panic_mode) dog_kick();  /* PLM */

    if (item) {
      finished = FALSE;

      status = diagbuf_send_pkt (&send);

      if (send.state == DIAG_SEND_STATE_COMPLETE) {
        diag_free (item);

        #ifdef FEATURE_DEBUG_DIAG_HEAP
        diagpkt_rsp_send_free_cnt++;
        MSG_FATAL("FREE diagpkt_rsp_send_free_cnt = %d, diag heap used = %d ",
        diagpkt_rsp_send_free_cnt,diag_heap_used_bytes(),0);
        #endif /* FEATURE_DEBUG_DIAG_HEAP */
        item = NULL;
      }
    }

    if (!item) {
      item = q_get (&diagpkt_rsp_q);

      if (item) {
        #ifdef FEATURE_DEBUG_DIAG_HEAP
        MSG_FATAL("Q_GET diagpkt_rsp_send rsp_q_cnt = %d, diag heap used = %d ",
                         q_cnt (&diagpkt_rsp_q),diag_heap_used_bytes(),0);

        MSG_FATAL(" Q_GET RSP cmd = %d, %d, %d", (*(item)).rsp.pkt[0],
             (*(item)).rsp.pkt[1], (*(item)).rsp.pkt[2] );
        #endif /* FEATURE_DEBUG_DIAG_HEAP */
        /* If the caller specified a function to be called with the 
           response packet, call it now. Otherwise, send to the comm layer.*/
        if (item->rsp_func) {
          /* Clear the command connection mask. */
          DIAG_CLR_MASK (diag_cx_state, DIAG_CX_CMD_S);

          item->rsp_func (item->rsp.pkt, item->rsp.length, 
                          item->rsp_func_param);

          /* This item is now complete */
          diag_free (item);

          #ifdef FEATURE_DEBUG_DIAG_HEAP
          diagpkt_rsp_send_free_cnt++;
          MSG_FATAL("FREE diagpkt_rsp_send_free_cnt = %d, diag heap used = %d",
              diagpkt_rsp_send_free_cnt,diag_heap_used_bytes(),0);
          #endif /* FEATURE_DEBUG_DIAG_HEAP */
          item = NULL;
        }
        else {
          send.pkt = item->rsp.pkt;
          send.last = (void *) ((uint32) send.pkt + item->rsp.length - 1);
          send.state = DIAG_SEND_STATE_START;
          send.terminate = TRUE;
        }
      }
      else {
        finished = TRUE;
      }
    }
  }

  /* Save local copies of persistent data back to static variables. */
  item_sav = item;

  return finished;
}


/* ==========================================================================
FUNCTION DIAGPKT_MASTER_TBL_INIT

DESCRIPTION Registers the table given to the diagpkt master table
============================================================================= */
static void
diagpkt_master_tbl_init (void)
{
  int i;
  static boolean initialized = FALSE;

  if (!initialized)
  {
    initialized = TRUE;

    for (i = 0; i < DIAGPKT_MSTR_TBL_SIZE; i++)
    {
      diagpkt_master_table[i] = NULL;
    }
  }
}

/* ==========================================================================
FUNCTION DIAGPKT_TBL_REG

DESCRIPTION Registers the table given to the diagpkt master table
============================================================================= */
void
diagpkt_tbl_reg (const diagpkt_master_table_type * tbl_ptr)
{
  int i;

  /* Make sure this is initialized */
  diagpkt_master_tbl_init ();

  for (i = 0; i < DIAGPKT_MSTR_TBL_SIZE; i++)
  {
    if (diagpkt_master_table[i] == NULL)
    {
      diagpkt_master_table[i] = tbl_ptr;
      break;
    }
  }
}

#ifdef DIAG_DEBUG_FS_PKT_LOG
#error code not present
#endif

/* Tell lint that we are OK with the fact that pkt_len and req_ptr are not
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req_ptr) */

#ifdef FEATURE_DIAG_MANUAL_DISPATCH_TABLE
/* We are forced to use this coupling since the ARM C++ compiler does not
   handler 'extern "C" {}' properly. */

#ifdef PLATFORM_LTK
#error code not present
#endif

extern void prldiag_init (void);
extern void toolsdiag_init (void);

#if defined(FEATURE_IS2000) && !defined(FEATURE_GSM_PLT)
  extern void cdma2kdiag_init (void);
#else
  extern void fsdiag_init (void);
  #if !defined(FEATURE_GSM_PLT) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_THIN_UI)
    extern void hsdiag_init (void);
    extern void vocdiag_init (void);
  #endif
  extern void nvdiag_init (void);
#endif

extern void diagdiag_init (void);

#ifdef FEATURE_WCDMA
#error code not present
#endif

#ifdef FEATURE_DIAG_MULTI_PORTS
extern void autoconfigdiag_init (void);
#endif

#ifdef FEATURE_REX_DIAG
extern void rexdiag_init (void);
#endif

extern void paramsdiag_init (void);

#if defined (FEATURE_ERR_EXTENDED_STORE) && (defined (FEATURE_SAVE_DEBUG_TRACE) || defined (FEATURE_DIAG_DEBUG_INVOKE_CRASH))
extern void diagdebug_init (void);
#endif

void diagpkt_init_manual_dispatch_table (void)
{
  static boolean initialized = FALSE;

  if (!initialized)
  {
    initialized = TRUE;
  
#ifdef PLATFORM_LTK
#error code not present
#endif

    prldiag_init();
    toolsdiag_init ();

#if defined(FEATURE_IS2000) && !defined(FEATURE_GSM_PLT)
    cdma2kdiag_init ();
#else
    fsdiag_init ();
#if !defined(FEATURE_GSM_PLT) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_THIN_UI)
    hsdiag_init ();
    vocdiag_init ();
#endif
    nvdiag_init ();
#endif

    diagdiag_init ();

#ifdef FEATURE_WCDMA
#error code not present
#endif

#ifdef FEATURE_DIAG_MULTI_PORTS
    autoconfigdiag_init();
#endif


#ifdef FEATURE_REX_DIAG
    rexdiag_init ();
#endif

    paramsdiag_init();

#if defined (FEATURE_ERR_EXTENDED_STORE) && (defined (FEATURE_SAVE_DEBUG_TRACE) || defined (FEATURE_DIAG_DEBUG_INVOKE_CRASH))
    diagdebug_init ();
#endif
  }
}
#endif
/*===========================================================================

FUNCTION DIAGPKT_PROCESS_REQUEST

DESCRIPTION
  This procedure formats a response packet in response to a request
  packet received from the Diagnostic Monitor.  Calls packet processing
  functions through a table, counts input and output types

===========================================================================*/
void
diagpkt_process_request (void *req_pkt, uint16 pkt_len, 
                         diag_cmd_rsp rsp_func, void *rsp_func_param)
{
  uint16 packet_id;     /* Command code for std or subsystem */
  uint8 subsys_id = DIAGPKT_NO_SUBSYS_ID;
  const diagpkt_master_table_type *mstr_tbl_entry = NULL;
  const diagpkt_user_table_entry_type *tbl_entry = NULL;
  int tbl_entry_count = 0;
  int i, j;
  void *rsp_pkt = NULL;
  boolean found = FALSE;
  uint16 cmd_code = 0xFF;

  static boolean initialized = 0;

#ifdef DIAG_DEBUG_FS_PKT_LOG
#error code not present
#endif /* DIAG_DEBUG_FS_PKT_LOG */

  if (!initialized)
  {
    initialized = 1;

    (void) q_init (&diagpkt_rsp_q);
  }

  packet_id = diagpkt_get_cmd_code (req_pkt);

  if ( packet_id == DIAG_SUBSYS_CMD_VER_2_F )
  {
    cmd_code = packet_id;
  }
  if ((packet_id == DIAG_SUBSYS_CMD_F) || ( packet_id == DIAG_SUBSYS_CMD_VER_2_F ))
  {
    subsys_id = diagpkt_subsys_get_id (req_pkt);
    packet_id = diagpkt_subsys_get_cmd_code (req_pkt);
  }

  /* Search the dispatch table for a matching subsystem ID.  If the
     subsystem ID matches, search that table for an entry for the given
     command code. */
  for (i = 0; !found && i < DIAGPKT_MSTR_TBL_SIZE; i++)
  {
    mstr_tbl_entry = diagpkt_master_table[i];

    if (mstr_tbl_entry != NULL && mstr_tbl_entry->subsysid == subsys_id
        && mstr_tbl_entry->cmd_code == cmd_code)
    {
      tbl_entry = mstr_tbl_entry->user_table;

      tbl_entry_count = (tbl_entry) ? mstr_tbl_entry->count : 0;

      for (j = 0; (tbl_entry!=NULL) && !found && j < tbl_entry_count; j++)
      {
        if (packet_id >= tbl_entry->cmd_code_lo && 
            packet_id <= tbl_entry->cmd_code_hi)
        {
          /* If the entry has no func, ignore it. */
          if (tbl_entry->func_ptr)
          {
            found = TRUE;

            if (rsp_func)
            {
              /* Set the cmd connection mask to ensure the TX chain will 
                 service the queue. */
              DIAG_SET_MASK (diag_cx_state, DIAG_CX_CMD_S);
            }
            
            /* Store rsp_func and rsp_func_param in a global to be captured
               when the dispatched function calls diagpkt_alloc().  If
               for some odd reason it never does, call rsp_func as a no-op. */
            diagpkt_rsp_func_store.pending = TRUE;
            diagpkt_rsp_func_store.rsp_func = rsp_func;
            diagpkt_rsp_func_store.rsp_func_param = rsp_func_param;

            rsp_pkt = (void *) (*tbl_entry->func_ptr) (req_pkt, pkt_len);

            if (diagpkt_rsp_func_store.pending == TRUE)
            {
              /* The dispatched function did not call diagpkt_alloc().
                 Go ahead and send an empty response. */
              diagpkt_rsp_func_store.pending = FALSE;

              if (diagpkt_rsp_func_store.rsp_func)
              {
                /* Clear the cmd connection mask */
                DIAG_CLR_MASK (diag_cx_state, DIAG_CX_CMD_S);

                diagpkt_rsp_func_store.rsp_func (NULL, 0, 
                  diagpkt_rsp_func_store.rsp_func_param);

                diagpkt_rsp_func_store.rsp_func = NULL;
                diagpkt_rsp_func_store.rsp_func_param = NULL;
              }
            }

            if (rsp_pkt)
            {
#ifdef FEATURE_DEBUG_DIAG_HEAP
              diagpkt_process_request_cmd_req_queued++;
              if( (subsys_id == 19) && (packet_id == 3))
              {
                diagpkt_process_req_fs_close_queued++;
              }
              MSG_FATAL("Total req_queued =%d fs_close req queued=%d",
                  diagpkt_process_request_cmd_req_queued,
                  diagpkt_process_req_fs_close_queued, 0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

              /* The most common case: response is returned.  Go ahead
                 and commit it here. */
              diagpkt_commit (rsp_pkt);
            }
          }
        }

        tbl_entry++;
      }
    }
  }

  /* Assume that rsp and rsp_pkt are NULL if !found */
  if (!found)
  {
    /* Packet id not found, send an error packet. */
    rsp_pkt = (void *) diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt, pkt_len);

    if (rsp_pkt)
    {
      /* Return the pointer to diagpk_rsp_type, not the ptr returned 
         to the avg user. */
      diagpkt_commit (rsp_pkt);
    }
  }

#ifdef DIAG_DEBUG_FS_PKT_LOG
#error code not present
#endif
  
  return;
}               /* diagpkt_process_request */

/*=========================================================================*/
PACKED void *
diagpkt_err_rsp (diagpkt_cmd_code_type code,
         PACKED void *req_pkt, uint16 req_len)
{
  DIAG_BAD_CMD_F_rsp_type *rsp;
  const unsigned int rsp_len = MIN (sizeof (DIAG_BAD_CMD_F_rsp_type),
               req_len + FPOS (DIAG_BAD_CMD_F_rsp_type, pkt));
  rsp = (DIAG_BAD_CMD_F_rsp_type *) diagpkt_alloc (code, rsp_len);

  if(req_pkt) 
  {
    memcpy ((void *) rsp->pkt,
            (void *) req_pkt, 
            rsp_len - FPOS (DIAG_BAD_CMD_F_rsp_type, pkt));
  }
  else if (req_len != 0)
  {
    MSG_HIGH("Non-0 request length (%d) and NULL request pointer!",req_len,0,0);
  }

  return ((void *) rsp);
}               /* diagkt_err_rsp */

/*==========================================================================*/
void
diagpkt_async_block (void)
{
  /* Wait until async sig is set by release() call. */
  (void) diag_wait (DIAG_ASYNC_BLOCK_SIG);
  (void) rex_clr_sigs (&diag_tcb, DIAG_ASYNC_BLOCK_SIG);
}

/*==========================================================================*/
void
diagpkt_async_release (void)
{
  /* Release by setting the ASYNC sig */
  (void) rex_set_sigs (&diag_tcb, DIAG_ASYNC_BLOCK_SIG);
}


/* Restore error reporting for these symbols */
/*lint +esym(715,pkt_len,req_ptr) */
