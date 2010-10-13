#ifndef WMSDEM_H
#define WMSDEM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- Interface with DEM

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the WMS interfaces used by DEM.

Copyright (c) 2007, 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsdem.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   PMD     Updated Copyright Information to include 2008
03/30/07   HQ      Initial version for power management.
===========================================================================*/


#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */
#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSDEM_H */

