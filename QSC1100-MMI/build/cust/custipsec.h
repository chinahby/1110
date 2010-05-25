#ifndef CUSTIPSEC_H
#define CUSTIPSEC_H
/*===========================================================================

              C U S T O M I Z A T I O N   O F   I P S E C

DESCRIPTION
  Configuration for IPSec and IKE.

  Copyright (c) 2003  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc60x0/build/cust/custipsec.h#1 $ $DateTime: 2006/02/10 15:05:43 $ $Author: milanp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/03   om      Created file.

===========================================================================*/


/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                          Security Services
---------------------------------------------------------------------------*/
/* Security Services
*/
#ifdef FEATURE_SEC_IPSEC

  /* VPN remote access support */
  #define FEATURE_SEC_IPSEC_VPN

  /* VPN test client code (via UI test menu) */
  #ifdef FEATURE_SEC_TESTAPPS
#error code not present
  #else
    #undef FEATURE_SEC_IPSEC_VPN_CLNT
  #endif

#endif  /* FEATURE_SEC_IPSEC */

#endif  /* CUSTIPSEC_H */

