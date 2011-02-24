/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

                                    A N D

                     N O N V O L A T I L E   M E M O R Y

                              I N T E R F A C E


GENERAL DESCRIPTION

  This file contains the header information for the GSDI to NV Interface.  This
  interface will be exercised by the GSDI Task and Virtual SIM Task.

EXTERNALIZED FUNCTIONS

  gsdi_get_nv()  - Used get items from nv

  gsdi_put_nv()  - Used put items into nv

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/03   jar     Added GSDI - NV Interface function for SLOT Preference
                   Reads: gsdi_get_nv_slot_preferences().
11/26/02   jar     Removed RTRE Code
09/14/02   jar     Changed FEATURE_MMGSDI_RTRE to Internal Feature MMGSDI_RTRE.
08/30/02  jar/ck   Removed gsdi_wait_nv.  Added gsdi_send_rtre_command.
                   Added feature MMGSDI_RTRE.
08/18/02   jar     Created gsdi_nv.h
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "nv.h"
#include "gsdi.h"

/*===========================================================================
FUNCTION GSDI_GET_NV

DESCRIPTION
  Get an item from the nonvolatile memory.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done.
===========================================================================*/
extern nv_stat_enum_type gsdi_get_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
);

/*===========================================================================
FUNCTION GSDI_PUT_NV

DESCRIPTION
  Put an item on to the nonvolatile memory.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done.
===========================================================================*/
extern nv_stat_enum_type gsdi_put_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
);

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION GSDI_GET_NV_SLOT_PREFERENCES

DESCRIPTION
  Function is used with Dual Slot Builds.  It is used to determine what the
  user preferences are for which Subscription they would like and from what
  slot.

  If the NV Item is not active, it will be set to the equivalent of
  NO USER PREFERENCE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets: gsdi_data.user_slot_prefs.gsm_app_pref
        gsdi_data.user_slot_prefs.cdma_app_pref
  To:
        GSDI_SLOT_1, GSDI_SLOT_2, or GSDI_SLOT_NONE.
===========================================================================*/
gsdi_returns_T gsdi_get_nv_slot_preferences( void );
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
