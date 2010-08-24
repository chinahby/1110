/*===========================================================================

         S E R V I C E   O P T I O N S   D E F I N I T I O N S

GENERAL DESCRIPTION
  This module contains definitions of service options other than data.

Copyright (c) 2000-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccso.h_v   1.0.2.0   30 Nov 2001 17:30:26   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccso.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/13/09   pk      Fixing compiler warnings
05/15/08   bb      Code changes for "MVS API Update" feature
11/02/00   lcc     Initial release for FEATURE_COMPLETE_SNM.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_MVS_MIGRATE
#error code not present
#endif /* FEATURE_MVS_MIGRATE */

/*===========================================================================

                           FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION mccso_initialize

DESCRIPTION
  This function initializes all the service options defined in mccso.c.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccso_initialize( void );

#ifdef FEATURE_MVS_MIGRATE
#error code not present
#endif

