#ifndef MCCREG_H
#define MCCREG_H
/*===========================================================================

           C D M A    R E G I S T R A T I O N    S E R V I C E S 
                       H E A D E R    F I L E

DESCRIPTION
  This file data and function declarations necessary for CDMA Registration
  Services.

Copyright (c) 1993-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccreg.h_v   1.4   01 Aug 2002 19:56:00   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mccreg.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/07   sb      Added support for operational mode feature.
01/15/07   sb      VOIP-1X handoff updates.
06/01/06   an      Added the bypass registration indicator changes
04/25/05   fc      Merged the following from MSMSHARED:
04/20/04   fc       Changes for the update of reg_sid, reg_nid, reg_reg_zone, 
                    auth and rand during AHO/APHO.
08/17/04   az      Rel D Negative SCI changes
08/03/04   pg      Added support for Rel D timer based registration.
11/19/03   va      Added support for REL C register_in_idle
07/23/03   bkm     Mainlined FEATURE_ANTIREGISTRATION_BIAS. 
08/01/02   fc      Added support for the count of the number of powerup 
                   registration performed. 
02/22/02   fc      Added support for Release A common channels.
02/14/02   fc      Merged from 1.3 in MSM archive.
 01/25/02  sj      Added prototype for funtion mccreg_uz_update().
 12/14/01  fc      Removed prototype for mccreg_blksys_cb_init.
06/26/01   fc      Merged the following from common archive:
           06/13/01   fc      Added prototype for mccreg_blksys_cb_init.
03/06/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           ks      Removed ENHANCED_STANDBY_II feature.
10/05/99   lh      Serving System update for CM is added.
06/22/99   ks      new structure mccreg_bs_reg_info is created for current BS
                   information. 
03/01/99   ejv     Merged functions for FEATURE_ANTIREGISTRATION_BIAS.
11/04/98   ych     Added prototype for mccreg_reset_reg().
05/13/96   jca     Updates for J-STD-008 registration procedures.
05/25/95   jca     Updates for IS-95A registration procedures.
03/18/93   jai     Created file. 

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "caii.h"
#include "nv.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Special REG_TYPE value that indicates registration access not required */
#define MCCREG_NO_REG_REQUIRED 0xFF

/* This constant is only used by DS to indicate un-registered status */
#define MAXUINT16 65535U            /* Largest 16-bit unsigned integer */

/* Type to hold an entry from the SID/NID list */
typedef struct
{
  q_link_type link;
  word sid;
  word nid;
  nv_block_or_sys_enum_type block_or_sys;
  byte band_class;
  int4 cnt;              /* Number of msecs before timer expires */
} mccreg_sid_nid_type;

/* This is used for the current BS info after a succesful
   CDMA registration */
typedef struct 
{ 
  word sid; 
  byte band_class; 
  nv_block_or_sys_enum_type block_or_sys; 
} mccreg_bs_reg_info; 

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MCCREG_INIT

DESCRIPTION
  Performs powerup initialization:
    - Initialize timers
    - Initialize queues

DEPENDENCIES
  This procedure should only be called once after powerup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_init( void );  

/*===========================================================================

FUNCTION MCCREG_GET_BS_REG_INFO

DESCRIPTION
  This function is called by other subsystems to receive the current Base
  Station information (SID,BAND_CLASS and BLOCK_OR_SYS type) where the 
  Mobile has registered.
  
DEPENDENCIES
  This should be called based on the current Mode of the MS. This function
  returns "FALSE" if the Mobile is not registered,

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  

===========================================================================*/

boolean mccreg_get_bs_reg_info( mccreg_bs_reg_info* info);

/*===========================================================================

FUNCTION MCCREG_IS_REGISTERED

DESCRIPTION
  This function is called by other subsystems to find out if the 
  Mobile has registered.
  
DEPENDENCIES
  This should be called based on the current Mode of the MS. This function
  returns "FALSE" if the Mobile is not registered,

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  

===========================================================================*/
boolean mccreg_is_registered( void );

/*===========================================================================

FUNCTION MCCREG_POWERUP

DESCRIPTION
  This function performs power-up registration procedures as called out
  in IS-95A section 6.6.5.5.1.1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_powerup( void );

/*===========================================================================

FUNCTION MCCREG_ACQ_INIT

DESCRIPTION
  This function performs registration variable initialization for a mode
  or serving system change as called out in IS-95A section 6.6.5.5.1.1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Flags are initialized.

===========================================================================*/

void mccreg_acq_init( void );

#ifdef FEATURE_USERZONE
/*===========================================================================

FUNCTION MCCREG_UZ_UPDATE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mccreg_uz_update
(
  void
);
#endif /* FEATURE_USERZONE */

/*===========================================================================

FUNCTION MCCREG_IDLE_INIT

DESCRIPTION
  This function performs processing necessary when entering the Idle state
  from the Initialization state (see IS-95A section 6.6.5.5.1.3).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_idle_init( void );

/*===========================================================================

FUNCTION MCCREG_IDLE_CHECK

DESCRIPTION
  This function performs idle registration procedures called out in
  IS-95A section 6.6.5.5.2.1.

DEPENDENCIES
  None

RETURN VALUE
  If a registration is required, the type of registration required will be
  returned.  Otherwise the special indicator MCCREG_NO_REG_REQUIRED will
  be returned.

SIDE EFFECTS
  None

===========================================================================*/

byte mccreg_idle_check( void );

/*===========================================================================

FUNCTION MCCREG_SYSPARM

DESCRIPTION
  This function processes the registration variables of a received System
  Parameters Message as specified in IS-95A section 6.6.5.5.2.2.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

void mccreg_sysparm( caii_sysparm_type *msg_ptr );

/*===========================================================================

FUNCTION MCCREG_POWERDOWN

DESCRIPTION
  This function performs the powerdown updates to the zone list, the 
  sid nid list and the distance registration variables in NV. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The NV SID/NID list, zone list and distance registration variables
  are updated.

===========================================================================*/

void mccreg_powerdown( void );

/*===========================================================================

FUNCTION MCCREG_IDLE_POWERDOWN

DESCRIPTION
  This function performs Idle Channel powerdown registration procedures
  as specified in IS-95A section 6.6.5.5.2.4.

DEPENDENCIES
  None

RETURN VALUE
  CAI_POWERDOWN_REG if the mobile is required to register.
  MCCREG_NO_REG_REQUIRED if the mobile is not required to register.

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

byte mccreg_idle_powerdown( void );

/*===========================================================================

FUNCTION MCCREG_SET_REG_SCI

DESCRIPTION
  This sets the reg_sci in mccreg_vars structure.
  It is set during explicit and implicit registration.
  
  This set value is later used to appropriately update the operational SCI when
  L2 ack for the registration is received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_set_reg_sci
(
  int8 sci
);

/*===========================================================================

FUNCTION MCCREG_SUCCESS

DESCRIPTION
  This function performs registration actions required after the mobile
  station receives an acknowledgement for a Registration Message, 
  Origination Message, or Page Response Message sent on the Access
  Channel as described in IS-95A section 6.6.5.5.3.1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

void mccreg_success( void );

/*===========================================================================

FUNCTION MCCREG_IDLE_ACK_OR_FAIL

DESCRIPTION
  This function performs registration actions required after an
  acknowledgement is received to any message other than a Registration,
  Origination or Page Response or after a failed access attempt.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

void mccreg_idle_ack_or_fail( void );

/*===========================================================================

FUNCTION MCCREG_TC_INIT

DESCRIPTION
  This function performs registration procedures required for entering
  the Traffic Channel Initialization Substate of the Mobile Station Control
  on the Traffic Channel State as specified in IS-95A section 6.6.5.5.4.1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Timer based registration is disabled.

===========================================================================*/

void mccreg_tc_init( void );

/*===========================================================================

FUNCTION MCCREG_MS_REG

DESCRIPTION
  This function performs registration procedures required after receiving
  a Mobile Station Registered Message as specified in IS-95A section
  6.6.5.5.4.3.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Registration variables are updated.

===========================================================================*/

void mccreg_ms_reg( caii_registered_type *msg_ptr );

/*===========================================================================

FUNCTION MCCREG_GET_MOB_TERM

DESCRIPTION
  This function retrieves the call termination enabled indicator.

DEPENDENCIES
  None.

RETURN VALUE
  Call termination enabled indicator.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccreg_get_mob_term( void );

/*===========================================================================

FUNCTION MCCREG_CLEAR_REG

DESCRIPTION
  This function resets CDMA registration indicators upon successful 
  registration on an analog system as specified in IS-95A sections
  2.6.3.8 and 2.6.3.9.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_clear_reg( void );

/*===========================================================================

FUNCTION MCCREG_RESET_REG

DESCRIPTION
  This function resets CDMA registration indicators upon successful
  registration on an analog system as specified in IS-95A sections
  2.6.3.8 and 2.6.3.9 by calling mccreg_clear_reg and then sets the REGISTEREDs
  to NO.
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccreg_reset_reg( void );

/*===========================================================================

FUNCTION MCCREG_RESET_REG_STATUS

DESCRIPTION
  This function resets the registration status to indicate that the mobile
  needs to register.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccreg_reset_reg_status( void );

/*===========================================================================

FUNCTION MCCREG_FIND_SID_NID

DESCRIPTION
  This function searches SID_NID_LISTs the input (SID,NID) pair.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to the (SID,NID) pair on SID_NID_LISTs if it is found.
  NULL otherwise.

SIDE EFFECTS
  None

===========================================================================*/

mccreg_sid_nid_type *mccreg_find_sid_nid( word sid,  word nid );

/*===========================================================================

FUNCTION MCCREG_ZONE_REG_REQUIRED

DESCRIPTION
  This function checks if a zone-based registration is required
  for the input (ZONE, SID,NID).

DEPENDENCIES
  None

RETURN VALUE
  TRUE if a zone-based registration is required for the input (ZONE, SID,NID).
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean mccreg_zone_reg_required
(
  word zone,  /* ZONE to find */
  word sid,   /* SID to find */
  word nid    /* NID to find */
);

/*===========================================================================

FUNCTION MCCREG_PARAMETER_REG_REQUIRED

DESCRIPTION
  This function checks if a zone-based registration is required
  for the input (SID,NID).

DEPENDENCIES
  None

RETURN VALUE
  TRUE if a parameter-based registration is required for the input (SID,NID).
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean mccreg_parameter_reg_required
(
  word sid,   /* SID to find */
  word nid    /* NID to find */
);

/*===========================================================================

FUNCTION MCCREG_UPDATE_LISTS

DESCRIPTION
  This function updates and expires timers on the SID/NID and zone lists.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_update_lists( void );

/*===========================================================================

FUNCTION MCCREG_SET_IDLE_REG_INFO

DESCRIPTION
  This function stores Idle state BS info to be used in registration 
  success/failure procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mccreg_set_idle_reg_info
(
  caii_sysparm_type *msg_ptr
    /* Pointer to received System Parameter message */
);

#ifdef FEATURE_MCC_POWERUP_REG_COUNT
/*===========================================================================

FUNCTION MCCREG_GET_POWERUP_REG_COUNT

DESCRIPTION
  This function retrieves the count for the number of powerup registrations
  performed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_get_powerup_reg_count(qword count);

/*===========================================================================

FUNCTION MCCREG_RESET_POWERUP_REG_COUNT

DESCRIPTION
  This function resets the count for the number of powerup registrations
  performed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_reset_powerup_reg_count(void);

/*===========================================================================

FUNCTION MCCREG_INCREMENT_POWERUP_REG_COUNT

DESCRIPTION
  This function increments the count for the number of powerup registrations
  performed by 1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_increment_powerup_reg_count(void);
#endif /* FEATURE_MCC_POWERUP_REG_COUNT */

#ifdef FEATURE_IS2000_REL_C
/*===========================================================================

FUNCTION MCCREG_SET_REGISTER_IN_IDLE

DESCRIPTION
  This function sets the register in idle flag which is used for
  enc/msg integritry resync required registration as commanded by BS on TC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Setting this may cause MS to perform "enc/msgint resync required" reg
  in the common channel.

===========================================================================*/

void mccreg_set_register_in_idle 
( 
  boolean register_in_idle
);
#endif

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

/*===========================================================================

FUNCTION MCCREG_SET_MOB_TERM

DESCRIPTION
  This function sets the call termination enabled indicator (MOB_TERMs)
  based on the input system parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mccreg_set_mob_term
(
  word sid,
    /* System ID */
  word nid
    /* Network ID */
);

#endif /* MCCREG_H */

