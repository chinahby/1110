/* =============================================================================
                                driftcorrection.c

DESCRIPTION
  This module provides drift compensation services between two clocks.  It
  accepts pairs of time readings (clock A, reference clock) and (clock B,
  reference clock) and calculates filtered drifts, between clock A and the
  reference clock and between clock B and the reference clock.  It combines
  the drifts and determines whether the drift between clock A and clock B has
  reached a specified threshold.  The user of this service can then make
  the proper compensation to the slave clock.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Portions copyrighted by QUALCOMM Incorporated;
Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
============================================================================= */

/* =============================================================================

                               EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/multimedia/qtv/player/avsync/main/latest/src/driftcorrection.c#9 $
$DateTime: 2008/09/24 02:10:11 $
$Change: 749684 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/5/06    ateng   Initial version
============================================================================= */

/* =============================================================================
                            INCLUDE FILES FOR THE MODULE
==============================================================================*/

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "target.h"
#include "driftCorrection.h"
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */


