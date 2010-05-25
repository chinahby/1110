/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Packet Processing

General Description
  Diagnostic packet processing routines for MUX1X sub-system access.

Copyright (c) 2003 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/mux1xdiag.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV and fixed Klockwork issues.
10/13/04   bn      Added logic allow user to set DV logmask from QXDM menu
06/29/04   jrp     Added MUX1X_DIAG_PAGING_STAT_F to diag table
04/20/04   jrp     Fixed lint warnings.
03/09/04   sr/bkm  Featurized DV code
11/14/03   cf/vlc  Fixed compiler warnings (casting) for RVCT compiler.
10/21/03   bn      Added support for retrieving paging statistic counters via DIAG
04/15/03   bn      Created new file for DIAG MUX1X sub-system access.
===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "comdef.h"
#include "customer.h"

#include "diagbuf.h"
#include "diagcmd.h"
#include "mux1xdiag.h"
#include "rxc.h"       /* Interface to RXC subtask */

/*-----------------------  defined and extern variables ----------------------------*/
/* MUX paging statistic counter for DIAG */
#define   MUX1XDIAG_GPM_PAG_MASK     0x01
#define   MUX1XDIAG_SPM_PAG_MASK     0x02
#define   MUX1XDIAG_RSSI_NEGATIVE_MASK    0x04
extern word rxc_gpm_page_matched_counter;
extern word rxc_spm_page_matched_counter;
extern word rxc_srch_rssi_negative_counter;
/*===========================================================================
Function: mux1xdiag_fer
Description:

       This function processes the MUX1X specific averaged FER request packet
       from QXDM and sends the FER response back.

============================================================================*/
static mux1xdiag_fer_s_type mux1xdiag_rxc_fer;
PACKED void *mux1xdiag_fer (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_MUX1X_MUX1X_DIAG_FER_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_MUX1X_MUX1X_DIAG_FER_F_rsp_type);

  /* Unused parameters */
  (void) req_pkt;
  (void) pkt_len;

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_MUX1X_MUX1X_DIAG_FER_F_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_MUX1X, MUX1X_DIAG_FER_F,
                                 rsp_len );

  /* call the function rxc_get_averaged_fer() to get the FER statistic info
  then use it to initialize the FER response structure*/
  rxc_get_averaged_fer( &mux1xdiag_rxc_fer);

  if(rsp_ptr != NULL)
  {
    rsp_ptr->meas_frames = mux1xdiag_rxc_fer.meas_frames;
    rsp_ptr->err_frames  = mux1xdiag_rxc_fer.err_frames;
  }

  return rsp_ptr ;
}

/*===========================================================================
Function: mux1xdiag_paging_stat
Description:

       This function processes the MUX1X specific paging related statistic 
       counters request package from QXDM and sends the response back.

============================================================================*/
PACKED void *mux1xdiag_paging_stat (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_MUX1X_MUX1X_DIAG_PAGING_STAT_F_req_type *req_ptr;
  DIAG_SUBSYS_MUX1X_MUX1X_DIAG_PAGING_STAT_F_rsp_type *rsp_ptr;

  const int req_len = sizeof(DIAG_SUBSYS_MUX1X_MUX1X_DIAG_PAGING_STAT_F_req_type);
  const int rsp_len = sizeof(DIAG_SUBSYS_MUX1X_MUX1X_DIAG_PAGING_STAT_F_rsp_type);

  if (pkt_len != req_len)
  {
    return NULL;
  }

  /* get the counter bit mask from the request package */
  req_ptr = (DIAG_SUBSYS_MUX1X_MUX1X_DIAG_PAGING_STAT_F_req_type *)req_pkt;

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_MUX1X_MUX1X_DIAG_PAGING_STAT_F_rsp_type *) 
               diagpkt_subsys_alloc(DIAG_SUBSYS_MUX1X, 
               MUX1X_DIAG_PAGING_STAT_F, rsp_len );

  if(rsp_ptr != NULL)
  {
    if (req_ptr->counters_bitmask & MUX1XDIAG_GPM_PAG_MASK)
    {
      rsp_ptr->gpm_counter  = rxc_gpm_page_matched_counter;
    }
    else
    {
      rxc_gpm_page_matched_counter = 0;
      rsp_ptr->gpm_counter  = 0;
    }
  
    if (req_ptr->counters_bitmask & MUX1XDIAG_SPM_PAG_MASK)
    {
      rsp_ptr->spm_counter = rxc_spm_page_matched_counter;
    }
    else
    {
      rsp_ptr->spm_counter  = 0;
      rxc_spm_page_matched_counter = 0;
    }
  
    if (req_ptr->counters_bitmask & MUX1XDIAG_RSSI_NEGATIVE_MASK)
    {
      rsp_ptr->rssi_negative_counter = rxc_srch_rssi_negative_counter;
    }
    else
    {  
      rxc_srch_rssi_negative_counter = 0;
      rsp_ptr->rssi_negative_counter  = 0;
    }
  }
  return rsp_ptr ;

}

/**************************************************************************
** DISPATCH TABLE.
Note:  Any new commands for MUX1X need to be added here.
****************************************************************************/

static const diagpkt_user_table_entry_type mux1xdiag_tbl[] =
{
  {MUX1X_DIAG_FER_F, MUX1X_DIAG_FER_F, mux1xdiag_fer},
  {MUX1X_DIAG_PAGING_STAT_F, MUX1X_DIAG_PAGING_STAT_F, mux1xdiag_paging_stat}
};


#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_MUX1X, mux1xdiag_tbl);

#else

  void mux1xdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_MUX1X, mux1xdiag_tbl);  /*lint !e717 do ... while(0) */
  }

#endif

#ifdef __cplusplus
  }
#endif
