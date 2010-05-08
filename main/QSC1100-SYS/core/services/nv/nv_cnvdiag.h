#ifndef NV_CNVDIAG_H
#define NV_CNVDIAG_H
/*==========================================================================

                   AutoConfig Diagnostic Packet Definitions

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

Copyright (c) 2003, 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           Edit History

$Header: //depot/asic/msmshared/services/nv1/nv_cnvdiag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/05   sl      First cut of module.

===========================================================================*/

#include "comdef.h"
#include "diagpkt.h"

#ifdef FEATURE_NV_CNV

/* Subsystem command codes */
#define NV_CNV_DIAG_BACKUP_F 0x0


/* -------------------------------------------------------------------------
** Packet definitions.
** ------------------------------------------------------------------------- */

typedef struct {
  diagpkt_subsys_header_v2_type header;  /* Sub System header */
} nv_cnv_diag_backup_req_type;


typedef struct {
  diagpkt_subsys_header_v2_type header; /* Sub System header */
} nv_cnv_diag_backup_delay_rsp_type;

/* -------------------------------------------------------------------------
** Init Function.
** ------------------------------------------------------------------------- */

void nv_cnvdiag_init (void);

#endif /* FEATURE_NV_CNV */
#endif /* NV_CNVDIAG_H */