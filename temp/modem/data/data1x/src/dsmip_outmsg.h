
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

Copyright (c) 2000 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_outmsg.h_v   1.1   12 Jul 2002 15:07:56   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_outmsg.h#1 $ $DateTime: 2007/11/06 01:07:58 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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

#endif /* _DSMIP_OUTMSG_H */
