#ifndef CUSTSOCK_H
#define CUSTSOCK_H
/*===========================================================================

            " C u s t - S O C K "   H E A D E R   F I L E

DESCRIPTION
  Configuration for Sockets Test Build

  Copyright (c) 2001,      by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/custsock.h_v   1.1   23 Oct 2001 19:32:56   donb  $
$Header: //depot/asic/qsc1100/build/cust/custsock.h#1 $ $DateTime: 2008/03/06 14:38:01 $ $Author: milanp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/21/01   bkm     Initial revision 
===========================================================================*/

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Test code for TCP sockets.
*/
#define FEATURE_DS_SOCKETS_TEST_TCP 
/* Sockets feature used to restrict sockets calls to LSPD service options.
*/
#undef FEATURE_DS_SOCKETS_USE_LSPD 

#endif /* CUSTSOCK_H */
