#ifndef VOCDIAG_H
#define VOCDIAG_H
/*==========================================================================

                Vocoder Diagnostic Packet Definitions

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of DIAG packets must be PACKED.
  
  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2001, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                             Edit History

  $Header: //depot/asic/msmshared/services/diag/vocdiag.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/07   as      Enabled pragma pack support for WINCE targets
08/20/01   jal     Updated for core VU
01/26/01   lad     Created file from what used to be diagpkt.h
===========================================================================*/

#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"


/* -------------------------------------------------------------------------
** Packet definitions for logging services.
** ------------------------------------------------------------------------- */

#if defined(T_WINNT) || defined(FEATURE_WINCE)
#error code not present
#endif
/*==========================================================================

PACKET   DIAG_VOC_PCM_LB_F

PURPOSE  Sent from the DM to the DMSS to start or stop a PCM loopback
         from the vocoder

============================================================================*/

typedef struct 
{
  byte cmd_code;
  boolean pcm_lb;        /* TRUE -> start, FALSE -> stop */
} vocdiag_pcm_lb_req_type;

/* Response packet is same format as request */

/*===========================================================================

PACKET   DIAG_VOC_PKT_LB_F

PURPOSE  Sent by DM to start/stop vocoder PKT loopback.

===========================================================================*/
typedef struct 
{
  byte cmd_code;
  boolean pkt_lb; /* TRUE => start, FALSE => stop */

} vocdiag_pkt_lb_req_type;

/* Response packet is same format as request */


#if defined(T_WINNT) || defined(FEATURE_WINCE)
#error code not present
#endif

#endif /* VOCDIAG_H */

