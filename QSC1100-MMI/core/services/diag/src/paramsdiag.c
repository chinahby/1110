/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Diagnostics Packet Processing Routines
                  for debug parameters

General Description
  Diagnostic packet processing routines for debug parameters

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/paramsdiag.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/08   vg      Updated code to use PACK() vs. PACKED
10/31/05   as      Fixed lint errors.
01/07/04   as      Created file.
===========================================================================*/

#ifdef __cplusplus
  extern "C" {
#endif

#include "customer.h"
#include "paramsdiag.h"
#include "clk.h"

/* Tell lint that we are OK with the fact that pkt_len and req_ptr are not 
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req_ptr) */


/*===========================================================================

                   DIAG COMMAND FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION PARAMSDIAG_CLK_UPTIME
DESCRIPTION
  This packet gives the time elapsed in milliseconds, since the last reset.

===========================================================================*/
PACK(void *) paramsdiag_clk_uptime (
  PACK(void *) req_pkt,
  uint16 pkt_len       
)
{
  paramsdiag_clk_uptime_rsp_type *rsp_ptr;
  const int rsp_len = sizeof (paramsdiag_clk_uptime_rsp_type);

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */
  
  /* Allocate memory for the response packet */
  rsp_ptr = (paramsdiag_clk_uptime_rsp_type *) diagpkt_subsys_alloc 
                                   (DIAG_SUBSYS_PARAMS,
                                    PARAMSDIAG_UPTIME_F, 
                                    rsp_len);

  if (rsp_ptr != NULL)
  {
    /* Fill in the response with uptime */
    clk_uptime_ms( rsp_ptr->uptime );
  }

  return (rsp_ptr);

} /* paramsdiag_clk_uptime */


/*===========================================================================

                  DIAG COMMAND FUNCTION DISPATCH TABLE

===========================================================================*/

static const diagpkt_user_table_entry_type paramsdiag_tbl[] =
{
  {PARAMSDIAG_UPTIME_F, PARAMSDIAG_UPTIME_F, paramsdiag_clk_uptime}
};



/*===========================================================================

FUNCTION PARAMSDIAG_INIT

DESCRIPTION
  Register the diag packet function dispatch table.

===========================================================================*/
#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_PARAMS, paramsdiag_tbl);

#else

  void paramsdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_PARAMS, paramsdiag_tbl);
  }

#endif


/* Restore error reporting for these symbols */
/*lint +esym(715,pkt_len,req_ptr) */

#ifdef __cplusplus
  }
#endif

