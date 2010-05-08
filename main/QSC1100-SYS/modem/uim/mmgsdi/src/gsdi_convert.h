#ifndef GSDI_CONVERT_H
#define GSDI_CONVERT_H
/*===========================================================================


           M M G S D I   U T I L I T Y   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2007, 2009 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_convert.h#2 $$ $DateTime: 2009/04/29 16:02:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/29/09   js      Removed redundant declarations
12/11/07   js      Added conversion function to map gsdi and mmgsdi status
11/19/07   sun     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"
#include "gsdi_exp.h"
#include "gsdi.h"
#include "mmgsdilib.h"


/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_TO_GSDI_STATUS

   DESCRIPTION:
     This function checks for valid range of the mmgsdi_return_enum_type and
     returns the corresponding gsdi_returns_T

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_ERROR:            Null Pointer was provided.

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_to_gsdi_status(
  mmgsdi_return_enum_type  mmgsdi_status,
  gsdi_returns_T          *gsdi_status_ptr
);


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_GSDI_STATUS_TO_MMGSDI

   DESCRIPTION:
     This function checks for valid range of the gsdi_returns_T,  and
     returns the corresponding mmgsdi_return_enum_type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_ERROR:            Null Pointer was provided.

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_gsdi_status_to_mmgsdi(
  gsdi_returns_T            gsdi_status,
  mmgsdi_return_enum_type  *mmgsdi_status_ptr
);


/*===========================================================================
FUNCTION MMGSDI_UTIL_CONVERT_REFRESH_MODE

DESCRIPTION
  This function is used to convert refresh mode from mmgsdi enum to
  gsdi enum

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_refresh_mode(
  mmgsdi_refresh_mode_enum_type mmgsdi_refresh,
  gsdi_refresh_modes_enum_type *gsdi_refresh_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_MMGSDI_TO_GSDI_SLOT

   DESCRIPTION:
     This is a utility function to convert slot of the type mmgsdi to
     gsdi_slot_id_type

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_convert_mmgsdi_to_gsdi_slot(
  mmgsdi_slot_id_enum_type mmgsdi_slot,
  gsdi_slot_id_type       *gsdi_slot_ptr
);


/*===========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_TO_GSDI_PIN_ID

   DESCRIPTION:
     This function checks for valid range of the mmgsdi_return_enum_type and
     returns the corresponding gsdi_returns_T

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_ERROR:            Null Pointer was provided.


   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_pin_enum_type mmgsdi_util_convert_from_gsdi_pin_id(
  gsdi_pin_ids_T pin_id
);


/* ===========================================================================
FUNCTION MMGSDI_UTIL_CONVERT_TO_MMGSDI_PIN_STATUS

DESCRIPTION
  This function converts gsdi_pin status to mmgsdi pin status
  for PIN1 and PIN2 for the selected app.

PARAMETERS:
  change_pin_state: Specifies if the pin state needs to be changed. This is
  used only when gsdi pin state is enabled and mmgsdi_pin_state could be
  enabled_not_verified and enabled_verified.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  NONE
===========================================================================*/
gsdi_returns_T mmgsdi_util_convert_to_mmgsdi_pin_status(
  gsdi_pin_ids_T              pin_id,
  gsdi_pin_status_T           gsdi_pin_status
);


/* ==========================================================================
FUNCTION MMGSDI_UTIL_MAP_GSDI_REFRESH_MODE_TO_MMGSDI

DESCRIPTION
  This function maps GSDI refresh mode to MMGSDI refresh mode

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_map_gsdi_refresh_mode_to_mmgsdi(
  gsdi_refresh_modes_enum_type   gsdi_refresh_mode,
  mmgsdi_refresh_mode_enum_type *mmgsdi_refresh_mode_ptr
);


#ifdef FEATURE_MMGSDI_CARD_ERROR_INFO
/* ============================================================================
FUNCTION:      MMGSDI_UTIL_CONVERT_TO_ERROR_INFO_EVENT

DESCRIPTION:
  This function will be used to convert a generic event to a detailed
  error event.

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  VOID

SIDE EFFECTS:
  NONE
============================================================================ */
void mmgsdi_util_convert_to_error_info_event (
  mmgsdi_slot_id_enum_type slot,
  gsdi_sim_events_T        sim_event,
  uim_status_type          error_condition,
  gsdi_sim_events_T      * sim_error_info_event_ptr
);


/* ============================================================================
FUNCTION:      MMGSDI_UTIL_DERIVE_SLOT_FROM_GSDI_EVENT

DESCRIPTION:
  This function returns an MMGSDI SLOT ID for the event
  provided

DEPENDENCIES:
  None

LIMITATIONS:

RETURN VALUE:
  mmgsdi_slot_id_enum_type

SIDE EFFECTS:
  NONE
============================================================================*/
mmgsdi_slot_id_enum_type mmgsdi_util_derive_slot_from_gsdi_event(
  gsdi_sim_events_T sim_event
);
#endif /* FEATURE_MMGSDI_CARD_ERROR_INFO */


/*===========================================================================
FUNCTION MMMGSDI_UTIL_CONVERT_TO_GSDI_PIN_INFO

DESCRIPTION
  This function converts mmgsdi pin status to gsdi pin status

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_to_gsdi_pin_info(
  gsdi_pin_info_item_T     * pin_info_ptr);


/*===========================================================================
FUNCTION MMMGSDI_UTIL_CONVERT_TO_MMGSDI_PIN_INFO

DESCRIPTION
  This function converts mmgsdi pin status to gsdi pin status

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_to_mmgsdi_pin_info(
  gsdi_pin_ids_T                 pin_id,
  uint8                          num_retries,
  uint32                         req_type
);


#endif /* GSDI_CONVERT_H */
