#ifndef PM_APP_DIAG_H
#define PM_APP_DIAG_H
/*==========================================================================

             Power Management Diagnostic Packet Definitions

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of DIAG packets must be PACKED.
  
  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                             Edit History

  $Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/inc/pmapp_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/25/05   Frank   Added diag lib.
04/05/04   rmd     Featurized PMIC DIAG services under 
                   "FEATURE_PMIC_DIAG_SERVICES".
03/24/04   rmd     Updated file to follow PMIC VU coding standards.
02/29/04   st      Created file.
===========================================================================*/


#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"

#if defined(T_WINNT)
#error code not present
#endif



/*==========================================================================
  
                  SUBSYSTEM PACKET CODE DEFINITIONS
   
============================================================================*/

#define PM_APP_DIAG_VREG_SET_F      0
#define PM_APP_DIAG_LIB_DIAG        1



/*==========================================================================

PACKET   PM_APP_DIAG_MSMC_VREG_SET

PURPOSE  Sent from the DM to the DMSS to set the PM6650 MSMC VREG voltage.

============================================================================*/

typedef PACKED struct 
{
  diagpkt_subsys_header_type  header;
  uint8                       vreg_id;   // pm_vreg_id_type
  uint16                      millivolts;
} pm_app_diag_vreg_set_req_type;



/*===========================================================================

FUNCTION PM_APP_DIAG_INIT

DESCRIPTION
  Register the diag packet function dispatch table.

===========================================================================*/

void pm_app_diag_init (void);


#endif /* PM_APP_DIAG_H */

