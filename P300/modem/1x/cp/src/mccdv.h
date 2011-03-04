#ifndef MCCDV_H
#define MCCDV_H
/*===========================================================================

          MAIN CONTROL CDMA DV

   This module maintains data structures and transactions specific to 
   to the IS-2000 DV feature as defined in IS-2000 Release C and D.

Copyright (c) 2003-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccdv.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/28/05   fc      Made FCS also available to Rel D.
02/08/05   sb      Changed function names as per code review comments.
12/28/04   va      Added fn prototype for mccdv_get_pref_dv_sector
12/01/04   sb      Added function to get the rlgains.
12/01/04   sb      Fixes for DV RIF HHO's.
11/22/04   sb      Miscellaneous fixes for Rel C DV Handoffs.
11/05/04   sb      Added support for Rel C DV handoffs.
09/28/04   fc      Updated TXC_TC_F command payload.
09/16/04   fc      Added support for reverse DV.
08/20/04   bkm     Added more function decls for mccdv functions.
06/29/04   bkm     Added function declarations for misc mccdv functions.
06/29/04   pg      Added functions mccdv_update_num_soft_switching_frames() and
                   mccdv_update_num_softer_switching_frames().
06/16/04   pg      Added support for Rate Change Message.
04/20/04   bkm     Removed mccdv_set_num_soft/softer_switching_frames_chm, 
                   use srv_update_nn_scr_switch_params() instead.
04/14/04   az      Added support for ERM - Rel C DV chan cfg change
04/08/04   bkm     Misc renaming for clarity.
01/13/04   bkm     Added MAX enum value to mccdv_switch_type_enum_type.
01/05/04   bkm     Initial revision.
===========================================================================*/

#include "cai.h"
#include "target.h"
#include "customer.h"
#include "caii.h"
#include "mccdma.h"
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
#include "mccsyncid.h"
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

#if defined(FEATURE_IS2000_REL_C_DV) || defined(FEATURE_IS2000_DV_SIG_TEST)
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV || FEATURE_IS2000_DV_SIG_TEST */

#endif /* MCCDV_H */

