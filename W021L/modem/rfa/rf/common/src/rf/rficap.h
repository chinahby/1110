#ifndef __RFICAP_H__
#define __RFICAP_H__

/*===========================================================================


           R F I C A P   D E F I N I I T I O N   F I L E

DESCRIPTION
  This file contains mode/band and hardware capabilities 
  and macros which are used by the RF unit. 

Copyright (c) 2006, 2007   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rficap.h#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/20/07   ra      Moved GPS features to rficap_mm_gps header file
11/15/06   ra      supports rficap_mm.h. The old header file was rf_mm.h. This conforms with file naming convention
                   where "rf_has_xx" is declared in a file named "rficap_xx.h"
11/09/06   jfc     Added this comment

===========================================================================*/

#include "rficap_mm.h"

#ifdef RF_HAS_FEATURE_WCDMA
#error code not present
#endif

#ifdef RF_HAS_FEATURE_CDMA1X
 #include "rficap_mm_cdma1x.h"
#endif

#ifdef RF_HAS_FEATURE_GPS
#error code not present
#endif 

#endif /* __RFICAP_H__ */

