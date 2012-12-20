
#ifndef _DSMIP_OUTMSG_H
#define _DSMIP_OUTMSG_H

/*===========================================================================
                                      
                        D S M I P _ O U T M S G . H

DESCRIPTION

 The Data Services Mobile IP output message generator header file.  

EXTERNALIZED FUNCTIONS

  mip_outmsg_send_sol()
    Composes and expedites an agent solicitation via the mip_io module

  mip_outmsg_send_rrq()
    Composes and expedites a registration request via the mip_io module

Copyright (c) 2000 - 2011 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_outmsg.h_v   1.1   12 Jul 2002 15:07:56   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_outmsg.h#2 $ $DateTime: 2011/02/24 23:31:53 $ $Author: msankar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/11    ms     Ported MOBILE_IP_DEREG feature.
05/10/02   jd      includes dsmip.h instead of dsmip_regsm.h 
                   (moved definition of mip_session_info_type)
12/20/00   jd      Created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#include "dsmip.h"


/*===========================================================================

                      EXTERNALIZED FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION MIPOUT_SEND_SOL()

DESCRIPTION
    Composes and expedites an agent solicitation via the mip_io module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mip_outmsg_send_sol 
(
  void
);

/*===========================================================================
FUNCTION MIP_OUTMSG_SEND_RRQ()

DESCRIPTION
    Composes and expedites an agent solicitation via the mip_io module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mip_outmsg_send_rrq 
(
  mip_session_info_type * mip_info
);

#ifdef FEATURE_DS_MOBILE_IP_DEREG
/*===========================================================================
FUNCTION MIPOUT_SEND_DEREG_RRQ()

DESCRIPTION
  Composes and sends a deregistration request to the specified foreign agent.

  The format and expected values of this message are described in the
  Mobile Interface Specification.

NOTES:
  * we ask to use the first listed COA in the agent advertisement as per
    rfc2002bis
  * we generate the following extensions:
    - MN_NAI
    - MN-HA authentication
    - MN-FA challenge, if rx'd in AAM or last RRP from this FA
    - MN-AAA authentication, if MN-FA challenge is present

PARAMETERS
  session - pointer to session info block for which we are registering.
             (RRQ is based on this and the mobility agent info block)

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_outmsg_send_dereg_rrq
(
  mip_session_info_type *session
);
#endif  /* FEATURE_DS_MOBILE_IP_DEREG */

#endif /* _DSMIP_OUTMSG_H */
