#ifndef WMSCM_GW_H
#define WMSCM_GW_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- GW Interfaces with CM and NAS

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the transport interfaces with Call Manager and NAS layers.

Copyright (c) 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmscm_gw.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/08   PMD     Modified wms_cm_gw_send_rl_*** prototypes
                   Moved FEATURE_GWSMS_BROADCAST feature flag
04/18/08   PMD     Removed FEATURE_WMS_CM_NAS_INTERFACES
02/14/08   PMD     Added support for common type wms_cm_conn_id_type
01/30/08   PMD/PY  Created cm/nas interfacing layer in wmscm_gw.h and wmscm_gw.c
===========================================================================*/


#if defined(FEATURE_GWSMS)
#error code not present
#endif /* FEATURE_GWSMS */

#endif /* WMSCM_GW_H */

