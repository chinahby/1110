#ifndef DS707_SID_H
#define DS707_SID_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                               S I D 

GENERAL DESCRIPTION
  This file contains functions used to handle SID changes.

EXTERNALIZED FUNCTIONS
  DS707_SID_ENABLE_RECON
    Enables reconnection out of dormancy when the SID changes.
    
  DS707_SID_DISABLE_RECON
    Disables reconnection out of dormancy when the SID changes.
    
  DS707_SID_CHANGE_HANDLER
    Registered with MC. Called by MC when the SID changes.  This will send
    a msg to 1X Pkt mgr, telling it to come out of dormancy.
    
  DS707_SID_REGISTER
    Registers the SID change handler with MC.
    
  DS707_SID_DEREGISTER
    Deregisters the SID change handler with MC.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None, though should call ds707_sid_register() at startup if want the 
  phone to re-orig out of dormancy when the SID changes.

   Copyright (c) 1999 by QUALCOMM Incorporated.  All Rights Reserved.
   Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_sid.h_v   1.1   18 Nov 2002 18:33:00   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_sid.h#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/02   ak      Updated file header comments.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707

/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION       DS707_SID_ENABLE_RECON

DESCRIPTION    Enables reconnection based on SID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_sid_enable_recon(void);

/*===========================================================================
FUNCTION       DS707_SID_DISABLE_RECON

DESCRIPTION    Disables reconnection based on SID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_sid_disable_recon(void);

/*===========================================================================
FUNCTION       DS707_SID_CHANGE_HANDLER

DESCRIPTION    Reset the Packet Data Dormant Timer to its default value, per
               IS-707-A.5/9 Section 2.2.6.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Resets the ds_dormant_timer value to its default value.
===========================================================================*/
void ds707_sid_change_handler
(
  word  old_sid,                                 /* Old System Id          */
  word  new_sid                                  /* New System Id          */
);

/*===========================================================================
FUNCTION       DS707_SID_REGISTER

DESCRIPTION    Registers the handler with MC.  Does not change the 
               enabled/disabled reconnection attribute.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_sid_register(void);

/*===========================================================================
FUNCTION       DS707_SID_DEREGISTER

DESCRIPTION    Deregisters the handler with MC.  Does not change the 
               enabled/disabled reconnection attribute.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_sid_deregister(void);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_SID_H        */
