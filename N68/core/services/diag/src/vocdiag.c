/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Packet Processing

General Description
  Diagnostic packet processing routines for the vocoder driver.

Copyright (c) 1990-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //depot/asic/msmshared/services/diag/vocdiag.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/05   as      Fixed lint errors.
08/20/01   jal     Updated for core VU
12/08/00   lad     Created file.
===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "comdef.h"
#include "customer.h"
#include "diagbuf.h"
#include "diagpkt.h"

#include "voc.h"
#include "vocdiag.h"


/* Tell lint that we are OK with the fact that pkt_len and req_ptr are not 
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req_ptr) */


#ifndef FEATURE_HWTC
/*===========================================================================

FUNCTION VOCDIAG_PCM_LB
DESCRIPTION
  Processes a request for Vocoder PCM loopback to be initiated or terminated

===========================================================================*/
PACKED void * vocdiag_pcm_lb (
  PACKED void *req_pkt,
  uint16 pkt_len       
)
{
  vocdiag_pcm_lb_req_type *req_ptr = (vocdiag_pcm_lb_req_type *) req_pkt;
  vocdiag_pcm_lb_req_type *rsp_ptr;
  const int rsp_len = sizeof (vocdiag_pcm_lb_req_type);

  (void) pkt_len; /* suppress compiler warning */

  /* Set up a Vocoder command to send the appropriate loopback req  */
  /*  pcm_lb = TRUE means on_off should be on=1                     */
  /*  pcm_lb = FALSE means on_off should be off=0                   */

  /* Send command to Vocoder */
  voc_pcm_loop_cmd (req_ptr->pcm_lb);

  rsp_ptr = (vocdiag_pcm_lb_req_type *) diagpkt_alloc (DIAG_VOC_PCM_LB_F, rsp_len);

  *rsp_ptr = *req_ptr;

  return (rsp_ptr);

} /* vocdiag_pcm_lb */



/*===========================================================================

FUNCTION VOCDIAG_PKT_LB
DESCRIPTION
  Processes a request for Vocoder Packet loopback to be initiated or 
  terminated

===========================================================================*/
PACKED void * vocdiag_pkt_lb (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  vocdiag_pkt_lb_req_type *req_ptr = (vocdiag_pkt_lb_req_type *) req_pkt;
  vocdiag_pkt_lb_req_type *rsp_ptr; /* Response is same as request type */
  const int rsp_len = sizeof (vocdiag_pkt_lb_req_type);
  
  (void) pkt_len; /* suppress compiler warning */

  /* Set up a Vocoder command to send the appropriate loopback req  */
  /*  pkt_lb = TRUE means on_off should be on=1                     */
  /*  pkt_lb = FALSE means on_off should be off=0                   */

  /* Send command to Vocoder */
  voc_pkt_loop_cmd (req_ptr->pkt_lb);
  
  rsp_ptr = (vocdiag_pkt_lb_req_type *) diagpkt_alloc (DIAG_VOC_PKT_LB_F, rsp_len);

  *rsp_ptr = *req_ptr;

  return (rsp_ptr);

} /* vocdiag_pkt_lb */

static const diagpkt_user_table_entry_type vocdiag_tbl[] =
{
  {DIAG_VOC_PCM_LB_F, DIAG_VOC_PCM_LB_F, vocdiag_pcm_lb},
  {DIAG_VOC_PKT_LB_F, DIAG_VOC_PKT_LB_F, vocdiag_pkt_lb}
};

#ifdef __cplusplus
  
  DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, vocdiag_tbl);

#else

void vocdiag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, vocdiag_tbl);
}

#endif 


#endif /* !FEATURE_HWTC */

/* Restore error reporting for these symbols */
/*lint +esym(715,pkt_len,req_ptr) */

#ifdef __cplusplus
  }
#endif

