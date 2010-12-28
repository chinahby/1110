#ifndef CUSTONCRPC_H
#define CUSTONCRPC_H
/*===========================================================================

            " M S M 5 1 0 0 -  S U R F "   H E A D E R   F I L E

DESCRIPTION
  Configuration for SURF Target using ONCRPC

  Copyright (c) 2003   by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/COMMON/vcs/custoncrpc.h_v   1.31   05 Nov 2003 16:33:00   pbostley  $
$Header: //depot/asic/msmshared/services/oncrpc/oncrpc/custoncrpc.h#37 $ $DateTime: 2005/06/23 15:56:32 $ $Author: dneiss $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/04   clp     Removed FEATURE_ONCRPC_SIO_REINIT that is no longer needed.
06/06/03   sv      Added FEATURE_ONCRPC_SIO_REINIT feature.
06/02/03   clp     Added FEATURE defs for Client support, WMS, RATS and Co, 
                   PMCLNT, RPCLNT. 
03/17/03   clp     Added FEATURE_ONCRPC_TCP with default on. Added undef 
                   placeholder for FEATURE_STA_RPCTEST.  Coverted back to 
                   unix line endings.
===========================================================================*/

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/


//------------------------------------------------------------------------------
// Leave this bracketing of #ifndef/#endif around #define FEATURE_ONCRPC
// so that we don't get a compiler error if FEATURE_ONCRPC was previously defined 
// (as can happen if it was defined as a command line option to the compiler).
// If you want to turn off the feature, just comment out the 
// #define FEATURE_ONCRPC line below, but leave the #ifndef/#endif.
//------------------------------------------------------------------------------
#ifndef FEATURE_ONCRPC
  #define FEATURE_ONCRPC
#endif


#ifdef FEATURE_ONCRPC
#error code not present
#endif

// If you want to use the ETM you must enable this.

//#define FEATURE_ETM 

// If you want to use ONCRPC over USB or USB on linux in any form
// FEATURE_USB_DIAG must be turned off 

//#undef  FEATURE_USB_DIAG

#endif /* CUSTONCRPC_H */

