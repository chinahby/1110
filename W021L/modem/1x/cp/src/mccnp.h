#ifndef MCCNP_H
#define MCCNP_H
/*===========================================================================

                    M A I N   C O N T R O L   T A S K

                                 C D M A

              N E T W O R K   P E R S O N A L I Z A T I O N

                         H E A D E R    F I L E

DESCRIPTION
  This file data and function declarations necessary for the lowest level
  functions to implement CDMA network personalization.

Copyright (c) 2004-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath$
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccnp.h#1 $
$DateTime: 2009/05/29 03:54:56 $
$Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/04   jah     initial version of FEATURE_NETWORK_PERSONALIZATION

===========================================================================*/

#include "comdef.h"     /* common types used in this file      */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Type for holding an IMSI check description for Network Personalization
*/
typedef struct {
  word mcc;            /* Mobile Country Code (MCC) */
  byte imsi_11_12;     /* Mobile Network Code (MNC) */
  dword imsi_s1_min;   /* Minimum value of S1       */
  dword imsi_s1_max;   /* Maximum value of S1       */
  word imsi_s2_min;    /* Minimum value of S2       */
  word imsi_s2_max;    /* Maximum value of S2       */
} cdma_imsi_check_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION MCCNP_IMSI_VALID

DESCRIPTION
  Validate an IMSI using the network personalization table.

  If the network personalization table is not initialized,
  the IMSI will be declared invalid.

DEPENDENCIES
  None

RETURN VALUE
  True    IMSI is valid

  False   IMSI is not valid

SIDE EFFECTS
  None

===========================================================================*/

extern boolean mccnp_imsi_valid
(
  word  imsi_mcc,      /* Mobile Country Code (MCC) */
  byte  imsi_11_12,    /* Mobile Network Code (MNC) */
  dword imsi_s1,       /* IMSI_S1                   */
  word  imsi_s2        /* IMSI_S2                   */
);


#if defined( FEATURE_OPEN_NETWORK_PERSONALIZATION )

/* <EJECT> */
/*===========================================================================

FUNCTION MCCNP_NP_TABLE_INIT

DESCRIPTION
  Initialize the Network Personalization Table

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccnp_np_table_init
(
  cdma_imsi_check_type *np_table,
  int num_entries
);

#endif /* FEATURE_OPEN_NETWORK_PERSONALIZATION */

#endif /* MCCNP_H */
