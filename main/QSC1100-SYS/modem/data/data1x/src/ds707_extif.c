/*===========================================================================
                         D S 7 0 7 _ E X T I F
GENERAL DESCRIPTION
  This file is the external interface exported by ds707 to the other layers
  in the DMSS code than need to query ds707 for its state or other 
  information. This file is intended to provide an interface to other layers 
  in the DMSS code and is not intended to be used as an interface by
  entities outside of DMSS.
  
  This is specific to packet data calls. 

EXTERNALIZED FUNCTIONS
  DS_EXTIF_VERIFY_SO()
    Wrapper for DS_VERIFY_SO. Asks DS if the originating SO is correct.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

 Copyright (c) 2003-2007 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_extif.c#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
10/24/07   sn         Removed external functions that are not needed for ULC. 
                      ULC requires only ds707_extif_verify_so().
09/10/05   gr         Merged changes to lock out GPS in the middle of a PPP 
                      resync during a handup from 1x to DO
04/12/05   vrk        Included wrapper function ds707_extif_verify_so
02/13/04   ak         Do get_dormant on phys link instead of iface.
01/27/04   vr         Added functions
                      ds707_extif_was_last_data_network_cdma and
                      ds707_extif_was_last_data_network_hdr.
10/02/03   vr         Changed function names.
09/16/03   vr         First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#include "ds707_extif.h"
#include "ds707_pkt_mgr.h"


/*===========================================================================
                            VARIABLES
===========================================================================*/

/*===========================================================================
                        INTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS_EXTIF_VERIFY_SO

DESCRIPTION   Wrapper for DS_VERIFY_SO. Called by MC.  
	      Asks DS if the originating SO is correct.

DEPENDENCIES  None

RETURN VALUE  service option to orig with

SIDE EFFECTS  None.
===========================================================================*/

word ds707_extif_verify_so(word so_to_verify)
{
  return (ds_verify_so(so_to_verify));
}

#endif /* FEATURE_DATA_IS707 */
