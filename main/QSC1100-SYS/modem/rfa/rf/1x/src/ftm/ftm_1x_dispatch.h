/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            F T M  R F

GENERAL DESCRIPTION
  This is the header file for the embedded FTM 1x Dispatch commands comming 
  from the diag service 

Copyright (c) 1994, 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001       by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/ftm/ftm_1x_dispatch.h#1 $ 
  $DateTime: 2008/04/11 05:11:57 $ 
  $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/04   jac     Mainlined ASYNC packet code.
05/18/04   rsr     Renamed FEATURE_FTM_ASYNC_PKT to FTM_HAS_ASYNC_PKT
05/18/04   rsr     Added include of ftmdiag.h
08/27/03   wd      Added support for variable length response pkt.
07/25/03   bmg     Adding second chain FTM API
07/09/03   bmg     Merged in Couger 3030->4040 changes:
  06/06/03 wd      Removed unnecessaries includes.
02/07/03   wd      Rename file from ftm_rf_1x_dispatch.h -> ftm_1x_dispatch.h
12/11/02   ra      mainlined CDMA1X/AMPS 
10-10-02   ra      Created 
===========================================================================*/

#ifndef FTM_1X_DISPATCH_H
#define FTM_1X_DISPATCH_H

#ifdef FEATURE_FACTORY_TESTMODE
#include "comdef.h"
#include "target.h"
#include "ftm.h"
#include "rfcom.h"
#include "ftmdiag.h"


//PROTOTYPES
ftm_rsp_pkt_type ftm_1x_dispatch(rfcom_device_enum_type, ftm_pkt_type  *);

#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_1X_DISPATCH_H */
