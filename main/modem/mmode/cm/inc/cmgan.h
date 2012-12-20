#ifndef CMGAN_H
#define CMGAN_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/inc/cmgan.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/24/09   aj   Add metacomments to support rpc
01/27/09   aj   Initial release.
===========================================================================*/


/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "customer.h"  /* Customer configuration file */

#ifdef FEATURE_CM_GAN
#error code not present
#endif /* FEATURE_CM_GAN*/

#endif /* CMGAN_H */

