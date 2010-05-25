/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               N V   S I M   S U B S Y S T E M

GENERAL DESCRIPTION
  Provides I/O services for NVM items on the SIM card.  This file contains 
  stub functions for this feature.

EXTERNALIZED FUNCTIONS
  nvsim_read
    Read an NVM item from the SIM card

  nvsim_write
    Write an NVM item to the SIM card

  nvsim_free
    Free an NVM item on the SIM card
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/nvsim.c_v   1.0   20 Sep 2002 07:37:18   tsummers  $
$Header: //source/qcom/qct/modem/uim/nvruim/rel/07H1_2/src/nvsim.c#1 $ $DateTime: 2008/12/15 12:00:08 $ $Author: nkukreja $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/12/04   ck      Included a header file outside the feature check to avoid
                   compliler warning.
08/19/02   ts      Created file with stub functions.

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#if defined( FEATURE_UIM_RUIM_W_GSM_ACCESS )
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */
