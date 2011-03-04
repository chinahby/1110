#ifndef WMSTSI_H
#define WMSTSI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    W I R E L E S S   M E S S A G I N G   S E R V I C E S
                   -- Translation Services

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the internal translation functions.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsts.h_v   1.2   26 Mar 2002 12:10:10   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmstsi.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   PMD     Updated Copyright Information to include 2008
06/27/07   PMD     Moved certain functions to wmsts.h
06/20/07   PMD     New File
===========================================================================*/



/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/
#include "wmsmsg.h"


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_RUIM

/*=========================================================================
FUNCTION
  wms_ts_cdma_pack_ruim_sms

DESCRIPTION
  This function packs the RUIM SMS Message

DEPENDENCIES
  None

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_ts_cdma_pack_ruim_sms
(
  const wms_msg_ruim_sms_s_type  * ruim_sms,
  uint8                    * result
);

/*=========================================================================
FUNCTION
  wms_ts_cdma_unpack_ruim_sms

DESCRIPTION
  This function unpacks the RUIM SMS Message

DEPENDENCIES
  None

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_ts_cdma_unpack_ruim_sms
(
  const uint8              * data,
  wms_msg_ruim_sms_s_type  * result
);

#endif /* FEATURE_CDSMS_RUIM */

#endif /* FEATURE_CDSMS */

#endif /* WMSTSI_H */


