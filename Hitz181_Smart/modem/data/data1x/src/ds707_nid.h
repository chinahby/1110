#ifndef DS707_NID_H
#define DS707_NID_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                        D A T A   S E R V I C E S

                                   N I D 

GENERAL DESCRIPTION
  This file contains functions used to handle NID changes.

EXTERNALIZED FUNCTIONS
  DS707_NID_ENABLE_RECON
    Enables a packet reconnection when the NID changes.
    
  DS707_NID_DISABLE_RECON
    Disables a packet reconnection when the NID changes.
    
  DS707_NID_CHANGE_HANDLER
    Registered with MC.  Called when the NID changes.  If enabled, will 
    send a msg to DS pkt mgr, telling it to come out of dormancy.
    
  DS707_NID_REGISTER
    Registers the NID change handler with MC.
    
  DS707_NID_DEREGISTER
    De-registers the NID change handler with MC.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None, though to have this function work, need to call ds707_nid_register(),
  so that MC can call the NID change handler when the NID changes.

   Copyright (c) 1999 by QUALCOMM Incorporated.  All Rights Reserved.
   Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_nid.h_v   1.1   18 Nov 2002 18:26:04   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_nid.h#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $

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
FUNCTION       DS707_NID_ENABLE_RECON

DESCRIPTION    Enables reconnection based on NID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_nid_enable_recon(void);

/*===========================================================================
FUNCTION       DS707_NID_DISABLE_RECON

DESCRIPTION    Disables reconnection based on NID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_nid_disable_recon(void);

/*===========================================================================
FUNCTION       DS707_NID_CHANGE_HANDLER

DESCRIPTION    Causes a data reconnection.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_nid_change_handler
(
  word  old_nid,                                 /* Old Network Id         */
  word  new_nid                                  /* New Network Id         */
);

/*===========================================================================
FUNCTION       DS707_NID_REGISTER

DESCRIPTION    Registers the handler with MC.  Does not change the 
               enabled/disabled reconnection attribute.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_nid_register(void);

/*===========================================================================
FUNCTION       DS707_NID_DEREGISTER

DESCRIPTION    Deregisters the handler with MC.  Does not change the 
               enabled/disabled reconnection attribute.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_nid_deregister(void);

#endif /* FEATURE_DATA_IS707   */
#endif /* DS707_NID_H          */

