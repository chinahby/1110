#ifndef MCNVDF_H
#define MCNVDF_H
/*==========================================================================

               M C   N V   D E F A U L T   S E R V I C E

DESCRIPTION
  This header file contains the interface definitions for the MC NV 
  default values service.

Copyright (c) 2000-2005 by QUALCOMM Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcnvdf.h_v   1.0.2.0   30 Nov 2001 17:32:20   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mcnvdf.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/09/00   ts      Created file.  Needed a service to return default values for
                   NV items that reside on the RUIM.  

===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "nv.h"

/*===========================================================================
 
                           DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MC_SET_ESN_FOR_DEFAULT

DESCRIPTION
  This function sets the static variable - esn - for use in deriving default
  values of some NV items processed by this service.

DEPENDENCIES
  This function must be called with the appropriate esn value before using
  the other functions.

RETURN VALUE
  None

SIDE EFFECTS
  The esn value will be set appropriately after this function is executed.

===========================================================================*/
extern void mc_set_esn_for_default
( 
  dword esn_value
);

/*===========================================================================

FUNCTION MC_DEFAULT_VALUES

DESCRIPTION
  This function returns default values for the NV items that reside on 
  the RUIM.  This function is used to provide default values in the
  case the RUIM data is not accessible.

DEPENDENCIES
  This function uses nvruim_default_imsi_s1 and nvruim_default_imsi_t_s1
  which use the static - esn variable.  This variable must be set by 
  nvruim_set_esn_for_default before this function is used.


RETURN VALUE
  Return default values for the NV access.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mc_default_values
( 
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data buffer for item */
);


#endif /* MCNVDF_H */
