/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- GW Interfaces with CM and NAS

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  implements the transport interfaces with Call Manager and NAS layers.

Copyright (c) 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmscm_gw.c#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/08   PMD     New CMCNas_MN_New Prototype changes
04/22/08   PMD     Added support for WMSE to talk to WMS Components for GW
04/18/08   PMD     Removed FEATURE_WMS_CM_NAS_INTERFACES
04/10/08   PMD     Fixed WMSCmdErr_FromCMCNas() prototype
02/26/08   PMD     Updated wmscm_gw changes for Enhanced Domain Selection
02/14/08   PMD     Added support for common type wms_cm_conn_id_type
02/11/08   PMD     Fixed wms_cm_gw_is_idle() for CM_NAS Interfaces
01/30/08   PMD/PY  Created cm/nas interfacing layer in wmscm_gw.h and wmscm_gw.c
===========================================================================*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "comdef.h"

#if defined(FEATURE_GWSMS)
#error code not present
#endif /* FEATURE_GWSMS */

