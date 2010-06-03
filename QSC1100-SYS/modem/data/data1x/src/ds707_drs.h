#ifndef DS707_DRS_H
#define DS707_DRS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 
                           D S 7 0 7 _ D R S

GENERAL DESCRIPTION
  This file contains functions that are used to see if the DRS bit should be
  set on originations out of dormancy.

EXTERNALIZED FUNCTIONS
  ds707_drs_get_drs_bit()
    Called on a pkt re-origination to see if Mobile IP requires the DRS bit
    to be set, indicating that the mobile has data to send.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

 Copyright (c) 2002-2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_drs.h_v   1.3   26 Nov 2002 08:20:34   ubabbar  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_drs.h#3 $ $DateTime: 2009/05/27 05:07:18 $ $Author: nsivakum $

when       who        what, where, why
--------   ---        -------------------------------------------------------
04/29/09   sn         Ported support for call throttle feature (DCTM).
07/28/08   psng       Removed function ds707_drs_init, as we are not caching
                      NV_DS_MIP_QC_DRS_OPT_I at power up.
11/25/02   usb        Renamed functions, added drs_init, completed drs 
                      algorithm handling.
11/18/02   jd         Added ds707_drs_mip_algorithm() to turn optimization
                      code on or off
07/29/02   ak         Initial revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707


/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/


/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_DRS_GET_MIP_DRS_BIT

DESCRIPTION   This function returns TRUE if the DRS bit should be set for a
              Mobile IP Call. Else returns FALSE if the DRS bit should 
              not be set.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_drs_get_mip_drs_bit(void);

/*===========================================================================
FUNCTION      DS707_DRS_HANDLE_SNPZID_CHANGE_RECON

DESCRIPTION   This function handles the reconection due to SID/NID/PZID 
              change. If the drs bit was set during reconnection then it 
              initiates a PPP resync on UM interface.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_drs_handle_snpzid_change_recon(void);

/*===========================================================================
FUNCTION      DS707_DRS_IS_IN_MIP_CALL

DESCRIPTION   Wrapper function that returns TRUE if we are in a mobile IP
              call

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_drs_is_in_mip_call(void);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_DRS_H        */

