#ifndef CMIDM_H
#define CMSSIDM_H
/*===========================================================================

                   I D M    H E A D E R    F I L E

DESCRIPTION
  This file contains all the definitions necessary for the Call Manager
  to interface with the Idle digital mode( IDM ) module.

Copyright (c) 2003 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmssidm.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/06/07   pk/cl   Added support for LN status notification from HDR
01/06/06   ic      Lint cleanup  
12/09/05   ic      Changes to cmssidm_estimate_base_station_idm() 
                   declaration so cmcall.c lints clean
10/20/05   sk      Added prototypes of functions used to access and update
                   base station IDM.
02/06/04   ws      Initial jaguar/mainline merge.
11/21/03   ic      Fixed FEATURE_HDR_HANDOFF featurization bug. 
11/19/03   ic      Featurized SS IDM object's functionality under
                   FEATURE_HDR_HANDOFF.
11/08/03   sj      Initial release.

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "sd.h"
#include "cmll.h"

#ifdef FEATURE_HDR_HANDOFF
#error code not present
#endif // FEATURE_HDR_HANDOFF
#endif /* CMSSIDM_H */

