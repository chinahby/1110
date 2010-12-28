/*===========================================================================

              MAIN CONTROL VOIP-1X HANDOFF SUPPORT FUNCTIONS

GENERAL DESCRIPTION
  This module contains the functions that support VOIP-1X handoffs.

EXTERNALIZED FUNCTIONS
  cdma_voip_ho
    This is the entrance procedure for the VOIP-1X Handoff state.

Copyright (c) 2006-2007 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccvoip.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
01/21/08   pg      SD interface due to added support of '+' code dialing.
02/28/07   sb      Use default for slot cycle index in ORM.
02/20/07   sb      Removed unnecessary fields from log structure.
02/14/07   sb      Added logging of 3g1x info.
01/30/07   sb      Changed message type to be 8 bits instead of 16.
01/15/07   sb      Added TCI as a substate.
06/28/06   sb      Created module to support VOIP-1X handoffs.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -e766*/
#include "target.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */
