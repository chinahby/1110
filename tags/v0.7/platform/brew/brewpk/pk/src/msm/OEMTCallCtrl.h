#ifndef OEMTCALLCTRL_H
#define OEMTCALLCTRL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           OEM Implementation of the number classification and call control

GENERAL DESCRIPTION
  This file contains the call control related functionality from the
  oem layer interface.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1999 through 2009 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/06   ak     Initial revision
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"

#include "cm.h"

#ifdef FEATURE_UI_PBM
#include "pbmlib.h"
#endif

#define AEET_CALLCONTROL_NO_RESTRICTION            0x0000
#define AEET_RESTRICT_CALLS_TO_EFS                 0x0001
#define AEET_RESTRICT_CALLS_TO_SPEED_DIAL_LIST     0x0002
#define AEET_RESTRICT_CALLS_TO_ADN_SLOT1           0x0004
#define AEET_RESTRICT_CALLS_TO_ADN_SLOT2           0x0008
#define AEET_RESTRICT_NO_EFS         0xFFFE
#define AEET_RESTRICT_NO_SPEED_DIAL  0xFFFD
#define AEET_RESTRICT_NO_ADN_SLOT1   0xFFFB
#define AEET_RESTRICT_NO_ADN_SLOT2   0xFFF7


/*===========================================================================
FUNCTION OEMT_CallCtrl_SetRestrictions

DESCRIPTION
  This function sets the restriction_bitmask to the value from Core.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void OEMTCallCtrl_SetRestrictions(byte restriction_bitmask);

/* <EJECT> */
/*===========================================================================
FUNCTION OEMT_IsRestricted

DESCRIPTION
  This function checks the restriction_bitmask that was set and returns TRUE
  if the PBM file given has the bit in the mask set which corresponds to this
  restriction.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean OEMTCallCtrl_IsRestricted(pbm_device_type pbm_file);
/*===========================================================================
FUNCTION OEMCallCtrl_Reg

DESCRIPTION
 Registration function called by the client at startup to indicate that call control
 will be done by the User Interface Layer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void OEMTCallCtrl_Reg(void);

#endif //OEMTCALLCTRL_H
