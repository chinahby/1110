
/*===========================================================================

    C A L L   M A N A G E R   G A N  C O N T R O L L E R   I N T E R F A C E

                        H E A D E R   F I L E

DESCRIPTION

   This header describes functions that interface the GAN controller to
   Call Manager. The GAN UI (GAN controller's interface to CM) needs to send
   reports to CM which has to be published as serving system events to UI and 
   other interested CM clients. 

  Copyright (c) 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmgan.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/24/09   aj      Replace FEATURE_GAN_AMSS_ONLY with FEATURE_CM_GAN
01/27/09   aj   Initial release.
===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
/*lint -esym(766,customer.h) */
#include "customer.h"  /* Customer configuration file */

#if defined(FEATURE_CM_GAN)
#error code not present
#else /* FEATURE_CM_GAN  */

/*===========================================================================
FUNCTION cmgan_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmgan_dummy( void )
/*lint -esym(714,cmgan_dummy) */
/*lint -esym(765,cmgan_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}

#endif /* FEATURE_CM_GAN */


