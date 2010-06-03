#ifndef AUTOCONFIGDIAG_H
#define AUTOCONFIGDIAG_H
/*==========================================================================

                   AutoConfig Diagnostic Packet Definitions

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

Copyright (c) 2003, 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

#include "comdef.h"
#include "diagpkt.h"

/* Subsystem command codes */
#define AUTOCONFIG_DIAG_RELEASE_PORT_F 0
#define AUTOCONFIG_DIAG_SWITCH_PORT_F 1


/* -------------------------------------------------------------------------
** Packet definitions.
** ------------------------------------------------------------------------- */

typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
}  autoconfig_diag_release_port_req_type;


typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
} autoconfig_diag_release_port_rsp_type;


typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
  uint16 diagcomm_port;               /* Port to which DIAG should switch to */
}  autoconfig_diag_switch_port_req_type;


typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
  uint16 diagcomm_port;               /* Port to which DIAG should switch to */
  uint8 status;                      /* Status (0 - SUCCESS  ;  1 - FAILED) */
} autoconfig_diag_switch_port_rsp_type;

#endif /* AUTOCONFIGDIAG_H */