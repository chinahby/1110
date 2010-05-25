#ifndef PARAMSDIAG_H
#define PARAMSDIAG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Diagnostics Packet Processing Routines
                  for debug parameters

General Description
  Diagnostic packet processing routines for debug parameters

Copyright (c) 2005,2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                             Edit History

  $Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/paramsdiag.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/07   as      Enabled pragma pack support for WINCE targets
01/07/05   as      Created file.
===========================================================================*/

#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "qw.h"

#if defined(T_WINNT) || defined(FEATURE_WINCE)
#error code not present
#endif


/*==========================================================================
  
                  SUBSYSTEM PACKET CODE DEFINITIONS
   
============================================================================*/

#define PARAMSDIAG_UPTIME_F                         0


/*===========================================================================

FUNCTION PARAMSDIAG_CLK_UPTIME
DESCRIPTION
  This packet gives the time elapsed in milliseconds, since the last reset.

===========================================================================*/
typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
}  paramsdiag_clk_uptime_req_type;


typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
  qword uptime;
} paramsdiag_clk_uptime_rsp_type;


/*===========================================================================

FUNCTION PARAMSDIAG_INIT

DESCRIPTION
  Register the diag packet function dispatch table.

===========================================================================*/

void paramsdiag_init (void);

#if defined(T_WINNT) || defined(FEATURE_WINCE)
#error code not present
#endif

#endif /* PARAMSDIAG_H */

