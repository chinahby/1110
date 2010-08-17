/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics Packet Processing Routines
                  TMC heap available Routines

General Description
  TMC heap diagnostic packet processing routines that are common to all 
  targets.

Copyright (c) 2003 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //depot/asic/qsc1100/services/tmc/tmcdiag.c#1 $ $DateTime: 2008/06/12 15:21:51 $ $Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/08   vph     Disable tmc_heap_small by added #indef FEATURE_LOW_MEMORY_USAGE
02/19/04   jqi     Fixed lint error.
06/23/03   as      Created file.

===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "comdef.h"
#include "customer.h"
#include "tmcdiag.h"
#include "diagcmd.h"
#include "tmc.h"
#include "err.h"


#ifdef FEATURE_REX_DYNA_MEM


/*===========================================================================
FUNCTION TMCDIAG_HEAP_CONSUMED

DESCRIPTION     
  This function gives details of memory heap for tmc_heap or 
  tmc_heap_small depending on the command code. The details include:
    total_physical_bytes, 
    current_block_count,
    used_blocks,
    header_bytes,
    used_bytes,
    free_bytes,
    wasted_bytes,
    largest_free_block,
    max_logical_used,
    max_logical_request,

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACKED void * 
tmcdiag_heap_consumed (PACKED void *req_pkt, uint16 pkt_len) /*lint -esym(715,pkt_len) */
{
  tmcdiag_heap_consumed_rsp_type *rsp;
  tmcdiag_heap_consumed_req_type *req = 
    (tmcdiag_heap_consumed_req_type *) req_pkt;

  diagpkt_subsys_cmd_code_type cmd_code = diagpkt_subsys_get_cmd_code (req);

  const int rsp_len = sizeof(tmcdiag_heap_consumed_rsp_type);

  /* Allocate the length as the response packet */
  rsp = (tmcdiag_heap_consumed_rsp_type *) 
    diagpkt_subsys_alloc ((int) DIAG_SUBSYS_TMC, cmd_code, rsp_len);
  
  /* Send the response with the heap information */
  if (rsp != NULL)
  {
    switch (cmd_code)
    {
      case TMCDIAG_HEAP_CONSUMED_F:
        mem_heap_get_totals( &tmc_heap,  &(rsp->heap_total) );
        break;
      #ifndef FEATURE_LOW_MEMORY_USAGE 
        case TMCDIAG_HEAP_SMALL_CONSUMED_F:
          mem_heap_get_totals( &tmc_heap_small,  &(rsp->heap_total) );
          break;
      #endif /* FEATURE_LOW_MEMORY_USAGE */
      default: 
        ERR( "Invalid Command code %d",cmd_code, 0 ,0 );
      break;
    }
  }

  return rsp;

} /* tmcdiag_heap_consumed */ /*lint +esym(715,pkt_len) */

#endif /* FEATURE_REX_DYNA_MEM */



/**************************************************************************
** TMC diag DISPATCH TABLE.
***************************************************************************/

static const diagpkt_user_table_entry_type tmcdiag_tbl[] =
{
#ifdef FEATURE_REX_DYNA_MEM
  #ifndef FEATURE_LOW_MEMORY_USAGE
    {TMCDIAG_HEAP_CONSUMED_F, TMCDIAG_HEAP_SMALL_CONSUMED_F, tmcdiag_heap_consumed}
  #else
    {TMCDIAG_HEAP_CONSUMED_F, TMCDIAG_HEAP_CONSUMED_F, tmcdiag_heap_consumed}
  #endif
#endif
};

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER ((int) DIAG_SUBSYS_TMC, tmcdiag_tbl);

#else

  void tmcdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER ((int) DIAG_SUBSYS_TMC, tmcdiag_tbl);
  }

#endif


#ifdef __cplusplus
  }
#endif
