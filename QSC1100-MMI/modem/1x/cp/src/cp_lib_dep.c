/*====*====*====*====*====*====*====*========*====*====*====*====*====*====*==*

        C D M A  C P  MODULE  -- L I B R A R Y   D E P E N D E N C I E S

GENERAL DESCRIPTION
  This file contains the feature dependencies for the cp library.  This
  file must be updated to reflect the features used when building the
  cp library.  In addition this file must always be shipped together
  with the cp library.
  
EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2004-2005 by QUALCOMM, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header:

when       who      what, where, why
--------   ---      -----------------------------------------------------------
04/24/06   fc       Fixed Rel A featurization.
01/23/06   fc       Added the check for AKA and MI.
10/31/05   an       Removed compilation warning by defining dummy extern
09/29/05   fc       Added the check for FEATURE_MEID_SUPPORT to be on if
                    FEATURE_IS2000_REL_D is on.
06/30/05   fc       Fixed compiler warning.
04/25/05   fc       Added the check for FEATURE_IS2000_REL_C_FCS to be on.
                    Removed the check for FEATURE_IS2000_P2 to be on.
02/15/05   sb       Removed requirement for FCS to be on, because 6800 does not
                    turn on Release C.
06/24/04   va       Implementation, first cut

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "mccdma.h"

/*===========================================================================

               Call Processing LIBRARY FEATURE REQUIREMENTS

===========================================================================*/

/* Required by:
     mccsyncid.c
*/
#if defined(FEATURE_IS2000_REL_C) && !defined(FEATURE_IS2000_REL_C_FCS)
#error Required by CP libary: FEATURE_IS2000_REL_C_FCS
#endif /* FEATURE_IS2000_REL_C && !FEATURE_IS2000_REL_C_FCS */

/* Required by:
     mccmeid.c
*/
#if defined(FEATURE_IS2000_REL_D) && !defined(FEATURE_MEID_SUPPORT)
#error code not present
#endif /* FEATURE_IS2000_REL_D && !FEATURE_MEID_SUPPORT */

/* Required by:
     mccaka.c
*/
#if defined(FEATURE_IS2000_REL_C_AKA) && !defined(FEATURE_IS2000_REL_C_MI)
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA && !FEATURE_IS2000_REL_C_MI */
