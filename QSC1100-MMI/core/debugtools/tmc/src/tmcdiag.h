#ifndef TMCDIAG_H
#define TMCDIAG_H
/*==========================================================================

                      Diagnostic Packet Definitions

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

Copyright (c) 2003 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

$Header: //depot/asic/qsc1100/services/tmc/tmcdiag.h#1 $ $DateTime: 2008/06/12 15:21:51 $ $Author: vphan $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
06/12/08   vph    Disable tmc_heap_small by added #ifndef FEATURE_LOW_MEMORY_USAGE
08/25/03   as     Created file

===========================================================================*/

#include "comdef.h"
#include "diagpkt.h"
#include "memheap.h"

/* Subsystem command codes */
#define TMCDIAG_HEAP_CONSUMED_F 0

#ifndef FEATURE_LOW_MEMORY_USAGE
  #define TMCDIAG_HEAP_SMALL_CONSUMED_F 1
#endif


/* -------------------------------------------------------------------------
** Packet definitions.
** ------------------------------------------------------------------------- */

#ifdef FEATURE_REX_DYNA_MEM
/*==========================================================================

PACKET   TMCDIAG_HEAP_CONSUMED

PURPOSE  Request sent from the DM to the DMSS, to get the heap size consumed
         and details for tmc_heap and tmc_heap_small based on subsystem 
		 command code.

============================================================================*/
typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
} tmcdiag_heap_consumed_req_type;


typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
  mem_heap_totals_type heap_total;
} tmcdiag_heap_consumed_rsp_type;


#endif /* FEATURE_REX_DYNA_MEM */

#endif /* TMCDIAG_H */
