#ifndef WMSDBG_H
#define WMSDBG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- Debug Group

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the data types for the Debug group.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM INCORPORATED.
  All Rights Reserved.
  
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsdbg.h_v   1.1   08 May 2002 08:41:50   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsdbg.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   PMD     Updated Copyright Information to include 2008
01/17/07   HQ      Updated copyright info to include 2007.
01/25/06   VK      Updated Copyright Info
10/08/05   VK      Updated copyright info
09/22/03   PMD     Changed Featurization from CDSMS to CDSMS || GWSMS
09/07/03   PMD     Added Debugging Information String Display for cmd_id,
                   cmd_err, cfg_event, msg_event, dc_event, bc_event,
                   bc_mm_event, dbg_event
05/07/02   HQ      Added more support under FEATURE_CDSMS.
03/25/02   HQ      Initial version for multimode WMS.
===========================================================================*/


#if (defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS))

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "wms.h"
#include "wmsi.h"



/*===========================================================================
                             
                         DATA TYPE DECLARATIONS
                             
===========================================================================*/

/* Debug data
*/
typedef struct wms_dbg_struct
{
  boolean                       init_complete;

} wms_dbg_s_type;



/* <EJECT> */
/*===========================================================================
                             
                       FUNCTION DEFINITIONS
                             
===========================================================================*/

/*===========================================================================
FUNCTION wms_dbg_s_ptr

DESCRIPTION
  Return the pointer to the Debug data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_dbg_s_type* wms_dbg_s_ptr
(
  void
);

/*===========================================================================
FUNCTION wms_dbg_process_cmd

DESCRIPTION
  Dispatch the Debug command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dbg_process_cmd
(
  wms_cmd_type  *cmd_ptr
);



/*===========================================================================
FUNCTION wms_dbg_init

DESCRIPTION
  Do Debug-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Debug data initialized.
===========================================================================*/
void wms_dbg_init
(
  void
);

#endif /* FEATURE_CDSMS */

#if (defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS))

#ifdef FEATURE_WMS_DEBUG_STRING

/*===========================================================================
FUNCTION wms_dbg_get_cmd_string

DESCRIPTION
  This function returns the Command given the Command ID

DEPENDENCIES
  Command Enumeration (wms_cmd_id_e_type)

RETURN VALUE
  Command String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_cmd_string
(
  uint16 cmd_id
);

/*===========================================================================
FUNCTION wms_dbg_get_cmd_err_string

DESCRIPTION
  This function returns the Command Error given the Command Error ID

DEPENDENCIES
  Command Error Enumeration (wms_cmd_err_e_type)

RETURN VALUE
  Command String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_cmd_err_string
(
  uint16 cmd_err
);

/*===========================================================================
FUNCTION wms_dbg_get_cfg_event_string

DESCRIPTION
  This function returns the Configuration event String given the cfg_event

DEPENDENCIES
  Configuration Event Enumeration (wms_cfg_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_cfg_event_string
(
  uint16 cfg_event
);

/*===========================================================================
FUNCTION wms_dbg_get_msg_event_string

DESCRIPTION
  This function returns the Message event String given the msg_event

DEPENDENCIES
  Message Event Enumeration (wms_msg_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_msg_event_string
(
  uint16 msg_event
);

/*===========================================================================
FUNCTION wms_dbg_get_dc_event_string

DESCRIPTION
  This function returns the DC event String given the _event

DEPENDENCIES
  DC Event Enumeration (wms_dc_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_dc_event_string
(
  uint16 dc_event
);

/*===========================================================================
FUNCTION wms_dbg_get_bc_event_string

DESCRIPTION
  This function returns the Broadcast event String given the bc_event

DEPENDENCIES
  Broadcast Event Enumeration (wms_bc_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_bc_event_string
(
  uint16 bc_event
);

/*===========================================================================
FUNCTION wms_dbg_get_event_bc_mm_string

DESCRIPTION
  This function returns the Multimode Broadcast event String given the bc_mm_event

DEPENDENCIES
  Multimode Broadcast Event Enumeration (wms_bc_mm_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_bc_mm_event_string
(
  uint16 bc_mm_event
);

/*===========================================================================
FUNCTION wms_dbg_get_dbg_event_string

DESCRIPTION
  This function returns the Debug event String given the dbg_event

DEPENDENCIES
  Debug Event Enumeration (wms_dbg_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_dbg_event_string
(
  uint16 dbg_event
);

#endif /* FEATURE_WMS_DEBUG_STRING */

#endif /* defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS) */
#endif /* WMSDBG_H */
