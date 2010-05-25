/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Compact NV (CNV) Diagnostic Interface

               Supports command for backup of NV data to CNV

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History
$Header: //depot/asic/msmshared/services/nv1/nv_cnvdiag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/05   sl      First cut of module.

===========================================================================*/

#include "customer.h"
#include "diagcmd.h"
#include "diagbuf.h"
#include "diagcomm.h"
#include "nv_cnv.h"
#include "nv_cnvdiag.h"

#ifdef FEATURE_NV_CNV

/* Response identifier for the delay response packet for CNV backup */
diagpkt_subsys_delayed_rsp_id_type cnv_backup_delay_rsp_id;

/* Indicates if a NV->CNV backup is currently in progress */
boolean nv_backup_in_progress = FALSE;

/* Command buffer for CNV interface */
static cnv_cmd_type cncmd;

PACKED void * nv_cnvdiag_backup (PACKED void *req_pkt, uint16 pkt_len);
void nv_cnvdiag_backup_complete (cnv_rpt_type *cnv_backup_rpt);

/*==========================================================================

PACKET   NV_CNV_BACKUP

PURPOSE  Request sent from the DM to the DMSS to backup the NV data to CNV.

RETURN VALUE
         Pointer to response packet.

============================================================================*/

PACKED void *
nv_cnvdiag_backup (PACKED void *req_pkt, uint16 pkt_len)
{
  /*  Cast copy of request pointer to proper type */
//  nv_cnv_diag_backup_req_type *req_ptr  = (nv_cnv_diag_backup_req_type *) req_pkt;
  nv_cnv_diag_backup_delay_rsp_type *rsp_ptr = NULL;

  /*  Allocate our packet */
  rsp_ptr = (nv_cnv_diag_backup_delay_rsp_type *) diagpkt_subsys_alloc_v2 (
                                DIAG_SUBSYS_CNV,
                                NV_CNV_DIAG_BACKUP_F,
                                sizeof (nv_cnv_diag_backup_delay_rsp_type) );

  if(rsp_ptr == NULL)
    return NULL;

  /* Save the packet id for later use in the delay respose */
  cnv_backup_delay_rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp_ptr);

  /* Set the first (0) response count */
  diagpkt_subsys_set_rsp_cnt(rsp_ptr, 0);

  if(nv_backup_in_progress != TRUE) {
    nv_backup_in_progress = TRUE;
    /* Set the status in the response to SUCCESS */
    diagpkt_subsys_set_status(rsp_ptr, 0);
    /* Send out the NV backup command to CNV */
    cncmd.hdr.command = CNV_BACKUP_NV_F;
    cncmd.hdr.rpt_function = nv_cnvdiag_backup_complete;
    cnv_cmd( &cncmd);
  }
  else {
    /* Set the status in the response to FAILURE */
    diagpkt_subsys_set_status(rsp_ptr, 1);
  }

  return rsp_ptr;
}

/*==========================================================================

PACKET   NV_CNV_BACKUP

PURPOSE  Callback from CNV on completion of NV backup. Sends a delayed response
         packet to DM.

RETURN VALUE

============================================================================*/

void nv_cnvdiag_backup_complete (cnv_rpt_type *cnv_backup_rpt)
{
  /* Delayed response packet */
  nv_cnv_diag_backup_delay_rsp_type *rsp_ptr = NULL;
  nv_backup_in_progress = FALSE;

  if (cnv_backup_rpt == NULL )
    return;

  rsp_ptr = (nv_cnv_diag_backup_delay_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                                DIAG_SUBSYS_CNV,
                                NV_CNV_DIAG_BACKUP_F,
                                cnv_backup_delay_rsp_id,
                                sizeof (nv_cnv_diag_backup_delay_rsp_type) );

  if (rsp_ptr == NULL)
    return;

  else
  {
    /* sets the response count value */
    diagpkt_subsys_set_rsp_cnt(rsp_ptr, 1);

    /* Set the status in the response. TRUE -> 0; FALSE->1 */
    diagpkt_subsys_set_status(rsp_ptr, !(cnv_backup_rpt->rpt.status));

    /*  Send out the final response packet */
    diagpkt_delay_commit(rsp_ptr);
  }
  return;
}

/**************************************************************************
** NV_CNVDIAG diag DISPATCH TABLE.
***************************************************************************/

static const diagpkt_user_table_entry_type nv_cnvdiag_tbl_delay_rsp[] =
{
  {NV_CNV_DIAG_BACKUP_F, NV_CNV_DIAG_BACKUP_F, nv_cnvdiag_backup}
};

void nv_cnvdiag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(DIAG_SUBSYS_CMD_VER_2_F,
                                           DIAG_SUBSYS_CNV,
                                           nv_cnvdiag_tbl_delay_rsp);
}

#endif /* FEATURE_NV_CNV */
