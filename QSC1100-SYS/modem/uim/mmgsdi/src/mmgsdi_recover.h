/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   R E C O V E R Y    F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the functions to support recovering UIM module
  after it has internally reset the card.

EXTERNALIZED FUNCTIONS
  mmgsdi_recover_after_reset


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2006 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_recover.h#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/06   pv      Initial Revision.
===========================================================================*/

#include "mmgsdilib.h"
#include "nv.h"

/* ----------------------------------------------------------------------------
   FUNCTION:      mmgsdi_recover_after_reset

   DESCRIPTION:
   This function is primarily responsible to try to get the CARD to a well
   defined state.  Based on the protocol, this function tries to perform the
   initialization procedures specified for the kind of card.
   
   If pin has been verified, it would be re-verified
   
   Terminal profile download will be done appropriately

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
   Card comes back to a known state or is powered down.

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_recover_after_reset(mmgsdi_slot_id_enum_type slot);

