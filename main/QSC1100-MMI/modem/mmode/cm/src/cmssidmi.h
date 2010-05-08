#ifndef CMIDMI_H
#define CMSSIDMI_H
/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
                            Idle  Digital  Mode ( IDM )

                 I N T E R N A L    H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the internal types necessary for implementing
  the CM Idle Digital Mode ( IDM ) module.

Copyright (c) 2003 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmssidmi.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Removed EXTERN define & moved MMEXTN define to cmi.h
03/01/09   sv      Lint cleanup.
02/06/04   ws      Initial jaguar/mainline merge.
11/21/03   ic      Fixed FEATURE_HDR_HANDOFF featurization bug. 
11/19/03   ic      Featurized SS IDM object's functionality under
                   FEATURE_HDR_HANDOFF.
                   Replaced CMSSIDM_AVOID_HANDUP_TIMER with
                   CMSSIDM_AVOID_HANDUP_TIME.
11/06/03   SJ      Initial release.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */

#ifdef FEATURE_HDR_HANDOFF
#error code not present
#endif // FEATURE_HDR_HANDOFF
#endif /* CMSSIDM_H */

