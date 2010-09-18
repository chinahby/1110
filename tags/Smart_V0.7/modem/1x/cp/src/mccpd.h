#ifndef MCCPD_H
#define MCCPD_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          M A I N  C O N T R O L
                          
        P O S I T I O N   D E T E R M I N A T I O N   S E R V I C E S 

GENERAL DESCRIPTION

  This file contains the support for Position Determination services.

EXTERNALIZED FUNCTIONS
  mcc_pd_init - Register the position determination service options with
                service negotiation manager (SNM).


INITIALIZATION AND SEQUENCING REQUIREMENTS
  mcc_pd_init needs to be called during the powerup.

Copyright (c) 2000-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccpd.h_v   1.0.2.0   30 Nov 2001 17:12:38   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccpd.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/18/04   ank     Mainlined FEATURE_GPSONE_DBM.
06/28/00   yll     Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

#endif /* MCCPD_H */

