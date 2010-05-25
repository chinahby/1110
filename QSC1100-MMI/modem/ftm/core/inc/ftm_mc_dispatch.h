/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                              FTM MC Dispatch

GENERAL DESCRIPTION
  This is the header file for the embedded FTM MC Dispatch commands comming 
  from the diag service.

Copyright (c) 1994, 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001       by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftm_mc_dispatch.h#1 $ 
  $DateTime: 2008/04/14 04:57:43 $ 
  $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/04   jac     Mainlined ASYNC packet code.
05/18/04   rsr     Added include ftmdiag.h
05/18/04   rsr     renamed FEATURE_FTM_ASYNC_PKT to FTM_HAS_ASYNC_PKT
09/05/03   bmg     Added FTM HDR commands
08/27/03   wd      Added support for variable length response pkt.
06/06/03   wd      Removed unnecessaries includes.
05-06-03   wd      Created 
===========================================================================*/

#ifndef FTM_MC_DISPATCH_H
#define FTM_MC_DISPATCH_H

#ifdef FEATURE_FACTORY_TESTMODE
#include "comdef.h"
#include "target.h"
#include "ftm.h"
#include "ftmdiag.h" 


//PAKETS
typedef PACKED struct
{
  diagpkt_subsys_header_type diag_hdr;
  ftm_cmd_header_type        ftm_hdr;
  ftm_packed_param_type      param;
}  ftm_mc_pkt_type;


//PROTOTYPES
ftm_rsp_pkt_type ftm_mc_dispatch (ftm_mc_pkt_type  *ftm);

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_MC_DISPATCH_H */
